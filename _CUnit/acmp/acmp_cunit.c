/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.                                        */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   acmp_cunit.c                                                                                          */
/*            The test function of ACMP for CUnit.                                                         */
/* Project:   M251                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"
#include "acmp_cunit.h"

extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int ACMP_Tests_Init(void)
{
    return 0;
}
int ACMP_Tests_Clean(void)
{
    return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: function_name                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               param - [in/out], parameter_description                                                   */
/*               param - [in/out], parameter_description                                                   */
/*                                                                                                         */
/* Returns:      the_value                                                                                 */
/* Side effects: the_effect                                                                                */
/* Description:                                                                                            */
/*               description                                                                               */
/*---------------------------------------------------------------------------------------------------------*/

CU_SuiteInfo ACMP_Suites[] =
{
    {"ACMP MACRO", ACMP_Tests_Init, ACMP_Tests_Clean, NULL, NULL, ACMP_MacroTests},
    {"ACMP API", ACMP_Tests_Init, ACMP_Tests_Clean, NULL, NULL, ACMP_ApiTests},
    CU_SUITE_INFO_NULL
};



void MACRO_ACMP_ENABLE_DISABLE_OUTPUT_INVERSE()
{
    CU_ASSERT_FALSE(ACMP01->CTL[0] & 0x8);
    CU_ASSERT_FALSE(ACMP01->CTL[1] & 0x8);
    ACMP_ENABLE_OUTPUT_INVERSE(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & 0x8) == ACMP_CTL_ACMPOINV_Msk);
    ACMP_ENABLE_OUTPUT_INVERSE(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[1] & 0x8) == ACMP_CTL_ACMPOINV_Msk);

    ACMP_DISABLE_OUTPUT_INVERSE(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & 0x8) == 0);
    ACMP_DISABLE_OUTPUT_INVERSE(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[1] & 0x8) == 0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);
  
    CU_ASSERT_FALSE(ACMP23->CTL[0] & 0x8);
    CU_ASSERT_FALSE(ACMP23->CTL[1] & 0x8);
    ACMP_ENABLE_OUTPUT_INVERSE(ACMP23, 0);
    CU_ASSERT((ACMP23->CTL[0] & 0x8) == ACMP_CTL_ACMPOINV_Msk);
    ACMP_ENABLE_OUTPUT_INVERSE(ACMP23, 1);
    CU_ASSERT((ACMP23->CTL[1] & 0x8) == ACMP_CTL_ACMPOINV_Msk);

    ACMP_DISABLE_OUTPUT_INVERSE(ACMP23, 0);
    CU_ASSERT((ACMP23->CTL[0] & 0x8) == 0);
    ACMP_DISABLE_OUTPUT_INVERSE(ACMP23, 1);
    CU_ASSERT((ACMP23->CTL[1] & 0x8) == 0);
  
    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP23RST);



}

