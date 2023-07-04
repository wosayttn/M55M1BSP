/******************************************************************************
* @file    timer_cunit.c
* @version V1.00
* @brief   CUnit test timer
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
#include "timer_cunit.h"

int Timer_Test_Init(void)
{
    return 0;
}
int Timer_Test_Clean(void)
{
    return 0;
}

#define ENABLE_GCR_SELFTEST     (outpw(0x40000E00, 0x1))
#define DISABLE_GCR_SELFTEST    (outpw(0x40000E00, 0x0))

TIMER_T *TimerCh[] =
{
    TIMER0,
    TIMER1,
    TIMER2,
    TIMER3
};

volatile uint8_t g_u8IsTMRWKF[4] = {0};
void TIMER0_IRQHandler(void)
{
    if (TIMER_GetIntFlag(TIMER0) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);

        while (TIMER_GetIntFlag(TIMER0) == 1);
    }

    if (TIMER_GetWakeupFlag(TIMER0) == 1)
    {
        /* Clear Timer0 wake-up flag */
        TIMER_ClearWakeupFlag(TIMER0);

        while (TIMER_GetWakeupFlag(TIMER0) == 1);

        g_u8IsTMRWKF[0] = 1;
    }
}

void TIMER1_IRQHandler(void)
{
    if (TIMER_GetIntFlag(TIMER1) == 1)
    {
        /* Clear Timer1 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER1);

        while (TIMER_GetIntFlag(TIMER1) == 1);
    }

    if (TIMER_GetWakeupFlag(TIMER1) == 1)
    {
        /* Clear Timer1 wake-up flag */
        TIMER_ClearWakeupFlag(TIMER1);

        while (TIMER_GetWakeupFlag(TIMER1) == 1);

        g_u8IsTMRWKF[1] = 1;
    }
}

void TIMER2_IRQHandler(void)
{
    if (TIMER_GetIntFlag(TIMER2) == 1)
    {
        /* Clear Timer2 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER2);

        while (TIMER_GetIntFlag(TIMER2) == 1);
    }

    if (TIMER_GetWakeupFlag(TIMER2) == 1)
    {
        /* Clear Timer2 wake-up flag */
        TIMER_ClearWakeupFlag(TIMER2);

        while (TIMER_GetWakeupFlag(TIMER2) == 1);

        g_u8IsTMRWKF[2] = 1;
    }
}

void TIMER3_IRQHandler(void)
{
    if (TIMER_GetIntFlag(TIMER3) == 1)
    {
        /* Clear Timer3 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER3);

        while (TIMER_GetIntFlag(TIMER3) == 1);
    }

    if (TIMER_GetWakeupFlag(TIMER3) == 1)
    {
        /* Clear Timer3 wake-up flag */
        TIMER_ClearWakeupFlag(TIMER3);

        while (TIMER_GetWakeupFlag(TIMER3) == 1);

        g_u8IsTMRWKF[3] = 1;
    }
}

int32_t TIMER_InitClock(void)
{
    SYS_UnlockReg();
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC, NULL);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_HIRC, NULL);
    CLK_SetModuleClock(TMR2_MODULE, CLK_TMRSEL_TMR2SEL_HIRC, NULL);
    CLK_SetModuleClock(TMR3_MODULE, CLK_TMRSEL_TMR3SEL_HIRC, NULL);

    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_EnableModuleClock(TMR3_MODULE);

    return 0;
}

int32_t IsTimerRegsCleared(TIMER_T *timer)
{
    if ((timer->CTL != 0x0) || (timer->CMP != 0x0) || (timer->EXTCTL != 0x0) ||
            (timer->INTSTS != 0) || (timer->EINTSTS != 0))
    {
        return -1;
    }

    return 0;
}

int32_t ClearTimerRegs(TIMER_T *timer)
{
    TIMER_Close(timer);

    timer->CMP = 0x0;

    timer->INTSTS = timer->INTSTS;
    timer->EINTSTS = timer->EINTSTS;
    CLK_SysTickDelay(1);

    if (IsTimerRegsCleared(timer) < 0)
        return -1;

    return 0;
}

const uint32_t au32OPModeSel[] =
{
    TIMER_ONESHOT_MODE,
    TIMER_PERIODIC_MODE,
    TIMER_TOGGLE_MODE,
    TIMER_CONTINUOUS_MODE,
};

const uint32_t au32CapEdgeSel[] =
{
    TIMER_CAPTURE_EVENT_FALLING,
    TIMER_CAPTURE_EVENT_RISING,
    TIMER_CAPTURE_EVENT_FALLING_RISING,
    TIMER_CAPTURE_EVENT_RISING_FALLING,
    0xFFFFFFFF, // reserved
    0xFFFFFFFF, // reserved
    TIMER_CAPTURE_EVENT_GET_LOW_PERIOD,
    TIMER_CAPTURE_EVENT_GET_HIGH_PERIOD,
};

const uint32_t au32CntEdgeSel[] =
{
    TIMER_COUNTER_EVENT_FALLING,
    TIMER_COUNTER_EVENT_RISING,
};

void MACRO_TIMER_SET_CMP_VALUE(void)
{
    volatile uint32_t i;

    if (TIMER_InitClock() != 0)
    {
        CU_FAIL("TIMER Init FAIL");
        return ;
    }

    /* Check TIMER_SET_CMP_VALUE */
    for (i = 0; i < 4; i++)
    {
        printf("TIMER_SET_CMP_VALUE (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[i]), 0);

        TIMER_SET_CMP_VALUE((TIMER_T *)TimerCh[i], 0x2);
        CU_ASSERT_EQUAL(TimerCh[i]->CMP, 0x2);

        TIMER_SET_CMP_VALUE((TIMER_T *)TimerCh[i], 0x5A5A5A);
        CU_ASSERT_EQUAL(TimerCh[i]->CMP, 0x5A5A5A);

        TIMER_SET_CMP_VALUE((TIMER_T *)TimerCh[i], 0xFFFFFF);
        CU_ASSERT_EQUAL(TimerCh[i]->CMP, 0xFFFFFF);
    }
}

void MACRO_TIMER_SET_PRESCALE_VALUE(void)
{
    volatile uint32_t i;

    if (TIMER_InitClock() != 0)
    {
        CU_FAIL("TIMER Init FAIL");
        return ;
    }

    /* Check TIMER_SET_PRESCALE_VALUE */
    for (i = 0; i < 4; i++)
    {
        printf("TIMER_SET_PRESCALE_VALUE (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[i]), 0);

        TIMER_SET_PRESCALE_VALUE((TIMER_T *)TimerCh[i], 0x2);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x2);

        TIMER_SET_PRESCALE_VALUE((TIMER_T *)TimerCh[i], 0x5A);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x5A);

        TIMER_SET_PRESCALE_VALUE((TIMER_T *)TimerCh[i], 0xFF);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0xFF);
    }
}

