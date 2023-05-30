/****************************************************************************
*                                                                           *
* Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.          *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   spi_i2s_cunit.h
*
* VERSION
*   1.0
*
* DESCRIPTION
*   The header file of CUnit test function
*
* HISTORY
*   2007-06-25    Ver 1.0 Created by NS21 WTLiu
*
* REMARK
*   None
****************************************************************************/

#ifndef _TEST_FUN_H_
#define _TEST_FUN_H_

#include <stdint.h>

void SetSPIModuleIdx(uint32_t u32SetValue);

extern CU_SuiteInfo suites[];
extern CU_TestInfo SPI_I2S_MacroTests[];
extern CU_TestInfo SPI_I2S_ApiTests[];

#endif /* _TEST_FUN_H_ */
