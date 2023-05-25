/*---------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/* Copyright (C) 2020 Nuvoton Technology Corp.                                                             */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   LPUART_cunit_test.c                                                                                    */
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
#include "lpuart_cunit.h"



#define LPUART_CH_NUM_MAX 1

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

CU_SuiteInfo LPUART_suites[] =
{
    {"LPUART API", suite_success_init, suite_success_clean, NULL, NULL, LPUART_ApiTests},
    {"LPUART MACRO", suite_success_init, suite_success_clean, NULL, NULL, LPUART_MacroTests},
    CU_SUITE_INFO_NULL
};



//LPUART_T *LpUartCh[10] =
//{
//    LPUART0,  //skip debug port UART0 test
//};

void LPUART_IPReset(LPUART_T *psLPUART)
{
    /* Unlock protected registers */
    SYS_UnlockReg(); 
    switch ((uint32_t)psLPUART)
    {
        case (uint32_t)LPUART0:
             SYS_ResetModule(SYS_LPUART0RST);  // Reset UART0 IP
            break;

    }
    /* Lock protected registers */
    SYS_LockReg();
}

void TestFunc_LPUART_EnableFlowCtrl()
{
    uint8_t u8LpUartChIdx;    //UART channel index

    /* UART 4 is  not support the Auto Flow Control*/
//    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX ; u8LpUartChIdx++)
    {
        LPUART_EnableFlowCtrl(LPUART0);
        CU_ASSERT((LPUART0->INTEN & (LPUART_INTEN_ATORTSEN_Msk | LPUART_INTEN_ATOCTSEN_Msk))
                  == (LPUART_INTEN_ATORTSEN_Msk | LPUART_INTEN_ATOCTSEN_Msk));
        CU_ASSERT((LPUART0->MODEM & LPUART_MODEM_RTSACTLV_Msk) == LPUART_MODEM_RTSACTLV_Msk);
        CU_ASSERT((LPUART0->MODEMSTS & LPUART_MODEMSTS_CTSACTLV_Msk) == LPUART_MODEMSTS_CTSACTLV_Msk);
        LPUART_DisableFlowCtrl(LPUART0);
        CU_ASSERT((LPUART0->INTEN & (LPUART_INTEN_ATORTSEN_Msk | LPUART_INTEN_ATOCTSEN_Msk)) == 0);
    }
}

const uint32_t au32LpUartINTEnSel[10] =
{
    LPUART_INTEN_RDAIEN_Msk,       //0 : Receive data available interrupt
    LPUART_INTEN_THREIEN_Msk,      //1 : Transmit holding register empty interrupt
    LPUART_INTEN_RLSIEN_Msk,       //2 : Receive line status interrupt
    LPUART_INTEN_MODEMIEN_Msk,     //3 : Modem status interrupt
    LPUART_INTEN_RXTOIEN_Msk,      //4 : Rx time-out interrupt
    LPUART_INTEN_BUFERRIEN_Msk,    //5 : Buffer error interrupt
    LPUART_INTEN_WKIEN_Msk,        //6 : Wake-up interrupt
    LPUART_INTEN_TOCNTEN_Msk,      //7 : Time-out interrupt
    LPUART_INTEN_ABRIEN_Msk,       //8 : Auto baud-rate interrupt
    LPUART_INTEN_TXENDIEN_Msk,     //9: Tx end interrupt
};

void TestFunc_LPUART_EnableInt()
{
    uint8_t u8LpUartChIdx;    //UART channel index
    uint8_t u8LpUartINTEnIdx; //bit

    /* test loop */
//    for (u8LpLpUartChIdx = 0; u8LpLpUartChIdx < LPUART_CH_NUM_MAX; u8LpLpUartChIdx++)
    {
        //enable interrupt
        for (u8LpUartINTEnIdx = 0; u8LpUartINTEnIdx < 10; u8LpUartINTEnIdx++)
        {
            //skip THRE interrupt
            if (u8LpUartINTEnIdx == 1 ) continue;

            LPUART_EnableInt(LPUART0, au32LpUartINTEnSel[u8LpUartINTEnIdx]);
            CU_ASSERT((LPUART0->INTEN & au32LpUartINTEnSel[u8LpUartINTEnIdx]) == au32LpUartINTEnSel[u8LpUartINTEnIdx]);
        }

        //disable interrupt
        for (u8LpUartINTEnIdx = 0; u8LpUartINTEnIdx < 10; u8LpUartINTEnIdx++)
        {
            LPUART_DisableInt(LPUART0, au32LpUartINTEnSel[u8LpUartINTEnIdx]);
            CU_ASSERT((LPUART0->INTEN & au32LpUartINTEnSel[u8LpUartINTEnIdx]) == 0);
        }

        //enable interrupt
        for (u8LpUartINTEnIdx = 0; u8LpUartINTEnIdx < 10; u8LpUartINTEnIdx++)
        {
            //skip THRE interrupt
            if (u8LpUartINTEnIdx == 1) continue;

            LPUART_EnableInt(LPUART0, au32LpUartINTEnSel[u8LpUartINTEnIdx]);
            LPUART_Close(LPUART0);
            CU_ASSERT((LPUART0->INTEN) == 0);
        }

    }

    NVIC_DisableIRQ(LPUART0_IRQn);

}

void SetLPUARTClockSource(LPUART_T* lpuart, uint32_t u32Source, uint32_t u32Div)
{
    if (lpuart == LPUART0)
    {
        /* Set UART clock source selection */
        CLK->LPUARTSEL = (CLK->LPUARTSEL & ~ CLK_LPUARTSEL_LPUART0SEL_Msk) | (u32Source << CLK_LPUARTSEL_LPUART0SEL_Pos);
        /* Set UART clock divider number */
        CLK->LPUARTDIV = (CLK->LPUARTDIV & ~ CLK_LPUARTDIV_LPUART0DIV_Msk) | (u32Div << CLK_LPUARTDIV_LPUART0DIV_Pos);
    }
 
}



