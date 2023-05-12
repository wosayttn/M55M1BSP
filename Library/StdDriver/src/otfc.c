/**************************************************************************//**
 * @file     otfc.c
 * @version  V1.00
 * @brief    OTFC driver source file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
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

void otfc_open(OTFC_T *otfc)
{

}

void otfc_cloce(OTFC_T *otfc)
{
    //SYS->OTFCRST |= SYS_OTFCRST_OTFCRST_Msk;
    //SYS->OTFCRST &= ~SYS_OTFCRST_OTFCRST_Msk;
}

/**
 * @brief Disable OTFC Protrction Region 0~3 and Clear Region Setting.
 *
 * @param otfc is the base address of OTFC module.
 * @param u8RPx is the Protection Region of OTFC module.
 */
void OTFC_PRDisable(OTFC_T *otfc, uint8_t u8RPx)
{
    /* Protection Region 0~3 Reset */
    OTFC_CTL_RST(otfc, u8RPx);

    /* Protection Region 0~3 Disable */
    OTFC_CTL_EN_OFF(otfc, u8RPx);
}

/**
 * @brief   Enable OTFC Protrction Region 0~3 and Use Key Source From Key store.
 *
 * @param[in] otfc  is the base address of OTFC module.
 * @param[in] u8PRx is the Protection Region of OTFC module.
 * @param[in] u8KeyNum  is the KEY Store KEY number
 * @param[in] u8KeyStoreSrc is the KEY Store Source
 */
void OTFC_PRFromKeyStore(OTFC_T *otfc, uint8_t u8PRx, uint8_t u8KeyNum, uint8_t u8KeyStoreSrc)
{
    /* Protection Region 0~3 Reset */
    OTFC_CTL_RST(otfc, u8PRx);

    /* Protection Region 0~3 key is read from registers OTFC_PRx_KEYx */
    otfc->PR[u8PRx].PR_KSCTL &= ~0xFF;

    otfc->PR[u8PRx].PR_KSCTL |= (u8KeyNum << OTFC_PR_KSCTL_NUM_Pos);
    otfc->PR[u8PRx].PR_KSCTL |= OTFC_PR_KSCTL_RSRC_Msk;
    otfc->PR[u8PRx].PR_KSCTL |= (u8KeyStoreSrc << OTFC_PR_KSCTL_RSSRC_Pos);
}

/**
 * @brief   Enable OTFC Protrction Region 0~3 and Use Key Source From Register.
 *
 * @param[in] otfc  is the base address of OTFC module.
 * @param[in] u8PRx is the Protection Region of OTFC module.
 * @param[in] u32SAddr  is the Protection Region 0~3 Start Address
 * @param[in] u32EAddr  is the Protection Region 0~3 End Address
 * @param[in] u32Key0   is the KEY keeps the security key for AES operation in protection region 0~3
 * @param[in] u32Key1   is the KEY keeps the security key for AES operation in protection region 0~3
 * @param[in] u32Key2   is the KEY keeps the security key for AES operation in protection region 0~3
 * @param[in] u32Key3   is the KEY keeps the security key for AES operation in protection region 0~3
 * @param[in] u32Scramble   if the Protection Region 0~3 Scramble key
 * @param[in] u32Nonce0 is the Protection Region 0~3 Nonce Word x Register
 * @param[in] u32Nonce1 is the Protection Region 0~3 Nonce Word x Register
 * @param[in] u32Nonce2 is the Protection Region 0~3 Nonce Word x Register
 */
void OTFC_PRFromRegister(OTFC_T *otfc,
                         uint8_t u8PRx,
                         uint32_t u32SAddr,
                         uint32_t u32EAddr,
                         uint32_t u32Key0, uint32_t u32Key1, uint32_t u32Key2, uint32_t u32Key3,
                         uint32_t u32Scramble,
                         uint32_t u32Nonce0, uint32_t u32Nonce1, uint32_t u32Nonce2)
{
    /* Protection Region 0~3 Reset */
    OTFC_CTL_RST(otfc, u8PRx);

    /* Protection Region 0~3 key is read from registers OTFC_PRx_KEYx */
    otfc->PR[u8PRx].PR_KSCTL &= ~0xFF;

    otfc->PR[u8PRx].PR_SADDR = u32SAddr;
    otfc->PR[u8PRx].PR_EADDR = u32EAddr;
    otfc->PR[u8PRx].PR_KEY0 = u32Key0;
    otfc->PR[u8PRx].PR_KEY1 = u32Key1;
    otfc->PR[u8PRx].PR_KEY2 = u32Key2;
    otfc->PR[u8PRx].PR_KEY3 = u32Key3;
    otfc->PR[u8PRx].PR_SCRAMBLE = u32Scramble;
    otfc->PR[u8PRx].PR_NONCE0 = u32Nonce0;
    otfc->PR[u8PRx].PR_NONCE1 = u32Nonce1;
    otfc->PR[u8PRx].PR_NONCE2 = u32Nonce2;

    /* Protection Region 0 Key and Nonce Swap */
    //otfc->CTL |= OTFC_CTL_KSWAP0_Msk;

    /* Protection Region 0 Differential Fault Attack Protection Enable */
    //otfc->CTL |= OTFC_CTL_DFAPEN0_Msk;

    /* Protection Region 0 Interrupt Enable Bit */
    //otfc->CTL |= OTFC_CTL_IEN0_Msk;

    /* Protection Region 0 Enable */
    OTFC_CTL_EN_ON(otfc, u8PRx);
}
