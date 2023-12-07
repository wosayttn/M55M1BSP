/****************************************************************************
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate DAC0 and DAC1 work in group mode
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#if !defined(ALIGN_AF_PINS)
static const uint16_t g_au16Sine[] = {2047, 2251, 2453, 2651, 2844, 3028, 3202, 3365, 3515, 3650, 3769, 3871, 3954,
                                      4019, 4064, 4088, 4095, 4076, 4040, 3984, 3908, 3813, 3701, 3573, 3429, 3272,
                                      3102, 2921, 2732, 2536, 2335, 2132, 1927, 1724, 1523, 1328, 1141,  962,  794,
                                      639,  497,  371,  262,  171,   99,   45,   12,    0,    7,   35,   84,  151,
                                      238,  343,  465,  602,  754,  919, 1095, 1281, 1475, 1674, 1876
                                     };

static const uint32_t g_u32ArraySize = sizeof(g_au16Sine) / sizeof(uint16_t);
static uint32_t g_u32Index = 0;
static uint32_t g_u32Dac0Done = 0, g_u32Dac1Done = 0;

NVT_ITCM void DAC01_IRQHandler(void);
void SYS_Init(void);

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    extern void initialise_monitor_handles(void);
#endif

NVT_ITCM void DAC01_IRQHandler(void)
{
    if (DAC_GET_INT_FLAG(DAC0, 0))
    {
        /* Clear the DAC conversion complete finish flag */
        DAC_CLR_INT_FLAG(DAC0, 0);
        DAC_WRITE_DATA(DAC0, 0, g_au16Sine[g_u32Index]);
        g_u32Dac0Done = 1;

    }

    if (DAC_GET_INT_FLAG(DAC1, 0))
    {

        /* Clear the DAC conversion complete finish flag */
        DAC_CLR_INT_FLAG(DAC1, 0);
        DAC_WRITE_DATA(DAC1, 0, g_au16Sine[g_u32Index >= g_u32ArraySize / 2 ? g_u32Index - g_u32ArraySize / 2 : g_u32Index + g_u32ArraySize / 2]);
        g_u32Dac1Done = 1;

        if (++g_u32Index == g_u32ArraySize)
            g_u32Index = 0;
    }

    if (g_u32Dac0Done == 1 && g_u32Dac1Done == 1)
    {
        DAC_START_CONV(DAC0);
        g_u32Dac0Done = g_u32Dac1Done = 0;
    }

    return;
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

    /* Switch SCLK clock source to APLL0 and Enable APLL0 180MHz clock */
    CLK_SetBusClock(CLK_SCLKSEL_SCLKSEL_APLL0, FREQ_180MHZ);
    /* Set PCLK1 divide 4 */
    CLK_SET_PCLK1DIV(4);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();
    /* Enable DAC01 module clock */
    CLK_EnableModuleClock(DAC01_MODULE);
    /* Enable GPB peripheral clock */
    CLK_EnableModuleClock(GPIOB_MODULE);
    /* Debug UART clock setting*/
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set PB multi-function pins for Debug UART RXD and TXD */
    SetDebugUartMFP();

}

int32_t main(void)
{
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

    printf("\n");
    printf("+------------------------------------------------------------------------+\n");
    printf("|                          DAC Driver Sample Code                        |\n");
    printf("+------------------------------------------------------------------------+\n");
    printf("DAC0 and DAC1 is configured in group mode and update simultaneously\n");

    /* Set multi-function pin for DAC voltage output */
    SET_DAC0_OUT_PB12();//conflict with UART0_RXD pin
    SET_DAC1_OUT_PB13();//conflict with UART0_TXD pin

    /* Disable digital input path of analog pin DAC0_OUT and DAC1_OUT to prevent leakage */
    GPIO_DISABLE_DIGITAL_PATH(PB, (1ul << 12) | (1ul << 13));

    /* Single Mode test */
    /* Set the software trigger, enable DAC even trigger mode and enable D/A converter */
    DAC_Open(DAC0, 0, DAC_SOFTWARE_TRIGGER);
    DAC_Open(DAC1, 0, DAC_SOFTWARE_TRIGGER);

    /* Enable DAC to work in group mode, once group mode enabled, DAC1 is configured by DAC0 registers */
    DAC_ENABLE_GROUP_MODE(DAC0);

    /* The DAC conversion settling time is 1us */
    DAC_SetDelayTime(DAC0, 1);

    /* Set DAC 12-bit holding data */
    DAC_WRITE_DATA(DAC0, 0, g_au16Sine[0]);
    DAC_WRITE_DATA(DAC1, 0, g_au16Sine[g_u32ArraySize / 2]);

    /* Clear the DAC conversion complete finish flag for safe */
    DAC_CLR_INT_FLAG(DAC0, 0);

    /* Enable the DAC interrupt */
    DAC_ENABLE_INT(DAC0, 0);
    DAC_ENABLE_INT(DAC1, 0);
    NVIC_EnableIRQ(DAC01_IRQn);

    /* Start A/D conversion */
    DAC_START_CONV(DAC0);

    while (1) {};

}
#else
int main()
{
    // Not support on M55M1 TEST CHIP.
    while (1) {};
}
#endif
/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
