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

#define NVT_DEBUG_MSG_OFF
#define NVT_BIT2WORD_CNT(u32BitCnt)     ((u32BitCnt) / 32 + ((u32BitCnt % 32) > 0))

uint8_t  g_au8Keyout[0xFF00 / 8];
uint32_t g_au32KeySize[] = {
//    KDF_KEY_SIZE_128, KDF_KEY_SIZE_163, KDF_KEY_SIZE_192, KDF_KEY_SIZE_224,
//    KDF_KEY_SIZE_233, KDF_KEY_SIZE_255, KDF_KEY_SIZE_256, KDF_KEY_SIZE_283,
//    KDF_KEY_SIZE_384, KDF_KEY_SIZE_409, KDF_KEY_SIZE_512, KDF_KEY_SIZE_521,
//    KDF_KEY_SIZE_571
    KDF_KEY_SIZE_128, KDF_KEY_SIZE_163, KDF_KEY_SIZE_192
};

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
        KDF_SetKeyInput(au8Data, sizeof(au8Data));
        KDF_SetSalt    (au8Data, sizeof(au8Data));
        KDF_SetLabel   (au8Data, sizeof(au8Data));
        KDF_SetCtxt    (au8Data, sizeof(au8Data));
    }
}

void Func_DeriveKey_Test(void)
{
    int32_t  i, j;
    uint32_t u32ByteCnt;
    // KDF_KEYIN_FROM_NVM need to read key from KS
    CU_ASSERT(KS_Open() == 0);

    for (i = 1; i < sizeof(g_au8Keyout); i += 130)
    {
        u32ByteCnt = (i / 8) + ((i % 8) > 0);
        memset(g_au8Keyout, 0, sizeof(g_au8Keyout));
        CU_ASSERT_FATAL(KDF_DeriveKey(eKDF_MODE_HKDF, KDF_KEYIN_FROM_REG | KDF_SALT_FROM_REG | KDF_LABEL_FROM_REG | KDF_CTXT_FROM_REG, i, g_au8Keyout) == 0);

#ifndef NVT_DEBUG_MSG_OFF
        for (j = 0; j < u32ByteCnt; j++)
        {
            if (j % 32 == 0)
                printf("\n\n");
            printf("%02X ", g_au8Keyout[j]);
        }
#endif
    }

    for (i = 1; i < (sizeof(g_au8Keyout) * 8); i += 130)
    {
        u32ByteCnt = (i / 8) + ((i % 8) > 0);
        memset(g_au8Keyout, 0, sizeof(g_au8Keyout));
        CU_ASSERT_FATAL(KDF_DeriveKey(eKDF_MODE_HKDF, KDF_KEYIN_FROM_REG | KDF_SALT_FROM_RANDOM | KDF_LABEL_FROM_RANDOM | KDF_CTXT_FROM_RANDOM, i, g_au8Keyout) == 0);

#ifndef NVT_DEBUG_MSG_OFF
        for (j = 0; j < u32ByteCnt; j++)
        {
            if (j % 32 == 0)
                printf("\n\n");
            printf("%02X ", g_au8Keyout[j]);
        }
#endif
    }
}

void Func_DeriveKeyToKS_Test(void)
{
    int32_t  i, j, k;
    int32_t  i32KeyIdx;
    uint32_t u32DeriveKeyParam, u32KeyMeta;
    uint32_t au32ReadKey[18];
    uint32_t au32MemType[]    = { KS_SRAM, KS_FLASH },
             au32KeyOwner[]   = { KDF_KS_OWNER_AES, KDF_KS_OWNER_HMAC, KDF_KS_OWNER_ECC, KDF_KS_OWNER_CPU, KDF_KS_OWNER_CHACHA },
             au32SecurePriv[] = { (KDF_KS_NON_SECURE | KDF_KS_NON_PRIV), (KDF_KS_NON_SECURE | KDF_KS_PRIV), (KDF_KS_SECURE | KDF_KS_NON_PRIV), (KDF_KS_SECURE | KDF_KS_PRIV) };

    CU_ASSERT(KS_Open() == 0);

    for (i = 0; i < (sizeof(au32MemType) / sizeof(au32MemType[0])); i++)
    {
        for (j = 0; j < (sizeof(g_au32KeySize) / sizeof(g_au32KeySize[0])); j++)
        {
            u32DeriveKeyParam = KDF_KEYIN_FROM_REG | KDF_SALT_FROM_REG | KDF_LABEL_FROM_REG | KDF_CTXT_FROM_REG;

            for (k = 0; k < (sizeof(au32KeyOwner) / sizeof(au32KeyOwner[0])); k++)
            {
                u32KeyMeta = au32KeyOwner[k] | au32SecurePriv[(i + j + k) % (sizeof(au32SecurePriv) / sizeof(au32SecurePriv[0]))];

                if (KS_GetRemainKeyCount(au32MemType[i]) == 0)
                    CU_ASSERT_FATAL(KS_EraseAll(au32MemType[i]) == 0);

                CU_ASSERT_FATAL((i32KeyIdx = KDF_DeriveKeyToKS(au32MemType[i], eKDF_MODE_KBKDF, g_au32KeySize[j], u32DeriveKeyParam, u32KeyMeta)) >= 0);

                if ((u32KeyMeta & KDF_KSCTL_OWNER_Msk) == KDF_KS_OWNER_CPU)
                {
                    CU_ASSERT_FATAL(KS_Read(au32MemType[i], i32KeyIdx, au32ReadKey, NVT_BIT2WORD_CNT(KDF_GetKeyBitSize(g_au32KeySize[j]))) == 0);
                }
                else
                {
                    CU_ASSERT_FATAL(KS_Read(au32MemType[i], i32KeyIdx, au32ReadKey, NVT_BIT2WORD_CNT(KDF_GetKeyBitSize(g_au32KeySize[j]))) != 0);
                }
            }
        }
    }
}

