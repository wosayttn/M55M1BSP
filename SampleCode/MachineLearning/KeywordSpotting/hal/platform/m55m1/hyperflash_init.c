#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "NuMicro.h"
#include "hyperflash_init.h"
#include "SPIM_PinConfig.h"

//------------------------------------------------------------------------------
static const uint32_t patterns[] =
{
    //0x00000000, 0xFFFFFFFF, 0x55aa55aa, 0xaa55aa55, 0x33cc33cc, 0xcc33cc33
    0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666
    //0x11111111, 0x22222222, 0x33333333, 0x44444444
    //0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF
};

__attribute__((aligned(32))) uint8_t tstbuf[32] = {0};
__attribute__((aligned(32))) uint8_t tstbuf2[32] = {0};
__attribute__((aligned(32))) uint8_t tstbuf3[32] = {0};

//------------------------------------------------------------------------------
uint16_t HyperFlash_ReadData1CmdSets2Byte(SPIM_T *pSPIMx, uint32_t u32LastCMD, uint32_t u32RdAddr);
void HyperFlash_WaitBusBusy(SPIM_T *pSPIMx);

//------------------------------------------------------------------------------
void popDat(uint8_t buf[], uint32_t bufSize)
{
    uint8_t *bufInd = buf;
    uint32_t bufRmn = bufSize;

    while (bufRmn)
    {
        uint32_t nTx = sizeof(patterns);

        if (nTx > bufRmn)
        {
            nTx = bufRmn;
        }

        memcpy(bufInd, (uint8_t *) patterns, nTx);
        bufInd += nTx;                              // Advance indicator.
        bufRmn -= nTx;
    }
}

uint32_t SPIM_GetDmmMapAddr(SPIM_T *pSPIMx)
{
    if (pSPIMx == SPIM0)
    {
        return SPIM0_DMM_MAP_ADDR;
    }
    else if (pSPIMx == SPIM1)
    {
        return SPIM1_DMM_MAP_ADDR;
    }

    return SPIM0_DMM_MAP_ADDR;
}

void HyperFlash_GetNoLatencyCross(void)
{
    uint32_t u32i = 0;
    uint32_t u32j = 0;
    uint32_t u32MinLTCY = 0x5;
    uint32_t u32MaxLTCY = 0x10;
    uint32_t u32MaxAddrBit = 0x07;
    uint8_t u32CrossCnt = 0;

    for (u32i = u32MinLTCY; u32i <= u32MaxLTCY; u32i++)
    {
        u32CrossCnt = 0;

        for (u32j = 0; u32j <= u32MaxAddrBit; u32j++)
        {
            if ((16 - u32i) < (u32j & (8 - 1)))
            {
                //printf("%d Have Cross page init at 0x%x Addr\r\n", u32i, u32j);
                u32CrossCnt++;
            }
        }

        if (u32CrossCnt == 0)
        {
            printf("%d No Latency Crossing A Page Boundary\r\n", u32i);
        }
    }
}

/**
 * @brief Send Hyper Flash Operaction Command
 *
 * @param pSPIMx
 * @param u32CMD
 * @param u32CMDData
 */
void HyperFlash_WriteOPCMD(SPIM_T *pSPIMx, uint32_t u32CMD, uint32_t u32Addr)
{
    SPIM_Hyper_Write2Byte(pSPIMx, (u32CMD * 2), u32Addr);
}

void HyperFlash_ChipErase(SPIM_T *pSPIMx)
{
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_80);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_10);

    HyperFlash_WaitBusBusy(pSPIMx);
}

void HyperFlash_EraseSector(SPIM_T *pSPIMx, uint32_t u32SectorCnt)
{
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_80);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(pSPIMx, u32SectorCnt, CMD_30);

    HyperFlash_WaitBusBusy(pSPIMx);
}

/**
 * @brief Wait Hyper Flash Program Busy
 *
 * @param pSPIMx
 */
