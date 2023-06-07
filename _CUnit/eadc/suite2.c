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
#include "eadc_cunit.h"

// Define WRONG_FPGA_CODE if you use old FPGA code that has some problems.
//#define WRONG_FPGA_CODE

#define M258             0
#define M256D            1
#define M258G            2
#define CHIP_VERSION     M258G

/*---------------------------------------------------------------------------------------------------------*/
/* CUnit Test Function                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/



uint32_t tmp0, tmp1;
void CU_S2T1_MACRO_EADC_BASIC(void)
{
    unsigned int moduleNum, moduleMask;
    unsigned int intNum;

    intNum = 0;

    EADC_CONV_RESET(EADC0);
    // ADCRST bit remains 1 during ADC reset, when ADC reset end, the ADCRST bit is automatically cleared to 0.
    CU_ASSERT_EQUAL(EADC0->CTL & EADC_CTL_ADCRST_Msk, 0);

    EADC_ENABLE_INT(EADC0, BIT0);
    CU_ASSERT_EQUAL(EADC0->CTL & ((BIT0) << EADC_CTL_ADCIEN0_Pos),0x04);
    EADC_DISABLE_INT(EADC0, BIT0);
    CU_ASSERT_EQUAL(EADC0->CTL & ((BIT0) << EADC_CTL_ADCIEN0_Pos), 0);

    EADC_ENABLE_INT(EADC0, BIT1);
    CU_ASSERT_EQUAL(EADC0->CTL & ((BIT1) << EADC_CTL_ADCIEN0_Pos),0x08);
    EADC_DISABLE_INT(EADC0, BIT1);
    CU_ASSERT_EQUAL(EADC0->CTL & ((BIT1) << EADC_CTL_ADCIEN0_Pos), 0);

    EADC_ENABLE_INT(EADC0, BIT2);
    CU_ASSERT_EQUAL(EADC0->CTL & ((BIT2) << EADC_CTL_ADCIEN0_Pos),0x10);
    EADC_DISABLE_INT(EADC0, BIT2);
    CU_ASSERT_EQUAL(EADC0->CTL & ((BIT2) << EADC_CTL_ADCIEN0_Pos), 0);

    EADC_ENABLE_INT(EADC0, BIT3);
    CU_ASSERT_EQUAL(EADC0->CTL & ((BIT3) << EADC_CTL_ADCIEN0_Pos),0x20);
    EADC_DISABLE_INT(EADC0, BIT3);
    CU_ASSERT_EQUAL(EADC0->CTL & ((BIT3) << EADC_CTL_ADCIEN0_Pos), 0);


    for (moduleNum = 0; moduleNum < 28; moduleNum++)    /* valid sample modules are 0 ~ 15 for MxCTL1  */
    {
        moduleMask = (BIT0 << moduleNum);
      
        if(moduleNum < 19)
        {
          EADC_ENABLE_INT_POSITION(EADC0, moduleNum);
          CU_ASSERT_EQUAL((&(EADC0->SCTL[0]))[moduleNum] & EADC_SCTL_INTPOS_Msk, 0x20);
          EADC_DISABLE_INT_POSITION(EADC0, moduleNum);
          CU_ASSERT_EQUAL((&(EADC0->SCTL[0]))[moduleNum] & EADC_SCTL_INTPOS_Msk, 0);
        }else if(moduleNum >= 19 && moduleNum < 24)
        {
           EADC_ENABLE_INT_POSITION(EADC0, moduleNum);
          CU_ASSERT_EQUAL((&(EADC0->SCTL19[0]))[moduleNum] & EADC_SCTL_INTPOS_Msk, 0x20);
          EADC_DISABLE_INT_POSITION(EADC0, moduleNum);
          CU_ASSERT_EQUAL((&(EADC0->SCTL19[0]))[moduleNum] & EADC_SCTL_INTPOS_Msk, 0);
        
        }
        EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, 0, moduleMask);
        CU_ASSERT_EQUAL(EADC0->INTSRC[0] & moduleMask, moduleMask);
        EADC_DISABLE_SAMPLE_MODULE_INT(EADC0, 0, moduleMask);
        CU_ASSERT_EQUAL(EADC0->INTSRC[0] & moduleMask, 0);

        EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, 1, moduleMask);
        CU_ASSERT_EQUAL(EADC0->INTSRC[1] & moduleMask, moduleMask);
        EADC_DISABLE_SAMPLE_MODULE_INT(EADC0, 1, moduleMask);
        CU_ASSERT_EQUAL(EADC0->INTSRC[1] & moduleMask, 0);

        EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, 2, moduleMask);
        CU_ASSERT_EQUAL(EADC0->INTSRC[2] & moduleMask, moduleMask);
        EADC_DISABLE_SAMPLE_MODULE_INT(EADC0, 2, moduleMask);
        CU_ASSERT_EQUAL(EADC0->INTSRC[2] & moduleMask, 0);

        EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, 3, moduleMask);
        CU_ASSERT_EQUAL(EADC0->INTSRC[3] & moduleMask, moduleMask);
        EADC_DISABLE_SAMPLE_MODULE_INT(EADC0, 3, moduleMask);
        CU_ASSERT_EQUAL(EADC0->INTSRC[3] & moduleMask, 0);
      }
        /*--- Run a simple EADC conversion sample ---*/
     for (moduleNum = 0; moduleNum < 28; moduleNum++)    /* valid sample modules are 0 ~ 15 for MxCTL1  */
    {
        moduleMask = (BIT0 << moduleNum);

        EADC_Open(EADC0, EADC_CTL_DIFFEN_SINGLE_END);

        if(moduleNum < 24)
        {
          EADC_ConfigSampleModule(EADC0, moduleNum, EADC_SOFTWARE_TRIGGER, 0);
        }
          EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk);
          CU_ASSERT_EQUAL(EADC0->STATUS2 & EADC_STATUS2_ADIF0_Msk, 0);
         
          EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, intNum, moduleMask);
          CU_ASSERT_EQUAL((&(EADC0->INTSRC[0]))[intNum] & moduleMask, moduleMask);
          EADC_START_CONV(EADC0, moduleMask);
          /* Don't test EADC_START_CONV() here for timing issue. Test it later. */
           while (EADC_IS_BUSY(EADC0) == 0){};
           unsigned int tmp_result;
           tmp_result = EADC_IS_BUSY(EADC0);
           CU_ASSERT_EQUAL((EADC0->STATUS2 & EADC_STATUS2_BUSY_Msk) >> EADC_STATUS2_BUSY_Pos, tmp_result);

            while (EADC_IS_BUSY(EADC0));

        // Ignore EADC_GET_INT_FLAG() if run wrong FPGA code