void Const_Test(void)
{
    int32_t  i;

    for (i = 0; i < (sizeof(g_au32KeySize) / sizeof(g_au32KeySize[0])); i++)
    {
        KDF->KSSIZE = g_au32KeySize[i];
        CU_ASSERT((KDF->KSSIZE & KDF_KSSIZE_SIZE_Msk) == g_au32KeySize[i])
    }

    KDF->CTL = (KDF->CTL & ~KDF_CTL_KEYINSEL_Msk) | KDF_KEYIN_FROM_REG;
    CU_ASSERT((KDF->CTL & KDF_CTL_KEYINSEL_Msk) ==  KDF_KEYIN_FROM_REG);
    KDF->CTL = (KDF->CTL & ~KDF_CTL_KEYINSEL_Msk) | KDF_KEYIN_FROM_KS_OTP;
    CU_ASSERT((KDF->CTL & KDF_CTL_KEYINSEL_Msk) ==  KDF_KEYIN_FROM_KS_OTP);

    KDF->CTL = (KDF->CTL & ~KDF_CTL_SALTSEL_Msk) | KDF_SALT_FROM_REG;
    CU_ASSERT((KDF->CTL & KDF_CTL_SALTSEL_Msk) ==  KDF_SALT_FROM_REG);
    KDF->CTL = (KDF->CTL & ~KDF_CTL_SALTSEL_Msk) | KDF_SALT_FROM_RANDOM;
    CU_ASSERT((KDF->CTL & KDF_CTL_SALTSEL_Msk) ==  KDF_SALT_FROM_RANDOM);

    KDF->CTL = (KDF->CTL & ~KDF_CTL_LBSEL_Msk) | KDF_LABEL_FROM_REG;
    CU_ASSERT((KDF->CTL & KDF_CTL_LBSEL_Msk) ==  KDF_LABEL_FROM_REG);
    KDF->CTL = (KDF->CTL & ~KDF_CTL_LBSEL_Msk) | KDF_LABEL_FROM_RANDOM;
    CU_ASSERT((KDF->CTL & KDF_CTL_LBSEL_Msk) ==  KDF_LABEL_FROM_RANDOM);

    KDF->CTL = (KDF->CTL & ~KDF_CTL_CTXTSEL_Msk) | KDF_CTXT_FROM_REG;
    CU_ASSERT((KDF->CTL & KDF_CTL_CTXTSEL_Msk) ==  KDF_CTXT_FROM_REG);
    KDF->CTL = (KDF->CTL & ~KDF_CTL_CTXTSEL_Msk) | KDF_CTXT_FROM_RANDOM;
    CU_ASSERT((KDF->CTL & KDF_CTL_CTXTSEL_Msk) ==  KDF_CTXT_FROM_RANDOM);
}

CU_SuiteInfo KDF_Suites[] = {
    { "KDF Const Test", KDF_Test_Init, KDF_Test_Clean, NULL, NULL, KDF_ConstTest },
    { "KDF Func  Test", KDF_Test_Init, KDF_Test_Clean, NULL, NULL, KDF_FuncTest },
    CU_SUITE_INFO_NULL
};

CU_TestInfo  KDF_ConstTest[] = {
    { "Const",  Const_Test },
    CU_TEST_INFO_NULL
};

CU_TestInfo  KDF_FuncTest[] = {
    { "Func",               Func_Test },
    { "Derive Key",         Func_DeriveKey_Test },
    { "Derive Key to KS",   Func_DeriveKeyToKS_Test },
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/