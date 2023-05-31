/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.                                        */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   qspi_cunit.c                                                                                          */
/*            The test function of QSPI for CUnit.                                                         */
/* Project:   M2351                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"


/*----------------------------------------------------------------------------*/
/* Includes of local headers                                                  */
/*----------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"
#include "qspi_cunit.h"
#include "PinConfig.h"

//------------------------------------------------------------------------------
extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;

/*----------------------------------------------------------------------------*/
/* Test function                                                              */
/*----------------------------------------------------------------------------*/
#define QSPI_MODULE_NUM             (1)
#define SPI_I2S_FIRSTNO             (1)

#define C_QSPI0                     (0)
#define C_QSPI1                     (1)

#define INTERNAL_REG_BASE           (0x48)
#define SPI_INTERNAL_SELFTEST_Msk   (0x80)

//------------------------------------------------------------------------------
QSPI_T *g_apSPIModule[] =
{
    QSPI0,
    QSPI1,
};

typedef enum
{
    eSPI_CLK_HIRC = 0,
    eSPI_CLK_HXT,
    eSPI_CLK_PCLK,

    /* Palladium Not Test */
    eSPI_CLK_PLL1, //M254, M256, M258 not support
    eSPI_CLK_PLL0, //M254, M256, M258 not support
    eSPI_CLK_HIRC48M,
} E_SPI_CLK;

static uint32_t gu32QSPIModuleIdx = 0;

//------------------------------------------------------------------------------
void SetQSPIModuleIdx(uint32_t u32SetValue)
{
    gu32QSPIModuleIdx = u32SetValue;
}

uint32_t GetQSPIModuleIdx(void)
{
    return gu32QSPIModuleIdx;
}

void *GetQSPIModule(uint32_t u32ModuleIdx)
{
    return g_apSPIModule[u32ModuleIdx];
}

char *GetTestSPIName(uint32_t u32Index)
{
    switch (u32Index)
    {
        case C_QSPI0:
            return "QSPI0";

        case C_QSPI1:
            return "QSPI1";

        default:
            break;
    }

    return NULL;
}

void ResetQSPI(uint32_t u32QSPIModule)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    switch (u32QSPIModule)
    {
        case C_QSPI0:
            SYS->QSPIRST |= (1 << (SYS_QSPIRST_QSPI0RST_Pos));
            SYS->QSPIRST &= ~(1 << (SYS_QSPIRST_QSPI0RST_Pos));
            break;

        case C_QSPI1:
            SYS->QSPIRST |= (1 << (SYS_QSPIRST_QSPI1RST_Pos));
            SYS->QSPIRST &= ~(1 << (SYS_QSPIRST_QSPI1RST_Pos));
            break;
    }

    while ((SYS->QSPIRST) != 0);

    /* Lock protected registers */
    SYS_LockReg();
}

void QSPI1_SetClkSrc(uint32_t u32ClkSrc)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    CLK->QSPISEL &= (~CLK_QSPISEL_QSPI1SEL_Msk);

    switch (u32ClkSrc)
    {
        case eSPI_CLK_HXT:
            CLK->QSPISEL |= CLK_QSPISEL_QSPI1SEL_HXT;
            break;

        case eSPI_CLK_PLL1:
        case eSPI_CLK_PLL0:
            CLK->QSPISEL |= CLK_QSPISEL_QSPI1SEL_APLL0_DIV2;
            break;

        case eSPI_CLK_PCLK:
            CLK->QSPISEL |= CLK_QSPISEL_QSPI1SEL_PCLK2;
            break;

        case eSPI_CLK_HIRC:
            CLK->QSPISEL |= CLK_QSPISEL_QSPI1SEL_HIRC;
            break;

        case eSPI_CLK_HIRC48M:
            CLK->QSPISEL |= CLK_QSPISEL_QSPI1SEL_HIRC48M_DIV4;
            break;
    }

    QSPI1_MOSI_PIN_INIT;
    QSPI1_MISO_PIN_INIT;
    QSPI1_CLK_PIN_INIT;
    QSPI1_CSS_PIN_INIT;
    QSPI1_MOSI1_PIN_INIT;
    QSPI1_MISO1_PIN_INIT;
}

void QSPI0_SetClkSrc(uint32_t u32ClkSrc)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    CLK->QSPISEL &= (~CLK_QSPISEL_QSPI0SEL_Msk);

    switch (u32ClkSrc)
    {
        case eSPI_CLK_HXT:
            CLK->QSPISEL |= CLK_QSPISEL_QSPI0SEL_HXT;
            break;

        case eSPI_CLK_PLL1:
        case eSPI_CLK_PLL0:
            CLK->QSPISEL |= CLK_QSPISEL_QSPI0SEL_APLL0_DIV2;
            break;

        case eSPI_CLK_PCLK:
            CLK->QSPISEL |= CLK_QSPISEL_QSPI0SEL_PCLK0;
            break;

        case eSPI_CLK_HIRC:
            CLK->QSPISEL |= CLK_QSPISEL_QSPI0SEL_HIRC;
            break;

        case eSPI_CLK_HIRC48M:
            CLK->QSPISEL |= CLK_QSPISEL_QSPI0SEL_HIRC48M_DIV4;
            break;
    }

    QSPI0_MOSI_PIN_INIT;
    QSPI0_MISO_PIN_INIT;
    QSPI0_CLK_PIN_INIT;
    QSPI0_CSS_PIN_INIT;
    QSPI0_MOSI1_PIN_INIT;
    QSPI0_MISO1_PIN_INIT;
}

void QSPI_ClkDisable(uint32_t u32SPIModule)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    switch (u32SPIModule)
    {
        case C_QSPI0:
            CLK->QSPICTL &= ~(CLK_QSPICTL_QSPI0CKEN_Msk);
            break;

        case C_QSPI1:
            CLK->QSPICTL &= ~(CLK_QSPICTL_QSPI1CKEN_Msk);
            break;
    }
}

void QSPI_ClkEnable(uint32_t u32SPIModule)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    switch (u32SPIModule)
    {
        case C_QSPI0:
            CLK->QSPICTL |= CLK_QSPICTL_QSPI0CKEN_Msk;
            break;

        case C_QSPI1:
            CLK->QSPICTL |= CLK_QSPICTL_QSPI1CKEN_Msk;
            break;
    }
}

void QSPI_CLK_Sel(uint32_t u32Module, uint32_t u32ClkSrc)
{
    QSPI_ClkEnable(u32Module);

    switch (u32Module)
    {
        case C_QSPI0:
            QSPI0_SetClkSrc(u32ClkSrc);
            break;

        case C_QSPI1:
            QSPI1_SetClkSrc(u32ClkSrc);
            break;

        default:
            printf("\r\n");
            printf("===================================================\r\n");
            printf("%s initiation failed, please check your configuration\r\n",
                   GetTestSPIName(u32Module));
            printf("===================================================\r\n");
            printf("\r\n");

            while (1);

            break;
    }
}

int QSPI_Tests_Init(void)
{
    QSPI_CLK_Sel(GetQSPIModuleIdx(), eSPI_CLK_PCLK);

    return 0;
}

int QSPI_Tests_Clean(void)
{
    QSPI_ClkDisable(GetQSPIModuleIdx());

    return 0;
}

void QSPI_DisableSelfTest(uint32_t u32QSPIModuleIdx)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    if (u32QSPIModuleIdx == C_QSPI0)
    {
        outp32(QSPI0_BASE + INTERNAL_REG_BASE, ~SPI_INTERNAL_SELFTEST_Msk);
    }
    else if (u32QSPIModuleIdx == C_QSPI1)
    {
        outp32(QSPI1_BASE + INTERNAL_REG_BASE, ~SPI_INTERNAL_SELFTEST_Msk);
    }

    SYS->ALTCTL0 &= ~SYS_ALTCTL0_SELFTEST_Msk;

    /* Lock protected registers */
    SYS_LockReg();
}

