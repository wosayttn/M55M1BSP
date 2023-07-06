/******************************************************************************
* @file    lptmr_cunit.c
* @version V1.00
* @brief   CUnit test LPTMR
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
#include "lptmr_cunit.h"

int LPTMR_Test_Init(void)
{
    return 0;
}
int LPTMR_Test_Clean(void)
{
    return 0;
}

#define ENABLE_GCR_SELFTEST     (outpw(0x40000E00, 0x1))
#define DISABLE_GCR_SELFTEST    (outpw(0x40000E00, 0x0))

LPTMR_T *LPTMRCh[] =
{
    LPTMR0,
    LPTMR1
};

volatile uint8_t g_u8IsLPTMRWKF[2] = {0};
void LPTMR0_IRQHandler(void)
{
    if (LPTMR_GetIntFlag(LPTMR0) == 1)
    {
        /* Clear LPTMR0 time-out interrupt flag */
        LPTMR_ClearIntFlag(LPTMR0);

        while (LPTMR_GetIntFlag(LPTMR0) == 1);
    }

    if (LPTMR_GetWakeupFlag(LPTMR0) == 1)
    {
        /* Clear LPTMR0 wake-up flag */
        LPTMR_ClearWakeupFlag(LPTMR0);

        while (LPTMR_GetWakeupFlag(LPTMR0) == 1);

        g_u8IsLPTMRWKF[0] = 1;
    }
}

void LPTMR1_IRQHandler(void)
{
    if (LPTMR_GetIntFlag(LPTMR1) == 1)
    {
        /* Clear LPTMR1 time-out interrupt flag */
        LPTMR_ClearIntFlag(LPTMR1);

        while (LPTMR_GetIntFlag(LPTMR1) == 1);
    }

    if (LPTMR_GetWakeupFlag(LPTMR1) == 1)
    {
        /* Clear LPTMR1 wake-up flag */
        LPTMR_ClearWakeupFlag(LPTMR1);

        while (LPTMR_GetWakeupFlag(LPTMR1) == 1);

        g_u8IsLPTMRWKF[1] = 1;
    }
}

int32_t LPTMR_InitClock(void)
{
    SYS_UnlockReg();
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    CLK->LPTMRSEL = (CLK->LPTMRSEL & ~(CLK_LPTMRSEL_LPTMR0SEL_Msk | CLK_LPTMRSEL_LPTMR1SEL_Msk)) |
                    (CLK_LPTMRSEL_LPTMR0SEL_HIRC | CLK_LPTMRSEL_LPTMR1SEL_HIRC);

    CLK_EnableModuleClock(LPTMR0_MODULE);
    CLK_EnableModuleClock(LPTMR1_MODULE);

    return 0;
}

int32_t IsLPTMRRegsCleared(LPTMR_T *LPTMR)
{
    if ((LPTMR->CTL != 0x0) || (LPTMR->CMP != 0x0) || (LPTMR->EXTCTL != 0x0) ||
            (LPTMR->INTSTS != 0) || (LPTMR->EINTSTS != 0))
    {
        return -1;
    }

    return 0;
}

int32_t ClearLPTMRRegs(LPTMR_T *LPTMR)
{
    LPTMR_Close(LPTMR);

    LPTMR->CMP = 0x0;

    LPTMR->INTSTS = LPTMR->INTSTS;
    LPTMR->EINTSTS = LPTMR->EINTSTS;

    if (IsLPTMRRegsCleared(LPTMR) < 0)
        return -1;

    return 0;
}

const uint32_t au32OPModeSel[] =
{
    LPTMR_ONESHOT_MODE,
    LPTMR_PERIODIC_MODE,
    LPTMR_TOGGLE_MODE,
    LPTMR_CONTINUOUS_MODE,
};

const uint32_t au32CapEdgeSel[] =
{
    LPTMR_CAPTURE_EVENT_FALLING,
    LPTMR_CAPTURE_EVENT_RISING,
    LPTMR_CAPTURE_EVENT_FALLING_RISING,
    LPTMR_CAPTURE_EVENT_RISING_FALLING,
    0xFFFFFFFF, // reserved
    0xFFFFFFFF, // reserved
    LPTMR_CAPTURE_EVENT_GET_LOW_PERIOD,
    LPTMR_CAPTURE_EVENT_GET_HIGH_PERIOD,
};

const uint32_t au32CntEdgeSel[] =
{
    LPTMR_COUNTER_EVENT_FALLING,
    LPTMR_COUNTER_EVENT_RISING,
};

void CONST_Operation_Mode(void)
{
    CU_ASSERT_EQUAL(LPTMR_ONESHOT_MODE, 0);
    CU_ASSERT_EQUAL(LPTMR_PERIODIC_MODE, (1UL << LPTMR_CTL_OPMODE_Pos));
    CU_ASSERT_EQUAL(LPTMR_TOGGLE_MODE, (2UL << LPTMR_CTL_OPMODE_Pos));
    CU_ASSERT_EQUAL(LPTMR_CONTINUOUS_MODE, (3UL << LPTMR_CTL_OPMODE_Pos));
}

void CONST_Event_Trigger(void)
{
    CU_ASSERT_EQUAL(LPTMR_TRGEN, LPTMR_TRGCTL_TRGEN_Msk);
    CU_ASSERT_EQUAL(LPTMR_TRG_TO_LPPDMA, LPTMR_TRGCTL_TRGLPPDMA_Msk);

    CU_ASSERT_EQUAL(LPTMR_TRGSEL_TIMEOUT_EVENT, LPTMR_TRGSRC_TIMEOUT_EVENT);
    CU_ASSERT_EQUAL(LPTMR_TRGSEL_CAPTURE_EVENT, LPTMR_TRGSRC_CAPTURE_EVENT);
}

