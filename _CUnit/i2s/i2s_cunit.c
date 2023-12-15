/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.           */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   i2s_cunit.c                                                                                           */
/*            The test function of I2S for CUnit.                                                          */
/* Project:   NUC123                                                                                       */
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
#include "i2s_cunit.h"
#include "common.h"

//------------------------------------------------------------------------------
extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;

//------------------------------------------------------------------------------
#define C_I2S0                          0
#define C_I2S1                          1

//------------------------------------------------------------------------------
I2S_T *g_apI2sModule[] =
{
    I2S0,
    I2S1,
};

static uint32_t u32I2SModuleIdx = 0;

/*----------------------------------------------------------------------------*/
/* Test function                                                              */
/*----------------------------------------------------------------------------*/
uint32_t GetI2SModuleIdx(void)
{
    return u32I2SModuleIdx;
}

void SetI2SModuleIdx(uint32_t u32SetValue)
{
    u32I2SModuleIdx = u32SetValue;
}

void *GetI2SModuel(uint32_t u32Idx)
{
    return g_apI2sModule[u32Idx];
}

void ResetI2SModule(I2S_T *i2s)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    if (i2s == I2S0)
    {
        SYS->I2SRST |= SYS_I2SRST_I2S0RST_Msk;
        SYS->I2SRST &= ~(SYS_I2SRST_I2S0RST_Msk);
    }
    else if (i2s == I2S1)
    {
        SYS->I2SRST |= SYS_I2SRST_I2S1RST_Msk;
        SYS->I2SRST &= ~(SYS_I2SRST_I2S1RST_Msk);
    }

    /* Lock protected registers */
    //SYS_LockReg();
}

int I2S_Tests_Init(void)
{
    return 0;
}

int I2S_Tests_Clean(void)
{
    return 0;
}

void I2S_ModuleInit(uint32_t u32I2SModule)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    switch (u32I2SModule)
    {
        case C_I2S0:
            CLK_EnableModuleClock(I2S0_MODULE);
            break;

        case C_I2S1:
            CLK_EnableModuleClock(I2S1_MODULE);
            break;
    }
}