void MACRO_ACMP_SET_NEG_SRC()
{
    CU_ASSERT_FALSE(ACMP01->CTL[0] & 0x70);
    ACMP_SET_NEG_SRC(ACMP01, 0, ACMP_CTL_NEGSEL_PIN);
    CU_ASSERT((ACMP01->CTL[0] & 0x70) == 0x00);
    ACMP_SET_NEG_SRC(ACMP01, 0, ACMP_CTL_NEGSEL_CRV);
    CU_ASSERT((ACMP01->CTL[0] & 0x70) == 0x10);
    ACMP_SET_NEG_SRC(ACMP01, 0, ACMP_CTL_NEGSEL_VBG);
    CU_ASSERT((ACMP01->CTL[0] & 0x70) == 0x20);
    ACMP_SET_NEG_SRC(ACMP01, 0, ACMP_CTL_NEGSEL_DAC0);
    CU_ASSERT((ACMP01->CTL[0] & 0x70) == 0x30);
    ACMP_SET_NEG_SRC(ACMP01, 0, ACMP_CTL_NEGSEL_DAC1);
    CU_ASSERT((ACMP01->CTL[0] & 0x70) == 0x40);
  
    CU_ASSERT_FALSE(ACMP01->CTL[1] & 0x70);
    ACMP_SET_NEG_SRC(ACMP01, 1, ACMP_CTL_NEGSEL_PIN);
    CU_ASSERT((ACMP01->CTL[1] & 0x70) == 0x00);
    ACMP_SET_NEG_SRC(ACMP01, 1, ACMP_CTL_NEGSEL_CRV);
    CU_ASSERT((ACMP01->CTL[1] & 0x70) == 0x10);
    ACMP_SET_NEG_SRC(ACMP01, 1, ACMP_CTL_NEGSEL_VBG);
    CU_ASSERT((ACMP01->CTL[1] & 0x70) == 0x20);
    ACMP_SET_NEG_SRC(ACMP01, 1, ACMP_CTL_NEGSEL_DAC0);
    CU_ASSERT((ACMP01->CTL[1] & 0x70) == 0x30);
    ACMP_SET_NEG_SRC(ACMP01, 1, ACMP_CTL_NEGSEL_DAC1);
    CU_ASSERT((ACMP01->CTL[1] & 0x70) == 0x40);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);

    CU_ASSERT_FALSE(ACMP23->CTL[0] & 0x70);
    ACMP_SET_NEG_SRC(ACMP23, 0, ACMP_CTL_NEGSEL_PIN);
    CU_ASSERT((ACMP23->CTL[0] & 0x70) == 0x00);
    ACMP_SET_NEG_SRC(ACMP23, 0, ACMP_CTL_NEGSEL_CRV);
    CU_ASSERT((ACMP23->CTL[0] & 0x70) == 0x10);
    ACMP_SET_NEG_SRC(ACMP23, 0, ACMP_CTL_NEGSEL_VBG);
    CU_ASSERT((ACMP23->CTL[0] & 0x70) == 0x20);
    ACMP_SET_NEG_SRC(ACMP23, 0, ACMP_CTL_NEGSEL_DAC0);
    CU_ASSERT((ACMP23->CTL[0] & 0x70) == 0x30);
    ACMP_SET_NEG_SRC(ACMP23, 0, ACMP_CTL_NEGSEL_DAC1);
    CU_ASSERT((ACMP23->CTL[0] & 0x70) == 0x40);
  
    CU_ASSERT_FALSE(ACMP23->CTL[1] & 0x70);
    ACMP_SET_NEG_SRC(ACMP23, 1, ACMP_CTL_NEGSEL_PIN);
    CU_ASSERT((ACMP23->CTL[1] & 0x70) == 0x00);
    ACMP_SET_NEG_SRC(ACMP23, 1, ACMP_CTL_NEGSEL_CRV);
    CU_ASSERT((ACMP23->CTL[1] & 0x70) == 0x10);
    ACMP_SET_NEG_SRC(ACMP23, 1, ACMP_CTL_NEGSEL_VBG);
    CU_ASSERT((ACMP23->CTL[1] & 0x70) == 0x20);
    ACMP_SET_NEG_SRC(ACMP23, 1, ACMP_CTL_NEGSEL_DAC0);
    CU_ASSERT((ACMP23->CTL[1] & 0x70) == 0x30);
    ACMP_SET_NEG_SRC(ACMP23, 1, ACMP_CTL_NEGSEL_DAC1);
    CU_ASSERT((ACMP23->CTL[1] & 0x70) == 0x40);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP23RST);
    
    
}

