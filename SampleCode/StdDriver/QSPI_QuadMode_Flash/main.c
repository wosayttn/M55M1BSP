/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief    Access SPI flash using QSPI quad mode
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*******************************************************************************
 * PRIVATE MACROS AND DEFINES
 ******************************************************************************/
#define TEST_NUMBER             1       /* page numbers */
#define TEST_LENGTH             256     /* length */
#define TEST_TIMEOUT            0xFFFF  /* timeout count */

#define SPI_FLASH_PORT          QSPI0

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/
static uint8_t g_au8SrcArray[TEST_LENGTH] = {0};
static uint8_t g_au8DestArray[TEST_LENGTH] = {0};

void D2D3_SwitchToNormalMode(void)
{
    SYS->GPA_MFP1 &= ~(SYS_GPA_MFP1_PA4MFP_Msk | SYS_GPA_MFP1_PA5MFP_Msk);
    GPIO_SetMode(PA, BIT4, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT5, GPIO_MODE_OUTPUT);
    PA4 = 1;
    PA5 = 1;
}

void D2D3_SwitchToQuadMode(void)
{
    SYS->GPA_MFP1 = (SYS->GPA_MFP1 & ~(SYS_GPA_MFP1_PA4MFP_Msk | SYS_GPA_MFP1_PA5MFP_Msk)) | SYS_GPA_MFP1_PA4MFP_QSPI0_MOSI1 | SYS_GPA_MFP1_PA5MFP_QSPI0_MISO1;
}

uint16_t SpiFlash_ReadMidDid(void)
{
    uint8_t u8RxData[6], u8IDCnt = 0;
    int i32Timeout = TEST_TIMEOUT;

    // /CS: active
    QSPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x90, Read Manufacturer/Device ID
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x90);

    // send 24-bit '0', dummy
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // receive 16-bit
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // wait tx finish
    while (QSPI_IS_BUSY(SPI_FLASH_PORT))
    {
        if (i32Timeout-- <= 0)
        {
            break;
        }
    }

    // /CS: de-active
    QSPI_SET_SS_HIGH(SPI_FLASH_PORT);

    while (!QSPI_GET_RX_FIFO_EMPTY_FLAG(SPI_FLASH_PORT))
    {
        u8RxData[u8IDCnt ++] = QSPI_READ_RX(SPI_FLASH_PORT);
    }

    return ((u8RxData[4] << 8) | u8RxData[5]);
}

void SpiFlash_ChipErase(void)
{
    int i32Timeout = TEST_TIMEOUT;

    // /CS: active
    QSPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x06, Write enable
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x06);

    // wait tx finish
    while (QSPI_IS_BUSY(SPI_FLASH_PORT))
    {
        if (i32Timeout-- <= 0)
        {
            break;
        }
    }

    // /CS: de-active
    QSPI_SET_SS_HIGH(SPI_FLASH_PORT);

    //////////////////////////////////////////

    // /CS: active
    QSPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0xC7, Chip Erase
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0xC7);

    i32Timeout = TEST_TIMEOUT;

    // wait tx finish
    while (QSPI_IS_BUSY(SPI_FLASH_PORT))
    {
        if (i32Timeout-- <= 0)
        {
            break;
        }
    }

    // /CS: de-active
    QSPI_SET_SS_HIGH(SPI_FLASH_PORT);

    QSPI_ClearRxFIFO(SPI_FLASH_PORT);
}

uint8_t SpiFlash_ReadStatusReg(void)
{
    uint8_t u8Val;
    int i32Timeout = TEST_TIMEOUT;

    QSPI_ClearRxFIFO(SPI_FLASH_PORT);

    // /CS: active
    QSPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x05, Read status register
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x05);

    // read status
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // wait tx finish
    while (QSPI_IS_BUSY(SPI_FLASH_PORT))
    {
        if (i32Timeout-- <= 0)
        {
            break;
        }
    }

    // /CS: de-active
    QSPI_SET_SS_HIGH(SPI_FLASH_PORT);

    // skip first rx data
    u8Val = QSPI_READ_RX(SPI_FLASH_PORT);
    u8Val = QSPI_READ_RX(SPI_FLASH_PORT);

    return u8Val;
}