int I2S_ModuleSelect(void)
{
    uint32_t u32Index = 0;
    S_TestOption sTestPort[] =
    {
        {"I2S0"},
        {"I2S1"},
    };

    u32Index = GetRequireOptions(sTestPort, sizeof(sTestPort) / sizeof(sTestPort[0]));

    SetI2SModuleIdx(u32Index);
    I2S_ModuleInit(u32Index);

    return (int)u32Index;
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
void MACRO_I2S_ENABLE_DISABLE_TX_ZCD()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    I2S_ENABLE_TX_ZCD(pI2SModule, 1);

    CU_ASSERT_TRUE(pI2SModule->CTL1 & 0x00000001);

    I2S_DISABLE_TX_ZCD(pI2SModule, 2);

    CU_ASSERT_FALSE(pI2SModule->CTL1 & 0x00000002);

    I2S_ENABLE_TX_ZCD(pI2SModule, 3);

    CU_ASSERT_TRUE(pI2SModule->CTL1 & 0x00000004);

    I2S_DISABLE_TX_ZCD(pI2SModule, 4);

    CU_ASSERT_FALSE(pI2SModule->CTL1 & 0x00000008);

    I2S_ENABLE_TX_ZCD(pI2SModule, 5);

    CU_ASSERT_TRUE(pI2SModule->CTL1 & 0x00000010);

    I2S_DISABLE_TX_ZCD(pI2SModule, 6);

    CU_ASSERT_FALSE(pI2SModule->CTL1 & 0x00000020);

    I2S_ENABLE_TX_ZCD(pI2SModule, 7);

    CU_ASSERT_TRUE(pI2SModule->CTL1 & 0x00000040);

    I2S_DISABLE_TX_ZCD(pI2SModule, 8);

    CU_ASSERT_FALSE(pI2SModule->CTL1 & 0x00000080);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void MACRO_I2S_ENABLE_DISABLE_TXDMA()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    I2S_ENABLE_TXDMA(pI2SModule);

    CU_ASSERT_TRUE(pI2SModule->CTL0 & 0x00100000);

    I2S_DISABLE_TXDMA(pI2SModule);

    CU_ASSERT_FALSE(pI2SModule->CTL0 & 0x00100000);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void MACRO_I2S_ENABLE_DISABLE_RXDMA()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    I2S_ENABLE_RXDMA(pI2SModule);

    CU_ASSERT_TRUE(pI2SModule->CTL0 & 0x00200000);

    I2S_DISABLE_RXDMA(pI2SModule);

    CU_ASSERT_FALSE(pI2SModule->CTL0 & 0x00200000);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void MACRO_I2S_ENABLE_DISABLE_TX()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    I2S_ENABLE_TX(pI2SModule);

    CU_ASSERT_TRUE(pI2SModule->CTL0 & 2);

    I2S_DISABLE_TX(pI2SModule);

    CU_ASSERT_FALSE(pI2SModule->CTL0 & 2);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void MACRO_I2S_ENABLE_DISABLE_RX()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    I2S_ENABLE_RX(pI2SModule);

    CU_ASSERT_TRUE(pI2SModule->CTL0 & 4);

    I2S_DISABLE_RX(pI2SModule);

    CU_ASSERT_FALSE(pI2SModule->CTL0 & 4);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void MACRO_I2S_ENABLE_DISABLE_TX_MUTE()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());
    I2S_ENABLE_TX_MUTE(pI2SModule);

    CU_ASSERT_TRUE(pI2SModule->CTL0 & 8);

    I2S_DISABLE_TX_MUTE(pI2SModule);

    CU_ASSERT_FALSE(pI2SModule->CTL0 & 8);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void MACRO_I2S_CLR_TX_FIFO()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    /* Write 3 data to TX FIFO */
    I2S_WRITE_TX_FIFO(pI2SModule, 1);
    I2S_WRITE_TX_FIFO(pI2SModule, 2);
    I2S_WRITE_TX_FIFO(pI2SModule, 3);

    I2S_CLR_TX_FIFO(pI2SModule);

    CU_ASSERT(I2S_GET_TX_FIFO_LEVEL(pI2SModule) == 0);
    /* CLR_TXFIFO bit should be cleared by hardware automatically */
    CU_ASSERT_FALSE(pI2SModule->CTL0 & 0x00040000);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void MACRO_I2S_CLR_RX_FIFO()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    /* Enable I2S RX function */
    pI2SModule->CTL0 = 5;

    while (pI2SModule->STATUS0 & I2S_STATUS0_RXEMPTY_Msk);

    /* Disable I2S RX function */
    pI2SModule->CTL0 = 0;

    I2S_CLR_RX_FIFO(pI2SModule);

    CU_ASSERT(I2S_GET_RX_FIFO_LEVEL(pI2SModule) == 0);
    /* CLR_RXFIFO bit should be cleared by hardware automatically */
    CU_ASSERT_FALSE(pI2SModule->CTL0 & 0x00080000);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void MACRO_I2S_SET_MONO_RX_CHANNEL()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    I2S_SET_MONO_RX_CHANNEL(pI2SModule, I2S_MONO_LEFT);

    CU_ASSERT_TRUE(pI2SModule->CTL0 & 0x00800000);

    I2S_SET_MONO_RX_CHANNEL(pI2SModule, I2S_MONO_RIGHT);

    CU_ASSERT_FALSE(pI2SModule->CTL0 & 0x00800000);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void MACRO_I2S_WRITE_TX_FIFO()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    /* Write 2 data to TX FIFO */
    I2S_WRITE_TX_FIFO(pI2SModule, 1);
    I2S_WRITE_TX_FIFO(pI2SModule, 2);

    CU_ASSERT(I2S_GET_TX_FIFO_LEVEL(pI2SModule) == 2);

    I2S_WRITE_TX_FIFO(pI2SModule, 3);

    CU_ASSERT(I2S_GET_TX_FIFO_LEVEL(pI2SModule) == 3);

    I2S_CLR_TX_FIFO(pI2SModule);
    CU_ASSERT(I2S_GET_TX_FIFO_LEVEL(pI2SModule) == 0);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void MACRO_I2S_READ_RX_FIFO()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0x8000;
    /* Enable I2S RX function */
    pI2SModule->CTL0 = 5;

    while ((pI2SModule->STATUS1 & 0x000F0000) != 0x00030000);

    /* Disable I2S RX function */
    pI2SModule->CTL0 = 0;

    I2S_READ_RX_FIFO(pI2SModule);

    CU_ASSERT(I2S_GET_RX_FIFO_LEVEL(pI2SModule) == 2);

    I2S_READ_RX_FIFO(pI2SModule);

    CU_ASSERT(I2S_GET_RX_FIFO_LEVEL(pI2SModule) == 1);

    I2S_READ_RX_FIFO(pI2SModule);

    CU_ASSERT(I2S_GET_RX_FIFO_LEVEL(pI2SModule) == 0);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void MACRO_I2S_GET_CLEAR_INT_FLAG()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0x50505050;

    /* Channel-0 zero-cross flag test */
    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0x8000;
    /* Enable left channel zero-cross detection function, I2S RX function, I2S TX function and I2S controller. */
    pI2SModule->CTL1 = 0x00000001;
    pI2SModule->CTL0 = 0xC4000007;

    while (I2S_GET_RX_FIFO_LEVEL(pI2SModule) <= 8) {}

    CU_ASSERT((I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH0ZCIF_Msk) & 0x00000001) == 0x00000001);

    I2S_CLR_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH0ZCIF_Msk);

    CU_ASSERT(I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH0ZCIF_Msk) == 0);
    pI2SModule->CTL0 = 0;

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0x50505050;

    /* Channel-1 zero-cross flag test */
    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0x8000;
    /* Enable right channel zero-cross detection function, I2S RX function, I2S TX function and I2S controller. */
    pI2SModule->CTL1 = 0x00000002;
    pI2SModule->CTL0 = 0xC4000007;

    while (I2S_GET_RX_FIFO_LEVEL(pI2SModule) <= 8) {}

    CU_ASSERT((I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH1ZCIF_Msk) & 0x00000002) == 0x00000002);
    I2S_CLR_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH1ZCIF_Msk);
    CU_ASSERT(I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH1ZCIF_Msk) == 0);
    pI2SModule->CTL0 = 0;

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0x50505050;

    /* Channel-2 zero-cross flag test */
    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0x8000;
    /* Enable right channel zero-cross detection function, I2S RX function, I2S TX function and I2S controller. */
    pI2SModule->CTL1 = 0x0000000F;
    pI2SModule->CTL0 = 0xC4000007;

    while (I2S_GET_RX_FIFO_LEVEL(pI2SModule) <= 8) {}

    CU_ASSERT((I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH2ZCIF_Msk) & 0x00000004) == 0x00000004);
    I2S_CLR_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH2ZCIF_Msk);
    CU_ASSERT(I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH2ZCIF_Msk) == 0);
    pI2SModule->CTL0 = 0;

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0x50505050;

    /* Channel-3 zero-cross flag test */
    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0x8000;
    /* Enable right channel zero-cross detection function, I2S RX function, I2S TX function and I2S controller. */
    pI2SModule->CTL1 = 0x00000008;
    pI2SModule->CTL0 = 0xC4000007;

    while (I2S_GET_RX_FIFO_LEVEL(pI2SModule) <= 14) {}

    CU_ASSERT((I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH3ZCIF_Msk) & 0x00000008) == 0x00000008);
    I2S_CLR_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH3ZCIF_Msk);
    CU_ASSERT(I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH3ZCIF_Msk) == 0);
    pI2SModule->CTL0 = 0;

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;

    /* Channel-4 zero-cross flag test */
    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0x1000;
    /* Enable right channel zero-cross detection function, I2S RX function, I2S TX function and I2S controller. */
    pI2SModule->CTL1 = 0x00000010;
    pI2SModule->CTL0 = 0xC4000007;

    while (I2S_GET_RX_FIFO_LEVEL(pI2SModule) <= 14) {}

    CU_ASSERT((I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH4ZCIF_Msk) & 0x00000010) == 0x00000010);
    I2S_CLR_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH4ZCIF_Msk);
    CU_ASSERT(I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH4ZCIF_Msk) == 0);
    pI2SModule->CTL0 = 0;

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;

    /* Channel-5 zero-cross flag test */
    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0x8000;
    /* Enable right channel zero-cross detection function, I2S RX function, I2S TX function and I2S controller. */
    pI2SModule->CTL1 = 0x00000020;
    pI2SModule->CTL0 = 0xC4000007;

    while (I2S_GET_RX_FIFO_LEVEL(pI2SModule) <= 14) {}

    CU_ASSERT((I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH5ZCIF_Msk) & 0x00000020) == 0x00000020);
    I2S_CLR_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH5ZCIF_Msk);
    CU_ASSERT(I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH5ZCIF_Msk) == 0);
    pI2SModule->CTL0 = 0;

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0x50505050;

    /* Channel-6 zero-cross flag test */
    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0x8000;
    /* Enable right channel zero-cross detection function, I2S RX function, I2S TX function and I2S controller. */
    pI2SModule->CTL1 = 0x00000040;
    pI2SModule->CTL0 |= 0xC4000007;

    while (I2S_GET_RX_FIFO_LEVEL(pI2SModule) <= 14) {}

    CU_ASSERT((I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH6ZCIF_Msk) & 0x00000040) == 0x00000040);
    I2S_CLR_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH6ZCIF_Msk);
    CU_ASSERT(I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH6ZCIF_Msk) == 0);
    pI2SModule->CTL0 = 0;

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0xA0A0A0A0;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0x50505050;
    pI2SModule->TXFIFO = 0x50505050;

    /* Channel-7 zero-cross flag test */
    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0x8000;
    /* Enable right channel zero-cross detection function, I2S RX function, I2S TX function and I2S controller. */
    pI2SModule->CTL1 = 0x00000080;
    pI2SModule->CTL0 = 0xC4000007;

    while (I2S_GET_RX_FIFO_LEVEL(pI2SModule) <= 14) {}

    CU_ASSERT((I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH7ZCIF_Msk) & 0x00000080) == 0x00000080);
    I2S_CLR_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH7ZCIF_Msk);
    CU_ASSERT(I2S_GET_ZC_INT_FLAG(pI2SModule, I2S_STATUS1_CH7ZCIF_Msk) == 0);
    pI2SModule->CTL0 = 0;

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    /* TX threshold interrupt flag test */
    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0x0800;
    /* Enable I2S controller and TX function; set TX threshold as 2. */
    pI2SModule->CTL0 = 0x3;
    pI2SModule->CTL1 = 0x200;

    CU_ASSERT(I2S_GET_INT_FLAG(pI2SModule, I2S_STATUS0_TXTHIF_Msk | I2S_STATUS0_I2STXINT_Msk | I2S_STATUS0_I2SINT_Msk) == 0x00040000);
    /* Enable TX threshold interrupt function */
    pI2SModule->IEN |= 0x400;

    CU_ASSERT(I2S_GET_INT_FLAG(pI2SModule, I2S_STATUS0_TXTHIF_Msk | I2S_STATUS0_I2STXINT_Msk | I2S_STATUS0_I2SINT_Msk) == 0x00040005);

    /* Write 3 data to TX FIFO */
    I2S_WRITE_TX_FIFO(pI2SModule, 1);
    I2S_WRITE_TX_FIFO(pI2SModule, 2);
    I2S_WRITE_TX_FIFO(pI2SModule, 3);

    CU_ASSERT(I2S_GET_INT_FLAG(pI2SModule, I2S_STATUS0_TXTHIF_Msk | I2S_STATUS0_I2STXINT_Msk | I2S_STATUS0_I2SINT_Msk) == 0);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    /* TX overflow interrupt flag test */
    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0x8000;
    /* Enable I2S controller */
    pI2SModule->CTL0 = 1;

    /* Write 9 data to TX FIFO */
    I2S_WRITE_TX_FIFO(pI2SModule, 1);
    I2S_WRITE_TX_FIFO(pI2SModule, 2);
    I2S_WRITE_TX_FIFO(pI2SModule, 3);
    I2S_WRITE_TX_FIFO(pI2SModule, 4);
    I2S_WRITE_TX_FIFO(pI2SModule, 5);
    I2S_WRITE_TX_FIFO(pI2SModule, 6);
    I2S_WRITE_TX_FIFO(pI2SModule, 7);
    I2S_WRITE_TX_FIFO(pI2SModule, 8);
    I2S_WRITE_TX_FIFO(pI2SModule, 9);
    I2S_WRITE_TX_FIFO(pI2SModule, 10);
    I2S_WRITE_TX_FIFO(pI2SModule, 11);
    I2S_WRITE_TX_FIFO(pI2SModule, 12);
    I2S_WRITE_TX_FIFO(pI2SModule, 13);
    I2S_WRITE_TX_FIFO(pI2SModule, 14);
    I2S_WRITE_TX_FIFO(pI2SModule, 15);
    I2S_WRITE_TX_FIFO(pI2SModule, 16);
    I2S_WRITE_TX_FIFO(pI2SModule, 17);
    I2S_WRITE_TX_FIFO(pI2SModule, 18);
    I2S_WRITE_TX_FIFO(pI2SModule, 19);

    CU_ASSERT(I2S_GET_INT_FLAG(pI2SModule, I2S_STATUS0_TXOVIF_Msk) == 0x00020000);

    I2S_CLR_INT_FLAG(pI2SModule, I2S_STATUS0_TXOVIF_Msk);

    CU_ASSERT(I2S_GET_INT_FLAG(pI2SModule, I2S_STATUS0_TXOVIF_Msk) == 0);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    /* TX underflow interrupt flag test */
    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0x8000;
    /* Enable I2S RX function, I2S TX function and I2S controller. */
    pI2SModule->CTL0 = 0x00000007;

    while (I2S_GET_RX_FIFO_LEVEL(pI2SModule) <= 2);

    pI2SModule->CTL0 = 0;

    CU_ASSERT(I2S_GET_INT_FLAG(pI2SModule, I2S_STATUS0_TXUDIF_Msk) == 0x00010000);

    I2S_CLR_INT_FLAG(pI2SModule, I2S_STATUS0_TXUDIF_Msk);

    CU_ASSERT(I2S_GET_INT_FLAG(pI2SModule, I2S_STATUS0_TXUDIF_Msk) == 0);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    /* RX threshold interrupt flag test */
    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0xFF00;
    /* Set RX threshold as 2; enable I2S RX function and I2S controller. */
    pI2SModule->CTL1 = 0x00020000;
    pI2SModule->CTL0 = 0x00000005;

    while (I2S_GET_RX_FIFO_LEVEL(pI2SModule) <= 2) {}

    //pI2SModule->CTL = 0;

    CU_ASSERT(I2S_GET_INT_FLAG(pI2SModule, I2S_STATUS0_RXTHIF_Msk | I2S_STATUS0_I2SRXINT_Msk | I2S_STATUS0_I2SINT_Msk) == 0x00000400);

    /* Enable RX threshold interrupt function */
    pI2SModule->IEN |= 0x4;

    CU_ASSERT(I2S_GET_INT_FLAG(pI2SModule, I2S_STATUS0_RXTHIF_Msk | I2S_STATUS0_I2SRXINT_Msk | I2S_STATUS0_I2SINT_Msk) == 0x00000403);
    pI2SModule->CTL0 = 1;
    I2S_READ_RX_FIFO(pI2SModule);
    I2S_READ_RX_FIFO(pI2SModule);
    I2S_READ_RX_FIFO(pI2SModule);
    I2S_READ_RX_FIFO(pI2SModule);
    I2S_READ_RX_FIFO(pI2SModule);
    I2S_READ_RX_FIFO(pI2SModule);
    I2S_READ_RX_FIFO(pI2SModule);

    CU_ASSERT(I2S_GET_INT_FLAG(pI2SModule, I2S_STATUS0_RXTHIF_Msk | I2S_STATUS0_I2SRXINT_Msk | I2S_STATUS0_I2SINT_Msk) == 0);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    /* RX overflow interrupt flag test */
    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0x8000;
    /* Enable I2S RX function, I2S TX function and I2S controller. */
    pI2SModule->CTL0 = 0x00000007;

    while (I2S_GET_RX_FIFO_LEVEL(pI2SModule) != 8);

    /* Write 3 data to TX FIFO */
    I2S_WRITE_TX_FIFO(pI2SModule, 1);
    I2S_WRITE_TX_FIFO(pI2SModule, 2);
    I2S_WRITE_TX_FIFO(pI2SModule, 3);
    I2S_WRITE_TX_FIFO(pI2SModule, 4);
    I2S_WRITE_TX_FIFO(pI2SModule, 5);
    I2S_WRITE_TX_FIFO(pI2SModule, 6);
    I2S_WRITE_TX_FIFO(pI2SModule, 7);
    I2S_WRITE_TX_FIFO(pI2SModule, 8);
    I2S_WRITE_TX_FIFO(pI2SModule, 9);
    I2S_WRITE_TX_FIFO(pI2SModule, 10);
    I2S_WRITE_TX_FIFO(pI2SModule, 11);
    I2S_WRITE_TX_FIFO(pI2SModule, 12);
    I2S_WRITE_TX_FIFO(pI2SModule, 13);
    I2S_WRITE_TX_FIFO(pI2SModule, 14);
    I2S_WRITE_TX_FIFO(pI2SModule, 15);

    while (I2S_GET_TX_FIFO_LEVEL(pI2SModule) != 0) {}

    CU_ASSERT(I2S_GET_INT_FLAG(pI2SModule, I2S_STATUS0_RXOVIF_Msk) == 0x00000200);
    I2S_CLR_INT_FLAG(pI2SModule, I2S_STATUS0_RXOVIF_Msk);
    CU_ASSERT(I2S_GET_INT_FLAG(pI2SModule, I2S_STATUS0_RXOVIF_Msk) == 0);
    pI2SModule->CTL0 = 0;

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    /* RX underflow interrupt flag test */
    /* Enable I2S controller */
    pI2SModule->CTL0 = 1;

    I2S_READ_RX_FIFO(pI2SModule);
    I2S_READ_RX_FIFO(pI2SModule);

    CU_ASSERT(I2S_GET_INT_FLAG(pI2SModule, I2S_STATUS0_RXUDIF_Msk) == 0x00000100);

    I2S_CLR_INT_FLAG(pI2SModule, I2S_STATUS0_RXUDIF_Msk);

    CU_ASSERT(I2S_GET_INT_FLAG(pI2SModule, I2S_STATUS0_RXUDIF_Msk) == 0);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void MACRO_I2S_GET_TX_FIFO_LEVEL()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    /* Write data to TX FIFO */
    I2S_WRITE_TX_FIFO(pI2SModule, 1);

    CU_ASSERT(I2S_GET_TX_FIFO_LEVEL(pI2SModule) == 1);

    I2S_WRITE_TX_FIFO(pI2SModule, 2);

    CU_ASSERT(I2S_GET_TX_FIFO_LEVEL(pI2SModule) == 2);

    I2S_WRITE_TX_FIFO(pI2SModule, 3);

    CU_ASSERT(I2S_GET_TX_FIFO_LEVEL(pI2SModule) == 3);

    I2S_WRITE_TX_FIFO(pI2SModule, 4);

    CU_ASSERT(I2S_GET_TX_FIFO_LEVEL(pI2SModule) == 4);

    I2S_WRITE_TX_FIFO(pI2SModule, 5);

    CU_ASSERT(I2S_GET_TX_FIFO_LEVEL(pI2SModule) == 5);

    I2S_WRITE_TX_FIFO(pI2SModule, 6);

    CU_ASSERT(I2S_GET_TX_FIFO_LEVEL(pI2SModule) == 6);

    I2S_WRITE_TX_FIFO(pI2SModule, 7);

    CU_ASSERT(I2S_GET_TX_FIFO_LEVEL(pI2SModule) == 7);

    I2S_WRITE_TX_FIFO(pI2SModule, 8);

    CU_ASSERT(I2S_GET_TX_FIFO_LEVEL(pI2SModule) == 8);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void MACRO_I2S_GET_RX_FIFO_LEVEL()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    /* Set I2S bit clock divider */
    pI2SModule->CLKDIV = 0xFF00;
    /* Enable I2S RX function */
    pI2SModule->CTL0 = 5;

    while ((pI2SModule->STATUS1 & 0x000F0000) != 0x00010000) __NOP();

    CU_ASSERT(I2S_GET_RX_FIFO_LEVEL(pI2SModule) == 1);

    while ((pI2SModule->STATUS1 & 0x000F0000) != 0x00020000) __NOP();

    CU_ASSERT(I2S_GET_RX_FIFO_LEVEL(pI2SModule) == 2);

    while ((pI2SModule->STATUS1 & 0x000F0000) != 0x00030000) __NOP();

    CU_ASSERT(I2S_GET_RX_FIFO_LEVEL(pI2SModule) == 3);

    while ((pI2SModule->STATUS1 & 0x000F0000) != 0x00040000) __NOP();

    CU_ASSERT(I2S_GET_RX_FIFO_LEVEL(pI2SModule) == 4);

    while ((pI2SModule->STATUS1 & 0x000F0000) != 0x00050000) __NOP();

    CU_ASSERT(I2S_GET_RX_FIFO_LEVEL(pI2SModule) == 5);

    while ((pI2SModule->STATUS1 & 0x000F0000) != 0x00060000) __NOP();

    CU_ASSERT(I2S_GET_RX_FIFO_LEVEL(pI2SModule) == 6);

    while ((pI2SModule->STATUS1 & 0x000F0000) != 0x00070000) __NOP();

    CU_ASSERT(I2S_GET_RX_FIFO_LEVEL(pI2SModule) == 7);

    while ((pI2SModule->STATUS1 & 0x000F0000) != 0x00080000) __NOP();

    CU_ASSERT(I2S_GET_RX_FIFO_LEVEL(pI2SModule) == 8);

    /* Disable I2S RX function */
    pI2SModule->CTL0 = 0;

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void API_I2S_Open()
{
    uint32_t u32ReturnValue;
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    /* Unlock protected registers */
    SYS_UnlockReg();

    if (GetI2SModuleIdx() == C_I2S0)
    {
        CLK->I2SSEL = (CLK->I2SSEL & ~CLK_I2SSEL_I2S0SEL_Msk) | CLK_I2SSEL_I2S0SEL_HIRC48M;
    }
    else if (GetI2SModuleIdx() == C_I2S1)
    {
        CLK->I2SSEL = (CLK->I2SSEL & ~CLK_I2SSEL_I2S1SEL_Msk) | CLK_I2SSEL_I2S1SEL_HIRC48M;
    }

    u32ReturnValue = I2S_Open(pI2SModule, I2S_MODE_MASTER, 8000, I2S_DATABIT_8, I2S_ENABLE_MONO, I2S_FORMAT_I2S);
    //printf("1 u32ReturnValue = %d\r\n", u32ReturnValue);
    //printf("1 pI2SModule->CLKDIV = %x\r\n", pI2SModule->CLKDIV);
    CU_ASSERT(pI2SModule->CTL0 == 0x00000041);
    CU_ASSERT(pI2SModule->CLKDIV == 0x0000BB00);
    CU_ASSERT(u32ReturnValue == 7978);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    u32ReturnValue = I2S_Open(pI2SModule, I2S_MODE_SLAVE, 8000, I2S_DATABIT_16, I2S_DISABLE_MONO, I2S_FORMAT_I2S_MSB);
    //printf("2 u32ReturnValue = %d\r\n", u32ReturnValue);
    //printf("2 pI2SModule->CLKDIV = %x\r\n", pI2SModule->CLKDIV);
    CU_ASSERT(pI2SModule->CTL0 == 0x01000111);
    CU_ASSERT(pI2SModule->CLKDIV == 0x00005D00);
    CU_ASSERT(u32ReturnValue == 7978);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    u32ReturnValue = I2S_Open(pI2SModule, I2S_MODE_SLAVE, 8000, I2S_DATABIT_16, I2S_DISABLE_MONO, I2S_FORMAT_I2S_LSB);
    //printf("3 u32ReturnValue = %d\r\n", u32ReturnValue);
    //printf("3 pI2SModule->CLKDIV = %x\r\n", pI2SModule->CLKDIV);
    CU_ASSERT(pI2SModule->CTL0 == 0x02000111);
    CU_ASSERT(pI2SModule->CLKDIV == 0x00005D00);
    CU_ASSERT(u32ReturnValue == 7978);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    u32ReturnValue = I2S_Open(pI2SModule, I2S_MODE_SLAVE, 16000, I2S_DATABIT_24, I2S_DISABLE_MONO, I2S_FORMAT_PCM);
    //printf("4 u32ReturnValue = %d\r\n", u32ReturnValue);
    //printf("4 pI2SModule->CLKDIV = %x\r\n", pI2SModule->CLKDIV);
    CU_ASSERT(pI2SModule->CTL0 == 0x04000121);
    CU_ASSERT(pI2SModule->CLKDIV == 0x0001E00);
    CU_ASSERT(u32ReturnValue == 16129);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    u32ReturnValue = I2S_Open(pI2SModule, I2S_MODE_SLAVE, 32000, I2S_DATABIT_32, I2S_DISABLE_MONO, I2S_FORMAT_PCM_MSB);
    //printf("5 u32ReturnValue = %d\r\n", u32ReturnValue);
    //printf("5 pI2SModule->CLKDIV = %x\r\n", pI2SModule->CLKDIV);
    CU_ASSERT(pI2SModule->CTL0 == 0x05000131);
    CU_ASSERT(pI2SModule->CLKDIV == 0x00000B00);
    CU_ASSERT(u32ReturnValue == 31250);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    u32ReturnValue = I2S_Open(pI2SModule, I2S_MODE_SLAVE, 32000, I2S_DATABIT_32, I2S_DISABLE_MONO, I2S_FORMAT_PCM_LSB);
    //printf("6 u32ReturnValue = %d\r\n", u32ReturnValue);
    //printf("6 pI2SModule->CLKDIV = %x\r\n", pI2SModule->CLKDIV);
    CU_ASSERT(pI2SModule->CTL0 == 0x06000131);
    CU_ASSERT(pI2SModule->CLKDIV == 0x00000B00);
    CU_ASSERT(u32ReturnValue == 31250);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void API_I2S_Close()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    pI2SModule->CTL0 = 1;

    I2S_Close(pI2SModule);

    CU_ASSERT_FALSE(pI2SModule->CTL0 & 1);
}