void API_TIMER_APIs(void)
{
    volatile uint32_t i, j;

    if (TIMER_InitClock() != 0)
    {
        CU_FAIL("TIMER Init FAIL");
        return ;
    }

    /* Check TIMER_Start, TIMER_Stop and TIMER_IS_ACTIVE */
    for (i = 0; i < 4; i++)
    {
        CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[i]), 0);

        TimerCh[i]->CMP = 0x00FFFFFF;
        TimerCh[i]->CTL = 0x18000000;
        CU_ASSERT_EQUAL(TIMER_IS_ACTIVE((TIMER_T *)TimerCh[i]), 0);

        TIMER_Start((TIMER_T *)TimerCh[i]);

        for (j = 0; j < SystemCoreClock / 1000000; j++)
            __NOP();

        CU_ASSERT_EQUAL(TIMER_IS_ACTIVE((TIMER_T *)TimerCh[i]), 1);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x5A000000);

        TIMER_Stop((TIMER_T *)TimerCh[i]);

        for (j = 0; j < SystemCoreClock / 1000000; j++)
            __NOP();

        CU_ASSERT_EQUAL(TIMER_IS_ACTIVE((TIMER_T *)TimerCh[i]), 0);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x18000000);
    }

    /* Check TIMER_SetTriggerSource, TIMER_TRGSEL_TIMEOUT_EVENT, TIMER_TRGSEL_CAPTURE_EVENT */
    for (i = 0; i < 4; i++)
    {
        TIMER_SetTriggerSource((TIMER_T *)TimerCh[i], TIMER_TRGSEL_CAPTURE_EVENT);
        CU_ASSERT_EQUAL(TimerCh[i]->TRGCTL & BIT0, 0x1);
        TIMER_SetTriggerSource((TIMER_T *)TimerCh[i], TIMER_TRGSEL_TIMEOUT_EVENT);
        CU_ASSERT_EQUAL(TimerCh[i]->TRGCTL & BIT0, 0x0);
    }

    /* Check TIMER_SetTriggerTarget, TIMER_TRG_TO_PWM, TIMER_TRG_TO_ADC, TIMER_TRG_TO_PDMA */
    for (i = 0; i < 4; i++)
    {
        TIMER_SetTriggerTarget((TIMER_T *)TimerCh[i], TIMER_TRG_TO_PWM);
        CU_ASSERT_EQUAL(TimerCh[i]->TRGCTL, BIT1);

        TIMER_SetTriggerTarget((TIMER_T *)TimerCh[i], TIMER_TRG_TO_EADC);
        CU_ASSERT_EQUAL(TimerCh[i]->TRGCTL, BIT2);

        TIMER_SetTriggerTarget((TIMER_T *)TimerCh[i], TIMER_TRG_TO_DAC);
        CU_ASSERT_EQUAL(TimerCh[i]->TRGCTL, BIT3);

        TIMER_SetTriggerTarget((TIMER_T *)TimerCh[i], TIMER_TRG_TO_PDMA);
        CU_ASSERT_EQUAL(TimerCh[i]->TRGCTL, BIT4);

        TIMER_SetTriggerTarget((TIMER_T *)TimerCh[i], (TIMER_TRG_TO_PWM | TIMER_TRG_TO_EADC | TIMER_TRG_TO_DAC | TIMER_TRG_TO_PDMA));
        CU_ASSERT_EQUAL(TimerCh[i]->TRGCTL, (BIT1 | BIT2 | BIT3 | BIT4));

        TIMER_SetTriggerTarget((TIMER_T *)TimerCh[i], 0);
        CU_ASSERT_EQUAL(TimerCh[i]->TRGCTL, 0);
    }

    for (i = 0; i < 4; i++)
    {
        CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[i]), 0);
    }

    /* Check TIMER_EnableFreqCounter, TIMER_DisableFreqCounter */
    TIMER_EnableFreqCounter(TIMER0, 0, 0, TRUE);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    CU_ASSERT_EQUAL(TIMER1->CMP, 0xFFFFFF);
    CU_ASSERT_EQUAL(TIMER1->EXTCTL, TIMER_EXTCTL_CAPIEN_Msk);
    CU_ASSERT_EQUAL(TIMER0->CTL, (TIMER_CTL_INTRGEN_Msk | TIMER_CTL_CNTEN_Msk | TIMER_CTL_EXTCNTEN_Msk | TIMER_CTL_ACTSTS_Msk));
    TIMER_EnableFreqCounter(TIMER2, 0, 0, TRUE);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    CU_ASSERT_EQUAL(TIMER3->CMP, 0xFFFFFF);
    CU_ASSERT_EQUAL(TIMER3->EXTCTL, TIMER_EXTCTL_CAPIEN_Msk);
    CU_ASSERT_EQUAL(TIMER2->CTL, (TIMER_CTL_INTRGEN_Msk | TIMER_CTL_CNTEN_Msk | TIMER_CTL_EXTCNTEN_Msk | TIMER_CTL_ACTSTS_Msk));
    TIMER_EnableFreqCounter(TIMER0, 1, 1, FALSE);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    CU_ASSERT_EQUAL(TIMER1->CMP, 0xFFFFFF);
    CU_ASSERT_EQUAL(TIMER1->EXTCTL, 0);
    CU_ASSERT_EQUAL(TIMER0->CTL, (TIMER_CTL_INTRGEN_Msk | TIMER_CTL_CNTEN_Msk | TIMER_CTL_EXTCNTEN_Msk | TIMER_CTL_ACTSTS_Msk));
    TIMER_EnableFreqCounter(TIMER2, 1, 1, FALSE);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    CU_ASSERT_EQUAL(TIMER3->CMP, 0xFFFFFF);
    CU_ASSERT_EQUAL(TIMER3->EXTCTL, 0);
    CU_ASSERT_EQUAL(TIMER2->CTL, (TIMER_CTL_INTRGEN_Msk | TIMER_CTL_CNTEN_Msk | TIMER_CTL_EXTCNTEN_Msk | TIMER_CTL_ACTSTS_Msk));

    for (i = 0; i < 4; i++)
    {
        TIMER_DisableFreqCounter((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL((TimerCh[i]->CTL & TIMER_CTL_INTRGEN_Msk), 0);
    }
}

void API_TIMER_CaptureDebounce(void)
{
    volatile uint32_t i;

    if (TIMER_InitClock() != 0)
    {
        CU_FAIL("TIMER Init FAIL");
        return ;
    }

    /* Check TIMER_EnableCaptureDebounce and TIMER_DisableCaptureDebounce */
    for (i = 0; i < 4; i++)
    {
        printf("TIMER_EnableCaptureDebounce (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[i]), 0);

        TIMER_EnableCaptureDebounce((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, 0x40);

        TIMER_DisableCaptureDebounce((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, 0x0);
    }

    /* Check TIMER_EnableCaptureInputNoiseFilter and TIMER_DisableCaptureInputNoiseFilter */
    for (i = 0; i < 4; i++)
    {
        printf("TIMER_EnableCaptureInputNoiseFilter (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearTimerRegs(TimerCh[i]), 0);

        TIMER_EnableCaptureInputNoiseFilter(TimerCh[i], 1, TIMER_CAPTURE_NOISE_FILTER_PCLK_DIV_2);
        CU_ASSERT_EQUAL(TimerCh[i]->CAPNF, (TIMER_CAPNF_CAPNFEN_Msk | (1 << TIMER_CAPNF_CAPNFCNT_Pos) | (1 << TIMER_CAPNF_CAPNFSEL_Pos)));

        TIMER_EnableCaptureInputNoiseFilter(TimerCh[i], 2, TIMER_CAPTURE_NOISE_FILTER_PCLK_DIV_4);
        CU_ASSERT_EQUAL(TimerCh[i]->CAPNF, (TIMER_CAPNF_CAPNFEN_Msk | (2 << TIMER_CAPNF_CAPNFCNT_Pos) | (2 << TIMER_CAPNF_CAPNFSEL_Pos)));

        TIMER_EnableCaptureInputNoiseFilter(TimerCh[i], 3, TIMER_CAPTURE_NOISE_FILTER_PCLK_DIV_8);
        CU_ASSERT_EQUAL(TimerCh[i]->CAPNF, (TIMER_CAPNF_CAPNFEN_Msk | (3 << TIMER_CAPNF_CAPNFCNT_Pos) | (3 << TIMER_CAPNF_CAPNFSEL_Pos)));

        TIMER_EnableCaptureInputNoiseFilter(TimerCh[i], 4, TIMER_CAPTURE_NOISE_FILTER_PCLK_DIV_16);
        CU_ASSERT_EQUAL(TimerCh[i]->CAPNF, (TIMER_CAPNF_CAPNFEN_Msk | (4 << TIMER_CAPNF_CAPNFCNT_Pos) | (4 << TIMER_CAPNF_CAPNFSEL_Pos)));

        TIMER_EnableCaptureInputNoiseFilter(TimerCh[i], 5, TIMER_CAPTURE_NOISE_FILTER_PCLK_DIV_32);
        CU_ASSERT_EQUAL(TimerCh[i]->CAPNF, (TIMER_CAPNF_CAPNFEN_Msk | (5 << TIMER_CAPNF_CAPNFCNT_Pos) | (5 << TIMER_CAPNF_CAPNFSEL_Pos)));

        TIMER_EnableCaptureInputNoiseFilter(TimerCh[i], 6, TIMER_CAPTURE_NOISE_FILTER_PCLK_DIV_64);
        CU_ASSERT_EQUAL(TimerCh[i]->CAPNF, (TIMER_CAPNF_CAPNFEN_Msk | (6 << TIMER_CAPNF_CAPNFCNT_Pos) | (6 << TIMER_CAPNF_CAPNFSEL_Pos)));

        TIMER_EnableCaptureInputNoiseFilter(TimerCh[i], 7, TIMER_CAPTURE_NOISE_FILTER_PCLK_DIV_128);
        CU_ASSERT_EQUAL(TimerCh[i]->CAPNF, (TIMER_CAPNF_CAPNFEN_Msk | (7 << TIMER_CAPNF_CAPNFCNT_Pos) | (7 << TIMER_CAPNF_CAPNFSEL_Pos)));

        TIMER_EnableCaptureInputNoiseFilter(TimerCh[i], 0, TIMER_CAPTURE_NOISE_FILTER_PCLK_DIV_1);
        CU_ASSERT_EQUAL(TimerCh[i]->CAPNF, (TIMER_CAPNF_CAPNFEN_Msk | (0 << TIMER_CAPNF_CAPNFCNT_Pos) | (0 << TIMER_CAPNF_CAPNFSEL_Pos)));

        TIMER_DisableCaptureInputNoiseFilter(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->CAPNF, 0x0);
    }
}

void API_TIMER_EventCounterDebounce(void)
{
    volatile uint32_t i;

    if (TIMER_InitClock() != 0)
    {
        CU_FAIL("TIMER Init FAIL");
        return ;
    }

    /* Check TIMER_EnableEventCounterDebounce and TIMER_DisableEventCounterDebounce */
    for (i = 0; i < 4; i++)
    {
        printf("TIMER_EnableEventCounterDebounce (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[i]), 0);

        TIMER_EnableEventCounterDebounce((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, 0x80);

        TIMER_DisableEventCounterDebounce((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, 0x0);
    }
}

void API_TIMER_OpenAndInterrupt(void)
{
    volatile uint32_t i, j, u32Timeout = 0;

    if (TIMER_InitClock() != 0)
    {
        CU_FAIL("TIMER Init FAIL");
        return ;
    }

    /* Check TIMER_Open, TIMER_EnableInt and TIMER_DisableInt*/
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < sizeof(au32OPModeSel) / sizeof(au32OPModeSel[0]); j++)
        {
            printf("OPModeSel %d (Ch-%d)\n", j, i);
            CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[i]), 0);

            TIMER_Open((TIMER_T *)TimerCh[i], au32OPModeSel[j], 0);
            CU_ASSERT_EQUAL(TimerCh[i]->CTL, (j << 27));

            TIMER_EnableInt((TIMER_T *)TimerCh[i]);
            CU_ASSERT_EQUAL(TimerCh[i]->CTL, ((1 << 29) | (j << 27)));

            TIMER_DisableInt((TIMER_T *)TimerCh[i]);
            CU_ASSERT_EQUAL(TimerCh[i]->CTL, (j << 27));

            TIMER_Close((TIMER_T *)TimerCh[i]);
            CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x0);
            CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, 0x0);
        }
    }
}

void API_TIMER_CaptureAndInterrupt(void)
{
    volatile uint32_t i, j, u32Timeout = 0;

    if (TIMER_InitClock() != 0)
    {
        CU_FAIL("TIMER Init FAIL");
        return ;
    }

    /* Check Capture Edge Macro - TIMER_EnableCapture, TIMER_DisableCapture, TIMER_EnableCaptureInt, TIMER_DisableCaptureInt */
    /* With TIMER_CAPTURE_FREE_COUNTING_MODE */
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < sizeof(au32CapEdgeSel) / sizeof(au32CapEdgeSel[0]); j++)
        {
            if ((j == 4) || (j == 5))
                continue;

            printf("CapEdgeSel %d (Ch-%d)\n", j, i);
            CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[i]), 0);

            TIMER_EnableCapture((TIMER_T *)TimerCh[i], TIMER_CAPTURE_FREE_COUNTING_MODE, au32CapEdgeSel[j]);
            TIMER_EnableCaptureInt((TIMER_T *)TimerCh[i]);
            CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, (0x20 | (0x8 | (j << TIMER_EXTCTL_CAPEDGE_Pos))));

            TIMER_DisableCapture((TIMER_T *)TimerCh[i]);
            CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, (0x20 | (j << TIMER_EXTCTL_CAPEDGE_Pos)));

            TIMER_DisableCaptureInt((TIMER_T *)TimerCh[i]);
            CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, (j << TIMER_EXTCTL_CAPEDGE_Pos));
        }
    }

    /* With TIMER_CAPTURE_COUNTER_RESET_MODE */
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < sizeof(au32CapEdgeSel) / 4; j++)
        {
            if ((j == 4) || (j == 5))
                continue;

            printf("CapEdgeSel %d (Ch-%d)\n", j, i);
            CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[i]), 0);

            TIMER_EnableCapture((TIMER_T *)TimerCh[i], TIMER_CAPTURE_COUNTER_RESET_MODE, au32CapEdgeSel[j]);
            CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, (0x18 | (j << TIMER_EXTCTL_CAPEDGE_Pos)));

            TIMER_DisableCapture((TIMER_T *)TimerCh[i]);
            CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, (0x10 | (j << TIMER_EXTCTL_CAPEDGE_Pos)));
        }
    }
}

