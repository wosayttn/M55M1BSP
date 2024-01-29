/*---------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/* Copyright (C) 2020 Nuvoton Technology Corp.                                                             */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   uart_cunit_test.c                                                                                    */
/*            The test function of VIC for CUnit.                                                         */
/* Project:   M253                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"
#include "uart_cunit.h"



#define UART_CH_NUM_MAX 10
#define LIN_CH_NUM_MAX  2

extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int suite_success_init(void)
{
    return 0;
}
int suite_success_clean(void)
{
    return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function: function_name                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               param - [in/out], parameter_description                                                   */
/*               param - [in/out], parameter_description                                                   */
/*                                                                                                         */
/* Returns:      the_value                                                                                 */
/* Side effects: the_effect                                                                                */
/* Description:                                                                                            */
/*               description                                                                               */
/*---------------------------------------------------------------------------------------------------------*/

CU_SuiteInfo UART_suites[] =
{
    {"UART API", suite_success_init, suite_success_clean, NULL, NULL, UART_ApiTests},
    {"UART MACRO", suite_success_init, suite_success_clean, NULL, NULL, UART_MacroTests},
    CU_SUITE_INFO_NULL
};


UART_T *UartCh[10] =
{
    UART0,  //skip debug port UART0 test
    UART1,
    UART2,
    UART3,
    UART4,
    UART5,
    UART6,
    UART7,
    UART8,
    UART9,
};

void UART_IPReset(UART_T *psUART)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    switch ((uint32_t)psUART)
    {
        case (uint32_t)UART0:
            SYS_ResetModule(SYS_UART0RST);  // Reset UART0 IP
            break;

        case (uint32_t)UART1:
            SYS_ResetModule(SYS_UART1RST);  // Reset UART1 IP
            break;

        case (uint32_t)UART2:
            SYS_ResetModule(SYS_UART2RST);  // Reset UART2 IP
            break;

        case (uint32_t)UART3:
            SYS_ResetModule(SYS_UART3RST);  // Reset UART3 IP
            break;

        case (uint32_t)UART4:
            SYS_ResetModule(SYS_UART4RST);  // Reset UART4 IP
            break;

        case (uint32_t)UART5:
            SYS_ResetModule(SYS_UART5RST);  // Reset UART5 IP
            break;

        case (uint32_t)UART6:
            SYS_ResetModule(SYS_UART6RST);  // Reset UART6 IP
            break;

        case (uint32_t)UART7:
            SYS_ResetModule(SYS_UART7RST);  // Reset UART7 IP
            break;

        case (uint32_t)UART8:
            SYS_ResetModule(SYS_UART8RST);  // Reset UART8 IP
            break;

        case (uint32_t)UART9:
            SYS_ResetModule(SYS_UART9RST);  // Reset UART9 IP
            break;
    }

    /* Lock protected registers */
    SYS_LockReg();
}

void TestFunc_UART_EnableFlowCtrl()
{
    uint8_t u8UartChIdx;    //UART channel index

    /* UART 4 is  not support the Auto Flow Control*/
    for (u8UartChIdx = 1; u8UartChIdx < (UART_CH_NUM_MAX - 1) ; u8UartChIdx++)
    {
        UART_EnableFlowCtrl(UartCh[u8UartChIdx]);
        CU_ASSERT((UartCh[u8UartChIdx]->INTEN & (UART_INTEN_ATORTSEN_Msk | UART_INTEN_ATOCTSEN_Msk))
                  == (UART_INTEN_ATORTSEN_Msk | UART_INTEN_ATOCTSEN_Msk));
        CU_ASSERT((UartCh[u8UartChIdx]->MODEM & UART_MODEM_RTSACTLV_Msk) == UART_MODEM_RTSACTLV_Msk);
        CU_ASSERT((UartCh[u8UartChIdx]->MODEMSTS & UART_MODEMSTS_CTSACTLV_Msk) == UART_MODEMSTS_CTSACTLV_Msk);
        UART_DisableFlowCtrl(UartCh[u8UartChIdx]);
        CU_ASSERT((UartCh[u8UartChIdx]->INTEN & (UART_INTEN_ATORTSEN_Msk | UART_INTEN_ATOCTSEN_Msk)) == 0);
    }
}

const uint32_t au32UartINTEnSel[12] =
{
    UART_INTEN_RDAIEN_Msk,       //0 : Receive data available interrupt
    UART_INTEN_THREIEN_Msk,      //1 : Transmit holding register empty interrupt
    UART_INTEN_RLSIEN_Msk,       //2 : Receive line status interrupt
    UART_INTEN_MODEMIEN_Msk,     //3 : Modem status interrupt
    UART_INTEN_RXTOIEN_Msk,      //4 : Rx time-out interrupt
    UART_INTEN_BUFERRIEN_Msk,    //5 : Buffer error interrupt
    UART_INTEN_LINIEN_Msk,       //6 : LIN bus interrupt
    UART_INTEN_WKIEN_Msk,        //7 : Wake-up interrupt
    UART_INTEN_TOCNTEN_Msk,      //8 : Time-out interrupt
    UART_INTEN_ABRIEN_Msk,       //9 : Auto baud-rate interrupt
    UART_INTEN_TXENDIEN_Msk,     //10: Tx end interrupt
    UART_INTEN_SWBEIEN_Msk       //11: Single-wire Bit Error Detection Interrupt
};

void TestFunc_UART_EnableInt()
{
    uint8_t u8UartChIdx;    //UART channel index
    uint8_t u8UartINTEnIdx; //bit

    /* test loop */
    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        //enable interrupt
        for (u8UartINTEnIdx = 0; u8UartINTEnIdx < 12; u8UartINTEnIdx++)
        {
            //skip THRE interrupt
            if (u8UartINTEnIdx == 1 || (u8UartChIdx >= 2  && u8UartINTEnIdx == 6)) continue;

            UART_EnableInt((UART_T *)UartCh[u8UartChIdx], au32UartINTEnSel[u8UartINTEnIdx]);
            CU_ASSERT((UartCh[u8UartChIdx]->INTEN & au32UartINTEnSel[u8UartINTEnIdx]) == au32UartINTEnSel[u8UartINTEnIdx]);
        }

        //disable interrupt
        for (u8UartINTEnIdx = 0; u8UartINTEnIdx < 11; u8UartINTEnIdx++)
        {
            UART_DisableInt(UartCh[u8UartChIdx], au32UartINTEnSel[u8UartINTEnIdx]);
            CU_ASSERT((UartCh[u8UartChIdx]->INTEN & au32UartINTEnSel[u8UartINTEnIdx]) == 0);
        }

        //enable interrupt
        for (u8UartINTEnIdx = 0; u8UartINTEnIdx < 11; u8UartINTEnIdx++)
        {
            //skip THRE interrupt
            if (u8UartINTEnIdx == 1) continue;

            UART_EnableInt(UartCh[u8UartChIdx], au32UartINTEnSel[u8UartINTEnIdx]);
            UART_Close(UartCh[u8UartChIdx]);
            CU_ASSERT((UartCh[u8UartChIdx]->INTEN) == 0);
        }

    }

    //    NVIC_DisableIRQ(UART0_IRQn);
    //    NVIC_DisableIRQ(UART1_IRQn);
    //    NVIC_DisableIRQ(UART2_IRQn);
    //    NVIC_DisableIRQ(UART3_IRQn);
    //    NVIC_DisableIRQ(UART4_IRQn);
}

