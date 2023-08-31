/***************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate how to update chip flash data through RS485 interface
 *           between chip RS485 and ISP Tool.
 *           Nuvoton NuMicro ISP Programming Tool is also required in this
 *           sample code to connect with chip RS485 and assign update file
 *           of Flash.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "targetdev.h"
#include "uart_transfer.h"

#define PLL_CLOCK       FREQ_180MHZ
#define nRTSPin         (PA0)
#define RECEIVE_MODE    (0)
#define TRANSMIT_MODE   (1)

int32_t g_FMC_i32ErrCode = 0;

int32_t SYS_Init(void)
{
    uint32_t u32TimeOutCnt = SystemCoreClock >> 1; /* 500ms time-out */

    /* Unlock protected registers */
    SYS_UnlockReg();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable Internal RC 12MHz clock */
    CLK->SRCCTL |= CLK_SRCCTL_HIRCEN_Msk;

    /* Waiting for Internal RC clock ready */
    while ((CLK->STATUS & CLK_STATUS_HIRCSTB_Msk) != CLK_STATUS_HIRCSTB_Msk)
    {
        if (--u32TimeOutCnt == 0)
        {
            return -1;
        }
    }

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
    PllClock        = PLL_CLOCK;
    SystemCoreClock = PllClock;
    CyclesPerUs = SystemCoreClock / 1000000UL;

    /* Enable UART0 module clock */
    CLK->UARTSEL0 = (CLK->UARTSEL0 & ~CLK_UARTSEL0_UART1SEL_Msk) | CLK_UARTSEL0_UART1SEL_HIRC;
    CLK->UARTCTL |= CLK_UARTCTL_UART1CKEN_Msk;
    /* Check clock stable */
    u32TimeOutCnt = SystemCoreClock >> 1;

    while ((CLK->UARTCTL & BIT31) == 0UL)
    {
        if (--u32TimeOutCnt == 0)
        {
            return -1;
        }
    }

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set multi-function pins for UART0 RXD and TXD */
    SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13();
    PA->MODE = (PA->MODE & (~GPIO_MODE_MODE0_Msk)) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE0_Pos);
    nRTSPin = RECEIVE_MODE;
    SET_UART1_RXD_PA2();
    SET_UART1_TXD_PA3();

    /* Lock protected registers */
    SYS_LockReg();

    return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    if (SYS_Init() < 0)
        goto _APROM;

    /* Init UART */
    UART_Init();

    /* Enable ISP */
    FMC_Open();
    FMC_ENABLE_AP_UPDATE();

    /* Get APROM and Data Flash size */
    g_u32ApromSize = GetApromSize();

    /* Set Systick time-out for 300ms */
    SysTick->LOAD = 300000 * CyclesPerUs;
    SysTick->VAL  = (0x00);
    SysTick->CTRL = SysTick->CTRL | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;   /* Use CPU clock */

    /* Wait for CMD_CONNECT command until Systick time-out */
    while (1)
    {
        /* Wait for CMD_CONNECT command */
        if ((g_u8bufhead >= 4) || (g_u8bUartDataReady == TRUE))
        {
            uint32_t u32lcmd;
            u32lcmd = inpw((uint32_t)g_au8uart_rcvbuf);

            if (u32lcmd == CMD_CONNECT)
            {
                goto _ISP;
            }
            else
            {
                g_u8bUartDataReady = FALSE;
                g_u8bufhead = 0;
            }
        }

        /* Systick time-out, then go to APROM */
        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
        {
            goto _APROM;
        }
    }

_ISP:

    /* Parse command from master and send response back */
    while (1)
    {
        if (g_u8bUartDataReady == TRUE)
        {
            g_u8bUartDataReady = FALSE;         /* Reset UART data ready flag */
            ParseCmd(g_au8uart_rcvbuf, 64);     /* Parse command from master */
            NVIC_DisableIRQ(UART1_IRQn);        /* Disable NVIC */
            nRTSPin = TRANSMIT_MODE;            /* Control RTS in transmit mode */
            PutString();                        /* Send response to master */

            /* Wait for data transmission is finished */
            while ((UART1->FIFOSTS & UART_FIFOSTS_TXEMPTYF_Msk) == 0);

            nRTSPin = RECEIVE_MODE;             /* Control RTS in receive mode */
            NVIC_EnableIRQ(UART1_IRQn);         /* Enable NVIC */
        }
    }

_APROM:

    /* Reset system and boot from APROM */
    FMC_SetVectorPageAddr(FMC_APROM_BASE);
    NVIC_SystemReset();

    /* Trap the CPU */
    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
