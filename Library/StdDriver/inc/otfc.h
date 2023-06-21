/**************************************************************************//**
 * @file    otfc.h
 * @version V1.00
 * @brief   OTFC driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
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

/* OTFC Wait State Timeout Counter. */
#define OTFC_TIMEOUT                    SystemCoreClock /*!< OTFC time-out counter (1 second time-out) */

/* OTFC Define Error Code */
#define OTFC_OK                         ( 0L)   /*!< OTFC operation OK */
#define OTFC_ERR_FAIL                   (-1L)   /*!< OTFC operation failed */
#define OTFC_ERR_TIMEOUT                (-2L)   /*!< OTFC operation abort due to timeout error */

/*----------------------------------------------------------------------------*/
/* OTFC_CTL constant definitions                                              */
/*----------------------------------------------------------------------------*/
#define OTFC_CTL_PR_Pos                 (8)     /*!< OTFC CTL Protection Region BIT SHIFT */

#define OTFC_CTL_RST_Pos                (1)
#define OTFC_CTL_SWAP_Pos               (2)
#define OTFC_CTL_INT_Pos                (7)

#define OTFC_STS_KSERR_Pos              (5)
#define OTFC_STS_DFAE_Pos               (6)
#define OTFC_STS_IF_Pos                 (7)

#define OTFC_PR_0                       (0)     /*!< OTFC Protection Region 0 */
#define OTFC_PR_1                       (1)     /*!< OTFC Protection Region 1 */
#define OTFC_PR_2                       (2)     /*!< OTFC Protection Region 2 */
#define OTFC_PR_3                       (3)     /*!< OTFC Protection Region 3 */

#define OTFC_KEY_SRC_REG                (0x00)    /*!< OTFC Protection Region key source from register */
#define OTFC_KEY_SRC_KS                 (0x01)    /*!< OTFC Protection Region key source from key store */

#define OTFC_KS_SRAM_KEY                (0x00)    /*!< Key is read from the SRAM of Key Store. */
#define OTFC_KS_OTP_KEY                 (0x10)    /*!< Key is read from the OTP of Key Store. */

/**
  * @brief  Enable OTFC Protection Region.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_ENABLE_PR(otfc, pr)                        \
    do                                                  \
    {                                                   \
        otfc->CTL |= (0x1ul << (pr * OTFC_CTL_PR_Pos)); \
    } while (0)

/**
 * @brief   Disable Protection Region
 * @param[in] otfc The base address of OTFC module.
 * @param[in] pr    Protection Region. It could be 0 ~ 3.
 *
 */
#define OTFC_DISABLE_PR(otfc, pr)                        \
    do                                                   \
    {                                                    \
        otfc->CTL &= ~(0x1ul << (pr * OTFC_CTL_PR_Pos)); \
    } while (0)

/**
  * @brief  Reset OTFC Protection Region.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_RESET_PR(otfc, pr)                                              \
    do                                                                       \
    {                                                                        \
        otfc->CTL |= (0x1ul << ((pr * OTFC_CTL_PR_Pos) + OTFC_CTL_RST_Pos)); \
    } while (0)

/**
 * @brief  Enable Swap Key, Scramble and Nonce.
 * @param[in] otfc The base address of OTFC module.
 * @param[in] pr    Protection Region. It could be 0 ~ 3.
 * \hideinitializer
 */
#define OTFC_ENABLE_KEY_SWAP(otfc, pr)                                        \
    do                                                                        \
    {                                                                         \
        otfc->CTL |= (0x1ul << ((pr * OTFC_CTL_PR_Pos) + OTFC_CTL_SWAP_Pos)); \
    } while (0)