void QSPI_EnableSelfTest(uint32_t u32QSPIModuleIdx)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    if (u32QSPIModuleIdx == C_QSPI0)
    {
        outp32(QSPI0_BASE + INTERNAL_REG_BASE, SPI_INTERNAL_SELFTEST_Msk);
    }
    else if (u32QSPIModuleIdx == C_QSPI1)
    {
        outp32(QSPI1_BASE + INTERNAL_REG_BASE, SPI_INTERNAL_SELFTEST_Msk);
    }

    SYS->ALTCTL0 |= SYS_ALTCTL0_SELFTEST_Msk;

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
void MACRO_QSPI_CLR_UNIT_TRANS_INT_FLAG()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPIModule->CLKDIV = 4;
    QSPIModule->CTL = QSPI_CTL_TXNEG_Msk | QSPI_CTL_QSPIEN_Msk; // 5
    QSPIModule->TX = 1;

    while ((QSPIModule->STATUS & QSPI_STATUS_UNITIF_Msk) == 0)
    {
        __NOP();
    }

    SPI_CLR_UNIT_TRANS_INT_FLAG(QSPIModule);
    CU_ASSERT((QSPIModule->STATUS & QSPI_STATUS_UNITIF_Msk) == 0);

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_ENABLE_DISABLE_2BIT_MODE()
{
    /* Check TWOB bit */
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    /* Check TWOB bit */
    CU_ASSERT((QSPIModule->CTL & QSPI_CTL_TWOBIT_Msk) == 0);
    QSPI_ENABLE_2BIT_MODE(QSPIModule);
    /* Check TWOB bit */
    CU_ASSERT((QSPIModule->CTL & QSPI_CTL_TWOBIT_Msk) == QSPI_CTL_TWOBIT_Msk);
    QSPI_DISABLE_2BIT_MODE(QSPIModule);
    /* Check TWOB bit */
    CU_ASSERT((QSPIModule->CTL & QSPI_CTL_TWOBIT_Msk) == 0);

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_ENABLE_DISABLE_3WIRE_MODE()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    /* Check SLV3WIRE bit */
    CU_ASSERT((QSPIModule->SSCTL & QSPI_SSCTL_SLV3WIRE_Msk) == 0);
    QSPI_ENABLE_3WIRE_MODE(QSPIModule);
    /* Check SLV3WIRE bit */
    CU_ASSERT((QSPIModule->SSCTL & QSPI_SSCTL_SLV3WIRE_Msk) == QSPI_SSCTL_SLV3WIRE_Msk);
    QSPI_DISABLE_3WIRE_MODE(QSPIModule);
    /* Check SLV3WIRE bit */
    CU_ASSERT((QSPIModule->SSCTL & QSPI_SSCTL_SLV3WIRE_Msk) == 0);

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_DISABLE_DUAL_MODE()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    /* Set DUALIOEN bit */
    QSPIModule->CTL |= QSPI_CTL_DUALIOEN_Msk;
    /* Check DUALIOEN bit */
    CU_ASSERT((QSPIModule->CTL & QSPI_CTL_DUALIOEN_Msk) == QSPI_CTL_DUALIOEN_Msk);
    QSPI_DISABLE_DUAL_MODE(QSPIModule);
    /* Check DUALIOEN bit */
    CU_ASSERT((QSPIModule->CTL & QSPI_CTL_DUALIOEN_Msk) == 0);
    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_DISABLE_QUAD_MODE()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    /* Set QUADIOEN bit */
    QSPIModule->CTL |= QSPI_CTL_QUADIOEN_Msk;
    /* Check QUADIOEN bit */
    CU_ASSERT((QSPIModule->CTL & QSPI_CTL_QUADIOEN_Msk) == QSPI_CTL_QUADIOEN_Msk);
    QSPI_DISABLE_QUAD_MODE(QSPIModule);
    /* Check QUADIOEN bit */
    CU_ASSERT((QSPIModule->CTL & QSPI_CTL_QUADIOEN_Msk) == 0);
    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_ENABLE_DUAL_INPUT_MODE()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    /* Set DUALIOEN bit */
    QSPI_ENABLE_DUAL_INPUT_MODE(QSPIModule);
    /* Check DUALIOEN bit */
    CU_ASSERT((QSPIModule->CTL & (QSPI_CTL_DUALIOEN_Msk | QSPI_CTL_DATDIR_Msk)) == QSPI_CTL_DUALIOEN_Msk);

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_ENABLE_DUAL_OUTPUT_MODE()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    /* Set DUALIOEN bit */
    QSPI_ENABLE_DUAL_OUTPUT_MODE(QSPIModule);
    /* Check DUALIOEN bit */
    CU_ASSERT((QSPIModule->CTL & (QSPI_CTL_DUALIOEN_Msk | QSPI_CTL_DATDIR_Msk)) == (QSPI_CTL_DUALIOEN_Msk | QSPI_CTL_DATDIR_Msk));

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_ENABLE_QUAD_INPUT_MODE()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    /* Set DUALIOEN bit */
    QSPI_ENABLE_QUAD_INPUT_MODE(QSPIModule);
    /* Check DUALIOEN bit */
    CU_ASSERT((QSPIModule->CTL & (QSPI_CTL_QUADIOEN_Msk | QSPI_CTL_DATDIR_Msk)) == QSPI_CTL_QUADIOEN_Msk);

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_ENABLE_QUAD_OUTPUT_MODE()
{
    /* In M2351, Quad I/O mode is only supported in QSPI0. */
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    /* Set DUALIOEN bit */
    QSPI_ENABLE_QUAD_OUTPUT_MODE(QSPIModule);
    /* Check DUALIOEN bit */
    CU_ASSERT((QSPIModule->CTL & (QSPI_CTL_QUADIOEN_Msk | QSPI_CTL_DATDIR_Msk)) == (QSPI_CTL_QUADIOEN_Msk | QSPI_CTL_DATDIR_Msk));

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_TRIGGER_DISABLE_RX_PDMA()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    /* Check RXPDMAEN bit */
    CU_ASSERT((QSPIModule->PDMACTL & QSPI_PDMACTL_RXPDMAEN_Msk) == 0);
    QSPI_TRIGGER_RX_PDMA(QSPIModule);
    /* Check RXPDMAEN bit */
    CU_ASSERT((QSPIModule->PDMACTL & QSPI_PDMACTL_RXPDMAEN_Msk) == QSPI_PDMACTL_RXPDMAEN_Msk);
    QSPI_DISABLE_RX_PDMA(QSPIModule);
    CU_ASSERT((QSPIModule->PDMACTL & QSPI_PDMACTL_RXPDMAEN_Msk) == 0);

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_TRIGGER_DISABLE_TX_PDMA()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    /* Check TXPDMAEN bit */
    CU_ASSERT((QSPIModule->PDMACTL & QSPI_PDMACTL_TXPDMAEN_Msk) == 0);
    QSPI_TRIGGER_TX_PDMA(QSPIModule);
    /* Check TXPDMAEN bit */
    CU_ASSERT((QSPIModule->PDMACTL & QSPI_PDMACTL_TXPDMAEN_Msk) == QSPI_PDMACTL_TXPDMAEN_Msk);
    QSPI_DISABLE_TX_PDMA(QSPIModule);
    CU_ASSERT((QSPIModule->PDMACTL & QSPI_PDMACTL_TXPDMAEN_Msk) == 0);

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_GET_RX_FIFO_COUNT_EMPTY_FLAG()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPIModule->CLKDIV = 0xFF;
    /* Master mode, SPI mode 0. */
    QSPIModule->CTL = QSPI_CTL_TXNEG_Msk | QSPI_CTL_QSPIEN_Msk;
    /* Check RX_EMPTY flag */
    CU_ASSERT(QSPI_GET_RX_FIFO_EMPTY_FLAG(QSPIModule) == 1);
    QSPIModule->TX = 1;

    /* Check unit transfer interrupt flag */
    while ((QSPIModule->STATUS & QSPI_STATUS_UNITIF_Msk) == 0) __NOP();

    QSPI_CLR_UNIT_TRANS_INT_FLAG(QSPIModule);
    CU_ASSERT(QSPI_GET_RX_FIFO_COUNT(QSPIModule) == 1);
    /* Check RX_EMPTY flag */
    CU_ASSERT(QSPI_GET_RX_FIFO_EMPTY_FLAG(QSPIModule) == 0);
    QSPIModule->TX = 2;

    while ((QSPIModule->STATUS & QSPI_STATUS_UNITIF_Msk) == 0) __NOP();

    QSPI_CLR_UNIT_TRANS_INT_FLAG(QSPIModule);
    CU_ASSERT(QSPI_GET_RX_FIFO_COUNT(QSPIModule) == 2);
    QSPIModule->TX = 3;

    while ((QSPIModule->STATUS & QSPI_STATUS_UNITIF_Msk) == 0) __NOP();

    QSPI_CLR_UNIT_TRANS_INT_FLAG(QSPIModule);
    CU_ASSERT(QSPI_GET_RX_FIFO_COUNT(QSPIModule) == 3);
    QSPIModule->TX = 4;

    while ((QSPIModule->STATUS & QSPI_STATUS_UNITIF_Msk) == 0) __NOP();

    QSPI_CLR_UNIT_TRANS_INT_FLAG(QSPIModule);
    CU_ASSERT(QSPI_GET_RX_FIFO_COUNT(QSPIModule) == 4);
    QSPIModule->TX = 5;

    while ((QSPIModule->STATUS & QSPI_STATUS_UNITIF_Msk) == 0) __NOP();

    QSPI_CLR_UNIT_TRANS_INT_FLAG(QSPIModule);
    CU_ASSERT(QSPI_GET_RX_FIFO_COUNT(QSPIModule) == 5);
    QSPIModule->TX = 6;

    while ((QSPIModule->STATUS & QSPI_STATUS_UNITIF_Msk) == 0) __NOP();

    QSPI_CLR_UNIT_TRANS_INT_FLAG(QSPIModule);
    CU_ASSERT(QSPI_GET_RX_FIFO_COUNT(QSPIModule) == 6);
    QSPIModule->TX = 7;

    while ((QSPIModule->STATUS & QSPI_STATUS_UNITIF_Msk) == 0) __NOP();

    QSPI_CLR_UNIT_TRANS_INT_FLAG(QSPIModule);
    CU_ASSERT(QSPI_GET_RX_FIFO_COUNT(QSPIModule) == 7);
    QSPIModule->TX = 8;

    while ((QSPIModule->STATUS & QSPI_STATUS_UNITIF_Msk) == 0) __NOP();

    QSPI_CLR_UNIT_TRANS_INT_FLAG(QSPIModule);
    CU_ASSERT(QSPI_GET_RX_FIFO_COUNT(QSPIModule) == 8);

    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_GET_TX_FIFO_EMPTY_FULL_FLAG()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPIModule->CLKDIV = 0xFF;
    /* Slave mode, SPI mode 0. */
    QSPIModule->CTL = QSPI_CTL_SLAVE_Msk | QSPI_CTL_TXNEG_Msk | QSPI_CTL_QSPIEN_Msk;
    /* Check TX_EMPTY flag */
    CU_ASSERT(QSPI_GET_TX_FIFO_EMPTY_FLAG(QSPIModule) == 1);
    /* Check TX_FULL flag */
    CU_ASSERT(QSPI_GET_TX_FIFO_FULL_FLAG(QSPIModule) == 0);
    QSPIModule->TX = 1;
    /* Check TX_EMPTY flag */
    CU_ASSERT(QSPI_GET_TX_FIFO_EMPTY_FLAG(QSPIModule) == 0);
    /* Check TX_FULL flag */
    CU_ASSERT(QSPI_GET_TX_FIFO_FULL_FLAG(QSPIModule) == 0);
    QSPIModule->TX = 2;
    QSPIModule->TX = 3;
    QSPIModule->TX = 4;
    QSPIModule->TX = 5;
    QSPIModule->TX = 6;
    QSPIModule->TX = 7;
    QSPIModule->TX = 8;
    /* Check TX_FULL flag */
    CU_ASSERT(QSPI_GET_TX_FIFO_FULL_FLAG(QSPIModule) == 1);

    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_READ_RX()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPIModule->CLKDIV = 4;
    /* Master mode, SPI mode 0. */
    QSPIModule->CTL = QSPI_CTL_TXNEG_Msk | QSPI_CTL_QSPIEN_Msk;
    /* Write 3 data to TX FIFO */
    QSPIModule->TX = 1;
    QSPIModule->TX = 2;
    QSPIModule->TX = 3;

    /* Wait data transfer */
    while ((QSPIModule->STATUS & QSPI_STATUS_BUSY_Msk) != 0) __NOP();

    CU_ASSERT(QSPI_GET_RX_FIFO_COUNT(QSPIModule) == 3);
    QSPI_READ_RX(QSPIModule);
    CU_ASSERT(QSPI_GET_RX_FIFO_COUNT(QSPIModule) == 2);
    QSPI_READ_RX(QSPIModule);
    CU_ASSERT(QSPI_GET_RX_FIFO_COUNT(QSPIModule) == 1);
    QSPI_READ_RX(QSPIModule);
    CU_ASSERT(QSPI_GET_RX_FIFO_COUNT(QSPIModule) == 0);

    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_WRITE_TX()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPIModule->CLKDIV = 0xFF;
    /* Slave mode, SPI mode 0. */
    QSPIModule->CTL = QSPI_CTL_SLAVE_Msk | QSPI_CTL_TXNEG_Msk | QSPI_CTL_QSPIEN_Msk;
    CU_ASSERT((QSPIModule->STATUS & QSPI_STATUS_TXCNT_Msk) == 0);
    QSPI_WRITE_TX(QSPIModule, 1);
    CU_ASSERT((QSPIModule->STATUS & QSPI_STATUS_TXCNT_Msk) == (0x1 << QSPI_STATUS_TXCNT_Pos));
    QSPI_WRITE_TX(QSPIModule, 2);
    CU_ASSERT((QSPIModule->STATUS & QSPI_STATUS_TXCNT_Msk) == (0x2 << QSPI_STATUS_TXCNT_Pos));

    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_SET_SS_HIGH()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPI_SET_SS_HIGH(QSPIModule);
    CU_ASSERT(QSPIModule->SSCTL == (QSPI_SSCTL_SS_Msk | QSPI_SSCTL_SSACTPOL_Msk));

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_SET_SS_LOW()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPI_SET_SS_LOW(QSPIModule);
    CU_ASSERT(QSPIModule->SSCTL == QSPI_SSCTL_SS_Msk);

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_ENABLE_DISABLE_BYTE_REORDER()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPI_ENABLE_BYTE_REORDER(QSPIModule);
    CU_ASSERT_TRUE(QSPIModule->CTL & QSPI_CTL_REORDER_Msk);
    QSPI_DISABLE_BYTE_REORDER(QSPIModule);
    CU_ASSERT_FALSE(QSPIModule->CTL & QSPI_CTL_REORDER_Msk);

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_SET_SUSPEND_CYCLE()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPI_SET_SUSPEND_CYCLE(QSPIModule, 0);
    CU_ASSERT((QSPIModule->CTL & QSPI_CTL_SUSPITV_Msk) == 0);
    QSPI_SET_SUSPEND_CYCLE(QSPIModule, 8);
    CU_ASSERT((QSPIModule->CTL & QSPI_CTL_SUSPITV_Msk) == (0x8 << QSPI_CTL_SUSPITV_Pos));
    QSPI_SET_SUSPEND_CYCLE(QSPIModule, 15);
    CU_ASSERT((QSPIModule->CTL & QSPI_CTL_SUSPITV_Msk) == (0xF << QSPI_CTL_SUSPITV_Pos));

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_SET_LSB_MSB_FIRST()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPI_SET_LSB_FIRST(QSPIModule);
    CU_ASSERT_TRUE(QSPIModule->CTL & QSPI_CTL_LSB_Msk);
    QSPI_SET_MSB_FIRST(QSPIModule);
    CU_ASSERT_FALSE(QSPIModule->CTL & QSPI_CTL_LSB_Msk);

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_SET_DATA_WIDTH()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPI_SET_DATA_WIDTH(QSPIModule, 32);
    CU_ASSERT((QSPIModule->CTL & QSPI_CTL_DWIDTH_Msk) == 0);
    QSPI_SET_DATA_WIDTH(QSPIModule, 8);
    CU_ASSERT((QSPIModule->CTL & QSPI_CTL_DWIDTH_Msk) == (0x8 << QSPI_CTL_DWIDTH_Pos));
    QSPI_SET_DATA_WIDTH(QSPIModule, 16);
    CU_ASSERT((QSPIModule->CTL & QSPI_CTL_DWIDTH_Msk) == (0x10 << QSPI_CTL_DWIDTH_Pos));

    /* Reset SPI */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_IS_BUSY()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPIModule->CLKDIV = 0xFF;
    QSPIModule->CTL |= QSPI_CTL_QSPIEN_Msk;
    QSPIModule->TX = 1;
    CU_ASSERT_TRUE(QSPI_IS_BUSY(QSPIModule));

    /* Wait data transfer */
    //while ((QSPIModule->STATUS & QSPI_STATUS_UNITIF_Msk) == 0)
    //{
    //    __NOP();
    //}

    //CU_ASSERT_FALSE(QSPI_IS_BUSY(QSPIModule));

    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());
}

