/****************************************************************************
*                                                                           *
* Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.          *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   main.c
*
* VERSION
*   1.0
*
* DESCRIPTION
*   The main program of CUnit test
*
* DATA STRUCTURES
*   None
*
* FUNCTIONS
*   AddTests
*   main
*
* HISTORY
*
*
* REMARK
*   None
****************************************************************************/

// Library header file
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "CUnit.h"
#include "Console.h"
#include "NuMicro.h"
#include "rtc_cunit.h"
#include "../pldm_emu.h" 

#ifndef DEBUG_PORT
    #define DEBUG_PORT UART0
#endif

#ifndef DEBUG_PORT_Init
void DEBUG_PORT_Init(UART_T* psUART, uint32_t u32Baudrate)
{
    UART_Open(psUART, u32Baudrate);
}
#endif

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

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);
    SYS_ResetModule(SYS_UART0RST);
    CLK_EnableModuleClock(RTC0_MODULE);

    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HIRC, CLK_UARTDIV0_UART0DIV(1));
    CLK_SetModuleClock(RTC0_MODULE, 0, 0);
    
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PD multi-function pins for UART0 RXD(PB.12) and TXD(PB.13) */
    SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13(); 
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

    /* Init DEBUG_PORT to 115200-8N1 for printf */
    DEBUG_PORT_Init(DEBUG_PORT, 115200);

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