void API_I2S_EnableInt_DisableInt()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    I2S_EnableInt(pI2SModule, I2S_IEN_CH0ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0x10000);
    I2S_DisableInt(pI2SModule, I2S_IEN_CH0ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0);

    I2S_EnableInt(pI2SModule, I2S_IEN_CH1ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0x20000);
    I2S_DisableInt(pI2SModule, I2S_IEN_CH1ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0);

    I2S_EnableInt(pI2SModule, I2S_IEN_CH2ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0x40000);
    I2S_DisableInt(pI2SModule, I2S_IEN_CH2ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0);

    I2S_EnableInt(pI2SModule, I2S_IEN_CH3ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0x80000);
    I2S_DisableInt(pI2SModule, I2S_IEN_CH3ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0);

    I2S_EnableInt(pI2SModule, I2S_IEN_CH4ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0x100000);
    I2S_DisableInt(pI2SModule, I2S_IEN_CH4ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0);

    I2S_EnableInt(pI2SModule, I2S_IEN_CH5ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0x200000);
    I2S_DisableInt(pI2SModule, I2S_IEN_CH5ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0);

    I2S_EnableInt(pI2SModule, I2S_IEN_CH6ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0x400000);
    I2S_DisableInt(pI2SModule, I2S_IEN_CH6ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0);

    I2S_EnableInt(pI2SModule, I2S_IEN_CH7ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0x800000);
    I2S_DisableInt(pI2SModule, I2S_IEN_CH7ZCIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0);

    I2S_EnableInt(pI2SModule, I2S_IEN_TXTHIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0x0400);
    I2S_DisableInt(pI2SModule, I2S_IEN_TXTHIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0);

    I2S_EnableInt(pI2SModule, I2S_IEN_TXOVFIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0x0200);
    I2S_DisableInt(pI2SModule, I2S_IEN_TXOVFIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0);

    I2S_EnableInt(pI2SModule, I2S_IEN_TXUDFIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0x0100);
    I2S_DisableInt(pI2SModule, I2S_IEN_TXUDFIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0);

    I2S_EnableInt(pI2SModule, I2S_IEN_RXTHIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0x0004);
    I2S_DisableInt(pI2SModule, I2S_IEN_RXTHIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0);

    I2S_EnableInt(pI2SModule, I2S_IEN_RXOVFIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0x0002);
    I2S_DisableInt(pI2SModule, I2S_IEN_RXOVFIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0);

    I2S_EnableInt(pI2SModule, I2S_IEN_RXUDFIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0x0001);
    I2S_DisableInt(pI2SModule, I2S_IEN_RXUDFIEN_Msk);
    CU_ASSERT(pI2SModule->IEN == 0);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);
}

