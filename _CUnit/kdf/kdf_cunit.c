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
#include "kdf_test_pattern.h"

#define NVT_DEBUG_MSG_OFF
#define NVT_TEST_KEYIN_NVM
#define NVT_BIT_PER_WORD    32
#define NVT_BITCNT_TO_WORDCNT(u32BitCnt)     (((u32BitCnt) / NVT_BIT_PER_WORD) + ((u32BitCnt % NVT_BIT_PER_WORD) > 0))

static uint32_t g_au32KeySize[] = {
    KDF_KS_KEY_SIZE_128, KDF_KS_KEY_SIZE_163, KDF_KS_KEY_SIZE_192, KDF_KS_KEY_SIZE_224,
    KDF_KS_KEY_SIZE_233, KDF_KS_KEY_SIZE_255, KDF_KS_KEY_SIZE_256, KDF_KS_KEY_SIZE_283,
    KDF_KS_KEY_SIZE_384, KDF_KS_KEY_SIZE_409, KDF_KS_KEY_SIZE_512, KDF_KS_KEY_SIZE_521,
    KDF_KS_KEY_SIZE_571
};
static uint32_t  s_au32Keyout[0xFF00 / NVT_BIT_PER_WORD];
static char     s_strKeyout[256];
static char     hex_char_tbl[] = "0123456789abcdef";

static char  ch2hex(char ch)
{
    if(ch <= '9')
    {
        return ch - '0';
    }
    else if((ch <= 'z') && (ch >= 'a'))
    {
        return ch - 'a' + 10U;
    }
    else
    {
        return ch - 'A' + 10U;
    }
}

/**
  * @brief  Extract specified nibble from an unsigned word in character format.
  *         For example:
  *                Suppose val32 is 0x786543210, get_Nth_nibble_char(val32, 3) will return a '3'.
  * @param[in]  val32   The strInput unsigned word
  * @param[in]  idx     The Nth nibble to be extracted.
  * @return  The nibble in character format.
  */
static char get_Nth_nibble_char(uint32_t val32, uint32_t idx)
{
    return hex_char_tbl[(val32 >> (idx * 4U)) & 0xF];
}

void Hex2Reg_Order(char strInput[], uint32_t volatile u32Reg[])
{
    char      hex;
    int       si, ri;
    uint32_t  i, val32;

    si = 0;
    ri = 0;

    while(si < (int)strlen(strInput))
    {
        val32 = 0UL;
        for(i = 0UL; (i < 8UL) && (si < (int)strlen(strInput)); i++)
        {
            hex = ch2hex(strInput[si]);
            val32 |= (uint32_t)hex << ((7 - i) * 4UL);
            si++;
        }
        u32Reg[ri++] = val32;
    }
}

void Reg2Hex_Order(int32_t i32Count, uint32_t volatile u32Reg[], char strOutput[])
{
    int32_t    idx, ri;
    uint32_t   i;

    strOutput[i32Count] = 0U;
    idx = 0;

    for(ri = 0; idx < i32Count; ri++)
    {
        for(i = 0UL; (i < 8UL) && (idx < i32Count); i++)
        {
            strOutput[idx] = get_Nth_nibble_char(u32Reg[ri], 7 - i);
            idx++;
        }
    }
}

int KDF_Test_Init(void)
{
    return 0;
}
int KDF_Test_Clean(void)
{
    return 0;
}

void Func_SetDataTest(void)
{
    int32_t i, j;
    uint8_t au8Data[40];

    for (i = 1; i < sizeof(au8Data); i++)
    {
        for (j = 0; j < sizeof(au8Data); j++)
        {
            au8Data[j] = i + j;
        }

        KDF_SetKeyInput(au8Data, i);
        KDF_SetSalt    (au8Data, i);
        KDF_SetLabel   (au8Data, i);
        KDF_SetContext (au8Data, i);
    }
}

