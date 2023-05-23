/**************************************************************************//**
 * @file     kdf.c
 * @version  V1.00
 * @brief    Key Derivation Function driver source file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include <string.h>
#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
    @{
*/

/** @addtogroup KDF_Driver Key Derivation Function Driver
    @{
*/

/** @addtogroup KDF_EXPORTED_FUNCTIONS Key Derivation Function Exported Functions
    @{
*/

/**
  * @brief      Get key size selection
  * @return     Others   Successful
  *             < 0      Fail
  * @details    This function is used to get key size selection.
  */
int32_t KDF_GetKeySizeSel(uint32_t u32KeyBitSize)
{
    int32_t  i;
    uint32_t au32KeyLenTbl[] = { 128, 163, 192, 224, 233, 255, 256, 283, 384, 409, 512, 521, 571 };
    
    for(i = 0; i < sizeof(au32KeyLenTbl) / sizeof(au32KeyLenTbl[0]); i++)
        if(u32KeyBitSize == au32KeyLenTbl[i])
            return i;
    
    return -1;
}

/**
 * @brief        Derive key
 *
 * @param[in]    eMode            The pointer of the specified UART module.
 *                                - \ref eKDF_MODE_HKDF : HKDF  (RFC 5869)
 *                                - \ref eKDF_MODE_KBKDF: KBKDF (NIST SP 800-108)
 * @param[in]    u32KeyBitSize    Total output key bit length
 * @param[in]    u32SrcSelect
 * @param[in]    bNext
 *
 * @return       None
 *
 * @details      Derive key output to register
 */
int32_t KDF_DeriveKeyToReg(E_KDF_MODE eMode, uint32_t u32KeyBitSize, uint32_t u32SrcSelect, uint32_t bNext)
{
    uint32_t u32TimeOutCount;
    
    if (bNext)
    {
        if ((KDF->STS & KDF_STS_BUSY_Msk) == 0)
            return KDF_ERR_PARAMETER;
        
        KDF->CTL |= KDF_CTL_NEXT_Msk;
    }
    else
    {
        KDF->CTL   = eMode | u32SrcSelect;
        KDF->KLEN  = u32KeyBitSize;
        KDF->KSCTL = KDF->KSCTL & ~KDF_KSCTL_WDST_Msk;
        KDF->CTL   = KDF->CTL | KDF_CTL_START_Msk;
    }
    
    /* Waiting for busy */
    u32TimeOutCount = KDF_TIMEOUT;
    while(KDF->STS & KDF_STS_HMACBUSY_Msk)
    {
        if(--u32TimeOutCount == 0)
        {
            return KDF_ERR_TIMEOUT;
        }
    }
    
    if(KDF->STS & (KDF_STS_NEXTERR_Msk | KDF_STS_KSERR_Msk))
        return KDF_ERR_FAIL;
    
    return 0;
}

int32_t KDF_FinishDeriveKeyToReg(void)
{
    uint32_t u32TimeOutCount;
    
    if (KDF->STS == 0)
        return 0;
    
    KDF->CTL |= KDF_CTL_FINISH_Msk;
    /* Waiting for busy */
    u32TimeOutCount = KDF_TIMEOUT;
    while(KDF->STS & (KDF_STS_BUSY_Msk | KDF_STS_HMACBUSY_Msk))
    {
        if(--u32TimeOutCount == 0)
        {
            return KDF_ERR_TIMEOUT;
        }
    }
    
    return 0;
}

void KDF_SetInputKey(uint8_t pu8InputKey[], uint32_t u32ByteCnt)
{
    if (u32ByteCnt > (32 * 8))
        memcpy((void *)KDF->KEYIN, (void *)pu8InputKey, (32 * 8));
    else
        memcpy((void *)KDF->KEYIN, (void *)pu8InputKey, u32ByteCnt);
}

void KDF_SetSalt(uint8_t pu8Salt[], uint32_t u32ByteCnt)
{
    if (u32ByteCnt > (32 * 8))
        memcpy((void *)KDF->SALT, (void *)pu8Salt, (32 * 8));
    else
        memcpy((void *)KDF->SALT, (void *)pu8Salt, u32ByteCnt);
}

void KDF_SetLabel(uint8_t pu8Label[], uint32_t u32ByteCnt)
{
    if (u32ByteCnt > (32 * 3))
        memcpy((void *)KDF->LABEL, (void *)pu8Label, (32 * 3));
    else
        memcpy((void *)KDF->LABEL, (void *)pu8Label, u32ByteCnt);
}

void KDF_SetCtxt(uint8_t pu8Ctxt[], uint32_t u32ByteCnt)
{
    if (u32ByteCnt > (32 * 4))
        memcpy((void *)KDF->CTXT, (void *)pu8Ctxt, (32 * 4));
    else
        memcpy((void *)KDF->CTXT, (void *)pu8Ctxt, u32ByteCnt);
}