uint32_t GetLpUartBaudrate(LPUART_T* lpuart)
{
    uint32_t u32LpUartClkSrcSel = 0, u32LpUartClkDivNum = 0;
    uint32_t au32ClkTbl[4] = {0,__LXT, __MIRC, __HIRC};
    uint32_t u32Baud_Div;

    if(lpuart==(LPUART_T*)LPUART0)
    {
        /* Get LPUART clock source selection */
        u32LpUartClkSrcSel = ((CLK->LPUARTSEL & CLK_LPUARTSEL_LPUART0SEL_Msk)) >> CLK_LPUARTSEL_LPUART0SEL_Pos;
        /* Get LPUART clock divider number */
        u32LpUartClkDivNum = (CLK->LPUARTDIV & CLK_LPUARTDIV_LPUART0DIV_Msk) >> CLK_LPUARTDIV_LPUART0DIV_Pos;

    }

    if(u32LpUartClkSrcSel == 0ul)
    {
         au32ClkTbl[0] =CLK_GetPCLK4Freq();
    }


    /* Get LPUART baud rate divider */
    u32Baud_Div = (lpuart->BAUD & LPUART_BAUD_BRD_Msk) >> LPUART_BAUD_BRD_Pos;

    /* Calculate UART baud rate if baud rate is set in MODE 0 */
    if ((lpuart->BAUD & (LPUART_BAUD_BAUDM1_Msk | LPUART_BAUD_BAUDM0_Msk)) == LPUART_BAUD_MODE0)
        return ((au32ClkTbl[u32LpUartClkSrcSel]) / (u32LpUartClkDivNum + 1) / (u32Baud_Div + 2)) >> 4;

    /* Calculate UART baud rate if baud rate is set in MODE 2 */
    else if ((lpuart->BAUD & (LPUART_BAUD_BAUDM1_Msk | LPUART_BAUD_BAUDM0_Msk)) == LPUART_BAUD_MODE2)
        return ((au32ClkTbl[u32LpUartClkSrcSel]) / (u32LpUartClkDivNum + 1) / (u32Baud_Div + 2));

    /* Calculate UART baud rate if baud rate is set in MODE 1 */
    else if ((lpuart->BAUD & (LPUART_BAUD_BAUDM1_Msk | LPUART_BAUD_BAUDM0_Msk)) == LPUART_BAUD_BAUDM1_Msk)
        return ((au32ClkTbl[u32LpUartClkSrcSel]) / (u32LpUartClkDivNum + 1) / (u32Baud_Div + 2)) / (((lpuart->BAUD & LPUART_BAUD_EDIVM1_Msk) >> LPUART_BAUD_EDIVM1_Pos) + 1);

    /* Unsupported baud rate setting */
    else
        return 0;
}

const uint32_t au32LpUartBRSel[11] =
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

const uint32_t au32LpUartClkSel[4] =
{
    0x0, //LPUARTSEL_PCLK4,
    0x1, //LPUARTSEL_LXT,
    0x2, //LPUARTSEL_MIRC,
    0x3, //UARTSEL_HIRC,
//    0x4, //UARTSEL_PLCK0,
//    0x5, //UARTSEL_LIRC
};


void TestFunc_LPUART_Open()
{

    uint8_t  u8LpUartChIdx;    //UART channel index
    uint8_t  u8LpUartBRIdx;    //baud rate index
    uint8_t  u8LpUartClkIdx;   //clock source index
    uint8_t  u8LpUartDivIdx;   //clock divider index
    uint32_t u32ClkTbl[4] = {0,__LXT, __MIRC, __HIRC};
    uint32_t i;


    //wait UART send message finish before change clock
    LPUART_WAIT_TX_EMPTY(LPUART0) {};

    /* Test loop */
    //select LPUART cahnnel
//    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {

        //select LPUART baud rate
        for (u8LpUartBRIdx = 0; u8LpUartBRIdx < 11; u8LpUartBRIdx++)
        {
            //select LPUART clock source
            for (u8LpUartClkIdx = 0; u8LpUartClkIdx < 4; u8LpUartClkIdx++)
            {
                //skip reserved clock source case
                if (u8LpUartClkIdx == 1 || u8LpUartClkIdx == 2 ) continue;

                //select LPUART clock divider
                for (u8LpUartDivIdx = 0; u8LpUartDivIdx < 16; u8LpUartDivIdx++)
                {
                    //skip BRD smaller than 3 case, BRD will be 3 if set BRD smaller than 3 in mode 2
                    if (u32ClkTbl[u8LpUartClkIdx] < (5 * (au32LpUartBRSel[u8LpUartBRIdx]) * (u8LpUartDivIdx + 1))) continue;

                    //select LPUART clock source and divider
                    if (au32LpUartClkSel[u8LpUartClkIdx] == 2)
                    {
                        if (u8LpUartDivIdx == 0) continue;
                    }

                    SetLPUARTClockSource(LPUART0, au32LpUartClkSel[u8LpUartClkIdx], u8LpUartDivIdx);

                    for (i = 0; i < 1000; i++) __NOP();

                    //test function
                    LPUART_Open(LPUART0, au32LpUartBRSel[u8LpUartBRIdx]);
                    //check
                    CU_ASSERT(LPUART0->FUNCSEL == LPUART_FUNCSEL_LPUART);
                    CU_ASSERT(LPUART0->LINE == (LPUART_WORD_LEN_8 | LPUART_PARITY_NONE | LPUART_STOP_BIT_1));
                    CU_ASSERT((LPUART0->FIFO & (LPUART_FIFO_RFITL_Msk | LPUART_BAUD_EDIVM1_Msk)) == 0);
                    CU_ASSERT(GetLpUartBaudrate(LPUART0) > (((au32LpUartBRSel[u8LpUartBRIdx]) * 90) / 100) &&
                              GetLpUartBaudrate(LPUART0) < (((au32LpUartBRSel[u8LpUartBRIdx]) * 110) / 100));
//                    UART_Open(LPUART0, au32LpUartBRSel[u8LpUartBRIdx]);
//                    UART_SelectSingleWireMode(LPUART0);
//                    //check
//                    CU_ASSERT(LPUART0->FUNCSEL == UART_FUNCSEL_SINGLE_WIRE);
//                    LPUART0->FUNCSEL &= ((~UART_FUNCSEL_FUNCSEL_Msk) | UART_FUNCSEL_UART) ;
//                    CU_ASSERT(LPUART0->FUNCSEL == UART_FUNCSEL_UART);
//                    CU_ASSERT(LPUART0->LINE == (UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1));
//                    CU_ASSERT((LPUART0->FIFO & (UART_FIFO_RFITL_Msk | UART_BAUD_EDIVM1_Msk)) == 0);
//                    CU_ASSERT(GetLpUartBaudrate(LPUART0) > (((au32LpUartBRSel[u8LpUartBRIdx]) * 90) / 100) &&
//                              GetLpUartBaudrate(LPUART0) < (((au32LpUartBRSel[u8LpUartBRIdx]) * 110) / 100));



                }
            }
        }

    }


}