void MACRO_QSPI_ENABLE_DISABLE()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    CU_ASSERT_FALSE(QSPIModule->CTL & QSPI_CTL_QSPIEN_Msk);
    QSPI_ENABLE(QSPIModule);
    CU_ASSERT_TRUE(QSPIModule->CTL & QSPI_CTL_QSPIEN_Msk);
    QSPI_DISABLE(QSPIModule);
    CU_ASSERT_FALSE(QSPIModule->CTL & QSPI_CTL_QSPIEN_Msk);

    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());
}

//QSPI API
void API_QSPI_Open()
{
    uint32_t u32ReturnValue;
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    u32ReturnValue = QSPI_Open(QSPIModule, QSPI_MASTER, QSPI_MODE_0, 8, 1000000);
    CU_ASSERT(QSPIModule->CTL == ((0x8 << QSPI_CTL_DWIDTH_Pos) |
                                  QSPI_CTL_TXNEG_Msk |
                                  QSPI_CTL_QSPIEN_Msk)); //0x00000805
    //printf("QSPIModule->CLKDIV = %x\r\n", QSPIModule->CLKDIV);
    CU_ASSERT(QSPIModule->CLKDIV == 0x0000002F);
    CU_ASSERT(QSPIModule->SSCTL == 0);
    CU_ASSERT(u32ReturnValue == 1000000);
    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());

    u32ReturnValue = QSPI_Open(QSPIModule, QSPI_SLAVE, QSPI_MODE_1, 16, 2000000);
    CU_ASSERT(QSPIModule->CTL == (QSPI_CTL_SLAVE_Msk | (0x10 << QSPI_CTL_DWIDTH_Pos) | QSPI_CTL_RXNEG_Msk | QSPI_CTL_QSPIEN_Msk)); // 0x00041003
    CU_ASSERT(QSPIModule->CLKDIV == 0x00000000);
    CU_ASSERT(QSPIModule->SSCTL == 0x0);
    CU_ASSERT(u32ReturnValue == 48000000);
    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());

    u32ReturnValue = QSPI_Open(QSPIModule, QSPI_SLAVE, QSPI_MODE_1, 16, 2000000);
    CU_ASSERT(QSPIModule->CTL == (QSPI_CTL_SLAVE_Msk | (0x10 << QSPI_CTL_DWIDTH_Pos) | QSPI_CTL_RXNEG_Msk | QSPI_CTL_QSPIEN_Msk)); // 0x00041003
    CU_ASSERT(QSPIModule->CLKDIV == 0x00000000);
    CU_ASSERT(QSPIModule->SSCTL == 0x0);
    CU_ASSERT(u32ReturnValue == 48000000);
    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());

    u32ReturnValue = QSPI_Open(QSPIModule, QSPI_SLAVE, QSPI_MODE_1, 16, 2000000);
    CU_ASSERT(QSPIModule->CTL == (QSPI_CTL_SLAVE_Msk | (0x10 << QSPI_CTL_DWIDTH_Pos) | QSPI_CTL_RXNEG_Msk | QSPI_CTL_QSPIEN_Msk)); // 0x00041003
    CU_ASSERT(QSPIModule->CLKDIV == 0x00000000);
    CU_ASSERT(QSPIModule->SSCTL == 0x0);
    CU_ASSERT(u32ReturnValue == 48000000);
    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());

    u32ReturnValue = QSPI_Open(QSPIModule, QSPI_SLAVE, QSPI_MODE_1, 16, 2000000);
    CU_ASSERT(QSPIModule->CTL == (QSPI_CTL_SLAVE_Msk | (0x10 << QSPI_CTL_DWIDTH_Pos) | QSPI_CTL_RXNEG_Msk | QSPI_CTL_QSPIEN_Msk)); // 0x00041003
    CU_ASSERT(QSPIModule->CLKDIV == 0x00000000);
    CU_ASSERT(QSPIModule->SSCTL == 0x0);
    CU_ASSERT(u32ReturnValue == 48000000);
    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());


    u32ReturnValue = QSPI_Open(QSPIModule, QSPI_SLAVE, QSPI_MODE_2, 24, 5000000);
    CU_ASSERT(QSPIModule->CTL == (QSPI_CTL_SLAVE_Msk | (0x18 << QSPI_CTL_DWIDTH_Pos) | QSPI_CTL_CLKPOL_Msk | QSPI_CTL_RXNEG_Msk | QSPI_CTL_QSPIEN_Msk)); // 0x0004180B
    CU_ASSERT(QSPIModule->CLKDIV == 0x00000000);
    CU_ASSERT(QSPIModule->SSCTL == 0x0);
    CU_ASSERT(u32ReturnValue == 48000000);
    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());

    u32ReturnValue = QSPI_Open(QSPIModule, QSPI_MASTER, QSPI_MODE_3, 32, 36000000);
    CU_ASSERT(QSPIModule->CTL == (QSPI_CTL_CLKPOL_Msk | QSPI_CTL_TXNEG_Msk | QSPI_CTL_QSPIEN_Msk));   //0x0000000D
    CU_ASSERT(QSPIModule->CLKDIV == 0x00000000);
    CU_ASSERT(QSPIModule->SSCTL == 0x0);
    //printf("1 u32ReturnValue = %d\r\n", u32ReturnValue);
    CU_ASSERT(u32ReturnValue == 48000000);
    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());

    u32ReturnValue = QSPI_Open(QSPIModule, QSPI_MASTER, QSPI_MODE_3, 32, 36000000);
    CU_ASSERT(QSPIModule->CTL == (QSPI_CTL_CLKPOL_Msk | QSPI_CTL_TXNEG_Msk | QSPI_CTL_QSPIEN_Msk));   //0x0000000D
    CU_ASSERT(QSPIModule->CLKDIV == 0x00000000);
    CU_ASSERT(QSPIModule->SSCTL == 0x0);
    //printf("2 u32ReturnValue = %d\r\n", u32ReturnValue);
    CU_ASSERT(u32ReturnValue == 48000000);
    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());

    u32ReturnValue = QSPI_Open(QSPIModule, QSPI_MASTER, QSPI_MODE_3, 32, 36000000);
    CU_ASSERT(QSPIModule->CTL == (QSPI_CTL_CLKPOL_Msk | QSPI_CTL_TXNEG_Msk | QSPI_CTL_QSPIEN_Msk));   //0x0000000D
    CU_ASSERT(QSPIModule->CLKDIV == 0x00000000);
    CU_ASSERT(QSPIModule->SSCTL == 0x0);
    //printf("3 u32ReturnValue = %d\r\n", u32ReturnValue);
    CU_ASSERT(u32ReturnValue == 48000000);
    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());

    u32ReturnValue = QSPI_Open(QSPIModule, QSPI_MASTER, QSPI_MODE_3, 32, 36000000);
    CU_ASSERT(QSPIModule->CTL == (QSPI_CTL_CLKPOL_Msk | QSPI_CTL_TXNEG_Msk | QSPI_CTL_QSPIEN_Msk));   //0x0000000D
    CU_ASSERT(QSPIModule->CLKDIV == 0x00000000);
    CU_ASSERT(QSPIModule->SSCTL == 0x0);
    //printf("4 u32ReturnValue = %d\r\n", u32ReturnValue);
    CU_ASSERT(u32ReturnValue == 48000000);
    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());
}

