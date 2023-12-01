/******************************************************************************
* @file    wdt_cunit.c
* @version V1.00
* @brief   wdt CUnit Test
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
#include "wdt_cunit.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

//#define D_msg   printf
#define D_msg(...)
#ifdef _WDT0
    #define WDT WDT0
    #define WDT_IRQHandler WDT0_IRQHandler
#else
    #define WDT WDT1
    #define WDT_IRQHandler WDT1_IRQHandler
#endif

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
    {"WDT MACRO", suite_success_init, suite_success_clean, NULL, NULL, WDT_MACRO},
    {"WDT API", suite_success_init, suite_success_clean, NULL, NULL, WDT_API},
    CU_SUITE_INFO_NULL
};

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
    else
    {
        //CLK->CLKSEL1 = (CLK->CLKSEL1&~CLK_CLKSEL1_TMR0SEL_Msk) | CLK_CLKSEL1_TMR0SEL_HXT;
        CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC, 0);
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
    else
    {
        TIMER0->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk | (12 - 1);
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

volatile uint8_t gu8IntFlag, gu8WakeupFlag;
volatile uint32_t gu32TDRTicks;
volatile uint32_t gu32RegWDTCTL;
NVT_ITCM void WDT_IRQHandler(void)
{
    static uint8_t u8Cnt = 0;
#if _WDT0
    CLK_WaitModuleClockReady(WDT0_MODULE);
#else
    CLK_WaitModuleClockReady(WDT1_MODULE);
#endif
    gu32RegWDTCTL = WDT->STATUS;

    if (WDT_GET_TIMEOUT_INT_FLAG(WDT) == 1)
    {

        gu32TDRTicks = TIMER0->CNT;
        gu8IntFlag = 1;

        /* Clear WDT time-out interrupt flag */
        WDT_CLEAR_TIMEOUT_INT_FLAG(WDT);

        if (u8Cnt == 0)
            WDT_RESET_COUNTER(WDT);
        else
            WDT->RSTCNT = WDT_RESET_COUNTER_KEYWORD;

        u8Cnt ^= 1;
    }

    if (WDT_GET_TIMEOUT_WAKEUP_FLAG(WDT) == 1)
    {

        gu8WakeupFlag = 1;

        /* Clear WDT time-out wake-up flag */
        WDT_CLEAR_TIMEOUT_WAKEUP_FLAG(WDT);
    }
}

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
int32_t WDT_InitClock(void)
{
    volatile uint32_t u32Timeout = SystemCoreClock / 1000;

    /* Enable LIRC */
    CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);

    /* Waiting for clock ready */
#if _WDT0
    CLK_SetModuleClock(WDT0_MODULE, CLK_WDTSEL_WDT0SEL_LIRC, 0);

    if (CLK_EnableModuleClock(WDT0_MODULE) == 1) return 0;

#else
    CLK_SetModuleClock(WDT1_MODULE, CLK_WDTSEL_WDT1SEL_LIRC, 0);

    if (CLK_EnableModuleClock(WDT1_MODULE) == 1) return 0;

#endif
    return -1;
}
#else
extern int32_t WDT_InitClock(void);
#endif

const uint32_t au32TOUTSel[] =
{
    WDT_TIMEOUT_2POW4,
    WDT_TIMEOUT_2POW6,
    WDT_TIMEOUT_2POW8,
    WDT_TIMEOUT_2POW10,
    WDT_TIMEOUT_2POW12,
    WDT_TIMEOUT_2POW14,
    WDT_TIMEOUT_2POW16,
    WDT_TIMEOUT_2POW18,
    WDT_TIMEOUT_2POW20,
};