#ifndef WRONG_FPGA_CODE

        while (EADC_GET_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk) == 0) {}

        CU_ASSERT_EQUAL(EADC0->STATUS2 & EADC_STATUS2_ADIF0_Msk, EADC_STATUS2_ADIF0_Msk);
#endif

        EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk);
        CU_ASSERT_EQUAL(EADC0->STATUS2 & EADC_STATUS2_ADIF0_Msk, 0);

        tmp_result = EADC_GET_DATA_OVERRUN_FLAG(EADC0, moduleMask);
        CU_ASSERT_EQUAL(((EADC0->STATUS0 >> EADC_STATUS0_OV_Pos) | (EADC0->STATUS1 & EADC_STATUS1_OV_Msk)) & moduleMask, tmp_result);

        tmp_result = EADC_IS_DATA_OV(EADC0);
        CU_ASSERT_EQUAL((EADC0->STATUS2 & EADC_STATUS2_AOV_Msk) >> EADC_STATUS2_AOV_Pos, tmp_result);

        tmp_result = EADC_GET_DATA_VALID_FLAG(EADC0, moduleMask);
        CU_ASSERT_EQUAL((((EADC0->STATUS0 & EADC_STATUS0_VALID_Msk) | ((EADC0->STATUS1 & EADC_STATUS1_VALID_Msk) << 16)) & moduleMask), tmp_result);

        tmp_result = EADC_IS_DATA_VALID(EADC0);
        CU_ASSERT_EQUAL((EADC0->STATUS2 & EADC_STATUS2_AVALID_Msk) >> EADC_STATUS2_AVALID_Pos, tmp_result);

        tmp_result = EADC_GET_SAMPLE_MODULE_OV_FLAG(EADC0, moduleMask);
        CU_ASSERT_EQUAL(EADC0->OVSTS & moduleMask, tmp_result);

        tmp_result = EADC_IS_SAMPLE_MODULE_OV(EADC0);
        CU_ASSERT_EQUAL((EADC0->STATUS2 & EADC_STATUS2_STOVF_Msk) >> EADC_STATUS2_STOVF_Pos, tmp_result);

        EADC_CLR_SAMPLE_MODULE_OV_FLAG(EADC0, moduleMask);
        CU_ASSERT_EQUAL(EADC0->OVSTS & moduleMask, 0);

        tmp_result = EADC_IS_INT_FLAG_OV(EADC0);
        CU_ASSERT_EQUAL((EADC0->STATUS2 & EADC_STATUS2_ADOVIF_Msk) >> EADC_STATUS2_ADOVIF_Pos, tmp_result);

        unsigned int conversion_result;
        conversion_result = EADC_GET_CONV_DATA(EADC0, moduleNum);
        CU_ASSERT_EQUAL((&(EADC0->DAT[0]))[moduleNum] & EADC_DAT_RESULT_Msk, conversion_result);


        /* Test EADC_START_CONV() here for timing issue. */
        EADC_START_CONV(EADC0, moduleMask);


        while (EADC_IS_BUSY(EADC0) == 0);


        tmp1 = EADC_GET_PENDING_CONV(EADC0) & moduleMask;

        EADC_STOP_CONV(EADC0, moduleMask);
