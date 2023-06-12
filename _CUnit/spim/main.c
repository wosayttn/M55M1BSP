/****************************************************************************
*                                                                           *
* Copyright (c) 2016 Nuvoton Technology Corp. All rights reserved.          *
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
*   The main program of M480 Crypto CUNIT test
*
* DATA STRUCTURES
*   None
*
* FUNCTIONS
*   AddTests
*   main
*
* HISTORY
*   2016-08-23    Ver 1.0 Created by MS10 YCHuang
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
#include "spim_cunit.h"
#include "hyper_cunit.h"
#include "otfc_cunit.h"
#include "octal_cunit.h"
#include "common.h"

//------------------------------------------------------------------------------
//#define HIGH_SPEED_BOARD

extern void *GetSPIMModule(uint32_t u32Idx);
extern void SetSPIMTestModuleIdx(uint32_t u32SetValue);

//------------------------------------------------------------------------------
// Internal funcfion definition
void AddTests(void);

//------------------------------------------------------------------------------
static uint32_t gu32SelTestSuite = 0;

//------------------------------------------------------------------------------
int SPIM_SelTestSuite(void)
{
    uint32_t u32Index = 0;
    S_TestOption sSelTestSuite[] =
    {
        {"SPIM Mode"},
        {"SPIM Octal Mode"},
        {"SPIM Hyper Mode"},
        {"OTFC Mode"},
    };

    printf("\r\n\r\n");
    gu32SelTestSuite = GetRequireOptions(sSelTestSuite, sizeof(sSelTestSuite) / sizeof(sSelTestSuite[0]));

    return (int)u32Index;
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

#ifdef __PLDM_EMU__
    /* r5464, Trim Filter Behavior */
    SYS->HIRC48MCFCTL = 0x00009999;
#endif

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
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_HIRC);

    /* Enable PLL0 200MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_200MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to PLL0 and divide 1 */
    /* Switch HCLK clock source to HXT */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0);
#endif //USE_HXT_SRC

    CLK_SET_HCLK0DIV(1);
    CLK_SET_HCLK1DIV(1);
    CLK_SET_HCLK2DIV(2);

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    CLK_PCLKDIV_PCLK0DIV(2);
    CLK_PCLKDIV_PCLK1DIV(2);
    CLK_PCLKDIV_PCLK2DIV(2);
    CLK_PCLKDIV_PCLK3DIV(2);
    CLK_PCLKDIV_PCLK4DIV(2);

    /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Enable CRYPTO and TRNG and Key Store module clock */
    CLK_EnableModuleClock(CRYPTO0_MODULE);
    CLK_EnableModuleClock(TRNG0_MODULE);
    CLK_EnableModuleClock(KS0_MODULE);
    CLK_EnableModuleClock(SRAM0_MODULE);
    CLK_EnableModuleClock(SRAM1_MODULE);
    CLK_EnableModuleClock(SRAM2_MODULE);
    CLK_EnableModuleClock(LPSRAM0_MODULE);
    CLK_EnableModuleClock(CANFD0_MODULE);
    CLK_EnableModuleClock(CANFD1_MODULE);

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

    /* Lock protected registers */
    //SYS_LockReg();
}

void DebugPort_Init(void)
{
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HIRC, CLK_UARTDIV0_UART0DIV(1));

    /*------------------------------------------------------------------------*/
    /* Init UART                                                              */
    /*------------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP3 &= ~(SYS_GPB_MFP3_PB12MFP_Msk | SYS_GPB_MFP3_PB13MFP_Msk);
    SYS->GPB_MFP3 |= (SYS_GPB_MFP3_PB12MFP_UART0_RXD | SYS_GPB_MFP3_PB13MFP_UART0_TXD);

    //SYS->GPC_MFP2 = (SYS->GPC_MFP2 & ~SYS_GPC_MFP2_PC11MFP_Msk) | SYS_GPC_MFP2_PC11MFP_UART0_RXD;
    //SYS->GPC_MFP3 = (SYS->GPC_MFP3 & ~SYS_GPC_MFP3_PC12MFP_Msk) | SYS_GPC_MFP3_PC12MFP_UART0_TXD;

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

int main()
{
    uint8_t idBuf[3];
    uint32_t u32SPIMPort = 0;
    uint32_t u32SPIMDiv = 0;
    SPIM_T *pSPIMModule = NULL;

#ifdef __PLDM_EMU__
    SCB_DisableICache();
    SCB_DisableDCache();

    //SCB_EnableICache();
    //SCB_EnableDCache();
#endif //__PLDM_EMU__

    /* Init System, IP clock and multi-function I/O */
    SYS_Init(); //In the end of SYS_Init() will issue SYS_LockReg() to lock protected register. If user want to write protected register, please issue SYS_UnlockReg() to unlock protected register.

    /* Init UART0 for printf */
    DebugPort_Init();

    //SPIM0 PSC, PSC_n
    SET_GPIO_PG8();
    GPIO_SetMode(PG, BIT8, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PG, BIT8, GPIO_PUSEL_DISABLE);
    PG8 = 1;

    SET_GPIO_PG7();
    GPIO_SetMode(PG, BIT7, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PG, BIT7, GPIO_PUSEL_DISABLE);
    PG7 = 0;

    //SPIM1 PSC, PSC_n
    SET_GPIO_PH11();
    GPIO_SetMode(PH, BIT11, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PH, BIT11, GPIO_PUSEL_DISABLE);
    PH11 = 1;

    SET_GPIO_PH10();
    GPIO_SetMode(PH, BIT10, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PH, BIT11, GPIO_PUSEL_DISABLE);
    PH10 = 0;

    SET_GPIO_PB7();
    GPIO_SetMode(PB, BIT7, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PB, BIT7, GPIO_PUSEL_DISABLE);
    PH10 = 0;

    SPIM_NVIC_Disable(C_SPIM0);
    SPIM_NVIC_Disable(C_SPIM1);

    u32SPIMPort = SPIMPortSelect(0);
    SetSPIMTestModuleIdx(u32SPIMPort);
    pSPIMModule = (SPIM_T *)GetSPIMModule(u32SPIMPort);

    //SPIM Def. Enable Cipher, First Disable the test.
    SPIM_DISABLE_CIPHER(pSPIMModule);

    u32SPIMDiv = GetSPIMClkDivNum();

    SPIM_SET_CLOCK_DIVIDER(pSPIMModule, u32SPIMDiv); /* Set SPIM clock as HCLK divided by 2 */
    printf("SPIM Clock Divider = %ld\r\n", SPIM_GET_CLOCK_DIVIDER(pSPIMModule));
    SPIM_SET_RXCLKDLY_RDDLYSEL(pSPIMModule, 0);      /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */

    SPIM_SelTestSuite();

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

    switch (gu32SelTestSuite)
    {
        case 0:
            if (CUE_SUCCESS != CU_register_suites(suites))
            {
                fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
                exit(EXIT_FAILURE);
            }

            break;

        case 1:
            if (CUE_SUCCESS != CU_register_suites(spim_octal_suites))
            {
                fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
                exit(EXIT_FAILURE);
            }

            break;

        case 2:
            if (CUE_SUCCESS != CU_register_suites(spim_hyper_suites))
            {
                fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
                exit(EXIT_FAILURE);
            }

            break;

        case 3:
            if (CUE_SUCCESS != CU_register_suites(otfc_suites))
            {
                fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
                exit(EXIT_FAILURE);
            }

            break;
    }
}
