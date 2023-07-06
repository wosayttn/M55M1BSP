/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.           */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   LPspi_i2s_cunit.c                                                                                       */
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
#include "lpspi_cunit.h"
#include "lpspi_common.h"
#include "PinConfig.h"

//------------------------------------------------------------------------------
extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;

static LPSPI_T *gapSPIModule[] =
{
    LPSPI0,
};

static uint32_t gu32SPIModuleIdx = 0;

/*----------------------------------------------------------------------------*/
/* Test function                                                              */
/*----------------------------------------------------------------------------*/
void SetLPSPIModuleIdx(uint32_t u32SetValue)
{
    gu32SPIModuleIdx = u32SetValue;
}

uint32_t GetLPSPIModuleIdx(void)
{
    return gu32SPIModuleIdx;
}

void *GetLPSPIModule(uint32_t u32ModuleIdx)
{
    return gapSPIModule[u32ModuleIdx];
}

int LPSPI_Tests_Init(void)
{
    LPSPI_CLK_Sel(GetLPSPIModuleIdx(), eLPSPI_CLK_HIRC);

    return 0;
}

int LPSPI_Tests_Clean(void)
{
    LPSPI_ClkDisable(GetLPSPIModuleIdx());

    return 0;
}

/* Set LPSPI_SS as output mode */
void SetSSPinToGPIO(uint32_t u32SPIModule)
{
    switch (u32SPIModule)
    {
        case C_LPSPI0:
            SET_GPIO_PA3();
            GPIO_SetMode(PA, BIT3, GPIO_MODE_OUTPUT);
            break;
    }
}

void SetSSPinToLow(uint32_t u32SPIModule)
{
    switch (u32SPIModule)
    {
        case C_LPSPI0:
            PA3 = 0;
            break;
    }
}

void SetSSPinToHigh(uint32_t u32SPIModule)
{
    switch (u32SPIModule)
    {
        case C_LPSPI0:
            PA3 = 1;
            break;
    }
}

/* Set LPSPI_CLK as GPIO output pin */
void SetCLKPinToGPIO(uint32_t u32SPIModule)
{
    switch (u32SPIModule)
    {
        case C_LPSPI0:
            SET_GPIO_PA2();
            GPIO_SetMode(PA, BIT2, GPIO_MODE_OUTPUT);
            PA2 = 0;
            break;
    }
}

void SetSPICLKMFP(uint32_t u32SPIModule)
{
    switch (u32SPIModule)
    {
        case C_LPSPI0:
            LPSPI0_CLK_PIN_INIT;
            break;
    }
}

/* Set LPSPI_MOSI as GPIO output pin */
void SetMOSIPinToGPIO(uint32_t u32SPIModule)
{

    switch (u32SPIModule)
    {
        case C_LPSPI0:
            SET_GPIO_PA0();
            GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
            break;
    }
}

void LPSPI_DisableSelfTest(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    SYS->ALTCTL0 &= ~(SYS_ALTCTL0_SELFTEST_Msk);

    outpw(LPSPI0_BASE + INTERNAL_REG_BASE, ~SPI_INTERNAL_SELFTEST_Msk);

    /* Lock protected registers */
    SYS_LockReg();
}