void CONST_Toggle_Pin(void)
{
    CU_ASSERT_EQUAL(LPTMR_TOUT_PIN_FROM_TX, LPTMR_TOUT_PIN_FROM_TMX);
    CU_ASSERT_EQUAL(LPTMR_TOUT_PIN_FROM_TX_EXT, LPTMR_TOUT_PIN_FROM_TMX_EXT);
}

void CONST_Event_Counter(void)
{
    CU_ASSERT_EQUAL(LPTMR_COUNTER_EVENT_FALLING, (0UL << LPTMR_EXTCTL_CNTPHASE_Pos));
    CU_ASSERT_EQUAL(LPTMR_COUNTER_EVENT_RISING, (1UL << LPTMR_EXTCTL_CNTPHASE_Pos));

    CU_ASSERT_EQUAL(LPTMR_EVENT_COUNTER_SOURCE_TMX, (0UL << LPTMR_EXTCTL_ECNTSSEL_Pos));
    CU_ASSERT_EQUAL(LPTMR_EVENT_COUNTER_SOURCE_ACMP0, (2UL << LPTMR_EXTCTL_ECNTSSEL_Pos));
    CU_ASSERT_EQUAL(LPTMR_EVENT_COUNTER_SOURCE_ACMP1, (3UL << LPTMR_EXTCTL_ECNTSSEL_Pos));
    CU_ASSERT_EQUAL(LPTMR_EVENT_COUNTER_SOURCE_ACMP2, (4UL << LPTMR_EXTCTL_ECNTSSEL_Pos));
    CU_ASSERT_EQUAL(LPTMR_EVENT_COUNTER_SOURCE_ACMP3, (5UL << LPTMR_EXTCTL_ECNTSSEL_Pos));
}

void CONST_Capture(void)
{
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_FREE_COUNTING_MODE, (0UL << LPTMR_EXTCTL_CAPFUNCS_Pos));
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_COUNTER_RESET_MODE, (1UL << LPTMR_EXTCTL_CAPFUNCS_Pos));

    CU_ASSERT_EQUAL(LPTMR_CAPTURE_EVENT_FALLING, (0UL << LPTMR_EXTCTL_CAPEDGE_Pos));
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_EVENT_RISING, (1UL << LPTMR_EXTCTL_CAPEDGE_Pos));
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_EVENT_FALLING_RISING, (2UL << LPTMR_EXTCTL_CAPEDGE_Pos));
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_EVENT_RISING_FALLING, (3UL << LPTMR_EXTCTL_CAPEDGE_Pos));
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_EVENT_GET_LOW_PERIOD, (6UL << LPTMR_EXTCTL_CAPEDGE_Pos));
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_EVENT_GET_HIGH_PERIOD, (7UL << LPTMR_EXTCTL_CAPEDGE_Pos));

    CU_ASSERT_EQUAL(LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_1, 0UL);
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_2, 1UL);
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_4, 2UL);
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_8, 3UL);
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_16, 4UL);
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_32, 5UL);
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_64, 6UL);
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_128, 7UL);

    CU_ASSERT_EQUAL(LPTMR_CAPSRC_TMX_EXT, 0UL << LPTMR_CTL_CAPSRC_Pos);
    CU_ASSERT_EQUAL(LPTMR_CAPSRC_INTERNAL, 1UL << LPTMR_CTL_CAPSRC_Pos);
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_FROM_ACMP0, LPTMR_INTERCAPSEL_ACMP0);
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_FROM_ACMP1, LPTMR_INTERCAPSEL_ACMP1);
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_FROM_ACMP2, LPTMR_INTERCAPSEL_ACMP2);
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_FROM_ACMP3, LPTMR_INTERCAPSEL_ACMP3);
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_FROM_INTERNAL, LPTMR_CAPTURE_FROM_ACMP0);
    CU_ASSERT_EQUAL(LPTMR_CAPTURE_FROM_EXTERNAL, 0xFFUL);
}

void MACRO_LPTMR_SET_CMP_VALUE(void)
{
    volatile uint32_t i;

    if (LPTMR_InitClock() != 0)
    {
        CU_FAIL("LPTMR Init FAIL");
        return ;
    }

    /* Check LPTMR_SET_CMP_VALUE */
    for (i = 0; i < 2; i++)
    {
        printf("LPTMR_SET_CMP_VALUE (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

        LPTMR_SET_CMP_VALUE(LPTMRCh[i], 0x2);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CMP, 0x2);

        LPTMR_SET_CMP_VALUE(LPTMRCh[i], 0x5A5A5A);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CMP, 0x5A5A5A);

        LPTMR_SET_CMP_VALUE(LPTMRCh[i], 0xA5A5A5);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CMP, 0xA5A5A5);

        LPTMR_SET_CMP_VALUE(LPTMRCh[i], 0xFFFFFF);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CMP, 0xFFFFFF);
    }
}

void MACRO_LPTMR_SET_PRESCALE_VALUE(void)
{
    volatile uint32_t i;

    if (LPTMR_InitClock() != 0)
    {
        CU_FAIL("LPTMR Init FAIL");
        return ;
    }

    /* Check LPTMR_SET_PRESCALE_VALUE */
    for (i = 0; i < 2; i++)
    {
        printf("LPTMR_SET_PRESCALE_VALUE (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

        LPTMR_SET_PRESCALE_VALUE(LPTMRCh[i], 0x2);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, 0x2);

        LPTMR_SET_PRESCALE_VALUE(LPTMRCh[i], 0x5A);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, 0x5A);

        LPTMR_SET_PRESCALE_VALUE(LPTMRCh[i], 0xA5);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, 0xA5);

        LPTMR_SET_PRESCALE_VALUE(LPTMRCh[i], 0xFF);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, 0xFF);
    }
}

