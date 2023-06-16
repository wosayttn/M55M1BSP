/**************************************************************************//**
 * @file     otfc.c
 * @version  V1.00
 * @brief    OTFC driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
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
static int32_t otfc_wait_busy(OTFC_T *otfc, uint32_t u32PR)
{
    int32_t i32Timeout = OTFC_TIMEOUT;

    /* Wait Protection Region 0 ~ 3 not Busy*/
    while (OTFC_GET_BUSY(otfc, u32PR) == 1)
    {
        if (i32Timeout-- <= 0)
        {
            return OTFC_ERR_TIMEOUT;
        }
    }

    return OTFC_OK;
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
 *                      - OTFC_KS_SRAM_KEY
 *                      - OTFC_KS_OTP_KEY
 * @param[in] u32Scramble if the Protection Region Scramble key
 * @param[in] u32Nonce0 is the Protection Region Nonce Word 0 Key
 * @param[in] u32Nonce1 is the Protection Region Nonce Word 1 Key
 * @param[in] u32Nonce2 is the Protection Region Nonce Word 2 Key
 */
int32_t OTFC_SetKeyFromKeyStore(OTFC_T *otfc, uint32_t u32PR,
                                uint32_t u32SAddr, uint32_t u32EAddr,
                                uint32_t u32KeyNum, uint32_t u32KeySrc,
                                uint32_t u32Scramble,
                                uint32_t u32Nonce0, uint32_t u32Nonce1, uint32_t u32Nonce2)
{
    /* Wait Protection Region 0 ~ 3 not Busy*/
    if (otfc_wait_busy(otfc, u32PR) != OTFC_OK)
    {
        return OTFC_ERR_TIMEOUT;
    }

    /* Reset Protection Region 0 ~ 3 */
    OTFC_RESET_PR(otfc, u32PR);

    /* Set Protection Region Start and End Address */
    OTFC_SET_START_ADDR(otfc, u32PR, u32SAddr);
    OTFC_SET_END_ADDR(otfc, u32PR, u32EAddr);

    /* Set Protection Region Scramble/Nonce0/1/2 Key */
    OTFC_SET_SCRAMBLE(otfc, u32PR, u32Scramble);
    OTFC_SET_NONCE0(otfc, u32PR, u32Nonce0);
    OTFC_SET_NONCE1(otfc, u32PR, u32Nonce1);
    OTFC_SET_NONCE2(otfc, u32PR, u32Nonce2);

    /* Clear protect region setting */
    OTFC_CLEAR_KSCTRL(otfc, u32PR);

    /* Set Key Store key number */
    otfc->PR[u32PR].PR_KSCTL |= (((u32KeyNum & 0x1F) << OTFC_PR_KSCTL_NUM_Pos)  |
                                 (u32KeySrc << OTFC_PR_KSCTL_RSSRC_Pos) |
                                 OTFC_PR_KSCTL_RSRC_Msk);

    return OTFC_OK;
}

/**
 * @brief Use Key table set OTFC protection Region 0 ~ 3 key Source to register.
 *
 * @param otfc
 * @param psKeyTable  OTFC key table.
 * @param u32PR       Protection region 0 ~ 3.
 * @param u32SAddr    Protection region start address.
 * @param u32EAddr    Protection region start address.
 */
int32_t OTFC_SetKeyTableToReg(OTFC_T *otfc,
                              OTFC_KEY_T *psKeyTable,
                              uint32_t u32PR,
                              uint32_t u32SAddr,
                              uint32_t u32EAddr)
{
    /* Wait Protection Region 0 ~ 3 not Busy*/
    if (otfc_wait_busy(otfc, u32PR) != OTFC_OK)
    {
        return OTFC_ERR_TIMEOUT;
    }

    /* Reset Protection Region 0 ~ 3 Setting*/
    OTFC_RESET_PR(otfc, u32PR);

    OTFC_CLEAR_KSCTRL(otfc, u32PR);

    /* Protection Region 0 ~ 3 key is read from registers OTFC_PRx_KEYx */
    OTFC_SET_START_ADDR(otfc, u32PR, u32SAddr);
    OTFC_SET_END_ADDR(otfc, u32PR, u32EAddr);
    OTFC_SET_KEY0(otfc, u32PR, psKeyTable->u32Key0);
    OTFC_SET_KEY1(otfc, u32PR, psKeyTable->u32Key1);
    OTFC_SET_KEY2(otfc, u32PR, psKeyTable->u32Key2);
    OTFC_SET_KEY3(otfc, u32PR, psKeyTable->u32Key3);
    OTFC_SET_SCRAMBLE(otfc, u32PR, psKeyTable->u32Scramble);
    OTFC_SET_NONCE0(otfc, u32PR, psKeyTable->u32Nonce0);
    OTFC_SET_NONCE1(otfc, u32PR, psKeyTable->u32Nonce1);
    OTFC_SET_NONCE2(otfc, u32PR, psKeyTable->u32Nonce2);

    return OTFC_OK;
}

/** @} end of group OTFC_EXPORTED_FUNCTIONS */
/** @} end of group OTFC_Driver */
/** @} end of group Standard_Driver */