const uint32_t au32LpUartWordSel[4] =
{
    LPUART_WORD_LEN_5,
    LPUART_WORD_LEN_6,
    LPUART_WORD_LEN_7,
    LPUART_WORD_LEN_8,
};

const uint32_t au32LpUartParitySel[5] =
{
    LPUART_PARITY_NONE,
    LPUART_PARITY_ODD,
    LPUART_PARITY_EVEN,
    LPUART_PARITY_MARK,
    LPUART_PARITY_SPACE,
};

const uint32_t au32LpUartStopSel[3] =
{
    LPUART_STOP_BIT_1,
    LPUART_STOP_BIT_1_5,
    LPUART_STOP_BIT_2,
};

void TestFunc_LPUART_SetLine_Config()
{
    //variable for baud rate
    uint8_t u8LpUartChIdx;    //channel index
    uint8_t u8LpUartBRIdx;    //baud rate index
    uint8_t u8LpUartClkIdx;   //clock source index
    uint8_t u8LpUartDivIdx;   //clock divider index
    uint32_t u32ClkTbl[4] = {0,__LXT, __MIRC, __HIRC};

    //variable for data format
    uint8_t u8LpUartWordIdx;      //word length index
    uint8_t u8LpUartParityIdx;    //parity index
    uint8_t u8LpUartStopIdx;      //stop bit length index

    //wait LPUART send message finish before change clock
    LPUART_WAIT_TX_EMPTY(LPUART0) {};

    /* Test loop */
    //select UART channel
//    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        //select UART baud rate
        for (u8LpUartBRIdx = 7; u8LpUartBRIdx < 8; u8LpUartBRIdx++)
        {
            //select UART clock source
            for (u8LpUartClkIdx = 0; u8LpUartClkIdx < 6; u8LpUartClkIdx++)
            {
                //skip reserved clock source case
                if (u8LpUartClkIdx == 1 || u8LpUartClkIdx == 2 ) continue;

                //select UART clock divider
                for (u8LpUartDivIdx = 0; u8LpUartDivIdx < 16; u8LpUartDivIdx++)
                {
                    //skip BRD smaller than 3 case, BRD will be 3 if set BRD smaller than 3 in mode 2
                    if (u32ClkTbl[u8LpUartClkIdx] < (5 * (au32LpUartBRSel[u8LpUartBRIdx]) * (u8LpUartDivIdx + 1))) continue;

                    //select UART word length
                    for (u8LpUartWordIdx = 0; u8LpUartWordIdx < 4; u8LpUartWordIdx++)
                    {
                        //select UART parity
                        for (u8LpUartParityIdx = 0; u8LpUartParityIdx < 5; u8LpUartParityIdx++)
                        {
                            //select UART stop bit length
                            for (u8LpUartStopIdx = 0; u8LpUartStopIdx < 3; u8LpUartStopIdx++)
                            {
                                //select UART clock source and divider
                                if (au32LpUartClkSel[u8LpUartClkIdx] == 2)
                                {
                                    if (u8LpUartDivIdx == 0) continue;
                                }

                                SetLPUARTClockSource(LPUART0, au32LpUartClkSel[u8LpUartClkIdx], u8LpUartDivIdx);

                                //test function
                                LPUART_SetLineConfig(LPUART0,                                 //select LPUART channel
                                                    au32LpUartBRSel[u8LpUartBRIdx],           //select LPUART baud rate
                                                    au32LpUartWordSel[u8LpUartWordIdx],       //select LPUART word length
                                                    au32LpUartParitySel[u8LpUartParityIdx],   //select LPUART parity
                                                    au32LpUartStopSel[u8LpUartStopIdx]);      //select LPUART stop bit length
                                //check
                                CU_ASSERT(LPUART0->LINE == (au32LpUartWordSel[u8LpUartWordIdx] | au32LpUartParitySel[u8LpUartParityIdx] | au32LpUartStopSel[u8LpUartStopIdx]));
                                CU_ASSERT(GetLpUartBaudrate(LPUART0) > (((au32LpUartBRSel[u8LpUartBRIdx]) * 90) / 100) &&
                                          GetLpUartBaudrate(LPUART0) < (((au32LpUartBRSel[u8LpUartBRIdx]) * 110) / 100));

                            } //u8LpUartStopIdx
                        } //u8LpUartParityIdx
                    } //u8LpUartWordIdx
                } //u8LpUartDivIdx
            } //u8LpUartClkIdx
        } //u8LpUartBRIdx
    } //u8LpUartChIdx


    //restore UART setting for other test
    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        LPUART0->LINE = (LPUART_WORD_LEN_8 | LPUART_PARITY_NONE | LPUART_STOP_BIT_1);
        LPUART0->BAUD = LPUART_BAUD_MODE2 | LPUART_BAUD_MODE2_DIVIDER(__HXT, 115200);
    }
}

void TestFunc_LPUART_SetTimeoutCnt()
{
    uint8_t u8LpUartChIdx;        //channel index
    uint16_t u16UartToutCnt;    //time out counter

    /* Test loop */
    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX ; u8LpUartChIdx++)
    {
        for (u16UartToutCnt = 0; u16UartToutCnt < 256; u16UartToutCnt++)
        {
            LPUART_SetTimeoutCnt(LPUART0, u16UartToutCnt);
            CU_ASSERT((LPUART0->INTEN & LPUART_INTEN_TOCNTEN_Msk) == LPUART_INTEN_TOCNTEN_Msk);
            CU_ASSERT((LPUART0->TOUT & LPUART_TOUT_TOIC_Msk) == (u16UartToutCnt << LPUART_TOUT_TOIC_Pos));
        }
    }
}


