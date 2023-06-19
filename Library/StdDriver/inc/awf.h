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

/*---------------------------------------------------------------------------------------------------------*/
/*  AWF constant definitions.                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define AWF_HTINT_ENABLE        (AWF_CTL_HTIEN_Msk)
#define AWF_LTINT_ENABLE        (AWF_CTL_LTIEN_Msk)
#define AWF_BOTHINT_ENABLE      (AWF_CTL_HTIEN_Msk | AWF_CTL_LTIEN_Msk)
#define AWF_BOTHINT_DISABLE     (0x0UL)
#define AWF_HTWK_ENABLE         (AWF_CTL_HTWKEN_Msk)
#define AWF_LTWK_ENABLE         (AWF_CTL_LTWKEN_Msk)
#define AWF_BOTHWK_ENABLE       (AWF_CTL_HTWKEN_Msk | AWF_CTL_LTWKEN_Msk)
#define AWF_BOTHWK_DISABLE      (0x0UL)
#define AWF_CLOSE               (0x0UL)


/**
  * @brief      This macro is used to set accumlation count.
  * @param[in]  u32Count is AccumlationCount. It could be 2~8.
  */
#define AWF_SET_ACC_COUNT(u32Count)     (AWF->CTL = (AWF->CTL & ~AWF_CTL_ACUCNT_Msk) | (u32Count << AWF_CTL_ACUCNT_Pos))

/**
  * @brief      This macro is used to set high threshold value.
  * @param[in]  u32Value is HTH Value. It could be 0~524288.
  */
#define AWF_SET_HTH_VALUE(u32Value)        (AWF->HTH = (AWF->HTH & ~AWF_HTH_AWFHTH_Msk) | (u32Value << AWF_HTH_AWFHTH_Pos))

/**
  * @brief      This macro is used to set low threshold value.
  * @param[in]  u32Value is LTH Value. It could be 0~524288.
  */
#define AWF_SET_LTH_VALUE(u32Value)        (AWF->LTH = (AWF->LTH & ~AWF_LTH_AWFLTH_Msk) | (u32Value << AWF_LTH_AWFLTH_Pos))

/**
  * @brief      This macro is used to set AWF word buffer initial value.
  * @param[in]  u32Value is WBINIT Value. It could be 0~65535.
  */
#define AWF_SET_WBINIT_VALUE(u32Value)      (AWF->WBINIT = (AWF->WBINIT & ~AWF_WBINIT_WBINIT_Msk) | (u32Value << AWF_WBINIT_WBINIT_Pos))

/**
  * @brief      This macro is used to Write AWF data holding register value.
  * @param[in]  u32Value is WBINIT Value. It could be 0~65535.
  */
#define AWF_WRITE_DAT(u32Value)     (AWF->DAT = u32Value)

/**
  * @brief      Get AWF HTH Interrupt Flag
  * @retval     0   AWF HTH Interrupt event has not occurred
  * @retval     1   AWF HTH Interrupt event occurs
  * @details    This macro indicates AWF HTH Interrupt occurred or not.
  * \hideinitializer
  */
#define AWF_GET_HTH_INT_FLAG()          ((AWF->STATUS & AWF_STATUS_HTHIS_Msk) >> AWF_STATUS_HTHIS_Pos)

/**
  * @brief      Get AWF LTH Interrupt Flag
  * @retval     0   AWF LTH Interrupt event has not occurred
  * @retval     1   AWF LTH Interrupt event occurs
  * @details    This macro indicates AWF LTH Interrupt occurred or not.
  * \hideinitializer
  */
#define AWF_GET_LTH_INT_FLAG()          ((AWF->STATUS & AWF_STATUS_LTHIS_Msk) >> AWF_STATUS_LTHIS_Pos)

/**
  * @brief      Clear AWF HTH Interrupt Flag
  * @details    Write 1 to clear HTH interrupt flag.
  * \hideinitializer
  */
#define AWF_CLEAR_HTH_INT_FLAG()       (AWF->STATUS |= AWF_STATUS_HTHIS_Msk)

/**
  * @brief      Clear AWF LTH Interrupt Flag
  * @details    Write 1 to clear LTH interrupt flag.
  * \hideinitializer
  */
#define AWF_CLEAR_LTH_INT_FLAG()       (AWF->STATUS |= AWF_STATUS_LTHIS_Msk)

/**
  * @brief      Enable AWF Interrupt
  * @param[in]  u32IntMask Specify the interrupt source. Including:
  *             - \ref AWF_CTL_HTIEN_Msk
  *             - \ref AWF_CTL_LTIEN_Msk
  * @details    This API is used to enable the specify AWF interrupt function.
  */
__STATIC_INLINE void AWF_EnableInt(uint32_t u32IntMask)
{
    AWF->CTL |= u32IntMask;
}

/**
  * @brief      Disable AWF Interrupt
  * @param[in]  u32IntMask Specify the interrupt source. Including:
  *             - \ref AWF_CTL_HTIEN_Msk
  *             - \ref AWF_CTL_LTIEN_Msk
  * @details    This API is used to disable the specify AWF interrupt function.
  */
__STATIC_INLINE void AWF_DisableInt(uint32_t u32IntMask)
{
    AWF->CTL &= ~u32IntMask;
}

/**
  * @brief      Enable Threshold Wake-up
  * @param[in]  u32TWKMask Specify the threshold. Including:
  *             - \ref AWF_CTL_HTWKEN_Msk
  *             - \ref AWF_CTL_LTWKEN_Msk
  * @details    This API is used to enable high threshold or low threshold Wake-up function.
  */
__STATIC_INLINE void AWF_EnableWakeup(uint32_t u32TWKMask)
{
    AWF->CTL |= u32TWKMask;
}

/**
  * @brief      Disable Threshold Wake-up
  * @param[in]  u32TWKMask Specify the threshold. Including:
  *             - \ref AWF_CTL_HTWKEN_Msk
  *             - \ref AWF_CTL_LTWKEN_Msk
  * @details    This API is used to enable high threshold or low threshold Wake-up function.
  */
__STATIC_INLINE void AWF_DisableWakeup(uint32_t u32TWKMask)
{
    AWF->CTL &= ~u32TWKMask;
}

void AWF_EnableInt(uint32_t u32IntMask);
void AWF_DisableInt(uint32_t u32IntMask);
void AWF_EnableWakeup(uint32_t u32TWKMask);
void AWF_DisableWakeup(uint32_t u32TWKMask);
uint32_t AWF_GetAccumlationValue(void);
void AWF_Open(uint32_t u32IntEn, uint32_t u32WakeupEn, uint32_t u32HTHValue, uint32_t u32LTHValue, uint32_t u32WBINITValue, uint32_t u32ACCCount);
void AWF_Close(void);

/** @} end of group AWF_EXPORTED_FUNCTIONS */
/** @} end of group AWF_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __AWF_H__ */
