/*---------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                             */
/*                                                                                                         */
/* Copyright (c) 2017 by Nuvoton Technology Corporation                                                    */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   suite3.c                                                                                              */
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

// Define WRONG_FPGA_CODE if you use old FPGA code that has some problems.
//#define WRONG_FPGA_CODE

/*---------------------------------------------------------------------------------------------------------*/
/* CUnit Test Function                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define M258             0
#define M256D            1
#define M258G            2
#define CHIP_VERSION     M258G

uint32_t au32TriggeSource[28]=
{
 EADC_SOFTWARE_TRIGGER,
 EADC_FALLING_EDGE_TRIGGER,
 EADC_RISING_EDGE_TRIGGER,
 EADC_FALLING_RISING_EDGE_TRIGGER, 
 EADC_ADINT0_TRIGGER,
 EADC_ADINT1_TRIGGER,
 EADC_TIMER0_TRIGGER,
 EADC_TIMER1_TRIGGER,
 EADC_TIMER2_TRIGGER,
 EADC_TIMER3_TRIGGER,
 EADC_EPWM0TG0_TRIGGER,
 EADC_EPWM0TG1_TRIGGER,
 EADC_EPWM0TG2_TRIGGER,
 EADC_EPWM0TG3_TRIGGER,
 EADC_EPWM0TG4_TRIGGER,
 EADC_EPWM0TG5_TRIGGER,
 EADC_EPWM1TG0_TRIGGER,
 EADC_EPWM1TG1_TRIGGER,
 EADC_EPWM1TG2_TRIGGER,
 EADC_EPWM1TG3_TRIGGER,
 EADC_EPWM1TG4_TRIGGER,
 EADC_EPWM1TG5_TRIGGER,
 EADC_BPWM0TG_TRIGGER,
 EADC_BPWM1TG_TRIGGER,
 EADC_ACMP0_INT_TRIGGER,
 EADC_ACMP1_INT_TRIGGER,
 EADC_ACMP2_INT_TRIGGER,
 EADC_ACMP3_INT_TRIGGER
};

void CU_EADC_API_Function_Test(EADC_T *psEadc);

void CU_EADC_API_Function_Test(EADC_T *psEadc)
{
    unsigned int moduleNum; //tmp_result;
    unsigned int intNum;
    uint32_t u32TrgScr = 0;

    intNum = 0;

    EADC_Open(psEadc, EADC_CTL_DIFFEN_DIFFERENTIAL);
    CU_ASSERT_EQUAL(psEadc->CTL & EADC_CTL_ADCEN_Msk, EADC_CTL_ADCEN_Msk);
    CU_ASSERT_EQUAL(psEadc->CTL & EADC_CTL_DIFFEN_Msk, EADC_CTL_DIFFEN_DIFFERENTIAL);
    
    EADC_Open(psEadc, EADC_CTL_DIFFEN_SINGLE_END);
    CU_ASSERT_EQUAL(psEadc->CTL & EADC_CTL_ADCEN_Msk, EADC_CTL_ADCEN_Msk);
    CU_ASSERT_EQUAL(psEadc->CTL & EADC_CTL_DIFFEN_Msk, EADC_CTL_DIFFEN_SINGLE_END);

    for (moduleNum = 0; moduleNum < 28; moduleNum++)    /* valid sample modules are 0 ~ 27 */
    {
        unsigned int moduleMask;
        moduleMask = (BIT0 << moduleNum);

        /* Sample module 16 ~ 18 don't support following features */
        if (moduleNum < 19)
        {
           
           for(u32TrgScr = 0 ;u32TrgScr <28 ; u32TrgScr++)
          {
            EADC_ConfigSampleModule(psEadc, moduleNum, au32TriggeSource[u32TrgScr], moduleNum);
            if(u32TrgScr == 1)
            {
              CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_EXTFEN_Msk, EADC_SCTL_EXTFEN_Msk);
              CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_EXTREN_Msk, 0);
              CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_TRGSEL_Msk, 0x10000);
            }
            else if(u32TrgScr == 2)
            {
              CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_EXTFEN_Msk, 0);
              CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_EXTREN_Msk, EADC_SCTL_EXTREN_Msk);
              CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_TRGSEL_Msk, 0x10000);
            }
            else if(u32TrgScr == 3)
            {
              CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_EXTFEN_Msk, EADC_SCTL_EXTFEN_Msk);
              CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_EXTREN_Msk, EADC_SCTL_EXTREN_Msk);
              CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_TRGSEL_Msk, 0x10000);
            }
            else
            {
              CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_EXTFEN_Msk, 0);
              CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_EXTREN_Msk, 0);
              CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_TRGSEL_Msk, au32TriggeSource[u32TrgScr]);
            }
              CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_CHSEL_Msk, moduleNum); 
           }
            
            EADC_SetTriggerDelayTime(psEadc, moduleNum, 0, EADC_SCTL_TRGDLYDIV_DIVIDER_1);
            CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_TRGDLYCNT_Msk, 0 << EADC_SCTL_TRGDLYCNT_Pos);
            CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_TRGDLYDIV_Msk, EADC_SCTL_TRGDLYDIV_DIVIDER_1);

            EADC_SetTriggerDelayTime(psEadc, moduleNum, 0x55, EADC_SCTL_TRGDLYDIV_DIVIDER_2);
            CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_TRGDLYCNT_Msk, 0x55 << EADC_SCTL_TRGDLYCNT_Pos);
            CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_TRGDLYDIV_Msk, EADC_SCTL_TRGDLYDIV_DIVIDER_2);

            EADC_SetTriggerDelayTime(psEadc, moduleNum, 0xAA, EADC_SCTL_TRGDLYDIV_DIVIDER_4);
            CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_TRGDLYCNT_Msk, 0xAA << EADC_SCTL_TRGDLYCNT_Pos);
            CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_TRGDLYDIV_Msk, EADC_SCTL_TRGDLYDIV_DIVIDER_4);

            EADC_SetTriggerDelayTime(psEadc, moduleNum, 0xFF, EADC_SCTL_TRGDLYDIV_DIVIDER_16);
            CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_TRGDLYCNT_Msk, 0xFF << EADC_SCTL_TRGDLYCNT_Pos);
            CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL_TRGDLYDIV_Msk, EADC_SCTL_TRGDLYDIV_DIVIDER_16);
            
            EADC_SetExtendSampleTime(psEadc, moduleNum, 0);
            CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL0_EXTSMPT_Msk, (0 << EADC_SCTL0_EXTSMPT_Pos));
            EADC_SetExtendSampleTime(psEadc, moduleNum, 0x55);
            CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL0_EXTSMPT_Msk, (0x55 << EADC_SCTL0_EXTSMPT_Pos));
            EADC_SetExtendSampleTime(psEadc, moduleNum, 0xAA);
            CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL0_EXTSMPT_Msk, (0xAA << EADC_SCTL0_EXTSMPT_Pos));
            EADC_SetExtendSampleTime(psEadc, moduleNum, 0xFF);
            CU_ASSERT_EQUAL((&(psEadc->SCTL[0]))[moduleNum] & EADC_SCTL0_EXTSMPT_Msk, (0xFF << EADC_SCTL0_EXTSMPT_Pos)); 

        }else if(moduleNum >= 19 &&  moduleNum < 24)
        {
        
           for(u32TrgScr = 0 ;u32TrgScr <28 ; u32TrgScr++)
          {
            EADC_ConfigSampleModule(psEadc, moduleNum, au32TriggeSource[u32TrgScr], moduleNum);
            if(u32TrgScr == 1)
            {
              CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_EXTFEN_Msk, EADC_SCTL_EXTFEN_Msk);
              CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_EXTREN_Msk, 0);
              CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_TRGSEL_Msk, 0x10000);
            }
            else if(u32TrgScr == 2)
            {
              CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_EXTFEN_Msk, 0);
              CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_EXTREN_Msk, EADC_SCTL_EXTREN_Msk);
              CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_TRGSEL_Msk, 0x10000);
            }
            else if(u32TrgScr == 3)
            {
              CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_EXTFEN_Msk, EADC_SCTL_EXTFEN_Msk);
              CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_EXTREN_Msk, EADC_SCTL_EXTREN_Msk);
              CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_TRGSEL_Msk, 0x10000);
            }
            else
            {
              CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_EXTFEN_Msk, 0);
              CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_EXTREN_Msk, 0);
              CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_TRGSEL_Msk, au32TriggeSource[u32TrgScr]);
            }
              CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_CHSEL_Msk, moduleNum); 
           }
            
            EADC_SetTriggerDelayTime(psEadc, moduleNum, 0, EADC_SCTL_TRGDLYDIV_DIVIDER_1);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_TRGDLYCNT_Msk, 0 << EADC_SCTL_TRGDLYCNT_Pos);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_TRGDLYDIV_Msk, EADC_SCTL_TRGDLYDIV_DIVIDER_1);

            EADC_SetTriggerDelayTime(psEadc, moduleNum, 0x55, EADC_SCTL_TRGDLYDIV_DIVIDER_2);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_TRGDLYCNT_Msk, 0x55 << EADC_SCTL_TRGDLYCNT_Pos);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_TRGDLYDIV_Msk, EADC_SCTL_TRGDLYDIV_DIVIDER_2);

            EADC_SetTriggerDelayTime(psEadc, moduleNum, 0xAA, EADC_SCTL_TRGDLYDIV_DIVIDER_4);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_TRGDLYCNT_Msk, 0xAA << EADC_SCTL_TRGDLYCNT_Pos);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_TRGDLYDIV_Msk, EADC_SCTL_TRGDLYDIV_DIVIDER_4);

            EADC_SetTriggerDelayTime(psEadc, moduleNum, 0xFF, EADC_SCTL_TRGDLYDIV_DIVIDER_16);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_TRGDLYCNT_Msk, 0xFF << EADC_SCTL_TRGDLYCNT_Pos);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL_TRGDLYDIV_Msk, EADC_SCTL_TRGDLYDIV_DIVIDER_16);
            
            EADC_SetExtendSampleTime(psEadc, moduleNum, 0);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL0_EXTSMPT_Msk, (0 << EADC_SCTL0_EXTSMPT_Pos));
            EADC_SetExtendSampleTime(psEadc, moduleNum, 0x55);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL0_EXTSMPT_Msk, (0x55 << EADC_SCTL0_EXTSMPT_Pos));
            EADC_SetExtendSampleTime(psEadc, moduleNum, 0xAA);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL0_EXTSMPT_Msk, (0xAA << EADC_SCTL0_EXTSMPT_Pos));
            EADC_SetExtendSampleTime(psEadc, moduleNum, 0xFF);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL0_EXTSMPT_Msk, (0xFF << EADC_SCTL0_EXTSMPT_Pos)); 
        
        }
        else
        {
        
            EADC_SetExtendSampleTime(psEadc, moduleNum, 0);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL19_EXTSMPT_Msk, (0 << EADC_SCTL19_EXTSMPT_Pos));
            EADC_SetExtendSampleTime(psEadc, moduleNum, 0x55);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL19_EXTSMPT_Msk, (0x55 << EADC_SCTL19_EXTSMPT_Pos));
            EADC_SetExtendSampleTime(psEadc, moduleNum, 0xAA);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL19_EXTSMPT_Msk, (0xAA << EADC_SCTL19_EXTSMPT_Pos));
            EADC_SetExtendSampleTime(psEadc, moduleNum, 0xFF);
            CU_ASSERT_EQUAL((&(psEadc->SCTL19[0]))[moduleNum-19] & EADC_SCTL19_EXTSMPT_Msk, (0xFF << EADC_SCTL19_EXTSMPT_Pos)); 
        }
       


        EADC_CLR_INT_FLAG(psEadc, EADC_STATUS2_ADIF0_Msk);
        EADC_ENABLE_SAMPLE_MODULE_INT(psEadc, intNum, moduleMask);
        EADC_START_CONV(psEadc, moduleMask);
        // Ignore EADC_GET_INT_FLAG() if run wrong FPGA code
#ifndef WRONG_FPGA_CODE
        //        while(EADC_GET_INT_FLAG(EADC, EADC_STATUS2_ADIF0_Msk) == 0) {}
#endif
        EADC_CLR_INT_FLAG(psEadc, EADC_STATUS2_ADIF0_Msk);

        unsigned int conversion_result;
        conversion_result = EADC_GET_CONV_DATA(psEadc, moduleNum);

        EADC_Close(psEadc);
        CU_ASSERT_EQUAL(psEadc->CTL & EADC_CTL_ADCEN_Msk, 0);
    }

    return;
}

void CU_EADC_API_Test(void)
{
   CU_EADC_API_Function_Test(EADC0);
   CU_EADC_API_Function_Test(EADC1);
}
