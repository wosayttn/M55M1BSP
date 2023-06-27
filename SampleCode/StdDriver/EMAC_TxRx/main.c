/******************************************************************************
* @file     main.c
* @version  V1.00
* @brief    This Ethernet sample tends to get a DHCP lease from DHCP server.
*           After IP address configured, this sample can reply to PING packets.
*           This sample shows how to use EMAC driver to simply handle RX and TX packets,
*           it is not suitable for performance and stress testing.
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
/*
 * This is a EMAC_TxRx project for M55M1 series MCU.
 * Users can create their own application based on this project.
 *
 * This project uses internal RC as APLL0 clock source and UART0 to print messages.
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
#include "emac.h"
#include "net.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_au8MacAddr[6] = DEFAULT_MAC0_ADDRESS;
uint8_t volatile g_au8IpAddr[4] = {0, 0, 0, 0};

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

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

    CLK_EnableModuleClock(EMAC0_MODULE);

    /* Set PB multi-function pins for Debug UART RXD and TXD */
    SetDebugUartMFP();

    SET_EMAC0_RMII_MDC_PE8();
    SET_EMAC0_RMII_MDIO_PE9();
    SET_EMAC0_RMII_TXD0_PE10();
    SET_EMAC0_RMII_TXD1_PE11();
    SET_EMAC0_RMII_TXEN_PE12();
    SET_EMAC0_RMII_REFCLK_PC8();
    SET_EMAC0_RMII_RXD0_PC7();
    SET_EMAC0_RMII_RXD1_PC6();
    SET_EMAC0_RMII_CRSDV_PA7();
    SET_EMAC0_RMII_RXERR_PA6();
    SET_EMAC0_PPS_PB6();

    /* Lock protected registers */
    SYS_LockReg();
}


/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    
    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    initialise_monitor_handles();
#endif

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    printf("+-----------------------------------+\n");
    printf("|    M55M1 EMAC Tx/Rx Sample Code   |\n");
    printf("|       - Get IP from DHCP server   |\n");
    printf("+-----------------------------------+\n\n");

    EMAC_Open(&g_au8MacAddr[0]);

    if (dhcp_start() < 0)
    {
        // Cannot get a DHCP lease
        printf("\nDHCP failed......\n");
    }

    while (1) {}
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
