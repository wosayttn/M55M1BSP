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
#include "UseCaseHandler.hpp"
#include "VisualWakeWordModel.hpp"
#include "Classifier.hpp"
#include "InputFiles.hpp"
#include "ImageUtils.hpp"
#include "UseCaseCommonUtils.hpp"
#include "hal.h"
#include "log_macros.h"

#include <algorithm>

#include "imlib.h"
#include "framebuffer.h"

#include "glcd.h"
#include "img_sensor.h"

using ImgClassClassifier = arm::app::Classifier;
static bool s_bImgLibInit = false;

//Used by omv library
__attribute__((section(".bss.sram.data"), aligned(16))) static char fb_array[OMV_FB_SIZE + OMV_FB_ALLOC_SIZE];
__attribute__((section(".bss.sram.data"), aligned(16))) static char jpeg_array[OMV_JPEG_BUF_SIZE];

char *_fb_base = NULL;
char *_fb_end = NULL;
char *_jpeg_buf = NULL;
char *_fballoc = NULL;

static void omv_init()
{
	image_t frameBuffer;

	if(s_bImgLibInit == false)
	{
		frameBuffer.w = GLCD_WIDTH;
		frameBuffer.h = GLCD_HEIGHT;
		frameBuffer.size = GLCD_WIDTH * GLCD_HEIGHT * 2;
		frameBuffer.pixfmt = PIXFORMAT_RGB565;

		_fb_base = fb_array;
		_fb_end =  fb_array + OMV_FB_SIZE - 1;
		_fballoc = _fb_base + OMV_FB_SIZE + OMV_FB_ALLOC_SIZE; 
		_jpeg_buf = jpeg_array;

		fb_alloc_init0();

		framebuffer_init0();
		framebuffer_init_from_image(&frameBuffer);

		s_bImgLibInit = true;
	}
}

namespace arm {
namespace app {

    /**
    * @brief            Helper function to load the current image into the input
    *                   tensor.
    * @param[in]        imIdx         Image index (from the pool of images available
    *                                 to the application).
    * @param[out]       inputTensor   Pointer to the input tensor to be populated.
    * @return           true if tensor is loaded, false otherwise.
    **/
    static bool LoadImageIntoTensor(uint32_t imIdx,
                                     TfLiteTensor *inputTensor);

