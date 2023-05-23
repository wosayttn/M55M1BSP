/****************************************************************************
*                                                                           *
* Copyright (c) 2014 - 2015 Nuvoton Technology Corp. All rights reserved.  *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   crypto_cunit.h
*
* VERSION
*   1.0
*
* DESCRIPTION
*   The header file of CUnit test function
*
* HISTORY
*   2016-08-31    Ver 1.0 Created by YCHuang
*
* REMARK
*   None
****************************************************************************/

#ifndef _SPIM_CUNIT_H_
#define _SPIM_CUNIT_H_

//------------------------------------------------------------------------------
#define TEST_FLASH_SIZE             (4 * 0x100000)//(32 * 0x100000)
#define FLASH_PAGE_SIZE             (0x10000)
#define TEST_JUMP_SIZE              (0x100000) //(0x900000) // (0x400000)
#define TEST_START_ADDR             (0x10000) //(FLASH_PAGE_SIZE * 1)
#define TEST_BUFF_SIZE              (0x200)//(2 * 1024)

#define TRANS_MODE_NOR                  (1 << 1) //normal mode
#define TRANS_MODE_CONT                 (1 << 2) //continue read
#define TRANS_MODE_WRAP_8Bit            (1 << 3) //WRAP Around Mode 8Bit
#define TRANS_MODE_WRAP_16Bit           (1 << 4) //WRAP Around Mode 16Bit
#define TRANS_MODE_WRAP_32Bit           (1 << 5) //WRAP Around Mode 32Bit
#define TRANS_MODE_WRAP_64Bit           (1 << 6) //WRAP Around Mode 64Bit

//------------------------------------------------------------------------------

extern CU_SuiteInfo suites[];
extern CU_TestInfo SPIM_ConstantTests[];
extern CU_TestInfo SPIM_MacroTests[];
extern CU_TestInfo SPIM_ApiTests[];

#endif /* _SPIM_CUNIT_H_ */