void TestFunc_LPUART_SelectRS485Mode()
{
    uint8_t u8LpUartChIdx;        //UART channel index
    uint8_t u8RS485ModeIdx;       //RS485 mode
    uint16_t u16RS485AddrIdx;     //RS485 address

    /* Test loop */
    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        for (u8RS485ModeIdx = 0; u8RS485ModeIdx < 8; u8RS485ModeIdx++)
        {
            //skip AAD and NMM set at the same time case
            if (u8RS485ModeIdx == 3 || u8RS485ModeIdx == 7) continue;

            for (u16RS485AddrIdx = 0; u16RS485AddrIdx < 256; u16RS485AddrIdx++)
            {
                LPUART_SelectRS485Mode(LPUART0, (u8RS485ModeIdx << LPUART_ALTCTL_RS485NMM_Pos), u16RS485AddrIdx);
                CU_ASSERT(LPUART0->FUNCSEL == LPUART_FUNCSEL_RS485);
                CU_ASSERT((LPUART0->ALTCTL & (LPUART_ALTCTL_RS485NMM_Msk | LPUART_ALTCTL_RS485AAD_Msk | LPUART_ALTCTL_RS485AUD_Msk))
                          == (u8RS485ModeIdx << LPUART_ALTCTL_RS485NMM_Pos));
                CU_ASSERT((LPUART0->ALTCTL & LPUART_ALTCTL_ADDRMV_Msk) == (u16RS485AddrIdx << LPUART_ALTCTL_ADDRMV_Pos));
            }
        }
    }

    //restore UART setting for other test
    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        LPUART0->LINE = (LPUART_WORD_LEN_8 | LPUART_PARITY_NONE | LPUART_STOP_BIT_1);
        LPUART0->BAUD = LPUART_BAUD_MODE2 | LPUART_BAUD_MODE2_DIVIDER(__HXT, 115200);
        LPUART0->FUNCSEL = LPUART_FUNCSEL_LPUART;
        LPUART0->ALTCTL &= ~(LPUART_ALTCTL_RS485NMM_Msk | LPUART_ALTCTL_RS485AAD_Msk | LPUART_ALTCTL_RS485AUD_Msk);
    }
}


void TestFunc_LPUART_TestMacroBR()
{
    uint8_t u8LpUartChIdx;    //UART channel index
    uint8_t u8LpUartBRIdx;    //baud rate index
    uint8_t u8LpUartClkIdx;   //clock source index
    uint8_t u8LpUartDivIdx;   //clock divider index
    uint32_t u32ClkTbl[4] = {0,__LXT, __MIRC, __HIRC};
    uint32_t i;

    /* Get HIRC frequency */
    //    u32ClkTbl[1] = __HIRC;
    //wait UART send message finish before change clock
    LPUART_WAIT_TX_EMPTY(LPUART0) {};

    /* Test macro: UART_BAUD_MODE0_DIVIDER */
    //select UART channel
    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        //select UART baud rate
        for (u8LpUartBRIdx = 3; u8LpUartBRIdx < 4; u8LpUartBRIdx++)
        {
            //select UART clock source
            for (u8LpUartClkIdx = 0; u8LpUartClkIdx < 4; u8LpUartClkIdx++)
            {
                //skip reserved clock source case
                if (u8LpUartClkIdx == 1 || u8LpUartClkIdx == 2 ) continue;

                //select UART clock divider
                for (u8LpUartDivIdx = 0; u8LpUartDivIdx < 16; u8LpUartDivIdx++)
                {
                    //skip BRD smaller than 0 case in mode 0
                    if (u32ClkTbl[u8LpUartClkIdx] < (5 * (au32LpUartBRSel[u8LpUartBRIdx]) * (u8LpUartDivIdx + 1)) << 4) continue;

                    //select UART clock source and divider
                    if (au32LpUartClkSel[u8LpUartClkIdx] == 2)
                    {
                        if (u8LpUartDivIdx == 0) continue;
                    }

                    SetLPUARTClockSource(LPUART0, au32LpUartClkSel[u8LpUartClkIdx], u8LpUartDivIdx);

                    /* Test macro: UART_BAUD_MODE0_DIVIDER */
                    LPUART0->BAUD = LPUART_BAUD_MODE0 | LPUART_BAUD_MODE0_DIVIDER(((u32ClkTbl[u8LpUartClkIdx]) / (u8LpUartDivIdx + 1)), au32LpUartBRSel[u8LpUartBRIdx]);

                    for (i = 0; i < 10000; i++) __NOP();

                    CU_ASSERT(GetLpUartBaudrate(LPUART0) > (((au32LpUartBRSel[u8LpUartBRIdx]) * 90) / 100) &&
                              GetLpUartBaudrate(LPUART0) < (((au32LpUartBRSel[u8LpUartBRIdx]) * 110) / 100));

                } //u8LpUartDivIdx
            } //u8LpUartClkIdx
        } //u8LpUartBRIdx
    } //u8LpLpUartChIdx

    /* Test macro: UART_BAUD_MODE2_DIVIDER */
    //select UART channel
    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        //select UART baud rate
        for (u8LpUartBRIdx = 7; u8LpUartBRIdx < 8; u8LpUartBRIdx++)
        {
            //select UART clock source
            for (u8LpUartClkIdx = 0; u8LpUartClkIdx < 6; u8LpUartClkIdx++)
            {
                //skip reserved clock source case
                if (u8LpUartClkIdx == 1 || u8LpUartClkIdx == 2 || u8LpUartClkIdx == 5) continue;

                //select UART clock divider
                for (u8LpUartDivIdx = 0; u8LpUartDivIdx < 16; u8LpUartDivIdx++)
                {
                    //skip BRD smaller than 3 case, BRD will be 3 if set BRD smaller than 3 in mode 2
                    if (u32ClkTbl[u8LpUartClkIdx] < (5 * (au32LpUartBRSel[u8LpUartBRIdx]) * (u8LpUartDivIdx + 1))) continue;

                    //select UART clock source and divider
                    if (au32LpUartClkSel[u8LpUartClkIdx] == 1)
                    {
                        if (u8LpUartDivIdx == 0) continue;
                    }

                    SetLPUARTClockSource(LPUART0, au32LpUartClkSel[u8LpUartClkIdx], u8LpUartDivIdx);

                    //test function
                    LPUART0->BAUD = LPUART_BAUD_MODE2 | LPUART_BAUD_MODE2_DIVIDER(((u32ClkTbl[u8LpUartClkIdx]) / (u8LpUartDivIdx + 1)), au32LpUartBRSel[u8LpUartBRIdx]);

                    for (i = 0; i < 10000; i++) __NOP();

                    CU_ASSERT(GetLpUartBaudrate(LPUART0) > (((au32LpUartBRSel[u8LpUartBRIdx]) * 90) / 100) &&
                              GetLpUartBaudrate(LPUART0) < (((au32LpUartBRSel[u8LpUartBRIdx]) * 110) / 100));

                } //u8LpUartDivIdx
            } //u8LpUartClkIdx
        } //u8LpUartBRIdx
    } //u8LpLpUartChIdx

    //restore UART debug port setting for printf
    CLK->LPUARTSEL |= CLK_LPUARTSEL_LPUART0SEL_Msk;
    CLK->UARTDIV0 &= (~CLK_LPUARTDIV_LPUART0DIV_Msk);

    //restore UART setting for other test
    for (u8LpUartChIdx = 1; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        LPUART0->LINE = (LPUART_WORD_LEN_8 | LPUART_PARITY_NONE | LPUART_STOP_BIT_1);
        LPUART0->BAUD = LPUART_BAUD_MODE2 | LPUART_BAUD_MODE2_DIVIDER(__HIRC, 115200);
    }
}

