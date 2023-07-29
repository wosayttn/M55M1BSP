/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Show SPIM DMA mode read/write Micron MT35xU02G octal flash function.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"

#define FLASH_BLOCK_SIZE            (8 * 1024)     /* Flash block size. Depend on the physical flash. */
#define TEST_BLOCK_ADDR             0x10000         /* Test block address on SPI flash. */
#define BUFFER_SIZE                 2048

NVT_NONCACHEABLE __attribute__((aligned(32))) uint8_t g_buff[BUFFER_SIZE] = {0};

/* Program SDR Command Phase */
extern SPIM_PHASE_T gsMtC2hWrCMD;
extern SPIM_PHASE_T gsMt8EhWrCMD;

/* Octal SDR Read Command Phase */
extern SPIM_PHASE_T gsMtCBhRdCMD;
extern SPIM_PHASE_T gsMtCChRdCMD;
extern SPIM_PHASE_T gsMt9DhRdCMD;
extern SPIM_PHASE_T gsMtFDhRdCMD;

/* Program DDR Command Phase */
extern SPIM_PHASE_T gsMt02hWrDDRCMD;
/* Octal DDR Read Command Phase */
extern SPIM_PHASE_T gsMt8BhRdDDRCMD;

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

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /* Init SPIM multi-function pins */
    SET_SPIM0_CLKN_PC5();
    SET_SPIM0_CLK_PC4();
    SET_SPIM0_D2_PC0();
    SET_SPIM0_D3_PG10();
    SET_SPIM0_D4_PG9();
    SET_SPIM0_D5_PG13();
    SET_SPIM0_D6_PG14();
    SET_SPIM0_D7_PG15();
    SET_SPIM0_MISO_PG12();
    SET_SPIM0_MOSI_PG11();
    SET_SPIM0_RESETN_PC2();
    SET_SPIM0_RWDS_PC1();
    SET_SPIM0_SS_PC3();

    PC->SMTEN |= GPIO_SMTEN_SMTEN0_Msk;
    PG->SMTEN |= GPIO_SMTEN_SMTEN0_Msk;

    /* Set SPIM I/O pins as high slew rate up to 80 MHz. */
    GPIO_SetSlewCtl(PC, BIT0, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PC, BIT1, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PC, BIT2, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PC, BIT3, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PC, BIT4, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PC, BIT5, GPIO_SLEWCTL_HIGH);

    GPIO_SetSlewCtl(PG, BIT9, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PG, BIT10, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PG, BIT11, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PG, BIT12, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PG, BIT13, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PG, BIT14, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PG, BIT15, GPIO_SLEWCTL_HIGH);
}

void OctalFlash_EnterDDRMode(SPIM_T *spim)
{
    uint8_t u8CMDBuf[1] = {0xE7};
    SPIM_PHASE_T gWrNVCRegCMD =
    {
        OPCODE_WR_VCONFIG,                                          //Command Code
        PHASE_NORMAL_MODE, PHASE_WIDTH_8,  PHASE_DISABLE_DTR,       //Command Phase
        0, 0, 0,                                                    //Register command not Address
        PHASE_NORMAL_MODE, PHASE_ORDER_MODE0,  PHASE_DISABLE_DTR,   //Data Phase
        0,
    };

    /* Disable 4-byte address mode */
    SPIM_Enable_4Bytes_Mode(spim, 0, 1);

    /* Set non-volatile register enter octal DDR mode */
    SPIM_IO_WritePhase(spim, &gWrNVCRegCMD, 0x00, u8CMDBuf, sizeof(u8CMDBuf));

    /* Enable 4-byte address mode */
    SPIM_Enable_4Bytes_Mode(spim, 1, 8);
}

void OctalFlash_ExitDDRMode(SPIM_T *spim)
{
    uint8_t u8CMDBuf[1] = {0xFF};
    SPIM_PHASE_T gWrNVCRegCMD =
    {
        OPCODE_WR_VCONFIG,                                      //Command Code
        PHASE_OCTAL_MODE, PHASE_WIDTH_8,  PHASE_ENABLE_DTR,     //Command Phase
        0, 0, 0,                                                //Register Command not Address
        PHASE_OCTAL_MODE, PHASE_ORDER_MODE0,  PHASE_ENABLE_DTR, //Data Phase
        0,
    };

    /* Set non-volatile register exit octal DDR mode */
    SPIM_IO_WritePhase(spim, &gWrNVCRegCMD, 0x00, u8CMDBuf, sizeof(u8CMDBuf));

    /* Disable 4-byte Address mode */
    SPIM_Enable_4Bytes_Mode(spim, 0, 1);
}