void SetUARTClockSource(UART_T *uart, uint32_t u32Source, uint32_t u32Div)
{
    /* Lock protected registers */
    SYS_UnlockReg();

    if (uart == UART0)
    {
        /* Set UART clock source selection */
        CLK->UARTSEL0 = (CLK->UARTSEL0 & ~ CLK_UARTSEL0_UART0SEL_Msk) | (u32Source << CLK_UARTSEL0_UART0SEL_Pos);
        /* Set UART clock divider number */
        CLK->UARTDIV0 = (CLK->UARTDIV0 & ~ CLK_UARTDIV0_UART0DIV_Msk) | (u32Div << CLK_UARTDIV0_UART0DIV_Pos);
    }
    else if (uart == UART1)
    {
        /* Set UART clock source selection */
        CLK->UARTSEL0 = (CLK->UARTSEL0 & ~ CLK_UARTSEL0_UART1SEL_Msk) | (u32Source << CLK_UARTSEL0_UART1SEL_Pos);
        /* Set UART clock divider number */
        CLK->UARTDIV0 = (CLK->UARTDIV0 & ~ CLK_UARTDIV0_UART1DIV_Msk) | (u32Div << CLK_UARTDIV0_UART1DIV_Pos);
    }
    else if (uart == UART2)
    {
        /* Set UART clock source selection */
        CLK->UARTSEL0 = (CLK->UARTSEL0 & ~ CLK_UARTSEL0_UART2SEL_Msk) | (u32Source << CLK_UARTSEL0_UART2SEL_Pos);
        /* Set UART clock divider number */
        CLK->UARTDIV0 = (CLK->UARTDIV0 & ~ CLK_UARTDIV0_UART2DIV_Msk) | (u32Div << CLK_UARTDIV0_UART2DIV_Pos);
    }
    else if (uart == UART3)
    {
        /* Set UART clock source selection */
        CLK->UARTSEL0 = (CLK->UARTSEL0 & ~ CLK_UARTSEL0_UART3SEL_Msk) | (u32Source << CLK_UARTSEL0_UART3SEL_Pos);
        /* Set UART clock divider number */
        CLK->UARTDIV0 = (CLK->UARTDIV0 & ~ CLK_UARTDIV0_UART3DIV_Msk) | (u32Div << CLK_UARTDIV0_UART3DIV_Pos);
    }
    else if (uart == UART4)
    {
        /* Set UART clock source selection */
        CLK->UARTSEL0 = (CLK->UARTSEL0 & ~ CLK_UARTSEL0_UART4SEL_Msk) | (u32Source << CLK_UARTSEL0_UART4SEL_Pos);
        /* Set UART clock divider number */
        CLK->UARTDIV0 = (CLK->UARTDIV0 & ~ CLK_UARTDIV0_UART4DIV_Msk) | (u32Div << CLK_UARTDIV0_UART4DIV_Pos);
    }
    else if (uart == UART5)
    {
        /* Set UART clock source selection */
        CLK->UARTSEL0 = (CLK->UARTSEL0 & ~ CLK_UARTSEL0_UART5SEL_Msk) | (u32Source << CLK_UARTSEL0_UART5SEL_Pos);
        /* Set UART clock divider number */
        CLK->UARTDIV0 = (CLK->UARTDIV0 & ~ CLK_UARTDIV0_UART5DIV_Msk) | (u32Div << CLK_UARTDIV0_UART5DIV_Pos);
    }
    else if (uart == UART6)
    {
        /* Set UART clock source selection */
        CLK->UARTSEL0 = (CLK->UARTSEL0 & ~ CLK_UARTSEL0_UART6SEL_Msk) | (u32Source << CLK_UARTSEL0_UART6SEL_Pos);
        /* Set UART clock divider number */
        CLK->UARTDIV0 = (CLK->UARTDIV0 & ~ CLK_UARTDIV0_UART6DIV_Msk) | (u32Div << CLK_UARTDIV0_UART6DIV_Pos);
    }
    else if (uart == UART7)
    {
        /* Set UART clock source selection */
        CLK->UARTSEL0 = (CLK->UARTSEL0 & ~ CLK_UARTSEL0_UART7SEL_Msk) | (u32Source << CLK_UARTSEL0_UART7SEL_Pos);
        /* Set UART clock divider number */
        CLK->UARTDIV0 = (CLK->UARTDIV0 & ~ CLK_UARTDIV0_UART7DIV_Msk) | (u32Div << CLK_UARTDIV0_UART7DIV_Pos);
    }
    else if (uart == UART8)
    {
        /* Set UART clock source selection */
        CLK->UARTSEL1 = (CLK->UARTSEL1 & ~ CLK_UARTSEL1_UART8SEL_Msk) | (u32Source << CLK_UARTSEL1_UART8SEL_Pos);
        /* Set UART clock divider number */
        CLK->UARTDIV1 = (CLK->UARTDIV1 & ~ CLK_UARTDIV1_UART8DIV_Msk) | (u32Div << CLK_UARTDIV1_UART8DIV_Pos);
    }
    else if (uart == UART9)
    {
        /* Set UART clock source selection */
        CLK->UARTSEL1 = (CLK->UARTSEL1 & ~ CLK_UARTSEL1_UART9SEL_Msk) | (u32Source << CLK_UARTSEL1_UART9SEL_Pos);
        /* Set UART clock divider number */
        CLK->UARTDIV1 = (CLK->UARTDIV1 & ~ CLK_UARTDIV1_UART9DIV_Msk) | (u32Div << CLK_UARTDIV1_UART9DIV_Pos);
    }

    /* Lock protected registers */
    SYS_LockReg();
}



uint32_t GetUartBaudrate(UART_T *uart)
{
    uint32_t u32UartClkSrcSel = 0, u32UartClkDivNum = 0;
    uint32_t au32ClkTbl[5] = {__HXT, __HIRC, __LXT, 0, __HIRC48M};
    uint32_t u32Baud_Div;

    if (uart == (UART_T *)UART0)
    {
        /* Get UART clock source selection */
        u32UartClkSrcSel = (CLK->UARTSEL0 & CLK_UARTSEL0_UART0SEL_Msk) >> CLK_UARTSEL0_UART0SEL_Pos;
        /* Get UART clock divider number */
        u32UartClkDivNum = (CLK->UARTDIV0 & CLK_UARTDIV0_UART0DIV_Msk) >> CLK_UARTDIV0_UART0DIV_Pos;
    }
    else if (uart == (UART_T *)UART1)
    {

        /* Get UART clock source selection */
        u32UartClkSrcSel = (CLK->UARTSEL0 & CLK_UARTSEL0_UART1SEL_Msk) >> CLK_UARTSEL0_UART1SEL_Pos;
        /* Get UART clock divider number */
        u32UartClkDivNum = (CLK->UARTDIV0 & CLK_UARTDIV0_UART1DIV_Msk) >> CLK_UARTDIV0_UART1DIV_Pos;
    }
    else if (uart == (UART_T *)UART2)
    {
        /* Get UART clock source selection */
        u32UartClkSrcSel = (CLK->UARTSEL0 & CLK_UARTSEL0_UART2SEL_Msk) >> CLK_UARTSEL0_UART2SEL_Pos;
        /* Get UART clock divider number */
        u32UartClkDivNum = (CLK->UARTDIV0 & CLK_UARTDIV0_UART2DIV_Msk) >> CLK_UARTDIV0_UART2DIV_Pos;
    }
    else if (uart == (UART_T *)UART3)
    {
        /* Get UART clock source selection */
        u32UartClkSrcSel = (CLK->UARTSEL0 & CLK_UARTSEL0_UART3SEL_Msk) >> CLK_UARTSEL0_UART3SEL_Pos;
        /* Get UART clock divider number */
        u32UartClkDivNum = (CLK->UARTDIV0 & CLK_UARTDIV0_UART3DIV_Msk) >> CLK_UARTDIV0_UART3DIV_Pos;
    }
    else if (uart == (UART_T *)UART4)
    {
        /* Get UART clock source selection */
        u32UartClkSrcSel = (CLK->UARTSEL0 & CLK_UARTSEL0_UART4SEL_Msk) >> CLK_UARTSEL0_UART4SEL_Pos;
        /* Get UART clock divider number */
        u32UartClkDivNum = (CLK->UARTDIV0 & CLK_UARTDIV0_UART4DIV_Msk) >> CLK_UARTDIV0_UART4DIV_Pos;
    }
    else if (uart == (UART_T *)UART5)
    {

        /* Get UART clock source selection */
        u32UartClkSrcSel = (CLK->UARTSEL0 & CLK_UARTSEL0_UART5SEL_Msk) >> CLK_UARTSEL0_UART5SEL_Pos;
        /* Get UART clock divider number */
        u32UartClkDivNum = (CLK->UARTDIV0 & CLK_UARTDIV0_UART5DIV_Msk) >> CLK_UARTDIV0_UART5DIV_Pos;
    }
    else if (uart == (UART_T *)UART6)
    {
        /* Get UART clock source selection */
        u32UartClkSrcSel = (CLK->UARTSEL0 & CLK_UARTSEL0_UART6SEL_Msk) >> CLK_UARTSEL0_UART6SEL_Pos;
        /* Get UART clock divider number */
        u32UartClkDivNum = (CLK->UARTDIV0 & CLK_UARTDIV0_UART6DIV_Msk) >> CLK_UARTDIV0_UART6DIV_Pos;
    }
    else if (uart == (UART_T *)UART7)
    {
        /* Get UART clock source selection */
        u32UartClkSrcSel = (CLK->UARTSEL0 & CLK_UARTSEL0_UART7SEL_Msk) >> CLK_UARTSEL0_UART7SEL_Pos;
        /* Get UART clock divider number */
        u32UartClkDivNum = (CLK->UARTDIV0 & CLK_UARTDIV0_UART7DIV_Msk) >> CLK_UARTDIV0_UART7DIV_Pos;
    }
    else if (uart == (UART_T *)UART8)
    {
        /* Get UART clock source selection */
        u32UartClkSrcSel = (CLK->UARTSEL1 & CLK_UARTSEL1_UART8SEL_Msk) >> CLK_UARTSEL1_UART8SEL_Pos;
        /* Get UART clock divider number */
        u32UartClkDivNum = (CLK->UARTDIV1 & CLK_UARTDIV1_UART8DIV_Msk) >> CLK_UARTDIV1_UART8DIV_Pos;
    }
    else if (uart == (UART_T *)UART9)
    {
        /* Get UART clock source selection */
        u32UartClkSrcSel = (CLK->UARTSEL1 & CLK_UARTSEL1_UART9SEL_Msk) >> CLK_UARTSEL1_UART9SEL_Pos;
        /* Get UART clock divider number */
        u32UartClkDivNum = (CLK->UARTDIV1 & CLK_UARTDIV1_UART9DIV_Msk) >> CLK_UARTDIV1_UART9DIV_Pos;
    }

    if (u32UartClkSrcSel == 3ul)
    {
        au32ClkTbl[u32UartClkSrcSel] = CLK_GetAPLL0ClockFreq() / 2;
    }


    /* Get UART baud rate divider */
    u32Baud_Div = (uart->BAUD & UART_BAUD_BRD_Msk) >> UART_BAUD_BRD_Pos;

    /* Calculate UART baud rate if baud rate is set in MODE 0 */
    if ((uart->BAUD & (UART_BAUD_BAUDM1_Msk | UART_BAUD_BAUDM0_Msk)) == UART_BAUD_MODE0)
        return ((au32ClkTbl[u32UartClkSrcSel]) / (u32UartClkDivNum + 1) / (u32Baud_Div + 2)) >> 4;

    /* Calculate UART baud rate if baud rate is set in MODE 2 */
    else if ((uart->BAUD & (UART_BAUD_BAUDM1_Msk | UART_BAUD_BAUDM0_Msk)) == UART_BAUD_MODE2)
        return ((au32ClkTbl[u32UartClkSrcSel]) / (u32UartClkDivNum + 1) / (u32Baud_Div + 2));

    /* Calculate UART baud rate if baud rate is set in MODE 1 */
    else if ((uart->BAUD & (UART_BAUD_BAUDM1_Msk | UART_BAUD_BAUDM0_Msk)) == UART_BAUD_BAUDM1_Msk)
        return ((au32ClkTbl[u32UartClkSrcSel]) / (u32UartClkDivNum + 1) / (u32Baud_Div + 2)) / (((uart->BAUD & UART_BAUD_EDIVM1_Msk) >> UART_BAUD_EDIVM1_Pos) + 1);

    /* Unsupported baud rate setting */
    else
        return 0;
}

