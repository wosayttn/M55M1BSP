/******************************************************************************
* @file    ks_cunit.c
* @version V1.00
* @brief   KeyStore CUnit Test
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
#include "ks_cunit.h"

const uint32_t gc_au32KeySize[] =
{
    KS_META_128, KS_META_163, KS_META_192, KS_META_224,
    KS_META_233, KS_META_255, KS_META_256, KS_META_283,
    KS_META_384, KS_META_409, KS_META_512, KS_META_521,
    KS_META_571, KS_META_1024, KS_META_1536, KS_META_2048,
    KS_META_3072, KS_META_4096
};

/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int Tests_Init(void)
{
    CLK_EnableModuleClock(KS0_MODULE);
    return 0;
}
int Tests_Clean(void)
{
    CLK_DisableModuleClock(KS0_MODULE);
    return 0;
}

CU_SuiteInfo KS_Suites[] =
{
    { "Macro Test", Tests_Init, Tests_Clean, NULL, NULL, KS_MacroTest },
    { "Func  Test", Tests_Init, Tests_Clean, NULL, NULL, KS_FuncTest  },
    CU_SUITE_INFO_NULL
};

void Macro_GetVersion(void)
{
    CU_PASS();
}

void Func_Open(void)
{
    CU_ASSERT(KS_Open() == 0);
}

void Func_ReadWriteEraseAll(void)
{
    int32_t i, j;
    uint32_t u32Meta;
    int32_t wlen;
    int32_t i32Key;
    uint32_t au32Key [128] = { 0 };
    uint32_t au32ReadKey[128] = { 0 };
    KS_MEM_Type eMemType;

    /* Erase all before testing */
    CU_ASSERT(KS_EraseAll(KS_SRAM) == 0);
    CU_ASSERT(KS_EraseAll(KS_FLASH) == 0);

    /* Remain key count is 32 */
    CU_ASSERT(KS_GetRemainKeyCount(KS_SRAM) == 32);
    CU_ASSERT(KS_GetRemainKeyCount(KS_FLASH) == 32);

#if 1
    /* SRAM */
    eMemType = KS_SRAM;

    for (i = 0; i < (sizeof(gc_au32KeySize) / sizeof(gc_au32KeySize[0])); i++)
    {
        u32Meta = KS_META_AES | gc_au32KeySize[i] | KS_META_READABLE;
        memset((void *)au32ReadKey, 0, sizeof(au32ReadKey));

        for (j = 0; j < 128; j++)
            au32Key[j] += j + i;

        CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
        CU_ASSERT(KS_GetRemainKeyCount(eMemType) == (KS_MAX_SRAM_KEY_CNT - i - 1));
        wlen = KS_GetKeyWordCnt(u32Meta);

        CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
        CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);
    }

    CU_ASSERT(KS_EraseAll(eMemType) == 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == KS_MAX_SRAM_KEY_CNT);

    /* Flash */
    eMemType = KS_FLASH;

    for (i = 0; i < (sizeof(gc_au32KeySize) / sizeof(gc_au32KeySize[0])); i++)
    {
        u32Meta = KS_META_AES | gc_au32KeySize[i] | KS_META_READABLE;
        memset((void *)au32ReadKey, 0, sizeof(au32ReadKey));

        for (j = 0; j < 128; j++)
            au32Key[j] += j + i;

        CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
        CU_ASSERT(KS_GetRemainKeyCount(eMemType) == (KS_MAX_FLASH_KEY_CNT - i - 1));
        wlen = KS_GetKeyWordCnt(u32Meta);

        CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
        CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);
    }

    CU_ASSERT(KS_EraseAll(eMemType) == 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == KS_MAX_FLASH_KEY_CNT);
#else
    /* SRAM */
    eMemType = KS_SRAM;

    u32Meta = KS_META_AES | KS_META_163 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 31);
    wlen = KS_GetKeyWordCnt(u32Meta);

    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_192 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 30);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_224 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 29);

    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_233 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 28);

    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_255 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 27);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_256 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 26);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_283 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 25);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_384 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 24);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_409 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 23);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_512 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 22);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_521 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 21);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_571 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 20);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    CU_ASSERT(KS_EraseAll(eMemType) == 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 32);

    /* FLASH */
    eMemType = KS_FLASH;

    u32Meta = KS_META_AES | KS_META_163 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 31);

    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_192 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 30);

    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_224 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 29);

    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_233 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 28);

    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_255 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 27);

    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_256 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 26);

    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_283 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 25);

    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_384 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 24);

    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_409 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 23);

    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_512 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 22);

    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_521 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 21);

    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    u32Meta = KS_META_AES | KS_META_571 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 20);

    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    CU_ASSERT(KS_EraseAll(eMemType) == 0);
    CU_ASSERT(KS_GetRemainKeyCount(eMemType) == 32);
