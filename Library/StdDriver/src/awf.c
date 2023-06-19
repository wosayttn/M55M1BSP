/**************************************************************************//**
 * @file     awf.c
 * @version  V1.00
 * @brief    AWF driver source file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
    @{
*/

/** @addtogroup AWF_Driver AWF Driver
    @{
*/

/** @addtogroup AWF_EXPORTED_FUNCTIONS AWF Exported Functions
    @{
*/

/**
  * @brief      Get Accumlation Value.
  * @return     Accumlation value.
  * @details    This API is used to get Accumlation value.
  *             Accumulated value will be accumulated from word0 to word[ACUCNT-1].
  */
uint32_t AWF_GetAccumlationValue(void)
{
    return (AWF->ACUVAL) & AWF_ACUVAL_ACUVAL_Msk;
}

/**
  * @brief      This API is used to enable awf function
  * @param[in]  u32IntEn is interrupt selection.
  *             - \ref AWF_HTINT_ENABLE
  *             - \ref AWF_LTINT_ENABLE
  *             - \ref AWF_BOTHINT_ENABLE
  *             - \ref AWF_BOTHINT_DISABLE
  * @param[in]  u32WakeupEn
  *             - \ref AWF_HTWK_ENABLE
  *             - \ref AWF_LTWK_ENABLE
  *             - \ref AWF_BOTHWK_ENABLE
  *             - \ref AWF_BOTHWK_DISABLE
  * @param[in]  u32ACCCount is AccumlationCount. It could be 2~8.
  * @param[in]  u32HTHValue is HTH Value. It could be 0~524288.
  * @param[in]  u32LTHValue is LTH Value. It could be 0~524288.
  * @param[in]  u32WBINITValue is WBINIT Value. It could be 0~65535.
  * @details    Determine interrupt and wake-up enable or disable. \n
  *             Set suitable threshold and word buffer initial value in this API.
  */
void AWF_Open(uint32_t u32IntEn, uint32_t u32WakeupEn, uint32_t u32HTHValue, uint32_t u32LTHValue, uint32_t u32WBINITValue, uint32_t u32ACCCount)
{
    /* Set interrupt function*/
    AWF->CTL = ((AWF->CTL & ~(AWF_CTL_HTIEN_Msk | AWF_CTL_LTIEN_Msk | AWF_CTL_HTWKEN_Msk | AWF_CTL_LTWKEN_Msk)) | u32IntEn | u32WakeupEn);

    /* Set Wake-up Function*/
    AWF->CTL = ((AWF->CTL & ~AWF_CTL_ACUCNT_Msk) | (u32ACCCount << AWF_CTL_ACUCNT_Pos));

    /* Set high threshold value*/
    AWF_SET_HTH_VALUE(u32HTHValue);

    /* Set low threshold value*/
    AWF_SET_LTH_VALUE(u32LTHValue);

    /* Set word buffer initial value*/
    AWF_SET_WBINIT_VALUE(u32WBINITValue);
}

/**
  * @brief      This API is used to disable awf function
  */
void AWF_Close(void)
{
    /* Clear all setting value */
    AWF->CTL = AWF_CLOSE;
    AWF_SET_WBINIT_VALUE(AWF_CLOSE);
    AWF_SET_HTH_VALUE(AWF_CLOSE);
    AWF_SET_LTH_VALUE(AWF_CLOSE);
}

/** @} end of group AWF_EXPORTED_FUNCTIONS */
/** @} end of group AWF_Driver */
/** @} end of group Standard_Driver */
