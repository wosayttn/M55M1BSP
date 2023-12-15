/**************************************************************************//**
 * @file     otfc_cunit.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 15/09/15 4:36p $
 * @brief    TC8263 series OTFC CUnit test function.
 *
 * @note
 * Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

//* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "common.h"
#include "spim_cunit.h"
#include "hyper_cunit.h"
#include "otfc_ks.h"

//------------------------------------------------------------------------------
void *GetOTFCModule(uint32_t u32Idx)
{
    switch (u32Idx)
    {
        case 0:
            return OTFC0;
            break;

        case 1:
            return OTFC1;
            break;
    }

    return NULL;
}

int OTFC_Tests_Init(void)
{
    uint32_t u32Delay = 0;
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    SPIM_HYPER_ENABLE_HYPMODE(pSPIMModule);     //Enable HyperBus Mode
    //CU_ASSERT_TRUE((pSPIMModule->CTL0 & SPIM_CTL0_HYPER_EN_Msk) >> SPIM_CTL0_HYPER_EN_Pos);

    SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 2); /* Set SPIM clock as HCLK divided by 2 */
    //CU_ASSERT(SPIM_GET_CLOCK_DIVIDER(pSPIMModule) == 1);

    //SPIM Def. Enable Cipher, First Disable the test.
    SPIM_DISABLE_CIPHER(pSPIMModule);
    //CU_ASSERT_TRUE((pSPIMModule->CTL0 & SPIM_CTL0_CIPHOFF_Msk) >> SPIM_CTL0_CIPHOFF_Pos);

    SPIM_Hyper_DefaultConfig(pSPIMModule, 780, 7, 7);

    SPIM_HYPER_Reset(pSPIMModule);

    for (u32Delay = 0; u32Delay < 0x400; u32Delay++) {}

    return 0;
}

int OTFC_Tests_Clean(void)
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    SPIM_SET_FLASH_MODE(pSPIMModule);     //Restore SPI Flash Mode
    //CU_ASSERT_FALSE((pSPIMModule->CTL0 & SPIM_CTL0_HYPER_EN_Msk) >> SPIM_CTL0_HYPER_EN_Pos);

    return 0;
}

void OTFC_Const_PR()
{
    CU_ASSERT(OTFC_CTL_PR_Pos == (8));

    CU_ASSERT(OTFC_CTL_RST_Pos == (1));
    CU_ASSERT(OTFC_CTL_SWAP_Pos == (2));
    CU_ASSERT(OTFC_CTL_INT_Pos == (7));

    CU_ASSERT(OTFC_STS_KSERR_Pos(0) == (5));
    CU_ASSERT(OTFC_STS_KSERR_Pos(1) == (13));
    CU_ASSERT(OTFC_STS_KSERR_Pos(2) == (21));
    CU_ASSERT(OTFC_STS_KSERR_Pos(3) == (29));

    //CU_ASSERT(OTFC_STS_DFAE_Pos(0) == (6));
    //CU_ASSERT(OTFC_STS_DFAE_Pos(1) == (6));
    //CU_ASSERT(OTFC_STS_DFAE_Pos(2) == (6));
    //CU_ASSERT(OTFC_STS_DFAE_Pos(3) == (6));

    CU_ASSERT(OTFC_STS_IF_Pos(0) == (7));
    CU_ASSERT(OTFC_STS_IF_Pos(1) == (15));
    CU_ASSERT(OTFC_STS_IF_Pos(2) == (23));
    CU_ASSERT(OTFC_STS_IF_Pos(3) == (31));

    CU_ASSERT(OTFC_PR_0 == (0));
    CU_ASSERT(OTFC_PR_1 == (1));
    CU_ASSERT(OTFC_PR_2 == (2));
    CU_ASSERT(OTFC_PR_3 == (3));

    CU_ASSERT(OTFC_KEY_SRC_REG == (0x00));
    CU_ASSERT(OTFC_KEY_SRC_KS == (0x01));

    CU_ASSERT(OTFC_KS_SRC_SRAM == (0x00));
    CU_ASSERT(OTFC_KS_SRC_OTP == (0x02));
}

