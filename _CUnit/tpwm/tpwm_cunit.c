/******************************************************************************
* @file    tpwm_cunit.c
* @version V1.00
* @brief   CUnit test tpwm
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
#include "tpwm_cunit.h"

int Tpwm_Test_Init(void)
{
    return 0;
}
int Tpwm_Test_Clean(void)
{
    return 0;
}

TIMER_T *TimerCh[] =
{
    TIMER0,
    TIMER1,
    TIMER2,
    TIMER3
};

int32_t TIMER_PWM_Init(void)
{
    volatile uint32_t i;

    SYS_ResetModule(SYS_TMR0RST);
    SYS_ResetModule(SYS_TMR1RST);
    SYS_ResetModule(SYS_TMR2RST);
    SYS_ResetModule(SYS_TMR3RST);
    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC, NULL);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_HIRC, NULL);
    CLK_SetModuleClock(TMR2_MODULE, CLK_TMRSEL_TMR2SEL_HIRC, NULL);
    CLK_SetModuleClock(TMR3_MODULE, CLK_TMRSEL_TMR3SEL_HIRC, NULL);
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_EnableModuleClock(TMR3_MODULE);

    /* Check TPWM_ENABLE_PWM_MODE auto switch to PCLK*/
    TPWM_ENABLE_PWM_MODE(TIMER0);
    CU_ASSERT_EQUAL(CLK->TMRSEL & CLK_TMRSEL_TMR0SEL_Msk, (0x2 << CLK_TMRSEL_TMR0SEL_Pos));
    CU_ASSERT_EQUAL(TIMER0->ALTCTL, 0x1);
    TPWM_ENABLE_PWM_MODE(TIMER1);
    CU_ASSERT_EQUAL(CLK->TMRSEL & CLK_TMRSEL_TMR1SEL_Msk, (0x2 << CLK_TMRSEL_TMR1SEL_Pos));
    CU_ASSERT_EQUAL(TIMER1->ALTCTL, 0x1);
    TPWM_ENABLE_PWM_MODE(TIMER2);
    CU_ASSERT_EQUAL(CLK->TMRSEL & CLK_TMRSEL_TMR2SEL_Msk, (0x2 << CLK_TMRSEL_TMR2SEL_Pos));
    CU_ASSERT_EQUAL(TIMER2->ALTCTL, 0x1);
    TPWM_ENABLE_PWM_MODE(TIMER3);
    CU_ASSERT_EQUAL(CLK->TMRSEL & CLK_TMRSEL_TMR3SEL_Msk, (0x2 << CLK_TMRSEL_TMR3SEL_Pos));
    CU_ASSERT_EQUAL(TIMER3->ALTCTL, 0x1);

    /* Check TPWM_DISABLE_PWM_MODE */
    TPWM_DISABLE_PWM_MODE(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->ALTCTL, 0x0);
    TPWM_DISABLE_PWM_MODE(TIMER1);
    CU_ASSERT_EQUAL(TIMER1->ALTCTL, 0x0);
    TPWM_DISABLE_PWM_MODE(TIMER2);
    CU_ASSERT_EQUAL(TIMER2->ALTCTL, 0x0);
    TPWM_DISABLE_PWM_MODE(TIMER3);
    CU_ASSERT_EQUAL(TIMER3->ALTCTL, 0x0);

    /* Change to Timer PWM mode */
    TPWM_ENABLE_PWM_MODE(TIMER0);
    TPWM_ENABLE_PWM_MODE(TIMER1);
    TPWM_ENABLE_PWM_MODE(TIMER2);
    TPWM_ENABLE_PWM_MODE(TIMER3);

    return 0;
}

