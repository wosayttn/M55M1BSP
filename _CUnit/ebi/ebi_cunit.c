/******************************************************************************
* @file    ebi_cunit.c
* @version V1.00
* @brief   EBI CUnit test
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "ebi_cunit.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

//#define D_msg   printf
#define D_msg(...)

extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int suite_success_init(void)
{
    return 0;
}
int suite_success_clean(void)
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

CU_SuiteInfo suites[] =
{
    {"EBI MACRO", suite_success_init, suite_success_clean, NULL, NULL, EBI_MACRO},
    {"EBI API", suite_success_init, suite_success_clean, NULL, NULL, EBI_API},
    CU_SUITE_INFO_NULL
};

void EBI_InitPins(void)
{
        /* EBI MCLK, ALE, nRD, nWR */
        SET_EBI_ALE_PA8();
        SET_EBI_MCLK_PA9();
        SET_EBI_nRD_PA11();
        SET_EBI_nWR_PA10();
        
        /* EBI nWRH and nWRL pins */
#if 0   /* TESTCHIP_ONLY not support */    
        SET_EBI_nWRH_PG8();
#else
        SET_EBI_nWRH_PB6();
#endif
        SET_EBI_nWRL_PB7();
         
        /* EBI nCS0 */
        SET_EBI_nCS0_PD12();
        /* EBI nCS1 */
        SET_EBI_nCS1_PD11();
        /* EBI nCS2 */
        SET_EBI_nCS2_PD10();
                
        /* AD0 ~ AD15 */
        SET_EBI_AD0_PC0();
        SET_EBI_AD1_PC1();
        SET_EBI_AD2_PC2();
        SET_EBI_AD3_PC3();
        SET_EBI_AD4_PC4();
        SET_EBI_AD5_PC5();
        SET_EBI_AD6_PD8();
        SET_EBI_AD7_PD9();
        SET_EBI_AD8_PE14();
        SET_EBI_AD9_PE15();
#if 0   /* TESTCHIP_ONLY not support */        
        SET_EBI_AD10_PD3();
        SET_EBI_AD11_PC14();
        SET_EBI_AD12_PB15();
        SET_EBI_AD13_PB14();
#else
        SET_EBI_AD10_PE7();
        SET_EBI_AD11_PE6();
        SET_EBI_AD12_PH8();
        SET_EBI_AD13_PH9();
#endif
        SET_EBI_AD14_PH10();
        SET_EBI_AD15_PH11();
            
        /* ADR0 ~ ADR19 */
        SET_EBI_ADR0_PH7();
        SET_EBI_ADR1_PB4();
        SET_EBI_ADR2_PB3();
        SET_EBI_ADR3_PB2();
        SET_EBI_ADR4_PC12();
        SET_EBI_ADR5_PC11();
        SET_EBI_ADR6_PC10();
        SET_EBI_ADR7_PC9();
        SET_EBI_ADR8_PB1();
        SET_EBI_ADR9_PG1();
        SET_EBI_ADR10_PC13();
        SET_EBI_ADR11_PG2();
        SET_EBI_ADR12_PG3();
        SET_EBI_ADR13_PG4();
        SET_EBI_ADR14_PF11();
        SET_EBI_ADR15_PE13();
        SET_EBI_ADR16_PB11();
        SET_EBI_ADR17_PB10();
        SET_EBI_ADR18_PF7();
        SET_EBI_ADR19_PF6();
}

