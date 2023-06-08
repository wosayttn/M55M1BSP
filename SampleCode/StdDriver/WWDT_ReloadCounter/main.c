/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate how to reload the WWDT0/1 counter value without resetting the MCU.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define RELOAD_CONDITION  3
#define WWDT_PORT   (0)
#if (WWDT_PORT != 0)
#define WWDT            WWDT1
#define WWDT_IRQHandler WWDT1_IRQHandler
#else
#define WWDT            WWDT0
#define WWDT_IRQHandler WWDT0_IRQHandler
#endif
/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
int IsDebugFifoEmpty(void)
{
    return ((DEBUG_PORT->FIFOSTS & UART_FIFOSTS_TXEMPTYF_Msk) != 0U);
}

void UserAlgorithm(void)
{
    printf("-> Reload WWDT counter (%d) in UserAlgorithm()\n",WWDT->CNT);

    while(!IsDebugFifoEmpty());

    /* To reload the WWDT counter value to 0x3F */
    WWDT_RELOAD_COUNTER(WWDT);
    /* Algorithm */
    PA0 ^= 1;
}

void Delay_ms(uint32_t ms)
{
    uint32_t i;
    uint32_t cnt = 0;

    printf("\nDelay %d ms before UserAlgorithm()\n",ms);
    printf("WWDT Counter\n");

    for(i=0 ; i<ms ; i++)
    {
        CLK_SysTickDelay(1000);

        if(WWDT->CNT != cnt)
        {
            cnt = WWDT->CNT;
            printf("%2d \n",WWDT->CNT);
        }
    }
}


volatile uint8_t g_u32WWDTINTCounts;

/**
 * @brief       IRQ Handler for WWDT Interrupt
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The WDT_IRQHandler is default IRQ of WWDT
 */
void WWDT_IRQHandler(void)
{
    if(WWDT_GET_INT_FLAG(WWDT) == 1)
    {
        /* Clear WWDT compare match interrupt flag */
        WWDT_CLEAR_INT_FLAG(WWDT);

        g_u32WWDTINTCounts++;

        printf("\nWWDT compare match interrupt occurred. (Interrupt Count %d)\n", g_u32WWDTINTCounts);
    }
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

    /* Enable PLL0 180MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_180MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to PLL0 and divide 1 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0, CLK_ACLKDIV_ACLKDIV(1));
    
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
    
    /* Set module clock*/
#if (WWDT_PORT != 0)    
    CLK_SetModuleClock(WWDT1_MODULE,CLK_WWDTSEL_WWDT1SEL_HCLK0_DIV2048,0);
#else
    CLK_SetModuleClock(WWDT0_MODULE,CLK_WWDTSEL_WWDT0SEL_HCLK0_DIV2048,0);
#endif

    /* Enable module clock */
    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);

#if (WWDT_PORT != 0)    
    CLK_EnableModuleClock(WWDT1_MODULE);
#else
    CLK_EnableModuleClock(WWDT0_MODULE);
#endif

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();
    
    /* Set PC multi-function pins for GPIO*/
    SET_GPIO_PA0();
}


