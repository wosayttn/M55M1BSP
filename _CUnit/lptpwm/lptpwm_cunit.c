/******************************************************************************
* @file    lptpwm_cunit.c
* @version V1.00
* @brief   CUnit test lptpwm
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
#include "lptpwm_cunit.h"

int lptpwm_Test_Init(void)
{
    return 0;
}
int lptpwm_Test_Clean(void)
{
    return 0;
}

LPTMR_T *LPTMRCh[] =
{
    LPTMR0,
    LPTMR1
};

int32_t LPTMR_PWM_Init(void)
{
    volatile uint32_t i;

    CLK_EnableModuleClock(LPTMR0_MODULE);
    CLK_EnableModuleClock(LPTMR1_MODULE);

    CLK->LPTMRSEL = (CLK->LPTMRSEL & ~(CLK_LPTMRSEL_LPTMR0SEL_Msk | CLK_LPTMRSEL_LPTMR1SEL_Msk)) |
                    (CLK_LPTMRSEL_LPTMR0SEL_HIRC | CLK_LPTMRSEL_LPTMR1SEL_HIRC);

    /* Check LPTPWM_ENABLE_PWM_MODE */
    LPTPWM_ENABLE_PWM_MODE(LPTMR0);
    CU_ASSERT_EQUAL(CLK->LPTMRSEL & CLK_LPTMRSEL_LPTMR0SEL_Msk, (0x4 << CLK_LPTMRSEL_LPTMR0SEL_Pos));
    CU_ASSERT_EQUAL(LPTMR0->CTL, BIT15);
    LPTPWM_ENABLE_PWM_MODE(LPTMR1);
    CU_ASSERT_EQUAL(CLK->LPTMRSEL & CLK_LPTMRSEL_LPTMR1SEL_Msk, (0x4 << CLK_LPTMRSEL_LPTMR1SEL_Pos));
    CU_ASSERT_EQUAL(LPTMR1->CTL, BIT15);

    /* Check LPTPWM_DISABLE_PWM_MODE */
    LPTPWM_DISABLE_PWM_MODE(LPTMR0);
    CU_ASSERT_EQUAL(LPTMR0->CTL & LPTMR_CTL_FUNCSEL_Msk, 0x0);
    LPTPWM_DISABLE_PWM_MODE(LPTMR1);
    CU_ASSERT_EQUAL(LPTMR1->CTL & LPTMR_CTL_FUNCSEL_Msk, 0x0);

    /* Change to LPTMR PWM mode */
    LPTPWM_ENABLE_PWM_MODE(LPTMR0);
    LPTPWM_ENABLE_PWM_MODE(LPTMR1);

    return 0;
}

void CONST_Output_Mode(void)
{
    CU_ASSERT_EQUAL(LPTPWM_CH0, BIT0);
    CU_ASSERT_EQUAL(LPTPWM_TOUT_PIN_FROM_TX, (0UL << LPTMR_PWMPOCTL_POSEL_Pos));
    CU_ASSERT_EQUAL(LPTPWM_TOUT_PIN_FROM_TX_EXT, (BIT8));
    CU_ASSERT_EQUAL(LPTPWM_AUTO_RELOAD_MODE, (0UL));
    CU_ASSERT_EQUAL(LPTPWM_ONE_SHOT_MODE, (1UL));
}

void CONST_Trigger_Mode(void)
{
    CU_ASSERT_EQUAL(LPTPWM_TRGEN, LPTMR_PWMTRGCTL_TRGEN_Msk);
    CU_ASSERT_EQUAL(LPTPWM_TRG_TO_LPPDMA, (LPTMR_PWMTRGCTL_TRGEN_Msk | LPTMR_PWMTRGCTL_PWMTRGLPPDMA_Msk));
    CU_ASSERT_EQUAL(LPTPWM_TRIGGER_AT_PERIOD_POINT, (0UL));
    CU_ASSERT_EQUAL(LPTPWM_TRIGGER_AT_COMPARE_POINT, (1UL));
    CU_ASSERT_EQUAL(LPTPWM_TRIGGER_AT_PERIOD_OR_COMPARE_POINT, (2UL));
}