#endif

    /* key for other engine test */
    eMemType = KS_SRAM;
    /* KS_META_HMAC */
    u32Meta = KS_META_HMAC | KS_META_ECC | KS_META_CPU | KS_META_571 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    /* KS_META_RSA_EXP */
    u32Meta = KS_META_RSA_EXP | KS_META_1024 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    /* KS_META_RSA_EXP */
    u32Meta = KS_META_RSA_EXP | KS_META_2048 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    /* KS_META_RSA_EXP */
    u32Meta = KS_META_RSA_EXP | KS_META_4096 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    /* Middle data should not be read */
    u32Meta = KS_META_RSA_MID | KS_META_1024 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) < 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) != 0);

    /* KS_META_CHACHA */
    u32Meta = KS_META_CHACHA | KS_META_4096 | KS_META_READABLE;
    memset((void *)au32ReadKey, 0, 128 * 4);

    for (i = 0; i < 128; i++)
        au32Key[i] += i;

    CU_ASSERT((i32Key = KS_Write(eMemType, u32Meta, au32Key)) >= 0);
    wlen = KS_GetKeyWordCnt(u32Meta);
    CU_ASSERT(KS_Read(eMemType, i32Key, au32ReadKey, wlen) == 0);
    CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, wlen * 4) == 0);

    CU_ASSERT(KS_EraseAll(eMemType) == 0);
}

void Func_EraseKey(void)
{
    uint32_t au32KeyIdx[6];
    uint32_t au32Key[8];
    int32_t  i, i32RemainKeyCnt;

    for (i = 0; i < 8; i++)
    {
        au32Key[i] = i + 3;
    }

    CU_ASSERT(KS_EraseAll(KS_SRAM) == 0);

    for (i = 0; i < 6; i++)
    {
        au32KeyIdx[i] = KS_Write(KS_SRAM, KS_META_AES | KS_META_256 | KS_META_READABLE, au32Key);
        CU_ASSERT(au32KeyIdx[i] >= 0);
    }

    for (i = 0; i < 6; i++)
    {
        i32RemainKeyCnt = KS_GetRemainKeyCount(KS_SRAM);
        CU_ASSERT(KS_Read(KS_SRAM, au32KeyIdx[i], au32Key, 8) == 0);
        CU_ASSERT(KS_EraseKey(au32KeyIdx[i]) == 0);
        CU_ASSERT(KS_GetRemainKeyCount(KS_SRAM) == (i32RemainKeyCnt + 1));
        CU_ASSERT(KS_Read(KS_SRAM, au32KeyIdx[i], au32Key, 8) != 0);
    }
}

void Func_RevokeTest(void)
{
    uint32_t au32KeyIdx[6];
    uint32_t au32Key[8];
    int32_t i;

    for (i = 0; i < 8; i++)
    {
        au32Key[i] = i + 3;
    }

    for (i = 0; i < 6; i++)
    {
        au32KeyIdx[i] = KS_Write(KS_SRAM, KS_META_AES | KS_META_256 | KS_META_READABLE, au32Key);
        CU_ASSERT(au32KeyIdx[i] >= 0);
    }

    for (i = 0; i < 6; i++)
    {
        CU_ASSERT(KS_Read(KS_SRAM, au32KeyIdx[i], au32Key, 8) == 0);
    }

    CU_ASSERT(KS_RevokeKey(KS_SRAM, au32KeyIdx[2]) == 0);
    CU_ASSERT(KS_RevokeKey(KS_SRAM, au32KeyIdx[5]) == 0);


    for (i = 0; i < 6; i++)
    {
        if ((i == 2) || (i == 5))
        {
            CU_ASSERT(KS_Read(KS_SRAM, au32KeyIdx[i], au32Key, 8) <= 0);
        }
        else
        {
            CU_ASSERT(KS_Read(KS_SRAM, au32KeyIdx[i], au32Key, 8) == 0);
        }
    }

    for (i = 0; i < 6; i++)
    {
        au32KeyIdx[i] = KS_Write(KS_FLASH, KS_META_AES | KS_META_256 | KS_META_READABLE, au32Key);
        CU_ASSERT(au32KeyIdx[i] >= 0);
    }


    for (i = 0; i < 6; i++)
    {
        CU_ASSERT(KS_Read(KS_FLASH, au32KeyIdx[i], au32Key, 8) == 0);
    }

    CU_ASSERT(KS_RevokeKey(KS_FLASH, au32KeyIdx[2]) == 0);
    CU_ASSERT(KS_RevokeKey(KS_FLASH, au32KeyIdx[5]) == 0);

    for (i = 0; i < 6; i++)
    {
        if ((i == 2) || (i == 5))
        {
            CU_ASSERT(KS_Read(KS_FLASH, au32KeyIdx[i], au32Key, 8) <= 0);
        }
        else
        {
            CU_ASSERT(KS_Read(KS_FLASH, au32KeyIdx[i], au32Key, 8) == 0);
        }
    }


    CU_ASSERT(KS_EraseAll(KS_SRAM) == 0);
    CU_ASSERT(KS_EraseAll(KS_FLASH) == 0);
}

