/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Perform A/D Conversion with LPADC burst mode.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define CONV_TOTAL_COUNT    20

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

    /* Set PB.2 - PB.3 to input mode */
    GPIO_SetMode(PB, BIT2 | BIT3, GPIO_MODE_INPUT);

    /* Configure the PB.2 - PB.3 LPADC analog input pins. */
    SET_LPADC0_CH2_PB2();
    SET_LPADC0_CH3_PB3();

    /* Disable the PB.2 - PB.3 digital input path to avoid the leakage current. */
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT2 | BIT3);

}

void LPADC_FunctionTest()
{
    uint8_t  u8Option;
    uint32_t u32ConvCount;
    int32_t  i32ConversionData[CONV_TOTAL_COUNT];

    printf("\n");
    printf("+----------------------------------------------------------------------+\n");
    printf("|                   LPADC burst mode sample code                       |\n");
    printf("+----------------------------------------------------------------------+\n");

    for (u32ConvCount = 0; u32ConvCount < CONV_TOTAL_COUNT; u32ConvCount++)
    {
        i32ConversionData[u32ConvCount] = 0;
    }

    /* Enable LPADC converter */
    LPADC_POWER_ON(LPADC0);

    while (1)
    {
        printf("Select input mode:\n");
        printf("  [1] Single end input (channel 2 only)\n");
        printf("  [2] Differential input (channel pair 1 only)\n");
        printf("  Other keys: exit burst mode test\n");
        u8Option = getchar();

        if (u8Option == '1')
        {
            /* Set input mode as single-end, burst mode, and select channel 2 */
            LPADC_Open(LPADC0, LPADC_ADCR_DIFFEN_SINGLE_END, LPADC_ADCR_ADMD_BURST, BIT2);

            /* Clear the A/D interrupt flag for safe */
            LPADC_CLR_INT_FLAG(LPADC0, LPADC_ADF_INT);

            /* Reset the LPADC interrupt indicator and trigger sample module to start A/D conversion */
            u32ConvCount = 0;
            LPADC_START_CONV(LPADC0);

            while (1)
            {
                /* Wait LPADC conversion completed */
                while (LPADC_GET_INT_FLAG(LPADC0, LPADC_ADF_INT) == 0);

                LPADC_CLR_INT_FLAG(LPADC0, LPADC_ADF_INT); /* clear ADF interrupt flag */

                /* Get the conversion result until VALIDF turns to 0 */
                while (LPADC0->ADSR0 & LPADC_ADSR0_VALIDF_Msk)
                {
                    /* Get the conversion result from LPADC channel 0 always */
                    i32ConversionData[u32ConvCount++] = LPADC_GET_CONVERSION_DATA(LPADC0, 0);

                    if (u32ConvCount == CONV_TOTAL_COUNT)
                        break;
                }

                if (u32ConvCount == CONV_TOTAL_COUNT)
                    break;
            }

            /* Stop A/D conversion */
            LPADC_STOP_CONV(LPADC0);

            /* Show the conversion result */
            for (u32ConvCount = 0; u32ConvCount < CONV_TOTAL_COUNT; u32ConvCount++)
            {
                printf("Conversion result of channel 2 [#%d]: 0x%X (%d)\n", u32ConvCount + 1, i32ConversionData[u32ConvCount], i32ConversionData[u32ConvCount]);
            }

            /* Clear remaining data in FIFO that got before stop LPADC */
            while (LPADC_IS_DATA_VALID(LPADC0, 0))
            {
                i32ConversionData[0] = LPADC_GET_CONVERSION_DATA(LPADC0, 0);
            }
        }
        else if (u8Option == '2')
        {
            /* Set input mode as differential, burst mode, and select channel 2 */
            LPADC_Open(LPADC0, LPADC_ADCR_DIFFEN_DIFFERENTIAL, LPADC_ADCR_ADMD_BURST, BIT2);

            /* Clear the A/D interrupt flag for safe */
            LPADC_CLR_INT_FLAG(LPADC0, LPADC_ADF_INT);

            /* Reset the LPADC interrupt indicator and trigger sample module to start A/D conversion */
            u32ConvCount = 0;
            LPADC_START_CONV(LPADC0);

            while (1)
            {
                /* Wait LPADC conversion completed */
                while (LPADC_GET_INT_FLAG(LPADC0, LPADC_ADF_INT) == 0);

                LPADC_CLR_INT_FLAG(LPADC0, LPADC_ADF_INT); /* clear ADF interrupt flag */

                /* Get the conversion result until VALIDF turns to 0 */
                while (LPADC0->ADSR0 & LPADC_ADSR0_VALIDF_Msk)
                {
                    /* Get the conversion result from LPADC channel 0 always */
                    i32ConversionData[u32ConvCount++] = LPADC_GET_CONVERSION_DATA(LPADC0, 0);

                    if (u32ConvCount == CONV_TOTAL_COUNT)
                        break;
                }

                if (u32ConvCount == CONV_TOTAL_COUNT)
                    break;
            }

            /* Stop A/D conversion */
            LPADC_STOP_CONV(LPADC0);

            /* Show the conversion result */
            for (u32ConvCount = 0; u32ConvCount < CONV_TOTAL_COUNT; u32ConvCount++)
            {
                printf("Conversion result of channel pair 1 [#%d]: 0x%X (%d)\n", u32ConvCount + 1, i32ConversionData[u32ConvCount], i32ConversionData[u32ConvCount]);
            }

            /* Clear remaining data in FIFO that got before stop LPADC */
            while (LPADC_IS_DATA_VALID(LPADC0, 0))
            {
                i32ConversionData[0] = LPADC_GET_CONVERSION_DATA(LPADC0, 0);
            }
        }
        else
            return;

        printf("\n");
    }
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

    printf("Exit LPADC sample code\n");

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