void MACRO_ACMP_ENABLE_DISABLE_HYSTERESIS()
{
    CU_ASSERT_FALSE(ACMP01->CTL[0] & 0x07000000);
    ACMP_ENABLE_HYSTERESIS(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & 0x07000000) == 0x04000000);
    ACMP_DISABLE_HYSTERESIS(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & 0x07000000) == 0x0);

    ACMP_CONFIG_HYSTERESIS(ACMP01, 0, ACMP_CTL_HYSTERESIS_40MV);
    CU_ASSERT((ACMP01->CTL[0] & 0x07000000) == 0x04000000);
    ACMP_CONFIG_HYSTERESIS(ACMP01, 0, ACMP_CTL_HYSTERESIS_20MV);
    CU_ASSERT((ACMP01->CTL[0] & 0x07000000) == 0x02000000);
    ACMP_CONFIG_HYSTERESIS(ACMP01, 0, ACMP_CTL_HYSTERESIS_DISABLE);
    CU_ASSERT((ACMP01->CTL[0] & 0x07000000) == 0x0);

    CU_ASSERT_FALSE(ACMP01->CTL[1] & 0x07000000);
    ACMP_ENABLE_HYSTERESIS(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[1] & 0x07000000) == 0x04000000);
    ACMP_DISABLE_HYSTERESIS(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[1] & 0x07000000) == 0x0);

    ACMP_CONFIG_HYSTERESIS(ACMP01, 1, ACMP_CTL_HYSTERESIS_40MV);
    CU_ASSERT((ACMP01->CTL[1] & 0x07000000) == 0x04000000);
    ACMP_CONFIG_HYSTERESIS(ACMP01, 1, ACMP_CTL_HYSTERESIS_20MV);
    CU_ASSERT((ACMP01->CTL[1] & 0x07000000) == 0x02000000);
    ACMP_CONFIG_HYSTERESIS(ACMP01, 1, ACMP_CTL_HYSTERESIS_DISABLE);
    CU_ASSERT((ACMP01->CTL[1] & 0x07000000) == 0x0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);

    CU_ASSERT_FALSE(ACMP23->CTL[0] & 0x07000000);
    ACMP_ENABLE_HYSTERESIS(ACMP23, 0);
    CU_ASSERT((ACMP23->CTL[0] & 0x07000000) == 0x04000000);
    ACMP_DISABLE_HYSTERESIS(ACMP23, 0);
    CU_ASSERT((ACMP23->CTL[0] & 0x07000000) == 0x0);

    ACMP_CONFIG_HYSTERESIS(ACMP23, 0, ACMP_CTL_HYSTERESIS_40MV);
    CU_ASSERT((ACMP23->CTL[0] & 0x07000000) == 0x04000000);
    ACMP_CONFIG_HYSTERESIS(ACMP23, 0, ACMP_CTL_HYSTERESIS_20MV);
    CU_ASSERT((ACMP23->CTL[0] & 0x07000000) == 0x02000000);
    ACMP_CONFIG_HYSTERESIS(ACMP23, 0, ACMP_CTL_HYSTERESIS_DISABLE);
    CU_ASSERT((ACMP23->CTL[0] & 0x07000000) == 0x0);

    CU_ASSERT_FALSE(ACMP23->CTL[1] & 0x07000000);
    ACMP_ENABLE_HYSTERESIS(ACMP23, 1);
    CU_ASSERT((ACMP23->CTL[1] & 0x07000000) == 0x04000000);
    ACMP_DISABLE_HYSTERESIS(ACMP23, 1);
    CU_ASSERT((ACMP23->CTL[1] & 0x07000000) == 0x0);

    ACMP_CONFIG_HYSTERESIS(ACMP23, 1, ACMP_CTL_HYSTERESIS_40MV);
    CU_ASSERT((ACMP23->CTL[1] & 0x07000000) == 0x04000000);
    ACMP_CONFIG_HYSTERESIS(ACMP23, 1, ACMP_CTL_HYSTERESIS_20MV);
    CU_ASSERT((ACMP23->CTL[1] & 0x07000000) == 0x02000000);
    ACMP_CONFIG_HYSTERESIS(ACMP23, 1, ACMP_CTL_HYSTERESIS_DISABLE);
    CU_ASSERT((ACMP23->CTL[1] & 0x07000000) == 0x0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP23RST);


}

