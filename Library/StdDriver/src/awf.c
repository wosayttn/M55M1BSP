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
  * @brief      Enable AWF Interrupt
  * @param[in]  u32IntFlagMask Specify the interrupt source. Including :
  *             - \ref AWF_CTL_HTIEN_Msk
  *             - \ref AWF_CTL_LTIEN_Msk
  * @return     None
  * @details    This API is used to enable the specify AWF interrupt function.
  */
void AWF_EnableInt(uint32_t u32IntMask)
{
    AWF->CTL |= u32IntMask;
}

/**
  * @brief      Disable AWF Interrupt
  * @param[in]  u32IntFlagMask Specify the interrupt source. Including :
  *             - \ref AWF_CTL_HTIEN_Msk
  *             - \ref AWF_CTL_LTIEN_Msk
  * @return     None
  * @details    This API is used to disable the specify AWF interrupt function.
  */
void AWF_DisableInt(uint32_t u32IntMask)
{
    AWF->CTL &= ~u32IntMask;
}

/**
  * @brief      Enable Threshold Wake-up
  * @param[in]  u32TWKMask Specify the threshold. Including :
  *             - \ref AWF_CTL_HTWKEN_Msk
  *             - \ref AWF_CTL_LTWKEN_Msk
  * @return     None
  * @details    This API is used to enable high threshold or low threshold Wake-up function.
  */
void AWF_EnableTWK(uint32_t u32TWKMask)
{
    AWF->CTL |= u32TWKMask;
}

/**
  * @brief      Disable Threshold Wake-up
  * @param[in]  u32TWKMask Specify the threshold. Including :
  *             - \ref AWF_CTL_HTWKEN_Msk
  *             - \ref AWF_CTL_LTWKEN_Msk
  * @return     None
  * @details    This API is used to enable high threshold or low threshold Wake-up function.
  */
void AWF_DisableTWK(uint32_t u32TWKMask)
{
    AWF->CTL &= ~u32TWKMask;
}

/**
  * @brief      Set Accumlation Count
  * @param[in]  u32Count is AccumlationCount. It could be 2~7.
  * @return     None
  * @details    This API is used to set accumlation count.
  */
void AWF_SetAccumlationCount(uint32_t u32Count)
{
    AWF->CTL = (AWF->CTL & ~AWF_CTL_ACUCNT_Msk) | (u32Count << AWF_CTL_ACUCNT_Pos);
}

/**
  * @brief      Set HTH Value
  * @param[in]  u32Value is HTH Value. It could be 0~524288.
  * @return     None
  * @details    This API is used to set high threshold value.
  */
void AWF_SetHTHValue(uint32_t u32Value)
{
    AWF->HTH = (AWF->HTH & ~AWF_HTH_AWFHTH_Msk) | (u32Value << AWF_HTH_AWFHTH_Pos);
}

/**
  * @brief      Set LTH Value
  * @param[in]  u32Value is LTH Value. It could be 0~524288.
  * @return     None
  * @details    This API is used to set low threshold value.
  */
void AWF_SetLTHValue(uint32_t u32Value)
{
    AWF->LTH = (AWF->LTH & ~AWF_LTH_AWFLTH_Msk) | (u32Value << AWF_LTH_AWFLTH_Pos);
}

/**
  * @brief      Set AWF Word Buffer Initial Value
  * @param[in]  u32Value is WBINIT Value. It could be 0~65535.
  * @return     None
  * @details    This API is used to set AWF word buffer initial value.
  */
void AWF_SetWBINITValue(uint32_t u32Value)
{
    AWF->WBINIT = (AWF->WBINIT & ~AWF_WBINIT_WBINIT_Msk) | (u32Value << AWF_WBINIT_WBINIT_Pos);
}

/**
  * @brief      Get Accumlation Value
  * @param[in]  None
  * @return     Accumlation value.
  * @details    This API is used to get Accumlation value.
  *             Accumulated value will be accumulated from word0 to word[ACUCNT-1]. 
  */
uint32_t AWF_GetAccumlationValue(void)
{
    return (AWF->ACUVAL) & AWF_ACUVAL_ACUVAL_Msk;
}

/** @} end of group AWF_EXPORTED_FUNCTIONS */
/** @} end of group AWF_Driver */
/** @} end of group Standard_Driver */
