/****************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate how ACMP compare DAC output with ACMP1_P1 value.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/* Generates a Square wave*/
const uint16_t g_au16Square[] = { 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050,
                                  3900, 3900, 3900, 3900, 3900, 3900, 3900, 3900, 3900, 3900
                                };

static uint32_t g_u32Index = 0;
const uint32_t g_u32ArraySize = sizeof(g_au16Square) / sizeof(uint16_t);

/*---------------------------------------------------------------------------------------------------------*/
/*                                 Define functions prototype                                              */
/*---------------------------------------------------------------------------------------------------------*/
void DAC01_IRQHandler(void);
void ACMP01_IRQHandler(void);
void SYS_Init(void);
int32_t main(void);


#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    extern void initialise_monitor_handles(void);
#endif


void DAC01_IRQHandler(void)
{
    if (DAC_GET_INT_FLAG(DAC0, 0))
    {

        if (g_u32Index == g_u32ArraySize)
        {
            g_u32Index = 0;
        }
        else
        {
            DAC_WRITE_DATA(DAC0, 0, g_au16Square[g_u32Index++]);
            DAC_START_CONV(DAC0);
            /* Clear the DAC conversion complete finish flag */
            DAC_CLR_INT_FLAG(DAC0, 0);
        }
    }

}

void ACMP01_IRQHandler(void)
{
    static uint32_t u32Cnt = 0;

    /* Clear ACMP 1 interrupt flag */
    ACMP_CLR_INT_FLAG(ACMP01, 1);

    /* Check Comparator 0 Output Status */
    if (ACMP_GET_OUTPUT(ACMP01, 1))
    {
        printf("ACMP1_P voltage >  DAC voltage (%u) ACMP1_O(%d)\n", u32Cnt, PC0);
    }
    else
    {
        printf("ACMP1_P voltage <= DAC voltage (%u) ACMP1_O(%d)\n", u32Cnt, PC0);
    }

    u32Cnt++;
}


void SYS_Init(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
  
    /* Enable External RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);

    /* Waiting for External RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Switch SCLK clock source to HIRC before PLL setting */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_HIRC, CLK_ACLKDIV_ACLKDIV(1));

   /* Enable PLL0 200MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_200MHZ, CLK_APLL0_SELECT);    

    /* Switch SCLK clock source to PLL0 and divide 1 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0, CLK_ACLKDIV_ACLKDIV(1));

    /* Set HCLK2 divide 2 */
    CLK_SET_HCLK2DIV(2);
    
    /* Set PCLKx divide 2 */
    CLK_SET_PCLK0DIV(2);
    CLK_SET_PCLK2DIV(2);
    CLK_SET_PCLK3DIV(2);
    CLK_SET_PCLK4DIV(2);
    /* Set PCLK1 divide 4 */
    CLK_SET_PCLK1DIV(4);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Enable ACMP01 peripheral clock */
    CLK_EnableModuleClock(ACMP01_MODULE);
    /* Enable GPB peripheral clock */
    CLK_EnableModuleClock(GPIOB_MODULE);
    /* Enable GPC peripheral clock */
    CLK_EnableModuleClock(GPIOC_MODULE);

    /* Enable DAC01 module clock */
    CLK_EnableModuleClock(DAC01_MODULE);


    /* Set PB.4 and PC.0 to input mode */
    PB->MODE &= ~(GPIO_MODE_MODE4_Msk);
    PC->MODE &= ~(GPIO_MODE_MODE0_Msk);

    /* Set PB4 multi-function pin for ACMP1 positive input pin and PC0 multi-function pin for ACMP1 output pin*/
    SET_ACMP1_P1_PB4();
    SET_ACMP1_O_PC0();

     /* Set PB multi-function pins for UART0 RXD and TXD */
    SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13();

    /* Disable digital input path of analog pin ACMP1_P1 to prevent leakage */
    GPIO_DISABLE_DIGITAL_PATH(PB, (1ul << 4));

    /* Lock protected registers */
    SYS_LockReg();
}

/*
 * When the voltage of the positive input is greater than the voltage of the
 * negative input, the analog comparator outputs logical one; otherwise, it outputs
 * logical zero. This sample code will show the expression of the comparator's
 * number when detecting a transition of analog comparator's output.
 */

int32_t main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();
    /* Lock protected registers */
    SYS_LockReg();

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    initialise_monitor_handles();
#endif

#if !(defined(DEBUG_ENABLE_SEMIHOST))
    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART0, 115200);
#endif

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);

   
    printf("\nThis sample code demonstrates ACMP1 function. Using ACMP1_P1 (PB4) as ACMP1\n");
    printf("positive input and using DAC output as the negative input.\n");
    printf("Please connect the ACMP1_P1(PB4) to 1.5v .\n");
    

    printf("The compare result reflects on ACMP1_O (PC0).\n");
    printf("Press any key to start ...\n");
    getchar();

    /* Set the software trigger, enable DAC even trigger mode and enable D/A converter */
    DAC_Open(DAC0, 0, DAC_SOFTWARE_TRIGGER);

    /* The DAC conversion settling time is 1ms */
    DAC_SetDelayTime(DAC0, 100);

    /* Set DAC 12-bit holding data */
    DAC_WRITE_DATA(DAC0, 0, 0x0);

    /* Clear the DAC conversion complete finish flag for safe */
    DAC_CLR_INT_FLAG(DAC0, 0);

    /* Enable the DAC interrupt */
    DAC_ENABLE_INT(DAC0, 0);

    NVIC_EnableIRQ(DAC01_IRQn);

    /* Start A/D conversion */
    DAC_START_CONV(DAC0);

    /* Configure ACMP1. Enable ACMP1 and select DAC voltage as the source of ACMP negative input. */
    ACMP_Open(ACMP01, 1, ACMP_CTL_NEGSEL_DAC0, ACMP_CTL_HYSTERESIS_DISABLE);

    /* Select P1 as ACMP positive input channel */
    ACMP_SELECT_P(ACMP01, 1, ACMP_CTL_POSSEL_P1);

    /* Clear ACMP 1 interrupt flag */
    ACMP_CLR_INT_FLAG(ACMP01, 1);
    /* Enable interrupt */
    ACMP_ENABLE_INT(ACMP01, 1);

    /* Enable ACMP01 interrupt */
    NVIC_EnableIRQ(ACMP01_IRQn);


    while (1);

}

/*** (C) COPYRIGHT 2019 Nuvoton Technology Corp. ***/

