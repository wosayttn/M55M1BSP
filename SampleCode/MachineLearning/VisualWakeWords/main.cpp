/**************************************************************************//**
 * @file     main.cpp
 * @version  V1.00
 * @brief    main function
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include "BoardInit.hpp"      /* Board initialisation */
#include "log_macros.h"      /* Logging macros (optional) */

#include "BufAttributes.hpp" /* Buffer attributes to be applied */
#include "Classifier.hpp"    /* Classifier for the result */
#include "ClassificationResult.hpp"
#include "InputFiles.hpp"             /* Baked-in input (not needed for live data) */
#include "VisualWakeWordModel.hpp"       /* Model API */
#include "Labels.hpp"

#include "imlib.h"			/* Image processing */
#include "framebuffer.h"

#undef PI /* PI macro conflict with CMSIS/DSP */
#include "NuMicro.h"

#define __PROFILE__

#if defined(__PROFILE__)
#include "Profiler.hpp"
#endif

using ViusalWakeWordClassifier = arm::app::Classifier;

namespace arm {
namespace app {
    /* Tensor arena buffer */
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;

    /* Optional getter function for the model pointer and its size. */
    namespace vww {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace vww */
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
    char chStdIn;

    /* Initialise the UART module to allow printf related functions (if using retarget) */
    BoardInit();

    /* Model object creation and initialisation. */
    arm::app::VisualWakeWordModel model;
    if (!model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::vww::GetModelPointer(),
                    arm::app::vww::GetModelLen())) {
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


    ViusalWakeWordClassifier classifier;  /* Classifier object. */
	
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

    const int inputImgCols = inputShape->data[arm::app::VisualWakeWordModel::ms_inputColsIdx];
    const int inputImgRows = inputShape->data[arm::app::VisualWakeWordModel::ms_inputRowsIdx];
    const uint32_t nChannels = inputShape->data[arm::app::VisualWakeWordModel::ms_inputChannelsIdx];

	/* VWW model preprocessing is image conversion from uint8 to [0,1] float values,
	 * then quantize them with input quantization info. */
	arm::app::QuantParams inQuantParams = arm::app::GetTensorQuantParams(inputTensor);

	//label information
    std::vector <std::string> labels;
    GetLabelsVector(labels);

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
		//resize framebuffer image to model input
		image_t resizeImg;

		roi.x = 0;
		roi.y = 0;
		roi.w = frameBuffer.w;
		roi.h = frameBuffer.h;

		resizeImg.w = inputImgCols;
		resizeImg.h = inputImgRows;
		resizeImg.data = (uint8_t *)inputTensor->data.data; //direct resize to input tensor buffer
	
		if (1 == nChannels)
			resizeImg.pixfmt =  PIXFORMAT_GRAYSCALE;
		else
			resizeImg.pixfmt = PIXFORMAT_RGB888;

		imlib_nvt_scale(&frameBuffer, &resizeImg, &roi);
		
		//Quantize input tensor data
		auto* req_data = static_cast<uint8_t *>(inputTensor->data.data);
		auto* signed_req_data = static_cast<int8_t *>(inputTensor->data.data);
		for (size_t i = 0; i < inputTensor->bytes; i++) {
			auto i_data_int8 = static_cast<int8_t>(((static_cast<float>(req_data[i]) / 255.0f) / inQuantParams.scale) + inQuantParams.offset);
			signed_req_data[i] = std::min<int8_t>(INT8_MAX, std::max<int8_t>(i_data_int8, INT8_MIN));
		}
		
        /* Run inference over this image. */
        info("Running inference on image %" PRIu32 " => %s\n", u8ImgIdx, get_filename(u8ImgIdx));

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
		
		std::vector<arm::app::ClassificationResult> results;

		classifier.GetClassificationResults(outputTensor, results,
											labels, 1,
											false);
		
		//show result
		info("Final results:\n");
		info("Total number of inferences: 1\n");
		for(int i =0; i < results.size(); i ++)
		{
			info("%" PRIu32 ") %" PRIu32 " (%f) -> %s\n", i,
				results[i].m_labelIdx, results[i].m_normalisedVal,
				results[i].m_label.c_str());
		}
		
#if defined(__PROFILE__)
        profiler.PrintProfilingResult();
#endif

		u8ImgIdx ++;
		if(u8ImgIdx >= NUMBER_OF_FILES)
			u8ImgIdx = 0;
	}

	return 0;
}