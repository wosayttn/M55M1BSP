/**************************************************************************//**
 * @file     spim_cunit.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 15/09/15 4:36p $
 * @brief    NUC4xx series SPIM CUnit test function.
 *
 * @note
 * Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

//* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "spim_cunit.h"
#include "hyper_cunit.h"
#include "common.h"

//------------------------------------------------------------------------------
static void do_hyper_dmm_w64bit(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvWrBuf, uint32_t u32NTx)
{
    uint8_t *u8WrBuf = NULL;
    uint32_t u32i = 0;
    uint32_t u32RemainSize = (u32NTx % 8);
    uint32_t u32DataCnt = 0;
    uint64_t *p64WrBuf = (uint64_t *)pvWrBuf;

    for (u32i = 0; u32i < (u32NTx - u32RemainSize); u32i += 8)
    {
        outpll(u32SAddr + u32i, p64WrBuf[u32DataCnt++]);
    }

    if (u32RemainSize != 0)
    {
        u8WrBuf = (uint8_t *)&pvWrBuf[(u32NTx - u32RemainSize)];

        for (u32i = 0; u32i < u32RemainSize; u32i++)
        {
            outpb(u32SAddr + (u32NTx - u32RemainSize) + u32i, u8WrBuf[u32i]);
        }
    }
}

static void do_hyper_dmm_r64bit(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvRdBuf, uint32_t u32NRx)
{
    uint8_t *u8RxBuf = NULL;
    uint32_t u32i = 0;
    uint32_t u32RemainSize = (u32NRx % 8);
    uint32_t u32DataCnt = 0;
    uint64_t *p64RxBuf = (uint64_t *)pvRdBuf;

    for (u32i = 0; u32i < (u32NRx - u32RemainSize); u32i += 8)
    {
        p64RxBuf[u32DataCnt++] = inpll(u32SAddr + u32i);
    }

    if (u32RemainSize != 0)
    {
        u8RxBuf = (uint8_t *)&pvRdBuf[(u32NRx - u32RemainSize)];

        for (u32i = 0; u32i < u32RemainSize; u32i++)
        {
            u8RxBuf[u32i] = inpb((u32SAddr - u32RemainSize) + u32i);
        }
    }
}

static void do_hyper_dmm_w32bit(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvWrBuf, uint32_t u32NTx)
{
    uint8_t *u8WrBuf = NULL;
    uint32_t *pu32WrBuf = (uint32_t *)pvWrBuf;
    uint32_t u32i = 0;
    uint32_t u32DataCnt = 0;
    uint32_t u32RemainSize = (u32NTx % 4);

    for (u32i = 0; u32i < u32NTx; u32i += 4)
    {
        outpw(u32SAddr + u32i, pu32WrBuf[u32DataCnt++]);
    }

    if (u32RemainSize != 0)
    {
        u8WrBuf = (uint8_t *)&pvWrBuf[(u32NTx - u32RemainSize)];

        for (u32i = 0; u32i < u32RemainSize; u32i++)
        {
            outpb(u32SAddr + (u32NTx - u32RemainSize) + u32i, u8WrBuf[u32i]);
        }

    }
}

static void do_hyper_dmm_r32bit(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvRdBuf, uint32_t u32NRx)
{
    uint8_t *u8Rxbuf = NULL;
    uint32_t *u32RxBuf = (uint32_t *)pvRdBuf;
    uint32_t u32i = 0;
    uint32_t u32DataCnt = 0;
    uint32_t u32RemainSize = (u32NRx % 4);

    for (u32i = 0; u32i < (u32NRx - u32RemainSize); u32i += 4)
    {
        u32RxBuf[u32DataCnt++] = inpw(u32SAddr + u32i);
    }

    if (u32RemainSize != 0)
    {
        u8Rxbuf = (uint8_t *)&pvRdBuf[(u32NRx - u32RemainSize)];

        for (u32i = 0; u32i < u32RemainSize; u32i++)
        {
            u8Rxbuf[u32i] = inpb((u32SAddr + u32NRx) - u32RemainSize);
        }
    }
}

static void do_hyper_dmm_w16bit(SPIM_T *pSPIMx,
                                uint32_t u32SAddr,
                                void *pvWrBuf,
                                uint32_t u32NTx)
{
    uint8_t *pu8WrBuf = NULL;
    uint16_t *pu16WrBuf = (uint16_t *)&pvWrBuf[0];
    uint32_t u32i = 0;
    uint32_t u32DataCnt = 0;
    uint32_t u32RemainSize = (u32NTx % 2);

    for (u32i = 0; u32i < (u32NTx - u32RemainSize); u32i += 2)
    {
        outps(u32SAddr + u32i, pu16WrBuf[u32DataCnt++]);
    }

    if (u32RemainSize != 0)
    {
        pu8WrBuf = (uint8_t *)&pvWrBuf[(u32NTx - u32RemainSize)];
        outpb((u32SAddr + (u32NTx - u32RemainSize)), *pu8WrBuf);
    }
}

static void do_hyper_dmm_r16bit(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvRdBuf, uint32_t u32NRx)
{
    uint8_t *pu8RxBuf = NULL;
    uint16_t u32DataCnt = 0;
    uint16_t *pu16RxBuf = (uint16_t *)pvRdBuf;
    uint32_t u32i = 0;
    uint32_t u32RemainSize = (u32NRx % 2);

    for (u32i = 0; u32i < (u32NRx - u32RemainSize); u32i += 2)
    {
        pu16RxBuf[u32DataCnt++] = inps(u32SAddr + u32i);
    }

    if (u32RemainSize != 0)
    {
        pu8RxBuf = (uint8_t *)&pvRdBuf[(u32NRx - u32RemainSize)];

        *pu8RxBuf = inpb((u32SAddr + u32NRx) - u32RemainSize);
    }
}

static void do_hyper_dmm_w8bit(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvWrBuf, uint32_t u32NTx)
{
    uint8_t *u8WrBuf = (uint8_t *)pvWrBuf;
    uint32_t u32i = 0;

    for (u32i = 0; u32i < u32NTx; u32i++)
    {
        outpb(u32SAddr + u32i, u8WrBuf[u32i]);

    }
}

static void do_hyper_dmm_r8bit(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvRdBuf, uint32_t u32NRx)
{
    uint32_t u32i = 0;
    uint8_t *pRxData = (uint8_t *)pvRdBuf;

    for (u32i = 0; u32i < u32NRx; u32i++)
    {
        pRxData[u32i] = inpb(u32SAddr + u32i);
    }
}

int SPIM_Tests_HyperInit(void)
{
    uint32_t u32Delay = 0;
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    SPIM_SET_HYPER_MODE(pSPIMModule, 1);     //Enable HyperBus Mode
    //CU_ASSERT_TRUE((pSPIMModule->CTL0 & SPIM_CTL0_HYPER_EN_Msk) >> SPIM_CTL0_HYPER_EN_Pos);

    SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 1); /* Set SPIM clock as HCLK divided by 2 */
    //CU_ASSERT(SPIM_GET_CLOCK_DIVIDER(pSPIMModule) == 1);

    //SPIM Def. Enable Cipher, First Disable the test.
    SPIM_DISABLE_CIPHER(pSPIMModule);
    //CU_ASSERT_TRUE((pSPIMModule->CTL0 & SPIM_CTL0_CIPHOFF_Msk) >> SPIM_CTL0_CIPHOFF_Pos);

    SPIM_Hyper_DefaultConfig(pSPIMModule, 780, 7, 7);

    SPIM_Hyper_Reset(pSPIMModule);

    for (u32Delay = 0; u32Delay < 0x400; u32Delay++) {}

    return 0;
}

