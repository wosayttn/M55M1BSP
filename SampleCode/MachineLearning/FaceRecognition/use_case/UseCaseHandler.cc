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

#include <vector>
#include <string>
#include <cinttypes>

#include "UseCaseHandler.hpp"
#include "InputFiles.hpp"
#include "MobileNetModel.hpp"
#include "UseCaseCommonUtils.hpp"
#include "ImageUtils.hpp"
#include "UseCaseCommonUtils.hpp"
#include "hal.h"
#include "log_macros.h"
#include "FaceRecognProcessing.hpp"
#include "Recognizer.hpp"

#include "imlib.h"
#include "framebuffer.h"

#include "glcd.h"
#include "img_sensor.h"

#if defined(__PROFILE)
extern "C" {
	extern uint64_t Prof_Get_SysTick_Cycle_Count(void);
}
#endif

using FaceRecognizer = arm::app::Recognizer;
using namespace std;

static bool s_bImgLibInit = false;

//Used by omv library
__attribute__((section(".sram.data"), aligned(16))) static char fb_array[OMV_FB_SIZE + OMV_FB_ALLOC_SIZE];
__attribute__((section(".sram.data"), aligned(16))) static char jpeg_array[OMV_JPEG_BUF_SIZE];

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

    bool FaceRecognImageHandler(ApplicationContext& ctx, uint32_t imgIndex, bool runAll, bool useCCAP)
	{
        auto& profiler = ctx.Get<Profiler&>("profiler");
        auto& model = ctx.Get<Model&>("model");
        auto initialImIdx = ctx.Get<uint32_t>("imgIndex");

		uint32_t dataPsnImgDownscaleFactor = 1;
        constexpr uint32_t dataPsnImgStartX = 0;
        constexpr uint32_t dataPsnImgStartY = 0;

        constexpr uint32_t dataPsnTxtInfStartX = 150;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        /* If the request has a valid size, set the image index. */
        if (imgIndex < NUMBER_OF_FILES) {
            if (!SetAppCtxIfmIdx(ctx, imgIndex, "imgIndex")) {
                return false;
            }
        }

        image_t frameBuffer;

        omv_init();
        framebuffer_init_image(&frameBuffer);

        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }
		
        TfLiteTensor* inputTensor = model.GetInputTensor(0);
        if (!inputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return false;
        } else if (inputTensor->dims->size < 3) {
            printf_err("Input tensor dimension should be >= 3\n");
            return false;
        }
 
        /* Get input shape */
        TfLiteIntArray* inputShape = model.GetInputShape(0);
        const uint32_t nCols = inputShape->data[arm::app::MobileNetModel::ms_inputColsIdx];
        const uint32_t nRows = inputShape->data[arm::app::MobileNetModel::ms_inputRowsIdx];
        const uint32_t nChannels = inputShape->data[arm::app::MobileNetModel::ms_inputChannelsIdx];
		
        /* Set up pre and post-processing. */
        FaceRecognPreProcess preprocess = FaceRecognPreProcess(&model);

		RecognitionResult result;
		string predictLabelInfo;

        FaceRecognPostProcess postprocess = FaceRecognPostProcess(ctx.Get<FaceRecognizer&>("recognizer"), &model,
                ctx.Get<std::vector<S_LABEL_INFO>&>("labels"), result);

        UseCaseRunner runner = UseCaseRunner(&preprocess, &postprocess, &model);
		std::vector<uint8_t>vecResizeData(nCols * nRows* nChannels);
		//uint8_t *pu8ResizeData = new uint8_t[DISPLAY_IMAGE_WIDTH * DISPLAY_IMAGE_HEIGHT * 3];
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

				/* Display message on the LCD - inference running. */

                /* Select the image to run inference with. */
                info("Running inference on image %" PRIu32 " => %s\n", ctx.Get<uint32_t>("imgIndex"),
                    get_filename(ctx.Get<uint32_t>("imgIndex")));
                        image_t srcImg;
				rectangle_t roi;

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

			FaceRecognPreProcess::S_FACE_RECOGN_IMAGE sSrcImage;
			FaceRecognPreProcess::S_FACE_RECOGN_IMAGE_ROI sROI;

			sSrcImage.eColorFormat = FaceRecognPreProcess::eFACE_RECOGN_COLOR_RGB565;
			sSrcImage.u32Width = frameBuffer.w;
			sSrcImage.u32Height = frameBuffer.h;
			sSrcImage.pu8Data = (uint8_t *)frameBuffer.data;