void Check_Basic_MACRO(void)
{
    volatile uint32_t i;

    LPTMR_PWM_Init();

    /* Check LPTPWM_SET_PRESCALER, LPTPWM_GET_PRESCALER */
    for (i = 0; i < 2; i++)
    {
        printf("LPTPWM_SET_PRESCALER (Ch-%d)\n", i);
        LPTPWM_SET_PRESCALER((LPTMR_T *)LPTMRCh[i], 0x5A);
        CU_ASSERT_EQUAL(LPTPWM_GET_PRESCALER((LPTMR_T *)LPTMRCh[i]), 0x5A);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMCLKPSC, 0x5A);
        LPTPWM_SET_PRESCALER((LPTMR_T *)LPTMRCh[i], 0xA5);
        CU_ASSERT_EQUAL(LPTPWM_GET_PRESCALER((LPTMR_T *)LPTMRCh[i]), 0xA5);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMCLKPSC, 0xA5);
        LPTPWM_SET_PRESCALER((LPTMR_T *)LPTMRCh[i], 0x0);
        CU_ASSERT_EQUAL(LPTPWM_GET_PRESCALER((LPTMR_T *)LPTMRCh[i]), 0x0);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMCLKPSC, 0x0);
    }

    /* Check LPTPWM_SET_PERIOD, LPTPWM_GET_PERIOD */
    for (i = 0; i < 2; i++)
    {
        printf("LPTPWM_SET_PERIOD (Ch-%d)\n", i);
        LPTPWM_SET_PERIOD((LPTMR_T *)LPTMRCh[i], (1200 - 1));
        CU_ASSERT_EQUAL(LPTPWM_GET_PERIOD((LPTMR_T *)LPTMRCh[i]), (1200 - 1));
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMPERIOD, (1200 - 1));
    }

    /* Check LPTPWM_SET_CMPDAT, LPTPWM_GET_CMPDAT */
    for (i = 0; i < 2; i++)
    {
        printf("LPTPWM_SET_CMPDAT (Ch-%d)\n", i);
        LPTPWM_SET_CMPDAT((LPTMR_T *)LPTMRCh[i], 600);
        CU_ASSERT_EQUAL(LPTPWM_GET_CMPDAT((LPTMR_T *)LPTMRCh[i]), 600);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMCMPDAT, 600);
    }

    /* Check LPTPWM_ENABLE_OUTPUT */
    for (i = 0; i < 2; i++)
    {
        printf("LPTPWM_ENABLE_OUTPUT (Ch-%d)\n", i);
        LPTPWM_ENABLE_OUTPUT((LPTMR_T *)LPTMRCh[i], LPTPWM_CH0);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMPOCTL & LPTMR_PWMPOCTL_POEN_Msk, LPTMR_PWMPOCTL_POEN_Msk);
        LPTPWM_ENABLE_OUTPUT((LPTMR_T *)LPTMRCh[i], 0);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMPOCTL & LPTMR_PWMPOCTL_POEN_Msk, 0);
    }

    /* Check LPTPWM_OUTPUT_SELECT */
    for (i = 0; i < 2; i++)
    {
        printf("LPTPWM_OUTPUT_SELECT (Ch-%d)\n", i);
        LPTPWM_SELECT_TOUT_PIN((LPTMR_T *)LPTMRCh[i], LPTPWM_TOUT_PIN_FROM_TX);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMPOCTL & LPTMR_PWMPOCTL_POSEL_Msk, 0);
        LPTPWM_SELECT_TOUT_PIN((LPTMR_T *)LPTMRCh[i], LPTPWM_TOUT_PIN_FROM_TX_EXT);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMPOCTL & LPTMR_PWMPOCTL_POSEL_Msk, LPTMR_PWMPOCTL_POSEL_Msk);
    }

    /* Check LPTPWM_SET_OUTPUT_INVERSE */
    for (i = 0; i < 2; i++)
    {
        printf("LPTPWM_SET_OUTPUT_INVERSE (Ch-%d)\n", i);
        LPTPWM_SET_OUTPUT_INVERSE((LPTMR_T *)LPTMRCh[i], LPTPWM_CH0);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMPOLCTL, 0x1);
        LPTPWM_SET_OUTPUT_INVERSE((LPTMR_T *)LPTMRCh[i], 0x0);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMPOLCTL, 0x0);
    }

    /* Check LPTPWM_ENABLE_PERIOD_INT, LPTPWM_DISABLE_PERIOD_INT */
    /* Check LPTPWM_ENABLE_CMP_UP_INT, LPTPWM_DISABLE_CMP_UP_INT */
    for (i = 0; i < 2; i++)
    {
        printf("LPTPWM_ENABLE_PERIOD_INT (Ch-%d)\n", i);
        LPTPWM_ENABLE_PERIOD_INT((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMINTEN0, 0x2);
        LPTPWM_DISABLE_PERIOD_INT((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMINTEN0, 0x0);

        printf("LPTPWM_ENABLE_CMP_UP_INT (Ch-%d)\n", i);
        LPTPWM_ENABLE_CMP_UP_INT((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMINTEN0, 0x4);
        LPTPWM_DISABLE_CMP_UP_INT((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMINTEN0, 0x0);
    }
}

void Check_Advance_MACRO(void)
{
    volatile uint32_t i;

    LPTMR_PWM_Init();

    CLK->LPTMRSEL = (CLK->LPTMRSEL & ~(CLK_LPTMRSEL_LPTMR0SEL_Msk | CLK_LPTMRSEL_LPTMR1SEL_Msk)) |
                    (CLK_LPTMRSEL_LPTMR0SEL_PCLK4 | CLK_LPTMRSEL_LPTMR1SEL_PCLK4);

    /* Initial LPTMR PWM */
    for (i = 0; i < 2; i++)
    {
        printf("LPTPWM_SET_CMPDAT (Ch-%d)\n", i);
        LPTPWM_SET_PERIOD((LPTMR_T *)LPTMRCh[i], 0xFFFF);
        LPTPWM_SET_CMPDAT((LPTMR_T *)LPTMRCh[i], 0x1000);
        LPTPWM_ENABLE_IFA((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMIFA, LPTMR_PWMIFA_IFAEN_Msk);
        LPTPWM_IFASEL_CMPUIF((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMIFA, (2 << LPTMR_PWMIFA_IFASEL_Pos));
    }

    /* Check LPTPWM_START_COUNTER */
    for (i = 0; i < 2; i++)
    {
        LPTPWM_START_COUNTER((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMCTL & BIT0, BIT0);
    }

    __NOP();

    for (i = 0; i < 2; i++)
    {
        CU_ASSERT(LPTMRCh[i]->PWMCNT);
    }

    /* Check LPTPWM_GET_REACH_MAX_CNT_STATUS */
    while (LPTPWM_GET_REACH_MAX_CNT_STATUS(LPTMR1) == 0) {};

    CU_ASSERT_EQUAL(LPTPWM_GET_REACH_MAX_CNT_STATUS(LPTMR0), 0x1);

    CU_ASSERT_EQUAL(LPTPWM_GET_REACH_MAX_CNT_STATUS(LPTMR1), 0x1);

    for (i = 0; i < 2; i++)
        CU_ASSERT(LPTMRCh[i]->PWMCNT < 0xFFFF);

    /* Check LPTPWM_CLEAR_COUNTER */
    for (i = 0; i < 2; i++)
    {
        while (1)
        {
            if ((LPTMRCh[i]->PWMCNT > 0x12000) && (LPTMRCh[i]->PWMCNT < 0x13000))
            {
                break;
            }
        }

        LPTPWM_CLEAR_COUNTER((LPTMR_T *)LPTMRCh[i]);
        __NOP();
        __NOP();
        CU_ASSERT((LPTMRCh[i]->PWMCNT < 0x10100));
    }

    /* Check LPTPWM_STOP_COUNTER */
    for (i = 0; i < 2; i++)
    {
        LPTPWM_STOP_COUNTER((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMPERIOD, 0x0);

        LPTMRCh[i]->PWMCTL &= ~LPTMR_PWMCTL_CNTEN_Msk; // Force disable PWM CNTEN bit
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMCTL & LPTMR_PWMCTL_CNTEN_Msk), 0x0);
    }

    /* Check LPTPWM_CLEAR_REACH_MAX_CNT_STATUS */
    for (i = 0; i < 2; i++)
    {
        LPTPWM_CLEAR_REACH_MAX_CNT_STATUS((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTPWM_GET_REACH_MAX_CNT_STATUS((LPTMR_T *)LPTMRCh[i]), 0x0);
    }

    /* Check LPTPWM_GET_PERIOD_INT_FLAG, LPTPWM_CLEAR_PERIOD_INT_FLAG */
    for (i = 0; i < 2; i++)
    {
        CU_ASSERT_EQUAL(LPTPWM_GET_PERIOD_INT_FLAG((LPTMR_T *)LPTMRCh[i]), 0x1);
        LPTPWM_CLEAR_PERIOD_INT_FLAG((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTPWM_GET_PERIOD_INT_FLAG((LPTMR_T *)LPTMRCh[i]), 0x0);
    }

    /* Check LPTPWM_GET_CMP_INT_FLAG, LPTPWM_CLEAR_CMP_INT_FLAG */
    for (i = 0; i < 2; i++)
    {
        CU_ASSERT_EQUAL(LPTPWM_GET_CMP_UP_INT_FLAG((LPTMR_T *)LPTMRCh[i]), 0x1);
        LPTPWM_CLEAR_CMP_UP_INT_FLAG((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTPWM_GET_CMP_UP_INT_FLAG((LPTMR_T *)LPTMRCh[i]), 0x0);
    }

    /* Check LPTPWM_GET_IFA_INT_FLAG, LPTPWM_CLR_IFA_INT_FLAG */
    for (i = 0; i < 2; i++)
    {
        CU_ASSERT_EQUAL(LPTPWM_GET_IFA_INT_FLAG((LPTMR_T *)LPTMRCh[i]), 0x1);
        LPTPWM_CLR_IFA_INT_FLAG((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTPWM_GET_IFA_INT_FLAG((LPTMR_T *)LPTMRCh[i]), 0x0);
    }

    /* Reset LPTMR PWM function */
    LPTPWM_DISABLE_PWM_MODE(LPTMR0);
    LPTPWM_DISABLE_PWM_MODE(LPTMR1);

    SYS_UnlockReg();
}

void Check_IFA_MACRO(void)
{
    volatile uint32_t i;

    LPTMR_PWM_Init();

    /* Check LPTPWM_IFA */
    for (i = 0; i < 2; i++)
    {
        printf("LPTPWM_ENABLE_IFA (Ch-%d)\n", i);
        LPTPWM_ENABLE_IFA((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMIFA, BIT31);
        LPTPWM_DISABLE_IFA((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMIFA, 0UL);
        LPTPWM_ENABLE_IFA_STOPCNT((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMIFA, BIT24);
        LPTPWM_DISABLE_IFA_STOPCNT((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMIFA, 0UL);
        LPTPWM_SET_IFACNT((LPTMR_T *)LPTMRCh[i], 0x5A5A);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMIFA, 0x5A5A);
        LPTPWM_SET_IFACNT((LPTMR_T *)LPTMRCh[i], 0xA5A5);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMIFA, 0xA5A5);
        LPTPWM_SET_IFACNT((LPTMR_T *)LPTMRCh[i], 0);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMIFA, 0);
        LPTPWM_IFASEL_PIF((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMIFA, (1 << LPTMR_PWMIFA_IFASEL_Pos));
        LPTPWM_IFASEL_CMPUIF((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMIFA, (2 << LPTMR_PWMIFA_IFASEL_Pos));
        LPTPWM_ENABLE_IFA_TRGLPPDMA((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMAPDMACTL, LPTMR_PWMAPDMACTL_APDMAEN_Msk);
        LPTPWM_DISABLE_IFA_TRGLPPDMA((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMAPDMACTL, 0UL);
        LPTPWM_ENABLE_IFA_INT((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMAINTEN, LPTMR_PWMAINTEN_IFAIEN_Msk);
        LPTPWM_DISABLE_IFA_INT((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->PWMAINTEN, 0UL);
    }
}

void API_LPTPWM_ConfigOutputFreqAndDuty(void)
{
    volatile uint32_t i;
    uint32_t u32TragerFreq, u32PERIOD, u32CMP;

    LPTMR_PWM_Init();

    /* Check LPTPWM_EnablePDCLK, LPTPWM_DisablePDCLK*/
    for (i = 0; i < 2; i++)
    {
        LPTPWM_EnablePDCLK(LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, LPTMR_CTL_PDCLKEN_Msk);

        LPTPWM_DisablePDCLK(LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, 0);
    }

    /* Check API_LPTPWM_ConfigOutputFreqAndDuty, LPTPWM_AUTO_RELOAD_MODE */
    u32TragerFreq = LPTPWM_ConfigOutputFreqAndDuty(LPTMR0, 12000, 50);
    CU_ASSERT_EQUAL(u32TragerFreq, 12000);
    u32PERIOD = LPTPWM_GET_PERIOD(LPTMR0);
    u32CMP = LPTPWM_GET_CMPDAT(LPTMR0);
    CU_ASSERT_EQUAL((u32CMP * 2), (u32PERIOD + 1));
    CU_ASSERT_EQUAL((LPTMR0->PWMCTL & BIT3), LPTPWM_AUTO_RELOAD_MODE);
    printf("\n%d, %d ", u32CMP, u32PERIOD);

    u32TragerFreq = LPTPWM_ConfigOutputFreqAndDuty(LPTMR1, 1000, 20);
    CU_ASSERT_EQUAL(u32TragerFreq, 1000);
    u32PERIOD = LPTPWM_GET_PERIOD(LPTMR1);
    u32CMP = LPTPWM_GET_CMPDAT(LPTMR1);
    CU_ASSERT_EQUAL((u32CMP * 5), (u32PERIOD + 1));
    CU_ASSERT_EQUAL((LPTMR1->PWMCTL & BIT3), LPTPWM_AUTO_RELOAD_MODE);
    printf("\n%d, %d ", u32CMP, u32PERIOD);

    /* Check LPTPWM_EnableCounter */
    for (i = 0; i < 2; i++)
    {
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMCTL & BIT0), 0x0);
    }

    for (i = 0; i < 2; i++)
    {
        LPTPWM_EnableCounter((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMCTL & BIT0), 0x1);
    }

    /* Check LPTPWM_DisableCounter */
    for (i = 0; i < 2; i++)
    {
        LPTPWM_DisableCounter((LPTMR_T *)LPTMRCh[i]);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMCTL & BIT0), 0x0);
    }

    /* Check LPTPWM_ONE_SHOT_MODE */
    for (i = 0; i < 2; i++)
    {
        LPTPWM_SET_CNT_MODE(LPTMRCh[i], LPTPWM_ONE_SHOT_MODE);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMCTL & BIT3), BIT3);
    }
}

void API_LPTPWM_Trigger(void)
{
    volatile uint32_t i;

    LPTMR_PWM_Init();

    /* Check LPTPWM_EnableTrigger, LPTPWM_DisableTrigger */
    for (i = 0; i < 4; i++)
    {
        printf("LPTPWM_EnableTrigger (Ch-%d)\n", i);
        LPTPWM_EnableTrigger((LPTMR_T *)LPTMRCh[i], LPTPWM_TRGEN, LPTPWM_TRIGGER_AT_PERIOD_POINT);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & BIT2), BIT2);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & 0x3), 0x0);
        LPTPWM_DisableTrigger((LPTMR_T *)LPTMRCh[i], LPTPWM_TRGEN);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & BIT2), 0x0);
        LPTPWM_EnableTrigger((LPTMR_T *)LPTMRCh[i], LPTPWM_TRGEN, LPTPWM_TRIGGER_AT_COMPARE_POINT);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & BIT2), BIT2);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & 0x3), 0x1);
        LPTPWM_DisableTrigger((LPTMR_T *)LPTMRCh[i], LPTPWM_TRGEN);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & BIT2), 0x0);
        LPTPWM_EnableTrigger((LPTMR_T *)LPTMRCh[i], LPTPWM_TRGEN, LPTPWM_TRIGGER_AT_PERIOD_OR_COMPARE_POINT);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & BIT2), BIT2);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & 0x3), 0x2);
        LPTPWM_DisableTrigger((LPTMR_T *)LPTMRCh[i], LPTPWM_TRGEN);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & BIT2), 0x0);

        LPTPWM_EnableTrigger((LPTMR_T *)LPTMRCh[i], LPTPWM_TRG_TO_LPPDMA, LPTPWM_TRIGGER_AT_PERIOD_POINT);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & (BIT2 | BIT9)), (BIT2 | BIT9));
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & 0x3), 0x0);
        LPTPWM_DisableTrigger((LPTMR_T *)LPTMRCh[i], LPTPWM_TRG_TO_LPPDMA);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & (BIT2 | BIT9)), 0x0);
        LPTPWM_EnableTrigger((LPTMR_T *)LPTMRCh[i], LPTPWM_TRG_TO_LPPDMA, LPTPWM_TRIGGER_AT_COMPARE_POINT);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & (BIT2 | BIT9)), (BIT2 | BIT9));
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & 0x3), 0x1);
        LPTPWM_DisableTrigger((LPTMR_T *)LPTMRCh[i], LPTPWM_TRG_TO_LPPDMA);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & (BIT2 | BIT9)), 0x0);
        LPTPWM_EnableTrigger((LPTMR_T *)LPTMRCh[i], LPTPWM_TRG_TO_LPPDMA, LPTPWM_TRIGGER_AT_PERIOD_OR_COMPARE_POINT);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & (BIT2 | BIT9)), (BIT2 | BIT9));
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & 0x3), 0x2);
        LPTPWM_DisableTrigger((LPTMR_T *)LPTMRCh[i], LPTPWM_TRG_TO_LPPDMA);
        CU_ASSERT_EQUAL((LPTMRCh[i]->PWMTRGCTL & (BIT2 | BIT9)), 0x0);
    }
}

