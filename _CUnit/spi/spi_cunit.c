/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.           */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   spi_i2s_cunit.c                                                                                       */
/*            The test function of SPI/I2S for CUnit.                                                      */
/* Project:   M251                                                                                         */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Includes of system headers                                                 */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*----------------------------------------------------------------------------*/
/* Includes of local headers                                                  */
/*----------------------------------------------------------------------------*/
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "spi_cunit.h"
#include "spi_common.h"
#include "PinConfig.h"

//------------------------------------------------------------------------------
extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;

//#define SPI_MODULE_NUM   2
//#define SPI_I2S_FIRSTNO  1

#define C_SPI0                      (0)
#define C_SPI1                      (1)
#define C_SPI2                      (2)
#define C_SPI3                      (3)

static SPI_T *gapSPIModule[] =
{
    SPI0,
    SPI1,
    SPI2,
    SPI3,
};

static uint32_t gu32SPIModuleIdx = 0;

/*----------------------------------------------------------------------------*/
/* Test function                                                              */
/*----------------------------------------------------------------------------*/
void SetSPIModuleIdx(uint32_t u32SetValue)
{
    gu32SPIModuleIdx = u32SetValue;
}

uint32_t GetSPIModuleIdx(void)
{
    return gu32SPIModuleIdx;
}

void *GetSPIModule(uint32_t u32ModuleIdx)
{
    return gapSPIModule[u32ModuleIdx];
}

int SPI_I2S_Tests_Init(void)
{
    SPI_CLK_Sel(GetSPIModuleIdx(), eSPI_CLK_HIRC48M);

    return 0;
}

int SPI_I2S_Tests_Clean(void)
{
    SPI_ClkDisable(GetSPIModuleIdx());

    return 0;
}

/* Set SPI_SS as output mode */
void SetSSPinToGPIO(uint32_t u32SPIModule)
{
    switch (u32SPIModule)
    {
        case C_SPI0:
            SET_GPIO_PA3();
            GPIO_SetMode(PA, BIT3, GPIO_MODE_OUTPUT);
            break;

        case C_SPI1:
            SET_GPIO_PA6();
            GPIO_SetMode(PA, BIT6, GPIO_MODE_OUTPUT);
            break;

        case C_SPI2:
            SET_GPIO_PA11();
            GPIO_SetMode(PA, BIT11, GPIO_MODE_OUTPUT);
            break;

        case C_SPI3:
            SET_GPIO_PB10();
            GPIO_SetMode(PB, BIT10, GPIO_MODE_OUTPUT);
            break;
    }
}

void SetSSPinToLow(uint32_t u32SPIModule)
{
    switch (u32SPIModule)
    {
        case C_SPI0:
            PA3 = 0;
            break;

        case C_SPI1:
            PA6 = 0;
            break;

        case C_SPI2:
            PA11 = 0;
            break;

        case C_SPI3:
            PB10 = 0;
            break;
    }
}

void SetSSPinToHigh(uint32_t u32SPIModule)
{
    switch (u32SPIModule)
    {
        case C_SPI0:
            PA3 = 1;
            break;

        case C_SPI1:
            PA6 = 1;
            break;

        case C_SPI2:
            PA11 = 1;
            break;

        case C_SPI3:
            PB10 = 1;
            break;
    }
}

/* Set SPI_CLK as GPIO output pin */
void SetCLKPinToGPIO(uint32_t u32SPIModule)
{
    switch (u32SPIModule)
    {
        case C_SPI0:
            SET_GPIO_PA2();
            GPIO_SetMode(PA, BIT3, GPIO_MODE_OUTPUT);
            PA2 = 0;
            break;

        case C_SPI1:
            SET_GPIO_PA7();
            GPIO_SetMode(PA, BIT7, GPIO_MODE_OUTPUT);
            PA7 = 0;
            break;

        case C_SPI2:
            SET_GPIO_PA10();
            GPIO_SetMode(PA, BIT10, GPIO_MODE_OUTPUT);
            PA10 = 0;
            break;

        case C_SPI3:
            SET_GPIO_PB11();
            GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);
            PB11 = 0;
            break;
    }
}

void SetSPICLKMFP(uint32_t u32SPIModule)
{
    switch (u32SPIModule)
    {
        case C_SPI0:
            SPI0_CLK_PIN_INIT;
            break;

        case C_SPI1:
            SPI1_CLK_PIN_INIT;
            break;

        case C_SPI2:
            SPI2_CLK_PIN_INIT;
            break;

        case C_SPI3:
            SPI3_CLK_PIN_INIT;
            break;
    }
}

