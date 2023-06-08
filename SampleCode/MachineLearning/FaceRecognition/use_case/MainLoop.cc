/**************************************************************************//**
 * @file     MainLoop.cc
 * @version  V0.10
 * @brief    Face recognition main loop
 * * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "hal.h"                    /* Brings in platform definitions */
#include "InputFiles.hpp"           /* For input data */
#include "UseCaseCommonUtils.hpp"   /* Utils functions */
#include "UseCaseHandler.hpp"       /* Handlers for different user options */
#include "log_macros.h"
#include "Labels.hpp"               /* For label strings. */
#include "MobileNetModel.hpp"       /* Model class for running inference. */
#include "Recognizer.hpp"
#include "NuMicro.h"

#if defined USE_CCAP
#include "glcd.h"
#include "img_sensor.h"
#endif

using FaceRecognizer = arm::app::Recognizer;

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

void main_loop(hal_platform& platform)
{
    arm::app::MobileNetModel model;  /* Model wrapper object. */

	printf("Enable I/D cache \n");
	SCB_EnableICache();
	SCB_EnableDCache();
	
    /* Load the model. */
    if (!model.Init()) {
        printf_err("Failed to initialise model\n");
        return;
    }

#if 1	
	printf("Set tesnor arena to WTRA \n");
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
#endif	
	
    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{&platform, "face recognition"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<hal_platform&>("platform", platform);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<uint32_t>("imgIndex", 0);

    FaceRecognizer recognizer;  /* Classifier wrapper object. */
    caseContext.Set<arm::app::Recognizer&>("recognizer", recognizer);

    std::vector<std::string> labels;
    std::vector<S_LABEL_INFO> LabelInfo;

    GetLabelsVector(labels);
	ParserLabelVector(labels, LabelInfo, nullptr);
    caseContext.Set<const std::vector<S_LABEL_INFO>&>("labels", LabelInfo);
	
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
                executionSuccessful = FaceRecognImageHandler(caseContext, caseContext.Get<uint32_t>("imgIndex"), false, true);
#else
                executionSuccessful = FaceRecognImageHandler(caseContext, caseContext.Get<uint32_t>("imgIndex"), false, false);                
#endif
                break;
            case common::MENU_OPT_RUN_INF_CHOSEN: {
                printf("    Enter the image index [0, %d]: ", NUMBER_OF_FILES-1);
                fflush(stdout);
                auto imgIndex = static_cast<uint32_t>(arm::app::ReadUserInputAsInt());
                executionSuccessful = FaceRecognImageHandler(caseContext, imgIndex, false, false);
                break;
            }
            case common::MENU_OPT_RUN_INF_ALL:
                executionSuccessful = FaceRecognImageHandler(caseContext, caseContext.Get<uint32_t>("imgIndex"), true, false);
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
	}while(executionSuccessful && bUseMenu);
    info("Main loop terminated.\n");
}