const uint32_t au32UartBRSel[11] =
{
    2400,
    4800,
    9600,
    14400,
    19200,
    38400,
    57600,
    115200,
    230400,
    460800,
    921600
};

const uint32_t au32UartClkSel[5] =
{
    0x0, //UARTSEL_HXT,
    0x1, //UARTSEL_HIRC
    0x2, //UARTSEL_LXT,
    0x3, //UARTSEL_APLL0_DIV2,
    0x4, //UARTSEL_HIRC48M ,
};


void TestFunc_UART_Open()
{

    uint8_t u8UartChIdx;    //UART channel index
    uint8_t u8UartBRIdx;    //baud rate index
    uint8_t u8UartClkIdx;   //clock source index
    uint8_t u8UartDivIdx;   //clock divider index
    uint32_t u32ClkTbl[5ul] = {__HXT, __HIRC, __LXT, 0, __HIRC48M};
    uint32_t i;

    /* Get APLL0/2 frequency */
    u32ClkTbl[3] = CLK_GetAPLL0ClockFreq() / 2;
    //wait UART send message finish before change clock
    UART_WAIT_TX_EMPTY(UART0) {};

    /* Test loop */
    //select UART cahnnel
    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {

        //select UART baud rate
        for (u8UartBRIdx = 0; u8UartBRIdx < 11; u8UartBRIdx++)
        {
            //select UART clock source
            for (u8UartClkIdx = 0; u8UartClkIdx < 5; u8UartClkIdx++)
            {
                //skip reserved clock source case
                //                if (u8UartClkIdx == 1 || u8UartClkIdx == 2 || u8UartClkIdx == 5) continue;
                if (u8UartClkIdx == 2) continue;

                //select UART clock divider
                for (u8UartDivIdx = 0; u8UartDivIdx < 16; u8UartDivIdx++)
                {
                    //skip BRD smaller than 3 case, BRD will be 3 if set BRD smaller than 3 in mode 2
                    if (u32ClkTbl[u8UartClkIdx] < 5 * (au32UartBRSel[u8UartBRIdx]) * (u8UartDivIdx + 1)) continue;

                    //select UART clock source and divider
                    if (au32UartClkSel[u8UartClkIdx] == 2)
                    {
                        if (u8UartDivIdx == 0) continue;
                    }

                    SetUARTClockSource(UartCh[u8UartChIdx], au32UartClkSel[u8UartClkIdx], u8UartDivIdx);

                    //                    for (i = 0; i < 1000; i++) __NOP();
                    for (i = 0; i < 10; i++) __NOP();

                    //test function
                    UART_Open(UartCh[u8UartChIdx], au32UartBRSel[u8UartBRIdx]);
                    //check
                    CU_ASSERT(UartCh[u8UartChIdx]->FUNCSEL == UART_FUNCSEL_UART);
                    CU_ASSERT(UartCh[u8UartChIdx]->LINE == (UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1));
                    CU_ASSERT((UartCh[u8UartChIdx]->FIFO & (UART_FIFO_RFITL_Msk | UART_BAUD_EDIVM1_Msk)) == 0);
                    CU_ASSERT(GetUartBaudrate(UartCh[u8UartChIdx]) > (((au32UartBRSel[u8UartBRIdx]) * 90) / 100) &&
                              GetUartBaudrate(UartCh[u8UartChIdx]) < (((au32UartBRSel[u8UartBRIdx]) * 110) / 100));
                    UART_Open(UartCh[u8UartChIdx], au32UartBRSel[u8UartBRIdx]);
                    UART_SelectSingleWireMode(UartCh[u8UartChIdx]);
                    //check
                    CU_ASSERT(UartCh[u8UartChIdx]->FUNCSEL == UART_FUNCSEL_SINGLE_WIRE);
                    UartCh[u8UartChIdx]->FUNCSEL &= ((~UART_FUNCSEL_FUNCSEL_Msk) | UART_FUNCSEL_UART) ;
                    CU_ASSERT(UartCh[u8UartChIdx]->FUNCSEL == UART_FUNCSEL_UART);
                    CU_ASSERT(UartCh[u8UartChIdx]->LINE == (UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1));
                    CU_ASSERT((UartCh[u8UartChIdx]->FIFO & (UART_FIFO_RFITL_Msk | UART_BAUD_EDIVM1_Msk)) == 0);
                    CU_ASSERT(GetUartBaudrate(UartCh[u8UartChIdx]) > (((au32UartBRSel[u8UartBRIdx]) * 90) / 100) &&
                              GetUartBaudrate(UartCh[u8UartChIdx]) < (((au32UartBRSel[u8UartBRIdx]) * 110) / 100));



                }
            }
        }

        UART_SetBaudRateFrationalDivider(UartCh[u8UartChIdx], 0xaa);
        CU_ASSERT((UartCh[u8UartChIdx]->BAUD & (UART_BAUD_BRFDEN_Msk | UART_BAUD_BAUDM1_Msk | UART_BAUD_BAUDM0_Msk)) == 0x70000000);
        CU_ASSERT((UartCh[u8UartChIdx]->BAUD & UART_BAUD_BRFD_Msk) == 0x00aa0000);
        UART_SetBaudRateFrationalDivider(UartCh[u8UartChIdx], 0x0);
        CU_ASSERT((UartCh[u8UartChIdx]->BAUD & (UART_BAUD_BRFDEN_Msk | UART_BAUD_BAUDM1_Msk | UART_BAUD_BAUDM0_Msk)) == 0x70000000);
        CU_ASSERT((UartCh[u8UartChIdx]->BAUD & UART_BAUD_BRFD_Msk) == 0x0);
        UART_SetBaudRateFrationalDivider(UartCh[u8UartChIdx], 0x55);
        CU_ASSERT((UartCh[u8UartChIdx]->BAUD & (UART_BAUD_BRFDEN_Msk | UART_BAUD_BAUDM1_Msk | UART_BAUD_BAUDM0_Msk)) == 0x70000000);
        CU_ASSERT((UartCh[u8UartChIdx]->BAUD & UART_BAUD_BRFD_Msk) == 0x00550000);
        UART_DisableBaudRateFrationalDivider(UartCh[u8UartChIdx]);
        CU_ASSERT((UartCh[u8UartChIdx]->BAUD & (UART_BAUD_BRFDEN_Msk)) == 0x0);
        CU_ASSERT((UartCh[u8UartChIdx]->BAUD & UART_BAUD_BRFD_Msk) == 0x0);
    }


}

const uint32_t au32UartWordSel[4] =
{
    UART_WORD_LEN_5,
    UART_WORD_LEN_6,
    UART_WORD_LEN_7,
    UART_WORD_LEN_8,
};

const uint32_t au32UartParitySel[5] =
{
    UART_PARITY_NONE,
    UART_PARITY_ODD,
    UART_PARITY_EVEN,
    UART_PARITY_MARK,
    UART_PARITY_SPACE,
};

const uint32_t au32UartStopSel[3] =
{
    UART_STOP_BIT_1,
    UART_STOP_BIT_1_5,
    UART_STOP_BIT_2,
};

