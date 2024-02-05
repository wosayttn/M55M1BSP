/**************************************************************************//**
 * @file     InferenceTask.cpp
 * @version  V0.10
 * @brief    Inference process source code
 * * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include "InferenceTask.hpp"
#include "log_macros.h"      /* Logging macros */

namespace InferenceProcess
{

InferenceProcess::InferenceProcess(
    Model *model)
    :   m_model(model)
{}

bool InferenceProcess::RunJob(
    object_detection::DetectorPostprocessing *pPostProc,
    int modelCols,
    int mode1Rows,
    int srcImgWidth,
    int srcImgHeight,
    std::vector<object_detection::DetectionResult> *results
)
{
//    info("Inference process task run job...\n");

#if defined(__PROFILE__)
    uint64_t u64StartCycle;
    uint64_t u64EndCycle;

    profiler.StartProfiling("Inference");
#endif

    bool runInf = m_model->RunInference();

#if defined(__PROFILE__)
    profiler.StopProfiling();
    profiler.PrintProfilingResult();
#endif

    TfLiteTensor *modelOutput0 = m_model->GetOutputTensor(0);
    TfLiteTensor *modelOutput1 = m_model->GetOutputTensor(1);

#if defined(__PROFILE__)
    u64StartCycle = pmu_get_systick_Count();
#endif

    pPostProc->RunPostProcessing(
        mode1Rows,
        modelCols,
        srcImgHeight,
        srcImgWidth,
        modelOutput0,
        modelOutput1,
        *results);

#if defined(__PROFILE__)
    u64EndCycle = pmu_get_systick_Count();
    info("post processing cycles %llu \n", (u64EndCycle - u64StartCycle));
#endif

    return runInf;
}

}// namespace InferenceProcess

/****************************************************************************
 * Mutex & Semaphore
 * Overrides weak-linked symbols in ethosu_driver.c to implement thread handling
 ****************************************************************************/

extern "C" {

    void *ethosu_mutex_create(void)
    {
        return xSemaphoreCreateMutex();
    }

    int ethosu_mutex_lock(void *mutex)
    {
        SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(mutex);
        return (xSemaphoreTake(handle, DEF_RTOS_WAIT_TIME) == pdTRUE) ? 0 : -1;
    }

    int ethosu_mutex_unlock(void *mutex)
    {
        SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(mutex);
        return (xSemaphoreGive(handle) == pdTRUE) ? 0 : -1;
    }

    void *ethosu_semaphore_create(void)
    {
        return xSemaphoreCreateBinary();
    }

    int ethosu_semaphore_take(void *sem)
    {
        SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(sem);
        return (xSemaphoreTake(handle, DEF_RTOS_WAIT_TIME) == pdTRUE) ? 0 : -1;
    }

    int ethosu_semaphore_give(void *sem)
    {
        SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(sem);
        return (xSemaphoreGive(handle) == pdTRUE) ? 0 : -1;
    }

    int ethosu_semaphore_give_from_ISR(void *sem)
    {
        SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(sem);
        BaseType_t xHighPriorityTaskWoken = pdFALSE;

        xSemaphoreGiveFromISR(handle, &xHighPriorityTaskWoken);
        portYIELD_FROM_ISR(xHighPriorityTaskWoken);
        return 0;
    }
}

void INFERENCE_ASSERT(int cond, const char *func, int line)
{
#define NVT_SET_PIN(port, pin)    (*((volatile uint32_t *)((0x40229800+(0x40*(port))) + ((pin)<<2))))

    if (!cond)
    {
        while (1)
        {
            printf("Timeout is at %s %d line\n", func, line);
            printf("Timeout is at %s %d line\n", func, line);
            NVT_SET_PIN(6, 15) = ~NVT_SET_PIN(6, 15) ; //PG15 pin light-off
            printf("Timeout is at %s %d line\n", func, line);
            printf("Timeout is at %s %d line\n", func, line);
        }
    }

    NVT_SET_PIN(6, 15) = ~NVT_SET_PIN(6, 15) ; //PG15 pin light-off
}

void inferenceProcessTask(void *pvParameters)
{
    struct ProcessTaskParams params = *reinterpret_cast<struct ProcessTaskParams *>(pvParameters);

    InferenceProcess::InferenceProcess inferenceProcess(params.model);

    BaseType_t RET;

    for (;;)
    {
        xInferenceJob *xJob;

        RET = xQueueReceive(params.queueHandle, &xJob, DEF_RTOS_WAIT_TIME);
        INFERENCE_ASSERT(RET == pdTRUE, __func__, __LINE__);

        inferenceProcess.RunJob(
            xJob->pPostProc,
            xJob->modelCols,
            xJob->mode1Rows,
            xJob->srcImgWidth,
            xJob->srcImgHeight,
            xJob->results
        );

        RET = xQueueSend(xJob->responseQueue, &xJob, DEF_RTOS_WAIT_TIME);
        INFERENCE_ASSERT(RET == pdTRUE, __func__, __LINE__);
    }
}