/* Set SPI_MOSI as GPIO output pin */
void SetMOSIPinToGPIO(uint32_t u32SPIModule)
{

    switch (u32SPIModule)
    {
        case C_SPI0:
            SET_GPIO_PA0();
            GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
            break;

        case C_SPI1:
            SET_GPIO_PB4();
            GPIO_SetMode(PB, BIT4, GPIO_MODE_OUTPUT);
            break;

        case C_SPI2:
            SET_GPIO_PA8();
            GPIO_SetMode(PA, BIT8, GPIO_MODE_OUTPUT);
            break;

        case C_SPI3:
            SET_GPIO_PB8();
            GPIO_SetMode(PB, BIT8, GPIO_MODE_OUTPUT);
            break;
    }
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
void MACRO_SPI_CLR_UNIT_TRANS_INT_FLAG()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPIModule->CLKDIV = 4;
    SPIModule->CTL = 5;
    SPIModule->TX = 1;

    while ((SPIModule->STATUS & 0x00000002) == 0) __NOP();

    SPI_CLR_UNIT_TRANS_INT_FLAG(SPIModule);
    CU_ASSERT((SPIModule->STATUS & 0x00000002) == 0);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_SPI_TRIGGER_DISABLE_RX_PDMA()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    CU_ASSERT((SPIModule->PDMACTL & 0x00000002) == 0);
    SPI_TRIGGER_RX_PDMA(SPIModule);

    /* Check RXPDMAEN bit */
    CU_ASSERT((SPIModule->PDMACTL & 0x00000002) == 0x00000002);
    SPI_DISABLE_RX_PDMA(SPIModule);
    CU_ASSERT((SPIModule->PDMACTL & 0x00000002) == 0);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_SPI_TRIGGER_DISABLE_TX_PDMA()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    /* Check TXPDMAEN bit */
    CU_ASSERT((SPIModule->PDMACTL & 0x00000001) == 0);
    SPI_TRIGGER_TX_PDMA(SPIModule);
    /* Check TXPDMAEN bit */
    CU_ASSERT((SPIModule->PDMACTL & 0x00000001) == 0x00000001);
    SPI_DISABLE_TX_PDMA(SPIModule);
    CU_ASSERT((SPIModule->PDMACTL & 0x00000001) == 0);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_SPI_GET_RX_FIFO_COUNT_EMPTY_FLAG()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPIModule->CLKDIV = 0xFF;
    /* Master mode, SPI mode 0. */
    SPIModule->CTL = 0x00000005;
    /* Check RX_EMPTY flag */
    CU_ASSERT(SPI_GET_RX_FIFO_EMPTY_FLAG(SPIModule) == 1);
    SPIModule->TX = 1;

    /* Check unit transfer interrupt flag */
    while ((SPIModule->STATUS & 0x00000002) == 0) __NOP();

    SPI_CLR_UNIT_TRANS_INT_FLAG(SPIModule);
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPIModule) == 1);
    /* Check RX_EMPTY flag */
    CU_ASSERT(SPI_GET_RX_FIFO_EMPTY_FLAG(SPIModule) == 0);
    SPIModule->TX = 2;

    while ((SPIModule->STATUS & 0x00000002) == 0) __NOP();

    SPI_CLR_UNIT_TRANS_INT_FLAG(SPIModule);
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPIModule) == 2);
    SPIModule->TX = 3;

    while ((SPIModule->STATUS & 0x00000002) == 0) __NOP();

    SPI_CLR_UNIT_TRANS_INT_FLAG(SPIModule);
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPIModule) == 3);
    SPIModule->TX = 4;

    while ((SPIModule->STATUS & 0x00000002) == 0) __NOP();

    SPI_CLR_UNIT_TRANS_INT_FLAG(SPIModule);
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPIModule) == 4);
    SPIModule->TX = 5;

    while ((SPIModule->STATUS & 0x00000002) == 0) __NOP();

    SPI_CLR_UNIT_TRANS_INT_FLAG(SPIModule);
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPIModule) == 4);
    SPIModule->TX = 6;

    while ((SPIModule->STATUS & 0x00000002) == 0) __NOP();

    SPI_CLR_UNIT_TRANS_INT_FLAG(SPIModule);
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPIModule) == 4);
    SPIModule->TX = 7;

    while ((SPIModule->STATUS & 0x00000002) == 0) __NOP();

    SPI_CLR_UNIT_TRANS_INT_FLAG(SPIModule);
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPIModule) == 4);
    SPIModule->TX = 8;

    while ((SPIModule->STATUS & 0x00000002) == 0) __NOP();

    SPI_CLR_UNIT_TRANS_INT_FLAG(SPIModule);
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPIModule) == 4);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_SPI_GET_TX_FIFO_EMPTY_FULL_FLAG()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPIModule->CLKDIV = 0xFF;
    /* Slave mode, SPI mode 0. */
    SPIModule->CTL = 0x00040005;
    /* Check TX_EMPTY flag */
    CU_ASSERT(SPI_GET_TX_FIFO_EMPTY_FLAG(SPIModule) == 1);
    /* Check TX_FULL flag */
    CU_ASSERT(SPI_GET_TX_FIFO_FULL_FLAG(SPIModule) == 0);
    SPIModule->TX = 1;
    /* Check TX_EMPTY flag */
    CU_ASSERT(SPI_GET_TX_FIFO_EMPTY_FLAG(SPIModule) == 0);
    /* Check TX_FULL flag */
    CU_ASSERT(SPI_GET_TX_FIFO_FULL_FLAG(SPIModule) == 0);
    SPIModule->TX = 2;
    SPIModule->TX = 3;
    SPIModule->TX = 4;
    SPIModule->TX = 5;
    SPIModule->TX = 6;
    SPIModule->TX = 7;
    SPIModule->TX = 8;
    /* Check TX_FULL flag */
    CU_ASSERT(SPI_GET_TX_FIFO_FULL_FLAG(SPIModule) == 1);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_SPI_READ_RX()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPIModule->CLKDIV = 4;
    /* Master mode, SPI mode 0. */
    SPIModule->CTL = 0x00000005;
    /* Write 3 data to TX FIFO */
    SPIModule->TX = 1;
    SPIModule->TX = 2;
    SPIModule->TX = 3;

    /* Wait data transfer */
    while ((SPIModule->STATUS & 0x00000001) != 0) __NOP();

    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPIModule) == 3);
    SPI_READ_RX(SPIModule);
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPIModule) == 2);
    SPI_READ_RX(SPIModule);
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPIModule) == 1);
    SPI_READ_RX(SPIModule);
    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPIModule) == 0);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_SPI_WRITE_TX()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPIModule->CLKDIV = 0xFF;
    /* Slave mode, SPI mode 0. */
    SPIModule->CTL = 0x00040005;
    CU_ASSERT((SPIModule->STATUS & 0xF0000000) == 0);
    SPI_WRITE_TX(SPIModule, 1);
    CU_ASSERT((SPIModule->STATUS & 0xF0000000) == 0x10000000);
    SPI_WRITE_TX(SPIModule, 2);
    CU_ASSERT((SPIModule->STATUS & 0xF0000000) == 0x20000000);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_SPI_SET_SS_HIGH()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPI_SET_SS_HIGH(SPIModule);
    CU_ASSERT(SPIModule->SSCTL == 5);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_SPI_SET_SS_LOW()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPI_SET_SS_LOW(SPIModule);
    CU_ASSERT(SPIModule->SSCTL == 1);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_SPI_ENABLE_DISABLE_BYTE_REORDER()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPI_ENABLE_BYTE_REORDER(SPIModule);
    CU_ASSERT_TRUE(SPIModule->CTL & 0x00080000);
    SPI_DISABLE_BYTE_REORDER(SPIModule);
    CU_ASSERT_FALSE(SPIModule->CTL & 0x00080000);
}

void MACRO_SPI_SET_SUSPEND_CYCLE()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPI_SET_SUSPEND_CYCLE(SPIModule, 0);
    CU_ASSERT((SPIModule->CTL & 0x000000F0) == 0);
    SPI_SET_SUSPEND_CYCLE(SPIModule, 8);
    CU_ASSERT((SPIModule->CTL & 0x000000F0) == 0x00000080);
    SPI_SET_SUSPEND_CYCLE(SPIModule, 15);
    CU_ASSERT((SPIModule->CTL & 0x000000F0) == 0x000000F0);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_SPI_SET_LSB_MSB_FIRST()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPI_SET_LSB_FIRST(SPIModule);
    CU_ASSERT_TRUE(SPIModule->CTL & 0x00002000);
    SPI_SET_MSB_FIRST(SPIModule);
    CU_ASSERT_FALSE(SPIModule->CTL & 0x00002000);
}