int SPIM_Tests_HyperClean(void)
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    SPIM_SET_HYPER_MODE(pSPIMModule, 0);     //Enable HyperBus Mode
    //CU_ASSERT_FALSE((pSPIMModule->CTL0 & SPIM_CTL0_HYPER_EN_Msk) >> SPIM_CTL0_HYPER_EN_Pos);

    return 0;
}

void SPIM_Hyper_EraseHRAM(SPIM_T *pSPIMx, uint32_t u32StartAddr, uint32_t u32EraseSize)
{
    uint16_t u16Data = 0;
    uint32_t u32i = 0;
    uint32_t u32RemainSize = (u32EraseSize % 2);

    for (u32i = 0; u32i <= (u32EraseSize - u32RemainSize); u32i += 2)
    {
        SPIM_Hyper_Write2Byte(pSPIMx, (u32StartAddr + u32i), 0x0000);
        u16Data = SPIM_Hyper_Read1Word(pSPIMx, (u32StartAddr + u32i));

        if (u16Data != 0x0000)
        {
            printf("Erase Hyper RAM fail!! Read address:0x%08x, data::0x%08x  expect: 0\n",
                   u32StartAddr + u32i,
                   u16Data);
            CU_FAIL();
        }
    }

    if (u32RemainSize != 0)
    {
        SPIM_Hyper_Write1Byte(pSPIMx, (u32StartAddr + (u32EraseSize - 1)), 0x00);
        u16Data = SPIM_Hyper_Read1Word(pSPIMx, (u32StartAddr + u32EraseSize));

        if ((u16Data & 0xFF) != 0)
        {
            printf("Erase Remain Hyper RAM fail!! Read address:0x%08x, data::0x%08x  expect: 0\n",
                   (u32StartAddr + (u32EraseSize - 1)),
                   u16Data);
            CU_FAIL();
        }
    }

    CU_PASS();
}