void MACRO_OTFC_CTL()
{
    OTFC_T *pOTFCModule = NULL;

    pOTFCModule = (OTFC_T *)GetOTFCModule(GetSPIMTestModuleIdx());

    /*
     *  OTFC_CTL_EN_ON() / OTFC_CTL_EN_OFF()
     */
    pOTFCModule->CTL = 0;
    OTFC_ENABLE_PR(pOTFCModule, OTFC_PR_0);
    CU_ASSERT_TRUE((pOTFCModule->CTL & (1UL << (0 * 8))) >> (0 * 8));
    OTFC_DISABLE_PR(pOTFCModule, OTFC_PR_0);
    CU_ASSERT_FALSE((pOTFCModule->CTL & (1UL << (0 * 8))) >> (0 * 8));

    OTFC_ENABLE_PR(pOTFCModule, 1);
    CU_ASSERT_TRUE((pOTFCModule->CTL & (1UL << (1 * 8))) >> (1 * 8));
    OTFC_DISABLE_PR(pOTFCModule, 1);
    CU_ASSERT_FALSE((pOTFCModule->CTL & (1UL << (1 * 8))) >> (1 * 8));

    OTFC_ENABLE_PR(pOTFCModule, 2);
    CU_ASSERT_TRUE((pOTFCModule->CTL & (1UL << (2 * 8))) >> (2 * 8));
    OTFC_DISABLE_PR(pOTFCModule, 2);
    CU_ASSERT_FALSE((pOTFCModule->CTL & (1UL << (2 * 8))) >> (2 * 8));

    OTFC_ENABLE_PR(pOTFCModule, 3);
    CU_ASSERT_TRUE((pOTFCModule->CTL & (1UL << (3 * 8))) >> (3 * 8));
    OTFC_DISABLE_PR(pOTFCModule, 3);
    CU_ASSERT_FALSE((pOTFCModule->CTL & (1UL << (3 * 8))) >> (3 * 8));

    /*
     *  OTFC_RESET_PR()
     */
    pOTFCModule->CTL = 0;
    OTFC_RESET_PR(pOTFCModule, 0);
    CU_ASSERT_FALSE((pOTFCModule->CTL & ((0 * 8) + 1)) >> ((0 * 8) + 1));
    OTFC_RESET_PR(pOTFCModule, 1);
    CU_ASSERT_FALSE((pOTFCModule->CTL & ((1 * 8) + 1)) >> ((1 * 8) + 1));
    OTFC_RESET_PR(pOTFCModule, 2);
    CU_ASSERT_FALSE((pOTFCModule->CTL & ((2 * 8) + 1)) >> ((2 * 8) + 1));
    OTFC_RESET_PR(pOTFCModule, 3);
    CU_ASSERT_FALSE((pOTFCModule->CTL & ((3 * 8) + 1)) >> ((3 * 8) + 1));

    /*
     *  OTFC_ENABLE_KEYSWAP()/OTFC_DISABLE_KEYSWAP()
     */
    pOTFCModule->CTL = 0;
    OTFC_ENABLE_KEYSWAP(pOTFCModule, 0);
    CU_ASSERT_TRUE((pOTFCModule->CTL & (1UL << ((0 * 8) + 2))) >> ((0 * 8) + 2));
    OTFC_DISABLE_KEYSWAP(pOTFCModule, 0);
    CU_ASSERT_FALSE((pOTFCModule->CTL & (1UL << ((0 * 8) + 2))) >> ((0 * 8) + 2));
    pOTFCModule->CTL = 0;
    OTFC_ENABLE_KEYSWAP(pOTFCModule, 1);
    CU_ASSERT_TRUE((pOTFCModule->CTL & (1UL << ((1 * 8) + 2))) >> ((1 * 8) + 2));
    OTFC_DISABLE_KEYSWAP(pOTFCModule, 1);
    CU_ASSERT_FALSE((pOTFCModule->CTL & (1UL << ((1 * 8) + 2))) >> ((1 * 8) + 2));
    pOTFCModule->CTL = 0;
    OTFC_ENABLE_KEYSWAP(pOTFCModule, 2);
    CU_ASSERT_TRUE((pOTFCModule->CTL & (1UL << ((2 * 8) + 2))) >> ((2 * 8) + 2));
    OTFC_DISABLE_KEYSWAP(pOTFCModule, 2);
    CU_ASSERT_FALSE((pOTFCModule->CTL & (1UL << ((2 * 8) + 2))) >> ((2 * 8) + 2));
    pOTFCModule->CTL = 0;
    OTFC_ENABLE_KEYSWAP(pOTFCModule, 3);
    CU_ASSERT_TRUE((pOTFCModule->CTL & (1UL << ((3 * 8) + 2))) >> ((3 * 8) + 2));
    OTFC_DISABLE_KEYSWAP(pOTFCModule, 3);
    CU_ASSERT_FALSE((pOTFCModule->CTL & (1UL << ((3 * 8) + 2))) >> ((3 * 8) + 2));

    /*
     *  OTFC_ENABLE_INT() / OTFC_DISABLE_INT()
     */
    pOTFCModule->CTL = 0;
    OTFC_ENABLE_INT(pOTFCModule, 0);
    CU_ASSERT_TRUE((pOTFCModule->CTL & (1UL << ((0 * 8) + 7))) >> ((0 * 8) + 7));
    OTFC_DISABLE_INT(pOTFCModule, 0);
    CU_ASSERT_FALSE((pOTFCModule->CTL & (1UL << ((0 * 8) + 7))) >> ((0 * 8) + 7));
    pOTFCModule->CTL = 0;
    OTFC_ENABLE_INT(pOTFCModule, 1);
    CU_ASSERT_TRUE((pOTFCModule->CTL & (1UL << ((1 * 8) + 7))) >> ((1 * 8) + 7));
    OTFC_DISABLE_INT(pOTFCModule, 1);
    CU_ASSERT_FALSE((pOTFCModule->CTL & (1UL << ((1 * 8) + 7))) >> ((1 * 8) + 7));
    pOTFCModule->CTL = 0;
    OTFC_ENABLE_INT(pOTFCModule, 2);
    CU_ASSERT_TRUE((pOTFCModule->CTL & (1UL << ((2 * 8) + 7))) >> ((2 * 8) + 7));
    OTFC_DISABLE_INT(pOTFCModule, 2);
    CU_ASSERT_FALSE((pOTFCModule->CTL & (1UL << ((2 * 8) + 7))) >> ((2 * 8) + 7));
    pOTFCModule->CTL = 0;
    OTFC_ENABLE_INT(pOTFCModule, 3);
    CU_ASSERT_TRUE((pOTFCModule->CTL & (1UL << ((3 * 8) + 7))) >> ((3 * 8) + 7));
    OTFC_DISABLE_INT(pOTFCModule, 3);
    CU_ASSERT_FALSE((pOTFCModule->CTL & (1UL << ((3 * 8) + 7))) >> ((3 * 8) + 7));

    CU_PASS();
}