uint8_t SpiFlash_ReadStatusReg2(void)
{
    uint8_t u8Val;
    int i32Timeout = TEST_TIMEOUT;

    QSPI_ClearRxFIFO(SPI_FLASH_PORT);

    // /CS: active
    QSPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x35, Read status register
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x35);

    // read status
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // wait tx finish
    while (QSPI_IS_BUSY(SPI_FLASH_PORT))
    {
        if (i32Timeout-- <= 0)
        {
            break;
        }
    }

    // /CS: de-active
    QSPI_SET_SS_HIGH(SPI_FLASH_PORT);

    // skip first rx data
    u8Val = QSPI_READ_RX(SPI_FLASH_PORT);
    u8Val = QSPI_READ_RX(SPI_FLASH_PORT);

    return u8Val;
}

void SpiFlash_WriteStatusReg(uint8_t u8Value1, uint8_t u8Value2)
{
    int i32Timeout = TEST_TIMEOUT;

    // /CS: active
    QSPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x06, Write enable
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x06);

    // wait tx finish
    while (QSPI_IS_BUSY(SPI_FLASH_PORT))
    {
        if (i32Timeout-- <= 0)
        {
            break;
        }
    }

    // /CS: de-active
    QSPI_SET_SS_HIGH(SPI_FLASH_PORT);

    ///////////////////////////////////////

    // /CS: active
    QSPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x01, Write status register
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x01);

    // write status
    QSPI_WRITE_TX(SPI_FLASH_PORT, u8Value1);
    QSPI_WRITE_TX(SPI_FLASH_PORT, u8Value2);

    i32Timeout = TEST_TIMEOUT;

    // wait tx finish
    while (QSPI_IS_BUSY(SPI_FLASH_PORT))
    {
        if (i32Timeout-- <= 0)
        {
            break;
        }
    }

    // /CS: de-active
    QSPI_SET_SS_HIGH(SPI_FLASH_PORT);
}

void SpiFlash_WaitReady(void)
{
    volatile uint8_t u8ReturnValue;
    int i32Timeout = TEST_TIMEOUT;

    do
    {
        u8ReturnValue = SpiFlash_ReadStatusReg();
        u8ReturnValue = (u8ReturnValue & 1);
    } while ((u8ReturnValue != 0) && (i32Timeout-- >= 0)); // check the BUSY bit
}

void SpiFlash_NormalPageProgram(uint32_t u32StartAddress, uint8_t *u8DataBuffer)
{
    uint32_t u32Cnt = 0;
    int i32Timeout = TEST_TIMEOUT;

    // /CS: active
    QSPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x06, Write enable
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x06);

    // wait tx finish
    while (QSPI_IS_BUSY(SPI_FLASH_PORT))
    {
        if (i32Timeout-- <= 0)
        {
            break;
        }
    }

    // /CS: de-active
    QSPI_SET_SS_HIGH(SPI_FLASH_PORT);


    // /CS: active
    QSPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x02, Page program
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x02);

    // send 24-bit start address
    QSPI_WRITE_TX(SPI_FLASH_PORT, (u32StartAddress >> 16) & 0xFF);
    QSPI_WRITE_TX(SPI_FLASH_PORT, (u32StartAddress >> 8)  & 0xFF);
    QSPI_WRITE_TX(SPI_FLASH_PORT, u32StartAddress & 0xFF);

    // write data
    while (1)
    {
        if (!QSPI_GET_TX_FIFO_FULL_FLAG(SPI_FLASH_PORT))
        {
            QSPI_WRITE_TX(SPI_FLASH_PORT, u8DataBuffer[u32Cnt++]);

            if (u32Cnt > 255) break;
        }
    }

    i32Timeout = TEST_TIMEOUT;

    // wait tx finish
    while (QSPI_IS_BUSY(SPI_FLASH_PORT))
    {
        if (i32Timeout-- <= 0)
        {
            break;
        }
    }

    // /CS: de-active
    QSPI_SET_SS_HIGH(SPI_FLASH_PORT);

    QSPI_ClearRxFIFO(SPI_FLASH_PORT);
}