void TestFunc_UART_SetLine_Config()
{
    //variable for baud rate
    uint8_t u8UartChIdx;    //channel index
    uint8_t u8UartBRIdx;    //baud rate index
    uint8_t u8UartClkIdx;   //clock source index
    uint8_t u8UartDivIdx;   //clock divider index
    uint32_t u32ClkTbl[5] = {__HXT, __HIRC, __LXT, 0, __HIRC48M};
    /* Get APLL0/2 frequency */
    u32ClkTbl[3] = CLK_GetAPLL0ClockFreq() / 2;
    //variable for data format
    uint8_t u8UartWordIdx;      //word length index
    uint8_t u8UartParityIdx;    //parity index
    uint8_t u8UartStopIdx;      //stop bit length index

    //wait UART send message finish before change clock
    UART_WAIT_TX_EMPTY(UART0) {};

    /* Test loop */
    //select UART channel
    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        //select UART baud rate
        for (u8UartBRIdx = 7; u8UartBRIdx < 8; u8UartBRIdx++)
        {
            //select UART clock source
            for (u8UartClkIdx = 0; u8UartClkIdx < 5; u8UartClkIdx++)
            {
                //skip reserved clock source case
                //                if (u8UartClkIdx == 1 || u8UartClkIdx == 2 || u8UartClkIdx == 5) continue;
                if (u8UartClkIdx == 2) continue;

                //select UART clock divider
                for (u8UartDivIdx = 0; u8UartDivIdx < 16; u8UartDivIdx++)
                {
                    //skip BRD smaller than 3 case, BRD will be 3 if set BRD smaller than 3 in mode 2
                    if (u32ClkTbl[u8UartClkIdx] < 5 * (au32UartBRSel[u8UartBRIdx]) * (u8UartDivIdx + 1)) continue;

                    //select UART word length
                    for (u8UartWordIdx = 0; u8UartWordIdx < 4; u8UartWordIdx++)
                    {
                        //select UART parity
                        for (u8UartParityIdx = 0; u8UartParityIdx < 5; u8UartParityIdx++)
                        {
                            //select UART stop bit length
                            for (u8UartStopIdx = 0; u8UartStopIdx < 3; u8UartStopIdx++)
                            {
                                //select UART clock source and divider
                                if (au32UartClkSel[u8UartClkIdx] == 2)
                                {
                                    if (u8UartDivIdx == 0) continue;
                                }

                                SetUARTClockSource(UartCh[u8UartChIdx], au32UartClkSel[u8UartClkIdx], u8UartDivIdx);

                                //test function
                                UART_SetLineConfig(UartCh[u8UartChIdx],                   //select UART channel
                                                   au32UartBRSel[u8UartBRIdx],           //select UART baud rate
                                                   au32UartWordSel[u8UartWordIdx],       //select UART word length
                                                   au32UartParitySel[u8UartParityIdx],   //select UART parity
                                                   au32UartStopSel[u8UartStopIdx]);      //select UART stop bit length
                                //check
                                CU_ASSERT(UartCh[u8UartChIdx]->LINE == (au32UartWordSel[u8UartWordIdx] | au32UartParitySel[u8UartParityIdx] | au32UartStopSel[u8UartStopIdx]));
                                CU_ASSERT(GetUartBaudrate(UartCh[u8UartChIdx]) > (((au32UartBRSel[u8UartBRIdx]) * 90) / 100) &&
                                          GetUartBaudrate(UartCh[u8UartChIdx]) < (((au32UartBRSel[u8UartBRIdx]) * 110) / 100));

                            } //u8UartStopIdx
                        } //u8UartParityIdx
                    } //u8UartWordIdx
                } //u8UartDivIdx
            } //u8UartClkIdx
        } //u8UartBRIdx
    } //u8UartChIdx


    //restore UART setting for other test
    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        UartCh[u8UartChIdx]->LINE = (UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1);
        UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HXT, 115200);
    }
}

void TestFunc_UART_SetTimeoutCnt()
{
    uint8_t u8UartChIdx;        //channel index
    uint16_t u16UartToutCnt;    //time out counter

    /* Test loop */
    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX ; u8UartChIdx++)
    {
        for (u16UartToutCnt = 0; u16UartToutCnt < 256; u16UartToutCnt++)
        {
            UART_BUS_IDLE_TIMEOUT_ENABLE(UartCh[u8UartChIdx]);
            UART_SetTimeoutCnt(UartCh[u8UartChIdx], u16UartToutCnt);
            CU_ASSERT((UartCh[u8UartChIdx]->TOUT & UART_TOUT_BITOMEN_Msk) == 0x80000000);
            CU_ASSERT((UartCh[u8UartChIdx]->INTEN & UART_INTEN_TOCNTEN_Msk) == UART_INTEN_TOCNTEN_Msk);
            CU_ASSERT((UartCh[u8UartChIdx]->TOUT & UART_TOUT_TOIC_Msk) == (u16UartToutCnt << UART_TOUT_TOIC_Pos));
            UART_BUS_IDLE_TIMEOUT_DISABLE(UartCh[u8UartChIdx]);
            CU_ASSERT((UartCh[u8UartChIdx]->TOUT & UART_TOUT_BITOMEN_Msk) == 0x0);
            CU_ASSERT((UartCh[u8UartChIdx]->INTEN & UART_INTEN_TOCNTEN_Msk) == UART_INTEN_TOCNTEN_Msk);
            CU_ASSERT((UartCh[u8UartChIdx]->TOUT & UART_TOUT_TOIC_Msk) == (u16UartToutCnt << UART_TOUT_TOIC_Pos));
        }
    }
}

void TestFunc_UART_SelectIrDAMode()
{
    uint8_t u8UartChIdx;    //UART channel index
    uint8_t u8UartBRIdx;    //baud rate index
    uint8_t u8UartClkIdx;   //clock source index
    uint8_t u8UartDivIdx;   //clock divider index
    uint32_t u32ClkTbl[5] = {__HXT, __HIRC, __LXT, 0, __HIRC48M};

    /* Get APLL0/2 frequency */
    u32ClkTbl[3] = CLK_GetAPLL0ClockFreq() / 2;
    //wait UART send message finish before change clock
    UART_WAIT_TX_EMPTY(UART0) {};

    /* Test loop */
    //select UART channel
    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        //select UART baud rate
        for (u8UartBRIdx = 0; u8UartBRIdx < 8; u8UartBRIdx++)
        {
            //select UART clock source
            for (u8UartClkIdx = 0; u8UartClkIdx < 5; u8UartClkIdx++)
            {
                //skip reserved clock source case
                //                if (u8UartClkIdx == 1 || u8UartClkIdx == 2 || u8UartClkIdx == 5) continue;
                if (u8UartClkIdx == 2) continue;

                //select UART clock divider
                for (u8UartDivIdx = 0; u8UartDivIdx < 16; u8UartDivIdx++)
                {
                    //skip BRD smaller than 0 case in mode 0
                    if (u32ClkTbl[u8UartClkIdx] < (5 * (au32UartBRSel[u8UartBRIdx]) * (u8UartDivIdx + 1)) << 4) continue;

                    //select UART clock source and divider
                    if (au32UartClkSel[u8UartClkIdx] == 2)
                    {
                        if (u8UartDivIdx == 0) continue;
                    }

                    SetUARTClockSource(UartCh[u8UartChIdx], au32UartClkSel[u8UartClkIdx], u8UartDivIdx);

                    //test function
                    UART_SelectIrDAMode(UartCh[u8UartChIdx], au32UartBRSel[u8UartBRIdx], UART_IRDA_TXEN);
                    CU_ASSERT(UartCh[u8UartChIdx]->FUNCSEL == UART_FUNCSEL_IrDA);
                    CU_ASSERT((UartCh[u8UartChIdx]->IRDA & UART_IRDA_TXEN) == UART_IRDA_TXEN);
                    CU_ASSERT((UartCh[u8UartChIdx]->IRDA & UART_IRDA_TXINV_Msk) == 0);
                    CU_ASSERT(GetUartBaudrate(UartCh[u8UartChIdx]) > (((au32UartBRSel[u8UartBRIdx]) * 90) / 100) &&
                              GetUartBaudrate(UartCh[u8UartChIdx]) < (((au32UartBRSel[u8UartBRIdx]) * 110) / 100));

                    UART_SelectIrDAMode(UartCh[u8UartChIdx], au32UartBRSel[u8UartBRIdx], UART_IRDA_RXEN);
                    CU_ASSERT(UartCh[u8UartChIdx]->FUNCSEL == UART_FUNCSEL_IrDA);
                    CU_ASSERT((UartCh[u8UartChIdx]->IRDA & UART_IRDA_TXEN_Msk) == 0);
                    CU_ASSERT((UartCh[u8UartChIdx]->IRDA & UART_IRDA_RXINV_Msk) == UART_IRDA_RXINV_Msk);
                    CU_ASSERT(GetUartBaudrate(UartCh[u8UartChIdx]) > (((au32UartBRSel[u8UartBRIdx]) * 90) / 100) &&
                              GetUartBaudrate(UartCh[u8UartChIdx]) < (((au32UartBRSel[u8UartBRIdx]) * 110) / 100));

                }
            }
        }
    }


    //restore UART setting for other test
    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        UartCh[u8UartChIdx]->LINE = (UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1);
        UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HXT, 115200);
        UartCh[u8UartChIdx]->FUNCSEL = UART_FUNCSEL_UART;
    }
}