void API_QSPI_Close()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    /* Set SPIEN bit to 1 */
    QSPIModule->CTL = QSPI_CTL_QSPIEN_Msk;
    QSPI_Close(QSPIModule);
    /* Check SPIEN bit */
    CU_ASSERT_FALSE(QSPIModule->CTL & QSPI_CTL_QSPIEN_Msk);

    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());
}

void API_QSPI_ClearRxFIFO()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPIModule->CLKDIV = 4;
    /* Master mode, SPI mode 0. */
    QSPIModule->CTL = (QSPI_CTL_QSPIEN_Msk | QSPI_CTL_TXNEG_Msk);
    /* Write 3 data to TX FIFO */
    QSPIModule->TX = 1;
    QSPIModule->TX = 2;
    QSPIModule->TX = 3;

    /* Wait data transfer */
    while ((QSPIModule->STATUS & QSPI_STATUS_BUSY_Msk) != 0) __NOP();

    CU_ASSERT(QSPI_GET_RX_FIFO_COUNT(QSPIModule) == 3);
    QSPI_ClearRxFIFO(QSPIModule);
    CU_ASSERT(QSPIModule->STATUS == 0x00059112);

    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());
}

void API_QSPI_ClearTxFIFO()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPIModule->CLKDIV = 0xFF;
    /* Slave mode, SPI mode 0. */
    QSPIModule->CTL = QSPI_CTL_SLAVE_Msk | QSPI_CTL_TXNEG_Msk | QSPI_CTL_QSPIEN_Msk;
    /* Write 3 data to TX FIFO */
    QSPIModule->TX = 1;
    QSPIModule->TX = 2;
    QSPIModule->TX = 3;
    CU_ASSERT((QSPIModule->STATUS & 0xF00F0000) == 0x30040000); //CU_ASSERT((QSPIModule->STATUS & 0xF0000000)== 0x30000000);
    QSPI_ClearTxFIFO(QSPIModule);
    CU_ASSERT((QSPIModule->STATUS & 0xF00F0000) == 0x00050000); //CU_ASSERT((QSPIModule->STATUS & 0xF0000000)== 0);

    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());
}