#ifndef WRONG_FPGA_CODE
        tmp0 = EADC0->PENDSTS;
        CU_ASSERT_EQUAL(tmp1, moduleMask);
        CU_ASSERT_EQUAL(EADC_GET_PENDING_CONV(EADC0) & moduleMask, 0);
#endif
        CU_ASSERT_EQUAL(EADC0->SWTRG & moduleMask, 0);

        EADC_Close(EADC0);
    }


    for (moduleNum = 0; moduleNum < 19; moduleNum++)    /* valid sample modules are 0 ~ 18 for PDMACTL  */
    {
        moduleMask = (BIT0 << moduleNum);
        EADC_ENABLE_SAMPLE_MODULE_PDMA(EADC0, moduleMask);
        CU_ASSERT_EQUAL(EADC0->PDMACTL, 0x1 << moduleNum);
        EADC_DISABLE_SAMPLE_MODULE_PDMA(EADC0, moduleMask);
        CU_ASSERT_EQUAL(EADC0->PDMACTL, 0);
    }



    return;
}


void CU_S2T2_MACRO_EADC_COMP(void)
{
    unsigned int moduleNum, moduleMask;
    unsigned int intNum;

    intNum = 0;
#if(CHIP_VERSION == M258)

    for (moduleNum = 0; moduleNum < 18; moduleNum++)    /* valid sample modules are 0 ~ 18 */
#else
    for (moduleNum = 0; moduleNum < 4; moduleNum++)
#endif
    {
        moduleMask = (BIT0 << moduleNum);

        EADC_Open(EADC0, NULL);



        EADC_ConfigSampleModule(EADC0, moduleNum, EADC_SOFTWARE_TRIGGER, 0);

        EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk);
        CU_ASSERT_EQUAL(EADC0->STATUS2 & EADC_STATUS2_ADIF0_Msk, 0);

        EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, intNum, moduleMask);
        CU_ASSERT_EQUAL((&(EADC0->INTSRC[0]))[intNum] & moduleMask, moduleMask);

        EADC_ENABLE_CMP_INT(EADC0, 0);
        EADC_ENABLE_CMP0(EADC0, moduleNum, EADC_CMP_CMPCOND_LESS_THAN, 0x500, 1);
        CU_ASSERT_EQUAL(EADC0->CMP[0],
                        (moduleNum << EADC_CMP_CMPSPL_Pos) |
                        (EADC_CMP_CMPCOND_LESS_THAN) |
                        (0x500 << EADC_CMP_CMPDAT_Pos) |
                        ((0) << EADC_CMP_CMPMCNT_Pos) |
                        (EADC_CMP_ADCMPEN_Msk) | (EADC_CMP_ADCMPIE_Msk)
                       );
        EADC_DISABLE_CMP_INT(EADC0, 0);

        EADC_ENABLE_CMP_INT(EADC0, 1);
        EADC_ENABLE_CMP1(EADC0, moduleNum, EADC_CMP_CMPCOND_GREATER_OR_EQUAL, 0x500, 1);
        CU_ASSERT_EQUAL(EADC0->CMP[1],
                        (moduleNum << EADC_CMP_CMPSPL_Pos) |
                        (EADC_CMP_CMPCOND_GREATER_OR_EQUAL) |
                        (0x500 << EADC_CMP_CMPDAT_Pos) |
                        ((0) << EADC_CMP_CMPMCNT_Pos) |
                        (EADC_CMP_ADCMPEN_Msk) | (EADC_CMP_ADCMPIE_Msk)
                       );
        EADC_DISABLE_CMP_INT(EADC0, 1);

        EADC_ENABLE_CMP_INT(EADC0, 2);
        EADC_ENABLE_CMP2(EADC0, moduleNum, EADC_CMP_CMPCOND_LESS_THAN, 0xC00, 1);
        CU_ASSERT_EQUAL(EADC0->CMP[2],
                        (moduleNum << EADC_CMP_CMPSPL_Pos) |
                        (EADC_CMP_CMPCOND_LESS_THAN) |
                        (0xC00 << EADC_CMP_CMPDAT_Pos) |
                        ((0) << EADC_CMP_CMPMCNT_Pos) |
                        (EADC_CMP_ADCMPEN_Msk) | (EADC_CMP_ADCMPIE_Msk)
                       );
        EADC_DISABLE_CMP_INT(EADC0, 2);

        EADC_ENABLE_CMP_INT(EADC0, 3);
        EADC_ENABLE_CMP3(EADC0, moduleNum, EADC_CMP_CMPCOND_GREATER_OR_EQUAL, 0xC00, 1);
        CU_ASSERT_EQUAL(EADC0->CMP[3],
                        (moduleNum << EADC_CMP_CMPSPL_Pos) |
                        (EADC_CMP_CMPCOND_GREATER_OR_EQUAL) |
                        (0xC00 << EADC_CMP_CMPDAT_Pos) |
                        ((0) << EADC_CMP_CMPMCNT_Pos) |
                        (EADC_CMP_ADCMPEN_Msk) | (EADC_CMP_ADCMPIE_Msk)
                       );
        EADC_DISABLE_CMP_INT(EADC0, 3);

        EADC_START_CONV(EADC0, moduleMask);

        // Ignore EADC_GET_INT_FLAG() if run wrong FPGA code
