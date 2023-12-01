/******************************************************************************
* @file    rtc_cunit.c
* @version V1.00
* @brief   rtc CUnit Test
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "rtc_cunit.h"

//#define D_msg   printf
#define D_msg(...)
//#define _PZ1_EMU_


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
    {"RTC CONSTANT & MACRO", suite_success_init, suite_success_clean, NULL, NULL, RTC_CONSTANT_MACRO},
    {"RTC API", suite_success_init, suite_success_clean, NULL, NULL, RTC_API},
    CU_SUITE_INFO_NULL
};

void RTC_WaitAccessEnable(void)
{
    // NULL function. NOT support in M2354
}

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
void Timer0_Init(uint32_t u32ClkSrc)
{
    if (u32ClkSrc == CLK_TMRSEL_TMR0SEL_LIRC)
    {
        //CLK->CLKSEL1 = (CLK->CLKSEL1&~CLK_CLKSEL1_TMR0SEL_Msk) | u32ClkSrc;
        CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_LIRC, 0);
    }
    else if(u32ClkSrc == CLK_TMRSEL_TMR0SEL_HIRC)
    {
        //CLK->CLKSEL1 = (CLK->CLKSEL1&~CLK_CLKSEL1_TMR0SEL_Msk) | CLK_CLKSEL1_TMR0SEL_HXT;
        CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC, 0);
    }
    else
    {
        CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HXT, 0);
    }

    /* Enable Clock Timer 0 module plck */
    CLK_EnableModuleClock(TMR0_MODULE);

    TIMER0->CNT = 0;
    TIMER0->CMP = 0xFFFFFF;

    if (u32ClkSrc == CLK_TMRSEL_TMR0SEL_LIRC)
    {
        /* Enable LIRC */
        CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);

        /* Waiting for clock ready */
        CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);
        TIMER0->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk;
    }
    else if (u32ClkSrc == CLK_TMRSEL_TMR0SEL_HIRC)
    {
        TIMER0->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk | (12 - 1);
    }
    else
    {
        TIMER0->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk | (24 - 1);
    }
}
#else
extern void Timer0_Init(uint32_t u32ClkSrc);
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
void Timer0_ResetrCounter(void)
{
    TIMER0->CNT = 0UL;
}
#else
extern void Timer0_ResetrCounter(void);
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
uint32_t Timer0_GetCounter(void)
{
    return TIMER0->CNT;
}
#else
extern uint32_t Timer0_GetCounter(void);
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
void NSC_NVIC_EnableIRQ(IRQn_Type IRQn)
{
    NVIC_EnableIRQ(IRQn);
}
#else
extern void NSC_NVIC_EnableIRQ(IRQn_Type IRQn);
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
void NSC_NVIC_DisableIRQ(IRQn_Type IRQn)
{
    NVIC_DisableIRQ(IRQn);
}
#else
extern void NSC_NVIC_DisableIRQ(IRQn_Type IRQn);
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
int32_t RTC_InitClock(void)
{
    volatile uint32_t u32Timeout = SystemCoreClock / 10;

    Timer0_Init(CLK_TMRSEL_TMR0SEL_HXT);

    /* Enable LXT */
    CLK_EnableXtalRC(CLK_SRCCTL_LXTEN_Msk);

    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);

    /* Waiting for clock ready */
    CLK_EnableModuleClock(RTC0_MODULE);

    return 0;
}
#else
extern int32_t RTC_InitClock(void);
#endif

volatile uint32_t gu32TickINT = 0;
NVT_ITCM void RTC_IRQHandler(void)
{
    /* To check if RTC TICK interrupt occurred */
    if (RTC_GET_TICK_INT_FLAG(RTC) == 1)
    {
        /* Clear RTC TICK interrupt flag */
        RTC_CLEAR_TICK_INT_FLAG(RTC);
        D_msg("TICK\n");
        gu32TickINT = 1;
    }
}

NVT_ITCM void RTCTAMPER_IRQHandler(void)
{
    RTC_T *pRTC;

    //    if((__PC()&NS_OFFSET) == NS_OFFSET)
    //        pRTC = RTC_NS;
    //    else
    pRTC = RTC;

    /* To check if RTC snooper pin interrupt occurred */
    if (RTC_GET_TAMPER_INT_STATUS(pRTC) == 1)
    {
        /* Clear RTC snooper pin interrupt flag */
        RTC_CLEAR_TAMPER_INT_FLAG(pRTC, RTC_GET_TAMPER_INT_FLAG(pRTC));
    }
}

const uint32_t au32TICKSel[] =
{
    RTC_TICK_1_SEC,
    RTC_TICK_1_2_SEC,
    RTC_TICK_1_4_SEC,
    RTC_TICK_1_8_SEC,
    RTC_TICK_1_16_SEC,
    RTC_TICK_1_32_SEC,
    RTC_TICK_1_64_SEC,
    RTC_TICK_1_128_SEC,
};

const uint32_t au32WEEKDAYSel[] =
{
    RTC_SUNDAY,
    RTC_MONDAY,
    RTC_TUESDAY,
    RTC_WEDNESDAY,
    RTC_THURSDAY,
    RTC_FRIDAY,
    RTC_SATURDAY,
};

const uint32_t au32SeedChangeRate[] =
{
    RTC_2POW10_CLK,
    RTC_2POW11_CLK,
    RTC_2POW12_CLK,
    RTC_2POW13_CLK,
    RTC_2POW14_CLK,
    RTC_2POW15_CLK,
    RTC_2POW16_CLK,
    RTC_2POW17_CLK,
};

const uint32_t u32RefPattern[] =
{
    RTC_REF_RANDOM_PATTERN,
    RTC_REF_SEED_VALUE,
};

const uint32_t au32SPRDataPattern[] =
{
    0x55555555, 0xAAAAAAAA,
    0xFFFFFFFF, 0x00000000,
};

