/**************************************************************************//**
 * @file     hyperflash_code.c
 * @version  V1.00
 * @brief    Collect of sub-routines running on SPIM flash.
 *
 *
 * @copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NuMicro.h"
#include "hyperflash_code.h"

#include "SPIM_PinConfig.h"

//------------------------------------------------------------------------------
__attribute__((aligned(32))) uint8_t g_au8SrcArray[HFLH_PAGE_SIZE] = {0};
__attribute__((aligned(32))) uint8_t g_au8DestArray[HFLH_PAGE_SIZE] = {0};

//------------------------------------------------------------------------------
/**
 * @brief Send Hyper Flash Operation Command
 *
 * @param spim
 * @param u32CMD
 * @param u32CMDData
 */
void HyperFlash_WriteOPCMD(SPIM_T *spim, uint32_t u32CMD, uint32_t u32Addr)
{
    SPIM_HYPER_Write2Byte(spim, (u32CMD * 2), u32Addr);
}

void HyperFlash_WriteNVCRegister(SPIM_T *spim, uint32_t u32WrData)
{
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, WRITE_NVCR_REG);

    SPIM_HYPER_Write2Byte(spim, CMD_NOOP_CODE, u32WrData);
}

/**
 * @brief Read HyperFlash Non-volatile config register
 *
 * @param spim
 * @return uint16_t Register value
 */
uint16_t HyperFlash_ReadNVCRegister(SPIM_T *spim)
{
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, READ_NVCR_REG);

    return SPIM_HYPER_Read2Word(spim, CMD_NOOP_CODE);
}

/**
 * @brief Wait Hyper Flash Program Busy
 *
 * @param spim
 */
int32_t HyperFlash_WaitBusBusy(SPIM_T *spim)
{
    int32_t i32Timeout = SPIM_TIMEOUT;
    uint32_t u32Status = 0;

    while (u32Status != 0x80)
    {
        HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, CMD_70);

        u32Status = (SPIM_HYPER_Read1Word(spim, CMD_NOOP_CODE) & 0x80);

        if (i32Timeout-- <= 0)
        {
            return SPIM_ERR_TIMEOUT;
        }
    }

    return SPIM_OK;
}

void HyperFlash_EraseSector(SPIM_T *spim, uint32_t u32SAddr)
{
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, CMD_80);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(spim, u32SAddr, CMD_30);

    HyperFlash_WaitBusBusy(spim);

#if 0 //(SPIM_REG_CACHE == 1)
    SPIM_INVALID_CACHE(spim);  // invalid cache
#endif //SPIM_CACHE_EN
}

void HyperFlash_DMA_WriteByPage(SPIM_T *pSPIMx, uint32_t u32SAddr, uint8_t *pu8WrBuf, uint32_t u32NTx)
{
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_A0);

    SPIM_HYPER_DMAWrite(pSPIMx, u32SAddr, pu8WrBuf, u32NTx);

    HyperFlash_WaitBusBusy(pSPIMx);
}

void HyperFlash_DMAWrite(SPIM_T *pSPIMx, uint32_t u32SAddr, uint8_t *pu8WrBuf, uint32_t u32NTx)
{
    uint32_t pageOffset = 0;

    pageOffset = u32SAddr % HFLH_PAGE_SIZE;

    if ((pageOffset + u32NTx) <= HFLH_PAGE_SIZE)
    {
        /* Do all the bytes fit onto one page ? */
        HyperFlash_DMA_WriteByPage(pSPIMx, u32SAddr, pu8WrBuf, u32NTx);
    }
    else
    {
        uint32_t toWr = 0;
        uint32_t buf_idx = 0;

        toWr = HFLH_PAGE_SIZE - pageOffset;               /* Size of data remaining on the first page. */

        HyperFlash_DMA_WriteByPage(pSPIMx, u32SAddr, &pu8WrBuf[buf_idx], toWr);

        u32SAddr += toWr;                         /* Advance indicator. */
        u32NTx -= toWr;
        buf_idx += toWr;

        while (u32NTx)
        {
            toWr = HFLH_PAGE_SIZE;

            if (toWr > u32NTx)
            {
                toWr = u32NTx;
            }

            HyperFlash_DMA_WriteByPage(pSPIMx, u32SAddr, &pu8WrBuf[buf_idx], toWr);

            u32SAddr += toWr;                 /* Advance indicator. */
            u32NTx -= toWr;
            buf_idx += toWr;
        }
    }
}

void HyperFlash_DMARead(SPIM_T *pSPIMx, uint32_t u32SAddr, uint8_t *pu8RdBuf, uint32_t u32NRx)
{
    SPIM_HYPER_DMARead(pSPIMx, u32SAddr, pu8RdBuf, u32NRx);
}

/**
 * @brief Training DLL component delay stop number
 *
 * @param spim
 */