void TestFunc_LPUART_TestMacroFIFO()
{
    uint8_t u8LpUartChIdx;    //UART channel index
    uint8_t i;  //number control

    //wait UART send message finish
    LPUART_WAIT_TX_EMPTY(LPUART0) {};

    /* Set UART1 internal loopback mode */
    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        LPUART0->FIFO |= LPUART_FIFO_RXRST_Msk;

        while ((LPUART0->FIFO & LPUART_FIFO_RXRST_Msk) == LPUART_FIFO_RXRST_Msk);

        LPUART0->MODEM |= 0x10;
        LPUART0->LINE = (LPUART_WORD_LEN_8 | LPUART_PARITY_NONE | LPUART_STOP_BIT_1);
        LPUART0->BAUD = LPUART_BAUD_MODE2 | LPUART_BAUD_MODE2_DIVIDER(__HIRC, 115200);
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
    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        //check initial state
        CU_ASSERT(LPUART_GET_TX_EMPTY(LPUART0) == LPUART_FIFOSTS_TXEMPTY_Msk);
        CU_ASSERT(LPUART_GET_RX_EMPTY(LPUART0) == LPUART_FIFOSTS_RXEMPTY_Msk);
        CU_ASSERT(LPUART_IS_TX_EMPTY(LPUART0) == 1);
        LPUART_WAIT_TX_EMPTY(LPUART0) {};
        CU_ASSERT(LPUART_IS_RX_READY(LPUART0) == 0);
        CU_ASSERT(LPUART_IS_TX_FULL(LPUART0) == 0);
        CU_ASSERT(LPUART_IS_RX_FULL(LPUART0) == 0);
        CU_ASSERT(LPUART_GET_TX_FULL(LPUART0) == 0);
        CU_ASSERT(LPUART_GET_RX_FULL(LPUART0) == 0);

        //check Tx full state
        //set CTS low level active
        LPUART0->MODEMSTS |= LPUART_MODEMSTS_CTSACTLV_Msk;
        //enable CTS auto flow control
        LPUART0->INTEN |= LPUART_INTEN_ATOCTSEN_Msk;

        if (u8LpUartChIdx != 4)
        {
            //send data to let Tx full
            for (i = 0; i < 16; i++)
            {
                LPUART_WRITE(LPUART0, (0x55 + i));
            }
        }
        else
        {
            //send data to let Tx full
            for (i = 0; i < 3; i++)
            {
                LPUART_WRITE(LPUART0, (0x55 + i));
            }
        }

        //check macro
        CU_ASSERT(LPUART_GET_TX_EMPTY(LPUART0) == 0);
        CU_ASSERT(LPUART_GET_RX_EMPTY(LPUART0) == LPUART_FIFOSTS_RXEMPTY_Msk);
        CU_ASSERT(LPUART_IS_TX_EMPTY(LPUART0) == 0);
        CU_ASSERT(LPUART_IS_RX_READY(LPUART0) == 0);
        CU_ASSERT(LPUART_IS_TX_FULL(LPUART0) == 1);
        CU_ASSERT(LPUART_IS_RX_FULL(LPUART0) == 0);
        CU_ASSERT(LPUART_GET_TX_FULL(LPUART0) == LPUART_FIFOSTS_TXFULL_Msk);
        CU_ASSERT(LPUART_GET_RX_FULL(LPUART0) == 0);

        //check Rx full state
        //set CTS high level active
        LPUART0->MODEMSTS &= ~LPUART_MODEMSTS_CTSACTLV_Msk;
        //wait Tx send finished
        LPUART_WAIT_TX_EMPTY(LPUART0) {};
        //check macro
        CU_ASSERT(LPUART_GET_TX_EMPTY(LPUART0) == LPUART_FIFOSTS_TXEMPTY_Msk);
        CU_ASSERT(LPUART_GET_RX_EMPTY(LPUART0) == 0);
        CU_ASSERT(LPUART_IS_TX_EMPTY(LPUART0) == 1);
        CU_ASSERT(LPUART_IS_RX_READY(LPUART0) == 1);
        CU_ASSERT(LPUART_IS_TX_FULL(LPUART0) == 0);
        CU_ASSERT(LPUART_IS_RX_FULL(LPUART0) == 1);
        CU_ASSERT(LPUART_GET_TX_FULL(LPUART0) == 0);
        CU_ASSERT(LPUART_GET_RX_FULL(LPUART0) == LPUART_FIFOSTS_RXFULL_Msk);

        if (u8LpUartChIdx != 4)
        {
            //read Rx empty
            for (i = 0; i < 16; i++)
            {
                LPUART_READ(LPUART0);
            }
        }
        else
        {
            //read Rx empty
            for (i = 0; i < 3; i++)
            {
                LPUART_READ(LPUART0);
            }
        }

        //check macro
        CU_ASSERT(LPUART_GET_TX_EMPTY(LPUART0) == LPUART_FIFOSTS_TXEMPTY_Msk);
        CU_ASSERT(LPUART_GET_RX_EMPTY(LPUART0) == LPUART_FIFOSTS_RXEMPTY_Msk);
        CU_ASSERT(LPUART_IS_TX_EMPTY(LPUART0) == 1);
        LPUART_WAIT_TX_EMPTY(LPUART0) {};
        CU_ASSERT(LPUART_IS_RX_READY(LPUART0) == 0);
        CU_ASSERT(LPUART_IS_TX_FULL(LPUART0) == 0);
        CU_ASSERT(LPUART_IS_RX_FULL(LPUART0) == 0);
        CU_ASSERT(LPUART_GET_TX_FULL(LPUART0) == 0);
        CU_ASSERT(LPUART_GET_RX_FULL(LPUART0) == 0);
    }

    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        //disable CTS auto flow control after test
        LPUART0->INTEN &= ~LPUART_INTEN_ATOCTSEN_Msk;
        //disable UART1 internal loopback mode after test */
        LPUART0->MODEM &= ~0x10;
    }
}

