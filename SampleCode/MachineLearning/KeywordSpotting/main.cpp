/**************************************************************************//**
 * @file     main.cpp
 * @version  V1.00
 * @brief    Keyword spotting sample code
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include "BoardInit.hpp"      /* Board initialisation */
#include "log_macros.h"      /* Logging macros (optional) */

#include "AudioUtils.hpp"
#include "BufAttributes.hpp" /* Buffer attributes to be applied */
#include "Classifier.hpp"    /* Classifier for the result */
#include "ClassificationResult.hpp"
#include "InputFiles.hpp"             /* Baked-in input (not needed for live data) */
#include "KwsModel.hpp"       /* Model API */
#include "Labels.hpp"
#include "KwsMfcc.hpp"
#include "KwsProcessing.hpp" /* Pre and Post Process */
#include "KwsResult.hpp"

#undef PI /* PI macro conflict with CMSIS/DSP */
#include "NuMicro.h"


namespace arm {
namespace app {
    /* Tensor arena buffer */
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;

    /* Optional getter function for the model pointer and its size. */
    namespace kws {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace kws */
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

int main()
{
    /* Initialise the UART module to allow printf related functions (if using retarget) */
    BoardInit();

    /* Model object creation and initialisation. */
    arm::app::KwsModel model;
    if (!model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::kws::GetModelPointer(),
                    arm::app::kws::GetModelLen())) {
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
	
    constexpr int minTensorDims = static_cast<int>(
        (arm::app::KwsModel::ms_inputRowsIdx > arm::app::KwsModel::ms_inputColsIdx)
            ? arm::app::KwsModel::ms_inputRowsIdx
            : arm::app::KwsModel::ms_inputColsIdx);

    const auto mfccFrameLength = 640;
    const auto mfccFrameStride = 320;
    const auto scoreThreshold  = 0.7;

    /* Get Input and Output tensors for pre/post processing. */
    TfLiteTensor* inputTensor  = model.GetInputTensor(0);
    TfLiteTensor* outputTensor = model.GetOutputTensor(0);
    if (!inputTensor->dims) {
        printf_err("Invalid input tensor dims\n");
        return 2;
    } else if (inputTensor->dims->size < minTensorDims) {
        printf_err("Input tensor dimension should be >= %d\n", minTensorDims);
        return 3;
    }

    /* Get input shape for feature extraction. */
    TfLiteIntArray* inputShape     = model.GetInputShape(0);
 #if defined (MODEL_DS_CNN)
        const uint32_t numMfccFeatures = 10;
        const uint32_t numMfccFrames = 49;
#else
        const uint32_t numMfccFeatures = inputShape->data[KwsModel::ms_inputColsIdx];
        const uint32_t numMfccFrames = inputShape->data[arm::app::KwsModel::ms_inputRowsIdx];
#endif

    /* We expect to be sampling 1 second worth of data at a time.
     * NOTE: This is only used for time stamp calculation. */
    const float secondsPerSample = 1.0 / arm::app::audio::KwsMFCC::ms_defaultSamplingFreq;

    /* Classifier object for results */
    arm::app::Classifier classifier;

    /* Object to hold label strings. */
    std::vector<std::string> labels;

    /* Populate the labels here. */
    GetLabelsVector(labels);

	uint8_t u8ClipIdx = 0;

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


		/* Declare a container to hold results from across the whole audio clip. */
		std::vector<arm::app::kws::KwsResult> finalResults;

		/* Object to hold classification results */
		std::vector<arm::app::ClassificationResult> singleInfResult;

        /* Set up pre and post-processing. */
        arm::app::KwsPreProcess preProcess = arm::app::KwsPreProcess(
            inputTensor, numMfccFeatures, numMfccFrames, mfccFrameLength, mfccFrameStride);

#if defined(MODEL_DS_CNN)
        arm::app::KwsPostProcess postProcess =
            arm::app::KwsPostProcess(outputTensor, classifier, labels, singleInfResult, false);
#else
        arm::app::KwsPostProcess postProcess =
            arm::app::KwsPostProcess(outputTensor, classifier, labels, singleInfResult, true);
#endif
				
		/* Creating a sliding window through the whole audio clip. */
		auto audioDataSlider = arm::app::audio::SlidingWindow<const int16_t>(
				get_audio_array(u8ClipIdx),
				get_audio_array_size(u8ClipIdx),
				preProcess.m_audioDataWindowSize, preProcess.m_audioDataStride);

		info("Using audio data from %s\n", get_filename(u8ClipIdx));
		
		/* Start sliding through audio clip. */
		while (audioDataSlider.HasNext()) 
		{
			const int16_t* inferenceWindow = audioDataSlider.Next();

			/* The first window does not have cache ready. */
			preProcess.m_audioWindowIndex = audioDataSlider.Index();

			info("Inference %zu/%zu\n", audioDataSlider.Index() + 1,
				 audioDataSlider.TotalStrides() + 1);

			/* Run the pre-processing, inference and post-processing. */
			if (!preProcess.DoPreProcess(inferenceWindow, arm::app::audio::KwsMFCC::ms_defaultSamplingFreq)) {
				printf_err("Pre-processing failed.");
				break;
			}

			if (!model.RunInference()) {
				printf_err("Inference failed.");
				break;
			}

			if (!postProcess.DoPostProcess()) {
				printf_err("Post-processing failed.");
				break;
			}

			/* Add results from this window to our final results vector. */
			finalResults.emplace_back(arm::app::kws::KwsResult(singleInfResult,
					audioDataSlider.Index() * secondsPerSample * preProcess.m_audioDataStride,
					audioDataSlider.Index(), scoreThreshold));

		} /* while (audioDataSlider.HasNext()) */
		
		for (const auto& result : finalResults) {

			std::string topKeyword{"<none>"};
			float score = 0.f;
			if (!result.m_resultVec.empty()) {
				topKeyword = result.m_resultVec[0].m_label;
				score      = result.m_resultVec[0].m_normalisedVal;
			}

			if (result.m_resultVec.empty()) {
				info("For timestamp: %f (inference #: %" PRIu32 "); label: %s; threshold: %f\n",
					 result.m_timeStamp,
					 result.m_inferenceNumber,
					 topKeyword.c_str(),
					 result.m_threshold);
			} else {
				for (uint32_t j = 0; j < result.m_resultVec.size(); ++j) {
					info("For timestamp: %f (inference #: %" PRIu32
						 "); label: %s, score: %f; threshold: %f\n",
						 result.m_timeStamp,
						 result.m_inferenceNumber,
						 result.m_resultVec[j].m_label.c_str(),
						 result.m_resultVec[j].m_normalisedVal,
						 result.m_threshold);
				}
			}
		}
		
		u8ClipIdx ++;
		if(u8ClipIdx >= NUMBER_OF_FILES)
			u8ClipIdx = 0;

	}
	return 0;
}