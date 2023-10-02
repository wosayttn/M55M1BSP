/**************************************************************************//**
 * @file     hyperram_code.c
 * @version  V1.00
 * @brief    Collect of sub-routines running on SPIM flash.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NuMicro.h"
#include "hyperram_code.h"

//------------------------------------------------------------------------------
__attribute__((aligned(32))) uint8_t g_au8SrcArray[BUFF_SIZE] = {0};
__attribute__((aligned(32))) uint8_t g_au8DestArray[BUFF_SIZE] = {0};

//------------------------------------------------------------------------------
/**
 * @brief Erase and check HyperRAM
 *
 * @param spim
 * @param u32StartAddr  erase start address
 * @param u32EraseSize  erase size
 */
void HyperRAM_Erase(SPIM_T *spim, uint32_t u32StartAddr, uint32_t u32EraseSize)
{
    uint16_t u16Data;
    uint32_t u32i = 0;
    uint32_t u32RemainSize = (u32EraseSize % 2);

    for (u32i = 0; u32i <= (u32EraseSize - u32RemainSize); u32i += 2)
    {
        SPIM_HYPER_Write2Byte(spim, (u32StartAddr + u32i), 0x0000);
        u16Data = SPIM_HYPER_Read1Word(spim, (u32StartAddr + u32i));

        if (u16Data != 0x0000)
        {
            printf("Erase Hyper RAM fail!! Read address:0x%08x, data::0x%08x  expect: 0\n",
                   u32StartAddr + u32i,
                   u16Data);

            while (1);
        }
    }

    if (u32RemainSize != 0)
    {
        SPIM_HYPER_Write1Byte(spim, (u32StartAddr + (u32EraseSize - 1)), 0x00);
        u16Data = SPIM_HYPER_Read1Word(spim, (u32StartAddr + u32EraseSize));

        if ((u16Data & 0xFF) != 0)
        {
            printf("Erase Remain Hyper RAM fail!! Read address:0x%08x, data::0x%08x  expect: 0\n",
                   (u32StartAddr + (u32EraseSize - 1)),
                   u16Data);

            while (1);
        }
    }
}

/**
 * @brief Training DLL component delay stop number
 *
 * @param spim
 */
void HyperRAM_TrainingDelayNumber(SPIM_T *spim)
{
    uint8_t u8RdDelay = 0;
    uint8_t u8Temp;
    uint8_t u8RdDelayIdx = 0;
    uint8_t u8RdDelayRes[SPIM_MAX_DLL_LATENCY] = {0};
    uint32_t u32i = 0;
    uint32_t u32j;
    uint32_t u32SrcAddr = 0;
    uint32_t u32TestSize = 32;
    uint32_t u32DMMAddr = SPIM_HYPER_GetDMMAddress(spim);
    int *pi32SrcAddr = (int *)(u32DMMAddr + u32SrcAddr);

    /* Erase HyperRAM */
    HyperRAM_Erase(spim, u32SrcAddr, u32TestSize);

    /* Write Data to HyperRAM */
    for (u32i = u32SrcAddr; u32i < u32TestSize; u32i++)
    {
        g_au8SrcArray[u32i] = (u32i + 0x01);
        SPIM_HYPER_Write1Byte(spim, u32i, g_au8SrcArray[u32i]);
    }

    //SPIM_HYPER_EnterDirectMapMode(spim);

    for (u8RdDelay = 0; u8RdDelay <= SPIM_MAX_DLL_LATENCY; u8RdDelay++)
    {
        memset(g_au8DestArray, 0, sizeof(g_au8DestArray));

        /* Set DLL calibration to select the valid delay step number */
        SPIM_HYPER_SetDLLDelayNum(spim, u8RdDelay);

        /* Read Data from HyperRAM */
        SPIM_HYPER_DMARead(spim, u32SrcAddr, g_au8DestArray, u32TestSize);
        //memcpy(g_au8DestArray, pi32SrcAddr, u32TestSize);

        /* Verify the data and save the number of successful delay steps */
        if (memcmp(g_au8SrcArray, g_au8DestArray, u32TestSize))
        {
            printf("!!!\tData compare failed at block 0x%x\n", u32SrcAddr);
        }
        else
        {
            printf("Delay Step Num : %d = Pass\r\n", u8RdDelay);
            u8RdDelayRes[u8RdDelayIdx++] = u8RdDelay;
        }
    }

    SPIM_HYPER_ExitDirectMapMode(spim);

    if (u8RdDelayIdx <= 1)
    {
        u8RdDelayIdx = 0;
    }
    else
    {
        if (u8RdDelayIdx >= 2)
        {
            u8RdDelayIdx = (u8RdDelayIdx / 2);
        }
        else
        {
            u8RdDelayIdx = 1;
        }
    }

    /* Set the number of intermediate delay steps */
    SPIM_HYPER_SetDLLDelayNum(spim, u8RdDelayRes[u8RdDelayIdx]);
}

/**
  * @brief      SPIM Default Config HyperBus Access Module Parameters.
  * @param      spim
  * @param      u32CSMaxLT Chip Select Maximum Low Time 0 ~ 0xFFFF, Default Set 0x02ED
  * @param      u32AcctRD Initial Read Access Time 1 ~ 0x1F, Default Set 0x04
  * @param      u32AcctWR Initial Write Access Time 1 ~ 0x1F, Default Set 0x04
  * @return     None.
  */
void SPIM_Hyper_DefaultConfig(SPIM_T *spim, uint32_t u32CSMaxLow, uint32_t u32AcctRD, uint32_t u32AcctWR)
{
    /* Chip Select Setup Time 2.5 */
    SPIM_HYPER_SET_CSST(spim, SPIM_HYPER_CSST_2_5_HCLK);

    /* Chip Select Hold Time 3.5 HCLK */
    SPIM_HYPER_SET_CSH(spim, SPIM_HYPER_CSH_3_5_HCLK);

    /* Chip Select High between Transaction as 2 HCLK cycles */
    SPIM_HYPER_SET_CSHI(spim, 2);

    /* Chip Select Masximum low time HCLK */
    SPIM_HYPER_SET_CSMAXLT(spim, u32CSMaxLow);

    /* Initial Device RESETN Low Time 255 */
    SPIM_HYPER_SET_RSTNLT(spim, 0xFF);

    /* Initial Read Access Time Clock cycle*/
    SPIM_HYPER_SET_ACCTRD(spim, u32AcctRD);

    /* Initial Write Access Time Clock cycle*/
    SPIM_HYPER_SET_ACCTWR(spim, u32AcctWR);
}

void HyperRAM_Init(SPIM_T *spim)
{
    /* Enable SPIM Hyper Bus Mode */
    SPIM_HYPER_Init(spim, 1);

    /* SPIM Def. Enable Cipher, First Disable the test. */
    SPIM_DISABLE_CIPHER(spim);

    /* Reset HyperRAM */
    SPIM_HYPER_Reset(spim);

    /* Set R/W Latency Number */
    SPIM_Hyper_DefaultConfig(spim, 780, 7, 7);

    /* Training DLL component delay stop number */
    HyperRAM_TrainingDelayNumber(spim);

    SPIM_HYPER_EnterDirectMapMode(spim);

#if (SPIM_REG_CACHE == 1) //TESTCHIP_ONLY not support
    /* Enable SPIM Cache */
    SPIM_HYPER_ENABLE_CACHE(spim);
#endif
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