void API_QSPI_EnableAutoSS_DisableAutoSS()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPI_EnableAutoSS(QSPIModule, QSPI_SS, QSPI_SS_ACTIVE_HIGH);
    CU_ASSERT(QSPIModule->SSCTL == (QSPI_SSCTL_AUTOSS_Msk |
                                    QSPI_SSCTL_SSACTPOL_Msk |
                                    QSPI_SSCTL_SS_Msk));
    QSPI_DisableAutoSS(QSPIModule);
    CU_ASSERT((QSPIModule->SSCTL & (QSPI_SSCTL_AUTOSS_Msk | QSPI_SSCTL_SS_Msk)) == 0x0);
    QSPI_EnableAutoSS(QSPIModule, QSPI_SS, QSPI_SS_ACTIVE_LOW);
    CU_ASSERT(QSPIModule->SSCTL == (QSPI_SSCTL_AUTOSS_Msk | QSPI_SSCTL_SS_Msk));
    QSPI_DisableAutoSS(QSPIModule);
    CU_ASSERT((QSPIModule->SSCTL & (QSPI_SSCTL_AUTOSS_Msk | QSPI_SSCTL_SS_Msk)) == 0x0);

    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());
}

void API_QSPI_SetBusClock_GetBusClock()
{
    volatile uint32_t u32ReturnValue;
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    u32ReturnValue = QSPI_SetBusClock(QSPIModule, 200 * 1000);
    //printf("1 QSPIModule->CLKDIV = %d\r\n", QSPIModule->CLKDIV);
    CU_ASSERT(QSPIModule->CLKDIV == 0x00000EF);
    CU_ASSERT(u32ReturnValue == 200000);
    u32ReturnValue = QSPI_GetBusClock(QSPIModule);
    CU_ASSERT(u32ReturnValue == 200000);
    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());

    u32ReturnValue = QSPI_SetBusClock(QSPIModule, 70 * 1000 * 1000);
    CU_ASSERT(QSPIModule->CLKDIV == 0x00000000);
    //printf("2 GetBusClock u32ReturnValue = %d\r\n", u32ReturnValue);
    CU_ASSERT(u32ReturnValue == 48 * 1000 * 1000);
    u32ReturnValue = QSPI_GetBusClock(QSPIModule);
    //printf("3 GetBusClock u32ReturnValue = %d\r\n", u32ReturnValue);
    CU_ASSERT(u32ReturnValue == 48 * 1000 * 1000);
    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());
}

void API_QSPI_SetFIFO()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPI_SetFIFO(QSPIModule, 1, 2);
    CU_ASSERT((QSPIModule->FIFOCTL & (QSPI_STATUS_TXCNT_Msk | QSPI_STATUS_RXCNT_Msk)) == 0x12000000);
    QSPI_SetFIFO(QSPIModule, 0, 3);
    CU_ASSERT((QSPIModule->FIFOCTL & (QSPI_STATUS_TXCNT_Msk | QSPI_STATUS_RXCNT_Msk)) == 0x03000000);
    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());
}