/*---------------------------------------------------------------------------------------------------------*/
/*  MAIN function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    double dTimeOutPeriodTime;
    double dCompareMatchPeriodTime;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    printf("+------------------------------------------------+\n");
    printf("|         WWDT Reload Counter Sample Code        |\n");
    printf("+------------------------------------------------+\n\n");

    printf(" Test Condition :\n");
    printf(" Condition 1 - Reload before the WWDT window region \n");
    printf(" Condition 2 - Reload after the WWDT window region  \n");
    printf(" Condition 3 - Reload inside the WWDT window region \n\n");

    /* To check if system has been reset by WWDT time-out reset or not */
    if(WWDT_GET_RESET_FLAG(WWDT) == 1)
    {
        printf("*** System has been reset by WWDT time-out reset event. [WWDT_CTL: 0x%08X] ***\n\n", WWDT->CTL);
        WWDT_CLEAR_RESET_FLAG(WWDT);
        while(1);
    }

    /* WWDT clock source is HCLK0 / 2048 */
    dTimeOutPeriodTime      = (((double)(1000000 * 2048) / (double)CLK_GetHCLK0Freq()) * 1024) * 64 / 1000;
    dCompareMatchPeriodTime = (((double)(1000000 * 2048) / (double)CLK_GetHCLK0Freq()) * 1024) * 32 / 1000;

    printf("# WWDT Settings: \n");
    printf("    - Clock source is HCLK/2048 (%d Hz)    \n", CLK_GetHCLK0Freq() / 2048);
    printf("    - WWDT counter prescale period is 1024  \n");
    printf("    - Interrupt enable                      \n");
    printf("    - Window Compare value is 32            \n");
    printf("      WWDT time-out period is 1024 * (64 * WWDT_CLK)      = %.2f ms\n", dTimeOutPeriodTime);
    printf("      WWDT compare match period is 1024 * (32 * WWDT_CLK) = %.2f ms\n\n", dCompareMatchPeriodTime);

    printf("# System will generate first WWDT compare match interrupt event after %.2f ms.\n", dCompareMatchPeriodTime);
    printf("    1.) use PA.0 high/low period to check reload WWDT counter period time\n");
    printf("    2.) reload WWDT counter value to avoid WWDT time-out reset\n");
    printf("        when interrupt counter value is less than 11.\n");
    printf("    3.) do not reload WWDT counter value to generate WWDT time-out reset system event\n");
    printf("        when interrupt counts large than 10.\n\n");

    /* Use PA.0 to check reload WWDT counter period time */
    GPIO_SetMode(PA, BIT0,GPIO_MODE_OUTPUT);
		
    PA0 = 1;

    /* Enable WWDT NVIC */
#if (WWDT_PORT != 0)
    NVIC_EnableIRQ(WWDT1_IRQn);
#else
    NVIC_EnableIRQ(WWDT0_IRQn);
#endif

    g_u32WWDTINTCounts = 0;

    printf("[WWDT_CTL: 0x%08X]\n\n", WWDT->CTL);
    printf("WWDT Counter 63                       32                        0\n");
    printf("              +------------------------+------------------------+\n");
    printf("Timeline (ms) 0                     %.2f                  %.2f\n", dCompareMatchPeriodTime, dTimeOutPeriodTime);
    printf("              |   Reset When Reload    |   WWDT Window Region   |  WWDT Time-Out Reset\n\n");

#if RELOAD_CONDITION == 1
    printf("Test Condition 1 - Reload before the WWDT window region \n");
#elif RELOAD_CONDITION == 2
    printf("Test Condition 2 - Reload after the WWDT window region  \n");
#elif RELOAD_CONDITION == 3
    printf("Test Condition 3 - Reload inside the WWDT window region \n");
#endif

    printf("\n* Reload WWDT counter in UserAlgorithm()\n");

    /*
        WWDT max time-out period is 1024*(64*WWDT_CLK) = 745.65 ms
        WWDT compare value is 32 = 1024*(32*WWDT_CLK) = 372.83 ms
        Enable WWDT compare match interrupt
    */
    /* Note: WWDT_CTL register can be written only once after chip is powered on or reset */
    WWDT_Open(WWDT,WWDT_PRESCALER_1024, 32, TRUE);

    while(1)
    {
#if RELOAD_CONDITION == 1
        /* Reload before the WWDT window region */
        /* CNTDAT > CMPDAT, Write RLDCNT 0x5AA5 to reload WWWDT counter will reset system */
        /* Delay_ms < 372.83 ms */
        Delay_ms((uint32_t) dCompareMatchPeriodTime / 2);
#elif RELOAD_CONDITION == 2
        /* Reload after the WWDT window region */
        /* Wait CNTDAT count to 0, System reset immediately */
        /* Delay_ms > 745.65 ms */
        Delay_ms((uint32_t) dTimeOutPeriodTime + 1);
#elif RELOAD_CONDITION == 3
        if(g_u32WWDTINTCounts > 10)
        {
            /* Reload after the WWDT window region */
            /* Wait CNTDAT count to 0, System reset immediately */
            Delay_ms((uint32_t) dTimeOutPeriodTime + 1);
        }
        else
        {
            /* Reload inside the WWDT window region */
            /* CNTDAT <= CMPDAT, Write RLDCNT 0x5AA5 will reload CNTDAT to 0x3F */
            /* 745.65 ms > Delay_ms > 372.83 ms */
            Delay_ms((uint32_t) dCompareMatchPeriodTime + 1);
        }
#endif

        UserAlgorithm();
    }
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
