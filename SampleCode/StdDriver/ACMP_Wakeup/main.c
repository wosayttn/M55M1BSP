/****************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Show how to wake up MCU from Power-down mode by ACMP wake-up function.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"


#define NPD0_MODE   0    // Power-down mode 0 
#define NPD1_MODE   1    // Power-down mode 1
#define NPD3_MODE   2    // Power-down mode 3
#define SPD0_MODE   3    // Standby Power-down mode

/*---------------------------------------------------------------------------------------------------------*/
/*                                       Global variables                                                  */
/*---------------------------------------------------------------------------------------------------------*/
volatile int32_t  g_i32WakeUp = FALSE;

/*---------------------------------------------------------------------------------------------------------*/
/*                                 Define functions prototype                                              */
/*---------------------------------------------------------------------------------------------------------*/
void ACMP01_IRQHandler(void);
void PWRWU_IRQHandler(void);
void EnterToPowerDown(uint32_t u32PDMode);
void SYS_Init(void);
int IsDebugFifoEmpty(void);
int32_t main(void);

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    extern void initialise_monitor_handles(void);
#endif

int IsDebugFifoEmpty(void)
{
 if((DEBUG_PORT->FIFOSTS & UART_FIFOSTS_TXEMPTY_Msk)!= 0)
   return 0;
 else
   return 1;
}

/*---------------------------------------------------------------------------------------------------------*/
/*                                         ACMP01  Handle                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void ACMP01_IRQHandler(void)
{
    CLK_WaitModuleClockReady(ACMP01_MODULE);
    CLK_WaitModuleClockReady(UART0_MODULE);
    printf("\nACMP1 interrupt!\n");
    /* Clear ACMP 1 interrupt flag */
    ACMP_CLR_INT_FLAG(ACMP01, 1);
    /* Clear wake-up interrupt flag */
    ACMP_CLR_WAKEUP_INT_FLAG(ACMP01, 1);
}
/*---------------------------------------------------------------------------------------------------------*/
/*                                         PWRWU  Handle                                                   */
/*---------------------------------------------------------------------------------------------------------*/

void PMC_IRQHandler(void)
{
  if(PMC->INTSTS &  PMC_INTSTS_PDWKIF_Msk)
  { 
     printf("PMC Wake up\n");  
     PMC->INTSTS |= PMC_INTSTS_CLRWK_Msk;
     g_i32WakeUp = TRUE;
  }

}
/**
  * @brief      Enter To Power Down
  * @param[in]   u32PDMode    The specified Power down module.
  *                               - \ref CLK_PMUCTL_PDMSEL_PD      : Power-down mode
  *                               - \ref CLK_PMUCTL_PDMSEL_FWPD    : Fast wake up
  *
  * @return     None
  *
  * @details    This API is used to get the current RTC date and time value.
  */

void EnterToPowerDown(uint32_t u32PDMode)
{
    g_i32WakeUp = FALSE;
    SYS_UnlockReg();
    /* To program PWRCTL register, it needs to disable register protection first. */
    PMC->INTEN &= ~PMC_INTEN_PDWKIEN_Msk;

    if (u32PDMode == NPD0_MODE)
        PMC_SetPowerDownMode(PMC_NPD0,PMC_PLCTL_PLSEL_PL0);    //Power down
    else if (u32PDMode == NPD1_MODE)
         PMC_SetPowerDownMode(PMC_NPD1,PMC_PLCTL_PLSEL_PL0);    //Power down
    else if (u32PDMode == NPD3_MODE)
         PMC_SetPowerDownMode(PMC_NPD3,PMC_PLCTL_PLSEL_PL0);    //Power down
        else if (u32PDMode == NPD3_MODE)
         PMC_SetPowerDownMode(PMC_SPD0,PMC_PLCTL_PLSEL_PL0);    //Power down

    PMC->INTEN |= PMC_INTEN_PDWKIEN_Msk;

    PMC_PowerDown();
    SYS_LockReg();

    while (g_i32WakeUp == FALSE);

}



void SYS_Init(void)
{

    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);


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
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CyclesPerUs automatically. */
    SystemCoreClockUpdate();

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Enable ACMP01 peripheral clock */
    CLK_EnableModuleClock(ACMP01_MODULE);
    /* Enable GPB peripheral clock */
    CLK_EnableModuleClock(GPIOB_MODULE);
    /* Enable GPC peripheral clock */
    CLK_EnableModuleClock(GPIOC_MODULE);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CyclesPerUs automatically. */
    SystemCoreClockUpdate();


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

}

/*
 * When the voltage of the positive input is greater than the voltage of the negative input,
 * the analog comparator outputs logical one; otherwise, it outputs logical zero.
 * This chip will be waked up from power down mode when detecting a transition of analog comparator's output.
 */

int32_t main(void)
{
    uint32_t u32DelayCount;

    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();
    /* Lock protected registers */
    SYS_LockReg();

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    initialise_monitor_handles();
#endif

    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART0, 115200);

    printf("\nThis sample code demonstrates ACMP1 function. Using ACMP1_P1 (PB4) as ACMP1\n");
    printf("positive input and using internal CRV as the negative input.\n");
    printf("The compare result reflects on ACMP1_O (PC0).\n");
    printf("Press any key to enter power down mode ...\n");
    getchar();

    /* Select VDDA as CRV source */
    ACMP_SELECT_CRV1_SRC(ACMP01, ACMP_VREF_CRV1SSEL_VDDA);
    /* Select CRV1 level: VDDA * 31 / 63 */
    ACMP_CRV1_SEL(ACMP01, 31);
    /* Configure ACMP1. Enable ACMP1 and select CRV as the source of ACMP negative input. */
    ACMP_Open(ACMP01, 1, ACMP_CTL_NEGSEL_CRV, ACMP_CTL_HYSTERESIS_DISABLE);

    /* Select P1 as ACMP positive input channel */
    ACMP_SELECT_P(ACMP01, 1, ACMP_CTL_POSSEL_P1);
    __NOP();

    for (u32DelayCount = 0; u32DelayCount < 100; u32DelayCount++); /* For ACMP setup time */

    __NOP();
    /* Clear ACMP 0 interrupt flag */
    ACMP_CLR_INT_FLAG(ACMP01, 1);

    /* Enable wake-up function */
    ACMP_ENABLE_WAKEUP(ACMP01, 1);
    /* Enable interrupt */
    ACMP_ENABLE_INT(ACMP01, 1);
    /* Enable ACMP01 interrupt */
    NVIC_EnableIRQ(ACMP01_IRQn);
    /* Enable PMC interrupt */
    NVIC_EnableIRQ(PMC_IRQn);
    printf("\nSystem enter power-down mode ... \n");

    /* To check if all the debug messages are finished */
    while (IsDebugFifoEmpty() == 0) {};

    EnterToPowerDown(NPD0_MODE);

    printf("Wake up by ACMP1!\n");

    ACMP_DISABLE_WAKEUP(ACMP01, 1);

    NVIC_DisableIRQ(ACMP01_IRQn);

    NVIC_DisableIRQ(PMC_IRQn);

    while (1);

}
/*** (C) COPYRIGHT 2019 Nuvoton Technology Corp. ***/

