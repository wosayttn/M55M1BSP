/******************************************************************************
* @file    ttmr_cunit.c
* @version V1.00
* @brief   CUnit test ttmr
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
#include "ttmr_cunit.h"

int TTmr_Test_Init(void)
{
    return 0;
}
int TTmr_Test_Clean(void)
{
    return 0;
}

TTMR_T *TimerCh[] =
{
    TTMR0,
    TTMR1
};

volatile uint8_t g_u8IsTMRWKF[4] = {0};
void TTMR0_IRQHandler(void)
{
    if (TTMR_GetIntFlag(TTMR0) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TTMR_ClearIntFlag(TTMR0);

        while (TTMR_GetIntFlag(TTMR0) == 1);
    }

    if (TTMR_GetWakeupFlag(TTMR0) == 1)
    {
        /* Clear Timer0 wake-up flag */
        TTMR_ClearWakeupFlag(TTMR0);

        while (TTMR_GetWakeupFlag(TTMR0) == 1);

        g_u8IsTMRWKF[0] = 1;
    }
}

void TTMR1_IRQHandler(void)
{
    if (TTMR_GetIntFlag(TTMR1) == 1)
    {
        /* Clear Timer1 time-out interrupt flag */
        TTMR_ClearIntFlag(TTMR1);

        while (TTMR_GetIntFlag(TTMR1) == 1);
    }

    if (TTMR_GetWakeupFlag(TTMR1) == 1)
    {
        /* Clear Timer1 wake-up flag */
        TTMR_ClearWakeupFlag(TTMR1);

        while (TTMR_GetWakeupFlag(TTMR1) == 1);

        g_u8IsTMRWKF[1] = 1;
    }
}

int32_t TTMR_InitClock(void)
{
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    CLK_EnableModuleClock(TTMR0_MODULE);
    CLK_EnableModuleClock(TTMR1_MODULE);

    CLK->TTMRSEL = (CLK->TTMRSEL & ~(CLK_TTMRSEL_TTMR0SEL_Msk | CLK_TTMRSEL_TTMR1SEL_Msk)) |
                   (CLK_TTMRSEL_TTMR0SEL_PCLK4 | CLK_TTMRSEL_TTMR1SEL_PCLK4);

    return 0;
}

int32_t IsTimerRegsCleared(TTMR_T *timer)
{
    if ((timer->CTL != 0x0) || (timer->CMP != 0x0) ||
            (timer->INTSTS != 0))
    {
        return -1;
    }

    return 0;
}

int32_t ClearTimerRegs(TTMR_T *timer)
{
    TTMR_Close(timer);

    timer->CMP = 0x0;

    timer->INTSTS = timer->INTSTS;

    if (IsTimerRegsCleared(timer) < 0)
        return -1;

    return 0;
}

const uint32_t au32OPModeSel[] =
{
    TTMR_ONESHOT_MODE,
    TTMR_PERIODIC_MODE,
    (2UL << TTMR_CTL_OPMODE_Pos),
    TTMR_CONTINUOUS_MODE,
};

void CONST_Operation_Mode(void)
{
    CU_ASSERT_EQUAL(TTMR_ONESHOT_MODE, 0);
    CU_ASSERT_EQUAL(TTMR_PERIODIC_MODE, (1UL << TTMR_CTL_OPMODE_Pos));
    CU_ASSERT_EQUAL(TTMR_CONTINUOUS_MODE, (3UL << TTMR_CTL_OPMODE_Pos));
}

void CONST_Event_Trigger(void)
{
    CU_ASSERT_EQUAL(TTMR_TRGEN, TTMR_TRGCTL_TRGEN_Msk);
    CU_ASSERT_EQUAL(TTMR_TRG_TO_LPPDMA, TTMR_TRGCTL_TRGLPPDMA_Msk);
}

