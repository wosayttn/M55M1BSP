/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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
#include "hal.h"                      /* Brings in platform definitions. */
#include "InputFiles.hpp"             /* For input images. */
#include "YoloFastestModel.hpp"       /* Model class for running inference. */
#include "UseCaseHandler.hpp"         /* Handlers for different user options. */
#include "UseCaseCommonUtils.hpp"     /* Utils functions. */
#include "DetectorPostProcessing.hpp" /* Post-processing class. */
#include "InferenceProcess.hpp"
#include "log_macros.h"
#include "Labels.hpp"               /* For label strings. */
#include "NuMicro.h"

#if defined USE_CCAP
#include "glcd.h"
#include "img_sensor.h"
#endif

#define INFERENCE_TASK_PRIO	4

static void DisplayDetectionMenu()
{
    printf("\n\n");
    printf("User input required\n");
    printf("Enter option number from:\n\n");
    printf("  %u. Run detection on next ifm\n", common::MENU_OPT_RUN_INF_NEXT);
    printf("  %u. Run detection ifm at chosen index\n", common::MENU_OPT_RUN_INF_CHOSEN);
    printf("  %u. Run detection on all ifm\n", common::MENU_OPT_RUN_INF_ALL);
    printf("  %u. Show NN model info\n", common::MENU_OPT_SHOW_MODEL_INFO);
    printf("  %u. List ifm\n\n", common::MENU_OPT_LIST_IFM);
    printf("  Choice: ");
    fflush(stdout);
}

enum { NonCache_index, WTRA_index, WBWARA_index };

static void initializeAttributes() {
#ifdef ARM_MPU_ARMV8_H
    /* Initialize attributes corresponding to the enums defined in mpu.hpp */
    const uint8_t WTRA =
        ARM_MPU_ATTR_MEMORY_(1, 0, 1, 0); // Non-transient, Write-Through, Read-allocate, Not Write-allocate
    const uint8_t WBWARA = ARM_MPU_ATTR_MEMORY_(1, 1, 1, 1); // Non-transient, Write-Back, Read-allocate, Write-allocate

    ARM_MPU_SetMemAttr(NonCache_index, ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));
    ARM_MPU_SetMemAttr(WTRA_index, ARM_MPU_ATTR(WTRA, WTRA));
    ARM_MPU_SetMemAttr(WBWARA_index, ARM_MPU_ATTR(WBWARA, WBWARA));
#endif
}

void loadAndEnableConfig(ARM_MPU_Region_t const *table, uint32_t cnt) {
#ifdef ARM_MPU_ARMV8_H
    initializeAttributes();

    ARM_MPU_Load(0, table, cnt);

    // Enable MPU with default priv access to all other regions
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
#endif
}

