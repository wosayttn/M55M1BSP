#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NuMicro.h"
#include "common.h"
#include "spim_common.h"
#include "flash_test.h"
#include "otfc_common.h"

//------------------------------------------------------------------------------
/**
 * @brief   Phase table DMA Write
 *
 * @param spim
 * @param psPhaseTable  Check SPI Flash specifications for support command codes,
 *                      and create command phase table.
 * @param u32Addr       Write Address.
 * @param is4ByteAddr   Enable 4 Bytes Address Mode.
 * @param u32WrSize     Write Data Size.
 * @param pu8TxBuf      Write Data Buffer.
 */
void SPIM_DMA_WritePhase(SPIM_T *spim, SPIM_PHASE_T *psPhaseTable,
                         int is4ByteAddr, uint32_t u32Addr, uint32_t u32WrSize,
                         uint8_t *pu8TxBuf)
{
    if (psPhaseTable != NULL)
    {
        SPIM_DMADMM_InitPhase(spim, psPhaseTable, SPIM_CTL0_OPMODE_PAGEWRITE);
    }

    SPIM_DMA_Write(spim,
                   u32Addr,
                   is4ByteAddr,
                   u32WrSize,
                   pu8TxBuf,
                   psPhaseTable->u32CMDCode);
}

/**
 * @brief   Phase table DMA Read.
 *
 * @param spim
 * @param psPhaseTable  Check SPI Flash specifications for support command codes,
 *                      and create command phase table.
 * @param u32Addr       Read SPI Flash Address.
 * @param is4ByteAddr   Enable 4 Bytes Address.
 * @param u32RdSize     Read Data Size.
 * @param pu8RxBuf      Read Data Buffer.
 * @param isSync        Wait SPIM Enable Done
 * @return int32_t
 */
int32_t SPIM_DMA_ReadPhase(SPIM_T *spim, SPIM_PHASE_T *psPhaseTable,
                           int is4ByteAddr, uint32_t u32Addr, uint32_t u32RdSize,
                           uint8_t *pu8RxBuf, int isSync)
{
    if (psPhaseTable != NULL)
    {
        SPIM_DMADMM_InitPhase(spim, psPhaseTable, SPIM_CTL0_OPMODE_PAGEREAD);
    }

    return SPIM_DMA_Read(spim,
                         u32Addr,
                         is4ByteAddr,
                         u32RdSize,
                         pu8RxBuf,
                         psPhaseTable->u32CMDCode,
                         isSync);
}

/**
 * @brief   Phase table DMM Read.
 *
 * @param spim
 * @param psPhaseTable  Check SPI Flash specifications for support command codes,
 *                      and create command phase table.
 * @param is4ByteAddr   Enable 4 Bytes Address.
 * @param u32IdleIntvl  Direct Map Mode Idle Interval Time.
 */
void SPIM_DMM_ReadPhase(SPIM_T *spim, SPIM_PHASE_T *psPhaseTable, int is4ByteAddr, uint32_t u32IdleIntvl)
{
    SPIM_DMADMM_InitPhase(spim, psPhaseTable, SPIM_CTL0_OPMODE_DIRECTMAP);

    SPIM_EnterDirectMapMode(spim, is4ByteAddr, psPhaseTable->u32CMDCode, u32IdleIntvl);
}

