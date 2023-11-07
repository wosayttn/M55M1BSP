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
#include "MobileNetModel.hpp"       /* Model API */
#include "Labels.hpp"
#include "ImgClassProcessing.hpp"

#include "imlib.h"          /* Image processing */
#include "framebuffer.h"

#define __USE_CCAP__

#if defined (__USE_CCAP__)
    #include "ImageSensor.h"
#endif

#undef PI /* PI macro conflict with CMSIS/DSP */
#include "NuMicro.h"

#define __PROFILE__

#if defined(__PROFILE__)
    #include "Profiler.hpp"
#endif

using ImageClassifier = arm::app::Classifier;

namespace arm
{
namespace app
{
/* Tensor arena buffer */
static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;

/* Optional getter function for the model pointer and its size. */
namespace mobilenet
{
extern uint8_t *GetModelPointer();
extern size_t GetModelLen();
} /* namespace mobilenet */
} /* namespace app */
} /* namespace arm */

/* Cache policy function */
enum { NonCache_index, WTRA_index, WBWARA_index };

static void initializeAttributes()
{
    /* Initialize attributes corresponding to the enums defined in mpu.hpp */
    const uint8_t WTRA =
        ARM_MPU_ATTR_MEMORY_(1, 0, 1, 0); // Non-transient, Write-Through, Read-allocate, Not Write-allocate
    const uint8_t WBWARA = ARM_MPU_ATTR_MEMORY_(1, 1, 1, 1); // Non-transient, Write-Back, Read-allocate, Write-allocate

    ARM_MPU_SetMemAttr(NonCache_index, ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));
    ARM_MPU_SetMemAttr(WTRA_index, ARM_MPU_ATTR(WTRA, WTRA));
    ARM_MPU_SetMemAttr(WBWARA_index, ARM_MPU_ATTR(WBWARA, WBWARA));
}

static void loadAndEnableConfig(ARM_MPU_Region_t const *table, uint32_t cnt)
{
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
    arm::app::MobileNetModel model;

    if (!model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::mobilenet::GetModelPointer(),
                    arm::app::mobilenet::GetModelLen()))
    {
        printf_err("Failed to initialise model\n");
        return 1;
    }

    /* Setup cache poicy of tensor arean buffer */
    info("Set tesnor arena cache policy to WTRA \n");
    const std::vector<ARM_MPU_Region_t> mpuConfig =
    {
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

    ImageClassifier classifier;  /* Classifier object. */

    uint8_t u8ImgIdx = 0;

    TfLiteTensor *inputTensor   = model.GetInputTensor(0);
    TfLiteTensor *outputTensor = model.GetOutputTensor(0);

    if (!inputTensor->dims)
    {
        printf_err("Invalid input tensor dims\n");
        return 2;
    }
    else if (inputTensor->dims->size < 3)
    {
        printf_err("Input tensor dimension should be >= 3\n");
        return 3;
    }

    TfLiteIntArray *inputShape = model.GetInputShape(0);

    const int inputImgCols = inputShape->data[arm::app::MobileNetModel::ms_inputColsIdx];
    const int inputImgRows = inputShape->data[arm::app::MobileNetModel::ms_inputRowsIdx];
    const uint32_t inputChannels = inputShape->data[arm::app::MobileNetModel::ms_inputChannelsIdx];

    //label information
    std::vector <std::string> labels;
    GetLabelsVector(labels);

    /* Set up pre and post-processing. */
    arm::app::ImgClassPreProcess preProcess = arm::app::ImgClassPreProcess(&model);

    std::vector<arm::app::ClassificationResult> results;
    std::string predictLabelInfo;
    arm::app::ImgClassPostProcess postProcess = arm::app::ImgClassPostProcess(classifier, &model,
                                                                              labels, results);

    //display framebuffer
    image_t frameBuffer;
    rectangle_t roi;

    //omv library init
    omv_init();
    framebuffer_init_image(&frameBuffer);

#if defined(__PROFILE__)
    arm::app::Profiler profiler;
	uint64_t u64StartCycle;
	uint64_t u64EndCycle;	
	uint64_t u64CCAPStartCycle;
	uint64_t u64CCAPEndCycle;	
#endif

#if defined (__USE_CCAP__)
    //Setup image senosr
    ImageSensor_Init();
    ImageSensor_Config(eIMAGE_FMT_RGB565, frameBuffer.w, frameBuffer.h);
#endif

#if !defined (__USE_CCAP__)
    info("Press 'n' to run next image inference \n");
    info("Press 'q' to exit program \n");

    while ((chStdIn = getchar()))
    {
        if (chStdIn == 'q')
            break;
        else if (chStdIn != 'n')
            continue;

#else

    while (1)
    {
#endif

#if !defined (__USE_CCAP__)

        const uint8_t *pu8ImgSrc = get_img_array(u8ImgIdx);

        if (nullptr == pu8ImgSrc)
        {
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
#endif

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
        resizeImg.pixfmt = PIXFORMAT_RGB888;

#if defined(__PROFILE__)
		u64StartCycle = pmu_get_systick_Count();
#endif
        imlib_nvt_scale(&frameBuffer, &resizeImg, &roi);
#if defined(__PROFILE__)
		u64EndCycle = pmu_get_systick_Count();
		info("resize cycles %llu \n", (u64EndCycle - u64StartCycle));
#endif

#if defined (__USE_CCAP__)
        //Capture new image
#if defined(__PROFILE__)
		u64CCAPStartCycle = pmu_get_systick_Count();
#endif
        ImageSensor_TriggerCapture((uint32_t)frameBuffer.data);
#endif		

#if defined(__PROFILE__)
		u64StartCycle = pmu_get_systick_Count();
#endif

        /* Run the pre-processing, inference and post-processing. */
        if (!preProcess.DoPreProcess(resizeImg.data, (resizeImg.w * resizeImg.h * inputChannels)))
        {
            goto round_done;
        }

#if defined(__PROFILE__)
		u64EndCycle = pmu_get_systick_Count();
		info("quantize cycles %llu \n", (u64EndCycle - u64StartCycle));
#endif
		
#if defined(__PROFILE__)
        profiler.StartProfiling("Inference");
#endif

        if (!model.RunInference())
        {
            printf_err("Inference failed.");
            return 5;
        }

#if defined(__PROFILE__)
        profiler.StopProfiling();
#endif

#if defined (__USE_CCAP__)
        //Capture new image

        ImageSensor_WaitCaptureDone();
#if defined(__PROFILE__)
		u64CCAPEndCycle = pmu_get_systick_Count();
		info("ccap capture cycles %llu \n", (u64CCAPEndCycle - u64CCAPStartCycle));
#endif
#endif		
		
        //results.clear(); not necessary ???
        predictLabelInfo.clear();

        if (postProcess.DoPostProcess())
        {
            predictLabelInfo =  results[0].m_label + std::string(":") + std::to_string(results[0].m_normalisedVal);
        }
        else
        {
            predictLabelInfo = std::string("???") + std::string(":") + std::to_string(results[0].m_normalisedVal);
        }

        //show result
        info("Final results:\n");
        info("%s\n", predictLabelInfo.c_str());

#if defined(__PROFILE__)
        profiler.PrintProfilingResult();
#endif


round_done:
        u8ImgIdx ++;

        if (u8ImgIdx >= NUMBER_OF_FILES)
            u8ImgIdx = 0;

    }
}