void LPSPI_EnableSelfTest(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    SYS->ALTCTL0 |= SYS_ALTCTL0_SELFTEST_Msk;

    outpw(LPSPI0_BASE + INTERNAL_REG_BASE, SPI_INTERNAL_SELFTEST_Msk);

    /* Lock protected registers */
    SYS_LockReg();
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
void MACRO_LPSPI_CLR_UNIT_TRANS_INT_FLAG()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPIModule->CLKDIV = 4;
    LPSPIModule->CTL = 5;
    LPSPIModule->TX = 1;

    while ((LPSPIModule->STATUS & 0x00000002) == 0) __NOP();

    LPSPI_CLR_UNIT_TRANS_INT_FLAG(LPSPIModule);
    CU_ASSERT((LPSPIModule->STATUS & 0x00000002) == 0);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void MACRO_LPSPI_TRIGGER_DISABLE_RX_PDMA()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    CU_ASSERT((LPSPIModule->PDMACTL & 0x00000002) == 0);
    LPSPI_TRIGGER_RX_PDMA(LPSPIModule);

    /* Check RXPDMAEN bit */
    CU_ASSERT((LPSPIModule->PDMACTL & 0x00000002) == 0x00000002);
    LPSPI_DISABLE_RX_PDMA(LPSPIModule);
    CU_ASSERT((LPSPIModule->PDMACTL & 0x00000002) == 0);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void MACRO_LPSPI_TRIGGER_DISABLE_TX_PDMA()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    /* Check TXPDMAEN bit */
    CU_ASSERT((LPSPIModule->PDMACTL & 0x00000001) == 0);
    LPSPI_TRIGGER_TX_PDMA(LPSPIModule);
    /* Check TXPDMAEN bit */
    CU_ASSERT((LPSPIModule->PDMACTL & 0x00000001) == 0x00000001);
    LPSPI_DISABLE_TX_PDMA(LPSPIModule);
    CU_ASSERT((LPSPIModule->PDMACTL & 0x00000001) == 0);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void MACRO_LPSPI_GET_RX_FIFO_COUNT_EMPTY_FLAG()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPIModule->CLKDIV = 0xFF;
    /* Master mode, SPI mode 0. */
    LPSPIModule->CTL = 0x00000005;
    /* Check RX_EMPTY flag */
    CU_ASSERT(LPSPI_GET_RX_FIFO_EMPTY_FLAG(LPSPIModule) == 1);
    LPSPIModule->TX = 1;

    /* Check unit transfer interrupt flag */
    while ((LPSPIModule->STATUS & 0x00000002) == 0) __NOP();

    LPSPI_CLR_UNIT_TRANS_INT_FLAG(LPSPIModule);
    CU_ASSERT(LPSPI_GET_RX_FIFO_COUNT(LPSPIModule) == 1);
    /* Check RX_EMPTY flag */
    CU_ASSERT(LPSPI_GET_RX_FIFO_EMPTY_FLAG(LPSPIModule) == 0);
    LPSPIModule->TX = 2;

    while ((LPSPIModule->STATUS & 0x00000002) == 0) __NOP();

    LPSPI_CLR_UNIT_TRANS_INT_FLAG(LPSPIModule);
    CU_ASSERT(LPSPI_GET_RX_FIFO_COUNT(LPSPIModule) == 2);
    LPSPIModule->TX = 3;

    while ((LPSPIModule->STATUS & 0x00000002) == 0) __NOP();

    LPSPI_CLR_UNIT_TRANS_INT_FLAG(LPSPIModule);
    CU_ASSERT(LPSPI_GET_RX_FIFO_COUNT(LPSPIModule) == 3);
    LPSPIModule->TX = 4;

    while ((LPSPIModule->STATUS & 0x00000002) == 0) __NOP();

    LPSPI_CLR_UNIT_TRANS_INT_FLAG(LPSPIModule);
    CU_ASSERT(LPSPI_GET_RX_FIFO_COUNT(LPSPIModule) == 4);
    LPSPIModule->TX = 5;

    while ((LPSPIModule->STATUS & 0x00000002) == 0) __NOP();

    LPSPI_CLR_UNIT_TRANS_INT_FLAG(LPSPIModule);
    CU_ASSERT(LPSPI_GET_RX_FIFO_COUNT(LPSPIModule) == 4);
    LPSPIModule->TX = 6;

    while ((LPSPIModule->STATUS & 0x00000002) == 0) __NOP();

    LPSPI_CLR_UNIT_TRANS_INT_FLAG(LPSPIModule);
    CU_ASSERT(LPSPI_GET_RX_FIFO_COUNT(LPSPIModule) == 4);
    LPSPIModule->TX = 7;

    while ((LPSPIModule->STATUS & 0x00000002) == 0) __NOP();

    LPSPI_CLR_UNIT_TRANS_INT_FLAG(LPSPIModule);
    CU_ASSERT(LPSPI_GET_RX_FIFO_COUNT(LPSPIModule) == 4);
    LPSPIModule->TX = 8;

    while ((LPSPIModule->STATUS & 0x00000002) == 0) __NOP();

    LPSPI_CLR_UNIT_TRANS_INT_FLAG(LPSPIModule);
    CU_ASSERT(LPSPI_GET_RX_FIFO_COUNT(LPSPIModule) == 4);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void MACRO_LPSPI_GET_TX_FIFO_EMPTY_FULL_FLAG()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPIModule->CLKDIV = 0xFF;
    /* Slave mode, SPI mode 0. */
    LPSPIModule->CTL = 0x00040005;
    /* Check TX_EMPTY flag */
    CU_ASSERT(LPSPI_GET_TX_FIFO_EMPTY_FLAG(LPSPIModule) == 1);
    /* Check TX_FULL flag */
    CU_ASSERT(LPSPI_GET_TX_FIFO_FULL_FLAG(LPSPIModule) == 0);
    LPSPIModule->TX = 1;
    /* Check TX_EMPTY flag */
    CU_ASSERT(LPSPI_GET_TX_FIFO_EMPTY_FLAG(LPSPIModule) == 0);
    /* Check TX_FULL flag */
    CU_ASSERT(LPSPI_GET_TX_FIFO_FULL_FLAG(LPSPIModule) == 0);
    LPSPIModule->TX = 2;
    LPSPIModule->TX = 3;
    LPSPIModule->TX = 4;
    LPSPIModule->TX = 5;
    LPSPIModule->TX = 6;
    LPSPIModule->TX = 7;
    LPSPIModule->TX = 8;
    /* Check TX_FULL flag */
    CU_ASSERT(LPSPI_GET_TX_FIFO_FULL_FLAG(LPSPIModule) == 1);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void MACRO_LPSPI_READ_RX()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPIModule->CLKDIV = 4;
    /* Master mode, SPI mode 0. */
    LPSPIModule->CTL = 0x00000005;
    /* Write 3 data to TX FIFO */
    LPSPIModule->TX = 1;
    LPSPIModule->TX = 2;
    LPSPIModule->TX = 3;

    /* Wait data transfer */
    while ((LPSPIModule->STATUS & 0x00000001) != 0) __NOP();

    CU_ASSERT(LPSPI_GET_RX_FIFO_COUNT(LPSPIModule) == 3);
    LPSPI_READ_RX(LPSPIModule);
    CU_ASSERT(LPSPI_GET_RX_FIFO_COUNT(LPSPIModule) == 2);
    LPSPI_READ_RX(LPSPIModule);
    CU_ASSERT(LPSPI_GET_RX_FIFO_COUNT(LPSPIModule) == 1);
    LPSPI_READ_RX(LPSPIModule);
    CU_ASSERT(LPSPI_GET_RX_FIFO_COUNT(LPSPIModule) == 0);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void MACRO_LPSPI_WRITE_TX()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPIModule->CLKDIV = 0xFF;
    /* Slave mode, SPI mode 0. */
    LPSPIModule->CTL = 0x00040005;
    CU_ASSERT((LPSPIModule->STATUS & 0xF0000000) == 0);
    LPSPI_WRITE_TX(LPSPIModule, 1);
    CU_ASSERT((LPSPIModule->STATUS & 0xF0000000) == 0x10000000);
    LPSPI_WRITE_TX(LPSPIModule, 2);
    CU_ASSERT((LPSPIModule->STATUS & 0xF0000000) == 0x20000000);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void MACRO_LPSPI_SET_SS_HIGH()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPI_SET_SS_HIGH(LPSPIModule);
    CU_ASSERT(LPSPIModule->SSCTL == 5);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void MACRO_LPSPI_SET_SS_LOW()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPI_SET_SS_LOW(LPSPIModule);
    CU_ASSERT(LPSPIModule->SSCTL == 1);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void MACRO_LPSPI_ENABLE_DISABLE_BYTE_REORDER()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPI_ENABLE_BYTE_REORDER(LPSPIModule);
    CU_ASSERT_TRUE(LPSPIModule->CTL & 0x00080000);
    LPSPI_DISABLE_BYTE_REORDER(LPSPIModule);
    CU_ASSERT_FALSE(LPSPIModule->CTL & 0x00080000);
}