void SPIM_HyperDMM_Func()
{
    uint32_t addr, i;
    //uint32_t u32STCount = 0;
    uint8_t u8PRx = 0;
    uint32_t u32DMMAddr = 0;
    int *pi32SrcAddr = NULL;
    uint8_t *u8TstBuf1 = (uint8_t *)GetTestBuffer1();
    uint8_t *u8TstBuf2 = (uint8_t *)GetTestBuffer2();
    uint32_t u32NBit = 0;
    SPIM_T *pSPIMx = NULL;

    pSPIMx = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());
    u32DMMAddr = GetDmmModeMapAddr(pSPIMx);

    SPIM_DISABLE_CIPHER(pSPIMx);

    for (addr = 0; addr < HRAM_PAGE_SIZE; addr += TEST_BUFF_SIZE)
    {
        /*------------------------------------------------------*/
        /*  Erase page and verify                               */
        /*------------------------------------------------------*/
        SPIM_Hyper_EraseHRAM(pSPIMx, addr, addr + TEST_BUFF_SIZE);

        popDat(u8TstBuf1, TEST_BUFF_SIZE);

        SPIM_Hyper_EnterDirectMapMode(pSPIMx); // Hyper Mode Switch to Direct Map mode.

        switch (u32NBit)
        {
            case 0:
                pi32SrcAddr = (int *)(u32DMMAddr + addr);
                memcpy(pi32SrcAddr, u8TstBuf1, TEST_BUFF_SIZE);
                break;

            case 8:
                do_hyper_dmm_w8bit(pSPIMx, (u32DMMAddr + addr), u8TstBuf1, TEST_BUFF_SIZE);
                break;

            case 16:
                do_hyper_dmm_w16bit(pSPIMx, (u32DMMAddr + addr), u8TstBuf1, TEST_BUFF_SIZE);
                break;

            case 32:
                do_hyper_dmm_w32bit(pSPIMx, (u32DMMAddr + addr), u8TstBuf1, TEST_BUFF_SIZE);
                break;

            case 64:
                do_hyper_dmm_w64bit(pSPIMx, (u32DMMAddr + addr), u8TstBuf1, TEST_BUFF_SIZE);
                break;
        }

        SPIM_Hyper_WaitDMMDone(pSPIMx);

        memset(u8TstBuf2, 0x0, TEST_BUFF_SIZE);

        switch (u32NBit)
        {
            case 0:
                pi32SrcAddr = (int *)(u32DMMAddr + addr);
                memcpy(u8TstBuf2, pi32SrcAddr, TEST_BUFF_SIZE);
                break;

            case 8:
                do_hyper_dmm_r8bit(pSPIMx, (u32DMMAddr + addr), u8TstBuf2, TEST_BUFF_SIZE);
                break;

            case 16:
                do_hyper_dmm_r16bit(pSPIMx, (u32DMMAddr + addr), u8TstBuf2, TEST_BUFF_SIZE);
                break;

            case 32:
                do_hyper_dmm_r32bit(pSPIMx, (u32DMMAddr + addr), u8TstBuf2, TEST_BUFF_SIZE);
                break;

            case 64:
                do_hyper_dmm_r64bit(pSPIMx, (u32DMMAddr + addr), u8TstBuf2, TEST_BUFF_SIZE);
                break;
        }

        SPIM_Hyper_WaitDMMDone(pSPIMx);

        if (memcmp(u8TstBuf1, u8TstBuf2, TEST_BUFF_SIZE))
        {
            //dump_compare_error(addr, u8TstBuf1, u8TstBuf2, TEST_BUFF_SIZE);
            printf("DMM Test failed \r\n");
            CU_FAIL();
        }
    }

    printf("Hyper DMM Test Done\r\n");
    CU_PASS();
}

