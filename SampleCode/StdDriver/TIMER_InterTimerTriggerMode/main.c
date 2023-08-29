/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Use the timer pin PB.5 to demonstrate inter timer trigger mode function.
 *          Also display the measured input frequency to UART console.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
/*
 * This sample uses internal RC as APLL0 clock source and UART0 to print messages.
 * Users may need to do extra system configuration according to their system design.
 *
 * I/D-Cache
 *   I/D-Cache are enabled by default for better performance,
 *   users can define NVT_ICACHE_OFF/NVT_DCACHE_OFF in project setting to disable cache.
 * Debug UART
 *   Default is DEBUG_PORT=UART0 in project setting
 *   system_M55M1.c has three weak functions as below to configure debug UART port.
 *     SetDebugUartMFP, SetDebugUartCLK and InitDebugUart
 *   Users can re-implement these functions according to system design.
 */
#include <stdio.h>
#include <stdlib.h>
#include "NuMicro.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
static volatile int complete = 0;

// Timer 0 is working in event count mode, and Timer 1 in capture mode, so we read the
// capture value from Timer 1, not timer 0.
NVT_ITCM void TIMER1_IRQHandler(void)
{
    // Timer counter value records the duration for 100 event counts.
    printf("Event frequency is %d Hz\n", 100000000 / TIMER_GetCounter(TIMER1) * 100);
    TIMER_ClearCaptureIntFlag(TIMER1);
    
    complete = 1;
}

static void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Enable PLL0 180MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_180MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to PLL0 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0);

    /* Set HCLK2 divide 2 */
    CLK_SET_HCLK2DIV(2);

    /* Set PCLKx divide 2 */
    CLK_SET_PCLK0DIV(2);
    CLK_SET_PCLK1DIV(2);
    CLK_SET_PCLK2DIV(2);
    CLK_SET_PCLK3DIV(2);
    CLK_SET_PCLK4DIV(2);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();
    /*---------------------------------------------------------------------------------------------------------*/
    /* Initialization for sample code                                                                          */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Select TIMER clock source */
    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_PCLK1, 0);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_PCLK1, 0);
    /* Enable TIMER module clock */
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);

    /* Enable GPIO clock */
    CLK_EnableModuleClock(GPIOB_MODULE);
    SET_TM0_PB5();
    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    initialise_monitor_handles();
#endif

    printf("System core clock = %d\n", SystemCoreClock);
    /* This sample code demonstrate inter timer trigger mode using Timer0 and Timer1.
     * In this mode, Timer0 is working as counter, and triggers Timer1.
     * Using Timer1 to calculate the amount of time used by Timer0 to count specified amount of events.
     * By dividing the time period recorded in Timer1 by the event counts, we get the event frequency.
     */
    printf("Inter timer trigger mode demo code\n");
    printf("Please connect input source with Timer 0 counter pin PB.5, press any key to continue\n");
    getchar();

    // Give a dummy target frequency here. Will over write prescale and compare value with macro
    TIMER_Open(TIMER0, TIMER_ONESHOT_MODE, 100);

    // Update prescale and compare value. Calculate average frequency every 100 events
    TIMER_SET_PRESCALE_VALUE(TIMER0, 0);
    TIMER_SET_CMP_VALUE(TIMER0, 100);

    // Update Timer 1 prescale value.
    TIMER_SET_PRESCALE_VALUE(TIMER1, 0);

    // We need capture interrupt
    NVIC_EnableIRQ(TIMER1_IRQn);

    while(1)
    {
        complete = 0;
        // Count event by timer 0, disable drop count (set to 0), disable timeout (set to 0). Enable interrupt after complete
        TIMER_EnableFreqCounter(TIMER0, 0, 0, TRUE);
        while(complete == 0);
    }
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
