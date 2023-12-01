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
#include "otg_cunit.h"
//#include "../pldm_emu.h"

#ifndef DEBUG_PORT
    #define DEBUG_PORT UART0
#endif

#ifndef DEBUG_PORT_Init
void DEBUG_PORT_Init(UART_T *psUART, uint32_t u32Baudrate)
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
    CLK_EnableXtalRC(CLK_SRCCTL_HIRC48MEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRC48MSTB_Msk);

    /* Switch SCLK clock source to PLL0 and Enable PLL0 180MHz clock */
    CLK_SetBusClock(CLK_SCLKSEL_SCLKSEL_APLL0, FREQ_180MHZ);
    
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);
    SYS_ResetModule(SYS_UART0RST);


    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HIRC, CLK_UARTDIV0_UART0DIV(1));
    /* Select USBD module clock source as HIRC48M and USBD module clock divider as 1 */
    CLK_SetModuleClock(USBD0_MODULE, CLK_USBSEL_USBSEL_HIRC48M, CLK_USBDIV_USBDIV(1));

    /* Enable USBD0 module clock */
    CLK_EnableModuleClock(USBD0_MODULE);
    //SYS_ResetModule(SYS_USBD0RST);

    /* Enable OTG mode clock */
    CLK_EnableModuleClock(OTG0_MODULE);
    SYS_ResetModule(SYS_OTG0RST);


    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PD multi-function pins for UART0 RXD(PB.12) and TXD(PB.13) */
    SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13();
    
    SET_USB_VBUS_PA12();

}

void exit(int32_t code)
{
    if (code)
        while (1); // Fail
    else
        while (1); // Success
}

int main(int argc, char *argv[])
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

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

void AddTests(void)
{
    CU_get_registry();
    CU_is_test_running();

    if (CUE_SUCCESS != CU_register_suites(suites))
    {
        fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }
}