void MACRO_LPTMR_SET_OPMODE_VALUE(void)
{
    volatile uint32_t i;

    if (LPTMR_InitClock() != 0)
    {
        CU_FAIL("LPTMR Init FAIL");
        return ;
    }

    /* Check LPTMR_SET_OPMODE_VALUE */
    for (i = 0; i < 2; i++)
    {
        printf("LPTMR_SET_OPMODE_VALUE (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

        LPTMR_SET_OPMODE(LPTMRCh[i], LPTMR_ONESHOT_MODE);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, LPTMR_ONESHOT_MODE);

        LPTMR_SET_OPMODE(LPTMRCh[i], LPTMR_PERIODIC_MODE);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, LPTMR_PERIODIC_MODE);

        LPTMR_SET_OPMODE(LPTMRCh[i], LPTMR_TOGGLE_MODE);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, LPTMR_TOGGLE_MODE);

        LPTMR_SET_OPMODE(LPTMRCh[i], LPTMR_CONTINUOUS_MODE);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, LPTMR_CONTINUOUS_MODE);
    }
}

void API_LPTMR_APIs(void)
{
    volatile uint32_t i, j;

    if (LPTMR_InitClock() != 0)
    {
        CU_FAIL("LPTMR Init FAIL");
        return ;
    }

    /* Check LPTMR_EnablePDCLK, LPTMR_DisablePDCLK*/
    for (i = 0; i < 2; i++)
    {
        LPTMR_EnablePDCLK(LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL & LPTMR_CTL_PDCLKEN_Msk, LPTMR_CTL_PDCLKEN_Msk);

        LPTMR_DisablePDCLK(LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL & LPTMR_CTL_PDCLKEN_Msk, 0);
    }

    /* Check LPTMR_Start, LPTMR_Stop and LPTMR_IS_ACTIVE */
    for (i = 0; i < 2; i++)
    {
        CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

        LPTMRCh[i]->CMP = 0x00FFFFFF;
        LPTMRCh[i]->CTL = 0x18000000;
        CU_ASSERT_EQUAL(LPTMR_IS_ACTIVE(LPTMRCh[i]), 0);

        LPTMR_Start(LPTMRCh[i]);

        for (j = 0; j < SystemCoreClock / 1000000; j++)
            __NOP();

        CU_ASSERT_EQUAL(LPTMR_IS_ACTIVE(LPTMRCh[i]), 1);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, 0x5A000000);

        LPTMR_Stop(LPTMRCh[i]);

        for (j = 0; j < SystemCoreClock / 1000000; j++)
            __NOP();

        CU_ASSERT_EQUAL(LPTMR_IS_ACTIVE(LPTMRCh[i]), 0);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, 0x18000000);
    }

    /* Check LPTMR_SetTriggerSource, LPTMR_TRGSEL_TIMEOUT_EVENT, LPTMR_TRGSEL_CAPTURE_EVENT */
    for (i = 0; i < 2; i++)
    {
        LPTMR_SetTriggerSource(LPTMRCh[i], LPTMR_TRGSEL_CAPTURE_EVENT);
        CU_ASSERT_EQUAL(LPTMRCh[i]->TRGCTL & BIT0, 0x1);
        LPTMR_SetTriggerSource(LPTMRCh[i], LPTMR_TRGSEL_TIMEOUT_EVENT);
        CU_ASSERT_EQUAL(LPTMRCh[i]->TRGCTL & BIT0, 0x0);
    }

    /* Check LPTMR_SetTriggerTarget, LPTMR_TRG_TO_PWM, LPTMR_TRG_TO_ADC, LPTMR_TRG_TO_PDMA */
    for (i = 0; i < 2; i++)
    {
        LPTMR_SetTriggerTarget(LPTMRCh[i], LPTMR_TRGEN);
        CU_ASSERT_EQUAL(LPTMRCh[i]->TRGCTL, BIT1);

        LPTMR_SetTriggerTarget(LPTMRCh[i], LPTMR_TRG_TO_LPPDMA);
        CU_ASSERT_EQUAL(LPTMRCh[i]->TRGCTL, BIT4);

        LPTMR_SetTriggerTarget(LPTMRCh[i], (LPTMR_TRGEN | LPTMR_TRG_TO_LPPDMA));
        CU_ASSERT_EQUAL(LPTMRCh[i]->TRGCTL, (BIT1 | BIT4));

        LPTMR_SetTriggerTarget(LPTMRCh[i], 0);
        CU_ASSERT_EQUAL(LPTMRCh[i]->TRGCTL, 0);
    }

    for (i = 0; i < 2; i++)
    {
        CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);
    }
}

