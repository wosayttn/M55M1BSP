/*
 * Copyright (c) 2021-2022 Arm Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/****************************************************************************\
 *               Main application file for ARM NPU on MPS3 board             *
\****************************************************************************/
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "hal.h"                    /* our hardware abstraction api */
#include "log_macros.h"
#include "TensorFlowLiteMicro.hpp"  /* our inference logic api */

#include <cstdio>

#define PRJ_DES_STR "ARM ML Embedded Evaluation Kit for MPS3 FPGA and FastModel"
#define PRJ_VER_STR "22.02"

//extern void main_loop(hal_platform& platform);
extern void main_loop(void *pvParameters); 
#ifdef __cplusplus
extern "C" {
#endif

extern void SysTick_Hook(void);
#ifdef __cplusplus
}
#endif


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm(" .global __ARM_use_no_argv\n");
#endif

#define MAINLOOP_TASK_PRIO	3

/* Print application information. */
static void print_application_intro()
{
    info("%s\n", PRJ_DES_STR);
    info("Version %s Build date: " __DATE__ " @ " __TIME__ "\n", PRJ_VER_STR);
    info("Copyright (C) ARM Ltd 2021-2022. All rights reserved.\n\n");
}

int main ()
{
	BaseType_t ret;
    hal_platform *platform = new(hal_platform);
    platform_timer  *timer = new(platform_timer);

    /* Initialise the HAL and platform. */
    hal_init(platform, timer);

    if (0 != hal_platform_init(platform)) {
		return -1;
	}


	/* Application information, UART should have been initialised. */
	print_application_intro();

	/* Check the version of TensorFlow Lite Micro. */
	PrintTensorFlowVersion();

	/* Run the application. */
	ret = xTaskCreate(main_loop, "main task", 2 * 1024, platform, MAINLOOP_TASK_PRIO, nullptr);
    if (ret != pdPASS) {
		info("FreeRTOS: Failed to create main look task \n");
		return -2;
	}	

    // Start Scheduler
    vTaskStartScheduler();

    /* This is unreachable without errors. */
    info("program terminating...\n");

    /* Release platform. */
    hal_platform_release(platform);
	delete platform;
	delete timer;
    return 0;
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
	info("vApplicationMallocFailedHook\n");
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
	info("vApplicationStackOverflowHook\n");
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

	SysTick_Hook();
}
/*-----------------------------------------------------------*/
