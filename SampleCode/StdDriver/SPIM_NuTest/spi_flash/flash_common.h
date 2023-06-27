/**************************************************************************//**
 * @file     flash_common.h
 * @version  V1.00
 * @brief    flash_common driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include <stdint.h>

#ifndef __FLASH_COMMON_H__
#define __FLASH_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup FLASH_COMMON_Driver FLASH_COMMON Driver
  @{
*/

/** @addtogroup FLASH_COMMON_EXPORTED_CONSTANTS FLASH_COMMON Exported Constants
  @{
*/

//------------------------------------------------------------------------------
// MACROS AND DEFINES
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GLOBAL FUNCTION PROTOTYPES
//------------------------------------------------------------------------------

int Flash_IO_MakePattern(SPIM_T *pSPIMx, uint32_t u32IsCipherEn, uint32_t u32SAddr, uint32_t u32TestSize);
int Flash_EraseBlock(SPIM_T *pSPIMx, uint32_t u32SAddr, uint32_t u32Is4ByteAddr);

/** @} end of group FLASH_COMMON_EXPORTED_FUNCTIONS */
/** @} end of group FLASH_COMMON_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif  /* __FLASH_COMMON_H__ */