void MACRO_LPSPI_SET_SUSPEND_CYCLE()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPI_SET_SUSPEND_CYCLE(LPSPIModule, 0);
    CU_ASSERT((LPSPIModule->CTL & 0x000000F0) == 0);
    LPSPI_SET_SUSPEND_CYCLE(LPSPIModule, 8);
    CU_ASSERT((LPSPIModule->CTL & 0x000000F0) == 0x00000080);
    LPSPI_SET_SUSPEND_CYCLE(LPSPIModule, 15);
    CU_ASSERT((LPSPIModule->CTL & 0x000000F0) == 0x000000F0);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void MACRO_LPSPI_SET_LSB_MSB_FIRST()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPI_SET_LSB_FIRST(LPSPIModule);
    CU_ASSERT_TRUE(LPSPIModule->CTL & 0x00002000);
    LPSPI_SET_MSB_FIRST(LPSPIModule);
    CU_ASSERT_FALSE(LPSPIModule->CTL & 0x00002000);
}

void MACRO_LPSPI_SET_DATA_WIDTH()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPI_SET_DATA_WIDTH(LPSPIModule, 32);
    CU_ASSERT((LPSPIModule->CTL & 0x00001F00) == 0);
    LPSPI_SET_DATA_WIDTH(LPSPIModule, 8);
    CU_ASSERT((LPSPIModule->CTL & 0x00001F00) == 0x00000800);
    LPSPI_SET_DATA_WIDTH(LPSPIModule, 16);
    CU_ASSERT((LPSPIModule->CTL & 0x00001F00) == 0x00001000);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void MACRO_LPSPI_IS_BUSY()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPIModule->CLKDIV = 0xFF;
    LPSPIModule->CTL |= 1;
    LPSPIModule->TX = 1;
    CU_ASSERT_TRUE(LPSPI_IS_BUSY(LPSPIModule));

    /* Wait data transfer */
    while ((LPSPIModule->STATUS & LPSPI_STATUS_UNITIF_Msk) == 0)
    {
        __NOP();
    }

    CLK_SysTickDelay(100);

    CU_ASSERT_FALSE(LPSPI_IS_BUSY(LPSPIModule));

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void MACRO_LPSPI_ENABLE_DISABLE()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    CU_ASSERT_FALSE(LPSPIModule->CTL & 1);
    LPSPI_ENABLE(LPSPIModule);
    CU_ASSERT_TRUE(LPSPIModule->CTL & 1);
    LPSPI_DISABLE(LPSPIModule);
    CU_ASSERT_FALSE(LPSPIModule->CTL & 1);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void MACRO_LPSPI_AUTO_MODE()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPI_SET_TRIGSRC(LPSPIModule, LPSPI_TRIGGER_LPTMER0);
    CU_ASSERT(LPSPI_GET_TRIGSRC(LPSPIModule) == LPSPI_TRIGGER_LPTMER0);
    LPSPI_SET_TRIGSRC(LPSPIModule, LPSPI_TRIGGER_LPTMER1);
    CU_ASSERT(LPSPI_GET_TRIGSRC(LPSPIModule) == LPSPI_TRIGGER_LPTMER1);
    LPSPI_SET_TRIGSRC(LPSPIModule, LPSPI_TRIGGER_TTMR0);
    CU_ASSERT(LPSPI_GET_TRIGSRC(LPSPIModule) == LPSPI_TRIGGER_TTMR0);
    LPSPI_SET_TRIGSRC(LPSPIModule, LPSPI_TRIGGER_TTMR1);
    CU_ASSERT(LPSPI_GET_TRIGSRC(LPSPIModule) == LPSPI_TRIGGER_TTMR1);
    LPSPI_SET_TRIGSRC(LPSPIModule, LPSPI_TRIGGER_GPIO0);
    CU_ASSERT(LPSPI_GET_TRIGSRC(LPSPIModule) == LPSPI_TRIGGER_GPIO0);
    LPSPI_SET_TRIGSRC(LPSPIModule, LPSPI_TRIGGER_GPIO1);
    CU_ASSERT(LPSPI_GET_TRIGSRC(LPSPIModule) == LPSPI_TRIGGER_GPIO1);
    LPSPI_SET_TRIGSRC(LPSPIModule, LPSPI_TRIGGER_GPIO2);
    CU_ASSERT(LPSPI_GET_TRIGSRC(LPSPIModule) == LPSPI_TRIGGER_GPIO2);
    LPSPI_SET_TRIGSRC(LPSPIModule, LPSPI_TRIGGER_GPIO3);
    CU_ASSERT(LPSPI_GET_TRIGSRC(LPSPIModule) == LPSPI_TRIGGER_GPIO3);

    LPSPI_AUTO_TRIGEN(LPSPIModule, 1);
    CU_ASSERT_TRUE((LPSPIModule->AUTOCTL & LPSPI_AUTOCTL_TRIGEN_Msk) >> LPSPI_AUTOCTL_TRIGEN_Pos);

    LPSPI_AUTO_TRIGEN(LPSPIModule, 0);
    CU_ASSERT_FALSE((LPSPIModule->AUTOCTL & LPSPI_AUTOCTL_TRIGEN_Msk) >> LPSPI_AUTOCTL_TRIGEN_Pos);

    LPSPI_AUTO_TCNTINT(LPSPIModule, 1);
    CU_ASSERT_TRUE((LPSPIModule->AUTOCTL & LPSPI_AUTOCTL_CNTIEN_Msk) >> LPSPI_AUTOCTL_CNTIEN_Pos);

    LPSPI_AUTO_TCNTINT(LPSPIModule, 0);
    CU_ASSERT_FALSE((LPSPIModule->AUTOCTL & LPSPI_AUTOCTL_CNTIEN_Msk) >> LPSPI_AUTOCTL_CNTIEN_Pos);

    LPSPI_AUTO_FULLRX(LPSPIModule, 1);
    CU_ASSERT_TRUE((LPSPIModule->AUTOCTL & LPSPI_AUTOCTL_FULLRXEN_Msk) >> LPSPI_AUTOCTL_FULLRXEN_Pos);

    LPSPI_AUTO_FULLRX(LPSPIModule, 0);
    CU_ASSERT_FALSE((LPSPIModule->AUTOCTL & LPSPI_AUTOCTL_FULLRXEN_Msk) >> LPSPI_AUTOCTL_FULLRXEN_Pos);

    LPSPI_AUTO_SSWKEN(LPSPIModule, 1);
    CU_ASSERT_TRUE((LPSPIModule->AUTOCTL & LPSPI_AUTOCTL_SSWKEN_Msk) >> LPSPI_AUTOCTL_SSWKEN_Pos);

    LPSPI_AUTO_SSWKEN(LPSPIModule, 0);
    CU_ASSERT_FALSE((LPSPIModule->AUTOCTL & LPSPI_AUTOCTL_SSWKEN_Msk) >> LPSPI_AUTOCTL_SSWKEN_Pos);

    LPSPI_AUTO_EN(LPSPIModule, 1);
    CU_ASSERT_TRUE((LPSPIModule->AUTOCTL & LPSPI_AUTOCTL_AUTOEN_Msk) >> LPSPI_AUTOCTL_AUTOEN_Pos);

    LPSPI_AUTO_EN(LPSPIModule, 0);
    CU_ASSERT_FALSE((LPSPIModule->AUTOCTL & LPSPI_AUTOCTL_AUTOEN_Msk) >> LPSPI_AUTOCTL_AUTOEN_Pos);

    //LPSPI_AUTO_SWTRIG(LPSPIModule, 1);
    //CU_ASSERT_FALSE((LPSPIModule->AUTOCTL & LPSPI_AUTOCTL_SWTRG_Msk) >> LPSPI_AUTOCTL_SWTRG_Pos);

    LPSPI_AUTO_SWTRIG(LPSPIModule, 0);
    CU_ASSERT_FALSE((LPSPIModule->AUTOCTL & LPSPI_AUTOCTL_SWTRG_Msk) >> LPSPI_AUTOCTL_SWTRG_Pos);

    LPSPI_AUTO_TCNTWKEN(LPSPIModule, 1);
    CU_ASSERT_TRUE((LPSPIModule->AUTOCTL & LPSPI_AUTOCTL_CNTWKEN_Msk) >> LPSPI_AUTOCTL_CNTWKEN_Pos);

    LPSPI_AUTO_TCNTWKEN(LPSPIModule, 0);
    CU_ASSERT_FALSE((LPSPIModule->AUTOCTL & LPSPI_AUTOCTL_CNTWKEN_Msk) >> LPSPI_AUTOCTL_CNTWKEN_Pos);

    LPSPI_SET_AUTO_TCNT(LPSPIModule, 1);
    CU_ASSERT(LPSPI_GET_AUTO_TCNT(LPSPIModule) == 1);

    LPSPI_SET_AUTO_TCNT(LPSPIModule, 0x1C);
    CU_ASSERT(LPSPI_GET_AUTO_TCNT(LPSPIModule) == 0x1C);

    LPSPI_SET_AUTO_TCNT(LPSPIModule, 0xFE);
    CU_ASSERT(LPSPI_GET_AUTO_TCNT(LPSPIModule) == 0xFE);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void API_LPSPI_Open()
{
    uint32_t u32ReturnValue;
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    u32ReturnValue = LPSPI_Open(LPSPIModule, LPSPI_MASTER, LPSPI_MODE_0, 8, 1000000);
    CU_ASSERT(LPSPIModule->CTL == 0x00000805);
    //printf("1 LPSPIModule->CLKDIV =  %x\r\n", LPSPIModule->CLKDIV);
    CU_ASSERT(LPSPIModule->CLKDIV == 0x0000000B);
    CU_ASSERT(LPSPIModule->SSCTL == 0);
    CU_ASSERT(u32ReturnValue == 1000000);
    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());


    u32ReturnValue = LPSPI_Open(LPSPIModule, LPSPI_SLAVE, LPSPI_MODE_1, 16, 2000000);
    CU_ASSERT(LPSPIModule->CTL == 0x00041003);
    //printf("2 LPSPIModule->CLKDIV =  %x\r\n", LPSPIModule->CLKDIV);
    //printf("2 u32ReturnValue =  %d\r\n", u32ReturnValue);
    CU_ASSERT(LPSPIModule->CLKDIV == 0x00000000);
    CU_ASSERT(LPSPIModule->SSCTL == 0x0);
    CU_ASSERT(u32ReturnValue == 50000000);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());

    u32ReturnValue = LPSPI_Open(LPSPIModule, LPSPI_MASTER, LPSPI_MODE_2, 24, 5000000);
    CU_ASSERT(LPSPIModule->CTL == 0x0000180B);
    //printf("3 LPSPIModule->CLKDIV =  %x\r\n", LPSPIModule->CLKDIV);
    //printf("3 u32ReturnValue =  %d\r\n", u32ReturnValue);
    CU_ASSERT(LPSPIModule->CLKDIV == 0x00000001);
    CU_ASSERT(LPSPIModule->SSCTL == 0x0);
    CU_ASSERT(u32ReturnValue == 6000000);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());

    u32ReturnValue = LPSPI_Open(LPSPIModule, LPSPI_MASTER, LPSPI_MODE_3, 32, 32000000);
    CU_ASSERT(LPSPIModule->CTL == 0x0000000D);
    //printf("4 LPSPIModule->CLKDIV =  %x\r\n", LPSPIModule->CLKDIV);
    //printf("4 u32ReturnValue =  %d\r\n", u32ReturnValue);
    CU_ASSERT(LPSPIModule->CLKDIV == 0x00000000);
    CU_ASSERT(LPSPIModule->SSCTL == 0x0);
    CU_ASSERT(u32ReturnValue == 12000000);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void API_LPSPI_Close()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPIModule->CTL = 0x0000FFF4;
    LPSPI_Close(LPSPIModule);
    CU_ASSERT(LPSPIModule->CTL == 0x00000034);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void API_LPSPI_ClearRxFIFO()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPIModule->CLKDIV = 4;
    /* Master mode, SPI mode 0. */
    LPSPIModule->CTL = 0x00000005;
    /* Write 3 data to TX FIFO */
    LPSPIModule->TX = 1;
    LPSPIModule->TX = 2;
    LPSPIModule->TX = 3;

    /* Wait data transfer */
    while ((LPSPIModule->STATUS & 1) != 0) __NOP();

    CU_ASSERT(LPSPI_GET_RX_FIFO_COUNT(LPSPIModule) == 3);
    LPSPI_ClearRxFIFO(LPSPIModule);
    CU_ASSERT(LPSPIModule->STATUS == 0x00059112);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void API_LPSPI_ClearTxFIFO()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPIModule->CLKDIV = 0xFF;
    /* Slave mode, SPI mode 0. */
    LPSPIModule->CTL = 0x00040005;
    /* Write 3 data to TX FIFO */
    LPSPIModule->TX = 1;
    LPSPIModule->TX = 2;
    LPSPIModule->TX = 3;
    CU_ASSERT((LPSPIModule->STATUS & 0xF00F0000) == 0x30000000);   //CU_ASSERT((LPSPIModule->STATUS & 0xF0000000)== 0x30000000);
    LPSPI_ClearTxFIFO(LPSPIModule);
    CU_ASSERT((LPSPIModule->STATUS & 0xF00F0000) == 0x00050000); //CU_ASSERT((LPSPIModule->STATUS & 0xF0000000)== 0);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void API_LPSPI_EnableAutoSS_DisableAutoSS()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPI_EnableAutoSS(LPSPIModule, LPSPI_SS, LPSPI_SS_ACTIVE_HIGH);
    CU_ASSERT(LPSPIModule->SSCTL == 0xD);
    LPSPI_DisableAutoSS(LPSPIModule);
    CU_ASSERT((LPSPIModule->SSCTL & (LPSPI_SSCTL_AUTOSS_Msk | LPSPI_SSCTL_SS_Msk)) == 0x0);
    LPSPI_EnableAutoSS(LPSPIModule, LPSPI_SS, LPSPI_SS_ACTIVE_LOW);
    CU_ASSERT(LPSPIModule->SSCTL == 0x9);
    LPSPI_DisableAutoSS(LPSPIModule);
    CU_ASSERT((LPSPIModule->SSCTL & (LPSPI_SSCTL_AUTOSS_Msk | LPSPI_SSCTL_SS_Msk)) == 0x0);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void API_LPSPI_SetBusClock_GetBusClock()
{
    volatile uint32_t u32ReturnValue;
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    u32ReturnValue = LPSPI_SetBusClock(LPSPIModule, 200 * 1000);
    //printf("LPSPIModule->CLKDIV =  %x\r\n", LPSPIModule->CLKDIV);
    CU_ASSERT(LPSPIModule->CLKDIV == 0x000003B);
    CU_ASSERT(u32ReturnValue == 200000);
    u32ReturnValue = LPSPI_GetBusClock(LPSPIModule);
    CU_ASSERT(u32ReturnValue == 200000);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void API_LPSPI_SetFIFO()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPI_SetFIFO(LPSPIModule, 1, 2);
    CU_ASSERT((LPSPIModule->FIFOCTL & 0xFF000000) == 0x12000000);
    LPSPI_SetFIFO(LPSPIModule, 0, 3);
    CU_ASSERT((LPSPIModule->FIFOCTL & 0xFF000000) == 0x03000000);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void API_LPSPI_EnableInt_DisableInt()
{
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPI_EnableInt(LPSPIModule, LPSPI_UNIT_INT_MASK);
    CU_ASSERT_TRUE(LPSPIModule->CTL & 0x00020000);
    LPSPI_DisableInt(LPSPIModule, LPSPI_UNIT_INT_MASK);
    CU_ASSERT_FALSE(LPSPIModule->CTL & 0x00020000);

    LPSPI_EnableInt(LPSPIModule, LPSPI_SSACT_INT_MASK);
    CU_ASSERT_TRUE(LPSPIModule->SSCTL & 0x00001000);
    LPSPI_DisableInt(LPSPIModule, LPSPI_SSACT_INT_MASK);
    CU_ASSERT_FALSE(LPSPIModule->SSCTL & 0x00001000);

    LPSPI_EnableInt(LPSPIModule, LPSPI_SSINACT_INT_MASK);
    CU_ASSERT_TRUE(LPSPIModule->SSCTL & 0x00002000);
    LPSPI_DisableInt(LPSPIModule, LPSPI_SSINACT_INT_MASK);
    CU_ASSERT_FALSE(LPSPIModule->SSCTL & 0x00002000);

    LPSPI_EnableInt(LPSPIModule, LPSPI_SLVUR_INT_MASK);
    CU_ASSERT_TRUE(LPSPIModule->SSCTL & 0x00000200);
    LPSPI_DisableInt(LPSPIModule, LPSPI_SLVUR_INT_MASK);
    CU_ASSERT_FALSE(LPSPIModule->SSCTL & 0x00000200);

    LPSPI_EnableInt(LPSPIModule, LPSPI_SLVBE_INT_MASK);
    CU_ASSERT_TRUE(LPSPIModule->SSCTL & 0x00000100);
    LPSPI_DisableInt(LPSPIModule, LPSPI_SLVBE_INT_MASK);
    CU_ASSERT_FALSE(LPSPIModule->SSCTL & 0x00000100);

    LPSPI_EnableInt(LPSPIModule, LPSPI_TXUF_INT_MASK);
    CU_ASSERT_TRUE(LPSPIModule->FIFOCTL & 0x00000080);
    LPSPI_DisableInt(LPSPIModule, LPSPI_TXUF_INT_MASK);
    CU_ASSERT_FALSE(LPSPIModule->FIFOCTL & 0x00000080);

    LPSPI_EnableInt(LPSPIModule, LPSPI_FIFO_TXTH_INT_MASK);
    CU_ASSERT_TRUE(LPSPIModule->FIFOCTL & 0x00000008);
    LPSPI_DisableInt(LPSPIModule, LPSPI_FIFO_TXTH_INT_MASK);
    CU_ASSERT_FALSE(LPSPIModule->FIFOCTL & 0x00000008);

    LPSPI_EnableInt(LPSPIModule, LPSPI_FIFO_RXTH_INT_MASK);
    CU_ASSERT_TRUE(LPSPIModule->FIFOCTL & 0x00000004);
    LPSPI_DisableInt(LPSPIModule, LPSPI_FIFO_RXTH_INT_MASK);
    CU_ASSERT_FALSE(LPSPIModule->FIFOCTL & 0x00000004);

    LPSPI_EnableInt(LPSPIModule, LPSPI_FIFO_RXOV_INT_MASK);
    CU_ASSERT_TRUE(LPSPIModule->FIFOCTL & 0x00000020);
    LPSPI_DisableInt(LPSPIModule, LPSPI_FIFO_RXOV_INT_MASK);
    CU_ASSERT_FALSE(LPSPIModule->FIFOCTL & 0x00000020);

    LPSPI_EnableInt(LPSPIModule, LPSPI_FIFO_RXTO_INT_MASK);
    CU_ASSERT_TRUE(LPSPIModule->FIFOCTL & 0x00000010);
    LPSPI_DisableInt(LPSPIModule, LPSPI_FIFO_RXTO_INT_MASK);
    CU_ASSERT_FALSE(LPSPIModule->FIFOCTL & 0x00000010);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
}

void API_LPSPI_GetIntFlag_ClearIntFlag()
{
    uint32_t u32DelayCount;
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    /* Unit transfer interrupt flag test */
    LPSPIModule->CLKDIV = 4;
    /* Master mode, SPI mode 0. */
    LPSPIModule->CTL = 5;
    LPSPIModule->TX = 0;

    while ((LPSPIModule->STATUS & 0x0000002) == 0) __NOP();

    /* Check unit transfer interrupt flag */
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_UNIT_INT_MASK) == LPSPI_UNIT_INT_MASK);
    LPSPI_ClearIntFlag(LPSPIModule, LPSPI_UNIT_INT_MASK);
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_UNIT_INT_MASK) == LPSPI_UNIT_INT_MASK);
    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
    __NOP();

    /* Slave selection signal active/inactive/slave-under-run/bit-count-error/slave-underflow interrupt flag test */
    /* Set PA.3 (SPI0_SS) as output mode */
    SetSSPinToGPIO(GetLPSPIModuleIdx());

    /* Set PA.2 (SPI0_CLK) as GPIO output pin */
    SetCLKPinToGPIO(GetLPSPIModuleIdx());

    /* Enable self-test function */
    LPSPI_EnableSelfTest();

    /* Set PA.3 (SPI0_SS) to high level */
    SetSSPinToHigh(GetLPSPIModuleIdx());

    LPSPIModule->CLKDIV = 0;
    /* Slave selection signal is low-active. */
    LPSPIModule->SSCTL |= 0;
    /* Slave mode, SPI mode 0. */
    LPSPIModule->CTL = 0x00040005;

    for (u32DelayCount = 0; u32DelayCount < 10; u32DelayCount++) __NOP();

    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_SSACT_INT_MASK) == 0);
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_SSINACT_INT_MASK) == 0);
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_TXUF_INT_MASK) == 0);

    /* Set PA.3 (SPI0_SS) to low level */
    SetSSPinToLow(GetLPSPIModuleIdx());

    for (u32DelayCount = 0; u32DelayCount < 5000; u32DelayCount++) __NOP();

    /* Check slave selection signal active interrupt flag */
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_SSACT_INT_MASK) == LPSPI_SSACT_INT_MASK);
    LPSPI_ClearIntFlag(LPSPIModule, LPSPI_SSACT_INT_MASK);
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_SSACT_INT_MASK) == 0);
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_SSINACT_INT_MASK) == 0);
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_SLVUR_INT_MASK) == 0);
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_SLVBE_INT_MASK) == 0);

    /* Check slave TX underflow interrupt flag */
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_TXUF_INT_MASK) == LPSPI_TXUF_INT_MASK);

    /* Set PA.3 (SPI0_SS) to high level */
    SetSSPinToHigh(GetLPSPIModuleIdx());

    __NOP();
    /* Check slave selection signal inactive interrupt flag */
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_SSINACT_INT_MASK) == LPSPI_SSINACT_INT_MASK);
    LPSPI_ClearIntFlag(LPSPIModule, LPSPI_SSINACT_INT_MASK);
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_SSINACT_INT_MASK) == 0);

    /* Clear slave TX underflow interrupt flag */
    LPSPI_ClearIntFlag(LPSPIModule, LPSPI_TXUF_INT_MASK);
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_TXUF_INT_MASK) == 0);

    /* Check slave bit count error interrupt flag */
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_SLVBE_INT_MASK) == LPSPI_SLVBE_INT_MASK);
    LPSPI_ClearIntFlag(LPSPIModule, LPSPI_SLVBE_INT_MASK);
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_SLVBE_INT_MASK) == 0);

    /* Check slave TX under run interrupt flag */
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_SLVUR_INT_MASK) == LPSPI_SLVUR_INT_MASK);
    LPSPI_ClearIntFlag(LPSPIModule, LPSPI_SLVUR_INT_MASK);
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_SLVUR_INT_MASK) == 0);

    /* Disable self-test function */
    LPSPI_DisableSelfTest();

    /* Set PA.2 (SPI0_CLK) as SPI function pin */
    SetSPICLKMFP(GetLPSPIModuleIdx());

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
    __NOP();

    /* TX threshold interrupt flag test */
    LPSPIModule->CLKDIV = 0xFF;
    /* Slave mode, SPI mode 0. */
    LPSPIModule->CTL = 0x00040005;
    /* Set TX and RX FIFO threshold */
    LPSPI_SetFIFO(LPSPIModule, 2, 2);
    /* Check TX threshold interrupt flag */
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_FIFO_TXTH_INT_MASK) == LPSPI_FIFO_TXTH_INT_MASK);
    /* Write to TX FIFO */
    LPSPIModule->TX = 1;
    LPSPIModule->TX = 2;
    LPSPIModule->TX = 3;
    LPSPIModule->TX = 4;
    /* Check TX threshold interrupt flag */
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_FIFO_TXTH_INT_MASK) == 0);
    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
    __NOP();

    /* RX threshold interrupt flag test */
    LPSPIModule->CLKDIV = 0xF;
    /* Set TX and RX FIFO threshold. */
    LPSPI_SetFIFO(LPSPIModule, 2, 2);
    /* Master mode, SPI mode 0. */
    LPSPIModule->CTL = 5;

    /* Check RX threshold interrupt flag */
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_FIFO_RXTH_INT_MASK) == 0);

    /* Write to TX FIFO */
    LPSPIModule->TX = 1;
    LPSPIModule->TX = 2;
    LPSPIModule->TX = 3;

    /* Wait data transfer */
    while (LPSPI_IS_BUSY(LPSPIModule)) __NOP();

    /* Check RX threshold interrupt flag */
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_FIFO_RXTH_INT_MASK) == LPSPI_FIFO_RXTH_INT_MASK);

    /* RX overrun interrupt flag test */
    /* Check RX overrun interrupt flag */
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_FIFO_RXOV_INT_MASK) == 0);

    /* Write to TX FIFO */
    LPSPIModule->TX = 4;
    LPSPIModule->TX = 5;
    LPSPIModule->TX = 6;
    LPSPIModule->TX = 7;
    LPSPIModule->TX = 8;
    LPSPIModule->TX = 9;

    /* Wait data transfer */
    while (LPSPI_IS_BUSY(LPSPIModule)) __NOP();

    /* Check RX overrun interrupt flag */
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_FIFO_RXOV_INT_MASK) == LPSPI_FIFO_RXOV_INT_MASK);
    LPSPI_ClearIntFlag(LPSPIModule, LPSPI_FIFO_RXOV_INT_MASK);
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_FIFO_RXOV_INT_MASK) == 0);

    /* RX time-out interrupt flag test */
    /* Clear RX time-out interrupt flag */
    LPSPI_ClearIntFlag(LPSPIModule, LPSPI_FIFO_RXTO_INT_MASK);
    /* Check RX time-out interrupt flag */
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_FIFO_RXTO_INT_MASK) == 0);
    /* Write to TX FIFO */
    LPSPIModule->TX = 1;
    LPSPIModule->TX = 2;
    LPSPIModule->TX = 3;

    /* Wait data transfer */
    while (LPSPI_IS_BUSY(LPSPIModule)) __NOP();

    /* Check RX time-out interrupt flag */
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_FIFO_RXTO_INT_MASK) == LPSPI_FIFO_RXTO_INT_MASK);
    LPSPI_ClearIntFlag(LPSPIModule, LPSPI_FIFO_RXTO_INT_MASK);
    CU_ASSERT(LPSPI_GetIntFlag(LPSPIModule, LPSPI_FIFO_RXTO_INT_MASK) == 0);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
    __NOP();
}