void MACRO_ACMP_ENABLE_DISABLE_INT()
{
    CU_ASSERT_FALSE(ACMP01->CTL[0] & 0x2);
    ACMP_ENABLE_INT(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & 0x2) == ACMP_CTL_ACMPIE_Msk);
    ACMP_DISABLE_INT(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & 0x2) == 0x0);

    CU_ASSERT_FALSE(ACMP01->CTL[1] & 0x2);
    ACMP_ENABLE_INT(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[1] & 0x2) == ACMP_CTL_ACMPIE_Msk);
    ACMP_DISABLE_INT(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[1] & 0x2) == 0x0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);
  
    CU_ASSERT_FALSE(ACMP23->CTL[0] & 0x2);
    ACMP_ENABLE_INT(ACMP23, 0);
    CU_ASSERT((ACMP23->CTL[0] & 0x2) == ACMP_CTL_ACMPIE_Msk);
    ACMP_DISABLE_INT(ACMP23, 0);
    CU_ASSERT((ACMP23->CTL[0] & 0x2) == 0x0);

    CU_ASSERT_FALSE(ACMP23->CTL[1] & 0x2);
    ACMP_ENABLE_INT(ACMP23, 1);
    CU_ASSERT((ACMP23->CTL[1] & 0x2) == ACMP_CTL_ACMPIE_Msk);
    ACMP_DISABLE_INT(ACMP23, 1);
    CU_ASSERT((ACMP23->CTL[1] & 0x2) == 0x0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP23RST);
  
}

void MACRO_ACMP_ENABLE_DISABLE_ACMP()
{
    CU_ASSERT_FALSE(ACMP01->CTL[0] & 0x01);
    ACMP_ENABLE(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & 0x01) == 0x01);
    ACMP_DISABLE(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & 0x01) == 0x0);

    CU_ASSERT_FALSE(ACMP01->CTL[1] & 0x01);
    ACMP_ENABLE(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[1] & 0x01) == 0x01);
    ACMP_DISABLE(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[1] & 0x01) == 0x0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);
  
    CU_ASSERT_FALSE(ACMP23->CTL[0] & 0x01);
    ACMP_ENABLE(ACMP23, 0);
    CU_ASSERT((ACMP23->CTL[0] & 0x01) == 0x01);
    ACMP_DISABLE(ACMP23, 0);
    CU_ASSERT((ACMP23->CTL[0] & 0x01) == 0x0);

    CU_ASSERT_FALSE(ACMP23->CTL[1] & 0x01);
    ACMP_ENABLE(ACMP23, 1);
    CU_ASSERT((ACMP23->CTL[1] & 0x01) == 0x01);
    ACMP_DISABLE(ACMP23, 1);
    CU_ASSERT((ACMP23->CTL[1] & 0x01) == 0x0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP23RST);
  
}

void MACRO_ACMP_GET_OUTPUT()
{
    int volatile ii;
    /*for real chip using*/
    ACMP_SET_NEG_SRC(ACMP01, 0, ACMP_CTL_NEGSEL_VBG);
    /*for FPGA using*/
    //    ACMP_SET_NEG_SRC(ACMP01, 0, ACMP_CTL_NEGSEL_PIN);
    /* Set PA.11 as output mode */
    PA->MODE = 1 << 22;

    /* Set PB.3 as output mode */
    PB->MODE = 1 << 6;

    /* Enable ACMP */
    ACMP_ENABLE(ACMP01, 0);
    /* Set PA.11 (ACMP0_P0) to low level */
    PA11 = 0;
    /* Set PB.3 (ACMP0_N) to low level */
    PB3 = 0;
    /* 2 ms stable time */
    CLK_SysTickDelay(2000);
    CU_ASSERT(ACMP_GET_OUTPUT(ACMP01, 0) == 0);
    CU_ASSERT((ACMP01->STATUS & 0x10) == 0);
    /* Set PA.11 (ACMP0_P0) to high level */
    PA11 = 1;

    for (ii = 0; ii < 10000; ii++)
    __NOP();

    CU_ASSERT(ACMP_GET_OUTPUT(ACMP01, 0) == 1);
    CU_ASSERT((ACMP01->STATUS & 0x10) == 0x10);
    /*for Real chip using*/
    ACMP_SET_NEG_SRC(ACMP01, 1, ACMP_CTL_NEGSEL_VBG);
    /*for FPGA using*/
    //    ACMP_SET_NEG_SRC(ACMP01, 1, ACMP_CTL_NEGSEL_PIN);
    /* Set PA.10 as output mode */
    PA->MODE = 1 << 20;
    /* Set PB.5 as output mode */
    PB->MODE = 1 << 10;

    /* Enable ACMP */
    ACMP_ENABLE(ACMP01, 1);
    /* Set PA.10 (ACMP1_P0) to low level */
    PA10 = 0;
    /* Set PB.5 (ACMP1_N) to low level */
    PB5  = 0;

    /* 2 ms stable time */
    CLK_SysTickDelay(2000);
    CU_ASSERT(ACMP_GET_OUTPUT(ACMP01, 1) == 0);
    CU_ASSERT((ACMP01->STATUS & 0x20) == 0);
    /* Set PA.10 (ACMP1_P0) to high level */
    PA10 = 1;

    for (ii = 0; ii < 10000; ii++)
        __NOP();

    CU_ASSERT(ACMP_GET_OUTPUT(ACMP01, 1) == 1);
    CU_ASSERT((ACMP01->STATUS & 0x20) == 0x20);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);
}