CU_SuiteInfo lptpwmSuites[] =
{
    { "lptpwm Const Test", lptpwm_Test_Init, lptpwm_Test_Clean, NULL, NULL, lptpwm_ConstTest },
    { "lptpwm Macro Test", lptpwm_Test_Init, lptpwm_Test_Clean, NULL, NULL, lptpwm_MacroTest },
    { "lptpwm Func  Test", lptpwm_Test_Init, lptpwm_Test_Clean, NULL, NULL, lptpwm_FuncTest },
    CU_SUITE_INFO_NULL
};

CU_TestInfo  lptpwm_ConstTest[] =
{
    { "Const Output_Mod",  CONST_Output_Mode },
    { "Const Trigger_Mode",  CONST_Trigger_Mode },
    CU_TEST_INFO_NULL
};

CU_TestInfo  lptpwm_MacroTest[] =
{
    {"Check LPTMR PWM Basic MACRO ",    Check_Basic_MACRO},
    {"Check LPTMR PWM Advance MACRO ",  Check_Advance_MACRO},
    CU_TEST_INFO_NULL
};

CU_TestInfo  lptpwm_FuncTest[] =
{
    {"Check Configure Freq. and Duty API ",       API_LPTPWM_ConfigOutputFreqAndDuty},
    {"Check Trigger LPIPs APIs ",                 API_LPTPWM_Trigger},
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/