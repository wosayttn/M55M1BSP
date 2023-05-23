/******************************************************************************
* @file    kdf_cunit.c
* @version V1.00
* @brief   KDF CUnit Test
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
#include "kdf_cunit.h"

uint8_t g_au8Keyout[0xFF00 / 8];

int KDF_Test_Init(void)
{
    return 0;
}
int KDF_Test_Clean(void)
{
    return 0;
}

void Func_Test(void)
{
    int32_t i;
    uint8_t au8Data[32];
    
    for (i = 0; i < 0xFF; i += 3)
    {
        memset(au8Data, i, sizeof(au8Data));
        KDF_SetInputKey(au8Data, sizeof(au8Data));
        KDF_SetSalt    (au8Data, sizeof(au8Data));
        KDF_SetLabel   (au8Data, sizeof(au8Data));
        KDF_SetCtxt    (au8Data, sizeof(au8Data));
    }
}

void Func_DeriveKey_Test(void)
{
    int32_t i, j;
    
    // KDF_KEYIN_FROM_NVM need to read key from KS
    CU_ASSERT(KS_Open() == 0);
    
    for (i = 1; i < sizeof(g_au8Keyout); i += 127)
    {
        memset(g_au8Keyout, 0, sizeof(g_au8Keyout));
        CU_ASSERT_FATAL(KDF_DeriveKey(eKDF_MODE_HKDF, KDF_KEYIN_FROM_REG | KDF_SALT_FROM_REG | KDF_LABEL_FROM_REG | KDF_CTXT_FROM_REG, i * 8, g_au8Keyout) == 0);
        
#if 0
        for (j = 0; j < i; j++)
        {
            if (j % 32 == 0)
                printf("\n\n");
            printf("%02X ", g_au8Keyout[j]);
        }
#endif
    }
    
    for (i = 1; i < sizeof(g_au8Keyout); i += 127)
    {
        memset(g_au8Keyout, 0, sizeof(g_au8Keyout));
        CU_ASSERT_FATAL(KDF_DeriveKey(eKDF_MODE_HKDF, KDF_KEYIN_FROM_REG | KDF_SALT_FROM_RANDOM | KDF_LABEL_FROM_RANDOM | KDF_CTXT_FROM_RANDOM, i * 8, g_au8Keyout) == 0);
#if 0
        for (j = 0; j < i; j++)
        {
            if (j % 32 == 0)
                printf("\n\n");
            printf("%02X ", g_au8Keyout[j]);
        }
#endif
    }
}

void Macro1_Test(void)
{
    //CU_FAIL("[Macro1_Test] Failed !\n");
    CU_PASS();
}

void Macro2_Test(void)
{
    CU_PASS();
}

void Const_Test(void)
{
    int32_t  i;
    uint32_t au32KeySize[] = { 
        KDF_KEY_SIZE_128, KDF_KEY_SIZE_163, KDF_KEY_SIZE_192, KDF_KEY_SIZE_224,
        KDF_KEY_SIZE_233, KDF_KEY_SIZE_255, KDF_KEY_SIZE_256, KDF_KEY_SIZE_283,
        KDF_KEY_SIZE_384, KDF_KEY_SIZE_409, KDF_KEY_SIZE_512, KDF_KEY_SIZE_521,
        KDF_KEY_SIZE_571
    };
    
    for (i = 0; i < (sizeof(au32KeySize) / sizeof(au32KeySize[0])); i++)
    {
        KDF_SET_KS_KEYSIZE(au32KeySize[i]);
        CU_ASSERT((KDF->KSSIZE & KDF_KSSIZE_SIZE_Msk) == au32KeySize[i])
    }
}

CU_SuiteInfo KDF_Suites[] = {
    { "Template Const Test", KDF_Test_Init, KDF_Test_Clean, NULL, NULL, KDF_ConstTest },
    { "Template Macro Test", KDF_Test_Init, KDF_Test_Clean, NULL, NULL, KDF_MacroTest },
    { "Template Func  Test", KDF_Test_Init, KDF_Test_Clean, NULL, NULL, KDF_FuncTest },
    CU_SUITE_INFO_NULL
};

CU_TestInfo  KDF_ConstTest[] = {
    { "Const",  Const_Test },
    CU_TEST_INFO_NULL
};

CU_TestInfo  KDF_MacroTest[] = {
    { "Macro1",  Macro1_Test },
    { "Macro2",  Macro2_Test },
    CU_TEST_INFO_NULL
};

CU_TestInfo  KDF_FuncTest[] = {
    { "Func", Func_Test },
    { "DeriveKey", Func_DeriveKey_Test },
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/