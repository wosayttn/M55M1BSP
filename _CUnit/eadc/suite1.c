/*---------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                             */
/*                                                                                                         */
/* Copyright (c) 2017 by Nuvoton Technology Corporation                                                    */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   suite1.c                                                                                              */
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
/* CUnit Test Function                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

void CU_S1T1_CONSTANT_EADC(void)
{

    CU_ASSERT_EQUAL(EADC_CTL_DIFFEN_SINGLE_END ,   0);
    CU_ASSERT_EQUAL(EADC_CTL_DIFFEN_DIFFERENTIAL,  0x100);
    CU_ASSERT_EQUAL(EADC_CTL_DMOF_STRAIGHT_BINARY, 0);
    CU_ASSERT_EQUAL(EADC_CTL_DMOF_TWOS_COMPLEMENT, 0x200);
   
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x0), (0x0));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x1), (0x1));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x2), (0x2));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x3), (0x3));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x4), (0x4));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x5), (0x5));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x6), (0x6));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x7), (0x7));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x8), (0x8));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x9), (0x9));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0xA), (0xA));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0xB), (0xB));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0xC), (0xC));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0xD), (0xD));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0xE), (0xE));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0xF), (0xF));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x10), (0x10));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x11), (0x11));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x12), (0x12));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x13), (0x13));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x14), (0x14));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x15), (0x15));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x16), (0x16));
    CU_ASSERT_EQUAL(EADC_SCTL_CHSEL(0x17), (0x17));

    CU_ASSERT_EQUAL(EADC_SCTL_TRGDLYDIV(0), 0x0);
    CU_ASSERT_EQUAL(EADC_SCTL_TRGDLYDIV(1), 0x40);
    CU_ASSERT_EQUAL(EADC_SCTL_TRGDLYDIV(2), 0x80);
    CU_ASSERT_EQUAL(EADC_SCTL_TRGDLYDIV(3), 0xC0);

    CU_ASSERT_EQUAL(EADC_SCTL_TRGDLYCNT(0x00), 0x0);
    CU_ASSERT_EQUAL(EADC_SCTL_TRGDLYCNT(0x55), 0x5500);
    CU_ASSERT_EQUAL(EADC_SCTL_TRGDLYCNT(0x7F), 0x7F00);
    CU_ASSERT_EQUAL(EADC_SCTL_TRGDLYCNT(0xAA), 0xAA00);
    CU_ASSERT_EQUAL(EADC_SCTL_TRGDLYCNT(0xFF), 0xFF00);

    CU_ASSERT_EQUAL(EADC_SOFTWARE_TRIGGER, 0);
    CU_ASSERT_EQUAL(EADC_FALLING_EDGE_TRIGGER,0x810000);
    CU_ASSERT_EQUAL(EADC_RISING_EDGE_TRIGGER, 0x410000);
    CU_ASSERT_EQUAL(EADC_FALLING_RISING_EDGE_TRIGGER, 0xC10000);
    CU_ASSERT_EQUAL(EADC_ADINT0_TRIGGER, 0x20000);
    CU_ASSERT_EQUAL(EADC_ADINT1_TRIGGER, 0x30000);
    CU_ASSERT_EQUAL(EADC_TIMER0_TRIGGER, 0x40000);
    CU_ASSERT_EQUAL(EADC_TIMER1_TRIGGER, 0x50000);
    CU_ASSERT_EQUAL(EADC_TIMER2_TRIGGER, 0x60000);
    CU_ASSERT_EQUAL(EADC_TIMER3_TRIGGER, 0x70000);
    CU_ASSERT_EQUAL(EADC_EPWM0TG0_TRIGGER,0x80000);
    CU_ASSERT_EQUAL(EADC_EPWM0TG1_TRIGGER,0x90000);
    CU_ASSERT_EQUAL(EADC_EPWM0TG2_TRIGGER,0xA0000);
    CU_ASSERT_EQUAL(EADC_EPWM0TG3_TRIGGER,0xB0000);
    CU_ASSERT_EQUAL(EADC_EPWM0TG4_TRIGGER,0xC0000);
    CU_ASSERT_EQUAL(EADC_EPWM0TG5_TRIGGER,0xD0000);
    CU_ASSERT_EQUAL(EADC_EPWM1TG0_TRIGGER,0xE0000);
    CU_ASSERT_EQUAL(EADC_EPWM1TG1_TRIGGER,0xF0000);
    CU_ASSERT_EQUAL(EADC_EPWM1TG2_TRIGGER,0x100000);
    CU_ASSERT_EQUAL(EADC_EPWM1TG3_TRIGGER,0x110000);
    CU_ASSERT_EQUAL(EADC_EPWM1TG4_TRIGGER,0x120000);
    CU_ASSERT_EQUAL(EADC_EPWM1TG5_TRIGGER,0x130000);
    CU_ASSERT_EQUAL(EADC_BPWM0TG_TRIGGER, 0x200000);
    CU_ASSERT_EQUAL(EADC_BPWM1TG_TRIGGER, 0x210000);
    CU_ASSERT_EQUAL(EADC_EPWM1TG4_TRIGGER,0x220000);
    CU_ASSERT_EQUAL(EADC_EPWM1TG5_TRIGGER,0x230000);
    CU_ASSERT_EQUAL(EADC_BPWM0TG_TRIGGER, 0x240000);
    CU_ASSERT_EQUAL(EADC_BPWM1TG_TRIGGER, 0x250000);
    

    CU_ASSERT_EQUAL(EADC_SCTL_TRGDLYDIV_DIVIDER_1, 0x0);
    CU_ASSERT_EQUAL(EADC_SCTL_TRGDLYDIV_DIVIDER_2, 0x40);
    CU_ASSERT_EQUAL(EADC_SCTL_TRGDLYDIV_DIVIDER_4, 0x80);
    CU_ASSERT_EQUAL(EADC_SCTL_TRGDLYDIV_DIVIDER_16,0xC0);

    CU_ASSERT_EQUAL(EADC_CMP_CMPCOND_LESS_THAN,0x0);
    CU_ASSERT_EQUAL(EADC_CMP_CMPCOND_GREATER_OR_EQUAL,0x4);
    CU_ASSERT_EQUAL(EADC_CMP_CMPWEN_ENABLE, 0x8000);
    CU_ASSERT_EQUAL(EADC_CMP_CMPWEN_DISABLE,0x0);
    CU_ASSERT_EQUAL(EADC_CMP_ADCMPIE_ENABLE,0x2);
    CU_ASSERT_EQUAL(EADC_CMP_ADCMPIE_DISABLE,0);

    CU_ASSERT_EQUAL(EADC_MCTL1_ACU_1, 0x0);
    CU_ASSERT_EQUAL(EADC_MCTL1_ACU_2, 0x10);
    CU_ASSERT_EQUAL(EADC_MCTL1_ACU_4, 0x20);
    CU_ASSERT_EQUAL(EADC_MCTL1_ACU_8, 0x30);
    CU_ASSERT_EQUAL(EADC_MCTL1_ACU_16,0x40);
    CU_ASSERT_EQUAL(EADC_MCTL1_ACU_32,0x50);
    CU_ASSERT_EQUAL(EADC_MCTL1_ACU_64,0x60);
    CU_ASSERT_EQUAL(EADC_MCTL1_ACU_128,0x70);
    CU_ASSERT_EQUAL(EADC_MCTL1_ACU_256,0x80);

}
