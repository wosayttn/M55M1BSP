/******************************************************************************
* @file    main.c
* @version V1.00
* @brief   KPI CUnit
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
#include "kpi_cunit.h"

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
    /* Enable UART0 module clock */
    SetDebugUartCLK();
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();
    //
    CLK->KPICTL |= CLK_KPICTL_KPI0CKEN_Msk;
    CLK->KPISEL = (CLK->KPISEL & (~CLK_KPISEL_KPI0SEL_Msk)) | CLK_KPISEL_KPI0SEL_HIRC;
    SYS->KPIRST |= SYS_KPIRST_KPI0RST_Msk;
    SYS->KPIRST ^= SYS_KPIRST_KPI0RST_Msk;
    SET_KPI_ROW0_PC5();
    SET_KPI_ROW1_PC4();
    SET_KPI_ROW2_PC3();
    SET_KPI_ROW3_PC2();
    SET_KPI_ROW4_PC1();
    SET_KPI_ROW5_PC0();
    SET_KPI_COL0_PA6();
    SET_KPI_COL1_PA7();
    SET_KPI_COL2_PC6();
    SET_KPI_COL3_PC7();
    SET_KPI_COL4_PC8();
    SET_KPI_COL5_PB6();
    SET_KPI_COL6_PB5();
    SET_KPI_COL7_PB4();
    //
    SYS_ResetModule(SYS_KPI0RST);
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
    printf("|       M55M1 KPI CUnit Test           |\n");
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
