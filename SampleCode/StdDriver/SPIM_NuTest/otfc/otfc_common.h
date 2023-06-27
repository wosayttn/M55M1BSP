/**************************************************************************//**
 * @file     otfc_common.h
 * @version  V1.00
 * @brief    otfc_common driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include <stdint.h>

#ifndef __OTFC_COMMON_H__
#define __OTFC_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup OTFC_COMMON_Driver OTFC_COMMON Driver
  @{
*/

/** @addtogroup OTFC_COMMON_EXPORTED_CONSTANTS OTFC_COMMON Exported Constants
  @{
*/

//------------------------------------------------------------------------------
// MACROS AND DEFINES
//------------------------------------------------------------------------------
/* OTFC Port Index */
#define C_OTFC0                     (0)
#define C_OTFC1                     (1)


//------------------------------------------------------------------------------
// GLOBAL FUNCTION PROTOTYPES
//------------------------------------------------------------------------------
void *OTFC_GetModule(uint32_t u32Index);

void OTFC_SetAESKey(uint8_t u8Index, uint32_t u32Key);
void *OTFC_GetAESKey(uint8_t u8Index);

void OTFC_EnableInt(uint32_t u32OTFCModule);
void OTFC_DisableInt(uint32_t u32OTFCModule);

void OTFC_SysReset(uint32_t u32Module);

void OTFC_SW_Cipher(uint8_t u8PRx, uint32_t u32StartAddr, uint32_t u32TestSize, uint8_t *u8SrcBuf, uint8_t *u8DestBuf, uint8_t u8Swap);

/** @} end of group OTFC_COMMON_EXPORTED_FUNCTIONS */
/** @} end of group OTFC_COMMON_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif  /* __OTFC_COMMON_H__ */
