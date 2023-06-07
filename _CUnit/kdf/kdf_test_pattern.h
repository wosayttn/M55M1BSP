/******************************************************************************
* @file    kdf_test_pattern.h
* @version V1.00
* @brief   KDF test pattern header file
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#ifndef __KDF_TEST_PATTERN_H__
#define __KDF_TEST_PATTERN_H__

typedef struct _kdf_test_pattern {
    char        strIKM[65];
    char        strSalt[65];
    char        strLabel[25];
    char        strContext[33];
    char        *strOKM;
    uint32_t    u32KeyByteSize;
    uint32_t    u32KeyBitSize;
} S_KDF_TEST_PATTERN;

#ifdef __cplusplus
extern "C"
{
#endif

extern const S_KDF_TEST_PATTERN gc_sHKDF_TestPattern[];
extern const uint32_t gc_u32HKDF_TestPatternCnt;
extern const S_KDF_TEST_PATTERN gc_sKBKDF_TestPattern[];
extern const uint32_t gc_u32KBKDF_TestPatternCnt;

#ifdef __cplusplus
}
#endif

#endif	// __KDF_TEST_PATTERN_H__