void CONSTANT_RTC(void)
{
    volatile uint32_t i = 0;
    volatile uint32_t u32Timeout;
    RTC_T *pRTC;

    //    if((__PC()&NS_OFFSET) == NS_OFFSET)
    //        pRTC = RTC_NS;
    //    else
    pRTC = RTC;

    /* Init RTC clock */
    CU_ASSERT_EQUAL(RTC_InitClock(), 0);

    CU_ASSERT_EQUAL(RTC_YEAR2000, 2000);
    CU_ASSERT_EQUAL(RTC_FCR_REFERENCE, 32752);

    /* Check RTC_INIT_KEY and RTC_WRITE_KEY Constant */
    u32Timeout = SystemCoreClock / 10;
    pRTC->INIT = RTC_INIT_KEY;

    while (pRTC->INIT != RTC_INIT_ACTIVE_Msk)
    {
        if (u32Timeout-- == 0)
        {
            CU_FAIL("INIT FAIL");
            return ;
        }
    }

    //        u32Timeout = SystemCoreClock/1000;
    //        while((pRTC->RWEN & RTC_RWEN_RWENF_Msk) != RTC_RWEN_RWENF_Msk) {
    //            if(u32Timeout-- == 0) {
    //                CU_FAIL("Default RWENF FAIL");
    //                break;
    //            }
    //        }
    //        u32Timeout = SystemCoreClock/1000;
    //        pRTC->RWEN = RTC_WRITE_KEY;
    //        while((pRTC->RWEN & RTC_RWEN_RWENF_Msk) == 0x0) {
    //            if(u32Timeout-- == 0) {
    //                CU_FAIL("Enable RWENF FAIL");
    //                break;
    //            }
    //        }

    /* Check RTC Tick Period Constant; RTC_SetTickPeriod()*/
    for (i = 0; i < (sizeof(au32TICKSel) / sizeof(au32TICKSel[0])); i++)
    {
        D_msg("TICK Sel %d\n", i);
        RTC_SetTickPeriod(au32TICKSel[i]);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL(pRTC->TICK, (i << RTC_TICK_TICK_Pos));
    }

    /* Check WEEKDAY Constant */
    for (i = 0; i < (sizeof(au32WEEKDAYSel) / sizeof(au32WEEKDAYSel[0])); i++)
    {
        D_msg("WEEKDAY Sel %d\n", i);
        RTC_WaitAccessEnable();
        pRTC->WEEKDAY = au32WEEKDAYSel[i] << RTC_WEEKDAY_WEEKDAY_Pos;
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL(pRTC->WEEKDAY, (i << RTC_WEEKDAY_WEEKDAY_Pos));
    }

    /* Check Snooper Detection Mode Constant;RTC_EnableSnooperDetection() */
    CU_ASSERT_EQUAL(MAX_TAMPER_PIN_NUM, 6);
    {
        uint32_t u32TamperSel = (RTC_TAMPER0_SELECT | RTC_TAMPER1_SELECT | RTC_TAMPER2_SELECT |
                                 RTC_TAMPER3_SELECT | RTC_TAMPER4_SELECT | RTC_TAMPER5_SELECT);
        uint32_t u32DetLevel = RTC_TAMPER_HIGH_LEVEL_DETECT;
        uint32_t u32Debounce = RTC_TAMPER_DEBOUNCE_ENABLE;

        RTC_StaticTamperDisable(u32TamperSel);

        RTC_StaticTamperEnable(u32TamperSel, u32DetLevel, u32Debounce);

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP0EN_Msk), (0x1 << 8));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP1EN_Msk), (0x1 << 12));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP2EN_Msk), (0x1 << 16));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP3EN_Msk), (0x1 << 20));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP4EN_Msk), (0x1 << 24));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP5EN_Msk), (0x1 << 28));

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP0LV_Msk), (0x1 << 9));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP1LV_Msk), (0x1 << 13));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP2LV_Msk), (0x1 << 17));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP3LV_Msk), (0x1 << 21));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP4LV_Msk), (0x1 << 25));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP5LV_Msk), (0x1 << 29));

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP0DEN_Msk), (0x1 << 10));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP1DEN_Msk), (0x1 << 14));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP2DEN_Msk), (0x1 << 18));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP3DEN_Msk), (0x1 << 22));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP4DEN_Msk), (0x1 << 26));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP5DEN_Msk), (0x1 << 30));

        RTC_StaticTamperDisable(u32TamperSel);

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP0EN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP1EN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP2EN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP3EN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP4EN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP5EN_Msk), 0);

        u32DetLevel = RTC_TAMPER_LOW_LEVEL_DETECT;
        u32Debounce = RTC_TAMPER_DEBOUNCE_DISABLE;

        RTC_StaticTamperEnable(u32TamperSel, u32DetLevel, u32Debounce);

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP0LV_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP1LV_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP2LV_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP3LV_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP4LV_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP5LV_Msk), 0);

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP0DEN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP1DEN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP2DEN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP3DEN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP4DEN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP5DEN_Msk), 0);

        RTC_StaticTamperDisable(u32TamperSel);

        /* To check if RTC snooper pin interrupt occurred */
        if (RTC_GET_TAMPER_INT_FLAG(pRTC))
        {
            D_msg("\n        * Get TAMPER FLAGs: 0x%x", RTC_GET_TAMPER_INT_STATUS(pRTC));
            /* Clear RTC snooper pin interrupt flag */

            RTC_CLEAR_TAMPER_INT_FLAG(pRTC, RTC_GET_TAMPER_INT_STATUS(pRTC));
            CU_ASSERT_EQUAL(RTC_GET_TAMPER_INT_STATUS(pRTC), 0);
        }

        D_msg("\n        * Get TAMPER FLAGs: 0x%x", RTC_GET_TAMPER_INT_STATUS(pRTC));
        RTC_StaticTamperDisable(u32TamperSel);
    }

    CU_ASSERT_EQUAL(MAX_PAIR_NUM, 3);
    {
        uint32_t u32PairSel = (RTC_PAIR0_SELECT | RTC_PAIR1_SELECT | RTC_PAIR2_SELECT);
        uint32_t u32Pair1Source = 0;
        uint32_t u32Pair2Source = 0;
        uint32_t u32Debounce_ = RTC_TAMPER_DEBOUNCE_ENABLE;

        RTC_DynamicTamperEnable(u32PairSel, u32Debounce_, u32Pair1Source, u32Pair2Source);

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP0EN_Msk), (0x1 << 8));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP1EN_Msk), (0x1 << 12));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP2EN_Msk), (0x1 << 16));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP3EN_Msk), (0x1 << 20));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP4EN_Msk), (0x1 << 24));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP5EN_Msk), (0x1 << 28));

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP0DEN_Msk), (0x1 << 10));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP1DEN_Msk), (0x1 << 14));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP2DEN_Msk), (0x1 << 18));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP3DEN_Msk), (0x1 << 22));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP4DEN_Msk), (0x1 << 26));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP5DEN_Msk), (0x1 << 30));

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYNPR0EN_Msk), (0x1 << 15));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYNPR1EN_Msk), (0x1 << 23));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYNPR2EN_Msk), (0x1ul << 31));

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYN1ISS_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYN2ISS_Msk), 0);

        RTC_DynamicTamperDisable(u32PairSel);

        /* To check if RTC snooper pin interrupt occurred */
        if (RTC_GET_TAMPER_INT_FLAG(pRTC))
        {
            D_msg("\n        * Get TAMPER FLAGs: 0x%x", RTC_GET_TAMPER_INT_STATUS(pRTC));
            /* Clear RTC snooper pin interrupt flag */

            RTC_CLEAR_TAMPER_INT_FLAG(pRTC, RTC_GET_TAMPER_INT_STATUS(pRTC));
            CU_ASSERT_EQUAL(RTC_GET_TAMPER_INT_STATUS(pRTC), 0);
        }

        D_msg("\n        * Get TAMPER FLAGs: 0x%x", RTC_GET_TAMPER_INT_STATUS(pRTC));
        RTC_DynamicTamperDisable(u32PairSel);

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP0EN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP1EN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP2EN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP3EN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP4EN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP5EN_Msk), 0);

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYNPR0EN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYNPR1EN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYNPR2EN_Msk), 0);

        u32Pair1Source = 1;
        u32Pair2Source = 1;
        u32Debounce_ = RTC_TAMPER_DEBOUNCE_DISABLE;

        RTC_DynamicTamperEnable(u32PairSel, u32Debounce_, u32Pair1Source, u32Pair2Source);

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP0EN_Msk), (0x1 << 8));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP1EN_Msk), (0x1 << 12));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP2EN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP3EN_Msk), (0x1 << 20));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP4EN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP5EN_Msk), (0x1 << 28));

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP0DEN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP1DEN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP2DEN_Msk), (0x1 << 18));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP3DEN_Msk), 0);
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP4DEN_Msk), (0x1 << 26));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_TAMP5DEN_Msk), 0);

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYNPR0EN_Msk), (0x1 << 15));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYNPR1EN_Msk), (0x1 << 23));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYNPR2EN_Msk), (0x1ul << 31));

        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYN1ISS_Msk), (0x1 << 0));
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYN2ISS_Msk), (0x1 << 1));
    }

    {
        for (i = 0; i < 8; i++)
        {
            RTC_DynamicTamperConfig(au32SeedChangeRate[i], 0, 0, 0);

            RTC_WaitAccessEnable();
            CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYNRATE_Msk), (i << 5));
        }

