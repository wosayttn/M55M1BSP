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

/**
 * @brief Send Hyper Flash Operation Command
 *
 * @param spim
 * @param u32CMD
 * @param u32CMDData
 */
void HyperFlash_WriteOPCMD(SPIM_T *spim, uint32_t u32CMD, uint32_t u32Addr)
{
    SPIM_Write2Byte(spim, (u32CMD * 2), u32Addr);
}

void HyperFlash_WriteNVCRegister(SPIM_T *spim, uint32_t u32WrData)
{
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, WRITE_NVCR_REG);

    SPIM_Write2Byte(spim, CMD_NOOP_CODE, u32WrData);
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

    return SPIM_Read2Word(spim, CMD_NOOP_CODE);
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

        u32Status = (SPIM_Read1Word(spim, CMD_NOOP_CODE) & 0x80);

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

#if (SPIM_CACHE_EN == 1)
    SPIM_INVALID_CACHE(spim)  // invalid cache
#endif //SPIM_CACHE_EN
}

void HyperFlash_DMA_WriteByPage(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvWrBuf, uint32_t u32NTx)
{
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_A0);

    SPIM_DMAWrite_Hyper(pSPIMx, u32SAddr, pvWrBuf, u32NTx);

    HyperFlash_WaitBusBusy(pSPIMx);
}