void API_I2S_EnableMCLK()
{
    uint32_t u32ReturnValue;
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    /* Unlock protected registers */
    SYS_UnlockReg();

    if (GetI2SModuleIdx() == C_I2S0)
    {
        CLK->I2SSEL = (CLK->I2SSEL & ~CLK_I2SSEL_I2S0SEL_Msk) | CLK_I2SSEL_I2S0SEL_HIRC;
    }
    else if (GetI2SModuleIdx() == C_I2S1)
    {
        CLK->I2SSEL = (CLK->I2SSEL & ~CLK_I2SSEL_I2S1SEL_Msk) | CLK_I2SSEL_I2S1SEL_HIRC;
    }

    u32ReturnValue = I2S_EnableMCLK(pI2SModule, 2048000);
    //printf("1 u32ReturnValue = %d\r\n", u32ReturnValue);
    //printf("1 pI2SModule->CLKDIV = %x\r\n", pI2SModule->CLKDIV);
    CU_ASSERT(pI2SModule->CTL0 == 0x00008000);
    CU_ASSERT(pI2SModule->CLKDIV == 0x00000002);
    CU_ASSERT(u32ReturnValue == 3000000);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    u32ReturnValue = I2S_EnableMCLK(pI2SModule, 12000000);
    //printf("2 u32ReturnValue = %d\r\n", u32ReturnValue);
    //printf("2 pI2SModule->CLKDIV = %x\r\n", pI2SModule->CLKDIV);
    CU_ASSERT(pI2SModule->CTL0 == 0x00008000);
    CU_ASSERT(pI2SModule->CLKDIV == 0x00000000);
    CU_ASSERT(u32ReturnValue == 12000000);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    /* Unlock protected registers */
    SYS_UnlockReg();

    if (GetI2SModuleIdx() == C_I2S0)
    {
        CLK->I2SSEL = (CLK->I2SSEL & ~CLK_I2SSEL_I2S0SEL_Msk) | CLK_I2SSEL_I2S0SEL_HXT;
    }
    else if (GetI2SModuleIdx() == C_I2S1)
    {
        CLK->I2SSEL = (CLK->I2SSEL & ~CLK_I2SSEL_I2S1SEL_Msk) | CLK_I2SSEL_I2S1SEL_HXT;
    }

    u32ReturnValue = I2S_EnableMCLK(pI2SModule, 12000000);
    //printf("3 u32ReturnValue = %d\r\n", u32ReturnValue);
    //printf("3 pI2SModule->CLKDIV = %x\r\n", pI2SModule->CLKDIV);
    CU_ASSERT(pI2SModule->CTL0 == 0x00008000);
    CU_ASSERT(pI2SModule->CLKDIV == 0x00000001);
    CU_ASSERT(u32ReturnValue == 12000000);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    /* Unlock protected registers */
    SYS_UnlockReg();

    if (GetI2SModuleIdx() == C_I2S0)
    {
        CLK->I2SSEL = (CLK->I2SSEL & ~CLK_I2SSEL_I2S0SEL_Msk) | CLK_I2SSEL_I2S0SEL_APLL1_DIV2;
    }
    else if (GetI2SModuleIdx() == C_I2S1)
    {
        CLK->I2SSEL = (CLK->I2SSEL & ~CLK_I2SSEL_I2S1SEL_Msk) | CLK_I2SSEL_I2S1SEL_APLL1_DIV2;
    }

    u32ReturnValue = I2S_EnableMCLK(pI2SModule, 12000000);
    //printf("4 u32ReturnValue = %d\r\n", u32ReturnValue);
    //printf("4 pI2SModule->CLKDIV = %x\r\n", pI2SModule->CLKDIV);
    CU_ASSERT(pI2SModule->CTL0 == 0x00008000);
    CU_ASSERT(pI2SModule->CLKDIV == 0x00000000);
    CU_ASSERT(u32ReturnValue == 0);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    /* Unlock protected registers */
    SYS_UnlockReg();

    if (GetI2SModuleIdx() == C_I2S0)
    {
        CLK->I2SSEL = (CLK->I2SSEL & ~CLK_I2SSEL_I2S0SEL_Msk) | CLK_I2SSEL_I2S0SEL_APLL0_DIV2;
    }
    else if (GetI2SModuleIdx() == C_I2S1)
    {
        CLK->I2SSEL = (CLK->I2SSEL & ~CLK_I2SSEL_I2S1SEL_Msk) | CLK_I2SSEL_I2S1SEL_APLL0_DIV2;
    }

    u32ReturnValue = I2S_EnableMCLK(pI2SModule, 12000000);
    //printf("5 u32ReturnValue = %d\r\n", u32ReturnValue);
    //printf("5 pI2SModule->CLKDIV = %x\r\n", pI2SModule->CLKDIV);
    CU_ASSERT(pI2SModule->CTL0 == 0x00008000);
    CU_ASSERT(pI2SModule->CLKDIV == 0x00000004);
    CU_ASSERT(u32ReturnValue == 12500000);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    /* Unlock protected registers */
    SYS_UnlockReg();

    if (GetI2SModuleIdx() == C_I2S0)
    {
        CLK->I2SSEL = (CLK->I2SSEL & ~CLK_I2SSEL_I2S0SEL_Msk) | CLK_I2SSEL_I2S0SEL_PCLK1;
    }
    else if (GetI2SModuleIdx() == C_I2S1)
    {
        CLK->I2SSEL = (CLK->I2SSEL & ~CLK_I2SSEL_I2S1SEL_Msk) | CLK_I2SSEL_I2S1SEL_PCLK3;
    }

    u32ReturnValue = I2S_EnableMCLK(pI2SModule, 12000000);
    //printf("6 u32ReturnValue = %d\r\n", u32ReturnValue);
    //printf("6 pI2SModule->CLKDIV = %x\r\n", pI2SModule->CLKDIV);
    CU_ASSERT(pI2SModule->CTL0 == 0x00008000);
    CU_ASSERT(pI2SModule->CLKDIV == 0x00000004);
    CU_ASSERT(u32ReturnValue == 12500000);

    /* Reset I2S */
    ResetI2SModule(pI2SModule);

    /* Unlock protected registers */
    SYS_UnlockReg();

    if (GetI2SModuleIdx() == C_I2S0)
    {
        CLK->I2SSEL = (CLK->I2SSEL & ~CLK_I2SSEL_I2S0SEL_Msk) | CLK_I2SSEL_I2S0SEL_HIRC48M;
    }
    else if (GetI2SModuleIdx() == C_I2S1)
    {
        CLK->I2SSEL = (CLK->I2SSEL & ~CLK_I2SSEL_I2S1SEL_Msk) | CLK_I2SSEL_I2S1SEL_HIRC48M;
    }

    u32ReturnValue = I2S_EnableMCLK(pI2SModule, 12000000);
    //printf("7 u32ReturnValue = %d\r\n", u32ReturnValue);
    //printf("7 pI2SModule->CLKDIV = %x\r\n", pI2SModule->CLKDIV);
    CU_ASSERT(pI2SModule->CTL0 == 0x00008000);
    CU_ASSERT(pI2SModule->CLKDIV == 0x00000002);
    CU_ASSERT(u32ReturnValue == 12000000);
}