void MACRO_SPI_SET_DATA_WIDTH()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPI_SET_DATA_WIDTH(SPIModule, 32);
    CU_ASSERT((SPIModule->CTL & 0x00001F00) == 0);
    SPI_SET_DATA_WIDTH(SPIModule, 8);
    CU_ASSERT((SPIModule->CTL & 0x00001F00) == 0x00000800);
    SPI_SET_DATA_WIDTH(SPIModule, 16);
    CU_ASSERT((SPIModule->CTL & 0x00001F00) == 0x00001000);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_SPI_IS_BUSY()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPIModule->CLKDIV = 0xFF;
    SPIModule->CTL |= 1;
    SPIModule->TX = 1;
    CU_ASSERT_TRUE(SPI_IS_BUSY(SPIModule));

    /* Wait data transfer */
    while ((SPIModule->STATUS & SPI_STATUS_UNITIF_Msk) == 0)
    {
        __NOP();
    }

    CLK_SysTickDelay(5);

    CU_ASSERT_FALSE(SPI_IS_BUSY(SPIModule));

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_SPI_ENABLE_DISABLE()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    CU_ASSERT_FALSE(SPIModule->CTL & 1);
    SPI_ENABLE(SPIModule);
    CU_ASSERT_TRUE(SPIModule->CTL & 1);
    SPI_DISABLE(SPIModule);
    CU_ASSERT_FALSE(SPIModule->CTL & 1);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_SPI_Open()
{
    uint32_t u32ReturnValue;
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    u32ReturnValue = SPI_Open(SPIModule, SPI_MASTER, SPI_MODE_0, 8, 1000000);
    CU_ASSERT(SPIModule->CTL == 0x00000805);
    //printf("1 SPIModule->CLKDIV =  %x\r\n", SPIModule->CLKDIV);
    CU_ASSERT(SPIModule->CLKDIV == 0x0000002F);
    CU_ASSERT(SPIModule->SSCTL == 0);
    CU_ASSERT(u32ReturnValue == 1000000);
    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());


    u32ReturnValue = SPI_Open(SPIModule, SPI_SLAVE, SPI_MODE_1, 16, 2000000);
    CU_ASSERT(SPIModule->CTL == 0x00041003);
    //printf("2 SPIModule->CLKDIV =  %x\r\n", SPIModule->CLKDIV);
    CU_ASSERT(SPIModule->CLKDIV == 0x00000000);
    CU_ASSERT(SPIModule->SSCTL == 0x0);
    CU_ASSERT(u32ReturnValue == 48000000);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());

    u32ReturnValue = SPI_Open(SPIModule, SPI_MASTER, SPI_MODE_2, 24, 5000000);
    CU_ASSERT(SPIModule->CTL == 0x0000180B);
    //printf("3 SPIModule->CLKDIV =  %x\r\n", SPIModule->CLKDIV);
    CU_ASSERT(SPIModule->CLKDIV == 0x00000009);
    CU_ASSERT(SPIModule->SSCTL == 0x0);
    CU_ASSERT(u32ReturnValue == 4800000);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());

    u32ReturnValue = SPI_Open(SPIModule, SPI_MASTER, SPI_MODE_3, 32, 32000000);
    CU_ASSERT(SPIModule->CTL == 0x0000000D);
    //printf("4 SPIModule->CLKDIV =  %x\r\n", SPIModule->CLKDIV);
    CU_ASSERT(SPIModule->CLKDIV == 0x00000001);
    CU_ASSERT(SPIModule->SSCTL == 0x0);
    CU_ASSERT(u32ReturnValue == 24000000);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_SPI_Close()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPIModule->CTL = 0x0000FFF4;
    SPI_Close(SPIModule);
    CU_ASSERT(SPIModule->CTL == 0x00000034);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_SPI_ClearRxFIFO()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPIModule->CLKDIV = 4;
    /* Master mode, SPI mode 0. */
    SPIModule->CTL = 0x00000005;
    /* Write 3 data to TX FIFO */
    SPIModule->TX = 1;
    SPIModule->TX = 2;
    SPIModule->TX = 3;

    /* Wait data transfer */
    while ((SPIModule->STATUS & 1) != 0) __NOP();

    CU_ASSERT(SPI_GET_RX_FIFO_COUNT(SPIModule) == 3);
    SPI_ClearRxFIFO(SPIModule);
    CU_ASSERT(SPIModule->STATUS == 0x00059112);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_SPI_ClearTxFIFO()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPIModule->CLKDIV = 0xFF;
    /* Slave mode, SPI mode 0. */
    SPIModule->CTL = 0x00040005;
    /* Write 3 data to TX FIFO */
    SPIModule->TX = 1;
    SPIModule->TX = 2;
    SPIModule->TX = 3;
    CU_ASSERT((SPIModule->STATUS & 0xF00F0000) == 0x30000000);   //CU_ASSERT((SPIModule->STATUS & 0xF0000000)== 0x30000000);
    SPI_ClearTxFIFO(SPIModule);
    CU_ASSERT((SPIModule->STATUS & 0xF00F0000) == 0x00050000); //CU_ASSERT((SPIModule->STATUS & 0xF0000000)== 0);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_SPI_EnableAutoSS_DisableAutoSS()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPI_EnableAutoSS(SPIModule, SPI_SS, SPI_SS_ACTIVE_HIGH);
    CU_ASSERT(SPIModule->SSCTL == 0xD);
    SPI_DisableAutoSS(SPIModule);
    CU_ASSERT((SPIModule->SSCTL & (SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SS_Msk)) == 0x0);
    SPI_EnableAutoSS(SPIModule, SPI_SS, SPI_SS_ACTIVE_LOW);
    CU_ASSERT(SPIModule->SSCTL == 0x9);
    SPI_DisableAutoSS(SPIModule);
    CU_ASSERT((SPIModule->SSCTL & (SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SS_Msk)) == 0x0);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_SPI_SetBusClock_GetBusClock()
{
    volatile uint32_t u32ReturnValue;
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    u32ReturnValue = SPI_SetBusClock(SPIModule, 200 * 1000);
    CU_ASSERT(SPIModule->CLKDIV == 0x00000EF);
    CU_ASSERT(u32ReturnValue == 200000);
    u32ReturnValue = SPI_GetBusClock(SPIModule);
    CU_ASSERT(u32ReturnValue == 200000);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_SPI_SetFIFO()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPI_SetFIFO(SPIModule, 1, 2);
    CU_ASSERT((SPIModule->FIFOCTL & 0xFF000000) == 0x12000000);
    SPI_SetFIFO(SPIModule, 0, 3);
    CU_ASSERT((SPIModule->FIFOCTL & 0xFF000000) == 0x03000000);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_SPI_EnableInt_DisableInt()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPI_EnableInt(SPIModule, SPI_UNIT_INT_MASK);
    CU_ASSERT_TRUE(SPIModule->CTL & 0x00020000);
    SPI_DisableInt(SPIModule, SPI_UNIT_INT_MASK);
    CU_ASSERT_FALSE(SPIModule->CTL & 0x00020000);

    SPI_EnableInt(SPIModule, SPI_SSACT_INT_MASK);
    CU_ASSERT_TRUE(SPIModule->SSCTL & 0x00001000);
    SPI_DisableInt(SPIModule, SPI_SSACT_INT_MASK);
    CU_ASSERT_FALSE(SPIModule->SSCTL & 0x00001000);

    SPI_EnableInt(SPIModule, SPI_SSINACT_INT_MASK);
    CU_ASSERT_TRUE(SPIModule->SSCTL & 0x00002000);
    SPI_DisableInt(SPIModule, SPI_SSINACT_INT_MASK);
    CU_ASSERT_FALSE(SPIModule->SSCTL & 0x00002000);

    SPI_EnableInt(SPIModule, SPI_SLVUR_INT_MASK);
    CU_ASSERT_TRUE(SPIModule->SSCTL & 0x00000200);
    SPI_DisableInt(SPIModule, SPI_SLVUR_INT_MASK);
    CU_ASSERT_FALSE(SPIModule->SSCTL & 0x00000200);

    SPI_EnableInt(SPIModule, SPI_SLVBE_INT_MASK);
    CU_ASSERT_TRUE(SPIModule->SSCTL & 0x00000100);
    SPI_DisableInt(SPIModule, SPI_SLVBE_INT_MASK);
    CU_ASSERT_FALSE(SPIModule->SSCTL & 0x00000100);

    SPI_EnableInt(SPIModule, SPI_TXUF_INT_MASK);
    CU_ASSERT_TRUE(SPIModule->FIFOCTL & 0x00000080);
    SPI_DisableInt(SPIModule, SPI_TXUF_INT_MASK);
    CU_ASSERT_FALSE(SPIModule->FIFOCTL & 0x00000080);

    SPI_EnableInt(SPIModule, SPI_FIFO_TXTH_INT_MASK);
    CU_ASSERT_TRUE(SPIModule->FIFOCTL & 0x00000008);
    SPI_DisableInt(SPIModule, SPI_FIFO_TXTH_INT_MASK);
    CU_ASSERT_FALSE(SPIModule->FIFOCTL & 0x00000008);

    SPI_EnableInt(SPIModule, SPI_FIFO_RXTH_INT_MASK);
    CU_ASSERT_TRUE(SPIModule->FIFOCTL & 0x00000004);
    SPI_DisableInt(SPIModule, SPI_FIFO_RXTH_INT_MASK);
    CU_ASSERT_FALSE(SPIModule->FIFOCTL & 0x00000004);

    SPI_EnableInt(SPIModule, SPI_FIFO_RXOV_INT_MASK);
    CU_ASSERT_TRUE(SPIModule->FIFOCTL & 0x00000020);
    SPI_DisableInt(SPIModule, SPI_FIFO_RXOV_INT_MASK);
    CU_ASSERT_FALSE(SPIModule->FIFOCTL & 0x00000020);

    SPI_EnableInt(SPIModule, SPI_FIFO_RXTO_INT_MASK);
    CU_ASSERT_TRUE(SPIModule->FIFOCTL & 0x00000010);
    SPI_DisableInt(SPIModule, SPI_FIFO_RXTO_INT_MASK);
    CU_ASSERT_FALSE(SPIModule->FIFOCTL & 0x00000010);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_SPI_GetIntFlag_ClearIntFlag()
{
    uint32_t u32DelayCount;
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    /* Unit transfer interrupt flag test */
    SPIModule->CLKDIV = 4;
    /* Master mode, SPI mode 0. */
    SPIModule->CTL = 5;
    SPIModule->TX = 0;

    while ((SPIModule->STATUS & 0x0000002) == 0) __NOP();

    /* Check unit transfer interrupt flag */
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_UNIT_INT_MASK) == SPI_UNIT_INT_MASK);
    SPI_ClearIntFlag(SPIModule, SPI_UNIT_INT_MASK);
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_UNIT_INT_MASK) == 0);
    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();

    /* Slave selection signal active/inactive/slave-under-run/bit-count-error/slave-underflow interrupt flag test */
    /* Set PA.3 (SPI0_SS) as output mode */
    SetSSPinToGPIO(GetSPIModuleIdx());

    /* Set PA.2 (SPI0_CLK) as GPIO output pin */
    SetCLKPinToGPIO(GetSPIModuleIdx());

    /* Enable self-test function */
    //outp32(0x40000014, 0x1);
    SYS->ALTCTL0 |= SYS_ALTCTL0_SELFTEST_Msk;
    SPI_ENABLE_SELFTEST(GetSPIModuleIdx());

    /* Set PA.3 (SPI0_SS) to high level */
    SetSSPinToHigh(GetSPIModuleIdx());

    SPIModule->CLKDIV = 0x1FF;
    /* Slave selection signal is low-active. */
    SPIModule->SSCTL |= 0;
    /* Slave mode, SPI mode 0. */
    SPIModule->CTL = 0x00040005;

    for (u32DelayCount = 0; u32DelayCount < 10; u32DelayCount++) __NOP();

    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_SSACT_INT_MASK) == 0);
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_SSINACT_INT_MASK) == 0);
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_TXUF_INT_MASK) == 0);

    /* Set PA.3 (SPI0_SS) to low level */
    //PA3 = 0;
    SetSSPinToLow(GetSPIModuleIdx());

    for (u32DelayCount = 0; u32DelayCount < 5000; u32DelayCount++) __NOP();

    /* Check slave selection signal active interrupt flag */
    //CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_SSACT_INT_MASK) == SPI_SSACT_INT_MASK);
    //SPI_ClearIntFlag(SPIModule, SPI_SSACT_INT_MASK);
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_SSACT_INT_MASK) == 0);
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_SSINACT_INT_MASK) == 0);
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_SLVUR_INT_MASK) == 0);
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_SLVBE_INT_MASK) == 0);

    /* Check slave TX underflow interrupt flag */
    //CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_TXUF_INT_MASK) == SPI_TXUF_INT_MASK);

    /* Set PA.3 (SPI0_SS) to high level */
    //PA3 = 1;
    SetSSPinToHigh(GetSPIModuleIdx());

    __NOP();
    /* Check slave selection signal inactive interrupt flag */
    //CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_SSINACT_INT_MASK) == SPI_SSINACT_INT_MASK);
    //SPI_ClearIntFlag(SPIModule, SPI_SSINACT_INT_MASK);
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_SSINACT_INT_MASK) == 0);

    /* Clear slave TX underflow interrupt flag */
    SPI_ClearIntFlag(SPIModule, SPI_TXUF_INT_MASK);
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_TXUF_INT_MASK) == 0);

    /* Check slave bit count error interrupt flag */
    //CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_SLVBE_INT_MASK) == SPI_SLVBE_INT_MASK);
    //SPI_ClearIntFlag(SPIModule, SPI_SLVBE_INT_MASK);
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_SLVBE_INT_MASK) == 0);

    /* Check slave TX under run interrupt flag */
    //CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_SLVUR_INT_MASK) == SPI_SLVUR_INT_MASK);
    //SPI_ClearIntFlag(SPIModule, SPI_SLVUR_INT_MASK);
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_SLVUR_INT_MASK) == 0);

    /* Disable self-test function */
    //outp32(0x40000014, 0x0);
    SYS->ALTCTL0 &= ~SYS_ALTCTL0_SELFTEST_Msk;
    SPI_DISABLE_SELFTEST(GetSPIModuleIdx());

    /* Set PA.2 (SPI0_CLK) as SPI function pin */
    //SYS->GPA_MFPL = (SYS->GPA_MFPL & ~SYS_GPA_MFPL_PA2MFP_Msk) | SYS_GPA_MFPL_PA2MFP_SPI0_CLK;
    SetSPICLKMFP(GetSPIModuleIdx());

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();


    /* TX threshold interrupt flag test */
    SPIModule->CLKDIV = 0xFF;
    /* Slave mode, SPI mode 0. */
    SPIModule->CTL = 0x00040005;
    /* Set TX and RX FIFO threshold */
    SPI_SetFIFO(SPIModule, 2, 2);
    /* Check TX threshold interrupt flag */
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_FIFO_TXTH_INT_MASK) == SPI_FIFO_TXTH_INT_MASK);
    /* Write to TX FIFO */
    SPIModule->TX = 1;
    SPIModule->TX = 2;
    SPIModule->TX = 3;
    SPIModule->TX = 4;
    /* Check TX threshold interrupt flag */
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_FIFO_TXTH_INT_MASK) == 0);
    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();

    /* RX threshold interrupt flag test */
    SPIModule->CLKDIV = 0xF;
    /* Set TX and RX FIFO threshold. */
    SPI_SetFIFO(SPIModule, 2, 2);
    /* Master mode, SPI mode 0. */
    SPIModule->CTL = 5;
    /* Check RX threshold interrupt flag */
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_FIFO_RXTH_INT_MASK) == 0);
    /* Write to TX FIFO */
    SPIModule->TX = 1;
    SPIModule->TX = 2;
    SPIModule->TX = 3;

    /* Wait data transfer */
    while (SPIModule->STATUS & SPI_STATUS_BUSY_Msk) __NOP();

    /* Check RX threshold interrupt flag */
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_FIFO_RXTH_INT_MASK) == SPI_FIFO_RXTH_INT_MASK);

    /* RX overrun interrupt flag test */
    /* Check RX overrun interrupt flag */
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_FIFO_RXOV_INT_MASK) == 0);
    /* Write to TX FIFO */
    SPIModule->TX = 4;
    SPIModule->TX = 5;
    SPIModule->TX = 6;
    SPIModule->TX = 7;
    SPIModule->TX = 8;
    SPIModule->TX = 9;

    /* Wait data transfer */
    while (SPIModule->STATUS & SPI_STATUS_BUSY_Msk) __NOP();

    /* Check RX overrun interrupt flag */
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_FIFO_RXOV_INT_MASK) == SPI_FIFO_RXOV_INT_MASK);
    SPI_ClearIntFlag(SPIModule, SPI_FIFO_RXOV_INT_MASK);
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_FIFO_RXOV_INT_MASK) == 0);

    /* RX time-out interrupt flag test */
    /* Clear RX time-out interrupt flag */
    SPI_ClearIntFlag(SPIModule, SPI_FIFO_RXTO_INT_MASK);
    /* Check RX time-out interrupt flag */
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_FIFO_RXTO_INT_MASK) == 0);
    /* Write to TX FIFO */
    SPIModule->TX = 1;
    SPIModule->TX = 2;
    SPIModule->TX = 3;

    /* Wait data transfer */
    while (SPIModule->STATUS & SPI_STATUS_BUSY_Msk) __NOP();

    /* Check RX time-out interrupt flag */
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_FIFO_RXTO_INT_MASK) == SPI_FIFO_RXTO_INT_MASK);
    SPI_ClearIntFlag(SPIModule, SPI_FIFO_RXTO_INT_MASK);
    CU_ASSERT(SPI_GetIntFlag(SPIModule, SPI_FIFO_RXTO_INT_MASK) == 0);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();
}


