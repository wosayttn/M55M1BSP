/**************************************************************************//**
 * @file     awf.h
 * @version  V1.00
 * @brief    AWF driver header file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#ifndef __AWF_H__
#define __AWF_H__

#ifdef __cplusplus
extern "C"
{
#endif


/**
  * @brief      Get AWF HTH Interrupt Flag
  * @retval     0   AWF HTH Interrupt event has not occurred
  * @retval     1   AWF HTH Interrupt event occurs
  * @details    This macro indicates AWF HTH Interrupt occurred or not.
  * \hideinitializer
  */
#define AWF_GET_HTH_INT_FLAG()          ((AWF->STATUS) & AWF_STATUS_HTHIS_Msk)

/**
  * @brief      Get AWF LTH Interrupt Flag
  * @retval     0   AWF LTH Interrupt event has not occurred
  * @retval     1   AWF LTH Interrupt event occurs
  * @details    This macro indicates AWF LTH Interrupt occurred or not.
  * \hideinitializer
  */
#define AWF_GET_LTH_INT_FLAG()          ((AWF->STATUS) & AWF_STATUS_LTHIS_Msk)

/**
  * @brief      Get AWF Wake Up Flag
  * @retval     0   AWF wake up event has not occurred
  * @retval     1   AWF wake up event occurs
  * @details    This macro indicates AWF wake up event occurred or not.
  * \hideinitializer
  */
#define AWF_GET_WAKEUP_FLAG()       ((AWF->STATUS) & AWF_STATUS_WAKEUP_Msk)

/**
  * @brief      Clear AWF HTH Interrupt Flag
  * @retval     None
  * @details    Write 1 to clear HTH interrupt flag.
  * \hideinitializer
  */
#define AWF_CLEAR_HTH_INT_FLAG()       ((AWF->STATUS) |= AWF_STATUS_HTHIS_Msk)

/**
  * @brief      Clear AWF LTH Interrupt Flag
  * @retval     None
  * @details    Write 1 to clear LTH interrupt flag.
  * \hideinitializer
  */
#define AWF_CLEAR_LTH_INT_FLAG()       ((AWF->STATUS) |= AWF_STATUS_LTHIS_Msk)

/**
  * @brief      Clear AWF Wake Up Flag
  * @retval     None
  * @details    Write 1 to clear wake-up flag.
  * \hideinitializer
  */
#define AWF_CLEAR_WAKEUP_FLAG()       ((AWF->STATUS) |= AWF_STATUS_WAKEUP_Msk)



void AWF_EnableInt(uint32_t u32IntMask);
void AWF_DisableInt(uint32_t u32IntMask);
void AWF_EnableTWK(uint32_t u32TWKMask);
void AWF_DisableTWK(uint32_t u32TWKMask);
void AWF_SetAccumlationCount(uint32_t u32Count);
void AWF_SetHTHValue(uint32_t u32Value);
void AWF_SetLTHValue(uint32_t u32Value);
uint32_t AWF_GetAccumlationValue(void);
void AWF_SetWBINITValue(uint32_t u32Value);

/** @} end of group AWF_EXPORTED_FUNCTIONS */
/** @} end of group AWF_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __AWF_H__ */
