/****************************************************************************
* FILENAME
*   eadc_cunit.h
*
* VERSION
*   1.0
*
* DESCRIPTION
*   The header file of CUnit test function
*
* REMARK
*   None
****************************************************************************/

#ifndef __EADC_CUNIT_H__
#define __EADC_CUNIT_H__

extern CU_SuiteInfo EADC_suites[];

void CU_S1T1_CONSTANT_EADC(void);
void CU_S2T1_MACRO_EADC_BASIC(void);
void CU_S2T2_MACRO_EADC_COMP(void);
void CU_S2T3_MACRO_EADC_DOUBLE_BUFFER(void);
void CU_S2T4_MACRO_EADC_OTHER(void);
void CU_EADC_API_Test(void);

#endif  /* end of __EADC_CUNIT_H__ */
