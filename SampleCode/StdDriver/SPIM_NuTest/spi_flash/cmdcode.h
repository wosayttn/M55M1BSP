/**************************************************************************//**
 * @file     cmdcode.h
 * @version  V1.00
 * @brief    cmdcode driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include <stdint.h>

#ifndef __CMDCODE_H__
#define __CMDCODE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup CMDCODE_Driver CMDCODE Driver
  @{
*/

/** @addtogroup CMDCODE_EXPORTED_CONSTANTS CMDCODE Exported Constants
  @{
*/

//------------------------------------------------------------------------------
// MACROS AND DEFINES
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GLOBAL FUNCTION PROTOTYPES
//------------------------------------------------------------------------------
void *SPIM_GetWinbondWrCMD(uint32_t u32Idx);
void *SPIM_GetWinbondRdCMD(uint32_t u32Idx);
int Flash_PhaseTableLength(SPIM_PHASE_T *pPhase);

/** @} end of group CMDCODE_EXPORTED_FUNCTIONS */
/** @} end of group CMDCODE_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif  /* __CMDCODE_H__ */
