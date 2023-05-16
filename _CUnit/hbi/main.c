/****************************************************************************
*                                                                           *
* Copyright (c) 2004 - 2007 Winbond Electronics Corp. All rights reserved.  *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   run_test.c
*
* VERSION
*   1.0
*
* DESCRIPTION
*   The  main program of CUnit test
*
* DATA STRUCTURES
*   None
*
* FUNCTIONS
*   AddTests
*   main
*
* HISTORY
*   2007-06-28    Ver 1.0 Modified by NS21 WTLiu
*
* REMARK
*   None
****************************************************************************/

/***************************************************************************
  HISTORY
  2007-06-28    V1.0
  1. Preliminary Version

***************************************************************************/

// Library header file
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "CUnit.h"
#include "Console.h"
#include "NuMicro.h"
#include "hbi_cunit.h"

#define PLLCTL_SETTING  CLK_PLLCTL_72MHz_HXT
#define PLL_CLOCK       72000000


// Internal funcfion definition
void AddTests(void);




void SYS_Init(void)
{

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Update System Core Clock */
    SystemCoreClockUpdate();    
   
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as HIRC and UART module clock divider as 1 */
    //CLK_SetModuleClock(
    
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set PB multi-function pins for UART0 RXD and TXD */
    SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13();
    
}


//void abort(void)
//{
//    return;
//}

void exit(int32_t code)
{
    if(code)
        while(1); // Fail
    else
        while(1); // Success
}

int main(int argc, char *argv[])
{

    /* Unlock protected registers for ISP function */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for printf */
    UART_Open(UART0, 115200);

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);

    if(CU_initialize_registry())
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

    while(SYS->PDID);
}

void AddTests(void)
{
    assert(NULL != CU_get_registry());
    assert(!CU_is_test_running());

    if(CUE_SUCCESS != CU_register_suites(suites))
    {
        fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }
}