void API_SPI_GetStatus()
{
    uint32_t u32DelayCount;
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPIModule->CLKDIV = 0;
    /* Check SPIEN flag */
    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_SPIEN_STS_MASK) == 0);
    /* Slave mode, SPI mode 0. */
    SPIModule->CTL = 0x00040005;
    /* Check busy flag */
    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_BUSY_MASK) == 0);
    /* Check SPIEN flag */
    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_SPIEN_STS_MASK) == SPI_SPIEN_STS_MASK);

    /* SPI TX empty flag test */
    /* Check TX empty flag */
    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_TX_EMPTY_MASK) == SPI_TX_EMPTY_MASK);
    /* Write to TX FIFO */
    SPIModule->TX = 1;
    SPIModule->TX = 2;
    /* Check TX empty flag */
    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_TX_EMPTY_MASK) == 0);

    /* Check busy flag */
    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_BUSY_MASK) == SPI_BUSY_MASK);

    /* SPI TX full flag test */
    /* Check TX full flag */
    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_TX_FULL_MASK) == 0);
    /* Write to TX FIFO */
    SPIModule->TX = 3;
    SPIModule->TX = 4;
    SPIModule->TX = 5;
    SPIModule->TX = 6;
    SPIModule->TX = 7;
    SPIModule->TX = 8;
    SPIModule->TX = 9;
    /* Check TX full flag */
    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_TX_FULL_MASK) == SPI_TX_FULL_MASK);
    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();

    /* SPI RX empty flag test */
    SPIModule->CLKDIV = 0xFF;
    /* Master mode, SPI mode 0. */
    SPIModule->CTL = 5;

    /* Check TX/RX reset flag */
    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_TXRX_RESET_MASK) == 0);
    SPIModule->FIFOCTL |= SPI_FIFOCTL_RXRST_Msk;
    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_TXRX_RESET_MASK) == SPI_TXRX_RESET_MASK);

    //for(u32DelayCount=0; u32DelayCount<256; u32DelayCount++) __NOP();
    for (u32DelayCount = 0; u32DelayCount < 10000; u32DelayCount++) __NOP();

    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_TXRX_RESET_MASK) == 0);  //test!!

    /* Check RX empty flag */
    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_RX_EMPTY_MASK) == SPI_RX_EMPTY_MASK);
    /* Write to TX FIFO */
    SPIModule->TX = 1;

    /* Wait data transfer */
    while (SPIModule->STATUS & SPI_STATUS_BUSY_Msk) __NOP();

    /* Check RX empty flag */
    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_RX_EMPTY_MASK) == 0);

    /* SPI RX full flag test */
    /* Check RX full flag */
    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_RX_FULL_MASK) == 0);
    /* Write to TX FIFO */
    SPIModule->TX = 2;
    SPIModule->TX = 3;
    SPIModule->TX = 4;
    SPIModule->TX = 5;
    SPIModule->TX = 6;
    SPIModule->TX = 7;
    SPIModule->TX = 8;

    /* Wait data transfer */
    while (SPIModule->STATUS & SPI_STATUS_BUSY_Msk) __NOP();

    /* Check RX full flag */
    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_RX_FULL_MASK) == SPI_RX_FULL_MASK);
    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();


    /* Check SPIx_SS line status */
    /* Set PA.3 (SPI0_SS) as GPIO output mode */
    SetSSPinToGPIO(GetSPIModuleIdx());

    /* Enable self-test function */
    //outp32(0x40000014, 0x1);
    SYS->ALTCTL0 |= SYS_ALTCTL0_SELFTEST_Msk;
    SPI_ENABLE_SELFTEST(GetSPIModuleIdx());

    /* Set PA.3 (SPI0_SS) to high level */
    SetSSPinToHigh(GetSPIModuleIdx());

    SPIModule->CLKDIV = 0;
    /* Slave selection signal is low-active. */
    SPIModule->SSCTL = 0;
    /* Slave mode, SPI mode 0. */
    SPIModule->CTL = 0x00040005;

    for (u32DelayCount = 0; u32DelayCount < 10; u32DelayCount++) __NOP();

    CU_ASSERT(SPI_GetStatus(SPIModule, SPI_SSLINE_STS_MASK) == SPI_SSLINE_STS_MASK);
    /* Set PA.3 (SPI0_SS) to low level */
    SetSSPinToLow(GetSPIModuleIdx());

    for (u32DelayCount = 0; u32DelayCount < 1000; u32DelayCount++) __NOP();

    //CU_ASSERT(SPI_GetStatus(SPIModule, SPI_SSLINE_STS_MASK) == 0);
    //__NOP();

    /* Disable self-test function */
    //outp32(0x40000014, 0x0);
    SYS->ALTCTL0 &= ~SYS_ALTCTL0_SELFTEST_Msk;
    SPI_DISABLE_SELFTEST(GetSPIModuleIdx());

    for (u32DelayCount = 0; u32DelayCount < 1000; u32DelayCount++) __NOP();

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();
}

