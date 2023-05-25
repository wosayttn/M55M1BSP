/*---------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/* Copyright (C) 2020 Nuvoton Technology Corp.                                                             */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   uuart_cunit_test.c                                                                                    */
/*            The test function of VIC for CUnit.                                                          */
/* Project:   M253                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "usci_uart_cunit.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

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

CU_SuiteInfo UUART_suites[] =
{
    {"UUART API", suite_success_init, suite_success_clean, NULL, NULL, UUART_ApiTests},
    {"UUART MACRO", suite_success_init, suite_success_clean, NULL, NULL, UUART_MacroTests},
    CU_SUITE_INFO_NULL
};



#define UUART_CH_NUM_MAX 1
UUART_T *UUartCh[] =
{
    UUART0,  //skip debug port UART0 test
};

void TestFunc_UUART_EnableInt()
{
    uint8_t u8UartChIdx;    //UART channel index

    /* test loop */
    for (u8UartChIdx = 0; u8UartChIdx < UUART_CH_NUM_MAX; u8UartChIdx++)
    {
        UUART_Open(UUartCh[u8UartChIdx], 115200);

        /* Enable USCI internal loopback for test */
        outpw(((uint32_t)UUartCh[u8UartChIdx]) + 0x84, (1 << 30));


        UUART_EnableInt(UUartCh[u8UartChIdx], UUART_ABR_INT_MASK);
        CU_ASSERT_EQUAL((UUartCh[u8UartChIdx]->PROTIEN & UUART_PROTIEN_ABRIEN_Msk), UUART_PROTIEN_ABRIEN_Msk);
        UUART_EnableInt(UUartCh[u8UartChIdx], UUART_RLS_INT_MASK);
        CU_ASSERT_EQUAL((UUartCh[u8UartChIdx]->PROTIEN & UUART_PROTIEN_RLSIEN_Msk), UUART_PROTIEN_RLSIEN_Msk);
        UUART_EnableInt(UUartCh[u8UartChIdx], UUART_BUF_RXOV_INT_MASK);
        CU_ASSERT_EQUAL((UUartCh[u8UartChIdx]->BUFCTL & UUART_BUFCTL_RXOVIEN_Msk), UUART_BUFCTL_RXOVIEN_Msk);
        UUART_EnableInt(UUartCh[u8UartChIdx], UUART_TXST_INT_MASK);
        CU_ASSERT_EQUAL((UUartCh[u8UartChIdx]->INTEN & UUART_INTEN_TXSTIEN_Msk), UUART_INTEN_TXSTIEN_Msk);
        UUART_EnableInt(UUartCh[u8UartChIdx], UUART_TXEND_INT_MASK);
        CU_ASSERT_EQUAL((UUartCh[u8UartChIdx]->INTEN & UUART_INTEN_TXENDIEN_Msk), UUART_INTEN_TXENDIEN_Msk);
        UUART_EnableInt(UUartCh[u8UartChIdx], UUART_RXST_INT_MASK);
        CU_ASSERT_EQUAL((UUartCh[u8UartChIdx]->INTEN & UUART_INTEN_RXSTIEN_Msk), UUART_INTEN_RXSTIEN_Msk);
        UUART_EnableInt(UUartCh[u8UartChIdx], UUART_RXEND_INT_MASK);
        CU_ASSERT_EQUAL((UUartCh[u8UartChIdx]->INTEN & UUART_INTEN_RXENDIEN_Msk), UUART_INTEN_RXENDIEN_Msk);


        UUART_DisableInt(UUartCh[u8UartChIdx], UUART_RLS_INT_MASK);
        CU_ASSERT_EQUAL((UUartCh[u8UartChIdx]->PROTIEN & 0x06), UUART_PROTIEN_ABRIEN_Msk);
        UUART_DisableInt(UUartCh[u8UartChIdx], UUART_ABR_INT_MASK);
        CU_ASSERT_EQUAL((UUartCh[u8UartChIdx]->PROTIEN & 0x06), 0);

        UUART_DisableInt(UUartCh[u8UartChIdx], UUART_BUF_RXOV_INT_MASK);
        CU_ASSERT_EQUAL((UUartCh[u8UartChIdx]->BUFCTL & UUART_BUFCTL_RXOVIEN_Msk), 0);
        UUART_DisableInt(UUartCh[u8UartChIdx], UUART_RXEND_INT_MASK);
        CU_ASSERT_EQUAL((UUartCh[u8UartChIdx]->INTEN & 0x1E), (UUART_INTEN_RXSTIEN_Msk | UUART_INTEN_TXENDIEN_Msk | UUART_INTEN_TXSTIEN_Msk));
        UUART_DisableInt(UUartCh[u8UartChIdx], UUART_RXST_INT_MASK);
        CU_ASSERT_EQUAL((UUartCh[u8UartChIdx]->INTEN & 0x1E), (UUART_INTEN_TXENDIEN_Msk | UUART_INTEN_TXSTIEN_Msk));
        UUART_DisableInt(UUartCh[u8UartChIdx], UUART_TXEND_INT_MASK);
        CU_ASSERT_EQUAL((UUartCh[u8UartChIdx]->INTEN & 0x1E), UUART_INTEN_TXSTIEN_Msk);
        UUART_DisableInt(UUartCh[u8UartChIdx], UUART_TXST_INT_MASK);
        CU_ASSERT_EQUAL((UUartCh[u8UartChIdx]->INTEN & 0x1E), 0);
    }

    for (u8UartChIdx = 0; u8UartChIdx < UUART_CH_NUM_MAX; u8UartChIdx++)
    {
        UUartCh[u8UartChIdx]->BUFCTL |= UUART_BUFCTL_RXRST_Msk;

        while ((UUartCh[u8UartChIdx]->BUFCTL & UUART_BUFCTL_RXRST_Msk) == UUART_BUFCTL_RXRST_Msk);

        // fill Tx
        UUART_WRITE(UUartCh[u8UartChIdx], 0x55);

        // wait data send out
        while ((UUartCh[u8UartChIdx]->PROTSTS & UUART_PROTSTS_TXENDIF_Msk) == 0) {}

        // check INT flag
        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_TXST_INT_MASK), UUART_TXST_INT_MASK);
        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_TXEND_INT_MASK), UUART_TXEND_INT_MASK);

        UUART_ClearIntFlag(UUartCh[u8UartChIdx], UUART_TXST_INT_MASK);
        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_TXST_INT_MASK), 0);
        UUART_ClearIntFlag(UUartCh[u8UartChIdx], UUART_TXEND_INT_MASK);
        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_TXEND_INT_MASK), 0);

        // wait data receive
        while ((UUartCh[u8UartChIdx]->PROTSTS & UUART_PROTSTS_RXENDIF_Msk) == 0) {}

        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_RXST_INT_MASK), UUART_RXST_INT_MASK);
        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_RXEND_INT_MASK), UUART_RXEND_INT_MASK);

        UUART_ClearIntFlag(UUartCh[u8UartChIdx], UUART_RXST_INT_MASK);
        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_RXST_INT_MASK), 0);
        UUART_ClearIntFlag(UUartCh[u8UartChIdx], UUART_RXEND_INT_MASK);
        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_RXEND_INT_MASK), 0);

        UUartCh[u8UartChIdx]->BUFCTL |= UUART_BUFCTL_RXRST_Msk;

        while ((UUartCh[u8UartChIdx]->BUFCTL & UUART_BUFCTL_RXRST_Msk) == UUART_BUFCTL_RXRST_Msk);

        // check Break Flag
        UUartCh[u8UartChIdx]->PROTCTL |= UUART_PROTCTL_BCEN_Msk;

        while ((UUartCh[u8UartChIdx]->PROTSTS & UUART_PROTSTS_BREAK_Msk) != UUART_PROTSTS_BREAK_Msk);

        UUartCh[u8UartChIdx]->PROTCTL &= ~UUART_PROTCTL_BCEN_Msk;

        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_RLS_INT_MASK), UUART_RLS_INT_MASK);

        UUART_ClearIntFlag(UUartCh[u8UartChIdx], UUART_RLS_INT_MASK);
        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_RLS_INT_MASK), 0);
        UUART_READ(UUartCh[u8UartChIdx]);

        // check Framing Error Flag
        UUartCh[u8UartChIdx]->PROTCTL |= UUART_PROTCTL_BCEN_Msk;

        while ((UUartCh[u8UartChIdx]->PROTSTS & UUART_PROTSTS_BREAK_Msk) != UUART_PROTSTS_BREAK_Msk);

        UUartCh[u8UartChIdx]->PROTCTL &= ~UUART_PROTCTL_BCEN_Msk;

        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_RLS_INT_MASK), UUART_RLS_INT_MASK);

        UUART_ClearIntFlag(UUartCh[u8UartChIdx], UUART_RLS_INT_MASK);
        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_RLS_INT_MASK), 0);
        UUART_READ(UUartCh[u8UartChIdx]);

        // check Parity Error Flag
        UUartCh[u8UartChIdx]->PROTCTL |= UUART_PROTCTL_PARITYEN_Msk;
        UUartCh[u8UartChIdx]->PROTCTL |= UUART_PROTCTL_BCEN_Msk;

        while ((UUartCh[u8UartChIdx]->PROTSTS & UUART_PROTSTS_BREAK_Msk) != UUART_PROTSTS_BREAK_Msk);

        UUartCh[u8UartChIdx]->PROTCTL &= ~(UUART_PROTCTL_BCEN_Msk | UUART_PROTCTL_PARITYEN_Msk);

        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_RLS_INT_MASK), UUART_RLS_INT_MASK);

        UUART_ClearIntFlag(UUartCh[u8UartChIdx], UUART_RLS_INT_MASK);
        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_RLS_INT_MASK), 0);
        UUART_READ(UUartCh[u8UartChIdx]);

        UUartCh[u8UartChIdx]->BUFCTL |= UUART_BUFCTL_RXRST_Msk;

        while ((UUartCh[u8UartChIdx]->BUFCTL & UUART_BUFCTL_RXRST_Msk) == UUART_BUFCTL_RXRST_Msk);

        // fill Tx
        UUART_CLR_PROT_INT_FLAG(UUartCh[u8UartChIdx], UUART_PROTSTS_TXENDIF_Msk);
        UUART_WRITE(UUartCh[u8UartChIdx], 0x56);

        while ((UUartCh[u8UartChIdx]->PROTSTS & UUART_PROTSTS_TXENDIF_Msk) == 0);

        // fill Tx
        UUART_CLR_PROT_INT_FLAG(UUartCh[u8UartChIdx], UUART_PROTSTS_TXENDIF_Msk);
        UUART_WRITE(UUartCh[u8UartChIdx], 0x57);

        while ((UUartCh[u8UartChIdx]->BUFSTS & UUART_BUFSTS_RXFULL_Msk) == 0);

        // check Receive Buffer Over-run Error Flag
        UUART_CLR_PROT_INT_FLAG(UUartCh[u8UartChIdx], UUART_PROTSTS_TXENDIF_Msk);
        UUART_WRITE(UUartCh[u8UartChIdx], 0x58);

        while ((UUartCh[u8UartChIdx]->PROTSTS & UUART_PROTSTS_TXENDIF_Msk) == 0) {}

        while ((UUartCh[u8UartChIdx]->BUFSTS & UUART_BUFSTS_RXOVIF_Msk) == 0) {}

        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_BUF_RXOV_INT_MASK), UUART_BUF_RXOV_INT_MASK);

        UUART_ClearIntFlag(UUartCh[u8UartChIdx], UUART_BUF_RXOV_INT_MASK);
        CU_ASSERT_EQUAL(UUART_GetIntFlag(UUartCh[u8UartChIdx], UUART_BUF_RXOV_INT_MASK), 0);

    }

}