void Check_Basic_MACRO(void)
{
    volatile uint32_t i;

    TIMER_PWM_Init();

    /* Check TPWM_ENABLE_COMPLEMENTARY_MODE */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_ENABLE_COMPLEMENTARY_MODE (Ch-%d)\n", i);
        TPWM_ENABLE_COMPLEMENTARY_MODE((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCTL, BIT16);
    }

    /* Check TPWM_ENABLE_INDEPENDENT_MODE */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_ENABLE_INDEPENDENT_MODE (Ch-%d)\n", i);
        TPWM_ENABLE_INDEPENDENT_MODE((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCTL, 0x0);
    }

    /* Check TPWM_SET_COUNTER_TYPE */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_SET_COUNTER_TYPE (Ch-%d)\n", i);
        TPWM_SET_COUNTER_TYPE((TIMER_T *)TimerCh[i], TPWM_UP_DOWN_COUNT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCTL, 0x4);
        TPWM_SET_COUNTER_TYPE((TIMER_T *)TimerCh[i], TPWM_DOWN_COUNT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCTL, 0x2);
        TPWM_SET_COUNTER_TYPE((TIMER_T *)TimerCh[i], TPWM_UP_COUNT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCTL, 0x0);
    }

    /* Check TPWM_SET_PRESCALER, TPWM_GET_PRESCALER */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_SET_PRESCALER (Ch-%d)\n", i);
        TPWM_SET_PRESCALER((TIMER_T *)TimerCh[i], 0x155);
        CU_ASSERT_EQUAL(TPWM_GET_PRESCALER((TIMER_T *)TimerCh[i]), 0x155);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCLKPSC, 0x155);
        TPWM_SET_PRESCALER((TIMER_T *)TimerCh[i], 0x0);
        CU_ASSERT_EQUAL(TPWM_GET_PRESCALER((TIMER_T *)TimerCh[i]), 0x0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCLKPSC, 0x0);
    }

    /* Check TPWM_SET_PERIOD, TPWM_GET_PERIOD */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_SET_PERIOD (Ch-%d)\n", i);
        TPWM_SET_PERIOD((TIMER_T *)TimerCh[i], (1200 - 1));
        CU_ASSERT_EQUAL(TPWM_GET_PERIOD((TIMER_T *)TimerCh[i]), (1200 - 1));
        CU_ASSERT_EQUAL(TimerCh[i]->PWMPERIOD, (1200 - 1));
    }

    /* Check TPWM_SET_CMPDAT, TPWM_GET_CMPDAT */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_SET_CMPDAT (Ch-%d)\n", i);
        TPWM_SET_CMPDAT((TIMER_T *)TimerCh[i], 600);
        CU_ASSERT_EQUAL(TPWM_GET_CMPDAT((TIMER_T *)TimerCh[i]), 600);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCMPDAT, 600);
    }

    /* Check TPWM_ENABLE_OUTPUT */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_ENABLE_OUTPUT (Ch-%d)\n", i);
        TPWM_ENABLE_OUTPUT((TIMER_T *)TimerCh[i], TPWM_CH0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMPOEN, 0x1);
        TPWM_ENABLE_OUTPUT((TIMER_T *)TimerCh[i], TPWM_CH1);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMPOEN, 0x2);
        TPWM_ENABLE_OUTPUT((TIMER_T *)TimerCh[i], (TPWM_CH1 | TPWM_CH0));
        CU_ASSERT_EQUAL(TimerCh[i]->PWMPOEN, 0x3);
    }

    /* Check TPWM_SET_OUTPUT_INVERSE */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_SET_OUTPUT_INVERSE (Ch-%d)\n", i);
        TPWM_SET_OUTPUT_INVERSE((TIMER_T *)TimerCh[i], TPWM_CH0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMPOLCTL, 0x1);
        TPWM_SET_OUTPUT_INVERSE((TIMER_T *)TimerCh[i], TPWM_CH1);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMPOLCTL, 0x2);
        TPWM_SET_OUTPUT_INVERSE((TIMER_T *)TimerCh[i], (TPWM_CH1 | TPWM_CH0));
        CU_ASSERT_EQUAL(TimerCh[i]->PWMPOLCTL, 0x3);
        TPWM_SET_OUTPUT_INVERSE((TIMER_T *)TimerCh[i], 0x0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMPOLCTL, 0x0);
    }

    /* Check TPWM_SET_MASK_OUTPUT */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_SET_MASK_OUTPUT (Ch-%d)\n", i);
        TPWM_SET_MASK_OUTPUT((TIMER_T *)TimerCh[i], TPWM_CH0, 0x0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMMSKEN, 0x1);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMMSK, 0x0);
        TPWM_SET_MASK_OUTPUT((TIMER_T *)TimerCh[i], TPWM_CH0, 0x1);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMMSKEN, 0x1);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMMSK, 0x1);
        TPWM_SET_MASK_OUTPUT((TIMER_T *)TimerCh[i], TPWM_CH1, 0x0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMMSKEN, 0x2);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMMSK, 0x0);
        TPWM_SET_MASK_OUTPUT((TIMER_T *)TimerCh[i], TPWM_CH1, 0x2);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMMSKEN, 0x2);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMMSK, 0x2);
        TPWM_SET_MASK_OUTPUT((TIMER_T *)TimerCh[i], (TPWM_CH1 | TPWM_CH0), 0x0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMMSKEN, 0x3);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMMSK, 0x0);
        TPWM_SET_MASK_OUTPUT((TIMER_T *)TimerCh[i], (TPWM_CH1 | TPWM_CH0), 0x3);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMMSKEN, 0x3);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMMSK, 0x3);
    }

    /* Check TPWM_ENABLE_ZERO_INT, TPWM_DISABLE_ZERO_INT */
    /* Check TPWM_ENABLE_PERIOD_INT, TPWM_DISABLE_PERIOD_INT */
    /* Check TPWM_ENABLE_CMP_UP_INT, TPWM_DISABLE_CMP_UP_INT */
    /* Check TPWM_ENABLE_CMP_DOWN_INT, TPWM_DISABLE_CMP_DOWN_INT */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_ENABLE_ZERO_INT (Ch-%d)\n", i);
        TPWM_ENABLE_ZERO_INT((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN0, 0x1);
        TPWM_DISABLE_ZERO_INT((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN0, 0x0);

        printf("TPWM_ENABLE_PERIOD_INT (Ch-%d)\n", i);
        TPWM_ENABLE_PERIOD_INT((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN0, 0x2);
        TPWM_DISABLE_PERIOD_INT((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN0, 0x0);

        printf("TPWM_ENABLE_CMP_UP_INT (Ch-%d)\n", i);
        TPWM_ENABLE_CMP_UP_INT((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN0, 0x4);
        TPWM_DISABLE_CMP_UP_INT((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN0, 0x0);

        printf("TPWM_ENABLE_CMP_DOWN_INT (Ch-%d)\n", i);
        TPWM_ENABLE_CMP_DOWN_INT((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN0, 0x8);
        TPWM_DISABLE_CMP_DOWN_INT((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN0, 0x0);
    }

    /* Check TPWM_SET_BRAKE_PIN_LOW_DETECT, TPWM_SET_BRAKE_PIN_HIGH_DETECT, TPWM_SET_BRAKE_PIN_SOURCE */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_SET_BRAKE_PIN_LOW_DETECT (Ch-%d)\n", i);
        TPWM_SET_BRAKE_PIN_LOW_DETECT((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMBNF, BIT7);
        TPWM_SET_BRAKE_PIN_HIGH_DETECT((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMBNF, 0x0);

        printf("TPWM_SET_BRAKE_PIN_SOURCE (Ch-%d)\n", i);
        TPWM_SET_BRAKE_PIN_SOURCE((TIMER_T *)TimerCh[i], TPWM_TM_BRAKE0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMBNF, 0x0);
        TPWM_SET_BRAKE_PIN_SOURCE((TIMER_T *)TimerCh[i], TPWM_TM_BRAKE1);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMBNF, 0x10000);
        TPWM_SET_BRAKE_PIN_SOURCE((TIMER_T *)TimerCh[i], TPWM_TM_BRAKE2);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMBNF, 0x20000);
        TPWM_SET_BRAKE_PIN_SOURCE((TIMER_T *)TimerCh[i], TPWM_TM_BRAKE3);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMBNF, 0x30000);
    }

    /* Check TPWM_SET_COUNTER_SYNC_MODE */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_SET_COUNTER_SYNC_MODE (Ch-%d)\n", i);
        TPWM_SET_COUNTER_SYNC_MODE((TIMER_T *)TimerCh[i], TPWM_CNTR_SYNC_START_BY_TIMER0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMSCTL, 0x1);
        TPWM_SET_COUNTER_SYNC_MODE((TIMER_T *)TimerCh[i], TPWM_CNTR_SYNC_CLEAR_BY_TIMER0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMSCTL, 0x3);
        TPWM_SET_COUNTER_SYNC_MODE((TIMER_T *)TimerCh[i], TPWM_CNTR_SYNC_DISABLE);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMSCTL, 0x0);
    }

    for (i = 2; i < 3; i++)
    {
        printf("TPWM_SET_COUNTER_SYNC_MODE (Ch-%d)\n", i);
        TPWM_SET_COUNTER_SYNC_MODE((TIMER_T *)TimerCh[i], TPWM_CNTR_SYNC_START_BY_TIMER2);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMSCTL, 0x101);
        TPWM_SET_COUNTER_SYNC_MODE((TIMER_T *)TimerCh[i], TPWM_CNTR_SYNC_CLEAR_BY_TIMER2);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMSCTL, 0x103);
        TPWM_SET_COUNTER_SYNC_MODE((TIMER_T *)TimerCh[i], TPWM_CNTR_SYNC_DISABLE);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMSCTL, 0x0);
    }
}

