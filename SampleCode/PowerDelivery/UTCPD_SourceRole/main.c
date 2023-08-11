
/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 13 $
 * $Date: 18/07/18 3:19p $
 * @brief
 *           Demonstrates Source Role Power Device
 * @note
 *
 * Copyright (c) 2022 The Chromium OS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "utcpdlib.h"

#define ADC_INIT
//#define ACMP_INIT

/*---------------------------------------------------------------------------------------------------------*/
/*                            ISR to handle UTCPD interrupt event                                          */
/*---------------------------------------------------------------------------------------------------------*/
NVT_ITCM void UTCPD_IRQHandler(void)
{
    uint32_t port = 0;
    tcpci_tcpc_alert(port);
}
void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

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
    /* Enable Internal RC 32kHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);
    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */

    //CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_HIRC);
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_HXT);;


    /* Enable GPA ~ GPJ peripheral clock */
    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOD_MODULE);
    CLK_EnableModuleClock(GPIOE_MODULE);
    CLK_EnableModuleClock(GPIOF_MODULE);
    CLK_EnableModuleClock(GPIOG_MODULE);
    CLK_EnableModuleClock(GPIOH_MODULE);
    CLK_EnableModuleClock(GPIOJ_MODULE);

    /* Debug UART clock setting*/
    SetDebugUartCLK();

    /* === Enable IP clock === */

    /* Enable TIMER 0 module clock */
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC, 0);
    //CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_LIRC, 0);

    /* Enable TIMER 1 module clock */
    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_HIRC, 0);
    //CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_LIRC, 0);

    /* Enable EADC peripheral clock */

    CLK_SetModuleClock(EADC0_MODULE, CLK_EADCSEL_EADC0SEL_PCLK0, CLK_EADCDIV_EADC0DIV(8));
    CLK_EnableModuleClock(EADC0_MODULE);

    /* Enable UTCPD clock */
    CLK_EnableModuleClock(UTCPD0_MODULE);

    /* Reset UTCPD  */
    SYS_ResetModule(SYS_UTCPD0RST);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
#if 0//TK_UTCPD_MOTOR
    SYS->GPB_MFP2 = (SYS->GPB_MFP2 & ~(SYS_GPB_MFP2_PB8MFP_Msk | SYS_GPB_MFP2_PB9MFP_Msk)) |
                    (SYS_GPB_MFP2_PB8MFP_UART0_RXD | SYS_GPB_MFP2_PB9MFP_UART0_TXD);
#else
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP3 = (SYS->GPB_MFP3 & ~(SYS_GPB_MFP3_PB12MFP_Msk | SYS_GPB_MFP3_PB13MFP_Msk)) |
                    (SYS_GPB_MFP3_PB12MFP_UART0_RXD | SYS_GPB_MFP3_PB13MFP_UART0_TXD);
#endif


    printf("PLL 64MHz --> HCLK --> CLKO / 4 = 16MHz on PB.14\n");
    /* Enable APLL0 72MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_72MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to APLL0 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0);

    /* Configure UUTCPD CC1/CC2 */
    SET_UTCPD0_CC1_PC0();
    SET_UTCPD0_CC2_PC1();

#ifdef FRS_WORKAROUND
    /* Initialize FRSCC1 and FRS_CC2 to default Low */
    SET_UTCPD0_FRSTX1_PC4();
    SET_UTCPD0_FRSTX2_PC5();
    frs_mux_selection(1, 1);
#endif


#ifdef ADC_INIT
    /* Set PB.2 - PB.3 to input mode For EADC pin to measure VBUS and VCONN */
    GPIO_SetMode(PB, BIT2, GPIO_MODE_INPUT);    //For VBUS Voltage
    GPIO_SetMode(PB, BIT3, GPIO_MODE_INPUT);    //For VCONN Voltage

    /* Configure the PB.2 - PB.3 ADC analog input pins. */
    SET_EADC0_CH2_PB2();
    SET_EADC0_CH3_PB3();
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT2 | BIT3);

