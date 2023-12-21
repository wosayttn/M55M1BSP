/***************************************************************************//**
 * @file     targetdev.h
 * @version  V1.00
 * @brief    ISP support function header file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __TARGET_H__
#define __TARGET_H__

#ifdef __cplusplus
extern "C"
{
#endif

// Nuvoton MCU Peripheral Access Layer Header File
#include "NuMicro.h"

#define DETECT_PIN       PB12

extern void GetDataFlashInfo(uint32_t *pu32Addr, uint32_t *pu32Size);
extern uint32_t GetApromSize(void);
extern uint32_t g_u32ApromSize, g_u32DataFlashAddr, g_u32DataFlashSize;

#ifdef __cplusplus
}
#endif

#endif  /* __TARGET_H__ */
