/******************************************************************************
* @file    fmc_cunit.h
* @version V1.00
* @brief   FMC CUnit Test
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#ifndef __FMC_CUNIT_H__
#define __FMC_CUNIT_H__

#define TEST_FLASH_ADDR          (uint32_t)(FMC_APROM_BASE + 0x20000)
#define TEST_DATA_FLASH_BASE     TEST_FLASH_ADDR
#define TEST_VECMAP_ADDR         (uint32_t)(FMC_APROM_BASE + 0x18000)

extern CU_SuiteInfo FMC_Suites[];
extern CU_TestInfo  FMC_ConstTest[];
extern CU_TestInfo  FMC_MacroTest[];
extern CU_TestInfo  FMC_FuncTest[];

#endif  // __FMC_CUNIT_H__