/**
* @brief Search SPI Flash Command and init DMA/DMM SPI flash read/wirte command phase
*
* @param spim
* @param pPhaseTable   Check SPI Flash specifications for support command codes,
*                      and create command phase table.
* @param u32TableSize  Phase Talbe total size
* @param u32OPMode     SPI Function Operation Mode
*                      - \ref SPIM_CTL0_OPMODE_PAGEWRITE : DMA Write mode
*                      - \ref SPIM_CTL0_OPMODE_PAGEREAD  : DMA Read mode
*                      - \ref SPIM_CTL0_OPMODE_DIRECTMAP : Direct Memory Mapping mode
* @param u32CMDCode
* @return     SPIM_OK          SPIM operation OK.
*             SPIM_ERR_FAIL    SPIM operation Fail.
*/
int32_t SPIM_FindAndInitDMADMMPhase(SPIM_T *spim,
                                    SPIM_PHASE_T *pPhaseTable,
                                    uint32_t u32TableSize,
                                    uint32_t u32OPMode,
                                    uint32_t u32CMDCode)
{
    uint32_t u32i = 1;
    int32_t i32found = SPIM_ERR_FAIL;
    uint32_t u32TableIdx = 0;
    SPIM_PHASE_T *pTmpTable = NULL;

    for (u32i = 0; u32i < u32TableSize; u32i++)
    {
        /*
         * If element is found in array then raise found flag
         * and terminate from loop.
         */
        if (pPhaseTable[u32i].u32CMDCode == u32CMDCode)
        {
            i32found = SPIM_OK;
            u32TableIdx = u32i;
            break;
        }
    }

    if (i32found != SPIM_OK)
    {
        return SPIM_ERR_FAIL;
    }

    pTmpTable = (SPIM_PHASE_T *)&pPhaseTable[u32TableIdx];

    SPIM_DMADMM_InitPhase(spim, pTmpTable, u32OPMode);

    return SPIM_OK;
}

#if 0 /* not use */
/**
  * @brief      Erase specified address range.
  * @param      u32Addr     Start u32Address to erase.
  * @param      is4ByteAddr 4-byte address or not.
  * @param      len         Number of bytes to erase.
  * @param      u32ErsCmd      Erase command.
  * @param      u32ErsBlkSize  Erased block size.
  * @param      u32NBit     N-bit transmit/receive.
  * @details    Erase specified address range. Even though the range is not block-aligned, the start block and end block are fully erased anyway.
  * @return     None.
  */
void Flash_EraseAddrRange(SPIM_T *pSPIMx, uint32_t u32Addr, int is4ByteAddr,
                          uint32_t len, uint32_t u32ErsCmd, uint32_t u32ErsBlkSize,
                          uint32_t u32NBit)
{
    printf("Flash_EraseAddrRange: 0x%x, 0x%x\n", u32Addr, len);

    do
    {
        uint32_t u32AddrInd = u32Addr / u32ErsBlkSize * u32ErsBlkSize;
        uint32_t endAddr = (u32Addr + len - 1) / u32ErsBlkSize * u32ErsBlkSize;

        while (u32AddrInd <= endAddr)
        {
            printf("u32AddrInd = %d\r\n", u32AddrInd);
            printf("endAddr = %d\r\n", endAddr);

            SPIM_EraseBlock(pSPIMx, u32AddrInd, is4ByteAddr, u32ErsCmd, u32NBit, 1);
            u32AddrInd += u32ErsBlkSize;                              // Advance indicator.
        }
    } while (0);
}
#endif //0

int Flash_EraseBlock(SPIM_T *pSPIMx, uint32_t u32SAddr, uint32_t u32Is4ByteAddr)
{
    uint8_t u32SrcBuf[TEST_BUFF_SIZE] = {0xFF};
    uint8_t u32DestBuf[TEST_BUFF_SIZE] = {0x0};
    /* 0x03: CMD_DMA_NORMAL_READ Command Phase Table */
    SPIM_PHASE_T sWB_03h_RdCMD =
    {
        CMD_DMA_NORMAL_READ,                                        // Command Code
        PHASE_NORMAL_MODE, PHASE_WIDTH_8, PHASE_DISABLE_DTR,        // Command Phase
        PHASE_NORMAL_MODE, PHASE_WIDTH_24, PHASE_DISABLE_DTR,       // Address Phase
        PHASE_NORMAL_MODE, PHASE_ORDER_MODE0, PHASE_DISABLE_DTR,    // Data Phase
        0,                                                          // Dummy Cycle Phase
    };

    printf("\tErase...\n");

    SPIM_EraseBlock(pSPIMx, u32SAddr, u32Is4ByteAddr, OPCODE_BE_64K, 1, 1);

    printf("\tErase Verify...\n");

    memset(u32SrcBuf, 0xFF, TEST_BUFF_SIZE);
    memset(u32DestBuf, 0xAA, TEST_BUFF_SIZE);
    SPIM_IO_ReadPhase(pSPIMx, &sWB_03h_RdCMD, u32SAddr, u32DestBuf, TEST_BUFF_SIZE);

    if (memcmp(u32SrcBuf, u32DestBuf, TEST_BUFF_SIZE))
    {
        printf("DMA write test - Erase verify failed\n");
        return SPIM_ERR_FAIL;
    }

    return SPIM_OK;
}