void SPIM_TrainingDLLDelayTime(SPIM_T *spim)
{
    uint8_t u8RdDelay = 0;
    uint8_t u8Temp;
    uint8_t u8RdDelayIdx = 0;
    uint8_t u8RdDelayRes[SPIM_MAX_DLL_LATENCY] = {0};
    uint32_t u32i = 0;
    uint32_t u32j;
    uint32_t u32SrcAddr = 0;
    uint32_t u32TestSize = 32;
    uint8_t u32SrcArray[32] = {0};
    uint8_t u32DestArray[32] = {0};
    //uint32_t u32DMMAddr = SPIM_HYPER_GetDMMAddress(spim);
    //int *pi32SrcAddr = (int *)(u32DMMAddr + u32SrcAddr);

    /* Erase HyperFlash */
    HyperFlash_EraseSector(spim, 0); //one sector = 256KB

    HyperFlash_DMAWrite(spim, u32SrcAddr, u32SrcArray, u32TestSize);

    /* Enter direct-mapped mode to run new applications */
    //SPIM_EnterDirectMapMode_Hyper(spim);

    for (u8RdDelay = 0; u8RdDelay <= SPIM_MAX_DLL_LATENCY; u8RdDelay++)
    {
        /* Set DLL calibration to select the valid delay step number */
        SPIM_HYPER_SetDLLDelayNum(spim, u8RdDelay);

        /* Read Data from HyperFlash */
        HyperFlash_DMARead(spim, u32SrcAddr, u32DestArray, u32TestSize);
        //memcpy(u32DestArray, pi32SrcAddr, u32TestSize);

        /* Verify the data and save the number of successful delay steps */
        if (memcmp(u32SrcArray, u32DestArray, u32TestSize))
        {
            printf("!!!\tData compare failed at block 0x%x\n", u32SrcAddr);
        }
        else
        {
            printf("RX Delay: %d = Pass\r\n", u8RdDelay);
            u8RdDelayRes[u8RdDelayIdx++] = u8RdDelay;
        }
    }

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


static uint32_t HyperFlash_GetLatencyNum(uint32_t u32Latency)
{
    if (u32Latency < 5)
    {
        u32Latency = 5;
    }
    else if (u32Latency > 16)
    {
        u32Latency = 16;
    }

    return (u32Latency - 5);
}

void HyperFlash_SetReadLatency(SPIM_T *spim, uint32_t u32Latency)
{
    uint32_t u32RegValue = 0;

    /* HyperFlash default read latency is 16 and write is always 1 */
    SPIM_Hyper_DefaultConfig(spim, HFLH_MAX_CS_LOW, 16, HFLH_WR_ACCTIME);

    SPIM_HYPER_SetDLLDelayNum(spim, 1);

    /* Read non-volatile config register default value */
    u32RegValue = HyperFlash_ReadNVCRegister(spim);
    printf("NVC Reg = %x\r\n", u32RegValue);

    /* clear latency bits */
    u32RegValue &= ~(0xF << 4);
    /* Set new latency */
    u32RegValue |= (HyperFlash_GetLatencyNum(u32Latency) << 4);

    /* Write to non-volatile config register */
    HyperFlash_WriteNVCRegister(spim, u32RegValue);

    /* SPIM set new latency value */
    SPIM_Hyper_DefaultConfig(spim, HFLH_MAX_CS_LOW, u32Latency, HFLH_WR_ACCTIME);

    /* Check if the new setup was successful */
    u32RegValue = HyperFlash_ReadNVCRegister(spim);
    printf("Verify NVC Reg = %x\r\n", u32RegValue);

    HyperFlash_WaitBusBusy(spim);
}

void HyperFlash_Init(SPIM_T *spim)
{
    /* Enable SPIM Hyper Bus Mode */
    SPIM_HYPER_Init(spim, 1);

    /* SPIM Def. Enable Cipher, First Disable the test. */
    SPIM_DISABLE_CIPHER(spim);

    SPIM_HYPER_Reset(spim);

    HyperFlash_SetReadLatency(spim, 9);

    //    SPIM_TrainingDLLDelayTime(spim);
    SPIM_HYPER_SetDLLDelayNum(spim, 1);

#if (SPIM_REG_CACHE == 1)
    {
        printf("Enable SPIM cache \n");
        SPIM_ENABLE_CACHE(spim);
        SPIM_SET_UPDCLNUM(spim, 4);
    }
#endif

}

void HyperFlash_PinConfig(SPIM_T *spim)
{
    if (spim == SPIM0)
    {
        //SPIM and OTFC clock was enabled on secure-domain code
        /* Enable SPIM0 module clock */
        CLK_EnableModuleClock(SPIM0_MODULE);
        /* Enable OTFC0 module clock */
        CLK_EnableModuleClock(OTFC0_MODULE);
        //printf("OTFCCTL = %d, addr = 0x%08X\r\n", CLK->OTFCCTL, &CLK->OTFCCTL);

        /* Init SPIM0 multi-function pins */
        SPIM0_CLK_PIN_INIT();
        SPIM0_CLKN_PIN_INIT();
        SPIM0_D2_PIN_INIT();
        SPIM0_D3_PIN_INIT();
        SPIM0_D4_PIN_INIT();
        SPIM0_D5_PIN_INIT();
        SPIM0_D6_PIN_INIT();
        SPIM0_D7_PIN_INIT();
        SPIM0_MISO_PIN_INIT();
        SPIM0_MOSI_PIN_INIT();
        SPIM0_SS_PIN_INIT();
        SPIM0_RWDS_PIN_INIT();

        /* Set SPIM0 I/O pins as high slew rate up to 80 MHz. */
        SPIM0_PIN_HIGH_SLEW();
    }
}

/*** (C) COPYRIGHT 2021 Nuvoton Technology Corp. ***/