void HyperFlash_WaitBusBusy(SPIM_T *pSPIMx)
{
    while (HyperFlash_ReadData1CmdSets2Byte(pSPIMx, CMD_70, CMD_NOOP_CODE) != 0x80);
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

void HyperFlash_WriteData4CmdSets(SPIM_T *pSPIMx, uint32_t u32LastCMD,
                                  uint32_t u32WrAddr, uint32_t u32WrData,
                                  uint32_t u32WrWidth)
{
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, u32LastCMD);

    switch (u32WrWidth)
    {
        case 2:
            SPIM_Hyper_Write2Byte(pSPIMx, u32WrAddr, u32WrData);
            break;

        case 4:
            SPIM_Hyper_Write4Byte(pSPIMx, u32WrAddr, u32WrData);
            break;
    }

    HyperFlash_WaitBusBusy(pSPIMx);
}

uint16_t HyperFlash_ReadData4CmdSets(SPIM_T *pSPIMx, uint32_t u32LastCMD, uint32_t u32RdAddr)
{
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, u32LastCMD);

    return HyperFlash_ReadData4Byte(pSPIMx, u32RdAddr);
}

void HyperFlash_OP3CmdSets(SPIM_T *pSPIMx, uint32_t u32LastCMD, uint32_t u32RdAddr)
{
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(pSPIMx, u32RdAddr, u32LastCMD);
}

uint16_t HyperFlash_ReadData1CmdSets2Byte(SPIM_T *pSPIMx, uint32_t u32LastCMD, uint32_t u32RdAddr)
{
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, u32LastCMD);

    return HyperFlash_ReadData2Byte(pSPIMx, u32RdAddr);
}

uint32_t HyperFlash_ReadData1CmdSets4Byte(SPIM_T *pSPIMx, uint32_t u32LastCMD, uint32_t u32RdAddr)
{
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, u32LastCMD);

    return HyperFlash_ReadData4Byte(pSPIMx, u32RdAddr);
}

void HyperFlash_DMMWrite(SPIM_T *pSPIMx)
{
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_A0);


    if (((SPIM_GET_OPMODE(pSPIMx) & SPIM_CTL0_OPMODE_Msk) >> SPIM_CTL0_OPMODE_Pos) != 0x03)
    {
        SPIM_Hyper_EnterDirectMapMode(pSPIMx); // Hyper Mode Switch to Direct Map mode.
    }
}

void HyperFlash_DMMRead(SPIM_T *pSPIMx)
{

    if (((SPIM_GET_OPMODE(pSPIMx) & SPIM_CTL0_OPMODE_Msk) >> SPIM_CTL0_OPMODE_Pos) != 0x03)
    {
        SPIM_Hyper_EnterDirectMapMode(pSPIMx); // Hyper Mode Switch to Direct Map mode.
    }
}


void HyperFlash_DMMWritePage(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvWrBuf, uint32_t u32NTx)
{
    int *pi32SrcAddr = NULL;
    uint8_t u8Delay = 0;

    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_A0);

    SPIM_Hyper_EnterDirectMapMode(pSPIMx);

    pi32SrcAddr = (int *)(u32SAddr);
    memcpy(pi32SrcAddr, pvWrBuf, u32NTx);

    SPIM_Hyper_WaitDMMDone(pSPIMx);

    HyperFlash_WaitBusBusy(pSPIMx);
}

void HyperFlash_DMMWriteData(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvWrBuf, uint32_t u32NTx)
{
    uint32_t   pageOffset, toWr;
    uint32_t   buf_idx = 0UL;
    uint8_t    *pu8WrBuf =  (uint8_t *)pvWrBuf;
    
    pageOffset = (u32SAddr % HFLH_PAGE_SIZE);

    if ((pageOffset + u32NTx) <= HFLH_PAGE_SIZE)
    {
        /* Do all the bytes fit onto one page ? */
        HyperFlash_DMMWritePage(pSPIMx, u32SAddr, pvWrBuf, u32NTx);
        //printf("DMM W1\r\n");
    }
    else
    {
        toWr = HFLH_PAGE_SIZE - pageOffset;               /* Size of data remaining on the first page. */

        HyperFlash_DMMWritePage(pSPIMx, u32SAddr, &pu8WrBuf[buf_idx], toWr);
        //printf("DMM W2\r\n");

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

            HyperFlash_DMMWritePage(pSPIMx, u32SAddr, &pu8WrBuf[buf_idx], toWr);
            //printf("DMM W3\r\n");
            u32SAddr += toWr;                 /* Advance indicator. */
            u32NTx -= toWr;
            buf_idx += toWr;
        }
    }
}