void TestFunc_UART_SelectRS485Mode()
{
    uint8_t u8UartChIdx;        //UART channel index
    uint8_t u8RS485ModeIdx;     //RS485 mode
    uint16_t u16RS485AddrIdx;   //RS485 address

    /* Test loop */
    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        for (u8RS485ModeIdx = 0; u8RS485ModeIdx < 8; u8RS485ModeIdx++)
        {
            //skip AAD and NMM set at the same time case
            if (u8RS485ModeIdx == 3 || u8RS485ModeIdx == 7) continue;

            for (u16RS485AddrIdx = 0; u16RS485AddrIdx < 256; u16RS485AddrIdx++)
            {
                UART_SelectRS485Mode(UartCh[u8UartChIdx], (u8RS485ModeIdx << UART_ALTCTL_RS485NMM_Pos), u16RS485AddrIdx);
                CU_ASSERT(UartCh[u8UartChIdx]->FUNCSEL == UART_FUNCSEL_RS485);
                CU_ASSERT((UartCh[u8UartChIdx]->ALTCTL & (UART_ALTCTL_RS485NMM_Msk | UART_ALTCTL_RS485AAD_Msk | UART_ALTCTL_RS485AUD_Msk))
                          == (u8RS485ModeIdx << UART_ALTCTL_RS485NMM_Pos));
                CU_ASSERT((UartCh[u8UartChIdx]->ALTCTL & UART_ALTCTL_ADDRMV_Msk) == (u16RS485AddrIdx << UART_ALTCTL_ADDRMV_Pos));
            }
        }
    }

    //restore UART setting for other test
    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        UartCh[u8UartChIdx]->LINE = (UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1);
        UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HXT, 115200);
        UartCh[u8UartChIdx]->FUNCSEL = UART_FUNCSEL_UART;
        UartCh[u8UartChIdx]->ALTCTL &= ~(UART_ALTCTL_RS485NMM_Msk | UART_ALTCTL_RS485AAD_Msk | UART_ALTCTL_RS485AUD_Msk);
    }
}


void TestFunc_UART_TestMacroBR()
{
    uint8_t u8UartChIdx;    //UART channel index
    uint8_t u8UartBRIdx;    //baud rate index
    uint8_t u8UartClkIdx;   //clock source index
    uint8_t u8UartDivIdx;   //clock divider index
    uint32_t u32BRD_Div;
    uint32_t u32ClkTbl[5ul] = {__HXT, __HIRC, __LXT, 0, __HIRC48M};
    uint32_t i;

    /* Get APLL0/2 frequency */
    u32ClkTbl[3] = CLK_GetAPLL0ClockFreq() / 2;
    //wait UART send message finish before change clock
    UART_WAIT_TX_EMPTY(UART0) {};

    /* Test macro: UART_BAUD_MODE0_DIVIDER */
    //select UART channel
    for (u8UartChIdx = 9; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        //select UART baud rate
        for (u8UartBRIdx = 3; u8UartBRIdx < 4; u8UartBRIdx++)
        {
            //select UART clock source
            for (u8UartClkIdx = 4; u8UartClkIdx < 5; u8UartClkIdx++)
            {
                //skip reserved clock source case
                if (u8UartClkIdx == 1 || u8UartClkIdx == 2 || u8UartClkIdx == 4) continue;

                //select UART clock divider
                for (u8UartDivIdx = 0; u8UartDivIdx < 16; u8UartDivIdx++)
                {
                    //skip BRD smaller than 0 case in mode 0
                    if (u32ClkTbl[u8UartClkIdx] < (5 * (au32UartBRSel[u8UartBRIdx]) * (u8UartDivIdx + 1)) << 4) continue;

                    //select UART clock source and divider
                    if (au32UartClkSel[u8UartClkIdx] == 2)
                    {
                        if (u8UartDivIdx == 0) continue;
                    }

                    SetUARTClockSource(UartCh[u8UartChIdx], au32UartClkSel[u8UartClkIdx], u8UartDivIdx);

                    u32BRD_Div = UART_BAUD_MODE0_DIVIDER(((u32ClkTbl[u8UartClkIdx]) / (u8UartDivIdx + 1)), au32UartBRSel[u8UartBRIdx]);

                    /* Test macro: UART_BAUD_MODE0_DIVIDER */
                    UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE0 | u32BRD_Div;

                    //                                      printf("%x,%x,%d,%d,%d\n",UartCh[u8UartChIdx]->BAUD,u32BRD_Div,u32ClkTbl[u8UartClkIdx],(au32UartBRSel[u8UartBRIdx]),u8UartDivIdx);
                    //                    for (i = 0; i < 10000; i++) __NOP();
                    for (i = 0; i < 100; i++) __NOP();

                    //                   printf("%d,%d,%d\n\n",u8UartClkIdx,GetUartBaudrate(UartCh[u8UartChIdx]),(au32UartBRSel[u8UartBRIdx]));

                    CU_ASSERT(GetUartBaudrate(UartCh[u8UartChIdx]) > (((au32UartBRSel[u8UartBRIdx]) * 90) / 100) &&
                              GetUartBaudrate(UartCh[u8UartChIdx]) < (((au32UartBRSel[u8UartBRIdx]) * 110) / 100));

                } //u8UartDivIdx
            } //u8UartClkIdx
        } //u8UartBRIdx
    } //u8UartChIdx

    /* Test macro: UART_BAUD_MODE2_DIVIDER */
    //select UART channel
    for (u8UartChIdx = 9; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        //select UART baud rate
        for (u8UartBRIdx = 7; u8UartBRIdx < 8; u8UartBRIdx++)
        {
            //select UART clock source
            for (u8UartClkIdx = 4; u8UartClkIdx < 5; u8UartClkIdx++)
            {
                //skip reserved clock source case
                if (u8UartClkIdx == 1 || u8UartClkIdx == 2 || u8UartClkIdx == 5) continue;

                //select UART clock divider
                for (u8UartDivIdx = 0; u8UartDivIdx < 16; u8UartDivIdx++)
                {
                    //skip BRD smaller than 3 case, BRD will be 3 if set BRD smaller than 3 in mode 2
                    if (u32ClkTbl[u8UartClkIdx] < 5 * (au32UartBRSel[u8UartBRIdx]) * (u8UartDivIdx + 1)) continue;

                    //select UART clock source and divider
                    if (au32UartClkSel[u8UartClkIdx] == 1)
                    {
                        if (u8UartDivIdx == 0) continue;
                    }

                    SetUARTClockSource(UartCh[u8UartChIdx], au32UartClkSel[u8UartClkIdx], u8UartDivIdx);

                    u32BRD_Div = UART_BAUD_MODE2_DIVIDER(((u32ClkTbl[u8UartClkIdx]) / (u8UartDivIdx + 1)), au32UartBRSel[u8UartBRIdx]);
                    //test function
                    UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE2 | u32BRD_Div;
                    //                    printf("%x,%x,%d,%d,%d\n",UartCh[u8UartChIdx]->BAUD,u32BRD_Div,u32ClkTbl[u8UartClkIdx],(au32UartBRSel[u8UartBRIdx]),u8UartDivIdx);

                    //                    for (i = 0; i < 10000; i++) __NOP();
                    for (i = 0; i < 100; i++) __NOP();

                    //                                  printf("%d,%d,%d\n\n",u8UartClkIdx,GetUartBaudrate(UartCh[u8UartChIdx]),(au32UartBRSel[u8UartBRIdx]));
                    CU_ASSERT(GetUartBaudrate(UartCh[u8UartChIdx]) > (((au32UartBRSel[u8UartBRIdx]) * 90) / 100) &&
                              GetUartBaudrate(UartCh[u8UartChIdx]) < (((au32UartBRSel[u8UartBRIdx]) * 110) / 100));

                } //u8UartDivIdx
            } //u8UartClkIdx
        } //u8UartBRIdx
    } //u8UartChIdx

    //restore UART debug port setting for printf
    //    CLK->UARTSEL0 |= CLK_UARTSEL0_UART0SEL_Msk;
    //    CLK->UARTDIV0 &= (~CLK_UARTDIV0_UART0DIV_Msk);

    //restore UART setting for other test
    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        UartCh[u8UartChIdx]->LINE = (UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1);
        UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HXT, 115200);
    }
}