void API_TIMER_EventCounter(void)
{
    volatile uint32_t i, j, u32Timeout = 0;

    if (TIMER_InitClock() != 0)
    {
        CU_FAIL("TIMER Init FAIL");
        return ;
    }

    /* Check Counter Edge Macro - TIMER_EnableEventCounter, TIMER_DisableEventCounter */
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < sizeof(au32CntEdgeSel) / 4; j++)
        {
            printf("CntEdgeSel %d (Ch-%d)\n", j, i);
            CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[i]), 0);

            TIMER_EnableEventCounter((TIMER_T *)TimerCh[i], au32CntEdgeSel[j]);
            CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, (j << 0));
            CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x01000000);

            TIMER_DisableEventCounter((TIMER_T *)TimerCh[i]);
            CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x0);
        }
    }

    /* Check TIMER_EventCounterSelect */
    for (i = 0; i < 4; i++)
    {
        CU_ASSERT_EQUAL(ClearTimerRegs(TimerCh[i]), 0);

        TIMER_EventCounterSelect(TimerCh[i], TIMER_EVENT_COUNTER_SOURCE_TMX);
        CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, TIMER_EVENT_COUNTER_SOURCE_TMX);
        TIMER_EventCounterSelect(TimerCh[i], TIMER_EVENT_COUNTER_SOURCE_USB);
        CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, TIMER_EVENT_COUNTER_SOURCE_USB);
        TIMER_EventCounterSelect(TimerCh[i], TIMER_EVENT_COUNTER_SOURCE_ACMP0);
        CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, TIMER_EVENT_COUNTER_SOURCE_ACMP0);
        TIMER_EventCounterSelect(TimerCh[i], TIMER_EVENT_COUNTER_SOURCE_ACMP1);
        CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, TIMER_EVENT_COUNTER_SOURCE_ACMP1);
        TIMER_EventCounterSelect(TimerCh[i], TIMER_EVENT_COUNTER_SOURCE_ACMP2);
        CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, TIMER_EVENT_COUNTER_SOURCE_ACMP2);
        TIMER_EventCounterSelect(TimerCh[i], TIMER_EVENT_COUNTER_SOURCE_ACMP3);
        CU_ASSERT_EQUAL(TimerCh[i]->EXTCTL, TIMER_EVENT_COUNTER_SOURCE_ACMP3);
    }

    /* With TIMER_SELECT_TOUT_PIN */
    for (i = 0; i < 4; i++)
    {
        TIMER_SELECT_TOUT_PIN((TIMER_T *)TimerCh[i], TIMER_TOUT_PIN_FROM_TMX_EXT);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL & BIT21, BIT21);

        TIMER_SELECT_TOUT_PIN((TIMER_T *)TimerCh[i], TIMER_TOUT_PIN_FROM_TMX);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL & BIT21, 0);
    }
}

void API_TIMER_Delay(void)
{
    volatile uint32_t i, u32TDR[3];

    if (TIMER_InitClock() != 0)
    {
        CU_FAIL("TIMER Init FAIL");
        return ;
    }

    /* Check TIMER_Delay */
    for (i = 0; i < 4; i++)
    {
        CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[i]), 0);

        if (i == 0)
        {
            TIMER1->CMP = 0xFFFFFF;
            TIMER1->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk | (12 - 1);

            TIMER1->CMP = 0xFFFFFF;
            TIMER_Delay((TIMER_T *)TimerCh[i], 1000);
            u32TDR[0] = TIMER_GetCounter(TIMER1);

            //            TIMER1->CMP = 0xFFFFFF;
            TIMER_ResetCounter(TIMER1);
            TIMER_Delay(TimerCh[i], 500);
            u32TDR[1] = TIMER_GetCounter(TIMER1);

            TIMER1->CMP = 0xFFFFFF;
            TIMER_Delay(TimerCh[i], 100);
            u32TDR[2] = TIMER_GetCounter(TIMER1);
            printf("TIMER[%d] [%d] [%d] [%d]\n",i, u32TDR[0], u32TDR[1], u32TDR[2]);
        }
        else
        {
            //            TIMER0->CMP = 0xFFFFFF;
            TIMER_ResetCounter(TIMER0);
            TIMER0->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk | (12 - 1);

            TIMER0->CMP = 0xFFFFFF;
            TIMER_Delay((TIMER_T *)TimerCh[i], 1000);
            u32TDR[0] = TIMER_GetCounter(TIMER0);

            TIMER0->CMP = 0xFFFFFF;
            TIMER_Delay(TimerCh[i], 500);
            u32TDR[1] = TIMER_GetCounter(TIMER0);

            TIMER0->CMP = 0xFFFFFF;
            TIMER_Delay(TimerCh[i], 100);
            u32TDR[2] = TIMER_GetCounter(TIMER0);
            printf("TIMER[%d] [%d] [%d] [%d]\n",i, u32TDR[0], u32TDR[1], u32TDR[2]);
        }