const uint32_t au32UUartBRSel[] =
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

const uint32_t au32HclkClkSel[] =
{
    CLK_SCLKSEL_SCLKSEL_HIRC,
//    CLK_SCLKSEL_SCLKSEL_MIRC,
//    CLK_SCLKSEL_SCLKSEL_HIRC48M,
    CLK_SCLKSEL_SCLKSEL_HXT,
//    CLK_SCLKSEL_SCLKSEL_APLL0,
    
};

void TestFunc_UUART_Open()
{
    uint8_t u8UUartChIdx;    //UART channel index
    uint8_t u8UartBRIdx;    //baud rate index
    uint8_t u8UartClkIdx;   //clock source index
    uint32_t u32baudrate;
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Test loop */
    //select UART channel
    for (u8UUartChIdx = 0; u8UUartChIdx < UUART_CH_NUM_MAX; u8UUartChIdx++)
    {
        //select HCLK clock source
        for (u8UartClkIdx = 0; u8UartClkIdx < (sizeof(au32HclkClkSel) / sizeof(uint32_t)); u8UartClkIdx++)
        {
            //select UART clock source
//            CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLKSEL_Msk)) | au32HclkClkSel[u8UartClkIdx];
               /* Switch SCLK clock source to PLL0 and divide 1 */
              CLK_SetSCLK(au32HclkClkSel[u8UartClkIdx], CLK_ACLKDIV_ACLKDIV(1));
            //select UART baud rate
            for (u8UartBRIdx = 0; u8UartBRIdx < (sizeof(au32UUartBRSel) / sizeof(uint32_t)); u8UartBRIdx++)
            {
                //test function
                u32baudrate = UUART_Open(UUartCh[u8UUartChIdx], au32UUartBRSel[u8UartBRIdx]);
                //printf("%d=%d\n",  au32UUartBRSel[u8UartBRIdx], u32baudrate);

                //check
                CU_ASSERT(UUartCh[u8UUartChIdx]->CTL == 2);
                CU_ASSERT(UUartCh[u8UUartChIdx]->LINECTL == (UUART_WORD_LEN_8 | UUART_LINECTL_LSB_Msk));
                CU_ASSERT(u32baudrate > (((au32UUartBRSel[u8UartBRIdx]) * 95) / 100) &&
                          u32baudrate < (((au32UUartBRSel[u8UartBRIdx]) * 105) / 100));
            }

        }
    }

    //select HCLK clock source
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_HIRC, CLK_ACLKDIV_ACLKDIV(1));
      /* Lock protected registers */
    SYS_LockReg();
}