void TestFunc_UART_TestMacroFIFO()
{
    uint8_t u8UartChIdx;    //UART channel index
    uint8_t i;  //number control

    //wait UART send message finish
    UART_WAIT_TX_EMPTY(UART0) {};

    /* Set UART1 internal loopback mode */
    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        UartCh[u8UartChIdx]->FIFO |= UART_FIFO_RXRST_Msk;

        while ((UartCh[u8UartChIdx]->FIFO & UART_FIFO_RXRST_Msk) == UART_FIFO_RXRST_Msk);

        UartCh[u8UartChIdx]->MODEM |= 0x10;
        UartCh[u8UartChIdx]->LINE = (UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1);
        UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(12000000, 115200);
    }

    /* Test macro : UART_WRITE
                    UART_READ
                    UART_GET_TX_EMPTY
                    UART_GET_RX_EMPTY
                    UART_IS_TX_EMPTY
                    UART_WAIT_TX_EMPTY
                    UART_IS_RX_READY
                    UART_IS_TX_FULL
                    UART_IS_RX_FULL
                    UART_GET_TX_FULL
                    UART_GET_RX_FULL */
    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        //check initial state
        CU_ASSERT(UART_GET_TX_EMPTY(UartCh[u8UartChIdx]) == UART_FIFOSTS_TXEMPTY_Msk);
        CU_ASSERT(UART_GET_RX_EMPTY(UartCh[u8UartChIdx]) == UART_FIFOSTS_RXEMPTY_Msk);
        CU_ASSERT(UART_IS_TX_EMPTY(UartCh[u8UartChIdx]) == 1);
        UART_WAIT_TX_EMPTY(UartCh[u8UartChIdx]) {};
        CU_ASSERT(UART_IS_RX_READY(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_IS_TX_FULL(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_IS_RX_FULL(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_GET_TX_FULL(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_GET_RX_FULL(UartCh[u8UartChIdx]) == 0);

        //check Tx full state
        //set CTS low level active
        UartCh[u8UartChIdx]->MODEMSTS |= UART_MODEMSTS_CTSACTLV_Msk;
        //enable CTS auto flow control
        UartCh[u8UartChIdx]->INTEN |= UART_INTEN_ATOCTSEN_Msk;

        //        if (u8UartChIdx != 4)
        //        {
        //send data to let Tx full
        for (i = 0; i < 16; i++)
        {
            UART_WRITE(UartCh[u8UartChIdx], (0x55 + i));
        }

        //        }
        //        else
        //        {
        //            //send data to let Tx full
        //            for (i = 0; i < 3; i++)
        //            {
        //                UART_WRITE(UartCh[u8UartChIdx], (0x55 + i));
        //            }
        //        }

        //check macro
        CU_ASSERT(UART_GET_TX_EMPTY(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_GET_RX_EMPTY(UartCh[u8UartChIdx]) == UART_FIFOSTS_RXEMPTY_Msk);
        CU_ASSERT(UART_IS_TX_EMPTY(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_IS_RX_READY(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_IS_TX_FULL(UartCh[u8UartChIdx]) == 1);
        CU_ASSERT(UART_IS_RX_FULL(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_GET_TX_FULL(UartCh[u8UartChIdx]) == UART_FIFOSTS_TXFULL_Msk);
        CU_ASSERT(UART_GET_RX_FULL(UartCh[u8UartChIdx]) == 0);

        //check Rx full state
        //set CTS high level active
        UartCh[u8UartChIdx]->MODEMSTS &= ~UART_MODEMSTS_CTSACTLV_Msk;
        //wait Tx send finished
        UART_WAIT_TX_EMPTY(UartCh[u8UartChIdx]) {};
        //check macro
        CU_ASSERT(UART_GET_TX_EMPTY(UartCh[u8UartChIdx]) == UART_FIFOSTS_TXEMPTY_Msk);
        CU_ASSERT(UART_GET_RX_EMPTY(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_IS_TX_EMPTY(UartCh[u8UartChIdx]) == 1);
        CU_ASSERT(UART_IS_RX_READY(UartCh[u8UartChIdx]) == 1);
        CU_ASSERT(UART_IS_TX_FULL(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_IS_RX_FULL(UartCh[u8UartChIdx]) == 1);
        CU_ASSERT(UART_GET_TX_FULL(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_GET_RX_FULL(UartCh[u8UartChIdx]) == UART_FIFOSTS_RXFULL_Msk);

        //        if (u8UartChIdx != 4)
        //        {
        //read Rx empty
        for (i = 0; i < 16; i++)
        {
            UART_READ(UartCh[u8UartChIdx]);
        }

        //        }
        //        else
        //        {
        //            //read Rx empty
        //            for (i = 0; i < 3; i++)
        //            {
        //                UART_READ(UartCh[u8UartChIdx]);
        //            }
        //        }

        //check macro
        CU_ASSERT(UART_GET_TX_EMPTY(UartCh[u8UartChIdx]) == UART_FIFOSTS_TXEMPTY_Msk);
        CU_ASSERT(UART_GET_RX_EMPTY(UartCh[u8UartChIdx]) == UART_FIFOSTS_RXEMPTY_Msk);
        CU_ASSERT(UART_IS_TX_EMPTY(UartCh[u8UartChIdx]) == 1);
        UART_WAIT_TX_EMPTY(UartCh[u8UartChIdx]) {};
        CU_ASSERT(UART_IS_RX_READY(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_IS_TX_FULL(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_IS_RX_FULL(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_GET_TX_FULL(UartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UART_GET_RX_FULL(UartCh[u8UartChIdx]) == 0);
    }

    for (u8UartChIdx = 0; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        //disable CTS auto flow control after test
        UartCh[u8UartChIdx]->INTEN &= ~UART_INTEN_ATOCTSEN_Msk;
        //disable UART1 internal loopback mode after test */
        UartCh[u8UartChIdx]->MODEM &= ~0x10;
    }
}

void TestFunc_UART_TestMacroRTS()
{
    uint8_t u8UartChIdx;    //UART channel index
    uint32_t i;

    for (u8UartChIdx = 0; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        //skip reserved clock source case
        if (u8UartChIdx == 4) continue;

        /* Test macro : UART_CLEAR_RTS
                        UART_SET_RTS */
        //disable RTS auto flow control
        UartCh[u8UartChIdx]->INTEN &= ~UART_INTEN_ATORTSEN_Msk;
        UART_SET_RTS(UartCh[u8UartChIdx]);

        while (!(UartCh[u8UartChIdx]->MODEM & UART_MODEM_RTSSTS_Msk)) {};

        //        for (i = 0; i < 1000000; i++) {};
        for (i = 0; i < 100; i++) {};

        CU_ASSERT((UartCh[u8UartChIdx]->MODEM & (UART_MODEM_RTSSTS_Msk | UART_MODEM_RTSACTLV_Msk | UART_MODEM_RTS_Msk)) == (UART_MODEM_RTSSTS_Msk | UART_MODEM_RTSACTLV_Msk | UART_MODEM_RTS_Msk));

        UART_CLEAR_RTS(UartCh[u8UartChIdx]);

        while ((UartCh[u8UartChIdx]->MODEM & UART_MODEM_RTSSTS_Msk)) {};

        CU_ASSERT((UartCh[u8UartChIdx]->MODEM & (UART_MODEM_RTSSTS_Msk | UART_MODEM_RTSACTLV_Msk | UART_MODEM_RTS_Msk)) == UART_MODEM_RTSACTLV_Msk);

        UART_RS485RTSDLY_SET(UartCh[u8UartChIdx], 0x5555);

        CU_ASSERT((UartCh[u8UartChIdx]->RS485DD & UART_RS485DD_RTSDDLY_Msk) == 0x5555);

        UART_RS485RTSDLY_SET(UartCh[u8UartChIdx], 0xaaaa);

        CU_ASSERT((UartCh[u8UartChIdx]->RS485DD & UART_RS485DD_RTSDDLY_Msk) == 0xaaaa);

        UART_RS485RTSDLY_SET(UartCh[u8UartChIdx], 0);

        CU_ASSERT((UartCh[u8UartChIdx]->RS485DD & UART_RS485DD_RTSDDLY_Msk) == 0);

        UART_DEGLITCH_ENABLE(UartCh[u8UartChIdx]);

        CU_ASSERT((UartCh[u8UartChIdx]->FUNCSEL & UART_FUNCSEL_DGE_Msk) == 0x40);

        UART_DEGLITCH_DISABLE(UartCh[u8UartChIdx]);

        CU_ASSERT((UartCh[u8UartChIdx]->FUNCSEL & UART_FUNCSEL_DGE_Msk) == 0x0);

        UART_TXRX_SWAP_ENABLE(UartCh[u8UartChIdx]);

        CU_ASSERT((UartCh[u8UartChIdx]->FUNCSEL & UART_FUNCSEL_TXRXSWP_Msk) == 0x80);

        UART_TXRX_SWAP_DISABLE(UartCh[u8UartChIdx]);

        CU_ASSERT((UartCh[u8UartChIdx]->FUNCSEL & UART_FUNCSEL_TXRXSWP_Msk) == 0x0);


    }
}
void TestFunc_UART_SelectLINMode()
{
    uint8_t u8UartChIdx;        //UART channel index
    uint8_t u8LinModeIdx;       //LIN mode
    uint16_t u16LinBreakLen;    //LIN Break Length

    /* Test loop */
    for (u8UartChIdx = 1; u8UartChIdx < 2; u8UartChIdx++)
    {
        for (u8LinModeIdx = 0; u8LinModeIdx < 2; u8LinModeIdx++)
        {
            for (u16LinBreakLen = 1; u16LinBreakLen < 16; u16LinBreakLen++)
            {
                UART_SelectLINMode(UartCh[u8UartChIdx], UART_ALTCTL_LINTXEN_Msk, u16LinBreakLen);
                UartCh[u8UartChIdx]->DAT = 0x55;
                UART_WAIT_TX_EMPTY(UartCh[u8UartChIdx]) {};
                CU_ASSERT(UartCh[u8UartChIdx]->FUNCSEL == UART_FUNCSEL_LIN);
                CU_ASSERT((UartCh[u8UartChIdx]->ALTCTL & UART_ALTCTL_BRKFL_Msk) == u16LinBreakLen);

                UART_SelectLINMode(UartCh[u8UartChIdx], UART_ALTCTL_LINRXEN_Msk, 0);
                CU_ASSERT(UartCh[u8UartChIdx]->FUNCSEL == UART_FUNCSEL_LIN);
                CU_ASSERT((UartCh[u8UartChIdx]->ALTCTL & UART_ALTCTL_LINRXEN_Msk) == UART_ALTCTL_LINRXEN_Msk);
            }
        }
    }

    //restore UART setting for other test
    for (u8UartChIdx = 1; u8UartChIdx < LIN_CH_NUM_MAX; u8UartChIdx++)
    {
        UartCh[u8UartChIdx]->FUNCSEL = UART_FUNCSEL_UART;
        UartCh[u8UartChIdx]->ALTCTL &= ~(UART_ALTCTL_LINTXEN_Msk | UART_ALTCTL_LINRXEN_Msk);
    }

}
void TestFunc_UART_TestMacroINT()
{
    uint8_t u8UartChIdx;    //UART channel index
    uint8_t u8UartINTEnIdx; //bit
    uint32_t i;

    //wait UART send message finish
    UART_WAIT_TX_EMPTY(UART0) {};

    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        for (u8UartINTEnIdx = 0; u8UartINTEnIdx < 11; u8UartINTEnIdx++)
        {
            /* Test macro : UART_ENABLE_INT
                            UART_DISABLE_INT */
            //Enable bit in IER register
            //0 : Receive data available interrupt
            //1 : Transmit holding register empty interrupt
            //2 : Receive line status interrupt
            //3 : Modem status interrupt
            //4 : Rx time-out interrupt
            //5 : Buffer error interrupt
            //6 : LIN bus interrupt
            //7 : CTS wake-up interrupt
            //8 : Data Wake-up interrupt
            //9 : Auto baud-rate interrupt
            //10: Tx end interrupt
            //11: Single-wire Bit Error Detection Interrupt

            UART_ENABLE_INT(UartCh[u8UartChIdx], au32UartINTEnSel[u8UartINTEnIdx]);
            CU_ASSERT((UartCh[u8UartChIdx]->INTEN & au32UartINTEnSel[u8UartINTEnIdx]) == au32UartINTEnSel[u8UartINTEnIdx]);
            UART_DISABLE_INT(UartCh[u8UartChIdx], au32UartINTEnSel[u8UartINTEnIdx]);
            CU_ASSERT((UartCh[u8UartChIdx]->INTEN & au32UartINTEnSel[u8UartINTEnIdx]) == 0);
        }
    }

    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        /* Test macro : UART_GET_INT_FLAG */

        //enable UART1 internal loopback
        UartCh[u8UartChIdx]->MODEM |= 0x10;

        //RX FIFO reset
        UartCh[u8UartChIdx]->FIFO |= UART_FIFO_RXRST_Msk;

        while ((UartCh[u8UartChIdx]->FIFO & UART_FIFO_RXRST_Msk) == UART_FIFO_RXRST_Msk);

        //0 : Receive data available interrupt
        //1 : Transmit holding register empty interrupt
        // set CTS low level active
        UartCh[u8UartChIdx]->MODEMSTS |= UART_MODEMSTS_CTSACTLV_Msk;
        // enable CTS auto flow control
        UartCh[u8UartChIdx]->INTEN |= UART_INTEN_ATOCTSEN_Msk;
        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_THREIF_Msk) == 1);
        // fill Tx
        UART_WRITE(UartCh[u8UartChIdx], 0x55);
        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_THREIF_Msk) == 0);
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_RDAIF_Msk) == 0);
        // set CTS high level active to release data in Tx FIFO
        UartCh[u8UartChIdx]->MODEMSTS &= ~UART_MODEMSTS_CTSACTLV_Msk;

        //UartCh[u8UartChIdx]->MODEMSTS |= UART_MODEMSTS_CTSACTLV_Msk;
        // wait data send out
        while ((UartCh[u8UartChIdx]->FIFOSTS & UART_FIFOSTS_TXEMPTYF_Msk) == 0) {};

        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_THREIF_Msk) == 1);

        // wait data receive
        while ((UartCh[u8UartChIdx]->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == UART_FIFOSTS_RXEMPTY_Msk) {};

        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_RDAIF_Msk) == 1);

        // read Rx
        UART_READ(UartCh[u8UartChIdx]);

        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_RDAIF_Msk) == 0);

        // disable CTS auto flow control after test
        UartCh[u8UartChIdx]->INTEN &= ~UART_INTEN_ATOCTSEN_Msk;

        //2 : Receive line status interrupt
        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_RLSIF_Msk) == 0);

        // send break
        UartCh[u8UartChIdx]->LINE |= UART_LINE_BCB_Msk;

        // wait break error happen
        while ((UartCh[u8UartChIdx]->FIFOSTS & UART_FIFOSTS_BIF_Msk) == 0) {};

        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_RLSIF_Msk) == 1);

        // stop send break
        UartCh[u8UartChIdx]->LINE &= ~UART_LINE_BCB_Msk;

        // clear receive line status flag
        UART_ClearIntFlag(UartCh[u8UartChIdx], UART_INTSTS_RLSINT_Msk);

        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_RLSIF_Msk) == 0);

        // clear Rx FIFO
        UartCh[u8UartChIdx]->FIFO |= UART_FIFO_RXRST_Msk;

        while ((UartCh[u8UartChIdx]->FIFO & UART_FIFO_RXRST_Msk) == UART_FIFO_RXRST_Msk) {};

        //3 : Modem status interrupt
        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_MODEMIF_Msk) == 1);

        // clear CTS flag

        UartCh[u8UartChIdx]->MODEMSTS |= UART_MODEMSTS_CTSDETF_Msk;

        for (i = 0; i < 100; i++) __NOP();

        UartCh[u8UartChIdx]->MODEMSTS |= UART_MODEMSTS_CTSDETF_Msk;

        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_MODEMIF_Msk) == 0);

        //4 : Rx time-out interrupt
        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_RXTOIF_Msk) == 0);
        // enable time out counter
        UartCh[u8UartChIdx]->INTEN |= UART_INTEN_TOCNTEN_Msk;
        // send Tx
        UART_WRITE(UartCh[u8UartChIdx], 0x55);

        // wait Rx time out happen
        while ((UartCh[u8UartChIdx]->INTSTS & UART_INTSTS_RXTOIF_Msk) == 0) {};

        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_RXTOIF_Msk) == 1);

        // disable time out counter after finish test
        UartCh[u8UartChIdx]->INTEN &= ~UART_INTEN_TOCNTEN_Msk;

        // read Rx
        UART_READ(UartCh[u8UartChIdx]);

        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_RXTOINT_Msk) == 0);

        //5 : Buffer error interrupt
        // check macro
        //        if (u8UartChIdx != 4)
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_BUFERRIF_Msk) == 0);

        // send data and wait Rx buffer error flag happen
        while ((UartCh[u8UartChIdx]->INTSTS & UART_INTSTS_BUFERRIF_Msk) == 0)
        {
            // send Tx
            UART_WRITE(UartCh[u8UartChIdx], 0x55);
        }

        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_BUFERRIF_Msk) == 1);
        // clear Rx buffer error flag
        UartCh[u8UartChIdx]->FIFOSTS |= UART_FIFOSTS_RXOVIF_Msk;

        while ((UartCh[u8UartChIdx]->FIFOSTS & UART_FIFOSTS_RXOVIF_Msk) == UART_FIFOSTS_RXOVIF_Msk) {};

        // check macro
        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_BUFERRIF_Msk) == 0);

        // clear Rx FIFO
        UartCh[u8UartChIdx]->FIFO |= UART_FIFO_RXRST_Msk;

        while ((UartCh[u8UartChIdx]->FIFOSTS & UART_FIFO_RXRST_Msk) == UART_FIFO_RXRST_Msk) {};

        /* Test macro : UART_RS485_CLEAR_ADDR_FLAG
                        UART_RS485_GET_ADDR_FLAG */
        // check macro
        CU_ASSERT(UART_RS485_GET_ADDR_FLAG(UartCh[u8UartChIdx]) == 0);

        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_RLSINT_Msk) == 0);

        // set RS485 mode
        UartCh[u8UartChIdx]->FUNCSEL = UART_FUNCSEL_RS485;

        // set RS485 address detection enable
        UartCh[u8UartChIdx]->ALTCTL |= (UART_ALTCTL_ADDRDEN_Msk | UART_ALTCTL_RS485NMM_Msk);

        // set parity 1
        UartCh[u8UartChIdx]->LINE = UART_WORD_LEN_8 | UART_PARITY_ODD | UART_STOP_BIT_1;

        // send Tx
        UART_WRITE(UartCh[u8UartChIdx], 0x55);

        // wait RS485 address flag happen
        while ((UartCh[u8UartChIdx]->FIFOSTS & UART_FIFOSTS_ADDRDETF_Msk) == 0) {};

        // check macro
        CU_ASSERT(UART_RS485_GET_ADDR_FLAG(UartCh[u8UartChIdx]) == 1);

        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_RLSIF_Msk) == 1);

        // clear RS485 address flag
        UART_RS485_CLEAR_ADDR_FLAG(UartCh[u8UartChIdx]);

        // check macro
        CU_ASSERT(UART_RS485_GET_ADDR_FLAG(UartCh[u8UartChIdx]) == 0);

        CU_ASSERT(UART_GET_INT_FLAG(UartCh[u8UartChIdx], UART_INTSTS_RLSIF_Msk) == 0);

        UartCh[u8UartChIdx]->ALTCTL = 0xC;


        //disable UART internal loopback mode after test */
        UartCh[u8UartChIdx]->MODEM &= ~0x10;

        UartCh[u8UartChIdx]->FUNCSEL = UART_FUNCSEL_UART;

        UartCh[u8UartChIdx]->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;

        UartCh[u8UartChIdx]->ALTCTL &= ~(UART_ALTCTL_ADDRDEN_Msk | UART_ALTCTL_RS485NMM_Msk);
    }

}