//        if (((u32TDR[0] > 1200) || (u32TDR[0] <= 1000)) ||
//                ((u32TDR[1] > 502500) || (u32TDR[1] <= 500000)) ||
//                ((u32TDR[2] > 1005000) || (u32TDR[2] <= 1000000)))
//        {
//            CU_FAIL("TIMER counter value FAIL");
//            break;
//        }
    }

    TIMER_Stop(TIMER0);
    TIMER_Stop(TIMER1);
    TIMER_Stop(TIMER2);
    TIMER_Stop(TIMER3);
}

void API_TIMER_Wakeup(void)
{
    volatile uint32_t i, u32TDR;

    if (TIMER_InitClock() != 0)
    {
        CU_FAIL("TIMER Init FAIL");
        return ;
    }

    /* Check TIMER_EnableWakeup and TIMER_DisableWakeup */
    for (i = 0; i < 4; i++)
    {
        printf("TIMER_EnableWakeup (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[i]), 0);

        TIMER_EnableWakeup((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x00800000);

        TIMER_DisableWakeup((TIMER_T *)TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x0);
    }

    /* Enable LIRC */
    /* Enable Internal RC 32KHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);
    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_LIRC, NULL);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_LIRC, NULL);
    CLK_SetModuleClock(TMR2_MODULE, CLK_TMRSEL_TMR2SEL_LIRC, NULL);
    CLK_SetModuleClock(TMR3_MODULE, CLK_TMRSEL_TMR3SEL_LIRC, NULL);
    CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[0]), 0);

    NVIC_EnableIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(TIMER1_IRQn);
    NVIC_EnableIRQ(TIMER2_IRQn);
    NVIC_EnableIRQ(TIMER3_IRQn);

    /* Check TIMER_GetIntFlag, TIMER_ClearIntFlag, TIMER_GetWakeupFlag, TIMER_ClearWakeupFlag and TIMER_GetCounter */
    for (i = 0; i < 4; i++)
    {
        TimerCh[i]->CMP = 10;
        TimerCh[i]->CTL = TIMER_CONTINUOUS_MODE | TIMER_CTL_INTEN_Msk | TIMER_CTL_WKEN_Msk;

        g_u8IsTMRWKF[i] = 0;

        TIMER_Start(TimerCh[i]);

        PMC_PowerDown();

        while (g_u8IsTMRWKF[i] == 0);

        u32TDR = TIMER_GetCounter(TimerCh[i]);

        if (u32TDR > 15)
        {
            CU_FAIL("TIMER counter value FAIL");
            break;
        }

        printf(" [%d]", u32TDR);

        TIMER_Stop(TimerCh[i]);
    }

    NVIC_DisableIRQ(TIMER0_IRQn);
    NVIC_DisableIRQ(TIMER1_IRQn);
    NVIC_DisableIRQ(TIMER2_IRQn);
    NVIC_DisableIRQ(TIMER3_IRQn);
    TIMER_Stop(TIMER0);
    TIMER_Stop(TIMER1);
    TIMER_Stop(TIMER2);
    TIMER_Stop(TIMER3);
}

void API_TIMER_GetModuleClock(void)
{
    volatile uint32_t i, j, u32Timeout = 0;

    if (TIMER_InitClock() != 0)
    {
        CU_FAIL("TIMER Init FAIL");
        return ;
    }

    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Check TIMER_GetModuleClock */
    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HXT, NULL);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_HXT, NULL);
    CLK_SetModuleClock(TMR2_MODULE, CLK_TMRSEL_TMR2SEL_HXT, NULL);
    CLK_SetModuleClock(TMR3_MODULE, CLK_TMRSEL_TMR3SEL_HXT, NULL);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER0), __HXT);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER1), __HXT);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER2), __HXT);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER3), __HXT);
    printf(" [%d]", TIMER_GetModuleClock(TIMER0));

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_EXT, NULL);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_EXT, NULL);
    CLK_SetModuleClock(TMR2_MODULE, CLK_TMRSEL_TMR2SEL_EXT, NULL);
    CLK_SetModuleClock(TMR3_MODULE, CLK_TMRSEL_TMR3SEL_EXT, NULL);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER0), 0UL);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER1), 0UL);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER2), 0UL);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER3), 0UL);
    printf(" [%d]", TIMER_GetModuleClock(TIMER1));

    /* r5464, Trim Filter Behavior */
    SYS->HIRC48MCFCTL = 0x00009999;
    CLK_EnableXtalRC(CLK_SRCCTL_HIRC48MEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRC48MSTB_Msk);

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC48M_DIV4, NULL);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_HIRC48M_DIV4, NULL);
    CLK_SetModuleClock(TMR2_MODULE, CLK_TMRSEL_TMR2SEL_HIRC48M_DIV4, NULL);
    CLK_SetModuleClock(TMR3_MODULE, CLK_TMRSEL_TMR3SEL_HIRC48M_DIV4, NULL);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER0), __HIRC48M / 4);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER1), __HIRC48M / 4);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER2), __HIRC48M / 4);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER3), __HIRC48M / 4);
    printf(" [%d]", TIMER_GetModuleClock(TIMER2));

    /* Bypass LXT Stable Count */
    SYS->ALTCTL1 |= SYS_ALTCTL1_LXTFSTB_Msk;
    CLK_EnableXtalRC(CLK_SRCCTL_LXTEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_LXT, NULL);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_LXT, NULL);
    CLK_SetModuleClock(TMR2_MODULE, CLK_TMRSEL_TMR2SEL_LXT, NULL);
    CLK_SetModuleClock(TMR3_MODULE, CLK_TMRSEL_TMR3SEL_LXT, NULL);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER0), __LXT);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER1), __LXT);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER2), __LXT);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER3), __LXT);
    printf(" [%d]", TIMER_GetModuleClock(TIMER0));

    CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_LIRC, NULL);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_LIRC, NULL);
    CLK_SetModuleClock(TMR2_MODULE, CLK_TMRSEL_TMR2SEL_LIRC, NULL);
    CLK_SetModuleClock(TMR3_MODULE, CLK_TMRSEL_TMR3SEL_LIRC, NULL);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER0), __LIRC);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER1), __LIRC);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER2), __LIRC);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER3), __LIRC);
    printf(" [%d]", TIMER_GetModuleClock(TIMER1));

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC, NULL);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_HIRC, NULL);
    CLK_SetModuleClock(TMR2_MODULE, CLK_TMRSEL_TMR2SEL_HIRC, NULL);
    CLK_SetModuleClock(TMR3_MODULE, CLK_TMRSEL_TMR3SEL_HIRC, NULL);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER0), __HIRC);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER1), __HIRC);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER2), __HIRC);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER3), __HIRC);
    printf(" [%d]", TIMER_GetModuleClock(TIMER2));

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_PCLK1, NULL);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_PCLK1, NULL);
    CLK_SetModuleClock(TMR2_MODULE, CLK_TMRSEL_TMR2SEL_PCLK3, NULL);
    CLK_SetModuleClock(TMR3_MODULE, CLK_TMRSEL_TMR3SEL_PCLK3, NULL);
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER0), CLK_GetPCLK1Freq());
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER1), CLK_GetPCLK1Freq());
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER2), CLK_GetPCLK3Freq());
    CU_ASSERT_EQUAL(TIMER_GetModuleClock(TIMER3), CLK_GetPCLK3Freq());
    printf(" [%d]", TIMER_GetModuleClock(TIMER3));
}

