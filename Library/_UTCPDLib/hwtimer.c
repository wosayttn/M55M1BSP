/******************************************************************************
 * @file     hwtimer.c
 * @version  V1.00
 * $Revision: 13 $
 * $Date: 18/07/18 3:19p $
 * @brief    Timer Base Porting Layer
 *
 * @note
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

extern uint64_t __hw_clock_source_read64(void);

static volatile uint64_t ms_l = 0xFFFFFFF0UL;
#if 0
void TMR0_IRQHandler(void)
{
    ms_l = ms_l + 1;
    /* clear timer interrupt flag */
    TIMER_ClearIntFlag(TIMER0);
}
#else
void UTCPD_TimerBaseInc(int port)
{
    ms_l = ms_l + 1;
}
#endif

volatile uint64_t u64Us;
uint64_t __hw_clock_source_read64(void)
{
    uint32_t u32ClkSrc;
    uint32_t u32Us;
    u32ClkSrc = (uint64_t)TIMER_GetModuleClock(TIMER0);

    if (u32ClkSrc > 1000000)
    {
        u32ClkSrc = u32ClkSrc / 1000000;
        u32Us = (uint64_t)TIMER0->CNT / u32ClkSrc;  //shink to us
    }
    else
    {
        u32Us = (uint64_t)TIMER0->CNT * (1000000 / u32ClkSrc); //extended
    }

    //return u64Us;
    return ((uint64_t)ms_l * 1000 + u32Us);
}

void usdelay(uint32_t u32us)
{
    uint32_t u32Us1, u32Us2;
    u32Us1 = (uint64_t)TIMER0->CNT / 12; //shink to us

    do
    {
        u32Us2 = (uint64_t)TIMER0->CNT / 12; //shink to us
    } while (abs(u32Us2 - u32Us1) < u32us);

}

void delay(uint32_t u32ms)
{
    uint64_t u64Btime;
    u64Btime = ms_l;

    do
    {
    } while ((ms_l - u64Btime) < u32ms);
}
