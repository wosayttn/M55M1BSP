/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Use Data Flash as back end storage media to simulate an USB pen drive.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "massstorage.h"

#define DATA_FLASH_BASE  0x00140000

/*--------------------------------------------------------------------------*/
void SYS_Init(void)
{
    uint32_t volatile i;

   /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    /* Enable External RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);

    /* Waiting for External RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
  
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

   /* Debug UART clock setting*/
    SetDebugUartCLK();

    /* Select HSUSBD */
    SYS->USBPHY &= ~SYS_USBPHY_HSUSBROLE_Msk;

    /* Enable USB PHY */
    SYS->USBPHY = (SYS->USBPHY & ~(SYS_USBPHY_HSUSBROLE_Msk)) | SYS_USBPHY_HSOTGPHYEN_Msk;

    for(i = 0; i < 0x1000; i++);   // delay > 10 us


    /* Enable HSUSBD module clock */
    CLK_EnableModuleClock(HSUSBD0_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

     /* Set multi-function pins for UART0 RXD and TXD */
    SetDebugUartMFP();
}

int32_t main(void)
{
    uint32_t au32Config[2];
  
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    InitDebugUart();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    printf("NuMicro HSUSBD Mass Storage\n");

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable FMC ISP function */
    FMC_Open();

    /* Enable Read/Write flash function */
    FMC_ENABLE_AP_UPDATE();

    /* Check if Data Flash Size is 64K. If not, to re-define Data Flash size and to enable Data Flash function */
//    if(FMC_ReadConfig(au32Config, 2) < 0)
//        return -1;

    if(((au32Config[0] & 0x01) == 1) || (au32Config[1] != DATA_FLASH_BASE))
    {
        FMC_ENABLE_CFG_UPDATE();
        au32Config[0] &= ~0x1;
        au32Config[1] = DATA_FLASH_BASE;
//        if(FMC_WriteConfig(au32Config, 2) < 0)
//            return -1;

//        FMC_ReadConfig(au32Config, 2);
        if(((au32Config[0] & 0x01) == 1) || (au32Config[1] != DATA_FLASH_BASE))
        {
            printf("Error: Program Config Failed!\n");

            /* Disable FMC ISP function */
            FMC_Close();
            SYS_LockReg();
            return -1;
        }

        printf("chip reset\n");

        /* Reset Chip to reload new CONFIG value */
         SYS_ResetChip();
    }

    HSUSBD_Open(&gsHSInfo, MSC_ClassRequest, NULL);

    /* Endpoint configuration */
    MSC_Init();

    /* Enable HSUSBD interrupt */
    NVIC_EnableIRQ(HSUSBD_IRQn);

    /* Start transaction */
    while(1)
    {
        if(HSUSBD_IS_ATTACHED())
        {
            HSUSBD_Start();
            break;
        }
    }

    while(1)
    {
        if(g_hsusbd_Configured)
            MSC_ProcessCmd();
    }
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/