/* -------------- I2S test -------------- */

void MACRO_I2S_ENABLE_DISABLE_TX_ZCD()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPII2S_ENABLE_TX_ZCD(SPIModule, SPII2S_RIGHT);
    /* Check RZCIEN bit */
    CU_ASSERT_TRUE(SPIModule->I2SCTL & 0x00010000);
    SPII2S_DISABLE_TX_ZCD(SPIModule, SPII2S_RIGHT);
    /* Check RZCIEN bit */
    CU_ASSERT_FALSE(SPIModule->I2SCTL & 0x00010000);
    SPII2S_ENABLE_TX_ZCD(SPIModule, SPII2S_LEFT);
    /* Check LZCIEN bit */
    CU_ASSERT_TRUE(SPIModule->I2SCTL & 0x00020000);
    SPII2S_DISABLE_TX_ZCD(SPIModule, SPII2S_LEFT);
    /* Check LZCIEN bit */
    CU_ASSERT_FALSE(SPIModule->I2SCTL & 0x00020000);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_I2S_ENABLE_DISABLE_TXDMA()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPII2S_ENABLE_TXDMA(SPIModule);
    /* Check TXPDMAEN bit */
    CU_ASSERT_TRUE(SPIModule->PDMACTL & 0x00000001);
    SPII2S_DISABLE_TXDMA(SPIModule);
    /* Check TXPDMAEN bit */
    CU_ASSERT_FALSE(SPIModule->PDMACTL & 0x00000001);
    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_I2S_ENABLE_DISABLE_RXDMA()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPII2S_ENABLE_RXDMA(SPIModule);
    /* Check RXPDMAEN bit */
    CU_ASSERT_TRUE(SPIModule->PDMACTL & 0x00000002);
    SPII2S_DISABLE_RXDMA(SPIModule);
    /* Check RXPDMAEN bit */
    CU_ASSERT_FALSE(SPIModule->PDMACTL & 0x00000002);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_I2S_ENABLE_DISABLE_TX()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPII2S_ENABLE_TX(SPIModule);
    /* Check TX enable bit */
    CU_ASSERT_TRUE(SPIModule->I2SCTL & 2);
    SPII2S_DISABLE_TX(SPIModule);
    /* Check TX enable bit */
    CU_ASSERT_FALSE(SPIModule->I2SCTL & 2);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_I2S_ENABLE_DISABLE_RX()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPII2S_ENABLE_RX(SPIModule);
    /* Check RX enable bit */
    CU_ASSERT_TRUE(SPIModule->I2SCTL & 4);
    SPII2S_DISABLE_RX(SPIModule);
    /* Check RX enable bit */
    CU_ASSERT_FALSE(SPIModule->I2SCTL & 4);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_I2S_ENABLE_DISABLE_TX_MUTE()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPII2S_ENABLE_TX_MUTE(SPIModule);
    /* Check TX mute enable bit */
    CU_ASSERT_TRUE(SPIModule->I2SCTL & 8);
    SPII2S_DISABLE_TX_MUTE(SPIModule);
    /* Check TX mute enable bit */
    CU_ASSERT_FALSE(SPIModule->I2SCTL & 8);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_I2S_CLR_TX_FIFO()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    /* Write 3 data to TX FIFO */
    SPII2S_WRITE_TX_FIFO(SPIModule, 1);
    SPII2S_WRITE_TX_FIFO(SPIModule, 2);
    SPII2S_WRITE_TX_FIFO(SPIModule, 3);

    SPII2S_CLR_TX_FIFO(SPIModule);

    CU_ASSERT(SPII2S_GET_TX_FIFO_LEVEL(SPIModule) == 0);
    /* CLR_TXFIFO bit should be cleared by hardware automatically */
    CU_ASSERT_FALSE(SPIModule->FIFOCTL & 0x00000200);
}

