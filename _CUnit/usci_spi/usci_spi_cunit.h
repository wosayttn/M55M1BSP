/****************************************************************************
*                                                                           *
* Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.          *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   usci_spi_cunit.h
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

//#define USPI_MODULE_NUM                         (2)

#define USPI_TEST_NUM                           (2)
#define USPI_FIRSTNO                            (0)

#define C_USPI0                                 (0)
#define C_USPI1                                 (1)
#define C_USPI2                                 (2)

#define EN_MULTI_USPI1                          (0)
#define EN_MULTI_USPI2                          (0)

extern CU_SuiteInfo suites[];
extern CU_TestInfo USCI_SPI_MacroTests[];
extern CU_TestInfo USCI_SPI_ApiTests[];
extern CU_TestInfo USCI_SPI_Wakeup_APIMacroTests[];

#endif /* _TEST_FUN_H_ */

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