void TestFunc_LPUART_TestMacroRTS()
{
    uint8_t u8LpUartChIdx;    //UART channel index
    uint32_t i;

    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        //skip reserved clock source case
//        if (u8LpUartChIdx == 4) continue;

        /* Test macro : LPUART_CLEAR_RTS
                        LPUART_SET_RTS */
        //disable RTS auto flow control
        LPUART0->INTEN &= ~LPUART_INTEN_ATORTSEN_Msk;
        LPUART_SET_RTS(LPUART0);

        while (!(LPUART0->MODEM & LPUART_MODEM_RTSSTS_Msk)) {};

        for (i = 0; i < 1000000; i++) {};

        CU_ASSERT((LPUART0->MODEM & (LPUART_MODEM_RTSSTS_Msk | LPUART_MODEM_RTSACTLV_Msk | LPUART_MODEM_RTS_Msk)) == (LPUART_MODEM_RTSSTS_Msk | LPUART_MODEM_RTSACTLV_Msk | LPUART_MODEM_RTS_Msk));

        LPUART_CLEAR_RTS(LPUART0);

        while ((LPUART0->MODEM & LPUART_MODEM_RTSSTS_Msk)) {};

        CU_ASSERT((LPUART0->MODEM & (LPUART_MODEM_RTSSTS_Msk | LPUART_MODEM_RTSACTLV_Msk | LPUART_MODEM_RTS_Msk)) == LPUART_MODEM_RTSACTLV_Msk);

    }
}
//void TestFunc_LPUART_SelectLINMode()
//{
//    uint8_t u8LpUartChIdx;        //UART channel index
//    uint8_t u8LinModeIdx;       //LIN mode
//    uint16_t u16LinBreakLen;    //LIN Break Length

//    /* Test loop */
//    for (u8LpUartChIdx = 1; u8LpUartChIdx < 2; u8LpUartChIdx++)
//    {
//        for (u8LinModeIdx = 0; u8LinModeIdx < 2; u8LinModeIdx++)
//        {
//            for (u16LinBreakLen = 1; u16LinBreakLen < 16; u16LinBreakLen++)
//            {
//                LPUART_SelectLINMode(LPUART0, LPUART_ALTCTL_LINTXEN_Msk, u16LinBreakLen);
//                LPUART0->DAT = 0x55;
//                LPUART_WAIT_TX_EMPTY(LPUART0) {};
//                CU_ASSERT(LPUART0->FUNCSEL == LPUART_FUNCSEL_LIN);
//                CU_ASSERT((LPUART0->ALTCTL & LPUART_ALTCTL_BRKFL_Msk) == u16LinBreakLen);

//                LPUART_SelectLINMode(LPUART0, LPUART_ALTCTL_LINRXEN_Msk, 0);
//                CU_ASSERT(LPUART0->FUNCSEL == LPUART_FUNCSEL_LIN);
//                CU_ASSERT((LPUART0->ALTCTL & LPUART_ALTCTL_LINRXEN_Msk) == LPUART_ALTCTL_LINRXEN_Msk);
//            }
//        }
//    }

//    //restore UART setting for other test
//    for (u8LpUartChIdx = 1; u8LpUartChIdx < LIN_CH_NUM_MAX; u8LpUartChIdx++)
//    {
//        LPUART0->FUNCSEL = LPUART_FUNCSEL_UART;
//        LPUART0->ALTCTL &= ~(LPUART_ALTCTL_LINTXEN_Msk | LPUART_ALTCTL_LINRXEN_Msk);
//    }