void Func_VerifyDeriveKey_Test(void)
{
    int32_t  i, j;
    uint8_t  au8Data[40];
    uint32_t u32ByteCnt;

    for (i = 0; i < gc_u32HKDF_TestPatternCnt; i++)
    {
        Hex2Reg_Order((char *)gc_sHKDF_TestPattern[i].strIKM, (uint32_t *)au8Data);
        KDF_SetKeyInput(au8Data, strlen(gc_sHKDF_TestPattern[i].strIKM) / 2);

        Hex2Reg_Order((char *)gc_sHKDF_TestPattern[i].strLabel, (uint32_t *)au8Data);
        KDF_SetLabel(au8Data, strlen(gc_sHKDF_TestPattern[i].strLabel) / 2);

        Hex2Reg_Order((char *)gc_sHKDF_TestPattern[i].strSalt, (uint32_t *)au8Data);
        KDF_SetSalt(au8Data, strlen(gc_sHKDF_TestPattern[i].strSalt) / 2);

        Hex2Reg_Order((char *)gc_sHKDF_TestPattern[i].strContext, (uint32_t *)au8Data);
        KDF_SetContext(au8Data, strlen(gc_sHKDF_TestPattern[i].strContext) / 2);

        u32ByteCnt = gc_sHKDF_TestPattern[i].u32KeyByteSize;
        memset(s_au32Keyout, 0, sizeof(s_au32Keyout));
        CU_ASSERT_FATAL(KDF_DeriveKey(eKDF_MODE_HKDF, KDF_KEYIN_FROM_REG | KDF_SALT_FROM_REG | KDF_LABEL_FROM_REG | KDF_CONTEXT_FROM_REG, gc_sHKDF_TestPattern[i].u32KeyBitSize, s_au32Keyout) == 0);
        Reg2Hex_Order(u32ByteCnt * 2, (uint32_t *)s_au32Keyout, s_strKeyout);
        CU_ASSERT(strcmp(s_strKeyout, gc_sHKDF_TestPattern[i].strOKM) == 0);

        if (strcmp(s_strKeyout, gc_sHKDF_TestPattern[i].strOKM) != 0)
        {
            printf("HKDF [%d] s_strKeyout: %s\n", i, s_strKeyout);
#ifndef NVT_DEBUG_MSG_OFF
            printf("s_au32Keyout:\n");
            for (j = 0; j < u32ByteCnt; j++)
            {
                if (j % 32 == 0)
                    printf("\n\n");
                printf("%02X ", s_au32Keyout[j]);
            }
#endif
        }
    }

    for (i = 0; i < gc_u32KBKDF_TestPatternCnt; i++)
    {
        Hex2Reg_Order((char *)gc_sKBKDF_TestPattern[i].strIKM, (uint32_t *)au8Data);
        KDF_SetKeyInput(au8Data, strlen(gc_sKBKDF_TestPattern[i].strIKM) * 2);

        Hex2Reg_Order((char *)gc_sKBKDF_TestPattern[i].strLabel, (uint32_t *)au8Data);
        KDF_SetLabel(au8Data, strlen(gc_sKBKDF_TestPattern[i].strLabel) * 2);

        Hex2Reg_Order((char *)gc_sKBKDF_TestPattern[i].strContext, (uint32_t *)au8Data);
        KDF_SetContext(au8Data, strlen(gc_sKBKDF_TestPattern[i].strContext) * 2);

        u32ByteCnt = gc_sKBKDF_TestPattern[i].u32KeyByteSize;
        memset(s_au32Keyout, 0, sizeof(s_au32Keyout));
        CU_ASSERT_FATAL(KDF_DeriveKey(eKDF_MODE_KBKDF, KDF_KEYIN_FROM_REG | KDF_SALT_FROM_REG | KDF_LABEL_FROM_REG | KDF_CONTEXT_FROM_REG, gc_sKBKDF_TestPattern[i].u32KeyBitSize, s_au32Keyout) == 0);
        Reg2Hex_Order(u32ByteCnt * 2, (uint32_t *)s_au32Keyout, s_strKeyout);
        CU_ASSERT(strcmp(s_strKeyout, gc_sKBKDF_TestPattern[i].strOKM) == 0);

        if (strcmp(s_strKeyout, gc_sKBKDF_TestPattern[i].strOKM) != 0)
        {
            printf("KBKDF [%d] s_strKeyout: %s\n", i, s_strKeyout);
#ifndef NVT_DEBUG_MSG_OFF
            printf("s_au32Keyout:\n");
            for (j = 0; j < u32ByteCnt; j++)
            {
                if (j % 32 == 0)
                    printf("\n\n");
                printf("%02X ", s_au32Keyout[j]);
            }
#endif
        }
    }
}