int Flash_WriteInPageData(SPIM_T *pSPIMx, uint32_t u32IsCipherEn,
                          uint32_t u32PRx, uint32_t u32SAddr,
                          int i32Is4ByteAddr, uint8_t *pu8WrBuf,
                          uint32_t u32TestSize)
{
    uint8_t *pu8TmpWrBuf = (uint8_t *)malloc(sizeof(uint8_t) * u32TestSize);
    uint8_t *pu8TmpWrBuf1 = (uint8_t *)malloc(sizeof(uint8_t) * u32TestSize);
    uint32_t u32CheckSize = 32;

    /* 0x02 : CMD_NORMAL_PAGE_PROGRAM Command Phase Table */
    SPIM_PHASE_T sWB_02h_WrCMD =
    {
        CMD_NORMAL_PAGE_PROGRAM,                                    //Command Code
        PHASE_NORMAL_MODE, PHASE_WIDTH_8,  PHASE_DISABLE_DTR,       //Command Phase
        PHASE_NORMAL_MODE, PHASE_WIDTH_24, PHASE_DISABLE_DTR,       //Address Phase
        PHASE_NORMAL_MODE, PHASE_ORDER_MODE0,  PHASE_DISABLE_DTR,   //Data Phase
        0,
    };

    /* 0x03: CMD_DMA_NORMAL_READ Command Phase Table */
    SPIM_PHASE_T sWB_03h_RdCMD =
    {
        CMD_DMA_NORMAL_READ,                                        // Command Code
        PHASE_NORMAL_MODE, PHASE_WIDTH_8, PHASE_DISABLE_DTR,        // Command Phase
        PHASE_NORMAL_MODE, PHASE_WIDTH_24, PHASE_DISABLE_DTR,       // Address Phase
        PHASE_NORMAL_MODE, PHASE_ORDER_MODE0, PHASE_DISABLE_DTR,    // Data Phase
        0,                                                          // Dummy Cycle Phase
    };

    if (u32IsCipherEn)
    {
        printf("\tCheck PlainText =>\n");
        HexDump(pu8WrBuf, u32CheckSize);

        /* Do software otfc cipher encryption */
        OTFC_SW_Cipher(u32PRx, u32SAddr, u32TestSize, pu8WrBuf, pu8TmpWrBuf, 0);

        /* Dump data to check if data is encrypted */
        printf("\tVerify CipherText =>\n");
        HexDump(pu8TmpWrBuf, u32CheckSize);
    }
    else
    {
        memcpy(pu8TmpWrBuf, pu8WrBuf, u32TestSize);
    }

    SPIM_IO_WritePhase(pSPIMx, &sWB_02h_WrCMD, u32SAddr, pu8TmpWrBuf, u32TestSize);

    memset(pu8TmpWrBuf1, 0x0, u32TestSize);

    SPIM_IO_ReadPhase(pSPIMx, &sWB_03h_RdCMD, u32SAddr, pu8TmpWrBuf1, u32TestSize);

    if (u32IsCipherEn)
    {
        /* Dump data to check if data is encrypted */
        printf("\tCheck CipherText =>\n");
        HexDump(pu8TmpWrBuf1, 64);

        /* Do software otfc cipher encryption */
        OTFC_SW_Cipher(u32PRx, u32SAddr, u32TestSize, pu8TmpWrBuf1, pu8TmpWrBuf, 0);

        /* Dump data to check if data is encrypted */
        printf("\tVerify PlainText =>\n");
        HexDump(pu8TmpWrBuf, 64);
    }
    else
    {
        memcpy(pu8TmpWrBuf, pu8TmpWrBuf1, u32TestSize);
    }

    // Compare.
    if (memcmp(pu8WrBuf, pu8TmpWrBuf, u32TestSize))
    {
        log_info(RED_BOLD"!!!\tMake Patten Data compare failed\n"RESET);
        dump_compare_error(u32SAddr, pu8WrBuf, pu8TmpWrBuf, u32TestSize);

        free(pu8TmpWrBuf);
        free(pu8TmpWrBuf1);

        return -1;
    }
    else
    {
        log_info("!!!\t0x%08x Make Patten Pass\n", u32SAddr);
    }

    free(pu8TmpWrBuf);
    free(pu8TmpWrBuf1);

    return 0;
}