/**
  * @brief  Disable Swap Key, Scramble and Nonce.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_DISABLE_KEY_SWAP(otfc, pr)                                        \
    do                                                                         \
    {                                                                          \
        otfc->CTL &= ~(0x1ul << ((pr * OTFC_CTL_PR_Pos) + OTFC_CTL_SWAP_Pos)); \
    } while (0)

/**
  * @brief  Enable Interrupt.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_ENABLE_INT(otfc, pr)                                            \
    do                                                                       \
    {                                                                        \
        otfc->CTL |= (0x1ul << ((pr * OTFC_CTL_PR_Pos) + OTFC_CTL_INT_Pos)); \
    } while (0)

/**
  * @brief  Disable Interrupt.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_DISABLE_INT(otfc, pr)                                            \
    do                                                                        \
    {                                                                         \
        otfc->CTL &= ~(0x1ul << ((pr * OTFC_CTL_PR_Pos) + OTFC_CTL_INT_Pos)); \
    } while (0)

/**
  * @brief  Get Busy Status. (Read Only)
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_GET_BUSY(otfc, pr)   \
    (otfc->STS & (0x1ul << (pr * OTFC_CTL_PR_Pos)))

/**
  * @brief  Get Access Key Store Error Flag. (Read Only)
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_GET_KSERR(otfc, pr)    \
    (otfc->STS & (0x1ul << ((pr * OTFC_CTL_PR_Pos) + OTFC_STS_KSERR_Pos)))

/**
* @brief    Get Differential Fault Attack Error Flag.
* @param[in] otfc The base address of OTFC module.
* @param[in] pr     Protection Region 0 ~ 3.
* \hideinitializer
*/
#define OTFC_GET_DFAERR(otfc, pr)   \
    (otfc->STS & (0x1ul << ((pr * OTFC_CTL_PR_Pos) + OTFC_STS_DFAE_Pos)))

/**
  * @brief  Get Interrupt Flag.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_GET_IF(otfc, pr)   \
    (otfc->STS & (0x1ul << ((pr * OTFC_CTL_PR_Pos) + OTFC_STS_IF_Pos)))

/**
  * @brief  Set Protection Start Address.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * @param[in] saddr is the protection start address.
  * \hideinitializer
  */
#define OTFC_SET_START_ADDR(otfc, pr, saddr) \
    do                                       \
    {                                        \
        otfc->PR[pr].PR_SADDR = saddr;       \
    } while (0)

/**
  * @brief  Set Protrction End Address.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * @param[in] eaddr is the protection end address.
  * \hideinitializer
  */
#define OTFC_SET_END_ADDR(otfc, pr, eaddr) \
    do                                     \
    {                                      \
        otfc->PR[pr].PR_EADDR |= eaddr;    \
    } while (0)

/**
  * @brief  Clear Key Store Control Setting.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * \hideinitializer
  */
#define OTFC_CLEAR_KSCTRL(otfc, pr)                                  \
    do                                                               \
    {                                                                \
        otfc->PR[pr].PR_KSCTL &= ~(0xFFul << OTFC_PR_KSCTL_NUM_Pos); \
    } while (0)

/**
  * @brief  Read the key source from the key store in SRAM.
  * @param[in] otfc     The base address of OTFC module.
  * @param[in] pr       Protection Region 0 ~ 3.
  * @param[in] key_nun  key number of key store.
  * \hideinitializer
  */
#define OTFC_ENABLE_KS_SRAM(otfc, pr, key_num)                        \
    do                                                                \
    {                                                                 \
        otfc->PR[pr].PR_KSCTL &= ~(0xFFul << OTFC_PR_KSCTL_NUM_Pos);  \
        otfc->PR[pr].PR_KSCTL = ((key_num << OTFC_PR_KSCTL_NUM_Pos) | \
                                 OTFC_PR_KSCTL_RSRC_Msk);             \
    } while (0)

/**
  * @brief  Read the key source from the key store in OTP.
  * @param[in] otfc     The base address of OTFC module.
  * @param[in] pr       Protection Region 0 ~ 3.
  * @param[in] key_nun  key number of key store.
  * \hideinitializer
  */
#define OTFC_ENABLE_KS_OTP(otfc, pr, key_num)                                   \
    do                                                                          \
    {                                                                           \
        otfc->PR[pr].PR_KSCTL &= ~(0xFFul << OTFC_PR_KSCTL_NUM_Pos);            \
        otfc->PR[pr].PR_KSCTL = ((key_num << OTFC_PR_KSCTL_NUM_Pos) |           \
                                 (OTFC_KS_OTP_KEY << OTFC_PR_KSCTL_RSSRC_Pos) | \
                                 OTFC_PR_KSCTL_RSRC_Msk);                       \
    } while (0)