#ifndef WRONG_FPGA_CODE

        while (EADC_GET_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk) == 0);

#endif
        EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk);

        unsigned int conversion_result, tmp_result;
        conversion_result = EADC_GET_CONV_DATA(EADC0, moduleNum);

        tmp_result = EADC_GET_INT_FLAG(EADC0, EADC_STATUS2_ADCMPF0_Msk);
        CU_ASSERT_EQUAL(EADC0->STATUS2 & EADC_STATUS2_ADCMPF0_Msk, tmp_result);
        EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADCMPF0_Msk);
        CU_ASSERT_EQUAL(EADC0->STATUS2 & EADC_STATUS2_ADCMPF0_Msk, 0);

        tmp_result = EADC_GET_INT_FLAG(EADC0, EADC_STATUS2_ADCMPF1_Msk);
        CU_ASSERT_EQUAL(EADC0->STATUS2 & EADC_STATUS2_ADCMPF1_Msk, tmp_result);
        EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADCMPF1_Msk);
        CU_ASSERT_EQUAL(EADC0->STATUS2 & EADC_STATUS2_ADCMPF1_Msk, 0);

        tmp_result = EADC_GET_INT_FLAG(EADC0, EADC_STATUS2_ADCMPF2_Msk);
        CU_ASSERT_EQUAL(EADC0->STATUS2 & EADC_STATUS2_ADCMPF2_Msk, tmp_result);
        EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADCMPF2_Msk);
        CU_ASSERT_EQUAL(EADC0->STATUS2 & EADC_STATUS2_ADCMPF2_Msk, 0);

        tmp_result = EADC_GET_INT_FLAG(EADC0, EADC_STATUS2_ADCMPF3_Msk);
        CU_ASSERT_EQUAL(EADC0->STATUS2 & EADC_STATUS2_ADCMPF3_Msk, tmp_result);
        EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADCMPF3_Msk);
        CU_ASSERT_EQUAL(EADC0->STATUS2 & EADC_STATUS2_ADCMPF3_Msk, 0);

        EADC_ENABLE_CMP_INT(EADC0, 0);
        CU_ASSERT_EQUAL(EADC0->CMP[0] & EADC_CMP_ADCMPIE_Msk, EADC_CMP_ADCMPIE_Msk);
        EADC_DISABLE_CMP_INT(EADC0, 0);
        CU_ASSERT_EQUAL(EADC0->CMP[0] & EADC_CMP_ADCMPIE_Msk, 0);

        EADC_ENABLE_CMP_INT(EADC0, 1);
        CU_ASSERT_EQUAL(EADC0->CMP[1] & EADC_CMP_ADCMPIE_Msk, EADC_CMP_ADCMPIE_Msk);
        EADC_DISABLE_CMP_INT(EADC0, 1);
        CU_ASSERT_EQUAL(EADC0->CMP[1] & EADC_CMP_ADCMPIE_Msk, 0);

        EADC_ENABLE_CMP_INT(EADC0, 2);
        CU_ASSERT_EQUAL(EADC0->CMP[2] & EADC_CMP_ADCMPIE_Msk, EADC_CMP_ADCMPIE_Msk);
        EADC_DISABLE_CMP_INT(EADC0, 2);
        CU_ASSERT_EQUAL(EADC0->CMP[2] & EADC_CMP_ADCMPIE_Msk, 0);

        EADC_ENABLE_CMP_INT(EADC0, 3);
        CU_ASSERT_EQUAL(EADC0->CMP[3] & EADC_CMP_ADCMPIE_Msk, EADC_CMP_ADCMPIE_Msk);
        EADC_DISABLE_CMP_INT(EADC0, 3);
        CU_ASSERT_EQUAL(EADC0->CMP[3] & EADC_CMP_ADCMPIE_Msk, 0);

        EADC_ENABLE_CMP_WINDOW_MODE(EADC0, 0);
        CU_ASSERT_EQUAL(EADC0->CMP[0] & EADC_CMP_CMPWEN_Msk, EADC_CMP_CMPWEN_Msk);
        EADC_DISABLE_CMP_WINDOW_MODE(EADC0, 0);
        CU_ASSERT_EQUAL(EADC0->CMP[0] & EADC_CMP_CMPWEN_Msk, 0);

        EADC_ENABLE_CMP_WINDOW_MODE(EADC0, 2);
        CU_ASSERT_EQUAL(EADC0->CMP[2] & EADC_CMP_CMPWEN_Msk, EADC_CMP_CMPWEN_Msk);
        EADC_DISABLE_CMP_WINDOW_MODE(EADC0, 2);
        CU_ASSERT_EQUAL(EADC0->CMP[2] & EADC_CMP_CMPWEN_Msk, 0);

        EADC_DISABLE_CMP0(EADC0);
        CU_ASSERT_EQUAL(EADC0->CMP[0] & EADC_CMP_ADCMPEN_Msk, 0);

        EADC_DISABLE_CMP1(EADC0);
        CU_ASSERT_EQUAL(EADC0->CMP[1] & EADC_CMP_ADCMPEN_Msk, 0);

        EADC_DISABLE_CMP2(EADC0);
        CU_ASSERT_EQUAL(EADC0->CMP[2] & EADC_CMP_ADCMPEN_Msk, 0);

        EADC_DISABLE_CMP3(EADC0);
        CU_ASSERT_EQUAL(EADC0->CMP[3] & EADC_CMP_ADCMPEN_Msk, 0);
    }

    return;
}


