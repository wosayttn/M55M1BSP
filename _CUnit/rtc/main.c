/******************************************************************************
* @file    main.c
* @version V1.00
* @brief   CUnit test main function
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

// Library header file
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "CUnit.h"
#include "Console.h"
#include "NuMicro.h"
#include "rtc_cunit.h"

// Internal funcfion definition
void AddTests(void);

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);
    CLK_EnableXtalRC(CLK_SRCCTL_LXTEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);

    /* Enable SYS clock */
    CLK_SetBusClock(CLK_SCLKSEL_SCLKSEL_APLL0, FREQ_180MHZ);

    /* Enable UART module clock */
    SetDebugUartCLK();


    CLK_EnableModuleClock(RTC0_MODULE);

    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HXT, CLK_UARTDIV0_UART0DIV(1));
    CLK_SetModuleClock(RTC0_MODULE, 0, 0);

    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PD multi-function pins for UART0 RXD(PB.12) and TXD(PB.13) */
    SetDebugUartMFP();

    SET_TAMPER0_PF6();
    SET_TAMPER1_PF7();
    SET_TAMPER2_PF8();
    SET_TAMPER3_PF9();
    SET_TAMPER4_PF10();
    SET_TAMPER5_PF11();
}
/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init DeubgUART for printf */
    InitDebugUart();

    if (CU_initialize_registry())
    {
        fprintf(stderr, " Initialization of Test Registry failed. ");
        exit(EXIT_FAILURE);
    }
    else
    {
        AddTests();
        CU_console_run_tests();
        CU_cleanup_registry();
    }

    while (SYS->PDID);
}

void exit(int32_t code)
{
    if (code)
        while (1); // Fail
    else
        while (1); // Success
}

void AddTests(void)
{
    assert(NULL != CU_get_registry());
    assert(!CU_is_test_running());

    if (CUE_SUCCESS != CU_register_suites(suites))
    {
        fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }
}

void __aeabi_assert(const char *s, const char *file, int line)
{
    printf("%s  %s  %d\n", s, file, line);

    while (1);

}