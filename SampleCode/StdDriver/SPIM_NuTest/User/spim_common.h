/**************************************************************************//**
 * @file     spim_common.h
 * @version  V1.00
 * @brief    spim_common driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include <stdint.h>

#ifndef __SPIM_COMMON_H__
#define __SPIM_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup SPIM_COMMON_Driver SPIM_COMMON Driver
  @{
*/

/** @addtogroup SPIM_COMMON_EXPORTED_CONSTANTS SPIM_COMMON Exported Constants
  @{
*/

//------------------------------------------------------------------------------
// MACROS AND DEFINES
//------------------------------------------------------------------------------
/* SPIM Port Index */
#define C_SPIM0                     (0)
#define C_SPIM1                     (1)

/* SPIM Test Size */
//#define SPIM_DMM_TEST_SIZE          (0x800) //Maximum size for DMM mode testing

#define TEST_FLASH_SIZE             (4 * 0x100000)//(32 * 0x100000)
#define FLASH_PAGE_SIZE             (0x10000)
#define TEST_JUMP_SIZE              (0x100000) //(0x900000) // (0x400000)
#define TEST_START_ADDR             (0x10000) //(FLASH_PAGE_SIZE * 1)
#define TEST_BUFF_SIZE              (0x200)//(2 * 1024)

extern __attribute__((aligned(32))) uint8_t tstbuf[TEST_BUFF_SIZE];
extern __attribute__((aligned(32))) uint8_t tstbuf2[TEST_BUFF_SIZE];
extern __attribute__((aligned(32))) uint8_t tstbuf3[TEST_BUFF_SIZE];

//------------------------------------------------------------------------------
// GLOBAL FUNCTION PROTOTYPES
//------------------------------------------------------------------------------
void SPIM_DisableModule(uint32_t u32SPIMModule);
void SPIM_EanbleModule(uint32_t u32SPIMModule);

void SPIM_DisableInt(uint32_t u32SPIMModule);
void SPIM_EnableInt(uint32_t u32SPIMModule);

void *SPIM_GetModule(uint32_t u32Index);
uint32_t SPIM_GetClkDivNum(void);
int SPIM_SelectModule(uint32_t u32SPIMPort);
void SPIM_SysReset(uint32_t u32SPIMModule);


/** @} end of group SPIM_COMMON_EXPORTED_FUNCTIONS */
/** @} end of group SPIM_COMMON_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif  /* __SPIM_COMMON_H__ */
