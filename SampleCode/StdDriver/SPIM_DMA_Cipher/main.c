/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate SPIM DMA read/write with cipher enabled. This sample
 *           also dumps SPI flash content via I/O mode read to prove it is encrypted cipher context.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"

//------------------------------------------------------------------------------
#define FLASH_BLOCK_SIZE            (8 * 1024)      /* Flash block size. Depend on the physical flash. */
#define TEST_BLOCK_ADDR             0x10000         /* Test block address on SPI flash. */
#define BUFFER_SIZE                 2048

//------------------------------------------------------------------------------
NVT_NONCACHEABLE __attribute__((aligned(4))) uint8_t g_buff[BUFFER_SIZE] = {0};

/* SPIM cipher key User defined. */
uint32_t gau32AESKey[8] =
{
    0x93484D6F, //Key0
    0x2F7A7F2A, //Key1
    0x063FF08A, //Key2
    0x7A29E38E, //Key3
    0x7A29E38E, //Scramble
    0x063FF08A, //NONCE0
    0x2F7A7F2A, //NONCE1
    0x93484D6F, //NONCE2
};

//------------------------------------------------------------------------------
/* Program Command Phase */
extern SPIM_PHASE_T gsWb02hWrCMD;
extern SPIM_PHASE_T gsWb12hWrCMD;

/* Standard Read Command Phase */
extern SPIM_PHASE_T gsWb0BhRdCMD;

/* Dual Read Command Phase */
extern SPIM_PHASE_T gsWbBBhRdCMD;
extern SPIM_PHASE_T gsWbBChRdCMD;

/* Quad Read Command Phase */
extern SPIM_PHASE_T gsWbEBhRdCMD;
extern SPIM_PHASE_T gsWbEChRdCMD;

//------------------------------------------------------------------------------
void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Enable PLL0 180MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_180MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to PLL0 and divide 1 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0);

    /* Set HCLK2 divide 2 */
    CLK_SET_HCLK2DIV(2);

    /* Set PCLKx divide 2 */
    CLK_SET_PCLK0DIV(2);
    CLK_SET_PCLK1DIV(2);
    CLK_SET_PCLK2DIV(2);
    CLK_SET_PCLK3DIV(2);
    CLK_SET_PCLK4DIV(2);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and cyclesPerUs automatically. */
    SystemCoreClockUpdate();

    /* Enable SPIM module clock */
    CLK_EnableModuleClock(SPIM0_MODULE);

    /* Enable OTFC module clock */
    CLK_EnableModuleClock(OTFC0_MODULE);

    /* Enable CRYPTO module clock */
    CLK_EnableModuleClock(CRYPTO0_MODULE);

    /* Enable TRNG module clock */
    CLK_EnableModuleClock(TRNG0_MODULE);

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /* Init SPIM multi-function pins */
    SET_SPIM0_CLK_PC4();
    SET_SPIM0_SS_PC3();
    SET_SPIM0_MISO_PG12();
    SET_SPIM0_MOSI_PG11();
    SET_SPIM0_D2_PC0();
    SET_SPIM0_D3_PG10();

    PC->SMTEN |= GPIO_SMTEN_SMTEN0_Msk;
    PG->SMTEN |= GPIO_SMTEN_SMTEN0_Msk;

    /* Set SPIM I/O pins as high slew rate up to 80 MHz. */
    GPIO_SetSlewCtl(PC, BIT0, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PC, BIT3, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PC, BIT4, GPIO_SLEWCTL_HIGH);

    GPIO_SetSlewCtl(PG, BIT10, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PG, BIT11, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PG, BIT12, GPIO_SLEWCTL_HIGH);
}

void DumpBufferHex(uint8_t *pucBuff, int nSize)
{
    int     nIdx, i;

    nIdx = 0;

    while (nSize > 0)
    {
        printf("0x%04X  ", nIdx);

        for (i = 0; i < 16; i++)
            printf("%02x ", pucBuff[nIdx + i]);

        printf("  ");

        for (i = 0; i < 16; i++)
        {
            if ((pucBuff[nIdx + i] >= 0x20) && (pucBuff[nIdx + i] < 127))
                printf("%c", pucBuff[nIdx + i]);
            else
                printf(".");

            nSize--;
        }

        nIdx += 16;
        printf("\n");
    }

    printf("\n");
}

