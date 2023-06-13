/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief
 *           Show the usage of EQEI UINT timer function.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t g_au32TMRINTCount[2] = {0};

void EQEI0_IRQHandler(void)
{
    if(EQEI_GET_INT_FLAG(EQEI0, EQEI_STATUS_UTIEF_Msk))     /* EQEI Unit Timer Event flag */
    {
        EQEI_CLR_INT_FLAG(EQEI0, EQEI_STATUS_UTIEF_Msk);
        printf("Unit TImer0 INT!\n\n");
        g_au32TMRINTCount[0]++;
    }

}

void EQEI1_IRQHandler(void)
{
    if(EQEI_GET_INT_FLAG(EQEI1, EQEI_STATUS_UTIEF_Msk))     /* EQEI Unit Timer Event flag */
    {
        EQEI_CLR_INT_FLAG(EQEI1, EQEI_STATUS_UTIEF_Msk);
        printf("Unit TImer1 INT!\n\n");
        g_au32TMRINTCount[1]++;
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
    /* Enable PLL0 72MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_72MHZ, CLK_APLL0_SELECT);

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

    /* Enable module clock */
    CLK_EnableModuleClock(EQEI0_MODULE);    
    CLK_EnableModuleClock(EQEI1_MODULE);
    
    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();
}

void UART0_Init()
{

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

int32_t main(void)
{
    uint32_t u32InitCount, au32Counts[2];    

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);

    printf("+-----------------------------------------+\n");
    printf("|     M55M1 EQEI Unit Timer Sample Code   |\n");
    printf("+-----------------------------------------+\n\n");
    printf("# Unit Timer0 Settings:\n");
    printf("    - Clock source is PCLK0       \n");
    printf("    - Compare Value frequency is 1 Hz\n");
    printf("    - Interrupt enable          \n");
    printf("# Unit Timer1 Settings:\n");
    printf("    - Clock source is PCLK2      \n");
    printf("    - Compare Value frequency is 2 Hz\n");
    printf("    - Interrupt enable          \n");

    /* Set Unit Timer compare value */
    EQEI0->UTCMP = 36000000;
    EQEI1->UTCMP = 36000000/2;    

    /* Enable EQEI interrupt */
    EQEI0->CTL2 |= (EQEI_CTL2_UTIEIEN_Msk | EQEI_STATUS_PHEF_Msk);
    EQEI1->CTL2 |= (EQEI_CTL2_UTIEIEN_Msk | EQEI_STATUS_PHEF_Msk);
    NVIC_EnableIRQ(EQEI0_IRQn);
    NVIC_EnableIRQ(EQEI1_IRQn);
  
    
    /* Clear Unit Timer0 ~ Timer1 interrupt counts to 0 */
    g_au32TMRINTCount[0] = g_au32TMRINTCount[1] = 0;
    u32InitCount = g_au32TMRINTCount[0];    

    /* Start EQEI Unit TImer */
    EQEI0->CTL2 |= EQEI_CTL2_UTEN_Msk;
    EQEI1->CTL2 |= EQEI_CTL2_UTEN_Msk;    

    /* Check EQEI Unit Timer0 ~ Timer1 interrupt counts */
    printf("# EQEI Unit Timer interrupt counts :\n");
    while(u32InitCount < 20)
    {
        if(g_au32TMRINTCount[0] != u32InitCount)
        {
            au32Counts[0] = g_au32TMRINTCount[0];
            au32Counts[1] = g_au32TMRINTCount[1];
            printf("    Unit Timer0:%3d    Unit Timer1:%3d\n", au32Counts[0], au32Counts[1]);
            u32InitCount = g_au32TMRINTCount[0];

            if((au32Counts[1] > (au32Counts[0] * 2 + 1)) || (au32Counts[1] < (au32Counts[0] * 2 - 1)))
            {
                printf("*** FAIL ***\n");
                while(1);
            }
        }
    }
    
    printf("*** PASS ***\n");

    while(1);    

}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