void API_LPTMR_CaptureDebounce(void)
{
    volatile uint32_t i;

    if (LPTMR_InitClock() != 0)
    {
        CU_FAIL("LPTMR Init FAIL");
        return ;
    }

    /* Check LPTMR_EnableCaptureDebounce and LPTMR_DisableCaptureDebounce */
    for (i = 0; i < 2; i++)
    {
        printf("LPTMR_EnableCaptureDebounce (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

        LPTMR_EnableCaptureDebounce(LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, 0x40);

        LPTMR_DisableCaptureDebounce(LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, 0x0);
    }

    /* Check LPTMR_EnableCaptureInputNoiseFilter and LPTMR_DisableCaptureInputNoiseFilter */
    for (i = 0; i < 2; i++)
    {
        printf("LPTMR_EnableCaptureInputNoiseFilter (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

        LPTMR_EnableCaptureInputNoiseFilter(LPTMRCh[i], 1, LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_2);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CAPNF, (LPTMR_CAPNF_CAPNFEN_Msk | (1 << LPTMR_CAPNF_CAPNFCNT_Pos) | (1 << LPTMR_CAPNF_CAPNFSEL_Pos)));

        LPTMR_EnableCaptureInputNoiseFilter(LPTMRCh[i], 2, LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_4);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CAPNF, (LPTMR_CAPNF_CAPNFEN_Msk | (2 << LPTMR_CAPNF_CAPNFCNT_Pos) | (2 << LPTMR_CAPNF_CAPNFSEL_Pos)));

        LPTMR_EnableCaptureInputNoiseFilter(LPTMRCh[i], 3, LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_8);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CAPNF, (LPTMR_CAPNF_CAPNFEN_Msk | (3 << LPTMR_CAPNF_CAPNFCNT_Pos) | (3 << LPTMR_CAPNF_CAPNFSEL_Pos)));

        LPTMR_EnableCaptureInputNoiseFilter(LPTMRCh[i], 4, LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_16);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CAPNF, (LPTMR_CAPNF_CAPNFEN_Msk | (4 << LPTMR_CAPNF_CAPNFCNT_Pos) | (4 << LPTMR_CAPNF_CAPNFSEL_Pos)));

        LPTMR_EnableCaptureInputNoiseFilter(LPTMRCh[i], 5, LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_32);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CAPNF, (LPTMR_CAPNF_CAPNFEN_Msk | (5 << LPTMR_CAPNF_CAPNFCNT_Pos) | (5 << LPTMR_CAPNF_CAPNFSEL_Pos)));

        LPTMR_EnableCaptureInputNoiseFilter(LPTMRCh[i], 6, LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_64);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CAPNF, (LPTMR_CAPNF_CAPNFEN_Msk | (6 << LPTMR_CAPNF_CAPNFCNT_Pos) | (6 << LPTMR_CAPNF_CAPNFSEL_Pos)));

        LPTMR_EnableCaptureInputNoiseFilter(LPTMRCh[i], 7, LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_128);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CAPNF, (LPTMR_CAPNF_CAPNFEN_Msk | (7 << LPTMR_CAPNF_CAPNFCNT_Pos) | (7 << LPTMR_CAPNF_CAPNFSEL_Pos)));

        LPTMR_EnableCaptureInputNoiseFilter(LPTMRCh[i], 0, LPTMR_CAPTURE_NOISE_FILTER_PCLK_DIV_1);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CAPNF, (LPTMR_CAPNF_CAPNFEN_Msk | (0 << LPTMR_CAPNF_CAPNFCNT_Pos) | (0 << LPTMR_CAPNF_CAPNFSEL_Pos)));

        LPTMR_DisableCaptureInputNoiseFilter(LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CAPNF, 0x0);
    }
}

void API_LPTMR_EventCounterDebounce(void)
{
    volatile uint32_t i;

    if (LPTMR_InitClock() != 0)
    {
        CU_FAIL("LPTMR Init FAIL");
        return ;
    }

    /* Check LPTMR_EnableEventCounterDebounce and LPTMR_DisableEventCounterDebounce */
    for (i = 0; i < 2; i++)
    {
        printf("LPTMR_EnableEventCounterDebounce (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

        LPTMR_EnableEventCounterDebounce(LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, 0x80);

        LPTMR_DisableEventCounterDebounce(LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, 0x0);
    }
}

void API_LPTMR_OpenAndInterrupt(void)
{
    volatile uint32_t i, j, u32Timeout = 0;

    if (LPTMR_InitClock() != 0)
    {
        CU_FAIL("LPTMR Init FAIL");
        return ;
    }

    /* Check LPTMR_Open, LPTMR_EnableInt and LPTMR_DisableInt*/
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < sizeof(au32OPModeSel) / sizeof(au32OPModeSel[0]); j++)
        {
            printf("OPModeSel %d (Ch-%d)\n", j, i);
            CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

            LPTMR_Open(LPTMRCh[i], au32OPModeSel[j], LPTMR_GetModuleClock(LPTMRCh[i]));
            CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, (j << 27));

            LPTMR_EnableInt(LPTMRCh[i]);
            CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, ((1 << 29) | (j << 27)));

            LPTMR_DisableInt(LPTMRCh[i]);
            CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, (j << 27));

            LPTMR_Close(LPTMRCh[i]);
            CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, 0x0);
            CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, 0x0);
        }
    }
}