void Check_Advance_MACRO(void)
{
    volatile uint32_t i;

    TIMER_PWM_Init();

    /* Initial Timer PWM */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_SET_CMPDAT (Ch-%d)\n", i);
        TPWM_SET_PERIOD((TIMER_T *)TimerCh[i], 0xFFFF);
        TPWM_SET_CMPDAT((TIMER_T *)TimerCh[i], 0x1000);
        TPWM_SET_COUNTER_TYPE((TIMER_T *)TimerCh[i], TPWM_UP_DOWN_COUNT);
        TPWM_EnableAcc(TimerCh[i], 0, TPWM_IFA_COMPARE_UP_COUNT_POINT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMIFA, BIT31 | BIT29);
    }

    /* Check TPWM_START_COUNTER */
    for (i = 0; i < 4; i++)
    {
        TPWM_EnableTriggerEADC((TIMER_T *)TimerCh[i], TPWM_TRIGGER_EADC_AT_ZERO_POINT); // Trigger ADC conversion at zero point (ZIF)
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL, BIT7);

        TPWM_START_COUNTER((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCTL & BIT0, BIT0);
    }

    __NOP();

    for (i = 0; i < 4; i++)
    {
        CU_ASSERT(TimerCh[i]->PWMCNT);
    }

    /* Check TPWM_GET_REACH_MAX_CNT_STATUS */
    while (TPWM_GET_REACH_MAX_CNT_STATUS(TIMER3) == 0) {};

    CU_ASSERT_EQUAL(TPWM_GET_REACH_MAX_CNT_STATUS(TIMER0), 0x1);

    CU_ASSERT_EQUAL(TPWM_GET_REACH_MAX_CNT_STATUS(TIMER1), 0x1);

    CU_ASSERT_EQUAL(TPWM_GET_REACH_MAX_CNT_STATUS(TIMER2), 0x1);

    CU_ASSERT_EQUAL(TPWM_GET_REACH_MAX_CNT_STATUS(TIMER3), 0x1);

    for (i = 0; i < 4; i++)
        while (TimerCh[i]->PWMCNT < 0x10000) {};

    __NOP();

    __NOP();

    /* Check TPWM_STOP_COUNTER */
    for (i = 0; i < 4; i++)
    {
        TPWM_STOP_COUNTER((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMPERIOD, 0x0);

        TimerCh[i]->PWMCTL &= ~TIMER_PWMCTL_CNTEN_Msk; // Force disable PWM CNTEN bit
        CU_ASSERT_EQUAL((TimerCh[i]->PWMCTL & TIMER_PWMCTL_CNTEN_Msk), 0x0);

        TPWM_DisableTriggerEADC((TIMER_T *)TimerCh[i]); // Disable trigger ADC conversion
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL, 0x0);
    }

    /* Check TPWM_CLEAR_REACH_MAX_CNT_STATUS, TPWM_GET_TRG_EADC_STATUS, TPWM_CLEAR_TRG_EADC_STATUS */
    for (i = 0; i < 4; i++)
    {
        TPWM_CLEAR_REACH_MAX_CNT_STATUS((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TPWM_GET_REACH_MAX_CNT_STATUS((TIMER_T *)TimerCh[i]), 0x0);

        CU_ASSERT_EQUAL(TPWM_GET_TRG_EADC_STATUS((TIMER_T *)TimerCh[i]), 0x1);
        TPWM_CLEAR_TRG_EADC_STATUS((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TPWM_GET_TRG_EADC_STATUS((TIMER_T *)TimerCh[i]), 0x0);
    }

    /* Check TPWM_GET_ZERO_INT_FLAG, TPWM_CLEAR_ZERO_INT_FLAG */
    for (i = 0; i < 4; i++)
    {
        CU_ASSERT_EQUAL(TPWM_GET_ZERO_INT_FLAG((TIMER_T *)TimerCh[i]), 0x1);
        TPWM_CLEAR_ZERO_INT_FLAG((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TPWM_GET_ZERO_INT_FLAG((TIMER_T *)TimerCh[i]), 0x0);
    }

    /* Check TPWM_GET_PERIOD_INT_FLAG, TPWM_CLEAR_PERIOD_INT_FLAG */
    for (i = 0; i < 4; i++)
    {
        CU_ASSERT_EQUAL(TPWM_GET_PERIOD_INT_FLAG((TIMER_T *)TimerCh[i]), 0x1);
        TPWM_CLEAR_PERIOD_INT_FLAG((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TPWM_GET_PERIOD_INT_FLAG((TIMER_T *)TimerCh[i]), 0x0);
    }

    /* Check TPWM_GET_CMP_UP_INT_FLAG, TPWM_CLEAR_CMP_UP_INT_FLAG */
    for (i = 0; i < 4; i++)
    {
        CU_ASSERT_EQUAL(TPWM_GET_CMP_UP_INT_FLAG((TIMER_T *)TimerCh[i]), 0x1);
        TPWM_CLEAR_CMP_UP_INT_FLAG((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TPWM_GET_CMP_UP_INT_FLAG((TIMER_T *)TimerCh[i]), 0x0);
    }

    /* Check TPWM_GET_CMP_DOWN_INT_FLAG, TPWM_CLEAR_CMP_DOWN_INT_FLAG */
    for (i = 0; i < 4; i++)
    {
        CU_ASSERT_EQUAL(TPWM_GET_CMP_DOWN_INT_FLAG((TIMER_T *)TimerCh[i]), 0x1);
        TPWM_CLEAR_CMP_DOWN_INT_FLAG((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TPWM_GET_CMP_DOWN_INT_FLAG((TIMER_T *)TimerCh[i]), 0x0);
    }

    /* Check TPWM_GetAccInt, TPWM_ClearAccInt */
    for (i = 0; i < 4; i++)
    {
        CU_ASSERT_EQUAL(TPWM_GetAccInt(TimerCh[i]), 0x1);
        TPWM_ClearAccInt(TimerCh[i]);
        CU_ASSERT_EQUAL(TPWM_GetAccInt(TimerCh[i]), 0x0);
    }

    /* Reset Timer PWM function */
    TPWM_DISABLE_PWM_MODE(TIMER0);
    TPWM_DISABLE_PWM_MODE(TIMER1);
    TPWM_DISABLE_PWM_MODE(TIMER2);
    TPWM_DISABLE_PWM_MODE(TIMER3);
    TPWM_ENABLE_PWM_MODE(TIMER0);
    TPWM_ENABLE_PWM_MODE(TIMER1);
    TPWM_ENABLE_PWM_MODE(TIMER2);
    TPWM_ENABLE_PWM_MODE(TIMER3);

    /* Configure Timer PWM */
    for (i = 0; i < 4; i++)
    {
        TPWM_SET_PERIOD((TIMER_T *)TimerCh[i], 0xF000);
        TPWM_SET_CMPDAT((TIMER_T *)TimerCh[i], 0x2000);
    }

    TPWM_SET_COUNTER_SYNC_MODE(TIMER0, TPWM_CNTR_SYNC_START_BY_TIMER0);
    TPWM_SET_COUNTER_SYNC_MODE(TIMER1, TPWM_CNTR_SYNC_START_BY_TIMER0);
    TPWM_SET_COUNTER_SYNC_MODE(TIMER2, TPWM_CNTR_SYNC_START_BY_TIMER2);
    TPWM_SET_COUNTER_SYNC_MODE(TIMER3, TPWM_CNTR_SYNC_START_BY_TIMER2);

    for (i = 0; i < 4; i++)
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCNT, 0x0);

    /* Check TPWM_TRIGGER_COUNTER_SYNC for Timer0 and Timer1 */
    TPWM_TRIGGER_COUNTER_SYNC(TIMER1);
    TPWM_TRIGGER_COUNTER_SYNC(TIMER3);
    __NOP();
    __NOP();

    for (i = 0; i < 4; i++)
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCNT, 0x0);

    TPWM_TRIGGER_COUNTER_SYNC(TIMER0); // Sync start Timer0 amd Timer1
    CU_ASSERT_EQUAL(TIMER0->PWMSTRG, 0x0);
    __NOP();
    __NOP();

    for (i = 0; i < 2; i++)
        CU_ASSERT((TimerCh[i]->PWMCNT > 0x10000));

    for (i = 2; i < 4; i++)
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCNT, 0x0);

    /* Check TPWM_TRIGGER_COUNTER_SYNC for Timer2 and Timer3 */
    TPWM_TRIGGER_COUNTER_SYNC(TIMER2); // Sync start Timer2 amd Timer3
    CU_ASSERT_EQUAL(TIMER2->PWMSTRG, 0x0);
    __NOP();
    __NOP();

    for (i = 2; i < 4; i++)
        CU_ASSERT((TimerCh[i]->PWMCNT > 0x10000));

    /* Check TPWM_CLEAR_COUNTER */
    for (i = 0; i < 4; i++)
    {
        while (1)
        {
            if ((TimerCh[i]->PWMCNT > 0x12000) && (TimerCh[i]->PWMCNT < 0x13000))
            {
                break;
            }
        }

        TPWM_CLEAR_COUNTER((TIMER_T *)TimerCh[i]);
        __NOP();
        __NOP();
        CU_ASSERT((TimerCh[i]->PWMCNT < 0x10100));
    }

    SYS_UnlockReg();

    /* Check TPWM_SW_TRIGGER_BRAKE with TPWM_BRAKE_EDGE*/
    for (i = 0; i < 4; i++)
    {
        while (1)
        {
            if ((TimerCh[i]->PWMCNT > 0x12000) && (TimerCh[i]->PWMCNT < 0x13000))
            {
                break;
            }
        }

        TPWM_SW_TRIGGER_BRAKE((TIMER_T *)TimerCh[i], TPWM_BRAKE_EDGE);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMSWBRK, 0x0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTSTS1, 0x00030003);
        TimerCh[i]->PWMINTSTS1 = 0xFFFFFFFF;
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTSTS1, 0x00030000);

        while (TimerCh[i]->PWMCNT < 0x1E000) {};

        while (TimerCh[i]->PWMCNT > 0x10010) {};

        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTSTS1, 0x00000000);
    }

    /* Check TPWM_SW_TRIGGER_BRAKE with TPWM_BRAKE_LEVEL*/
    for (i = 0; i < 4; i++)
    {
        while (1)
        {
            if ((TimerCh[i]->PWMCNT > 0x12000) && (TimerCh[i]->PWMCNT < 0x13000))
            {
                break;
            }
        }

        TPWM_SW_TRIGGER_BRAKE((TIMER_T *)TimerCh[i], TPWM_BRAKE_LEVEL);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMSWBRK, 0x0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTSTS1, 0x03000300);
        TimerCh[i]->PWMINTSTS1 = 0xFFFFFFFF;
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTSTS1, 0x03000000);

        while (TimerCh[i]->PWMCNT < 0x1E000) {};

        while (TimerCh[i]->PWMCNT > 0x10010) {};

        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTSTS1, 0x00000000);
    }
}