int32_t EBI_InitClock(void)
{
    CLK_EnableModuleClock(EBI0_MODULE);
    
    if (CLK->EBICTL & CLK_EBICTL_EBI0CKEN_Msk)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int32_t ResetEBIRegister(uint32_t u32BankNum)
{
    if (u32BankNum == EBI_BANK0)
    {
        EBI->CTL0 = 0x0;
        EBI->TCTL0 = 0x0;

        if ((EBI->CTL0 == 0) && (EBI->TCTL0 == 0))
            return 0;
    }
    else if (u32BankNum == EBI_BANK1)
    {
        EBI->CTL1 = 0x0;
        EBI->TCTL1 = 0x0;

        if ((EBI->CTL1 == 0) && (EBI->TCTL1 == 0))
            return 0;
    }
    else if (u32BankNum == EBI_BANK2)
    {
        EBI->CTL2 = 0x0;
        EBI->TCTL2 = 0x0;

        if ((EBI->CTL2 == 0) && (EBI->TCTL2 == 0))
            return 0;
    }

    return -1;
}

const uint32_t au32MCLKSel[] =
{
    EBI_MCLKDIV_1,
    EBI_MCLKDIV_2,
    EBI_MCLKDIV_4,
    EBI_MCLKDIV_8,
    EBI_MCLKDIV_16,
    EBI_MCLKDIV_32,
    EBI_MCLKDIV_64,
    EBI_MCLKDIV_128,
};

const uint32_t au32TimingSel[] =
{
    EBI_TIMING_FASTEST,
    EBI_TIMING_VERYFAST,
    EBI_TIMING_FAST,
    EBI_TIMING_NORMAL,
    EBI_TIMING_SLOW,
    EBI_TIMING_VERYSLOW,
    EBI_TIMING_SLOWEST,
};

const uint32_t au32TimingSetting[] =
{
    0x00000000, 0x00000000,
    0x00030000, 0x03003318,
    0x00000100, 0x00000000,
    0x00030100, 0x03003318,
    0x00070100, 0x07007738,
    0x00070200, 0x07007738,
    0x00070300, 0x07007738,
};

void CONSTANT_EBI(void)
{
    volatile uint32_t i;

    /* Init EBI clock */
    CU_ASSERT_EQUAL(EBI_InitClock(), 0);

    CU_ASSERT_EQUAL(EBI_BANK0_BASE_ADDR, 0x60000000);
    CU_ASSERT_EQUAL(EBI_BANK1_BASE_ADDR, 0x60100000);
    CU_ASSERT_EQUAL(EBI_BANK2_BASE_ADDR, 0x60200000);

    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);

    CU_ASSERT_EQUAL(EBI_MAX_SIZE, (1024 * 1024));

    /* Check EBI CS Active Level Constant */
    EBI->CTL0 = (EBI_CS_ACTIVE_LOW << EBI_CTL_CSPOLINV_Pos);
    CU_ASSERT_EQUAL(EBI->CTL0, 0x0);
    EBI->CTL0 = (EBI_CS_ACTIVE_HIGH << EBI_CTL_CSPOLINV_Pos);
    CU_ASSERT_EQUAL(EBI->CTL0, 0x4);

    EBI->CTL1 = (EBI_CS_ACTIVE_LOW << EBI_CTL_CSPOLINV_Pos);
    CU_ASSERT_EQUAL(EBI->CTL1, 0x0);
    EBI->CTL1 = (EBI_CS_ACTIVE_HIGH << EBI_CTL_CSPOLINV_Pos);
    CU_ASSERT_EQUAL(EBI->CTL1, 0x4);

    EBI->CTL2 = (EBI_CS_ACTIVE_LOW << EBI_CTL_CSPOLINV_Pos);
    CU_ASSERT_EQUAL(EBI->CTL2, 0x0);
    EBI->CTL2 = (EBI_CS_ACTIVE_HIGH << EBI_CTL_CSPOLINV_Pos);
    CU_ASSERT_EQUAL(EBI->CTL2, 0x4);

    /* Check EBI MCLK divider Constant */
    for (i = 0; i < (sizeof(au32MCLKSel) / sizeof(au32MCLKSel[0])); i++)
    {
        D_msg("EBI0 MCLKDiv %d\n", i);
        EBI->CTL0 = au32MCLKSel[i] << EBI_CTL_MCLKDIV_Pos;
        CU_ASSERT_EQUAL(EBI->CTL0, (i << EBI_CTL_MCLKDIV_Pos));
    }

    for (i = 0; i < (sizeof(au32MCLKSel) / sizeof(au32MCLKSel[0])); i++)
    {
        D_msg("EBI1 MCLKDiv %d\n", i);
        EBI->CTL1 = au32MCLKSel[i] << EBI_CTL_MCLKDIV_Pos;
        CU_ASSERT_EQUAL(EBI->CTL1, (i << EBI_CTL_MCLKDIV_Pos));
    }

    for (i = 0; i < (sizeof(au32MCLKSel) / sizeof(au32MCLKSel[0])); i++)
    {
        D_msg("EBI2 MCLKDiv %d\n", i);
        EBI->CTL2 = au32MCLKSel[i] << EBI_CTL_MCLKDIV_Pos;
        CU_ASSERT_EQUAL(EBI->CTL2, (i << EBI_CTL_MCLKDIV_Pos));
    }

    /* Check EBI_ENABLE_WRITE_BUFFER, EBI_DISABLE_WRITE_BUFFER MACRO*/
    EBI_ENABLE_WRITE_BUFFER();
    CU_ASSERT_EQUAL((EBI->CTL0 & BIT24), BIT24);
    CU_ASSERT_EQUAL((EBI->CTL1 & BIT24), 0x0);
    CU_ASSERT_EQUAL((EBI->CTL2 & BIT24), 0x0);
    EBI_DISABLE_WRITE_BUFFER();
    CU_ASSERT_EQUAL((EBI->CTL0 & BIT24), 0x0);

    /* Check EBI_OPMODE_CACCESS, EBI_OPMODE_NORMAL Constant*/
    EBI->CTL0 = 0x0;
    EBI->CTL1 = 0x0;
    EBI->CTL2 = 0x0;
    EBI->CTL0 = EBI_OPMODE_CACCESS;
    EBI->CTL1 = EBI_OPMODE_CACCESS;
    EBI->CTL2 = EBI_OPMODE_CACCESS;
    CU_ASSERT_EQUAL((EBI->CTL0 & BIT4), BIT4);
    CU_ASSERT_EQUAL((EBI->CTL1 & BIT4), BIT4);
    CU_ASSERT_EQUAL((EBI->CTL2 & BIT4), BIT4);
    EBI->CTL0 = EBI_OPMODE_NORMAL;
    EBI->CTL1 = EBI_OPMODE_NORMAL;
    EBI->CTL2 = EBI_OPMODE_NORMAL;
    CU_ASSERT_EQUAL(EBI->CTL0, 0x0);
    CU_ASSERT_EQUAL(EBI->CTL1, 0x0);
    CU_ASSERT_EQUAL(EBI->CTL2, 0x0);

#if 0
    printf("\n\n*** NOTE: Test Read/Write MACRO in real chip ***\n\n");
#else
    printf("\n\n*** NOTE: Test Read/Write MACRO in real chip ***\n\n");
    /* Check EBI0 Read/Write MACRO */
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
    EBI_InitPins();
    EBI_Open(EBI_BANK0, EBI_BUSWIDTH_8BIT, EBI_TIMING_FASTEST, 0, EBI_CS_ACTIVE_LOW);
    CU_ASSERT_EQUAL(EBI0_READ_DATA8(0x0), 0x0);
    CU_ASSERT_EQUAL(EBI0_READ_DATA8(0x1), 0x1);
    CU_ASSERT_EQUAL(EBI0_READ_DATA8(0x2), 0x2);
    CU_ASSERT_EQUAL(EBI0_READ_DATA8(0x3), 0x3);

    EBI_Open(EBI_BANK0, EBI_BUSWIDTH_16BIT, EBI_TIMING_FASTEST, 0, EBI_CS_ACTIVE_HIGH);
    CU_ASSERT_EQUAL(EBI0_READ_DATA16(0x0), 0x0000);
    CU_ASSERT_EQUAL(EBI0_READ_DATA16(0x2), 0x0001);
    CU_ASSERT_EQUAL(EBI0_READ_DATA16(0x4), 0x0002);
    CU_ASSERT_EQUAL(EBI0_READ_DATA16(0x6), 0x0003);

    CU_ASSERT_EQUAL(EBI0_READ_DATA32(0x0), 0x00010000);
    CU_ASSERT_EQUAL(EBI0_READ_DATA32(0x4), 0x00030002);
    CU_ASSERT_EQUAL(EBI0_READ_DATA32(0x8), 0x00050004);
    CU_ASSERT_EQUAL(EBI0_READ_DATA32(0xC), 0x00070006);

    EBI0_WRITE_DATA8(0, 0xFF);
    CU_ASSERT_EQUAL(EBI0_READ_DATA8(0), 0xFF);
    EBI0_WRITE_DATA16(0, 0x0000);
    CU_ASSERT_EQUAL(EBI0_READ_DATA16(0), 0x0);
    EBI0_WRITE_DATA32(0, 0xFFFFFFFF);
    CU_ASSERT_NOT_EQUAL(EBI0_READ_DATA32(0), 0x0);
    EBI0_WRITE_DATA16(0, 0x0000);

    /* Check EBI1 Read/Write MACRO */
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
    //EBI_InitPins();
    EBI_Open(EBI_BANK1, EBI_BUSWIDTH_8BIT, EBI_TIMING_FASTEST, 0, EBI_CS_ACTIVE_LOW);
    CU_ASSERT_EQUAL(EBI1_READ_DATA8(0x0), 0x0);
    CU_ASSERT_EQUAL(EBI1_READ_DATA8(0x1), 0x1);
    CU_ASSERT_EQUAL(EBI1_READ_DATA8(0x2), 0x2);
    CU_ASSERT_EQUAL(EBI1_READ_DATA8(0x3), 0x3);

    EBI_Open(EBI_BANK1, EBI_BUSWIDTH_16BIT, EBI_TIMING_FASTEST, 0, EBI_CS_ACTIVE_HIGH);
    CU_ASSERT_EQUAL(EBI1_READ_DATA16(0x0), 0x0000);
    CU_ASSERT_EQUAL(EBI1_READ_DATA16(0x2), 0x0001);
    CU_ASSERT_EQUAL(EBI1_READ_DATA16(0x4), 0x0002);
    CU_ASSERT_EQUAL(EBI1_READ_DATA16(0x6), 0x0003);

    CU_ASSERT_EQUAL(EBI1_READ_DATA32(0x0), 0x00010000);
    CU_ASSERT_EQUAL(EBI1_READ_DATA32(0x4), 0x00030002);
    CU_ASSERT_EQUAL(EBI1_READ_DATA32(0x8), 0x00050004);
    CU_ASSERT_EQUAL(EBI1_READ_DATA32(0xC), 0x00070006);

    EBI1_WRITE_DATA8(0, 0xFF);
    CU_ASSERT_EQUAL(EBI1_READ_DATA8(0), 0xFF);
    EBI1_WRITE_DATA16(0, 0x0000);
    CU_ASSERT_EQUAL(EBI1_READ_DATA16(0), 0x0);
    EBI1_WRITE_DATA32(0, 0xFFFFFFFF);
    CU_ASSERT_NOT_EQUAL(EBI1_READ_DATA32(0), 0x0);
    EBI1_WRITE_DATA16(0, 0x0000);

    /* Check EBI2 Read/Write MACRO */
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
    //EBI_InitPins();
    EBI_Open(EBI_BANK2, EBI_BUSWIDTH_8BIT, EBI_TIMING_FASTEST, 0, EBI_CS_ACTIVE_LOW);
    CU_ASSERT_EQUAL(EBI2_READ_DATA8(0x0), 0x0);
    CU_ASSERT_EQUAL(EBI2_READ_DATA8(0x1), 0x1);
    CU_ASSERT_EQUAL(EBI2_READ_DATA8(0x2), 0x2);
    CU_ASSERT_EQUAL(EBI2_READ_DATA8(0x3), 0x3);

    EBI_Open(EBI_BANK2, EBI_BUSWIDTH_16BIT, EBI_TIMING_FASTEST, 0, EBI_CS_ACTIVE_HIGH);
    CU_ASSERT_EQUAL(EBI2_READ_DATA16(0x0), 0x0000);
    CU_ASSERT_EQUAL(EBI2_READ_DATA16(0x2), 0x0001);
    CU_ASSERT_EQUAL(EBI2_READ_DATA16(0x4), 0x0002);
    CU_ASSERT_EQUAL(EBI2_READ_DATA16(0x6), 0x0003);

    CU_ASSERT_EQUAL(EBI2_READ_DATA32(0x0), 0x00010000);
    CU_ASSERT_EQUAL(EBI2_READ_DATA32(0x4), 0x00030002);
    CU_ASSERT_EQUAL(EBI2_READ_DATA32(0x8), 0x00050004);
    CU_ASSERT_EQUAL(EBI2_READ_DATA32(0xC), 0x00070006);

    EBI2_WRITE_DATA8(0, 0xFF);
    CU_ASSERT_EQUAL(EBI2_READ_DATA8(0), 0xFF);
    EBI2_WRITE_DATA16(0, 0x0000);
    CU_ASSERT_EQUAL(EBI2_READ_DATA16(0), 0x0);
    EBI2_WRITE_DATA32(0, 0xFFFFFFFF);
    CU_ASSERT_NOT_EQUAL(EBI2_READ_DATA32(0), 0x0);
    EBI2_WRITE_DATA16(0, 0x0000);
#endif
}

