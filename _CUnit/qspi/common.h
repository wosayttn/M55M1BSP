/**************************************************************************//**
 * @file     common.h
 * @version  V1.00
 * @brief    common driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include <stdint.h>

#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup COMMON_Driver COMMON Driver
  @{
*/

/** @addtogroup COMMON_EXPORTED_CONSTANTS COMMON Exported Constants
  @{
*/

typedef struct
{
    char *chName;
} S_TestOption;

typedef struct
{
    char *chName;
    int (*fp)(uint32_t);
    uint32_t u32AutoTest;
} S_TestMenu;

//------------------------------------------------------------------------------
uint32_t sysGetNum(void);
uint32_t GetRequireOptions(S_TestOption *pOption, uint32_t u32Size);
void RunTestFunction(S_TestMenu *pTestMenu, uint32_t u32MenuSize, uint32_t u32SPIMModule);

/** @} end of group COMMON_EXPORTED_FUNCTIONS */
/** @} end of group COMMON_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif  /* __COMMON_H__ */