const uint32_t au32UartRxTrgSel[4] =
{
    UART_FIFO_RFITL_1BYTE,
    UART_FIFO_RFITL_4BYTES,
    UART_FIFO_RFITL_8BYTES,
    UART_FIFO_RFITL_14BYTES,
};

const uint32_t au32UartRTSTrgSel[4] =
{
    UART_FIFO_RTSTRGLV_1BYTE,
    UART_FIFO_RTSTRGLV_4BYTES,
    UART_FIFO_RTSTRGLV_8BYTES,
    UART_FIFO_RTSTRGLV_14BYTES,
};

void TestFunc_UART_TestConstant()
{
    uint8_t u8UartChIdx;        //UART channel index
    uint16_t u16TestIdx;
    uint32_t i;

    //wait UART send message finish
    UART_WAIT_TX_EMPTY(UART0) {};

    // Only uart0/uart1 support LIN mode for TC8237
    // Only uart0 support LIN mode for M252

    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        //select Rx FIFO trigger level
        for (u16TestIdx = 0; u16TestIdx < 4; u16TestIdx++)
        {
            UartCh[u8UartChIdx]->FIFO = ((UartCh[u8UartChIdx]->FIFO & (~UART_FIFO_RFITL_Msk)) | au32UartRxTrgSel[u16TestIdx]);

            //            if (u8UartChIdx != 4)
            {
                CU_ASSERT((UartCh[u8UartChIdx]->FIFO & UART_FIFO_RFITL_Msk) == au32UartRxTrgSel[u16TestIdx]);
            }
            //            else
            //            {
            //                CU_ASSERT((UartCh[u8UartChIdx]->FIFO & UART_FIFO_RFITL_Msk) == au32UartRxTrgSel[0]);
            //            }

        }

        //select RTS trigger level index
        for (u16TestIdx = 0; u16TestIdx < 4; u16TestIdx++)
        {
            UartCh[u8UartChIdx]->FIFO = ((UartCh[u8UartChIdx]->FIFO & (~UART_FIFO_RTSTRGLV_Msk)) | au32UartRTSTrgSel[u16TestIdx]);

            //            if (u8UartChIdx != 4)
            //            {
            CU_ASSERT((UartCh[u8UartChIdx]->FIFO & UART_FIFO_RTSTRGLV_Msk) == au32UartRTSTrgSel[u16TestIdx]);
            //            }
            //            else
            //            {
            //                CU_ASSERT((UartCh[u8UartChIdx]->FIFO & UART_FIFO_RTSTRGLV_Msk) == au32UartRTSTrgSel[0]);
            //            }
        }

        //select RTS active level
        UartCh[u8UartChIdx]->MODEM = ((UartCh[u8UartChIdx]->MODEM & (~UART_MODEM_RTSACTLV_Msk)) | UART_RTS_IS_LOW_LEV_ACTIVE);
        CU_ASSERT((UartCh[u8UartChIdx]->MODEM & UART_MODEM_RTSACTLV_Msk) == UART_RTS_IS_LOW_LEV_ACTIVE);

        UartCh[u8UartChIdx]->MODEM = ((UartCh[u8UartChIdx]->MODEM & (~UART_MODEM_RTSACTLV_Msk)) | UART_RTS_IS_HIGH_LEV_ACTIVE);
        CU_ASSERT((UartCh[u8UartChIdx]->MODEM & UART_MODEM_RTSACTLV_Msk) == UART_RTS_IS_HIGH_LEV_ACTIVE);
    }

    CU_ASSERT(UART0_FIFO_SIZE == 16);
    CU_ASSERT(UART1_FIFO_SIZE == 16);
    CU_ASSERT(UART2_FIFO_SIZE == 16);
    CU_ASSERT(UART3_FIFO_SIZE == 16);
    CU_ASSERT(UART4_FIFO_SIZE == 16);
    CU_ASSERT(UART5_FIFO_SIZE == 16);
    CU_ASSERT(UART6_FIFO_SIZE == 16);
    CU_ASSERT(UART7_FIFO_SIZE == 16);
    CU_ASSERT(UART8_FIFO_SIZE == 16);
    CU_ASSERT(UART9_FIFO_SIZE == 16);


    //    UART_IPReset(UART0);
    UART_IPReset(UART1);
    UART_IPReset(UART2);
    UART_IPReset(UART3);
    UART_IPReset(UART4);
}