void API_TIMER_CaptureStatus(void)
{
    volatile uint32_t i, u32TDR, u32Timeout;

    if (TIMER_InitClock() != 0)
    {
        CU_FAIL("TIMER Init FAIL");
        return ;
    }

    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_PCLK1, NULL);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_PCLK1, NULL);
    CLK_SetModuleClock(TMR2_MODULE, CLK_TMRSEL_TMR2SEL_PCLK3, NULL);
    CLK_SetModuleClock(TMR3_MODULE, CLK_TMRSEL_TMR3SEL_PCLK3, NULL);
    /* Enable internal self-test mode */
    ENABLE_GCR_SELFTEST;
    /* Set TM0_EXT ~ TM3_EXT capture pins, PA.11, PA.10, PA.9, PA.8 as GPIO output pins */
    GPIO_SetMode(PA, BIT11 | BIT10 | BIT9 | BIT8, GPIO_MODE_OUTPUT);

    /* Check TIMER_StartCapture, TIMER_GetCaptureIntFlag, TIMER_ClearCaptureIntFlag and TIMER_GetCaptureData */

    /* For TIMER0 CAP*/
    PA11 = 1;

    TIMER0->CMP = 0xFFFFFF;
    TIMER0->CTL = TIMER_CONTINUOUS_MODE | TIMER_CTL_CNTEN_Msk;
    TIMER_StartCapture(TIMER0);
    CU_ASSERT_EQUAL((TIMER0->EXTCTL & TIMER_EXTCTL_CAPEN_Msk), TIMER_EXTCTL_CAPEN_Msk);
    TIMER_EnableCapture(TIMER0, TIMER_CAPTURE_FREE_COUNTING_MODE, TIMER_CAPTURE_EVENT_FALLING_RISING);

    u32Timeout = 0;

    while (TIMER_GetCaptureIntFlag(TIMER0) == 1)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER0 CAP 01 FAIL");
            goto exit_test;
        }
    }

    TIMER_ClearCaptureIntFlag(TIMER0);

    TIMER0->CMP = 0xFFFFFF;
    __NOP();
    __NOP();
    PA11 = 0;

    u32Timeout = 0;

    while (TIMER_GetCaptureIntFlag(TIMER0) == 0)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER0 CAP 02 FAIL");
            goto exit_test;
        }
    }

    CU_ASSERT_EQUAL(TIMER_GetCaptureIntFlagOV(TIMER0), 0x0);

    printf(" [%d]", TIMER_GetCaptureData(TIMER0));
    CU_ASSERT_EQUAL(TIMER_GetCaptureData(TIMER0), 0x0);
    u32Timeout = 0;
    PA11 = 1;

    while (TIMER_GetCaptureIntFlagOV(TIMER0) == 0)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER0 CAP 04 FAIL");
            goto exit_test;
        }
    }

    TIMER_ClearCaptureIntFlag(TIMER0);
    u32Timeout = 0;

    while (TIMER_GetCaptureIntFlag(TIMER0) == 1)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER0 CAP 03 FAIL");
            goto exit_test;
        }
    }

    CU_ASSERT_EQUAL(TIMER_GetCaptureIntFlagOV(TIMER0), 0x0);

    /* For TIMER1 CAP */
    PA10 = 1;

    TIMER1->CMP = 0xFFFFFF;
    TIMER1->CTL = TIMER_CONTINUOUS_MODE | TIMER_CTL_CNTEN_Msk;
    TIMER_StartCapture(TIMER1);
    CU_ASSERT_EQUAL((TIMER1->EXTCTL & TIMER_EXTCTL_CAPEN_Msk), TIMER_EXTCTL_CAPEN_Msk);
    TIMER_EnableCapture(TIMER1, TIMER_CAPTURE_FREE_COUNTING_MODE, TIMER_CAPTURE_EVENT_FALLING_RISING);

    u32Timeout = 0;

    while (TIMER_GetCaptureIntFlag(TIMER1) == 1)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER1 CAP 01 FAIL");
            goto exit_test;
        }
    }

    TIMER_ClearCaptureIntFlag(TIMER1);

    TIMER1->CMP = 0xFFFFFF;
    __NOP();
    __NOP();
    PA10 = 0;

    u32Timeout = 0;

    while (TIMER_GetCaptureIntFlag(TIMER1) == 0)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER1 CAP 02 FAIL");
            goto exit_test;
        }
    }

    CU_ASSERT_EQUAL(TIMER_GetCaptureIntFlagOV(TIMER1), 0x0);
    printf(" [%d]", TIMER_GetCaptureData(TIMER1));
    CU_ASSERT_EQUAL(TIMER_GetCaptureData(TIMER1), 0x0);
    u32Timeout = 0;
    PA10 = 1;

    while (TIMER_GetCaptureIntFlagOV(TIMER1) == 0)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER1 CAP 04 FAIL");
            goto exit_test;
        }
    }

    TIMER_ClearCaptureIntFlag(TIMER1);
    u32Timeout = 0;

    while (TIMER_GetCaptureIntFlag(TIMER1) == 1)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER1 CAP 03 FAIL");
            goto exit_test;
        }
    }

    CU_ASSERT_EQUAL(TIMER_GetCaptureIntFlagOV(TIMER1), 0x0);

    /* For TIMER2 CAP */
    PA9 = 1;

    TIMER2->CMP = 0xFFFFFF;
    TIMER2->CTL = TIMER_CONTINUOUS_MODE | TIMER_CTL_CNTEN_Msk;
    TIMER_StartCapture(TIMER2);
    CU_ASSERT_EQUAL((TIMER2->EXTCTL & TIMER_EXTCTL_CAPEN_Msk), TIMER_EXTCTL_CAPEN_Msk);
    TIMER_EnableCapture(TIMER2, TIMER_CAPTURE_FREE_COUNTING_MODE, TIMER_CAPTURE_EVENT_FALLING_RISING);

    u32Timeout = 0;

    while (TIMER_GetCaptureIntFlag(TIMER2) == 1)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER2 CAP 01 FAIL");
            goto exit_test;
        }
    }

    TIMER_ClearCaptureIntFlag(TIMER2);

    TIMER2->CMP = 0xFFFFFF;
    __NOP();
    __NOP();
    PA9 = 0;

    u32Timeout = 0;

    while (TIMER_GetCaptureIntFlag(TIMER2) == 0)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER2 CAP 02 FAIL");
            goto exit_test;
        }
    }

    CU_ASSERT_EQUAL(TIMER_GetCaptureIntFlagOV(TIMER2), 0x0);
    printf(" [%d]", TIMER_GetCaptureData(TIMER2));
    CU_ASSERT_EQUAL(TIMER_GetCaptureData(TIMER2), 0x0);
    u32Timeout = 0;
    PA9 = 1;

    while (TIMER_GetCaptureIntFlagOV(TIMER2) == 0)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER2 CAP 04 FAIL");
            goto exit_test;
        }
    }

    TIMER_ClearCaptureIntFlag(TIMER2);
    u32Timeout = 0;

    while (TIMER_GetCaptureIntFlag(TIMER2) == 1)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER2 CAP 03 FAIL");
            goto exit_test;
        }
    }

    CU_ASSERT_EQUAL(TIMER_GetCaptureIntFlagOV(TIMER2), 0x0);

    /* For TIMER3 CAP */
    PA8 = 1;

    TIMER3->CMP = 0xFFFFFF;
    TIMER3->CTL = TIMER_CONTINUOUS_MODE | TIMER_CTL_CNTEN_Msk;
    TIMER_StartCapture(TIMER3);
    CU_ASSERT_EQUAL((TIMER3->EXTCTL & TIMER_EXTCTL_CAPEN_Msk), TIMER_EXTCTL_CAPEN_Msk);
    TIMER_EnableCapture(TIMER3, TIMER_CAPTURE_FREE_COUNTING_MODE, TIMER_CAPTURE_EVENT_FALLING_RISING);

    u32Timeout = 0;

    while (TIMER_GetCaptureIntFlag(TIMER3) == 1)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER3 CAP 01 FAIL");
            goto exit_test;
        }
    }

    TIMER_ClearCaptureIntFlag(TIMER3);

    TIMER3->CMP = 0xFFFFFF;
    __NOP();
    __NOP();
    PA8 = 0;

    u32Timeout = 0;

    while (TIMER_GetCaptureIntFlag(TIMER3) == 0)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER3 CAP 02 FAIL");
            goto exit_test;
        }
    }

    CU_ASSERT_EQUAL(TIMER_GetCaptureIntFlagOV(TIMER3), 0x0);
    printf(" [%d]", TIMER_GetCaptureData(TIMER3));
    CU_ASSERT_EQUAL(TIMER_GetCaptureData(TIMER3), 0x0);
    u32Timeout = 0;
    PA8 = 1;

    while (TIMER_GetCaptureIntFlagOV(TIMER3) == 0)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER3 CAP 04 FAIL");
            goto exit_test;
        }
    }

    TIMER_ClearCaptureIntFlag(TIMER3);
    u32Timeout = 0;

    while (TIMER_GetCaptureIntFlag(TIMER3) == 1)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("TIMER3 CAP 03 FAIL");
            goto exit_test;
        }
    }

    CU_ASSERT_EQUAL(TIMER_GetCaptureIntFlagOV(TIMER3), 0x0);

    /* Check TIMER_StopCapture */
    TIMER_StopCapture(TIMER0);
    CU_ASSERT_EQUAL((TIMER0->EXTCTL & TIMER_EXTCTL_CAPEN_Msk), 0x0);
    TIMER_StopCapture(TIMER1);
    CU_ASSERT_EQUAL((TIMER1->EXTCTL & TIMER_EXTCTL_CAPEN_Msk), 0x0);
    TIMER_StopCapture(TIMER2);
    CU_ASSERT_EQUAL((TIMER2->EXTCTL & TIMER_EXTCTL_CAPEN_Msk), 0x0);
    TIMER_StopCapture(TIMER3);
    CU_ASSERT_EQUAL((TIMER3->EXTCTL & TIMER_EXTCTL_CAPEN_Msk), 0x0);

exit_test:
    TIMER_Stop(TIMER0);
    TIMER_Stop(TIMER1);
    TIMER_Stop(TIMER2);
    TIMER_Stop(TIMER3);

    /* Disable internal self-test mode */
    DISABLE_GCR_SELFTEST;
}

