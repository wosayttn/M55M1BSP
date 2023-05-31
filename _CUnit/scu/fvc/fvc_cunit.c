/******************************************************************************
* @file    fvc_cunit.c
* @version V1.00
* @brief   FVC CUnit Test
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
#include "fvc_cunit.h"

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

CU_SuiteInfo FVC_Suites[] =
{
    { "Func Test", Tests_Init, Tests_Clean, NULL, NULL, FVC_FuncTest },
    CU_SUITE_INFO_NULL
};

void Func_Open(void)
{
    CU_ASSERT_FATAL(FVC_Open() == 0);
}

void Func_ReadWriteVer(void)
{
    uint32_t i;

    for (i = 0; i < 64; i++)
    {
        CU_ASSERT(FVC_SetNVC(0, i) == 0);
        CU_ASSERT(FVC_GetNVC(0) == i);
    }

    for (; i < 80; i++)
    {
        CU_ASSERT(FVC_SetNVC(0, i) == -1);
    }

    for (i = 0; i < 64; i++)
    {
        CU_ASSERT(FVC_SetNVC(1, i) == 0);
        CU_ASSERT(FVC_GetNVC(1) == i);
    }

    for (; i < 80; i++)
    {
        CU_ASSERT(FVC_SetNVC(1, i) == -1);
    }

    for (i = 0; i < 256; i++)
    {
        CU_ASSERT(FVC_SetNVC(4, i) == 0);
        CU_ASSERT(FVC_GetNVC(4) == i);
    }

    for (; i < 300; i++)
    {
        CU_ASSERT(FVC_SetNVC(4, i) == -1);
    }

    for (i = 0; i < 256; i++)
    {
        CU_ASSERT(FVC_SetNVC(5, i) == 0);
        CU_ASSERT(FVC_GetNVC(5) == i);
    }

    for (; i < 300; i++)
    {
        CU_ASSERT(FVC_SetNVC(5, i) == -1);
    }

    CU_ASSERT(FVC_SetNVC(0, 3)  == 0);
    CU_ASSERT(FVC_SetNVC(1, 5)  == 0);

    CU_ASSERT(FVC_SetNVC(2, 11) == -1);
    CU_ASSERT(FVC_SetNVC(3, 23) == -1);
}

void Func_Monotonic(void)
{
    FVC_EnableMonotone();
    CU_ASSERT_EQUAL(FVC->CTL & FVC_CTL_MONOEN_Msk, FVC_CTL_MONOEN_Msk);
}

CU_TestInfo FVC_FuncTest[] =
{

    { "Open test",               Func_Open         },
    { "Read/Write version test", Func_ReadWriteVer },
    /* Monotonic can be set to 1 but cannot be cleared to 0 in real chip.*/
    { "Monotonic test",          Func_Monotonic    },
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/