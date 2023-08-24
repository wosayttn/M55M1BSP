/***************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate how to update chip flash data through I2C interface.
 *           Nuvoton NuMicro ISP Programming Tool is also required in this
 *           sample code to connect with chip I2C and assign update file
 *           of Flash.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "targetdev.h"
#include "i2c_transfer.h"

#define PLL_CLOCK       FREQ_180MHZ

uint32_t Pclk0;
uint32_t Pclk1;

int32_t SYS_Init(void)
{
    uint32_t u32TimeOutCnt;

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable HIRC clock */
    CLK->SRCCTL |= CLK_SRCCTL_HIRCEN_Msk;

    /* Wait for HIRC clock ready */
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

    while (!(CLK->STATUS & CLK_STATUS_HIRCSTB_Msk))
    {
        if (--u32TimeOutCnt == 0)
            return -1;
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
    CLK->UARTSEL0 = (CLK->UARTSEL0 & ~CLK_UARTSEL0_UART0SEL_Msk) | CLK_UARTSEL0_UART0SEL_HIRC;
    CLK->UARTCTL |= CLK_UARTCTL_UART0CKEN_Msk;
    /* Check clock stable */
    u32TimeOutCnt = SystemCoreClock >> 1;

    while ((CLK->UARTCTL & BIT31) == 0UL)
    {
        if (--u32TimeOutCnt == 0)
        {
            return -1;
        }
    }

    /* Enable I2C1 clock */
    CLK->I2CCTL |= CLK_I2CCTL_I2C1CKEN_Msk;

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set multi-function pins for UART0 RXD and TXD */
    SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13();

    /* Set I2C1 multi-function pins */
    SET_I2C1_SDA_PA2();
    SET_I2C1_SCL_PA3();

    /* I2C pin enable schmitt trigger */
    PA->SMTEN |= GPIO_SMTEN_SMTEN2_Msk | GPIO_SMTEN_SMTEN3_Msk;

    return 0;
}

int main(void)
{
    uint32_t cmd_buff[16];

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    if (SYS_Init() < 0)
        goto _APROM;

    /* Enable ISP */
    FMC_Open();
    FMC_ENABLE_AP_UPDATE();

    g_apromSize = GetApromSize();
    GetDataFlashInfo(&g_dataFlashAddr, &g_dataFlashSize);
    I2C_Init();
    SysTick->LOAD = 300000 * CyclesPerUs;
    SysTick->VAL   = (0x00);
    SysTick->CTRL = SysTick->CTRL | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    while (1)
    {
        if (bI2cDataReady == 1)
        {
            goto _ISP;
        }

        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
        {
            goto _APROM;
        }
    }

_ISP:

    while (1)
    {
        if (bI2cDataReady == 1)
        {
            memcpy(cmd_buff, i2c_rcvbuf, 64);
            bI2cDataReady = 0;
            ParseCmd((unsigned char *)cmd_buff, 64);
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
