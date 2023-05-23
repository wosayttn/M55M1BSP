/**************************************************************************//**
 * @file     otfc.c
 * @version  V1.00
 * @brief    OTFC driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup OTFC_Driver SPI Driver
  @{
*/

/** @addtogroup OTFC_EXPORTED_FUNCTIONS OTFC Exported Functions
  @{
*/
/**
 * @brief Disable OTFC Protection Region 0 ~ 3 and Clear Region Setting.
 *
 * @param otfc is the base address of OTFC module.
 * @param u8RPx is the Protection Region of OTFC module.
 */
void OTFC_PR_Disable(OTFC_T *otfc, uint32_t u32RPx)
{
    /* Disable Protection Region 0 ~ 3 */
    OTFC_CTL_PR_OFF(otfc, u32RPx);

    /* Reset Protection Region 0 ~ 3 */
    OTFC_CTL_PR_RST(otfc, u32RPx);
}

/**
 * @brief   Enable OTFC Protection Region 0 ~ 3 and Use Key Source From Key store.
 *
 * @param[in] otfc  is the base address of OTFC module.
 * @param[in] u32PR is the Protection Region 0 ~ 3.
 * @param[in] u32SAddr  is the Protection Region Start Address
 * @param[in] u32EAddr  is the Protection Region End Address
 * @param[in] u32KeyNum is the KEY Store of KEY number
 * @param[in] u8KeySrc  is the Key Source form Key Store SRAM or OTP
 *                      - OTFC_KS_KEY_FROM_SRAM
 *                      - OTFC_KS_KEY_FROM_OTP
 * @param[in] u32Scramble if the Protection Region Scramble key
 * @param[in] u32Nonce0 is the Protection Region Nonce Word 0 Key
 * @param[in] u32Nonce1 is the Protection Region Nonce Word 1 Key
 * @param[in] u32Nonce2 is the Protection Region Nonce Word 2 Key
 */
void OTFC_PR_KeyFromKeyStore(OTFC_T *otfc, uint32_t u32PR,
                             uint32_t u32SAddr, uint32_t u32EAddr,
                             uint32_t u32KeyNum, uint32_t u8KeySrc,
                             uint32_t u32Scramble,
                             uint32_t u32Nonce0, uint32_t u32Nonce1, uint32_t u32Nonce2)
{
    uint32_t u32Timeout = 0xFFFF;

    /* Wait Protection Region 0 ~ 3 not Busy*/
    while (OTFC_STS_BUSY(otfc, u32PR) == 1)
    {
        if (u32Timeout-- <= 0)
        {
            break;
        }
    }

    /* Reset Protection Region 0 ~ 3 */
    OTFC_CTL_PR_RST(otfc, u32PR);

    /* Set Protection Region Start and End Address */
    OTFC_PR_START_ADDR(otfc, u32PR, u32SAddr);
    OTFC_PR_END_ADDR(otfc, u32PR, u32EAddr);

    /* Set Protection Region Scramble/Nonce0/1/2 Key */
    OTFC_PR_SCRAMBLE(otfc, u32PR, u32Scramble);
    OTFC_PR_NONCE0(otfc, u32PR, u32Nonce0);
    OTFC_PR_NONCE1(otfc, u32PR, u32Nonce1);
    OTFC_PR_NONCE2(otfc, u32PR, u32Nonce2);

    /* Set Key Store key number */
    otfc->PR[u32PR].PR_KSCTL &= ~(0xFFul << OTFC_PR_KSCTL_NUM_Pos);
    otfc->PR[u32PR].PR_KSCTL = ((u32KeyNum << OTFC_PR_KSCTL_NUM_Pos)  |
                                (u8KeySrc << OTFC_PR_KSCTL_RSSRC_Pos) |
                                OTFC_PR_KSCTL_RSRC_Msk);

    /* Protection Region Enable */
    OTFC_CTL_PR_ON(otfc, u32PR);
}

/**
 * @brief   Enable OTFC Protrction Region 0 ~ 3 and Use Key Source From Register.
 *
 * @param[in] otfc  is the base address of OTFC module.
 * @param[in] u8PRx is the Protection Region 0 ~ 3.
 * @param[in] u32SAddr  is the Protection Region Start Address
 * @param[in] u32EAddr  is the Protection Region End Address
 * @param[in] u32Key0   is the KEY keeps the security key for AES operation in protection region 0 ~ 3
 * @param[in] u32Key1   is the KEY keeps the security key for AES operation in protection region 0 ~ 3
 * @param[in] u32Key2   is the KEY keeps the security key for AES operation in protection region 0 ~ 3
 * @param[in] u32Key3   is the KEY keeps the security key for AES operation in protection region 0 ~ 3
 * @param[in] u32Scramble   if the Protection Region 0 ~ 3 Scramble key
 * @param[in] u32Nonce0 is the Protection Region 0 ~ 3 Nonce Word 0 Key
 * @param[in] u32Nonce1 is the Protection Region 0 ~ 3 Nonce Word 1 Key
 * @param[in] u32Nonce2 is the Protection Region 0 ~ 3 Nonce Word 2 Key
 */
void OTFC_PR_KeyFromRegister(OTFC_T *otfc,
                             uint32_t u32PR,
                             uint32_t u32SAddr,
                             uint32_t u32EAddr,
                             uint32_t u32Key0, uint32_t u32Key1, uint32_t u32Key2, uint32_t u32Key3,
                             uint32_t u32Scramble,
                             uint32_t u32Nonce0, uint32_t u32Nonce1, uint32_t u32Nonce2)
{
    uint32_t u32Timeout = 0xFFFF;

    while (OTFC_STS_BUSY(otfc, u32PR) == 1)
    {
        if (u32Timeout-- <= 0)
        {
            break;
        }
    }

    /* Reset Protection Region 0 ~ 3 Setting*/
    OTFC_CTL_PR_RST(otfc, u32PR);

    OTFC_CLEAR_KSCTRL(otfc, u32PR);

    /* Protection Region 0 ~ 3 key is read from registers OTFC_PRx_KEYx */
    OTFC_PR_START_ADDR(otfc, u32PR, u32SAddr);
    OTFC_PR_END_ADDR(otfc, u32PR, u32EAddr);
    OTFC_PR_KEY0(otfc, u32PR, u32Key0);
    OTFC_PR_KEY1(otfc, u32PR, u32Key1);
    OTFC_PR_KEY2(otfc, u32PR, u32Key2);
    OTFC_PR_KEY3(otfc, u32PR, u32Key3);
    OTFC_PR_SCRAMBLE(otfc, u32PR, u32Scramble);
    OTFC_PR_NONCE0(otfc, u32PR, u32Nonce0);
    OTFC_PR_NONCE1(otfc, u32PR, u32Nonce1);
    OTFC_PR_NONCE2(otfc, u32PR, u32Nonce2);

    /* Protection Region Enable */
    OTFC_CTL_PR_ON(otfc, u32PR);
}
