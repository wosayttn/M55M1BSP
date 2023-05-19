#ifndef _CLK_CUNIT_H_
#define _CLK_CUNIT_H_

extern CU_SuiteInfo CLKSuites[];
extern CU_TestInfo  CLK_ConstTest[];
extern CU_TestInfo  CLK_MacroTest[];
extern CU_TestInfo  CLK_FuncTest[];

#define CLK_CLKOSEL_CLKOSEL_SOF            (0xdUL << CLK_CLKOSEL_CLKOSEL_Pos)          /*!< Select CLKO clock source from LIRC \hideinitializer */

#endif