#if 0 /* seed reload bit will auto-clear, can not check this bit */

        for (i = 0; i < 2; i++)
        {
            RTC_DynamicTamperConfig(au32SeedChangeRate[0], i, 0, 0);

            RTC_WaitAccessEnable();
            CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_SEEDRLD_Msk), (0x0 << 4));
        }

#endif

        for (i = 0; i < 2; i++)
        {
            RTC_DynamicTamperConfig(au32SeedChangeRate[0], 0, u32RefPattern[i], 0);

            RTC_WaitAccessEnable();

            if (u32RefPattern[i] == RTC_REF_SEED_VALUE)
            {
                CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYNSRC_Msk), (0x1 << 3));
            }

            if (u32RefPattern[i] == RTC_REF_RANDOM_PATTERN)
            {
                CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYNSRC_Msk), (0x0 << 3));
            }

            //                if(u32RefPattern[i] == REF_PREVIOUS_PATTERN)
            //                {
            //                    CU_ASSERT_EQUAL((pRTC->TAMPCTL & RTC_TAMPCTL_DYNSRC_Msk), (0x1 << 2));
            //                }
        }

        for (i = 0; i < 5; i++)
        {
            RTC_DynamicTamperConfig(au32SeedChangeRate[0], 0, 0, (i * 5));

            RTC_WaitAccessEnable();
            CU_ASSERT_EQUAL((pRTC->TAMPSEED & RTC_TAMPSEED_SEED_Msk), (i * 5));
        }
    }

    RTC_EnableSpareAccess();
    pRTC->TAMPCTL = 0;
    pRTC->INTSTS = 0x3f00;
}