int dma_read_write(int is4ByteAddr, uint32_t u32RdCmd, uint32_t WrCmd, uint32_t u32DDREn)
{
    uint32_t i = 0, offset = 0;             /* variables */
    uint32_t *pData = NULL;
    uint32_t u32EraseNBit = 1;

    /* Octal Flash in Octal DDR mode, erase command uses 8 data lines */
    if (u32DDREn == SPIM_OP_ENABLE)
    {
        u32EraseNBit = 8;
    }

    /*
     *  Erase flash page
     */
    printf("Erase SPI flash block 0x%x...", TEST_BLOCK_ADDR);
    SPIM_EraseBlock(SPIM0, TEST_BLOCK_ADDR, is4ByteAddr, OPCODE_BE_64K, u32EraseNBit, SPIM_OP_ENABLE);
    printf("done.\n");

    /*
     *  Verify flash page be erased
     */
    printf("Verify SPI flash block 0x%x be erased...", TEST_BLOCK_ADDR);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        SPIM_IO_Read(SPIM0, TEST_BLOCK_ADDR + offset, is4ByteAddr, BUFFER_SIZE,
                     g_buff, OPCODE_FAST_READ, 8, 8, 8, 1);

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
    printf("+------------------------------------------------------------------------+\n");
    printf("|      SPIM DMA mode read/write Micron MT35XU02G octal flash sample      |\n");
    printf("+------------------------------------------------------------------------+\n");

    SYS_UnlockReg();                            /* Unlock register lock protect */

    /**
     * @brief Set SPIM clock as HCLK divided by 4
     * @note  Octal flash in DDR mode only support 1 or 2.
     */
    SPIM_SET_CLOCK_DIVIDER(SPIM0, 2);

    SPIM_SET_RXCLKDLY_RDDLYSEL(SPIM0, 0);       /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */

    SPIM_DISABLE_CIPHER(SPIM0);

    SPIM_DISABLE_CACHE(pSPIMx);

    if (SPIM_InitFlash(SPIM0, 1) != 0)          /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        goto lexit;
    }

    SPIM_ReadJedecId(SPIM0, idBuf, sizeof(idBuf), 1, 0);
    printf("SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n",
           idBuf[0], idBuf[1], idBuf[2]);

    printf("\n[Fast IO Read] 3-bytes address mode, Fast Read Octal SDR command...\r\n");

    SPIM_DMADMM_InitPhase(SPIM0, &gsMtCBhRdCMD, SPIM_CTL0_OPMODE_PAGEREAD);
    SPIM_DMADMM_InitPhase(SPIM0, &gsMtC2hWrCMD, SPIM_CTL0_OPMODE_PAGEWRITE);

    if (dma_read_write(0, gsMtCBhRdCMD.u32CMDCode, gsMtC2hWrCMD.u32CMDCode, SPIM_OP_DISABLE) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    printf("\n[Fast IO Read] 4-bytes address mode, Fast Read Octal SDR command...\r\n");

    SPIM_DMADMM_InitPhase(SPIM0, &gsMtCChRdCMD, SPIM_CTL0_OPMODE_PAGEREAD);
    SPIM_DMADMM_InitPhase(SPIM0, &gsMt8EhWrCMD, SPIM_CTL0_OPMODE_PAGEWRITE);

    if (dma_read_write(1, gsMtCChRdCMD.u32CMDCode, gsMt8EhWrCMD.u32CMDCode, SPIM_OP_DISABLE) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    printf("\n[Fast Read Output] 3-bytes address mode, Fast Read Octal DDR command...\r\n");

    SPIM_DMADMM_InitPhase(SPIM0, &gsMt9DhRdCMD, SPIM_CTL0_OPMODE_PAGEREAD);
    SPIM_DMADMM_InitPhase(SPIM0, &gsMtC2hWrCMD, SPIM_CTL0_OPMODE_PAGEWRITE);

    if (dma_read_write(0, gsMt9DhRdCMD.u32CMDCode, gsMtC2hWrCMD.u32CMDCode, SPIM_OP_DISABLE) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    printf("\n[Fast IO Read] 4-bytes address mode, Fast Read Octal DDR command...\r\n");

    SPIM_DMADMM_InitPhase(SPIM0, &gsMtFDhRdCMD, SPIM_CTL0_OPMODE_PAGEREAD);
    SPIM_DMADMM_InitPhase(SPIM0, &gsMt8EhWrCMD, SPIM_CTL0_OPMODE_PAGEWRITE);

    if (dma_read_write(1, gsMtFDhRdCMD.u32CMDCode, gsMt8EhWrCMD.u32CMDCode, SPIM_OP_DISABLE) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    printf("[OK].\n");

    printf("\n[Fast Read Output DDR Mode] 4-bytes address mode, Fast Read Octal DDR command...\r\n");

    OctalFlash_EnterDDRMode(SPIM0); /* Enable Octal Flash DDR Mode */

    SPIM_DMADMM_InitPhase(SPIM0, &gsMt8BhRdDDRCMD, SPIM_CTL0_OPMODE_PAGEREAD);
    SPIM_DMADMM_InitPhase(SPIM0, &gsMt02hWrDDRCMD, SPIM_CTL0_OPMODE_PAGEWRITE);

    if (dma_read_write(1, gsMt8BhRdDDRCMD.u32CMDCode, gsMt02hWrDDRCMD.u32CMDCode, SPIM_OP_ENABLE) < 0)
    {
        printf("  FAILED!!\n");
        goto lexit;
    }

    OctalFlash_ExitDDRMode(SPIM0);  /* Disable Octal Flash DDR Mode */

    printf("[OK].\n");

lexit:

    SYS_LockReg();                     /* Lock protected registers */

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