void SPIM_HyperDMA_Func()
{
    uint32_t offset = 0;
    SPIM_T *pSPIMx = NULL;
    uint8_t *u8TstBuf1 = (uint8_t *)GetTestBuffer1();
    uint8_t *u8TstBuf2 = (uint8_t *)GetTestBuffer2();

    pSPIMx = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    for (offset = 0; offset < HRAM_PAGE_SIZE; offset += TEST_BUFF_SIZE)
    {
        /*------------------------------------------------------*/
        /*  Erase page and verify                               */
        /*------------------------------------------------------*/
        // Erase accessed address range.
        SPIM_Hyper_EraseHRAM(pSPIMx, offset, TEST_BUFF_SIZE);
        popDat(u8TstBuf1, TEST_BUFF_SIZE);

        SPIM_Hyper_DMAWrite(pSPIMx, offset, u8TstBuf1, TEST_BUFF_SIZE);

        memset(u8TstBuf2, 0x0, TEST_BUFF_SIZE);
        SPIM_Hyper_DMARead(pSPIMx, offset, u8TstBuf2, TEST_BUFF_SIZE);

        if (memcmp(u8TstBuf1, u8TstBuf2, TEST_BUFF_SIZE))
        {
            printf("SPIM Hyper DMA Test Failed\r\n");
            CU_FAIL();
        }
    }

    CU_PASS();
}

