/**************************************************************************//**
 * @file     hyperflash_code.c
 * @version  V1.00
 * @brief    Collect of sub-routines running on SPIM flash.
 *
 *
 * @copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>

#include "NuMicro.h"
#include "hyperflash_code.h"

/**
 * @brief Send Hyper Flash Operation Command
 *
 * @param spim
 * @param u32CMD
 * @param u32CMDData
 */
void HyperFlash_WriteOPCMD(SPIM_T *spim, uint32_t u32CMD, uint32_t u32Addr)
{
    SPIM_Hyper_Write2Byte(spim, (u32CMD * 2), u32Addr);
}

uint32_t HyperFlash_ReadData4Byte(SPIM_T *pSPIMx, uint32_t u32Addr)
{
    uint32_t u32RdData = 0;

    u32RdData = SPIM_Hyper_Read2Word(pSPIMx, u32Addr);

    return u32RdData;
}

uint16_t HyperFlash_ReadData2Byte(SPIM_T *pSPIMx, uint32_t u32Addr)
{
    uint16_t u16Data = 0;

    u16Data = SPIM_Hyper_Read1Word(pSPIMx, u32Addr);

    return (u16Data & 0xFFFF);
}

uint16_t HyperFlash_ReadData4CmdSets(SPIM_T *spim, uint32_t u32LastCMD, uint32_t u32RdAddr)
{
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, u32LastCMD);

    return HyperFlash_ReadData4Byte(spim, u32RdAddr);
}

uint16_t HyperFlash_ReadData1CmdSets2Byte(SPIM_T *spim, uint32_t u32LastCMD, uint32_t u32RdAddr)
{
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, u32LastCMD);

    return HyperFlash_ReadData2Byte(spim, u32RdAddr);
}

/**
 * @brief Wait Hyper Flash Program Busy
 *
 * @param spim
 */
void HyperFlash_WaitBusBusy(SPIM_T *spim)
{
    while (HyperFlash_ReadData1CmdSets2Byte(spim, CMD_70, CMD_NOOP_CODE) != 0x80);
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

    SPIM_Hyper_DMAWrite(pSPIMx, u32SAddr, pvWrBuf, u32NTx);

    HyperFlash_WaitBusBusy(pSPIMx);
}

void HyperFlash_DMAWrite(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvWrBuf, uint32_t u32NTx)
{
    uint32_t   pageOffset, toWr;
    uint32_t   buf_idx = 0UL;

    pageOffset = u32SAddr % HFLH_PAGE_SIZE;

    if ((pageOffset + u32NTx) <= HFLH_PAGE_SIZE)
    {
        /* Do all the bytes fit onto one page ? */
        HyperFlash_DMA_WriteByPage(pSPIMx, u32SAddr, pvWrBuf, u32NTx);
    }
    else
    {
        toWr = HFLH_PAGE_SIZE - pageOffset;               /* Size of data remaining on the first page. */

        HyperFlash_DMA_WriteByPage(pSPIMx, u32SAddr, &pvWrBuf[buf_idx], toWr);

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

            HyperFlash_DMA_WriteByPage(pSPIMx, u32SAddr, &pvWrBuf[buf_idx], toWr);

            u32SAddr += toWr;                 /* Advance indicator. */
            u32NTx -= toWr;
            buf_idx += toWr;
        }
    }
}

void HyperFlash_DMARead(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvRdBuf, uint32_t u32NRx)
{
    SPIM_Hyper_DMARead(pSPIMx, u32SAddr, pvRdBuf, u32NRx);
}

/*** (C) COPYRIGHT 2021 Nuvoton Technology Corp. ***/
