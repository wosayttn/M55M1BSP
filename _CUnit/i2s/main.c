/****************************************************************************
*                                                                           *
* Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.          *
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

#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "i2s_cunit.h"

//------------------------------------------------------------------------------
//#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT
//#define PLL_CLOCK           50000000
#define USE_HXT_SRC             (0)

//------------------------------------------------------------------------------
// Internal funcfion definition
void AddTests(void);

//------------------------------------------------------------------------------
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
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_192MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to PLL0 and divide 1 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0, CLK_ACLKDIV_ACLKDIV(1));

    /* Set HCLK2 divide 1 */
    CLK_SET_HCLK2DIV(1);

    /* Set PCLKx divide 1 */
    CLK_SET_PCLK0DIV(1);
    CLK_SET_PCLK1DIV(1);
    CLK_SET_PCLK2DIV(1);
    CLK_SET_PCLK3DIV(1);
    CLK_SET_PCLK4DIV(1);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable IP clock */
    CLK_EnableModuleClock(I2S0_MODULE);
    CLK_EnableModuleClock(I2S1_MODULE);

    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOD_MODULE);
    CLK_EnableModuleClock(GPIOE_MODULE);
    CLK_EnableModuleClock(GPIOF_MODULE);
    CLK_EnableModuleClock(GPIOG_MODULE);
    CLK_EnableModuleClock(GPIOH_MODULE);
    CLK_EnableModuleClock(GPIOI_MODULE);
    CLK_EnableModuleClock(GPIOJ_MODULE);
}

void DebugUART_Init(void)
{
    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /* Select UART clock source from HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HIRC, CLK_UARTDIV0_UART0DIV(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

#if (USE_HXT_SRC == 1)
    DEBUG_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(153600, 9600); // The setting is for Palladium
#else
    DEBUG_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(153600, 38400); // The setting is for Palladium
#endif //USE_HXT_SRC

    /* Lock protected registers */
    //SYS_LockReg();
#if 0
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HXT, CLK_UARTDIV0_UART0DIV(1));

    /*------------------------------------------------------------------------*/
    /* Init UART                                                              */
    /*------------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP3 &= ~(SYS_GPB_MFP3_PB12MFP_Msk | SYS_GPB_MFP3_PB13MFP_Msk);
    SYS->GPB_MFP3 |= (SYS_GPB_MFP3_PB12MFP_UART0_RXD | SYS_GPB_MFP3_PB13MFP_UART0_TXD);
    //SYS->GPD_MFP0 &= ~(SYS_GPD_MFP0_PD2MFP_Msk | SYS_GPD_MFP0_PD3MFP_Msk);
    //SYS->GPD_MFP0 |= (SYS_GPD_MFP0_PD2MFP_UART0_RXD | SYS_GPD_MFP0_PD3MFP_UART0_TXD);
    //SYS->GPH_MFP2 = (SYS->GPH_MFP2 & (~SYS_GPH_MFP2_PH10MFP_Msk)) | (SYS_GPH_MFP2_PH10MFP_UART0_TXD);
    //SYS->GPH_MFP2 = (SYS->GPH_MFP2 & (~SYS_GPH_MFP2_PH11MFP_Msk)) | (SYS_GPH_MFP2_PH11MFP_UART0_RXD);

    /* Reset IP */
    //SYS->UARTRST |=  SYS_UARTRST_UART0RST_Msk;
    //SYS->UARTRST &= ~SYS_UARTRST_UART0RST_Msk;

#ifdef __PLDM_EMU__
#if (USE_HXT_SRC == 1)
    DEBUG_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(153600, 9600); // The setting is for Palladium
#else
    DEBUG_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(153600, 38400); // The setting is for Palladium
#endif //USE_HXT_SRC
#else
    /* Configure UART0 and set UART0 Baudrate */
    //UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(PLL_CLOCK, 115200);
    UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(12000000, 115200);
#endif //#ifndef __PLDM_EMU__
    UART0->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
#endif //0
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
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    DebugUART_Init();

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
    //    assert(NULL != CU_get_registry());
    //    assert(!CU_is_test_running());

    if (CUE_SUCCESS != CU_register_suites(suites))
    {
        fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }
}
