/**************************************************************************//**
 * @file     kdf.c
 * @version  V1.00
 * @brief    Key Derivation Function driver source file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

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
  * @brief      Get key store size selection
  * @retval     Others   Successful
  * @retval     < 0      Fail
  * @details    This function is used to get key store size selection.
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
