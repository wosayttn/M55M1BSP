/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Demonstrate how to initial SecureISP function in secure code.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "hid_transfer.h"

extern int32_t ExecuteSecureISP(void);

void SYS_Init(void);
void UART0_Init(void);

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable internal RC 48MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRC48MEN_Msk);

    /* Waiting for internal RC 48MHz clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRC48MSTB_Msk);

    /* Enable PLL0 180MHz clock from HIRC and switch SCLK clock source to PLL0 */
    CLK_SetBusClock(CLK_SCLKSEL_SCLKSEL_APLL0, FREQ_180MHZ);

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

    printf("\nCPU @ %d Hz\n\n", SystemCoreClock);
    printf("[ SecureISP Demo (VECMAP: 0x%x) ]\n\n", FMC_GetVECMAP());

    ExecuteSecureISP();

    while (1) {}
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
