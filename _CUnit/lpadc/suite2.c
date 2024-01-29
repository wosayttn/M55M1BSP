/*---------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                             */
/*                                                                                                         */
/* Copyright (c) 2017 by Nuvoton Technology Corporation                                                    */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   suite2.c                                                                                              */
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

//#define M258             0
//#define M256D            1
//#define M258G            2
//#define CHIP_VERSION     M258G

/*---------------------------------------------------------------------------------------------------------*/
/* CUnit Test Function                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/



uint32_t tmp0, tmp1;
void LPADC_MACRO_BASIC_TEST(LPADC_T *psLpadc);
void LPADC_MACRO_COMP_TEST(LPADC_T *psLpadc);
void LPADC_MACRO_OTHER_TEST(LPADC_T *psLpadc);


void CU_S2T1_MACRO_LPADC_BASIC(void)
{
    LPADC_MACRO_BASIC_TEST(LPADC0);
}
void LPADC_MACRO_BASIC_TEST(LPADC_T *psLpadc)
{
    unsigned int moduleNum, moduleMask;
    unsigned int intNum;

    intNum = 0;
    // Power on LPADC
    LPADC_POWER_ON(psLpadc);
    // ADCRST bit remains 1 during ADC reset, when ADC reset end, the ADCRST bit is automatically cleared to 0.
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADEN_Msk, (0x1 << 0));

    while (!(psLpadc->ADSR0 & LPADC_ADSR0_ADPRDY_Msk)) {}

    CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_ADPRDY_Msk, 0x1000000);
    // Power off LPADC
    LPADC_POWER_DOWN(psLpadc);
    // ADCRST bit remains 1 during ADC reset, when ADC reset end, the ADCRST bit is automatically cleared to 0.
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADEN_Msk, 0x0);
    CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_ADPRDY_Msk, 0x0);

    /*--- Run a  LPADC conversion sample ---*/
    for (moduleNum = 0; moduleNum < 24; moduleNum++)    /* valid sample modules are 0 ~ 15 for MxCTL1  */
    {

        moduleMask = (BIT0 << moduleNum);

        LPADC_Open(psLpadc, LPADC_ADCR_DIFFEN_SINGLE_END, LPADC_ADCR_ADMD_SINGLE, moduleMask);

        /*Not suport the 24-27 LPADC Channel*/
        if (moduleNum > 23 && moduleNum <= 27)
            continue;

        LPADC_CLR_INT_FLAG(psLpadc, LPADC_ADF_INT);
        CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_ADF_Msk, 0);
        LPADC_SET_INPUT_CHANNEL(psLpadc, moduleMask);
        CU_ASSERT_EQUAL(psLpadc->ADCHER & LPADC_ADCHER_CHEN_Msk, moduleMask)
        LPADC_START_CONV(psLpadc);

        /* Don't test EADC_START_CONV() here for timing issue. Test it later. */
        while (LPADC_IS_BUSY(psLpadc) == 0) {};

        unsigned int tmp_result;

        CU_ASSERT_EQUAL((psLpadc->ADSR0 & LPADC_ADSR0_BUSY_Msk) >> LPADC_ADSR0_BUSY_Pos, 1);

        while (LPADC_IS_BUSY(psLpadc));


        tmp_result = LPADC_GET_CONVERSION_DATA(psLpadc, moduleNum);
        // Ignore EADC_GET_INT_FLAG() if run wrong FPGA code
#ifndef WRONG_FPGA_CODE

        //        while (LPADC_GET_INT_FLAG(psLpadc, LPADC_ADF_INT) == 0) {}

        //        CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_ADF_Msk, LPADC_ADF_INT);
#endif

        LPADC_CLR_INT_FLAG(psLpadc, LPADC_ADF_INT);
        CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_ADF_Msk, 0);

        tmp_result = LPADC_IS_DATA_OVERRUN(psLpadc, moduleMask);
        CU_ASSERT_EQUAL(((psLpadc->ADSR2 & moduleMask)), tmp_result);


        tmp_result = LPADC_IS_DATA_VALID(psLpadc, moduleMask);

        CU_ASSERT_EQUAL(((psLpadc->ADSR1 & moduleMask)), tmp_result);


        LPADC_Close(psLpadc);
    }


    return;
}



