/******************************************************************************
* @file    main.c
* @version V1.00
* @brief   CUnit test main function
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
#include "npu_cunit.h"
#include "../pldm_emu.h"

#ifndef DEBUG_PORT
    #define DEBUG_PORT UART0
#endif

#ifndef DEBUG_PORT_Init
void DEBUG_PORT_Init(UART_T *psUART, uint32_t u32Baudrate)
{
    UART_Open(psUART, u32Baudrate);
}
#endif

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

    /* Switch SCLK clock source to APLL0 and Enable APLL0 180MHz clock */
    CLK_SetBusClock(CLK_SCLKSEL_SCLKSEL_APLL0, CLK_APLLCTL_APLLSRC_HXT, FREQ_180MHZ);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable UART clock */
    CLK_EnableModuleClock(UART0_MODULE);
    SYS_ResetModule(SYS_UART0RST);

    /* Enable FMC0 module */
    CLK_EnableModuleClock(FMC0_MODULE);

    /* Enable NPU module clock */
    CLK_EnableModuleClock(NPU0_MODULE);

    /* Select UART6 module clock source as HIRC and UART6 module clock divider as 1 */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set multi-function pins for UART RXD and TXD */
    SetDebugUartMFP();

    /* Lock protected registers */
    SYS_LockReg();
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

    if (CUE_SUCCESS != CU_register_suites(NPU_Suites))
    {
        fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }
}

extern void TestFunc_NPU_Invoke();
extern void TestFunc_NPU_Open_Close();

int main(int argc, char *argv[])
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* UART init - will enable valid use of printf (stdout
     * re-directed at this UART (UART6) */
    InitDebugUart();

    printf("\n\n");
    printf("+--------------------------------------+\n");
    printf("|         M55M1 NPU CUnit Test         |\n");
    printf("+--------------------------------------+\n");

    if (CU_initialize_registry())
    {
        fprintf(stderr, " Initialization of Test Registry failed. ");
        exit(EXIT_FAILURE);
    }
    else
    {

        char *resultStr;
        AddTests();
        CU_run_all_tests();
        resultStr = CU_get_run_results_string();
        printf("%s \n", resultStr);
        CU_cleanup_registry();
    }

    while (1) ;
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/