void TrainingDllLatency()
{
    uint32_t u32i = 0;
    uint32_t u32j = 0;
    uint8_t u8RdDelay = 0;
    uint8_t u8Temp = 0;
    uint8_t u8RdDelayIdx = 0;
    uint8_t u8RdDelayRes[SPIM_MAX_DLL_LATENCY] = {0};
    uint32_t u32SrcAddr = 0;
    uint32_t u32TestSize = 32;
    SPIM_T *pSPIMx = NULL;
#if defined (__ARMCC_VERSION)
    __attribute__((aligned(32))) uint8_t u8TstBuf1[32] = {0};
    __attribute__((aligned(32))) uint8_t u8TstBuf2[32] = {0};
#else
    __align(32) uint8_t u8TstBuf1[32] = {0};
    __align(32) uint8_t u8TstBuf2[32] = {0};
#endif //__ARMCC_VERSION

    pSPIMx = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    SPIM_Hyper_EraseHRAM(pSPIMx, u32SrcAddr, u32TestSize);

    popDat(&u8TstBuf1[0], u32TestSize);

    for (u32i = u32SrcAddr; u32i < u32TestSize; u32i++)
    {
        SPIM_Hyper_Write1Byte(pSPIMx, u32i, u8TstBuf1[u32i]);
    }

    for (u8RdDelay = 0; u8RdDelay <= SPIM_MAX_DLL_LATENCY; u8RdDelay++)
    {
        memset(u8TstBuf2, 0, u32TestSize);
        SPIM_CtrlDLLDelayTime(pSPIMx, 0, 0, 0, 0, u8RdDelay);

        SPIM_Hyper_DMARead(pSPIMx, u32SrcAddr, u8TstBuf2, u32TestSize);

        if (memcmp(u8TstBuf1, u8TstBuf2, u32TestSize))
        {
            printf("!!!\t%d Latency number failed\n", u8RdDelay);
            //CU_FAIL();
        }
        else
        {
            printf("RX Delay: %d = Pass\r\n", u8RdDelay);
            u8RdDelayRes[u8RdDelayIdx++] = u8RdDelay;
        }
    }

    //[sort]
    for (u32i = 0; u32i <= u8RdDelayIdx; u32i = u32i + 1)
    {
        for (u32j = u32i + 1; u32j < u8RdDelayIdx; u32j = u32j + 1)
        {
            if (u8RdDelayRes[u32i] > u8RdDelayRes[u32j])
            {
                u8Temp = u8RdDelayRes[u32i];
                u8RdDelayRes[u32i] = u8RdDelayRes[u32j];
                u8RdDelayRes[u32j] = u8Temp;
            }
        }
    }

    if (u8RdDelayIdx > 2)
    {
        u8RdDelayIdx = (u8RdDelayIdx / 2) - 1;
    }
    else
    {
        u8RdDelayIdx = 0;
    }

    printf("\r\nDLL Delay Time Num = %d\r\n\r\n", u8RdDelayRes[u8RdDelayIdx]);

    SPIM_CtrlDLLDelayTime(pSPIMx, 0, 0, 0, 0, u8RdDelayRes[u8RdDelayIdx]);
    CU_ASSERT(((pSPIMx->DLL0 & SPIM_DLL0_DLL_DNUM_Msk) >> SPIM_DLL0_DLL_DNUM_Pos) == u8RdDelayRes[u8RdDelayIdx]);

    CU_PASS();
}

void SPIM_Const_HYPER_REG()
{
    CU_ASSERT(HYPER_RAM_ID_REG0 == (0x00000000));
    CU_ASSERT(HYPER_RAM_ID_REG1 == (0x00000002));
    CU_ASSERT(HYPER_RAM_CONFIG_REG0 == (0x00001000));
    CU_ASSERT(HYPER_RAM_CONFIG_REG1 == (0x00001002));
}

void SPIM_Const_HYPER_CMD()
{
    CU_ASSERT(SPIM_HYPER_CMD_IDLE == (0x00000000));
    CU_ASSERT(SPIM_HYPER_CMD_RESET == (0x00000001));
    CU_ASSERT(SPIM_HYPER_CMD_READ_HRAM_REGISTER == (0x00000002));
    CU_ASSERT(SPIM_HYPER_CMD_EXIT_HS_PD == (0x00000005));
    CU_ASSERT(SPIM_HYPER_CMD_WRITE_HRAM_REGISTER == (0x00000007));
    CU_ASSERT(SPIM_HYPER_CMD_READ_1_WORD == (0x00000008));
    CU_ASSERT(SPIM_HYPER_CMD_READ_2_WORD == (0x00000009));
    CU_ASSERT(SPIM_HYPER_CMD_WRITE_1_BYTE == (0x0000000C));
    CU_ASSERT(SPIM_HYPER_CMD_WRITE_2_BYTE == (0x0000000D));
    CU_ASSERT(SPIM_HYPER_CMD_WRITE_3_BYTE == (0x0000000E));
    CU_ASSERT(SPIM_HYPER_CMD_WRITE_4_BYTE == (0x0000000F));
}

