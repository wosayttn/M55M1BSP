/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Demonstrate LPTMR PWM accumulator interrupt to stop counting.
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
static volatile uint32_t gu32Period;

NVT_ITCM void LPTMR0_IRQHandler(void)
{
    LPTPWM_ClearAccInt(LPTMR0);
    
    printf("\nCheck if output toggles 11 times then stop toggles.\n");
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
    /* Select LPTMR clock source */
    CLK_SetModuleClock(LPTMR0_MODULE, CLK_LPTMRSEL_LPTMR0SEL_HIRC, 0);
    /* Enable LPTMR module clock */
    CLK_EnableModuleClock(LPTMR0_MODULE);

    /* Set LPTMR0 PWM CH0(TM0) pin */
    CLK_EnableModuleClock(GPIOB_MODULE);
    SET_LPTM0_PB5();

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
    printf("+----------------------------------------------------------------------+\n");
    printf("|    Low Power Timer PWM Accumulator Inerrupt Stop Mode Sample Code    |\n");
    printf("+----------------------------------------------------------------------+\n\n");

    printf("  This sample code demonstrate Low Power Timer0 PWM accumulator stop mode.\n");
    printf("  When accumulator interrupt happens, the output of Low Power Timer0 PWM stops.\n");
    printf("  Since interrupt accumulator count is set to 10, the output toggles 11 times then stops.\n");
    printf("    - LPTMR0 PWM_CH0 on PB.5\n");

    printf("\n\nPress any key to start Low Power Timer0 PWM.\n");
    getchar();

    /* Change Low Power Timer to PWM counter mode */
    LPTPWM_ENABLE_PWM_MODE(LPTMR0);

    /* Set Low Power Timer0 PWM output frequency is 18000 Hz, duty 50% in up count type */
    LPTPWM_ConfigOutputFreqAndDuty(LPTMR0, 18000, 50);

    /* Enable output of PWM_CH0 */
    LPTPWM_ENABLE_OUTPUT(LPTMR0, TPWM_CH0);

    /* Enable Low Power Timer0 PWM accumulator function, interrupt count 10, accumulator source select to period point */
    LPTPWM_EnableAcc(LPTMR0, 10, LPTPWM_IFA_PERIOD_POINT);

    /* Enable Low Power Timer0 PWM accumulator interrupt */
    LPTPWM_EnableAccInt(LPTMR0);

    /* Enable Low Power Timer0 PWM accumulator stop mode */
    LPTPWM_EnableAccStopMode(LPTMR0);

    /* Enable Low Power Timer0 interrupt */
    NVIC_EnableIRQ(LPTMR0_IRQn);

    /* Start Low Power Timer PWM counter */
    LPTPWM_START_COUNTER(LPTMR0);

    /* Got no where to go, just loop forever */
    while (1) ;
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/