void MACRO_TIMER_SET_CMP_VALUE(void)
{
    volatile uint32_t i;

    if (TTMR_InitClock() != 0)
    {
        CU_FAIL("TTMR Init FAIL");
        return ;
    }

    /* Check TIMER_SET_CMP_VALUE */
    for (i = 0; i < 2; i++)
    {
        printf("TTMR_SET_CMP_VALUE (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearTimerRegs(TimerCh[i]), 0);

        TTMR_SET_CMP_VALUE(TimerCh[i], 0x2);
        CU_ASSERT_EQUAL(TimerCh[i]->CMP, 0x2);

        TTMR_SET_CMP_VALUE(TimerCh[i], 0x5A5A5A);
        CU_ASSERT_EQUAL(TimerCh[i]->CMP, 0x5A5A5A);

        TTMR_SET_CMP_VALUE(TimerCh[i], 0xA5A5A5);
        CU_ASSERT_EQUAL(TimerCh[i]->CMP, 0xA5A5A5);

        TTMR_SET_CMP_VALUE(TimerCh[i], 0xFFFFFF);
        CU_ASSERT_EQUAL(TimerCh[i]->CMP, 0xFFFFFF);
    }
}

void MACRO_TIMER_SET_PRESCALE_VALUE(void)
{
    volatile uint32_t i;

    if (TTMR_InitClock() != 0)
    {
        CU_FAIL("TTMR Init FAIL");
        return ;
    }

    /* Check TIMER_SET_PRESCALE_VALUE */
    for (i = 0; i < 2; i++)
    {
        printf("TTMR_SET_PRESCALE_VALUE (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearTimerRegs(TimerCh[i]), 0);

        TTMR_SET_PRESCALE_VALUE(TimerCh[i], 0x2);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x2);

        TTMR_SET_PRESCALE_VALUE(TimerCh[i], 0x5A);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x5A);

        TTMR_SET_PRESCALE_VALUE(TimerCh[i], 0xA5);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0xA5);

        TTMR_SET_PRESCALE_VALUE(TimerCh[i], 0xFF);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0xFF);
    }
}

void MACRO_TIMER_SET_OPMODE_VALUE(void)
{
    volatile uint32_t i;

    if (TTMR_InitClock() != 0)
    {
        CU_FAIL("TTMR Init FAIL");
        return ;
    }

    /* Check TTMR_SET_OPMODE_VALUE */
    for (i = 0; i < 2; i++)
    {
        printf("TTMR_SET_OPMODE_VALUE (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearTimerRegs(TimerCh[i]), 0);

        TTMR_SET_OPMODE(TimerCh[i], TTMR_ONESHOT_MODE);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, TTMR_ONESHOT_MODE);

        TTMR_SET_OPMODE(TimerCh[i], TTMR_PERIODIC_MODE);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, TTMR_PERIODIC_MODE);

        TTMR_SET_OPMODE(TimerCh[i], TTMR_CONTINUOUS_MODE);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, TTMR_CONTINUOUS_MODE);
    }
}

void API_TIMER_APIs(void)
{
    volatile uint32_t i, j;

    if (TTMR_InitClock() != 0)
    {
        CU_FAIL("TTMR Init FAIL");
        return ;
    }

    /* Check TTMR_EnablePDCLK, TTMR_DisablePDCLK*/
    for (i = 0; i < 2; i++)
    {
        TTMR_EnablePDCLK(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, TTMR_CTL_PDCLKEN_Msk);

        TTMR_DisablePDCLK(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0);
    }

    /* Check TTMR_Start, TTMR_Stop and TTMR_IS_ACTIVE */
    for (i = 0; i < 2; i++)
    {
        CU_ASSERT_EQUAL(ClearTimerRegs(TimerCh[i]), 0);

        TimerCh[i]->CMP = 0x00FFFFFF;
        TimerCh[i]->CTL = 0x18000000;
        CU_ASSERT_EQUAL(TTMR_IS_ACTIVE(TimerCh[i]), 0);

        TTMR_Start(TimerCh[i]);

        for (j = 0; j < SystemCoreClock / 1000000; j++)
            __NOP();

        CU_ASSERT_EQUAL(TTMR_IS_ACTIVE(TimerCh[i]), 1);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x5A000000);

        TTMR_Stop(TimerCh[i]);

        for (j = 0; j < SystemCoreClock / 1000000; j++)
            __NOP();

        CU_ASSERT_EQUAL(TTMR_IS_ACTIVE(TimerCh[i]), 0);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x18000000);
    }

    /* Check TTMR_SetTriggerTarget*/
    for (i = 0; i < 2; i++)
    {
        TTMR_SetTriggerTarget(TimerCh[i], TTMR_TRGEN);
        CU_ASSERT_EQUAL(TimerCh[i]->TRGCTL, BIT1);

        TTMR_SetTriggerTarget(TimerCh[i], TTMR_TRG_TO_LPPDMA);
        CU_ASSERT_EQUAL(TimerCh[i]->TRGCTL, BIT4);

        TTMR_SetTriggerTarget(TimerCh[i], (TTMR_TRGEN | TTMR_TRG_TO_LPPDMA));
        CU_ASSERT_EQUAL(TimerCh[i]->TRGCTL, (BIT1 | BIT4));

        TTMR_SetTriggerTarget(TimerCh[i], 0);
        CU_ASSERT_EQUAL(TimerCh[i]->TRGCTL, 0);
    }

    for (i = 0; i < 2; i++)
    {
        CU_ASSERT_EQUAL(ClearTimerRegs(TimerCh[i]), 0);
    }
}