void Func_DeriveKey_Test(void)
{
    int32_t  i;
    uint32_t u32TestBitSize, u32ByteCnt, u32TestIdx;
    uint32_t au32TestDeriveKeyParam[] = {
        KDF_KEYIN_FROM_REG    | KDF_SALT_FROM_REG    | KDF_LABEL_FROM_REG    | KDF_CONTEXT_FROM_REG,
        KDF_KEYIN_FROM_REG    | KDF_SALT_FROM_REG    | KDF_LABEL_FROM_REG    | KDF_CONTEXT_FROM_RANDOM,
        KDF_KEYIN_FROM_REG    | KDF_SALT_FROM_REG    | KDF_LABEL_FROM_RANDOM | KDF_CONTEXT_FROM_REG,
        KDF_KEYIN_FROM_REG    | KDF_SALT_FROM_REG    | KDF_LABEL_FROM_RANDOM | KDF_CONTEXT_FROM_RANDOM,
        KDF_KEYIN_FROM_REG    | KDF_SALT_FROM_RANDOM | KDF_LABEL_FROM_REG    | KDF_CONTEXT_FROM_REG,
        KDF_KEYIN_FROM_REG    | KDF_SALT_FROM_RANDOM | KDF_LABEL_FROM_REG    | KDF_CONTEXT_FROM_RANDOM,
        KDF_KEYIN_FROM_REG    | KDF_SALT_FROM_RANDOM | KDF_LABEL_FROM_RANDOM | KDF_CONTEXT_FROM_REG,
        KDF_KEYIN_FROM_REG    | KDF_SALT_FROM_RANDOM | KDF_LABEL_FROM_RANDOM | KDF_CONTEXT_FROM_RANDOM,
#ifdef NVT_TEST_KEYIN_NVM
        KDF_KEYIN_FROM_NVM | KDF_SALT_FROM_REG    | KDF_LABEL_FROM_REG    | KDF_CONTEXT_FROM_REG,
        KDF_KEYIN_FROM_NVM | KDF_SALT_FROM_REG    | KDF_LABEL_FROM_REG    | KDF_CONTEXT_FROM_RANDOM,
        KDF_KEYIN_FROM_NVM | KDF_SALT_FROM_REG    | KDF_LABEL_FROM_RANDOM | KDF_CONTEXT_FROM_REG,
        KDF_KEYIN_FROM_NVM | KDF_SALT_FROM_REG    | KDF_LABEL_FROM_RANDOM | KDF_CONTEXT_FROM_RANDOM,
        KDF_KEYIN_FROM_NVM | KDF_SALT_FROM_RANDOM | KDF_LABEL_FROM_REG    | KDF_CONTEXT_FROM_REG,
        KDF_KEYIN_FROM_NVM | KDF_SALT_FROM_RANDOM | KDF_LABEL_FROM_REG    | KDF_CONTEXT_FROM_RANDOM,
        KDF_KEYIN_FROM_NVM | KDF_SALT_FROM_RANDOM | KDF_LABEL_FROM_RANDOM | KDF_CONTEXT_FROM_REG,
        KDF_KEYIN_FROM_NVM | KDF_SALT_FROM_RANDOM | KDF_LABEL_FROM_RANDOM | KDF_CONTEXT_FROM_RANDOM,
#endif
    };
    // KDF_KEYIN_FROM_NVM need to read key from KS
    CU_ASSERT(KS_Open() == 0);

#ifdef NVT_TEST_KEYIN_NVM
    // Write KS OTP Key15
    if ((KS->OTPSTS & BIT15) == 0)
        CU_ASSERT(KS_WriteOTP(15, KS_META_256, s_au32Keyout) == 15);
#endif

    for (u32TestBitSize = 1, u32TestIdx = 0; u32TestBitSize < sizeof(s_au32Keyout); u32TestBitSize += 200, u32TestIdx++)
    {
        u32ByteCnt = (u32TestBitSize / 8) + ((u32TestBitSize % 8) > 0);
        memset(s_au32Keyout, 0, sizeof(s_au32Keyout));
        CU_ASSERT_FATAL(KDF_DeriveKey(eKDF_MODE_HKDF, au32TestDeriveKeyParam[u32TestIdx % 16], u32TestBitSize, s_au32Keyout) == 0);

#ifndef NVT_DEBUG_MSG_OFF
        for (i = 0; i < u32ByteCnt; i++)
        {
            if (i % 32 == 0)
                printf("\n\n");
            printf("%02X ", s_au32Keyout[i]);
        }
#endif
        memset(s_au32Keyout, 0, sizeof(s_au32Keyout));
        CU_ASSERT_FATAL(KDF_DeriveKey(eKDF_MODE_HKDF, au32TestDeriveKeyParam[u32TestIdx % 16], u32TestBitSize, s_au32Keyout) == 0);

#ifndef NVT_DEBUG_MSG_OFF
        for (i = 0; i < u32ByteCnt; i++)
        {
            if (i % 32 == 0)
                printf("\n\n");
            printf("%02X ", s_au32Keyout[i]);
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
            u32DeriveKeyParam = KDF_KEYIN_FROM_REG | KDF_SALT_FROM_REG | KDF_LABEL_FROM_REG | KDF_CONTEXT_FROM_REG;

            for (k = 0; k < (sizeof(au32KeyOwner) / sizeof(au32KeyOwner[0])); k++)
            {
                u32KeyMeta = au32KeyOwner[k] | au32SecurePriv[(i + j + k) % (sizeof(au32SecurePriv) / sizeof(au32SecurePriv[0]))];

                if (KS_GetRemainKeyCount(au32MemType[i]) == 0)
                    CU_ASSERT_FATAL(KS_EraseAll(au32MemType[i]) == 0);

                CU_ASSERT_FATAL((i32KeyIdx = KDF_DeriveKeyToKS(au32MemType[i], eKDF_MODE_KBKDF, u32DeriveKeyParam, g_au32KeySize[j], u32KeyMeta)) >= 0);

                if ((u32KeyMeta & KDF_KSCTL_OWNER_Msk) == KDF_KS_OWNER_CPU)
                {
                    CU_ASSERT_FATAL(KS_Read(au32MemType[i], i32KeyIdx, au32ReadKey, NVT_BITCNT_TO_WORDCNT(KDF_GetKeyBitSize(g_au32KeySize[j]))) == 0);
                }
                else
                {
                    CU_ASSERT_FATAL(KS_Read(au32MemType[i], i32KeyIdx, au32ReadKey, NVT_BITCNT_TO_WORDCNT(KDF_GetKeyBitSize(g_au32KeySize[j]))) != 0);
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
    KDF->CTL = (KDF->CTL & ~KDF_CTL_KEYINSEL_Msk) | KDF_KEYIN_FROM_NVM;
    CU_ASSERT((KDF->CTL & KDF_CTL_KEYINSEL_Msk) ==  KDF_KEYIN_FROM_NVM);

    KDF->CTL = (KDF->CTL & ~KDF_CTL_SALTSEL_Msk) | KDF_SALT_FROM_REG;
    CU_ASSERT((KDF->CTL & KDF_CTL_SALTSEL_Msk) ==  KDF_SALT_FROM_REG);
    KDF->CTL = (KDF->CTL & ~KDF_CTL_SALTSEL_Msk) | KDF_SALT_FROM_RANDOM;
    CU_ASSERT((KDF->CTL & KDF_CTL_SALTSEL_Msk) ==  KDF_SALT_FROM_RANDOM);

    KDF->CTL = (KDF->CTL & ~KDF_CTL_LBSEL_Msk) | KDF_LABEL_FROM_REG;
    CU_ASSERT((KDF->CTL & KDF_CTL_LBSEL_Msk) ==  KDF_LABEL_FROM_REG);
    KDF->CTL = (KDF->CTL & ~KDF_CTL_LBSEL_Msk) | KDF_LABEL_FROM_RANDOM;
    CU_ASSERT((KDF->CTL & KDF_CTL_LBSEL_Msk) ==  KDF_LABEL_FROM_RANDOM);

    KDF->CTL = (KDF->CTL & ~KDF_CTL_CTXTSEL_Msk) | KDF_CONTEXT_FROM_REG;
    CU_ASSERT((KDF->CTL & KDF_CTL_CTXTSEL_Msk) ==  KDF_CONTEXT_FROM_REG);
    KDF->CTL = (KDF->CTL & ~KDF_CTL_CTXTSEL_Msk) | KDF_CONTEXT_FROM_RANDOM;
    CU_ASSERT((KDF->CTL & KDF_CTL_CTXTSEL_Msk) ==  KDF_CONTEXT_FROM_RANDOM);
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
    { "Set Data",           Func_SetDataTest },
    { "Derive Key",         Func_DeriveKey_Test },
    { "Vefify Derive Key",  Func_VerifyDeriveKey_Test },
    { "Derive Key to KS",   Func_DeriveKeyToKS_Test },
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/