void OTFC_KeyFormRegister_Func()
{
    uint32_t offset = 0;
    SPIM_T *pSPIMx = NULL;
    OTFC_T *pOTFCModule = NULL;
    uint32_t u32PRx = 0;
    uint8_t *u8TstBuf1 = (uint8_t *)GetTestBuffer1();
    uint8_t *u8TstBuf2 = (uint8_t *)GetTestBuffer2();
    // Key = 0x7A29E38E 063FF08A 2F7A7F2A 93484D6F 93484D6F 2F7A7F2A 063FF08A 7A29E38E
    uint32_t au32OTFCKey[8] =
    {
        0x93484D6F, //Key0
        0x2F7A7F2A, //Key1
        0x063FF08A, //Key2
        0x7A29E38E, //Key3
        0x7A29E38E, //Scramble
        0x063FF08A, //NONCE0
        0x2F7A7F2A, //NONCE1
        0x93484D6F, //NONCE2
    };

    pSPIMx = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());
    pOTFCModule = (OTFC_T *)GetOTFCModule(GetSPIMTestModuleIdx());

    TrainingDllLatency();

    for (offset = 0; offset < HRAM_PAGE_SIZE; offset += TEST_BUFF_SIZE)
    {
        /*------------------------------------------------------*/
        /*  Erase page and verify                               */
        /*------------------------------------------------------*/
        // Erase accessed address range.
        SPIM_EraseHyperRAM(pSPIMx, offset, TEST_BUFF_SIZE);
        popDat(u8TstBuf1, TEST_BUFF_SIZE);

        OTFC_SetKeyFromKeyReg(pOTFCModule,
                              au32OTFCKey,
                              u32PRx,
                              offset,
                              offset + TEST_BUFF_SIZE);

        OTFC_ENABLE_PR(pOTFCModule, u32PRx);
        SPIM_ENABLE_CIPHER(pSPIMx);

        SPIM_HYPER_DMAWrite(pSPIMx, offset, u8TstBuf1, TEST_BUFF_SIZE);

        memset(u8TstBuf2, 0x0, TEST_BUFF_SIZE);
        SPIM_HYPER_DMARead(pSPIMx, offset, u8TstBuf2, TEST_BUFF_SIZE);

        if (memcmp(u8TstBuf1, u8TstBuf2, TEST_BUFF_SIZE))
        {
            printf("SPIM Hyper DMA Test Failed\r\n");
            CU_FAIL();
        }

        SPIM_DISABLE_CIPHER(pSPIMx);
        OTFC_DISABLE_PR(pOTFCModule, u32PRx);

        u32PRx++;

        if (u32PRx >= 4)
        {
            u32PRx = 0;
        }
    }

    CU_PASS();
}