void MACRO_RTC(void)
{
    volatile uint32_t i = 0, j = 0;
    volatile uint32_t u32Value, u32Timeout;
    S_RTC_TIME_DATA_T sWriteRTC, sReadRTC;
    RTC_T *pRTC;

    //    if((__PC()&NS_OFFSET) == NS_OFFSET)
    //        pRTC = RTC_NS;
    //    else
    pRTC = RTC;

    /* Init RTC clock */
    CU_ASSERT_EQUAL(RTC_InitClock(), 0);

    u32Timeout = SystemCoreClock / 1000;
    pRTC->INIT = RTC_INIT_KEY;

    while (pRTC->INIT != RTC_INIT_ACTIVE_Msk)
    {
        if (u32Timeout-- == 0)
        {
            CU_FAIL("INIT FAIL");
            return ;
        }
    }

    /* Check RTC_IS_LEAP_YEAR Macro; RTC_SetDate() */
    RTC_SetDate(2016, 1, 24, RTC_SUNDAY);
    RTC_WaitAccessEnable();
    CU_ASSERT_EQUAL(pRTC->LEAPYEAR, 1);
    RTC_SetDate(2015, 1, 24, RTC_SATURDAY);
    RTC_WaitAccessEnable();
    CU_ASSERT_EQUAL(pRTC->LEAPYEAR, 0);

    /* Check RTC_GET_TICK_INT_FLAG and RTC_CLEAR_TICK_INT_FLAG Macro; RTC_SetTickPeriod()*/
    u32Timeout = SystemCoreClock / 100;
    RTC_SetTickPeriod(RTC_TICK_1_128_SEC);

    while (1)
    {
        if (RTC_GET_TICK_INT_FLAG(pRTC) == 1)
        {
            RTC_CLEAR_TICK_INT_FLAG(pRTC);
            break;
        }

        if (u32Timeout-- == 0)
        {
            CU_FAIL("RTC_GET_TICK_INT_FLAG FAIL");
            return ;
        }
    }

    while (RTC_GET_TICK_INT_FLAG(pRTC) == 0);

    RTC_CLEAR_TICK_INT_FLAG(pRTC);
    Timer0_ResetrCounter();
    CU_ASSERT_EQUAL(RTC_GET_TICK_INT_FLAG(pRTC), 0);

    u32Timeout = SystemCoreClock / 100;

    while (1)
    {
        if (RTC_GET_TICK_INT_FLAG(pRTC) == 1)
        {
            RTC_CLEAR_TICK_INT_FLAG(pRTC);
            break;
        }

        if (u32Timeout-- == 0)
        {
            CU_FAIL("RTC_GET_TICK_INT_FLAG FAIL");
            return ;
        }
    }

    u32Value = Timer0_GetCounter();
    D_msg("CNT: %d\n", u32Value);
#ifdef _PZ1_EMU_ // in pz1 LXT is 3.276Mhz

    if ((u32Value > (78 + 5)) || (u32Value < (78 - 5)))
    {
        CU_FAIL("TICK Period FAIL");
        return ;
    }

#else //for real chip  32.768Khz

    if ((u32Value > (7812 + 50)) || (u32Value < (7812 - 50)))
    {
        CU_FAIL("TICK Period FAIL");
        return ;
    }

#endif
    RTC_WaitAccessEnable();
    pRTC->TICK = RTC_TICK_1_SEC << RTC_TICK_TICK_Pos;

    RTC_WaitAccessEnable();
    //from RTC set time to start conmt, need 2*(1/128) second.
    pRTC->CAL  = 0x00140301;
    pRTC->TIME = 0x00020101;
    /* Check RTC_GET_ALARM_INT_FLAG and RTC_CLEAR_ALARM_INT_FLAG Macro; RTC_SetAlarmDateAndTime(), RTC_GetAlarmDateAndTime() */
    u32Timeout = SystemCoreClock / 2;
    RTC_CLEAR_ALARM_INT_FLAG(pRTC);
    RTC_WaitAccessEnable();
    pRTC->CALM = 0x00140301;
    RTC_WaitAccessEnable();
    pRTC->TALM = 0x00020102;

    while (RTC_GET_ALARM_INT_FLAG(pRTC) == 0);

    RTC_CLEAR_ALARM_INT_FLAG(pRTC);
    Timer0_ResetrCounter();

    sWriteRTC.u32Year       = 2014;
    sWriteRTC.u32Month      = 3;
    sWriteRTC.u32Day        = 1;
    sWriteRTC.u32Hour       = 2;
    sWriteRTC.u32Minute     = 1;
    sWriteRTC.u32Second     = 3;
    sWriteRTC.u32TimeScale  = RTC_CLOCK_24;
    RTC_SetAlarmDateAndTime(&sWriteRTC);

    while (1)
    {
        if (RTC_GET_ALARM_INT_FLAG(pRTC) == 1)
        {
            RTC_CLEAR_ALARM_INT_FLAG(pRTC);
            break;
        }

        if (u32Timeout-- == 0)
        {
            CU_FAIL("RTC_GET_ALARM_INT_FLAG FAIL");
            return ;
        }

    }

    u32Value = Timer0_GetCounter();
    CU_ASSERT_EQUAL(RTC_GET_ALARM_INT_FLAG(pRTC), 0);
    D_msg("CNT: %d\n", u32Value);
#ifdef _PZ1_EMU_ // in pz1 LXT is 3.276Mhz

    if ((u32Value > (10000 + 20)) || (u32Value < (10000 - 20)))
    {
        CU_FAIL("ALARM Period FAIL");
        return ;
    }

#else

    if ((u32Value > (1000000 + 200)) || (u32Value < (1000000 - 200)))
    {
        CU_FAIL("ALARM Period FAIL");
        return ;
    }

#endif
    //printf("Check Data\r\n");
    RTC_WaitAccessEnable();
    CU_ASSERT_EQUAL(pRTC->CALM, 0x00140301);
    CU_ASSERT_EQUAL(pRTC->TALM, 0x00020103);
    RTC_GetAlarmDateAndTime(&sReadRTC);
    CU_ASSERT_EQUAL(sReadRTC.u32Year,   2014);
    CU_ASSERT_EQUAL(sReadRTC.u32Month,  3);
    CU_ASSERT_EQUAL(sReadRTC.u32Day,    1);
    CU_ASSERT_EQUAL(sReadRTC.u32Hour,   2);
    CU_ASSERT_EQUAL(sReadRTC.u32Minute, 1);
    CU_ASSERT_EQUAL(sReadRTC.u32Second, 3);
    CU_ASSERT_EQUAL(sReadRTC.u32TimeScale, RTC_CLOCK_24);
    D_msg("%d/%02d/%02d %02d:%02d:%02d\n",
          sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);

    /* Check RTC_ENABLE_TICK_WAKEUP, RTC_DISABLE_TICK_WAKEUP */
    RTC_ENABLE_TICK_WAKEUP(pRTC);
    CU_ASSERT_EQUAL((pRTC->INTEN & BIT1), BIT1);
    RTC_DISABLE_TICK_WAKEUP(pRTC);
    CU_ASSERT_EQUAL((pRTC->INTEN & BIT1), 0x0);

    /* Check RTC_WRITE_SPARE_REGISTER and RTC_READ_SPARE_REGISTER Macro; RTC_EnableSpareAccess(), RTC_DisableSpareRegister() */
    RTC_EnableSpareAccess();
    CU_ASSERT_EQUAL(pRTC->SPRCTL, (pRTC->SPRCTL | RTC_SPRCTL_SPRRWEN_Msk));
    RTC_EnableSpareAccess();
    pRTC->TAMPCTL = 0;
    pRTC->SPRCTL = pRTC->SPRCTL;
    D_msg("\n RTC->SPRCTL = 0x%x ", pRTC->SPRCTL);
    D_msg("\n RTC->TAMPCTL = 0x%x ", pRTC->TAMPCTL);

    for (i = 0; i < (sizeof(au32SPRDataPattern) / sizeof(au32SPRDataPattern[0])); i++)
    {
        for (j = 0; j < 20; j++)
        {
            RTC_WaitAccessEnable();
            RTC_WRITE_SPARE_REGISTER(pRTC, j, au32SPRDataPattern[i]);
            RTC_WaitAccessEnable();
            CU_ASSERT_EQUAL(RTC_READ_SPARE_REGISTER(pRTC, j), au32SPRDataPattern[i]);
            D_msg("SPARE[%d]:0x%x\r\n", j, RTC_READ_SPARE_REGISTER(pRTC, j));
        }
    }

    RTC_WaitAccessEnable();
    RTC_WRITE_SPARE_REGISTER(pRTC, 0, 0x5A5AA5A5);
    RTC_WaitAccessEnable();
    CU_ASSERT_EQUAL(RTC_READ_SPARE_REGISTER(pRTC, 0), 0x5A5AA5A5);
    RTC_DisableSpareRegister();
    RTC_WaitAccessEnable();
    RTC_WRITE_SPARE_REGISTER(pRTC, 0, 0x5555AAAA);
    RTC_EnableSpareAccess();
    RTC_WaitAccessEnable();
    CU_ASSERT_EQUAL(RTC_READ_SPARE_REGISTER(pRTC, 0), 0x5A5AA5A5);
    RTC_DisableSpareRegister();
}

