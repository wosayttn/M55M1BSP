/*
 * Copyright (c) 2022, Arm Limited and affiliates.
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

/*********************    Autogenerated file. DO NOT EDIT *******************
 * Generated from gen_rgb_cpp.py tool  file.
 * Date: 2022-04-14 09:45:40.508838
 ***************************************************************************/

#ifndef GENERATED_IMAGES_H
#define GENERATED_IMAGES_H

#include <cstdint>

#define NUMBER_OF_FILES  (2U)
#define IMAGE_DATA_SIZE  (27648U)
#define IMAGE_WIDTH (96)
#define IMAGE_HEIGHT (96)

extern const uint8_t im0[IMAGE_DATA_SIZE];
extern const uint8_t im1[IMAGE_DATA_SIZE];

const char* get_filename(const uint32_t idx);
const uint8_t* get_img_array(const uint32_t idx);

#endif /* GENERATED_IMAGES_H */