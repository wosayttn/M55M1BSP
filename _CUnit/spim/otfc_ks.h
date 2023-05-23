/**************************************************************************//**
 * @file    otfc.h
 * @version V1.00
 * @brief   OTFC driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#ifndef __OTFC_KS_H__
#define __OTFC_KS_H__

#include <stdint.h>

/*----------------------------------------------------------------------------*/
/* Include related headers                                                    */
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup OTFC_Driver OTFC Driver
  @{
*/

/** @addtogroup OTFC_EXPORTED_CONSTANTS OTFC Exported Constants
  @{
*/

/*----------------------------------------------------------------------------*/
/* Define Function Prototypes                                                 */
/*----------------------------------------------------------------------------*/
int32_t KS_SetAESToKeyStoreSRAM(KS_MEM_Type eMemType, uint32_t u32Readable, uint32_t au32Key[]);

int32_t KS_SetAESToKeyStoreOTP(int i32KeyIdx, uint32_t *pu32AESKey);
int32_t KS_EraseAESToKeyStoreOTP(int i32KeyIdx);

/** @} end of group OTFC_EXPORTED_FUNCTIONS */
/** @} end of group OTFC_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __OTFC_KS_H__ */
