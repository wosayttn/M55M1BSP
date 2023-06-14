/**************************************************************************//**
 * @file    hyperram_init.h
 * @version V1.00
 * @brief   HyerRAM driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#ifndef __HYPERRAM_INIT_H__
#define __HYPERRAM_INIT_H__

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

//------------------------------------------------------------------------------
// HyerRAM Test Port
//------------------------------------------------------------------------------
#define DLL_MAX_DELAY_NUM                   (0x05) //0x1F

#define TEST_BUFF_SIZE                      (0x200)

/** @addtogroup HyerRAM_Driver OTFC Driver
  @{
*/

/** @addtogroup HyerRAM_EXPORTED_CONSTANTS HyerRAM Exported Constants
  @{
*/

/*----------------------------------------------------------------------------*/
/* Define Function Prototypes                                                 */
/*----------------------------------------------------------------------------*/
/* SPIM Init HyperBus Mode */
void SPIM_HyperRAM_Init(SPIM_T *pSPIMx, uint32_t u32CacheOn);

#if 0
/* HyperRAM Training DLL Delay Time Function */
int SPIM_HyperRAMDLLDelayTimeTraining(SPIM_T *pSPIMx);

/* Erase HyperRAM */
void SPIM_Hyper_EraseHRAM(SPIM_T *pSPIMx, uint32_t u32StartAddr, uint32_t u32EndAddr);

/* HyperRAM Enter DMM Mode API */
uint32_t SPIM_GetDmmMapAddr(SPIM_T *pSPIMx);

/* HyperRAM CMD IO R/W API */
void SPIM_HRAM_IO_Write(SPIM_T *pSPIMx, uint32_t u32Saddr, void *pvWrBuf, uint32_t u32NTx);
void SPIM_HRAM_IO_Read(SPIM_T *pSPIMx, uint32_t u32Saddr, void *pvRdBuf, uint32_t u32NRx);
#endif
/** @} end of group HyerRAM_EXPORTED_FUNCTIONS */
/** @} end of group HyerRAM_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __HYPERRAM_INIT_H__ */