void MACRO_ACMP_GET_CLR_INT_FLAG()
{
    int volatile ii;
    ACMP_SET_NEG_SRC(ACMP01, 0, ACMP_CTL_NEGSEL_VBG);
    /* Set PA.11 as output mode */
    PA->MODE = 1 << 22;
    /* Enable ACMP */
    ACMP_ENABLE(ACMP01, 0);
    /* Set PA.11 (ACMP0_P0) to low level */
    PA11 = 0;
    /* 2 ms stable time */
    CLK_SysTickDelay(2000);
    /* Set PA.11 (ACMP0_P0) to high level */
    PA11 = 1;
    __NOP();
    CU_ASSERT(ACMP_GET_INT_FLAG(ACMP01, 0) == 1);
    CU_ASSERT((ACMP01->STATUS & 0x1) == 0x1);
    ACMP_CLR_INT_FLAG(ACMP01, 0);
    CU_ASSERT((ACMP01->STATUS & 0x1) == 0x0);

    ACMP_SET_NEG_SRC(ACMP01, 1, ACMP_CTL_NEGSEL_VBG);
    /* Set PA.10 as output mode */
    PA->MODE = 1 << 20;
    /* Enable ACMP */
    ACMP_ENABLE(ACMP01, 1);
    /* Set PA.10 (ACMP1_P0) to low level */
    PA10 = 0;
    /* 2 ms stable time */
    CLK_SysTickDelay(2000);
    /* Set PA.10 (ACMP1_P0) to high level */
    PA10 = 1;

    for (ii = 0; ii < 10000; ii++)
    __NOP();

    CU_ASSERT(ACMP_GET_INT_FLAG(ACMP01, 1) == 1);
    CU_ASSERT((ACMP01->STATUS & 0x2) == 0x2);
    ACMP_CLR_INT_FLAG(ACMP01, 1);
    CU_ASSERT((ACMP01->STATUS & 0x2) == 0x0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);
}

void MACRO_ACMP_ENABLE_DISABLE_WAKEUP()
{
    CU_ASSERT((ACMP01->CTL[0] & 0x10000) == 0);
    ACMP_ENABLE_WAKEUP(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & 0x10000) == 0x10000);
    ACMP_DISABLE_WAKEUP(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & 0x10000) == 0);

    CU_ASSERT((ACMP01->CTL[1] & 0x10000) == 0);
    ACMP_ENABLE_WAKEUP(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[1] & 0x10000) == 0x10000);
    ACMP_DISABLE_WAKEUP(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[1] & 0x10000) == 0);


    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);
}

void MACRO_ACMP_CLR_WAKEUP_INT_FLAG()
{
    ACMP_CLR_WAKEUP_INT_FLAG(ACMP01, 0);
    CU_ASSERT((ACMP01->STATUS & 0x300) == 0x0);
    ACMP_CLR_WAKEUP_INT_FLAG(ACMP01, 1);
    CU_ASSERT((ACMP01->STATUS & 0x300) == 0x0);
}

