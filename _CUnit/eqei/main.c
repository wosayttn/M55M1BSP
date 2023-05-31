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
#include "eqei_cunit.h"
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

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
    
    /* Enable PLL0 200MHz clock */
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
    CLK_EnableModuleClock(EQEI0_MODULE);
    CLK_EnableModuleClock(EQEI1_MODULE);
    CLK_EnableModuleClock(EQEI2_MODULE);
    CLK_EnableModuleClock(EQEI3_MODULE);
    
    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOD_MODULE);
    CLK_EnableModuleClock(GPIOE_MODULE);
    CLK_EnableModuleClock(GPIOF_MODULE);
    
    SYS_ResetModule(SYS_UART0RST);
    SYS_ResetModule(SYS_EQEI0RST);
    SYS_ResetModule(SYS_EQEI1RST);
    SYS_ResetModule(SYS_EQEI2RST);
    SYS_ResetModule(SYS_EQEI3RST);
    
    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HIRC, CLK_UARTDIV0_UART0DIV(1));

    
    SystemCoreClockUpdate();
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PD multi-function pins for UART0 RXD(PB.12) and TXD(PB.13) */
    SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13(); 
}

void exit(int32_t code)
{
    if (code)
        while (1)
            ; // Fail
    else
        while (1)
            ; // Success
}

void Enable_SelfTest(void)
{
    SYS_UnlockReg();
    /* Enable global self test mode */
    //outp32(0x40000014, inp32(0x40000014) | 1);
    SYS->ALTCTL0 |= BIT0;

    GPIO_SetMode(PA, BIT4, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT3, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT5, GPIO_MODE_OUTPUT);
    PA4 = PA3 = PA5 = 0;

    GPIO_SetMode(PA, BIT9, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT8, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT10, GPIO_MODE_OUTPUT);
    PA9 = PA8 = PA10 = 0;

    GPIO_SetMode(PD, BIT0, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PD, BIT13,GPIO_MODE_OUTPUT);
    GPIO_SetMode(PD, BIT1, GPIO_MODE_OUTPUT);
    PD0 = PD13 = PD1 = 0;

    GPIO_SetMode(PA, BIT1, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT2, GPIO_MODE_OUTPUT);
    PA1 = PA0 = PA2 = 0;
}

void Disable_SelfTest(void)
{
    SYS_UnlockReg();
    /* Enable global self test mode */
    //outp32(0x40000014, inp32(0x40000014) & ~1);
    SYS->ALTCTL0 &= ~(BIT0);
}

int main(int argc, char *argv[])
{

    /* Unlock protected registers for ISP function */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init DEBUG_PORT to 115200-8N1 for printf */
    DEBUG_PORT_Init(DEBUG_PORT, 115200);

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);

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

    while (SYS->PDID)
        ;
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