const uint32_t au32UUartWordSel[4] =
{
    UUART_WORD_LEN_6,
    UUART_WORD_LEN_7,
    UUART_WORD_LEN_8,
    UUART_WORD_LEN_9,
};

const uint32_t au32UUartParitySel[3] =
{
    UUART_PARITY_NONE,
    UUART_PARITY_ODD,
    UUART_PARITY_EVEN,
};

const uint32_t au32UUartStopSel[2] =
{
    UUART_STOP_BIT_1,
    UUART_STOP_BIT_2,
};

void TestFunc_UUART_SetLine_Config()
{
    //variable for baud rate
    uint8_t u8UUartChIdx;    //channel index
    uint8_t u8UartBRIdx;    //baud rate index
    uint8_t u8UartClkIdx;   //clock source index

    //variable for data format
    uint8_t u8UartWordIdx;      //word length index
    uint8_t u8UartParityIdx;    //parity index
    uint8_t u8UartStopIdx;      //stop bit length index
    uint32_t u32baudrate;
     /* Unlock protected registers */
    SYS_UnlockReg();
    /* Test loop */
    //select UART channel
    for (u8UUartChIdx = 0; u8UUartChIdx < UUART_CH_NUM_MAX; u8UUartChIdx++)
    {
        //select UART baud rate
        for (u8UartBRIdx = 7; u8UartBRIdx < 8; u8UartBRIdx++)
        {
            //select HCLK clock source
            for (u8UartClkIdx = 0; u8UartClkIdx < (sizeof(au32HclkClkSel) / sizeof(uint32_t)); u8UartClkIdx++)
            {
                //select UART clock source
//                CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLKSEL_Msk)) | au32HclkClkSel[u8UartClkIdx];
                              CLK_SetSCLK(au32HclkClkSel[u8UartClkIdx], CLK_ACLKDIV_ACLKDIV(1));
                //select UART word length
                for (u8UartWordIdx = 0; u8UartWordIdx < (sizeof(au32UUartWordSel) / sizeof(uint32_t)); u8UartWordIdx++)
                {
                    //select UART parity
                    for (u8UartParityIdx = 0; u8UartParityIdx < (sizeof(au32UUartParitySel) / sizeof(uint32_t)); u8UartParityIdx++)
                    {
                        //select UART stop bit length
                        for (u8UartStopIdx = 0; u8UartStopIdx < (sizeof(au32UUartStopSel) / sizeof(uint32_t)); u8UartStopIdx++)
                        {
                            //test function
                            u32baudrate = UUART_SetLine_Config(UUartCh[u8UUartChIdx],    //select UART channel
                                                               au32UUartBRSel[u8UartBRIdx],           //select UART baud rate
                                                               au32UUartWordSel[u8UartWordIdx],       //select UART word length
                                                               au32UUartParitySel[u8UartParityIdx],   //select UART parity
                                                               au32UUartStopSel[u8UartStopIdx]);      //select UART stop bit length
                            //check
                            CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->LINECTL & UUART_LINECTL_DWIDTH_Msk, au32UUartWordSel[u8UartWordIdx]);
                            CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->PROTCTL & (UUART_PROTCTL_STICKEN_Msk | UUART_PROTCTL_EVENPARITY_Msk |
                                                                              UUART_PROTCTL_PARITYEN_Msk), au32UUartParitySel[u8UartParityIdx]);
                            CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->PROTCTL & UUART_PROTCTL_STOPB_Msk, au32UUartStopSel[u8UartStopIdx]);

                            CU_ASSERT(u32baudrate > (((au32UUartBRSel[u8UartBRIdx]) * 95) / 100) &&
                                      u32baudrate < (((au32UUartBRSel[u8UartBRIdx]) * 105) / 100));

                        } //u8UartStopIdx
                    } //u8UartParityIdx
                } //u8UartWordIdx
            } //u8UartClkIdx
        } //u8UartBRIdx
    } //u8UartChIdx

    //select HCLK clock source
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_HIRC, CLK_ACLKDIV_ACLKDIV(1));
        /* Lock protected registers */
    SYS_LockReg();
}