void Const_Test(void)
{
    CU_ASSERT_EQUAL(TIMER_ONESHOT_MODE, (0UL << TIMER_CTL_OPMODE_Pos));
    CU_ASSERT_EQUAL(TIMER_PERIODIC_MODE, (1UL << TIMER_CTL_OPMODE_Pos));
    CU_ASSERT_EQUAL(TIMER_TOGGLE_MODE, (2UL << TIMER_CTL_OPMODE_Pos));
    CU_ASSERT_EQUAL(TIMER_CONTINUOUS_MODE, (3UL << TIMER_CTL_OPMODE_Pos));

    CU_ASSERT_EQUAL(TIMER_TOUT_PIN_FROM_TMX, (0UL << TIMER_CTL_TGLPINSEL_Pos));
    CU_ASSERT_EQUAL(TIMER_TOUT_PIN_FROM_TMX_EXT, (1UL << TIMER_CTL_TGLPINSEL_Pos));

    CU_ASSERT_EQUAL(TIMER_COUNTER_EVENT_FALLING, (0UL << TIMER_EXTCTL_CNTPHASE_Pos));
    CU_ASSERT_EQUAL(TIMER_COUNTER_EVENT_RISING, (1UL << TIMER_EXTCTL_CNTPHASE_Pos));

    CU_ASSERT_EQUAL(TIMER_CAPTURE_FREE_COUNTING_MODE, (0UL << TIMER_EXTCTL_CAPFUNCS_Pos));
    CU_ASSERT_EQUAL(TIMER_CAPTURE_COUNTER_RESET_MODE, (1UL << TIMER_EXTCTL_CAPFUNCS_Pos));

    CU_ASSERT_EQUAL(TIMER_CAPTURE_EVENT_FALLING, (0UL << TIMER_EXTCTL_CAPEDGE_Pos));
    CU_ASSERT_EQUAL(TIMER_CAPTURE_EVENT_RISING, (1UL << TIMER_EXTCTL_CAPEDGE_Pos));
    CU_ASSERT_EQUAL(TIMER_CAPTURE_EVENT_FALLING_RISING, (2UL << TIMER_EXTCTL_CAPEDGE_Pos));
    CU_ASSERT_EQUAL(TIMER_CAPTURE_EVENT_RISING_FALLING, (3UL << TIMER_EXTCTL_CAPEDGE_Pos));
    CU_ASSERT_EQUAL(TIMER_CAPTURE_EVENT_GET_LOW_PERIOD, (6UL << TIMER_EXTCTL_CAPEDGE_Pos));
    CU_ASSERT_EQUAL(TIMER_CAPTURE_EVENT_GET_HIGH_PERIOD, (7UL << TIMER_EXTCTL_CAPEDGE_Pos));

    CU_ASSERT_EQUAL(TIMER_TRGSRC_TIMEOUT_EVENT, (0UL << TIMER_TRGCTL_TRGSSEL_Pos));
    CU_ASSERT_EQUAL(TIMER_TRGSRC_CAPTURE_EVENT, (1UL << TIMER_TRGCTL_TRGSSEL_Pos));

    CU_ASSERT_EQUAL(TIMER_CAPSRC_TX_EXT, (0UL << TIMER_CTL_CAPSRC_Pos));
    CU_ASSERT_EQUAL(TIMER_CAPSRC_INTERNAL, (1UL << TIMER_CTL_CAPSRC_Pos));

    CU_ASSERT_EQUAL(TIMER_INTERCAPSEL_ACMP0, (0UL << TIMER_EXTCTL_ICAPSEL_Pos));
    CU_ASSERT_EQUAL(TIMER_INTERCAPSEL_ACMP1, (1UL << TIMER_EXTCTL_ICAPSEL_Pos));
    CU_ASSERT_EQUAL(TIMER_INTERCAPSEL_HXT, (2UL << TIMER_EXTCTL_ICAPSEL_Pos));
    CU_ASSERT_EQUAL(TIMER_INTERCAPSEL_LXT, (3UL << TIMER_EXTCTL_ICAPSEL_Pos));
    CU_ASSERT_EQUAL(TIMER_INTERCAPSEL_HIRC, (4UL << TIMER_EXTCTL_ICAPSEL_Pos));
    CU_ASSERT_EQUAL(TIMER_INTERCAPSEL_LIRC, (5UL << TIMER_EXTCTL_ICAPSEL_Pos));

    CU_ASSERT_EQUAL(TIMER_TRG_TO_PWM, (TIMER_TRGCTL_TRGPWM_Msk));
    CU_ASSERT_EQUAL(TIMER_TRG_TO_EADC, (TIMER_TRGCTL_TRGEADC_Msk));
    CU_ASSERT_EQUAL(TIMER_TRG_TO_DAC, (TIMER_TRGCTL_TRGDAC_Msk));
    CU_ASSERT_EQUAL(TIMER_TRG_TO_PDMA, (TIMER_TRGCTL_TRGPDMA_Msk));

    CU_ASSERT_EQUAL(TIMER_CMP_MAX_VALUE, (0xFFFFFFUL));
    CU_PASS();
}

void Macro_Test(void)
{
    if (TIMER_InitClock() != 0)
    {
        CU_FAIL("TIMER Init FAIL");
        return ;
    }

    TIMER_SET_CMP_VALUE(TIMER0, 2);
    CU_ASSERT_EQUAL(TIMER0->CMP, 2);
    TIMER_SET_CMP_VALUE(TIMER1, 0x5a5a5a);
    CU_ASSERT_EQUAL(TIMER1->CMP, 0x5a5a5a);
    TIMER_SET_CMP_VALUE(TIMER2, 0xa5a5a5);
    CU_ASSERT_EQUAL(TIMER2->CMP, 0xa5a5a5);
    TIMER_SET_CMP_VALUE(TIMER3, 0xffffff);
    CU_ASSERT_EQUAL(TIMER3->CMP, 0xffffff);

    TIMER_SET_PRESCALE_VALUE(TIMER0, 0);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_PSC_Msk, 0);
    TIMER_SET_PRESCALE_VALUE(TIMER1, 0xff);
    CU_ASSERT_EQUAL(TIMER1->CTL & TIMER_CTL_PSC_Msk, 0xff);

    CU_ASSERT_EQUAL(TIMER_IS_ACTIVE(TIMER0), (TIMER0->CTL & TIMER_CTL_ACTSTS_Msk) ? 1 : 0);

    TIMER_SELECT_TOUT_PIN(TIMER0, TIMER_TOUT_PIN_FROM_TX);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_TGLPINSEL_Msk, TIMER_TOUT_PIN_FROM_TX);
    TIMER_SELECT_TOUT_PIN(TIMER1, TIMER_TOUT_PIN_FROM_TMX_EXT);
    CU_ASSERT_EQUAL(TIMER1->CTL & TIMER_CTL_TGLPINSEL_Msk, TIMER_TOUT_PIN_FROM_TMX_EXT);

    TIMER_SET_OPMODE(TIMER0, TIMER_ONESHOT_MODE);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_OPMODE_Msk, TIMER_ONESHOT_MODE);
    TIMER_SET_OPMODE(TIMER1, TIMER_PERIODIC_MODE);
    CU_ASSERT_EQUAL(TIMER1->CTL & TIMER_CTL_OPMODE_Msk, TIMER_PERIODIC_MODE);
    TIMER_SET_OPMODE(TIMER2, TIMER_TOGGLE_MODE);
    CU_ASSERT_EQUAL(TIMER2->CTL & TIMER_CTL_OPMODE_Msk, TIMER_TOGGLE_MODE);
    TIMER_SET_OPMODE(TIMER3, TIMER_CONTINUOUS_MODE);
    CU_ASSERT_EQUAL(TIMER3->CTL & TIMER_CTL_OPMODE_Msk, TIMER_CONTINUOUS_MODE);

    /* Set TIMER0 compare value to large value for One-shot mode
       in order to keep CNTEN to 1 when check it. */
    TIMER0->CTL = TIMER_ONESHOT_MODE;
    TIMER_SET_CMP_VALUE(TIMER0, 0xffffff);
    TIMER_Start(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_CNTEN_Msk, TIMER_CTL_CNTEN_Msk);

    TIMER_Stop(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_CNTEN_Msk, 0);

    TIMER_EnableWakeup(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_WKEN_Msk, TIMER_CTL_WKEN_Msk);

    TIMER_DisableWakeup(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_WKEN_Msk, 0);

    TIMER_EnableCaptureDebounce(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPDBEN_Msk, TIMER_EXTCTL_CAPDBEN_Msk);

    TIMER_DisableCaptureDebounce(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPDBEN_Msk, 0);

    TIMER_EnableEventCounterDebounce(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CNTDBEN_Msk, TIMER_EXTCTL_CNTDBEN_Msk);

    TIMER_DisableEventCounterDebounce(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CNTDBEN_Msk, 0);

    TIMER_EnableInt(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_INTEN_Msk, TIMER_CTL_INTEN_Msk);

    TIMER_DisableInt(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_INTEN_Msk, 0);

    TIMER_EnableCaptureInt(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPIEN_Msk, TIMER_EXTCTL_CAPIEN_Msk);

    TIMER_DisableCaptureInt(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPIEN_Msk, 0);

    CU_ASSERT_EQUAL(TIMER_GetIntFlag(TIMER0), (TIMER0->INTSTS & TIMER_INTSTS_TIF_Msk) ? 1 : 0);

    TIMER_ClearIntFlag(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->INTSTS & TIMER_INTSTS_TIF_Msk, 0);

    CU_ASSERT_EQUAL(TIMER_GetCaptureIntFlag(TIMER0), TIMER0->EINTSTS);

    TIMER_ClearCaptureIntFlag(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->EINTSTS & TIMER_EINTSTS_CAPIF_Msk, 0);

    CU_ASSERT_EQUAL(TIMER_GetWakeupFlag(TIMER0), (TIMER0->INTSTS & TIMER_INTSTS_TWKF_Msk) ? 1 : 0);

    TIMER_ClearWakeupFlag(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->INTSTS & TIMER_INTSTS_TWKF_Msk, 0);

    CU_ASSERT_EQUAL(TIMER_GetCaptureData(TIMER0), TIMER0->CAP);

    CU_ASSERT_EQUAL(TIMER_GetCounter(TIMER0), TIMER0->CNT);

    CU_PASS();
}