void CU_S2T2_MACRO_LPADC_COMP(void)
{
    LPADC_MACRO_COMP_TEST(LPADC0);
}
void LPADC_MACRO_COMP_TEST(LPADC_T *psLpadc)
{
    unsigned int moduleNum, moduleMask;
    unsigned int intNum;

    intNum = 0;

    for (moduleNum = 0; moduleNum < 32; moduleNum++)
    {
        /*Not suport the 24-27 LPADC Channel*/
        if (moduleNum > 23 && moduleNum <= 27)
            continue;

        moduleMask = (BIT0 << moduleNum);

        LPADC_Open(psLpadc, LPADC_ADCR_DIFFEN_SINGLE_END, LPADC_ADCR_ADMD_SINGLE, moduleMask);

        LPADC_CLR_INT_FLAG(psLpadc, LPADC_ADF_INT);
        CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_ADF_Msk, 0);

        CU_ASSERT_EQUAL((LPADC0->ADCHER & moduleMask), moduleMask);

        //       LPADC_EnableInt(psLpadc,LPADC_CMP0_INT);
        LPADC_ENABLE_CMP0(psLpadc, moduleNum, LPADC_ADCMPR_CMPCOND_LESS_THAN, 0x500, 1);
        CU_ASSERT_EQUAL(psLpadc->ADCMPR[0],
                        (moduleNum << LPADC_ADCMPR_CMPCH_Pos) |
                        (EADC_CMP_CMPCOND_LESS_THAN) |
                        (0x500 << LPADC_ADCMPR_CMPD_Pos) |
                        ((0) << LPADC_ADCMPR_CMPMATCNT_Pos) |
                        (LPADC_ADCMPR_CMPEN_Msk)
                       );
        LPADC_DisableInt(psLpadc, LPADC_CMP0_INT);


        //       LPADC_EnableInt(psLpadc,LPADC_CMP1_INT);
        LPADC_ENABLE_CMP1(psLpadc, moduleNum, LPADC_ADCMPR_CMPCOND_GREATER_OR_EQUAL, 0x500, 1);
        CU_ASSERT_EQUAL(psLpadc->ADCMPR[1],
                        (moduleNum << LPADC_ADCMPR_CMPCH_Pos) |
                        (LPADC_ADCMPR_CMPCOND_GREATER_OR_EQUAL) |
                        (0x500 << LPADC_ADCMPR_CMPD_Pos) |
                        ((0) << LPADC_ADCMPR_CMPMATCNT_Pos) |
                        (LPADC_ADCMPR_CMPEN_Msk)
                       );
        LPADC_DisableInt(psLpadc, LPADC_CMP0_INT);


        LPADC_START_CONV(psLpadc);

        // Ignore EADC_GET_INT_FLAG() if run wrong FPGA code
#ifndef WRONG_FPGA_CODE

        //       while (LPADC_GET_INT_FLAG(psLpadc, LPADC_ADF_INT) == 0);

#endif
        LPADC_CLR_INT_FLAG(psLpadc, LPADC_ADF_INT);

        unsigned int conversion_result, tmp_result;
        conversion_result = LPADC_GET_CONVERSION_DATA(psLpadc, moduleNum);

        tmp_result = LPADC_GET_INT_FLAG(psLpadc, LPADC_CMP0_INT);
        CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_CMPF0_Msk, tmp_result);
        LPADC_CLR_INT_FLAG(psLpadc, LPADC_ADSR0_CMPF0_Msk);
        CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_CMPF0_Msk, 0);

        tmp_result = LPADC_GET_INT_FLAG(psLpadc, LPADC_CMP1_INT);
        CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_CMPF1_Msk, tmp_result);
        LPADC_CLR_INT_FLAG(psLpadc, LPADC_ADSR0_CMPF1_Msk);
        CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_CMPF1_Msk, 0);



        LPADC_EnableInt(psLpadc, LPADC_CMP0_INT);
        CU_ASSERT_EQUAL(psLpadc->ADCMPR[0] & LPADC_ADCMPR_CMPIE_Msk, (0x1 << 1));
        LPADC_DisableInt(psLpadc, LPADC_CMP0_INT);
        CU_ASSERT_EQUAL(psLpadc->ADCMPR[0] & LPADC_ADCMPR_CMPIE_Msk, (0x0));
        LPADC_EnableInt(psLpadc, LPADC_CMP1_INT);
        CU_ASSERT_EQUAL(psLpadc->ADCMPR[1] & LPADC_ADCMPR_CMPIE_Msk, (0x1 << 1));
        LPADC_DisableInt(psLpadc, LPADC_CMP1_INT);
        CU_ASSERT_EQUAL(psLpadc->ADCMPR[1] & LPADC_ADCMPR_CMPIE_Msk, (0x0));


        LPADC_ENABLE_CMP_WINDOW_MODE(psLpadc, 0);
        CU_ASSERT_EQUAL(psLpadc->ADCMPR[0] & LPADC_ADCMPR_CMPWEN_Msk, 0x1 << 15);
        LPADC_DISABLE_CMP_WINDOW_MODE(psLpadc, 0);
        CU_ASSERT_EQUAL(psLpadc->ADCMPR[0] & LPADC_ADCMPR_CMPWEN_Msk, 0);

        LPADC_DISABLE_CMP0(psLpadc);
        CU_ASSERT_EQUAL(psLpadc->ADCMPR[0], 0x0);

        LPADC_DISABLE_CMP1(psLpadc);
        CU_ASSERT_EQUAL(psLpadc->ADCMPR[1], 0x0);

    }

    return;
}

