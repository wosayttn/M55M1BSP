/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate SPIM DMM mode read function. This sample programs SPI
 *           flash with DMA write and verify flash with DMA read and DMM mode CPU read respectively.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"

//------------------------------------------------------------------------------
#define FLASH_BLOCK_SIZE            (8 * 1024)  /* Flash block size. Depend on the physical flash. */
#define TEST_BLOCK_ADDR             0x10000     /* Test block address on SPI flash. */
#define BUFFER_SIZE                 2048

//------------------------------------------------------------------------------
__attribute__((aligned(32))) uint8_t g_buff[BUFFER_SIZE] = {0};

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

    /* Enable GPIO Module clock */
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOG_MODULE);

    /* Enable UART module clock */
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

int dma_rw_patten(SPIM_T *spim, int is4ByteAddr, uint32_t u32RdCmd, uint32_t WrCmd)
{
    uint32_t i, offset;             /* variables */
    uint32_t *pData;

    /*
     *  Erase flash page
     */
    printf("\tErase SPI flash block 0x%x...", TEST_BLOCK_ADDR);
    SPIM_EraseBlock(spim, TEST_BLOCK_ADDR, is4ByteAddr, OPCODE_BE_64K, 1, 1);
    printf("done.\n");

    /*
    *  Verify flash page be erased
    */
    printf("\tVerify SPI flash block 0x%x be erased...", TEST_BLOCK_ADDR);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        SPIM_DMA_Read(spim, TEST_BLOCK_ADDR + offset, is4ByteAddr, BUFFER_SIZE, g_buff, u32RdCmd, 1);

        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != 0xFFFFFFFF)
            {
                printf("FAILED!\n");
                printf("\tFlash address 0x%x, read 0x%x!\n", TEST_BLOCK_ADDR + i, *pData);
                return -1;
            }
        }
    }

    printf("done.\n");

    /*
    *  Program data to flash block
    */
    printf("\tProgram sequential data to flash block 0x%x...", TEST_BLOCK_ADDR);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            (*pData) = (i << 16) | (TEST_BLOCK_ADDR + offset + i);
        }

        SPIM_DMA_Write(spim, TEST_BLOCK_ADDR + offset, is4ByteAddr, BUFFER_SIZE, g_buff, WrCmd);
    }

    printf("done.\n");

    return 0;
}

int dmm_read(SPIM_T *spim, int is4ByteAddr, uint32_t u32RdCmd, uint32_t WrCmd)
{
    uint32_t i, offset;             /* variables */
    uint32_t *pData;
    uint32_t u32DMMAddr = SPIM_GetDMMAddress(spim);

    /*
     *  Verify flash block data with DMM read
     */
    printf("\tVerify SPI flash block 0x%x data with DMM read...", TEST_BLOCK_ADDR);

    if ((u32RdCmd == CMD_DMA_NORMAL_QUAD_READ) || (u32RdCmd == CMD_DMA_FAST_QUAD_READ) ||
            (u32RdCmd == CMD_DMA_FAST_READ_QUAD_OUTPUT))
        SPIM_SetQuadEnable(spim, 1, 1);

    SPIM_EnterDirectMapMode(spim, is4ByteAddr, u32RdCmd, 0);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        memcpy(g_buff, (uint8_t *)(u32DMMAddr + TEST_BLOCK_ADDR + offset), BUFFER_SIZE);

        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if ((*pData) != ((i << 16) | (TEST_BLOCK_ADDR + offset + i)))
            {
                printf("FAILED!\n");
                printf("\tFlash address 0x%x, read 0x%x, expect 0x%x!\n", TEST_BLOCK_ADDR + i, *pData, (i << 16) | (TEST_BLOCK_ADDR + offset + i));
                SPIM_ExitDirectMapMode(spim);
                return -1;
            }
        }
    }

    printf("done.\n");

    SPIM_ExitDirectMapMode(spim);

    SPIM_SetQuadEnable(spim, 0, 1);

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

    printf("+-------------------------------------------+\n");
    printf("|         SPIM DMM mode read sample         |\n");
    printf("+-------------------------------------------+\n");

    SPIM_SET_CLOCK_DIVIDER(SPIM0, 1);       /* Set SPIM clock as HCLK divided by 2 */

    SPIM_SET_RXCLKDLY_RDDLYSEL(SPIM0, 0);   /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */

    if (SPIM_InitFlash(SPIM0, 1) != 0)      /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        goto lexit;
    }

    SPIM_DISABLE_CIPHER(SPIM0);

    SPIM_ReadJedecId(SPIM0, idBuf, sizeof(idBuf), 1, 0);
    printf("SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n", idBuf[0], idBuf[1], idBuf[2]);

#if (SPIM_REG_CACHE == 1) // TESTCHIP_ONLY not support
    SPIM_ENABLE_CACHE(SPIM0);
#endif

    SPIM_DMADMM_InitPhase(SPIM0, &gsWb02hWrCMD, SPIM_CTL0_OPMODE_PAGEWRITE);
    SPIM_DMADMM_InitPhase(SPIM0, &gsWb0BhRdCMD, SPIM_CTL0_OPMODE_DIRECTMAP);

    if (dma_rw_patten(SPIM0, 0, gsWb0BhRdCMD.u32CMDCode, gsWb02hWrCMD.u32CMDCode) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("\n[1] 3-bytes address mode, normal read...\r\n");

    SPIM_DMADMM_InitPhase(SPIM0, &gsWb0BhRdCMD, SPIM_CTL0_OPMODE_DIRECTMAP);

    if (dmm_read(SPIM0, 0, gsWb0BhRdCMD.u32CMDCode, gsWb02hWrCMD.u32CMDCode) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    printf("\n[2] 3-bytes address mode, dual read...\r\n");

    SPIM_DMADMM_InitPhase(SPIM0, &gsWbBBhRdCMD, SPIM_CTL0_OPMODE_DIRECTMAP);

    if (dmm_read(SPIM0, 0, gsWbBBhRdCMD.u32CMDCode, gsWb02hWrCMD.u32CMDCode) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    printf("\n[3] 4-bytes address mode, dual read...\r\n");
    SPIM_DMADMM_InitPhase(SPIM0, &gsWbBChRdCMD, SPIM_CTL0_OPMODE_DIRECTMAP);

    if (dmm_read(SPIM0, 1, gsWbBChRdCMD.u32CMDCode, gsWb02hWrCMD.u32CMDCode) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    printf("\n[4] 4-bytes address mode, quad read...\r\n");

    SPIM_DMADMM_InitPhase(SPIM0, &gsWbEChRdCMD, SPIM_CTL0_OPMODE_DIRECTMAP);

    if (dmm_read(SPIM0, 1, gsWbEChRdCMD.u32CMDCode, gsWb02hWrCMD.u32CMDCode) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    printf("\nSPIM DMM read demo done.\n");

lexit:

    /* Lock protected registers */
    SYS_LockReg();

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
