/*---------------------------------------------------------------------------------------------------------*/
/* Winbond Electronics Corporation confidential                                                            */
/*                                                                                                         */
/* Copyright (c) 2007 by Winbond Electronics Corporation                                                   */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   timer_cunit_test.c                                                                                    */
/*            The test function of VIC for CUnit.                                                          */
/* Project:   DA8205                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "hbi_cunit.h"




/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int suite_success_init(void)
{
    return 0;
}
int suite_success_clean(void)
{
    return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function: function_name                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               param - [in/out], parameter_description                                                   */
/*               param - [in/out], parameter_description                                                   */
/*                                                                                                         */
/* Returns:      the_value                                                                                 */
/* Side effects: the_effect                                                                                */
/* Description:                                                                                            */
/*               description                                                                               */
/*---------------------------------------------------------------------------------------------------------*/

CU_SuiteInfo suites[] =
{
    {"HBI API", suite_success_init, suite_success_clean, NULL, NULL, HBI_ApiTests},

    CU_SUITE_INFO_NULL
};


void TestFunc()
{
    extern int32_t g_HBI_i32ErrCode;
    uint32_t u32Data, u32Data2;

    HBI_Reset();
    CU_ASSERT(g_HBI_i32ErrCode == 0);

    HBI_ExitHSAndDPD();
    CU_ASSERT(g_HBI_i32ErrCode == 0);

    u32Data = HBI_ReadHyperRAMReg(HYPERRAM_ID_REG0);
    CU_ASSERT(u32Data > 0);
    CU_ASSERT((u32Data & 0xf) == 0x6);

    u32Data = HBI_ReadHyperRAMReg(HYPERRAM_ID_REG1);
    CU_ASSERT(u32Data > 0);
    CU_ASSERT((u32Data & 0xf) == 0x1);

    u32Data = HBI_ReadHyperRAMReg(HYPERRAM_CONFIG_REG0);
    CU_ASSERT(u32Data > 0);

    u32Data = HBI_ReadHyperRAMReg(HYPERRAM_CONFIG_REG1);
    CU_ASSERT(u32Data > 0);

    u32Data = HBI_ReadHyperRAMReg(HYPERRAM_CONFIG_REG0);
    u32Data &= ~0x3;
    HBI_WriteHyperRAMReg(HYPERRAM_CONFIG_REG0, u32Data);
    u32Data2 = HBI_ReadHyperRAMReg(HYPERRAM_CONFIG_REG0);
    CU_ASSERT(u32Data == u32Data2);
    u32Data |= 0x3;
    HBI_WriteHyperRAMReg(HYPERRAM_CONFIG_REG0, u32Data);
    u32Data2 = HBI_ReadHyperRAMReg(HYPERRAM_CONFIG_REG0);
    CU_ASSERT(u32Data == u32Data2);

    HBI_Write1Byte(0x121, 0x5a);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_OK);
    HBI_Write1Byte(0x120, 0xaa);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_OK);
    HBI_Write1Byte(0x122, 0xbb);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_OK);
    HBI_Write1Byte(0x123, 0xcc);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_OK);
    u32Data = HBI_Read2Byte(0x120);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_OK);
    CU_ASSERT(u32Data == 0x5aaa);
    u32Data = HBI_Read2Byte(0x121);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_ERR_ALIGN);
    g_HBI_i32ErrCode = 0;
    u32Data = HBI_Read4Byte(0x121);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_ERR_ALIGN);
    g_HBI_i32ErrCode = 0;
    u32Data = HBI_Read4Byte(0x122);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_ERR_ALIGN);
    g_HBI_i32ErrCode = 0;
    u32Data = HBI_Read4Byte(0x123);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_ERR_ALIGN);
    g_HBI_i32ErrCode = 0;
    u32Data = HBI_Read4Byte(0x120);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_OK);
    CU_ASSERT(u32Data == 0xccbb5aaa);

    HBI_Write2Byte(0x23, 0xaabb);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_ERR_ALIGN);
    g_HBI_i32ErrCode = 0;
    HBI_Write2Byte(0x22, 0xaabb);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_OK);
    u32Data = HBI_Read2Byte(0x22);
    CU_ASSERT(u32Data == 0xaabb);


    HBI_Write3Byte(0x23, 0xaabbcc);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_OK);
    HBI_Write1Byte(0x25, 0x77);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_OK);
    HBI_Write3Byte(0x22, 0xaabbcc);
    CU_ASSERT(g_HBI_i32ErrCode == HBI_OK);
    u32Data = HBI_Read2Byte(0x22);
    CU_ASSERT(u32Data == 0xbbcc);
    u32Data = HBI_Read4Byte(0x22);
    CU_ASSERT(u32Data == 0x77aabbcc);


}

CU_TestInfo HBI_ApiTests[] =
{
    {"Testing HBI Function:", TestFunc},

    CU_TEST_INFO_NULL
};
