/*
 * FreeRTOS Kernel V10.5.1
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/* Scheduler includes. */

#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "NuMicro.h"

/**
 * @brief Constants required to manipulate the NVIC.
 */
#define portNVIC_SYSTICK_CTRL_REG             ( *( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG             ( *( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG    ( *( ( volatile uint32_t * ) 0xe000e018 ) )
#define portNVIC_SHPR3_REG                    ( *( ( volatile uint32_t * ) 0xe000ed20 ) )
#define portNVIC_SYSTICK_ENABLE_BIT           ( 1UL << 0UL )
#define portNVIC_SYSTICK_INT_BIT              ( 1UL << 1UL )
#define portNVIC_SYSTICK_CLK_BIT              ( 1UL << 2UL )
#define portNVIC_SYSTICK_COUNT_FLAG_BIT       ( 1UL << 16UL )
#define portNVIC_PEND_SYSTICK_CLEAR_BIT       ( 1UL << 25UL )
#define portNVIC_PEND_SYSTICK_SET_BIT         ( 1UL << 26UL )
#define portMIN_INTERRUPT_PRIORITY            ( 255UL )
#define portNVIC_PENDSV_PRI                   ( portMIN_INTERRUPT_PRIORITY << 16UL )
#define portNVIC_SYSTICK_PRI                  ( portMIN_INTERRUPT_PRIORITY << 24UL )
/*-----------------------------------------------------------*/


#if( configUSE_TICKLESS_IDLE == 1 )

/*
 * The number of SysTick increments that make up one tick period.
 */
static uint32_t ulTimerCountsForOneTick = 0;

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
//static uint32_t xMaximumPossibleSuppressedTicks = 0;

/*
 * Compensate for the CPU cycles that pass while the SysTick is stopped (low
 * power functionality only.
 */
static uint32_t ulStoppedTimerCompensation = 0;

extern S_RTC_TIME_DATA_T sWriteRTC, sReadRTC;
extern volatile uint8_t g_u8IsRTCAlarmINT, g_u8IsGPIOINT;
extern void PowerDownFunction(void);

extern volatile uint32_t g_u32WriteRTC_Ticks, g_u32ReadRTC_Ticks;

#define RTC_TIME_HZCNT_Pos               (24)                                              /*!< RTC_T::TIME: HZCNT Position            */
#define RTC_TIME_HZCNT_Msk               (0x7ful << RTC_TIME_HZCNT_Pos)                    /*!< RTC_T::TIME: HZCNT Mask                */

#define RTC_TALM_HZCNT_Pos               (24)                                              /*!< RTC_T::TALM: HZCNT Position            */
#define RTC_TALM_HZCNT_Msk               (0x7ful << RTC_TALM_HZCNT_Pos)                    /*!< RTC_T::TALM: HZCNT Mask                */

static void TimeAdd(S_RTC_TIME_DATA_T* t, volatile uint32_t *pu32RtcTicks, uint32_t u32rtcTicks)
{
    uint32_t h, m, s, tick;
    int32_t i32Next;

    m = u32rtcTicks / 128 / 60;
    h = m / 60;
    m = m % 60;
    s = u32rtcTicks / 128 % 60;
    tick = u32rtcTicks & 0x7f;

    *pu32RtcTicks += tick;
    t->u32Second += s;
    t->u32Minute += m;
    t->u32Hour += h;

    do
    {
        i32Next = 0;

        if(*pu32RtcTicks >= 128)
        {
            *pu32RtcTicks -= 128;
            t->u32Second += 1;
            i32Next = 1;
        }

        if(t->u32Second >= 60)
        {
            t->u32Second -= 60;
            t->u32Minute += 1;
            i32Next = 1;
        }

        if(t->u32Minute >= 60)
        {
            t->u32Minute -= 60;
            t->u32Hour += 1;
            i32Next = 1;
        }

        if(t->u32Hour >= 24)
        {
            t->u32Hour -= 24;
            t->u32Minute += 1;
            i32Next = 1;
        }
    } while(i32Next);
}