void HyperFlash_DMMReadData(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvRdBuf, uint32_t u32NRx)
{
    int *pi32SrcAddr = NULL;

    SPIM_Hyper_EnterDirectMapMode(pSPIMx);

    pi32SrcAddr = (int *)u32SAddr;
    memcpy(pvRdBuf, pi32SrcAddr, u32NRx);

    SPIM_Hyper_WaitDMMDone(pSPIMx);
    HyperFlash_WaitBusBusy(pSPIMx);
}

void HyperFlash_IO_Write(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvWrBuf, uint32_t u32NTx)
{
    uint8_t *pu8WrBuf = (uint8_t *)pvWrBuf;
    uint16_t u16WrData = 0;
    uint16_t *pu16WrBuf = (uint16_t *)&pvWrBuf;
    uint32_t u32i = 0;
    uint32_t u32DataCnt = 0;
    uint32_t u32RemainSize = (u32NTx % 2);

    for (u32i = 0; u32i < (u32NTx - u32RemainSize); u32i += 2)
    {
        //memcpy(&u16WrData, &u8WrBuf[u32i], sizeof(uint16_t));
        //u16WrData = (uint16_t)pu16WrBuf[u32Data++];
        HyperFlash_WriteData4CmdSets(pSPIMx,
                                     CMD_A0,
                                     (u32SAddr + u32i),
                                     pu16WrBuf[u32DataCnt++],
                                     2);

        if ((u32i % HFLH_PAGE_SIZE) == 0)
        {
            HyperFlash_WaitBusBusy(pSPIMx);
        }
    }

    HyperFlash_WaitBusBusy(pSPIMx);

    if (u32RemainSize != 0)
    {
        pu8WrBuf = &pu8WrBuf[u32NTx - u32RemainSize];
        u16WrData = *pu8WrBuf;
        //outpb((u32SAddr - u32RemainSize), *pu8WrBuf);
        HyperFlash_WriteData4CmdSets(pSPIMx,
                                     CMD_A0,
                                     (u32SAddr + u32NTx),
                                     u16WrData,
                                     2);
    }
}

void HyperFlash_IO_Read(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvRdBuf, uint32_t u32NRx)
{
    uint8_t *pu8RxBuf = (uint8_t *)pvRdBuf;
    uint16_t u32DataCnt = 0;
    uint16_t *pu16RxBuf = (uint16_t *)pvRdBuf;
    uint16_t u16RdData = 0;
    uint32_t u32i = 0;
    uint32_t u32RemainSize = (u32NRx % 2);

    for (u32i = 0; u32i < (u32NRx - u32RemainSize); u32i += 2)
    {
        pu16RxBuf[u32DataCnt++] = HyperFlash_ReadData2Byte(pSPIMx, u32SAddr + u32i);
    }

    if (u32RemainSize != 0)
    {
        pu8RxBuf = &pu8RxBuf[(u32NRx - u32RemainSize)];
        //u16RdData = HyperFlash_ReadData2Byte(pSPIMx, ((u32SAddr + u32NRx)- u32RemainSize));
        *pu8RxBuf = ((HyperFlash_ReadData2Byte(pSPIMx, ((u32SAddr + u32NRx) - u32RemainSize)) >> 8) & 0xFF);
    }
}

static void HyperFlash_DMA_WriteByPage(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvWrBuf, uint32_t u32NTx)
{
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_A0);

    SPIM_Hyper_DMAWrite(pSPIMx, u32SAddr, pvWrBuf, u32NTx);
}

