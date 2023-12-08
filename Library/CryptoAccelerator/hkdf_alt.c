/*
 *  HKDF implementation -- RFC 5869
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include "common.h"

#if defined(MBEDTLS_HKDF_C)

#include <string.h>
#include "mbedtls/hkdf.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include <string.h>
#include "NuMicro.h"

/* 
   HKDF is the composition of two functions, 
   HKDF-Extract and HKDF-Expand: 
   HKDF(salt, IKM, info, length) = HKDF-Expand(HKDF-Extract(salt, IKM), info, length)
*/

//Check and Convert ikm_len
static int __nvt_hkdf_KeyBitSizeConv(size_t i_len)
{
    uint32_t au32KeyBitLenTbl[13] = { 128, 163, 192, 224, 233, 255, 256, 283, 384, 409, 512, 521, 571 };
    uint8_t  u8ii;
    for (u8ii=0; u8ii<13; u8ii++)
    {
        if( (uint32_t)(i_len) == au32KeyBitLenTbl[u8ii] )
        {
            return au32KeyBitLenTbl[u8ii];
        }
    }
    return -1;
}

static int __nvt_hkdf(const mbedtls_md_info_t *md, const unsigned char *salt,
                  size_t salt_len, const unsigned char *ikm, size_t ikm_len,
                  const unsigned char *info, size_t info_len,
                  unsigned char *okm, size_t okm_len)
{
    //Use REG to keep HKDF material info.
    uint32_t u32DeriveKeyParam = KDF_KEYIN_FROM_REG | KDF_SALT_FROM_REG | KDF_LABEL_FROM_REG | KDF_CONTEXT_FROM_REG;
    
    uint32_t u32BitCnt = __nvt_hkdf_KeyBitSizeConv(ikm_len);
    
    int i32Ret;
        
    KDF_SetKeyInput(ikm, u32BitCnt);
    
    KDF_SetSalt(salt, (uint32_t)(salt_len));
    
    KDF_SetContext(info, (uint32_t)(info_len));
    
    i32Ret = KDF_DeriveKey(eKDF_MODE_HKDF, u32DeriveKeyParam, (uint32_t)(okm_len), (uint32_t*)(okm));
    
    return i32Ret;
}

int mbedtls_hkdf( const mbedtls_md_info_t *md, const unsigned char *salt,
                  size_t salt_len, const unsigned char *ikm, size_t ikm_len,
                  const unsigned char *info, size_t info_len,
                  unsigned char *okm, size_t okm_len )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    
    ret = __nvt_hkdf( md, salt, salt_len, ikm, ikm_len, info, info_len, okm, okm_len);

    return( ret );
}

int mbedtls_hkdf_extract( const mbedtls_md_info_t *md,
                          const unsigned char *salt, size_t salt_len,
                          const unsigned char *ikm, size_t ikm_len,
                          unsigned char *prk )
{

    return 0;
}

int mbedtls_hkdf_expand( const mbedtls_md_info_t *md, const unsigned char *prk,
                         size_t prk_len, const unsigned char *info,
                         size_t info_len, unsigned char *okm, size_t okm_len )
{

    return 0;
}

#endif /* MBEDTLS_HKDF_C */