const uint32_t au32RDSel[] =
{
    WDT_RESET_DELAY_1026CLK,
    WDT_RESET_DELAY_130CLK,
    WDT_RESET_DELAY_18CLK,
    WDT_RESET_DELAY_3CLK,
};

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
void CONSTANT_WDT_SETTING(void)
{
    volatile uint32_t i;

    if (WDT_InitClock() != 0)
    {
        CU_FAIL("WDT Clock FAIL");
        return ;
    }

    /* Select time-out interval; enable interrupt; enable reset; enable wake-up */
    for (i = 0; i < sizeof(au32TOUTSel) / sizeof(au32TOUTSel[0]); i++)
    {
        D_msg("TOUTSel %d\n", i);
        WDT_Open(WDT, au32TOUTSel[i], WDT_RESET_DELAY_18CLK, TRUE, TRUE);
        WDT_EnableInt(WDT);

        CU_ASSERT_EQUAL(WDT->ALTCTL, 0x2);

        while ((WDT->CTL & WDT_CTL_SYNC_Msk) != 0) {};

        CU_ASSERT_EQUAL(WDT->CTL, ((i << 8) | 0xD2));
    }

    WDT_Close(WDT);

    /* Select reset delay period; disable interrupt; disable reset; disable wake-up */
    for (i = 0; i < sizeof(au32RDSel) / sizeof(au32RDSel[0]); i++)
    {
        D_msg("RDSel %d\n", i);
        WDT_Open(WDT, WDT_TIMEOUT_2POW14, au32RDSel[i], FALSE, FALSE);
        WDT_EnableInt(WDT);
        WDT_DisableInt(WDT);

        CU_ASSERT_EQUAL(WDT->ALTCTL, i);

        while ((WDT->CTL & WDT_CTL_SYNC_Msk) != 0) {};

        CU_ASSERT_EQUAL(WDT->CTL, 0x580);
    }

    WDT_Close(WDT);
}
#else
extern void CONSTANT_WDT_SETTING(void);
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
void API_WDT_Interrupt(void)
{
    if (WDT_InitClock() != 0)
    {
        CU_FAIL("WDT Clock FAIL");
        return ;
    }

    /* Check WDT_EnableInt */
    WDT->CTL = 0x700;
    WDT->ALTCTL = 0x3;
    WDT_EnableInt(WDT);
    CU_ASSERT_EQUAL(WDT->ALTCTL, 0x3);

    while ((WDT->CTL & WDT_CTL_SYNC_Msk) != 0) {};

    CU_ASSERT_EQUAL(WDT->CTL, 0x740);

    printf("INT-Check:0x%08x\r\n", WDT->CTL);

    /* Check WDT_DisableInt */
    WDT->ALTCTL = 0x0;

    WDT_DisableInt(WDT);

    CU_ASSERT_EQUAL(WDT->ALTCTL, 0x0);

    while ((WDT->CTL & WDT_CTL_SYNC_Msk) != 0) {};

    CU_ASSERT_EQUAL(WDT->CTL, 0x700);

    printf("INT-Check:0x%08x\r\n", WDT->CTL);

    WDT_Close(WDT);
}
#else
extern void API_WDT_Interrupt(void);
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
void API_WDT_Open(void)
{
    volatile uint32_t u32Timeout = 0;

    if (WDT_InitClock() != 0)
    {
        CU_FAIL("WDT Clock FAIL");
        return ;
    }

    /* Enable WDT0/1 NVIC */
#if _WDT0
    NVIC_EnableIRQ(WDT0_IRQn);
#else
    NVIC_EnableIRQ(WDT1_IRQn);
#endif
    Timer0_Init(CLK_TMRSEL_TMR0SEL_HIRC);
    Timer0_ResetrCounter();

    /* WDT time-out period is around 8 ms if LIRC is 32kHz  */
    WDT_Open(WDT, WDT_TIMEOUT_2POW8, WDT_RESET_DELAY_18CLK, TRUE, TRUE);
    WDT_EnableInt(WDT);

    gu8IntFlag = gu8WakeupFlag = 0;

    while (gu8IntFlag == 0)
    {
        if ((TIMER0->CNT > __HIRC / 12) || (u32Timeout > SystemCoreClock / 20))
        {
            CU_FAIL("WDT time-out INT FAIL");
            WDT_Close(WDT);
            return ;
        }

        u32Timeout++;
    }

    while ((WDT->CTL & WDT_CTL_SYNC_Msk) != 0) {};

    CU_ASSERT_EQUAL(WDT->CTL, 0x2D2);

    CU_ASSERT_EQUAL(gu32RegWDTCTL, 0x1);

    if ((gu32TDRTicks > (7812 + 1000)) || (gu32TDRTicks < (7812 - 1000)))
    {
        CU_FAIL("WDT time-out period time FAIL");
        printf("FAIL. (Get tick: %d) (L:%d)", gu32TDRTicks, __LINE__);
        WDT_Close(WDT);
        return ;
    }

    WDT_Close(WDT);
}
#else
extern void API_WDT_Open(void);
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
void API_WDT_Wakeup(void)
{
    volatile uint32_t u32Timeout = 0;

    if (WDT_InitClock() != 0)
    {
        CU_FAIL("WDT Clock FAIL");
        return ;
    }

    /* Enable WDT0/1 NVIC */
#if _WDT0
    NVIC_EnableIRQ(WDT0_IRQn);
#else
    NVIC_EnableIRQ(WDT1_IRQn);
#endif

    Timer0_Init(CLK_TMRSEL_TMR0SEL_LIRC);
    Timer0_ResetrCounter();

    /* WDT time-out period is around 8 ms */
    WDT_Open(WDT, WDT_TIMEOUT_2POW8, WDT_RESET_DELAY_18CLK, TRUE, TRUE);
    WDT_EnableInt(WDT);

    gu8IntFlag = gu8WakeupFlag = 0;

    /* Enter to power-down */
    {
        UART_WAIT_TX_EMPTY(DEBUG_PORT);

        PMC_SetPowerDownMode(PMC_NPD0, PMC_PLCTL_PLSEL_PL1);

        PMC_PowerDown();

        //        SCB->SCR = 4;

        //        /* To program PWRCON register, it needs to disable register protection first. */
        //        CLK->PWRCTL = (CLK->PWRCTL & ~CLK_PWRCTL_PDEN_Msk) | CLK_PWRCTL_PDWKIEN_Msk;
        //        CLK->PWRCTL |= CLK_PWRCTL_PDEN_Msk;

        //        __WFI();
    }

    while ((gu8IntFlag == 0) || (gu8WakeupFlag == 0))
    {
        if (u32Timeout > SystemCoreClock / 20)
        {
            CU_FAIL("WDT wake-up status FAIL");
            WDT_Close(WDT);
            return ;
        }

        u32Timeout++;
    }

    while ((WDT->CTL & WDT_CTL_SYNC_Msk) != 0) {};

    CU_ASSERT_EQUAL(WDT->CTL, 0x2D2);

    CU_ASSERT_EQUAL(gu32RegWDTCTL, 0x3);

    if ((gu32TDRTicks > (256 + 100)) || (gu32TDRTicks < (256 - 100)))
    {
        CU_FAIL("WDT time-out period time FAIL");
        printf("FAIL. (Get tick: %d) (L:%d)", gu32TDRTicks, __LINE__);
        WDT_Close(WDT);
        return ;
    }

    WDT_Close(WDT);
}
#else
extern void API_WDT_Wakeup(void) ;
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
void API_WDT_ResetTest(void)
{
    if (WDT_InitClock() != 0)
    {
        CU_FAIL("WDT Clock FAIL");
        return ;
    }

    if (WDT_GET_RESET_FLAG(WDT))
    {
        printf("\n\nLast status. [WDT time-out reset occurred (0x%X)]\n", WDT->CTL);
        UART_WAIT_TX_EMPTY(DEBUG_PORT);
        WDT_CLEAR_RESET_FLAG(WDT);

        if (WDT_GET_RESET_FLAG(WDT) != 0)
        {
            printf("\nL: failed.\n");

            while (1) {}
        }
    }
    else
    {
        printf("\n\n*** Please execute [R]un again to check test results... ***\n");
        UART_WAIT_TX_EMPTY(DEBUG_PORT);

        /* WDT time-out period is around 0.5 ms */
        WDT_Open(WDT, WDT_TIMEOUT_2POW4, NULL, TRUE, TRUE);

        while (SYS->PDID);
    }
}
#else
extern void API_WDT_ResetTest(void) ;
#endif

CU_TestInfo WDT_MACRO[] =
{
    {"Check WDT Time-out Reset Test ",                        API_WDT_ResetTest},
    {"Check WDT CONSTANT (with WDT_Open and WDT_Close API) ", CONSTANT_WDT_SETTING},
    CU_TEST_INFO_NULL
};

CU_TestInfo WDT_API[] =
{
    {"Check WDT Enable/Disable Interrupt API ",                             API_WDT_Interrupt},
    {"Check WDT_Open API (with Interrupt Status and Counter Reset MACRO) ", API_WDT_Open},
    {"Check WDT Wake-up Behavior (with Wake-up Interrupt Status MACRO) ",   API_WDT_Wakeup},
    CU_TEST_INFO_NULL
};