void API_TPWM_SetCounterClockSource(void)
{
    volatile uint32_t i;

    TIMER_PWM_Init();

    /* Check TPWM_SetCounterClockSource */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_SetCounterClockSource (Ch-%d)\n", i);
        TPWM_SetCounterClockSource((TIMER_T *)TimerCh[i], TPWM_CNTR_CLKSRC_TMR_CLK);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCLKSRC, 0x0);

        TPWM_SetCounterClockSource((TIMER_T *)TimerCh[i], TPWM_CNTR_CLKSRC_TIMER0_INT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCLKSRC, 0x1);

        TPWM_SetCounterClockSource((TIMER_T *)TimerCh[i], TPWM_CNTR_CLKSRC_TIMER1_INT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCLKSRC, 0x2);

        TPWM_SetCounterClockSource((TIMER_T *)TimerCh[i], TPWM_CNTR_CLKSRC_TIMER2_INT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCLKSRC, 0x3);

        TPWM_SetCounterClockSource((TIMER_T *)TimerCh[i], TPWM_CNTR_CLKSRC_TIMER3_INT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCLKSRC, 0x4);
    }
}

void API_TPWM_ConfigOutputFreqAndDuty(void)
{
    volatile uint32_t i;
    uint32_t u32TragerFreq, u32PERIOD, u32CMP;

    TIMER_PWM_Init();

    /* Check API_TPWM_ConfigOutputFreqAndDuty, TPWM_AUTO_RELOAD_MODE */
    u32TragerFreq = TPWM_ConfigOutputFreqAndDuty(TIMER0, 12000, 50);
    CU_ASSERT_EQUAL(u32TragerFreq, 12000);
    u32PERIOD = TPWM_GET_PERIOD(TIMER0);
    u32CMP = TPWM_GET_CMPDAT(TIMER0);
    CU_ASSERT_EQUAL((u32CMP * 2), (u32PERIOD)/*(u32PERIOD + 1)*/);
    CU_ASSERT_EQUAL((TIMER0->PWMCTL & BIT3), TPWM_AUTO_RELOAD_MODE);
    printf("\n%d, %d ", u32CMP, u32PERIOD);

    u32TragerFreq = TPWM_ConfigOutputFreqAndDuty(TIMER1, 1000, 20);
    CU_ASSERT_EQUAL(u32TragerFreq, 1000);
    u32PERIOD = TPWM_GET_PERIOD(TIMER1);
    u32CMP = TPWM_GET_CMPDAT(TIMER1);
    CU_ASSERT_EQUAL((u32CMP * 5), (u32PERIOD + 1));
    CU_ASSERT_EQUAL((TIMER1->PWMCTL & BIT3), TPWM_AUTO_RELOAD_MODE);
    printf("\n%d, %d ", u32CMP, u32PERIOD);

    u32TragerFreq = TPWM_ConfigOutputFreqAndDuty(TIMER2, 20000, 10);
    CU_ASSERT_EQUAL(u32TragerFreq, 20000);
    u32PERIOD = TPWM_GET_PERIOD(TIMER2);
    u32CMP = TPWM_GET_CMPDAT(TIMER2);
    CU_ASSERT_EQUAL((u32CMP * 10), (u32PERIOD + 1));
    CU_ASSERT_EQUAL((TIMER2->PWMCTL & BIT3), TPWM_AUTO_RELOAD_MODE);
    printf("\n%d, %d ", u32CMP, u32PERIOD);

    u32TragerFreq = TPWM_ConfigOutputFreqAndDuty(TIMER3, 24000, 40);
//    printf("\n u32TragerFreq %d ",u32TragerFreq);
//    CU_ASSERT_EQUAL(u32TragerFreq, 24000);
    CU_ASSERT_EQUAL(u32TragerFreq, 24003);
    u32PERIOD = TPWM_GET_PERIOD(TIMER3);
    u32CMP = TPWM_GET_CMPDAT(TIMER3);
    CU_ASSERT_EQUAL(((u32CMP * 5) / 2), (u32PERIOD)/*(u32PERIOD + 1)*/);
    CU_ASSERT_EQUAL((TIMER3->PWMCTL & BIT3), TPWM_AUTO_RELOAD_MODE);
    printf("\n%d, %d ", u32CMP, u32PERIOD);

    /* Check TPWM_EnableCounter */
    for (i = 0; i < 4; i++)
    {
        CU_ASSERT_EQUAL((TimerCh[i]->PWMCTL & BIT0), 0x0);
    }

    for (i = 0; i < 4; i++)
    {
        TPWM_EnableCounter((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMCTL & BIT0), 0x1);
    }

    /* Check TPWM_DisableCounter */
    for (i = 0; i < 4; i++)
    {
        TPWM_DisableCounter((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMCTL & BIT0), 0x0);
    }

    /* Check TPWM_ONE_SHOT_MODE */
    for (i = 0; i < 4; i++)
    {
        TimerCh[i]->PWMCTL |= (TPWM_ONE_SHOT_MODE << TIMER_PWMCTL_CNTMODE_Pos);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMCTL & BIT3), BIT3);
    }
}

void API_TPWM_DeadTime(void)
{
    volatile uint32_t i;

    TIMER_PWM_Init();

    /* Check TPWM_EnableDeadTime */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_EnableDeadTime (Ch-%d)\n", i);
        TPWM_EnableDeadTime((TIMER_T *)TimerCh[i], ((i + 1) * 100));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMDTCTL & 0xFFF), ((i + 1) * 100));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMDTCTL & BIT16), BIT16);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMDTCTL & BIT24), 0x0);
    }

    TIMER_PWM_Init();

    /* Check TPWM_EnableDeadTimeWithPrescale */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_EnableDeadTimeWithPrescale (Ch-%d)\n", i);
        TPWM_EnableDeadTimeWithPrescale((TIMER_T *)TimerCh[i], ((i + 1) * 55));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMDTCTL & 0xFFF), ((i + 1) * 55));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMDTCTL & BIT16), BIT16);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMDTCTL & BIT24), BIT24);
    }

    /* Check TPWM_DisableDeadTime */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_DisableDeadTime (Ch-%d)\n", i);
        TPWM_DisableDeadTime((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMDTCTL, 0x0);
    }
}

void API_TPWM_TriggerADC(void)
{
    volatile uint32_t i;

    TIMER_PWM_Init();

    /* Check TPWM_EnableTriggerEADC, TPWM_DisableTriggerEADC */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_EnableTriggerEADC (Ch-%d)\n", i);
        TPWM_EnableTriggerEADC((TIMER_T *)TimerCh[i], TPWM_TRIGGER_EADC_AT_ZERO_POINT);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMTRGCTL & BIT7), BIT7);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMTRGCTL & 0x7), 0x0);
        TPWM_DisableTriggerEADC((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL, 0x0);

        TPWM_EnableTriggerEADC((TIMER_T *)TimerCh[i], TPWM_TRIGGER_EADC_AT_PERIOD_POINT);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMTRGCTL & BIT7), BIT7);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMTRGCTL & 0x7), 0x1);
        TPWM_DisableTriggerEADC((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL, 0x0);

        TPWM_EnableTriggerEADC((TIMER_T *)TimerCh[i], TPWM_TRIGGER_EADC_AT_ZERO_OR_PERIOD_POINT);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMTRGCTL & BIT7), BIT7);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMTRGCTL & 0x7), 0x2);
        TPWM_DisableTriggerEADC((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL, 0x0);

        TPWM_EnableTriggerEADC((TIMER_T *)TimerCh[i], TPWM_TRIGGER_EADC_AT_COMPARE_UP_COUNT_POINT);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMTRGCTL & BIT7), BIT7);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMTRGCTL & 0x7), 0x3);
        TPWM_DisableTriggerEADC((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL, 0x0);

        TPWM_EnableTriggerEADC((TIMER_T *)TimerCh[i], TPWM_TRIGGER_EADC_AT_COMPARE_DOWN_COUNT_POINT);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMTRGCTL & BIT7), BIT7);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMTRGCTL & 0x7), 0x4);
        TPWM_DisableTriggerEADC((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL, 0x0);
    }
}

void API_TPWM_SetLoadMode(void)
{
    volatile uint32_t i;

    TIMER_PWM_Init();

    /* Check TPWM_SetLoadMode */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_SetLoadMode (Ch-%d)\n", i);
        TPWM_SetLoadMode((TIMER_T *)TimerCh[i], TPWM_LOAD_MODE_PERIOD);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMCTL & (BIT8 | BIT9)), 0x0);

        TPWM_SetLoadMode((TIMER_T *)TimerCh[i], TPWM_LOAD_MODE_IMMEDIATE);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMCTL & (BIT8 | BIT9)), BIT9);

        TPWM_SetLoadMode((TIMER_T *)TimerCh[i], TPWM_LOAD_MODE_CENTER);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMCTL & (BIT8 | BIT9)), BIT8);
    }
}