void CU_S2T4_MACRO_LPADC_OTHER(void)
{

    LPADC_MACRO_OTHER_TEST(LPADC0);

}
void LPADC_MACRO_OTHER_TEST(LPADC_T *psLpadc)
{
    unsigned int moduleNum, moduleMask;
    LPADC_Open(psLpadc, LPADC_ADCR_DIFFEN_SINGLE_END, LPADC_ADCR_ADMD_SINGLE, BIT0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_DIFFEN_Msk, 0x0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADMD_Msk, 0x0);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_ADEN_Msk, 0x1);
    CU_ASSERT_EQUAL(psLpadc->ADSR0 & LPADC_ADSR0_ADPRDY_Msk, 0x1000000);
    CU_ASSERT_EQUAL(psLpadc->ADCHER & LPADC_ADCHER_CHEN_Msk, 0x01);


    for (moduleNum = 0; moduleNum < 28; moduleNum++)
    {
        /*Not suport the 24-27 LPADC Channel*/
        if (moduleNum > 23 && moduleNum <= 27)
            continue;

        moduleMask = (BIT0 << moduleNum);
        LPADC_SET_INPUT_CHANNEL(psLpadc, moduleMask);

        CU_ASSERT_EQUAL(psLpadc->ADCHER & moduleMask, 0x1 << moduleNum);
    }

    LPADC_SET_DMOF(psLpadc, LPADC_ADCR_DMOF_UNSIGNED_OUTPUT);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_DMOF_Msk, LPADC_ADCR_DMOF_UNSIGNED_OUTPUT);
    LPADC_SET_DMOF(psLpadc, LPADC_ADCR_DMOF_TWOS_COMPLEMENT);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_DMOF_Msk, LPADC_ADCR_DMOF_TWOS_COMPLEMENT);


    LPADC_ENABLE_LPPDMA(psLpadc);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_PTEN_Msk, (0x1 << 9));
    LPADC_DISABLE_LPPDMA(psLpadc);
    CU_ASSERT_EQUAL(psLpadc->ADCR & LPADC_ADCR_PTEN_Msk, (0x0));

    LPADC_AUTO_OP_ENABLE(psLpadc);
    CU_ASSERT_EQUAL(psLpadc->AUTOCTL & LPADC_AUTOCTL_AUTOEN_Msk, (0x1 << 31));
    LPADC_AUTO_OP_DISABLE(psLpadc);
    CU_ASSERT_EQUAL(psLpadc->AUTOCTL & LPADC_AUTOCTL_AUTOEN_Msk, (0x0));

    LPADC_CONVERSION_END_WAKEUP_ENABLE(psLpadc);
    CU_ASSERT_EQUAL(psLpadc->AUTOCTL & LPADC_AUTOCTL_ADWKEN_Msk, (0x1 << 8));
    LPUART_CONVERSION_END_WAKEUP_DISABLE(psLpadc);
    CU_ASSERT_EQUAL(psLpadc->AUTOCTL & LPADC_AUTOCTL_ADWKEN_Msk, (0x0));

    LPADC_COMPARATOR_0_MATCH_WAKEUP_ENABLE(psLpadc);
    CU_ASSERT_EQUAL(psLpadc->AUTOCTL & LPADC_AUTOCTL_CMP0WKEN_Msk, (0x1 << 9));
    LPADC_COMPARATOR_0_MATCH_WAKEUP_DISABLE(psLpadc);
    CU_ASSERT_EQUAL(psLpadc->AUTOCTL & LPADC_AUTOCTL_CMP0WKEN_Msk, (0x0));


    LPADC_COMPARATOR_1_MATCH_WAKEUP_ENABLE(psLpadc);
    CU_ASSERT_EQUAL(psLpadc->AUTOCTL & LPADC_AUTOCTL_CMP1WKEN_Msk, (0x1 << 10));
    LPADC_COMPARATOR_1_MATCH_WAKEUP_DISABLE(psLpadc);
    CU_ASSERT_EQUAL(psLpadc->AUTOCTL & LPADC_AUTOCTL_CMP1WKEN_Msk, (0x0));

    return;
}

