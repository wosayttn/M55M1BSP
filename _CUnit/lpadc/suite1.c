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
#include "lpadc_cunit.h"

/*---------------------------------------------------------------------------------------------------------*/
/* CUnit Test Function                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

void CU_S1T1_CONSTANT_LPADC(void)
{
     uint32_t u32TestCnt;

    CU_ASSERT_EQUAL(LPADC_ADCR_ADEN_CONVERTER_DISABLE ,0);
    CU_ASSERT_EQUAL(LPADC_ADCR_ADEN_CONVERTER_ENABLE ,0x1);
    CU_ASSERT_EQUAL(LPADC_ADCR_ADMD_SINGLE, 0x0);
    CU_ASSERT_EQUAL(LPADC_ADCR_ADMD_SINGLE, 0x4);
    CU_ASSERT_EQUAL(LPADC_ADCR_ADMD_SINGLE, 0x8);
    CU_ASSERT_EQUAL(LPADC_ADCR_ADMD_SINGLE, 0xC);
    
    CU_ASSERT_EQUAL(LPADC_ADCR_DMOF_UNSIGNED_OUTPUT,0x0);
    CU_ASSERT_EQUAL(LPADC_ADCR_DMOF_TWOS_COMPLEMENT,0x400);


    CU_ASSERT_EQUAL(LPADC_LOW_LEVEL_TRIGGER, 0x0);
    CU_ASSERT_EQUAL(LPADC_HIGH_LEVEL_TRIGGER, 0x40);
    CU_ASSERT_EQUAL(LPADC_FALLING_EDGE_TRIGGER, 0x80);
    CU_ASSERT_EQUAL(LPADC_RISING_EDGE_TRIGGER, 0xC0);
    CU_ASSERT_EQUAL(LPADC_BPWM_TRIGGER,  0x20000);
    CU_ASSERT_EQUAL(LPADC_EPWM_TRIGGER,  0x40000);
    CU_ASSERT_EQUAL(LPADC_ACMP0_TRIGGER, 0x80000);
    CU_ASSERT_EQUAL(LPADC_ACMP1_TRIGGER, 0x90000);
    CU_ASSERT_EQUAL(LPADC_ACMP2_TRIGGER, 0xA0000);
    CU_ASSERT_EQUAL(LPADC_ACMP3_TRIGGER, 0xB0000);
    
    CU_ASSERT_EQUAL(LPADC_ADCR_TRGCOND_LOW_LEVEL,   0x0);
    CU_ASSERT_EQUAL(LPADC_ADCR_TRGCOND_HIGH_LEVEL,  0x40);
    CU_ASSERT_EQUAL(LPADC_ADCR_TRGCOND_FALLING_EDGE,0x80);
    CU_ASSERT_EQUAL(LPADC_ADCR_TRGCOND_RISING_EDGE, 0xC0);
    
    CU_ASSERT_EQUAL(LPADC_ADCR_RESSEL_12BITS,0x0);
    CU_ASSERT_EQUAL(LPADC_ADCR_RESSEL_10BITS,0x10000000);
    CU_ASSERT_EQUAL(LPADC_ADCR_RESSEL_8BITS, 0x20000000);
  
    for(u32TestCnt = 0;u32TestCnt<0xFFF;u32TestCnt++)
    {
     CU_ASSERT_EQUAL(LPADC_ADCMPR_CMPD(u32TestCnt), (u32TestCnt << 16));
    }
    
    for(u32TestCnt = 1;u32TestCnt <= 16;u32TestCnt++)
    {
     CU_ASSERT_EQUAL(LPADC_ADCMPR_CMPMATCNT(u32TestCnt), (u32TestCnt << 8));
    }
    
    for(u32TestCnt = 0;u32TestCnt < 0x20 ;u32TestCnt++)
    {
     CU_ASSERT_EQUAL(LPADC_ADCMPR_CMPCH(u32TestCnt), (u32TestCnt << 3));
    }
    
    CU_ASSERT_EQUAL(LPADC_ADCMPR_CMPCOND_LESS_THAN, 0x0);
    CU_ASSERT_EQUAL(LPADC_ADCMPR_CMPCOND_GREATER_OR_EQUAL,0x4);
    CU_ASSERT_EQUAL(LPADC_ADCMPR_CMPIE_INTERRUPT_ENABLE, 0x2);
    CU_ASSERT_EQUAL(LPADC_ADF_INT,  0x1);
    CU_ASSERT_EQUAL(LPADC_CMP0_INT, 0x2);
    CU_ASSERT_EQUAL(LPADC_CMP1_INT, 0x4);
    
    CU_ASSERT_EQUAL(LPADC_AUTOCTL_TRIGSEL_LPTMR0, (0x0));
    CU_ASSERT_EQUAL(LPADC_AUTOCTL_TRIGSEL_LPTMR1, (0x1));
    CU_ASSERT_EQUAL(LPADC_AUTOCTL_TRIGSEL_TTMR0,  (0x2));
    CU_ASSERT_EQUAL(LPADC_AUTOCTL_TRIGSEL_TTMR1,  (0x3));
    CU_ASSERT_EQUAL(LPADC_AUTOCTL_TRIGSEL_WKIOA0, (0x4));
    CU_ASSERT_EQUAL(LPADC_AUTOCTL_TRIGSEL_WKIOB0, (0x5));
    CU_ASSERT_EQUAL(LPADC_AUTOCTL_TRIGSEL_WKIOC0, (0x6));
    CU_ASSERT_EQUAL(LPADC_AUTOCTL_TRIGSEL_WKIOD0, (0x7));
    CU_ASSERT_EQUAL(LPADC_AUTOCTL_TRIGSEL_ACMP0, (0x8));
    CU_ASSERT_EQUAL(LPADC_AUTOCTL_TRIGSEL_ACMP1, (0x9));
    CU_ASSERT_EQUAL(LPADC_AUTOCTL_TRIGSEL_ACMP2, (0x10));
    CU_ASSERT_EQUAL(LPADC_AUTOCTL_TRIGSEL_ACMP3, (0x11));


}