void spiFlash_EnableQEBit(void)
{
    uint8_t u8Status1 = SpiFlash_ReadStatusReg();
    uint8_t u8Status2 = SpiFlash_ReadStatusReg2();

    u8Status2 |= 0x2;
    SpiFlash_WriteStatusReg(u8Status1, u8Status2);
    SpiFlash_WaitReady();
}

void spiFlash_DisableQEBit(void)
{
    uint8_t u8Status1 = SpiFlash_ReadStatusReg();
    uint8_t u8Status2 = SpiFlash_ReadStatusReg2();

    u8Status2 &= ~0x2;
    SpiFlash_WriteStatusReg(u8Status1, u8Status2);
    SpiFlash_WaitReady();
}

void SpiFlash_QuadFastRead(uint32_t u32StartAddress, uint8_t *u8DataBuffer)
{
    uint32_t u32Cnt;
    int i32Timeout = TEST_TIMEOUT;

    // enable quad mode
    spiFlash_EnableQEBit();

    // /CS: active
    QSPI_SET_SS_LOW(SPI_FLASH_PORT);

    // Command: 0xEB, Fast Read quad data
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0xEB);

    while (QSPI_IS_BUSY(SPI_FLASH_PORT))
    {
        if (i32Timeout-- <= 0)
        {
            break;
        }
    }

    // enable QSPI quad IO mode and set direction to input
    D2D3_SwitchToQuadMode();
    QSPI_ENABLE_QUAD_OUTPUT_MODE(SPI_FLASH_PORT);

    // send 24-bit start address
    QSPI_WRITE_TX(SPI_FLASH_PORT, (u32StartAddress >> 16) & 0xFF);
    QSPI_WRITE_TX(SPI_FLASH_PORT, (u32StartAddress >> 8)  & 0xFF);
    QSPI_WRITE_TX(SPI_FLASH_PORT, u32StartAddress & 0xFF);

    // dummy byte
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    QSPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    i32Timeout = TEST_TIMEOUT;

    while (QSPI_IS_BUSY(SPI_FLASH_PORT))
    {
        if (i32Timeout-- <= 0)
        {
            break;
        }
    }

    QSPI_ENABLE_QUAD_INPUT_MODE(SPI_FLASH_PORT);

    // clear RX buffer
    QSPI_ClearRxFIFO(SPI_FLASH_PORT);

    // read data
    for (u32Cnt = 0; u32Cnt < 256; u32Cnt++)
    {
        QSPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

        i32Timeout = TEST_TIMEOUT;

        while (QSPI_IS_BUSY(SPI_FLASH_PORT))
        {
            if (i32Timeout-- <= 0)
            {
                break;
            }
        }

        u8DataBuffer[u32Cnt] = QSPI_READ_RX(SPI_FLASH_PORT);
    }

    i32Timeout = TEST_TIMEOUT;

    // wait tx finish
    while (QSPI_IS_BUSY(SPI_FLASH_PORT))
    {
        if (i32Timeout-- <= 0)
        {
            break;
        }
    }

    // /CS: de-active
    QSPI_SET_SS_HIGH(SPI_FLASH_PORT);

    QSPI_DISABLE_QUAD_MODE(SPI_FLASH_PORT);
    D2D3_SwitchToNormalMode();

    // disable quad mode
    spiFlash_DisableQEBit();
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Enable PLL0 180MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_180MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to PLL0 and divide 1 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0, CLK_ACLKDIV_ACLKDIV(1));

    /* Set HCLK2 divide 2 */
    CLK_SET_HCLK2DIV(2);

    /* Set PCLKx divide 2 */
    CLK_SET_PCLK0DIV(2);
    CLK_SET_PCLK1DIV(2);
    CLK_SET_PCLK2DIV(2);
    CLK_SET_PCLK3DIV(2);
    CLK_SET_PCLK4DIV(2);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and cyclesPerUs automatically. */
    SystemCoreClockUpdate();

    /* Select UART clock source from HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HIRC, CLK_UARTDIV0_UART0DIV(1));

    /* Select PCLK0 as the clock source of QSPI0 */
    CLK_SetModuleClock(QSPI0_MODULE, CLK_QSPISEL_QSPI0SEL_PCLK0, MODULE_NoMsk);

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable QSPI0 peripheral clock */
    CLK_EnableModuleClock(QSPI0_MODULE);

    /* Set PB multi-function pins for UART0 RXD=PB.12 and TXD=PB.13 */
    SetDebugUartMFP();

    /* Setup QSPI0 multi-function pins */
    SYS->GPA_MFP0 |= (SYS_GPA_MFP0_PA0MFP_QSPI0_MOSI0 |
                      SYS_GPA_MFP0_PA1MFP_QSPI0_MISO0 |
                      SYS_GPA_MFP0_PA2MFP_QSPI0_CLK |
                      SYS_GPA_MFP0_PA3MFP_QSPI0_SS);
    SYS->GPA_MFP1 |= (SYS_GPA_MFP1_PA4MFP_QSPI0_MOSI1 |
                      SYS_GPA_MFP1_PA5MFP_QSPI0_MISO1);

    /* Enable QSPI0 clock pin (PA2) schmitt trigger */
    PA->SMTEN |= GPIO_SMTEN_SMTEN2_Msk;

    /* Enable QSPI0 I/O high slew rate */
    GPIO_SetSlewCtl(PA, 0x3F, GPIO_SLEWCTL_HIGH);

    /* Lock protected registers */
    SYS_LockReg();
}