void HyperFlash_DMAWrite(SPIM_T *pSPIMx, uint32_t u32SAddr, void *pvWrBuf, uint32_t u32NTx)
{
    uint32_t   pageOffset, toWr;
    uint32_t   buf_idx = 0UL;
    uint8_t *pu8WrBuf = (uint8_t *)pvWrBuf;

    pageOffset = u32SAddr % HFLH_PAGE_SIZE;

    if ((pageOffset + u32NTx) <= HFLH_PAGE_SIZE)
    {
        /* Do all the bytes fit onto one page ? */
        //SPIM_WriteInPageDataByPageWrite(spim, u32SAddr, is4ByteAddr, u32NTx, pu8TxBuf, wrCmd, 1);
        HyperFlash_DMA_WriteByPage(pSPIMx, u32SAddr, pu8WrBuf, u32NTx);
    }
    else
    {
        toWr = HFLH_PAGE_SIZE - pageOffset;               /* Size of data remaining on the first page. */

        //SPIM_WriteInPageDataByPageWrite(spim, u32SAddr, is4ByteAddr, toWr, &pu8TxBuf[buf_idx], wrCmd, 1);
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

            //SPIM_WriteInPageDataByPageWrite(spim, u32SAddr, is4ByteAddr, toWr, &pu8TxBuf[buf_idx], wrCmd, 1);
            HyperFlash_DMA_WriteByPage(pSPIMx, u32SAddr, &pu8WrBuf[buf_idx], toWr);

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

void HyperFlash_ResetModule(SPIM_T *pSPIMx)
{
    uint32_t u32Delay = 0;

    SPIM_Hyper_ResetHyperRAM(pSPIMx);

    for (u32Delay = 0; u32Delay < 0x800; u32Delay++) {}
}

int SPIM_HyperFlashDLLDelayTimeTraining(SPIM_T *pSPIMx)
{
    uint32_t u32i = 0;
    uint32_t u32j = 0;
    uint32_t u32OutValid =  0;
    uint32_t u32LockTime =  0;
    uint32_t u32ClkOnNum = 0;
    uint8_t u8RdDelay = 0;
    uint8_t u8Temp = 0;
    uint8_t u8RdDelayIdx = 0;
    uint8_t u8RdDelayRes[DLL_MAX_DELAY_NUM] = {0};
    uint32_t u32DLLOutVaild = 0;
    uint32_t u32RdData = 0;
    uint32_t u32Addr = 0;
    uint32_t u32Status = 0;
    uint32_t u32TestSize = 32;

    HyperFlash_EraseSector(pSPIMx, 0);
    popDat(tstbuf, u32TestSize);
    HyperFlash_IO_Write(pSPIMx, 0, tstbuf, u32TestSize);

    for (u8RdDelay = 0; u8RdDelay <= DLL_MAX_DELAY_NUM; u8RdDelay++)
    {
        SPIM_CtrlDLLDelayTime(pSPIMx, 0, 0, 0, 0, u8RdDelay);

        HyperFlash_DMARead(pSPIMx, 0, tstbuf2, u32TestSize);

        if (memcmp(tstbuf, tstbuf2, u32TestSize))
        {
            printf("!!!\tData compare failed at block 0x%x\n", 0);
        }
        else
        {
            printf("RX Delay: %d = Pass\r\n", u8RdDelay);
            u8RdDelayRes[u8RdDelayIdx++] = u8RdDelay;
        }
    }

    //[sort]
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
        u8RdDelayIdx = (u8RdDelayIdx / 2);//   -1;
    }
    else
    {
        u8RdDelayIdx = 0;
        //u8RdDelayRes[u8RdDelayIdx] = 0;
    }

    printf("\r\nDLL Delay Time Num = %d\r\n\r\n", u8RdDelayRes[u8RdDelayIdx]);

    SPIM_CtrlDLLDelayTime(pSPIMx, 0, 0, 0, 0, u8RdDelayRes[u8RdDelayIdx]);

    return 0;
}

uint32_t HyperFlash_SetLatency(uint32_t u32Latency)
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

void HyperFlash_ConfigNVCRReg(SPIM_T *pSPIMx, uint32_t u32Latency)
{
    uint32_t u32NVCRData = 0;

    //HyperFlash_GetNoLatencyCross();

    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, READ_NVCR_REG);

    u32NVCRData = SPIM_Hyper_Read1Word(pSPIMx, CMD_NOOP_CODE);
    printf("NVCR Reg = %x\r\n", u32NVCRData);

    u32NVCRData &= ~(0xF << 4);
    u32NVCRData |= (HyperFlash_SetLatency(u32Latency) << 4); //Latency
    //u32NVCRData |= (1 << 12); //Driver Drive Strength
    //HyperFlash_WriteData4CmdSets(pSPIMx, WRITE_NVCR_REG, CMD_NOOP_CODE, u32NVCRData, 2);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(pSPIMx, CMD_COMMON_555, WRITE_NVCR_REG);
    SPIM_Hyper_Write2Byte(pSPIMx, CMD_NOOP_CODE, u32NVCRData);

    SPIM_Hyper_DefConfig(pSPIMx, HFLH_MAX_CS_LOW, 9, HFLH_WR_ACCTIME);

    HyperFlash_WaitBusBusy(pSPIMx);
	//HyperFlash_ResetModule(pSPIMx);

    u32NVCRData = HyperFlash_ReadData4CmdSets(pSPIMx, READ_NVCR_REG, CMD_NOOP_CODE);
    printf("NVCR Verify NVCR Reg = %x\r\n", u32NVCRData);

    u32NVCRData = HyperFlash_ReadData4CmdSets(pSPIMx, READ_VCR_REG, CMD_NOOP_CODE);
    printf("VCR Reg = %x\r\n", u32NVCRData);
}