void API_QSPI_EnableInt_DisableInt()
{
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPI_EnableInt(QSPIModule, QSPI_UNIT_INT_MASK);
    CU_ASSERT_TRUE(QSPIModule->CTL & QSPI_CTL_UNITIEN_Msk);
    QSPI_DisableInt(QSPIModule, QSPI_UNIT_INT_MASK);
    CU_ASSERT_FALSE(QSPIModule->CTL & QSPI_CTL_UNITIEN_Msk);

    QSPI_EnableInt(QSPIModule, QSPI_SSACT_INT_MASK);
    CU_ASSERT_TRUE(QSPIModule->SSCTL & QSPI_SSCTL_SSACTIEN_Msk);
    QSPI_DisableInt(QSPIModule, QSPI_SSACT_INT_MASK);
    CU_ASSERT_FALSE(QSPIModule->SSCTL & QSPI_SSCTL_SSACTIEN_Msk);

    QSPI_EnableInt(QSPIModule, QSPI_SSINACT_INT_MASK);
    CU_ASSERT_TRUE(QSPIModule->SSCTL & QSPI_SSCTL_SSINAIEN_Msk);
    QSPI_DisableInt(QSPIModule, QSPI_SSINACT_INT_MASK);
    CU_ASSERT_FALSE(QSPIModule->SSCTL & QSPI_SSCTL_SSINAIEN_Msk);

    QSPI_EnableInt(QSPIModule, QSPI_SLVUR_INT_MASK);
    CU_ASSERT_TRUE(QSPIModule->SSCTL & QSPI_SSCTL_SLVURIEN_Msk);
    QSPI_DisableInt(QSPIModule, QSPI_SLVUR_INT_MASK);
    CU_ASSERT_FALSE(QSPIModule->SSCTL & QSPI_SSCTL_SLVURIEN_Msk);

    QSPI_EnableInt(QSPIModule, QSPI_SLVBE_INT_MASK);
    CU_ASSERT_TRUE(QSPIModule->SSCTL & QSPI_SSCTL_SLVBEIEN_Msk);
    QSPI_DisableInt(QSPIModule, QSPI_SLVBE_INT_MASK);
    CU_ASSERT_FALSE(QSPIModule->SSCTL & QSPI_SSCTL_SLVBEIEN_Msk);

    QSPI_EnableInt(QSPIModule, QSPI_SLVTO_INT_MASK);
    CU_ASSERT_TRUE(QSPIModule->SSCTL & QSPI_SSCTL_SLVTOIEN_Msk);
    QSPI_DisableInt(QSPIModule, QSPI_SLVTO_INT_MASK);
    CU_ASSERT_FALSE(QSPIModule->SSCTL & QSPI_SSCTL_SLVTOIEN_Msk);

    QSPI_EnableInt(QSPIModule, QSPI_TXUF_INT_MASK);
    CU_ASSERT_TRUE(QSPIModule->FIFOCTL & QSPI_FIFOCTL_TXUFIEN_Msk);
    QSPI_DisableInt(QSPIModule, QSPI_TXUF_INT_MASK);
    CU_ASSERT_FALSE(QSPIModule->FIFOCTL & QSPI_FIFOCTL_TXUFIEN_Msk);

    QSPI_EnableInt(QSPIModule, QSPI_FIFO_TXTH_INT_MASK);
    CU_ASSERT_TRUE(QSPIModule->FIFOCTL & QSPI_FIFOCTL_TXTHIEN_Msk);
    QSPI_DisableInt(QSPIModule, QSPI_FIFO_TXTH_INT_MASK);
    CU_ASSERT_FALSE(QSPIModule->FIFOCTL & QSPI_FIFOCTL_TXTHIEN_Msk);

    QSPI_EnableInt(QSPIModule, QSPI_FIFO_RXTH_INT_MASK);
    CU_ASSERT_TRUE(QSPIModule->FIFOCTL & QSPI_FIFOCTL_RXTHIEN_Msk);
    QSPI_DisableInt(QSPIModule, QSPI_FIFO_RXTH_INT_MASK);
    CU_ASSERT_FALSE(QSPIModule->FIFOCTL & QSPI_FIFOCTL_RXTHIEN_Msk);

    QSPI_EnableInt(QSPIModule, QSPI_FIFO_RXOV_INT_MASK);
    CU_ASSERT_TRUE(QSPIModule->FIFOCTL & QSPI_FIFOCTL_RXOVIEN_Msk);
    QSPI_DisableInt(QSPIModule, QSPI_FIFO_RXOV_INT_MASK);
    CU_ASSERT_FALSE(QSPIModule->FIFOCTL & QSPI_FIFOCTL_RXOVIEN_Msk);

    QSPI_EnableInt(QSPIModule, QSPI_FIFO_RXTO_INT_MASK);
    CU_ASSERT_TRUE(QSPIModule->FIFOCTL & QSPI_FIFOCTL_RXTOIEN_Msk);
    QSPI_DisableInt(QSPIModule, QSPI_FIFO_RXTO_INT_MASK);
    CU_ASSERT_FALSE(QSPIModule->FIFOCTL & QSPI_FIFOCTL_RXTOIEN_Msk);

    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());
}