#if 0  /* ADC comparator */
    GPIO_SetMode(PB, BIT4, GPIO_MODE_INPUT);    //For VBUS Over Curerent (ADC compaarator)
    GPIO_SetMode(PB, BIT0, GPIO_MODE_INPUT);    //For VCONN Over Curerent  (ADC compaarator)
    /* VBUS Over Current from EADC comparator 0 */
    SYS->GPB_MFP1 = (SYS->GPB_MFP1 & ~(SYS_GPB_MFP1_PB4MFP_Msk) |   SYS_GPB_MFP1_PB4MFP_EADC0_CH4);

    /* VCONN Over Current from EADC comparator 1*/
    SYS->GPB_MFP0 = (SYS->GPB_MFP0 & ~(SYS_GPB_MFP0_PB0MFP_Msk) |   SYS_GPB_MFP0_PB0MFP_EADC0_CH0);

    /* Disable the PB.2 - PB.3 digital input path to avoid the leakage current. */
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT2 | BIT3 | BIT4 | BIT0);
#endif
#endif

    SET_INT1_PB4();
    SET_INT0_PB5();

    /* Lock protected registers */
    //SYS_LockReg();

    /* Set GPB[1:0] multi-function pins debug port */
    SET_GPIO_PB0();
    SET_GPIO_PB1();

#ifdef ACMP_INIT
    ACMP_Pin_Open();
#endif
}

/*---------------------------------------------------------------------------------------------------------*/
/* Base timer initializtion function                                                                       */
/* The timer is used to maintain the PD protocol stack.                                                    */
/* The tick unit is 1 millisecond.                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void TIMER0_Init(boid)
{
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);
    /* Enable timer interrupt */
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TIMER0_IRQn);
    /* Start Timer 0 */
    TIMER_Start(TIMER0);
}

/*---------------------------------------------------------------------------------------------------------*/
/* VBUS/VCONN voltage conversion timer initializtion function                                              */
/* The timer is used to trigger EADC to conversion VBUS or VCONN.                                          */
/* The tick unit is 10 millisecond (depends on the system requirement)                                     */
/*---------------------------------------------------------------------------------------------------------*/
void TIMER1_Init(void)
{
    /* Reset TMR1 */
    SYS_ResetModule(SYS_TMR1RST);

    /* Set timer frequency to 100HZ */
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 100);

    /* Enable timer interrupt */
    //    TIMER_EnableInt(TIMER0);
    //    NVIC_EnableIRQ(TIMER1_IRQn);

    /* Timer0 trigger target is EADC */
    TIMER1->TRGCTL |= TIMER_TRGCTL_TRGEADC_Msk;
    /* Start Timer 0 */
    TIMER_Start(TIMER1);
}

/**
 * @brief       Timer0 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Timer0 default IRQ.
 *              Software Timer base for PD check time our mechanism
 */

NVT_ITCM void TIMER0_IRQHandler(void)
{
    UTCPD_TimerBaseInc();
    /* clear timer interrupt flag */
    TIMER_ClearIntFlag(TIMER0);
}


/**
 * @brief       Timer1 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Timer1 default IRQ.
 *              Utilizaton for driving ADC trigger conversion VBUS and VCONN to periodicly
 */
NVT_ITCM void TIMER1_IRQHandler(void)
{
    static uint32_t u32Sec = 1;

    //printf("%d sec\n", u32Sec++);

    /* clear timer interrupt flag */
    TIMER_ClearIntFlag(TIMER1);
}

/**
 * @brief       UUTCPD Callback Function
 *
 * @param       event: UUTCPD_PD_ATTACHED = 0,           : Port partner attached or disattached
 *                     UUTCPD_PD_CONTRACT = 1,           : PD contract established
 *
 *              op: 0 = event flag set
 *                  1 = event flag clear
 * @return      None
 *
 * @details     None
 *
 */
void UTCPD_Callback(int port, E_UTCPD_PD_EVENT event, uint32_t op)
{
    printf("Callback event = %d\n", event);
    printf("op = %d\n", op);
}


void pd_task(void)
{
    int port = TASK_ID_TO_PD_PORT(task_get_current());

    /*
     * If port does not exist, return
     */
    if (port >= board_get_usb_pd_port_count())
        return;

    /* Install UUTCPD Callback Function */
    UTCPD_InstallCallback(port, UTCPD_Callback);

    while (1)
    {
        //      pd_timer_init(port);
        //      pd_task_init(port);
        pd_task_reinit(port);
        /* As long as pd_task_loop returns true, keep running the loop.
         * pd_task_loop returns false when the code needs to re-init
         * the task, so once the code breaks out of the inner while
         * loop, the re-init code at the top of the outer while loop
         * will run.
         */


        while (pd_task_loop(port))
        {
            /* User Tasks:
             * Please separate User Tasks code piece by piece.
             * Suggestion not to over 100us for every piece code.
             * pd_task_loop() needs to be called every 1ms
             */
#if (CONFIG_COMMAND_SHELL == 1)
            UART_Commandshell(port);
#endif
            continue;
        }
    }
}


