/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Show how to check Key Store status
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

void SYS_Init(void)
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

    /* Enable module clock */
    CLK_EnableModuleClock(KS0_MODULE);
    CLK_EnableModuleClock(CRYPTO0_MODULE);

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
/*  MAIN function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    int32_t i, i32KeyCnt;

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART for print message */
    InitDebugUart();

    printf("+------------------------------------------+\n");
    printf("|      M55M1 KS Key Status Sample Code     |\n");
    printf("+------------------------------------------+\n");

    if (KS_Open() != 0)
    {
        printf("KS_Open failed !\n");
        goto Error_Exit;
    }

    printf("KS SRAM  remain size     : %d\n", KS_GetRemainSize(KS_SRAM));
    printf("KS SRAM  remain key count: %d\n", KS_GetRemainKeyCount(KS_SRAM));
    printf("KS Flash remain size     : %d\n", KS_GetRemainSize(KS_FLASH));
    printf("KS Flash remain key count: %d\n", KS_GetRemainKeyCount(KS_FLASH));
    printf("KS OTP keys:\n");

    for (i32KeyCnt = 0, i = 0; i <= KS_MAX_OTPKEY_CNT; i++)
    {
        printf("   OTP%-2d [%c]\n", i, (KS_GET_OTPKEY_STS(i) ? 'v' : 'x'));

        if (KS->OTPSTS & (1 << i))
        {
            i32KeyCnt++;
        }
    }

    printf("------------------------------------------------\n");

    printf("Total %d OTP Keys in Key Store\n", i32KeyCnt);

Error_Exit:
    while (1) {}
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
