/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Show how to use ECAP interface to get input frequency
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
/*
 * This is a template project for M55M1 series MCU.
 * Users can create their own application based on this project.
 *
 * This template uses internal RC as APLL0 clock source and UART0 to print messages.
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
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t u32Status;
uint32_t u32IC0Hold;

void TIMER0_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER0) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);

        /*PA.0 gpio toggle */
        PA0 ^= 1;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  ECAP0 IRQ Handler                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
void ECAP0_IRQHandler(void)
{
    /* Get input Capture status */
    u32Status = ECAP_GET_INT_STATUS(ECAP0);

    /* Check input capture channel 0 flag */
    if((u32Status & ECAP_STATUS_CAPTF0_Msk) == ECAP_STATUS_CAPTF0_Msk)
    {
        /* Clear input capture channel 0 flag */
        ECAP_CLR_CAPTURE_FLAG(ECAP0, ECAP_STATUS_CAPTF0_Msk);

        /* Get input capture counter hold value */
        u32IC0Hold = ECAP0->HLD0;
    }

    /* Check input capture channel 1 flag */
    if((u32Status & ECAP_STATUS_CAPTF1_Msk) == ECAP_STATUS_CAPTF1_Msk)
    {
        /* Clear input capture channel 1 flag */
        ECAP_CLR_CAPTURE_FLAG(ECAP0, ECAP_STATUS_CAPTF1_Msk);
    }

    /* Check input capture channel 2 flag */
    if((u32Status & ECAP_STATUS_CAPTF2_Msk) == ECAP_STATUS_CAPTF2_Msk)
    {
        /* Clear input capture channel 2 flag */
        ECAP_CLR_CAPTURE_FLAG(ECAP0, ECAP_STATUS_CAPTF2_Msk);
    }

    /* Check input capture compare-match flag */
    if((u32Status & ECAP_STATUS_CAPCMPF_Msk) == ECAP_STATUS_CAPCMPF_Msk)
    {
        /* Clear input capture compare-match flag */
        ECAP_CLR_CAPTURE_FLAG(ECAP0,ECAP_STATUS_CAPCMPF_Msk);
    }

    /* Check input capture overflow flag */
    if((u32Status & ECAP_STATUS_CAPOVF_Msk) == ECAP_STATUS_CAPOVF_Msk)
    {
        /* Clear input capture overflow flag */
        ECAP_CLR_CAPTURE_FLAG(ECAP0,ECAP_STATUS_CAPOVF_Msk);
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

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /* Enable ECAP0 module clock */
    CLK_EnableModuleClock(ECAP0_MODULE);

    /* Select TMR0 module clock source */
    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC, 0);

    /* Enable TIMER0 module clock */
    CLK_EnableModuleClock(TMR0_MODULE);
    /* Enable GPIO clock */
    CLK_EnableModuleClock(GPIOA_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();
    /* Set PA.10 for ECAP0_IC0*/
    SET_ECAP0_IC0_PA10();

    /* Lock protected registers */
    SYS_LockReg();
}

void ECAP0_Init(void)
{
    /* Enable ECAP0*/
    ECAP_Open(ECAP0, ECAP_DISABLE_COMPARE);

    /* Select Reload function */
    ECAP_SET_CNT_CLEAR_EVENT(ECAP0, (ECAP_CTL1_CAP0RLDEN_Msk|ECAP_CTL1_CAP1RLDEN_Msk));

    /* Enable ECAP0 Input Channel 0*/
    ECAP_ENABLE_INPUT_CHANNEL(ECAP0, ECAP_CTL0_IC0EN_Msk);

    /* Enable ECAP0 source from IC0 */
    ECAP_SEL_INPUT_SRC(ECAP0, ECAP_IC0, ECAP_CAP_INPUT_SRC_FROM_IC);

    /* Select IC0 detect rising edge */
    ECAP_SEL_CAPTURE_EDGE(ECAP0, ECAP_IC0, ECAP_RISING_EDGE);

    /* Input Channel 0 interrupt enabled */
    ECAP_EnableINT(ECAP0, ECAP_CTL0_CAPIEN0_Msk);
}

void Timer0_Init(void)
{

    /* Open Timer0 in periodic mode, enable interrupt and 1 interrupt tick per second */
    TIMER_Open(TIMER0,TIMER_PERIODIC_MODE,10000);
    TIMER_EnableInt(TIMER0);

    /* Enable Timer0 NVIC */
    NVIC_EnableIRQ(TIMER0_IRQn);

}

int main(void)
{
    uint32_t u32Hz=0, u32Hz_DET=0;

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    initialise_monitor_handles();
#endif

    printf("System core clock = %d\n", SystemCoreClock);
    printf("+----------------------------------------------------------+\n");
    printf("|   M460 Enhanced Input Capture Timer Driver Sample Code   |\n");
    printf("+----------------------------------------------------------+\n");
    printf("\n");
    printf("  !! GPIO PA.0 toggle periodically    !!\n");
    printf("  !! Connect PA.0 --> PA.10(ECAP0_IC0) !!\n\n");
    printf("     Press any key to start test\n\n");
    getchar();

    /* Initial ECAP0 function */
    ECAP0_Init();

    /* Initial Timer0 function */
    Timer0_Init();

    /* Configure PA.0 as output mode */
    GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);

    /* Start Timer0 counting */
    TIMER_Start(TIMER0);

    /* Delay 200ms */
    CLK_SysTickDelay(200000);

    /* Init & clear ECAP interrupt status flags */
    u32Status = ECAP_GET_INT_STATUS(ECAP0);
    ECAP0->STATUS = u32Status;

    /* ECAP_CNT starts up-counting */
    ECAP_CNT_START(ECAP0);

    while(1)
    {
        if(u32Status != 0)
        {
            /* Input Capture status is changed, and get a new hold value of input capture counter */
            u32Status = 0;

            /* Calculate the IC0 input frequency */
            u32Hz_DET = (SystemCoreClock/2) / (u32IC0Hold + 1);


            if(u32Hz != u32Hz_DET)
            {
                /* If IC0 input frequency is changed, Update frequency */
                u32Hz = u32Hz_DET;
            }
            else
            {
                printf("\nECAP0_IC0 input frequency is %d (Hz),u32IC0Hold=0x%08x\n", u32Hz,u32IC0Hold);
                TIMER_Stop(TIMER0); //Disable timer Counting.
                break;
            }
        }
    }
    /* Disable External Interrupt */
    NVIC_DisableIRQ(ECAP0_IRQn);
    NVIC_DisableIRQ(TIMER0_IRQn);

    /* Disable ECAP function */
    ECAP_Close(ECAP0);

    /* Disable Timer0 IP clock */
    CLK_DisableModuleClock(TMR0_MODULE);

    /* Disable ECAP IP clock */
    CLK_DisableModuleClock(ECAP0_MODULE);

    printf("\nExit ECAP sample code\n");

    /* Got no where to go, just loop forever */
    while (1) ;
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/