int main()
{
    uint32_t i, u32Data, count = 0;
    uint32_t role;
    char ch;
    int32_t port = 0;

    /* Unlock protected registers to operate FMC ISP function */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 to 115200-8n1 for print message */
    UART_Open(UART0, 115200);
    printf("UART Initial\n");

    /* VBSRCEN */
    SET_UTCPD0_VBSRCEN_PA2();
    outp32(UTCPD0_BASE + TCPC_REG_PINPL, inp32(UTCPD0_BASE + TCPC_REG_PINPL) | TCPC_REG_PINPL_SRCEN);

    /* VBSNKEN */
    outp32(UTCPD0_BASE + TCPC_REG_PINPL, inp32(UTCPD0_BASE + TCPC_REG_PINPL) | TCPC_REG_PINPL_SNKEN);
    SET_UTCPD0_VBSNKEN_PA3();


#if (CONFIG_COMMAND_SHELL == 1)
    /* Enable UART RDA interrupt for command */
    NVIC_EnableIRQ(UART0_IRQn);
    //UART_EnableInt(UART0, (UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk));
    UART_EnableInt(UART0, UART_INTEN_RDAIEN_Msk);
#endif


    printf("DRP: UART Init\n");

    /* Set timer frequency to 1000HZ for system time base */
    TIMER0_Init();

    /* VCONN Discharge: Don't force VCONN Discharge First */
    SET_GPIO_PA0();
    SET_GPIO_PA1();
    PA1 = 0;
    GPIO_SetMode(PA, BIT1, GPIO_MODE_OUTPUT);

#if 1   /* VCONN Enable: VCEN0:PA0, VCEN1:PB0, Active Low */
    /* VCEN0 PA0 */
    SET_UTCPD0_VCNEN1_PA0();
    /* VCEN1 PB0 */
    SET_UTCPD0_VCNEN2_PB0();

    //vconn_mux_selection(1, 1);        /* CC1VCENS, CC2VCENS */
    outp32(UTCPD0_BASE + UTCPD_MUXSEL, (inp32(UTCPD0_BASE + UTCPD_MUXSEL) & ~(CC2VCENS | CC1VCENS)) |
           ((1 << 24) | (1 << 28)));

    vconn_polarity_active_low();//VCENx active low
#endif


    SYS->UTCPDCTL = SYS->UTCPDCTL | 0x02;
    i2c_write32(port, NULL, UTCPD_PHYCTL, 3);

#ifdef ADC_INIT
    /* Set timer frequency to 100HZ for measuring VBUS/VCONN*/
    TIMER1_Init();
    EADC_ConfigPins();
    EADC_Init();
    //EADC_Compare_Init();
#endif

#ifdef ACMP_INIT
    ACMP_Init();
#endif

    /* Google EC need to enable interrupt */
    NVIC_EnableIRQ(UTCPD_IRQn);

    pd_task();

}



#if 0
void HardFault_Handler(void)
{
    printf("Hard Fault\n");

    while (1);
}
#else
void hard_fault_handler_c(unsigned int *hardfault_args, unsigned lr_value)
{
#if 0
    unsigned int stacked_r0;
    unsigned int stacked_r1;
    unsigned int stacked_r2;
    unsigned int stacked_r3;
    unsigned int stacked_r12;
    unsigned int stacked_lr;
    unsigned int stacked_pc;
    unsigned int stacked_psr;
    stacked_r0 = ((unsigned int) hardfault_args[0]);
    stacked_r1 = ((unsigned int) hardfault_args[1]);
    stacked_r2 = ((unsigned int) hardfault_args[2]);
    stacked_r3 = ((unsigned int) hardfault_args[3]);
    stacked_r12 = ((unsigned int)hardfault_args[4]);
    stacked_lr = ((unsigned int) hardfault_args[5]);
    stacked_pc = ((unsigned int) hardfault_args[6]);
    stacked_psr = ((unsigned int) hardfault_args[7]);

    {
        printf("[Hard fault handler]\r\n");
        printf("R0 = %x\r\n", stacked_r0);
        printf("R1 = %x\r\n", stacked_r1);
        printf("R2 = %x\r\n", stacked_r2);
        printf("R3 = %x\r\n", stacked_r3);
        printf("R12 = %x\r\n", stacked_r12);
        printf("Stacked LR = %x\r\n", stacked_lr);
        printf("Stacked PC = %x\r\n", stacked_pc);
        printf("Stacked PSR = %x\r\n", stacked_psr);
        printf("SCB_SHCSR=%x\r\n", SCB->SHCSR);
        printf("Current LR = %x\r\n", lr_value);
    }
#endif

    while (1);
}
/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
#endif /* #if 0 */