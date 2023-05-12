/**************************************************************************//**
 * @file    otfc.h
 * @version V1.00
 * @brief   OTFC driver header file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
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
#define OTFC_RSRC_FROM_REGISTER         (0x00)
#define OTFC_RSRC_FROM_KEYSTORE         (0x01)

#define OTFC_RSSRC_SRC_SRAM             (0x00)
#define OTFC_RSSRC_SRC_OTP              (0x10)

/**
  * @brief  Enable OTFC Protection Region 0~3 function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region of OTFC module.
  * \hideinitializer
  */
#define OTFC_CTL_EN_ON(otfc, pr)    (otfc->CTL |= (0x1ul << (pr * 8)))           /*!< OTFC_CTL: Protection Region 0~3 Enable                                       \hideinitializer */

/**
  * @brief  Disable OTFC Protection Region 0~3 function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region of OTFC module.
  * \hideinitializer
  */
#define OTFC_CTL_EN_OFF(otfc, pr)   (otfc->CTL &= ~(0x1ul << (pr * 8)))

/**
  * @brief  Reset OTFC Protection Region 0~3 function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region of OTFC module.
  * \hideinitializer
  */
#define OTFC_CTL_RST(otfc, pr)    (otfc->CTL |= (0x1ul << ((pr * 8) + 1)))

/**
  * @brief  Enable OTFC Protection Region 0~3 Key and Nonce Swap function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region of OTFC module.
  * \hideinitializer
  */
#define OTFC_ENABLE_KSWAP(otfc, pr)   (otfc->CTL |= (0x1ul << ((pr * 8) + 2)))

/**
  * @brief  Disable OTFC Protection Region 0~3 Key and Nonce Swap function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region of OTFC module.
  * \hideinitializer
  */
#define OTFC_DISABLE_KSWAP(otfc, pr)    (otfc->CTL &= ~(0x1ul << ((pr * 8) + 2)))

/**
  * @brief  OTFC Protection Region 0~3 Differential Fault Attack Protection Enable function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region of OTFC module.
  * \hideinitializer
  */
#define OTFC_CTL_DFAPEN(otfc, pr)   (otfc->CTL |= (0x1ul << ((pr * 8) + 3)))

/**
  * @brief  OTFC Protection Region 0~3 Interrupt Enable Bit function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region of OTFC module.
  * \hideinitializer
  */
#define OTFC_CTL_INT_ON(otfc, pr)   (otfc->CTL |= (0x1ul << ((pr * 8) + 7)))

/**
  * @brief  OTFC Protection Region 0~3 Interrupt Enable Bit function.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region of OTFC module.
  * \hideinitializer
  */
#define OTFC_CTL_INT_OFF(otfc, pr)  (otfc->CTL &= ~(0x1ul << ((pr * 8) + 7)))

/**
  * @brief  OTFC Protrction Region 0~3 Busy. (Read Only)
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region of OTFC module.
  * \hideinitializer
  */
#define OTFC_STS_BUSY(otfc, pr)     (otfc->STS & (0x1ul << (pr * 8)))

/**
  * @brief  OTFC Protrction Region 0 Access Key Store Error Flag. (Read Only)
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region of OTFC module.
  * \hideinitializer
  */
#define OTFC_STS_KSERR(otfc, pr)    (otfc->STS & (0x1ul << ((pr * 8) + 5)))

/**
  * @brief  OTFC Protrction Region 0~3 Differential Fault Attack Error Flag.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region of OTFC module.
  * \hideinitializer
  */
#define OTFC_STS_DFAERR(otfc, pr)   (otfc->STS & (0x1ul << ((pr * 8) + 6)))

/**
  * @brief  OTFC Protrction Region 0~3 Interrupt Flag.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region of OTFC module.
  * \hideinitializer
  */
#define OTFC_STS_IF(otfc, pr)       (otfc->STS & (0x1ul << ((pr * 8) + 7)))

/**
  * @brief  OTFC Protrction Region 0~3 Read Key Source From Key Store SRAM.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region of OTFC module.
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
  * @brief  OTFC Protrction Region 0~3 Read Key Source From Key Store OTP.
  * @param[in] otfc is the base address of OTFC module.
  * @param[in] pr is the Protection Region of OTFC module.
  * @param[in] key_nun is the key number of key store.
  * \hideinitializer
  */
#define OTFC_KSCTRL_OTP(otfc, pr, key_num)                            \
	do                                                                \
	{                                                                 \
		otfc->PR[pr].PR_KSCTL &= ~(0xFFul << OTFC_PR_KSCTL_NUM_Pos);  \
		otfc->PR[pr].PR_KSCTL = ((key_num << OTFC_PR_KSCTL_NUM_Pos) | \
								 (1 << OTFC_PR_KSCTL_RSSRC_Pos) |     \
								 OTFC_PR_KSCTL_RSRC_Msk);             \
	} while (0)

	/*----------------------------------------------------------------------------*/
	/* Define Function Prototypes                                                 */
	/*----------------------------------------------------------------------------*/
	void OTFC_PRFromRegister(OTFC_T *otfc,
							 uint8_t u8PRx,
							 uint32_t u32SAddr,
							 uint32_t u32EAddr,
							 uint32_t u32Key0, uint32_t u32Key1, uint32_t u32Key2, uint32_t u32Key3,
							 uint32_t u32Scramble,
							 uint32_t u32Nonce0, uint32_t u32Nonce1, uint32_t u32Nonce2);

	void OTFC_PRDisable(OTFC_T *otfc, uint8_t u8PRx);
	void OTFC_PRFromKeyStore(OTFC_T *otfc, uint8_t u8PRx, uint8_t u8KeyNum, uint8_t u8KeyStoreSrc);

	/** @} end of group OTFC_EXPORTED_FUNCTIONS */
	/** @} end of group OTFC_Driver */
	/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __OTFC_H__ */
