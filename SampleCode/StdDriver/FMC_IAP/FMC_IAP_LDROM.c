/******************************************************************************
* @file    main.c
* @version V1.00
* @brief   Template for M55M1 series MCU
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

typedef void (*PFN_FUNC_PTR)(void);

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

    /* Lock protected registers */
    SYS_LockReg();
}

int main()
{
    PFN_FUNC_PTR pfnAPROM_Entry;   /* function pointer */
    uint32_t     u32TimeOutCnt;     /* time-out counter */

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART for print message */
    InitDebugUart();

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    initialise_monitor_handles();
#endif

    printf("\n\n");
    printf("+-------------------------------------+\n");
    printf("|     M55M1 FMC IAP Sample Code       |\n");
    printf("|          [LDROM code]               |\n");
    printf("+-------------------------------------+\n");

    SYS_UnlockReg();    /* Unlock protected registers */
    FMC_Open();         /* Enable FMC ISP function */

    printf("\n\nPress any key to branch to APROM...\n");
    getchar();                         /* block on waiting for any one character input from UART0 */

    printf("\n\nChange VECMAP and branch to APROM...\n");
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
    while (!(UART0->FIFOSTS & UART_FIFOSTS_TXEMPTY_Msk))        /* wait until UART0 TX FIFO is empty */
        if (--u32TimeOutCnt == 0) break;

    /*  NOTE!
     *     Before change VECMAP, user MUST disable all interrupts.
     */
    FMC_SetVectorPageAddr(FMC_APROM_BASE);        /* Vector remap APROM page 0 to address 0. */
    if (g_FMC_i32ErrCode != 0)
    {
        printf("FMC_SetVectorPageAddr(FMC_APROM_BASE) failed!\n");
        goto lexit;
    }

    SYS_LockReg();                                /* Lock protected registers */

    /*
     *  The reset handler address of an executable image is located at offset 0x4.
     *  Thus, this sample get reset handler address of APROM code from FMC_APROM_BASE + 0x4.
     */
    pfnAPROM_Entry = (PFN_FUNC_PTR) * (uint32_t *)(FMC_APROM_BASE + 4);

    /*
     *  The stack base address of an executable image is located at offset 0x0.
     *  Thus, this sample get stack base address of APROM code from FMC_APROM_BASE + 0x0.
     */

    __set_MSP(FMC_APROM_BASE);

    /*
     *  Branch to the APROM code's reset handler in way of function call.
     */
    pfnAPROM_Entry();

lexit:

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