void MACRO_I2S_CLR_RX_FIFO()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    /* Enable I2S RX function */
    SPIModule->I2SCTL = 5;

    while (SPIModule->STATUS & SPI_I2SSTS_RXEMPTY_Msk);

    /* Disable I2S RX function */
    SPIModule->I2SCTL = 0;

    SPII2S_CLR_RX_FIFO(SPIModule);

    CU_ASSERT(SPII2S_GET_RX_FIFO_LEVEL(SPIModule) == 0);
    /* CLR_RXFIFO bit should be cleared by hardware automatically */
    CU_ASSERT_FALSE(SPIModule->FIFOCTL & 0x00000100);
}

void MACRO_I2S_SET_MONO_RX_CHANNEL()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPII2S_SET_MONO_RX_CHANNEL(SPIModule, SPII2S_MONO_LEFT);
    /* Check RXLCH bit */
    CU_ASSERT_TRUE(SPIModule ->I2SCTL & 0x00800000);
    SPII2S_SET_MONO_RX_CHANNEL(SPIModule, SPII2S_MONO_RIGHT);
    /* Check RXLCH bit */
    CU_ASSERT_FALSE(SPIModule ->I2SCTL & 0x00800000);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_I2S_WRITE_TX_FIFO()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    /* Set I2S bit clock divider */
    SPIModule->I2SCLK = 0x1FF00;
    /* Slave mode; I2S enabled. */
    SPIModule->I2SCTL = 0x00000101;

    /* Write 2 data to TX FIFO */
    SPII2S_WRITE_TX_FIFO(SPIModule, 1);
    SPII2S_WRITE_TX_FIFO(SPIModule, 2);

    CU_ASSERT(SPII2S_GET_TX_FIFO_LEVEL(SPIModule) == 2);

    SPII2S_WRITE_TX_FIFO(SPIModule, 3);

    CU_ASSERT(SPII2S_GET_TX_FIFO_LEVEL(SPIModule) == 3);

    SPII2S_CLR_TX_FIFO(SPIModule);
    CU_ASSERT(SPII2S_GET_TX_FIFO_LEVEL(SPIModule) == 0);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();
}

void MACRO_I2S_READ_RX_FIFO()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    /* Set I2S bit clock divider */
    SPIModule->I2SCLK = 0x8000;
    /* Enable I2S RX function */
    SPIModule->I2SCTL = 5;

    while ((SPIModule->I2SSTS & 0x0F000000) != 0x03000000);

    /* Disable I2S RX function */
    SPIModule->I2SCTL = 1;

    SPII2S_READ_RX_FIFO(SPIModule);

    CU_ASSERT(SPII2S_GET_RX_FIFO_LEVEL(SPIModule) == 2);

    SPII2S_READ_RX_FIFO(SPIModule);

    CU_ASSERT(SPII2S_GET_RX_FIFO_LEVEL(SPIModule) == 1);

    SPII2S_READ_RX_FIFO(SPIModule);

    CU_ASSERT(SPII2S_GET_RX_FIFO_LEVEL(SPIModule) == 0);
}

void MACRO_I2S_GET_CLEAR_INT_FLAG()
{
    uint32_t u32DelayCount;
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    /* Left channel zero-cross flag test */
    /* Set I2S bit clock divider */
    SPIModule->I2SCLK = 0x8000;
    /* Enable left channel zero-cross detection function, I2S RX function, I2S TX function and I2S controller. */
    SPIModule->I2SCTL = 0x00020007;

    while (SPII2S_GET_RX_FIFO_LEVEL(SPIModule) <= 2);

    SPIModule->I2SCTL = 1;

    CU_ASSERT(SPII2S_GET_INT_FLAG(SPIModule, SPI_I2SSTS_LZCIF_Msk) == 0x00200000);

    SPII2S_CLR_INT_FLAG(SPIModule, SPI_I2SSTS_LZCIF_Msk);

    CU_ASSERT(SPII2S_GET_INT_FLAG(SPIModule, SPI_I2SSTS_LZCIF_Msk) == 0);
    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();

    /* Right channel zero-cross flag test */
    /* Set I2S bit clock divider */
    SPIModule->I2SCLK = 0x8000;
    /* Enable right channel zero-cross detection function, I2S RX function, I2S TX function and I2S controller. */
    SPIModule->I2SCTL = 0x00010007;

    while (SPII2S_GET_RX_FIFO_LEVEL(SPIModule) <= 2);

    SPIModule->I2SCTL = 1;

    CU_ASSERT(SPII2S_GET_INT_FLAG(SPIModule, SPI_I2SSTS_RZCIF_Msk) == 0x00100000);

    SPII2S_CLR_INT_FLAG(SPIModule, SPI_I2SSTS_RZCIF_Msk);

    CU_ASSERT(SPII2S_GET_INT_FLAG(SPIModule, SPI_I2SSTS_RZCIF_Msk) == 0);
    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();

    /* TX threshold interrupt flag test */
    /* Set I2S bit clock divider */
    SPIModule->I2SCLK = 0x8000;
    /* Enable I2S controller and TX function; set TX threshold as 2. */
    SPIModule->FIFOCTL = 0x20000000;
    SPIModule->I2SCTL = 0x3;

    CU_ASSERT(SPII2S_GET_INT_FLAG(SPIModule, SPI_I2SSTS_TXTHIF_Msk) == 0x00040000);

    /* Write 4 data to TX FIFO */
    SPII2S_WRITE_TX_FIFO(SPIModule, 1);
    SPII2S_WRITE_TX_FIFO(SPIModule, 2);
    SPII2S_WRITE_TX_FIFO(SPIModule, 3);
    SPII2S_WRITE_TX_FIFO(SPIModule, 4);

    CU_ASSERT(SPII2S_GET_INT_FLAG(SPIModule, SPI_I2SSTS_TXTHIF_Msk) == 0);
    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();

    /* TX underflow interrupt flag test */
    /* Set I2S bit clock divider */
    SPIModule->I2SCLK = 0x8000;
    /* Enable I2S RX function, I2S TX function and I2S controller. */
    SPIModule->I2SCTL = 0x00000007;

    while (SPII2S_GET_RX_FIFO_LEVEL(SPIModule) <= 2);

    SPIModule->I2SCTL = 1;

    CU_ASSERT(SPII2S_GET_INT_FLAG(SPIModule, SPI_I2SSTS_TXUFIF_Msk) == 0x00080000);

    SPII2S_CLR_INT_FLAG(SPIModule, SPI_I2SSTS_TXUFIF_Msk);

    CU_ASSERT(SPII2S_GET_INT_FLAG(SPIModule, SPI_I2SSTS_TXUFIF_Msk) == 0);
    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();

    /* RX threshold interrupt flag test */
    /* Set I2S bit clock divider */
    SPIModule->I2SCLK = 0xFF00;
    /* Set RX threshold as 2; enable I2S RX function and I2S controller. */
    SPIModule->FIFOCTL = 0x02000000;
    SPIModule->I2SCTL = 0x00000005;

    while (SPII2S_GET_RX_FIFO_LEVEL(SPIModule) <= 2) __NOP();

    //SPIModule->I2SCTL = 0;

    CU_ASSERT(SPII2S_GET_INT_FLAG(SPIModule, SPI_I2SSTS_RXTHIF_Msk) == 0x00000400);

    SPII2S_READ_RX_FIFO(SPIModule);
    SPII2S_READ_RX_FIFO(SPIModule);

    CU_ASSERT(SPII2S_GET_INT_FLAG(SPIModule, SPI_I2SSTS_RXTHIF_Msk) == 0);
    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();

    /* RX overflow interrupt flag test */
    /* Set I2S bit clock divider */
    SPIModule->I2SCLK = 0x8000;
    /* Enable I2S RX function, I2S TX function and I2S controller. */
    SPIModule->I2SCTL = 0x00000007;

    while (SPII2S_GET_RX_FIFO_LEVEL(SPIModule) != 4);

    /* Write 4 data to TX FIFO */
    SPII2S_WRITE_TX_FIFO(SPIModule, 1);
    SPII2S_WRITE_TX_FIFO(SPIModule, 2);
    SPII2S_WRITE_TX_FIFO(SPIModule, 3);
    SPII2S_WRITE_TX_FIFO(SPIModule, 4);

    while (SPII2S_GET_TX_FIFO_LEVEL(SPIModule) != 0);

    SPIModule->I2SCTL = 1;

    CU_ASSERT(SPII2S_GET_INT_FLAG(SPIModule, SPI_I2SSTS_RXOVIF_Msk) == 0x00000800);

    SPII2S_CLR_INT_FLAG(SPIModule, SPI_I2SSTS_RXOVIF_Msk);

    CU_ASSERT(SPII2S_GET_INT_FLAG(SPIModule, SPI_I2SSTS_RXOVIF_Msk) == 0);
    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();

    /* RX time-out interrupt flag test */
    /* Set I2S bit clock divider */
    SPIModule->I2SCLK = 0x100;
    /* Enable I2S RX function, TX function and I2S controller */
    SPIModule->I2SCTL = 7;

    SPII2S_WRITE_TX_FIFO(SPIModule, 1);
    SPII2S_WRITE_TX_FIFO(SPIModule, 2);
    SPII2S_WRITE_TX_FIFO(SPIModule, 3);
    SPII2S_WRITE_TX_FIFO(SPIModule, 4);

    while (SPII2S_GET_TX_FIFO_LEVEL(SPIModule) != 0) __NOP();

    for (u32DelayCount = 0; u32DelayCount < 1000; u32DelayCount++) __NOP();

    CU_ASSERT(SPII2S_GET_INT_FLAG(SPIModule, SPI_I2SSTS_RXTOIF_Msk) == 0x00001000);

    SPII2S_CLR_INT_FLAG(SPIModule, SPI_I2SSTS_RXTOIF_Msk);

    CU_ASSERT(SPII2S_GET_INT_FLAG(SPIModule, SPI_I2SSTS_RXTOIF_Msk) == 0);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
    __NOP();
}

