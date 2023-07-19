/**************************************************************************//**
 * @file     main.cpp
 * @version  V1.00
 * @brief    Face recognition sample code
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include <vector>
#include <string>
#include <cinttypes>

#include "BoardInit.hpp"      /* Board initialisation */
#include "log_macros.h"      /* Logging macros (optional) */

#include "BufAttributes.hpp" /* Buffer attributes to be applied */
#include "InputFiles.hpp"             /* Baked-in input (not needed for live data) */
#include "MobileNetModel.hpp"       /* Model API */
#include "Labels.hpp"
#include "Recognizer.hpp"
#include "FaceRecognProcessing.hpp"

#include "imlib.h"			/* Image processing */
#include "framebuffer.h"

#undef PI /* PI macro conflict with CMSIS/DSP */
#include "NuMicro.h"

#define __PROFILE__

#if defined(__PROFILE__)
#include "Profiler.hpp"
#endif

using FaceRecognizer = arm::app::Recognizer;

namespace arm {
namespace app {
    /* Tensor arena buffer */
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;

    /* Optional getter function for the model pointer and its size. */
    namespace mobilenet {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace mobilenet */
} /* namespace app */
} /* namespace arm */

/* Cache policy function */
enum { NonCache_index, WTRA_index, WBWARA_index };

static void initializeAttributes() {
    /* Initialize attributes corresponding to the enums defined in mpu.hpp */
    const uint8_t WTRA =
        ARM_MPU_ATTR_MEMORY_(1, 0, 1, 0); // Non-transient, Write-Through, Read-allocate, Not Write-allocate
    const uint8_t WBWARA = ARM_MPU_ATTR_MEMORY_(1, 1, 1, 1); // Non-transient, Write-Back, Read-allocate, Write-allocate

    ARM_MPU_SetMemAttr(NonCache_index, ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));
    ARM_MPU_SetMemAttr(WTRA_index, ARM_MPU_ATTR(WTRA, WTRA));
    ARM_MPU_SetMemAttr(WBWARA_index, ARM_MPU_ATTR(WBWARA, WBWARA));
}

static void loadAndEnableConfig(ARM_MPU_Region_t const *table, uint32_t cnt) {
    initializeAttributes();

    ARM_MPU_Load(0, table, cnt);

    // Enable MPU with default priv access to all other regions
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
}

/* Image processing initiate function */
//Used by omv library
#define GLCD_WIDTH 320
#define GLCD_HEIGHT 240

#undef OMV_FB_SIZE
#define OMV_FB_SIZE ((GLCD_WIDTH * GLCD_HEIGHT * 2) + 1024)

__attribute__((section(".bss.sram.data"), aligned(16))) static char fb_array[OMV_FB_SIZE + OMV_FB_ALLOC_SIZE];
__attribute__((section(".bss.sram.data"), aligned(16))) static char jpeg_array[OMV_JPEG_BUF_SIZE];

char *_fb_base = NULL;
char *_fb_end = NULL;
char *_jpeg_buf = NULL;
char *_fballoc = NULL;

static void omv_init()
{
	image_t frameBuffer;

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
}

int main()
{
    /* Initialize the UART module to allow printf related functions (if using retarget) */
    BoardInit();

    /* Model object creation and initialisation. */
    arm::app::MobileNetModel model;
    if (!model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::mobilenet::GetModelPointer(),
                    arm::app::mobilenet::GetModelLen())) {
        printf_err("Failed to initialise model\n");
        return 1;
    }

	/* Setup cache poicy of tensor arean buffer */ 
	info("Set tesnor arena cache policy to WTRA \n");
    const std::vector<ARM_MPU_Region_t> mpuConfig = {
        {
            // SRAM for tensor arena
            ARM_MPU_RBAR(((unsigned int)arm::app::tensorArena),        // Base
                         ARM_MPU_SH_NON,    // Non-shareable
                         0,                 // Read-only
                         1,                 // Non-Privileged
                         1),                // eXecute Never enabled
            ARM_MPU_RLAR((((unsigned int)arm::app::tensorArena) + ACTIVATION_BUF_SZ - 1),        // Limit
                         WTRA_index) // Attribute index - Write-Through, Read-allocate
	     }
	};

    // Setup MPU configuration
    loadAndEnableConfig(&mpuConfig[0], mpuConfig.size());

    FaceRecognizer recognizer;  /* Classifier object. */
    uint8_t u8ImgIdx = 0;

    TfLiteTensor* inputTensor   = model.GetInputTensor(0);
    TfLiteTensor* outputTensor = model.GetOutputTensor(0);

    if (!inputTensor->dims) {
        printf_err("Invalid input tensor dims\n");
        return 2;
	}
	else if (inputTensor->dims->size < 3) {
		printf_err("Input tensor dimension should be >= 3\n");
		return 3;
	}		

    TfLiteIntArray* inputShape = model.GetInputShape(0);

    const int inputImgCols = inputShape->data[arm::app::MobileNetModel::ms_inputColsIdx];
    const int inputImgRows = inputShape->data[arm::app::MobileNetModel::ms_inputRowsIdx];
    const uint32_t inputChannels = inputShape->data[arm::app::MobileNetModel::ms_inputChannelsIdx];

	//label information
    std::vector<std::string> labels;
    std::vector<S_LABEL_INFO> labelInfo;

    GetLabelsVector(labels);
	ParserLabelVector(labels, labelInfo, nullptr);

	/* Set up pre and post-processing. */
	arm::app::FaceRecognPreProcess preProcess = arm::app::FaceRecognPreProcess(&model);

	arm::app::RecognitionResult result;
	std::string predictLabelInfo;

	arm::app::FaceRecognPostProcess postProcess = arm::app::FaceRecognPostProcess(recognizer, &model,
			labelInfo, result);

	//display framebuffer
	image_t frameBuffer;
	rectangle_t roi;

	//omv library init
	omv_init();
	framebuffer_init_image(&frameBuffer);