void API_TIMER_OpenAndInterrupt(void)
{
    volatile uint32_t i, j, u32Timeout = 0;

    if (TTMR_InitClock() != 0)
    {
        CU_FAIL("TTMR Init FAIL");
        return ;
    }

    /* Check TTMR_Open, TTMR_EnableInt and TTMR_DisableInt*/
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < sizeof(au32OPModeSel) / sizeof(au32OPModeSel[0]); j++)
        {
            printf("OPModeSel %d (Ch-%d)\n", j, i);
            CU_ASSERT_EQUAL(ClearTimerRegs(TimerCh[i]), 0);

            TTMR_Open(TimerCh[i], au32OPModeSel[j], TTMR_GetModuleClock(TimerCh[i]));
            CU_ASSERT_EQUAL(TimerCh[i]->CTL, (j << 27));

            TTMR_EnableInt(TimerCh[i]);
            CU_ASSERT_EQUAL(TimerCh[i]->CTL, ((1 << 29) | (j << 27)));

            TTMR_DisableInt(TimerCh[i]);
            CU_ASSERT_EQUAL(TimerCh[i]->CTL, (j << 27));

            TTMR_Close(TimerCh[i]);
            CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x0);
        }
    }
}

void API_TIMER_Delay(void)
{
    volatile uint32_t i, u32TDR[3];

    if (TTMR_InitClock() != 0)
    {
        CU_FAIL("TTMR Init FAIL");
        return ;
    }

    /* Check TTMR_Delay */
    for (i = 0; i < 2; i++)
    {
        CU_ASSERT_EQUAL(ClearTimerRegs(TimerCh[i]), 0);

        if (i == 0)
        {
            TTMR1->CMP = 0xFFFFFF;
            TTMR1->CTL = TTMR_PERIODIC_MODE | TTMR_CTL_CNTEN_Msk | (12 - 1);

            TTMR1->CMP = 0xFFFFFF;
            TTMR_Delay(TimerCh[i], 1000);
            u32TDR[0] = TTMR_GetCounter(TTMR1);

            TTMR_ResetCounter(TTMR1);
            TTMR_Delay(TimerCh[i], 500000);
            u32TDR[1] = TTMR_GetCounter(TTMR1);

            TTMR1->CMP = 0xFFFFFF;
            TTMR_Delay(TimerCh[i], 1000000);
            u32TDR[2] = TTMR_GetCounter(TTMR1);
            printf(" [%d] [%d] [%d]", u32TDR[0], u32TDR[1], u32TDR[2]);
        }
        else
        {
            TTMR_ResetCounter(TTMR0);
            TTMR0->CTL = TTMR_PERIODIC_MODE | TTMR_CTL_CNTEN_Msk | (12 - 1);

            TTMR0->CMP = 0xFFFFFF;
            TTMR_Delay(TimerCh[i], 1000);
            u32TDR[0] = TTMR_GetCounter(TTMR0);

            TTMR0->CMP = 0xFFFFFF;
            TTMR_Delay(TimerCh[i], 500000);
            u32TDR[1] = TTMR_GetCounter(TTMR0);

            TTMR0->CMP = 0xFFFFFF;
            TTMR_Delay(TimerCh[i], 1000000);
            u32TDR[2] = TTMR_GetCounter(TTMR0);
        }

        if (((u32TDR[0] > 1200) || (u32TDR[0] <= 1000)) ||
                ((u32TDR[1] > 502500) || (u32TDR[1] <= 500000)) ||
                ((u32TDR[2] > 1005000) || (u32TDR[2] <= 1000000)))
        {
            CU_FAIL("TTMR counter value FAIL");
            break;
        }
    }

    TTMR_Stop(TTMR0);
    TTMR_Stop(TTMR1);
}