void MACRO_ACMP_SELECT_P()
{
    ACMP_SELECT_P(ACMP01, 0, ACMP_CTL_POSSEL_P0);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_POSSEL_Msk) == 0);
    ACMP_SELECT_P(ACMP01, 0, ACMP_CTL_POSSEL_P1);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_POSSEL_Msk) == 0x40);
    ACMP_SELECT_P(ACMP01, 0, ACMP_CTL_POSSEL_P2);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_POSSEL_Msk) == 0x80);
    ACMP_SELECT_P(ACMP01, 0, ACMP_CTL_POSSEL_P3);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_POSSEL_Msk) == 0xC0);

    ACMP_SELECT_P(ACMP01, 1, ACMP_CTL_POSSEL_P0);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_POSSEL_Msk) == 0);
    ACMP_SELECT_P(ACMP01, 1, ACMP_CTL_POSSEL_P1);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_POSSEL_Msk) == 0x40);
    ACMP_SELECT_P(ACMP01, 1, ACMP_CTL_POSSEL_P2);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_POSSEL_Msk) == 0x80);
    ACMP_SELECT_P(ACMP01, 1, ACMP_CTL_POSSEL_P3);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_POSSEL_Msk) == 0xC0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);
}

void MACRO_ACMP_ENABLE_DISABLE_FILTER()
{
    ACMP_ENABLE_FILTER(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & 0x1000) == 0x1000);
    ACMP_DISABLE_FILTER(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & 0x1000) == 0);

    ACMP_ENABLE_FILTER(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[1] & 0x1000) == 0x1000);
    ACMP_DISABLE_FILTER(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[1] & 0x1000) == 0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);
}

void MACRO_ACMP_SET_FILTER()
{
    ACMP_SET_FILTER(ACMP01, 0, ACMP_CTL_FILTSEL_1PCLK);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_FILTSEL_Msk) == 0x2000);
    ACMP_SET_FILTER(ACMP01, 0, ACMP_CTL_FILTSEL_2PCLK);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_FILTSEL_Msk) == 0x4000);
    ACMP_SET_FILTER(ACMP01, 0, ACMP_CTL_FILTSEL_4PCLK);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_FILTSEL_Msk) == 0x6000);
    ACMP_SET_FILTER(ACMP01, 0, ACMP_CTL_FILTSEL_8PCLK);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_FILTSEL_Msk) == 0x8000);
    ACMP_SET_FILTER(ACMP01, 0, ACMP_CTL_FILTSEL_16PCLK);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_FILTSEL_Msk) == 0xA000);
    ACMP_SET_FILTER(ACMP01, 0, ACMP_CTL_FILTSEL_32PCLK);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_FILTSEL_Msk) == 0xC000);
    ACMP_SET_FILTER(ACMP01, 0, ACMP_CTL_FILTSEL_64PCLK);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_FILTSEL_Msk) == 0xE000);
    ACMP_SET_FILTER(ACMP01, 0, ACMP_CTL_FILTSEL_OFF);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_FILTSEL_Msk) == 0);

    ACMP_SET_FILTER(ACMP01, 1, ACMP_CTL_FILTSEL_1PCLK);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_FILTSEL_Msk) == 0x2000);
    ACMP_SET_FILTER(ACMP01, 1, ACMP_CTL_FILTSEL_2PCLK);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_FILTSEL_Msk) == 0x4000);
    ACMP_SET_FILTER(ACMP01, 1, ACMP_CTL_FILTSEL_4PCLK);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_FILTSEL_Msk) == 0x6000);
    ACMP_SET_FILTER(ACMP01, 1, ACMP_CTL_FILTSEL_8PCLK);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_FILTSEL_Msk) == 0x8000);
    ACMP_SET_FILTER(ACMP01, 1, ACMP_CTL_FILTSEL_16PCLK);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_FILTSEL_Msk) == 0xA000);
    ACMP_SET_FILTER(ACMP01, 1, ACMP_CTL_FILTSEL_32PCLK);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_FILTSEL_Msk) == 0xC000);
    ACMP_SET_FILTER(ACMP01, 1, ACMP_CTL_FILTSEL_64PCLK);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_FILTSEL_Msk) == 0xE000);
    ACMP_SET_FILTER(ACMP01, 1, ACMP_CTL_FILTSEL_OFF);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_FILTSEL_Msk) == 0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);
}

