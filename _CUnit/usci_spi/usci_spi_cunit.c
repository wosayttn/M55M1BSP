/*----------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.                                        */
/*                                                                                                         */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   usci_spi_cunit.c                                                         */
/*            The test function of USCI_SPI for CUnit.                        */
/* Project:   M251                                                            */
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
#include "usci_spi_cunit.h"
#include "PinConfig.h"

//------------------------------------------------------------------------------
//extern unsigned int STATE0;
//extern unsigned int STATE1;
//extern unsigned int ERR_STATE0;
//extern unsigned int ERR_STATE1;

//------------------------------------------------------------------------------
static USPI_T *g_apUSPIModule[] =
{
    USPI0,
    //USPI1,
};

static uint32_t u32USPIModuleIdx = 0;

/*----------------------------------------------------------------------------*/
/* Test function                                                              */
/*----------------------------------------------------------------------------*/
void SetUSPIModuleIdx(uint32_t u32Value)
{
    u32USPIModuleIdx = u32Value;
}

uint32_t GetUSPIModuleIdx(void)
{
    return u32USPIModuleIdx;
}

void *GetUSPIModule(uint32_t u32ModuleIdx)
{
    return g_apUSPIModule[u32ModuleIdx];
}

int USCI_SPI_Tests_Init(void)
{
    return 0;
}

int USCI_SPI_Tests_Clean(void)
{
    return 0;
}

/* Disable self-test function */
void USPI_DisableSelfTest(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Disable self-test function */
    //SYS->ALTCTL0 &= ~(SYS_ALTCTL0_SELFTEST_Msk);
    outpw(SYS_BASE + 0xE00, 0x00);

    outpw(USCI0_BASE + 0x84, ~(1 << 30));

    /* Lock protected registers */
    SYS_LockReg();
}

/* Enable self-test function */
void USPI_EnableSelfTest(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable self-test function */
    //SYS->ALTCTL0 |= SYS_ALTCTL0_SELFTEST_Msk;
    outpw(SYS_BASE + 0xE00, 0x01);

    outpw(USCI0_BASE + 0x84, (1 << 30));


    /* Lock protected registers */
    SYS_LockReg();
}

/*----------------------------------------------------------------------------*/
/* Function: function_name                                                    */
/*                                                                            */
/* Parameters:                                                                */
/*               param - [in/out], parameter_description                      */
/*               param - [in/out], parameter_description                      */
/*                                                                            */
/* Returns:      the_value                                                    */
/* Side effects: the_effect                                                   */
/* Description:                                                               */
/*               description                                                  */
/*----------------------------------------------------------------------------*/
void InitUSPIxModulePIN(uint32_t u32Module)
{
    switch (u32Module)
    {
        case C_USPI0:
            /* Slave selection signal active/inactive/slave-under-run/bit-count-error/slave-time-out interrupt flag test */
            /* Set PB.0 (USPI0_CTL0) as output mode */
            SET_GPIO_PB0();
            GPIO_SetMode(PB, BIT0, GPIO_MODE_OUTPUT);

            /* Set PA.11 (USPI0_CLK) as GPIO output pin */
            SET_GPIO_PA11();
            GPIO_SetMode(PA, BIT11, GPIO_MODE_OUTPUT);
            /* Set PA.11 (USPI0_CLK) to low level */
            PA11 = 0;

            USPI_EnableSelfTest();

            SET_GPIO_PA10();
            GPIO_SetMode(PA, BIT10, GPIO_MODE_OUTPUT);

            USPI0_MISO_PIN_INIT();

            /* Set PB.0 (USPI0_CTL0) to high level */
            PB0 = 1;
            break;

#if (EN_MULTI_USPI1 == 1)

        case C_USPI1:
            /* Slave selection signal active/inactive/slave-under-run/bit-count-error/slave-time-out interrupt flag test */
            /* Set PB.5 (USPI1_CTL0) as output mode */
            PB->MODE = (PB->MODE & ~GPIO_MODE_MODE5_Msk) | (1 << GPIO_MODE_MODE5_Pos);
            /* Set PB.1 (USPI1_CLK) as GPIO output pin */
            SYS->GPB_MFPL &= ~SYS_GPB_MFPL_PB1MFP_Msk;
            PB->MODE = (PB->MODE & ~GPIO_MODE_MODE1_Msk) | (1 << GPIO_MODE_MODE1_Pos);
            /* Set PB.1 (USPI1_CLK) to low level */
            PB1 = 0;
            /* Enable self-test function */
            //outp32(0x40000014, 1);
            SYS->ALTCTL0 |= SYS_ALTCTL0_SELFTEST_Msk;

            /* Set PB.5 (USPI1_CTL0) to high level */
            PB5 = 1;
            break;
#endif //EN_MULTI_USPI1

#if (EN_MULTI_USPI2 == 1)

        case C_USPI2:
            /* Slave selection signal active/inactive/slave-under-run/bit-count-error/slave-time-out interrupt flag test */
            /* Set PA.3 (USPI2_CTL0) as output mode */
            PA->MODE = (PA->MODE & ~GPIO_MODE_MODE3_Msk) | (1 << GPIO_MODE_MODE3_Pos);
            /* Set PA.2 (USPI2_CLK) as GPIO output pin */
            SYS->GPA_MFPL &= ~SYS_GPA_MFPL_PA2MFP_Msk;
            PA->MODE = (PA->MODE & ~GPIO_MODE_MODE2_Msk) | (1 << GPIO_MODE_MODE2_Pos);
            /* Set PA.1 (USPI2_CLK) to low level */
            PA2 = 0;
            /* Enable self-test function */
            //outp32(0x40000014, 1);
            SYS->ALTCTL0 |= SYS_ALTCTL0_SELFTEST_Msk;

            /* Set PA.3 (USPI2_CTL0) to high level */
            PA3 = 1;
            break;
#endif //EN_MULTI_USPI2
    }
}

void USPIxCtlHigh(uint32_t u32Module)
{
    switch (u32Module)
    {
        case C_USPI0:
            /* Set PB.0 (USPI0_CTL0) to high level */
            PB0 = 1;
            break;

#if (EN_MULTI_USPI1 == 1)

        case C_USPI1:
            /* Set PB.5 (USPI1_CTL0) to high level */
            PB5 = 1;
            break;
#endif //EN_MULTI_USPI1

#if (EN_MULTI_USPI2 == 1)

        case C_USPI2:
            /* Set PA.3 (USPI2_CTL0) to high level */
            PA3 = 1;
            break;
#endif //EN_MULTI_USPI2
    }
}

void USPIxCtlLow(uint32_t u32Module)
{
    switch (u32Module)
    {
        case C_USPI0:
            /* Set PB.0 (USPI0_CTL0) to low level */
            PB0 = 0;
            break;

#if (EN_MULTI_USPI1 == 1)

        case C_USPI1:
            /* Set PB.5 (USPI1_CTL0) to low level */
            PB5 = 0;
            break;
#endif //EN_MULTI_USPI1

#if (EN_MULTI_USPI2 == 1)

        case C_USPI2:
            /* Set PA.3 (USPI2_CTL0) to low level */
            PA3 = 0;
            break;
#endif //EN_MULTI_USPI2
    }
}

void USPIxClkHigh(uint32_t u32Module)
{
    switch (u32Module)
    {
        case C_USPI0:
            /* Set PA.11 (USPI0_CLK) to low level */
            PA11 = 1;
            break;

#if (EN_MULTI_USPI1 == 1)

        case C_USPI1:
            /* Set PB.1 (USPI0_CLK) to low level */
            PB1 = 1;
            break;
#endif //EN_MULTI_USPI1

#if (EN_MULTI_USPI2 == 1)

        case C_USPI2:
            /* Set PA.2 (USPI0_CLK) to low level */
            PA2 = 1;
            break;
#endif //EN_MULTI_USPI2
    }
}

