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
#ifndef PLATFORM_HAL_H
#define PLATFORM_HAL_H

/**
 * This file should present a C API for the main application logic to use
 * and be indifferent to the lower level platform. In addition to this it
 * will also need to be aware of the API exposed by data acquisition and
 * data presentation modules.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "platform_drivers.h"   /* Platform drivers */
#include "hal_timer.h"              /* Timer/profiler API */
#include "hal_lcd.h"            /* LCD functions */

#include <inttypes.h>
#include <stdbool.h>

/* Structure to define a platform context to be used by the application */
typedef struct hal_platform_context {
    int inited;                         /**< initialised */
    char plat_name[64];                 /**< name of this platform */
    platform_timer* timer;              /**< timer */
    int (* platform_init)();            /**< pointer to platform initialisation function */
    void (* platform_release)();        /**< pointer to platform release function */
} hal_platform;

/**
 * @brief           Initialise the HAL structure based on compile time config. This
 *                  should be called before any other function in this API.
 * @param[in,out]   platform    Pointer to a pre-allocated platform struct.
 * @param[in,out]   timer       Pointer to a pre-allocated timer module.
 * @return          0 if successful, error code otherwise.
 **/
int hal_init(hal_platform* platform, platform_timer* timer);


/**
 * @brief       Initialise the HAL platform. This will go and initialise all the
 *              modules on the platform the application requires to run.
 * @param[in]   platform    Pointer to a pre-allocated and initialised
 *                          platform structure.
 * @return      0 if successful, error code otherwise.
 **/
int hal_platform_init(hal_platform* platform);


/**
 * @brief       Release the HAL platform. This should release resources acquired.
 * @param[in]   platform    pointer to a pre-allocated and initialised
 *                          platform structure.
 **/
void hal_platform_release(hal_platform* platform);

/**
 * @brief       Gets user input from the stdin interface.
 * @param[out]  user_input  Pointer to a buffer where the input will be stored.
 * @param[in]   size        Buffer size in bytes.
 * @return      True if successful, false otherwise.
 */
bool hal_get_user_input(char* user_input, int size);

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_HAL_H */