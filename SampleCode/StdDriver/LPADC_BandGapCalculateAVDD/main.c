/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate how to calculate battery voltage( AVdd ) by using band-gap.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t g_u32LpadcIntFlag;
volatile uint32_t g_u32BandGapConvValue;

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    extern void initialise_monitor_handles(void);
#endif

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

   /* Enable APLL0 180MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_180MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to APLL0 */
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

   
    /* LPADC clock source is HIRC = 12MHz, set divider to 1, LPADC clock is 12 MHz */
    CLK_SetModuleClock(LPADC0_MODULE, CLK_LPADCSEL_LPADC0SEL_HIRC, CLK_LPADCDIV_LPADC0DIV(1));

    /* Enable LPADC module clock */
    CLK_EnableModuleClock(LPADC0_MODULE);

    /* Debug UART clock setting*/
    SetDebugUartCLK();

    /*----------------------------------------------------------------------*/
    /* Init I/O Multi-function                                              */
    /*----------------------------------------------------------------------*/

    /* Set PB multi-function pins for Debug UART RXD and TXD */
    SetDebugUartMFP();

}

void LPADC_FunctionTest()
{
    int32_t  i32ConversionData;
    int32_t  i32BuiltInData=0;

    printf("\n");
    printf("+----------------------------------------------------------------------+\n");
    printf("|   LPADC for calculate battery voltage( AVdd ) by using band-gap test |\n");
    printf("+----------------------------------------------------------------------+\n");

    printf("+----------------------------------------------------------------------+\n");
    printf("|   LPADC clock source -> PCLK2  = 12 MHz                              |\n");
    printf("|   LPADC clock divider          = 1                                   |\n");
    printf("|   LPADC clock                  = 12 MHz / 1 = 12 MHz                 |\n");
    printf("|   LPADC extended sampling time = 20                                  |\n");
    printf("|   LPADC conversion time = 20 + LPADC extended sampling time = 40     |\n");
    printf("|   LPADC conversion rate = 12 MHz / 40 = 300 kSPS                     |\n");
    printf("+----------------------------------------------------------------------+\n");


    /* Enable LPADC converter */
    LPADC_POWER_ON(LPADC0);

    /* Set input mode as single-end, Single mode, and select channel 29 (band-gap voltage) */
    LPADC_Open(LPADC0, LPADC_ADCR_DIFFEN_SINGLE_END, LPADC_ADCR_ADMD_SINGLE, BIT29);

    /* The maximum sampling rate will be 300 kSPS for Band-gap. */
    /* Set sample module extended sampling time to 20. */
    LPADC_SetExtendSampleTime(LPADC0, 0, 20);

    /* Clear the A/D interrupt flag for safe */
    LPADC_CLR_INT_FLAG(LPADC0, LPADC_ADF_INT);

    /* Enable the sample module interrupt */
    LPADC_EnableInt(LPADC0, LPADC_ADF_INT);
    NVIC_EnableIRQ(LPADC0_IRQn);

    /* Reset the LPADC interrupt indicator and trigger sample module 0 to start A/D conversion */
    g_u32LpadcIntFlag = 0;
    LPADC_START_CONV(LPADC0);

    /* Wait LPADC interrupt (g_u32LpadcIntFlag will be set at LPADC0_IRQHandler function) */
    while(g_u32LpadcIntFlag == 0);

    /* Disable the A/D interrupt */
    LPADC_DisableInt(LPADC0, LPADC_ADF_INT);

    /* Get the conversion result of the channel 29 */
    i32ConversionData = LPADC_GET_CONVERSION_DATA(LPADC0, 29);

    /* Enable RMC ISP function to read built-in band-gap A/D conversion result*/
    SYS_UnlockReg();
    FMC_Open();
//    i32BuiltInData = FMC_ReadBandGap(); wait the FMC driver add

    /* Use Conversion result of Band-gap to calculating AVdd */

    printf("      AVdd           i32BuiltInData                   \n");
    printf("   ---------- = -------------------------             \n");
    printf("      3072          i32ConversionData                 \n");
    printf("                                                      \n");
    printf("AVdd =  3072 * i32BuiltInData / i32ConversionData     \n\n");

    printf("Built-in band-gap A/D conversion result: 0x%X (%d) \n", i32BuiltInData, i32BuiltInData);
    printf("Conversion result of Band-gap:           0x%X (%d) \n\n", i32ConversionData, i32ConversionData);

    printf("AVdd = 3072 * %d / %d = %d mV \n\n", i32BuiltInData, i32ConversionData, 3072*i32BuiltInData/i32ConversionData);
}


void LPADC0_IRQHandler(void)
{
    g_u32LpadcIntFlag = 1;
    LPADC_CLR_INT_FLAG(LPADC0, LPADC_ADF_INT); /* Clear the A/D interrupt flag */
}

int32_t main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    initialise_monitor_handles();
#endif

    /* Init Debug UART for printf */
    InitDebugUart();

    printf("\nSystem clock rate: %d Hz", SystemCoreClock);

    /* LPADC function test */
    LPADC_FunctionTest();

    /* Disable LPADC IP clock */
    CLK_DisableModuleClock(LPADC0_MODULE);

    /* Lock protected registers */
    SYS_LockReg();

    /* Disable LPADC Interrupt */
    NVIC_DisableIRQ(LPADC0_IRQn);

    printf("Exit LPADC sample code\n");

    while(1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/