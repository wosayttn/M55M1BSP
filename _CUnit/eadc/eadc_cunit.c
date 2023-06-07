/*---------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                             */
/*                                                                                                         */
/* Copyright (c) 2017 by Nuvoton Technology Corporation                                                    */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   eadc_cunit.c                                                                                           */
/*            The test function of EADC for CUnit.                                                          */
/* Project:   M251                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "eadc_cunit.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Define local functions                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
int suite_success_init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
    
     /* Enable ADC module clock */
    CLK_EnableModuleClock(EADC0_MODULE);
    /* Set ADC clock source to PCLK0/2=100MHz, set divider to 4, ADC clock will be 100/10 MHz */
    CLK_SetModuleClock(EADC0_MODULE, CLK_EADCSEL_EADC0SEL_PCLK0, CLK_EADCDIV_EADC0DIV(10));

    /* Enable ADC module clock */
    CLK_EnableModuleClock(EADC1_MODULE);
    /* Set ADC clock source to PCLK2/2=100MHz, set divider to 4, ADC clock will be 100/10 MHz */
    CLK_SetModuleClock(EADC1_MODULE, CLK_EADCSEL_EADC1SEL_PCLK2, CLK_EADCDIV_EADC1DIV(10));

    /* Reset IP */
    SYS_ResetModule(SYS_EADC0RST);
    /* Reset IP */
    SYS_ResetModule(SYS_EADC1RST);
    
    /* Lock protected registers */
    SYS_LockReg();
    return 0;
}


int suite_success_clean(void)
{
    return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Define 2nd layer Test Items                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
CU_TestInfo Suite_1_CONSTANT[] =
{
    {"S1T1-EADC CONSTANT",  CU_S1T1_CONSTANT_EADC},
    CU_TEST_INFO_NULL
};


CU_TestInfo Suite_2_MACRO[] =
{
    {"S2T1-EADC MACRO Basic EADC Conversion",   CU_S2T1_MACRO_EADC_BASIC},
    {"S2T2-EADC MACRO EADC Conversion Compare", CU_S2T2_MACRO_EADC_COMP},
    //    {"S2T3-EADC MACRO Double Buffer Mode", CU_S2T3_MACRO_EADC_DOUBLE_BUFFER},  //not release
    {"S2T4-EADC MACRO Others",                  CU_S2T4_MACRO_EADC_OTHER},
    CU_TEST_INFO_NULL
};


CU_TestInfo Suite_3_API[] =
{
    {"EADC API function Test",  CU_EADC_API_Test},
    CU_TEST_INFO_NULL
};


/*---------------------------------------------------------------------------------------------------------*/
/* Define 1st layer Suite Items                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
CU_SuiteInfo EADC_suites[] =
{
    {"S1-EADC CONSTANT test",   suite_success_init, suite_success_clean, NULL, NULL, Suite_1_CONSTANT},
    {"S2-EADC MACRO test",      suite_success_init, suite_success_clean, NULL, NULL, Suite_2_MACRO},
    {"S3-EADC API test",        suite_success_init, suite_success_clean, NULL, NULL, Suite_3_API},
    CU_SUITE_INFO_NULL
};