//}
void TestFunc_LPUART_TestMacroINT()
{
    uint8_t u8LpUartChIdx;    //UART channel index
    uint8_t u8UartINTEnIdx; //bit
    uint32_t i;

    //wait UART send message finish
    LPUART_WAIT_TX_EMPTY(UART0) {};

    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        for (u8UartINTEnIdx = 0; u8UartINTEnIdx < 10; u8UartINTEnIdx++)
        {
            /* Test macro : LPUART_ENABLE_INT
                            LPUART_DISABLE_INT */
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

            LPUART_ENABLE_INT(LPUART0, au32LpUartINTEnSel[u8UartINTEnIdx]);
            CU_ASSERT((LPUART0->INTEN & au32LpUartINTEnSel[u8UartINTEnIdx]) == au32LpUartINTEnSel[u8UartINTEnIdx]);
            LPUART_DISABLE_INT(LPUART0, au32LpUartINTEnSel[u8UartINTEnIdx]);
            CU_ASSERT((LPUART0->INTEN & au32LpUartINTEnSel[u8UartINTEnIdx]) == 0);
        }
    }

    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        /* Test macro : LPUART_GET_INT_FLAG */

        //enable UART1 internal loopback
        LPUART0->MODEM |= 0x10;

        //RX FIFO reset
        LPUART0->FIFO |= LPUART_FIFO_RXRST_Msk;

        while ((LPUART0->FIFO & LPUART_FIFO_RXRST_Msk) == LPUART_FIFO_RXRST_Msk);

        //0 : Receive data available interrupt
        //1 : Transmit holding register empty interrupt
        // set CTS low level active
        LPUART0->MODEMSTS |= LPUART_MODEMSTS_CTSACTLV_Msk;
        // enable CTS auto flow control
        LPUART0->INTEN |= LPUART_INTEN_ATOCTSEN_Msk;
        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_THREIF_Msk) == 1);
        // fill Tx
        LPUART_WRITE(LPUART0, 0x55);
        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_THREIF_Msk) == 0);
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_RDAIF_Msk) == 0);
        // set CTS high level active to release data in Tx FIFO
        LPUART0->MODEMSTS &= ~LPUART_MODEMSTS_CTSACTLV_Msk;

        //LPUART0->MODEMSTS |= LPUART_MODEMSTS_CTSACTLV_Msk;
        // wait data send out
        while ((LPUART0->FIFOSTS & LPUART_FIFOSTS_TXEMPTYF_Msk) == 0) {};

        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_THREIF_Msk) == 1);

        // wait data receive
        while ((LPUART0->FIFOSTS & LPUART_FIFOSTS_RXEMPTY_Msk) == LPUART_FIFOSTS_RXEMPTY_Msk) {};

        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_RDAIF_Msk) == 1);

        // read Rx
        LPUART_READ(LPUART0);

        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_RDAIF_Msk) == 0);

        // disable CTS auto flow control after test
        LPUART0->INTEN &= ~LPUART_INTEN_ATOCTSEN_Msk;

        //2 : Receive line status interrupt
        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_RLSIF_Msk) == 0);

        // send break
        LPUART0->LINE |= LPUART_LINE_BCB_Msk;

        // wait break error happen
        while ((LPUART0->FIFOSTS & LPUART_FIFOSTS_BIF_Msk) == 0) {};

        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_RLSIF_Msk) == 1);

        // stop send break
        LPUART0->LINE &= ~LPUART_LINE_BCB_Msk;

        // clear receive line status flag
        LPUART_ClearIntFlag(LPUART0, LPUART_INTSTS_RLSINT_Msk);

        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_RLSIF_Msk) == 0);

        // clear Rx FIFO
        LPUART0->FIFO |= LPUART_FIFO_RXRST_Msk;

        while ((LPUART0->FIFO & LPUART_FIFO_RXRST_Msk) == LPUART_FIFO_RXRST_Msk) {};

        //3 : Modem status interrupt
        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_MODEMIF_Msk) == 1);

        // clear CTS flag

        LPUART0->MODEMSTS |= LPUART_MODEMSTS_CTSDETF_Msk;

        for (i = 0; i < 10000; i++) __NOP();

        LPUART0->MODEMSTS |= LPUART_MODEMSTS_CTSDETF_Msk;

        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_MODEMIF_Msk) == 0);

        //4 : Rx time-out interrupt
        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_RXTOIF_Msk) == 0);
        // enable time out counter
        LPUART0->INTEN |= LPUART_INTEN_TOCNTEN_Msk;
        // send Tx
        LPUART_WRITE(LPUART0, 0x55);

        // wait Rx time out happen
        while ((LPUART0->INTSTS & LPUART_INTSTS_RXTOIF_Msk) == 0) {};

        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_RXTOIF_Msk) == 1);

        // disable time out counter after finish test
        LPUART0->INTEN &= ~LPUART_INTEN_TOCNTEN_Msk;

        // read Rx
        LPUART_READ(LPUART0);

        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_RXTOINT_Msk) == 0);

        //5 : Buffer error interrupt
        // check macro
        if (u8LpUartChIdx != 4)
            CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_BUFERRIF_Msk) == 0);

        // send data and wait Rx buffer error flag happen
        while ((LPUART0->INTSTS & LPUART_INTSTS_BUFERRIF_Msk) == 0)
        {
            // send Tx
            LPUART_WRITE(LPUART0, 0x55);
        }

        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_BUFERRIF_Msk) == 1);
        // clear Rx buffer error flag
        LPUART0->FIFOSTS |= LPUART_FIFOSTS_RXOVIF_Msk;

        while ((LPUART0->FIFOSTS & LPUART_FIFOSTS_RXOVIF_Msk) == LPUART_FIFOSTS_RXOVIF_Msk) {};

        // check macro
        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_BUFERRIF_Msk) == 0);

        // clear Rx FIFO
        LPUART0->FIFO |= LPUART_FIFO_RXRST_Msk;

        while ((LPUART0->FIFOSTS & LPUART_FIFO_RXRST_Msk) == LPUART_FIFO_RXRST_Msk) {};

        /* Test macro : LPUART_RS485_CLEAR_ADDR_FLAG
                        LPUART_RS485_GET_ADDR_FLAG */
        // check macro
        CU_ASSERT(LPUART_RS485_GET_ADDR_FLAG(LPUART0) == 0);

        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_RLSINT_Msk) == 0);

        // set RS485 mode
        LPUART0->FUNCSEL = LPUART_FUNCSEL_RS485;

        // set RS485 address detection enable
        LPUART0->ALTCTL |= (LPUART_ALTCTL_ADDRDEN_Msk | LPUART_ALTCTL_RS485NMM_Msk);

        // set parity 1
        LPUART0->LINE = LPUART_WORD_LEN_8 | LPUART_PARITY_ODD | LPUART_STOP_BIT_1;

        // send Tx
        LPUART_WRITE(LPUART0, 0x55);

        // wait RS485 address flag happen
        while ((LPUART0->FIFOSTS & LPUART_FIFOSTS_ADDRDETF_Msk) == 0) {};

        // check macro
        CU_ASSERT(LPUART_RS485_GET_ADDR_FLAG(LPUART0) == 1);

        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_RLSIF_Msk) == 1);

        // clear RS485 address flag
        LPUART_RS485_CLEAR_ADDR_FLAG(LPUART0);

        // check macro
        CU_ASSERT(LPUART_RS485_GET_ADDR_FLAG(LPUART0) == 0);

        CU_ASSERT(LPUART_GET_INT_FLAG(LPUART0, LPUART_INTSTS_RLSIF_Msk) == 0);

        LPUART0->ALTCTL = 0xC;


        //disable UART internal loopback mode after test */
        LPUART0->MODEM &= ~0x10;

        LPUART0->FUNCSEL = LPUART_FUNCSEL_LPUART;

        LPUART0->LINE = LPUART_WORD_LEN_8 | LPUART_PARITY_NONE | LPUART_STOP_BIT_1;

        LPUART0->ALTCTL &= ~(LPUART_ALTCTL_ADDRDEN_Msk | LPUART_ALTCTL_RS485NMM_Msk);
    }

}


