/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Use LPTMR to wake up system from Power-down mode periodically.
 *          Please refer to the sample code PMC_PowerDown_MinCurrent to set
 *          the minimum current of the system in Power-down mode.
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
 *   system_M55M1.c has three weak functions as below to configure debug UART port.
 *     SetDebugUartMFP, SetDebugUartCLK and InitDebugUart
 *   Users can re-implement these functions according to system design.
 */
#include <stdio.h>
#include <stdlib.h>
#include "NuMicro.h"

static volatile uint32_t g_u32PDWK;

NVT_ITCM void LPTMR0_IRQHandler(void)
{
    CLK_WaitModuleClockReady(LPTMR0_MODULE);//TESTCHIP_ONLY
    CLK_WaitModuleClockReady(UART0_MODULE);//TESTCHIP_ONLY
    /* Clear wake up flag */
    LPTMR_ClearWakeupFlag(LPTMR0);
    /* Clear interrupt flag */
    LPTMR_ClearIntFlag(LPTMR0);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Power Wake-up IRQ Handler                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
NVT_ITCM void PMC_IRQHandler(void)
{
    g_u32PDWK = PMC_GetPMCWKSrc();
    /* check power down wakeup flag */
    if (g_u32PDWK & PMC_INTSTS_PDWKIF_Msk)
    {
        PMC->INTSTS |= PMC_INTSTS_CLRWK_Msk;

        while (PMC_GetPMCWKSrc() & PMC_INTSTS_PDWKIF_Msk);
    }
}

static void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Release GPIO hold status */
    PMC_RELEASE_GPIO();

    /* Set PF multi-function pins for XT1_OUT(PF.2) and XT1_IN(PF.3) */
    SET_XT1_OUT_PF2();
    SET_XT1_IN_PF3();

    /* Set PF multi-function pins for X32_OUT(PF.4) and X32_IN(PF.5) */
    SET_X32_OUT_PF4();
    SET_X32_IN_PF5();

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
    /* Enable LIRC*/
    CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);
    /* Enable LPTMR module clock */
    CLK_SetModuleClock(LPTMR0_MODULE, CLK_LPTMRSEL_LPTMR0SEL_LIRC, 0);
    CLK_EnableModuleClock(LPTMR0_MODULE);

    /* Set PC multi-function pin for CLKO(PC.13) */
    SET_CLKO_PC13();

    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    int i = 0;
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    initialise_monitor_handles();
#endif

    printf("System core clock = %d\n", SystemCoreClock);
    printf("LPTMR power down/wake up sample code\n");
    while(!UART_IS_TX_EMPTY(UART0));

    /* Output selected clock to CKO*/
    CLK_EnableCKO(CLK_CLKOSEL_CLKOSEL_SYSCLK, 3, CLK_CLKOCTL_DIV1EN_DIV_FREQSEL);

    /* Initial LPTMR0 to periodic mode with 1Hz, since system is fast (12MHz)
       and LPTMR is slow (32KHz), and following function calls all modified LPTMR's
       CTL register, so add extra delay between each function call and make sure the
       setting take effect */
    LPTMR_Open(LPTMR0, LPTMR_PERIODIC_MODE, 1);
    CLK_SysTickDelay(50);
    /* Enable LPTMR0 wake up system */
    LPTMR_EnableWakeup(LPTMR0);
    CLK_SysTickDelay(50);
    /* Enable LPTMR0 interrupt */
    LPTMR_EnableInt(LPTMR0);
    PMC_ENABLE_INT();
    CLK_SysTickDelay(50);
    NVIC_EnableIRQ(LPTMR0_IRQn);
    NVIC_EnableIRQ(PMC_IRQn);
    /* Start LPTMR0 counting */
    LPTMR_Start(LPTMR0);
    CLK_SysTickDelay(50);
    /* Unlock protected registers */
    SYS_UnlockReg();
    while(1)
    {
        printf("Enter Power-down !\n");
        g_u32PDWK = 0;
        while(!UART_IS_TX_EMPTY(UART0));
        PMC_PowerDown();
        while(!g_u32PDWK);
        printf("Wake %d\n", i++);
        CLK_SysTickDelay(1000000);
    }
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