/* Main */
int main(void)
{
    uint32_t u32ByteCount, u32FlashAddress, u32PageNumber;
    uint32_t u32Error = 0;
    uint16_t u16ID;

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    D2D3_SwitchToNormalMode();

    /* Init UART to 115200-8n1 for print message */
    InitDebugUart();

    /* Configure SPI_FLASH_PORT as a master, MSB first, 8-bit transaction, QSPI Mode-0 timing, clock is 20MHz */
    QSPI_Open(SPI_FLASH_PORT, QSPI_MASTER, QSPI_MODE_0, 8, 20000000);

    /* Disable auto SS function, control SS signal manually. */
    QSPI_DisableAutoSS(SPI_FLASH_PORT);

    printf("\n\n");
    printf("+-------------------------------------------------------------------------+\n");
    printf("|                  QSPI Quad Mode with Flash Sample Code                  |\n");
    printf("+-------------------------------------------------------------------------+\n");

    if ((u16ID = SpiFlash_ReadMidDid()) != 0xEF14)
    {
        printf("Wrong ID, 0x%x\n", u16ID);

        while (1);
    }
    else
        printf("Flash found: W25X16 ...\n");

    printf("Erase chip ...");

    /* Erase SPI flash */
    SpiFlash_ChipErase();

    /* Wait ready */
    SpiFlash_WaitReady();

    printf("[OK]\n");

    /* init source data buffer */
    for (u32ByteCount = 0; u32ByteCount < TEST_LENGTH; u32ByteCount++)
    {
        g_au8SrcArray[u32ByteCount] = u32ByteCount;
    }

    printf("Start to write data to Flash ...");
    /* Program SPI flash */
    u32FlashAddress = 0;

    for (u32PageNumber = 0; u32PageNumber < TEST_NUMBER; u32PageNumber++)
    {
        /* page program */
        SpiFlash_NormalPageProgram(u32FlashAddress, g_au8SrcArray);
        SpiFlash_WaitReady();
        u32FlashAddress += 0x100;
    }

    printf("[OK]\n");

    /* clear destination data buffer */
    for (u32ByteCount = 0; u32ByteCount < TEST_LENGTH; u32ByteCount++)
    {
        g_au8DestArray[u32ByteCount] = 0;
    }

    printf("Read & Compare ...");

    /* Read SPI flash */
    u32FlashAddress = 0;

    for (u32PageNumber = 0; u32PageNumber < TEST_NUMBER; u32PageNumber++)
    {
        /* page read */
        SpiFlash_QuadFastRead(u32FlashAddress, g_au8DestArray);
        u32FlashAddress += 0x100;

        for (u32ByteCount = 0; u32ByteCount < TEST_LENGTH; u32ByteCount++)
        {
            if (g_au8DestArray[u32ByteCount] != g_au8SrcArray[u32ByteCount])
                u32Error ++;
        }
    }

    if (u32Error == 0)
        printf("[OK]\n");
    else
        printf("[FAIL]\n");

    while (1);
}


/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
