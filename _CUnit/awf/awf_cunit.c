/******************************************************************************
* @file    AWF_cunit.c
* @version V1.00
* @brief   CUnit test AWF
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
#include "awf_cunit.h"

int AWF_Test_Init(void)
{
    CLK_EnableModuleClock(AWF0_MODULE);
    return 0;
}
int AWF_Test_Clean(void)
{
    return 0;
}

void TestFunc_AWF_EnableInt(void)
{
    AWF_EnableInt(AWF_CTL_HTIEN_Msk);
    CU_ASSERT((AWF->CTL & AWF_CTL_HTIEN_Msk)>>AWF_CTL_HTIEN_Pos == 1UL);
    AWF_DisableInt(AWF_CTL_HTIEN_Msk);
    CU_ASSERT((AWF->CTL & AWF_CTL_HTIEN_Msk)>>AWF_CTL_HTIEN_Pos == 0UL);
    AWF_EnableInt(AWF_CTL_LTIEN_Msk);
    CU_ASSERT((AWF->CTL & AWF_CTL_LTIEN_Msk)>>AWF_CTL_LTIEN_Pos == 1UL);
    AWF_DisableInt(AWF_CTL_LTIEN_Msk);
    CU_ASSERT((AWF->CTL & AWF_CTL_LTIEN_Msk)>>AWF_CTL_LTIEN_Pos == 0UL);    
}

void TestFunc_AWF_EnableTWK(void)
{
    AWF_EnableTWK(AWF_CTL_HTWKEN_Msk);
    CU_ASSERT((AWF->CTL & AWF_CTL_HTWKEN_Msk)>>AWF_CTL_HTWKEN_Pos == 1UL);
    AWF_DisableTWK(AWF_CTL_HTWKEN_Msk);
    CU_ASSERT((AWF->CTL & AWF_CTL_HTWKEN_Msk)>>AWF_CTL_HTWKEN_Pos == 0UL);
    AWF_EnableTWK(AWF_CTL_LTWKEN_Msk);
    CU_ASSERT((AWF->CTL & AWF_CTL_LTWKEN_Msk)>>AWF_CTL_LTWKEN_Pos == 1UL);
    AWF_DisableTWK(AWF_CTL_LTWKEN_Msk);
    CU_ASSERT((AWF->CTL & AWF_CTL_LTWKEN_Msk)>>AWF_CTL_LTWKEN_Pos == 0UL);
}

void TestFunc_AWF_SetAccumlationCount(void)
{
    uint32_t TempValue;
    
    for(TempValue = 0; TempValue <= 0xF ; TempValue++)
    {
        AWF_SetAccumlationCount(TempValue);

        if(TempValue <= 8)
        {
            CU_ASSERT((AWF->CTL&AWF_CTL_ACUCNT_Msk)>>AWF_CTL_ACUCNT_Pos == TempValue);
        }
        else
        {
            CU_ASSERT((AWF->CTL&AWF_CTL_ACUCNT_Msk)>>AWF_CTL_ACUCNT_Pos == 0UL);
        }
    }
}

void TestFunc_AWF_SetHTHValue(void)
{
    uint32_t TempValue;
    
    for(TempValue = 1; TempValue <= 0x7FFFF ; TempValue = TempValue << 1)
    {
        AWF_SetHTHValue(TempValue);
        CU_ASSERT((AWF->HTH&AWF_HTH_AWFHTH_Msk)>>AWF_HTH_AWFHTH_Pos == TempValue);
    }
}

void TestFunc_AWF_SetLTHValue(void)
{
    uint32_t TempValue;
    
    for(TempValue = 1; TempValue <= 0x7FFFF ; TempValue = TempValue << 1)
    {
        AWF_SetLTHValue(TempValue);
        CU_ASSERT((AWF->LTH&AWF_LTH_AWFLTH_Msk)>>AWF_LTH_AWFLTH_Pos == TempValue);
    }
}

void TestFunc_AWF_GetAccumlationValue(void)
{
    uint32_t TempValue,AccmTempValue;
    AWF_SetWBINITValue(0);
    AWF_SetAccumlationCount(8);
    
    for(TempValue = 1; TempValue <= 0xFFFF ; TempValue = TempValue << 1)
    {
        AWF_SetDAT(TempValue);       
        AccmTempValue = (AWF->W0 + AWF->W1 + AWF->W2 + AWF->W3 + AWF->W4 + AWF->W5 + AWF->W6 + AWF->W7);
        CU_ASSERT(AWF_GetAccumlationValue() == AccmTempValue);
    }
}

void TestFunc_AWF_SetWBINITValue(void)
{
    uint32_t TempValue,AccmTempValue;
    AWF_SetAccumlationCount(8);
    
    for(TempValue = 1; TempValue <= 0xFFFF ; TempValue = TempValue << 1)
    {
        AWF_SetWBINITValue(TempValue);
        CU_ASSERT(AWF->W0 == TempValue);
        CU_ASSERT(AWF->W1 == TempValue);
        CU_ASSERT(AWF->W2 == TempValue);
        CU_ASSERT(AWF->W3 == TempValue);
        CU_ASSERT(AWF->W4 == TempValue);
        CU_ASSERT(AWF->W5 == TempValue);
        CU_ASSERT(AWF->W6 == TempValue);
        CU_ASSERT(AWF->W7 == TempValue);
    }
}

void TestFunc_AWF_TestMacro(void)
{
    printf("\n");
    AWF_CLEAR_HTH_INT_FLAG();
    AWF_CLEAR_LTH_INT_FLAG();
    CU_ASSERT(AWF_GET_HTH_INT_FLAG() == 0UL);
    CU_ASSERT(AWF_GET_LTH_INT_FLAG() == 0UL);
    
    AWF_SetDAT(50);
    AWF_SetHTHValue(0);
    AWF_SetLTHValue(100);
    CU_ASSERT(AWF_GET_HTH_INT_FLAG() == 1UL);
    CU_ASSERT(AWF_GET_LTH_INT_FLAG() == 1UL);
    
    AWF_SetWBINITValue(0);
    AWF_SetHTHValue(0);
    AWF_SetLTHValue(0);    
    AWF_CLEAR_HTH_INT_FLAG();
    AWF_CLEAR_LTH_INT_FLAG();    
    CU_ASSERT(AWF_GET_HTH_INT_FLAG() == 0UL);
    CU_ASSERT(AWF_GET_LTH_INT_FLAG() == 0UL);
}

CU_SuiteInfo AWFSuites[] = {
    { "AWF Macro Test", AWF_Test_Init, AWF_Test_Clean, NULL, NULL, AWF_MacroTest },
    { "AWF Func  Test", AWF_Test_Init, AWF_Test_Clean, NULL, NULL, AWF_FuncTest },
    CU_SUITE_INFO_NULL
};

CU_TestInfo  AWF_MacroTest[] = {
    { "Macro",  TestFunc_AWF_TestMacro },
    CU_TEST_INFO_NULL
};

CU_TestInfo  AWF_FuncTest[] = {
    { "AWF_Enable/DisableInterrupt", TestFunc_AWF_EnableInt },
    { "AWF_Enable/DisableThresholdWakeup", TestFunc_AWF_EnableTWK },
    { "AWF_SetAccumlationCount", TestFunc_AWF_SetAccumlationCount },
    { "AWF_SetHTHValue", TestFunc_AWF_SetHTHValue },
    { "AWF_SetLTHValue", TestFunc_AWF_SetLTHValue },
    { "AWF_GetAccumlationValue", TestFunc_AWF_GetAccumlationValue },
    { "AWF_SetWBINITValue", TestFunc_AWF_SetWBINITValue },    
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/