/**************************************************************************//**
 * @file     LoadUserImage.c
 * @version  V1.00
 * @brief    Load user image to ITCM
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "NuMicro.h"
#include "UserImageBase.h"

#define TEST_BUFF_SIZE                  (0x200)
#define ITCM_BASE_ADDRESS               (0x00000400)

int load_image_to_ITCM(uint32_t image_base, uint32_t image_limit)
{
    uint32_t i, u32ImageSize = 0, *pu32Loader = NULL;


    u32ImageSize = image_limit - image_base;
    pu32Loader = (uint32_t *)image_base;

    printf("ImageSize = 0x%08X\n", u32ImageSize);
    printf("Loading user image to ITCM...");

    for (i = 0; i < u32ImageSize; i += 4, pu32Loader++)
    {
        outpw(ITCM_BASE_ADDRESS + i, *pu32Loader);
    }

    printf("OK.\n");

    return 0;
}

uint32_t SRAM_LoadCodeAndRun(void)
{
    if (&UserImageBase_start == &UserImageBase_finish)
    {
        printf("User image is not ready!!\n");

        while (1);
    }

    if (load_image_to_ITCM((uint32_t)&UserImageBase, (uint32_t)&UserImageBase_finish) < 0)
        return -1;

    return 0;
}