void USPIxClkLow(uint32_t u32Module)
{
    switch (u32Module)
    {
        case C_USPI0:
            /* Set PA.11 (USPI0_CLK) to low level */
            PA11 = 0;
            break;

#if (EN_MULTI_USPI1 == 1)

        case C_USPI1:
            /* Set PB.1 (USPI0_CLK) to low level */
            PB1 = 0;
            break;
#endif //EN_MULTI_USPI1

#if (EN_MULTI_USPI2 == 1)

        case C_USPI2:
            /* Set PA.2 (USPI0_CLK) to low level */
            PA2 = 0;
            break;
#endif //EN_MULTI_USPI2
    }
}

void ResetUSPIxModule(uint32_t u32Module)
{
    SYS_UnlockReg();

    switch (u32Module)
    {
        case C_USPI0:
            /* Reset USPI0 */
            SYS->USCIRST |= (1 << (SYS_USCIRST_USCI0RST_Pos));
            SYS->USCIRST &= ~(SYS_USCIRST_USCI0RST_Msk);
            break;

#if (EN_MULTI_USPI1 == 1)

        case C_USPI1:
            /* Reset USPI1 */
            SYS->USCIRST |= (1 << (SYS_USCIRST_USCI1RST_Pos));
            SYS->USCIRST &= ~(SYS_USCIRST_USCI1RST_Msk);
            break;
#endif //EN_MULTI_USPI1

#if (EN_MULTI_USPI2 == 1)

        case C_USPI2:
            /* Reset USPI1 */
            SYS->USCIRST |= (1 << (SYS_USCIRST_USCI2RST_Pos));
            SYS->USCIRST &= ~(SYS_USCIRST_USCI2RST_Msk);
            break;
#endif //EN_MULTI_USPI2
    }

    SYS_LockReg();
}

