/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Demonstrate Timer PWM Complementary mode and Dead-Time function.
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
    /* Select TIMER clock source */
    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_PCLK1, 0);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_PCLK1, 0);
    /* Enable TIMER module clock */
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);

    /* Enable GPIO clock */
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK_EnableModuleClock(GPIOH_MODULE);
    /* Set Timer0 PWM CH0, CH1 and Timer1 PWM CH0, CH1 */
    SET_TM0_PB5();
    SET_TM0_EXT_PH0();
    SET_TM1_PB4();
    SET_TM1_EXT_PH1();
    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    uint32_t u32Period, u32CMP, u32Prescaler, u32DeadTime;

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    initialise_monitor_handles();
#endif

    printf("System core clock = %d\n", SystemCoreClock);
    printf("+--------------------------------------------------------------+\n");
    printf("|    Timer PWM Complementary mode and Dead-Time Sample Code    |\n");
    printf("+--------------------------------------------------------------+\n\n");

    /* Configure Timer0 PWM */
    /* Change Timer to PWM counter mode */
    TPWM_ENABLE_PWM_MODE(TIMER0);

    /* Set PWM mode as complementary mode*/
    TPWM_ENABLE_COMPLEMENTARY_MODE(TIMER0);

    /* Set Timer0 PWM output frequency is 6000 Hz, duty 40% */
    TPWM_ConfigOutputFreqAndDuty(TIMER0, 6000, 40);

    /* Enable output of PWM_CH0 and PWM_CH1 */
    TPWM_ENABLE_OUTPUT(TIMER0, (TPWM_CH1|TPWM_CH0));

    /* Get u32Prescaler, u32Period and u32CMP after called TPWM_ConfigOutputFreqAndDuty() API */
    u32Prescaler = (TPWM_GET_PRESCALER(TIMER0) + 1);
    u32Period = (TPWM_GET_PERIOD(TIMER0) + 1);
    u32CMP = TPWM_GET_CMPDAT(TIMER0);
    u32DeadTime = u32CMP/2;

    printf("# Timer0 PWM output frequency is 600 Hz and duty 40%%.\n");
    printf("    - Counter clock source:    PCLK \n");
    printf("    - Counter clock prescaler: %d \n", u32Prescaler);
    printf("    - Counter type:            Up count type \n");
    printf("    - Operation mode:          Complementary in auto-reload mode \n");
    printf("    - Period value:            %d \n", u32Period);
    printf("    - Comparator value:        %d \n", u32CMP);
    printf("# I/O configuration:\n");
    printf("    - Timer0 PWM_CH0 on PB.5, PWM_CH1 on PH.0\n\n");

    /* Configure Timer1 PWM */
    printf("# Timer1 PWM output frequency is 600 Hz and duty 20%% with dead-time insertion.\n");
    printf("    - Counter clock source:    PCLK \n");
    printf("    - Counter clock prescaler: %d \n", u32Prescaler);
    printf("    - Counter type:            Up count type \n");
    printf("    - Operation mode:          Complementary in auto-reload mode \n");
    printf("    - Period value:            %d \n", u32Period);
    printf("    - Comparator value:        %d \n", u32CMP);
    printf("    - Dead-Time interval:      %d \n", u32DeadTime);
    printf("# I/O configuration:\n");
    printf("    - Timer1 PWM_CH0 on PB.4, PWM_CH1 on PH.1\n\n");

    /* Change Timer to PWM counter mode */
    TPWM_ENABLE_PWM_MODE(TIMER1);

    /* Set PWM mode as complementary mode*/
    TPWM_ENABLE_COMPLEMENTARY_MODE(TIMER1);

    /* Set Timer1 PWM output frequency is 6000 Hz, duty 40% */
    TPWM_ConfigOutputFreqAndDuty(TIMER1, 6000, 40);

    /* Enable output of PWM_CH0 and PWM_CH1 */
    TPWM_ENABLE_OUTPUT(TIMER1, (TPWM_CH1|TPWM_CH0));

    /* Enable and configure dead-time interval is (u32DeadTime * TMR1_PWMCLK * prescaler) */
    SYS_UnlockReg(); // Unlock protected registers
    TPWM_EnableDeadTimeWithPrescale(TIMER1, (u32DeadTime-1));
    SYS_LockReg(); // Lock protected registers

    printf("*** Check Timer0 and Timer1 PWM output waveform by oscilloscope ***\n");

    /* Start Timer0 and Timer1 PWM counter by trigger Timer0 sync. start */
    TPWM_SET_COUNTER_SYNC_MODE(TIMER0, TPWM_CNTR_SYNC_START_BY_TIMER0);
    TPWM_SET_COUNTER_SYNC_MODE(TIMER1, TPWM_CNTR_SYNC_START_BY_TIMER0);
    TPWM_TRIGGER_COUNTER_SYNC(TIMER0);

    /* Got no where to go, just loop forever */
    while (1) ;
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
