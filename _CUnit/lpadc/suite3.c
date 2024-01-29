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
#include "lpadc_cunit.h"

// Define WRONG_FPGA_CODE if you use old FPGA code that has some problems.
//#define WRONG_FPGA_CODE

/*---------------------------------------------------------------------------------------------------------*/
/* CUnit Test Function                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define M258             0
#define M256D            1
#define M258G            2
#define CHIP_VERSION     M258G

uint32_t au32STADC_TriggeEvent[4] =
{
    LPADC_LOW_LEVEL_TRIGGER,
    LPADC_HIGH_LEVEL_TRIGGER,
    LPADC_FALLING_EDGE_TRIGGER,
    LPADC_RISING_EDGE_TRIGGER,

};
uint32_t au32ExTriggeSource[10] =
{
    LPADC_STADC_TRIGGER,
    LPADC_STADC_TRIGGER,
    LPADC_STADC_TRIGGER,
    LPADC_STADC_TRIGGER,
    LPADC_BPWM_TRIGGER,
    LPADC_EPWM_TRIGGER,
    LPADC_ACMP0_TRIGGER,
    LPADC_ACMP1_TRIGGER,
    LPADC_ACMP2_TRIGGER,
    LPADC_ACMP3_TRIGGER
};

uint32_t au32AutoCtlTriggeSource[13] =
{
    LPADC_AUTOCTL_TRIGSEL_SOFTWARE,
    LPADC_AUTOCTL_TRIGSEL_LPTMR0,
    LPADC_AUTOCTL_TRIGSEL_LPTMR1,
    LPADC_AUTOCTL_TRIGSEL_TTMR0,
    LPADC_AUTOCTL_TRIGSEL_TTMR1,
    LPADC_AUTOCTL_TRIGSEL_WKIOA0,
    LPADC_AUTOCTL_TRIGSEL_WKIOB0,
    LPADC_AUTOCTL_TRIGSEL_WKIOC0,
    LPADC_AUTOCTL_TRIGSEL_WKIOD0,
    LPADC_AUTOCTL_TRIGSEL_ACMP0,
    LPADC_AUTOCTL_TRIGSEL_ACMP1,
    LPADC_AUTOCTL_TRIGSEL_ACMP2,
    LPADC_AUTOCTL_TRIGSEL_ACMP3
};

void CU_LPADC_API_Function_Test(LPADC_T *psLpadc);

void CU_LPADC_API_Function_Test(LPADC_T *psLpadc)
{
    unsigned int moduleNum; //tmp_result;
    unsigned int u32Cnt;
    uint32_t moduleMask = 0;
    uint32_t u32TrgScr = 0;

    LPADC_Open(psLpadc, LPADC_ADCR_DIFFEN_SINGLE_END, LPADC_ADCR_ADMD_SINGLE, BIT0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_DIFFEN_Msk, 0x0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADMD_Msk, 0x0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADEN_Msk, 0x1);
    CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_ADPRDY_Msk, 0x1000000);
    CU_ASSERT_EQUAL(psLpadc->ADCHER & LPADC_ADCHER_CHEN_Msk, 0x01);

    LPADC_Open(psLpadc, LPADC_ADCR_DIFFEN_DIFFERENTIAL, LPADC_ADCR_ADMD_SINGLE, BIT0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_DIFFEN_Msk, 0x400);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADMD_Msk, 0x0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADEN_Msk, 0x1);
    CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_ADPRDY_Msk, 0x1000000);
    CU_ASSERT_EQUAL(psLpadc->ADCHER & LPADC_ADCHER_CHEN_Msk, 0x01);

    LPADC_Open(psLpadc, LPADC_ADCR_DIFFEN_SINGLE_END, LPADC_ADCR_ADMD_BURST, BIT0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_DIFFEN_Msk, 0x0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADMD_Msk, (0x1 << 2));
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADEN_Msk, 0x1);
    CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_ADPRDY_Msk, 0x1000000);
    CU_ASSERT_EQUAL(psLpadc->ADCHER & LPADC_ADCHER_CHEN_Msk, 0x01);

    LPADC_Open(psLpadc, LPADC_ADCR_DIFFEN_SINGLE_END, LPADC_ADCR_ADMD_SINGLE_CYCLE, BIT0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_DIFFEN_Msk, 0x0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADMD_Msk, (0x2 << 2));
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADEN_Msk, 0x1);
    CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_ADPRDY_Msk, 0x1000000);
    CU_ASSERT_EQUAL(psLpadc->ADCHER & LPADC_ADCHER_CHEN_Msk, 0x01);

    LPADC_Open(psLpadc, LPADC_ADCR_DIFFEN_SINGLE_END, LPADC_ADCR_ADMD_CONTINUOUS, BIT0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_DIFFEN_Msk, 0x0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADMD_Msk, (0x3 << 2));
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADEN_Msk, 0x1);
    CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_ADPRDY_Msk, 0x1000000);
    CU_ASSERT_EQUAL(psLpadc->ADCHER & LPADC_ADCHER_CHEN_Msk, 0x01);

    LPADC_Close(psLpadc);
    CU_ASSERT_EQUAL(psLpadc->ADCAL & LPADC_ADCR_ADEN_Msk, 0);

    for (u32TrgScr = 0; u32TrgScr < 10; u32TrgScr++)
    {

        if (u32TrgScr < 4)
        {
            LPADC_EnableHWTrigger(psLpadc, au32ExTriggeSource[u32TrgScr], au32STADC_TriggeEvent[u32TrgScr]);
            CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_TRGS_Msk, 0x0);
            CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_TRGCOND_Msk, (u32TrgScr << LPADC_ADCR_TRGCOND_Pos));
        }
        else
        {
            LPADC_EnableHWTrigger(psLpadc, au32ExTriggeSource[u32TrgScr], 0);
            CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_TRGS_Msk, au32ExTriggeSource[u32TrgScr]);
            CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_TRGCOND_Msk, 0x0);
        }

        CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_TRGEN_Msk, (0x1 << 8));
    }

    for (u32TrgScr = 0; u32TrgScr < 13; u32TrgScr++)
    {
        LPADC_SelectAutoOperationMode(psLpadc, au32AutoCtlTriggeSource[u32TrgScr]);

        if (u32TrgScr == 0)
        {
            CU_ASSERT_EQUAL(psLpadc->AUTOCTL & LPADC_AUTOCTL_AUTOEN_Msk, (0x1 << 31));
            CU_ASSERT_EQUAL(psLpadc->AUTOCTL & LPADC_AUTOCTL_TRIGSEL_Msk, 0x0);
            CU_ASSERT_EQUAL(psLpadc->AUTOCTL & LPADC_AUTOCTL_TRIGEN_Msk,  0x0);
        }
        else
        {
            CU_ASSERT_EQUAL(psLpadc->AUTOCTL & LPADC_AUTOCTL_AUTOEN_Msk, (0x1 << 31));
            CU_ASSERT_EQUAL(psLpadc->AUTOCTL & LPADC_AUTOCTL_TRIGSEL_Msk, (u32TrgScr - 1) << 0);
            CU_ASSERT_EQUAL(psLpadc->AUTOCTL & LPADC_AUTOCTL_TRIGEN_Msk, (0x1 << 4));
        }
    }

    LPADC_DisableHWTrigger(psLpadc);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_TRGS_Msk, 0x0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_TRGCOND_Msk, 0x0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_TRGEN_Msk, 0x0);

    LPADC_EnableInt(psLpadc, LPADC_ADF_INT);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADIE_Msk, (0x1 << 1));
    CU_ASSERT_EQUAL(psLpadc->ADCMPR[0] & LPADC_ADCMPR_CMPIE_Msk, (0x0));
    CU_ASSERT_EQUAL(psLpadc->ADCMPR[1] & LPADC_ADCMPR_CMPIE_Msk, (0x0));
    LPADC_EnableInt(psLpadc, LPADC_CMP0_INT);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADIE_Msk, (0x1 << 1));
    CU_ASSERT_EQUAL(psLpadc->ADCMPR[0] & LPADC_ADCMPR_CMPIE_Msk, (0x1 << 1));
    CU_ASSERT_EQUAL(psLpadc->ADCMPR[1] & LPADC_ADCMPR_CMPIE_Msk, (0x0));
    LPADC_EnableInt(psLpadc, LPADC_CMP1_INT);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADIE_Msk, (0x1 << 1));
    CU_ASSERT_EQUAL(psLpadc->ADCMPR[0] & LPADC_ADCMPR_CMPIE_Msk, (0x1 << 1));
    CU_ASSERT_EQUAL(psLpadc->ADCMPR[1] & LPADC_ADCMPR_CMPIE_Msk, (0x1 << 1));

    LPADC_DisableInt(psLpadc, LPADC_ADF_INT);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADIE_Msk, (0x0));
    CU_ASSERT_EQUAL(psLpadc->ADCMPR[0] & LPADC_ADCMPR_CMPIE_Msk, (0x1 << 1));
    CU_ASSERT_EQUAL(psLpadc->ADCMPR[1] & LPADC_ADCMPR_CMPIE_Msk, (0x1 << 1));
    LPADC_DisableInt(psLpadc, LPADC_CMP0_INT);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADIE_Msk, (0x0));
    CU_ASSERT_EQUAL(psLpadc->ADCMPR[0] & LPADC_ADCMPR_CMPIE_Msk, (0x0));
    CU_ASSERT_EQUAL(psLpadc->ADCMPR[1] & LPADC_ADCMPR_CMPIE_Msk, (0x1 << 1));
    LPADC_DisableInt(psLpadc, LPADC_CMP1_INT);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADIE_Msk, (0x0));
    CU_ASSERT_EQUAL(psLpadc->ADCMPR[0] & LPADC_ADCMPR_CMPIE_Msk, (0x0));
    CU_ASSERT_EQUAL(psLpadc->ADCMPR[1] & LPADC_ADCMPR_CMPIE_Msk, (0x0));


    for (u32Cnt = 0; u32Cnt < 16384; u32Cnt++)
    {
        LPADC_SetExtendSampleTime(psLpadc, 0, u32Cnt);
        CU_ASSERT_EQUAL(psLpadc->ESMPCTL & LPADC_ESMPCTL_EXTSMPT_Msk, u32Cnt);
    }

    for (moduleNum = 0; moduleNum < 32; moduleNum++)    /* valid sample modules are 0 ~ 27 */
    {
        /*Not suport the 24-27 LPADC Channel*/
        if (moduleNum > 23 && moduleNum <= 27)
            continue;

        moduleMask = BIT0 << moduleNum ;
        LPADC_Open(psLpadc, LPADC_ADCR_DIFFEN_SINGLE_END, LPADC_ADCR_ADMD_SINGLE, moduleMask);
        CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_DIFFEN_Msk, 0x0);
        CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADMD_Msk, 0x0);
        CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADEN_Msk, 0x1);
        CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_ADPRDY_Msk, 0x1000000);
        CU_ASSERT_EQUAL(psLpadc->ADCHER & LPADC_ADCHER_CHEN_Msk, BIT0 << moduleNum);

        LPADC_CLR_INT_FLAG(psLpadc, LPADC_ADSR0_ADF_Msk);
        LPADC_START_CONV(psLpadc);

#ifndef WRONG_FPGA_CODE
        //        while(LPADC_GET_INT_FLAG(psLpadc, LPADC_ADSR0_ADF_Msk) == 0) {}
#endif
        LPADC_CLR_INT_FLAG(psLpadc, LPADC_ADSR0_ADF_Msk);

        unsigned int conversion_result;
        conversion_result = LPADC_GET_CONVERSION_DATA(psLpadc, moduleNum);

        LPADC_Close(psLpadc);
        CU_ASSERT_EQUAL(psLpadc->ADCAL & LPADC_ADCR_ADEN_Msk, 0);
    }

    LPADC_Calibration(psLpadc);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADEN_Msk, LPADC_ADCR_ADEN_Msk);
    CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_ADPRDY_Msk, LPADC_ADSR0_ADPRDY_Msk);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_RESET_Msk, 0);
    CU_ASSERT_EQUAL(psLpadc->ADCAL & LPADC_ADCAL_CALEN_Msk, 0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADST_Msk, 0);
    CU_ASSERT_EQUAL(psLpadc->ADCALSTS & LPADC_ADCALSTS_CALIF_Msk, 0);
    return;
}

void CU_LPADC_API_Test(void)
{
    CU_LPADC_API_Function_Test(LPADC0);
}
