/**************************************************************************//**
 * @file     DetectorResult.hpp
 * @version  V0.10
 * @brief    Detecotr result header
 * * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#ifndef DETECTOR_RESULT_HPP
#define DETECTOR_RESULT_HPP

#include <string>
#include <vector>


namespace arm {
namespace app {

    /**
     * @brief   Class representing a multi detect box result.
     */
    class DetectorResult {
    public:
        typedef struct
        {
            float m_x;
            float m_y;
            float m_w;
            float m_h;
        }S_BOX_POS;

        typedef struct
        {
            float m_score;
            std::string m_class;
            S_BOX_POS m_box;
        }S_DETECTOR_BOX;

        std::vector<S_DETECTOR_BOX> m_boxes;
        float m_num_box;

        DetectorResult() = default;
        ~DetectorResult() = default;

   };

} /* namespace app */
} /* namespace arm */

#endif /* DETECTOR_RESULT_HPP */