void API_RTC_DataTime_Func(void)
{
    volatile uint32_t i = 0, j = 0;
    volatile uint32_t u32Value, u32Timeout;
    S_RTC_TIME_DATA_T sWriteRTC, sReadRTC;
    RTC_T *pRTC;

    //    if((__PC()&NS_OFFSET) == NS_OFFSET)
    //        pRTC = RTC_NS;
    //    else
    pRTC = RTC;

    /* Init RTC clock */
    CU_ASSERT_EQUAL(RTC_InitClock(), 0);

    u32Timeout = SystemCoreClock / 1000;
    pRTC->INIT = RTC_INIT_KEY;

    while (pRTC->INIT != RTC_INIT_ACTIVE_Msk)
    {
        if (u32Timeout-- == 0)
        {
            CU_FAIL("INIT FAIL");
            return ;
        }
    }

    /* Date/Time 1: 24-Hour */
    /* Check RTC_Open(); Open RTC and start counting */
    sWriteRTC.u32Year       = 2014;
    sWriteRTC.u32Month      = 5;
    sWriteRTC.u32Day        = 15;
    sWriteRTC.u32DayOfWeek  = RTC_THURSDAY;
    sWriteRTC.u32Hour       = 13;
    sWriteRTC.u32Minute     = 14;
    sWriteRTC.u32Second     = 5;
    sWriteRTC.u32TimeScale  = RTC_CLOCK_24;
    RTC_Open(&sWriteRTC);
    RTC_WaitAccessEnable();
    /* Check RTC_GetDateAndTime(); Read current RTC date/time */
    RTC_GetDateAndTime(&sReadRTC);
    CU_ASSERT_EQUAL(sReadRTC.u32Year,   2014);
    CU_ASSERT_EQUAL(sReadRTC.u32Month,  5);
    CU_ASSERT_EQUAL(sReadRTC.u32Day,    15);
    CU_ASSERT_EQUAL(sReadRTC.u32Hour,   13);
    CU_ASSERT_EQUAL(sReadRTC.u32Minute, 14);
    CU_ASSERT_EQUAL(sReadRTC.u32Second, 5);
    CU_ASSERT_EQUAL(sReadRTC.u32DayOfWeek, RTC_THURSDAY);
    CU_ASSERT_EQUAL(sReadRTC.u32TimeScale, RTC_CLOCK_24);
    /* Check RTC_GetDayOfWeek(), RTC_IS_LEAP_YEAR() */
    CU_ASSERT_EQUAL(RTC_GetDayOfWeek(), RTC_THURSDAY);
    CU_ASSERT_EQUAL(RTC_IS_LEAP_YEAR(pRTC), 0);
    D_msg("%d/%02d/%02d %02d:%02d:%02d\n",
          sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);

    /* Date/Time 2: 12-Hour, PM */
    /* Check RTC_Open(); Open RTC and start counting */
    sWriteRTC.u32Year       = 2016;
    sWriteRTC.u32Month      = 1;
    sWriteRTC.u32Day        = 24;
    sWriteRTC.u32DayOfWeek  = RTC_SUNDAY;
    sWriteRTC.u32Hour       = 14;
    sWriteRTC.u32Minute     = 24;
    sWriteRTC.u32Second     = 0;
    sWriteRTC.u32TimeScale  = RTC_CLOCK_12;
    sWriteRTC.u32AmPm       = RTC_PM;
    RTC_Open(&sWriteRTC);
    RTC_WaitAccessEnable();
    /* Check RTC_GetDateAndTime(); Read current RTC date/time */
    RTC_GetDateAndTime(&sReadRTC);
    CU_ASSERT_EQUAL(sReadRTC.u32Year,   2016);
    CU_ASSERT_EQUAL(sReadRTC.u32Month,  1);
    CU_ASSERT_EQUAL(sReadRTC.u32Day,    24);
    CU_ASSERT_EQUAL(sReadRTC.u32Hour,   14);
    CU_ASSERT_EQUAL(sReadRTC.u32Minute, 24);
    CU_ASSERT_EQUAL(sReadRTC.u32Second, 0);
    CU_ASSERT_EQUAL(sReadRTC.u32DayOfWeek, RTC_SUNDAY);
    CU_ASSERT_EQUAL(sReadRTC.u32TimeScale, RTC_CLOCK_12);
    /* Check RTC_GetDayOfWeek(), RTC_IS_LEAP_YEAR() */
    CU_ASSERT_EQUAL(RTC_GetDayOfWeek(), RTC_SUNDAY);
    CU_ASSERT_EQUAL(RTC_IS_LEAP_YEAR(pRTC), 1);
    CU_ASSERT_EQUAL((pRTC->TIME & BIT21), BIT21);
    D_msg("%d/%02d/%02d %02d:%02d:%02d\n",
          sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);

    /* Date/Time 3: 12-Hour, AM */
    /* Check RTC_Open(); Open RTC and start counting */
    sWriteRTC.u32Year       = 2016;
    sWriteRTC.u32Month      = 2;
    sWriteRTC.u32Day        = 28;
    sWriteRTC.u32DayOfWeek  = RTC_SUNDAY;
    sWriteRTC.u32Hour       = 3;
    sWriteRTC.u32Minute     = 30;
    sWriteRTC.u32Second     = 10;
    sWriteRTC.u32TimeScale  = RTC_CLOCK_12;
    sWriteRTC.u32AmPm       = RTC_AM;
    RTC_Open(&sWriteRTC);
    RTC_WaitAccessEnable();
    /* Check RTC_GetDateAndTime(); Read current RTC date/time */
    RTC_GetDateAndTime(&sReadRTC);
    CU_ASSERT_EQUAL(sReadRTC.u32Year,   2016);
    CU_ASSERT_EQUAL(sReadRTC.u32Month,  2);
    CU_ASSERT_EQUAL(sReadRTC.u32Day,    28);
    CU_ASSERT_EQUAL(sReadRTC.u32Hour,   3);
    CU_ASSERT_EQUAL(sReadRTC.u32Minute, 30);
    CU_ASSERT_EQUAL(sReadRTC.u32Second, 10);
    CU_ASSERT_EQUAL(sReadRTC.u32DayOfWeek, RTC_SUNDAY);
    CU_ASSERT_EQUAL(sReadRTC.u32TimeScale, RTC_CLOCK_12);
    /* Check RTC_GetDayOfWeek(), RTC_IS_LEAP_YEAR() */
    CU_ASSERT_EQUAL(RTC_GetDayOfWeek(), RTC_SUNDAY);
    CU_ASSERT_EQUAL(RTC_IS_LEAP_YEAR(pRTC), 1);
    CU_ASSERT_EQUAL((pRTC->TIME & BIT21), 0);
    D_msg("%d/%02d/%02d %02d:%02d:%02d\n",
          sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);

    /* Date/Time 1: 24-Hour */
    /* Check RTC_SetDateAndTime(); Configure RTC and start counting */
    sWriteRTC.u32Year       = 2014;
    sWriteRTC.u32Month      = 5;
    sWriteRTC.u32Day        = 15;
    sWriteRTC.u32DayOfWeek  = RTC_THURSDAY;
    sWriteRTC.u32Hour       = 13;
    sWriteRTC.u32Minute     = 14;
    sWriteRTC.u32Second     = 5;
    sWriteRTC.u32TimeScale  = RTC_CLOCK_24;
    RTC_SetDateAndTime(&sWriteRTC);
    RTC_WaitAccessEnable();
    /* Check RTC_GetDateAndTime(); Read current RTC date/time */
    RTC_GetDateAndTime(&sReadRTC);
    CU_ASSERT_EQUAL(sReadRTC.u32Year,   2014);
    CU_ASSERT_EQUAL(sReadRTC.u32Month,  5);
    CU_ASSERT_EQUAL(sReadRTC.u32Day,    15);
    CU_ASSERT_EQUAL(sReadRTC.u32Hour,   13);
    CU_ASSERT_EQUAL(sReadRTC.u32Minute, 14);
    CU_ASSERT_EQUAL(sReadRTC.u32Second, 5);
    CU_ASSERT_EQUAL(sReadRTC.u32DayOfWeek, RTC_THURSDAY);
    CU_ASSERT_EQUAL(sReadRTC.u32TimeScale, RTC_CLOCK_24);
    /* Check RTC_GetDayOfWeek(), RTC_IS_LEAP_YEAR() */
    CU_ASSERT_EQUAL(RTC_GetDayOfWeek(), RTC_THURSDAY);
    CU_ASSERT_EQUAL(RTC_IS_LEAP_YEAR(pRTC), 0);
    D_msg("%d/%02d/%02d %02d:%02d:%02d\n",
          sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);

    /* Check RTC_SetAlarmDate(), RTC_SetAlarmTime() */
    RTC_SetAlarmDate(2014, 3, 29);
    RTC_SetAlarmTime(5, 6, 7, RTC_CLOCK_24, RTC_AM);
    RTC_GetAlarmDateAndTime(&sReadRTC);
    CU_ASSERT_EQUAL(sReadRTC.u32Year,   2014);
    CU_ASSERT_EQUAL(sReadRTC.u32Month,  3);
    CU_ASSERT_EQUAL(sReadRTC.u32Day,    29);
    CU_ASSERT_EQUAL(sReadRTC.u32Hour,   5);
    CU_ASSERT_EQUAL(sReadRTC.u32Minute, 6);
    CU_ASSERT_EQUAL(sReadRTC.u32Second, 7);
    CU_ASSERT_EQUAL(sReadRTC.u32TimeScale, RTC_CLOCK_24);
    D_msg("%d/%02d/%02d %02d:%02d:%02d\n",
          sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);

    /* Check RTC_EnableInt(), RTC_DisableInt() */
    RTC_EnableInt(RTC_INTEN_ALMIEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, RTC_INTEN_ALMIEN_Msk);
    RTC_DisableInt(RTC_INTEN_ALMIEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, 0);
    RTC_EnableInt(RTC_INTEN_TICKIEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, RTC_INTEN_TICKIEN_Msk);
    RTC_DisableInt(RTC_INTEN_TICKIEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, 0);
    RTC_EnableInt(RTC_INTEN_TAMP0IEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, RTC_INTEN_TAMP0IEN_Msk);
    RTC_DisableInt(RTC_INTEN_TAMP0IEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, 0);
    RTC_EnableInt(RTC_INTEN_TAMP1IEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, RTC_INTEN_TAMP1IEN_Msk);
    RTC_DisableInt(RTC_INTEN_TAMP1IEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, 0);
    RTC_EnableInt(RTC_INTEN_TAMP2IEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, RTC_INTEN_TAMP2IEN_Msk);
    RTC_DisableInt(RTC_INTEN_TAMP2IEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, 0);
    RTC_EnableInt(RTC_INTEN_TAMP3IEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, RTC_INTEN_TAMP3IEN_Msk);
    RTC_DisableInt(RTC_INTEN_TAMP3IEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, 0);
    RTC_EnableInt(RTC_INTEN_TAMP4IEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, RTC_INTEN_TAMP4IEN_Msk);
    RTC_DisableInt(RTC_INTEN_TAMP4IEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, 0);
    RTC_EnableInt(RTC_INTEN_TAMP5IEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, RTC_INTEN_TAMP5IEN_Msk);
    RTC_DisableInt(RTC_INTEN_TAMP5IEN_Msk);
    CU_ASSERT_EQUAL(pRTC->INTEN, 0);

    /* Check RTC_SetTime() */
    RTC_SetTime(13, 5, 6, RTC_CLOCK_24, RTC_PM);
    RTC_WaitAccessEnable();
    RTC_GetDateAndTime(&sReadRTC);
    CU_ASSERT_EQUAL(sReadRTC.u32Hour,   13);
    CU_ASSERT_EQUAL(sReadRTC.u32Minute, 5);
    CU_ASSERT_EQUAL(sReadRTC.u32Second, 6);
    CU_ASSERT_EQUAL(sReadRTC.u32TimeScale, RTC_CLOCK_24);
    
    
    /* Check RTC_32KCalibration() */
    RTC_WaitAccessEnable();
    RTC_32KCalibration(327736500);
    RTC_WaitAccessEnable();
    CU_ASSERT_EQUAL(pRTC->FREQADJ, 0x152a);
    RTC_WaitAccessEnable();
    RTC_32KCalibration(327632500);
    RTC_WaitAccessEnable();
    CU_ASSERT_EQUAL(pRTC->FREQADJ, 0x0B10);
    RTC_WaitAccessEnable();
    RTC_32KCalibration(327680000);
    RTC_WaitAccessEnable();
    CU_ASSERT_EQUAL(pRTC->FREQADJ, 0x1000);
    RTC_WaitAccessEnable();

     /*
    TAMPER_IRQn
    NVIC_EnableIRQ(TAMPER_IRQn);
    */
    /* Check RTC Interrupt behavior */
    D_msg("===>P0\n");
    pRTC->TICK = RTC_TICK_1_128_SEC;
    RTC_CLEAR_TICK_INT_FLAG(RTC);
    D_msg("===>P0\n");
    RTC_EnableInt(RTC_INTEN_TICKIEN_Msk);
    D_msg("===>P0\n");
    NVIC_EnableIRQ(RTC_IRQn);
    D_msg("===>P0\n");
    //RTC_WaitAccessEnable();
    
    
    u32Timeout = SystemCoreClock / 100;

    while (gu32TickINT == 0)
    {
        if (u32Timeout-- == 0)
        {
            CU_FAIL("TICK INT FAIL");
            RTC_Close();
            return ;
        }
    }
    
    D_msg("===>P1\n");

    RTC_DisableInt(RTC_INTEN_TICKIEN_Msk);
    /* Disable RTC NVIC */
    NVIC_DisableIRQ(RTC_IRQn);


    /* Check RTC_SetAlarmDateMask API */
    RTC_SetAlarmDateMask(1, 1, 1, 1, 1, 1);
    CU_ASSERT_EQUAL(pRTC->CAMSK, 0x3F);
    RTC_SetAlarmDateMask(0, 1, 0, 1, 0, 1);
    CU_ASSERT_EQUAL(pRTC->CAMSK, 0x15);
    RTC_SetAlarmDateMask(1, 0, 1, 0, 1, 0);
    CU_ASSERT_EQUAL(pRTC->CAMSK, 0x2A);
    RTC_SetAlarmDateMask(0, 0, 0, 0, 0, 0);
    CU_ASSERT_EQUAL(pRTC->CAMSK, 0x0);

    /* Check RTC_SetAlarmTimeMask API */
    RTC_SetAlarmTimeMask(1, 1, 1, 1, 1, 1);
    CU_ASSERT_EQUAL(pRTC->TAMSK, 0x3F);
    RTC_SetAlarmTimeMask(0, 1, 0, 1, 0, 1);
    CU_ASSERT_EQUAL(pRTC->TAMSK, 0x15);
    RTC_SetAlarmTimeMask(1, 0, 1, 0, 1, 0);
    CU_ASSERT_EQUAL(pRTC->TAMSK, 0x2A);
    RTC_SetAlarmTimeMask(0, 0, 0, 0, 0, 0);
    CU_ASSERT_EQUAL(pRTC->TAMSK, 0x0);

    /* Disable RTC NVIC */
    NSC_NVIC_DisableIRQ(RTC_IRQn);
    pRTC->INTSTS |= pRTC->INTSTS;

    RTC_Close();
    
    D_msg("===>P2\n");
}

