/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Demonstrate LPTMR Auto-operation with LPPDMA when chip enters power-down mode.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
/*
 * This sample uses internal RC as APLL0 clock source and UART0 to print messages.
 * Users may need to do extra system configuration according to their system design.
 *
 * I/D-Cache
 *   I/D-Cache are enabled by default for better performance,
 *   users can define NVT_ICACHE_OFF/NVT_DCACHE_OFF in project setting to disable cache.
 * Debug UART
 *   Default is DEBUG_PORT=UART0 in project setting
 *   system_M55M1.c has three weak functions as below to configure debug UART port.
 *     SetDebugUartMFP, SetDebugUartCLK and InitDebugUart
 *   Users can re-implement these functions according to system design.
 */
#include <stdio.h>
#include <stdlib.h>
#include "NuMicro.h"

#define LPTMR_LPPDMA_CH      1
#define DATA_COUNT          10

#if (defined(__GNUC__) && !defined(__ARMCC_VERSION))
    static uint32_t s_au32CAPValue[DATA_COUNT] __attribute__((section(".lpSram")));
#else
    static uint32_t s_au32CAPValue[DATA_COUNT] __attribute__((section(".ARM.__at_0x20310100")));
#endif

static volatile uint32_t s_u32IsTestOver = 0;

NVT_ITCM void LPPDMA_IRQHandler(void)
{
    CLK_WaitModuleClockReady(LPPDMA0_MODULE);//TESTCHIP_ONLY
    CLK_WaitModuleClockReady(UART0_MODULE);//TESTCHIP_ONLY
    uint32_t status = LPPDMA_GET_INT_STATUS(LPPDMA);

    if (status & LPPDMA_INTSTS_ABTIF_Msk)   /* abort */
    {
        /* Check if channel 1 has abort error */
        if (LPPDMA_GET_ABORT_STS(LPPDMA) & (LPPDMA_ABTSTS_ABTIF0_Msk<<LPTMR_LPPDMA_CH))
            s_u32IsTestOver = 2;

        /* Clear abort flag of channel 1 */
        LPPDMA_CLR_ABORT_FLAG(LPPDMA, (LPPDMA_ABTSTS_ABTIF0_Msk<<LPTMR_LPPDMA_CH));
    }
    else if (status & LPPDMA_INTSTS_TDIF_Msk)     /* done */
    {
        /* Check transmission of channel 1 has been transfer done */
        if (LPPDMA_GET_TD_STS(LPPDMA) & (LPPDMA_TDSTS_TDIF0_Msk<<LPTMR_LPPDMA_CH))
            s_u32IsTestOver = 1;

        /* Clear transfer done flag of channel 1 */
        LPPDMA_CLR_TD_FLAG(LPPDMA, (LPPDMA_TDSTS_TDIF0_Msk<<LPTMR_LPPDMA_CH));
    }
    else
        printf("unknown interrupt !!\n");
    if (status & LPPDMA_INTSTS_WKF_Msk)     /* wake up */
    {
        printf("wake up !!\n");

        /* Clear wake up flag */
        LPPDMA->INTSTS = status;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  LPTMR Function                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void LPTMR_Trigger_Init(void)
{
    /* Open LPTMR0 to periodic mode and timeout 1000 times per second */
    LPTMR_Open(LPTMR0, LPTMR_PERIODIC_MODE, 1000);
    LPTMR_SetTriggerSource(LPTMR0, LPTMR_TRGSRC_TIMEOUT_EVENT);
    /* Enable LPTMR Power-down engine clock */
    LPTMR_EnablePDCLK(LPTMR0);
    /* Enable LPTMR0 to trigger LPPDMA */
    LPTMR_SetTriggerTarget(LPTMR0, LPTMR_TRG_TO_LPPDMA);
}

void LPPDMA_Init(void)
{
    /* Reset PDMA module */
    SYS_ResetModule(SYS_LPPDMA0RST);

    /* Enable PDMA channels */
    LPPDMA_Open(LPPDMA, 1<<LPTMR_LPPDMA_CH);

    /*=======================================================================
      LPPDMA channel configuration:
      -----------------------------------------------------------------------
        Word length = 32 bits
        Transfer Count = DATA_COUNT
        Source = &LPTMR0->CNT
        Source Address = Fixed
        Destination = s_au32CAPValue
        Destination Address = Incresing
        Burst Type = Single Transfer
    =========================================================================*/
    /* Set transfer width (32 bits) and transfer count */
    LPPDMA_SetTransferCnt(LPPDMA, LPTMR_LPPDMA_CH, LPPDMA_WIDTH_32, DATA_COUNT);
    /* Set source/destination address and attributes */
    LPPDMA_SetTransferAddr(LPPDMA, LPTMR_LPPDMA_CH, (uint32_t)&LPTMR0->CNT, LPPDMA_SAR_FIX, (uint32_t)s_au32CAPValue, LPPDMA_DAR_INC);
    /* Set request source; set basic mode. */
    LPPDMA_SetTransferMode(LPPDMA, LPTMR_LPPDMA_CH, LPPDMA_LPTMR0, FALSE, 0);
    /* Single request type. SPI only support PDMA single request type. */
    LPPDMA_SetBurstType(LPPDMA, LPTMR_LPPDMA_CH, LPPDMA_REQ_SINGLE, 0);
    /* Enable Channel Transfer done interrupt */
    LPPDMA_EnableInt(LPPDMA, LPTMR_LPPDMA_CH, LPPDMA_INT_TRANS_DONE);
    NVIC_EnableIRQ(LPPDMA_IRQn);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Function for System Entry to Power Down Mode                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void PowerDownFunction(void)
{
    SYS_UnlockReg();
    /* Switch SCLK clock source to HIRC */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_HIRC);
    /* Check if all the debug messages are finished */
    UART_WAIT_TX_EMPTY(DEBUG_PORT);
    /* Set Power-down mode */
    PMC_SetPowerDownMode(PMC_NPD0, PMC_PLCTL_PLSEL_PL1);
    /* Enter to Power-down mode */
    PMC_PowerDown();
    /* Switch SCLK clock source to PLL0 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0);
    SYS_LockReg();
}

static void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Enable PLL0 180MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_180MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to PLL0 */
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

    /*---------------------------------------------------------------------------------------------------------*/
    /* Initialization for sample code                                                                          */
    /*---------------------------------------------------------------------------------------------------------*/
    /* enable LPSRAM clock */
    CLK_EnableModuleClock(LPSRAM0_MODULE);
    /* Enable LPPDMA Clock */
    CLK_EnableModuleClock(LPPDMA0_MODULE);
    /* Select LPTMR0 module clock source from HIRC */
    CLK_SetModuleClock(LPTMR0_MODULE, CLK_LPTMRSEL_LPTMR0SEL_HIRC, MODULE_NoMsk);
    /* Enable LPTMR 0 module clock */
    CLK_EnableModuleClock(LPTMR0_MODULE);
    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    initialise_monitor_handles();
#endif

    printf("System core clock = %d\n", SystemCoreClock);
    printf("+-------------------------------------------------------------+\n");
    printf("|         LPTMR Auto Operation Mode Sample Code                |\n");
    printf("+-------------------------------------------------------------+\n");
    printf("\n");
    printf(" 1. Initialize LPTMR0 and LPPDMA. \n");
    printf(" 2. Let system enter Powerr Mode. \n");
    printf(" 3. LPTMR0 will trigger LPPDMA to do CNT data transfer. After finishing data transfer, \n");
    printf("    system will be woken up and the received data can be checked. \n");

    LPTMR_Trigger_Init();
    LPPDMA_Init();
    s_u32IsTestOver = 0;
    memset(s_au32CAPValue, 0, sizeof(s_au32CAPValue));
    LPTMR_Start(LPTMR0);
    PowerDownFunction();
    NVIC_DisableIRQ(LPPDMA_IRQn);
    LPPDMA_DisableInt(LPPDMA, LPTMR_LPPDMA_CH, LPPDMA_INT_TRANS_DONE);
    LPTMR_Stop(LPTMR0);
    /* Waiting for LPPDMA transfer done.  g_u32IsTestOver is set by LPPDMA interrupt handler */
    while (s_u32IsTestOver == 0);
    /* Check transfer result */
    if (s_u32IsTestOver == 1)
        printf("LPPDMA trasnfer LPTMR done.\n");
    else if (s_u32IsTestOver == 2)
        printf("LPPDMA trasnfer LPTMR abort...\n");
    if(s_au32CAPValue[9]==LPTMR0->CMP)
        printf("LPTMR Auto Operation PASS.\n");
    else
        printf("LPTMR Auto Operation FAIL.\n");
    /* Got no where to go, just loop forever */
    while (1) ;
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/