//------------------------------------------------------------------------------
void MACRO_USCI_SPI_ENABLE_DISABLE_3WIRE_MODE()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;
    pUSPIModule->PROTCTL |=  USPI_PROTCTL_PROTEN_Msk;

    /* Check SLV3WIRE bit */
    CU_ASSERT_FALSE(pUSPIModule->PROTCTL & 0x00000002);
    USPI_ENABLE_3WIRE_MODE(pUSPIModule);
    /* Check SLV3WIRE bit */
    CU_ASSERT_TRUE(pUSPIModule->PROTCTL & 0x00000002);
    USPI_DISABLE_3WIRE_MODE(pUSPIModule);
    CU_ASSERT_FALSE(pUSPIModule->PROTCTL & 0x00000002);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void MACRO_USCI_SPI_GET_RX_EMPTY_FLAG()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    /* Select USCI_SPI0 protocol */
    pUSPIModule->CTL &= ~USPI_CTL_FUNMODE_Msk;
    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;
    /* Master mode, USPI mode 0. */
    pUSPIModule->PROTCTL = USPI_MASTER | USPI_MODE_0;
    pUSPIModule->LINECTL = 0;
    /* Set USCI_SPI0 clock rate = f_PCLK0 / 2*(5+1) */
    pUSPIModule->BRGEN = (pUSPIModule->BRGEN & (~USPI_BRGEN_CLKDIV_Msk)) | (5 << USPI_BRGEN_CLKDIV_Pos);
    /* Enable USCI_SPI0 protocol */
    pUSPIModule->PROTCTL |= USPI_PROTCTL_PROTEN_Msk;

    /* Check RX_EMPTY flag */
    CU_ASSERT(USPI_GET_RX_EMPTY_FLAG(pUSPIModule) == 1);
    pUSPIModule->TXDAT = 1;

    /* Check busy state */
    while (USPI_IS_BUSY(pUSPIModule))
    {
        __NOP();
    }

    /* Check RX_EMPTY flag */
    CU_ASSERT(USPI_GET_RX_EMPTY_FLAG(pUSPIModule) == 0);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void MACRO_USCI_SPI_GET_TX_EMPTY_FULL_FLAG()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    /* Select USCI_SPI0 protocol */
    pUSPIModule->CTL &= ~USPI_CTL_FUNMODE_Msk;
    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;
    /* Slave mode, USPI mode 0. */
    pUSPIModule->PROTCTL = USPI_SLAVE | USPI_MODE_0;
    pUSPIModule->LINECTL = 0;
    /* Set USCI_SPI0 clock rate = f_PCLK0 */
    pUSPIModule->BRGEN = pUSPIModule->BRGEN & (~USPI_BRGEN_CLKDIV_Msk);
    /* Enable USCI_SPI0 protocol */
    pUSPIModule->PROTCTL |= USPI_PROTCTL_PROTEN_Msk;

    /* Check TX_EMPTY flag */
    CU_ASSERT(USPI_GET_TX_EMPTY_FLAG(pUSPIModule) == 1);
    /* Check TX_FULL flag */
    CU_ASSERT(USPI_GET_TX_FULL_FLAG(pUSPIModule) == 0);
    pUSPIModule->TXDAT = 1;
    /* Check TX_EMPTY flag */
    CU_ASSERT(USPI_GET_TX_EMPTY_FLAG(pUSPIModule) == 0);
    /* Check TX_FULL flag */
    CU_ASSERT(USPI_GET_TX_FULL_FLAG(pUSPIModule) == 1);
    pUSPIModule->TXDAT = 2;
    pUSPIModule->TXDAT = 3;
    pUSPIModule->TXDAT = 4;
    /* Check TX_FULL flag */
    CU_ASSERT(USPI_GET_TX_FULL_FLAG(pUSPIModule) == 1);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void MACRO_USCI_SPI_WRITE_TX_READ_RX()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    /* Select USCI_SPI0 protocol */
    pUSPIModule->CTL &= ~USPI_CTL_FUNMODE_Msk;
    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;

    //Enable SELFTEST
    switch (GetUSPIModuleIdx())
    {
        case C_USPI0:
            *(volatile uint32_t *)(USCI0_BASE + 0x84) |= (1 << 30);
            break;
#if (EN_MULTI_USPI1 == 1)

        case C_USPI1:
            *(volatile uint32_t *)(USCI1_BASE + 0x84) |= (1 << 30);
            break;
#endif //EN_MULTI_USPI1

#if (EN_MULTI_USPI2 == 1)

        case C_USPI2:
            *(volatile uint32_t *)(USCI2_BASE + 0x84) |= (1 << 30);
            break;
#endif //EN_MULTI_USPI2
    }

    /* Master mode, USPI mode 0. */
    pUSPIModule->PROTCTL = USPI_MASTER | USPI_MODE_0;
    pUSPIModule->LINECTL = 0;
    /* Set USCI_SPI0 clock rate = f_PCLK0 / 2*(5+1) */
    pUSPIModule->BRGEN = (pUSPIModule->BRGEN & (~USPI_BRGEN_CLKDIV_Msk)) |
                         (5 << USPI_BRGEN_CLKDIV_Pos);
    /* Enable USCI_SPI0 protocol */
    pUSPIModule->PROTCTL |= USPI_PROTCTL_PROTEN_Msk;

    /* Write 3 data to TX register */
    USPI_WRITE_TX(pUSPIModule, 1);

    while (pUSPIModule->BUFSTS & USPI_BUFSTS_TXFULL_Msk);

    USPI_WRITE_TX(pUSPIModule, 2);

    while (pUSPIModule->BUFSTS & USPI_BUFSTS_TXFULL_Msk);

    USPI_WRITE_TX(pUSPIModule, 3);
    CU_ASSERT(USPI_READ_RX(pUSPIModule) == 1);

    /* Wait data transfer */
    while (pUSPIModule->PROTSTS & USPI_PROTSTS_BUSY_Msk)
    {
        __NOP();
    }

    /* Read data from RX register */
    CU_ASSERT(USPI_READ_RX(pUSPIModule) == 2);
    CU_ASSERT(USPI_READ_RX(pUSPIModule) == 3);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void MACRO_USCI_SPI_SET_SS_HIGH()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;
    pUSPIModule->PROTCTL |= USPI_PROTCTL_PROTEN_Msk;

    USPI_SET_SS_HIGH(pUSPIModule);
    CU_ASSERT_FALSE(pUSPIModule->LINECTL & 0x00000080);
    CU_ASSERT(pUSPIModule->PROTCTL == 0x80000304);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void MACRO_USCI_SPI_SET_SS_LOW()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;
    pUSPIModule->PROTCTL |= USPI_PROTCTL_PROTEN_Msk;

    USPI_SET_SS_LOW(pUSPIModule);
    CU_ASSERT_TRUE(pUSPIModule->LINECTL & 0x00000080);
    CU_ASSERT(pUSPIModule->PROTCTL == 0x80000300);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void MACRO_USCI_SPI_SET_SUSPEND_CYCLE()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;
    pUSPIModule->PROTCTL |=  USPI_PROTCTL_PROTEN_Msk;

    USPI_SET_SUSPEND_CYCLE(pUSPIModule, 0);
    CU_ASSERT((pUSPIModule->PROTCTL & 0x00000F00) == 0);
    USPI_SET_SUSPEND_CYCLE(pUSPIModule, 8);
    CU_ASSERT((pUSPIModule->PROTCTL & 0x00000F00) == 0x00000800);
    USPI_SET_SUSPEND_CYCLE(pUSPIModule, 15);
    CU_ASSERT((pUSPIModule->PROTCTL & 0x00000F00) == 0x00000F00);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void MACRO_USCI_SPI_SET_LSB_MSB_FIRST()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;
    pUSPIModule->PROTCTL |=  USPI_PROTCTL_PROTEN_Msk;

    USPI_SET_LSB_FIRST(pUSPIModule);
    CU_ASSERT_TRUE(pUSPIModule->LINECTL & 0x00000001);
    USPI_SET_MSB_FIRST(pUSPIModule);
    CU_ASSERT_FALSE(pUSPIModule->LINECTL & 0x00000001);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void MACRO_USCI_SPI_SET_DATA_WIDTH()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;
    pUSPIModule->PROTCTL |=  USPI_PROTCTL_PROTEN_Msk;

    USPI_SET_DATA_WIDTH(pUSPIModule, 16);
    CU_ASSERT((pUSPIModule->LINECTL & 0x00000F00) == 0);
    USPI_SET_DATA_WIDTH(pUSPIModule, 8);
    CU_ASSERT((pUSPIModule->LINECTL & 0x00000F00) == 0x00000800);
    USPI_SET_DATA_WIDTH(pUSPIModule, 4);
    CU_ASSERT((pUSPIModule->LINECTL & 0x00000F00) == 0x00000400);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void MACRO_USCI_SPI_IS_BUSY()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    /* Select USCI_SPI0 protocol */
    pUSPIModule->CTL &= ~USPI_CTL_FUNMODE_Msk;
    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;
    /* master mode, USPI mode 0. */
    pUSPIModule->PROTCTL = USPI_MASTER | USPI_MODE_0;
    pUSPIModule->LINECTL = 0;
    /* Set USCI_SPI0 clock rate = f_PCLK0 */
    pUSPIModule->BRGEN = pUSPIModule->BRGEN & (~USPI_BRGEN_CLKDIV_Msk);
    /* Enable USCI_SPI0 protocol */
    pUSPIModule->PROTCTL |= USPI_PROTCTL_PROTEN_Msk;

    pUSPIModule->TXDAT = 1;
    CU_ASSERT_TRUE(USPI_IS_BUSY(pUSPIModule));

    /* Wait data transfer */
    while (pUSPIModule->PROTSTS & USPI_PROTSTS_BUSY_Msk)
    {
        __NOP();
    }

    CU_ASSERT_FALSE(USPI_IS_BUSY(pUSPIModule));

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void MACRO_USCI_SPI_GetIntFlag_ClearIntFlag()
{
    volatile uint32_t u32DelayCount = 0;
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    /* Select USCI_SPI0 protocol */
    pUSPIModule->CTL &= ~USPI_CTL_FUNMODE_Msk;
    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;

    /* Master mode, USPI mode 0. */
    pUSPIModule->PROTCTL = USPI_MASTER | USPI_MODE_0;
    pUSPIModule->LINECTL = 0;
    /* Set USCI_SPI0 clock rate = f_PCLK0 / 2*(359+1) */
    pUSPIModule->BRGEN = (pUSPIModule->BRGEN & (~USPI_BRGEN_CLKDIV_Msk)) |
                         (359 << USPI_BRGEN_CLKDIV_Pos);
    /* Enable USCI_SPI0 protocol */
    pUSPIModule->PROTCTL |= USPI_PROTCTL_PROTEN_Msk;

    pUSPIModule->TXDAT = 0x1112;

    while ((pUSPIModule->PROTSTS & USPI_PROTSTS_BUSY_Msk) != 0)
    {
        __NOP();
    }

    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_TXSTIF_Msk) == USPI_PROTSTS_TXSTIF_Msk);
    USPI_CLR_PROT_INT_FLAG(pUSPIModule, USPI_PROTSTS_TXSTIF_Msk);
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_TXSTIF_Msk) == 0);

    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_TXENDIF_Msk) == USPI_PROTSTS_TXENDIF_Msk);
    USPI_CLR_PROT_INT_FLAG(pUSPIModule, USPI_PROTSTS_TXENDIF_Msk);
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_TXENDIF_Msk) == 0);

    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_RXSTIF_Msk) == USPI_PROTSTS_RXSTIF_Msk);
    USPI_CLR_PROT_INT_FLAG(pUSPIModule, USPI_PROTSTS_RXSTIF_Msk);
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_RXSTIF_Msk) == 0);

    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_RXENDIF_Msk) == USPI_PROTSTS_RXENDIF_Msk);
    USPI_CLR_PROT_INT_FLAG(pUSPIModule, USPI_PROTSTS_RXENDIF_Msk);
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_RXENDIF_Msk) == 0);

    pUSPIModule->TXDAT = 0x1113;

    while ((pUSPIModule->PROTSTS & USPI_PROTSTS_BUSY_Msk) != 0)
    {
        __NOP();
    }

    pUSPIModule->TXDAT = 0x1114;

    while ((pUSPIModule->PROTSTS & USPI_PROTSTS_BUSY_Msk) != 0)
    {
        __NOP();
    }

    CU_ASSERT((USPI_GET_BUF_STATUS(pUSPIModule) & USPI_BUFSTS_RXOVIF_Msk) == USPI_BUFSTS_RXOVIF_Msk);
    USPI_CLR_BUF_INT_FLAG(pUSPIModule, USPI_BUFSTS_RXOVIF_Msk);
    CU_ASSERT((USPI_GET_BUF_STATUS(pUSPIModule) & USPI_BUFSTS_RXOVIF_Msk) == 0);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
    __NOP();

    InitUSPIxModulePIN(GetUSPIModuleIdx());

    /* Slave selection signal is low-active; set slave time-out period. */
    pUSPIModule->CTL = 1;
    pUSPIModule->CTLIN0 = 1 << USPI_CTLIN0_ININV_Pos;
    pUSPIModule->PROTCTL = (pUSPIModule->PROTCTL & (~USPI_PROTCTL_SLVTOCNT_Msk)) | (50 << USPI_PROTCTL_SLVTOCNT_Pos);

    /* Slave mode, USPI mode 0. */
    pUSPIModule->LINECTL = (0 << USPI_LINECTL_DWIDTH_Pos) | USPI_LINECTL_LSB_Msk;
    pUSPIModule->PROTCTL |= (USPI_PROTCTL_PROTEN_Msk |
                             (0 << USPI_PROTCTL_SCLKMODE_Pos) |
                             USPI_PROTCTL_SLAVE_Msk);
    pUSPIModule->BRGEN |= (1 << USPI_BRGEN_TMCNTEN_Pos);

    for (u32DelayCount = 0; u32DelayCount < 10; u32DelayCount++)
    {
        __NOP();
    }

    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_SSACTIF_Msk) == 0);
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_SSINAIF_Msk) == 0);
    CU_ASSERT((USPI_GET_BUF_STATUS(pUSPIModule) & USPI_BUFSTS_TXUDRIF_Msk) == 0);

    USPIxCtlLow(GetUSPIModuleIdx());

    for (u32DelayCount = 0; u32DelayCount < 10; u32DelayCount++)
    {
        __NOP();
    }

    /* Check slave selection signal active interrupt flag */
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_SSACTIF_Msk) == USPI_PROTSTS_SSACTIF_Msk);
    USPI_CLR_PROT_INT_FLAG(pUSPIModule, USPI_PROTSTS_SSACTIF_Msk);
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_SSACTIF_Msk) == 0);
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_SSINAIF_Msk) == 0);
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_SLVTOIF_Msk) == 0);
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_SLVBEIF_Msk) == 0);

    /* Check TX under run interrupt flag */
    CU_ASSERT((USPI_GET_BUF_STATUS(pUSPIModule) & USPI_BUFSTS_TXUDRIF_Msk) == USPI_BUFSTS_TXUDRIF_Msk);

    USPIxClkHigh(GetUSPIModuleIdx());

    for (u32DelayCount = 0; u32DelayCount < 10; u32DelayCount++);

    USPIxClkLow(GetUSPIModuleIdx());

    for (u32DelayCount = 0; u32DelayCount < 1000; u32DelayCount++)
    {
        __NOP();
    }

    /* Check slave time-out interrupt flag */
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_SLVTOIF_Msk) == USPI_PROTSTS_SLVTOIF_Msk);
    USPI_CLR_PROT_INT_FLAG(pUSPIModule, USPI_PROTSTS_SLVTOIF_Msk);
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_SLVTOIF_Msk) == 0);

    USPIxCtlHigh(GetUSPIModuleIdx());

    __NOP();
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_SSINAIF_Msk) == USPI_PROTSTS_SSINAIF_Msk);
    USPI_CLR_PROT_INT_FLAG(pUSPIModule, USPI_PROTSTS_SSINAIF_Msk);
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_SSINAIF_Msk) == 0);

    /* Clear TX under run interrupt flag */
    USPI_CLR_BUF_INT_FLAG(pUSPIModule, USPI_BUFSTS_TXUDRIF_Msk);
    CU_ASSERT((USPI_GET_BUF_STATUS(pUSPIModule) & USPI_BUFSTS_TXUDRIF_Msk) == 0);

    /* Slave bit count error interrupt flag test */
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_SLVBEIF_Msk) == USPI_PROTSTS_SLVBEIF_Msk);
    USPI_CLR_PROT_INT_FLAG(pUSPIModule, USPI_PROTSTS_SLVBEIF_Msk);
    CU_ASSERT((USPI_GET_PROT_STATUS(pUSPIModule) & USPI_PROTSTS_SLVBEIF_Msk) == 0);

    /* Disable self-test function */
    USPI_DisableSelfTest();

    /* Set PA.11 (USPI0_CLK) as USCI function pin */
    USPI0_CLK_PIN_INIT();

    /* Reset USPI0 */
    ResetUSPIxModule(GetUSPIModuleIdx());
    __NOP();

    /* RX overrun interrupt flag test */
    /* Select USCI_SPI0 protocol */
    pUSPIModule->CTL &= ~USPI_CTL_FUNMODE_Msk;
    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;
    /* Master mode, USPI mode 0. */
    pUSPIModule->PROTCTL = USPI_MASTER | USPI_MODE_0;
    pUSPIModule->LINECTL = 0;
    /* Set USCI_SPI0 clock rate = f_PCLK0 / 2*(359+1) */
    pUSPIModule->BRGEN = (pUSPIModule->BRGEN & (~USPI_BRGEN_CLKDIV_Msk)) | 359 << USPI_BRGEN_CLKDIV_Pos;
    /* Enable USCI_SPI0 protocol */
    pUSPIModule->PROTCTL |= USPI_PROTCTL_PROTEN_Msk;
    /* Check RX overrun interrupt flag */
    CU_ASSERT((USPI_GET_BUF_STATUS(pUSPIModule) & USPI_BUFSTS_RXOVIF_Msk) == 0);
    /* Write to TX register */
    pUSPIModule->TXDAT = 0x1115;

    while (pUSPIModule->BUFSTS & USPI_BUFSTS_TXFULL_Msk);

    pUSPIModule->TXDAT = 0x1116;

    while (pUSPIModule->BUFSTS & USPI_BUFSTS_TXFULL_Msk);

    pUSPIModule->TXDAT = 0x1117;

    /* Wait data transfer */
    while (pUSPIModule->PROTSTS & USPI_PROTSTS_BUSY_Msk)
    {
        __NOP();
    }

    /* Check RX overrun interrupt flag */
    CU_ASSERT((USPI_GET_BUF_STATUS(pUSPIModule) & USPI_BUFSTS_RXOVIF_Msk) == USPI_BUFSTS_RXOVIF_Msk);
    USPI_CLR_BUF_INT_FLAG(pUSPIModule, USPI_BUFSTS_RXOVIF_Msk);
    CU_ASSERT((USPI_GET_BUF_STATUS(pUSPIModule) & USPI_BUFSTS_RXOVIF_Msk) == 0);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void MACRO_USCI_SPI_EnableProtocolInt_DisableProtocolInt()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    /* Select USCI_SPI0 protocol */
    pUSPIModule->CTL &= ~USPI_CTL_FUNMODE_Msk;
    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;

    USPI_ENABLE_PROT_INT(pUSPIModule, USPI_PROTIEN_SSINAIEN_Msk);
    CU_ASSERT_TRUE(pUSPIModule->PROTIEN & 0x1);
    USPI_DISABLE_PROT_INT(pUSPIModule, USPI_PROTIEN_SSINAIEN_Msk);
    CU_ASSERT_FALSE(pUSPIModule->PROTIEN & 0x1);

    USPI_ENABLE_PROT_INT(pUSPIModule, USPI_PROTIEN_SSACTIEN_Msk);
    CU_ASSERT_TRUE(pUSPIModule->PROTIEN & 0x2);
    USPI_DISABLE_PROT_INT(pUSPIModule, USPI_PROTIEN_SSACTIEN_Msk);
    CU_ASSERT_FALSE(pUSPIModule->PROTIEN & 0x2);

    USPI_ENABLE_PROT_INT(pUSPIModule, USPI_PROTIEN_SLVTOIEN_Msk);
    CU_ASSERT_TRUE(pUSPIModule->PROTIEN & 0x4);
    USPI_DISABLE_PROT_INT(pUSPIModule, USPI_PROTIEN_SLVTOIEN_Msk);
    CU_ASSERT_FALSE(pUSPIModule->PROTIEN & 0x4);

    USPI_ENABLE_PROT_INT(pUSPIModule, USPI_PROTIEN_SLVBEIEN_Msk);
    CU_ASSERT_TRUE(pUSPIModule->PROTIEN & 0x8);
    USPI_DISABLE_PROT_INT(pUSPIModule, USPI_PROTIEN_SLVBEIEN_Msk);
    CU_ASSERT_FALSE(pUSPIModule->PROTIEN & 0x8);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void MACRO_USCI_SPI_EnableBufferInt_DisableBufferInt()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    /* Select USCI_SPI0 protocol */
    pUSPIModule->CTL &= ~USPI_CTL_FUNMODE_Msk;
    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;

    USPI_ENABLE_BUF_INT(pUSPIModule, USPI_BUFCTL_TXUDRIEN_Msk);
    CU_ASSERT_TRUE(pUSPIModule->BUFCTL & 0x40);
    USPI_DISABLE_BUF_INT(pUSPIModule, USPI_BUFCTL_TXUDRIEN_Msk);
    CU_ASSERT_FALSE(pUSPIModule->BUFCTL & 0x40);

    USPI_ENABLE_BUF_INT(pUSPIModule, USPI_BUFCTL_RXOVIEN_Msk);
    CU_ASSERT_TRUE(pUSPIModule->BUFCTL & 0x4000);
    USPI_DISABLE_BUF_INT(pUSPIModule, USPI_BUFCTL_RXOVIEN_Msk);
    CU_ASSERT_FALSE(pUSPIModule->BUFCTL & 0x4000);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void MACRO_USCI_SPI_EnablePDMA_DisablePDMA()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    /* Select USCI_SPI0 protocol */
    pUSPIModule->CTL &= ~USPI_CTL_FUNMODE_Msk;
    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;

    USPI_TRIGGER_RX_PDMA(pUSPIModule);
    CU_ASSERT((pUSPIModule->PDMACTL & USPI_PDMACTL_RXPDMAEN_Msk) == USPI_PDMACTL_RXPDMAEN_Msk);
    USPI_DISABLE_RX_PDMA(pUSPIModule);
    CU_ASSERT((pUSPIModule->PDMACTL & USPI_PDMACTL_RXPDMAEN_Msk) == 0);
    USPI_TRIGGER_TX_PDMA(pUSPIModule);
    CU_ASSERT((pUSPIModule->PDMACTL & USPI_PDMACTL_TXPDMAEN_Msk) == USPI_PDMACTL_TXPDMAEN_Msk);
    USPI_DISABLE_TX_PDMA(pUSPIModule);
    CU_ASSERT((pUSPIModule->PDMACTL & USPI_PDMACTL_TXPDMAEN_Msk) == 0);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void MACRO_USCI_SPI_EnableTransferInt_DisableTransferInt()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    /* Select USCI_SPI0 protocol */
    pUSPIModule->CTL &= ~USPI_CTL_FUNMODE_Msk;
    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;

    USPI_ENABLE_TRANS_INT(pUSPIModule, USPI_INTEN_TXSTIEN_Msk);
    CU_ASSERT_TRUE(pUSPIModule->INTEN & 0x2);
    USPI_DISABLE_TRANS_INT(pUSPIModule, USPI_INTEN_TXSTIEN_Msk);
    CU_ASSERT_FALSE(pUSPIModule->INTEN & 0x2);

    USPI_ENABLE_TRANS_INT(pUSPIModule, USPI_INTEN_TXENDIEN_Msk);
    CU_ASSERT_TRUE(pUSPIModule->INTEN & 0x4);
    USPI_DISABLE_TRANS_INT(pUSPIModule, USPI_INTEN_TXENDIEN_Msk);
    CU_ASSERT_FALSE(pUSPIModule->INTEN & 0x4);

    USPI_ENABLE_TRANS_INT(pUSPIModule, USPI_INTEN_RXSTIEN_Msk);
    CU_ASSERT_TRUE(pUSPIModule->INTEN & 0x8);
    USPI_DISABLE_TRANS_INT(pUSPIModule, USPI_INTEN_RXSTIEN_Msk);
    CU_ASSERT_FALSE(pUSPIModule->INTEN & 0x8);

    USPI_ENABLE_TRANS_INT(pUSPIModule, USPI_INTEN_RXENDIEN_Msk);
    CU_ASSERT_TRUE(pUSPIModule->INTEN & 0x10);
    USPI_DISABLE_TRANS_INT(pUSPIModule, USPI_INTEN_RXENDIEN_Msk);
    CU_ASSERT_FALSE(pUSPIModule->INTEN & 0x10);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void API_USCI_SPI_Open()
{
    uint32_t u32ReturnValue = 0;
    uint32_t u32Pclk = 0, u32ClkDiv = 0, u32BusClock = 0;
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    u32BusClock = 1000000;

    if (GetUSPIModuleIdx() == C_USPI0)
    {
        u32Pclk = CLK_GetPCLK0Freq();
    }

    u32ClkDiv = (uint32_t)((((((u32Pclk / 2) * 10) / (u32BusClock)) + 5) / 10) - 1);

    u32ReturnValue = USPI_Open(pUSPIModule, USPI_MASTER, USPI_MODE_0, 4, 1000000);
    CU_ASSERT(pUSPIModule->CTL == 0x00000001);
    CU_ASSERT((pUSPIModule->BRGEN & USPI_BRGEN_CLKDIV_Msk) == (u32ClkDiv << USPI_BRGEN_CLKDIV_Pos));

    CU_ASSERT(pUSPIModule->LINECTL == 0x00000480);
    CU_ASSERT(pUSPIModule->PROTCTL == 0x80000300);
    CU_ASSERT(u32ReturnValue == 1000000);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void API_USCI_SPI_Close()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    pUSPIModule->CTL = 0x00000001;
    USPI_Close(pUSPIModule);
    CU_ASSERT(pUSPIModule->CTL == 0x00000000);
}