void API_TIMER_Wakeup(void)
{
    volatile uint32_t i, u32TDR;

    if (TTMR_InitClock() != 0)
    {
        CU_FAIL("TTMR Init FAIL");
        return ;
    }

    /* Check TTMR_EnableWakeup and TTMR_DisableWakeup */
    for (i = 0; i < 2; i++)
    {
        printf("TTMR_EnableWakeup (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearTimerRegs(TimerCh[i]), 0);

        TTMR_EnableWakeup(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, BIT23);

        TTMR_DisableWakeup(TimerCh[i]);
        CU_ASSERT_EQUAL(TimerCh[i]->CTL, 0x0);
    }

    /* Enable Internal RC 32KHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);
    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);

    CLK->TTMRSEL = (CLK->TTMRSEL & ~(CLK_TTMRSEL_TTMR0SEL_Msk | CLK_TTMRSEL_TTMR1SEL_Msk)) |
                   (CLK_TTMRSEL_TTMR0SEL_LIRC | CLK_TTMRSEL_TTMR1SEL_LIRC);

    CU_ASSERT_EQUAL(ClearTimerRegs(TimerCh[0]), 0);

    NVIC_EnableIRQ(TTMR0_IRQn);
    NVIC_EnableIRQ(TTMR1_IRQn);

    /* Check TTMR_GetIntFlag, TTMR_ClearIntFlag, TTMR_GetWakeupFlag, TTMR_ClearWakeupFlag and TTMR_GetCounter */
    for (i = 0; i < 2; i++)
    {
        TimerCh[i]->CMP = 100;
        TimerCh[i]->CTL = TTMR_CONTINUOUS_MODE | TTMR_CTL_INTEN_Msk;
        TTMR_EnableWakeup(TimerCh[i]);

        g_u8IsTMRWKF[i] = 0;

        TTMR_Start(TimerCh[i]);

        PMC_PowerDown();

        while (g_u8IsTMRWKF[i] == 0);

        u32TDR = TTMR_GetCounter(TimerCh[i]);

        printf(" [%d]", u32TDR);

        if (u32TDR > 120)
        {
            CU_FAIL("TTMR counter value FAIL");
            break;
        }

        TTMR_Stop(TimerCh[i]);
    }

    NVIC_DisableIRQ(TTMR0_IRQn);
    NVIC_DisableIRQ(TTMR1_IRQn);
    TTMR_Stop(TTMR0);
    TTMR_Stop(TTMR1);
}

void API_TIMER_GetModuleClock(void)
{
    if (TTMR_InitClock() != 0)
    {
        CU_FAIL("TTMR Init FAIL");
        return ;
    }

    /* Check TIMER_GetModuleClock */
    CLK->TTMRSEL = (CLK->TTMRSEL & ~(CLK_TTMRSEL_TTMR0SEL_Msk | CLK_TTMRSEL_TTMR1SEL_Msk)) |
                   (CLK_TTMRSEL_TTMR0SEL_MIRC | CLK_TTMRSEL_TTMR1SEL_MIRC);
    CU_ASSERT_EQUAL(TTMR_GetModuleClock(TTMR0), __MIRC);
    CU_ASSERT_EQUAL(TTMR_GetModuleClock(TTMR1), __MIRC);
    printf(" [%d]", TTMR_GetModuleClock(TTMR0));

    CLK->TTMRSEL = (CLK->TTMRSEL & ~(CLK_TTMRSEL_TTMR0SEL_Msk | CLK_TTMRSEL_TTMR1SEL_Msk)) |
                   (CLK_TTMRSEL_TTMR0SEL_LIRC | CLK_TTMRSEL_TTMR1SEL_LIRC);
    CU_ASSERT_EQUAL(TTMR_GetModuleClock(TTMR0), __LIRC);
    CU_ASSERT_EQUAL(TTMR_GetModuleClock(TTMR1), __LIRC);
    printf(" [%d]", TTMR_GetModuleClock(TTMR1));

    CLK->TTMRSEL = (CLK->TTMRSEL & ~(CLK_TTMRSEL_TTMR0SEL_Msk | CLK_TTMRSEL_TTMR1SEL_Msk)) |
                   (CLK_TTMRSEL_TTMR0SEL_HIRC | CLK_TTMRSEL_TTMR1SEL_HIRC);
    CU_ASSERT_EQUAL(TTMR_GetModuleClock(TTMR0), __HIRC);
    CU_ASSERT_EQUAL(TTMR_GetModuleClock(TTMR1), __HIRC);
    printf(" [%d]", TTMR_GetModuleClock(TTMR0));

    CLK->TTMRSEL = (CLK->TTMRSEL & ~(CLK_TTMRSEL_TTMR0SEL_Msk | CLK_TTMRSEL_TTMR1SEL_Msk)) |
                   (CLK_TTMRSEL_TTMR0SEL_PCLK4 | CLK_TTMRSEL_TTMR1SEL_PCLK4);
    CU_ASSERT_EQUAL(TTMR_GetModuleClock(TTMR0), CLK_GetPCLK4Freq());
    CU_ASSERT_EQUAL(TTMR_GetModuleClock(TTMR1), CLK_GetPCLK4Freq());
    printf(" [%d]", TTMR_GetModuleClock(TTMR1));

    CLK->TTMRSEL = (CLK->TTMRSEL & ~(CLK_TTMRSEL_TTMR0SEL_Msk | CLK_TTMRSEL_TTMR1SEL_Msk)) |
                   (CLK_TTMRSEL_TTMR0SEL_LXT | CLK_TTMRSEL_TTMR1SEL_LXT);
    CU_ASSERT_EQUAL(TTMR_GetModuleClock(TTMR0), __LXT);
    CU_ASSERT_EQUAL(TTMR_GetModuleClock(TTMR1), __LXT);
    printf(" [%d]", TTMR_GetModuleClock(TTMR0));
}

CU_SuiteInfo TTmrSuites[] =
{
    { "TTmr Const Test", TTmr_Test_Init, TTmr_Test_Clean, NULL, NULL, TTmr_ConstTest },
    { "TTmr Macro Test", TTmr_Test_Init, TTmr_Test_Clean, NULL, NULL, TTmr_MacroTest },
    { "TTmr Func  Test", TTmr_Test_Init, TTmr_Test_Clean, NULL, NULL, TTmr_FuncTest },
    CU_SUITE_INFO_NULL
};

CU_TestInfo  TTmr_ConstTest[] =
{
    { "Check Operation Mode",  CONST_Operation_Mode },
    { "Check Event Trigger",  CONST_Event_Trigger },
    CU_TEST_INFO_NULL
};

CU_TestInfo  TTmr_MacroTest[] =
{
    { "Check TTMR_SET_CMP_VALUE",  MACRO_TIMER_SET_CMP_VALUE },
    { "Check TTMR_SET_PRESCALE_VALUE",  MACRO_TIMER_SET_PRESCALE_VALUE },
    { "Check TTMR_SET_OPMODE",  MACRO_TIMER_SET_OPMODE_VALUE },
    { "Check TTMR_IS_ACTIVE",  API_TIMER_APIs },
    CU_TEST_INFO_NULL
};

CU_TestInfo  TTmr_FuncTest[] =
{
    {"Check TTMR APIs...",                                     API_TIMER_APIs},
    {"Check TTMR Open/Close and Enable/Disable Interrupt API", API_TIMER_OpenAndInterrupt},
    {"Check TTMR Delay API",                                   API_TIMER_Delay},
    {"Check Interrupt, Wake-up Function and Status API",        API_TIMER_Wakeup},
    {"Check Get Module Clock API",                              API_TIMER_GetModuleClock},
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/