void CU_S2T4_MACRO_EADC_OTHER(void)
{
    unsigned int moduleNum;// moduleMask;


    for (moduleNum = 0; moduleNum < 28; moduleNum++)    /* valid sample modules are 0 ~ 15 */
    {
        //moduleMask = (BIT0 << moduleNum);
        if(moduleNum < 19)
        {
          EADC_ENABLE_LEFT_ALIGN(EADC0, moduleNum);
          CU_ASSERT_EQUAL((&(EADC0->MCTL1[0]))[moduleNum] & EADC_MCTL1_ALIGN_Msk, EADC_MCTL1_ALIGN_Msk);
          EADC_DISABLE_LEFT_ALIGN(EADC0, moduleNum);
          CU_ASSERT_EQUAL((&(EADC0->MCTL1[0]))[moduleNum] & EADC_MCTL1_ALIGN_Msk, 0);

          EADC_ENABLE_AVG(EADC0, moduleNum);
          CU_ASSERT_EQUAL((&(EADC0->MCTL1[0]))[moduleNum] & EADC_MCTL1_AVG_Msk, EADC_MCTL1_AVG_Msk);
          EADC_DISABLE_AVG(EADC0, moduleNum);
          CU_ASSERT_EQUAL((&(EADC0->MCTL1[0]))[moduleNum] & EADC_MCTL1_AVG_Msk, 0);

          EADC_ENABLE_ACU(EADC0, moduleNum, EADC_MCTL1_ACU_1);
          CU_ASSERT_EQUAL((&(EADC0->MCTL1[0]))[moduleNum] & EADC_MCTL1_ACU_Msk, EADC_MCTL1_ACU_1);
          EADC_ENABLE_ACU(EADC0, moduleNum, EADC_MCTL1_ACU_16);
          CU_ASSERT_EQUAL((&(EADC0->MCTL1[0]))[moduleNum] & EADC_MCTL1_ACU_Msk, EADC_MCTL1_ACU_16);
          EADC_ENABLE_ACU(EADC0, moduleNum, EADC_MCTL1_ACU_256);
          CU_ASSERT_EQUAL((&(EADC0->MCTL1[0]))[moduleNum] & EADC_MCTL1_ACU_Msk, EADC_MCTL1_ACU_256);
          EADC_DISABLE_ACU(EADC0, moduleNum);
          CU_ASSERT_EQUAL((&(EADC0->MCTL1[0]))[moduleNum] & EADC_MCTL1_ACU_Msk, 0);
        }
         else
        {
          if(moduleNum >= 19 && moduleNum < 24)
          {
            EADC_ENABLE_LEFT_ALIGN(EADC0, moduleNum);
            CU_ASSERT_EQUAL((&(EADC0->M19CTL1[0]))[moduleNum] & EADC_M19CTL1_ALIGN_Msk, EADC_M19CTL1_ALIGN_Msk);
            EADC_DISABLE_LEFT_ALIGN(EADC0, moduleNum);
            CU_ASSERT_EQUAL((&(EADC0->M19CTL1[0]))[moduleNum] & EADC_M19CTL1_ALIGN_Msk, 0);

            EADC_ENABLE_AVG(EADC0, moduleNum);
            CU_ASSERT_EQUAL((&(EADC0->M19CTL1[0]))[moduleNum] & EADC_M19CTL1_AVG_Msk, EADC_M19CTL1_AVG_Msk);
            EADC_DISABLE_AVG(EADC0, moduleNum);
            CU_ASSERT_EQUAL((&(EADC0->M19CTL1[0]))[moduleNum] & EADC_M19CTL1_AVG_Msk, 0);
          } 
          EADC_ENABLE_ACU(EADC0, moduleNum, EADC_MCTL1_ACU_1);
          CU_ASSERT_EQUAL((&(EADC0->M19CTL1[0]))[moduleNum] & EADC_M19CTL1_ACU_Msk, EADC_MCTL1_ACU_1);
          EADC_ENABLE_ACU(EADC0, moduleNum, EADC_MCTL1_ACU_16);
          CU_ASSERT_EQUAL((&(EADC0->M19CTL1[0]))[moduleNum] & EADC_M19CTL1_ACU_Msk, EADC_MCTL1_ACU_16);
          EADC_ENABLE_ACU(EADC0, moduleNum, EADC_MCTL1_ACU_256);
          CU_ASSERT_EQUAL((&(EADC0->M19CTL1[0]))[moduleNum] & EADC_M19CTL1_ACU_Msk, EADC_MCTL1_ACU_256);
          EADC_DISABLE_ACU(EADC0, moduleNum);
          CU_ASSERT_EQUAL((&(EADC0->M19CTL1[0]))[moduleNum] & EADC_MCTL1_ACU_Msk, 0);
        
        }
       
        
    }

   

#if 0 //20200903 removed
    EADC_ENABLE_OFFSETCANCEL(EADC, 0);
    CU_ASSERT_EQUAL(EADC->OFFSETCAL & EADC_OFFSETCAL_OFFSETCANCEL_Msk, 0);
    CU_ASSERT_EQUAL(EADC->CTL & EADC_CTL_CALEN_Msk, EADC_CTL_CALEN_Msk);
    EADC_ENABLE_OFFSETCANCEL(EADC, 0xA);
    CU_ASSERT_EQUAL(EADC->OFFSETCAL & EADC_OFFSETCAL_OFFSETCANCEL_Msk, 0xA);
    CU_ASSERT_EQUAL(EADC->CTL & EADC_CTL_CALEN_Msk, EADC_CTL_CALEN_Msk);
    EADC_ENABLE_OFFSETCANCEL(EADC, 0x1F);
    CU_ASSERT_EQUAL(EADC->OFFSETCAL & EADC_OFFSETCAL_OFFSETCANCEL_Msk, 0x1F);
    CU_ASSERT_EQUAL(EADC->CTL & EADC_CTL_CALEN_Msk, EADC_CTL_CALEN_Msk);
    EADC_DISABLE_OFFSETCANCEL(EADC);
    CU_ASSERT_EQUAL(EADC->OFFSETCAL, 0);
    CU_ASSERT_EQUAL(EADC->CTL & EADC_CTL_CALEN_Msk, 0);
#endif
    return;
}