    /* Image inference classification handler. */
    bool ClassifyImageHandler(ApplicationContext &ctx, uint32_t imgIndex, bool runAll, bool useCCAP)
    {
        auto& profiler = ctx.Get<Profiler&>("profiler");

        constexpr uint32_t dataPsnImgDownscaleFactor = 1;
        constexpr uint32_t dataPsnImgStartX = 10;
        constexpr uint32_t dataPsnImgStartY = 35;

        constexpr uint32_t dataPsnTxtInfStartX = 150;
        constexpr uint32_t dataPsnTxtInfStartY = 70;

        auto& model = ctx.Get<Model&>("model");

        /* If the request has a valid size, set the image index. */
        if (imgIndex < NUMBER_OF_FILES) {
            if (!SetAppCtxIfmIdx(ctx, imgIndex,"imgIndex")) {
                return false;
            }
        }

        image_t frameBuffer;
		rectangle_t roi;

        omv_init();
        framebuffer_init_image(&frameBuffer);

        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        auto curImIdx = ctx.Get<uint32_t>("imgIndex");

        TfLiteTensor *outputTensor = model.GetOutputTensor(0);
        TfLiteTensor *inputTensor = model.GetInputTensor(0);

        if (!inputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return false;
        } else if (inputTensor->dims->size < 3) {
            printf_err("Input tensor dimension should be >= 3\n");
            return false;
        }
        TfLiteIntArray* inputShape = model.GetInputShape(0);
        const uint32_t nCols = inputShape->data[arm::app::VisualWakeWordModel::ms_inputColsIdx];
        const uint32_t nRows = inputShape->data[arm::app::VisualWakeWordModel::ms_inputRowsIdx];
        if (arm::app::VisualWakeWordModel::ms_inputChannelsIdx >= static_cast<uint32_t>(inputShape->size)) {
            printf_err("Invalid channel index.\n");
            return false;
        }
        const uint32_t nChannels = inputShape->data[arm::app::VisualWakeWordModel::ms_inputChannelsIdx];

        std::vector<ClassificationResult> results;

        do {
            hal_lcd_clear(COLOR_BLACK);

            /* Strings for presentation/logging. */
            std::string str_inf{"Running inference... "};

			if (useCCAP == false) {
				const uint8_t* imgSrc = get_img_array(ctx.Get<uint32_t>("imgIndex"));
				if (nullptr == imgSrc) {
					printf_err("Failed to get image index %" PRIu32 " (max: %u)\n", ctx.Get<uint32_t>("imgIndex"),
							   NUMBER_OF_FILES - 1);
					return false;
				}

                /* Select the image to run inference with. */
                info("Running inference on image %" PRIu32 " => %s\n", ctx.Get<uint32_t>("imgIndex"),
                    get_filename(ctx.Get<uint32_t>("imgIndex")));
                image_t srcImg;

				srcImg.w = IMAGE_WIDTH;
				srcImg.h = IMAGE_HEIGHT;
				srcImg.data = (uint8_t *)imgSrc;
				srcImg.pixfmt = PIXFORMAT_RGB888;
	
				roi.x = 0;
				roi.y = 0;
				roi.w = IMAGE_WIDTH;
				roi.h = IMAGE_HEIGHT;

				imlib_nvt_scale(&srcImg, &frameBuffer, &roi);
			}
            else
            {
                //capture frame from CCAP
				img_sensor_capture((uint32_t)(frameBuffer.data));    
            }

            /* Display this image on the LCD. */
            hal_lcd_display_image(
                frameBuffer.data,
                frameBuffer.w, frameBuffer.h, 2,
                dataPsnImgStartX, dataPsnImgStartY, dataPsnImgDownscaleFactor);

			/*resize image to model input*/
			image_t resizeImg;

			roi.x = 0;
			roi.y = 0;
			roi.w = frameBuffer.w;
			roi.h = frameBuffer.h;

			resizeImg.w = nCols;
			resizeImg.h = nRows;
			resizeImg.data = (uint8_t *)inputTensor->data.data; //direct resize to input tensor buffer

			if (1 == nChannels)
				resizeImg.pixfmt =  PIXFORMAT_GRAYSCALE;
			else
				resizeImg.pixfmt = PIXFORMAT_RGB888;

			imlib_nvt_scale(&frameBuffer, &resizeImg, &roi);

            /* Vww model preprocessing is image conversion from uint8 to [0,1] float values,
             * then quantize them with input quantization info. */
            QuantParams inQuantParams = GetTensorQuantParams(inputTensor);

            auto* req_data = static_cast<uint8_t *>(inputTensor->data.data);
            auto* signed_req_data = static_cast<int8_t *>(inputTensor->data.data);
            for (size_t i = 0; i < inputTensor->bytes; i++) {
                auto i_data_int8 = static_cast<int8_t>(((static_cast<float>(req_data[i]) / 255.0f) / inQuantParams.scale) + inQuantParams.offset);
                signed_req_data[i] = std::min<int8_t>(INT8_MAX, std::max<int8_t>(i_data_int8, INT8_MIN));
            }

            /* Display message on the LCD - inference running. */
            hal_lcd_display_text(
                                str_inf.c_str(), str_inf.size(),
                                dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            if (!RunInference(model, profiler)) {
                return false;
            }

            /* Erase. */
            str_inf = std::string(str_inf.size(), ' ');
            hal_lcd_display_text(
                                str_inf.c_str(), str_inf.size(),
                                dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            auto& classifier = ctx.Get<Classifier&>("classifier");
            classifier.GetClassificationResults(outputTensor, results,
                                                ctx.Get<std::vector <std::string>&>("labels"), 1,
                                                false);

            /* Add results to context for access outside handler. */
            ctx.Set<std::vector<ClassificationResult>>("results", results);

#if VERIFY_TEST_OUTPUT
            arm::app::DumpTensor(outputTensor);
#endif /* VERIFY_TEST_OUTPUT */

            if (!PresentInferenceResult(results)) {
                return false;
            }

            profiler.PrintProfilingResult();
            IncrementAppCtxIfmIdx(ctx,"imgIndex");

        } while (runAll && ctx.Get<uint32_t>("imgIndex") != curImIdx);

        return true;
    }

    static bool LoadImageIntoTensor(const uint32_t imIdx,
                                     TfLiteTensor *inputTensor)
    {
        const size_t copySz = inputTensor->bytes < IMAGE_DATA_SIZE ?
                                inputTensor->bytes : IMAGE_DATA_SIZE;
        if (imIdx >= NUMBER_OF_FILES) {
            printf_err("invalid image index %" PRIu32 " (max: %u)\n", imIdx,
                       NUMBER_OF_FILES - 1);
            return false;
        }

        if (arm::app::VisualWakeWordModel::ms_inputChannelsIdx >= static_cast<uint32_t>(inputTensor->dims->size)) {
            printf_err("Invalid channel index.\n");
            return false;
        }
        const uint32_t nChannels = inputTensor->dims->data[arm::app::VisualWakeWordModel::ms_inputChannelsIdx];

        const uint8_t* srcPtr = get_img_array(imIdx);
        auto* dstPtr = static_cast<uint8_t *>(inputTensor->data.data);
        if (1 == nChannels) {
            /**
             * Visual Wake Word model accepts only one channel =>
             * Convert image to grayscale here
             **/
            image::RgbToGrayscale(srcPtr, dstPtr, copySz);
        } else {
            memcpy(inputTensor->data.data, srcPtr, copySz);
        }

        debug("Image %" PRIu32 " loaded\n", imIdx);
        return true;
    }

} /* namespace app */
} /* namespace arm */