void MACRO_ACMP_CRV_SEL()
{
    uint32_t u32Count, u32ErrorCount = 0;

    for (u32Count = 0; u32Count < 16; u32Count++)
    {
        ACMP_CRV_SEL(ACMP01, u32Count);

        if ((ACMP01->VREF & ACMP_VREF_CRVCTL_Msk) != u32Count)
            u32ErrorCount++;
    }

    CU_ASSERT_EQUAL(u32ErrorCount, 0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);
}

void MACRO_ACMP_SELECT_CRV_SRC()
{
    ACMP_SELECT_CRV_SRC(ACMP01, ACMP_VREF_CRVSSEL_AVDD);
    CU_ASSERT((ACMP01->VREF & ACMP_VREF_CRVSSEL_Msk) == 0);
    ACMP_SELECT_CRV_SRC(ACMP01, ACMP_VREF_CRVSSEL_INTVREF);
    CU_ASSERT((ACMP01->VREF & ACMP_VREF_CRVSSEL_Msk) == 0x40);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);
}

void MACRO_ACMP_SELECT_INT_COND()
{
    ACMP_SELECT_INT_COND(ACMP01, 0, ACMP_CTL_INTPOL_R);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_INTPOL_Msk) == 0x100);
    ACMP_SELECT_INT_COND(ACMP01, 0, ACMP_CTL_INTPOL_F);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_INTPOL_Msk) == 0x200);
    ACMP_SELECT_INT_COND(ACMP01, 0, ACMP_CTL_INTPOL_RF);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_INTPOL_Msk) == 0);

    ACMP_SELECT_INT_COND(ACMP01, 1, ACMP_CTL_INTPOL_R);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_INTPOL_Msk) == 0x100);
    ACMP_SELECT_INT_COND(ACMP01, 1, ACMP_CTL_INTPOL_F);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_INTPOL_Msk) == 0x200);
    ACMP_SELECT_INT_COND(ACMP01, 1, ACMP_CTL_INTPOL_RF);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_INTPOL_Msk) == 0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);
}


void MACRO_ACMP_WINDOW_XXX(void)
{
    ACMP_ENABLE_WINDOW_LATCH(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_WLATEN_Msk) == ACMP_CTL_WLATEN_Msk);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_WLATEN_Msk) == 0);
    ACMP_ENABLE_WINDOW_LATCH(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_WLATEN_Msk) == ACMP_CTL_WLATEN_Msk);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_WLATEN_Msk) == ACMP_CTL_WLATEN_Msk);
    ACMP_DISABLE_WINDOW_LATCH(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_WLATEN_Msk) == 0);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_WLATEN_Msk) == ACMP_CTL_WLATEN_Msk);
    ACMP_DISABLE_WINDOW_LATCH(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_WLATEN_Msk) == 0);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_WLATEN_Msk) == 0);


    ACMP_ENABLE_WINDOW_COMPARE(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_WCMPSEL_Msk) == ACMP_CTL_WCMPSEL_Msk);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_WCMPSEL_Msk) == 0);
    ACMP_ENABLE_WINDOW_COMPARE(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_WCMPSEL_Msk) == ACMP_CTL_WCMPSEL_Msk);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_WCMPSEL_Msk) == ACMP_CTL_WCMPSEL_Msk);
    ACMP_DISABLE_WINDOW_COMPARE(ACMP01, 0);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_WCMPSEL_Msk) == 0);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_WCMPSEL_Msk) == ACMP_CTL_WCMPSEL_Msk);
    ACMP_DISABLE_WINDOW_COMPARE(ACMP01, 1);
    CU_ASSERT((ACMP01->CTL[0] & ACMP_CTL_WCMPSEL_Msk) == 0);
    CU_ASSERT((ACMP01->CTL[1] & ACMP_CTL_WCMPSEL_Msk) == 0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);

}

