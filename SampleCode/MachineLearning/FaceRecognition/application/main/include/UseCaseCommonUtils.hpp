/*
 * Copyright (c) 2021-2022  Arm Limited. All rights reserved.
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
#ifndef USECASE_COMMON_UTILS_HPP
#define USECASE_COMMON_UTILS_HPP

#include "hal.h"
#include "AppContext.hpp"
#include "InputFiles.hpp"
#include "Model.hpp"
#include "BaseProcessing.hpp"
#include "Profiler.hpp"

#include "UseCaseHandler.hpp"       /* Handlers for different user options. */

void DisplayCommonMenu();

/**
   * @brief           Helper function to increment current input feature vector index.
   * @param[in,out]   ctx       Pointer to the application context object.
   * @param[in]       useCase   Use case name
   **/
void IncrementAppCtxIfmIdx(arm::app::ApplicationContext& ctx, const std::string& useCase);

/**
   * @brief           Helper function to set the input feature map index.
   * @param[in,out]   ctx          Pointer to the application context object.
   * @param[in]       idx          Value to be set.
   * @param[in]       ctxIfmName   Input Feature Map name
   * @return          true if index is set, false otherwise.
   **/
bool SetAppCtxIfmIdx(arm::app::ApplicationContext& ctx, uint32_t idx, const std::string& ctxIfmName);


namespace common {

  enum OPCODES {
        MENU_OPT_RUN_INF_NEXT = 1, /* Run on next vector. */
        MENU_OPT_RUN_INF_CHOSEN, /* Run on a user provided vector index. */
        MENU_OPT_RUN_INF_ALL, /* Run inference on all. */
        MENU_OPT_SHOW_MODEL_INFO, /* Show model info. */
        MENU_OPT_LIST_IFM /* List the current IFM. */
  };

}

namespace arm {
namespace app {
    /**
     * @brief           Read input and return as an integer.
     * @return          Integer value corresponding to the user input.
     **/
    int ReadUserInputAsInt();

#if VERIFY_TEST_OUTPUT
    /**
     * @brief       Helper function to dump a tensor to stdout
     * @param[in]   tensor  tensor to be dumped
     * @param[in]   lineBreakForNumElements     number of elements
     *              after which line break will be added.
     **/
    void DumpTensor(const TfLiteTensor* tensor,
                    size_t lineBreakForNumElements = 16);


    void DumpTensorData(const uint8_t* tensorData,
                        size_t size,
                        size_t lineBreakForNumElements = 16);
#endif /* VERIFY_TEST_OUTPUT */

    /**
     * @brief       List the files baked in the application.
     * @param[in]   ctx   Reference to the application context.
     * @return      true or false based on event being handled.
     **/
    bool ListFilesHandler(ApplicationContext& ctx);

    /**
     * @brief   Use case runner class that will handle calling pre-processing,
     *          inference and post-processing.
     *          After constructing an instance of this class the user can call
     *          PreProcess(), RunInference() and PostProcess() to perform inference.
     */
    class UseCaseRunner {

    private:
        BasePreProcess* m_preProcess;
        BasePostProcess* m_postProcess;
        Model* m_model;

    public:
        explicit UseCaseRunner(BasePreProcess* preprocess, BasePostProcess* postprocess, Model* model)
        : m_preProcess{preprocess},
          m_postProcess{postprocess},
          m_model{model}
          {};

        /**
         * @brief       Runs pre-processing as defined by PreProcess object within the runner.
         *              Templated for the input data type.
         * @param[in]   inputData    Pointer to the data that inference will be performed on.
         * @param[in]   inputSize    Size of the input data that inference will be performed on.
         * @return      true if successful, false otherwise.
         **/
        template<typename T>
        bool PreProcess(T* inputData, size_t inputSize) {
            if (!this->m_preProcess->DoPreProcess(inputData, inputSize)) {
                printf_err("Pre-processing failed.");
                return false;
            }
            return true;
        }

        /**
         * @brief       Runs inference with the Model object within the runner.
         * @return      true if successful, false otherwise.
         **/
        bool RunInference() {
            if (!this->m_model->RunInference()) {
                printf_err("Inference failed.");
                return false;
            }
            return true;
        }

        /**
         * @brief       Runs post-processing as defined by PostProcess object within the runner.
         * @return      true if successful, false otherwise.
         **/
        bool PostProcess() {
            if (!this->m_postProcess->DoPostProcess()) {
                printf_err("Post-processing failed.");
                return false;
            }
            return true;
        }
    };

} /* namespace app */
} /* namespace arm */


#endif /* USECASE_COMMON_UTILS_HPP */