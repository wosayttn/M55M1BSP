/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
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
#include "hal.h"                    /* Brings in platform definitions. */
#include "Classifier.hpp"           /* Classifier. */
#include "InputFiles.hpp"           /* For input images. */
#include "Labels.hpp"               /* For label strings. */
#include "VisualWakeWordModel.hpp" /* Model class for running inference. */
#include "UseCaseHandler.hpp"       /* Handlers for different user options. */
#include "UseCaseCommonUtils.hpp"   /* Utils functions. */
#include "log_macros.h"

#if defined USE_CCAP
#include "glcd.h"
#include "img_sensor.h"
#endif

using ViusalWakeWordClassifier = arm::app::Classifier;

void main_loop(hal_platform &platform)
{
    arm::app::VisualWakeWordModel model;  /* Model wrapper object. */

    /* Load the model. */
    if (!model.Init()) {
        printf_err("Failed to initialise model\n");
        return;
    }

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{&platform, "vww"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<hal_platform&>("platform", platform);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<uint32_t>("imgIndex", 0);

    ViusalWakeWordClassifier classifier;  /* Classifier wrapper object. */
    caseContext.Set<arm::app::Classifier&>("classifier", classifier);

    std::vector <std::string> labels;
    GetLabelsVector(labels);
    caseContext.Set<const std::vector <std::string>&>("labels", labels);

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
	/* Loop. */
    bool executionSuccessful = true;
    constexpr bool bUseMenu = NUMBER_OF_FILES > 1 ? true : false;
#endif

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
				executionSuccessful = ClassifyImageHandler(caseContext, caseContext.Get<uint32_t>("imgIndex"), false, true);
#else
				executionSuccessful = ClassifyImageHandler(caseContext, caseContext.Get<uint32_t>("imgIndex"), false, false);
#endif
				break;
            case common::MENU_OPT_RUN_INF_CHOSEN: {
                printf("    Enter the image index [0, %d]: ", NUMBER_OF_FILES-1);
                fflush(stdout);
                auto imgIndex = static_cast<uint32_t>(arm::app::ReadUserInputAsInt());
                executionSuccessful = ClassifyImageHandler(caseContext, imgIndex, false, false);
                break;
            }
            case common::MENU_OPT_RUN_INF_ALL:
                executionSuccessful = ClassifyImageHandler(caseContext, caseContext.Get<uint32_t>("imgIndex"), true, false);
                break;
            case common::MENU_OPT_SHOW_MODEL_INFO: {
                executionSuccessful = model.ShowModelInfoHandler();
                break;
            }
            case common::MENU_OPT_LIST_IFM:
                executionSuccessful = ListFilesHandler(caseContext);
                break;
            default:
                printf("Incorrect choice, try again.");
                break;
        }
    } while (executionSuccessful && bUseMenu);
    info("Main loop terminated.\n");

}