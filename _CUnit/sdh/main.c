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

#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "sdh_cunit.h"
#include "PinConfig.h"

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

    /* Enable HXT Clock Source */
    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Enable MIRC Clock Source */
    CLK_EnableXtalRC(CLK_SRCCTL_MIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_MIRCSTB_Msk);

    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);
    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Enable HIRC48M Clock Source */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRC48MEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRC48MSTB_Msk);

#if (USE_HXT_SRC == 1)
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_HXT, CLK_ACLKDIV_ACLKDIV(1));
#else
    /* Switch SCLK clock source to HIRC before PLL setting */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_HIRC, CLK_ACLKDIV_ACLKDIV(1));

    /* Enable PLL0 200MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_200MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to PLL0 and divide 1 */
    /* Switch HCLK clock source to HXT */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0, CLK_ACLKDIV_ACLKDIV(1));
#endif //USE_HXT_SRC

    CLK_SET_HCLK0DIV(1);
    CLK_SET_HCLK1DIV(1);
    CLK_SET_HCLK2DIV(1);

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    //CLK->PCLKDIV = CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2;
    CLK_PCLKDIV_PCLK0DIV(1);
    CLK_PCLKDIV_PCLK1DIV(1);
    CLK_PCLKDIV_PCLK2DIV(1);
    CLK_PCLKDIV_PCLK3DIV(1);
    CLK_PCLKDIV_PCLK4DIV(1);
    //CLK_PCLKDIV_PCLK5DIV(1);

    /* Update System Core Clock */
    SystemCoreClockUpdate();

    CLK_EnableModuleClock(SDH0_MODULE);
    CLK_SetModuleClock(SDH0_MODULE, CLK_SDHSEL_SDH0SEL_APLL1_DIV2, CLK_SDHDIV_SDH0DIV(10));

    CLK_EnableModuleClock(SDH1_MODULE);
    CLK_SetModuleClock(SDH1_MODULE, CLK_SDHSEL_SDH1SEL_APLL1_DIV2, CLK_SDHDIV_SDH1DIV(10));

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

    SDH0_CD_PIN_INIT();
    SDH0_CLK_PIN_INIT();
    SDH0_CMD_PIN_INIT();
    SDH0_DAT0_PIN_INIT();
    SDH0_DAT1_PIN_INIT();
    SDH0_DAT2_PIN_INIT();
    SDH0_DAT3_PIN_INIT();
#if defined __PLDM_EMU_EMMC__
    SET_GPIO_PD13();
    GPIO_SetMode(PD, BIT13, GPIO_MODE_INPUT);
    PD13 = 0;
#endif //__PLDM_EMU_EMMC__

    SDH1_CD_PIN_INIT();
    SDH1_CLK_PIN_INIT();
    SDH1_CMD_PIN_INIT();
    SDH1_DAT0_PIN_INIT();
    SDH1_DAT1_PIN_INIT();
    SDH1_DAT2_PIN_INIT();
    SDH1_DAT3_PIN_INIT();
#if defined __PLDM_EMU_EMMC__
    SET_GPIO_PA6();
    GPIO_SetMode(PA, BIT6, GPIO_MODE_INPUT);
    PA6 = 1;
#endif //__PLDM_EMU_EMMC__

}

void DebugPort_Init()
{
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select IP clock source */
    //CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HIRC, CLK_UARTDIV0_UART0DIV(1));

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
    DEBUG_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(153600, 38400); // The setting is for Palladium
#else
    /* Configure UART0 and set UART0 Baudrate */
    //UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(PLL_CLOCK, 115200);
    DEBUG_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(12000000, 115200);
#endif //#ifndef __PLDM_EMU__
    DEBUG_PORT->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;

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
    DebugPort_Init();

    NVIC_EnableIRQ(SDH0_IRQn);

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