void TestFunc_UART_ReadWrite()
{
    uint8_t u8UartChIdx, i;        //UART channel index
    uint8_t u8TxData[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t u8RxData[8] = {0};

    /* Test loop */
    for (u8UartChIdx = 1; u8UartChIdx < UART_CH_NUM_MAX; u8UartChIdx++)
    {
        UartCh[u8UartChIdx]->MODEM |= 0x10; // set loop back mode

        /* Configure UART0 and set UART0 baud rate */
        UartCh[u8UartChIdx]->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HIRC, 115200);
        UartCh[u8UartChIdx]->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;

        /* Reset Tx/Rx FIFO */
        UartCh[u8UartChIdx]->FIFO |= 0x6;

        while (UartCh[u8UartChIdx]->FIFO & 0x6) {};

        //        if (u8UartChIdx != 4)
        //        {
        /* Write data */
        UART_Write(UartCh[u8UartChIdx], u8TxData, 8);

        /* Read data */
        UART_Read(UartCh[u8UartChIdx], u8RxData, 8);

        for (i = 0; i < 8; i++)
        {
            CU_ASSERT(u8TxData[i] == u8RxData[i]);
        }

        //        }
        //        else
        //        {
        //            /* Write data */
        //            UART_Write(UartCh[u8UartChIdx], u8TxData, 1);
        //            /* Read data */
        //            UART_Read(UartCh[u8UartChIdx], u8RxData, 1);

        //            CU_ASSERT(u8TxData[0] == u8RxData[0]);

        //        }

    }

}


CU_TestInfo UART_ApiTests[] =
{
    {"UART_Enable/DisableFlowCtrl Function.", TestFunc_UART_EnableFlowCtrl},
    {"UART_Enable/DisableInt Function.", TestFunc_UART_EnableInt},
    {"UART_Open Function.", TestFunc_UART_Open},
    {"UART_SetLine_Config Function.", TestFunc_UART_SetLine_Config},
    {"UART_SetTimeoutCnt Function.", TestFunc_UART_SetTimeoutCnt},
    {"UART_SelectIrDAMode Function.", TestFunc_UART_SelectIrDAMode},
    {"UART_SelectRS485Mode Function.", TestFunc_UART_SelectRS485Mode},
    {"Testing UART_ReadWrite Function.", TestFunc_UART_ReadWrite},
    {"Testing UART_SelectLINMode Function.", TestFunc_UART_SelectLINMode},
    CU_TEST_INFO_NULL
};




CU_TestInfo UART_MacroTests[] =
{
    {"Macro about baud rate.", TestFunc_UART_TestMacroBR},
    {"Macro about FIFO.", TestFunc_UART_TestMacroFIFO},
    {"Macro about RTS.", TestFunc_UART_TestMacroRTS},
    {"Macro about interrupt.", TestFunc_UART_TestMacroINT},
    {"Constant.", TestFunc_UART_TestConstant},

    CU_TEST_INFO_NULL
};

