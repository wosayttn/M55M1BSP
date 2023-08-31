/***************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate how to update chip flash data through USB HID interface
 *           between chip USB device and PC.
 *           Nuvoton NuMicro ISP Programming Tool is also required in this
 *           sample code to connect with chip USB device and assign update file
 *           of Flash.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "targetdev.h"
#include "hid_transfer.h"

#define PLL_CLOCK       FREQ_180MHZ
#define TRIM_INIT       (SYS_BASE + 0x10C)

int32_t g_FMC_i32ErrCode = 0;

void ProcessHardFault(void);
void SH_Return(void);
void SendChar_ToUART(void);
int32_t SYS_Init(void);

void ProcessHardFault(void) {}
void SH_Return(void) {}
void SendChar_ToUART(void) {}

uint32_t CLK_GetPLLClockFreq(void)
{
    return PLL_CLOCK;
}

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
    CyclesPerUs = SystemCoreClock / 1000000UL;

    /* Select USBD */
    SYS->USBPHY = (SYS->USBPHY & ~SYS_USBPHY_USBROLE_Msk) | SYS_USBPHY_OTGPHYEN_Msk;

    /* Enable USBD module clock */
    CLK->USBDCTL |= CLK_USBDCTL_USBD0CKEN_Msk;

    /* Select USB module clock source as HIRC48M and USB clock divider as 1 */
    CLK->USBSEL = (CLK->USBSEL & (~CLK_USBSEL_USBSEL_Msk)) | CLK_USBSEL_USBSEL_HIRC48M;
    CLK->USBDIV = (CLK->USBDIV & (~CLK_USBDIV_USBDIV_Msk)) | CLK_USBDIV_USBDIV(1);

    /* Enable GPIO B clock */
    CLK->GPIOCTL |= CLK_GPIOCTL_GPIOBCKEN_Msk;

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB.12 to input mode */
    PB->MODE &= ~(GPIO_MODE_MODE12_Msk);
    SET_GPIO_PB12();

    /* USBD multi-function pins for VBUS, D+, D-, and ID pins */
    SET_USB_VBUS_PA12();
    SET_USB_D_MINUS_PA13();
    SET_USB_D_PLUS_PA14();
    SET_USB_OTG_ID_PA15();

    return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    uint32_t u32TrimInit;

    /* Unlock write-protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    if ((SYS_Init() < 0) || (DETECT_PIN != 0))
        goto _APROM;

    /* Enable ISP */
    FMC_Open();
    FMC_ENABLE_AP_UPDATE();

    /* Get APROM and Data Flash size */
    g_u32ApromSize = GetApromSize();

    while (DETECT_PIN == 0)
    {
        /* Open USB controller */
        USBD_Open(&gsInfo, HID_ClassRequest, NULL);
        /*Init Endpoint configuration for HID */
        HID_Init();
        /* Start USB device */
        USBD_Start();

        /* Backup default trim */
        u32TrimInit = M32(TRIM_INIT);
        /* Clear SOF */
        USBD->INTSTS = USBD_INTSTS_SOFIF_Msk;

        /* M55M1 has 8 KB LDROM, changed to use IRQ mode */
        /* Enable USBD interrupt */
        NVIC_EnableIRQ(USBD_IRQn);

        while (DETECT_PIN == 0)
        {
            /* Start USB trim if it is not enabled. */
            if ((SYS->TCTL48M & SYS_TCTL48M_FREQSEL_Msk) != 1)
            {
                /* Start USB trim only when SOF */
                if (USBD->INTSTS & USBD_INTSTS_SOFIF_Msk)
                {
                    /* Clear SOF */
                    USBD->INTSTS = USBD_INTSTS_SOFIF_Msk;

                    /*
                        USB clock trim function:
                        HIRC Trimming with boundary function enhances robustility
                        and keeps HIRC in right frequency while receiving unstable USB signal
                    */
                    SYS->TCTL48M = (0x1 << SYS_TCTL48M_FREQSEL_Pos) | SYS_TCTL48M_REFCKSEL_Msk |
                                   SYS_TCTL48M_BOUNDEN_Msk | (8 << SYS_TCTL48M_BOUNDARY_Pos);
                }
            }

            /* Disable USB Trim when any error found */
            if (SYS->TISTS48M & (SYS_TISTS48M_CLKERRIF_Msk | SYS_TISTS48M_TFAILIF_Msk))
            {
                /* Init TRIM */
                M32(TRIM_INIT) = u32TrimInit;

                /* Disable crystal-less */
                SYS->TCTL48M = 0;

                /* Clear trim error flags */
                SYS->TCTL48M = SYS_TISTS48M_CLKERRIF_Msk | SYS_TISTS48M_TFAILIF_Msk;

                /* Clear SOF */
                USBD_CLR_INT_FLAG(USBD_INTSTS_SOFIF_Msk);
            }

            if (g_u8UsbDataReady == TRUE)
            {
                ParseCmd((uint8_t *)g_u8UsbRcvBuff, 64);
                EP2_Handler();
                g_u8UsbDataReady = FALSE;
            }
        }

        goto _APROM;
    }

_APROM:
    /* Reset system and boot from APROM */
    FMC_SetVectorPageAddr(FMC_APROM_BASE);
    NVIC_SystemReset();

    /* Trap the CPU */
    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
