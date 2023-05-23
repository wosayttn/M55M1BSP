/**************************************************************************//**
 * @file     kdf.h
 * @version  V1.00
 * @brief    KDF driver header file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#ifndef __KDF_H__
#define __KDF_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
    @{
*/

/** @addtogroup KDF_Driver KDF Driver
    @{
*/

/** @addtogroup KDF_EXPORTED_CONSTANTS KEYSTORE Exported Constants
    @{
*/

typedef enum _E_KDF_MODE
{
    eKDF_MODE_HKDF  = 0,    /*!< HKDF (RFC 5869)            */
    eKDF_MODE_KBKDF,        /*!< KBKDF (NIST SP 800-108)    */
} E_KDF_MODE;

#define KDF_START               0UL
#define KDF_NEXT                1UL
#define KDF_KS_OWNER_AES        (0x0UL << KDF_KSCTL_OWNER_Pos)
#define KDF_KS_OWNER_HMAC       (0x1UL << KDF_KSCTL_OWNER_Pos)
#define KDF_KS_OWNER_ECC        (0x4UL << KDF_KSCTL_OWNER_Pos)
#define KDF_KS_OWNER_CPU        (0x5UL << KDF_KSCTL_OWNER_Pos)
#define KDF_KS_OWNER_CHACHA     (0x6UL << KDF_KSCTL_OWNER_Pos)
#define KDF_KS_DST_SRAM         (0x0UL << KDF_KSCTL_WSDST_Pos)
#define KDF_KS_DST_FLASH        (0x1UL << KDF_KSCTL_WSDST_Pos)
#define KDF_KEYOUT_TO_REG       (0x0UL << KDF_KSCTL_WDST_Pos)
#define KDF_KEYOUT_TO_KS        (0x1UL << KDF_KSCTL_WDST_Pos)
#define KDF_KS_NON_PRIV         (0x0UL << KDF_KSCTL_PRIV_Pos)
#define KDF_KS_PRIV             (0x1UL << KDF_KSCTL_PRIV_Pos)
#define KDF_KS_NON_SECURE       (0x0UL << KDF_KSCTL_TRUST_Pos)
#define KDF_KS_SECURE           (0x1UL << KDF_KSCTL_TRUST_Pos)
#define KDF_KEY_SIZE_128        (0x0UL)
#define KDF_KEY_SIZE_163        (0x1UL)
#define KDF_KEY_SIZE_192        (0x2UL)
#define KDF_KEY_SIZE_224        (0x3UL)
#define KDF_KEY_SIZE_233        (0x4UL)
#define KDF_KEY_SIZE_255        (0x5UL)
#define KDF_KEY_SIZE_256        (0x6UL)
#define KDF_KEY_SIZE_283        (0x7UL)
#define KDF_KEY_SIZE_384        (0x8UL)
#define KDF_KEY_SIZE_409        (0x9UL)
#define KDF_KEY_SIZE_512        (0xAUL)
#define KDF_KEY_SIZE_521        (0xBUL)
#define KDF_KEY_SIZE_571        (0xCUL)

#define KDF_TIMEOUT             SystemCoreClock                     /*!< 1 second time-out \hideinitializer         */
#define KDF_OK                  ( 0L)
#define KDF_ERR_FAIL            (-1L)                               /*!< KDF failed                                  */
#define KDF_ERR_TIMEOUT         (-2L)                               /*!< KDF operation abort due to timeout error    */
#define KDF_ERR_INIT            (-3L)                               /*!< KDF initial failed                          */
#define KDF_ERR_BUSY            (-4L)                               /*!< KDF is in busy state                        */
#define KDF_ERR_PARAMETER       (-5L)                               /*!< Wrong input parameters                     */

/**
  * @brief      Enable scramble function
  * @details    This function is used to enable scramle function of Key Store.
  */

#define KDF_IS_BUSY()               ( KDF->STS & KDF_STS_BUSY_Msk)
#define KDF_IS_HMAC_BUSY()          ((KDF->STS & KDF_STS_HAMCBUSY_Msk) >> KDF_STS_HAMCBUSY_Pos)
#define KDF_IS_ERROR()              ( KDF->STS & (KDF_STS_KSERR_Msk | KDF_STS_NEXTERR_Msk))

#define KDF_KEYIN_FROM_REG          (0UL << KDF_CTL_KEYINSEL_Pos)
#define KDF_KEYIN_FROM_NVM          (1UL << KDF_CTL_KEYINSEL_Pos)
#define KDF_SALT_FROM_REG           (0UL << KDF_CTL_SALTSEL_Pos)
#define KDF_SALT_FROM_RANDOM        (1UL << KDF_CTL_SALTSEL_Pos)
#define KDF_LABEL_FROM_REG          (0UL << KDF_CTL_LBSEL_Pos)
#define KDF_LABEL_FROM_RANDOM       (1UL << KDF_CTL_LBSEL_Pos)
#define KDF_CTXT_FROM_REG           (0UL << KDF_CTL_CTXTSEL_Pos)
#define KDF_CTXT_FROM_RANDOM        (1UL << KDF_CTL_CTXTSEL_Pos)
#define KDF_MODE_HKDF               (0UL << KDF_CTL_MODE_Pos)
#define KDF_MODE_KBKDF              (1UL << KDF_CTL_MODE_Pos)

#define KDF_CTL_START()                 (KDF->CTL |=  KDF_CTL_START_Msk)
#define KDF_CTL_NEXT()                  (KDF->CTL |=  KDF_CTL_NEXT_Msk)
#define KDF_CTL_STOP()                  (KDF->CTL |=  KDF_CTL_FINSIH_Msk)

#define KDF_SET_KEY_LENGTH(u32KeyLen)   (KDF->KLEN = (u32KeyLen) & 0xFF00)
#define KDF_SET_KS_KEYSIZE(u32KeySize)  (KDF->KSSIZE = (u32KeySize) & KDF_KSSIZE_SIZE_Msk)

/** @} end of group KDF_EXPORTED_CONSTANTS */

//extern int32_t g_KDF_i32ErrCode;

/** @addtogroup KDF_EXPORTED_FUNCTIONS Key Store Exported Functions
    @{
*/

void KDF_SetInputKey(uint8_t pu8InputKey[], uint32_t u32ByteCnt);
void KDF_SetSalt(uint8_t pu8Salt[], uint32_t u32ByteCnt);
void KDF_SetLabel(uint8_t pu8Label[], uint32_t u32ByteCnt);
void KDF_SetCtxt(uint8_t pu8Ctxt[], uint32_t u32ByteCnt);
int32_t KDF_GetKeySizeSel(uint32_t u32KeyBitSize);
int32_t KDF_DeriveKeyToReg(E_KDF_MODE eMode, uint32_t u32KeyBitSize, uint32_t u32SrcSelect, uint32_t bNext);
int32_t KDF_FinishDeriveKeyToReg(void);
int32_t KDF_DeriveKey(E_KDF_MODE eMode, uint32_t u32KeyParam, uint32_t u32KeyBitSize, uint8_t *pu8Keyout);
int32_t KDF_DeriveKeyToKS(KS_MEM_Type eMemType, E_KDF_MODE eMode, uint32_t u32KeyBitSize, uint32_t u32SrcSelect, uint32_t u32KeyOwner, uint32_t u32SecurePriv);

/** @} end of group KDF_EXPORTED_FUNCTIONS */
/** @} end of group KDF_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __KDF_H__ */