void API_LPTMR_CaptureAndInterrupt(void)
{
    volatile uint32_t i, j, u32Timeout = 0;

    if (LPTMR_InitClock() != 0)
    {
        CU_FAIL("LPTMR Init FAIL");
        return ;
    }

    /* Check LPTMR_CaptureSelect */
    for (i = 0; i < 2; i++)
    {
        CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

        LPTMR_CaptureSelect(LPTMRCh[i], LPTMR_CAPTURE_FROM_INTERNAL);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, (1UL << LPTMR_CTL_CAPSRC_Pos));
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, LPTMR_INTERCAPSEL_ACMP0);
        LPTMR_CaptureSelect(LPTMRCh[i], LPTMR_CAPTURE_FROM_EXTERNAL);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, (0UL << LPTMR_CTL_CAPSRC_Pos));
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, 0);
        LPTMR_CaptureSelect(LPTMRCh[i], LPTMR_CAPTURE_FROM_ACMP0);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, (1UL << LPTMR_CTL_CAPSRC_Pos));
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, LPTMR_INTERCAPSEL_ACMP0);
        LPTMR_CaptureSelect(LPTMRCh[i], LPTMR_CAPTURE_FROM_ACMP1);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, (1UL << LPTMR_CTL_CAPSRC_Pos));
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, LPTMR_INTERCAPSEL_ACMP1);
        LPTMR_CaptureSelect(LPTMRCh[i], LPTMR_CAPTURE_FROM_ACMP2);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, (1UL << LPTMR_CTL_CAPSRC_Pos));
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, LPTMR_INTERCAPSEL_ACMP2);
        LPTMR_CaptureSelect(LPTMRCh[i], LPTMR_CAPTURE_FROM_ACMP3);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, (1UL << LPTMR_CTL_CAPSRC_Pos));
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, LPTMR_INTERCAPSEL_ACMP3);
    }

    /* Check Capture Edge Macro - LPTMR_EnableCapture, LPTMR_DisableCapture, LPTMR_EnableCaptureInt, LPTMR_DisableCaptureInt */
    /* With LPTMR_CAPTURE_FREE_COUNTING_MODE */
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < sizeof(au32CapEdgeSel) / sizeof(au32CapEdgeSel[0]); j++)
        {
            if ((j == 4) || (j == 5))
                continue;

            printf("CapEdgeSel %d (Ch-%d)\n", j, i);
            CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

            LPTMR_EnableCapture(LPTMRCh[i], LPTMR_CAPTURE_FREE_COUNTING_MODE, au32CapEdgeSel[j]);
            LPTMR_EnableCaptureInt(LPTMRCh[i]);
            CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, (0x20 | (0x8 | (j << LPTMR_EXTCTL_CAPEDGE_Pos))));

            LPTMR_DisableCapture(LPTMRCh[i]);
            CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, (0x20 | (j << LPTMR_EXTCTL_CAPEDGE_Pos)));

            LPTMR_DisableCaptureInt(LPTMRCh[i]);
            CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, (j << LPTMR_EXTCTL_CAPEDGE_Pos));
        }
    }

    /* With LPTMR_CAPTURE_COUNTER_RESET_MODE */
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < sizeof(au32CapEdgeSel) / 4; j++)
        {
            if ((j == 4) || (j == 5))
                continue;

            printf("CapEdgeSel %d (Ch-%d)\n", j, i);
            CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

            LPTMR_EnableCapture(LPTMRCh[i], LPTMR_CAPTURE_COUNTER_RESET_MODE, au32CapEdgeSel[j]);
            CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, (0x18 | (j << LPTMR_EXTCTL_CAPEDGE_Pos)));

            LPTMR_DisableCapture(LPTMRCh[i]);
            CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, (0x10 | (j << LPTMR_EXTCTL_CAPEDGE_Pos)));
        }
    }
}

void API_LPTMR_EventCounter(void)
{
    volatile uint32_t i, j, u32Timeout = 0;

    if (LPTMR_InitClock() != 0)
    {
        CU_FAIL("LPTMR Init FAIL");
        return ;
    }

    /* Check Counter Edge Macro - LPTMR_EnableEventCounter, LPTMR_DisableEventCounter */
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < sizeof(au32CntEdgeSel) / 4; j++)
        {
            printf("CntEdgeSel %d (Ch-%d)\n", j, i);
            CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

            LPTMR_EnableEventCounter(LPTMRCh[i], au32CntEdgeSel[j]);
            CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, (j << 0));
            CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, 0x01000000);

            LPTMR_DisableEventCounter(LPTMRCh[i]);
            CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, 0x0);
        }
    }

    /* Check LPTMR_EventCounterSelect */
    for (i = 0; i < 2; i++)
    {
        CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

        LPTMR_EventCounterSelect(LPTMRCh[i], LPTMR_EVENT_COUNTER_SOURCE_TMX);
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, LPTMR_EVENT_COUNTER_SOURCE_TMX);
        LPTMR_EventCounterSelect(LPTMRCh[i], LPTMR_EVENT_COUNTER_SOURCE_ACMP0);
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, LPTMR_EVENT_COUNTER_SOURCE_ACMP0);
        LPTMR_EventCounterSelect(LPTMRCh[i], LPTMR_EVENT_COUNTER_SOURCE_ACMP1);
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, LPTMR_EVENT_COUNTER_SOURCE_ACMP1);
        LPTMR_EventCounterSelect(LPTMRCh[i], LPTMR_EVENT_COUNTER_SOURCE_ACMP2);
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, LPTMR_EVENT_COUNTER_SOURCE_ACMP2);
        LPTMR_EventCounterSelect(LPTMRCh[i], LPTMR_EVENT_COUNTER_SOURCE_ACMP3);
        CU_ASSERT_EQUAL(LPTMRCh[i]->EXTCTL, LPTMR_EVENT_COUNTER_SOURCE_ACMP3);
    }

    /* With LPTMR_SELECT_TOUT_PIN */
    for (i = 0; i < 2; i++)
    {
        LPTMR_SELECT_TOUT_PIN(LPTMRCh[i], LPTMR_TOUT_PIN_FROM_TX_EXT);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL & BIT21, BIT21);

        LPTMR_SELECT_TOUT_PIN(LPTMRCh[i], LPTMR_TOUT_PIN_FROM_TX);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL & BIT21, 0);
    }
}