#if 0
void CU_S2T3_MACRO_EADC_DOUBLE_BUFFER(void)
{
    unsigned int moduleNum, moduleMask, conversion_result, conversion_result2;
    unsigned int intNum;
    unsigned int tmpcnt;

    intNum = 0;

    for (moduleNum = 0; moduleNum < 4; moduleNum++)    /* valid sample modules are 0 ~ 3 for double buffer mode */
    {
        moduleMask = (BIT0 << moduleNum);

        EADC_ENABLE_DOUBLE_BUFFER(EADC, moduleNum);
        CU_ASSERT_EQUAL((&(EADC->SCTL0))[moduleNum] & EADC_SCTL0_DBMEN_Msk, EADC_SCTL0_DBMEN_Msk);

        /*--- Run a simple EADC conversion sample ---*/

        EADC_Open(EADC, NULL);

        while ((EADC->PWRCTL & EADC_PWRCTL_READY_Msk) == 0);

        EADC_ConfigSampleModule(EADC, moduleNum, EADC_SOFTWARE_TRIGGER, 0);

        EADC_CLR_INT_FLAG(EADC, EADC_STATUS2_ADIF0_Msk);
        CU_ASSERT_EQUAL(EADC->STATUS2 & EADC_STATUS2_ADIF0_Msk, 0);

        EADC_ENABLE_SAMPLE_MODULE_INT(EADC, intNum, moduleMask);
        CU_ASSERT_EQUAL((&(EADC->INTSRC[0]))[intNum] & moduleMask, moduleMask);

        /* First EADC conversion */
        EADC_START_CONV(EADC, moduleMask);

        while ((EADC->SWTRG & moduleMask) == moduleMask) {}

        for (tmpcnt = 0; tmpcnt < 256 ; tmpcnt++)
        {
            __NOP();
        }

        //        /* Second EADC conversion */
        EADC_START_CONV(EADC, moduleMask);

        while (EADC_IS_BUSY(EADC) == 0);

        while (EADC_IS_BUSY(EADC) == 1);


        // Ignore EADC_GET_INT_FLAG() if run wrong FPGA code
#ifndef WRONG_FPGA_CODE
        //        while(EADC_GET_INT_FLAG(EADC, EADC_STATUS2_ADIF0_Msk) == 0) {}
        CU_ASSERT_EQUAL(EADC->STATUS2 & EADC_STATUS2_ADIF0_Msk, EADC_STATUS2_ADIF0_Msk);
#endif

        EADC_CLR_INT_FLAG(EADC, EADC_STATUS2_ADIF0_Msk);
        CU_ASSERT_EQUAL(EADC->STATUS2 & EADC_STATUS2_ADIF0_Msk, 0);

        conversion_result = EADC_GET_CONV_DATA(EADC, moduleNum);
        CU_ASSERT_EQUAL((&(EADC->DAT[0]))[moduleNum] & EADC_DAT0_RESULT_Msk, conversion_result);

        conversion_result2 = EADC_GET_DOUBLE_DATA(EADC, moduleNum);
        CU_ASSERT_EQUAL((&(EADC->DDAT[0]))[moduleNum] & EADC_DDAT0_RESULT_Msk, conversion_result2);

        EADC_DISABLE_DOUBLE_BUFFER(EADC, moduleNum);
        CU_ASSERT_EQUAL((&(EADC->SCTL[0]))[moduleNum] & EADC_SCTL0_DBMEN_Msk, 0);

        EADC_Close(EADC);
    }

    return;
}
#endif