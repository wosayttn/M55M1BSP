/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * @brief    Use Flash as storage to implement a USB Mass-Storage device.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "massstorage.h"

#define CRYSTAL_LESS        1
#define TRIM_INIT           (SYS_BASE+0xF40)

void SYS_Init(void);

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

   /* Enable APLL0 180MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_180MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to APLL0 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0);

    /* Set HCLK2 divide 2 */
    CLK_SET_HCLK2DIV(2);
    
    /* Set PCLKx divide 2 */
    CLK_SET_PCLK1DIV(2);
    CLK_SET_PCLK0DIV(2);
    CLK_SET_PCLK2DIV(2);
    CLK_SET_PCLK3DIV(2);
    CLK_SET_PCLK4DIV(2);

    SystemCoreClockUpdate();

    /* Enable GPA ~ GPJ peripheral clock */
    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOD_MODULE);
    CLK_EnableModuleClock(GPIOE_MODULE);
    CLK_EnableModuleClock(GPIOF_MODULE);
    CLK_EnableModuleClock(GPIOG_MODULE);
    CLK_EnableModuleClock(GPIOH_MODULE);
    CLK_EnableModuleClock(GPIOJ_MODULE);

#if (!CRYSTAL_LESS)
    /* Enable External RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);

    /* Waiting for External RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

   /* Enable APLL0 180MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HXT, FREQ_180MHZ, CLK_APLL1_SELECT);   

    /* Select USB clock source as PLL/2 and USB clock divider as 2 */
    CLK_SetModuleClock(USBD_MODULE, CLK_USBSEL_USBSEL_APLL1_DIV2, CLK_USBDIV_USBDIV(2));
#else
    /* Enable HIRC48M clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRC48MEN_Msk);

    /* Waiting for HIRC48M clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRC48MSTB_Msk);

    /* Set core clock to 1800MHz */
    CLK_SetCoreClock(FREQ_180MHZ);

    /* Select USB clock source as HIRC48M and USB clock divider as 1 */
    CLK_SetModuleClock(USBD0_MODULE, CLK_USBSEL_USBSEL_HIRC48M, CLK_USBDIV_USBDIV(1));
#endif

   /* Debug UART clock setting*/
    SetDebugUartCLK();

    /* Select USBD */
    SYS->USBPHY = (SYS->USBPHY & ~SYS_USBPHY_USBROLE_Msk) | SYS_USBPHY_OTGPHYEN_Msk ;

    /* Enable USBD module clock */
    CLK_EnableModuleClock(USBD0_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set multi-function pins for UART0 RXD and TXD */
    SetDebugUartMFP();

    /* USBD multi-function pins for VBUS, D+, D-, and ID pins */
    SET_USB_VBUS_PA12();
    SET_USB_D_MINUS_PA13();
    SET_USB_D_PLUS_PA14();
    SET_USB_OTG_ID_PA15();
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
#if CRYSTAL_LESS
    uint32_t u32TrimInit;
#endif

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    InitDebugUart();

    printf("\n");
    printf("+-------------------------------------------------------+\n");
    printf("|          NuMicro USB MassStorage Sample Code          |\n");
    printf("+-------------------------------------------------------+\n");

    /* Enable FMC ISP function */
    FMC_Open();

    /* Enable Read/Write flash function */
    FMC_ENABLE_AP_UPDATE();

    printf("NuMicro USB MassStorage Start!\n");

    USBD_Open(&gsInfo, MSC_ClassRequest, NULL);
    USBD_SetConfigCallback(MSC_SetConfig);

    /* Endpoint configuration */
    MSC_Init();
    USBD_Start();

    NVIC_EnableIRQ(USBD_IRQn);

#if CRYSTAL_LESS
    /* Backup default trim */
    u32TrimInit = M32(TRIM_INIT);
#endif

    /* Clear SOF */
    USBD->INTSTS = USBD_INTSTS_SOFIF_Msk;

    while(1)
    {
#if CRYSTAL_LESS
         /* Start USB trim if it is not enabled. */
        if((SYS->TCTL48M & SYS_TCTL48M_FREQSEL_Msk) != 1)
        {
            /* Start USB trim only when SOF */
            if(USBD->INTSTS & USBD_INTSTS_SOFIF_Msk)
            {
                /* Clear SOF */
                USBD_CLR_INT_FLAG(USBD_INTSTS_SOFIF_Msk);

                /* Re-enable crystal-less */
                SYS->TCTL48M = 0x01;
                SYS->TCTL48M |= SYS_TCTL48M_REFCKSEL_Msk | SYS_TCTL48M_BOUNDEN_Msk | (8 << SYS_TCTL48M_BOUNDARY_Pos);
            }
        }

        /* Disable USB Trim when error */
        if(SYS->TISTS48M & (SYS_TISTS48M_CLKERRIF_Msk | SYS_TISTS48M_TFAILIF_Msk))
        {
            /* Init TRIM */
            M32(TRIM_INIT) = u32TrimInit;

            /* Disable crystal-less */
            SYS->TISTS48M = 0;

            /* Clear error flags */
            SYS->TISTS12M = SYS_TISTS48M_CLKERRIF_Msk | SYS_TISTS48M_TFAILIF_Msk;

            /* Clear SOF */
            USBD_CLR_INT_FLAG(USBD_INTSTS_SOFIF_Msk);
        }
#endif

        MSC_ProcessCmd();
    }
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/