/***************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate how to upgrade firmware between HUSB device and PC
 *           through USB DFU (Device Firmware Upgrade) class.
 *           WindowsDriver and WindowsTool are also included in this sample code
 *           to connect with USB device.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "fmc_user.h"
#include "dfu_transfer.h"

#define PLL_CLOCK       FREQ_180MHZ

int32_t g_FMC_i32ErrCode = 0;

int32_t SYS_Init(void)
{
    uint32_t u32TimeOutCnt = SystemCoreClock >> 1; /* 500ms time-out */

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable Internal RC clock */
    CLK->SRCCTL |= (CLK_SRCCTL_HIRCEN_Msk | CLK_SRCCTL_HIRC48MEN_Msk);

    /* Waiting for Internal RC clock ready */
    while ((CLK->STATUS & (CLK_STATUS_HIRCSTB_Msk | CLK_STATUS_HIRC48MSTB_Msk)) != (CLK_STATUS_HIRCSTB_Msk | CLK_STATUS_HIRC48MSTB_Msk))
    {
        if (--u32TimeOutCnt == 0)
        {
            return -1;
        }
    }

    /* Set SCLK clock source as HIRC first */
    CLK->SCLKSEL = (CLK->SCLKSEL & (~CLK_SCLKSEL_SCLKSEL_Msk)) | CLK_SCLKSEL_SCLKSEL_HIRC;

    /* Disable PLL clock before setting PLL frequency */
    CLK->SRCCTL &= ~CLK_SRCCTL_APLL0EN_Msk;

    /* Set PLL clock as 180MHz from HIRC */
    CLK->APLL0CTL = CLK_APLLCTL_180MHz;

    /* Wait for PLL clock ready */
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

    while (!(CLK->STATUS & CLK_STATUS_APLL0STB_Msk))
    {
        if (--u32TimeOutCnt == 0)
            return -1;
    }

    /* Switch to power level 0 for safe */
    PMC->PLCTL = (PMC->PLCTL & (~PMC_PLCTL_PLSEL_Msk)) | PMC_PLCTL_PLSEL_PL0;

    /* Set Flash Access Cycle to 8 for safe */
    FMC->CYCCTL = (FMC->CYCCTL & (~FMC_CYCCTL_CYCLE_Msk)) | (8);

    /* Switch SCLK to new SCLK source */
    CLK->SCLKSEL = (CLK->SCLKSEL & (~CLK_SCLKSEL_SCLKSEL_Msk)) | CLK_SCLKSEL_SCLKSEL_APLL0;

    /* Set power level according to new SCLK */
    PMC->PLCTL = (PMC->PLCTL & (~PMC_PLCTL_PLSEL_Msk)) | PMC_PLCTL_PLSEL_PL1;

    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

    while (PMC->PLSTS & PMC_PLSTS_PLCBUSY_Msk)
    {
        if (u32TimeOutCnt-- == 0) break;
    }

    /* Switch flash access cycle to suitable value base on SCLK */
    FMC->CYCCTL = (FMC->CYCCTL & (~FMC_CYCCTL_CYCLE_Msk)) | (6);

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
    CyclesPerUs = SystemCoreClock / 1000000UL;  /* For CLK_SysTickDelay() */

    /* Select HSUSBD */
    SYS->USBPHY &= ~SYS_USBPHY_HSUSBROLE_Msk;

    /* Enable USB PHY */
    SYS->USBPHY = (SYS->USBPHY & ~(SYS_USBPHY_HSUSBROLE_Msk | SYS_USBPHY_HSUSBACT_Msk)) | SYS_USBPHY_OTGPHYEN_Msk;
    CLK_SysTickDelay(20);   // delay > 10 us
    SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;

    /* Enable HSUSBD module clock */
    CLK->HSUSBDCTL |= CLK_HSUSBDCTL_HSUSBD0CKEN_Msk;

    /* Enable GPIO B clock */
    CLK->GPIOCTL |= CLK_GPIOCTL_GPIOBCKEN_Msk;

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB.12 to input mode */
    PB->MODE &= ~(GPIO_MODE_MODE12_Msk);
    SET_GPIO_PB12();

    return 0;
}

void HSUSBD_IRQHandler(void);
/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    /* Unlock write-protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    if (SYS_Init() < 0)
        goto _APROM;

    /* Enable ISP */
    FMC_Open();
    FMC_ENABLE_AP_UPDATE();

    /* Get APROM and Data Flash size */
    g_u32ApromSize = GetApromSize();
    GetDataFlashInfo(&g_u32DataFlashAddr, &g_u32DataFlashSize);

    if (DETECT_PIN != 0)
    {
        goto _APROM;
    }

    /* Open HSUSBD controller */
    HSUSBD_Open(NULL, DFU_ClassRequest, NULL);

    /*Init Endpoint configuration for DFU */
    DFU_Init();

    /* Start transaction */
    HSUSBD->OPER = HSUSBD_OPER_HISPDEN_Msk;   /* high-speed */
    HSUSBD_CLR_SE0();

    /* M55M1 has 8 KB LDROM, changed to use IRQ mode */
    /* Enable HSUSBD interrupt */
    NVIC_EnableIRQ(HSUSBD_IRQn);

    while (DETECT_PIN == 0)
    {

    }

_APROM:
    /* Reset system and boot from APROM */
    FMC_SetVectorPageAddr(FMC_APROM_BASE);
    NVIC_SystemReset();

    /* Trap the CPU */
    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