void SPIM_Const_HYPER_CONFIG1()
{
    /*
        CSST
    */
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSST_2_5_HCLK == (0x1));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSST_1_5_HCLK == (0x0));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSST_3_5_HCLK == (0x2));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSST_4_5_HCLK == (0x3));

    /*
        CSH
    */
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSH_0_5_HCLK == (0x0));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSH_1_5_HCLK == (0x1));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSH_2_5_HCLK == (0x2));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSH_3_5_HCLK == (0x3));

    /*
        CSHI
    */
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSHI_2_HCLK == (0x02));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSHI_3_HCLK == (0x03));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSHI_4_HCLK == (0x04));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSHI_5_HCLK == (0x05));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSHI_6_HCLK == (0x06));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSHI_7_HCLK == (0x07));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSHI_8_HCLK == (0x08));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSHI_9_HCLK == (0x09));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSHI_10_HCLK == (0x0A));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSHI_11_HCLK == (0x0B));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSHI_12_HCLK == (0x0C));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSHI_13_HCLK == (0x0D));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSHI_14_HCLK == (0x0E));
    CU_ASSERT(SPIM_HYPER_CONFIG1_CSHI_15_HCLK == (0x0F));
}

void MACRO_SPIM_HYPER_CONFIG1()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    /*
     *  SPIM_HYPER_CONFIG1_SET_CSST()
     */
    pSPIMModule->HYPER_CONFIG1 = 0;
    SPIM_HYPER_CONFIG1_SET_CSST(pSPIMModule, SPIM_HYPER_CONFIG1_CSST_1_5_HCLK);
    CU_ASSERT(((pSPIMModule->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSST_Msk) >> SPIM_HYPER_CONFIG1_CSST_Pos) == SPIM_HYPER_CONFIG1_CSST_1_5_HCLK);
    pSPIMModule->HYPER_CONFIG1 = 0;
    SPIM_HYPER_CONFIG1_SET_CSST(pSPIMModule, SPIM_HYPER_CONFIG1_CSST_2_5_HCLK);
    CU_ASSERT(((pSPIMModule->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSST_Msk) >> SPIM_HYPER_CONFIG1_CSST_Pos) == SPIM_HYPER_CONFIG1_CSST_2_5_HCLK);
    pSPIMModule->HYPER_CONFIG1 = 0;
    SPIM_HYPER_CONFIG1_SET_CSST(pSPIMModule, SPIM_HYPER_CONFIG1_CSST_3_5_HCLK);
    CU_ASSERT(((pSPIMModule->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSST_Msk) >> SPIM_HYPER_CONFIG1_CSST_Pos) == SPIM_HYPER_CONFIG1_CSST_3_5_HCLK);
    pSPIMModule->HYPER_CONFIG1 = 0;
    SPIM_HYPER_CONFIG1_SET_CSST(pSPIMModule, SPIM_HYPER_CONFIG1_CSST_4_5_HCLK);
    CU_ASSERT(((pSPIMModule->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSST_Msk) >> SPIM_HYPER_CONFIG1_CSST_Pos) == SPIM_HYPER_CONFIG1_CSST_4_5_HCLK);

    /*
     *  SPIM_HYPER_CONFIG1_SET_CSH()
     */
    pSPIMModule->HYPER_CONFIG1 = 0;
    SPIM_HYPER_CONFIG1_SET_CSH(pSPIMModule, SPIM_HYPER_CONFIG1_CSH_0_5_HCLK);
    CU_ASSERT(((pSPIMModule->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSH_Msk) >> SPIM_HYPER_CONFIG1_CSH_Pos) == SPIM_HYPER_CONFIG1_CSH_0_5_HCLK);
    pSPIMModule->HYPER_CONFIG1 = 0;
    SPIM_HYPER_CONFIG1_SET_CSH(pSPIMModule, SPIM_HYPER_CONFIG1_CSH_1_5_HCLK);
    CU_ASSERT(((pSPIMModule->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSH_Msk) >> SPIM_HYPER_CONFIG1_CSH_Pos) == SPIM_HYPER_CONFIG1_CSH_1_5_HCLK);
    pSPIMModule->HYPER_CONFIG1 = 0;
    SPIM_HYPER_CONFIG1_SET_CSH(pSPIMModule, SPIM_HYPER_CONFIG1_CSH_2_5_HCLK);
    CU_ASSERT(((pSPIMModule->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSH_Msk) >> SPIM_HYPER_CONFIG1_CSH_Pos) == SPIM_HYPER_CONFIG1_CSH_2_5_HCLK);
    pSPIMModule->HYPER_CONFIG1 = 0;
    SPIM_HYPER_CONFIG1_SET_CSH(pSPIMModule, SPIM_HYPER_CONFIG1_CSH_3_5_HCLK);
    CU_ASSERT(((pSPIMModule->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSH_Msk) >> SPIM_HYPER_CONFIG1_CSH_Pos) == SPIM_HYPER_CONFIG1_CSH_3_5_HCLK);

    /*
     *  SPIM_HYPER_CONFIG1_SET_CSHI()
     */
    pSPIMModule->HYPER_CONFIG1 = 0;
    SPIM_HYPER_CONFIG1_SET_CSHI(pSPIMModule, 2);
    CU_ASSERT(((pSPIMModule->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSHI_Msk) >> SPIM_HYPER_CONFIG1_CSHI_Pos) == 2);
    pSPIMModule->HYPER_CONFIG1 = 0;
    SPIM_HYPER_CONFIG1_SET_CSHI(pSPIMModule, 7);
    CU_ASSERT(((pSPIMModule->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSHI_Msk) >> SPIM_HYPER_CONFIG1_CSHI_Pos) == 7);
    pSPIMModule->HYPER_CONFIG1 = 0;
    SPIM_HYPER_CONFIG1_SET_CSHI(pSPIMModule, 12);
    CU_ASSERT(((pSPIMModule->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSHI_Msk) >> SPIM_HYPER_CONFIG1_CSHI_Pos) == 12);
    pSPIMModule->HYPER_CONFIG1 = 0;
    SPIM_HYPER_CONFIG1_SET_CSHI(pSPIMModule, 15);
    CU_ASSERT(((pSPIMModule->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSHI_Msk) >> SPIM_HYPER_CONFIG1_CSHI_Pos) == 15);

    /*
     *  SPIM_HYPER_CONFIG1_SET_CSMAXLT()
     */
    pSPIMModule->HYPER_CONFIG1 = 0;
    SPIM_HYPER_CONFIG1_SET_CSMAXLT(pSPIMModule, 0x55AA);
    CU_ASSERT(SPIM_HYPER_CONFIG1_GET_CSMAXLT(pSPIMModule) == 0x55AA);
    pSPIMModule->HYPER_CONFIG1 = 0;
    SPIM_HYPER_CONFIG1_SET_CSMAXLT(pSPIMModule, 0xAA55);
    CU_ASSERT(SPIM_HYPER_CONFIG1_GET_CSMAXLT(pSPIMModule) == 0xAA55);

    CU_PASS();
}