//void main_loop(hal_platform& platform)
void main_loop(void *pvParameters) 
{
	hal_platform* platform = (hal_platform*)pvParameters;
	//hal_platform platform = *platform_arg;
	
    arm::app::YoloFastestModel model;  /* Model wrapper object. */

	printf("Enable I/D cache \n");
	SCB_EnableICache();
	SCB_EnableDCache();
	
    /* Load the model. */
    if (!model.Init()) {
        printf_err("Failed to initialise model\n");
		vTaskDelete(nullptr);
        return;
    }

	printf("Set tesnor arena to non-cacheable \n");
    // MPU setup
    const std::vector<ARM_MPU_Region_t> mpuConfig = {
        {
            // SRAM for tensor arena
            ARM_MPU_RBAR(((unsigned int)model.GetTensorArena()),        // Base
                         ARM_MPU_SH_NON,    // Non-shareable
                         0,                 // Read-only
                         1,                 // Non-Privileged
                         1),                // eXecute Never enabled
            ARM_MPU_RLAR((((unsigned int)model.GetTensorArena()) + ACTIVATION_BUF_SZ - 1),        // Limit
                         WTRA_index) // Attribute index - Write-Back, Write-Allocate, Read-allocate
	     }
	};

    // Setup MPU configuration
    loadAndEnableConfig(&mpuConfig[0], mpuConfig.size());
	
    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{platform, "object_detection"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    //caseContext.Set<hal_platform&>("platform", platform);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<uint32_t>("imgIndex", 0);
    arm::app::object_detection::DetectorPostprocessing postp(0.5, 0.45, numClasses, 0);
    caseContext.Set<arm::app::object_detection::DetectorPostprocessing&>("postprocess", postp);

    std::vector<std::string> labels;
    GetLabelsVector(labels);
    caseContext.Set<const std::vector <std::string>&>("labels", labels);

	//Init inference resource and create task
	struct ProcessTaskParams taskParam;
	QueueHandle_t inferenceProcessQueue = xQueueCreate(1, sizeof(xInferenceJob *));
	QueueHandle_t inferenceResponseQueue = xQueueCreate(1, sizeof(xInferenceJob *));
    caseContext.Set<QueueHandle_t &>("inference_proc_queue", inferenceProcessQueue);
    caseContext.Set<QueueHandle_t &>("inference_resp_queue", inferenceResponseQueue);
	
	taskParam.model = &model;
	taskParam.queueHandle = inferenceProcessQueue;

	BaseType_t ret;
 
	ret = xTaskCreate(inferenceProcessTask, "inference task", 1 * 1024, &taskParam, INFERENCE_TASK_PRIO, nullptr);
    if (ret != pdPASS) {
		printf_err("FreeRTOS: Failed to inference process task \n");
		vTaskDelete(nullptr);
		return;
	}	

#if defined USE_CCAP
    TfLiteIntArray* inputShape = model.GetInputShape(0);

    //const uint32_t model_nCols = inputShape->data[arm::app::MobileNetModel::ms_inputColsIdx];
    //const uint32_t model_nRows = inputShape->data[arm::app::MobileNetModel::ms_inputRowsIdx];
    const uint32_t model_nCols = GLCD_WIDTH;
    const uint32_t model_nRows = GLCD_HEIGHT;

    img_sensor_init();
    img_sensor_config(eIMG_FMT_RGB565, model_nCols, model_nRows);

    bool executionSuccessful = true;
    constexpr bool bUseMenu = false;
#else
    /* Main program loop. */
    bool executionSuccessful = true;
    constexpr bool bUseMenu = NUMBER_OF_FILES > 1 ? true : false;
#endif
	
    /* Loop. */
    do {
        int menuOption = common::MENU_OPT_RUN_INF_NEXT;
        if (bUseMenu) {
            DisplayCommonMenu();
            menuOption = arm::app::ReadUserInputAsInt();
            printf("\n");
        }
        switch (menuOption) {
            case common::MENU_OPT_RUN_INF_NEXT:
#if defined USE_CCAP
                executionSuccessful = ObjectDetectionHandler(caseContext, caseContext.Get<uint32_t>("imgIndex"), false, true);
#else
                executionSuccessful = ObjectDetectionHandler(caseContext, caseContext.Get<uint32_t>("imgIndex"), false, false);
#endif
				break;
            case common::MENU_OPT_RUN_INF_CHOSEN: {
                printf("    Enter the image index [0, %d]: ", NUMBER_OF_FILES-1);
                fflush(stdout);
                auto imgIndex = static_cast<uint32_t>(arm::app::ReadUserInputAsInt());
                executionSuccessful = ObjectDetectionHandler(caseContext, imgIndex, false, false);
                break;
            }
            case common::MENU_OPT_RUN_INF_ALL:
                executionSuccessful = ObjectDetectionHandler(caseContext, caseContext.Get<uint32_t>("imgIndex"), true, false);
                break;
            case common::MENU_OPT_SHOW_MODEL_INFO:
                executionSuccessful = model.ShowModelInfoHandler();
                break;
            case common::MENU_OPT_LIST_IFM:
                executionSuccessful = ListFilesHandler(caseContext);
                break;
            default:
                printf("Incorrect choice, try again.");
                break;
        }
    } while (executionSuccessful && bUseMenu);

    info("Main loop terminated.\n");
    vTaskDelete(nullptr);
}
