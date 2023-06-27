/**************************************************************************//**
 * @file     id_test.c
 * @version  V1.0
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "NuMicro.h"
#include "spim_common.h"

//------------------------------------------------------------------------------
#define TEST_IP         "SPIM"
#define TEST_ITEM       "ID Test"

//------------------------------------------------------------------------------
int g_CID = MFGID_WINBOND;

int DoIdTest(SPIM_T *pSPIMx, unsigned int serNo)
{
    uint8_t idBuf[3] = {0};

    SPIM_ReadJedecId(pSPIMx, idBuf, sizeof(idBuf), 1, 0, 0);

    printf("[%s][%s]ID=0x%02X, 0x%02X, 0x%02X\n",
           TEST_IP,
           TEST_ITEM,
           idBuf[0],
           idBuf[1],
           idBuf[2]);

    if (idBuf[0] != MFGID_WINBOND &&
            idBuf[0] != MFGID_MXIC &&
            idBuf[0] != MFGID_EON &&
            idBuf[0] != MFGID_ISSI &&
            idBuf[0] != MFGID_SPANSION &&
            idBuf[0] != MFGID_MICRON)
    {
        return SPIM_ERR_FAIL;
    }

    if (idBuf[0] == MFGID_MICRON)
    {
        SPIM_InitFlash(pSPIMx, 1);                          // Init flash.
    }

    g_CID = idBuf[0];

    return SPIM_OK;
}

int Flash_IDTest(uint32_t u32SPIMModule)
{
    SPIM_T *pSPIMx = (SPIM_T *)SPIM_GetModule(u32SPIMModule);

    printf("[%s%d][%s] ...\n", TEST_IP, u32SPIMModule, TEST_ITEM);

    SPIM_InitFlash(pSPIMx, 1);                          // Init flash.

    if (DoIdTest(pSPIMx, 1) < 0)
    {
        printf("[%s][%s]FAILED\n", TEST_IP, TEST_ITEM);
        return SPIM_ERR_FAIL;
    }

    printf("[%s][%s]PASSED\n", TEST_IP, TEST_ITEM);

    return SPIM_OK;
}