void API_I2S_DisableMCLK()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    /* Set MCLKEN bit to 1 */
    pI2SModule->CTL0 = 0x8000;
    I2S_DisableMCLK(pI2SModule);

    /* Check MCLKEN bit */
    CU_ASSERT_FALSE(pI2SModule->CTL0 & 0x8000);
}

void API_I2S_SetFIFO()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    I2S_SetFIFO(pI2SModule, 0, 0);
    CU_ASSERT((pI2SModule->CTL1 & 0x000F0F00) == (I2S_FIFO_TX_LEVEL_WORD_0 | I2S_FIFO_RX_LEVEL_WORD_1));

    I2S_SetFIFO(pI2SModule, 1, 1);
    CU_ASSERT((pI2SModule->CTL1 & 0x000F0F00) == (I2S_FIFO_TX_LEVEL_WORD_1 | I2S_FIFO_RX_LEVEL_WORD_2));

    I2S_SetFIFO(pI2SModule, 2, 2);
    CU_ASSERT((pI2SModule->CTL1 & 0x000F0F00) == (I2S_FIFO_TX_LEVEL_WORD_2 | I2S_FIFO_RX_LEVEL_WORD_3));

    I2S_SetFIFO(pI2SModule, 3, 3);
    CU_ASSERT((pI2SModule->CTL1 & 0x000F0F00) == (I2S_FIFO_TX_LEVEL_WORD_3 | I2S_FIFO_RX_LEVEL_WORD_4));

    I2S_SetFIFO(pI2SModule, 4, 4);
    CU_ASSERT((pI2SModule->CTL1 & 0x000F0F00) == (I2S_FIFO_TX_LEVEL_WORD_4 | I2S_FIFO_RX_LEVEL_WORD_5));

    I2S_SetFIFO(pI2SModule, 5, 5);
    CU_ASSERT((pI2SModule->CTL1 & 0x000F0F00) == (I2S_FIFO_TX_LEVEL_WORD_5 | I2S_FIFO_RX_LEVEL_WORD_6));

    I2S_SetFIFO(pI2SModule, 6, 6);
    CU_ASSERT((pI2SModule->CTL1 & 0x000F0F00) == (I2S_FIFO_TX_LEVEL_WORD_6 | I2S_FIFO_RX_LEVEL_WORD_7));

    I2S_SetFIFO(pI2SModule, 7, 7);
    CU_ASSERT((pI2SModule->CTL1 & 0x000F0F00) == (I2S_FIFO_TX_LEVEL_WORD_7 | I2S_FIFO_RX_LEVEL_WORD_8));
}

