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
//------------------------------------------------------------------------------
// MACROS AND DEFINES
//------------------------------------------------------------------------------
#ifdef FOR_SIMULATION
#define log_info(...)
#else
#define log_info printf
#endif

#define RED_BOLD            "\x1b[;31;1m"
#define BLU_BOLD            "\x1b[;34;1m"
#define YEL_BOLD            "\x1b[;33;1m"
#define GRN_BOLD            "\x1b[;32;1m"
#define CYAN_BOLD_ITALIC    "\x1b[;36;1;3m"
#define RESET               "\x1b[0;m"

/**
  * @brief Get a 64-bit unsigned value from specified address
  * @param[in] addr Address to get 64-bit data from
  * @return  64-bit unsigned value stored in specified address
  * @note The input address must be 64-bit aligned
  */
#define M64(addr) (*((vu64 *) (addr)))

/**
  * @brief Set a 64-bit unsigned value to specified I/O port
  * @param[in] port Port address to set 64-bit data
  * @param[in] value Value to write to I/O port
  * @return  None
  * @note The output port must be 64-bit aligned
  */
#define outpll(port,value)   (*((volatile unsigned long long *)(port))=(value))

/**
  * @brief Get a 64-bit unsigned value from specified I/O port
  * @param[in] port Port address to get 64-bit data from
  * @return  64-bit unsigned value stored in specified I/O port
  * @note The input port must be 64-bit aligned
  */
#define inpll(port)          ((*((volatile unsigned long long *)(port))))


#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(x) (sizeof((*x)) / sizeof((x[0])))
#endif

//------------------------------------------------------------------------------
// TYPEDEFS
//------------------------------------------------------------------------------
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
// GLOBAL FUNCTION PROTOTYPES
//------------------------------------------------------------------------------
uint32_t sysGetNum(void);
uint32_t GetRequireOptions(S_TestOption *pOption, uint32_t u32Size);
void RunTestFunction(S_TestMenu *pTestMenu, uint32_t u32MenuSize, uint32_t u32SPIMModule);
void dump_compare_error(uint32_t addr, uint8_t *buf_expect, uint8_t *buf_compare, int count);
void HexDump(uint8_t *pucBuff, int nSize);
void popDat(uint8_t *pu8buf, uint32_t bufSize);

/** @} end of group COMMON_EXPORTED_FUNCTIONS */
/** @} end of group COMMON_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif  /* __COMMON_H__ */
