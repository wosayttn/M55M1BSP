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

/****************************************************************************\
 *               Main application file for ARM NPU on M55M1 board             *
\****************************************************************************/

#include "hal.h"                    /* our hardware abstraction api */
#include "log_macros.h"
#include "TensorFlowLiteMicro.hpp"  /* our inference logic api */

#include <cstdio>

#define PRJ_DES_STR "ARM ML Embedded Evaluation Kit for MPS3 FPGA and FastModel"
#define PRJ_VER_STR "22.02"

extern void main_loop(hal_platform& platform);

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm(" .global __ARM_use_no_argv\n");
#endif

/* Print application information. */
static void print_application_intro()
{
    info("%s\n", PRJ_DES_STR);
    info("Version %s Build date: " __DATE__ " @ " __TIME__ "\n", PRJ_VER_STR);
    info("Copyright (C) ARM Ltd 2021-2022. All rights reserved.\n\n");
}

int main ()
{
    hal_platform    platform;
    platform_timer  timer;

    /* Initialise the HAL and platform. */
    hal_init(&platform, &timer);

    if (0 == hal_platform_init(&platform)) {
        /* Application information, UART should have been initialised. */
        print_application_intro();

        /* Check the version of TensorFlow Lite Micro. */
        PrintTensorFlowVersion();
        /* Run the application. */
        main_loop(platform);
    }

    /* This is unreachable without errors. */
    info("program terminating...\n");

    /* Release platform. */
    hal_platform_release(&platform);
    return 0;
}