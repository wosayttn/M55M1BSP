/*
 * Copyright (c) 2019-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/****************************************************************************
 * Includes
 ****************************************************************************/

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include <inttypes.h>
#include <stdio.h>
#include <vector>

#include "NuMicro.h"
#include "ethosu_driver.h"

#include "inference_process.hpp"
#include "arm_profiler.hpp"

// Model data (Defined & changable by modifiying compile definition in CMakeLists.txt)
#include "input.h"
#include "model.h"
#include "output.h"

using namespace std;
using namespace InferenceProcess;

/****************************************************************************
 * Defines
 ****************************************************************************/

// Nr. of tasks to process inferences with. Task reserves driver & runs inference (Normally 1 per NPU, but not a must)
#define NUM_INFERENCE_TASKS 1
// Nr. of tasks to create jobs and recieve responses
#define NUM_JOB_TASKS 2
// Nr. of jobs to create per job task
#define NUM_JOBS_PER_TASK 1

// Tensor arena size
#ifdef TENSOR_ARENA_SIZE // If defined in model.h
#define TENSOR_ARENA_SIZE_PER_INFERENCE TENSOR_ARENA_SIZE
#else // If not defined, use maximum available
#define TENSOR_ARENA_SIZE_PER_INFERENCE 2000000 / NUM_INFERENCE_TASKS
#endif

/****************************************************************************
 * Variables
 ****************************************************************************/
#if defined(ETHOSU_FAST_MEMORY_SIZE) && ETHOSU_FAST_MEMORY_SIZE > 0
__attribute__((aligned(16), section(".bss.ethosu_scratch"))) uint8_t ethosu_scratch[ETHOSU_FAST_MEMORY_SIZE];
#else
#define ethosu_scratch          0
#define ETHOSU_FAST_MEMORY_SIZE 0
#endif

struct ethosu_driver ethosu0_driver;

/****************************************************************************
 * NPU interrupt handler
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void NPU_IRQHandler(void)
{
	ethosu_irq_handler(&ethosu0_driver);
}

#ifdef __cplusplus
}
#endif


/****************************************************************************
 * InferenceJob
 ****************************************************************************/

struct ProcessTaskParams {
    ProcessTaskParams() : queueHandle(nullptr), tensorArena(nullptr), arenaSize(0) {}
    ProcessTaskParams(QueueHandle_t _queue, uint8_t *_tensorArena, size_t _arenaSize) :
        queueHandle(_queue), tensorArena(_tensorArena), arenaSize(_arenaSize) {}

    QueueHandle_t queueHandle;
    uint8_t *tensorArena;
    size_t arenaSize;
};

namespace {
// Number of total completed jobs, needed to exit application correctly if NUM_JOB_TASKS > 1
int totalCompletedJobs = 0;

// TensorArena static initialisation
const size_t arenaSize = TENSOR_ARENA_SIZE_PER_INFERENCE;

// Declare below variables in global scope to avoid stack since FreeRTOS resets stack when the scheduler is started
QueueHandle_t inferenceProcessQueue;
ProcessTaskParams taskParams[NUM_INFERENCE_TASKS];
} // namespace

__attribute__((section(".bss.tensor_arena"), aligned(16)))
uint8_t inferenceProcessTensorArena[NUM_INFERENCE_TASKS][arenaSize];

// Wrapper around InferenceProcess::InferenceJob. Adds responseQueue and status for FreeRTOS multi-tasking purposes.
struct xInferenceJob : public InferenceJob {
    QueueHandle_t responseQueue;
    bool status;

    xInferenceJob() : InferenceJob(), responseQueue(nullptr), status(false) {}
    xInferenceJob(const string &_name,
                  const DataPtr &_networkModel,
                  const vector<DataPtr> &_input,
                  const vector<DataPtr> &_output,
                  const vector<DataPtr> &_expectedOutput,
                  const size_t _numBytesToPrint,
                  const vector<uint8_t> &_pmuEventConfig,
                  const uint32_t _pmuCycleCounterEnable,
                  QueueHandle_t _queue) :
        InferenceJob(_name,
                     _networkModel,
                     _input,
                     _output,
                     _expectedOutput,
                     _numBytesToPrint,
                     _pmuEventConfig,
                     _pmuCycleCounterEnable),
        responseQueue(_queue), status(false) {}
};

/****************************************************************************
 * Mutex & Semaphore
 * Overrides weak-linked symbols in ethosu_driver.c to implement thread handling
 ****************************************************************************/

extern "C" {

void *ethosu_mutex_create(void) {
    return xSemaphoreCreateMutex();
}

int ethosu_mutex_lock(void *mutex) {
    SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(mutex);
    xSemaphoreTake(handle, portMAX_DELAY);
	return 0;
}

int ethosu_mutex_unlock(void *mutex) {
    SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(mutex);
    xSemaphoreGive(handle);
	return 0;
}

void *ethosu_semaphore_create(void) {
    return xSemaphoreCreateBinary();
}

int ethosu_semaphore_take(void *sem) {
    SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(sem);
    xSemaphoreTake(handle, portMAX_DELAY);
	return 0;
}

int ethosu_semaphore_give(void *sem) {
    SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(sem);
    xSemaphoreGive(handle);
	return 0;
}
}

/****************************************************************************
 * Functions
 ****************************************************************************/

//  inferenceProcessTask - Run jobs from queue with available driver
void inferenceProcessTask(void *pvParameters) {
    ProcessTaskParams params = *reinterpret_cast<ProcessTaskParams *>(pvParameters);

    class InferenceProcess inferenceProcess(params.tensorArena, params.arenaSize);

    for (;;) {
        xInferenceJob *xJob;

        xQueueReceive(params.queueHandle, &xJob, portMAX_DELAY);
        bool status  = inferenceProcess.runJob(*xJob);
        xJob->status = status;
        xQueueSend(xJob->responseQueue, &xJob, portMAX_DELAY);
    }
}

