/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Show how to trigger LPADC by STADC pin.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t g_u32LpadcIntFlag, g_u32COVNUMFlag = 0;

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
    GPIO_SetMode(PB, BIT2|BIT3, GPIO_MODE_INPUT);

    /* Configure the PB.2 - PB.3 ADC analog input pins.  */
    SET_LPADC0_CH2_PB2();
    SET_LPADC0_CH3_PB3();

    /* Disable the PB.2 - PB.3 digital input path to avoid the leakage current. */
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT2|BIT3);

    /* Set PC.13 to input mode */
    GPIO_SetMode(PC, BIT13, GPIO_MODE_INPUT);
    /* Configure the PC.13 LPADC trigger pins. */
    SET_LPADC0_ST_PC13();

}

void LPADC_FunctionTest()
{
    uint8_t  u8Option;
    uint8_t  u8Index;
    uint32_t u32COVNUMFlag;
    int32_t  i32ConversionData[6] = {0};

    printf("\n");
    printf("+----------------------------------------------------------------------+\n");
    printf("|                 LPADC trigger by STADC pin test                      |\n");
    printf("+----------------------------------------------------------------------+\n");

    printf("\nIn this test, software will get 6 conversion result from the specified channel\n");
    printf("   that triggered by STADC pin (PC.13).\n");

    /* Enable LPADC converter */
    LPADC_POWER_ON(LPADC0);

    while(1)
    {
        printf("Select input mode:\n");
        printf("  [1] Single end input (channel 2 only)\n");
        printf("  [2] Differential input (channel pair 1 only)\n");
        printf("  Other keys: exit single mode test\n");
        u8Option = getchar();
        if(u8Option == '1')
        {
            /* Set input mode as single-end, Single mode, and select channel 2 */
            LPADC_Open(LPADC0, LPADC_ADCR_DIFFEN_SINGLE_END, LPADC_ADCR_ADMD_SINGLE, BIT2);

            /* Configure the sample module and enable STADC pin trigger source */
            LPADC_EnableHWTrigger(LPADC0, LPADC_STADC_TRIGGER, LPADC_ADCR_TRGCOND_FALLING_EDGE);

            /* Clear the A/D interrupt flag for safe */
            LPADC_CLR_INT_FLAG(LPADC0, LPADC_ADF_INT);

            /* Enable the sample module interrupt */
            LPADC_EnableInt(LPADC0, LPADC_ADF_INT);
            NVIC_EnableIRQ(LPADC0_IRQn);

            printf("Wait falling edge from STADC pin (PC.13) ...\n");

            /* Reset the LPADC indicator and wait falling edge on STADC pin */
            g_u32LpadcIntFlag = 0;
            g_u32COVNUMFlag = 0;

            while(1)
            {
                /* Wait LPADC interrupt (g_u32LpadcIntFlag will be set at IRQ_Handler function) */
                while(g_u32LpadcIntFlag == 0);

                /* Reset the LPADC interrupt indicator */
                g_u32LpadcIntFlag = 0;

                /* Get the conversion result of LPADC channel 2 */
                u32COVNUMFlag = g_u32COVNUMFlag - 1;
                i32ConversionData[u32COVNUMFlag] = LPADC_GET_CONVERSION_DATA(LPADC0, 2);

                if(g_u32COVNUMFlag >= 6)
                    break;
            }

            /* Disable the sample module interrupt */
            LPADC_DisableInt(LPADC0, LPADC_ADF_INT);

            printf("Conversion result of channel pair 1:\n");
            for(u8Index = 0; (u8Index) < 6; u8Index++)
                printf("                                0x%X (%d)\n", i32ConversionData[u8Index], i32ConversionData[u8Index]);
        }
        else if(u8Option == '2')
        {
            /* Set input mode as differential, Single mode, and select channel 2 */
            LPADC_Open(LPADC0, LPADC_ADCR_DIFFEN_DIFFERENTIAL, LPADC_ADCR_ADMD_SINGLE, BIT2);

            /* Configure the sample module and enable STADC pin trigger source */
            LPADC_EnableHWTrigger(LPADC0, LPADC_STADC_TRIGGER, LPADC_ADCR_TRGCOND_FALLING_EDGE);

            /* Clear the A/D interrupt flag for safe */
            LPADC_CLR_INT_FLAG(LPADC0, LPADC_ADF_INT);

            /* Enable the sample module interrupt */
            LPADC_EnableInt(LPADC0, LPADC_ADF_INT);
            NVIC_EnableIRQ(LPADC0_IRQn);

            printf("Wait falling edge from STADC pin (PC.13) ...\n");

            /* Reset the LPADC indicator and wait falling edge on STADC pin */
            g_u32LpadcIntFlag = 0;
            g_u32COVNUMFlag = 0;

            while(1)
            {
                /* Wait LPADC interrupt (g_u32LpadcIntFlag will be set at IRQ_Handler function) */
                while(g_u32LpadcIntFlag == 0);

                /* Reset the LPADC interrupt indicator */
                g_u32LpadcIntFlag = 0;

                /* Get the conversion result of the sample module 0 */
                u32COVNUMFlag = g_u32COVNUMFlag - 1;
                i32ConversionData[u32COVNUMFlag] = LPADC_GET_CONVERSION_DATA(LPADC0, 2);

                if(g_u32COVNUMFlag >= 6)
                    break;
            }

            /* Disable the sample module interrupt */
            LPADC_DisableInt(LPADC0, LPADC_ADF_INT);

            printf("Conversion result of channel pair 1:\n");
            for(u8Index = 0; (u8Index) < 6; u8Index++)
                printf("                                0x%X (%d)\n", i32ConversionData[u8Index], i32ConversionData[u8Index]);
        }
        else
            return ;
    }
}

void LPADC0_IRQHandler(void)
{
    LPADC_CLR_INT_FLAG(LPADC0, LPADC_ADF_INT); /* Clear the A/D interrupt flag */
    g_u32LpadcIntFlag = 1;
    g_u32COVNUMFlag++;
    printf("[#%d] LPADC conversion done.\n", g_u32COVNUMFlag);
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

    /* Disable External Interrupt */
    NVIC_DisableIRQ(LPADC0_IRQn);

    printf("Exit LPADC sample code\n");

    while(1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/