void API_LPTMR_Delay(void)
{
    volatile uint32_t i, u32TDR[3];

    if (LPTMR_InitClock() != 0)
    {
        CU_FAIL("LPTMR Init FAIL");
        return ;
    }

    /* Check LPTMR_Delay */
    for (i = 0; i < 2; i++)
    {
        CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

        if (i == 0)
        {
            LPTMR1->CMP = 0xFFFFFF;
            LPTMR1->CTL = LPTMR_PERIODIC_MODE | LPTMR_CTL_CNTEN_Msk | (12 - 1);

            LPTMR1->CMP = 0xFFFFFF;
            LPTMR_Delay(LPTMRCh[i], 1000);
            u32TDR[0] = LPTMR_GetCounter(LPTMR1);

            LPTMR_ResetCounter(LPTMR1);
            LPTMR_Delay(LPTMRCh[i], 500);
            u32TDR[1] = LPTMR_GetCounter(LPTMR1);

            LPTMR1->CMP = 0xFFFFFF;
            LPTMR_Delay(LPTMRCh[i], 100);
            u32TDR[2] = LPTMR_GetCounter(LPTMR1);
            printf("LPTMR[%d] [%d] [%d] [%d]\n", i, u32TDR[0], u32TDR[1], u32TDR[2]);
        }
        else
        {
            LPTMR_ResetCounter(LPTMR0);
            LPTMR0->CTL = LPTMR_PERIODIC_MODE | LPTMR_CTL_CNTEN_Msk | (12 - 1);

            LPTMR0->CMP = 0xFFFFFF;
            LPTMR_Delay(LPTMRCh[i], 1000);
            u32TDR[0] = LPTMR_GetCounter(LPTMR0);

            LPTMR0->CMP = 0xFFFFFF;
            LPTMR_Delay(LPTMRCh[i], 500);
            u32TDR[1] = LPTMR_GetCounter(LPTMR0);

            LPTMR0->CMP = 0xFFFFFF;
            LPTMR_Delay(LPTMRCh[i], 100);
            u32TDR[2] = LPTMR_GetCounter(LPTMR0);
            printf("LPTMR[%d] [%d] [%d] [%d]\n", i, u32TDR[0], u32TDR[1], u32TDR[2]);
        }

        //        if (((u32TDR[0] > 1200) || (u32TDR[0] <= 1000)) ||
        //                ((u32TDR[1] > 502500) || (u32TDR[1] <= 500000)) ||
        //                ((u32TDR[2] > 1005000) || (u32TDR[2] <= 1000000)))
        //        {
        //            CU_FAIL("LPTMR counter value FAIL");
        //            break;
        //        }
    }

    LPTMR_Stop(LPTMR0);
    LPTMR_Stop(LPTMR1);
}

void API_LPTMR_Wakeup(void)
{
    volatile uint32_t i, u32TDR;

    if (LPTMR_InitClock() != 0)
    {
        CU_FAIL("LPTMR Init FAIL");
        return ;
    }

    /* Check LPTMR_EnableWakeup and LPTMR_DisableWakeup */
    for (i = 0; i < 2; i++)
    {
        printf("LPTMR_EnableWakeup (Ch-%d)\n", i);
        CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[i]), 0);

        LPTMR_EnableWakeup(LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, BIT23 | BIT16);

        LPTMR_DisableWakeup(LPTMRCh[i]);
        CU_ASSERT_EQUAL(LPTMRCh[i]->CTL, BIT16);
    }

    /* Enable Internal RC 32KHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);
    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);

    CLK->LPTMRSEL = (CLK->LPTMRSEL & ~(CLK_LPTMRSEL_LPTMR0SEL_Msk | CLK_LPTMRSEL_LPTMR1SEL_Msk)) |
                    (CLK_LPTMRSEL_LPTMR0SEL_LIRC | CLK_LPTMRSEL_LPTMR1SEL_LIRC);

    CU_ASSERT_EQUAL(ClearLPTMRRegs(LPTMRCh[0]), 0);

    NVIC_EnableIRQ(LPTMR0_IRQn);
    NVIC_EnableIRQ(LPTMR1_IRQn);

    /* Check LPTMR_GetIntFlag, LPTMR_ClearIntFlag, LPTMR_GetWakeupFlag, LPTMR_ClearWakeupFlag and LPTMR_GetCounter */
    for (i = 0; i < 2; i++)
    {
        LPTMRCh[i]->CMP = 10;
        LPTMRCh[i]->CTL = LPTMR_CONTINUOUS_MODE | LPTMR_CTL_INTEN_Msk;
        LPTMR_EnableWakeup(LPTMRCh[i]);

        g_u8IsLPTMRWKF[i] = 0;

        LPTMR_Start(LPTMRCh[i]);

        PMC_PowerDown();

        while (g_u8IsLPTMRWKF[i] == 0);

        u32TDR = LPTMR_GetCounter(LPTMRCh[i]);

        if (u32TDR > 15)
        {
            CU_FAIL("LPTMR counter value FAIL");
            break;
        }

        printf(" [%d]", u32TDR);

        LPTMR_Stop(LPTMRCh[i]);
    }

    NVIC_DisableIRQ(LPTMR0_IRQn);
    NVIC_DisableIRQ(LPTMR1_IRQn);
    LPTMR_Stop(LPTMR0);
    LPTMR_Stop(LPTMR1);
}