void SPIM_NVIC_Disable(SPIM_T *pSPIMx)
{
    if (pSPIMx == SPIM0)
    {
        NVIC_DisableIRQ(SPIM0_IRQn);
    }
    else if (pSPIMx == SPIM1)
    {
        NVIC_DisableIRQ(SPIM1_IRQn);
    }
}

void InitSPIMPort(SPIM_T *pSPIMx)
{
    if (pSPIMx == SPIM0)
    {
        /* Enable SPIM0 module clock */
        CLK_EnableModuleClock(SPIM0_MODULE);
        /* Enable OTFC0 module clock */
        CLK_EnableModuleClock(OTFC0_MODULE);
        //printf("OTFCCTL = %d, addr = 0x%08X\r\n", CLK->OTFCCTL, &CLK->OTFCCTL);

        /* Init SPIM0 multi-function pins */
        SPIM0_RST_PIN_INIT();
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
    else if (pSPIMx == SPIM1)
    {
        /* Enable SPIM1 module clock */
        CLK_EnableModuleClock(SPIM1_MODULE);
        /* Enable OTFC1 module clock */
        CLK_EnableModuleClock(OTFC1_MODULE);

        /* Init SPIM1 multi-function pins */
        SPIM1_RST_PIN_INIT();
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
    }
}

void SPIM_HyperFlash_Init(SPIM_T *pSPIMx, uint32_t u32CacheOn)
{
#if 1
    //SPIM0 PSC, PSC_n
    SET_GPIO_PG8();
    GPIO_SetMode(PG, BIT8, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PG, BIT8, GPIO_PUSEL_DISABLE);
    PG8 = 1;

    SET_GPIO_PG7();
    GPIO_SetMode(PG, BIT7, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PG, BIT7, GPIO_PUSEL_DISABLE);
    PG7 = 0;

    //SPIM1 PSC, PSC_n
    SET_GPIO_PH11();
    GPIO_SetMode(PH, BIT11, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PH, BIT11, GPIO_PUSEL_DISABLE);
    PH11 = 1;

    SET_GPIO_PH10();
    GPIO_SetMode(PH, BIT10, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PH, BIT11, GPIO_PUSEL_DISABLE);
    PH10 = 0;
#endif //0

    SPIM_NVIC_Disable(pSPIMx);

    InitSPIMPort(pSPIMx);

    SPIM_SET_HYPER_MODE(pSPIMx, 1); // Enable HyperBus Mode

    SPIM_SET_CLOCK_DIVIDER(pSPIMx, 1); /* Set SPIM clock as HCLK divided by 2 */

#if (SPIM_CACHE_EN == 1)
    SPIM_DISABLE_CACHE(pSPIMx);
#endif

    HyperFlash_ResetModule(pSPIMx);

    SPIM_Hyper_DefConfig(pSPIMx, HFLH_MAX_CS_LOW, 16, HFLH_WR_ACCTIME);
    SPIM_CtrlDLLDelayTime(pSPIMx, 0, 0, 0, 0, 2);
//    SPIM_CtrlDLLDelayTime(pSPIMx, 0, 0, 0, 0, 1); //for fast clock 500M RD design

    HyperFlash_ConfigNVCRReg(pSPIMx, 9);

    SPIM_Hyper_DefConfig(pSPIMx, HFLH_MAX_CS_LOW, 9, HFLH_WR_ACCTIME);

    //SPIM_HyperFlashDLLDelayTimeTraining(pSPIMx);
    SPIM_CtrlDLLDelayTime(pSPIMx, 0, 0, 0, 0, 1);

#if (SPIM_CACHE_EN == 1)
    if (u32CacheOn)
    {
        SPIM_ENABLE_CACHE(pSPIMx);
    }
#endif

    SPIM_Hyper_EnterDirectMapMode(pSPIMx);
}