//  inferenceSenderTask - Creates NUM_INFERNECE_JOBS jobs, queues them, and then listens for completion status
void inferenceSenderTask(void *pvParameters) {
    int ret = 0;

    QueueHandle_t inferenceProcessQueue = reinterpret_cast<QueueHandle_t>(pvParameters);
    xInferenceJob jobs[NUM_JOBS_PER_TASK];

    // Create queue for response messages
    QueueHandle_t senderQueue = xQueueCreate(NUM_JOBS_PER_TASK, sizeof(xInferenceJob *));

    // Create and queue the jobs
    for (int n = 0; n < NUM_JOBS_PER_TASK; n++) {
        // Create job
        xInferenceJob *job = &jobs[n];
        job->name          = string(modelName);
        job->networkModel  = DataPtr((void *)networkModelData, sizeof(networkModelData));
        job->input.push_back(DataPtr(inputData, sizeof(inputData)));
        job->expectedOutput.push_back(DataPtr(expectedOutputData, sizeof(expectedOutputData)));
        job->responseQueue = senderQueue;
        // Send job
        printf("Sending inference job: job=%p, name=%s\n", job, job->name.c_str());
        xQueueSend(inferenceProcessQueue, &job, portMAX_DELAY);
    }

    // Listen for completion status
    do {
        xInferenceJob *pSendJob;
        xQueueReceive(senderQueue, &pSendJob, portMAX_DELAY);
        printf("inferenceSenderTask: received response for job: %s, status = %u\n",
               pSendJob->name.c_str(),
               pSendJob->status);

        totalCompletedJobs++;
        ret = (pSendJob->status);
        if (pSendJob->status != 0) {
            break;
        }
    } while (totalCompletedJobs < NUM_JOBS_PER_TASK * NUM_JOB_TASKS);

    vQueueDelete(senderQueue);

    printf("FreeRTOS application returning %d.\n", ret);
    exit(ret);
}

/****************************************************************************
 * Application
 ****************************************************************************/

static void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Switch SCLK clock source to APLL0 and Enable APLL0 180MHz clock */
    CLK_SetBusClock(CLK_SCLKSEL_SCLKSEL_APLL0, CLK_APLLCTL_APLLSRC_HXT, FREQ_180MHZ);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable FMC0 module clock to keep FMC clock when CPU idle but NPU running*/
	CLK_EnableModuleClock(FMC0_MODULE);

    /* Enable NPU module clock */
    CLK_EnableModuleClock(NPU0_MODULE);

	/* Select UART module clock source as HIRC and UART module clock divider as 1 */
	SetDebugUartCLK();

	/*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set multi-function pins for UART RXD and TXD */
	SetDebugUartMFP();

}


// FreeRTOS application. NOTE: Additional tasks may require increased heap size.
int main() {
    BaseType_t ret;

    /* Unlock protected registers */
    SYS_UnlockReg();
    SYS_Init(); /* Init System, IP clock and multi-function I/O */

    /* Init UART to 115200-8n1 for print message */
    InitDebugUart();

    SYS_LockReg();                   /* Unlock register lock protect */

    printf("\nThis sample code test ethosu function.\n");

    // Initialize Ethos-U NPU driver
    if (ethosu_init(&ethosu0_driver,
                    reinterpret_cast<void *>(NPU_BASE),
                    ethosu_scratch,
                    ETHOSU_FAST_MEMORY_SIZE,
                    1,
                    1)) {
        printf("Failed to initialize NPU.\n");
        return -1;
    }

    // Enable Ethos-U interrupt
    NVIC_EnableIRQ(static_cast<IRQn_Type>(NPU_IRQn));

    inferenceProcessQueue = xQueueCreate(NUM_JOBS_PER_TASK, sizeof(xInferenceJob *));

    // inferenceSender tasks to create and queue the jobs
    for (int n = 0; n < NUM_JOB_TASKS; n++) {
        ret = xTaskCreate(inferenceSenderTask, "inferenceSenderTask", 2 * 1024, inferenceProcessQueue, 2, nullptr);
        if (ret != pdPASS) {
            printf("FreeRTOS: Failed to create 'inferenceSenderTask%i'\n", n);
            exit(1);
        }
    }

    // Create inferenceProcess tasks to process the queued jobs
    for (int n = 0; n < NUM_INFERENCE_TASKS; n++) {
        taskParams[n] = ProcessTaskParams(inferenceProcessQueue, inferenceProcessTensorArena[n], arenaSize);
        ret           = xTaskCreate(inferenceProcessTask, "inferenceProcessTask", 8 * 1024, &taskParams[n], 3, nullptr);
        if (ret != pdPASS) {
            printf("FreeRTOS: Failed to create 'inferenceProcessTask%i'\n", n);
            exit(1);
        }
    }

    // Start Scheduler
    vTaskStartScheduler();

    printf("FreeRTOS application failed to initialise \n");
    exit(1);
}


/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	printf("vApplicationMallocFailedHook\n");
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	printf("vApplicationStackOverflowHook\n");
	taskDISABLE_INTERRUPTS();

  //__BKPT();

    //printf("Stack overflow task name=%s\n", pcTaskName);


	for( ;; );



}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()).  The code in this
	tick hook implementation is for demonstration only - it has no real
	purpose.  It just gives a semaphore every 50ms.  The semaphore unblocks a
	task that then toggles an LED.  Additionally, the call to
	vQueueSetAccessQueueSetFromISR() is part of the "standard demo tasks"
	functionality. */

	Profiler_Tick_Hook();
}
/*-----------------------------------------------------------*/