void API_USCI_SPI_ClearRxBuf()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    /* Master mode, USPI mode 0. */
    USPI_Open(pUSPIModule, USPI_MASTER, USPI_MODE_0, 16, 100000);
    /* Write 1 data to TX register */
    pUSPIModule->TXDAT = 1;

    /* Wait data transfer */
    while ((pUSPIModule->PROTSTS & 0x00020000) != 0)
    {
        __NOP();
    }

    USPI_ClearRxBuf(pUSPIModule);
    CU_ASSERT(pUSPIModule->BUFSTS == 0x00000101);

    USPI_Close(pUSPIModule);
}

void API_USCI_SPI_ClearTxBuf()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    InitUSPIxModulePIN(GetUSPIModuleIdx());

    /* Slave mode, USPI mode 0. */
    USPI_Open(pUSPIModule, USPI_SLAVE, USPI_MODE_0, 16, NULL);

    /* Write 1 data to TX register */
    pUSPIModule->TXDAT = 1;
    CU_ASSERT((pUSPIModule->BUFSTS & 0x00000F00) == 0x00000200);
    USPI_ClearTxBuf(pUSPIModule);
    CU_ASSERT((pUSPIModule->BUFSTS & 0x00000F00) == 0x00000100);

    USPI_Close(pUSPIModule);

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Disable self-test function */
    USPI_DisableSelfTest();
}