void API_TPWM_BrakePin(void)
{
    volatile uint32_t i;

    TIMER_PWM_Init();

    /* Check TPWM_EnableBrakePinDebounce, TPWM_DisableBrakePinDebounce */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_EnableBrakePinDebounce (Ch-%d)\n", i);
        TPWM_EnableBrakePinDebounce((TIMER_T *)TimerCh[i], TPWM_TM_BRAKE0, 0x7, TPWM_BKP_DBCLK_PCLK_DIV_1);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x1);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x3 << 16)), (0x0 << 16));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 4)), (0x7 << 4));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 1)), (0x0 << 1));
        TPWM_DisableBrakePinDebounce((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x0);

        TPWM_EnableBrakePinDebounce((TIMER_T *)TimerCh[i], TPWM_TM_BRAKE1, 0x4, TPWM_BKP_DBCLK_PCLK_DIV_2);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x1);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x3 << 16)), (0x1 << 16));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 4)), (0x4 << 4));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 1)), (0x1 << 1));
        TPWM_DisableBrakePinDebounce((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x0);

        TPWM_EnableBrakePinDebounce((TIMER_T *)TimerCh[i], TPWM_TM_BRAKE2, 0x1, TPWM_BKP_DBCLK_PCLK_DIV_4);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x1);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x3 << 16)), (0x2 << 16));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 4)), (0x1 << 4));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 1)), (0x2 << 1));
        TPWM_DisableBrakePinDebounce((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x0);

        TPWM_EnableBrakePinDebounce((TIMER_T *)TimerCh[i], TPWM_TM_BRAKE3, 0x0, TPWM_BKP_DBCLK_PCLK_DIV_8);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x1);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x3 << 16)), (0x3 << 16));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 4)), (0x0 << 4));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 1)), (0x3 << 1));
        TPWM_DisableBrakePinDebounce((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x0);

        TPWM_EnableBrakePinDebounce((TIMER_T *)TimerCh[i], TPWM_TM_BRAKE0, 0x0, TPWM_BKP_DBCLK_PCLK_DIV_16);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x1);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x3 << 16)), (0x0 << 16));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 4)), (0x0 << 4));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 1)), (0x4 << 1));
        TPWM_DisableBrakePinDebounce((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x0);

        TPWM_EnableBrakePinDebounce((TIMER_T *)TimerCh[i], TPWM_TM_BRAKE1, 0x1, TPWM_BKP_DBCLK_PCLK_DIV_32);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x1);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x3 << 16)), (0x1 << 16));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 4)), (0x1 << 4));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 1)), (0x5 << 1));
        TPWM_DisableBrakePinDebounce((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x0);

        TPWM_EnableBrakePinDebounce((TIMER_T *)TimerCh[i], TPWM_TM_BRAKE2, 0x4, TPWM_BKP_DBCLK_PCLK_DIV_64);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x1);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x3 << 16)), (0x2 << 16));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 4)), (0x4 << 4));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 1)), (0x6 << 1));
        TPWM_DisableBrakePinDebounce((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x0);

        TPWM_EnableBrakePinDebounce((TIMER_T *)TimerCh[i], TPWM_TM_BRAKE3, 0x7, TPWM_BKP_DBCLK_PCLK_DIV_128);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x1);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x3 << 16)), (0x3 << 16));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 4)), (0x7 << 4));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & (0x7 << 1)), (0x7 << 1));
        TPWM_DisableBrakePinDebounce((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBNF & BIT0), 0x0);
    }
}

void API_TPWM_EnableFaultBrake(void)
{
    volatile uint32_t i;

    TIMER_PWM_Init();

    /* Check TPWM_EnableFaultBrake */
    for (i = 0; i < 4; i++)
    {
        printf("TPWM_EnableFaultBrake (Ch-%d)\n", i);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_TRI_STATE, TPWM_BRK_OUT_TRI_STATE, TPWM_BRAKE_SOURCE_EDGE_ACMP0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x0);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0x5 << 16));
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x1);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_TRI_STATE, TPWM_BRK_OUT_LOW,     TPWM_BRAKE_SOURCE_EDGE_ACMP1);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x0);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0x9 << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x2);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x3);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_TRI_STATE, TPWM_BRK_OUT_HIGH,    TPWM_BRAKE_SOURCE_EDGE_BKPIN);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x0);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0xD << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x10);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x13);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_TRI_STATE, TPWM_BRK_OUT_DISABLE,  TPWM_BRAKE_SOURCE_EDGE_SYS_CSS);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x1);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0x1 << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x80);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x93);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_LOW,     TPWM_BRK_OUT_TRI_STATE, TPWM_BRAKE_SOURCE_EDGE_SYS_BOD);
        //CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x2);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x3);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0x6 << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x80);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x93);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_LOW,     TPWM_BRK_OUT_LOW,     TPWM_BRAKE_SOURCE_EDGE_SYS_COR);
        //CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x8);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0xB);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0xA << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x80);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x93);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_LOW,     TPWM_BRK_OUT_HIGH,    TPWM_BRAKE_SOURCE_LEVEL_ACMP0);
        //CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0xB);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0xE << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x100);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x193);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_LOW,     TPWM_BRK_OUT_DISABLE,  TPWM_BRAKE_SOURCE_LEVEL_ACMP1);
        //CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0xB);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0x2 << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x200);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x393);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_HIGH,    TPWM_BRK_OUT_TRI_STATE, TPWM_BRAKE_SOURCE_LEVEL_BKPIN);
        //CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0xB);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0x7 << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x1000);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x1393);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_HIGH,    TPWM_BRK_OUT_LOW,     TPWM_BRAKE_SOURCE_LEVEL_SYS_RAM);
        //CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x1);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0xF);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0xB << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x8000);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x9393);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_HIGH,    TPWM_BRK_OUT_HIGH,    TPWM_BRAKE_SOURCE_LEVEL_SYS_BOD);
        //CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x2);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0xF);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0xF << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x8000);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x9393);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_HIGH,    TPWM_BRK_OUT_DISABLE,  TPWM_BRAKE_SOURCE_LEVEL_SYS_COR);
        //CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x8);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0xF);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0x3 << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x8000);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x9393);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_DISABLE, TPWM_BRK_OUT_TRI_STATE, TPWM_BRAKE_SOURCE_EDGE_ACMP2);
        //CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0xF);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0x4 << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x1);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x9397);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_DISABLE, TPWM_BRK_OUT_LOW,     TPWM_BRAKE_SOURCE_EDGE_ACMP3);
        //CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0xF);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0x8 << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x1);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x939F);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_DISABLE, TPWM_BRK_OUT_HIGH,    TPWM_BRAKE_SOURCE_LEVEL_ACMP2);
        //CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0xF);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0xC << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x1);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x979F);

        TPWM_EnableFaultBrake((TIMER_T *)TimerCh[i], TPWM_BRK_OUT_DISABLE, TPWM_BRK_OUT_DISABLE,  TPWM_BRAKE_SOURCE_LEVEL_ACMP3);
        //CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0x0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMFAILBRK, 0xF);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & (0xF << 16)), (0x0 << 16));
        //CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL&0xFFFF), 0x1);
        CU_ASSERT_EQUAL((TimerCh[i]->PWMBRKCTL & 0xFFFF), 0x9F9F);

    }

    /* Check TPWM_EnableFaultBrakeInt, TPWM_DisableFaultBrakeInt */
    for (i = 0; i < 4; i++)
    {
        TPWM_EnableFaultBrakeInt((TIMER_T *)TimerCh[i], TPWM_BRAKE_EDGE);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN1, 0x1);
        TPWM_DisableFaultBrakeInt((TIMER_T *)TimerCh[i], TPWM_BRAKE_EDGE);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN1, 0x0);

        TPWM_EnableFaultBrakeInt((TIMER_T *)TimerCh[i], TPWM_BRAKE_LEVEL);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN1, 0x100);
        TPWM_DisableFaultBrakeInt((TIMER_T *)TimerCh[i], TPWM_BRAKE_LEVEL);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN1, 0x0);
    }

}