void MACRO_SPIM_HYPER_CONFIG2()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    /*
     *  SPIM_HYPER_CONFIG2_SET_ACCTWR()
     */
    pSPIMModule->HYPER_CONFIG2 = 0;
    SPIM_HYPER_CONFIG2_SET_ACCTWR(pSPIMModule, 0x1);
    CU_ASSERT(SPIM_HYPER_CONFIG2_GET_ACCTWR(pSPIMModule) == 0x1);
    pSPIMModule->HYPER_CONFIG2 = 0;
    SPIM_HYPER_CONFIG2_SET_ACCTWR(pSPIMModule, 0x1F);
    CU_ASSERT(SPIM_HYPER_CONFIG2_GET_ACCTWR(pSPIMModule) == 0x1F);

    /*
     *  SPIM_HYPER_CONFIG2_SET_RSTNLT()
     */
    pSPIMModule->HYPER_CONFIG2 = 0;
    SPIM_HYPER_CONFIG2_SET_RSTNLT(pSPIMModule, 0x10);
    CU_ASSERT(SPIM_HYPER_CONFIG2_GET_RSTNLT(pSPIMModule) == 0x10);
    pSPIMModule->HYPER_CONFIG2 = 0;
    SPIM_HYPER_CONFIG2_SET_RSTNLT(pSPIMModule, 0xC0);
    CU_ASSERT(SPIM_HYPER_CONFIG2_GET_RSTNLT(pSPIMModule) == 0xC0);
    pSPIMModule->HYPER_CONFIG2 = 0;
    SPIM_HYPER_CONFIG2_SET_RSTNLT(pSPIMModule, 0xFF);
    CU_ASSERT(SPIM_HYPER_CONFIG2_GET_RSTNLT(pSPIMModule) == 0xFF);

    /*
     *  SPIM_HYPER_CONFIG2_SET_ACCTRD()
     */
    pSPIMModule->HYPER_CONFIG2 = 0;
    SPIM_HYPER_CONFIG2_SET_ACCTRD(pSPIMModule, 0x1);
    CU_ASSERT(SPIM_HYPER_CONFIG2_GET_ACCTRD(pSPIMModule) == 0x1);
    pSPIMModule->HYPER_CONFIG2 = 0;
    SPIM_HYPER_CONFIG2_SET_ACCTRD(pSPIMModule, 0x1F);
    CU_ASSERT(SPIM_HYPER_CONFIG2_GET_ACCTRD(pSPIMModule) == 0x1F);

    CU_PASS();
}

