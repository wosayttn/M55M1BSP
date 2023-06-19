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
#include "UseCaseHandler.hpp"
#include "InputFiles.hpp"
#include "YoloFastestModel.hpp"
#include "UseCaseCommonUtils.hpp"
#include "DetectorPostProcessing.hpp"
#include "hal.h"
#include "log_macros.h"
#include "InferenceProcess.hpp"

#include "imlib.h"
#include "framebuffer.h"
#include "glcd.h"
#include "img_sensor.h"

#include <cinttypes>

#define NUM_FRAMEBUF 2

typedef enum{
	eFRAMEBUF_EMPTY,
	eFRAMEBUF_FULL,
	eFRAMEBUF_INF	
}E_FRAMEBUF_STATE;

typedef struct{
	E_FRAMEBUF_STATE eState;
	image_t frameImage;
	std::vector<object_detection::DetectionResult> results;
}S_FRAMEBUF;

static bool s_bImgLibInit = false;
//Used by omv library
__attribute__((section(".bss.sram.data"), aligned(16))) static char fb_array[OMV_FB_SIZE + OMV_FB_ALLOC_SIZE];
__attribute__((section(".bss.sram.data"), aligned(16))) static char jpeg_array[OMV_JPEG_BUF_SIZE];
__attribute__((section(".bss.sram.data"), aligned(16))) static char frame_buf1[OMV_FB_SIZE];

char *_fb_base = NULL;
char *_fb_end = NULL;
char *_jpeg_buf = NULL;
char *_fballoc = NULL;

S_FRAMEBUF s_asFramebuf[NUM_FRAMEBUF];

static void omv_init()
{
	image_t frameBuffer;
	int i;
	
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

		for(i = 0 ; i < NUM_FRAMEBUF; i++)
		{
			s_asFramebuf[i].eState = eFRAMEBUF_EMPTY;
		}
		
		framebuffer_init_image(&s_asFramebuf[0].frameImage);
		s_asFramebuf[1].frameImage.w = GLCD_WIDTH;
		s_asFramebuf[1].frameImage.h = GLCD_HEIGHT;
		s_asFramebuf[1].frameImage.size = GLCD_WIDTH * GLCD_HEIGHT * 2;
		s_asFramebuf[1].frameImage.pixfmt = PIXFORMAT_RGB565;
		s_asFramebuf[1].frameImage.data = (uint8_t *)frame_buf1;
		
		s_bImgLibInit = true;
	}
}

static S_FRAMEBUF* get_empty_framebuf()
{
	int i;

	for(i = 0; i < NUM_FRAMEBUF; i ++)
	{
		if(s_asFramebuf[i].eState == eFRAMEBUF_EMPTY)
			return &s_asFramebuf[i];
	}

	return NULL;
}

static S_FRAMEBUF* get_full_framebuf()
{
	int i;

	for(i = 0; i < NUM_FRAMEBUF; i ++)
	{
		if(s_asFramebuf[i].eState == eFRAMEBUF_FULL)
			return &s_asFramebuf[i];
	}

	return NULL;
}

static S_FRAMEBUF* get_inf_framebuf()
{
	int i;

	for(i = 0; i < NUM_FRAMEBUF; i ++)
	{
		if(s_asFramebuf[i].eState == eFRAMEBUF_INF)
			return &s_asFramebuf[i];
	}

	return NULL;
}


namespace arm {
namespace app {

    /**
     * @brief           Presents inference results along using the data presentation
     *                  object.
     * @param[in]       results            Vector of detection results to be displayed.
     * @param[in]       labels             Vector of detection class lable to be displayed.
     * @return          true if successful, false otherwise.
     **/
    static bool PresentInferenceResult(const std::vector<arm::app::object_detection::DetectionResult>& results,
	                                   std::vector<std::string>& labels);

    /**
     * @brief           Draw boxes directly on the LCD for all detected objects.
     * @param[in]       results            Vector of detection results to be displayed.
     * @param[in]       imageStartX        X coordinate where the image starts on the LCD.
     * @param[in]       imageStartY        Y coordinate where the image starts on the LCD.
     * @param[in]       imgDownscaleFactor How much image has been downscaled on LCD.
     **/
    static void DrawDetectionBoxes(
            const std::vector<arm::app::object_detection::DetectionResult>& results,
           uint32_t imgStartX,
           uint32_t imgStartY,
           uint32_t imgDownscaleFactor);
		
	static void DrawImageDetectionBoxes(
		const std::vector<arm::app::object_detection::DetectionResult>& results,
		image_t *drawImg,
		std::vector<std::string>& labels);
			
