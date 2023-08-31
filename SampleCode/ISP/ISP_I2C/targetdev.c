/***************************************************************************//**
 * @file     targetdev.c
 * @version  V1.00
 * @brief    ISP support function source file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include "targetdev.h"
#include "isp_user.h"

/* Supports maximum 2MB (APROM) */
uint32_t GetApromSize()
{
    /* The smallest of APROM u32Size is FMC_FLASH_PAGE_SIZE. */
    uint32_t u32Size = FMC_FLASH_PAGE_SIZE, u32Data;
    int result;

    do
    {
        result = FMC_Read_User(FMC_APROM_BASE + u32Size, &u32Data);

        if (result < 0)
        {
            return u32Size;
        }
        else
        {
            u32Size *= 2;
        }
    } while (1);
}

// Data Flash is shared with APROM.
// The size and start address are defined in CONFIG1.
void GetDataFlashInfo(uint32_t *addr, uint32_t *size)
{
    uint32_t uData;
    *size = 0;
    FMC_Read_User(Config0, &uData);

    if ((uData & 0x01) == 0)   //DFEN enable
    {
        FMC_Read_User(Config1, &uData);

        uData &= 0x000FFFFF;

        if (uData > g_u32ApromSize || uData & (FMC_FLASH_PAGE_SIZE - 1))   //avoid config1 value from error
        {
            uData = g_u32ApromSize;
        }

        *addr = uData;
        *size = g_u32ApromSize - uData;
    }
    else
    {
        *addr = g_u32ApromSize;
        *size = 0;
    }
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/