const uint32_t au32UartRxTrgSel[4] =
{
    LPUART_FIFO_RFITL_1BYTE,
    LPUART_FIFO_RFITL_4BYTES,
    LPUART_FIFO_RFITL_8BYTES,
    LPUART_FIFO_RFITL_14BYTES,
};

const uint32_t au32UartRTSTrgSel[4] =
{
    LPUART_FIFO_RTSTRGLV_1BYTE,
    LPUART_FIFO_RTSTRGLV_4BYTES,
    LPUART_FIFO_RTSTRGLV_8BYTES,
    LPUART_FIFO_RTSTRGLV_14BYTES,
};

void TestFunc_LPUART_TestConstant()
{
    uint8_t u8LpUartChIdx;        //UART channel index
    uint16_t u16TestIdx;
    uint32_t i;

    //wait UART send message finish
    LPUART_WAIT_TX_EMPTY(UART0) {};

    // Only uart0/uart1 support LIN mode for TC8237
    // Only uart0 support LIN mode for M252

    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        //select Rx FIFO trigger level
        for (u16TestIdx = 0; u16TestIdx < 4; u16TestIdx++)
        {
            LPUART0->FIFO = ((LPUART0->FIFO & (~LPUART_FIFO_RFITL_Msk)) | au32UartRxTrgSel[u16TestIdx]);

            if (u8LpUartChIdx != 4)
            {
                CU_ASSERT((LPUART0->FIFO & LPUART_FIFO_RFITL_Msk) == au32UartRxTrgSel[u16TestIdx]);
            }
            else
            {
                CU_ASSERT((LPUART0->FIFO & LPUART_FIFO_RFITL_Msk) == au32UartRxTrgSel[0]);
            }

        }

        //select RTS trigger level index
        for (u16TestIdx = 0; u16TestIdx < 4; u16TestIdx++)
        {
            LPUART0->FIFO = ((LPUART0->FIFO & (~LPUART_FIFO_RTSTRGLV_Msk)) | au32UartRTSTrgSel[u16TestIdx]);

            if (u8LpUartChIdx != 4)
            {
                CU_ASSERT((LPUART0->FIFO & LPUART_FIFO_RTSTRGLV_Msk) == au32UartRTSTrgSel[u16TestIdx]);
            }
            else
            {
                CU_ASSERT((LPUART0->FIFO & LPUART_FIFO_RTSTRGLV_Msk) == au32UartRTSTrgSel[0]);
            }
        }

        //select RTS active level
        LPUART0->MODEM = ((LPUART0->MODEM & (~LPUART_MODEM_RTSACTLV_Msk)) | LPUART_RTS_IS_LOW_LEV_ACTIVE);
        CU_ASSERT((LPUART0->MODEM & LPUART_MODEM_RTSACTLV_Msk) == LPUART_RTS_IS_LOW_LEV_ACTIVE);

        LPUART0->MODEM = ((LPUART0->MODEM & (~LPUART_MODEM_RTSACTLV_Msk)) | LPUART_RTS_IS_HIGH_LEV_ACTIVE);
        CU_ASSERT((LPUART0->MODEM & LPUART_MODEM_RTSACTLV_Msk) == LPUART_RTS_IS_HIGH_LEV_ACTIVE);
    }

    CU_ASSERT(LPUART0_FIFO_SIZE == 16);

    LPUART_IPReset(LPUART0);

}

void TestFunc_LPUART_ReadWrite()
{
    uint8_t u8LpUartChIdx, i;        //UART channel index
    uint8_t u8TxData[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t u8RxData[8] = {0};

    /* Test loop */
    for (u8LpUartChIdx = 0; u8LpUartChIdx < LPUART_CH_NUM_MAX; u8LpUartChIdx++)
    {
        LPUART0->MODEM |= 0x10; // set loop back mode

        /* Configure UART0 and set UART0 baud rate */
        LPUART0->BAUD = LPUART_BAUD_MODE2 | LPUART_BAUD_MODE2_DIVIDER(__HIRC, 115200);
        LPUART0->LINE = LPUART_WORD_LEN_8 | LPUART_PARITY_NONE | LPUART_STOP_BIT_1;

        /* Reset Tx/Rx FIFO */
        LPUART0->FIFO |= 0x6;

        while (LPUART0->FIFO & 0x6);

        if (u8LpUartChIdx != 4)
        {
            /* Write data */
            LPUART_Write(LPUART0, u8TxData, 8);
            /* Read data */
            LPUART_Read(LPUART0, u8RxData, 8);

            for (i = 0; i < 8; i++)
            {
                CU_ASSERT(u8TxData[i] == u8RxData[i]);
            }
        }
        else
        {
            /* Write data */
            LPUART_Write(LPUART0, u8TxData, 1);
            /* Read data */
            LPUART_Read(LPUART0, u8RxData, 1);

            CU_ASSERT(u8TxData[0] == u8RxData[0]);

        }

    }

}


CU_TestInfo LPUART_ApiTests[] =
{
    {"LPUART_Enable/DisableFlowCtrl Function.", TestFunc_LPUART_EnableFlowCtrl},
    {"LPUART_Enable/DisableInt Function.", TestFunc_LPUART_EnableInt},
    {"LPUART_Open Function.", TestFunc_LPUART_Open},
    {"LPUART_SetLine_Config Function.", TestFunc_LPUART_SetLine_Config},
    {"LPUART_SetTimeoutCnt Function.", TestFunc_LPUART_SetTimeoutCnt},
    {"LPUART_SelectRS485Mode Function.", TestFunc_LPUART_SelectRS485Mode},
    {"Testing LPUART_ReadWrite Function.", TestFunc_LPUART_ReadWrite},
    CU_TEST_INFO_NULL
};




CU_TestInfo LPUART_MacroTests[] =
{
    {"Macro about baud rate.", TestFunc_LPUART_TestMacroBR},
    {"Macro about FIFO.", TestFunc_LPUART_TestMacroFIFO},
    {"Macro about RTS.", TestFunc_LPUART_TestMacroRTS},
    {"Macro about interrupt.", TestFunc_LPUART_TestMacroINT},
    {"Constant.", TestFunc_LPUART_TestConstant},

    CU_TEST_INFO_NULL
};

