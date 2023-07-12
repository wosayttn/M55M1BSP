/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Show SPIM DMA mode read/write function.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"

#define FLASH_BLOCK_SIZE            (64 * 1024)     /* Flash block size. Depend on the physical flash. */
#define TEST_BLOCK_ADDR             0x10000         /* Test block address on SPI flash. */
#define BUFFER_SIZE                 2048

__attribute__((aligned(32))) uint8_t g_buff[BUFFER_SIZE] = {0};

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

int dma_read_write(int is4ByteAddr, uint32_t u32RdCmd, uint32_t WrCmd)
{
    uint32_t i = 0, offset = 0;             /* variables */
    uint32_t *pData = NULL;

    //if (SPIM_Enable_4Bytes_Mode(SPIM0, is4ByteAddr, 1) != 0)
    //{
    //    printf("SPIM_Enable_4Bytes_Mode failed!\n");
    //    return -1;
    //}

    /*
     *  Erase flash page
     */
    printf("Erase SPI flash block 0x%x...", TEST_BLOCK_ADDR);
    SPIM_EraseBlock(SPIM0, TEST_BLOCK_ADDR, is4ByteAddr, OPCODE_BE_64K, 1, 1);
    printf("done.\n");

    /*
     *  Verify flash page be erased
     */
    //if ((u32RdCmd == CMD_DMA_NORMAL_QUAD_READ) || (u32RdCmd == CMD_DMA_FAST_QUAD_READ) ||
    //        (u32RdCmd == CMD_DMA_FAST_READ_QUAD_OUTPUT))
    //    SPIM_SetQuadEnable(SPIM0, 1, 1);

    printf("Verify SPI flash block 0x%x be erased...", TEST_BLOCK_ADDR);

    //for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        SPIM_DMA_Read(SPIM0, TEST_BLOCK_ADDR + offset, is4ByteAddr, BUFFER_SIZE,
                      g_buff, u32RdCmd, 1);
        //SPIM_IO_Read(SPIM0, TEST_BLOCK_ADDR + offset, is4ByteAddr, BUFFER_SIZE,
        //             g_buff, OPCODE_FAST_READ, 1, 1, 1, 1);

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

    //SPIM_SetQuadEnable(SPIM0, 0, 1);

    /*
     *  Program data to flash block
     */
    printf("Program sequential data to flash block 0x%x...", TEST_BLOCK_ADDR);

    //for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
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
    //if ((u32RdCmd == CMD_DMA_NORMAL_QUAD_READ) || (u32RdCmd == CMD_DMA_FAST_QUAD_READ) ||
    //        (u32RdCmd == CMD_DMA_FAST_READ_QUAD_OUTPUT))
    //    SPIM_SetQuadEnable(SPIM0, 1, 1);

    printf("Verify SPI flash block 0x%x data...", TEST_BLOCK_ADDR);

    //for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
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

    //SPIM_SetQuadEnable(SPIM0, 0, 1);
    printf("done.\n");
    return 0;
}

int main()
{
    uint8_t idBuf[3];
    /* 0x02h : CMD_NORMAL_PAGE_PROGRAM Command Phase Table */
    SPIM_PHASE_T sWb02hWrCMD =
    {
        CMD_NORMAL_PAGE_PROGRAM,                                    //Command Code
        PHASE_NORMAL_MODE, PHASE_WIDTH_8,  PHASE_DISABLE_DTR,       //Command Phase
        PHASE_NORMAL_MODE, PHASE_WIDTH_24, PHASE_DISABLE_DTR,       //Address Phase
        PHASE_NORMAL_MODE, PHASE_ORDER_MODE0,  PHASE_DISABLE_DTR,   //Data Phase
        0,
    };

    /* 0x0B: CMD_DMA_FAST_READ Command Phase Table */
    SPIM_PHASE_T sWb03hRdCMD =
    {
        CMD_DMA_NORMAL_READ,                                        // Command Code
        PHASE_NORMAL_MODE, PHASE_WIDTH_8, PHASE_DISABLE_DTR,        // Command Phase
        PHASE_NORMAL_MODE, PHASE_WIDTH_24, PHASE_DISABLE_DTR,       // Address Phase
        PHASE_NORMAL_MODE, PHASE_ORDER_MODE0, PHASE_DISABLE_DTR,    // Data Phase
        0,                                                          // Dummy Cycle Phase
    };

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
    printf("|    M55M1 SPIM DMA mode read/write sample  |\n");
    printf("+-------------------------------------------+\n");

    SYS_UnlockReg();                            /* Unlock register lock protect */

    SPIM_SET_CLOCK_DIVIDER(SPIM0, 2);           /* Set SPIM clock as HCLK divided by 4 */

    SPIM_SET_RXCLKDLY_RDDLYSEL(SPIM0, 0);       /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */

    SPIM_DISABLE_CIPHER(SPIM0);

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

    SPIM_DMADMM_InitPhase(SPIM0, &sWb03hRdCMD, SPIM_CTL0_OPMODE_PAGEREAD);
    SPIM_DMADMM_InitPhase(SPIM0, &sWb02hWrCMD, SPIM_CTL0_OPMODE_PAGEWRITE);

    if (dma_read_write(0, sWb03hRdCMD.u32CMDCode, sWb02hWrCMD.u32CMDCode) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");
#if 0
    printf("\n[Fast Read Dual Output] 3-bytes address mode, Fast Read Dual command...");

    if (dma_read_write(0, CMD_DMA_FAST_READ_DUAL_OUTPUT, CMD_NORMAL_PAGE_PROGRAM, 8) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    printf("\n[Fast Read Quad Output] 3-bytes address mode, Fast Read Quad command...");

    if (dma_read_write(0, CMD_DMA_FAST_QUAD_READ, CMD_NORMAL_PAGE_PROGRAM, 4) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");
#endif //0
#if 0  /* W25Q20 does not support 4-bytes address mode. */
    printf("\n[Fast Read Dual I/O] 4-bytes address mode, dual read...");

    if (dma_read_write(1, CMD_DMA_FAST_DUAL_READ, CMD_NORMAL_PAGE_PROGRAM, 8) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    printf("\n[Fast Read Quad I/O] 4-bytes address mode, quad read...");

    if (dma_read_write(1, CMD_DMA_FAST_QUAD_READ, CMD_NORMAL_PAGE_PROGRAM, 4) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");
#endif

    printf("\nSPIM DMA read/write demo done.\n");

lexit:

    SYS_LockReg();                     /* Lock protected registers */

    while (1);
}

/*** (C) COPYRIGHT 2021 Nuvoton Technology Corp. ***/