void MACRO_I2S_GET_TX_FIFO_LEVEL()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    /* Set I2S bit clock divider */
    SPIModule->I2SCLK = 0x1FF00;
    /* Enable I2S controller */
    SPIModule->I2SCTL = 1;

    /* Write data to TX FIFO */
    SPII2S_WRITE_TX_FIFO(SPIModule, 1);

    CU_ASSERT(SPII2S_GET_TX_FIFO_LEVEL(SPIModule) == 1);

    SPII2S_WRITE_TX_FIFO(SPIModule, 2);

    CU_ASSERT(SPII2S_GET_TX_FIFO_LEVEL(SPIModule) == 2);

    SPII2S_WRITE_TX_FIFO(SPIModule, 3);

    CU_ASSERT(SPII2S_GET_TX_FIFO_LEVEL(SPIModule) == 3);

    SPII2S_WRITE_TX_FIFO(SPIModule, 4);

    CU_ASSERT(SPII2S_GET_TX_FIFO_LEVEL(SPIModule) == 4);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void MACRO_I2S_GET_RX_FIFO_LEVEL()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    /* Set I2S bit clock divider */
    SPIModule->I2SCLK = 0x1FF00;
    /* Enable I2S RX function and I2S controller */
    SPIModule->I2SCTL = 5;

    while ((SPIModule->I2SSTS & 0x0F000000) != 0x01000000) __NOP();

    CU_ASSERT(SPII2S_GET_RX_FIFO_LEVEL(SPIModule) == 1);

    while ((SPIModule->I2SSTS & 0x0F000000) != 0x02000000) __NOP();

    CU_ASSERT(SPII2S_GET_RX_FIFO_LEVEL(SPIModule) == 2);

    while ((SPIModule->I2SSTS & 0x0F000000) != 0x03000000) __NOP();

    CU_ASSERT(SPII2S_GET_RX_FIFO_LEVEL(SPIModule) == 3);

    while ((SPIModule->I2SSTS & 0x0F000000) != 0x04000000) __NOP();

    CU_ASSERT(SPII2S_GET_RX_FIFO_LEVEL(SPIModule) == 4);

    /* Disable I2S RX function */
    SPIModule->I2SCTL = 0;

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_I2S_Open()
{
    uint32_t u32ReturnValue;
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPI_CLK_Sel(GetSPIModuleIdx(), eSPI_CLK_HIRC48M) ;
    u32ReturnValue = SPII2S_Open(SPIModule, SPII2S_MODE_MASTER, 16000, SPII2S_DATABIT_24, SPII2S_MONO, SPII2S_FORMAT_PCMA);
    CU_ASSERT(SPIModule->I2SCTL == 0x20000067);
    CU_ASSERT(SPIModule->FIFOCTL == 0x21000000);
    CU_ASSERT(SPIModule->I2SCLK == 0x00001E00);
    CU_ASSERT(u32ReturnValue == 16129);

    ResetSPI(GetSPIModuleIdx());
    __NOP();

    u32ReturnValue = SPII2S_Open(SPIModule, SPII2S_MODE_SLAVE, 16000, SPII2S_DATABIT_32, SPII2S_STEREO, SPII2S_FORMAT_PCMB);
    CU_ASSERT(SPIModule->I2SCTL == 0x30000137);
    CU_ASSERT(SPIModule->FIFOCTL == 0x21000000);
    CU_ASSERT(SPIModule->I2SCLK == 0x00000000);
    CU_ASSERT(u32ReturnValue == 48000000);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_I2S_Close()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPIModule->I2SCTL = 1;
    SPII2S_Close(SPIModule);
    CU_ASSERT_FALSE(SPIModule->I2SCTL & 1);
    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_I2S_EnableInt_DisableInt()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPII2S_EnableInt(SPIModule, SPII2S_FIFO_TXTH_INT_MASK);
    CU_ASSERT((SPIModule->FIFOCTL & 0x8) == 0x8);
    SPII2S_DisableInt(SPIModule, SPII2S_FIFO_TXTH_INT_MASK);
    CU_ASSERT((SPIModule->FIFOCTL & 0x8) == 0);

    SPII2S_EnableInt(SPIModule, SPII2S_FIFO_RXTH_INT_MASK);
    CU_ASSERT((SPIModule->FIFOCTL & 0x4) == 0x4);
    SPII2S_DisableInt(SPIModule, SPII2S_FIFO_RXTH_INT_MASK);
    CU_ASSERT((SPIModule->FIFOCTL & 0x4) == 0);

    SPII2S_EnableInt(SPIModule, SPII2S_FIFO_RXOV_INT_MASK);
    CU_ASSERT((SPIModule->FIFOCTL & 0x20) == 0x20);
    SPII2S_DisableInt(SPIModule, SPII2S_FIFO_RXOV_INT_MASK);
    CU_ASSERT((SPIModule->FIFOCTL & 0x20) == 0);

    SPII2S_EnableInt(SPIModule, SPII2S_FIFO_RXTO_INT_MASK);
    CU_ASSERT((SPIModule->FIFOCTL & 0x10) == 0x10);
    SPII2S_DisableInt(SPIModule, SPII2S_FIFO_RXTO_INT_MASK);
    CU_ASSERT((SPIModule->FIFOCTL & 0x10) == 0);

    SPII2S_EnableInt(SPIModule, SPII2S_TXUF_INT_MASK);
    CU_ASSERT((SPIModule->FIFOCTL & 0x80) == 0x80);
    SPII2S_DisableInt(SPIModule, SPII2S_TXUF_INT_MASK);
    CU_ASSERT((SPIModule->FIFOCTL & 0x80) == 0);

    SPII2S_EnableInt(SPIModule, SPII2S_RIGHT_ZC_INT_MASK);
    CU_ASSERT(SPIModule->I2SCTL == 0x01000000);
    SPII2S_DisableInt(SPIModule, SPII2S_RIGHT_ZC_INT_MASK);
    CU_ASSERT(SPIModule->I2SCTL == 0);

    SPII2S_EnableInt(SPIModule, SPII2S_LEFT_ZC_INT_MASK);
    CU_ASSERT(SPIModule->I2SCTL == 0x02000000);
    SPII2S_DisableInt(SPIModule, SPII2S_LEFT_ZC_INT_MASK);
    CU_ASSERT(SPIModule->I2SCTL == 0);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_I2S_EnableMCLK()
{
    uint32_t u32ReturnValue;
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPI_CLK_Sel(GetSPIModuleIdx(), eSPI_CLK_HIRC48M) ;
    u32ReturnValue = SPII2S_EnableMCLK(SPIModule, 2048000);
    CU_ASSERT(SPIModule ->I2SCTL == 0x00008000);
    CU_ASSERT(SPIModule ->I2SCLK == 0x0000000B);
    CU_ASSERT(u32ReturnValue == 2181818);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());

    u32ReturnValue = SPII2S_EnableMCLK(SPIModule, 48000000);
    CU_ASSERT(SPIModule ->I2SCTL == 0x00008000);
    CU_ASSERT(SPIModule ->I2SCLK == 0x00000000);
    CU_ASSERT(u32ReturnValue == 48000000);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_I2S_DisableMCLK()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    /* Set MCLKEN bit to 1 */
    SPIModule->I2SCTL = 0x8000;
    SPII2S_DisableMCLK(SPIModule);
    /* Check MCLKEN bit */
    CU_ASSERT_FALSE(SPIModule ->I2SCTL & 0x8000);

    /* Reset SPI */
    ResetSPI(GetSPIModuleIdx());
}