void Func_Test(void)
{
    uint32_t tmp;
    uint32_t timerClkSrc, targetFreq, finalFreq;

    if (TIMER_InitClock() != 0)
    {
        CU_FAIL("TIMER Init FAIL");
        return ;
    }

    timerClkSrc = TIMER_GetModuleClock(TIMER0);

    targetFreq = timerClkSrc / 2;
    finalFreq = TIMER_Open(TIMER0, TIMER_ONESHOT_MODE, targetFreq);
    CU_ASSERT_EQUAL(finalFreq, targetFreq);
    CU_ASSERT_EQUAL(timerClkSrc / TIMER0->CMP / (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1), targetFreq);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_OPMODE_Msk, TIMER_ONESHOT_MODE);

    targetFreq = timerClkSrc / 3;
    finalFreq = TIMER_Open(TIMER0, TIMER_ONESHOT_MODE, targetFreq);
    CU_ASSERT_EQUAL(finalFreq, targetFreq);
    CU_ASSERT_EQUAL(timerClkSrc / TIMER0->CMP / (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1), targetFreq);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_OPMODE_Msk, TIMER_ONESHOT_MODE);

    targetFreq = timerClkSrc / 6;
    finalFreq = TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, targetFreq);
    CU_ASSERT_EQUAL(finalFreq, targetFreq);
    CU_ASSERT_EQUAL(timerClkSrc / TIMER0->CMP / (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1), targetFreq);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_OPMODE_Msk, TIMER_PERIODIC_MODE);

    targetFreq = timerClkSrc / 8;
    finalFreq = TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, targetFreq);
    CU_ASSERT_EQUAL(finalFreq, targetFreq);
    CU_ASSERT_EQUAL(timerClkSrc / TIMER0->CMP / (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1), targetFreq);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_OPMODE_Msk, TIMER_PERIODIC_MODE);

    targetFreq = 11;
    finalFreq = TIMER_Open(TIMER0, TIMER_TOGGLE_MODE, targetFreq);
    CU_ASSERT_EQUAL(finalFreq, targetFreq);
    CU_ASSERT_EQUAL(timerClkSrc / TIMER0->CMP / (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1), targetFreq);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_OPMODE_Msk, TIMER_TOGGLE_MODE);

    targetFreq = 7;
    finalFreq = TIMER_Open(TIMER0, TIMER_TOGGLE_MODE, targetFreq);
    CU_ASSERT_EQUAL(finalFreq, targetFreq);
    CU_ASSERT_EQUAL(timerClkSrc / TIMER0->CMP / (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1), targetFreq);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_OPMODE_Msk, TIMER_TOGGLE_MODE);

    targetFreq = 3;
    finalFreq = TIMER_Open(TIMER0, TIMER_CONTINUOUS_MODE, targetFreq);
    CU_ASSERT_EQUAL(finalFreq, targetFreq);
    CU_ASSERT_EQUAL(timerClkSrc / TIMER0->CMP / (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1), targetFreq);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_OPMODE_Msk, TIMER_CONTINUOUS_MODE);

    targetFreq = 1;
    finalFreq = TIMER_Open(TIMER0, TIMER_CONTINUOUS_MODE, targetFreq);
    CU_ASSERT_EQUAL(finalFreq, targetFreq);
    CU_ASSERT_EQUAL(timerClkSrc / TIMER0->CMP / (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1), targetFreq);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_OPMODE_Msk, TIMER_CONTINUOUS_MODE);

    /* Case 1: target frequency > (timer clock source / 2) --> return maximum working frequency */
    targetFreq = (timerClkSrc / 2) + 1;
    finalFreq = TIMER_Open(TIMER0, TIMER_ONESHOT_MODE, targetFreq);
    CU_ASSERT_EQUAL(finalFreq, timerClkSrc / 2);
    CU_ASSERT_EQUAL(timerClkSrc / TIMER0->CMP / (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1), timerClkSrc / 2);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_OPMODE_Msk, TIMER_ONESHOT_MODE);

    /* Case 2: timer clock source CANNOT to be divisible by target frequency --> return nearest frequency */
    targetFreq = (timerClkSrc / 2) - 1;
    finalFreq = TIMER_Open(TIMER0, TIMER_ONESHOT_MODE, targetFreq);
    CU_ASSERT_EQUAL(finalFreq, timerClkSrc / 2);
    CU_ASSERT_EQUAL(timerClkSrc / TIMER0->CMP / (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1), timerClkSrc / 2);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_OPMODE_Msk, TIMER_ONESHOT_MODE);

    TIMER_Close(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->CTL, 0);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL, 0);