void API_EBI_Open(void)
{
    volatile uint32_t i;

    D_msg("Open\n", i);
    /* Init EBI clock */
    CU_ASSERT_EQUAL(EBI_InitClock(), 0);

    /* Check EBI_Open and EBI_Close API */
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
    EBI->CTL1 = 0x5;
    EBI->TCTL1 = 0xA0;
    EBI->CTL2 = 0x5;
    EBI->TCTL2 = 0xA0;

    for (i = 0; i < (sizeof(au32TimingSel) / sizeof(au32TimingSel[0])); i++)
    {
        D_msg("EBI0 TimingSel %d\n", i);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
        EBI_Open(EBI_BANK0, EBI_BUSWIDTH_8BIT, au32TimingSel[i], EBI_OPMODE_NORMAL, EBI_CS_ACTIVE_LOW);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL0, (0x00000001 | au32TimingSetting[i * 2]));
        CU_ASSERT_EQUAL(EBI->TCTL0, au32TimingSetting[(i * 2) + 1]);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
        EBI_Open(EBI_BANK0, EBI_BUSWIDTH_8BIT, au32TimingSel[i], EBI_OPMODE_CACCESS, EBI_CS_ACTIVE_HIGH);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL0, (0x00000015 | au32TimingSetting[i * 2]));
        CU_ASSERT_EQUAL(EBI->TCTL0, au32TimingSetting[(i * 2) + 1]);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
        EBI_Open(EBI_BANK0, EBI_BUSWIDTH_16BIT, au32TimingSel[i], EBI_OPMODE_ADSEPARATE, EBI_CS_ACTIVE_LOW);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL0, (0x0000000B | au32TimingSetting[i*2]));
        CU_ASSERT_EQUAL(EBI->TCTL0, au32TimingSetting[(i*2)+1]);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
        EBI_Open(EBI_BANK0, EBI_BUSWIDTH_16BIT, au32TimingSel[i], EBI_OPMODE_NORMAL, EBI_CS_ACTIVE_HIGH);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL0, (0x00000007 | au32TimingSetting[i * 2]));
        CU_ASSERT_EQUAL(EBI->TCTL0, au32TimingSetting[(i * 2) + 1]);
        
        EBI_Close(EBI_BANK0);
        CU_ASSERT_EQUAL(EBI->CTL0, (0x00000006 | au32TimingSetting[i * 2]));
    }

    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
    EBI->CTL0 = 0x5;
    EBI->TCTL0 = 0xA0;
    EBI->CTL2 = 0x5;
    EBI->TCTL2 = 0xA0;

    for (i = 0; i < (sizeof(au32TimingSel) / sizeof(au32TimingSel[0])); i++)
    {
        D_msg("EBI1 TimingSel %d\n", i);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
        EBI_Open(EBI_BANK1, EBI_BUSWIDTH_8BIT, au32TimingSel[i], EBI_OPMODE_NORMAL, EBI_CS_ACTIVE_LOW);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, (0x00000001 | (au32TimingSetting[i * 2] & ~EBI_CTL_TALE_Msk)));
        CU_ASSERT_EQUAL(EBI->TCTL1, au32TimingSetting[(i * 2) + 1]);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
        EBI_Open(EBI_BANK1, EBI_BUSWIDTH_8BIT, au32TimingSel[i], EBI_OPMODE_CACCESS, EBI_CS_ACTIVE_HIGH);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, (0x00000015 | (au32TimingSetting[i * 2] & ~EBI_CTL_TALE_Msk)));
        CU_ASSERT_EQUAL(EBI->TCTL1, au32TimingSetting[(i * 2) + 1]);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
        EBI_Open(EBI_BANK1, EBI_BUSWIDTH_16BIT, au32TimingSel[i], EBI_OPMODE_ADSEPARATE, EBI_CS_ACTIVE_LOW);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, (0x0000000B | (au32TimingSetting[i*2]&~EBI_CTL_TALE_Msk)));
        CU_ASSERT_EQUAL(EBI->TCTL1, au32TimingSetting[(i*2)+1]);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
        EBI_Open(EBI_BANK1, EBI_BUSWIDTH_16BIT, au32TimingSel[i], EBI_OPMODE_NORMAL, EBI_CS_ACTIVE_HIGH);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, (0x00000007 | (au32TimingSetting[i * 2] & ~EBI_CTL_TALE_Msk)));
        CU_ASSERT_EQUAL(EBI->TCTL1, au32TimingSetting[(i * 2) + 1]);
        EBI_Close(EBI_BANK1);
        CU_ASSERT_EQUAL(EBI->CTL1, (0x00000006 | (au32TimingSetting[i * 2] & ~EBI_CTL_TALE_Msk)));
    }

    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
    EBI->CTL0 = 0x5;
    EBI->TCTL0 = 0xA0;
    EBI->CTL1 = 0x5;
    EBI->TCTL1 = 0xA0;

    for (i = 0; i < (sizeof(au32TimingSel) / sizeof(au32TimingSel[0])); i++)
    {
        D_msg("EBI2 TimingSel %d\n", i);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
        EBI_Open(EBI_BANK2, EBI_BUSWIDTH_8BIT, au32TimingSel[i], EBI_OPMODE_NORMAL, EBI_CS_ACTIVE_LOW);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, (0x00000001 | (au32TimingSetting[i * 2] & ~EBI_CTL_TALE_Msk)));
        CU_ASSERT_EQUAL(EBI->TCTL2, au32TimingSetting[(i * 2) + 1]);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
        EBI_Open(EBI_BANK2, EBI_BUSWIDTH_8BIT, au32TimingSel[i], EBI_OPMODE_CACCESS, EBI_CS_ACTIVE_HIGH);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, (0x00000015 | (au32TimingSetting[i * 2] & ~EBI_CTL_TALE_Msk)));
        CU_ASSERT_EQUAL(EBI->TCTL2, au32TimingSetting[(i * 2) + 1]);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
        EBI_Open(EBI_BANK2, EBI_BUSWIDTH_16BIT, au32TimingSel[i], EBI_OPMODE_ADSEPARATE, EBI_CS_ACTIVE_LOW);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, (0x0000000B | (au32TimingSetting[i*2]&~EBI_CTL_TALE_Msk)));
        CU_ASSERT_EQUAL(EBI->TCTL2, au32TimingSetting[(i*2)+1]);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
        EBI_Open(EBI_BANK2, EBI_BUSWIDTH_16BIT, au32TimingSel[i], EBI_OPMODE_NORMAL, EBI_CS_ACTIVE_HIGH);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, (0x00000007 | (au32TimingSetting[i * 2] & ~EBI_CTL_TALE_Msk)));
        CU_ASSERT_EQUAL(EBI->TCTL2, au32TimingSetting[(i * 2) + 1]);
        EBI_Close(EBI_BANK2);
        CU_ASSERT_EQUAL(EBI->CTL2, (0x00000006 | (au32TimingSetting[i * 2] & ~EBI_CTL_TALE_Msk)));
    }
}