void API_QSPI_GetIntFlag_ClearIntFlag()
{
    volatile uint32_t u32DelayCount = 0;
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    /* Unit transfer interrupt flag test */
    QSPIModule->CLKDIV = 4;
    /* Master mode, SPI mode 0. */
    QSPIModule->CTL = 5;
    QSPIModule->TX = 0;

    while ((QSPIModule->STATUS & QSPI_STATUS_UNITIF_Msk) == 0) __NOP();

    /* Check unit transfer interrupt flag */
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_UNIT_INT_MASK) == QSPI_UNIT_INT_MASK);
    QSPI_ClearIntFlag(QSPIModule, QSPI_UNIT_INT_MASK);
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_UNIT_INT_MASK) == 0);
    /* Reset QSPIMODULE */
    ResetQSPI(GetQSPIModuleIdx());
    __NOP();

    if (GetQSPIModuleIdx() == C_QSPI0)
    {
        /* Slave selection signal active/inactive/slave-time-out/slave-under-run/bit-count-error/slave-underflow interrupt flag test */
        /* Set PA.3 (QSPI0_SS) as output mode */
        SET_GPIO_PA3();
        GPIO_SetMode(PA, BIT3, GPIO_MODE_OUTPUT);

        /* Set PA.2 (QSPI0_CLK) as GPIO output pin */
        SET_GPIO_PA2();
        GPIO_SetMode(PA, BIT2, GPIO_MODE_OUTPUT);
        /* Set PA.2 (QSPI0_CLK) to low level */
        PA2 = 0;

        /* Enable self-test function */
        QSPI_EnableSelfTest(GetQSPIModuleIdx());

        /* Set PA.3 (QSPI0_SS) to high level */
        PA3 = 1;
    }
    else if (GetQSPIModuleIdx() == C_QSPI1)
    {
        /* Slave selection signal active/inactive/slave-time-out/slave-under-run/bit-count-error/slave-underflow interrupt flag test */
        /* Set PC.5 (QSPI1_SS) as output mode */
        SET_GPIO_PC5();
        GPIO_SetMode(PC, BIT5, GPIO_MODE_OUTPUT);

        /* Set PC.4 (QSPI1_CLK) as GPIO output pin */
        SET_GPIO_PC4();
        GPIO_SetMode(PC, BIT4, GPIO_MODE_OUTPUT);
        /* Set PC.4 (QSPI1_CLK) to low level */
        PC4 = 0;

        /* Enable self-test function */
        /* Enable self-test function */
        QSPI_EnableSelfTest(GetQSPIModuleIdx());

        /* Set PC.5 (QSPI1_SS) to high level */
        PC5 = 1;
    }

    QSPIModule->CLKDIV = 0;
    /* Slave selection signal is low-active; set slave time-out period. */
    QSPIModule->SSCTL = (QSPIModule->SSCTL & (~QSPI_SSCTL_SLVTOCNT_Msk)) |
                        (50 << QSPI_SSCTL_SLVTOCNT_Pos);
    /* Slave mode, SPI mode 0. */
    QSPIModule->CTL = 0x00040005;

    for (u32DelayCount = 0; u32DelayCount < 10; u32DelayCount++) __NOP();

    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SSACT_INT_MASK) == 0);
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SSINACT_INT_MASK) == 0);
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_TXUF_INT_MASK) == 0);

    if (GetQSPIModuleIdx() == C_QSPI0)
    {
        /* Set PA.3 (QSPI0_SS) to low level */
        PA3 = 0;
    }
    else if (GetQSPIModuleIdx() == C_QSPI1)
    {
        /* Set PC.5 (QSPI1_SS) to low level */
        PC5 = 0;
    }

    for (u32DelayCount = 0; u32DelayCount < 10; u32DelayCount++) __NOP();

    /* Check slave selection signal active interrupt flag */
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SSACT_INT_MASK) == QSPI_SSACT_INT_MASK);
    QSPI_ClearIntFlag(QSPIModule, QSPI_SSACT_INT_MASK);
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SSACT_INT_MASK) == 0);
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SSINACT_INT_MASK) == 0);
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SLVTO_INT_MASK) == 0);
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SLVUR_INT_MASK) == 0);
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SLVBE_INT_MASK) == 0);

    /* Check slave TX underflow interrupt flag */
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_TXUF_INT_MASK) == QSPI_TXUF_INT_MASK);

    //    /* Set PA.2 (QSPI0_CLK) to high level */
    //    PA2 = 1;
    //    for(u32DelayCount=0; u32DelayCount<10; u32DelayCount++);
    //    /* Set PA.2 (QSPI0_CLK) to low level */
    //    PA2 = 0;
    //    for(u32DelayCount=0; u32DelayCount<1000; u32DelayCount++) __NOP();

    //    /* Check slave time-out interrupt flag */
    //    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SLVTO_INT_MASK)==QSPI_SLVTO_INT_MASK);
    //    QSPI_ClearIntFlag(QSPIModule, QSPI_SLVTO_INT_MASK);
    //    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SLVTO_INT_MASK)==0);

    if (GetQSPIModuleIdx() == C_QSPI0)
    {
        /* Set PA.3 (QSPI0_SS) to high level */
        PA3 = 1;
    }
    else if (GetQSPIModuleIdx() == C_QSPI1)
    {
        /* Set PC.5 (QSPI1_SS) to high level */
        PC5 = 1;
    }

    __NOP();
    /* Check slave selection signal inactive interrupt flag */
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SSINACT_INT_MASK) == QSPI_SSINACT_INT_MASK);
    QSPI_ClearIntFlag(QSPIModule, QSPI_SSINACT_INT_MASK);
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SSINACT_INT_MASK) == 0);

    /* Clear slave TX underflow interrupt flag */
    QSPI_ClearIntFlag(QSPIModule, QSPI_TXUF_INT_MASK);
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_TXUF_INT_MASK) == 0);

    /* Check slave bit count error interrupt flag */
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SLVBE_INT_MASK) == QSPI_SLVBE_INT_MASK);
    QSPI_ClearIntFlag(QSPIModule, QSPI_SLVBE_INT_MASK);
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SLVBE_INT_MASK) == 0);

    /* Check slave TX under run interrupt flag */
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SLVUR_INT_MASK) == QSPI_SLVUR_INT_MASK);
    QSPI_ClearIntFlag(QSPIModule, QSPI_SLVUR_INT_MASK);
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_SLVUR_INT_MASK) == 0);

    /* Disable self-test function */
    QSPI_DisableSelfTest(GetQSPIModuleIdx());

    if (GetQSPIModuleIdx() == C_QSPI0)
    {
        /* Set QSPI0_CLK as SPI function pin */
        QSPI0_CLK_PIN_INIT;
    }
    else if (GetQSPIModuleIdx() == C_QSPI1)
    {
        /* Set QSPI1_CLK as SPI function pin */
        QSPI1_CLK_PIN_INIT;
    }

    /* Reset QSPIMODULE */
    ResetQSPI(GetQSPIModuleIdx());
    __NOP();

    /* TX threshold interrupt flag test */
    QSPIModule->CLKDIV = 0xFF;
    /* Slave mode, SPI mode 0. */
    QSPIModule->CTL = 0x00040005;
    /* Set TX and RX FIFO threshold */
    QSPI_SetFIFO(QSPIModule, 2, 2);
    /* Check TX threshold interrupt flag */
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_FIFO_TXTH_INT_MASK) == QSPI_FIFO_TXTH_INT_MASK);
    /* Write to TX FIFO */
    QSPIModule->TX = 1;
    QSPIModule->TX = 2;
    QSPIModule->TX = 3;
    QSPIModule->TX = 4;
    /* Check TX threshold interrupt flag */
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_FIFO_TXTH_INT_MASK) == 0);
    /* Reset QSPIMODULE */
    ResetQSPI(GetQSPIModuleIdx());;
    __NOP();

    /* RX threshold interrupt flag test */
    QSPIModule->CLKDIV = 0xF;
    /* Set TX and RX FIFO threshold. */
    QSPI_SetFIFO(QSPIModule, 2, 2);
    /* Master mode, SPI mode 0. */
    QSPIModule->CTL = 5;
    /* Check RX threshold interrupt flag */
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_FIFO_RXTH_INT_MASK) == 0);
    /* Write to TX FIFO */
    QSPIModule->TX = 1;
    QSPIModule->TX = 2;
    QSPIModule->TX = 3;

    /* Wait data transfer */
    while (QSPIModule->STATUS & QSPI_STATUS_BUSY_Msk) __NOP();

    /* Check RX threshold interrupt flag */
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_FIFO_RXTH_INT_MASK) == QSPI_FIFO_RXTH_INT_MASK);

    /* RX overrun interrupt flag test */
    /* Check RX overrun interrupt flag */
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_FIFO_RXOV_INT_MASK) == 0);
    /* Write to TX FIFO */
    QSPIModule->TX = 4;
    QSPIModule->TX = 5;
    QSPIModule->TX = 6;
    QSPIModule->TX = 7;
    QSPIModule->TX = 8;
    QSPIModule->TX = 9;

    /* Wait data transfer */
    while (QSPIModule->STATUS & QSPI_STATUS_BUSY_Msk) __NOP();

    /* Check RX overrun interrupt flag */
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_FIFO_RXOV_INT_MASK) == QSPI_FIFO_RXOV_INT_MASK);
    QSPI_ClearIntFlag(QSPIModule, QSPI_FIFO_RXOV_INT_MASK);
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_FIFO_RXOV_INT_MASK) == 0);

    /* RX time-out interrupt flag test */
    /* Clear RX time-out interrupt flag */
    QSPI_ClearIntFlag(QSPIModule, QSPI_FIFO_RXTO_INT_MASK);
    /* Check RX time-out interrupt flag */
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_FIFO_RXTO_INT_MASK) == 0);
    /* Write to TX FIFO */
    QSPIModule->TX = 1;
    QSPIModule->TX = 2;
    QSPIModule->TX = 3;

    /* Wait data transfer */
    while (QSPIModule->STATUS & QSPI_STATUS_BUSY_Msk) __NOP();

    /* Check RX time-out interrupt flag */
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_FIFO_RXTO_INT_MASK) == QSPI_FIFO_RXTO_INT_MASK);
    QSPI_ClearIntFlag(QSPIModule, QSPI_FIFO_RXTO_INT_MASK);
    CU_ASSERT(QSPI_GetIntFlag(QSPIModule, QSPI_FIFO_RXTO_INT_MASK) == 0);

    /* Reset QSPIMODULE */
    ResetQSPI(GetQSPIModuleIdx());
    __NOP();
}