void API_I2S_SetTDM()
{
    I2S_T *pI2SModule = NULL;

    pI2SModule = (I2S_T *)GetI2SModuel(GetI2SModuleIdx());

    I2S_ConfigureTDM(pI2SModule, I2S_TDM_WIDTH_8BIT, I2S_TDM_2CH, I2S_TDM_SYNC_ONE_BCLK);
    CU_ASSERT((pI2SModule->CTL0 & (I2S_CTL0_TDMCHNUM_Msk | I2S_CTL0_CHWIDTH_Msk | I2S_CTL0_PCMSYNC_Msk)) == 0);

    I2S_ConfigureTDM(pI2SModule, I2S_TDM_WIDTH_16BIT, I2S_TDM_4CH, I2S_TDM_SYNC_ONE_CHANNEL);
    CU_ASSERT((pI2SModule->CTL0 & (I2S_CTL0_TDMCHNUM_Msk | I2S_CTL0_CHWIDTH_Msk | I2S_CTL0_PCMSYNC_Msk)) == 0x58000000);

    I2S_ConfigureTDM(pI2SModule, I2S_TDM_WIDTH_24BIT, I2S_TDM_6CH, I2S_TDM_SYNC_ONE_CHANNEL);
    CU_ASSERT((pI2SModule->CTL0 & (I2S_CTL0_TDMCHNUM_Msk | I2S_CTL0_CHWIDTH_Msk | I2S_CTL0_PCMSYNC_Msk)) == 0xA8000000);

    I2S_ConfigureTDM(pI2SModule, I2S_TDM_WIDTH_32BIT, I2S_TDM_8CH, I2S_TDM_SYNC_ONE_CHANNEL);
    CU_ASSERT((pI2SModule->CTL0 & (I2S_CTL0_TDMCHNUM_Msk | I2S_CTL0_CHWIDTH_Msk | I2S_CTL0_PCMSYNC_Msk)) == 0xF8000000);
}

CU_TestInfo I2S_MacroTests[] =
{
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
    {"Set I2S FIFO threshold", API_I2S_SetFIFO},

    CU_TEST_INFO_NULL
};

CU_TestInfo I2S_ApiTests[] =
{

    {"I2S Open", API_I2S_Open},
    {"I2S Close", API_I2S_Close},
    {"Enable/Disable I2S interrupt function", API_I2S_EnableInt_DisableInt},
    {"Enable MCLK", API_I2S_EnableMCLK},
    {"Disable MCLK", API_I2S_DisableMCLK},
    {"Set I2S TDM parameters", API_I2S_SetTDM},

    CU_TEST_INFO_NULL
};

CU_SuiteInfo suites[] =
{
    {"I2S MACRO", I2S_Tests_Init, I2S_Tests_Clean, NULL, NULL, I2S_MacroTests},
    {"I2S API", I2S_Tests_Init, I2S_Tests_Clean, NULL, NULL, I2S_ApiTests},
    CU_SUITE_INFO_NULL
};
