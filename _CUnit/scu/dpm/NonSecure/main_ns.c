/**************************************************************************//**
 * @file     main_ns.c
 * @version  V1.00
 * @brief    Non-secure sample code for TrustZone
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "nsclib.h"
#include "dpm_cunit.h"

/*--------------------------------------------------------------------------*
 * Non-Secure SysTick IRQ Handler
 *--------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
    static uint32_t u32Ticks;

    if ((u32Ticks++ % 1000) == 0)
    {
        printf("Non-Secure SysTick\n");
    }
}

void DEBUG_PORT_Init(void)
{
    /* Init UART to 115200-8n1 for print message */
    DEBUG_PORT->LINE = (UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1);
    DEBUG_PORT->BAUD = (UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HIRC, 115200));
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

    if (CUE_SUCCESS != CU_register_suites(DPM_Suites))
    {
        fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }
}

void CUnitTest(void)
{
    printf("\n\n");
    printf("+--------------------------------------+\n");
    printf("|    M55M1 DPM Non-Secure CUnit Test   |\n");
    printf("+--------------------------------------+\n");

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
}

/*--------------------------------------------------------------------------*
 *  Main function
 *--------------------------------------------------------------------------*/
int main(void)
{
    DEBUG_PORT_Init();

    printf("+---------------------------------------------+\n");
    printf("|           Execute Non-Secure code           |\n");
    printf("+---------------------------------------------+\n");
    SecureFunc();

    /* Generate Systick interrupt each 10 ms */
    Secure_GetSystemCoreClock();
    SysTick_Config(SystemCoreClock / 1000);

    CUnitTest();

    while(1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