void API_QSPI_GetStatus()
{
    uint32_t u32DelayCount;
    QSPI_T *QSPIModule = NULL;

    QSPIModule = (QSPI_T *)GetQSPIModule(GetQSPIModuleIdx());

    QSPIModule->CLKDIV = 0;
    /* Check SPIEN flag */
    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_QSPIEN_STS_MASK) == 0);
    /* Slave mode, SPI mode 0. */
    QSPIModule->CTL = 0x00040005;
    /* Check busy flag */
    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_BUSY_MASK) == 0);
    /* Check SPIEN flag */
    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_QSPIEN_STS_MASK) == QSPI_QSPIEN_STS_MASK);

    /* SPI TX empty flag test */
    /* Check TX empty flag */
    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_TX_EMPTY_MASK) == QSPI_TX_EMPTY_MASK);
    /* Write to TX FIFO */
    QSPIModule->TX = 1;
    QSPIModule->TX = 2;
    /* Check TX empty flag */
    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_TX_EMPTY_MASK) == 0);

    /* Check busy flag */
    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_BUSY_MASK) == QSPI_BUSY_MASK);

    /* SPI TX full flag test */
    /* Check TX full flag */
    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_TX_FULL_MASK) == 0);
    /* Write to TX FIFO */
    QSPIModule->TX = 3;
    QSPIModule->TX = 4;
    QSPIModule->TX = 5;
    QSPIModule->TX = 6;
    QSPIModule->TX = 7;
    QSPIModule->TX = 8;
    QSPIModule->TX = 9;
    /* Check TX full flag */
    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_TX_FULL_MASK) == QSPI_TX_FULL_MASK);
    /* Reset QSPIMODULE */
    ResetQSPI(GetQSPIModuleIdx());
    __NOP();

    /* SPI RX empty flag test */
    QSPIModule->CLKDIV = 0xFF;
    /* Master mode, SPI mode 0. */
    QSPIModule->CTL = 5;

    /* Check TX/RX reset flag */
    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_TXRX_RESET_MASK) == 0);
    QSPIModule->FIFOCTL |= QSPI_FIFOCTL_RXRST_Msk;
    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_TXRX_RESET_MASK) == QSPI_TXRX_RESET_MASK);

    for (u32DelayCount = 0; u32DelayCount < 256; u32DelayCount++) __NOP();

    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_TXRX_RESET_MASK) == 0);

    /* Check RX empty flag */
    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_RX_EMPTY_MASK) == QSPI_RX_EMPTY_MASK);
    /* Write to TX FIFO */
    QSPIModule->TX = 1;

    /* Wait data transfer */
    while (QSPIModule->STATUS & QSPI_STATUS_BUSY_Msk) __NOP();

    /* Check RX empty flag */
    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_RX_EMPTY_MASK) == 0);

    /* SPI RX full flag test */
    /* Check RX full flag */
    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_RX_FULL_MASK) == 0);
    /* Write to TX FIFO */
    QSPIModule->TX = 2;
    QSPIModule->TX = 3;
    QSPIModule->TX = 4;
    QSPIModule->TX = 5;
    QSPIModule->TX = 6;
    QSPIModule->TX = 7;
    QSPIModule->TX = 8;

    /* Wait data transfer */
    while (QSPIModule->STATUS & QSPI_STATUS_BUSY_Msk) __NOP();

    /* Check RX full flag */
    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_RX_FULL_MASK) == QSPI_RX_FULL_MASK);
    /* Reset QSPIMODULE */
    ResetQSPI(GetQSPIModuleIdx());
    __NOP();

    if (GetQSPIModuleIdx() == C_QSPI0)
    {
        /* Check SPIx_SS line status */
        /* Set PA.3 (QSPI0_SS) as output mode */
        SET_GPIO_PA3();
        GPIO_SetMode(PA, BIT3, GPIO_MODE_OUTPUT);

        /* Enable self-test function */
        QSPI_EnableSelfTest(GetQSPIModuleIdx());

        /* Set PA.3 (QSPI0_SS) to high level */
        PA3 = 1;
    }
    else if (GetQSPIModuleIdx() == C_QSPI1)
    {
        /* Set PC.5 (QSPI1_SS) as output mode */
        SET_GPIO_PC5();
        GPIO_SetMode(PC, BIT5, GPIO_MODE_OUTPUT);

        /* Enable self-test function */
        QSPI_EnableSelfTest(GetQSPIModuleIdx());

        PC5 = 1;
    }

    QSPIModule->CLKDIV = 0;
    /* Slave selection signal is low-active. */
    QSPIModule->SSCTL = 0;
    /* Slave mode, SPI mode 0. */
    QSPIModule->CTL = 0x00040005;

    for (u32DelayCount = 0; u32DelayCount < 10; u32DelayCount++) __NOP();

    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_SSLINE_STS_MASK) == QSPI_SSLINE_STS_MASK);

    if (GetQSPIModuleIdx() == C_QSPI0)
    {
        /* Set PA.3 (QSPI0_SS) to low level */
        PA3 = 0;
    }
    else if (GetQSPIModuleIdx() == C_QSPI1)
    {
        /* Set PC.5 (QSPI1_SS) to low level */
        PC5 = 0;
    }

    for (u32DelayCount = 0; u32DelayCount < 100; u32DelayCount++) __NOP();

    CU_ASSERT(QSPI_GetStatus(QSPIModule, QSPI_SSLINE_STS_MASK) == 0);
    __NOP();

    /* Disable self-test function */
    QSPI_DisableSelfTest(GetQSPIModuleIdx());

    /* Reset QSPI0 */
    ResetQSPI(GetQSPIModuleIdx());

    __NOP();
}

CU_TestInfo QSPI_MacroTests[] =
{
    {"Clear SPI unit transfer interrupt flag", MACRO_QSPI_CLR_UNIT_TRANS_INT_FLAG},
    {"Enable/Disable SPI 2-Bit transfer mode", MACRO_QSPI_ENABLE_DISABLE_2BIT_MODE},
    {"Enable/Disable SPI Slave 3-Wire mode", MACRO_QSPI_ENABLE_DISABLE_3WIRE_MODE},
    {"Disable SPI Dual I/O mode", MACRO_QSPI_DISABLE_DUAL_MODE},
    {"Disable SPI Quad I/O mode", MACRO_QSPI_DISABLE_QUAD_MODE},
    {"Enable SPI Dual input mode", MACRO_QSPI_ENABLE_DUAL_INPUT_MODE},
    {"Enable SPI Dual output mode", MACRO_QSPI_ENABLE_DUAL_OUTPUT_MODE},
    {"Enable SPI Quad input mode", MACRO_QSPI_ENABLE_QUAD_INPUT_MODE},
    {"Enable SPI Quad output mode", MACRO_QSPI_ENABLE_QUAD_OUTPUT_MODE},
    {"Trigger/Disable SPI RX DMA transfer", MACRO_QSPI_TRIGGER_DISABLE_RX_PDMA},
    {"Trigger/Disable SPI TX DMA transfer", MACRO_QSPI_TRIGGER_DISABLE_TX_PDMA},
    {"Get SPI RX FIFO count and empty flag", MACRO_QSPI_GET_RX_FIFO_COUNT_EMPTY_FLAG},
    {"Get SPI TX FIFO empty and full flag", MACRO_QSPI_GET_TX_FIFO_EMPTY_FULL_FLAG},
    {"Read SPI RX", MACRO_QSPI_READ_RX},
    {"Write SPI TX", MACRO_QSPI_WRITE_TX},
    {"Set SPI SS pin to high state", MACRO_QSPI_SET_SS_HIGH},
    {"Set SPI SS pin to low state", MACRO_QSPI_SET_SS_LOW},
    {"Enable/Disable SPI Byte Reorder function", MACRO_QSPI_ENABLE_DISABLE_BYTE_REORDER},
    {"Set SPI suspend cycle", MACRO_QSPI_SET_SUSPEND_CYCLE},
    {"Set SPI transfer sequence with LSB/MSB first", MACRO_QSPI_SET_LSB_MSB_FIRST},
    {"Set SPI data width", MACRO_QSPI_SET_DATA_WIDTH},
    {"Get SPI busy flag", MACRO_QSPI_IS_BUSY},
    {"Enable/Disable SPI", MACRO_QSPI_ENABLE_DISABLE},
    CU_TEST_INFO_NULL
};

CU_TestInfo QSPI_ApiTests[] =
{
    {"SPI Open", API_QSPI_Open},
    {"SPI Close", API_QSPI_Close},
    {"Clear SPI RX FIFO", API_QSPI_ClearRxFIFO},
    {"Clear SPI TX FIFO", API_QSPI_ClearTxFIFO},
    {"Enable/Disable SPI automatic slave selection function", API_QSPI_EnableAutoSS_DisableAutoSS},
    {"Set/Get SPI bus clock rate", API_QSPI_SetBusClock_GetBusClock},
    {"Set SPI FIFO threshold", API_QSPI_SetFIFO},
    {"Enable/Disable SPI interrupt function", API_QSPI_EnableInt_DisableInt},
    {"Get/Clear SPI interrupt flag", API_QSPI_GetIntFlag_ClearIntFlag},
    {"Get SPI status", API_QSPI_GetStatus},

    CU_TEST_INFO_NULL
};

CU_SuiteInfo suites[] =
{
    {"QSPI MACRO", QSPI_Tests_Init, QSPI_Tests_Clean, NULL, NULL, QSPI_MacroTests},
    {"QSPI API", QSPI_Tests_Init, QSPI_Tests_Clean, NULL, NULL, QSPI_ApiTests},
    CU_SUITE_INFO_NULL
};

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