const uint32_t au32IOMode[] =
{
    RTC_IO_MODE_INPUT,
    RTC_IO_MODE_OUTPUT,
    RTC_IO_MODE_OPEN_DRAIN,
    RTC_IO_MODE_QUASI,
};

const uint32_t au32IODigit[] =
{
    RTC_IO_DIGITAL_ENABLE,
    RTC_IO_DIGITAL_DISABLE,
};

const uint32_t au32IOPullSel[] =
{
    RTC_IO_PULL_UP_DOWN_DISABLE,
    RTC_IO_PULL_UP_ENABLE,
    RTC_IO_PULL_DOWN_ENABLE,
};

void API_RTC_IO_Control_Func(void)
{
    volatile uint32_t u32Timeout;
    volatile uint32_t pin, mode, digit, pull, level, offset;
    RTC_T *pRTC;

    //    if((__PC()&NS_OFFSET) == NS_OFFSET)
    //        pRTC = RTC_NS;
    //    else
    pRTC = RTC;

    /* Init RTC clock */
    CU_ASSERT_EQUAL(RTC_InitClock(), 0);

    /* Check RTC_INIT_KEY */
    u32Timeout = SystemCoreClock / 10;
    pRTC->INIT = RTC_INIT_KEY;

    while (pRTC->INIT != RTC_INIT_ACTIVE_Msk)
    {
        if (u32Timeout-- == 0)
        {
            CU_FAIL("INIT FAIL");
            return ;
        }
    }

    RTC->GPIOCTL0 = 0x0;
    RTC->GPIOCTL1 = 0x0;
    CU_ASSERT_EQUAL(RTC->GPIOCTL0, 0);
    CU_ASSERT_EQUAL(RTC->GPIOCTL1, 0);

    for (pin = 4; pin <= 11; pin++)
    {
        for (mode = 0; mode < (sizeof(au32IOMode) / sizeof(au32IOMode[0])); mode++)
        {
            for (digit = 0; digit < (sizeof(au32IODigit) / sizeof(au32IODigit[0])); digit++)
            {
                for (pull = 0; pull < (sizeof(au32IOPullSel) / sizeof(au32IOPullSel[0])); pull++)
                {
                    level = 0;
                    RTC_SetGPIOMode(pin, mode, digit, pull, level);

                    if (pin <= 7) // PF.4 ~ PF.7
                    {
                        offset = (pin - 4) * 8;
                        CU_ASSERT_EQUAL(pRTC->GPIOCTL0 & (0x3F << offset), (((mode << 0) | (0 << 2) | (digit << 3) | (pull << 4)) << offset));
                        D_msg("RTC->GPIOCTL0: 0x%x\n", pRTC->GPIOCTL0);
                    }
                    else
                    {
                        offset = (pin - 8) * 8;
                        CU_ASSERT_EQUAL(pRTC->GPIOCTL1 & (0x3F << offset), (((mode << 0) | (0 << 2) | (digit << 3) | (pull << 4)) << offset));
                        D_msg("RTC->GPIOCTL1: 0x%x\n", pRTC->GPIOCTL0);
                    }

                    level = 1;
                    RTC_SetGPIOMode(pin, mode, digit, pull, level);

                    if (pin <= 7) // PF.4 ~ PF.7
                    {
                        offset = (pin - 4) * 8;
                        CU_ASSERT_EQUAL(pRTC->GPIOCTL0 & (0x3F << offset), (((mode << 0) | (1 << 2) | (digit << 3) | (pull << 4)) << offset));
                    }
                    else
                    {
                        offset = (pin - 8) * 8;
                        CU_ASSERT_EQUAL(pRTC->GPIOCTL1 & (0x3F << offset), (((mode << 0) | (1 << 2) | (digit << 3) | (pull << 4)) << offset));
                    }

                    level = 0;
                    RTC_SetGPIOMode(pin, mode, digit, pull, level);

                    if (pin <= 7) // PF.4 ~ PF.7
                    {
                        offset = (pin - 4) * 8;
                        CU_ASSERT_EQUAL(pRTC->GPIOCTL0 & (0x3F << offset), (((mode << 0) | (0 << 2) | (digit << 3) | (pull << 4)) << offset));
                    }
                    else
                    {
                        offset = (pin - 8) * 8;
                        CU_ASSERT_EQUAL(pRTC->GPIOCTL1 & (0x3F << offset), (((mode << 0) | (0 << 2) | (digit << 3) | (pull << 4)) << offset));
                    }
                }
            }
        }

        mode = digit = pull = level = 0;
        RTC_SetGPIOMode(pin, mode, digit, pull, level);
        RTC_SetGPIOLevel(pin, 0);

        if (pin <= 7) // PF.4 ~ PF.7
        {
            offset = (pin - 4) * 8;
            CU_ASSERT_EQUAL(pRTC->GPIOCTL0 & (0x3F << offset), (((mode << 0) | (0 << 2) | (digit << 3) | (pull << 4)) << offset));
        }
        else
        {
            offset = (pin - 8) * 8;
            CU_ASSERT_EQUAL(pRTC->GPIOCTL1 & (0x3F << offset), (((mode << 0) | (0 << 2) | (digit << 3) | (pull << 4)) << offset));
        }

        mode = digit = pull = 1;
        level = 0;
        RTC_SetGPIOMode(pin, mode, digit, pull, level);
        RTC_SetGPIOLevel(pin, 1);

        if (pin <= 7) // PF.4 ~ PF.7
        {
            offset = (pin - 4) * 8;
            CU_ASSERT_EQUAL(pRTC->GPIOCTL0 & (0x3F << offset), (((mode << 0) | (1 << 2) | (digit << 3) | (pull << 4)) << offset));
        }
        else
        {
            offset = (pin - 8) * 8;
            CU_ASSERT_EQUAL(pRTC->GPIOCTL1 & (0x3F << offset), (((mode << 0) | (1 << 2) | (digit << 3) | (pull << 4)) << offset));
        }

        mode = digit = pull = 0;
        level = 0;
        RTC_SetGPIOMode(pin, mode, digit, pull, level);
        RTC_SetGPIOLevel(pin, 0);

        if (pin <= 7) // PF.4 ~ PF.7
        {
            offset = (pin - 4) * 8;
            CU_ASSERT_EQUAL(pRTC->GPIOCTL0 & (0x3F << offset), (((mode << 0) | (0 << 2) | (digit << 3) | (pull << 4)) << offset));
        }
        else
        {
            offset = (pin - 8) * 8;
            CU_ASSERT_EQUAL(pRTC->GPIOCTL1 & (0x3F << offset), (((mode << 0) | (0 << 2) | (digit << 3) | (pull << 4)) << offset));
        }
    }

    /* Check RTC_SET_IOCTL_BY_GPIO, RTC_SET_IOCTL_BY_RTC, RTC_GET_IOCTL_PROPERTY */
    RTC_SET_IOCTL_BY_GPIO(pRTC);
    CU_ASSERT_EQUAL(RTC_GET_IOCTL_PROPERTY(pRTC), 0);
    CU_ASSERT_EQUAL(pRTC->LXTCTL & BIT8, 0);
    RTC_SET_IOCTL_BY_RTC(pRTC);
    CU_ASSERT_EQUAL(RTC_GET_IOCTL_PROPERTY(pRTC), 1);
    CU_ASSERT_EQUAL(pRTC->LXTCTL & BIT8, BIT8);
    RTC_SET_IOCTL_BY_GPIO(pRTC);
    CU_ASSERT_EQUAL(RTC_GET_IOCTL_PROPERTY(pRTC), 0);
    CU_ASSERT_EQUAL(pRTC->LXTCTL & BIT8, 0);

    RTC_Close();
}