void API_LPSPI_GetStatus()
{
    uint32_t u32DelayCount = 0;
    LPSPI_T *LPSPIModule = NULL;

    LPSPIModule = (LPSPI_T *)GetLPSPIModule(GetLPSPIModuleIdx());

    LPSPIModule->CLKDIV = 0;

    /* Check SPIEN flag */
    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_SPIEN_STS_MASK) == 0);

    /* Slave mode, SPI mode 0. */
    LPSPIModule->CTL = 0x00040005;

    /* Check busy flag */
    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_BUSY_MASK) == 0);

    /* Check SPIEN flag */
    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_SPIEN_STS_MASK) == LPSPI_SPIEN_STS_MASK);

    /* SPI TX empty flag test */
    /* Check TX empty flag */
    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_TX_EMPTY_MASK) == LPSPI_TX_EMPTY_MASK);

    /* Write to TX FIFO */
    LPSPIModule->TX = 1;
    LPSPIModule->TX = 2;

    /* Check TX empty flag */
    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_TX_EMPTY_MASK) == 0);

    /* Check busy flag */
    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_BUSY_MASK) == LPSPI_BUSY_MASK);

    /* SPI TX full flag test */
    /* Check TX full flag */
    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_TX_FULL_MASK) == 0);

    /* Write to TX FIFO */
    LPSPIModule->TX = 3;
    LPSPIModule->TX = 4;
    LPSPIModule->TX = 5;
    LPSPIModule->TX = 6;
    LPSPIModule->TX = 7;
    LPSPIModule->TX = 8;
    LPSPIModule->TX = 9;

    /* Check TX full flag */
    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_TX_FULL_MASK) == LPSPI_TX_FULL_MASK);

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
    __NOP();

    /* SPI RX empty flag test */
    LPSPIModule->CLKDIV = 0xFF;

    /* Master mode, SPI mode 0. */
    LPSPIModule->CTL = 5;

    /* Check TX/RX reset flag */
    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_TXRX_RESET_MASK) == 0);
    LPSPIModule->FIFOCTL |= LPSPI_FIFOCTL_RXRST_Msk;
    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_TXRX_RESET_MASK) == LPSPI_TXRX_RESET_MASK);

    //for(u32DelayCount=0; u32DelayCount<256; u32DelayCount++) __NOP();
    for (u32DelayCount = 0; u32DelayCount < 100000; u32DelayCount++) __NOP();

    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_TXRX_RESET_MASK) == 0); //test!!

    /* Check RX empty flag */
    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_RX_EMPTY_MASK) == LPSPI_RX_EMPTY_MASK);
    /* Write to TX FIFO */
    LPSPIModule->TX = 1;

    /* Wait data transfer */
    while (LPSPI_IS_BUSY(LPSPIModule)) __NOP();

    /* Check RX empty flag */
    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_RX_EMPTY_MASK) == 0);

    /* SPI RX full flag test */
    /* Check RX full flag */
    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_RX_FULL_MASK) == 0);
    /* Write to TX FIFO */
    LPSPIModule->TX = 2;
    LPSPIModule->TX = 3;
    LPSPIModule->TX = 4;
    LPSPIModule->TX = 5;
    LPSPIModule->TX = 6;
    LPSPIModule->TX = 7;
    LPSPIModule->TX = 8;

    /* Wait data transfer */
    while (LPSPI_IS_BUSY(LPSPIModule)) __NOP();

    /* Check RX full flag */
    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_RX_FULL_MASK) == LPSPI_RX_FULL_MASK);
    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
    __NOP();


    /* Check SPIx_SS line status */
    /* Set PA.3 (SPI0_SS) as GPIO output mode */
    SetSSPinToGPIO(GetLPSPIModuleIdx());

    /* Enable self-test function */
    LPSPI_EnableSelfTest();

    /* Set PA.3 (SPI0_SS) to high level */
    SetSSPinToHigh(GetLPSPIModuleIdx());

    LPSPIModule->CLKDIV = 0;
    /* Slave selection signal is low-active. */
    LPSPIModule->SSCTL = 0;
    /* Slave mode, SPI mode 0. */
    LPSPIModule->CTL = 0x00040005;

    for (u32DelayCount = 0; u32DelayCount < 10; u32DelayCount++) __NOP();

    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_SSLINE_STS_MASK) == LPSPI_SSLINE_STS_MASK);
    /* Set PA.3 (SPI0_SS) to low level */
    SetSSPinToLow(GetLPSPIModuleIdx());

    for (u32DelayCount = 0; u32DelayCount < 100; u32DelayCount++) __NOP();

    CU_ASSERT(LPSPI_GetStatus(LPSPIModule, LPSPI_SSLINE_STS_MASK) == 0);
    __NOP();

    /* Disable self-test function */
    LPSPI_DisableSelfTest();

    for (u32DelayCount = 0; u32DelayCount < 1000; u32DelayCount++) __NOP();

    /* Reset SPI */
    LPSPI_ModuleReset(GetLPSPIModuleIdx());
    __NOP();
}