void OTFC_KeyFormKSSRAM_Func()
{
    uint32_t offset = 0;
    SPIM_T *pSPIMx = NULL;
    OTFC_T *pOTFCModule = NULL;
    uint32_t u32PRx = 0;
    int i32KeyIdx = 0;
    uint8_t *u8TstBuf1 = (uint8_t *)GetTestBuffer1();
    uint8_t *u8TstBuf2 = (uint8_t *)GetTestBuffer2();
    // Key = 0x7A29E38E 063FF08A 2F7A7F2A 93484D6F 93484D6F 2F7A7F2A 063FF08A 7A29E38E
    uint32_t gau32AESKey[8] =
    {
        0x93484D6F, //Key0
        0x2F7A7F2A, //Key1
        0x063FF08A, //Key2
        0x7A29E38E, //Key3
        0x7A29E38E, //Scramble
        0x063FF08A, //NONCE0
        0x2F7A7F2A, //NONCE1
        0x93484D6F, //NONCE2
    };
    KS_MEM_Type eMemType = KS_SRAM;

    pSPIMx = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());
    pOTFCModule = (OTFC_T *)GetOTFCModule(GetSPIMTestModuleIdx());

    TrainingDllLatency();

    if (KS_Open() != 0)
    {
        printf("KS init failed !\n");

        while (1);
    }

    i32KeyIdx = KS_SetAESToKeyStoreSRAM(eMemType, TRUE, gau32AESKey);

    OTFC_SetKeyFromKeyStore(pOTFCModule,
                            u32PRx,
                            offset,
                            offset + HRAM_PAGE_SIZE,
                            i32KeyIdx,
                            OTFC_KS_SRC_SRAM);

    OTFC_SetScrambleNum(pOTFCModule, u32PRx, gau32AESKey[4]);

    OTFC_SetNonceNum(pOTFCModule, u32PRx, gau32AESKey[5], gau32AESKey[6], gau32AESKey[7]);

    for (offset = 0; offset < HRAM_PAGE_SIZE; offset += TEST_BUFF_SIZE)
    {
        /*------------------------------------------------------*/
        /*  Erase page and verify                               */
        /*------------------------------------------------------*/
        // Erase accessed address range.
        SPIM_EraseHyperRAM(pSPIMx, offset, TEST_BUFF_SIZE);
        popDat(u8TstBuf1, TEST_BUFF_SIZE);

        OTFC_ENABLE_PR(pOTFCModule, u32PRx);
        SPIM_ENABLE_CIPHER(pSPIMx);

        SPIM_HYPER_DMAWrite(pSPIMx, offset, u8TstBuf1, TEST_BUFF_SIZE);

        memset(u8TstBuf2, 0x0, TEST_BUFF_SIZE);
        SPIM_HYPER_DMARead(pSPIMx, offset, u8TstBuf2, TEST_BUFF_SIZE);

        if (memcmp(u8TstBuf1, u8TstBuf2, TEST_BUFF_SIZE))
        {
            printf("SPIM Hyper DMA Test Failed\r\n");
            CU_FAIL();
        }
    }

    SPIM_DISABLE_CIPHER(pSPIMx);
    OTFC_DISABLE_PR(pOTFCModule, u32PRx);

    CU_PASS();
}

CU_TestInfo OTFC_ConstantTests[] =
{
    {"SPIM CONST Address", OTFC_Const_PR},

    CU_TEST_INFO_NULL
};

CU_TestInfo OTFC_MacroTests[] =
{
    {"OTFC CTL Macros", MACRO_OTFC_CTL},
    //{"HYPER_CONFIG2 Macros", MACRO_SPIM_HYPER_CONFIG2},
    //{"HYPER_INT Macros", MACRO_SPIM_HYPER_INTEN},

    CU_TEST_INFO_NULL,
};

CU_TestInfo OTFC_ApiTests[] =
{
    {"OTFC Key from Register Function", OTFC_KeyFormRegister_Func},
    {"OTFC Key from KS SRAM Function", OTFC_KeyFormKSSRAM_Func},

    CU_TEST_INFO_NULL
};

/*----------------------------------------------------------------------------*/
/* Function: function_name                                                    */
/*                                                                            */
/* Parameters:                                                                */
/*               param - [in/out], parameter_description                      */
/*               param - [in/out], parameter_description                      */
/*                                                                            */
/* Returns:      the_value                                                    */
/* Side effects: the_effect                                                   */
/* Description:                                                               */
/*               description                                                  */
/*----------------------------------------------------------------------------*/
CU_SuiteInfo otfc_suites[] =
{
    {"OTFC CONST", OTFC_Tests_Init, OTFC_Tests_Clean, NULL, NULL, OTFC_ConstantTests},
    {"OTFC MACRO", OTFC_Tests_Init, OTFC_Tests_Clean, NULL, NULL, OTFC_MacroTests},
    {"OTFC API", OTFC_Tests_Init, OTFC_Tests_Clean, NULL, NULL, OTFC_ApiTests},

    CU_SUITE_INFO_NULL,
};