static TickType_t SleepProcess(TickType_t x)
{
    if(x == 0)
        return x;

    /* Set RTC alarm accord to x */

    /* Get start time from RTC */
    RTC_GetDateAndTime(&sReadRTC);
    g_u32ReadRTC_Ticks = ((RTC->TIME & RTC_TIME_HZCNT_Msk) >> RTC_TIME_HZCNT_Pos);

    /* Backup RTC start time for GPIO wake up */
    memcpy(&sWriteRTC, &sReadRTC, sizeof(S_RTC_TIME_DATA_T));
    g_u32WriteRTC_Ticks = g_u32ReadRTC_Ticks;

    /* Calculate idle end time for RTC alarm wake up */
    TimeAdd(&sReadRTC, &g_u32ReadRTC_Ticks, x * 128 / 1000 - 1);

    /* Set wake up alarm */
    RTC_SetAlarmDateAndTime(&sReadRTC);
    RTC->TALM |= (RTC->TALM & (~RTC_TALM_HZCNT_Msk)) | (g_u32ReadRTC_Ticks << RTC_TALM_HZCNT_Pos);

    /* Enable RTC alarm interrupt and wake-up function will be enabled also */
    RTC_EnableInt(RTC_INTEN_ALMIEN_Msk);
    GPIO_CLR_INT_FLAG(PB, BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
    NVIC_EnableIRQ(GPB_IRQn);
    g_u8IsGPIOINT = 0;
    PowerDownFunction();

    /* Wake up now. Enable interrupt for process the pending interrupt handler */
    __enable_irq();
    __ISB();
    __DSB();

    /* Just make sure RTC Alarm event to continue */
    while(g_u8IsRTCAlarmINT == 0);
    g_u8IsRTCAlarmINT = 0;

    __disable_irq();
    RTC_DisableInt(RTC_INTEN_ALMIEN_Msk);
    NVIC_DisableIRQ(GPB_IRQn);

    portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_ENABLE_BIT;

    /* Set the new reload value. */
    portNVIC_SYSTICK_LOAD_REG = 0;

    /* Clear the SysTick count flag and set the count value back to
     * zero. */
    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;

    /* Restart SysTick. */
    portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

    return x;
}

void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
    uint32_t ulReloadValue, ulCompleteTickPeriods;

    /* Stop the SysTick momentarily. The time the SysTick is stopped for
     * is accounted for as best it can be, but using the tickless mode will
     * inevitably result in some tiny drift of the time maintained by the
     * kernel with respect to calendar time. */
    portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_ENABLE_BIT;

    /* Calculate the reload value required to wait xExpectedIdleTime
     * tick periods. -1 is used because this code will execute part way
     * through one of the tick periods. */
    ulReloadValue = portNVIC_SYSTICK_CURRENT_VALUE_REG + ( ulTimerCountsForOneTick * ( xExpectedIdleTime - 1UL ) );
    if( ulReloadValue > ulStoppedTimerCompensation )
    {
        ulReloadValue -= ulStoppedTimerCompensation;
    }

    /* Enter a critical section but don't use the taskENTER_CRITICAL()
     * method as that will mask interrupts that should exit sleep mode. */
    __disable_irq();
    __DSB();
    __ISB();

    /* If a context switch is pending or a task is waiting for the scheduler
     * to be unsuspended then abandon the low power entry. */
    if( eTaskConfirmSleepModeStatus() == eAbortSleep )
    {
        /* Restart from whatever is left in the count register to complete
         * this tick period. */
        portNVIC_SYSTICK_LOAD_REG = portNVIC_SYSTICK_CURRENT_VALUE_REG;

        /* Restart SysTick. */
        portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

        /* Reset the reload register to the value required for normal tick
         * periods. */
        portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;

        /* Re-enable interrupts - see comments above the cpsid instruction()
         * above. */
        __enable_irq();
    }
    else
    {
        /* Set the new reload value. */
        portNVIC_SYSTICK_LOAD_REG = ulReloadValue;

        /* Clear the SysTick count flag and set the count value back to
         * zero. */
        portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;

        /* Restart SysTick. */
        portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

        /* Sleep until something happens. */
        xExpectedIdleTime = SleepProcess(xExpectedIdleTime);

        /* Re-enable interrupts to allow the interrupt that brought the MCU
         * out of sleep mode to execute immediately. See comments above
         * __disable_irq() call above. */
        __enable_irq();
        __DSB();
        __ISB();

        /* Disable interrupts again because the clock is about to be stopped
         * and interrupts that execute while the clock is stopped will increase
         * any slippage between the time maintained by the RTOS and calendar
         * time. */
        __disable_irq();
        __DSB();
        __ISB();

        /* Disable the SysTick clock without reading the
         * portNVIC_SYSTICK_CTRL_REG register to ensure the
         * portNVIC_SYSTICK_COUNT_FLAG_BIT is not cleared if it is set. Again,
         * the time the SysTick is stopped for is accounted for as best it can
         * be, but using the tickless mode will inevitably result in some tiny
         * drift of the time maintained by the kernel with respect to calendar
         * time */
        portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT );

        /* Determine if the SysTick clock has already counted to zero and
         * been set back to the current reload value (the reload back being
         * correct for the entire expected idle time) or if the SysTick is yet
         * to count to zero (in which case an interrupt other than the SysTick
         * must have brought the system out of sleep mode). */
        if( ( portNVIC_SYSTICK_CTRL_REG & portNVIC_SYSTICK_COUNT_FLAG_BIT ) != 0 )
        {
            uint32_t ulCalculatedLoadValue;

            /* The tick interrupt is already pending, and the SysTick count
             * reloaded with ulReloadValue. Reset the
             * portNVIC_SYSTICK_LOAD_REG with whatever remains of this tick
             * period. */
            ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL ) - ( ulReloadValue - portNVIC_SYSTICK_CURRENT_VALUE_REG );

            /* Don't allow a tiny value, or values that have somehow
             * underflowed because the post sleep hook did something
             * that took too long. */
            if( ( ulCalculatedLoadValue < ulStoppedTimerCompensation ) || ( ulCalculatedLoadValue > ulTimerCountsForOneTick ) )
            {
                ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL );
            }

            portNVIC_SYSTICK_LOAD_REG = ulCalculatedLoadValue;

            /* As the pending tick will be processed as soon as this
             * function exits, the tick value maintained by the tick is stepped
             * forward by one less than the time spent waiting. */
            ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
        }

        /* Restart SysTick so it runs from portNVIC_SYSTICK_LOAD_REG
         * again, then set portNVIC_SYSTICK_LOAD_REG back to its standard
         * value. */
        portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
        portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;
        vTaskStepTick( ulCompleteTickPeriods );
        portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;

        /* Exit with interrpts enabled. */
        __enable_irq();
    }
}

#endif /* configUSE_TICKLESS_IDLE */