void MACRO_SPIM_HYPER_INTEN()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    /*
     *  SPIM_HYPER_ENABLE_INT()
     */
    pSPIMModule->HYPER_INTEN = 0;
    SPIM_HYPER_ENABLE_INT(pSPIMModule);
    CU_ASSERT_TRUE(SPIM_HYPER_GET_INT(pSPIMModule));

    pSPIMModule->HYPER_INTEN = 0;
    SPIM_HYPER_DISABLE_INT(pSPIMModule);
    CU_ASSERT_FALSE(SPIM_HYPER_GET_INT(pSPIMModule));
}

CU_TestInfo SPIM_HyperConstantTests[] =
{
    {"SPIM CONST HYOER REG", SPIM_Const_HYPER_REG},
    {"SPIM CONST HYOER CMD", SPIM_Const_HYPER_CMD},
    {"SPIM CONST HYOER CONFIG1", SPIM_Const_HYPER_CONFIG1},

    CU_TEST_INFO_NULL,
};

CU_TestInfo SPIM_HyperMacroTests[] =
{
    {"HYPER_CONFIG1 Macros", MACRO_SPIM_HYPER_CONFIG1},
    {"HYPER_CONFIG2 Macros", MACRO_SPIM_HYPER_CONFIG2},
    {"HYPER_INT Macros", MACRO_SPIM_HYPER_INTEN},

    CU_TEST_INFO_NULL,
};

CU_TestInfo SPIM_HyperApiTests[] =
{
    {"SPIM Hyper DLL Training Function", TrainingDllLatency},
    {"SPIM Hyper DMA Function", SPIM_HyperDMA_Func},
    {"SPIM Hyper DMM Function", SPIM_HyperDMM_Func},

    CU_TEST_INFO_NULL
};

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
CU_SuiteInfo spim_hyper_suites[] =
{
    //{"SPIM HYPER CONST", SPIM_Tests_HyperInit, SPIM_Tests_HyperClean, NULL, NULL, SPIM_HyperConstantTests},
    //{"SPIM HYPER MACRO", SPIM_Tests_HyperInit, SPIM_Tests_HyperClean, NULL, NULL, SPIM_HyperMacroTests},
    {"SPIM HYPER API", SPIM_Tests_HyperInit, SPIM_Tests_HyperClean, NULL, NULL, SPIM_HyperApiTests},

    CU_SUITE_INFO_NULL,
};
