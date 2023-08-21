/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Use DMIC_VAD to wake up system from Power-down mode periodically.
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

#define VAD_DETECT_SAMPLERATE      (8000)

static volatile uint32_t g_u32PDWK;

void VAD_Init(void)
{
    DMIC_VAD_BIQ_T sBIQCoeff;
    // (1) Enable DMIC for VAD(Voice active detection)
    // Select DMIC_VAD CLK source from HIRC.
    CLK_SetModuleClock(DMIC0_MODULE, CLK_DMICSEL_DMIC0SEL_HIRC, 0UL);
    CLK_SetModuleClock(VAD0SEL_MODULE, CLK_DMICSEL_VAD0SEL_HIRC, 0UL);
    // Enable DMIC_VAD clock.
    CLK_EnableModuleClock(VAD0SEL_MODULE);
    // DMIC_VAD IPReset.
    SYS_ResetModule(SYS_DMIC0RST);
    // Set channel's latch data falling type.
    DMIC_SET_LATCHEDGE_CH01(DMIC0, DMIC_LATCHDATA_CH01FR);
    // GPIO multi-function.
    SET_DMIC0_CLKLP_PA3();

    // (2) Enable VAD and input its detect parameter.
    // Enable VAD down sample rate 64.
    DMIC_VAD_SET_DOWNSAMPLE(VAD0, DMIC_VAD_DOWNSAMPLE_64);
    // Set detect sample rate.
    DMIC_VAD_SetSampleRate(VAD0, VAD_DETECT_SAMPLERATE);
    // Writer BIQ coefficient(a0,a1,b0,b1,b2)
    sBIQCoeff.u16BIQCoeffA1 = 0xC715;
    sBIQCoeff.u16BIQCoeffA2 = 0x19A0;
    sBIQCoeff.u16BIQCoeffB0 = 0x1CA3;
    sBIQCoeff.u16BIQCoeffB1 = 0xC6BB;
    sBIQCoeff.u16BIQCoeffB2 = 0x1CA3;
    DMIC_VAD_SetBIQCoeff(VAD0, &sBIQCoeff);
    // Enable VAD BIQ filter.
    DMIC_VAD_ENABLE_BIQ(VAD0);
    // Set short term attack time.
    DMIC_VAD_SET_STAT(VAD0, DMIC_VAD_STAT_2MS);
    // Set long term attack time
    DMIC_VAD_SET_LTAT(VAD0, DMIC_VAD_LTAT_64MS);
    // Set short term power threshold.
    DMIC_VAD_SET_STTHRE(VAD0, DMIC_VAD_POWERTHRE_0DB);
    // Set long term power threshold.
    DMIC_VAD_SET_LTTHRE(VAD0, DMIC_VAD_POWERTHRE_10DB);
    // Set deviation threshold.
    DMIC_VAD_SET_DEVTHRE(VAD0, DMIC_VAD_POWERTHRE_10DB);
}

void VAD_Start(void)
{
    NVIC_EnableIRQ(DMIC0VAD_IRQn);
    DMIC_VAD_ENABLE(VAD0);
}

void VAD_Stop(void)
{
    DMIC_VAD_DISABLE(VAD0);
    NVIC_DisableIRQ(DMIC0VAD_IRQn);
}

NVT_ITCM void DMIC0VAD_IRQHandler()
{
    CLK_WaitModuleClockReady(DMIC0_MODULE);//TESTCHIP_ONLY
    CLK_WaitModuleClockReady(UART0_MODULE);//TESTCHIP_ONLY
    printf("VAD_IS_ACTIVE? %lx\n", DMIC_VAD_IS_ACTIVE(VAD0));
    DMIC_VAD_CLR_ACTIVE(VAD0);
    VAD_Stop();
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Power Wake-up IRQ Handler                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
NVT_ITCM void PMC_IRQHandler(void)
{
    g_u32PDWK = PMC_GetPMCWKSrc();

    /* check power down wakeup flag */
    if (g_u32PDWK & PMC_INTSTS_PDWKIF_Msk)
    {
        PMC->INTSTS |= PMC_INTSTS_CLRWK_Msk;

        while (PMC_GetPMCWKSrc() & PMC_INTSTS_PDWKIF_Msk);
    }
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
    printf("DMIC_VAD power down/wake up sample code\n");
    VAD_Init();
    VAD_Start();
    PMC_ENABLE_INT();
    NVIC_EnableIRQ(PMC_IRQn);
    g_u32PDWK = 0;
    printf("Enter Power-down !\n");
    PowerDownFunction();

    while (!g_u32PDWK) {};

    printf("Wake Up PASS\n");

    /* Got no where to go, just loop forever */
    while (1) ;
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
