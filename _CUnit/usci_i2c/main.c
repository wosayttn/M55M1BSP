/******************************************************************************
* @file    main.c
* @version V1.00
* @brief   USCI_I2C CUnit
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "usci_i2c_cunit.h"

#ifndef DEBUG_PORT
#define DEBUG_PORT UART0
#endif

void InitDebugUart(void)
{
    DEBUG_PORT->BAUD = (UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(153600, 38400));
    DEBUG_PORT->LINE = (UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1);
}

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
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0, CLK_ACLKDIV_ACLKDIV(1));
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
    /* Select UART clock source from HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HIRC, CLK_UARTDIV0_UART0DIV(1));
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();
    //
    CLK->USCICTL = CLK_USCICTL_USCI0CKEN_Msk;
    SET_USCI0_CLK_PA11();
    SET_USCI0_DAT0_PA10();
    SYS_ResetModule(SYS_USCI0RST);
    //
    CLK->I2CCTL |= CLK_I2CCTL_I2C0CKEN_Msk;
    SET_I2C0_SCL_PA5();
    SET_I2C0_SDA_PA4();
    SYS_ResetModule(SYS_I2C0RST);
    //
    SYS_LockReg();
    //
}

void exit(int32_t code)
{
    if (code)
        while (1);  // Fail
    else
        while (1);  // Success
}

void AddTests(void)
{
    assert((NULL != CU_get_registry()));
    assert(!CU_is_test_running());

    if (CUE_SUCCESS != CU_register_suites(suites)) {
        fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART for print message */
    InitDebugUart();
    printf("\n\n");
    printf("+--------------------------------------+\n");
    printf("|       M55M1 USCI_I2C CUnit Test      |\n");
    printf("+--------------------------------------+\n");

    if (CU_initialize_registry()) {
        fprintf(stderr, " Initialization of Test Registry failed. ");
        exit(EXIT_FAILURE);
    } else {
        AddTests();
        CU_console_run_tests();
        CU_cleanup_registry();
    }

    while (1) ;
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
