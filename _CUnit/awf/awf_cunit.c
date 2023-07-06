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
    SYS_ResetModule(SYS_AWF0RST);
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

void TestFunc_AWF_EnableWakeup(void)
{
    AWF_EnableWakeup(AWF_CTL_HTWKEN_Msk);
    CU_ASSERT((AWF->CTL & AWF_CTL_HTWKEN_Msk)>>AWF_CTL_HTWKEN_Pos == 1UL);
    AWF_DisableWakeup(AWF_CTL_HTWKEN_Msk);
    CU_ASSERT((AWF->CTL & AWF_CTL_HTWKEN_Msk)>>AWF_CTL_HTWKEN_Pos == 0UL);
    AWF_EnableWakeup(AWF_CTL_LTWKEN_Msk);
    CU_ASSERT((AWF->CTL & AWF_CTL_LTWKEN_Msk)>>AWF_CTL_LTWKEN_Pos == 1UL);
    AWF_DisableWakeup(AWF_CTL_LTWKEN_Msk);
    CU_ASSERT((AWF->CTL & AWF_CTL_LTWKEN_Msk)>>AWF_CTL_LTWKEN_Pos == 0UL);
}

void TestFunc_AWF_SET_ACC_COUNT(void)
{
    uint32_t TempValue;
    
    for(TempValue = 0; TempValue <= 0xF ; TempValue++)
    {
        AWF_SET_ACC_COUNT(TempValue);

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

void TestFunc_AWF_SET_HTH(void)
{
    uint32_t TempValue;
    
    for(TempValue = 1; TempValue <= 0x7FFFF ; TempValue = TempValue << 1)
    {
        AWF_SET_HTH(TempValue);
        CU_ASSERT((AWF->HTH&AWF_HTH_AWFHTH_Msk)>>AWF_HTH_AWFHTH_Pos == TempValue);
    }
}

void TestFunc_AWF_SET_LTH(void)
{
    uint32_t TempValue;
    
    for(TempValue = 1; TempValue <= 0x7FFFF ; TempValue = TempValue << 1)
    {
        AWF_SET_LTH(TempValue);
        CU_ASSERT((AWF->LTH&AWF_LTH_AWFLTH_Msk)>>AWF_LTH_AWFLTH_Pos == TempValue);
    }
}

void TestFunc_AWF_GET_ACUVAL(void)
{
    uint32_t TempValue,AccmTempValue;
    AWF_SET_WBINIT(0);
    AWF_SET_ACC_COUNT(8);
    
    for(TempValue = 1; TempValue <= 0xFFFF ; TempValue = TempValue << 1)
    {
        AWF_WRITE_DAT(TempValue);       
        AccmTempValue = (AWF->W0 + AWF->W1 + AWF->W2 + AWF->W3 + AWF->W4 + AWF->W5 + AWF->W6 + AWF->W7);
        CU_ASSERT(AWF_GET_ACUVAL() == AccmTempValue);
    }
}

void TestFunc_AWF_SET_WBINIT(void)
{
    uint32_t TempValue,AccmTempValue;
    AWF_SET_ACC_COUNT(8);
    
    for(TempValue = 1; TempValue <= 0xFFFF ; TempValue = TempValue << 1)
    {
        AWF_SET_WBINIT(TempValue);
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
    AWF_Close();
    
    AWF_CLEAR_HTH_INT_FLAG();
    AWF_CLEAR_LTH_INT_FLAG();
    CU_ASSERT(AWF_GET_HTH_INT_FLAG() == 0UL);
    CU_ASSERT(AWF_GET_LTH_INT_FLAG() == 0UL);
    
    AWF_Open(AWF_BOTHINT_DISABLE,AWF_BOTHWK_DISABLE,0,100,0,8);
    
    AWF_WRITE_DAT(50);

    CU_ASSERT(AWF_GET_HTH_INT_FLAG() == 1UL);
    CU_ASSERT(AWF_GET_LTH_INT_FLAG() == 1UL);
    
    AWF_SET_WBINIT(0);
    AWF_SET_HTH(0);
    AWF_SET_LTH(0);    
    AWF_CLEAR_HTH_INT_FLAG();
    AWF_CLEAR_LTH_INT_FLAG();    
    CU_ASSERT(AWF_GET_HTH_INT_FLAG() == 0UL);
    CU_ASSERT(AWF_GET_LTH_INT_FLAG() == 0UL);
}

void TestFunc_AWF_Open(void)
{
    uint32_t TempValue;
  
    AWF_Open(AWF_BOTHINT_ENABLE,AWF_BOTHWK_DISABLE,0,0,0,0);
    CU_ASSERT((AWF->CTL & AWF_CTL_HTIEN_Msk)>>AWF_CTL_HTIEN_Pos == 1UL);    
    CU_ASSERT((AWF->CTL & AWF_CTL_LTIEN_Msk)>>AWF_CTL_LTIEN_Pos == 1UL);
    
    AWF_Open(AWF_BOTHINT_DISABLE,AWF_BOTHWK_DISABLE,0,0,0,0);
    CU_ASSERT((AWF->CTL & AWF_CTL_LTIEN_Msk)>>AWF_CTL_LTIEN_Pos == 0UL);
    CU_ASSERT((AWF->CTL & AWF_CTL_HTIEN_Msk)>>AWF_CTL_HTIEN_Pos == 0UL); 
    
    AWF_Open(AWF_HTINT_ENABLE,AWF_BOTHWK_DISABLE,0,0,0,0);
    CU_ASSERT((AWF->CTL & AWF_CTL_HTIEN_Msk)>>AWF_CTL_HTIEN_Pos == 1UL);
    
    AWF_Open(AWF_LTINT_ENABLE,AWF_BOTHWK_DISABLE,0,0,0,0);
    CU_ASSERT((AWF->CTL & AWF_CTL_LTIEN_Msk)>>AWF_CTL_LTIEN_Pos == 1UL);


    AWF_Open(AWF_BOTHINT_DISABLE,AWF_BOTHWK_ENABLE,0,0,0,0);
    CU_ASSERT((AWF->CTL & AWF_CTL_HTWKEN_Msk)>>AWF_CTL_HTWKEN_Pos == 1UL);
    CU_ASSERT((AWF->CTL & AWF_CTL_LTWKEN_Msk)>>AWF_CTL_LTWKEN_Pos == 1UL);

    AWF_Open(AWF_BOTHINT_DISABLE,AWF_BOTHWK_DISABLE,0,0,0,0);
    CU_ASSERT((AWF->CTL & AWF_CTL_HTWKEN_Msk)>>AWF_CTL_HTWKEN_Pos == 0UL);
    CU_ASSERT((AWF->CTL & AWF_CTL_LTWKEN_Msk)>>AWF_CTL_LTWKEN_Pos == 0UL);

    AWF_Open(AWF_BOTHINT_DISABLE,AWF_HTWK_ENABLE,0,0,0,0);
    CU_ASSERT((AWF->CTL & AWF_CTL_HTWKEN_Msk)>>AWF_CTL_HTWKEN_Pos == 1UL);    
    AWF_Open(AWF_BOTHINT_DISABLE,AWF_LTWK_ENABLE,0,0,0,0);
    CU_ASSERT((AWF->CTL & AWF_CTL_LTWKEN_Msk)>>AWF_CTL_LTWKEN_Pos == 1UL);
 
    for(TempValue = 1; TempValue <= 0x7FFFF ; TempValue = TempValue << 1)
    {
        AWF_Open(AWF_BOTHINT_DISABLE,AWF_BOTHWK_DISABLE,TempValue,0,0,0);
        CU_ASSERT((AWF->HTH&AWF_HTH_AWFHTH_Msk)>>AWF_HTH_AWFHTH_Pos == TempValue);
    }

    for(TempValue = 1; TempValue <= 0x7FFFF ; TempValue = TempValue << 1)
    {
        AWF_Open(AWF_BOTHINT_DISABLE,AWF_BOTHWK_DISABLE,0,TempValue,0,0);
        CU_ASSERT((AWF->LTH&AWF_LTH_AWFLTH_Msk)>>AWF_LTH_AWFLTH_Pos == TempValue);
    }

    for(TempValue = 1; TempValue <= 0xFFFF ; TempValue = TempValue << 1)
    {
        AWF_Open(AWF_BOTHINT_DISABLE,AWF_BOTHWK_DISABLE,0,0,TempValue,0);
        CU_ASSERT(AWF->W0 == TempValue);
        CU_ASSERT(AWF->W1 == TempValue);
        CU_ASSERT(AWF->W2 == TempValue);
        CU_ASSERT(AWF->W3 == TempValue);
        CU_ASSERT(AWF->W4 == TempValue);
        CU_ASSERT(AWF->W5 == TempValue);
        CU_ASSERT(AWF->W6 == TempValue);
        CU_ASSERT(AWF->W7 == TempValue);
    }

    for(TempValue = 0; TempValue <= 0xF ; TempValue++)
    {
        AWF_Open(AWF_BOTHINT_DISABLE,AWF_BOTHWK_DISABLE,0,0,0,TempValue);

        if(TempValue <= 8)
        {
            CU_ASSERT((AWF->CTL&AWF_CTL_ACUCNT_Msk)>>AWF_CTL_ACUCNT_Pos == TempValue);
        }
        else
        {
            CU_ASSERT((AWF->CTL&AWF_CTL_ACUCNT_Msk)>>AWF_CTL_ACUCNT_Pos == 0UL);
        }
    }

    AWF_Close();
    CU_ASSERT((AWF->CTL & AWF_CTL_LTIEN_Msk)>>AWF_CTL_LTIEN_Pos == 0UL);
    CU_ASSERT((AWF->CTL & AWF_CTL_HTIEN_Msk)>>AWF_CTL_HTIEN_Pos == 0UL);
    CU_ASSERT((AWF->CTL & AWF_CTL_HTWKEN_Msk)>>AWF_CTL_HTWKEN_Pos == 0UL);
    CU_ASSERT((AWF->CTL & AWF_CTL_LTWKEN_Msk)>>AWF_CTL_LTWKEN_Pos == 0UL);
    CU_ASSERT((AWF->HTH&AWF_HTH_AWFHTH_Msk)>>AWF_HTH_AWFHTH_Pos == 0UL);
    CU_ASSERT((AWF->LTH&AWF_LTH_AWFLTH_Msk)>>AWF_LTH_AWFLTH_Pos == 0UL);  
    CU_ASSERT(AWF->W0 == 0UL);
    CU_ASSERT(AWF->W1 == 0UL);
    CU_ASSERT(AWF->W2 == 0UL);
    CU_ASSERT(AWF->W3 == 0UL);
    CU_ASSERT(AWF->W4 == 0UL);
    CU_ASSERT(AWF->W5 == 0UL);
    CU_ASSERT(AWF->W6 == 0UL);
    CU_ASSERT(AWF->W7 == 0UL);
    CU_ASSERT((AWF->CTL&AWF_CTL_ACUCNT_Msk)>>AWF_CTL_ACUCNT_Pos == 0UL);    
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
    { "AWF_Enable/DisableThresholdWakeup", TestFunc_AWF_EnableWakeup },
    { "AWF_SET_ACC_COUNT", TestFunc_AWF_SET_ACC_COUNT },
    { "AWF_SET_HTH", TestFunc_AWF_SET_HTH },
    { "AWF_SET_LTH", TestFunc_AWF_SET_LTH },
    { "AWF_GET_ACUVAL", TestFunc_AWF_GET_ACUVAL },
    { "AWF_SET_WBINIT", TestFunc_AWF_SET_WBINIT },
    { "AWF_Open/Close", TestFunc_AWF_Open },
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/