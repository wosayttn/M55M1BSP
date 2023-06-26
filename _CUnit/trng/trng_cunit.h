/****************************************************************************
*                                                                           *
* Copyright (c) 2014 - 2015 Nuvoton Technology Corp. All rights reserved.  *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   trng_cunit.h
*
* VERSION
*   1.0
*
* DESCRIPTION
*   The header file of CUnit test function
*
* HISTORY
*   2015-09-14    Ver 1.0 Created by YCHuang
*
* REMARK
*   None
****************************************************************************/

#ifndef _CRYPTO_CUNIT_H_
#define _CRYPTO_CUNIT_H_

extern CU_SuiteInfo Trng_suites[];
extern CU_TestInfo Trng_ConstantTests[];
extern CU_TestInfo Trng_MacroTests[];
extern CU_TestInfo Trng_ApiTests[];

#endif