void API_USCI_SPI_EnableAutoSS_DisableAutoSS()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    pUSPIModule->CTL = 1 << USPI_CTL_FUNMODE_Pos;
    pUSPIModule->PROTCTL |=  USPI_PROTCTL_PROTEN_Msk;

    USPI_EnableAutoSS(pUSPIModule, 0, USPI_SS_ACTIVE_HIGH);
    CU_ASSERT(pUSPIModule->PROTCTL == 0x80000308);
    CU_ASSERT(pUSPIModule->LINECTL == 0);
    USPI_DisableAutoSS(pUSPIModule);
    CU_ASSERT((pUSPIModule->PROTCTL & (USPI_PROTCTL_AUTOSS_Msk | USPI_PROTCTL_SS_Msk)) == 0x0);
    USPI_EnableAutoSS(pUSPIModule, 0, USPI_SS_ACTIVE_LOW);
    CU_ASSERT(pUSPIModule->PROTCTL == 0x80000308);
    CU_ASSERT(pUSPIModule->LINECTL == 0x80);
    USPI_DisableAutoSS(pUSPIModule);
    CU_ASSERT((pUSPIModule->PROTCTL & (USPI_PROTCTL_AUTOSS_Msk | USPI_PROTCTL_SS_Msk)) == 0x0);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void API_USCI_SPI_SetBusClock_GetBusClock()
{
    uint32_t u32ReturnValue, u32ClkDiv, u32Pclk, u32BusClock;
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    u32BusClock = 200 * 1000;
    u32Pclk = CLK_GetPCLK0Freq();
    u32ClkDiv = (uint32_t)((((((u32Pclk / 2) * 10) / (u32BusClock)) + 5) / 10) - 1) & 0x3FF;

    u32ReturnValue = USPI_SetBusClock(pUSPIModule, 200 * 1000);
    CU_ASSERT((pUSPIModule->BRGEN & USPI_BRGEN_CLKDIV_Msk) == (u32ClkDiv << USPI_BRGEN_CLKDIV_Pos));
    CU_ASSERT(u32ReturnValue == 200 * 1000);
    u32ReturnValue = USPI_GetBusClock(pUSPIModule);
    CU_ASSERT(u32ReturnValue == 200 * 1000);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());

#if 0
    u32BusClock = 200 * 1000;
    u32Pclk = CLK_GetPCLK0Freq();
    u32ClkDiv = (uint32_t)((((((u32Pclk / 2) * 10) / (u32BusClock)) + 5) / 10) - 1) & 0x3FF;

    u32ReturnValue = USPI_SetBusClock(USPI0, 200 * 1000);
    CU_ASSERT((USPI0->BRGEN & USPI_BRGEN_CLKDIV_Msk) == (u32ClkDiv << USPI_BRGEN_CLKDIV_Pos));
    CU_ASSERT(u32ReturnValue == 200 * 1000);
    u32ReturnValue = USPI_GetBusClock(USPI0);
    CU_ASSERT(u32ReturnValue == 200 * 1000);
    /* Reset USPI0 */
    SYS->IPRST2 |= SYS_IPRST2_USCI0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_USCI0RST_Msk;