CU_TestInfo LPSPI_MacroTests[] =
{
    {"Clear LPSPI unit transfer interrupt flag", MACRO_LPSPI_CLR_UNIT_TRANS_INT_FLAG},
    {"Trigger/Disable LPSPI RX DMA transfer", MACRO_LPSPI_TRIGGER_DISABLE_RX_PDMA},
    {"Trigger/Disable LPSPI TX DMA transfer", MACRO_LPSPI_TRIGGER_DISABLE_TX_PDMA},
    {"Get LPSPI RX FIFO count and empty flag", MACRO_LPSPI_GET_RX_FIFO_COUNT_EMPTY_FLAG},
    {"Get LPSPI TX FIFO empty and full flag", MACRO_LPSPI_GET_TX_FIFO_EMPTY_FULL_FLAG},
    {"Read LPSPI RX", MACRO_LPSPI_READ_RX},
    {"Write LPSPI TX", MACRO_LPSPI_WRITE_TX},
    {"Set LPSPI SS pin to high state", MACRO_LPSPI_SET_SS_HIGH},
    {"Set LPSPI SS pin to low state", MACRO_LPSPI_SET_SS_LOW},
    {"Enable/Disable LPSPI Byte Reorder function", MACRO_LPSPI_ENABLE_DISABLE_BYTE_REORDER},
    {"Set LPSPI suspend cycle", MACRO_LPSPI_SET_SUSPEND_CYCLE},
    {"Set LPSPI transfer sequence with LSB/MSB first", MACRO_LPSPI_SET_LSB_MSB_FIRST},
    {"Set LPSPI data width", MACRO_LPSPI_SET_DATA_WIDTH},
    {"Get LPSPI busy flag", MACRO_LPSPI_IS_BUSY},
    {"Enable/Disable LPSPI", MACRO_LPSPI_ENABLE_DISABLE},

    {"LPSPI Auto Mode", MACRO_LPSPI_AUTO_MODE},

    CU_TEST_INFO_NULL
};