void API_EBI_SetBusTiming(void)
{
    volatile uint32_t i;

    /* Init EBI clock */
    CU_ASSERT_EQUAL(EBI_InitClock(), 0);

    /* Check EBI_SetBusTiming API */
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
    EBI->CTL1 = 0x5;
    EBI->TCTL1 = 0xA0;
    EBI->CTL2 = 0x5;
    EBI->TCTL2 = 0xA0;

    for (i = 0; i < (sizeof(au32MCLKSel) / sizeof(au32MCLKSel[0])); i++)
    {
        D_msg("EBI0 MCLKSel %d\n", i);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
        EBI_SetBusTiming(EBI_BANK0, 0x55555555, au32MCLKSel[i]);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL0, (i << EBI_CTL_MCLKDIV_Pos));
        CU_ASSERT_EQUAL(EBI->TCTL0, 0x05405550);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
        EBI_SetBusTiming(EBI_BANK0, 0xAAAAAAAA, au32MCLKSel[i]);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL0, (i << EBI_CTL_MCLKDIV_Pos));
        CU_ASSERT_EQUAL(EBI->TCTL0, 0x0A80A2A8);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
        EBI_SetBusTiming(EBI_BANK0, 0xFFFFFFFF, au32MCLKSel[i]);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL0, (i << EBI_CTL_MCLKDIV_Pos));
        CU_ASSERT_EQUAL(EBI->TCTL0, 0x0FC0F7F8);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
        EBI_SetBusTiming(EBI_BANK0, 0x00000000, au32MCLKSel[i]);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL0, (i << EBI_CTL_MCLKDIV_Pos));
        CU_ASSERT_EQUAL(EBI->TCTL0, 0x00000000);
    }

    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
    EBI->CTL0 = 0x5;
    EBI->TCTL0 = 0xA0;
    EBI->CTL2 = 0x5;
    EBI->TCTL2 = 0xA0;

    for (i = 0; i < (sizeof(au32MCLKSel) / sizeof(au32MCLKSel[0])); i++)
    {
        D_msg("EBI1 MCLKSel %d\n", i);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
        EBI_SetBusTiming(EBI_BANK1, 0x55555555, au32MCLKSel[i]);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, (i << EBI_CTL_MCLKDIV_Pos));
        CU_ASSERT_EQUAL(EBI->TCTL1, 0x05405550);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
        EBI_SetBusTiming(EBI_BANK1, 0xAAAAAAAA, au32MCLKSel[i]);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, (i << EBI_CTL_MCLKDIV_Pos));
        CU_ASSERT_EQUAL(EBI->TCTL1, 0x0A80A2A8);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
        EBI_SetBusTiming(EBI_BANK1, 0xFFFFFFFF, au32MCLKSel[i]);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, (i << EBI_CTL_MCLKDIV_Pos));
        CU_ASSERT_EQUAL(EBI->TCTL1, 0x0FC0F7F8);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
        EBI_SetBusTiming(EBI_BANK1, 0x00000000, au32MCLKSel[i]);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL2, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, (i << EBI_CTL_MCLKDIV_Pos));
        CU_ASSERT_EQUAL(EBI->TCTL1, 0x00000000);
    }

    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK0), 0);
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK1), 0);
    CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
    EBI->CTL0 = 0x5;
    EBI->TCTL0 = 0xA0;
    EBI->CTL1 = 0x5;
    EBI->TCTL1 = 0xA0;

    for (i = 0; i < (sizeof(au32MCLKSel) / sizeof(au32MCLKSel[0])); i++)
    {
        D_msg("EBI2 MCLKSel %d\n", i);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
        EBI_SetBusTiming(EBI_BANK2, 0x55555555, au32MCLKSel[i]);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, (i << EBI_CTL_MCLKDIV_Pos));
        CU_ASSERT_EQUAL(EBI->TCTL2, 0x05405550);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
        EBI_SetBusTiming(EBI_BANK2, 0xAAAAAAAA, au32MCLKSel[i]);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, (i << EBI_CTL_MCLKDIV_Pos));
        CU_ASSERT_EQUAL(EBI->TCTL2, 0x0A80A2A8);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
        EBI_SetBusTiming(EBI_BANK2, 0xFFFFFFFF, au32MCLKSel[i]);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, (i << EBI_CTL_MCLKDIV_Pos));
        CU_ASSERT_EQUAL(EBI->TCTL2, 0x0FC0F7F8);

        CU_ASSERT_EQUAL(ResetEBIRegister(EBI_BANK2), 0);
        EBI_SetBusTiming(EBI_BANK2, 0x00000000, au32MCLKSel[i]);
        CU_ASSERT_EQUAL(EBI->CTL0, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL0, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL1, 0x5);
        CU_ASSERT_EQUAL(EBI->TCTL1, 0xA0);
        CU_ASSERT_EQUAL(EBI->CTL2, (i << EBI_CTL_MCLKDIV_Pos));
        CU_ASSERT_EQUAL(EBI->TCTL2, 0x00000000);
    }
}

CU_TestInfo EBI_MACRO[] =
{
    {"Check EBI CONSTANT ", CONSTANT_EBI},
    CU_TEST_INFO_NULL
};

CU_TestInfo EBI_API[] =
{
    {"Check EBI_Open and EBI_Close API ", API_EBI_Open},
    {"Check EBI_SetBusTiming API ",       API_EBI_SetBusTiming},
    CU_TEST_INFO_NULL
};