#if defined EN_MULTI_USPI1
    u32BusClock = 1 * 1000 * 1000;
    u32Pclk = CLK_GetPCLK1Freq();
    u32ClkDiv = (uint32_t)((((((u32Pclk / 2) * 10) / (u32BusClock)) + 5) / 10) - 1) & 0x3FF;

    u32ReturnValue = USPI_SetBusClock(USPI1, 1 * 1000 * 1000);
    CU_ASSERT((USPI1->BRGEN & USPI_BRGEN_CLKDIV_Msk) == (u32ClkDiv << USPI_BRGEN_CLKDIV_Pos));
    CU_ASSERT(u32ReturnValue == 1 * 1000 * 1000);
    u32ReturnValue = USPI_GetBusClock(USPI1);
    CU_ASSERT(u32ReturnValue == 1 * 1000 * 1000);
    /* Reset USPI1 */
    SYS->IPRST2 |= SYS_IPRST2_USCI1RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_USCI1RST_Msk;
#endif //EN_MULTI_USPI1

#if defined EN_MULTI_USPI2
    u32ReturnValue = USPI_SetBusClock(USPI2, 72 * 1000 * 1000);
    CU_ASSERT(USPI2->BRGEN == 0x00003C00);
    CU_ASSERT(u32ReturnValue == 36 * 1000 * 1000);
    u32ReturnValue = USPI_GetBusClock(USPI2);
    CU_ASSERT(u32ReturnValue == 36 * 1000 * 1000);
    /* Reset USPI2 */
    SYS->IPRST2 |= SYS_IPRST2_USCI2RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_USCI2RST_Msk;
#endif //EN_MULTI_USPI2
#endif //0
}

void API_USCI_SPI_EnableInt_DisableInt()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    USPI_Open(pUSPIModule, USPI_MASTER, USPI_MODE_0, 4, 100000);

    USPI_EnableInt(pUSPIModule, USPI_SSINACT_INT_MASK);
    CU_ASSERT_TRUE(pUSPIModule->PROTIEN & 0x1);
    USPI_DisableInt(pUSPIModule, USPI_SSINACT_INT_MASK);
    CU_ASSERT_FALSE(pUSPIModule->PROTIEN & 0x1);

    USPI_EnableInt(pUSPIModule, USPI_SSACT_INT_MASK);
    CU_ASSERT_TRUE(pUSPIModule->PROTIEN & 0x2);
    USPI_DisableInt(pUSPIModule, USPI_SSACT_INT_MASK);
    CU_ASSERT_FALSE(pUSPIModule->PROTIEN & 0x2);

    USPI_EnableInt(pUSPIModule, USPI_SLVTO_INT_MASK);
    CU_ASSERT_TRUE(pUSPIModule->PROTIEN & 0x4);
    USPI_DisableInt(pUSPIModule, USPI_SLVTO_INT_MASK);
    CU_ASSERT_FALSE(pUSPIModule->PROTIEN & 0x4);

    USPI_EnableInt(pUSPIModule, USPI_SLVBE_INT_MASK);
    CU_ASSERT_TRUE(pUSPIModule->PROTIEN & 0x8);
    USPI_DisableInt(pUSPIModule, USPI_SLVBE_INT_MASK);
    CU_ASSERT_FALSE(pUSPIModule->PROTIEN & 0x8);

    USPI_EnableInt(pUSPIModule, USPI_TXUDR_INT_MASK);
    CU_ASSERT_TRUE(pUSPIModule->BUFCTL & 0x40);
    USPI_DisableInt(pUSPIModule, USPI_TXUDR_INT_MASK);
    CU_ASSERT_FALSE(pUSPIModule->BUFCTL & 0x40);

    USPI_EnableInt(pUSPIModule, USPI_RXOV_INT_MASK);
    CU_ASSERT_TRUE(pUSPIModule->BUFCTL & 0x4000);
    USPI_DisableInt(pUSPIModule, USPI_RXOV_INT_MASK);
    CU_ASSERT_FALSE(pUSPIModule->BUFCTL & 0x4000);

    USPI_EnableInt(pUSPIModule, USPI_TXST_INT_MASK);
    CU_ASSERT_TRUE(pUSPIModule->INTEN & 0x2);
    USPI_DisableInt(pUSPIModule, USPI_TXST_INT_MASK);
    CU_ASSERT_FALSE(pUSPIModule->INTEN & 0x2);

    USPI_EnableInt(pUSPIModule, USPI_TXEND_INT_MASK);
    CU_ASSERT_TRUE(pUSPIModule->INTEN & 0x4);
    USPI_DisableInt(pUSPIModule, USPI_TXEND_INT_MASK);
    CU_ASSERT_FALSE(pUSPIModule->INTEN & 0x4);

    USPI_EnableInt(pUSPIModule, USPI_RXST_INT_MASK);
    CU_ASSERT_TRUE(pUSPIModule->INTEN & 0x8);
    USPI_DisableInt(pUSPIModule, USPI_RXST_INT_MASK);
    CU_ASSERT_FALSE(pUSPIModule->INTEN & 0x8);

    USPI_EnableInt(pUSPIModule, USPI_RXEND_INT_MASK);
    CU_ASSERT_TRUE(pUSPIModule->INTEN & 0x10);
    USPI_DisableInt(pUSPIModule, USPI_RXEND_INT_MASK);
    CU_ASSERT_FALSE(pUSPIModule->INTEN & 0x10);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
}