    /* Object detection classification handler. */
    bool ObjectDetectionHandler(ApplicationContext& ctx, uint32_t imgIndex, bool runAll, bool useCCAP)
    {
        auto& profiler = ctx.Get<Profiler&>("profiler");


		
        constexpr uint32_t dataPsnImgDownscaleFactor = 1;
        constexpr uint32_t dataPsnImgStartX = 0;
        constexpr uint32_t dataPsnImgStartY = 0;

        constexpr uint32_t dataPsnTxtInfStartX = 150;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        hal_lcd_clear(COLOR_BLACK);

        auto& model = ctx.Get<Model&>("model");
		std::vector<std::string>& labels = ctx.Get<std::vector<std::string>&>("labels");
	
        /* If the request has a valid size, set the image index. */
        if (imgIndex < NUMBER_OF_FILES) {
            if (!SetAppCtxIfmIdx(ctx, imgIndex, "imgIndex")) {
                return false;
            }
        }

		omv_init();

        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        auto curImIdx = ctx.Get<uint32_t>("imgIndex");

        TfLiteTensor* inputTensor = model.GetInputTensor(0);

        if (!inputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return false;
        } else if (inputTensor->dims->size < 3) {
            printf_err("Input tensor dimension should be >= 3\n");
            return false;
        }

        TfLiteIntArray* inputShape = model.GetInputShape(0);

        const uint32_t nCols = inputShape->data[arm::app::YoloFastestModel::ms_inputColsIdx];
        const uint32_t nRows = inputShape->data[arm::app::YoloFastestModel::ms_inputRowsIdx];
        const uint32_t nChannels = inputShape->data[arm::app::YoloFastestModel::ms_inputChannelsIdx];

        /* Get pre/post-processing objects. */
        auto& postp = ctx.Get<object_detection::DetectorPostprocessing&>("postprocess");

		auto& inferenceProcessQueue = ctx.Get<QueueHandle_t &>("inference_proc_queue");
		auto& inferenceResponseQueue = ctx.Get<QueueHandle_t &>("inference_resp_queue");
		
		S_FRAMEBUF* infFramebuf;
		S_FRAMEBUF* fullFramebuf;
		S_FRAMEBUF* emptyFramebuf;
		
        do {
            hal_lcd_clear(COLOR_BLACK);
            /* Strings for presentation/logging. */
            std::string str_inf{"Running inference... "};

            const uint8_t* curr_image = get_img_array(ctx.Get<uint32_t>("imgIndex"));

            /* Select the image to run inference with. */
            info("Running inference on image %" PRIu32 " => %s\n", ctx.Get<uint32_t>("imgIndex"),
                get_filename(ctx.Get<uint32_t>("imgIndex")));

			rectangle_t roi;
			struct xInferenceJob* inferenceJob = new(struct xInferenceJob);

			profiler.StartProfiling("Inference");

			infFramebuf = get_inf_framebuf();

			if(infFramebuf)
			{
				/* Detector post-processing*/

				inferenceJob->responseQueue = inferenceResponseQueue;
				inferenceJob->pPostProc = &postp;
				inferenceJob->modelCols = nCols;
				inferenceJob->mode1Rows = nRows;
				inferenceJob->srcImgWidth = infFramebuf->frameImage.w;
				inferenceJob->srcImgHeight = infFramebuf->frameImage.h;
				inferenceJob->results = &infFramebuf->results; //&results;
				
				xQueueReceive(inferenceResponseQueue, &inferenceJob, portMAX_DELAY);
			}

			fullFramebuf = get_full_framebuf();
			
			if(fullFramebuf)
			{
				//resize full image to input tensor
				image_t resizeImg;

				roi.x = 0;
				roi.y = 0;
				roi.w = fullFramebuf->frameImage.w;
				roi.h = fullFramebuf->frameImage.h;

				resizeImg.w = nCols;
				resizeImg.h = nRows;
				resizeImg.data = (uint8_t *)inputTensor->data.data; //direct resize to input tensor buffer
				resizeImg.pixfmt = PIXFORMAT_RGB888;

				imlib_nvt_scale(&fullFramebuf->frameImage, &resizeImg, &roi);

				/* If the data is signed. */
				if (model.IsDataSigned()) {
					image::ConvertImgToInt8(inputTensor->data.data, inputTensor->bytes);
				}
				
				//trigger inference
				inferenceJob->responseQueue = inferenceResponseQueue;
				inferenceJob->pPostProc = &postp;
				inferenceJob->modelCols = nCols;
				inferenceJob->mode1Rows = nRows;
				inferenceJob->srcImgWidth = fullFramebuf->frameImage.w;
				inferenceJob->srcImgHeight = fullFramebuf->frameImage.h;
				inferenceJob->results = &fullFramebuf->results;
				
				xQueueSend(inferenceProcessQueue, &inferenceJob, portMAX_DELAY);
				fullFramebuf->eState = eFRAMEBUF_INF;
			}

			if(infFramebuf)
			{
				//draw bbox and render
				/* Draw boxes. */
				DrawImageDetectionBoxes(infFramebuf->results, &infFramebuf->frameImage, labels);

				hal_lcd_display_image(
					infFramebuf->frameImage.data,
					infFramebuf->frameImage.w, infFramebuf->frameImage.h, 2,
					dataPsnImgStartX, dataPsnImgStartY, dataPsnImgDownscaleFactor);

				PresentInferenceResult(infFramebuf->results, labels);
				infFramebuf->eState = eFRAMEBUF_EMPTY;
			}

			emptyFramebuf = get_empty_framebuf();
			
			if(emptyFramebuf)
			{
				if (useCCAP == false) 
				{

					//copy source image to frame buffer
					image_t srcImg;				

					srcImg.w = IMAGE_WIDTH;
					srcImg.h = IMAGE_HEIGHT;
					srcImg.data = (uint8_t *)curr_image;
					srcImg.pixfmt = PIXFORMAT_RGB888;
				
					roi.x = 0;
					roi.y = 0;
					roi.w = IMAGE_WIDTH;
					roi.h = IMAGE_HEIGHT;

					imlib_nvt_scale(&srcImg, &emptyFramebuf->frameImage, &roi);
				}
				else
				{
					//capture frame from CCAP
					img_sensor_capture((uint32_t)(emptyFramebuf->frameImage.data));    
				}
					
				emptyFramebuf->results.clear();
				emptyFramebuf->eState = eFRAMEBUF_FULL;
			}
			profiler.StopProfiling();

#if VERIFY_TEST_OUTPUT
            arm::app::DumpTensor(modelOutput0);
            arm::app::DumpTensor(modelOutput1);
#endif /* VERIFY_TEST_OUTPUT */

            profiler.PrintProfilingResult();

            IncrementAppCtxIfmIdx(ctx,"imgIndex");

        } while (runAll && ctx.Get<uint32_t>("imgIndex") != curImIdx);

        return true;
    }