void API_RTC_ClockSource_Func(void)
{
    volatile uint32_t u32Timeout;
    RTC_T *pRTC;

    //    if((__PC()&NS_OFFSET) == NS_OFFSET)
    //        pRTC = RTC_NS;
    //    else
    pRTC = RTC;


    /* Init RTC clock */
    CU_ASSERT_EQUAL(RTC_InitClock(), 0);

    /* Check RTC_INIT_KEY */
    u32Timeout = SystemCoreClock / 10;
    pRTC->INIT = RTC_INIT_KEY;

    while (pRTC->INIT != RTC_INIT_ACTIVE_Msk)
    {
        if (u32Timeout-- == 0)
        {
            CU_FAIL("INIT FAIL");
            return ;
        }
    }

    RTC_SetClockSource(RTC_CLOCK_SOURCE_LXT);
    CU_ASSERT_EQUAL(pRTC->LXTCTL & (BIT7 | BIT6), 0);
    RTC_SetClockSource(RTC_CLOCK_SOURCE_LIRC);
    CU_ASSERT_EQUAL(pRTC->LXTCTL & BIT7, BIT7);
#ifndef _PZ1_EMU_
    RTC_SetClockSource(RTC_CLOCK_SOURCE_LIRC32K);
    CU_ASSERT_EQUAL(pRTC->LXTCTL & (BIT7 | BIT6 | BIT0), (BIT6 | BIT0));
#endif
    RTC_SetClockSource(RTC_CLOCK_SOURCE_LXT);
    CU_ASSERT_EQUAL(pRTC->LXTCTL & (BIT7 | BIT6), 0);
    RTC_SetClockSource(RTC_CLOCK_SOURCE_LIRC);
    CU_ASSERT_EQUAL(pRTC->LXTCTL & BIT7, BIT7);
#ifndef _PZ1_EMU_
    RTC_SetClockSource(RTC_CLOCK_SOURCE_LIRC32K);
    CU_ASSERT_EQUAL(pRTC->LXTCTL & (BIT7 | BIT6 | BIT0), (BIT6 | BIT0));
#endif
    RTC_SetClockSource(RTC_CLOCK_SOURCE_LXT);
    CU_ASSERT_EQUAL(pRTC->LXTCTL & (BIT7 | BIT6), 0);

    RTC_Close();
}

