/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate the methods of detecting reset abnormalities.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static volatile uint32_t s_u32ResetFlagCount = 5;

void SysTick_Handler(void);
void SYS_Init(void);

/*--------------------------------------------------------------------------------------------------------*/
/*  SysTick IRQ Handler                                                                                   */
/*--------------------------------------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
    if(s_u32ResetFlagCount >= 5)
    {
        printf("Reset abnormalities happened! The same reset event occurs repeatedly for a fixed period of time.\n");

        s_u32ResetFlagCount = 0;
    }
}

void SYS_Init(void)
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

    /* Switch SCLK clock source to PLL0 and divide 1 */
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

    /* Lock protected registers */
    SYS_LockReg();
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

    /* Generate interrupt each 1 s */
    SysTick_Config(SystemCoreClock / 1);
    
    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
    printf("+-------------------------------------+\n");
    printf("|   SYS Reset Detection Sample Code   |\n");
    printf("+-------------------------------------+\n\n");
    
    printf("Check UART message to confirm if any reset abnormalities have occurred.\n\n");

    while(1)
    {
        /* Get reset source is from reset pin reset */
        if(SYS_IS_RSTPIN_RST())
        {
            /* Clear reset source flag */
            SYS_ClearResetSrc(SYS_RSTSTS_PINRF_Msk);

            s_u32ResetFlagCount++;
        }

        /* Get reset sources are from Chip reset and CPU reset */
        while(SYS_IS_POR_RST() && SYS_IS_CPU_RST())
        {
            printf("Reset abnormalities happened! Different reset events occur at the same time.\n");
        }
    }
}