void API_LPTMR_GetModuleClock(void)
{
    volatile uint32_t i, j, u32Timeout = 0;

    if (LPTMR_InitClock() != 0)
    {
        CU_FAIL("LPTMR Init FAIL");
        return ;
    }

    CLK_EnableXtalRC(CLK_SRCCTL_MIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_MIRCSTB_Msk);

    /* Check LPTMR_GetModuleClock */
    CLK->LPTMRSEL = (CLK->LPTMRSEL & ~(CLK_LPTMRSEL_LPTMR0SEL_Msk | CLK_LPTMRSEL_LPTMR1SEL_Msk)) |
                    (CLK_LPTMRSEL_LPTMR0SEL_MIRC | CLK_LPTMRSEL_LPTMR1SEL_MIRC);
    CU_ASSERT_EQUAL(LPTMR_GetModuleClock(LPTMR0), __MIRC);
    CU_ASSERT_EQUAL(LPTMR_GetModuleClock(LPTMR1), __MIRC);
    printf(" [%d]", LPTMR_GetModuleClock(LPTMR0));

    CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);

    CLK->LPTMRSEL = (CLK->LPTMRSEL & ~(CLK_LPTMRSEL_LPTMR0SEL_Msk | CLK_LPTMRSEL_LPTMR1SEL_Msk)) |
                    (CLK_LPTMRSEL_LPTMR0SEL_LIRC | CLK_LPTMRSEL_LPTMR1SEL_LIRC);
    CU_ASSERT_EQUAL(LPTMR_GetModuleClock(LPTMR0), __LIRC);
    CU_ASSERT_EQUAL(LPTMR_GetModuleClock(LPTMR1), __LIRC);
    printf(" [%d]", LPTMR_GetModuleClock(LPTMR1));

    CLK->LPTMRSEL = (CLK->LPTMRSEL & ~(CLK_LPTMRSEL_LPTMR0SEL_Msk | CLK_LPTMRSEL_LPTMR1SEL_Msk)) |
                    (CLK_LPTMRSEL_LPTMR0SEL_HIRC | CLK_LPTMRSEL_LPTMR1SEL_HIRC);
    CU_ASSERT_EQUAL(LPTMR_GetModuleClock(LPTMR0), __HIRC);
    CU_ASSERT_EQUAL(LPTMR_GetModuleClock(LPTMR1), __HIRC);
    printf(" [%d]", LPTMR_GetModuleClock(LPTMR0));

    CLK->LPTMRSEL = (CLK->LPTMRSEL & ~(CLK_LPTMRSEL_LPTMR0SEL_Msk | CLK_LPTMRSEL_LPTMR1SEL_Msk)) |
                    (CLK_LPTMRSEL_LPTMR0SEL_PCLK4 | CLK_LPTMRSEL_LPTMR1SEL_PCLK4);
    CU_ASSERT_EQUAL(LPTMR_GetModuleClock(LPTMR0), CLK_GetPCLK4Freq());
    CU_ASSERT_EQUAL(LPTMR_GetModuleClock(LPTMR1), CLK_GetPCLK4Freq());
    printf(" [%d]", LPTMR_GetModuleClock(LPTMR1));

    CLK_EnableXtalRC(CLK_SRCCTL_LXTEN_Msk);
    //CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);

    CLK->LPTMRSEL = (CLK->LPTMRSEL & ~(CLK_LPTMRSEL_LPTMR0SEL_Msk | CLK_LPTMRSEL_LPTMR1SEL_Msk)) |
                    (CLK_LPTMRSEL_LPTMR0SEL_LXT | CLK_LPTMRSEL_LPTMR1SEL_LXT);
    CU_ASSERT_EQUAL(LPTMR_GetModuleClock(LPTMR0), __LXT);
    CU_ASSERT_EQUAL(LPTMR_GetModuleClock(LPTMR1), __LXT);
    printf(" [%d]", LPTMR_GetModuleClock(LPTMR0));
}

void API_LPTMR_CaptureStatus(void)
{
    volatile uint32_t i, u32TDR, u32Timeout;

    if (LPTMR_InitClock() != 0)
    {
        CU_FAIL("LPTMR Init FAIL");
        return ;
    }

    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK->LPTMRSEL = (CLK->LPTMRSEL & ~(CLK_LPTMRSEL_LPTMR0SEL_Msk | CLK_LPTMRSEL_LPTMR1SEL_Msk)) |
                    (CLK_LPTMRSEL_LPTMR0SEL_PCLK4 | CLK_LPTMRSEL_LPTMR1SEL_PCLK4);
    /* Enable internal self-test mode */
    ENABLE_GCR_SELFTEST;

    /* Check LPTMR_StartCapture, LPTMR_GetCaptureIntFlag, LPTMR_ClearCaptureIntFlag and LPTMR_GetCaptureData */
    /* Set TM0_EXT ~ TM1_EXT capture pins, PA.11, PA.10 as GPIO output pins */
    GPIO_SetMode(PA, BIT11 | BIT10, GPIO_MODE_OUTPUT);

    /* Set TM0 ~ TM1 capture pins, PB.5, PB.4 as GPIO output pins */
    GPIO_SetMode(PB, BIT5 | BIT4, GPIO_MODE_OUTPUT);
    /* Set PA.11 (T0_EXT pin) as GPIO output mode */
    PA11 = 1;
    PB5 = 1;

    LPTMR0->CMP = 0xFFFFFF;
    LPTMR0->CTL = LPTMR_CONTINUOUS_MODE | LPTMR_CTL_CNTEN_Msk;
    LPTMR_StartCapture(LPTMR0);
    CU_ASSERT_EQUAL((LPTMR0->EXTCTL & LPTMR_EXTCTL_CAPEN_Msk), LPTMR_EXTCTL_CAPEN_Msk);
    LPTMR_EnableCapture(LPTMR0, LPTMR_CAPTURE_FREE_COUNTING_MODE, LPTMR_CAPTURE_EVENT_FALLING_RISING);
    u32Timeout = 0;

    while (LPTMR_GetCaptureIntFlag(LPTMR0) == 1)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("LPTMR0 CAP 01 FAIL");
            goto exit_test;
        }
    }

    LPTMR_ClearCaptureIntFlag(LPTMR0);

    LPTMR0->CMP = 0xFFFFFF;
    __NOP();
    __NOP();
    PA11 = 0;
    PB5 = 0;

    u32Timeout = 0;

    while (LPTMR_GetCaptureIntFlag(LPTMR0) == 0)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("LPTMR0 CAP 02 FAIL");
            goto exit_test;
        }
    }

    CU_ASSERT_EQUAL(LPTMR_GetCaptureIntFlagOV(LPTMR0), 0x0);

    printf(" [%d]", LPTMR_GetCaptureData(LPTMR0));
    CU_ASSERT_EQUAL(LPTMR_GetCaptureData(LPTMR0), 0x0);
    u32Timeout = 0;
    PA11 = 1;
    PB5 = 1;

    while (LPTMR_GetCaptureIntFlagOV(LPTMR0) == 0)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("LPTMR0 CAP 04 FAIL");
            goto exit_test;
        }
    }

    LPTMR_ClearCaptureIntFlag(LPTMR0);
    u32Timeout = 0;

    while (LPTMR_GetCaptureIntFlag(LPTMR0) == 1)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("LPTMR0 CAP 03 FAIL");
            goto exit_test;
        }
    }

    CU_ASSERT_EQUAL(LPTMR_GetCaptureIntFlagOV(LPTMR0), 0x0);

    /* For LPTMR1 CAP */
    PA10 = 1;
    PB4 = 1;
    LPTMR1->CMP = 0xFFFFFF;
    LPTMR1->CTL = LPTMR_CONTINUOUS_MODE | LPTMR_CTL_CNTEN_Msk;
    LPTMR_StartCapture(LPTMR1);
    CU_ASSERT_EQUAL((LPTMR1->EXTCTL & LPTMR_EXTCTL_CAPEN_Msk), LPTMR_EXTCTL_CAPEN_Msk);
    LPTMR_EnableCapture(LPTMR1, LPTMR_CAPTURE_FREE_COUNTING_MODE, LPTMR_CAPTURE_EVENT_FALLING_RISING);
    u32Timeout = 0;

    while (LPTMR_GetCaptureIntFlag(LPTMR1) == 1)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("LPTMR1 CAP 01 FAIL");
            goto exit_test;
        }
    }

    LPTMR_ClearCaptureIntFlag(LPTMR1);

    LPTMR1->CMP = 0xFFFFFF;
    __NOP();
    __NOP();
    PA10 = 0;
    PB4 = 0;

    u32Timeout = 0;

    while (LPTMR_GetCaptureIntFlag(LPTMR1) == 0)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("LPTMR1 CAP 02 FAIL");
            goto exit_test;
        }
    }

    CU_ASSERT_EQUAL(LPTMR_GetCaptureIntFlagOV(LPTMR1), 0x0);

    printf(" [%d]", LPTMR_GetCaptureData(LPTMR1));
    CU_ASSERT_EQUAL(LPTMR_GetCaptureData(LPTMR1), 0x0);
    u32Timeout = 0;
    PA10 = 1;
    PB4 = 1;

    while (LPTMR_GetCaptureIntFlagOV(LPTMR1) == 0)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("LPTMR1 CAP 04 FAIL");
            goto exit_test;
        }
    }

    LPTMR_ClearCaptureIntFlag(LPTMR1);
    u32Timeout = 0;

    while (LPTMR_GetCaptureIntFlag(LPTMR1) == 1)
    {
        if (u32Timeout++ >= (SystemCoreClock / 10000))
        {
            CU_FAIL("LPTMR1 CAP 03 FAIL");
            goto exit_test;
        }
    }

    CU_ASSERT_EQUAL(LPTMR_GetCaptureIntFlagOV(LPTMR1), 0x0);

    /* Check LPTMR_StopCapture */
    LPTMR_StopCapture(LPTMR0);
    CU_ASSERT_EQUAL((LPTMR0->EXTCTL & LPTMR_EXTCTL_CAPEN_Msk), 0x0);
    LPTMR_StopCapture(LPTMR1);
    CU_ASSERT_EQUAL((LPTMR1->EXTCTL & LPTMR_EXTCTL_CAPEN_Msk), 0x0);

