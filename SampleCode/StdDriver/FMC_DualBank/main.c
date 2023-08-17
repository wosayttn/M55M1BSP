/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Show FMC dual bank capability.
 *          Program APROM bank1 while code is running on APROM bank0
 *          without being blocked.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "NuMicro.h"

#define DB_PROG_LEN         (4 * FMC_FLASH_PAGE_SIZE)  /* Background program length  */
#define CRC32_LOOP_CNT      500                        /* Loop count                 */

/*
 *  Dual bank background program state
 */
enum
{
    DB_STATE_START,                              /* Start background dual bank program       */
    DB_STATE_ERASE,                              /* Executing ISP page erase                 */
    DB_STATE_PROGRAM,                            /* Executing ISP write                      */
    DB_STATE_DONE,                               /* All queued ISP operations finished. Idle */
    DB_STATE_FAIL                                /* ISP command failed or verify error       */
};

static volatile int32_t   db_state = DB_STATE_DONE; /* dual bank background program state       */
static volatile uint32_t  db_length;                /* dual bank program remaining length       */
static volatile uint32_t  db_addr;                  /* dual bank program current flash address  */
static volatile uint32_t  g_tick_cnt;               /* timer ticks - 100 ticks per second       */

NVT_ITCM void SysTick_Handler(void)
{
    g_tick_cnt++;                                /* increase timer tick                      */

    if (db_state == DB_STATE_DONE)               /* Background program is in idle state      */
    {
        return;
    }

    if (db_length == 0)                          /* Background program done?                 */
    {
        db_state = DB_STATE_DONE;                /* enter idle state                         */
        return;
    }

    if (FMC->MPSTS & FMC_MPSTS_MPBUSY_Msk)
        return;                                  /* ISP is busy, postpone to next called     */

    /*
     *  Dual-bank background program...
     */
    switch (db_state)
    {
        case DB_STATE_START:
            if (db_addr & ~FMC_PAGE_ADDR_MASK)
            {
                printf("Warning - dual bank start address is not page aligned!\n");
                db_state = DB_STATE_FAIL;
                break;
            }

            if (db_length & ~FMC_PAGE_ADDR_MASK)
            {
                printf("Warning - dual bank length is not page aligned!\n");
                db_state = DB_STATE_FAIL;
                break;
            }

            db_state = DB_STATE_ERASE;              /* Next state is to erase flash            */
            break;

        case DB_STATE_ERASE:
            printf("Erase 0x%x [%d]\n", db_addr, g_tick_cnt);
            FMC->ISPCMD  = FMC_ISPCMD_PAGE_ERASE;   /* ISP page erase command                   */
            FMC->ISPADDR = db_addr;                 /* page address                             */
            FMC->ISPTRG  = FMC_ISPTRG_ISPGO_Msk;    /* trigger ISP page erase and no wait       */

            db_state = DB_STATE_PROGRAM;            /* Next state is to program flash           */
            break;

        case DB_STATE_PROGRAM:
            if ((db_addr & ~FMC_PAGE_ADDR_MASK) == 0)
                printf("Erase done [%d]\n", g_tick_cnt);

            FMC->ISPCMD  = FMC_ISPCMD_PROGRAM;      /* ISP word program command                 */
            FMC->ISPADDR = db_addr;                 /* word program address                     */
            FMC->ISPDAT  = db_addr;                 /* 32-bits data to be programmed            */
            FMC->ISPTRG  = FMC_ISPTRG_ISPGO_Msk;    /* trigger ISP program and no wait          */

            db_addr += 4;                           /* advance to next word                     */
            db_length -= 4;

            if ((db_addr & ~FMC_PAGE_ADDR_MASK) == 0)
            {
                /* have reached start of next page          */
                db_state = DB_STATE_ERASE;          /* next state, erase page                   */
            }

            break;

        default:
            printf("Unknown db_state state!\n");
            break;
    }
}

static void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable PLL0 180MHz clock from HIRC and switch SCLK clock source to PLL0 */
    CLK_SetBusClock(CLK_SCLKSEL_SCLKSEL_APLL0, FREQ_180MHZ);

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