#if defined(__PROFILE__)
	arm::app::Profiler profiler;
#endif

#if 0
	while((chStdIn = getchar()))
	{
		if(chStdIn == 'Q')
			break;
		else if(chStdIn != 'R')
			continue;
#else
	while(1)
	{
#endif
		const uint8_t* pu8ImgSrc = get_img_array(u8ImgIdx);
		
		if (nullptr == pu8ImgSrc) {
			printf_err("Failed to get image index %" PRIu32 " (max: %u)\n", u8ImgIdx,
					   NUMBER_OF_FILES - 1);
			return 4;
		}		

		//resize source image to framebuffer
		image_t srcImg;

		srcImg.w = IMAGE_WIDTH;
		srcImg.h = IMAGE_HEIGHT;
		srcImg.data = (uint8_t *)pu8ImgSrc;
		srcImg.pixfmt = PIXFORMAT_RGB888;

		roi.x = 0;
		roi.y = 0;
		roi.w = IMAGE_WIDTH;
		roi.h = IMAGE_HEIGHT;
		imlib_nvt_scale(&srcImg, &frameBuffer, &roi);

		//TODO: display image on LCD

		//Face detect
		arm::app::FaceRecognPreProcess::S_FACE_RECOGN_IMAGE sSrcImage;
		arm::app::FaceRecognPreProcess::S_FACE_RECOGN_IMAGE_ROI sROI;

		sSrcImage.eColorFormat = arm::app::FaceRecognPreProcess::eFACE_RECOGN_COLOR_RGB565;
		sSrcImage.u32Width = frameBuffer.w;
		sSrcImage.u32Height = frameBuffer.h;
		sSrcImage.pu8Data = (uint8_t *)frameBuffer.data;
		
		if(preProcess.RunFaceDetect(&sSrcImage, &sROI) == false)
		{
			info("Face not found on image %" PRIu32 " => %s\n", u8ImgIdx, get_filename(u8ImgIdx));
			goto round_done;
		}
		
		info("Image %" PRIu32 " => %s\n", u8ImgIdx, get_filename(u8ImgIdx));
		info("Face detect on image (x,y,w,h) => (%d, %d, %d, %d)\n", sROI.u32X, sROI.u32Y, sROI.u32Width, sROI.u32Height);
		
		// Resize detected face to model input
		arm::app::FaceRecognPreProcess::S_FACE_RECOGN_IMAGE sResizeImage;

		sResizeImage.eColorFormat = arm::app::FaceRecognPreProcess::eFACE_RECOGN_COLOR_RGB888;
		sResizeImage.u32Width = inputImgCols;
		sResizeImage.u32Height = inputImgRows;
		sResizeImage.pu8Data = (uint8_t *)inputTensor->data.data;
		
		preProcess.FaceResize(&sSrcImage, &sResizeImage, &sROI);  

		/* Run the pre-processing, inference and post-processing. */
		if (!preProcess.DoPreProcess(sResizeImage.pu8Data, (sResizeImage.u32Width * sResizeImage.u32Height * inputChannels))) {
			goto round_done;
		}

#if defined(__PROFILE__)
		profiler.StartProfiling("Inference");
#endif
        if (!model.RunInference()) {
            printf_err("Inference failed.");
            return 5;
        }
#if defined(__PROFILE__)
		profiler.StopProfiling();
#endif

		predictLabelInfo.clear();

		if (postProcess.DoPostProcess()) {
			predictLabelInfo =  result.m_label + std::string(":") + std::to_string(result.m_predict);         
		}
		else {
			predictLabelInfo = std::string("???") + std::string(":") + std::to_string(result.m_predict);
		}

		//show result
		info("Final results:\n");
		info("%s\n", predictLabelInfo.c_str());

#if defined(__PROFILE__)
        profiler.PrintProfilingResult();
#endif

round_done:		
		u8ImgIdx ++;
		if(u8ImgIdx >= NUMBER_OF_FILES)
			u8ImgIdx = 0;
	}
	return 0;
}