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
#include "lpadc_cunit.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Define local functions                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
int suite_success_init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
    
     /* Enable ADC module clock */
    CLK_EnableModuleClock(LPADC0_MODULE);
    /* Set ADC clock source to PCLK4/4=50MHz, set divider to 4, ADC clock will be 50/2 MHz */
    CLK_SetModuleClock(LPADC0_MODULE, CLK_LPADCSEL_LPADC0SEL_PCLK4, CLK_LPADCDIV_LPADC0DIV(1));

    /* Reset IP */
    SYS_ResetModule(SYS_LPADC0RST);
    
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
    {"S1T1-LPADC CONSTANT",  CU_S1T1_CONSTANT_LPADC},
    CU_TEST_INFO_NULL
};


CU_TestInfo Suite_2_MACRO[] =
{
    {"S2T1-LPADC MACRO Basic EADC Conversion",   CU_S2T1_MACRO_LPADC_BASIC},
    {"S2T2-LPADC MACRO EADC Conversion Compare", CU_S2T2_MACRO_LPADC_COMP},
    {"S2T4-LPADC MACRO Others",                  CU_S2T4_MACRO_LPADC_OTHER},
    CU_TEST_INFO_NULL
};


CU_TestInfo Suite_3_API[] =
{
    {"LPADC API function Test",  CU_LPADC_API_Test},
    CU_TEST_INFO_NULL
};


/*---------------------------------------------------------------------------------------------------------*/
/* Define 1st layer Suite Items                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
CU_SuiteInfo LPADC_suites[] =
{
    {"S1-LPADC CONSTANT test",   suite_success_init, suite_success_clean, NULL, NULL, Suite_1_CONSTANT},
    {"S2-LPADC MACRO test",      suite_success_init, suite_success_clean, NULL, NULL, Suite_2_MACRO},
    {"S3-LPADC API test",        suite_success_init, suite_success_clean, NULL, NULL, Suite_3_API},
    CU_SUITE_INFO_NULL
};
