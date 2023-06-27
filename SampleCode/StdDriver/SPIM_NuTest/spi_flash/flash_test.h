/**************************************************************************//**
 * @file     flash_test.h
 * @version  V1.00
 * @brief    flash_test driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include <stdint.h>

#ifndef __FLASH_TEST_H__
#define __FLASH_TEST_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup FLASH_TEST_Driver FLASH_TEST Driver
  @{
*/

/** @addtogroup FLASH_TEST_EXPORTED_CONSTANTS FLASH_TEST Exported Constants
  @{
*/

//------------------------------------------------------------------------------
// MACROS AND DEFINES
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GLOBAL FUNCTION PROTOTYPES
//------------------------------------------------------------------------------
/* SPI Flash Test Menu */
int Flash_TestMenu(uint32_t u32SPIMModule);

/* SPI Flash Test function */
int Flash_IDTest(uint32_t u32SPIMModule);
int Flash_DMA_Test(uint32_t u32SPIMModule);

// Normal I/O mode Test
int Flash_IO_WriteTest(uint32_t u32SPIMModule);
int Flash_IO_ReadTest(uint32_t u32SPIMModule);

/** @} end of group FLASH_TEST_EXPORTED_FUNCTIONS */
/** @} end of group FLASH_TEST_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif  /* __FLASH_TEST_H__ */