void API_USCI_SPI_GetIntFlag_ClearIntFlag()
{
    uint32_t u32DelayCount;
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    /* Master mode, USPI mode 0. */
    USPI_Open(pUSPIModule, USPI_MASTER, USPI_MODE_0, 8, 1000000);
    pUSPIModule->TXDAT = 1;

    while ((pUSPIModule->PROTSTS & USPI_PROTSTS_BUSY_Msk) != 0)
    {
        __NOP();
    }

    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_TXST_INT_MASK) == USPI_TXST_INT_MASK);
    USPI_ClearIntFlag(pUSPIModule, USPI_TXST_INT_MASK);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_TXST_INT_MASK) == 0);

    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_TXEND_INT_MASK) == USPI_TXEND_INT_MASK);
    USPI_ClearIntFlag(pUSPIModule, USPI_TXEND_INT_MASK);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_TXEND_INT_MASK) == 0);

    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_RXST_INT_MASK) == USPI_RXST_INT_MASK);
    USPI_ClearIntFlag(pUSPIModule, USPI_RXST_INT_MASK);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_RXST_INT_MASK) == 0);

    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_RXEND_INT_MASK) == USPI_RXEND_INT_MASK);
    USPI_ClearIntFlag(pUSPIModule, USPI_RXEND_INT_MASK);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_RXEND_INT_MASK) == 0);

    pUSPIModule->TXDAT = 1;

    while ((pUSPIModule->PROTSTS & USPI_PROTSTS_BUSY_Msk) != 0)
    {
        __NOP();
    }

    pUSPIModule->TXDAT = 2;

    while ((pUSPIModule->PROTSTS & USPI_PROTSTS_BUSY_Msk) != 0)
    {
        __NOP();
    }

    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_RXOV_INT_MASK) == USPI_RXOV_INT_MASK);
    USPI_ClearIntFlag(pUSPIModule, USPI_RXOV_INT_MASK);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_RXOV_INT_MASK) == 0);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
    __NOP();

    InitUSPIxModulePIN(GetUSPIModuleIdx());

    /* Slave selection signal is low-active; set slave time-out period. */
    pUSPIModule->CTL = 1;
    pUSPIModule->CTLIN0 = 1 << USPI_CTLIN0_ININV_Pos;
    pUSPIModule->PROTCTL = (pUSPIModule->PROTCTL & (~USPI_PROTCTL_SLVTOCNT_Msk)) |
                           (50 << USPI_PROTCTL_SLVTOCNT_Pos);

    /* Slave mode, USPI mode 0. */
    pUSPIModule->LINECTL = (0 << USPI_LINECTL_DWIDTH_Pos) | USPI_LINECTL_LSB_Msk;
    pUSPIModule->PROTCTL |= (USPI_PROTCTL_PROTEN_Msk |
                             (0 << USPI_PROTCTL_SCLKMODE_Pos) |
                             USPI_PROTCTL_SLAVE_Msk);
    pUSPIModule->BRGEN |= (1 << USPI_BRGEN_TMCNTEN_Pos);

    for (u32DelayCount = 0; u32DelayCount < 10; u32DelayCount++)
    {
        __NOP();
    }

    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_SSACT_INT_MASK) == 0);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_SSINACT_INT_MASK) == 0);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_TXUDR_INT_MASK) == 0);

    USPIxCtlLow(GetUSPIModuleIdx());

    for (u32DelayCount = 0; u32DelayCount < 10; u32DelayCount++)
    {
        __NOP();
    }

    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_SSACT_INT_MASK) == USPI_SSACT_INT_MASK);
    USPI_ClearIntFlag(pUSPIModule, USPI_SSACT_INT_MASK);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_SSACT_INT_MASK) == 0);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_SSINACT_INT_MASK) == 0);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_SLVTO_INT_MASK) == 0);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_SLVBE_INT_MASK) == 0);

    /* Check TX BUF under run interrupt flag */
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_TXUDR_INT_MASK) == USPI_TXUDR_INT_MASK);

    USPIxClkHigh(GetUSPIModuleIdx());

    for (u32DelayCount = 0; u32DelayCount < 10; u32DelayCount++);

    USPIxClkLow(GetUSPIModuleIdx());

    for (u32DelayCount = 0; u32DelayCount < 1000; u32DelayCount++)
    {
        __NOP();
    }

    /* Check slave time-out interrupt flag */
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_SLVTO_INT_MASK) == USPI_SLVTO_INT_MASK);
    USPI_ClearIntFlag(pUSPIModule, USPI_SLVTO_INT_MASK);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_SLVTO_INT_MASK) == 0);

    USPIxCtlHigh(GetUSPIModuleIdx());

    __NOP();
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_SSINACT_INT_MASK) == USPI_SSINACT_INT_MASK);
    USPI_ClearIntFlag(pUSPIModule, USPI_SSINACT_INT_MASK);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_SSINACT_INT_MASK) == 0);

    /* Clear TX BUF under run interrupt flag */
    USPI_ClearIntFlag(pUSPIModule, USPI_TXUDR_INT_MASK);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_TXUDR_INT_MASK) == 0);

    /* Slave bit count error interrupt flag test */
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_SLVBE_INT_MASK) == USPI_SLVBE_INT_MASK);
    USPI_ClearIntFlag(pUSPIModule, USPI_SLVBE_INT_MASK);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_SLVBE_INT_MASK) == 0);


    /* Disable self-test function */
    USPI_DisableSelfTest();

    /* Set PA.11 (USPI0_CLK) as USCI function pin */
    USPI0_CLK_PIN_INIT();

    /* Lock protected registers */
    SYS_LockReg();

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
    __NOP();

    /* RX overrun interrupt flag test */
    /* Master mode, USPI mode 0. */
    USPI_Open(pUSPIModule, USPI_MASTER, USPI_MODE_0, 16, 100000);
    /* Check RX overrun interrupt flag */
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_RXOV_INT_MASK) == 0);
    /* Write to TX register */
    pUSPIModule->TXDAT = 1;

    while (pUSPIModule->BUFSTS & USPI_BUFSTS_TXFULL_Msk);

    pUSPIModule->TXDAT = 2;

    while (pUSPIModule->BUFSTS & USPI_BUFSTS_TXFULL_Msk);

    pUSPIModule->TXDAT = 3;

    /* Wait data transfer */
    while (pUSPIModule->PROTSTS & USPI_PROTSTS_BUSY_Msk)
    {
        __NOP();
    }

    /* Check RX overrun interrupt flag */
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_RXOV_INT_MASK) == USPI_RXOV_INT_MASK);
    USPI_ClearIntFlag(pUSPIModule, USPI_RXOV_INT_MASK);
    CU_ASSERT(USPI_GetIntFlag(pUSPIModule, USPI_RXOV_INT_MASK) == 0);

    USPI_Close(pUSPIModule);
}

void API_USCI_SPI_GetStatus()
{
    volatile uint32_t u32DelayCount = 0;
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    /* Slave mode, USPI mode 0. */
    USPI_Open(pUSPIModule, USPI_SLAVE, USPI_MODE_0, 16, NULL);
    /* Check busy flag */
    CU_ASSERT(USPI_GetStatus(pUSPIModule, USPI_BUSY_MASK) == 0);

    /* USCI_SPI TX empty flag test */
    /* Check TX empty flag */
    CU_ASSERT(USPI_GetStatus(pUSPIModule, USPI_TX_EMPTY_MASK) == USPI_TX_EMPTY_MASK);
    /* Write to TX register */
    pUSPIModule->TXDAT = 1;
    /* Check TX empty flag */
    CU_ASSERT(USPI_GetStatus(pUSPIModule, USPI_TX_EMPTY_MASK) == 0);

    /* Check busy flag */
    CU_ASSERT(USPI_GetStatus(pUSPIModule, USPI_BUSY_MASK) == USPI_BUSY_MASK);

    /* USCI_SPI TX full flag test */
    pUSPIModule->BUFCTL |= USPI_BUFCTL_TXCLR_Msk;
    /* Check TX full flag */
    CU_ASSERT(USPI_GetStatus(pUSPIModule, USPI_TX_FULL_MASK) == 0);
    /* Write to TX register */
    pUSPIModule->TXDAT = 2;
    /* Check TX full flag */
    CU_ASSERT(USPI_GetStatus(pUSPIModule, USPI_TX_FULL_MASK) == USPI_TX_FULL_MASK);
    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());
    __NOP();

    /* USCI_SPI RX empty flag test */
    /* Master mode, USPI mode 0. */
    USPI_Open(pUSPIModule, USPI_MASTER, USPI_MODE_0, 16, 100000);
    /* Check RX empty flag */
    CU_ASSERT(USPI_GetStatus(pUSPIModule, USPI_RX_EMPTY_MASK) == USPI_RX_EMPTY_MASK);
    /* Write to TX register */
    pUSPIModule->TXDAT = 1;

    /* Wait data transfer */
    while (pUSPIModule->PROTSTS & USPI_PROTSTS_BUSY_Msk)
    {
        __NOP();
    }

    /* Check RX empty flag */
    CU_ASSERT(USPI_GetStatus(pUSPIModule, USPI_RX_EMPTY_MASK) == 0);

    /* USCI_SPI RX full flag test */
    /* Check RX full flag */
    CU_ASSERT(USPI_GetStatus(pUSPIModule, USPI_RX_FULL_MASK) == 0);
    /* Write to TX register */
    pUSPIModule->TXDAT = 2;

    while (pUSPIModule->BUFSTS & USPI_BUFSTS_TXFULL_Msk);

    pUSPIModule->TXDAT = 3;

    /* Wait data transfer */
    while (pUSPIModule->PROTSTS & USPI_PROTSTS_BUSY_Msk)
    {
        __NOP();
    }

    /* Check RX full flag */
    CU_ASSERT(USPI_GetStatus(pUSPIModule, USPI_RX_FULL_MASK) == USPI_RX_FULL_MASK);

    /* Reset USPIx */
    ResetUSPIxModule(GetUSPIModuleIdx());

    __NOP();

    InitUSPIxModulePIN(GetUSPIModuleIdx());

    /* Slave mode, USPI mode 0. */
    USPI_Open(pUSPIModule, USPI_SLAVE, USPI_MODE_0, 16, NULL);

    /* Slave selection signal is low-active. */
    pUSPIModule->CTLIN0 |= USPI_CTLIN0_ININV_Msk;

    //pUSPIModule->CTLIN0 = (pUSPIModule->CTLIN0 & (~USPI_CTLIN0_ININV_Msk)) | USPI_CTLIN0_ININV_Msk;
    for (u32DelayCount = 0; u32DelayCount < 10; u32DelayCount++)
    {
        __NOP();
    }

    CU_ASSERT(USPI_GetStatus(pUSPIModule, USPI_SSLINE_STS_MASK) == USPI_SSLINE_STS_MASK);

    USPIxCtlLow(GetUSPIModuleIdx());

    for (u32DelayCount = 0; u32DelayCount < 100; u32DelayCount++)
    {
        __NOP();
    }

    CU_ASSERT(USPI_GetStatus(pUSPIModule, USPI_SSLINE_STS_MASK) == 0);
    __NOP();

    /* Disable self-test function */
    USPI_DisableSelfTest();

    USPI_Close(pUSPIModule);
}

