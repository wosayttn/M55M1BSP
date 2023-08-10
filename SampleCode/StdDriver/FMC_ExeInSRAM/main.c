/****************************************************************************//**
 * @file    main.c
 * @version V1.0
 * @brief   Implement a code and execute in SRAM to program embedded Flash.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

/* Vector table has been placed in DTCM by default. */
extern int32_t FlashAccess_OnSRAM(void);

volatile uint32_t g_u32Ticks = 0;

/* Declared NVT_ITCM to place SysTick_Handler in ITCM */
NVT_ITCM void SysTick_Handler(void)
{
    //if ((g_u32Ticks++ % 1000) == 0)
    {
        //printf("[%s@0x%08X] Time elapsed: %d\n", __func__, __PC(), (g_u32Ticks / 1000));
    }
    printf("[%s@0x%08X] Time elapsed: %d\n", __func__, __PC(), (g_u32Ticks++));
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

    /* Lock protected registers */
    SYS_LockReg();
}

int32_t main(void)
{
    uint32_t  i;
    uint32_t *pu32Vectors = (uint32_t *)FMC_APROM_BASE;

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART for print message */
    InitDebugUart();

    printf("+--------------------------------------------+\n");
    printf("| M55M1 FMC Code Execute in SRAM Sample Code |\n");
    printf("+--------------------------------------------+\n");

    /* SysTick used for test interrupts in SRAM */
    SysTick_Config(SystemCoreClock / 1000);

    /*
       This sample code demonstrates how to make a sub-routine code executed in SRAM.
    */

    printf("Will branch to SRAM address: 0x%08X\n", (uint32_t)FlashAccess_OnSRAM);

    if (FlashAccess_OnSRAM())
    {
        printf("Flash access return error !\n");
    }
    else
    {
        printf("Flash access return ok.\n");
    }

    printf("\nEnd of FMC Sample Code\n");

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
