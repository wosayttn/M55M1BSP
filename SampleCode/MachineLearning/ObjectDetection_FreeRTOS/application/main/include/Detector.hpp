/**************************************************************************//**
 * @file     Detector.hpp
 * @version  V0.10
 * @brief    Detector header
 * * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include "DetectorResult.hpp"
#include "TensorFlowLiteMicro.hpp"
#include "Labels.hpp"

#include <vector>
namespace arm {
namespace app {

    /**
     * @brief   Detector - a helper class to get certain number of top
     *          results from the output vector from a object detector NN.
     **/

    class Detector{
    public:
        /** @brief Constructor. */
        Detector();

        /**
         * @brief       Gets the detector results from the
         *              output vector.
         * @param[in]   outputScoreTensor   Inference output score tensor from an NN model.
         * @param[in]   outputClassTensor   Inference output class tensor from an NN model.
         * @param[in]   outputBoxesTensor   Inference output boxes tensor from an NN model.
         * @param[in]   outputNumBoxTensor  Inference output number box tensor from an NN model.
         * @param[out]   detectorResults    Detector result.
         * @return      true if successful, false otherwise.
         **/

        virtual bool GetDetectorResults(
            TfLiteTensor* outputScoreTensor,
            TfLiteTensor* outputClassTensor,
            TfLiteTensor* outputBoxesTensor,
            TfLiteTensor* outputNumBoxTensor,
            DetectorResult &detectorResults,
            const std::vector <std::string>& labels);
    };

} /* namespace app */
} /* namespace arm */

#endif /* DETECTOR_HPP */