void API_ACMP_Open_Close()
{
    ACMP_Open(ACMP01, 0, ACMP_CTL_NEGSEL_CRV, ACMP_CTL_HYSTERESIS_30MV);
    CU_ASSERT(ACMP01->CTL[0] == 0x03000011);
    ACMP_Open(ACMP01, 0, ACMP_CTL_NEGSEL_VBG, ACMP_CTL_HYSTERESIS_20MV);
    CU_ASSERT(ACMP01->CTL[0] == 0x02000021);
    ACMP_Open(ACMP01, 0, ACMP_CTL_NEGSEL_DAC, ACMP_CTL_HYSTERESIS_10MV);
    CU_ASSERT(ACMP01->CTL[0] == 0x01000031);
    ACMP_Open(ACMP01, 0, ACMP_CTL_NEGSEL_PIN, ACMP_CTL_HYSTERESIS_DISABLE);
    CU_ASSERT(ACMP01->CTL[0] == 1);
    ACMP_Close(ACMP01, 0);
    CU_ASSERT(ACMP01->CTL[0] == 0);

    ACMP_Open(ACMP01, 1, ACMP_CTL_NEGSEL_CRV, ACMP_CTL_HYSTERESIS_30MV);
    CU_ASSERT(ACMP01->CTL[1] == 0x03000011);
    ACMP_Open(ACMP01, 1, ACMP_CTL_NEGSEL_VBG, ACMP_CTL_HYSTERESIS_20MV);
    CU_ASSERT(ACMP01->CTL[1] == 0x02000021);
    ACMP_Open(ACMP01, 1, ACMP_CTL_NEGSEL_DAC, ACMP_CTL_HYSTERESIS_10MV);
    CU_ASSERT(ACMP01->CTL[1] == 0x01000031);
    ACMP_Open(ACMP01, 1, ACMP_CTL_NEGSEL_PIN, ACMP_CTL_HYSTERESIS_DISABLE);
    CU_ASSERT(ACMP01->CTL[1] == 1);
    ACMP_Close(ACMP01, 1);
    CU_ASSERT(ACMP01->CTL[1] == 0);

    /* Reset ACMP */
    SYS_ResetModule(SYS_ACMP01RST);
}



CU_TestInfo ACMP_MacroTests[] =
{

    {"Enable/Disable ACMP output inverse function", MACRO_ACMP_ENABLE_DISABLE_OUTPUT_INVERSE},
    {"Set ACMP negative input source", MACRO_ACMP_SET_NEG_SRC},
    {"Enable/Disable ACMP hysteresis function", MACRO_ACMP_ENABLE_DISABLE_HYSTERESIS},
    {"Enable/Disable ACMP interrupt function", MACRO_ACMP_ENABLE_DISABLE_INT},
    {"Enable/Disable ACMP", MACRO_ACMP_ENABLE_DISABLE_ACMP},
    {"Get ACMP output value", MACRO_ACMP_GET_OUTPUT},
    {"Get/Clear ACMP interrupt flag", MACRO_ACMP_GET_CLR_INT_FLAG},
    {"Enable/Disable ACMP wake-up function", MACRO_ACMP_ENABLE_DISABLE_WAKEUP},
    {"Clear ACMP wake-up interrupt flag", MACRO_ACMP_CLR_WAKEUP_INT_FLAG},
    {"Select ACMP positive input pin", MACRO_ACMP_SELECT_P},
    {"Enable/Disable ACMP filter function", MACRO_ACMP_ENABLE_DISABLE_FILTER},
    {"Set ACMP filter function", MACRO_ACMP_SET_FILTER},
    {"Select comparator reference voltage", MACRO_ACMP_CRV_SEL},
    {"Select the source of CRV", MACRO_ACMP_SELECT_CRV_SRC},
    {"Select ACMP interrupt condition", MACRO_ACMP_SELECT_INT_COND},
    {"Window XXX mode", MACRO_ACMP_WINDOW_XXX},
    CU_TEST_INFO_NULL
};

CU_TestInfo ACMP_ApiTests[] =
{

    {"ACMP open and close test", API_ACMP_Open_Close},

    CU_TEST_INFO_NULL
};



