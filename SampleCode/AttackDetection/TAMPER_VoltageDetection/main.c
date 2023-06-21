/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Show the usage of TAMPER voltage detection function.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Functions and variables declaration                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void);
void UART_Init(void);

/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void)
{
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);
    /* Enable Internal RC 32KHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    /* Waiting for Low speed Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);

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
    /* set power level to level 1 */
    PMC_SetPowerLevel(PMC_PLCTL_PLSEL_PL1);

    /* Enable module clock */
    CLK_EnableModuleClock(TAMPER0_MODULE);
    CLK_EnableModuleClock(FMC0_MODULE);
    CLK_EnableModuleClock(ISP0_MODULE);

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

}
/*---------------------------------------------------------------------------------------------------------*/
/* Init UART                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void UART_Init(void)
{
    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}
/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    uint32_t u32TimeOutCnt;
    uint32_t u32Confi3Read;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART for printf */
    UART_Init();

    /* Enable FMC ISP function */
    FMC_Open();

    /* Enable APROM update function */
    FMC_ENABLE_AP_UPDATE();

    /* Enable User Configuration update function */
    FMC_ENABLE_CFG_UPDATE();

    /* Enable Tamper Domain */
    u32Confi3Read = FMC_Read(FMC_USER_CONFIG_3);
    printf("CONFI3=0x%8X\n", u32Confi3Read);

    if (u32Confi3Read == 0x5aa5ffff)
    {
        FMC_ENABLE_ISP();
        FMC_ENABLE_CFG_UPDATE();
        FMC_WriteConfig(FMC_USER_CONFIG_3, 0x5a5affff);
        SYS_ResetChip();

        while (1);
    }

    /* Lock protected registers */
    SYS_LockReg();

    printf("\n\nCPU @ %uHz\n", SystemCoreClock);
    printf("+------------------------------------------+\n");
    printf("|   TAMPER Voltage Detection Sample Code   |\n");
    printf("+------------------------------------------+\n\n");
    printf("System will reset if any voltage detection interrupt is detected.\n\n");
    printf("Press any key to start test.\n");
    getchar();
    printf("\n");

    /* Reset tamper coreblock */
    TAMPER_CORE_RESET();
    TAMPER_CORE_RELEASE();

    /* Enable over voltage detector and wait until stable */
    SYS_UnlockReg();

    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

    while (SYS->OVDCTL & SYS_OVDCTL_WRBUSY_Msk)
    {
        if (--u32TimeOutCnt == 0)
        {
            printf("Wait for over voltage detector stable time-out!\n");

            return -1;
        }
    }

    SYS->OVDCTL = SYS_OVDCTL_OVDEN_Msk;

    SYS_LockReg();

    /* Enable battery loss detector */
    RTC->TEST = RTC_TEST_BATDETEN_Msk;
    SYS_UnlockReg();
    SYS->BATLDCTL = SYS_BATLDCTL_BATLDEN_Msk;
    SYS_LockReg();

    /* Initialize the trim value of under-shoot and over-shoot detection level */
    TAMPER_TLVD_TRIM_INIT(TAMPER_LBSTRIM_TLVDSEL_0_90V);
    TAMPER_TOVD_TRIM_INIT(TAMPER_LBSTRIM_TOVDSEL_1_40V);

    /* Clear different voltage interrupt flag */
    TAMPER_CLR_INT_STATUS(TAMPER_INTSTS_OVPOUTIF_Msk | TAMPER_INTSTS_VBATLOSSIF_Msk | TAMPER_INTSTS_BODIF_Msk);

    /* Enable different voltage detection interrupt */
    TAMPER_EnableInt(TAMPER_INTEN_OVPIEN_Msk | TAMPER_INTEN_RTCLVRIEN_Msk | TAMPER_INTEN_VLOSSIEN_Msk | TAMPER_INTEN_BODIEN_Msk);

    /* Enable to trigger chip reset */
    TAMPER_ENABLE_CHIPRST();

    /* Wait for voltage detection interrupt happened */
    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/