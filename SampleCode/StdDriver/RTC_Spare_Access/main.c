/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate the RTC spare_register read/write function and displays test result to the UART console.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Functions and variables declaration                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void);
void UART_Init(void);


void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/  
    /* Set X32_OUT(PF.4) and X32_IN(PF.5)*/
    SET_X32_IN_PF5(); 
    SET_X32_OUT_PF4();
    
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);
    /* Enable External LXT clock */
    CLK_EnableXtalRC(CLK_SRCCTL_LXTEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    /* Waiting for LXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);

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
   
    /* Enable module clock */
    CLK_EnableModuleClock(RTC0_MODULE);

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();
    
}

void UART_Init(void)
{
    /* Configure UART and set UART Baudrate */
    UART_Open(DEBUG_PORT, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  MAIN function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    S_RTC_TIME_DATA_T sInitTime;
    S_RTC_TIME_DATA_T *sptrInitTime;
    uint32_t i, u32SPRData = 0;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART for printf */
    UART_Init();

    /* Lock protected registers */
    SYS_LockReg();

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
    printf("+------------------------------------------------+\n");
    printf("|    RTC Spare Register Read/Write Sample Code   |\n");
    printf("+------------------------------------------------+\n\n");
    
    
    /* Set LXT as RTC clock source */
    RTC_SetClockSource(RTC_CLOCK_SOURCE_LXT);

    /* Time Setting */
    sInitTime.u32Year       = 2023;
    sInitTime.u32Month      = 6;
    sInitTime.u32Day        = 8;
    sInitTime.u32Hour       = 13;
    sInitTime.u32Minute     = 0;
    sInitTime.u32Second     = 0;
    sInitTime.u32DayOfWeek  = RTC_MONDAY;
    sInitTime.u32TimeScale  = RTC_CLOCK_24;
    
    /* check rtc reset status */
    sptrInitTime = (RTC->INIT & RTC_INIT_ACTIVE_Msk) ? NULL : &sInitTime;
    
    if(RTC_Open(sptrInitTime) != 0)
    {
        printf("\n RTC initial fail!!");
        printf("\n Please check h/w setting!!");
        return -1;
    }

    printf("# RTC Spare Register Read/Write Test:\n\n");

    /* Enable spare register */
    RTC_EnableSpareAccess();

    /* Write spare register */
    for(i = 0; i < 20; i++)
    {
        RTC_WRITE_SPARE_REGISTER(RTC, i, i);
    }

    /* Check spare register data */
    for(i = 0; i < 20; i++)
    {
        u32SPRData = RTC_READ_SPARE_REGISTER(RTC, i);
        if(u32SPRData != i)
        {
            printf(" SPARE_REGISTER[%d] = 0x%x.\n", i, u32SPRData);
            printf(" Get spare register Fail!! \n");
            return -1;
        }
        else
        {
            printf(" SPARE_REGISTER[%d] = 0x%x.\n", i, u32SPRData);
        }
    }

    printf("\n Compare spare registers data ... Pass!! \n");

    while(1) {}
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