void API_RTC_ClockDetector_Func(void)
{
    volatile uint32_t u32Timeout;
    RTC_T *pRTC;

    pRTC = RTC;

    /* Init RTC clock */
    CU_ASSERT_EQUAL(RTC_InitClock(), 0);

    u32Timeout = SystemCoreClock / 10;
    pRTC->INIT = RTC_INIT_KEY;

    while (pRTC->INIT != RTC_INIT_ACTIVE_Msk)
    {
        if (u32Timeout-- == 0)
        {
            CU_FAIL("INIT FAIL");
            return ;
        }
    }

    RTC_SetClockSource(RTC_CLOCK_SOURCE_LXT);
    printf("\n\nChange LXT Frequency to 0 after, press any key....\r\n");
    getchar();

    RTC_EnableClockFrequencyDetector(125, 125);
    printf("CLKDCTL:%x,%x,%x\r\n", pRTC->CLKDCTL, (125 << (RTC_CDBR_FAILBD_Pos)), (125 << (RTC_CDBR_STOPBD_Pos)));
    CU_ASSERT_EQUAL((pRTC->CDBR & RTC_CDBR_FAILBD_Msk), (125 << (RTC_CDBR_FAILBD_Pos)));
    CU_ASSERT_EQUAL((pRTC->CDBR & RTC_CDBR_STOPBD_Msk), (125 << (RTC_CDBR_STOPBD_Pos)));
    CU_ASSERT_EQUAL((pRTC->CLKDCTL & RTC_CLKDCTL_LXTFDEN_Msk), RTC_CLKDCTL_LXTFDEN_Msk);
    CU_ASSERT_EQUAL((pRTC->CLKDCTL & RTC_CLKDCTL_LXTSTSW_Msk), RTC_CLKDCTL_LXTSTSW_Msk);
    CU_ASSERT_EQUAL((pRTC->CLKDCTL & RTC_CLKDCTL_LXTFSW_Msk), RTC_CLKDCTL_LXTFSW_Msk);
    CU_ASSERT_EQUAL((pRTC->LXTCTL & BIT0), BIT0);

    /* Check Fail / Stop intsts*/
    u32Timeout = SystemCoreClock / 10;

    while (1)
    {
        if (RTC_GET_CLKFAIL_INT_FLAG(pRTC) == 1)
        {
            printf("CLKDCTL:%x\r\n", pRTC->CLKDCTL);
            printf("INTSTS:0x%x\r\n",pRTC->INTSTS);
            CU_ASSERT_EQUAL((pRTC->CLKDCTL & RTC_CLKDCTL_SWLIRCF_Msk), RTC_CLKDCTL_SWLIRCF_Msk);
            CU_ASSERT_EQUAL((pRTC->CLKDCTL & RTC_CLKDCTL_LXTSLOWF_Msk), RTC_CLKDCTL_LXTSLOWF_Msk);

            RTC_CLEAR_CLKFAIL_INT_FLAG(pRTC);
            break;
        }

        if (u32Timeout-- == 0)
        {
            CU_FAIL("RTC_GET_CLKFAIL_INT_FLAG FAIL");
            return ;
        }
    }

    CU_ASSERT_EQUAL(RTC_GET_CLKFAIL_INT_FLAG(pRTC), 0);

    u32Timeout = SystemCoreClock / 10;

    while (1)
    {
        if (RTC_GET_CLKSTOP_INT_FLAG(pRTC) == 1)
        {
            RTC_CLEAR_CLKSTOP_INT_FLAG(pRTC);
            break;
        }

        if (u32Timeout-- == 0)
        {
            CU_FAIL("RTC_CLEAR_CLKSTOP_INT_FLAG FAIL");
            return ;
        }
    }
    
    CU_ASSERT_EQUAL(RTC_GET_CLKSTOP_INT_FLAG(pRTC), 0);

    RTC_DisableClockFrequencyDetector();
    CU_ASSERT_EQUAL((pRTC->CDBR & RTC_CDBR_FAILBD_Msk), (0xF0 << (RTC_CDBR_FAILBD_Pos)));
    CU_ASSERT_EQUAL((pRTC->CDBR & RTC_CDBR_STOPBD_Msk), (0xF0 << (RTC_CDBR_STOPBD_Pos)));
    CU_ASSERT_EQUAL((pRTC->CLKDCTL & RTC_CLKDCTL_LXTFDEN_Msk), 0);
    CU_ASSERT_EQUAL((pRTC->CLKDCTL & RTC_CLKDCTL_LXTSTSW_Msk), 0);
    CU_ASSERT_EQUAL((pRTC->CLKDCTL & RTC_CLKDCTL_LXTFSW_Msk), 0);
    CU_ASSERT_EQUAL((pRTC->LXTCTL & BIT0), 0);

    RTC_Close();
}

CU_TestInfo RTC_CONSTANT_MACRO[] =
{
    {"Check RTC CONSTANT ", CONSTANT_RTC},
    {"Check RTC MACRO ",    MACRO_RTC},
    CU_TEST_INFO_NULL
};

CU_TestInfo RTC_API[] =
{
    {"Check RTC Data, Time APIs ",      API_RTC_DataTime_Func},
    {"Check RTC IO Control APIs ",      API_RTC_IO_Control_Func},
    {"Check RTC Clock Source API ",     API_RTC_ClockSource_Func},
    {"Check RTC Clock Detector API",    API_RTC_ClockDetector_Func},
    CU_TEST_INFO_NULL
};
