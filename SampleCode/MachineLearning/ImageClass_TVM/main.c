/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate run nn inference via TVM
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "ethosu_driver.h"

#include "tvm_runtime.h"
#include "tvm/runtime/crt/stack_allocator.h"
#include "tvmgen_default.h"
#include "tvm/runtime/crt/module.h"

#include "inputs.h"
#include "labels.h"

/****************************************************************************
 * autogen section: Output tensor data
 ****************************************************************************/
__attribute__((aligned(16), section(".bss.noinit.tvm"))) static uint8_t s_MobilenetV2_Predictions_Reshape_11_buffer[TVMGEN_DEFAULT_MOBILENETV2_PREDICTIONS_RESHAPE_11_SIZE];

/****************************************************************************
 * autogen section: EthosU IRQ handler 
 ****************************************************************************/
#define ethosu_scratch          0
#define ETHOSU_FAST_MEMORY_SIZE 0
struct ethosu_driver ethosu0_driver;
void NPU_IRQHandler(void) {
	ethosu_irq_handler(&ethosu0_driver);
}

// Called to start system timer.
tvm_crt_error_t TVMPlatformTimerStart() {
  return kTvmErrorNoError;
}

// Called to stop system timer.
tvm_crt_error_t TVMPlatformTimerStop(double* elapsed_time_seconds) {
  return kTvmErrorNoError;
}


const TVMModule* TVMSystemLibEntryPoint(void) { return NULL; }

/****************************************************************************
 * InferenceJob
 ****************************************************************************/

int main(void) {

    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART0, 115200);

    printf("\nThis sample code run inference by TVM \n");

/****************************************************************************
 * autogen section: EthosU init 
 ****************************************************************************/
	if (ethosu_init(&ethosu0_driver,
		(void *)(NPU_BASE),
		ethosu_scratch,
		ETHOSU_FAST_MEMORY_SIZE,
		1,
		1)) {
		printf("Failed to initialize NPU.\n");
		return -1;
	}
	NVIC_EnableIRQ(NPU_IRQn);
	ethosu_request_power(&ethosu0_driver);
	struct ethosu_driver *driver = ethosu_reserve_driver();
	struct tvmgen_default_devices devices = {
		.ethos_u = driver,
	};

/****************************************************************************
 * autogen section: Input tensor init 
 ****************************************************************************/
	struct tvmgen_default_inputs inputs = {
		.tfl_quantize = (void *)input,
	};

/****************************************************************************
 * autogen section: Output tensor init 
 ****************************************************************************/
	struct tvmgen_default_outputs outputs = {
		.MobilenetV2_Predictions_Reshape_11 = s_MobilenetV2_Predictions_Reshape_11_buffer,
	};

    printf("Running Inference \n");

    tvmgen_default_run(&inputs, &outputs, &devices);

    ethosu_release_driver(driver);
	// Calculate index of max value
	uint8_t max_value = 0;
	int32_t max_index = -1;

	for (unsigned int i = 0; i < TVMGEN_DEFAULT_MOBILENETV2_PREDICTIONS_RESHAPE_11_SIZE; ++i) {
		if (s_MobilenetV2_Predictions_Reshape_11_buffer[i] > max_value) {
			max_value = s_MobilenetV2_Predictions_Reshape_11_buffer[i];
			max_index = i;
		}
	}
	printf("The image has been classified as '%s'\n", labels[max_index]);
    return 0;
}