#ifdef POWER_DOWN_TEST
void USCI_IRQHandler(void)
{
    NVIC_DisableIRQ(USCI_IRQn);
}

void API_MACRO_USCI_SPI_Wakeup()
{
    /* Reset USPI1 */
    SYS->IPRST2 |= SYS_IPRST2_USCI1RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_USCI1RST_Msk;

    /* Master mode, USPI mode 0. */
    USPI_Open(USPI1, USPI_MASTER, USPI_MODE_0, 16, 100000);

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable power wake-up interrupt */
    CLK->PWRCTL |= CLK_PWRCTL_PDWKIEN_Msk;

    NVIC_EnableIRQ(USCI_IRQn);
    //NVIC_EnableIRQ(PWRWU_IRQn);

    /* Enable SPI wake-up */
    USPI_EnableWakeup(USPI1);
    CU_ASSERT((USPI1->WKCTL & USPI_WKCTL_WKEN_Msk) == USPI_WKCTL_WKEN_Msk);

    /* Processor use deep sleep */
    SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;

    /* System power down enable */
    CLK->PWRCTL |= CLK_PWRCTL_PDEN_Msk;

    printf("\n");
    printf("Enter PD 0x%x 0x%x\n", USPI1->PROTCTL, USPI1->PROTSTS);
    printf("\n");
    printf("CHIP enter power down status.\n");

    /* Waiting for UART printf finish*/
    while (!(((UUART0->BUFSTS) & UUART_BUFSTS_TXEMPTY_Msk) >> UUART_BUFSTS_TXEMPTY_Pos))
        if (USPI1->PROTSTS != 0)
        {
            USPI1->PROTSTS = USPI1->PROTSTS;
        }

    /*  Use WFI instruction to idle the CPU. NOTE:
    If ICE is attached, system will wakeup immediately because ICE is a wakeup event. */
    __WFI();
    __NOP();
    __NOP();
    __NOP();

    /* Disable power wake-up interrupt */
    CLK->PWRCTL &= ~CLK_PWRCTL_PDWKIEN_Msk;

    while (!USPI_GET_WAKEUP_FLAG(USPI1));

    CU_ASSERT(USPI_GET_WAKEUP_FLAG(USPI1) == 1);
    USPI_CLR_WAKEUP_FLAG(USPI1);
    CU_ASSERT(USPI_GET_WAKEUP_FLAG(USPI1) == 0);

    /* Lock protected registers */
    SYS_LockReg();

    USPI_DisableWakeup(USPI1);
    CU_ASSERT((USPI1->WKCTL & USPI_WKCTL_WKEN_Msk) == 0);
    USPI_Close(USPI1);

    printf("Wake up!!\n");
}
#else
void API_MACRO_USCI_SPI_Wakeup()
{
    USPI_T *pUSPIModule = NULL;

    pUSPIModule = (USPI_T *)GetUSPIModule(GetUSPIModuleIdx());

    USPI_EnableWakeup(pUSPIModule);
    CU_ASSERT((pUSPIModule->WKCTL & USPI_WKCTL_WKEN_Msk) == USPI_WKCTL_WKEN_Msk);

    USPI_DisableWakeup(pUSPIModule);
    CU_ASSERT((pUSPIModule->WKCTL & USPI_WKCTL_WKEN_Msk) == 0);

#if 0
    USPI_EnableWakeup(USPI0);
    CU_ASSERT((USPI0->WKCTL & USPI_WKCTL_WKEN_Msk) == USPI_WKCTL_WKEN_Msk);

    USPI_DisableWakeup(USPI0);
    CU_ASSERT((USPI0->WKCTL & USPI_WKCTL_WKEN_Msk) == 0);
#endif //0
}
#endif

CU_TestInfo USCI_SPI_MacroTests[] =
{
    {"Enable/Disable USCI_SPI slave 3-wire mode", MACRO_USCI_SPI_ENABLE_DISABLE_3WIRE_MODE},
    {"Get USCI_SPI RX buffer empty flag", MACRO_USCI_SPI_GET_RX_EMPTY_FLAG},
    {"Get USCI_SPI TX buffer empty and full flag", MACRO_USCI_SPI_GET_TX_EMPTY_FULL_FLAG},
    {"Read USCI_SPI RX and write USCI_SPI TX", MACRO_USCI_SPI_WRITE_TX_READ_RX},
    {"Set USCI_SPI SS pin to high state", MACRO_USCI_SPI_SET_SS_HIGH},
    {"Set USCI_SPI SS pin to low state", MACRO_USCI_SPI_SET_SS_LOW},
    {"Set USCI_SPI suspend cycle", MACRO_USCI_SPI_SET_SUSPEND_CYCLE},
    {"Set USCI_SPI transfer sequence with LSB/MSB first", MACRO_USCI_SPI_SET_LSB_MSB_FIRST},
    {"Set USCI_SPI data width", MACRO_USCI_SPI_SET_DATA_WIDTH},
    {"Get USCI_SPI busy flag", MACRO_USCI_SPI_IS_BUSY},
    {"Get/Clear USCI_SPI Protocol/Buffer interrupt flag", MACRO_USCI_SPI_GetIntFlag_ClearIntFlag}, //Independence test
    {"Enable/Disable USCI_SPI Protocol Interrupt", MACRO_USCI_SPI_EnableProtocolInt_DisableProtocolInt},
    {"Enable/Disable USCI_SPI Buffer Interrupt", MACRO_USCI_SPI_EnableBufferInt_DisableBufferInt},
    {"Enable/Disable USCI_SPI Transfer Interrupt", MACRO_USCI_SPI_EnableTransferInt_DisableTransferInt},
    {"Enable/Disable USCI_SPI PDMAt", MACRO_USCI_SPI_EnablePDMA_DisablePDMA},
    CU_TEST_INFO_NULL,
};

CU_TestInfo USCI_SPI_ApiTests[] =
{
    {"USCI_SPI Open", API_USCI_SPI_Open},
    {"USCI_SPI Close", API_USCI_SPI_Close},
    {"Clear USCI_SPI RX buffer", API_USCI_SPI_ClearRxBuf},
    {"Clear USCI_SPI TX buffer", API_USCI_SPI_ClearTxBuf},
    {"Enable/Disable USCI_SPI automatic slave selection function", API_USCI_SPI_EnableAutoSS_DisableAutoSS},
    {"Set/Get USCI_SPI bus clock rate", API_USCI_SPI_SetBusClock_GetBusClock},
    {"Enable/Disable USCI_SPI interrupt function", API_USCI_SPI_EnableInt_DisableInt},
    {"Get/Clear USCI_SPI interrupt flag", API_USCI_SPI_GetIntFlag_ClearIntFlag},
    {"Get USCI_SPI status", API_USCI_SPI_GetStatus},
    CU_TEST_INFO_NULL,
};

CU_TestInfo USCI_SPI_Wakeup_APIMacroTests[] =
{
    {"USCI_SPI wake-up function", API_MACRO_USCI_SPI_Wakeup},
    CU_TEST_INFO_NULL,
};

CU_SuiteInfo suites[] =
{
    {"USCI_SPI MACRO", USCI_SPI_Tests_Init, USCI_SPI_Tests_Clean, NULL, NULL, USCI_SPI_MacroTests},
    {"USCI_SPI API", USCI_SPI_Tests_Init, USCI_SPI_Tests_Clean, NULL, NULL, USCI_SPI_ApiTests},
    {"USCI_SPI API_MACRO", USCI_SPI_Tests_Init, USCI_SPI_Tests_Clean, NULL, NULL, USCI_SPI_Wakeup_APIMacroTests},
    CU_SUITE_INFO_NULL
};

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
