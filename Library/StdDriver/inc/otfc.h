/**************************************************************************//**
 * @file    otfc.h
 * @version V1.00
 * @brief   OTFC driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#ifndef __OTFC_H__
#define __OTFC_H__

#include <stdint.h>

/*----------------------------------------------------------------------------*/
/* Include related headers                                                    */
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
    @{
*/

/** @addtogroup OTFC_Driver OTFC Driver
    @{
*/

/** @addtogroup OTFC_EXPORTED_CONSTANTS OTFC Exported Constants
    @{
*/

/*----------------------------------------------------------------------------*/
/* OTFC_CTL constant definitions                                              */
/*----------------------------------------------------------------------------*/
#define OTFC_CTL_PR_SHIFT               (8) /*!< OTFC CTL Protection Region BIT SHIFT */

#define OTFC_CTL_RST_SHIFT              (1)
#define OTFC_CTL_SWAP_SHITF             (2)
#define OTFC_CTL_DFAP_SHITF             (3)
#define OTFC_CTL_INT_SHITF              (7)

#define OTFC_STS_KSERR_SHIFT            (5)
#define OTFC_STS_DFAE_SHIFT             (6)
#define OTFC_STS_IF_SHIFT               (7)

#define OTFC_PR_0                       (0)   /*!< OTFC Protection Region 0 */
#define OTFC_PR_1                       (1)   /*!< OTFC Protection Region 1 */
#define OTFC_PR_2                       (2)   /*!< OTFC Protection Region 2 */
#define OTFC_PR_3                       (3)   /*!< OTFC Protection Region 3 */

#define OTFC_KEY_SRC_FROM_REG           (0x00)    /*!< OTFC Protection Region key source from register */
#define OTFC_KEY_SRC_FROM_KS            (0x01)    /*!< OTFC Protection Region key source from key store */

#define OTFC_KS_KEY_FROM_SRAM           (0x00)    /*!< Key is read from the SRAM of Key Store. */
#define OTFC_KS_KEY_FROM_OTP            (0x10)    /*!< Key is read from the OTP of Key Store. */

/**
  * @brief  Enable OTFC Protection Region 0 ~ 3 function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_CTL_PR_ON(otfc, pr)                      \
  do                                                  \
  {                                                   \
    otfc->CTL |= (0x1ul << (pr * OTFC_CTL_PR_SHIFT)); \
  } while (0)

/**
  * @brief  Disable OTFC Protection Region 0 ~ 3 function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_CTL_PR_OFF(otfc, pr)                      \
  do                                                   \
  {                                                    \
    otfc->CTL &= ~(0x1ul << (pr * OTFC_CTL_PR_SHIFT)); \
  } while (0)

/**
  * @brief  Reset OTFC Protection Region 0 ~ 3 function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_CTL_PR_RST(otfc, pr)                                            \
  do                                                                         \
  {                                                                          \
    otfc->CTL |= (0x1ul << ((pr * OTFC_CTL_PR_SHIFT) + OTFC_CTL_RST_SHIFT)); \
  } while (0)

/**
  * @brief  Enable OTFC Protection Region 0 ~ 3 Key and Nonce Swap function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_KEY_SWAP_EN(otfc, pr)                                            \
  do                                                                          \
  {                                                                           \
    otfc->CTL |= (0x1ul << ((pr * OTFC_CTL_PR_SHIFT) + OTFC_CTL_SWAP_SHITF)); \
  } while (0)

/**
  * @brief  Disable OTFC Protection Region 0 ~ 3 Key and Nonce Swap function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_KEY_SWAP_OFF(otfc, pr)                                            \
  do                                                                           \
  {                                                                            \
    otfc->CTL &= ~(0x1ul << ((pr * OTFC_CTL_PR_SHIFT) + OTFC_CTL_SWAP_SHITF)); \
  } while (0)

#if 0 /* no use */
/**
  * @brief  Enable OTFC Protection Region 0 ~ 3 Differential Fault Attack Protection Enable function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_CTL_DFAP_ON(otfc, pr)                                            \
  do                                                                          \
  {                                                                           \
    otfc->CTL |= (0x1ul << ((pr * OTFC_CTL_PR_SHIFT) + OTFC_CTL_DFAP_SHITF)); \
  } while (0)

/**
  * @brief  Disable OTFC Protection Region 0 ~ 3 Differential Fault Attack Protection Enable function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_CTL_DFAP_OFF(otfc, pr)                                            \
  do                                                                           \
  {                                                                            \
    otfc->CTL &= ~(0x1ul << ((pr * OTFC_CTL_PR_SHIFT) + OTFC_CTL_DFAP_SHITF)); \
  } while (0)
#endif //0

/**
  * @brief  OTFC Protection Region 0 ~ 3 Interrupt Enable Bit function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_CTL_INT_ON(otfc, pr)                                            \
  do                                                                         \
  {                                                                          \
    otfc->CTL |= (0x1ul << ((pr * OTFC_CTL_PR_SHIFT) + OTFC_CTL_INT_SHITF)); \
  } while (0)

/**
  * @brief  OTFC Protection Region 0 ~ 3 Interrupt Enable Bit function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_CTL_INT_OFF(otfc, pr)                                            \
  do                                                                          \
  {                                                                           \
    otfc->CTL &= ~(0x1ul << ((pr * OTFC_CTL_PR_SHIFT) + OTFC_CTL_INT_SHITF)); \
  } while (0)

/**
  * @brief  OTFC Protrction Region 0 ~ 3 Busy. (Read Only)
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_STS_BUSY(otfc, pr)   (otfc->STS & (0x1ul << (pr * OTFC_CTL_PR_SHIFT)))

/**
  * @brief  OTFC Protrction Region 0 Access Key Store Error Flag. (Read Only)
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_STS_KSERR(otfc, pr)    (otfc->STS & (0x1ul << ((pr * OTFC_CTL_PR_SHIFT) + OTFC_STS_KSERR_SHIFT)))

#if 0 /* no use */
/**
  * @brief  OTFC Protrction Region 0 ~ 3 Differential Fault Attack Error Flag.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_STS_DFAERR(otfc, pr)   (otfc->STS & (0x1ul << ((pr * OTFC_CTL_PR_SHIFT) + OTFC_STS_DFAE_SHIFT)))
#endif //0

/**
  * @brief  OTFC Protrction Region 0 ~ 3 Interrupt Flag.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_STS_IF(otfc, pr)   (otfc->STS & (0x1ul << ((pr * OTFC_CTL_PR_SHIFT) + OTFC_STS_IF_SHIFT)))

/**
  * @brief  OTFC Protrction Region 0 ~ 3 Start Address Register.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * @param[in] saddr is the protection start address.
  * \hideinitializer
  */
#define OTFC_PR_START_ADDR(otfc, pr, saddr) \
  do                                        \
  {                                         \
    otfc->PR[pr].PR_SADDR |= saddr;         \
  } while (0)

/**
  * @brief  OTFC Protrction Region End Address Register.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * @param[in] saddr is the protection end address.
  * \hideinitializer
  */
#define OTFC_PR_END_ADDR(otfc, pr, eaddr) \
  do                                      \
  {                                       \
    otfc->PR[pr].PR_EADDR |= eaddr;       \
  } while (0)

/**
  * @brief  Clear OTFC Protrction Region 0 ~ 3 KSCTRL Register Setting
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_CLEAR_KSCTRL(otfc, pr)                              \
  do                                                             \
  {                                                              \
    otfc->PR[pr].PR_KSCTL &= ~(0xFFul << OTFC_PR_KSCTL_NUM_Pos); \
  } while (0)

/**
  * @brief  OTFC Protrction Region 0 ~ 3 Read Key Source From Key Store SRAM.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * @param[in] key_nun is the key number of key store.
  * \hideinitializer
  */
#define OTFC_KSCTRL_SRAM(otfc, pr, key_num)                           \
    do                                                                \
    {                                                                 \
        otfc->PR[pr].PR_KSCTL &= ~(0xFFul << OTFC_PR_KSCTL_NUM_Pos);  \
        otfc->PR[pr].PR_KSCTL = ((key_num << OTFC_PR_KSCTL_NUM_Pos) | \
                                 OTFC_PR_KSCTL_RSRC_Msk);             \
    } while (0)

/**
  * @brief  OTFC Protrction Region 0 ~ 3 Read Key Source From Key Store OTP.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * @param[in] key_nun is the key number of key store.
  * \hideinitializer
  */
#define OTFC_KSCTRL_OTP(otfc, pr, key_num)                                       \
  do                                                                             \
  {                                                                              \
    otfc->PR[pr].PR_KSCTL &= ~(0xFFul << OTFC_PR_KSCTL_NUM_Pos);                 \
    otfc->PR[pr].PR_KSCTL = ((key_num << OTFC_PR_KSCTL_NUM_Pos) |                \
                             (OTFC_KS_KEY_FROM_OTP << OTFC_PR_KSCTL_RSSRC_Pos) | \
                             OTFC_PR_KSCTL_RSRC_Msk);                            \
  } while (0)

/**
  * @brief  The KEY keeps the security key for AES operation in protection region 0 ~ 3.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * @param[in] key The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_PR_KEY0(otfc, pr, key) \
  do                                \
  {                                 \
    otfc->PR[pr].PR_KEY0 = key;     \
  } while (0)

/**
  * @brief  The KEY keeps the security key for AES operation in protection region 0 ~ 3.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * @param[in] key The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_PR_KEY1(otfc, pr, key) \
  do                                \
  {                                 \
    otfc->PR[pr].PR_KEY1 = key;     \
  } while (0)

/**
  * @brief  The KEY keeps the security key for AES operation in protection region 0 ~ 3.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * @param[in] key The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_PR_KEY2(otfc, pr, key) \
  do                                \
  {                                 \
    otfc->PR[pr].PR_KEY2 = key;     \
  } while (0)

/**
  * @brief  The KEY keeps the security key for AES operation in protection region 0 ~ 3.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * @param[in] key The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_PR_KEY3(otfc, pr, key) \
  do                                \
  {                                 \
    otfc->PR[pr].PR_KEY3 = key;     \
  } while (0)

/**
  * @brief  The scramble key for address cipher funtcion in protection region 0 ~ 3.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * @param[in] key The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_PR_SCRAMBLE(otfc, pr, key) \
  do                                    \
  {                                     \
    otfc->PR[pr].PR_SCRAMBLE = key;     \
  } while (0)

/**
  * @brief  Three nonces are for the address cipher function in OTFC.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * @param[in] key The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_PR_NONCE0(otfc, pr, key) \
  do                                  \
  {                                   \
    otfc->PR[pr].PR_NONCE0 = key;     \
  } while (0)

/**
  * @brief  Three nonces are for the address cipher function in OTFC.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * @param[in] key The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_PR_NONCE1(otfc, pr, key) \
  do                                  \
  {                                   \
    otfc->PR[pr].PR_NONCE1 = key;     \
  } while (0)

/**
  * @brief  Three nonces are for the address cipher function in OTFC.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region 0 ~ 3.
  * @param[in] key The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_PR_NONCE2(otfc, pr, key) \
  do                                  \
  {                                   \
    otfc->PR[pr].PR_NONCE2 = key;     \
  } while (0)

/** @} end of group OTFC_EXPORTED_CONSTANTS */

/** @addtogroup OTFC_EXPORTED_FUNCTIONS OTFC Exported Functions
  @{
*/
void OTFC_PR_KeyFromRegister(OTFC_T *otfc,
                             uint32_t u32PR,
                             uint32_t u32SAddr,
                             uint32_t u32EAddr,
                             uint32_t u32Key0, uint32_t u32Key1, uint32_t u32Key2, uint32_t u32Key3,
                             uint32_t u32Scramble,
                             uint32_t u32Nonce0, uint32_t u32Nonce1, uint32_t u32Nonce2);

void OTFC_PR_KeyFromKeyStore(OTFC_T *otfc, uint32_t u32PR,
                             uint32_t u32SAddr, uint32_t u32EAddr,
                             uint32_t u32KeyNum, uint32_t u8KeySrc,
                             uint32_t u32Scramble,
                             uint32_t u32Nonce0, uint32_t u32Nonce1, uint32_t u32Nonce2);
void OTFC_PR_Disable(OTFC_T *otfc, uint32_t u32RPx);

/** @} end of group OTFC_EXPORTED_FUNCTIONS */
/** @} end of group OTFC_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __OTFC_H__ */
