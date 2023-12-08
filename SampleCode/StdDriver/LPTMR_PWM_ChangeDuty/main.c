/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Change duty cycle and period of output waveform in PWM down count type.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
/*
 * This sample uses internal RC as APLL0 clock source and UART to print messages.
 * Users may need to do extra system configuration according to their system design.
 *
 * I/D-Cache
 *   I/D-Cache are enabled by default for better performance,
 *   users can define NVT_ICACHE_OFF/NVT_DCACHE_OFF in project setting to disable cache.
 * Debug UART
 *   system_M55M1.c has three weak functions as below to configure debug UART port.
 *     SetDebugUartMFP, SetDebugUartCLK and InitDebugUart
 *   Users can re-implement these functions according to system design.
 */
#include <stdio.h>
#include <stdlib.h>
#include "NuMicro.h"

static volatile uint32_t g_u32Period;

NVT_ITCM void LPTMR0_IRQHandler(void)
{
    static uint32_t s_u32Toggle = 0;

    if (LPTPWM_GET_PERIOD_INT_FLAG(LPTMR0))
    {
        if (s_u32Toggle == 0)
        {
            /* Set PWM period to generate output frequency 36000 Hz */
            LPTPWM_SET_PERIOD(LPTMR0, ((g_u32Period / 2) - 1));

            /* Set PWM duty, 40% */
            LPTPWM_SET_CMPDAT(LPTMR0, (((g_u32Period / 2) * 4) / 10));
        }
        else
        {
            /* Set PWM period to generate output frequency 18000 Hz */
            LPTPWM_SET_PERIOD(LPTMR0, (g_u32Period - 1));

            /* Set PWM duty, 50% */
            LPTPWM_SET_CMPDAT(LPTMR0, (g_u32Period / 2));
        }

        s_u32Toggle ^= 1;
        LPTPWM_CLEAR_PERIOD_INT_FLAG(LPTMR0);
    }
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

    /* Enable UART module clock */
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

    /* Set LPTMR0 PWM CH0(LPTM0) pin */
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
    printf("+---------------------------------------------------------+\n");
    printf("|    Low Power Timer PWM Change Duty Cycle Sample Code    |\n");
    printf("+---------------------------------------------------------+\n\n");

    printf("# Low Power TMR0 PWM_CH0 frequency of first period is 18000 Hz and duty is 50%%.\n");
    printf("# Low Power TMR0 PWM_CH0 frequency of second period is 36000 Hz and duty is 40%%.\n");
    printf("# I/O configuration:\n");
    printf("    - LPTMR0 PWM_CH0 on PB.5\n\n");

    /* Change Low Power Timer to PWM counter mode */
    LPTPWM_ENABLE_PWM_MODE(LPTMR0);

    /* Set Low Power TMR0 PWM output frequency is 18000 Hz, duty 50% in up count type */
    if (LPTPWM_ConfigOutputFreqAndDuty(LPTMR0, 18000, 50) != 18000)
    {
        printf("Set the frequency different from the user\n");
    }

    /* Get initial period and comparator value */
    g_u32Period = LPTPWM_GET_PERIOD(LPTMR0) + 1;

    /* Enable output of LPTPWM_CH0 */
    LPTPWM_ENABLE_OUTPUT(LPTMR0, TPWM_CH0);

    /* Enable period event interrupt */
    LPTPWM_ENABLE_PERIOD_INT(LPTMR0);
    NVIC_EnableIRQ(LPTMR0_IRQn);

    /* Start Low Power Timer PWM counter */
    TPWM_START_COUNTER(LPTMR0);

    printf("*** Check Low Power TMR0 PWM_CH0 output waveform by oscilloscope ***\n");

    /* Got no where to go, just loop forever */
    while (1) ;
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
