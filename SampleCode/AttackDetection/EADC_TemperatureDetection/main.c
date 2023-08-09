/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Show how to measure temperature by EADC.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define VREF_VOLTAGE (3.3)

/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
static volatile uint32_t g_u32AdcIntFlag;

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void);
void UART0_Init(void);
double GetTemperature(void);
uint32_t GetTemperatureCodeFromADC(void);

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    extern void initialise_monitor_handles(void);
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* EADC IRQ Handler                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
NVT_ITCM void EADC00_IRQHandler(void)
{
    g_u32AdcIntFlag = 1;
    /* Clear the A/D ADINT0 interrupt flag */
    EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk);
}

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

    /* Enable EADC peripheral clock */
    CLK_SetModuleClock(EADC0_MODULE, CLK_EADCSEL_EADC0SEL_PCLK0, CLK_EADCDIV_EADC0DIV(15));

    /* Enable EADC module clock */
    CLK_EnableModuleClock(EADC0_MODULE);

    /* Debug UART clock setting*/
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set PB multi-function pins for Debug UART RXD and TXD */
    SetDebugUartMFP();

    /* Enable temperature sensor */
    SYS->IVSCTL |= SYS_IVSCTL_VTEMPEN_Msk;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Get temperature                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
double GetTemperature(void)
{
    double dmVoffset = 708.58154;

    double dAvgTemperatureCode = 0;
    double dmVT = 0;
    double dT;

    dAvgTemperatureCode = 0;
    dmVT = 0;

    /* Get ADC code of temperature sensor */
    dAvgTemperatureCode = GetTemperatureCodeFromADC();

    /* ADC code to voltage conversion formula: */
    dmVT = (dAvgTemperatureCode) * (VREF_VOLTAGE) * 1000 / 4096;

    /* Get temperature with temperature sensor */
    /* Temperature sensor formula: Tx = (VT - Voffset)/(-1.8118) */
    dT = (dmVT - dmVoffset) / (-1.8118);

    return (dT);
}

/*---------------------------------------------------------------------------------------------------------*/
/* EADC function test                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t GetTemperatureCodeFromADC(void)
{

    /* Set input mode as single-end and enable the A/D converter */
    EADC_Open(EADC0, EADC_CTL_DIFFEN_SINGLE_END);

    /* Set sample module 25 external sampling time to 0xFF */
    EADC_SetExtendSampleTime(EADC0, 25, 0xFF);

    /* Clear the A/D ADINT0 interrupt flag for safe */
    EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk);

    /* Enable the sample module 25 interrupt.  */
    EADC_ENABLE_INT(EADC0, BIT0);//Enable sample module A/D ADINT0 interrupt.
    EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, 0, BIT25);//Enable sample module 25 interrupt.
    NVIC_EnableIRQ(EADC00_IRQn);

    /* Reset the ADC interrupt indicator and trigger sample module 25 to start A/D conversion */
    g_u32AdcIntFlag = 0;
    EADC_START_CONV(EADC0, BIT25);

    /* Wait EADC conversion done */
    while (g_u32AdcIntFlag == 0);

    /* Disable the ADINT0 interrupt */
    EADC_DISABLE_INT(EADC0, BIT0);

    /* Return the conversion result of the sample module 25 */

    return EADC_GET_CONV_DATA(EADC0, 25);;
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    double dTemperature = 0.0;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    initialise_monitor_handles();
#endif

    /* Init Debug UART for printf */
    InitDebugUart();

    /* Lock protected registers */
    SYS_LockReg();


    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
    printf("+--------------------------------------------+\n");
    printf("|   EADC Tamperature Detection Sample Code   |\n");
    printf("+--------------------------------------------+\n\n");

    /* Measure temperature */
    dTemperature = GetTemperature();

    if (dTemperature > 50)
        printf("Abnormal temperature: %dC\n", (int32_t)dTemperature);
    else
        printf("Chip temperature: %dC\n", (int32_t)dTemperature);

    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Reset EADC module */
    SYS_ResetModule(SYS_EADC0RST);
    /* Disable EADC IP clock */
    CLK_DisableModuleClock(EADC0_MODULE);
    /* Lock protected registers */
    SYS_LockReg();
    /* Disable External Interrupt */
    NVIC_DisableIRQ(EADC00_IRQn);

    while (1);

}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
