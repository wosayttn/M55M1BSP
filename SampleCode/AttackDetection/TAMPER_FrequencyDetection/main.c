/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Show the usage of LXT clock frequency monitor function.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Functions and variables declaration                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define TIMER1_DELAY_COUNTER            (10)
#define TAMPER_CLK_FAIL_BOUNDARY        (2)
#define TAMPER_CLK_STOP_BOUNDARY        (2)

static volatile uint32_t s_u32SpareData = 0;

void SYS_Init(void);
void UART_Init(void);
void GetActiveLXTandTLIRC32Freq(uint32_t *u32LXTFreq, uint32_t *LIRC32Freq);

/**
 * @brief       IRQ Handler for TAMPER Interrupt
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The TAMPER_IRQHandler is default IRQ of TAMPER, declared in startup_M55M1.c.
 */
NVT_ITCM void TAMPER_IRQHandler(void)
{
    uint32_t u32FlagStatus;

    /* Tamper interrupt occurred */
    if (TAMPER_GET_INT_FLAG())
    {
        /* Get tamper interrupt status */
        u32FlagStatus = TAMPER_GET_INT_STATUS();

        if (u32FlagStatus & TAMPER_INTSTS_CLKFAILIF_Msk)
        {
            printf("LXT frequency is abnormal!\n\n");

            /* Disable LXT clock frequency monitor fail interrupt */
            TAMPER_DisableInt(TAMPER_INTEN_CLKFIEN_Msk);

            /* Clear tamper interrupt status */
            TAMPER_CLR_INT_STATUS(u32FlagStatus);
        }

        if (u32FlagStatus & TAMPER_INTSTS_CLKSTOPIF_Msk)
        {
            printf("LXT frequency is almost stopped!\n\n");

            /* Disable LXT clock frequency monitor stop interrupt */
            TAMPER_DisableInt(TAMPER_INTEN_CLKSTOPIEN_Msk);

            /* Clear tamper interrupt status */
            TAMPER_CLR_INT_STATUS(u32FlagStatus);
        }

        /* Check spare register data */
        s_u32SpareData = RTC_READ_SPARE_REGISTER(RTC, 0);
        printf(" SPARE_REGISTER[%d] = 0x%x.\n\n", 0, s_u32SpareData);
    }
}
/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void)
{
    /* Set X32_OUT(PF.4) and X32_IN(PF.5)*/
    SET_X32_IN_PF5();
    SET_X32_OUT_PF4();

    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);
    /* Enable Internal RC 32KHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);
    /* Enable External RC 32KHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_LXTEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    /* Waiting for Low speed Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);
    /* Waiting for Low speed External RC clock ready */
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
    /* set power level to level 1 */
    PMC_SetPowerLevel(PMC_PLCTL_PLSEL_PL1);

    /* Enable module clock */
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK_EnableModuleClock(GPIOD_MODULE);
    CLK_EnableModuleClock(TAMPER0_MODULE);
    CLK_EnableModuleClock(FMC0_MODULE);
    CLK_EnableModuleClock(ISP0_MODULE);
    CLK_EnableModuleClock(RTC0_MODULE);

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /* set PD.1 as GPIO input mode */
    SET_GPIO_PD1();
    GPIO_SetMode(PD, BIT1, GPIO_MODE_INPUT);

    /* Set PC multi-function pins for CLKO(PC.13) */
    SET_CLKO_PC13();
}
/*---------------------------------------------------------------------------------------------------------*/
/* Init UART                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void UART_Init(void)
{
    /* Configure UART0 and set UART0 Baudrate */
    //UART_Open(UART0, 115200);
    DEBUG_PORT->BAUD = (UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(153600, 38400));
    DEBUG_PORT->LINE = (UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1);
}
/*---------------------------------------------------------------------------------------------------------*/
/* Get LXT and LIRC Frequency                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void GetActiveLXTandTLIRC32Freq(uint32_t *u32LXTFreq, uint32_t *LIRC32Freq)
{
    uint32_t u32GetCNT, u32Period;

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC, 0);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_LXT, 0);

    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);

    /* Timer0 config*/
    TIMER_SET_CMP_VALUE(TIMER0, __HIRC);
    TIMER_SET_OPMODE(TIMER0, TIMER_ONESHOT_MODE);
    TIMER_Stop(TIMER0);
    /* Timer1 config*/
    TIMER_SET_CMP_VALUE(TIMER1, 0xFFFFFF);
    TIMER_SET_OPMODE(TIMER1, TIMER_PERIODIC_MODE);
    TIMER_Start(TIMER1);
    /* clear TIMER0 interrupt flag*/
    TIMER_ClearIntFlag(TIMER0);
    /* reset TIMER1 counter*/
    TIMER_ResetCounter(TIMER1);

    while (1)
    {
        if (TIMER_GetCounter(TIMER1) == TIMER1_DELAY_COUNTER)
        {
            TIMER_Start(TIMER0);
            break;
        }
    }

    while (TIMER_GetIntFlag(TIMER0) == 0);

    u32GetCNT = TIMER_GetCounter(TIMER1);
    *u32LXTFreq = u32GetCNT - TIMER1_DELAY_COUNTER;

    /* Feeds TLIRC32 CKO to PD.0 (set PD.1 as GPIO input mode) */
    /* clear TIMER0 interrupt flag*/
    TIMER_ClearIntFlag(TIMER0);
    TIMER_ClearWakeupFlag(TIMER0);
    /* reset TIMER1 counter*/
    TIMER_ResetCounter(TIMER1);
    CLK_SysTickDelay(10000);

    while (PD1 == 0) {}

    while (PD1 == 1) {}

    TIMER_SET_CMP_VALUE(TIMER0, 0xFFFFFF);

    while (PD1 == 0) {}

    while (PD1 == 1) {}

    u32Period = TIMER_GetCounter(TIMER0);
    *LIRC32Freq = (1000000 * 12) / u32Period;
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    uint32_t u32LXTFreq, u32LIRC32Freq, u32STDCount, u32FailBoundary, u32StopBoundary;
    uint32_t u32Confi3Read;
    S_RTC_TIME_DATA_T sInitTime;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART for printf */
    UART_Init();

    /* Enable FMC ISP function */
    FMC_Open();

    /* Enable APROM update function */
    FMC_ENABLE_AP_UPDATE();

    /* Enable User Configuration update function */
    FMC_ENABLE_CFG_UPDATE();

    /* Enable Tamper Domain */
    u32Confi3Read = FMC_Read(FMC_USER_CONFIG_3);

    printf("CONFI3=0x%8X\n", u32Confi3Read);

    if (u32Confi3Read == 0x5aa5ffff)
    {
        FMC_ENABLE_ISP();
        FMC_ENABLE_CFG_UPDATE();
        FMC_WriteConfig(FMC_USER_CONFIG_3, 0x5a5affff);
        SYS_ResetChip();

        while (1);
    }

    /* Output selected clock to CKO, CKO Clock = LIRC / 1 */
    CLK_EnableCKO(CLK_CLKOSEL_CLKOSEL_LXT, 0, 1);

    printf("\n\nCPU @ %uHz\n", SystemCoreClock);
    printf("+--------------------------------------------+\n");
    printf("|   TAMPER Frequency Detection Sample Code   |\n");
    printf("+--------------------------------------------+\n\n");
    printf("Please connect PC.13 and PD.1 to get LIRC32 frequency.\n");
    printf("The spare register data will be cleared to zero if LXT frequency is abnormal.\n");
    printf("\nStop LXT to test.\n\n");

    //GetActiveLXTandTLIRC32Freq(&u32LXTFreq, &u32LIRC32Freq);
    u32LXTFreq = 27000;
    u32LIRC32Freq = 32000;
    getchar();

    if (u32LXTFreq > u32LIRC32Freq)
    {
        u32STDCount = ((1000000000 / u32LXTFreq) * 255) / (1000000000 / u32LIRC32Freq);

        printf("LXT is faster than TLIRC32. [%uHz > %uHz]\n", u32LXTFreq, u32LIRC32Freq);
    }
    else
    {
        u32STDCount = ((1000000000 / u32LIRC32Freq) * 255) / (1000000000 / u32LXTFreq);

        printf("TLIRC32 is faster than LXT. [%uHz > %uHz]\n", u32LIRC32Freq, u32LXTFreq);
    }

    /* Lock protected registers */
    SYS_LockReg();

    /* Reset tamper coreblock */
    TAMPER_CORE_RESET();
    TAMPER_CORE_RELEASE();

    /* Set the LXT clock frequency monitor fail/stop boundary value.
       The fail/stop boundary value should be less than u32STDCount.
    */
    u32FailBoundary = u32STDCount - TAMPER_CLK_FAIL_BOUNDARY;
    u32StopBoundary = u32STDCount - TAMPER_CLK_STOP_BOUNDARY;

    TAMPER_SET_CLKFAIL_BOUNDARY(u32FailBoundary);
    TAMPER_SET_CLKSTOP_BOUNDARY(u32StopBoundary);

    /* Enable LXT clock detection */
    TAMPER_ENABLE_LXTDET();

    /* Clear LXT clock frequency monitor fail/stop interrupt flag */
    TAMPER_CLR_INT_STATUS(TAMPER_INTSTS_CLKFAILIF_Msk | TAMPER_INTSTS_CLKSTOPIF_Msk);

    /* Enable LXT clock frequency monitor fail/stop interrupt */
    TAMPER_EnableInt(TAMPER_INTEN_CLKFIEN_Msk | TAMPER_INTEN_CLKSTOPIEN_Msk);
    NVIC_EnableIRQ(TAMPER_IRQn);

    /* Enable to clear RTC spare register */
    TAMPER_ENABLE_RTCSPCLR();

    /* Open RTC and start counting */
    sInitTime.u32Year       = 2020;
    sInitTime.u32Month      = 6;
    sInitTime.u32Day        = 1;
    sInitTime.u32Hour       = 12;
    sInitTime.u32Minute     = 30;
    sInitTime.u32Second     = 0;
    sInitTime.u32DayOfWeek  = RTC_MONDAY;
    sInitTime.u32TimeScale  = RTC_CLOCK_24;

    if (RTC_Open(&sInitTime) != 0)
    {
        printf("\n RTC initial fail!!");
        printf("\n Please check h/w setting!!");

        return -1;
    }

    /* Enable spare register */
    RTC_EnableSpareAccess();
    /* Write spare register */
    RTC_WRITE_SPARE_REGISTER(RTC, 0, 0x12345678);

    /* Check spare register data */
    s_u32SpareData = RTC_READ_SPARE_REGISTER(RTC, 0);

    printf("# SPARE_REGISTER[%d] = 0x%x.\n\n", 0, s_u32SpareData);

    /* Wait for LXT clock frequency monitor fail/stop interrupt happened */
    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/