void HyperFlash_DMAWrite(SPIM_T *pSPIMx, uint32_t u32SAddr, uint8_t *pu8WrBuf, uint32_t u32NTx)
{
    uint32_t   pageOffset, toWr;
    uint32_t   buf_idx = 0UL;

    pageOffset = u32SAddr % HFLH_PAGE_SIZE;

    if ((pageOffset + u32NTx) <= HFLH_PAGE_SIZE)
    {
        /* Do all the bytes fit onto one page ? */
        HyperFlash_DMA_WriteByPage(pSPIMx, u32SAddr, pu8WrBuf, u32NTx);
    }
    else
    {
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

void HyperFlash_DMARead(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvRdBuf, uint32_t u32NRx)
{
    SPIM_DMARead_Hyper(pSPIMx, u32SAddr, pvRdBuf, u32NRx);
}

/**
 * @brief Training DLL component delay stop number
 *
 * @param spim
 */
void SPIM_TrainingDLLDelayTime(SPIM_T *spim)
{
    uint8_t u8RdDelay = 0;
    uint8_t u8Temp = 0;
    uint8_t u8RdDelayIdx = 0;
    uint8_t u8RdDelayRes[SPIM_MAX_DLL_LATENCY] = {0};
    uint32_t u32i = 0;
    uint32_t u32j = 0;
    uint32_t u32SrcAddr = 0;
    uint32_t u32TestSize = 32;
    uint32_t u32SrcArray[32] = {0};
    uint32_t u32DestArray[32] = {0};
    uint32_t u32DMMAddr = SPIM_GetDirectMapAddress(spim);
    int *pi32SrcAddr = (int *)(u32DMMAddr + u32SrcAddr);

    /* Erase HyperFlash */
    HyperFlash_EraseSector(spim, 0); //one sector = 256KB

    HyperFlash_DMAWrite(spim, u32SrcAddr, (uint8_t *)u32SrcArray, u32TestSize);

    /* Enter direct-mapped mode to run new applications */
    SPIM_EnterDirectMapMode_Hyper(spim);

    for (u8RdDelay = 0; u8RdDelay <= SPIM_MAX_DLL_LATENCY; u8RdDelay++)
    {
        /* Set DLL calibration to select the valid delay step number */
        SPIM_CtrlDLLDelayTime(spim, 0, 0, 0, 0, u8RdDelay);

        /* Read Data from HyperFlash */
        //HyperFlash_DMARead(spim, u32SrcAddr, u32DestArray, u32TestSize);
        memcpy(u32DestArray, pi32SrcAddr, u32TestSize);

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

    /* Sort delay step number */
    for (u32i = 0 ; u32i <= u8RdDelayIdx ; u32i = u32i + 1)
    {
        for (u32j = u32i + 1 ; u32j < u8RdDelayIdx ; u32j = u32j + 1)
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

    /* Set the number of intermediate delay steps */
    SPIM_CtrlDLLDelayTime(spim, 0, 0, 0, 0, u8RdDelayRes[u8RdDelayIdx]);
}

/**
  * @brief      SPIM Default Config HyperBus Access Module Parameters.
  * @param      spim
  * @param      u32CSMaxLT Chip Select Maximum Low Time 0 ~ 0xFFFF, Default Set 0x02ED
  * @param      u32AcctRD Initial Read Access Time 1 ~ 0x1F, Default Set 0x04
  * @param      u32AcctWR Initial Write Access Time 1 ~ 0x1F, Default Set 0x04
  * @return     None.
  */
void SPIM_HyperFlash_DefaultConfig(SPIM_T *spim, uint32_t u32CSMaxLow, uint32_t u32AcctRD, uint32_t u32AcctWR)
{
    /* Chip Select Setup Time 2.5 */
    SPIM_SET_HYPER_CONFIG1_CSST(spim, SPIM_HYPER_CONFIG1_CSST_2_5_HCLK);

    /* Chip Select Hold Time 3.5 HCLK */
    SPIM_SET_HYPER_CONFIG1_CSH(spim, SPIM_HYPER_CONFIG1_CSH_3_5_HCLK);

    /* Chip Select High between Transaction as 2 HCLK cycles */
    SPIM_SET_HYPER_CONFIG1_CSHI(spim, 2);

    /* Chip Select Masximum low time HCLK */
    SPIMS_SET_HYPER_CONFIG1_CSMAXLT(spim, u32CSMaxLow);

    /* Initial Device RESETN Low Time 255 */
    SPIM_SET_HYPER_CONFIG2_RSTNLT(spim, 0xFF);

    /* Initial Read Access Time Clock cycle*/
    SPIM_SET_HYPER_CONFIG2_ACCTRD(spim, u32AcctRD);

    /* Initial Write Access Time Clock cycle*/
    SPIM_SET_HYPER_CONFIG2_ACCTWR(spim, u32AcctWR);
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
    SPIM_HyperFlash_DefaultConfig(spim, HFLH_MAX_CS_LOW, 16, HFLH_WR_ACCTIME);

    SPIM_CtrlDLLDelayTime(spim, 0, 0, 0, 0, 1);

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
    SPIM_HyperFlash_DefaultConfig(spim, HFLH_MAX_CS_LOW, u32Latency, HFLH_WR_ACCTIME);

    /* Check if the new setup was successful */
    u32RegValue = HyperFlash_ReadNVCRegister(spim);
    printf("Verify NVC Reg = %x\r\n", u32RegValue);

    HyperFlash_WaitBusBusy(spim);
}

void HyperFlash_Init(SPIM_T *spim)
{
    /* Enable SPIM Hyper Bus Mode */
    SPIM_InitHyper(spim, 2);

#if (SPIM_CACHE_EN == 1)
    /* Enable SPIM Cache */
    SPIM_DISABLE_CACHE(spim);
#endif //SPIM_CACHE_EN

    /* SPIM Def. Enable Cipher, First Disable the test. */
    SPIM_DISABLE_CIPHER(spim);

    SPIM_ResetHyper(spim);

    HyperFlash_SetReadLatency(spim, 9);

    SPIM_TrainingDLLDelayTime(spim);
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

        //SPIM0 PSC, PSC_n
        SET_GPIO_PG8();
        GPIO_SetMode(PG, BIT8, GPIO_MODE_OUTPUT);
        GPIO_SetPullCtl(PG, BIT8, GPIO_PUSEL_DISABLE);
        PG8 = 1;

        SET_GPIO_PG7();
        GPIO_SetMode(PG, BIT7, GPIO_MODE_OUTPUT);
        GPIO_SetPullCtl(PG, BIT7, GPIO_PUSEL_DISABLE);
        PG7 = 0;
    }
    else if (spim == SPIM1)
    {
     	//SPIM and OTFC clock was enabled on secure-domain code
        /* Enable SPIM1 module clock */
        CLK_EnableModuleClock(SPIM1_MODULE);
        /* Enable OTFC1 module clock */
        CLK_EnableModuleClock(OTFC1_MODULE);

        /* Init SPIM1 multi-function pins */
        SPIM1_CLK_PIN_INIT();
        SPIM1_CLKN_PIN_INIT();
        SPIM1_D2_PIN_INIT();
        SPIM1_D3_PIN_INIT();
        SPIM1_D4_PIN_INIT();
        SPIM1_D5_PIN_INIT();
        SPIM1_D6_PIN_INIT();
        SPIM1_D7_PIN_INIT();
        SPIM1_MISO_PIN_INIT();
        SPIM1_MOSI_PIN_INIT();
        SPIM1_SS_PIN_INIT();
        SPIM1_RWDS_PIN_INIT();

        /* Set SPIM1 I/O pins as high slew rate up to 80 MHz. */
        SPIM1_PIN_HIGH_SLEW();

        //SPIM1 PSC, PSC_n
        SET_GPIO_PH11();
        GPIO_SetMode(PH, BIT11, GPIO_MODE_OUTPUT);
        GPIO_SetPullCtl(PH, BIT11, GPIO_PUSEL_DISABLE);
        PH11 = 1;

        SET_GPIO_PH10();
        GPIO_SetMode(PH, BIT10, GPIO_MODE_OUTPUT);
        GPIO_SetPullCtl(PH, BIT11, GPIO_PUSEL_DISABLE);
        PH10 = 0;
    }
}

/*** (C) COPYRIGHT 2021 Nuvoton Technology Corp. ***/