void TestFunc_UUART_TestMacroBUF()
{
    uint8_t u8UartChIdx;    //UART channel index
    uint8_t i;              //number control

    //wait UART send message finish
    UUART_WAIT_TX_EMPTY(UUART0);

    /* Set UART1 internal loopback mode */
    for (u8UartChIdx = 0; u8UartChIdx < UUART_CH_NUM_MAX; u8UartChIdx++)
    {
        UUartCh[u8UartChIdx]->BUFCTL |= UUART_BUFCTL_RXRST_Msk;

        while ((UUartCh[u8UartChIdx]->BUFCTL & UUART_BUFCTL_RXRST_Msk) == UUART_BUFCTL_RXRST_Msk);

        ///UUART_Open(UUartCh[u8UartChIdx], 115200);
        UUART_Open(UUartCh[u8UartChIdx], 9600);

        /* Enable USCI internal loopback for test */
        outpw(((uint32_t)UUartCh[u8UartChIdx]) + 0x84, (1 << 30));
    }

    /* Test macro : UART_WRITE
                    UUART_READ
                    UUART_GET_TX_EMPTY
                    UUART_GET_RX_EMPTY
                    UUART_IS_TX_EMPTY
                    UUART_IS_RX_EMPTY
                    UUART_WAIT_TX_EMPTY
                    UUART_IS_TX_FULL
                    UUART_IS_RX_FULL
                    UUART_GET_TX_FULL
                    UUART_GET_RX_FULL */
    for (u8UartChIdx = 0; u8UartChIdx < UUART_CH_NUM_MAX; u8UartChIdx++)
    {
        //check initial state
        CU_ASSERT(UUART_GET_TX_EMPTY(UUartCh[u8UartChIdx]) == UUART_BUFSTS_TXEMPTY_Msk);
        CU_ASSERT(UUART_GET_RX_EMPTY(UUartCh[u8UartChIdx]) == UUART_BUFSTS_RXEMPTY_Msk);
        CU_ASSERT(UUART_IS_TX_EMPTY(UUartCh[u8UartChIdx]) == 1);
        CU_ASSERT(UUART_IS_RX_EMPTY(UUartCh[u8UartChIdx]) == 1);
        UUART_WAIT_TX_EMPTY(UUartCh[u8UartChIdx]) {}
        CU_ASSERT(UUART_IS_TX_FULL(UUartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UUART_IS_RX_FULL(UUartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UUART_GET_TX_FULL(UUartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UUART_GET_RX_FULL(UUartCh[u8UartChIdx]) == 0);

        //check Tx full state
        //set CTS low level active
        //UUartCh[u8UartChIdx]->CTLIN0 |= UUART_CTLIN0_ININV_Msk;
        //enable CTS auto flow control
        UUartCh[u8UartChIdx]->PROTCTL |= UUART_PROTCTL_CTSAUTOEN_Msk;
        //send data to let Tx full
        UUART_WRITE(UUartCh[u8UartChIdx], (0x55));

        //check macro
        CU_ASSERT(UUART_GET_TX_EMPTY(UUartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UUART_GET_RX_EMPTY(UUartCh[u8UartChIdx]) == UUART_BUFSTS_RXEMPTY_Msk);
        CU_ASSERT(UUART_IS_TX_EMPTY(UUartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UUART_IS_RX_EMPTY(UUartCh[u8UartChIdx]) == 1);
        CU_ASSERT(UUART_IS_TX_FULL(UUartCh[u8UartChIdx]) == 1);
        CU_ASSERT(UUART_IS_RX_FULL(UUartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UUART_GET_TX_FULL(UUartCh[u8UartChIdx]) == UUART_BUFSTS_TXFULL_Msk);
        CU_ASSERT(UUART_GET_RX_FULL(UUartCh[u8UartChIdx]) == 0);

        //set CTS low level active
        UUartCh[u8UartChIdx]->CTLIN0 &= ~UUART_CTLIN0_ININV_Msk;

        UUART_WAIT_TX_EMPTY(UUartCh[u8UartChIdx]);
        UUART_WRITE(UUartCh[u8UartChIdx], (0x56));

        //check Rx full state
        //wait Tx send finished
        UUART_WAIT_TX_EMPTY(UUartCh[u8UartChIdx]) {}
        //check macro
        CU_ASSERT(UUART_GET_TX_EMPTY(UUartCh[u8UartChIdx]) == UUART_BUFSTS_TXEMPTY_Msk);
        CU_ASSERT(UUART_GET_RX_EMPTY(UUartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UUART_IS_TX_EMPTY(UUartCh[u8UartChIdx]) == 1);
        CU_ASSERT(UUART_IS_RX_EMPTY(UUartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UUART_IS_TX_FULL(UUartCh[u8UartChIdx]) == 0);

        while ((UUartCh[u8UartChIdx]->BUFSTS & UUART_BUFSTS_RXFULL_Msk) == 0) {}

        CU_ASSERT(UUART_IS_RX_FULL(UUartCh[u8UartChIdx]) == 1);
        CU_ASSERT(UUART_GET_TX_FULL(UUartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UUART_GET_RX_FULL(UUartCh[u8UartChIdx]) == UUART_BUFSTS_RXFULL_Msk);

        //read Rx empty
        for (i = 0; i < 2; i++)
        {
            UUART_READ(UUartCh[u8UartChIdx]);
        }

        //check macro
        CU_ASSERT(UUART_GET_TX_EMPTY(UUartCh[u8UartChIdx]) == UUART_BUFSTS_TXEMPTY_Msk);
        CU_ASSERT(UUART_GET_RX_EMPTY(UUartCh[u8UartChIdx]) == UUART_BUFSTS_RXEMPTY_Msk);
        CU_ASSERT(UUART_IS_TX_EMPTY(UUartCh[u8UartChIdx]) == 1);
        CU_ASSERT(UUART_IS_RX_EMPTY(UUartCh[u8UartChIdx]) == 1);
        UUART_WAIT_TX_EMPTY(UUartCh[u8UartChIdx]) {}
        CU_ASSERT(UUART_IS_TX_FULL(UUartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UUART_IS_RX_FULL(UUartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UUART_GET_TX_FULL(UUartCh[u8UartChIdx]) == 0);
        CU_ASSERT(UUART_GET_RX_FULL(UUartCh[u8UartChIdx]) == 0);

        //disable CTS auto flow control
        UUartCh[u8UartChIdx]->PROTCTL &= ~UUART_PROTCTL_CTSAUTOEN_Msk;
    }

}

void TestFunc_UUART_TestMacroINT()
{
    uint8_t u8UUartChIdx;    //UART channel index

    /* Reset USCI */
    SYS_ResetModule(SYS_USCI0RST);

    for (u8UUartChIdx = 0; u8UUartChIdx < UUART_CH_NUM_MAX; u8UUartChIdx++)
    {
        UUART_Open(UUartCh[u8UUartChIdx], 115200);

        /* Enable USCI internal loopback for test */
        outpw(((uint32_t)UUartCh[u8UUartChIdx]) + 0x84, (1 << 30));

        //protocol interrupt
        UUART_ENABLE_PROT_INT(UUartCh[u8UUartChIdx], UUART_PROTIEN_RLSIEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->PROTIEN & UUART_PROTIEN_RLSIEN_Msk, UUART_PROTIEN_RLSIEN_Msk);
        UUART_ENABLE_PROT_INT(UUartCh[u8UUartChIdx], UUART_PROTIEN_ABRIEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->PROTIEN & UUART_PROTIEN_ABRIEN_Msk, UUART_PROTIEN_ABRIEN_Msk);
        //UUART_ENABLE_PROT_INT(UUartCh[u8UartChIdx], UUART_PROTIEN_BRKIEN_Msk);
        //CU_ASSERT_EQUAL(UUartCh[u8UartChIdx]->PROTIEN & UUART_PROTIEN_BRKIEN_Msk, UUART_PROTIEN_BRKIEN_Msk);

        //        UUART_DISABLE_PROT_INT(UUartCh[u8UartChIdx], UUART_PROTIEN_RLSIEN_Msk);
        //        CU_ASSERT_EQUAL(UUartCh[u8UartChIdx]->PROTIEN & 0x07, (UUART_PROTIEN_ABRIEN_Msk | UUART_PROTIEN_BRKIEN_Msk));
        //        UUART_DISABLE_PROT_INT(UUartCh[u8UartChIdx], UUART_PROTIEN_ABRIEN_Msk);
        //        CU_ASSERT_EQUAL(UUartCh[u8UartChIdx]->PROTIEN & 0x07, UUART_PROTIEN_BRKIEN_Msk);
        //        UUART_DISABLE_PROT_INT(UUartCh[u8UartChIdx], UUART_PROTIEN_BRKIEN_Msk);
        //        CU_ASSERT_EQUAL(UUartCh[u8UartChIdx]->PROTIEN & 0x07, 0);

        UUART_DISABLE_PROT_INT(UUartCh[u8UUartChIdx], UUART_PROTIEN_RLSIEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->PROTIEN & 0x06, UUART_PROTIEN_ABRIEN_Msk);
        UUART_DISABLE_PROT_INT(UUartCh[u8UUartChIdx], UUART_PROTIEN_ABRIEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->PROTIEN & 0x06, 0);


        //buffer interrupt
        UUART_ENABLE_BUF_INT(UUartCh[u8UUartChIdx], UUART_BUFCTL_RXOVIEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->BUFCTL & UUART_BUFCTL_RXOVIEN_Msk, UUART_BUFCTL_RXOVIEN_Msk);

        UUART_DISABLE_BUF_INT(UUartCh[u8UUartChIdx], UUART_BUFCTL_RXOVIEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->BUFCTL & UUART_BUFCTL_RXOVIEN_Msk, 0);

        //transfer interrupt
        UUART_ENABLE_TRANS_INT(UUartCh[u8UUartChIdx], UUART_INTEN_RXENDIEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->INTEN & UUART_INTEN_RXENDIEN_Msk, UUART_INTEN_RXENDIEN_Msk);
        UUART_ENABLE_TRANS_INT(UUartCh[u8UUartChIdx], UUART_INTEN_RXSTIEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->INTEN & UUART_INTEN_RXSTIEN_Msk, UUART_INTEN_RXSTIEN_Msk);
        UUART_ENABLE_TRANS_INT(UUartCh[u8UUartChIdx], UUART_INTEN_TXENDIEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->INTEN & UUART_INTEN_TXENDIEN_Msk, UUART_INTEN_TXENDIEN_Msk);
        UUART_ENABLE_TRANS_INT(UUartCh[u8UUartChIdx], UUART_INTEN_TXSTIEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->INTEN & UUART_INTEN_TXSTIEN_Msk, UUART_INTEN_TXSTIEN_Msk);

        UUART_DISABLE_TRANS_INT(UUartCh[u8UUartChIdx], UUART_INTEN_RXENDIEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->INTEN & 0x1E, (UUART_INTEN_RXSTIEN_Msk | UUART_INTEN_TXENDIEN_Msk | UUART_INTEN_TXSTIEN_Msk));
        UUART_DISABLE_TRANS_INT(UUartCh[u8UUartChIdx], UUART_INTEN_RXSTIEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->INTEN & 0x1E, (UUART_INTEN_TXENDIEN_Msk | UUART_INTEN_TXSTIEN_Msk));
        UUART_DISABLE_TRANS_INT(UUartCh[u8UUartChIdx], UUART_INTEN_TXENDIEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->INTEN & 0x1E, UUART_INTEN_TXSTIEN_Msk);
        UUART_DISABLE_TRANS_INT(UUartCh[u8UUartChIdx], UUART_INTEN_TXSTIEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->INTEN & 0x1E, 0);

    }

    for (u8UUartChIdx = 0; u8UUartChIdx < UUART_CH_NUM_MAX; u8UUartChIdx++)
    {
        /* Test macro : UUART_GET_PROT_STATUS */
        UUartCh[u8UUartChIdx]->BUFCTL |= UUART_BUFCTL_RXRST_Msk;

        while ((UUartCh[u8UUartChIdx]->BUFCTL & UUART_BUFCTL_RXRST_Msk) == UUART_BUFCTL_RXRST_Msk);

        // fill Tx
        UUART_WRITE(UUartCh[u8UUartChIdx], 0x55);

        // wait data send out
        while ((UUartCh[u8UUartChIdx]->PROTSTS & UUART_PROTSTS_TXENDIF_Msk) == 0) {}

        // check macro
        CU_ASSERT_EQUAL(UUART_GET_PROT_STATUS(UUartCh[u8UUartChIdx]) & UUART_PROTSTS_TXSTIF_Msk, UUART_PROTSTS_TXSTIF_Msk);
        CU_ASSERT_EQUAL(UUART_GET_PROT_STATUS(UUartCh[u8UUartChIdx]) & UUART_PROTSTS_TXENDIF_Msk, UUART_PROTSTS_TXENDIF_Msk);

        UUART_CLR_PROT_INT_FLAG(UUartCh[u8UUartChIdx], UUART_PROTSTS_TXSTIF_Msk);
        CU_ASSERT_EQUAL(UUART_GET_PROT_STATUS(UUartCh[u8UUartChIdx]) & UUART_PROTSTS_TXSTIF_Msk, 0);
        UUART_CLR_PROT_INT_FLAG(UUartCh[u8UUartChIdx], UUART_PROTSTS_TXENDIF_Msk);
        CU_ASSERT_EQUAL(UUART_GET_PROT_STATUS(UUartCh[u8UUartChIdx]) & UUART_PROTSTS_TXENDIF_Msk, 0);

        // wait data receive
        while ((UUartCh[u8UUartChIdx]->PROTSTS & UUART_PROTSTS_RXENDIF_Msk) == 0) {}

        CU_ASSERT_EQUAL(UUART_GET_PROT_STATUS(UUartCh[u8UUartChIdx]) & UUART_PROTSTS_RXSTIF_Msk, UUART_PROTSTS_RXSTIF_Msk);
        CU_ASSERT_EQUAL(UUART_GET_PROT_STATUS(UUartCh[u8UUartChIdx]) & UUART_PROTSTS_RXENDIF_Msk, UUART_PROTSTS_RXENDIF_Msk);

        UUART_CLR_PROT_INT_FLAG(UUartCh[u8UUartChIdx], UUART_PROTSTS_RXSTIF_Msk);
        CU_ASSERT_EQUAL(UUART_GET_PROT_STATUS(UUartCh[u8UUartChIdx]) & UUART_PROTSTS_RXSTIF_Msk, 0);
        UUART_CLR_PROT_INT_FLAG(UUartCh[u8UUartChIdx], UUART_PROTSTS_RXENDIF_Msk);
        CU_ASSERT_EQUAL(UUART_GET_PROT_STATUS(UUartCh[u8UUartChIdx]) & UUART_PROTSTS_RXENDIF_Msk, 0);

        UUartCh[u8UUartChIdx]->BUFCTL |= UUART_BUFCTL_RXRST_Msk;

        while ((UUartCh[u8UUartChIdx]->BUFCTL & UUART_BUFCTL_RXRST_Msk) == UUART_BUFCTL_RXRST_Msk);

        // check Break Flag
        UUartCh[u8UUartChIdx]->PROTCTL |= UUART_PROTCTL_BCEN_Msk;

        while ((UUartCh[u8UUartChIdx]->PROTSTS & UUART_PROTSTS_BREAK_Msk) != UUART_PROTSTS_BREAK_Msk);

        UUartCh[u8UUartChIdx]->PROTCTL &= ~UUART_PROTCTL_BCEN_Msk;

        CU_ASSERT_EQUAL(UUART_GET_PROT_STATUS(UUartCh[u8UUartChIdx]) & UUART_PROTSTS_BREAK_Msk, UUART_PROTSTS_BREAK_Msk);

        UUART_CLR_PROT_INT_FLAG(UUartCh[u8UUartChIdx], UUART_PROTSTS_BREAK_Msk);
        CU_ASSERT_EQUAL(UUART_GET_PROT_STATUS(UUartCh[u8UUartChIdx]) & 0xE0, 0);
        UUART_READ(UUartCh[u8UUartChIdx]);

        // check Framing Error Flag
        UUartCh[u8UUartChIdx]->PROTCTL |= UUART_PROTCTL_BCEN_Msk;

        while ((UUartCh[u8UUartChIdx]->PROTSTS & UUART_PROTSTS_BREAK_Msk) != UUART_PROTSTS_BREAK_Msk);

        UUartCh[u8UUartChIdx]->PROTCTL &= ~UUART_PROTCTL_BCEN_Msk;

        CU_ASSERT_EQUAL(UUART_GET_PROT_STATUS(UUartCh[u8UUartChIdx]) & UUART_PROTSTS_FRMERR_Msk, UUART_PROTSTS_FRMERR_Msk);

        UUART_CLR_PROT_INT_FLAG(UUartCh[u8UUartChIdx], UUART_PROTSTS_FRMERR_Msk);
        CU_ASSERT_EQUAL(UUART_GET_PROT_STATUS(UUartCh[u8UUartChIdx]) & 0xE0, 0);
        UUART_READ(UUartCh[u8UUartChIdx]);

        // check Parity Error Flag
        UUartCh[u8UUartChIdx]->PROTCTL |= UUART_PROTCTL_PARITYEN_Msk;
        UUartCh[u8UUartChIdx]->PROTCTL |= UUART_PROTCTL_BCEN_Msk;

        while ((UUartCh[u8UUartChIdx]->PROTSTS & UUART_PROTSTS_BREAK_Msk) != UUART_PROTSTS_BREAK_Msk);

        UUartCh[u8UUartChIdx]->PROTCTL &= ~(UUART_PROTCTL_BCEN_Msk | UUART_PROTCTL_PARITYEN_Msk);

        CU_ASSERT_EQUAL(UUART_GET_PROT_STATUS(UUartCh[u8UUartChIdx]) & UUART_PROTSTS_PARITYERR_Msk, UUART_PROTSTS_PARITYERR_Msk);

        UUART_CLR_PROT_INT_FLAG(UUartCh[u8UUartChIdx], UUART_PROTSTS_PARITYERR_Msk);
        CU_ASSERT_EQUAL(UUART_GET_PROT_STATUS(UUartCh[u8UUartChIdx]) & 0xE0, 0);
        UUART_READ(UUartCh[u8UUartChIdx]);

        /* Test macro : UUART_GET_BUF_STATUS */
        UUartCh[u8UUartChIdx]->BUFCTL |= UUART_BUFCTL_RXRST_Msk;

        while ((UUartCh[u8UUartChIdx]->BUFCTL & UUART_BUFCTL_RXRST_Msk) == UUART_BUFCTL_RXRST_Msk);

        // fill Tx
        UUART_CLR_PROT_INT_FLAG(UUartCh[u8UUartChIdx], UUART_PROTSTS_TXENDIF_Msk);
        UUART_WRITE(UUartCh[u8UUartChIdx], 0x56);

        while ((UUartCh[u8UUartChIdx]->PROTSTS & UUART_PROTSTS_TXENDIF_Msk) == 0);

        // fill Tx
        UUART_CLR_PROT_INT_FLAG(UUartCh[u8UUartChIdx], UUART_PROTSTS_TXENDIF_Msk);
        UUART_WRITE(UUartCh[u8UUartChIdx], 0x57);

        while ((UUartCh[u8UUartChIdx]->BUFSTS & UUART_BUFSTS_RXFULL_Msk) == 0);

        // check Receive Buffer Over-run Error Flag
        UUART_CLR_PROT_INT_FLAG(UUartCh[u8UUartChIdx], UUART_PROTSTS_RXENDIF_Msk);
        UUART_WRITE(UUartCh[u8UUartChIdx], 0x58);

        while ((UUartCh[u8UUartChIdx]->PROTSTS & UUART_PROTSTS_RXENDIF_Msk) == 0) {}

        CU_ASSERT_EQUAL(UUART_GET_BUF_STATUS(UUartCh[u8UUartChIdx]) & UUART_BUFSTS_RXOVIF_Msk, UUART_BUFSTS_RXOVIF_Msk);

        UUART_CLR_BUF_INT_FLAG(UUartCh[u8UUartChIdx], UUART_BUFSTS_RXOVIF_Msk);
        CU_ASSERT_EQUAL(UUART_GET_BUF_STATUS(UUartCh[u8UUartChIdx]) & UUART_BUFSTS_RXOVIF_Msk, 0);

    }

}

void TestFunc_UUART_TestMacroPDMA()
{
    /* Select USCI_UART protocol */
    UUART0->CTL &= ~USPI_CTL_FUNMODE_Msk;
    UUART0->CTL = 2 << USPI_CTL_FUNMODE_Pos;

    UUART_TRIGGER_RX_PDMA(UUART0);
    CU_ASSERT((UUART0->PDMACTL & UUART_PDMACTL_RXPDMAEN_Msk) == UUART_PDMACTL_RXPDMAEN_Msk);
    UUART_DISABLE_RX_PDMA(UUART0);
    CU_ASSERT((UUART0->PDMACTL & UUART_PDMACTL_RXPDMAEN_Msk) == 0);
    UUART_TRIGGER_TX_PDMA(UUART0);
    CU_ASSERT((UUART0->PDMACTL & UUART_PDMACTL_TXPDMAEN_Msk) == UUART_PDMACTL_TXPDMAEN_Msk);
    UUART_DISABLE_TX_PDMA(UUART0);
    CU_ASSERT((UUART0->PDMACTL & UUART_PDMACTL_TXPDMAEN_Msk) == 0);

}

void TestFunc_UUART_ReadWrite()
{
    uint8_t u8UUartChIdx, i;        //UART channel index
    uint8_t u8TxData[2] = {1, 2}, u8RxData[2] = {0};

    /* Reset USCI */
    //SYS_ResetModule(USCI1_RST);

    /* Test loop */
    for (u8UUartChIdx = 0; u8UUartChIdx < UUART_CH_NUM_MAX; u8UUartChIdx++)
    {
        UUART_Open(UUartCh[u8UUartChIdx], 115200);

        /* Enable USCI internal loopback for test */
        outpw(((uint32_t)UUartCh[u8UUartChIdx]) + 0x84, (1 << 30));

        /* Write data */
        UUART_Write(UUartCh[u8UUartChIdx], u8TxData, 2);

        /* Read data */
        UUART_Read(UUartCh[u8UUartChIdx], u8RxData, 2);

        for (i = 0; i < 2; i++)
        {
            CU_ASSERT(u8TxData[i] == u8RxData[i]);
        }

    }

}


void TestFunc_UUART_Wakeup()
{
    uint8_t u8UUartChIdx;        //UART channel index

    for (u8UUartChIdx = 0; u8UUartChIdx < UUART_CH_NUM_MAX; u8UUartChIdx++)
    {
        //UUART_EnableWakeup()
        UUART_EnableWakeup(UUartCh[u8UUartChIdx], UUART_PROTCTL_DATWKEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->WKCTL, UUART_WKCTL_WKEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->PROTCTL & UUART_PROTCTL_DATWKEN_Msk, UUART_PROTCTL_DATWKEN_Msk);

        //UUART_DisableWakeup()
        UUART_DisableWakeup(UUartCh[u8UUartChIdx]);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->WKCTL, 0);

        UUART_EnableWakeup(UUartCh[u8UUartChIdx], UUART_PROTCTL_CTSWKEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->WKCTL, UUART_WKCTL_WKEN_Msk);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->PROTCTL & UUART_PROTCTL_CTSWKEN_Msk, UUART_PROTCTL_CTSWKEN_Msk);

        UUART_DisableWakeup(UUartCh[u8UUartChIdx]);
        CU_ASSERT_EQUAL(UUartCh[u8UUartChIdx]->WKCTL, 0);

        //UUART_CLR_WK_FLAG()
        //UUART_GET_WK_FLAG()
        UUART_GET_WAKEUP_FLAG(UUartCh[u8UUartChIdx]);
        CU_ASSERT_EQUAL(UUART_GET_WAKEUP_FLAG(UUartCh[u8UUartChIdx]), 0);
    }
}


void TestFunc_UUART_EnableFlowCtrl()
{
    uint8_t u8UUartChIdx;    //UUART channel index

    for (u8UUartChIdx = 0; u8UUartChIdx < UUART_CH_NUM_MAX; u8UUartChIdx++)
    {
        //UUART_EnableFlowCtrl()
        UUART_EnableFlowCtrl(UUartCh[u8UUartChIdx]);
        CU_ASSERT((UUartCh[u8UUartChIdx]->LINECTL & UUART_LINECTL_CTLOINV_Msk) == 0);
        CU_ASSERT((UUartCh[u8UUartChIdx]->CTLIN0 & UUART_CTLIN0_ININV_Msk) == 0);
        CU_ASSERT((UUartCh[u8UUartChIdx]->PROTCTL & (UUART_PROTCTL_RTSAUTOEN_Msk | UUART_PROTCTL_CTSAUTOEN_Msk))
                  == (UUART_PROTCTL_RTSAUTOEN_Msk | UUART_PROTCTL_CTSAUTOEN_Msk));
        //UUART_DisableFlowCtrl()
        UUART_DisableFlowCtrl(UUartCh[u8UUartChIdx]);
        CU_ASSERT((UUartCh[u8UUartChIdx]->PROTCTL & (UUART_PROTCTL_RTSAUTOEN_Msk | UUART_PROTCTL_CTSAUTOEN_Msk)) == 0);
    }
}



CU_TestInfo UUART_ApiTests[] =
{

    {"UUART_Enable/Disable Int Function.", TestFunc_UUART_EnableInt},
    {"UUART_Open Function.", TestFunc_UUART_Open},
    {"UUART_SetLine_Config Function.", TestFunc_UUART_SetLine_Config},
    {"Testing UUART_ReadWrite Function.", TestFunc_UUART_ReadWrite},
    {"Testing UUART_Wakeup Function.", TestFunc_UUART_Wakeup},
    {"UUART_Enable/DisableFlowCtrl Function.", TestFunc_UUART_EnableFlowCtrl},

    CU_TEST_INFO_NULL
};

CU_TestInfo UUART_MacroTests[] =
{
    {"Macro about BUF.", TestFunc_UUART_TestMacroBUF},
    {"Macro about interrupt.", TestFunc_UUART_TestMacroINT},
    {"Macro about PDMA.", TestFunc_UUART_TestMacroPDMA},

    CU_TEST_INFO_NULL
};
