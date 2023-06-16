/**************************************************************************//**
 * @file     Detector.cc
 * @version  V0.10
 * @brief    Detector function
 * * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "Detector.hpp"

#include "TensorFlowLiteMicro.hpp"
#include "log_macros.h"

#include <vector>
#include <string>
#include <set>
#include <cstdint>
#include <cinttypes>
#include <cmath>

struct TF_BoxCornerEncoding {
  float ymin;
  float xmin;
  float ymax;
  float xmax;
};


namespace arm {
namespace app {
	Detector::Detector()
	{
	}
    bool Detector::GetDetectorResults(
            TfLiteTensor* outputScoreTensor,
            TfLiteTensor* outputClassTensor,
            TfLiteTensor* outputBoxesTensor,
            TfLiteTensor* outputNumBoxTensor,
            DetectorResult &detectorResults,
            const std::vector <std::string>& labels)
	{

		int i;

		if ((outputScoreTensor == nullptr) ||
			(outputClassTensor == nullptr) ||
			(outputBoxesTensor == nullptr) ||
			(outputNumBoxTensor == nullptr))
		{
            printf_err("Output vector is null pointer.\n");
            return false;
        }

		float *tensor_buffer = tflite::GetTensorData<float>(outputNumBoxTensor);
		float *tensor_score_buffer = tflite::GetTensorData<float>(outputScoreTensor);
		float *tensor_class_buffer = tflite::GetTensorData<float>(outputClassTensor);
		struct TF_BoxCornerEncoding *tensor_box_buffer = (struct TF_BoxCornerEncoding *)tflite::GetTensorData<float>(outputBoxesTensor);

		detectorResults.m_boxes.clear();
		detectorResults.m_num_box = *tensor_buffer;
		
		for( i = 0; i < detectorResults.m_num_box; i ++)
		{
			arm::app::DetectorResult::S_DETECTOR_BOX sNewBox;

			sNewBox.m_score = *tensor_score_buffer;
			sNewBox.m_class = labels[(int)*tensor_class_buffer];

			sNewBox.m_box.m_x = tensor_box_buffer->xmin;
			sNewBox.m_box.m_y = tensor_box_buffer->ymin;
			sNewBox.m_box.m_w = tensor_box_buffer->xmax - tensor_box_buffer->xmin;
			sNewBox.m_box.m_h = tensor_box_buffer->ymax - tensor_box_buffer->ymin;

			detectorResults.m_boxes.push_back(sNewBox);
			tensor_score_buffer ++;
			tensor_class_buffer ++;
			tensor_box_buffer ++;
		}
		
		return true;
	}
	
} /* namespace app */
} /* namespace arm */