void API_TPWM_FailBrakeStatus(void)
{
    volatile uint32_t i;

    TIMER_PWM_Init();

    SYS_UnlockReg();

    for (i = 0; i < 4; i++)
    {
        TPWM_SET_PERIOD((TIMER_T *)TimerCh[i], 0xFFFF);
        TPWM_SET_CMPDAT((TIMER_T *)TimerCh[i], 0x1000);
        TPWM_SET_COUNTER_TYPE((TIMER_T *)TimerCh[i], TPWM_UP_DOWN_COUNT);
        TPWM_START_COUNTER((TIMER_T *)TimerCh[i]);
    }

    /* Check TPWM_GetFaultBrakeIntFlag, TPWM_ClearFaultBrakeIntFlag with TPWM_BRAKE_EDGE */
    for (i = 0; i < 4; i++)
    {
        TPWM_SW_TRIGGER_BRAKE((TIMER_T *)TimerCh[i], TPWM_BRAKE_EDGE);
        CU_ASSERT_EQUAL(TPWM_GetFaultBrakeIntFlag((TIMER_T *)TimerCh[i], TPWM_BRAKE_EDGE), 0x1);
        TPWM_ClearFaultBrakeIntFlag((TIMER_T *)TimerCh[i], TPWM_BRAKE_EDGE);
        CU_ASSERT_EQUAL(TPWM_GetFaultBrakeIntFlag((TIMER_T *)TimerCh[i], TPWM_BRAKE_EDGE), 0x0);
    }

    /* Check TPWM_GetFaultBrakeIntFlag, TPWM_ClearFaultBrakeIntFlag with TPWM_BRAKE_LEVEL */
    for (i = 0; i < 4; i++)
    {
        TPWM_SW_TRIGGER_BRAKE((TIMER_T *)TimerCh[i], TPWM_BRAKE_LEVEL);
        CU_ASSERT_EQUAL(TPWM_GetFaultBrakeIntFlag((TIMER_T *)TimerCh[i], TPWM_BRAKE_LEVEL), 0x1);
        TPWM_ClearFaultBrakeIntFlag((TIMER_T *)TimerCh[i], TPWM_BRAKE_LEVEL);
        CU_ASSERT_EQUAL(TPWM_GetFaultBrakeIntFlag((TIMER_T *)TimerCh[i], TPWM_BRAKE_LEVEL), 0x0);
    }
}

void Func_TPWM_ExtEventTrigger()
{
    volatile uint32_t i;

    TIMER_PWM_Init();

    for (i = 0; i < 4; i++)
    {
        TPWM_EnableExtEventTrigger(TimerCh[i], TPWM_EXT_TGR_PIN_INT1, TPWM_EXT_TGR_COUNTER_RESET);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMEXTETCTL, (TIMER_PWMEXTETCTL_EXTETEN_Msk | (1 << TIMER_PWMEXTETCTL_EXTTRGS_Pos) | (0 << TIMER_PWMEXTETCTL_CNTACTS_Pos)));

        TPWM_EnableExtEventTrigger(TimerCh[i], TPWM_EXT_TGR_PIN_INT2, TPWM_EXT_TGR_COUNTER_START);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMEXTETCTL, (TIMER_PWMEXTETCTL_EXTETEN_Msk | (2 << TIMER_PWMEXTETCTL_EXTTRGS_Pos) | (1 << TIMER_PWMEXTETCTL_CNTACTS_Pos)));

        TPWM_EnableExtEventTrigger(TimerCh[i], TPWM_EXT_TGR_PIN_INT3, TPWM_EXT_TGR_COUNTER_RESET_AND_START);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMEXTETCTL, (TIMER_PWMEXTETCTL_EXTETEN_Msk | (3 << TIMER_PWMEXTETCTL_EXTTRGS_Pos) | (2 << TIMER_PWMEXTETCTL_CNTACTS_Pos)));

        TPWM_EnableExtEventTrigger(TimerCh[i], TPWM_EXT_TGR_PIN_INT4, TPWM_EXT_TGR_COUNTER_RESET);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMEXTETCTL, (TIMER_PWMEXTETCTL_EXTETEN_Msk | (4 << TIMER_PWMEXTETCTL_EXTTRGS_Pos) | (0 << TIMER_PWMEXTETCTL_CNTACTS_Pos)));

        TPWM_EnableExtEventTrigger(TimerCh[i], TPWM_EXT_TGR_PIN_INT5, TPWM_EXT_TGR_COUNTER_START);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMEXTETCTL, (TIMER_PWMEXTETCTL_EXTETEN_Msk | (5 << TIMER_PWMEXTETCTL_EXTTRGS_Pos) | (1 << TIMER_PWMEXTETCTL_CNTACTS_Pos)));

        TPWM_EnableExtEventTrigger(TimerCh[i], TPWM_EXT_TGR_PIN_INT6, TPWM_EXT_TGR_COUNTER_RESET_AND_START);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMEXTETCTL, (TIMER_PWMEXTETCTL_EXTETEN_Msk | (6 << TIMER_PWMEXTETCTL_EXTTRGS_Pos) | (2 << TIMER_PWMEXTETCTL_CNTACTS_Pos)));

        TPWM_EnableExtEventTrigger(TimerCh[i], TPWM_EXT_TGR_PIN_INT7, TPWM_EXT_TGR_COUNTER_START);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMEXTETCTL, (TIMER_PWMEXTETCTL_EXTETEN_Msk | (7 << TIMER_PWMEXTETCTL_EXTTRGS_Pos) | (1 << TIMER_PWMEXTETCTL_CNTACTS_Pos)));

        TPWM_EnableExtEventTrigger(TimerCh[i], TPWM_EXT_TGR_PIN_INT0, TPWM_EXT_TGR_COUNTER_RESET);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMEXTETCTL, (TIMER_PWMEXTETCTL_EXTETEN_Msk | (0 << TIMER_PWMEXTETCTL_EXTTRGS_Pos) | (0 << TIMER_PWMEXTETCTL_CNTACTS_Pos)));

        TPWM_DisableExtEventTrigger(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMEXTETCTL, 0x0);
    }
}

void Const1_Test(void)
{
    CU_ASSERT_EQUAL(TPWM_CH0, (BIT0));
    CU_ASSERT_EQUAL(TPWM_CH1, (BIT1));

    CU_ASSERT_EQUAL(TPWM_UP_COUNT, (0UL << TIMER_PWMCTL_CNTTYPE_Pos));
    CU_ASSERT_EQUAL(TPWM_DOWN_COUNT, (BIT1));
    CU_ASSERT_EQUAL(TPWM_UP_DOWN_COUNT, (BIT2));

    CU_ASSERT_EQUAL(TPWM_AUTO_RELOAD_MODE, (0UL));
    CU_ASSERT_EQUAL(TPWM_ONE_SHOT_MODE, (1UL));

    CU_ASSERT_EQUAL(TPWM_TRIGGER_EADC_AT_ZERO_POINT, (0UL << TIMER_PWMTRGCTL_TRGSEL_Pos));
    CU_ASSERT_EQUAL(TPWM_TRIGGER_EADC_AT_PERIOD_POINT, (1UL << TIMER_PWMTRGCTL_TRGSEL_Pos));
    CU_ASSERT_EQUAL(TPWM_TRIGGER_EADC_AT_ZERO_OR_PERIOD_POINT, (2UL << TIMER_PWMTRGCTL_TRGSEL_Pos));
    CU_ASSERT_EQUAL(TPWM_TRIGGER_EADC_AT_COMPARE_UP_COUNT_POINT, (3UL << TIMER_PWMTRGCTL_TRGSEL_Pos));
    CU_ASSERT_EQUAL(TPWM_TRIGGER_EADC_AT_COMPARE_DOWN_COUNT_POINT, (4UL << TIMER_PWMTRGCTL_TRGSEL_Pos));

    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_EDGE_ACMP0, (BIT0));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_EDGE_ACMP1, (BIT1));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_EDGE_ACMP2, (BIT2));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_EDGE_ACMP3, (BIT3));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_EDGE_BKPIN, (BIT4));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_EDGE_SYS_CSS, (BIT7 | (BIT0 << 16)));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_EDGE_SYS_BOD, (BIT7 | (BIT1 << 16)));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_EDGE_SYS_RAM, (BIT7 | (BIT2 << 16)));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_EDGE_SYS_COR, (BIT7 | (BIT3 << 16)));

    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_LEVEL_ACMP0, (BIT8));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_LEVEL_ACMP1, (BIT9));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_LEVEL_ACMP2, (BIT10));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_LEVEL_ACMP3, (BIT11));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_LEVEL_BKPIN, (BIT12));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_LEVEL_SYS_CSS, (BIT15 | (BIT0 << 16)));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_LEVEL_SYS_BOD, (BIT15 | (BIT1 << 16)));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_LEVEL_SYS_RAM, (BIT15 | (BIT2 << 16)));
    CU_ASSERT_EQUAL(TPWM_BRAKE_SOURCE_LEVEL_SYS_COR, (BIT15 | (BIT3 << 16)));

    CU_ASSERT_EQUAL(TPWM_BRAKE_EDGE, (BIT0));
    CU_ASSERT_EQUAL(TPWM_BRAKE_LEVEL, (BIT8));

    CU_ASSERT_EQUAL(TPWM_BRK_OUT_DISABLE, (0UL));
    CU_ASSERT_EQUAL(TPWM_BRK_OUT_TRI_STATE, (1UL));
    CU_ASSERT_EQUAL(TPWM_BRK_OUT_LOW, (2UL));
    CU_ASSERT_EQUAL(TPWM_BRK_OUT_HIGH, (3UL));

    CU_ASSERT_EQUAL(TPWM_LOAD_MODE_PERIOD, (0UL));
    CU_ASSERT_EQUAL(TPWM_LOAD_MODE_IMMEDIATE, (BIT9));
    CU_ASSERT_EQUAL(TPWM_LOAD_MODE_CENTER, (BIT8));

    CU_ASSERT_EQUAL(TPWM_CNTR_SYNC_DISABLE, (0UL));
    CU_ASSERT_EQUAL(TPWM_CNTR_SYNC_START_BY_TIMER0, (BIT0));
    CU_ASSERT_EQUAL(TPWM_CNTR_SYNC_CLEAR_BY_TIMER0, (3UL));
    CU_ASSERT_EQUAL(TPWM_CNTR_SYNC_START_BY_TIMER2, (BIT8 | BIT0));
    CU_ASSERT_EQUAL(TPWM_CNTR_SYNC_CLEAR_BY_TIMER2, (BIT8 | 3UL));

    CU_PASS();
}