uint32_t  func_crc32(uint32_t u32StartAddr, uint32_t u32ByteSize)
{
    return 0;
}

void StartTimer0(void)
{
    CLK_EnableModuleClock(TMR0_MODULE);     /* Enable TIMER0 clock          */
    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HXT, MODULE_NoMsk);
    TIMER0->CTL = 0;                        /* Disable timer                */
    TIMER0->INTSTS = (TIMER_INTSTS_TWKF_Msk | TIMER_INTSTS_TIF_Msk);  /* Clear interrupt status */
    TIMER0->CMP = 0xFFFFFE;                 /* Set maximum time             */
    TIMER0->CNT = 0;                        /* Clear timer counter          */
    /* Start TIMER0 */
    TIMER0->CTL = (11 << TIMER_CTL_PSC_Pos) | TIMER_ONESHOT_MODE | TIMER_CTL_CNTEN_Msk;
}

uint32_t  GetTimer0Counter(void)
{
    return (TIMER0->CNT & TIMER_CNT_CNT_Msk);
}

int main(void)
{
    uint32_t    u32LoopCnt;     /* Loop counter         */
    uint32_t    u32Addr;        /* Flash address        */
    uint32_t    u32TimerCnt;    /* TIMER0 counter value */

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART for print message */
    InitDebugUart();

    printf("+------------------------------------------+\n");
    printf("|    M55M1 FMC Dual Bank Sample Code       |\n");
    printf("+------------------------------------------+\n");

    SYS_UnlockReg();                /* Unlock protected registers       */
    FMC_Open();                     /* Enable FMC ISP function          */
    FMC_ENABLE_AP_UPDATE();         /* Enable FMC erase/program APROM   */

    db_state = DB_STATE_DONE;       /* Dual bank program state idle     */

    SysTick_Config(1000);
    StartTimer0();

    for (u32LoopCnt = 0; u32LoopCnt < CRC32_LOOP_CNT; u32LoopCnt++)
    {
        func_crc32(0x0, 0x10000);      /* Calculate 64KB CRC32 value, just to consume CPU time  */
    }

    u32TimerCnt = GetTimer0Counter();
    printf("u32TimerCnt: 0x%08X\n", u32TimerCnt);
    /* TIMER0->CNT is the elapsed us */
    printf("\nTime elapsed without program bank1: %d.%d seconds. Ticks: %d\n\n", u32TimerCnt / 1000000, u32TimerCnt / 1000, g_tick_cnt);

    db_addr   = FMC_APROM_BANK1_BASE;   /* Dual bank background program address */
    db_length = DB_PROG_LEN;            /* Dual bank background length          */
    db_state  = DB_STATE_START;         /* Start background dual bank program   */

    SysTick_Config(1000);
    StartTimer0();

    for (u32LoopCnt = 0; u32LoopCnt < CRC32_LOOP_CNT; u32LoopCnt++)
    {
        func_crc32(0x0, 0x10000);      /* Calculate 64KB CRC32 value, just to consume CPU time  */
    }

    u32TimerCnt = GetTimer0Counter();

    /* TIMER0->CNT is the elapsed us */
    printf("\nTime elapsed with program bank1: %d.%d seconds. Ticks: %d\n\n", u32TimerCnt / 1000000, u32TimerCnt / 1000, g_tick_cnt);

    while (db_state != DB_STATE_DONE) ;

    /*
     *  Verify ...
     */
    for (u32Addr = FMC_APROM_BANK1_BASE; u32Addr < FMC_APROM_BANK1_BASE + DB_PROG_LEN; u32Addr += 4)
    {
        if (inpw(u32Addr) != u32Addr)
        {
            printf("Flash address 0x%x verify failed ! expect: 0x%x, read: 0x%x.\n", u32Addr, u32Addr, inpw(u32Addr));
            goto lexit;
        }
    }

    printf("Verify OK.\n");

lexit:
    FMC_Close();                       /* Disable FMC ISP function  */
    SYS_LockReg();                     /* Lock protected registers  */
    printf("\nEnd of FMC Dual Bank Sample Code\n");

    /* Got no where to go, just u32LoopCnt forever */
    while (1) ;
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