//    TIMER_Delay(TIMER0, 1000000);
//    CU_ASSERT_EQUAL(TIMER0->CMP * (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1) / (timerClkSrc / 1000000), 1000000);
//    TIMER_Delay(TIMER0, 100000);
//    CU_ASSERT_EQUAL(TIMER0->CMP * (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1) / (timerClkSrc / 1000000), 100000);
//    TIMER_Delay(TIMER0, 10000);
//    CU_ASSERT_EQUAL(TIMER0->CMP * (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1) / (timerClkSrc / 1000000), 10000);
    TIMER_Delay(TIMER0, 1000);
    CU_ASSERT_EQUAL(TIMER0->CMP * (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1) / (timerClkSrc / 1000000), 1000);
    TIMER_Delay(TIMER0, 100);
    CU_ASSERT_EQUAL(TIMER0->CMP * (((TIMER0->CTL & TIMER_CTL_PSC_Msk) >> TIMER_CTL_PSC_Pos) + 1) / (timerClkSrc / 1000000), 100);

    TIMER_EnableCapture(TIMER0, TIMER_CAPTURE_FREE_COUNTING_MODE, TIMER_CAPTURE_EVENT_FALLING);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPFUNCS_Msk, TIMER_CAPTURE_FREE_COUNTING_MODE);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPEDGE_Msk, TIMER_CAPTURE_EVENT_FALLING);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPEN_Msk, TIMER_EXTCTL_CAPEN_Msk);

    TIMER_EnableCapture(TIMER0, TIMER_CAPTURE_FREE_COUNTING_MODE, TIMER_CAPTURE_EVENT_RISING);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPFUNCS_Msk, TIMER_CAPTURE_FREE_COUNTING_MODE);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPEDGE_Msk, TIMER_CAPTURE_EVENT_RISING);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPEN_Msk, TIMER_EXTCTL_CAPEN_Msk);

    TIMER_EnableCapture(TIMER0, TIMER_CAPTURE_COUNTER_RESET_MODE, TIMER_CAPTURE_EVENT_FALLING_RISING);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPFUNCS_Msk, TIMER_CAPTURE_COUNTER_RESET_MODE);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPEDGE_Msk, TIMER_CAPTURE_EVENT_FALLING_RISING);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPEN_Msk, TIMER_EXTCTL_CAPEN_Msk);

    TIMER_EnableCapture(TIMER0, TIMER_CAPTURE_COUNTER_RESET_MODE, TIMER_CAPTURE_EVENT_RISING_FALLING);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPFUNCS_Msk, TIMER_CAPTURE_COUNTER_RESET_MODE);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPEDGE_Msk, TIMER_CAPTURE_EVENT_RISING_FALLING);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPEN_Msk, TIMER_EXTCTL_CAPEN_Msk);

    TIMER_EnableCapture(TIMER0, TIMER_CAPTURE_FREE_COUNTING_MODE, TIMER_CAPTURE_EVENT_GET_LOW_PERIOD);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPFUNCS_Msk, TIMER_CAPTURE_FREE_COUNTING_MODE);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPEDGE_Msk, TIMER_CAPTURE_EVENT_GET_LOW_PERIOD);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPEN_Msk, TIMER_EXTCTL_CAPEN_Msk);

    TIMER_EnableCapture(TIMER0, TIMER_CAPTURE_COUNTER_RESET_MODE, TIMER_CAPTURE_EVENT_GET_HIGH_PERIOD);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPFUNCS_Msk, TIMER_CAPTURE_COUNTER_RESET_MODE);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPEDGE_Msk, TIMER_CAPTURE_EVENT_GET_HIGH_PERIOD);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPEN_Msk, TIMER_EXTCTL_CAPEN_Msk);

    TIMER_DisableCapture(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CAPEN_Msk, 0);

    TIMER_EnableEventCounter(TIMER0, TIMER_COUNTER_EVENT_FALLING);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CNTPHASE_Msk, TIMER_COUNTER_EVENT_FALLING);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_EXTCNTEN_Msk, TIMER_CTL_EXTCNTEN_Msk);

    TIMER_EnableEventCounter(TIMER0, TIMER_COUNTER_EVENT_RISING);
    CU_ASSERT_EQUAL(TIMER0->EXTCTL & TIMER_EXTCTL_CNTPHASE_Msk, TIMER_COUNTER_EVENT_RISING);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_EXTCNTEN_Msk, TIMER_CTL_EXTCNTEN_Msk);

    TIMER_DisableEventCounter(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_EXTCNTEN_Msk, 0);

    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HXT, NULL);
    tmp = TIMER_GetModuleClock(TIMER0);
    CU_ASSERT_EQUAL(tmp, __HXT);

    /* Bypass LXT Stable Count */
    SYS->ALTCTL1 |= SYS_ALTCTL1_LXTFSTB_Msk;
    CLK_EnableXtalRC(CLK_SRCCTL_LXTEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_LXT, NULL);
    tmp = TIMER_GetModuleClock(TIMER0);
    CU_ASSERT_EQUAL(tmp, __LXT);

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_PCLK1, NULL);
    tmp = TIMER_GetModuleClock(TIMER0);
    CU_ASSERT_EQUAL(tmp, CLK_GetPCLK1Freq());

    CLK_SetModuleClock(TMR2_MODULE, CLK_TMRSEL_TMR2SEL_PCLK3, NULL);
    tmp = TIMER_GetModuleClock(TIMER2);
    CU_ASSERT_EQUAL(tmp, CLK_GetPCLK3Freq());

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_EXT, NULL);
    tmp = TIMER_GetModuleClock(TIMER0);
    CU_ASSERT_EQUAL(tmp, 0);

    CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_LIRC, NULL);
    tmp = TIMER_GetModuleClock(TIMER0);
    CU_ASSERT_EQUAL(tmp, __LIRC);

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC, NULL);
    tmp = TIMER_GetModuleClock(TIMER0);
    CU_ASSERT_EQUAL(tmp, __HIRC);

    /* r5464, Trim Filter Behavior */
    SYS->HIRC48MCFCTL = 0x00009999;
    CLK_EnableXtalRC(CLK_SRCCTL_HIRC48MEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRC48MSTB_Msk);

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC48M_DIV4, NULL);
    tmp = TIMER_GetModuleClock(TIMER0);
    CU_ASSERT_EQUAL(tmp, __HIRC48M / 4);

    TIMER_EnableFreqCounter(TIMER0, NULL, NULL, TRUE);
    CU_ASSERT_EQUAL(TIMER1->CMP, 0xFFFFFF);
    CU_ASSERT_EQUAL(TIMER1->EXTCTL & TIMER_EXTCTL_CAPIEN_Msk, TIMER_EXTCTL_CAPIEN_Msk);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_INTRGEN_Msk, TIMER_CTL_INTRGEN_Msk);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_CNTEN_Msk, TIMER_CTL_CNTEN_Msk);

    TIMER_EnableFreqCounter(TIMER2, NULL, NULL, FALSE);
    CU_ASSERT_EQUAL(TIMER3->CMP, 0xFFFFFF);
    CU_ASSERT_EQUAL(TIMER3->EXTCTL & TIMER_EXTCTL_CAPIEN_Msk, 0);
    CU_ASSERT_EQUAL(TIMER2->CTL & TIMER_CTL_INTRGEN_Msk, TIMER_CTL_INTRGEN_Msk);
    CU_ASSERT_EQUAL(TIMER2->CTL & TIMER_CTL_CNTEN_Msk, TIMER_CTL_CNTEN_Msk);

    TIMER_DisableFreqCounter(TIMER0);
    CU_ASSERT_EQUAL(TIMER0->CTL & TIMER_CTL_INTRGEN_Msk, 0);
    TIMER_DisableFreqCounter(TIMER2);
    CU_ASSERT_EQUAL(TIMER2->CTL & TIMER_CTL_INTRGEN_Msk, 0);

    TIMER_SetTriggerSource(TIMER0, TIMER_TRGSRC_TIMEOUT_EVENT);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGSSEL_Msk, TIMER_TRGSRC_TIMEOUT_EVENT);
    TIMER_SetTriggerSource(TIMER0, TIMER_TRGSRC_CAPTURE_EVENT);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGSSEL_Msk, TIMER_TRGSRC_CAPTURE_EVENT);

    TIMER_SetTriggerTarget(TIMER0, TIMER_TRG_TO_PWM);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGPWM_Msk, TIMER_TRG_TO_PWM);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGEADC_Msk, 0);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGDAC_Msk, 0);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGPDMA_Msk, 0);

    TIMER_SetTriggerTarget(TIMER0, TIMER_TRG_TO_EADC);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGPWM_Msk, 0);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGEADC_Msk, TIMER_TRG_TO_EADC);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGDAC_Msk, 0);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGPDMA_Msk, 0);

    TIMER_SetTriggerTarget(TIMER0, TIMER_TRG_TO_DAC);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGPWM_Msk, 0);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGEADC_Msk, 0);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGDAC_Msk, TIMER_TRG_TO_DAC);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGPDMA_Msk, 0);

    TIMER_SetTriggerTarget(TIMER0, TIMER_TRG_TO_PDMA);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGPWM_Msk, 0);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGEADC_Msk, 0);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGDAC_Msk, 0);
    CU_ASSERT_EQUAL(TIMER0->TRGCTL & TIMER_TRGCTL_TRGPDMA_Msk, TIMER_TRG_TO_PDMA);

    CU_PASS();
}

CU_SuiteInfo TimerSuites[] =
{
    { "Timer Const Test", Timer_Test_Init, Timer_Test_Clean, NULL, NULL, Timer_ConstTest },
    { "Timer Macro Test", Timer_Test_Init, Timer_Test_Clean, NULL, NULL, Timer_MacroTest },
    { "Timer Func  Test", Timer_Test_Init, Timer_Test_Clean, NULL, NULL, Timer_FuncTest },
    CU_SUITE_INFO_NULL
};

CU_TestInfo  Timer_ConstTest[] =
{
    { "Check TIMER Const",  Const_Test },
    CU_TEST_INFO_NULL
};

CU_TestInfo  Timer_MacroTest[] =
{
    {"Check TIMER Macro_Test ",      Macro_Test},
    {"Check TIMER_SET_CMP_VALUE ",      MACRO_TIMER_SET_CMP_VALUE},
    {"Check TIMER_SET_PRESCALE_VALUE ", MACRO_TIMER_SET_PRESCALE_VALUE},
    {"Check TIMER_IS_ACTIVE ",          API_TIMER_APIs},
    CU_TEST_INFO_NULL
};

CU_TestInfo  Timer_FuncTest[] =
{
    {"Check TIMER Func_Test",                                  Func_Test},
    {"Check TIMER Start/Stop/APIs ",                                  API_TIMER_APIs},
    {"Check Enable/Disable Capture Debounce API ",               API_TIMER_CaptureDebounce},
    {"Check Enable/Disable Event Counter Debounce API ",         API_TIMER_EventCounterDebounce},
    {"Check TIMER Open/Close and Enable/Disable Interrupt API ", API_TIMER_OpenAndInterrupt},
    {"Check Enable/Disable Capture Function and Interrupt API ", API_TIMER_CaptureAndInterrupt},
    {"Check Enable/Disable Event Counter Function API ",         API_TIMER_EventCounter},
    {"Check Get Module Clock API ",                              API_TIMER_GetModuleClock},
    {"Check Capture Interrupt Status API ",                      API_TIMER_CaptureStatus},
    {"Check TIMER Delay API ",                                   API_TIMER_Delay},
    {"Check Interrupt, Wake-up Function and Status API ",        API_TIMER_Wakeup},
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/