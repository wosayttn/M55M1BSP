/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate Get the current RTC data/time per tick.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define T_5SEC              (5)  // 5 Sec For Tick Time
#define T_60SEC             (60) // 60Sec
#define T_TICK_1_4_SEC      (4)  // 1/4 Sec.

/*---------------------------------------------------------------------------------------------------------*/
/* Functions and variables declaration                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
static volatile uint32_t g_u32RTCTInt = 0;

void RTC_IRQHandler(void);
void SYS_Init(void);
void UART_Init(void);

/**
 * @brief       IRQ Handler for RTC Interrupt
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The RTC_IRQHandler is default IRQ of RTC, declared in startup_M2354.s.
 */
void RTC_IRQHandler(void)
{
    if(RTC_GET_TICK_INT_FLAG(RTC) == 1)
    {
        /* Clear RTC tick interrupt flag */
        RTC_CLEAR_TICK_INT_FLAG(RTC);
        
        g_u32RTCTInt ++;
        
        PA2 ^= 1;
    }
}

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
    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(RTC0_MODULE);

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();
    
    SET_GPIO_PA2();
    

    
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
    S_RTC_TIME_DATA_T sInitTime, sCurTime;
    S_RTC_TIME_DATA_T *sptrInitTime;
    uint32_t u32Sec;
    uint8_t u8IsNewDateTime = 0;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART for printf */
    UART_Init();

    /* Lock protected registers */
    SYS_LockReg();

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
    printf("+-----------------------------------------+\n");
    printf("|    RTC Date/Time and Tick Sample Code   |\n");
    printf("+-----------------------------------------+\n\n");

    /* Set LXT as RTC clock source */
    RTC_SetClockSource(RTC_CLOCK_SOURCE_LXT);
    
    /* Enable RTC NVIC */
    NVIC_EnableIRQ(RTC_IRQn);

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
    
    if(sptrInitTime == NULL)
    {
        RTC_GetDateAndTime(&sCurTime);
        sInitTime.u32Second = ((sCurTime.u32Second + T_5SEC) > T_60SEC) ? (sCurTime.u32Second + T_5SEC)- T_60SEC : sCurTime.u32Second + T_5SEC;
    }
    else
    {
    	sInitTime.u32Second = sInitTime.u32Second + T_5SEC;
    }

    /* Enable RTC tick interrupt, one RTC tick is 1 second */
    RTC_EnableInt(RTC_INTEN_TICKIEN_Msk);
    RTC_SetTickPeriod(RTC_TICK_1_4_SEC);

    printf("# Showing RTC date/time on UART0.\n\n");
    printf("1.) Use PA.2 to check tick period time is 1/4 or not.\n");
    printf("2.) Show RTC date/time and change date/time after 5 seconds:\n");

    /* Use PA.2 to check tick period time */
    GPIO_SetMode(PA, BIT2, GPIO_MODE_OUTPUT);
    PA2 = 1;

    u32Sec = 0;
    g_u32RTCTInt = 0;
    while(1)
    {
        if(g_u32RTCTInt == T_TICK_1_4_SEC)
        {
            g_u32RTCTInt = 0;

            /* Read current RTC date/time */
            RTC_GetDateAndTime(&sCurTime);
            printf("    %d/%02d/%02d %02d:%02d:%02d\n",
                   sCurTime.u32Year, sCurTime.u32Month, sCurTime.u32Day, sCurTime.u32Hour, sCurTime.u32Minute, sCurTime.u32Second);

            if(u32Sec == sCurTime.u32Second)
            {
                printf("\nRTC time is incorrect.\n");
                return -1;
            }

            u32Sec = sCurTime.u32Second;

            if(u8IsNewDateTime == 0)
            {
                if(u32Sec == sInitTime.u32Second)
                {
                    printf("\n");
                    printf("3.) Update new date/time to 2023/6/9 13:12:11.\n");

                    u8IsNewDateTime = 1;
                    RTC_SetDate(2023, 6, 9, RTC_WEDNESDAY);
                    RTC_SetTime(13, 12, 11, RTC_CLOCK_24, RTC_AM);
                }
            }
        }
    }
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