void Func_RemainSizeTest(void)
{
    uint32_t u32Size, u32Size2;
    uint32_t au32KeyIdx;
    uint32_t au32Key[8];

    /* KS_SRAM */
    CU_ASSERT((u32Size   = KS_GetRemainSize(KS_SRAM)) > 0);
    CU_ASSERT((au32KeyIdx = KS_Write(KS_SRAM, KS_META_AES | KS_META_256 | KS_META_READABLE, au32Key)) >= 0);
    CU_ASSERT((u32Size2  = KS_GetRemainSize(KS_SRAM)) < u32Size);
    CU_ASSERT(KS_GetRemainKeyCount(KS_SRAM) < KS_MAX_SRAM_KEY_CNT);

    CU_ASSERT(KS_EraseAll(KS_SRAM) == 0);
    CU_ASSERT(KS_GetRemainSize(KS_SRAM) == KS_MAX_SRAM_SPACE);
    CU_ASSERT(KS_GetRemainKeyCount(KS_SRAM) == KS_MAX_SRAM_KEY_CNT);

    /* KS_FLASH */
    CU_ASSERT((u32Size   = KS_GetRemainSize(KS_FLASH)) > 0);
    CU_ASSERT((au32KeyIdx = KS_Write(KS_FLASH, KS_META_AES | KS_META_256 | KS_META_READABLE, au32Key)) >= 0);
    CU_ASSERT((u32Size2  = KS_GetRemainSize(KS_FLASH)) < u32Size);
    CU_ASSERT(KS_GetRemainKeyCount(KS_FLASH) < KS_MAX_FLASH_KEY_CNT);

    CU_ASSERT(KS_EraseAll(KS_FLASH) == 0);
    CU_ASSERT(KS_GetRemainSize(KS_FLASH) == KS_MAX_FLASH_SPACE)
    CU_ASSERT(KS_GetRemainKeyCount(KS_FLASH) == KS_MAX_FLASH_KEY_CNT);
}

void Func_RemanenceTest(void)
{
    CU_ASSERT(KS_EraseAll(KS_SRAM) == 0);

    //printf("\n\n  KS_TrigReman() = %d\n", KS_TrigReman());
    CU_ASSERT(KS_ToggleSRAM() == 1);
    CU_ASSERT(KS_ToggleSRAM() == 0);
    CU_ASSERT(KS_ToggleSRAM() == 1);
    CU_ASSERT(KS_ToggleSRAM() == 0);
    CU_ASSERT(KS_ToggleSRAM() == 1);
    CU_ASSERT(KS_ToggleSRAM() == 0);
    CU_ASSERT(KS_ToggleSRAM() == 1);
    CU_ASSERT(KS_ToggleSRAM() == 0);
    CU_ASSERT(KS_ToggleSRAM() == 1);
    CU_ASSERT(KS_ToggleSRAM() == 0);
    CU_ASSERT(KS_ToggleSRAM() == 1);
    CU_ASSERT(KS_ToggleSRAM() == 0);
    CU_ASSERT(KS_ToggleSRAM() == 1);
    CU_ASSERT(KS_ToggleSRAM() == 0);
}

