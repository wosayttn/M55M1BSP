/****************************************************************************
*                                                                           *
* Copyright (c) 2004 - 2007 Winbond Electronics Corp. All rights reserved.  *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   test_fun.h
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

#ifndef _SDH_CUNIT_H_
#define _SDH_CUNIT_H_

int SDH_ModuleSelect(void);

extern CU_SuiteInfo suites[];
extern CU_TestInfo SDH_CONSTANT_MACRO[];
extern CU_TestInfo SDH_API[];

#endif /* _SDH_CUNIT_H_ */