int Flash_IO_MakePattern(SPIM_T *pSPIMx, uint32_t u32IsCipherEn, uint32_t u32SAddr, uint32_t u32TestSize)
{
    uint32_t page_addr = 0, offset = 0;
    uint32_t u32PRx = 0;
    int is4ByteAddr = 0;
    uint32_t u32PageOffset = 0;
    uint8_t *pu8SrcBuf = (uint8_t *)malloc(sizeof(uint8_t) * u32TestSize);

    SPIM_DISABLE_CIPHER(pSPIMx);

    popDat(pu8SrcBuf, u32TestSize);

    printf("\nWriting test pattern [Cipher %s]...\n",
           (u32IsCipherEn ? "ON" : "OFF"));

    // using 4-bytes mode anyway
    SPIM_Enable_4Bytes_Mode(pSPIMx, is4ByteAddr, 1, 0);    // Enter/Exit 4-Byte Address Mode.

    /*------------------------------------------------------*/
    /*  Erase page and verify                               */
    /*------------------------------------------------------*/
    printf("Erase SPI flash block 0x%x...", u32SAddr);
    SPIM_EraseBlock(pSPIMx, u32SAddr, is4ByteAddr, OPCODE_BE_64K, 1, 1);

    u32PageOffset = u32SAddr % 256UL;

    if ((u32PageOffset + u32TestSize) <= 256UL)
    {
        /* Do all the bytes fit onto one page ? */
        Flash_WriteInPageData(pSPIMx, u32IsCipherEn, u32PRx, u32SAddr, is4ByteAddr, pu8SrcBuf, u32TestSize);
    }
    else
    {
        uint32_t buf_idx = 0UL;
        uint32_t toWr = 256UL - u32PageOffset;               /* Size of data remaining on the first page. */

        Flash_WriteInPageData(pSPIMx, u32IsCipherEn, u32PRx, u32SAddr, is4ByteAddr, &pu8SrcBuf[buf_idx], toWr);

        u32SAddr += toWr;                         /* Advance indicator. */
        u32TestSize -= toWr;
        buf_idx += toWr;

        while (u32TestSize)
        {
            toWr = 256UL;

            if (toWr > u32TestSize)
            {
                toWr = u32TestSize;
            }

            Flash_WriteInPageData(pSPIMx, u32IsCipherEn, u32PRx, u32SAddr, is4ByteAddr, &pu8SrcBuf[buf_idx], toWr);

            u32SAddr += toWr;                 /* Advance indicator. */
            u32TestSize -= toWr;
            buf_idx += toWr;
        }
    }

    free(pu8SrcBuf);

    return 0;
}