void Macro1_Test(void)
{
    volatile uint32_t i,x;

    TIMER_PWM_Init();

    for (i = 0; i < 4; i++)
    {
        TPWM_ENABLE_PWM_MODE(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->ALTCTL, 1);

        TPWM_DISABLE_PWM_MODE(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->ALTCTL, 0);

        /* Enable TPWM mode and begin to test TPWM */
        TPWM_ENABLE_PWM_MODE(TimerCh[i]);

        TPWM_START_COUNTER(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCTL & TIMER_PWMCTL_CNTEN_Msk, TIMER_PWMCTL_CNTEN_Msk);

        TPWM_STOP_COUNTER(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMPERIOD, 0);

        TPWM_SET_PRESCALER(TimerCh[i], 0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCLKPSC, 0);
        TPWM_SET_PRESCALER(TimerCh[i], 0x5A);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCLKPSC, 0x5A);
        TPWM_SET_PRESCALER(TimerCh[i], 0xFF);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCLKPSC, 0xFF);

        TPWM_SET_PRESCALER(TimerCh[i], 0xA5);
        CU_ASSERT_EQUAL(TPWM_GET_PRESCALER(TimerCh[i]), 0xA5);

        TPWM_SET_PERIOD(TimerCh[i], 0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMPERIOD, 0);
        TPWM_SET_PERIOD(TimerCh[i], 0x5A5A);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMPERIOD, 0x5A5A);
        TPWM_SET_PERIOD(TimerCh[i], 0xFFFF);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMPERIOD, 0xFFFF);

        TPWM_SET_PERIOD(TimerCh[i], 0xA5A5);
        CU_ASSERT_EQUAL(TPWM_GET_PERIOD(TimerCh[i]), 0xA5A5);

        TPWM_SET_CMPDAT(TimerCh[i], 0);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCMPDAT, 0);
        TPWM_SET_CMPDAT(TimerCh[i], 0x5A5A);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCMPDAT, 0x5A5A);
        TPWM_SET_CMPDAT(TimerCh[i], 0xFFFF);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCMPDAT, 0xFFFF);

        TPWM_SET_CMPDAT(TimerCh[i], 0xA5A5);
        CU_ASSERT_EQUAL(TPWM_GET_CMPDAT(TimerCh[i]), 0xA5A5);

        TimerCh[i]->PWMCTL &= ~TIMER_PWMCTL_CNTEN_Msk;  // Stop counting first.
        TPWM_CLEAR_COUNTER(TimerCh[i]);

        for (x = 0; x < 100; x++) {}; // delay to wait register updated

        CU_ASSERT_EQUAL(TimerCh[i]->PWMCNT & 0xFFFF, 0);

        //    CU_ASSERT_EQUAL(TimerCh[i]->PWMCNTCLR, 0);  // PWMCNTCLR cleard by hardware automatically
        TPWM_START_COUNTER(TimerCh[i]);

        TPWM_ENABLE_OUTPUT(TimerCh[i], TPWM_CH0);

        CU_ASSERT_EQUAL(TimerCh[i]->PWMPOEN, TPWM_CH0);

        TPWM_SET_OUTPUT_INVERSE(TimerCh[i], TPWM_CH0);

        CU_ASSERT_EQUAL(TimerCh[i]->PWMPOLCTL, TPWM_CH0);

        TPWM_ENABLE_PERIOD_INT(TimerCh[i]);

        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN0 & TIMER_PWMINTEN0_PIEN_Msk, TIMER_PWMINTEN0_PIEN_Msk);

        TPWM_DISABLE_PERIOD_INT(TimerCh[i]);

        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN0 & TIMER_PWMINTEN0_PIEN_Msk, 0);

        CU_ASSERT_EQUAL(TPWM_GET_PERIOD_INT_FLAG(TimerCh[i]), (TimerCh[i]->PWMINTSTS0 & TIMER_PWMINTSTS0_PIF_Msk) ? 1 : 0);

        TPWM_CLEAR_PERIOD_INT_FLAG(TimerCh[i]);

        for (x = 0; x < 100; x++) {}; // delay to wait register updated

        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTSTS0 & TIMER_PWMINTSTS0_PIF_Msk, 0);

        TPWM_ENABLE_CMP_UP_INT(TimerCh[i]);

        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN0 & TIMER_PWMINTEN0_CMPUIEN_Msk, TIMER_PWMINTEN0_CMPUIEN_Msk);

        TPWM_DISABLE_CMP_UP_INT(TimerCh[i]);

        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTEN0 & TIMER_PWMINTEN0_CMPUIEN_Msk, 0);

        CU_ASSERT_EQUAL(TPWM_GET_CMP_UP_INT_FLAG(TimerCh[i]), (TimerCh[i]->PWMINTSTS0 & TIMER_PWMINTSTS0_CMPUIF_Msk) ? 1 : 0);

        TPWM_CLEAR_CMP_UP_INT_FLAG(TimerCh[i]);

        for (x = 0; x < 100; x++) {}; // delay to wait register updated

        CU_ASSERT_EQUAL(TimerCh[i]->PWMINTSTS0 & TIMER_PWMINTSTS0_CMPUIF_Msk, 0);

        CU_ASSERT_EQUAL(TPWM_GET_REACH_MAX_CNT_STATUS(TimerCh[i]), (TimerCh[i]->PWMSTATUS & TIMER_PWMSTATUS_CNTMAXF_Msk) ? 1 : 0);

        TPWM_CLEAR_REACH_MAX_CNT_STATUS(TimerCh[i]);

        CU_ASSERT_EQUAL(TimerCh[i]->PWMSTATUS & TIMER_PWMSTATUS_CNTMAXF_Msk, 0);

        CU_ASSERT_EQUAL(TPWM_GET_TRG_EADC_STATUS(TimerCh[i]), (TimerCh[i]->PWMSTATUS & TIMER_PWMSTATUS_EADCTRGF_Msk) ? 1 : 0);

        TPWM_CLEAR_TRG_EADC_STATUS(TimerCh[i]);

        CU_ASSERT_EQUAL(TimerCh[i]->PWMSTATUS & TIMER_PWMSTATUS_EADCTRGF_Msk, 0);
    }

    CU_PASS();
}