void API_I2S_SetFIFO()
{
    SPI_T *SPIModule = NULL;

    SPIModule = (SPI_T *)GetSPIModule(GetSPIModuleIdx());

    SPII2S_SetFIFO(SPIModule, 0, 3);
    CU_ASSERT((SPIModule->FIFOCTL & 0xFF000000) == (SPII2S_FIFO_TX_LEVEL_WORD_0 | SPII2S_FIFO_RX_LEVEL_WORD_4));

    SPII2S_SetFIFO(SPIModule, 3, 0);
    CU_ASSERT((SPIModule->FIFOCTL & 0xFF000000) == (SPII2S_FIFO_TX_LEVEL_WORD_3 | SPII2S_FIFO_RX_LEVEL_WORD_1));

    SPII2S_SetFIFO(SPIModule, 2, 2);
    CU_ASSERT((SPIModule->FIFOCTL & 0xFF000000) == (SPII2S_FIFO_TX_LEVEL_WORD_2 | SPII2S_FIFO_RX_LEVEL_WORD_3));

    SPII2S_SetFIFO(SPIModule, 1, 1);
    CU_ASSERT((SPIModule->FIFOCTL & 0xFF000000) == (SPII2S_FIFO_TX_LEVEL_WORD_1 | SPII2S_FIFO_RX_LEVEL_WORD_2));
}

CU_TestInfo SPI_I2S_MacroTests[] =
{
    {"Clear SPI unit transfer interrupt flag", MACRO_SPI_CLR_UNIT_TRANS_INT_FLAG},
    {"Trigger/Disable SPI RX DMA transfer", MACRO_SPI_TRIGGER_DISABLE_RX_PDMA},
    {"Trigger/Disable SPI TX DMA transfer", MACRO_SPI_TRIGGER_DISABLE_TX_PDMA},
    {"Get SPI RX FIFO count and empty flag", MACRO_SPI_GET_RX_FIFO_COUNT_EMPTY_FLAG},
    {"Get SPI TX FIFO empty and full flag", MACRO_SPI_GET_TX_FIFO_EMPTY_FULL_FLAG},
    {"Read SPI RX", MACRO_SPI_READ_RX},
    {"Write SPI TX", MACRO_SPI_WRITE_TX},
    {"Set SPI SS pin to high state", MACRO_SPI_SET_SS_HIGH},
    {"Set SPI SS pin to low state", MACRO_SPI_SET_SS_LOW},
    {"Enable/Disable SPI Byte Reorder function", MACRO_SPI_ENABLE_DISABLE_BYTE_REORDER},
    {"Set SPI suspend cycle", MACRO_SPI_SET_SUSPEND_CYCLE},
    {"Set SPI transfer sequence with LSB/MSB first", MACRO_SPI_SET_LSB_MSB_FIRST},
    {"Set SPI data width", MACRO_SPI_SET_DATA_WIDTH},
    {"Get SPI busy flag", MACRO_SPI_IS_BUSY},
    {"Enable/Disable SPI", MACRO_SPI_ENABLE_DISABLE},

    {"Enable/Disable I2S TX zero-cross function", MACRO_I2S_ENABLE_DISABLE_TX_ZCD},
    {"Enable/Disable I2S TX PDMA function", MACRO_I2S_ENABLE_DISABLE_TXDMA},
    {"Enable/Disable I2S RX PDMA function", MACRO_I2S_ENABLE_DISABLE_RXDMA},
    {"Enable/Disable I2S TX function", MACRO_I2S_ENABLE_DISABLE_TX},
    {"Enable/Disable I2S RX function", MACRO_I2S_ENABLE_DISABLE_RX},
    {"Enable/Disable I2S TX mute function", MACRO_I2S_ENABLE_DISABLE_TX_MUTE},
    {"Clear I2S TX FIFO", MACRO_I2S_CLR_TX_FIFO},
    {"Clear I2S RX FIFO", MACRO_I2S_CLR_RX_FIFO},
    {"Select RX channel in I2S MONO mode", MACRO_I2S_SET_MONO_RX_CHANNEL},
    {"Write I2S TX FIFO", MACRO_I2S_WRITE_TX_FIFO},
    {"Read I2S RX FIFO", MACRO_I2S_READ_RX_FIFO},
    {"Get/Clear I2S interrupt flag", MACRO_I2S_GET_CLEAR_INT_FLAG},
    {"Get I2S TX FIFO level", MACRO_I2S_GET_TX_FIFO_LEVEL},
    {"Get I2S RX FIFO level", MACRO_I2S_GET_RX_FIFO_LEVEL},

    CU_TEST_INFO_NULL
};

CU_TestInfo SPI_I2S_ApiTests[] =
{
    {"SPI Open", API_SPI_Open},
    {"SPI Close", API_SPI_Close},
    {"Clear SPI RX FIFO", API_SPI_ClearRxFIFO},
    {"Clear SPI TX FIFO", API_SPI_ClearTxFIFO},
    {"Enable/Disable SPI automatic slave selection function", API_SPI_EnableAutoSS_DisableAutoSS},
    {"Set/Get SPI bus clock rate", API_SPI_SetBusClock_GetBusClock},
    {"Set SPI FIFO threshold", API_SPI_SetFIFO},
    {"Enable/Disable SPI interrupt function", API_SPI_EnableInt_DisableInt},
    {"Get/Clear SPI interrupt flag", API_SPI_GetIntFlag_ClearIntFlag},
    {"Get SPI status", API_SPI_GetStatus},

    {"I2S Open", API_I2S_Open},
    {"I2S Close", API_I2S_Close},
    {"Enable/Disable I2S interrupt function", API_I2S_EnableInt_DisableInt},
    {"Enable MCLK", API_I2S_EnableMCLK},
    {"Disable MCLK", API_I2S_DisableMCLK},
    {"Set I2S FIFO threshold", API_I2S_SetFIFO},

    CU_TEST_INFO_NULL
};

CU_SuiteInfo suites[] =
{
    {"SPI/I2S MACRO", SPI_I2S_Tests_Init, SPI_I2S_Tests_Clean, NULL, NULL, SPI_I2S_MacroTests},
    {"SPI/I2S API", SPI_I2S_Tests_Init, SPI_I2S_Tests_Clean, NULL, NULL, SPI_I2S_ApiTests},
    CU_SUITE_INFO_NULL
};

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
