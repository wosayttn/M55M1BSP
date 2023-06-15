/****************************************************************************
*                                                                           *
* Copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.          *
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
#include "usbd_cunit.h"
#include "../pldm_emu.h"
 
// Internal funcfion definition
void AddTests(void);


void SYS_Init(void)
{
     /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/


    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Enable External RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);

    /* Waiting for External RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Enable PLL0 200MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_200MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to PLL0 and divide 1 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0);

    /* Set HCLK2 divide 2 */
    CLK_SET_HCLK2DIV(2);
    
    /* Set PCLKx divide 2 */
    CLK_SET_PCLK0DIV(2);
    CLK_SET_PCLK1DIV(2);
    CLK_SET_PCLK2DIV(2);
    CLK_SET_PCLK3DIV(2);
    CLK_SET_PCLK4DIV(4);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Set the Debug UART port clock */
    SetDebugUartCLK();

    /* Enable GPA module clock */
    CLK_EnableModuleClock(GPIOA_MODULE);
    /* Enable GPB module clock */
    CLK_EnableModuleClock(GPIOB_MODULE);
    /* Enable GPA module clock */
    CLK_EnableModuleClock(GPIOD_MODULE);
    /* Enable GPB module clock */
    CLK_EnableModuleClock(GPIOE_MODULE);
     /* Enable GPA module clock */
    CLK_EnableModuleClock(GPIOG_MODULE);
    /* Enable GPB module clock */
    CLK_EnableModuleClock(GPIOH_MODULE);


   /* Update System Core Clock */

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();
    /* Enable USBD module clock */
    CLK_EnableModuleClock(USBD0_MODULE);

    CLK_SetModuleClock(USBD0_MODULE, CLK_USBSEL_USBSEL_HIRC48M, CLK_USBDIV_USBDIV(1));
    /* Enable IP clock */
    CLK_EnableModuleClock(USBD0_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

}




void exit(int32_t code)
{
    if(code)
        while(1); // Fail
    else
        while(1); // Success
}

int main(int argc, char *argv[])
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
//    SYS_LockReg();

//    UART_Open(DEBUG_PORT, 115200);
    DEBUG_PORT_Init(DEBUG_PORT, 115200);

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
    CU_get_registry();
    CU_is_test_running();

    if(CUE_SUCCESS != CU_register_suites(suites))
    {
        fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }
}
