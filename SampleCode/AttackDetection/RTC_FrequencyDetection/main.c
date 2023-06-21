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
void SYS_Init(void);
void RTC_Init(void);
void UART0_Init(void);
void TIMER0_Init(void);
void GetActiveLXTandLIRC32Freq(uint32_t *u32LXTFreq, uint32_t *LIRC32Freq);

/**
 * @brief       IRQ Handler for RTC Interrupt
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The RTC_IRQHandler is default IRQ of RTC, declared in startup_M55M1.c.
 */
void RTCTAMPER_IRQHandler(void)
{
    uint32_t u32Reg;

    u32Reg = RTC->INTSTS;

    if (u32Reg & RTC_INTSTS_CLKFIF_Msk)
    {
        printf("LXT clock frequency monitor fail interrupt is happened!\n");
        printf("LXT frequency is abnormal! RTC clock is switched to LIRC32.\n\n");

        /* Disable LXT clock frequency monitor fail interrupt */
        RTC->INTEN &= ~RTC_INTEN_CLKFIEN_Msk;

        /* Write 1 to clear LXT Clock frequency monitor fail interrupt */
        RTC->INTSTS = RTC_INTSTS_CLKFIF_Msk;
    }

    if (u32Reg & RTC_INTSTS_CLKSTIF_Msk)
    {
        printf("LXT clock frequency monitor stop interrupt is happened!\n");
        printf("LXT frequency is abnormal! RTC clock is switched to LIRC32.\n\n");

        /* Disable LXT clock frequency monitor stop interrupt */
        RTC->INTEN &= ~RTC_INTEN_CLKSTIEN_Msk;

        /* Write 1 to clear LXT Clock frequency monitor stop interrupt */
        RTC->INTSTS = RTC_INTSTS_CLKSTIF_Msk;
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
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(RTC0_MODULE);

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /* Set PC multi-function pins for CLKO(PC.13) */
    SET_CLKO_PC13();
}
/*---------------------------------------------------------------------------------------------------------*/
/* Init RTC clock and clear interrupt flag status                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void RTC_Init(void)
{
    /* Back to RTC initial setting */
    RTC->CLKDCTL &= ~(RTC_CLKDCTL_LXTFDEN_Msk | RTC_CLKDCTL_LXTFSW_Msk | RTC_CLKDCTL_LXTSTSW_Msk);
    RTC->LXTCTL &= ~(RTC_LXTCTL_LIRC32KEN_Msk | RTC_LXTCTL_C32KSEL_Msk);
    RTC->INTSTS = RTC_INTSTS_CLKFIF_Msk | RTC_INTSTS_CLKSTIF_Msk;
}
/*---------------------------------------------------------------------------------------------------------*/
/* Init UART                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void UART0_Init(void)
{
    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}
/*---------------------------------------------------------------------------------------------------------*/
/* Init TIMER0                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
void TIMER0_Init(void)
{
    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC, 0);

    CLK_EnableModuleClock(TMR0_MODULE);

    TIMER0->CMP = 0xFFFFFF;
    TIMER0->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk | (12 - 1);
}
/*---------------------------------------------------------------------------------------------------------*/
/* Get LXT and LIRC Frequency                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void GetActiveLXTandLIRC32Freq(uint32_t *u32LXTFreq, uint32_t *LIRC32Freq)
{
    uint32_t u32GetCNT;

    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC, 0);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_LXT, 0);

    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);

    TIMER0->CMP = __HIRC;
    TIMER0->CTL = 0;

    TIMER1->CMP = 0xFFFFFF;
    TIMER1->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk;

    /* LXT source from external LXT */
    RTC->LXTCTL &= ~(RTC_LXTCTL_RTCCKSEL_Msk | RTC_LXTCTL_LIRC32KEN_Msk | RTC_LXTCTL_C32KSEL_Msk);
    CLK_SysTickDelay(10000);
    TIMER0->INTSTS = 0x3;
    TIMER1->CNT = 0x555; //reset counter value
    printf("BP\r\n");

    while (1)
    {
        if (TIMER1->CNT == 10)
        {
            TIMER0->CTL = TIMER_CTL_CNTEN_Msk;
            break;
        }
    }

    while (TIMER0->INTSTS == 0);

    u32GetCNT = TIMER1->CNT;
    *u32LXTFreq = u32GetCNT - 10;

    /* LXT source from LIRC32 */
    RTC->LXTCTL = (RTC->LXTCTL & ~RTC_LXTCTL_RTCCKSEL_Msk) | ((RTC_LXTCTL_LIRC32KEN_Msk | RTC_LXTCTL_C32KSEL_Msk));
    CLK_SysTickDelay(10000);
    TIMER0->INTSTS = 0x3;
    TIMER1->CNT = 0x555; //reset counter value

    while (1)
    {
        if (TIMER1->CNT == 10)
        {
            TIMER0->CTL = TIMER_CTL_CNTEN_Msk;
            break;
        }
    }

    while (TIMER0->INTSTS == 0);

    u32GetCNT = TIMER1->CNT;
    *LIRC32Freq = u32GetCNT - 10;

    /* LXT source from external LXT */
    RTC->LXTCTL &= ~(RTC_LXTCTL_RTCCKSEL_Msk | RTC_LXTCTL_LIRC32KEN_Msk | RTC_LXTCTL_C32KSEL_Msk);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    uint32_t u32LXTFreq, u32LIRC32Freq, u32STDCount;
    int32_t i32Option;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init RTC */
    RTC_Init();

    /* Init UART0 for printf */
    UART0_Init();

    /* Init TIMER0 */
    TIMER0_Init();

    printf("\n\nCPU @ %uHz\n", SystemCoreClock);
    printf("+-------------------------------------------------------------+\n");
    printf("|             RTC Frequency Detection Sample Code             |\n");
    printf("+-------------------------------------------------------------+\n");
    printf("| LXT clock frequency monitor fail/stop interrupt will happen |\n");
    printf("| if LXT clock frequency isn't oscillating in normal range.   |\n");
    printf("| User can switch RTC clock source from LXT to LIRC32.        |\n");
    printf("+-------------------------------------------------------------+\n");

    GetActiveLXTandLIRC32Freq(&u32LXTFreq, &u32LIRC32Freq);

    if (u32LXTFreq > u32LIRC32Freq)
    {
        u32STDCount = ((1000000000 / u32LXTFreq) * 255) / (1000000000 / u32LIRC32Freq);
        printf("LXT is faster than LIRC32. [%uHz > %uHz]\n", u32LXTFreq, u32LIRC32Freq);
    }
    else
    {
        u32STDCount = ((1000000000 / u32LIRC32Freq) * 255) / (1000000000 / u32LXTFreq);
        printf("LIRC32 is faster than LXT. [%uHz > %uHz]\n", u32LIRC32Freq, u32LXTFreq);
    }

    /* Enable clock output, select CLKO clock source as LXT and set clock output frequency is LXT.
       RTC clock source will be switched to LIRC32 if LXT frequency isn't oscillating in normal range.
       You can check if RTC clock source is switched to LIRC32 by clock output pin output frequency.
    */

    /* Output selected clock to CKO, CKO Clock = LXT / 1 */
    CLK_EnableCKO(CLK_CLKOSEL_CLKOSEL_LXT, 0, 1);

    /* Lock protected registers */
    SYS_LockReg();

    /* Set the LXT clock frequency monitor fail/stop boundary value.
       The fail/stop boundary value should be less than u32STDCount.
    */
    RTC->CDBR = ((u32STDCount - 5) << RTC_CDBR_FAILBD_Pos) | ((u32STDCount - 5) << RTC_CDBR_STOPBD_Pos);

    /* Set clock fail/stop detector function enabled */
    RTC->CLKDCTL = RTC_CLKDCTL_LXTFDEN_Msk;

    /* Set clock fail/stop detector switch LIRC32K enabled */
    RTC->CLKDCTL |= RTC_CLKDCTL_LXTFSW_Msk | RTC_CLKDCTL_LXTSTSW_Msk;

    /* Enable LIRC32K source */
    RTC->LXTCTL |= RTC_LXTCTL_LIRC32KEN_Msk;

    /* Clock frequency monitor fail/stop interrupt enabled */
    RTC->INTEN = RTC_INTEN_CLKFIEN_Msk | RTC_INTEN_CLKSTIEN_Msk;

    /* Enable RTC tamper detection interrupt */
    NVIC_EnableIRQ(RTCTAMPER_IRQn);

    printf("\nSelect:\n");
    printf("    [0] Smaller frequency deviation monitoring\n");
    printf("    [1] Larger frequency deviation monitoring\n");

    i32Option = getchar();
    printf("\n");
    printf("Select item [%c]\n", i32Option);

    if (i32Option == '0')
    {
        printf("Modify LXT fail boundary to detect FAIL flag.\n\n");

        /* Modify the LXT clock frequency monitor fail boundary value to detect FAIL flag.
           The fail boundary value should be more than u32STDCount.
        */
        RTC->CDBR = ((u32STDCount + 1) << RTC_CDBR_FAILBD_Pos);
    }
    else if (i32Option == '1')
    {
        printf("Stop LXT to detect FAIL and STOP flag.\n\n");
    }

    /* Wait for clock frequency monitor fail/stop detector interrupt happened */
    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/