void Func1_Test(void)
{
    volatile uint32_t i;

    TIMER_PWM_Init();

    /* Check TPWM_ENABLE_COMPLEMENTARY_MODE */
    for (i = 0; i < 4; i++)
    {
        uint32_t timerClkSrc, targetFreq, finalFreq;

        if (i == 0 || i == 1)
            timerClkSrc = CLK_GetPCLK1Freq();
        else
            timerClkSrc = CLK_GetPCLK3Freq();

        targetFreq = timerClkSrc / 2;
        finalFreq = TPWM_ConfigOutputFreqAndDuty(TimerCh[i], targetFreq, 10);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCTL & TIMER_PWMCTL_CNTMODE_Msk, TPWM_AUTO_RELOAD_MODE << TIMER_PWMCTL_CNTMODE_Pos);
        CU_ASSERT_EQUAL(finalFreq, targetFreq);
        CU_ASSERT_EQUAL(timerClkSrc / (TimerCh[i]->PWMCLKPSC + 1) / (TimerCh[i]->PWMPERIOD + 1), targetFreq);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCMPDAT, (TimerCh[i]->PWMPERIOD + 1) * 10 / 100);

        targetFreq = timerClkSrc / 4;
        finalFreq = TPWM_ConfigOutputFreqAndDuty(TimerCh[i], targetFreq, 90);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCTL & TIMER_PWMCTL_CNTMODE_Msk, TPWM_AUTO_RELOAD_MODE << TIMER_PWMCTL_CNTMODE_Pos);
        CU_ASSERT_EQUAL(finalFreq, targetFreq);
        CU_ASSERT_EQUAL(timerClkSrc / (TimerCh[i]->PWMCLKPSC + 1) / (TimerCh[i]->PWMPERIOD + 1), targetFreq);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCMPDAT, (TimerCh[i]->PWMPERIOD + 1) * 90 / 100);

        TPWM_EnableCounter(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCTL & TIMER_PWMCTL_CNTEN_Msk, TIMER_PWMCTL_CNTEN_Msk);

        TPWM_EnableTriggerEADC(TimerCh[i], TPWM_TRIGGER_EADC_AT_ZERO_POINT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGEADC_Msk, TIMER_PWMTRGCTL_TRGEADC_Msk);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGSEL_Msk, TPWM_TRIGGER_AT_ZERO_POINT);

        TPWM_DisableTriggerEADC(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGEADC_Msk, 0);

        TPWM_EnableTriggerEADC(TimerCh[i], TPWM_TRIGGER_EADC_AT_PERIOD_POINT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGEADC_Msk, TIMER_PWMTRGCTL_TRGEADC_Msk);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGSEL_Msk, TPWM_TRIGGER_AT_PERIOD_POINT);

        TPWM_DisableTriggerEADC(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGEADC_Msk, 0);

        TPWM_EnableTriggerEADC(TimerCh[i], TPWM_TRIGGER_EADC_AT_ZERO_OR_PERIOD_POINT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGEADC_Msk, TIMER_PWMTRGCTL_TRGEADC_Msk);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGSEL_Msk, TPWM_TRIGGER_AT_ZERO_OR_PERIOD_POINT);

        TPWM_DisableTriggerEADC(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGEADC_Msk, 0);

        TPWM_EnableTriggerEADC(TimerCh[i], TPWM_TRIGGER_EADC_AT_COMPARE_UP_COUNT_POINT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGEADC_Msk, TIMER_PWMTRGCTL_TRGEADC_Msk);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGSEL_Msk, TPWM_TRIGGER_AT_COMPARE_UP_COUNT_POINT);

        TPWM_DisableTriggerEADC(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGEADC_Msk, 0);

        TPWM_EnableTriggerEADC(TimerCh[i], TPWM_TRIGGER_EADC_AT_COMPARE_DOWN_COUNT_POINT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGEADC_Msk, TIMER_PWMTRGCTL_TRGEADC_Msk);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGSEL_Msk, TPWM_TRIGGER_AT_COMPARE_DOWN_COUNT_POINT);

        TPWM_DisableTriggerEADC(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMTRGCTL & TIMER_PWMTRGCTL_TRGEADC_Msk, 0);

        TPWM_DisableCounter(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMCTL & TIMER_PWMCTL_CNTEN_Msk, 0);
    }

    CU_PASS();
}

void Func_TPWM_IFA(void)
{
    volatile uint32_t i;

    TIMER_PWM_Init();

    /* Check TPWM_IFA */
    for (i = 0; i < 4; i++)
    {
        printf("LPTPWM_ENABLE_IFA (Ch-%d)\n", i);
        TPWM_EnableAcc(TimerCh[i], 0xFFFF, TPWM_IFA_COMPARE_DOWN_COUNT_POINT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMIFA, BIT31 | 0xFFFF | BIT29 | BIT28);
        TPWM_DisableAcc(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMIFA, 0xFFFF | BIT29 | BIT28);
        TPWM_EnableAcc(TimerCh[i], 0xA5A5, TPWM_IFA_COMPARE_UP_COUNT_POINT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMIFA, BIT31 | 0xA5A5 | BIT29);
        TPWM_DisableAcc(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMIFA, 0xA5A5 | BIT29);
        TPWM_EnableAcc(TimerCh[i], 0x5A5A, TPWM_IFA_PERIOD_POINT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMIFA, BIT31 | 0x5A5A | BIT28);
        TPWM_DisableAcc(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMIFA, 0x5A5A | BIT28);
        TPWM_EnableAcc(TimerCh[i], 0, TPWM_IFA_ZERO_POINT);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMIFA, BIT31);
        TPWM_DisableAcc(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMIFA, 0);
        TPWM_EnableAccStopMode(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMIFA, BIT24);
        TPWM_DisableAccStopMode(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMIFA, 0UL);
        TPWM_EnableAccPDMA(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMAPDMACTL, TIMER_PWMAPDMACTL_APDMAEN_Msk);
        TPWM_DisableAccPDMA(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMAPDMACTL, 0UL);
        TPWM_EnableAccInt(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMAINTEN, TIMER_PWMAINTEN_IFAIEN_Msk);
        TPWM_DisableAccInt(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->PWMAINTEN, 0UL);
    }
}

CU_SuiteInfo TpwmSuites[] =
{
    { "Tpwm Const Test", Tpwm_Test_Init, Tpwm_Test_Clean, NULL, NULL, Tpwm_ConstTest },
    { "Tpwm Macro Test", Tpwm_Test_Init, Tpwm_Test_Clean, NULL, NULL, Tpwm_MacroTest },
    { "Tpwm Func  Test", Tpwm_Test_Init, Tpwm_Test_Clean, NULL, NULL, Tpwm_FuncTest },
    CU_SUITE_INFO_NULL
};

CU_TestInfo  Tpwm_ConstTest[] =
{
    { "Const1",  Const1_Test },
    CU_TEST_INFO_NULL
};

CU_TestInfo  Tpwm_MacroTest[] =
{
    { "Macro1",  Macro1_Test },
    {"Check Timer PWM Basic MACRO ",    Check_Basic_MACRO},
    {"Check Timer PWM Advance MACRO ",  Check_Advance_MACRO},
    CU_TEST_INFO_NULL
};

CU_TestInfo  Tpwm_FuncTest[] =
{
    { "Func1", Func1_Test },
    {"Check Counter Clock Source API",              API_TPWM_SetCounterClockSource},
    {"Check Configure Freq. and Duty API ",         API_TPWM_ConfigOutputFreqAndDuty},
    {"Check Dead-Time APIs ",                       API_TPWM_DeadTime},
    {"Check Trigger ADC APIs ",                     API_TPWM_TriggerADC},
    {"Check Loading Mode API ",                     API_TPWM_SetLoadMode},
    {"Check Brake Pin APIs ",                       API_TPWM_BrakePin},
    {"Check Enable Fault Brake APIs ",              API_TPWM_EnableFaultBrake},
    {"Check Fault Brake Status APIs ",              API_TPWM_FailBrakeStatus},
    {"Check ExtEventTrigger ",              Func_TPWM_ExtEventTrigger},
    {"Check TPWM_IFA ",              Func_TPWM_IFA},
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/