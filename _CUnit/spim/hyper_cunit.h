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

#ifndef _SPIM_HYPER_CUNIT_H_
#define _SPIM_HYPER_CUNIT_H_

//------------------------------------------------------------------------------
#define HRAM_START_ADDR               (0)       //(0x4000)
#define HRAM_JUMP_SIZE                (0x200)   //(0x4000)
#define HRAM_MAX_SIZE                 (0x1000)  //(HRAM_JUMP_SIZE * 4)
#define HRAM_PAGE_SIZE                (0x1000)  //(TEST_BUFF_SIZE * 4)

//------------------------------------------------------------------------------
int SPIM_Tests_HyperInit(void);
void SPIM_HYPER_EraseHRAM(SPIM_T *pSPIMx, uint32_t u32StartAddr, uint32_t u32EraseSize);
void TrainingDllLatency();

void SPIM_Const_HYPER_REG();
void SPIM_Const_HYPER_CMD();
void SPIM_Const_HYPER_CONFIG1();

void MACRO_SPIM_HYPER_CONFIG1();

extern CU_SuiteInfo spim_hyper_suites[];

extern CU_TestInfo SPIM_HyperConstantTests[];
extern CU_TestInfo SPIM_HyperMacroTests[];

#endif /* _SPIM_HYPER_CUNIT_H_ */
