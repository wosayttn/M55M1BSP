/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Demonstrate how to use EPWM brake function.
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

/**
 * @brief       EPWM0 Brake0 IRQ Handler
 * @param       None
 * @return      None
 * @details     ISR to handle EPWM0 Brake0 interrupt event
 */
NVT_ITCM void BRAKE0_IRQHandler(void)
{
    printf("\nFault brake!\n");
    printf("Press any key to unlock brake state. (EPWM0 channel 0 output will toggle again)\n");
    getchar();

    /* Clear brake interrupt flag */
    EPWM_ClearFaultBrakeIntFlag(EPWM0, EPWM_FB_EDGE);
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
    /* Select EPWM1 module clock source */
    CLK_SetModuleClock(EPWM0_MODULE, CLK_EPWMSEL_EPWM0SEL_PCLK0, 0);

    /* Enable EPWM1 module clock */
    CLK_EnableModuleClock(EPWM0_MODULE);

    /* Enable GPIO clock */
    CLK_EnableModuleClock(GPIOE_MODULE);
    CLK_EnableModuleClock(GPIOD_MODULE);
    /* Set multi-function pin for EPWM */
    SET_EPWM0_CH0_PE7();
    SET_EPWM0_CH1_PE6();
    SET_EPWM0_CH2_PE5();
    SET_EPWM0_CH3_PE4();

    /* Set multi-function pin for EPWM brake pin */
    SET_EPWM0_BRAKE0_PE8();
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
    printf("\nConnet PE.8 (EPWM0 brake pin 0) to PD.3.\n");
    printf("It will generate brake interrupt and EPWM0 channel 0 output stop toggling.\n");

    GPIO_SetMode(PD, BIT3, GPIO_MODE_OUTPUT);
    PD3 = 0;

    /* EPWM0 frequency is 100Hz, duty 30%, */
    EPWM_ConfigOutputChannel(EPWM0, 0, 100, 30);

    /* Enable output of all EPWM channels */
    EPWM_EnableOutput(EPWM0, 0x3F);

    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Enable brake and interrupt */
    EPWM_EnableFaultBrake(EPWM0, EPWM_CH_0_MASK, 1, EPWM_FB_EDGE_BKP0);
    EPWM_EnableFaultBrakeInt(EPWM0, 0);
    /* Enable brake noise filter : brake pin 0, filter count=7, filter clock=HCLK/128 */
    EPWM_EnableBrakeNoiseFilter(EPWM0, 0, 7, EPWM_NF_CLK_DIV_128);
    /* Clear brake interrupt flag */
    EPWM_ClearFaultBrakeIntFlag(EPWM0, EPWM_FB_EDGE);

    NVIC_EnableIRQ(BRAKE0_IRQn);

    /* Start */
    EPWM_Start(EPWM0, 1);

    printf("\nPress any key to generate a brake event\n");
    getchar();
    PD3 = 1;

    /* Got no where to go, just loop forever */
    while (1) ;
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