    static bool PresentInferenceResult(const std::vector<arm::app::object_detection::DetectionResult>& results,
	                                   std::vector<std::string>& labels)
    {
        hal_lcd_set_text_color(COLOR_GREEN);

        /* If profiling is enabled, and the time is valid. */
        info("Final results:\n");
        info("Total number of inferences: 1\n");

        for (uint32_t i = 0; i < results.size(); ++i) {
            info("%" PRIu32 ") %s(%f) -> %s {x=%d,y=%d,w=%d,h=%d}\n", i,
			    labels[results[i].m_cls].c_str(),
                results[i].m_normalisedVal, "Detection box:",
                results[i].m_x0, results[i].m_y0, results[i].m_w, results[i].m_h );
        }

        return true;
    }

	
	static void DrawImageDetectionBoxes(
		const std::vector<arm::app::object_detection::DetectionResult>& results,
		image_t *drawImg,
		std::vector<std::string>& labels)
	{
        for (const auto& result: results) {
			imlib_draw_rectangle(drawImg, result.m_x0, result.m_y0, result.m_w, result.m_h, COLOR_B5_MAX, 1, false);
			imlib_draw_string(drawImg, result.m_x0, result.m_y0 - 16, labels[result.m_cls].c_str(), COLOR_B5_MAX, 2, 0, 0, false,
                       false, false, false, 0, false, false);
		}
	}
	
    static void DrawDetectionBoxes(const std::vector<arm::app::object_detection::DetectionResult>& results,
                                   uint32_t imgStartX,
                                   uint32_t imgStartY,
                                   uint32_t imgDownscaleFactor)
    {
        uint32_t lineThickness = 1;

        for (const auto& result: results) {
            /* Top line. */
            hal_lcd_display_box(imgStartX + result.m_x0/imgDownscaleFactor,
                    imgStartY + result.m_y0/imgDownscaleFactor,
                    result.m_w/imgDownscaleFactor, lineThickness, COLOR_GREEN);
            /* Bot line. */
            hal_lcd_display_box(imgStartX + result.m_x0/imgDownscaleFactor,
                    imgStartY + (result.m_y0 + result.m_h)/imgDownscaleFactor - lineThickness,
                    result.m_w/imgDownscaleFactor, lineThickness, COLOR_GREEN);

            /* Left line. */
            hal_lcd_display_box(imgStartX + result.m_x0/imgDownscaleFactor,
                    imgStartY + result.m_y0/imgDownscaleFactor,
                    lineThickness, result.m_h/imgDownscaleFactor, COLOR_GREEN);
            /* Right line. */
            hal_lcd_display_box(imgStartX + (result.m_x0 + result.m_w)/imgDownscaleFactor - lineThickness,
                    imgStartY + result.m_y0/imgDownscaleFactor,
                    lineThickness, result.m_h/imgDownscaleFactor, COLOR_GREEN);
        }
    }

} /* namespace app */
} /* namespace arm */