exit_test:
    LPTMR_Stop(LPTMR0);
    LPTMR_Stop(LPTMR1);

    /* Disable internal self-test mode */
    DISABLE_GCR_SELFTEST;
}

CU_SuiteInfo LPTMRSuites[] =
{
    { "LPTMR Const Test", LPTMR_Test_Init, LPTMR_Test_Clean, NULL, NULL, LPTMR_ConstTest },
    { "LPTMR Macro Test", LPTMR_Test_Init, LPTMR_Test_Clean, NULL, NULL, LPTMR_MacroTest },
    { "LPTMR Func  Test", LPTMR_Test_Init, LPTMR_Test_Clean, NULL, NULL, LPTMR_FuncTest },
    CU_SUITE_INFO_NULL
};

CU_TestInfo  LPTMR_ConstTest[] =
{
    { "Check Operation_Mode",  CONST_Operation_Mode },
    { "Check Event_Trigger",  CONST_Event_Trigger },
    { "Check Toggle_Pin",  CONST_Toggle_Pin },
    { "Check Event_Counter",  CONST_Event_Counter },
    { "Check Capture",  CONST_Capture },
    CU_TEST_INFO_NULL
};

CU_TestInfo  LPTMR_MacroTest[] =
{
    {"Check LPTMR_SET_CMP_VALUE ",      MACRO_LPTMR_SET_CMP_VALUE},
    {"Check LPTMR_SET_PRESCALE_VALUE ", MACRO_LPTMR_SET_PRESCALE_VALUE},
    {"Check LPTMR_SET_OPMODE_VALUE ", MACRO_LPTMR_SET_OPMODE_VALUE},
    {"Check LPTMR_IS_ACTIVE ",          API_LPTMR_APIs},
    CU_TEST_INFO_NULL
};

CU_TestInfo  LPTMR_FuncTest[] =
{
    {"Check LPTMR APIs... ",                                     API_LPTMR_APIs},
    {"Check Enable/Disable Capture Debounce API ",               API_LPTMR_CaptureDebounce},
    {"Check Enable/Disable Event Counter Debounce API ",         API_LPTMR_EventCounterDebounce},
    {"Check LPTMR Open/Close and Enable/Disable Interrupt API ", API_LPTMR_OpenAndInterrupt},
    {"Check Enable/Disable Capture Function and Interrupt API ", API_LPTMR_CaptureAndInterrupt},
    {"Check Enable/Disable Event Counter Function API ",         API_LPTMR_EventCounter},
    {"Check LPTMR Delay API ",                                   API_LPTMR_Delay},
    {"Check Get Module Clock API ",                              API_LPTMR_GetModuleClock},
    {"Check Capture Interrupt Status API ",                      API_LPTMR_CaptureStatus},
    {"Check Interrupt, Wake-up Function and Status API ",        API_LPTMR_Wakeup},
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