/**
  * @brief  The KEY keeps the security key for AES .
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * @param[in] key  The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_SET_KEY0(otfc, pr, key) \
    do                               \
    {                                \
        otfc->PR[pr].PR_KEY0 = key;  \
    } while (0)

/**
  * @brief  The KEY keeps the security key for AES.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * @param[in] key  The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_SET_KEY1(otfc, pr, key) \
    do                               \
    {                                \
        otfc->PR[pr].PR_KEY1 = key;  \
    } while (0)

/**
  * @brief  The KEY keeps the security key for AES.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * @param[in] key The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_SET_KEY2(otfc, pr, key) \
    do                               \
    {                                \
        otfc->PR[pr].PR_KEY2 = key;  \
    } while (0)

/**
  * @brief  The KEY keeps the security key for AES.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * @param[in] key  The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_SET_KEY3(otfc, pr, key) \
    do                               \
    {                                \
        otfc->PR[pr].PR_KEY3 = key;  \
    } while (0)

/**
  * @brief  The scramble key for address cipher.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * @param[in] key  The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_SET_SCRAMBLE(otfc, pr, key) \
    do                                   \
    {                                    \
        otfc->PR[pr].PR_SCRAMBLE = key;  \
    } while (0)

/**
  * @brief  Three nonces are for the address cipher function in OTFC.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * @param[in] key The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_SET_NONCE0(otfc, pr, key) \
    do                                 \
    {                                  \
        otfc->PR[pr].PR_NONCE0 = key;  \
    } while (0)

/**
  * @brief  Three nonces are for the address cipher function in OTFC.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * @param[in] key The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_SET_NONCE1(otfc, pr, key) \
    do                                 \
    {                                  \
        otfc->PR[pr].PR_NONCE1 = key;  \
    } while (0)

/**
  * @brief  Three nonces are for the address cipher function in OTFC.
  * @param[in] otfc The base address of OTFC module.
  * @param[in] pr   Protection Region. It could be 0 ~ 3.
  * @param[in] key The KEY keeps the security key for AES.
  * \hideinitializer
  */
#define OTFC_SET_NONCE2(otfc, pr, key) \
    do                                 \
    {                                  \
        otfc->PR[pr].PR_NONCE2 = key;  \
    } while (0)

/**
 * @brief Enable OTFC Protection Region.
 *
 * @param otfc  The base address of OTFC module.
 * @param u32PR Protection Region 0 ~ 3.
 * @return __STATIC_INLINE
 */
__STATIC_INLINE int32_t OTFC_Enable_PR(OTFC_T *otfc, uint32_t u32PR)
{
    if (u32PR > 3)
    {
        return OTFC_ERR_FAIL;
    }

    OTFC_ENABLE_PR(otfc, u32PR);

    return OTFC_OK;
}

/**
 * @brief Disable OTFC Protection Region.
 *
 * @param otfc  The base address of OTFC module.
 * @param u32PR Protection Region 0 ~ 3.
 * @return __STATIC_INLINE
 */
__STATIC_INLINE int32_t OTFC_Disable_PR(OTFC_T *otfc, uint32_t u32PR)
{
    if (u32PR > 3)
    {
        return OTFC_ERR_FAIL;
    }

    OTFC_DISABLE_PR(otfc, u32PR);

    return OTFC_OK;
}

/** @} end of group OTFC_EXPORTED_CONSTANTS */

typedef struct
{
    uint32_t u32Key0; /*!< The security key for AES */
    uint32_t u32Key1; /*!< The security key for AES */
    uint32_t u32Key2; /*!< The security key for AES */
    uint32_t u32Key3; /*!< The security key for AES */

    uint32_t u32Scramble; /*!< Scramble key */

    uint32_t u32Nonce0; /*!< Nonce Word */
    uint32_t u32Nonce1; /*!< Nonce Word */
    uint32_t u32Nonce2; /*!< Nonce Word */
} OTFC_KEY_T;

/** @addtogroup OTFC_EXPORTED_FUNCTIONS OTFC Exported Functions
  @{
*/
int32_t OTFC_SetKeyTableToReg(OTFC_T *otfc,
                              OTFC_KEY_T *psKeyTable,
                              uint32_t u32PR,
                              uint32_t u32SAddr,
                              uint32_t u32EAddr);

int32_t OTFC_SetKeyFromKeyStore(OTFC_T *otfc, uint32_t u32PR,
                                uint32_t u32SAddr, uint32_t u32EAddr,
                                uint32_t u32KeyNum, uint32_t u32KeySrc,
                                uint32_t u32Scramble,
                                uint32_t u32Nonce0, uint32_t u32Nonce1, uint32_t u32Nonce2);

/** @} end of group OTFC_EXPORTED_FUNCTIONS */
/** @} end of group OTFC_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __OTFC_H__ */