/**
 * @brief        Derive key
 *
 * @param[in]    eMode          The pointer of the specified UART module.
 *                              - \ref eKDF_MODE_HKDF : HKDF  (RFC 5869)
 *                              - \ref eKDF_MODE_KBKDF: KBKDF (NIST SP 800-108)
 * @param[in]    u32KeyParam    To specify keyin, salt, label and context selection.
 *                              - KDF_KEYIN_FROM_REG or KDF_KEYIN_FROM_NVM
 *                              - KDF_SALT_FROM_REG or KDF_SALT_FROM_RANDOM
 *                              - KDF_LABEL_FROM_REG or KDF_LABEL_FROM_RANDOM
 *                              - KDF_CTXT_FROM_REG or KDF_CTXT_FROM_RANDOM
 *                              If KDF_xxx_FROM_REG is set, users can use these functions - 
 *                                KDF_SetInputKey, KDF_SetSalt, KDF_SetLabel and KDF_SetCtxt
 *                              to write key parameters before deriving key
 * @param[in]    u32KeyBitSize  To specify output key bit length
 * @param[out]   pu8KeyOut
 *
 * @return       0: Success
 *               Others: Fail
 * @details      Derive key with specified key parameters and key length
 */
int32_t KDF_DeriveKey(E_KDF_MODE eMode, uint32_t u32KeyParam, uint32_t u32KeyBitSize, uint8_t *pu8KeyOut)
{
    int32_t  i, j;
    uint32_t u32Idx;
    int32_t  i32LeftKeyBitSize = u32KeyBitSize;
    uint32_t u32TimeOutCount, u32ByteCnt, u32WordCnt;
    
    KDF->CTL   = eMode | u32KeyParam;
    KDF->KLEN  = u32KeyBitSize;
    KDF->KSCTL = KDF->KSCTL & ~KDF_KSCTL_WDST_Msk;
    KDF->CTL   = KDF->CTL | KDF_CTL_START_Msk;

    do
    {
        /* Waiting for busy */
        u32TimeOutCount = KDF_TIMEOUT;
        
        while(KDF->STS & KDF_STS_HMACBUSY_Msk)
        {
            if(--u32TimeOutCount == 0)
            {
                return KDF_ERR_TIMEOUT;
            }
        }
        
        if(KDF->STS & (KDF_STS_NEXTERR_Msk | KDF_STS_KSERR_Msk))
            return KDF_ERR_FAIL;
        
        u32ByteCnt = (i32LeftKeyBitSize / 8) + ((i32LeftKeyBitSize % 8) > 0);
        u32WordCnt = (u32ByteCnt / 4) + ((u32ByteCnt % 4) > 0);
        
        if (u32ByteCnt > 32)
            u32Idx = (u32ByteCnt - 32);
        else
            u32Idx = 0;
        
        u32ByteCnt = u32ByteCnt > 32 ? 32 : u32ByteCnt;
        u32WordCnt = u32WordCnt > 8 ? 8 : u32WordCnt;
        
        for (i = 0; i < 4; i++)
        {
            if ((u32ByteCnt % 4) && (i == (u32ByteCnt % 4)))
                break;
            
            pu8KeyOut[u32Idx + i] = (KDF->KEYOUT[u32WordCnt - 1 - (i / 4)] >> ((3 - (i % 4)) * 8)) & 0xFF;
        }
        
        u32Idx += i;
        
        for (j = 0; j < u32ByteCnt - i; j++)
        {
            pu8KeyOut[u32Idx + j] = (KDF->KEYOUT[u32WordCnt - 2 - (j / 4)] >> ((3 - (j % 4)) * 8)) & 0xFF;
        }
        
        i32LeftKeyBitSize -= (u32ByteCnt * 8);
        KDF->CTL |= KDF_CTL_NEXT_Msk;
    } while (i32LeftKeyBitSize > 0);
    
    KDF->CTL |= KDF_CTL_FINISH_Msk;
    /* Waiting for busy */
    u32TimeOutCount = KDF_TIMEOUT;
    while(KDF->STS & (KDF_STS_BUSY_Msk | KDF_STS_HMACBUSY_Msk))
    {
        if(--u32TimeOutCount == 0)
        {
            return KDF_ERR_TIMEOUT;
        }
    }
    
    return 0;
}

int32_t KDF_DeriveKeyToKS(KS_MEM_Type eMemType, E_KDF_MODE eMode, uint32_t u32KeyBitSize, uint32_t u32SrcSelect, uint32_t u32KeyOwner, uint32_t u32SecurePriv)
{
    uint32_t u32TimeOutCount;
    
    KDF->CTL    = eMode | u32SrcSelect;
    KDF->KSCTL  = (u32KeyOwner | (eMemType << KDF_KSCTL_WSDST_Pos) | KDF_KEYOUT_TO_KS | u32SecurePriv);
    KDF->KSSIZE = KDF_GetKeySizeSel(u32KeyBitSize);
    KDF->CTL    = KDF->CTL | KDF_CTL_START_Msk;
    
    /* Waiting for busy */
    u32TimeOutCount = KDF_TIMEOUT;
    
    while(KDF->STS & (KDF_STS_BUSY_Msk | KDF_STS_HMACBUSY_Msk))
    {
        if(--u32TimeOutCount == 0)
        {
            return KDF_ERR_TIMEOUT;
        }
    }
    
    if(KDF->STS & (KDF_STS_NEXTERR_Msk | KDF_STS_KSERR_Msk))
        return KDF_ERR_FAIL;
    
    return (KDF->KSSTS & KDF_KSSTS_NUM_Msk);
}

/** @} end of group KDF_EXPORTED_FUNCTIONS */
/** @} end of group KDF_Driver */
/** @} end of group Standard_Driver */
