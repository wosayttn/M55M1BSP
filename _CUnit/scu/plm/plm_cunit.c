/******************************************************************************
* @file    scu_cunit.c
* @version V1.00
* @brief   PLM CUnit Test
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "plm_cunit.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int Tests_Init(void)
{
    return 0;
}
int Tests_Clean(void)
{
    return 0;
}

CU_SuiteInfo PLM_Suites[] =
{
    {"API Test", Tests_Init, Tests_Clean, NULL, NULL, PLM_FuncTest},
    CU_SUITE_INFO_NULL
};

void API_StageTest()
{
#if 1
    CU_ASSERT(PLM_GetStage() == PLM_VENDOR);
#else
    /* In real chip, it needs ROMMAP erase to restore PLM stage to ePLM_STAGE_VENDOR. */
    CU_ASSERT(PLM_SetStage(ePLM_STAGE_OEM) == 0);
    CU_ASSERT(PLM_GetStage() == ePLM_STAGE_OEM);

    CU_ASSERT(PLM_SetStage(ePLM_STAGE_DEPLOYED) == 0);
    CU_ASSERT(PLM_GetStage() == ePLM_STAGE_DEPLOYED);

    /* After enter RMA stage, it needs to sign image with ECC-P256_NuROTPK to boot. */
    //CU_ASSERT(PLM_SetStage(ePLM_STAGE_RMA) == 0);
    //CU_ASSERT(PLM_GetStage() == ePLM_STAGE_RMA);
#endif
}

CU_TestInfo PLM_FuncTest[] =
{
    { "Stage test", API_StageTest },
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/