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

#define KDF_START               0
#define KDF_NEXT                1
#define KDF_KS_OWNER_AES        (0x0 << KDF_KSCTL_OWNER_Pos)
#define KDF_KS_OWNER_HMAC       (0x1 << KDF_KSCTL_OWNER_Pos)
#define KDF_KS_OWNER_ECC        (0x4 << KDF_KSCTL_OWNER_Pos)
#define KDF_KS_OWNER_CPU        (0x5 << KDF_KSCTL_OWNER_Pos)
#define KDF_KS_OWNER_CHACHA     (0x6 << KDF_KSCTL_OWNER_Pos)
#define KDF_KS_DST_SRAM         (0x0 << KDF_KSCTL_WSDST_Pos)
#define KDF_KS_DST_FLASH        (0x1 << KDF_KSCTL_WSDST_Pos)
#define KDF_KEYOUT_TO_REG       (0x0 << KDF_KSCTL_WDST_Pos)
#define KDF_KEYOUT_TO_KS        (0x1 << KDF_KSCTL_WDST_Pos)
#define KDF_KS_NON_PRIV         (0x0 << KDF_KSCTL_PRIV_Pos)
#define KDF_KS_PRIV             (0x1 << KDF_KSCTL_PRIV_Pos)
#define KDF_KS_NON_SECURE       (0x0 << KDF_KSCTL_TRUST_Pos)
#define KDF_KS_SECURE           (0x1 << KDF_KSCTL_TRUST_Pos)
#define KDF_KS_128_BIT          (0x0)
#define KDF_KS_163_BIT          (0x1)
#define KDF_KS_192_BIT          (0x2)
#define KDF_KS_224_BIT          (0x3)
#define KDF_KS_233_BIT          (0x4)
#define KDF_KS_255_BIT          (0x5)
#define KDF_KS_256_BIT          (0x6)
#define KDF_KS_283_BIT          (0x7)
#define KDF_KS_384_BIT          (0x8)
#define KDF_KS_409_BIT          (0x9)
#define KDF_KS_512_BIT          (0xA)
#define KDF_KS_521_BIT          (0xB)
#define KDF_KS_571_BIT          (0xC)

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

#define KDF_KEYIN_FROM_REG          (0 << KDF_CTL_KEYINSEL_Pos)
#define KDF_KEYIN_FROM_NVM          (1 << KDF_CTL_KEYINSEL_Pos)
#define KDF_SALT_FROM_REG           (0 << KDF_CTL_SALTSEL_Pos)
#define KDF_SALT_FROM_RANDOM        (1 << KDF_CTL_SALTSEL_Pos)
#define KDF_LABEL_FROM_REG          (0 << KDF_CTL_LBSEL_Pos)
#define KDF_LABEL_FROM_RANDOM       (1 << KDF_CTL_LBSEL_Pos)
#define KDF_CTXT_FROM_REG           (0 << KDF_CTL_CTXTSEL_Pos)
#define KDF_CTXT_FROM_RANDOM        (1 << KDF_CTL_CTXTSEL_Pos)
#define KDF_MODE_HKDF               (0 << KDF_CTL_MODE_Pos)
#define KDF_MODE_KBKDF              (1 << KDF_CTL_MODE_Pos)

#define KDF_CTL_START()                 (KDF->CTL |=  KDF_CTL_START_Msk)
#define KDF_CTL_NEXT()                  (KDF->CTL |=  KDF_CTL_NEXT_Msk)
#define KDF_CTL_STOP()                  (KDF->CTL |=  KDF_CTL_FINSIH_Msk)

#define KDF_SET_KEY_LENGTH(u32KeyLen)               (KDF->KLEN = u32KeyLen & 0xFF00)
#define KDF_WRITE_KEYINx(u32WordIdx, u32Key)        (KDF->KEYIN[u32WordIdx] = u32Key)
#define KDF_READ_KEYOUTx(u32WordIdx)                (KDF->KEYOUT[u32WordIdx])
#define KDF_WRITE_SALTx(u32WordIdx, u32Salt)        (KDF->SALT[u32WordIdx] = u32Salt)
#define KDF_WRITE_LABELx(u32WordIdx, u32Label)      (KDF->SALT[u32WordIdx] = u32Label)
#define KDF_WRITE_CONTEXTx(u32WordIdx, u32Context)  (KDF->SALT[u32WordIdx] = u32Context)

/** @} end of group KDF_EXPORTED_CONSTANTS */

//extern int32_t g_KDF_i32ErrCode;

/** @addtogroup KDF_EXPORTED_FUNCTIONS Key Store Exported Functions
    @{
*/

int32_t KDF_GetKeySizeSel(uint32_t u32KeyBitSize);
int32_t KDF_DeriveKeyToReg(E_KDF_MODE eMode, uint32_t u32KeyBitSize, uint32_t u32SrcSelect, uint32_t bNext);
int32_t KDF_FinishDeriveKeyToReg(void);
int32_t KDF_DeriveKeyToKS(KS_MEM_Type eMemType, E_KDF_MODE eMode, uint32_t u32KeyBitSize, uint32_t u32SrcSelect, uint32_t u32KeyOwner, uint32_t u32SecurePriv);

/** @} end of group KDF_EXPORTED_FUNCTIONS */
/** @} end of group KDF_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __KDF_H__ */