int dma_read_write(int is4ByteAddr, uint32_t u32RdCmd, uint32_t WrCmd)
{
    volatile uint32_t i = 0, offset = 0;             /* variables */
    uint32_t *pData = NULL;

    SPIM_ENABLE_CIPHER(SPIM0);

    /*
     *  Erase flash page
     */
    printf("Erase SPI flash block 0x%x...", TEST_BLOCK_ADDR);
    SPIM_EraseBlock(SPIM0, TEST_BLOCK_ADDR, is4ByteAddr, OPCODE_BE_64K, 1, 1);
    printf("done.\n");

    /*
     *  Verify flash page be erased
     */
    printf("Verify SPI flash block 0x%x be erased...", TEST_BLOCK_ADDR);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        SPIM_IO_Read(SPIM0, TEST_BLOCK_ADDR + offset, is4ByteAddr, BUFFER_SIZE,
                     g_buff, OPCODE_FAST_READ, 1, 1, 1, 1);

        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != 0xFFFFFFFF)
            {
                printf("FAILED!\n");
                printf("Flash address 0x%x, read 0x%x!\n", TEST_BLOCK_ADDR + i, *pData);
                return -1;
            }
        }
    }

    printf("done.\n");

    /*
     *  Program data to flash block
     */
    printf("Program sequential data to flash block 0x%x...", TEST_BLOCK_ADDR);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
            (*pData) = (i << 16) | (TEST_BLOCK_ADDR + offset + i);

        SPIM_DMA_Write(SPIM0, TEST_BLOCK_ADDR + offset, is4ByteAddr, BUFFER_SIZE, g_buff, WrCmd);
    }

    printf("done.\n");

    /*
     *  Verify flash block data
     */
    printf("Verify SPI flash block 0x%x data...", TEST_BLOCK_ADDR);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        SPIM_DMA_Read(SPIM0, TEST_BLOCK_ADDR + offset, is4ByteAddr, BUFFER_SIZE, g_buff, u32RdCmd, 1);

        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if ((*pData) != ((i << 16) | (TEST_BLOCK_ADDR + offset + i)))
            {
                printf("FAILED!\n");
                printf("Flash address 0x%x, read 0x%x, expect 0x%x!\n",
                       TEST_BLOCK_ADDR + i, *pData, (i << 16) | (TEST_BLOCK_ADDR + offset + i));
                return -1;
            }
        }
    }

    printf("done.\n");

    memset(g_buff, 0, 64);
    printf("\n\nDump SPI flash with I/O read. It's the SPIM cipher encrypted text.\n");
    SPIM_IO_Read(SPIM0, TEST_BLOCK_ADDR, is4ByteAddr, BUFFER_SIZE, g_buff, OPCODE_FAST_READ, 1, 1, 1, 1);
    DumpBufferHex(g_buff, 64);

    memset(g_buff, 0, 64);
    printf("\n\nDump SPI flash with DMA read. It's the plain text. "
           "The cipher text was decrypted by SPIM while doing DMA read.\n");
    SPIM_DMA_Read(SPIM0, TEST_BLOCK_ADDR, is4ByteAddr, BUFFER_SIZE, g_buff, u32RdCmd, 1);
    DumpBufferHex(g_buff, 64);

    SPIM_DISABLE_CIPHER(SPIM0);

    return 0;
}

int main()
{
    uint8_t idBuf[3];

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

    /*------------------------------------------------------------------------*/
    /* SAMPLE CODE                                                            */
    /*------------------------------------------------------------------------*/
    printf("+-------------------------------------------+\n");
    printf("|    M55M1 SPIM DMA Cipher mode sample      |\n");
    printf("+-------------------------------------------+\n");

    /* Unlock register lock protect */
    SYS_UnlockReg();

    /* Set SPIM clock as HCLK divided by 4 */
    SPIM_SET_CLOCK_DIVIDER(SPIM0, 2);

    /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */
    SPIM_SET_RXCLKDLY_RDDLYSEL(SPIM0, 0);

    /* Set Cipher Key and protection region */
    OTFC_SetKeyFromKeyReg(OTFC0, gau32AESKey, OTFC_PR_0, TEST_BLOCK_ADDR, BUFFER_SIZE);
    OTFC_ENABLE_PR(OTFC0, OTFC_PR_0);

    SPIM_DISABLE_CACHE(SPIM0);

    if (SPIM_InitFlash(SPIM0, 1) != 0)          /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        goto lexit;
    }

    SPIM_ReadJedecId(SPIM0, idBuf, sizeof(idBuf), 1, 0);
    printf("SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n",
           idBuf[0], idBuf[1], idBuf[2]);

    printf("\n[Fast Read] 3-bytes address mode, Fast Read command...");

    SPIM_DMADMM_InitPhase(SPIM0, &gsWb0BhRdCMD, SPIM_CTL0_OPMODE_PAGEREAD);
    SPIM_DMADMM_InitPhase(SPIM0, &gsWb02hWrCMD, SPIM_CTL0_OPMODE_PAGEWRITE);

    if (dma_read_write(0, gsWb0BhRdCMD.u32CMDCode, gsWb02hWrCMD.u32CMDCode) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    printf("\n[Fast Read Dual Output] 3-bytes address mode, Fast Read Dual command...");
    SPIM_DMADMM_InitPhase(SPIM0, &gsWbBBhRdCMD, SPIM_CTL0_OPMODE_PAGEREAD);

    if (dma_read_write(0, gsWbBBhRdCMD.u32CMDCode, gsWb02hWrCMD.u32CMDCode) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    printf("\n[Fast Read Quad Output] 3-bytes address mode, Fast Read Quad command...");
    SPIM_DMADMM_InitPhase(SPIM0, &gsWbEBhRdCMD, SPIM_CTL0_OPMODE_PAGEREAD);

    if (dma_read_write(0, gsWbEBhRdCMD.u32CMDCode, gsWb02hWrCMD.u32CMDCode) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    SPIM_DMADMM_InitPhase(SPIM0, &gsWb12hWrCMD, SPIM_CTL0_OPMODE_PAGEWRITE);

    printf("\n[Fast Read Dual I/O] 4-bytes address mode, dual read...");
    SPIM_DMADMM_InitPhase(SPIM0, &gsWbBChRdCMD, SPIM_CTL0_OPMODE_PAGEREAD);

    if (dma_read_write(1, gsWbBChRdCMD.u32CMDCode, gsWb12hWrCMD.u32CMDCode) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    printf("\n[Fast Read Quad I/O] 4-bytes address mode, quad read...");
    SPIM_DMADMM_InitPhase(SPIM0, &gsWbEChRdCMD, SPIM_CTL0_OPMODE_PAGEREAD);

    if (dma_read_write(1, gsWbEChRdCMD.u32CMDCode, gsWb12hWrCMD.u32CMDCode) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    printf("\nSPIM DMA read/write demo done.\n");

lexit:

    SYS_LockReg();                     /* Lock protected registers */

    while (1);
}

/*** (C) COPYRIGHT 2021 Nuvoton Technology Corp. ***/
