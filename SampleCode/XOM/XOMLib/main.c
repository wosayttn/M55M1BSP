/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief    Show how to config/erase XOM region.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "NuMicro.h"
#include "xomapi.h"

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

    /* Enable UART module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /* Lock protected registers */
    SYS_LockReg();
}


int32_t main(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();

    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

    /*
        This sample code is used to show how to build an XOM libary.

        The location of XOM region is defined by linker file: xom_scatter.scf(Keil)/xom.icf(IAR)
        The API header file is xomapi.h
        The XOM functions are implemented in xom.c

        This project is only used to build code for XOM region and test its funcitons.
        To enable XOM region, please use "NuMicro ICP Programming Tool".

        example flow:
        1. Build XOMLib_Code
        2. Download XOMLib_Code by press key "F8" in Keil MDK.
        3. Use defined XOM_START and XOM_SIZE in xom.scatter or xom.icf and
           open "NuMicro ICP Programming Tool" to set and enable XOM region.
        4. Build XOMLib to generate library (xomlib.lib in Keil or xomlib.a in IAR) located at lib directory.
        5. Pass xomlib.lib(Keil)/xomlib.a(IAR) & xomlib.h to the people who will call the funcitons in XOM.
    */

    printf("\n\n");
    printf("+----------------------------------------+\n");
    printf("|      FMC XOM Libary Build Example      |\n");
    printf("+----------------------------------------+\n");

    /* Unlock protected registers to operate FMC ISP function */
    SYS_UnlockReg();

    /* Enable FMC ISP function and enable APROM active*/
    FMC_Open();
    FMC_ENABLE_AP_UPDATE();

    /* Read User Configuration */
    printf("\n");
    printf("XOM Status = 0x%X\n", FMC->XOMSTS);

    /* Run XOM function */
    printf("\n");
    printf(" 100 + 200 = %d\n", XOM_Add(100, 200));
    printf(" 500 - 100 = %d\n", XOM_Sub(500, 100));
    printf(" 200 * 100 = %d\n", XOM_Mul(200, 100));
    printf("1000 / 250 = %d\n", XOM_Div(1000, 250));
    XOM_Sum(0, 3);


    while (1);
}


/*** (C) COPYRIGHT 2019 Nuvoton Technology Corp. ***/