void Func_ScrambleTest(void)
{
    int32_t  i, i32KeyIdx;
    uint32_t u32Meta, u32WordCnt;
    uint32_t au32WriteKey[8], au32ScrKey[4], au32ReadKey[8];

    CU_ASSERT(KS_EraseAll(KS_SRAM) == 0);

    for (i = 0; i < KS_MAX_SRAM_KEY_CNT; i++)
    {
        KS_SCRAMBLING();
        memset(au32WriteKey, i, sizeof(au32WriteKey));
        memset(au32ScrKey, (0xA5 + i), sizeof(au32ScrKey));
        memset(au32ReadKey, 0, sizeof(au32ReadKey));
        u32Meta = KS_META_ECC | KS_META_256 | KS_META_READABLE;

        memcpy((void *)KS->SCMBKEY, au32ScrKey, sizeof(au32ScrKey));
        CU_ASSERT((i32KeyIdx = KS_Write(KS_SRAM, u32Meta, au32WriteKey)) >= 0);
        u32WordCnt = KS_GetKeyWordCnt(u32Meta);
        CU_ASSERT(KS_Read(KS_SRAM, i32KeyIdx, au32ReadKey, u32WordCnt) == 0);
        CU_ASSERT(strncmp((void *)au32WriteKey, (void *)au32ReadKey, u32WordCnt * 4) == 0);
    }
}

void Func_OTP_ReadWriteErase(void)
{
    int32_t  i, i32KeyIdx;
    int32_t  i32KeySizeIdx = 0;
    uint32_t u32Meta, u32WordCnt;
    uint32_t au32Key[8], au32ReadKey[8];
    uint32_t bSecure = TRUE,
             bPriv   = TRUE;

    // Skip first two OTP key or chip will enter secure boot check fail loop
    for (i = 2; i <= KS_MAX_OTPKEY_CNT; i++)
    {
        if (KS_GET_OTPKEY_STS(i))
        {
            printf("OTP %d is not empty - Skip\n", i);
            continue ;
        }

        memset(au32Key, i + 0x5A, sizeof(au32Key));
        memset(au32ReadKey, 0, sizeof(au32ReadKey));

        u32Meta = KS_META_CPU | KS_META_READABLE | gc_au32KeySize[i32KeySizeIdx++] | (bSecure ? KS_META_SECURE : KS_META_NONSECURE) | (bPriv ? KS_META_PRIV : KS_META_NONPRIV);
        CU_ASSERT((i32KeyIdx = KS_WriteOTP(i, u32Meta, au32Key)) == i);
        u32WordCnt = KS_GetKeyWordCnt(u32Meta);

        if (i == KS_KDF_ROOT_OTPKEY)
        {
            CU_ASSERT(KS_Read(KS_OTP, i32KeyIdx, au32ReadKey, u32WordCnt) != 0);
        }
        else
        {
            CU_ASSERT(KS_Read(KS_OTP, i32KeyIdx, au32ReadKey, u32WordCnt) == 0);
            CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, u32WordCnt * 4) <= 0);
        }

        CU_ASSERT(KS_GET_OTPKEY_STS(i32KeyIdx) == TRUE);

        if (i % 2 > 0)
        {
            // Test lock OTP
            CU_ASSERT(KS_LockOTPKey(i) == 0);
            CU_ASSERT(KS_EraseOTPKey(i) != 0);


            if (i == KS_KDF_ROOT_OTPKEY)
            {
                CU_ASSERT(KS_Read(KS_OTP, i32KeyIdx, au32ReadKey, u32WordCnt) != 0);
            }
            else
            {
                CU_ASSERT(KS_Read(KS_OTP, i32KeyIdx, au32ReadKey, u32WordCnt) == 0);
                CU_ASSERT(strncmp((void *)au32Key, (void *)au32ReadKey, u32WordCnt * 4) <= 0);
            }
        }
        else
        {
            // Test erase OTP
            CU_ASSERT(KS_EraseOTPKey(i) == 0);
            CU_ASSERT(KS_Read(KS_OTP, i32KeyIdx, au32ReadKey, u32WordCnt) != 0);
        }

        bSecure ^= 1;
        bPriv ^= bSecure;

        if (gc_au32KeySize[i32KeySizeIdx] == KS_META_256)
            i32KeySizeIdx = 0;
    }
}

CU_TestInfo KS_MacroTest[] =
{
    { "Get Fimrware version", Macro_GetVersion },
    CU_TEST_INFO_NULL
};

CU_TestInfo KS_FuncTest[] =
{
    { "Open test",                  Func_Open                },
    { "Read/Write/EraseAll test",   Func_ReadWriteEraseAll   },
    { "Erase key test",             Func_EraseKey            },
    { "Revoke key test",            Func_RevokeTest          },
    { "Remain key size test",       Func_RemainSizeTest      },
    { "Remanence test",             Func_RemanenceTest       },
    { "Scramble test",              Func_ScrambleTest        },
    // Be careful when test in real chip (Need ROMMAP erase to clear OTP key)
    { "OTP Read/Write/Erase test",  Func_OTP_ReadWriteErase  },
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