#if defined(__PROFILE)
			platform_reset_counters();
			uint64_t u64StartCPUCycle = Prof_Get_SysTick_Cycle_Count();
#endif			

			if(preprocess.RunFaceDetect(&sSrcImage, &sROI) == false)
			{
				info("Face not found on image %" PRIu32 " => %s\n", ctx.Get<uint32_t>("imgIndex"),
					get_filename(ctx.Get<uint32_t>("imgIndex")));
				IncrementAppCtxIfmIdx(ctx,"imgIndex");
				continue;
			}

			info("Face detect on image (x,y,w,h) => (%d, %d, %d, %d)\n", sROI.u32X, sROI.u32Y, sROI.u32Width, sROI.u32Height);
#if defined(__PROFILE)
			uint64_t u64Temp = Prof_Get_SysTick_Cycle_Count();

			printf("Face detect cycle count %lld \n", u64Temp - u64StartCPUCycle);
#endif			
			
			FaceRecognPreProcess::S_FACE_RECOGN_IMAGE sResizeImage;

			sResizeImage.eColorFormat = FaceRecognPreProcess::eFACE_RECOGN_COLOR_RGB888;
			sResizeImage.u32Width = nCols;
			sResizeImage.u32Height = nRows;
			sResizeImage.pu8Data = &vecResizeData[0];
			
#if defined(__PROFILE)
			platform_reset_counters();
			u64StartCPUCycle = Prof_Get_SysTick_Cycle_Count();
#endif			
			preprocess.FaceResize(&sSrcImage, &sResizeImage, &sROI);  

#if defined(__PROFILE)
			u64Temp = Prof_Get_SysTick_Cycle_Count();			
			printf("Face resize cycle count %lld \n", u64Temp - u64StartCPUCycle);
#endif			

            /* Display this image on the LCD. */
			imlib_draw_rectangle(&frameBuffer, sROI.u32X, sROI.u32Y, sROI.u32Width, sROI.u32Height, COLOR_B5_MAX, 2, false);
            hal_lcd_display_image(
                frameBuffer.data,
                frameBuffer.w, frameBuffer.h, 2,
                dataPsnImgStartX, dataPsnImgStartY, dataPsnImgDownscaleFactor);

            /* Run the pre-processing, inference and post-processing. */
            if (!runner.PreProcess(sResizeImage.pu8Data, (sResizeImage.u32Width * sResizeImage.u32Height * 3))) {
                return false;
            }

            profiler.StartProfiling("Inference");

            if (!runner.RunInference()) {
                return false;
            }

            profiler.StopProfiling();
			predictLabelInfo.clear();
		
            if (runner.PostProcess()) {
				predictLabelInfo =  result.m_label + string(":") + to_string(result.m_predict);         
            }
			else {
				predictLabelInfo = string("???") + string(":") + to_string(result.m_predict);
			}

			imlib_draw_string(&frameBuffer, 10, 220, predictLabelInfo.c_str(), COLOR_B5_MAX, 2, 4, 4, false,
                       false, false, false, 0, false, false);

            hal_lcd_display_image(
                frameBuffer.data,
                frameBuffer.w, frameBuffer.h, 2,
                dataPsnImgStartX, dataPsnImgStartY, dataPsnImgDownscaleFactor);

            profiler.PrintProfilingResult();
            IncrementAppCtxIfmIdx(ctx,"imgIndex");

        } while (runAll && ctx.Get<uint32_t>("imgIndex") != initialImIdx);
        return true;
	}
} /* namespace app */
} /* namespace arm */