CU_TestInfo LPSPI_ApiTests[] =
{
    {"LPSPI Open", API_LPSPI_Open},
    {"LPSPI Close", API_LPSPI_Close},
    {"Clear LPSPI RX FIFO", API_LPSPI_ClearRxFIFO},
    {"Clear LPSPI TX FIFO", API_LPSPI_ClearTxFIFO},
    {"Enable/Disable LPSPI automatic slave selection function", API_LPSPI_EnableAutoSS_DisableAutoSS},
    {"Set/Get LPSPI bus clock rate", API_LPSPI_SetBusClock_GetBusClock},
    {"Set LPSPI FIFO threshold", API_LPSPI_SetFIFO},
    {"Enable/Disable LPSPI interrupt function", API_LPSPI_EnableInt_DisableInt},
    {"Get/Clear LPSPI interrupt flag", API_LPSPI_GetIntFlag_ClearIntFlag},
    {"Get LPSPI status", API_LPSPI_GetStatus},

    CU_TEST_INFO_NULL
};

CU_SuiteInfo suites[] =
{
    {"LPSPI MACRO", LPSPI_Tests_Init, LPSPI_Tests_Clean, NULL, NULL, LPSPI_MacroTests},
    {"LPSPI API", LPSPI_Tests_Init, LPSPI_Tests_Clean, NULL, NULL, LPSPI_ApiTests},
    CU_SUITE_INFO_NULL
};

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
