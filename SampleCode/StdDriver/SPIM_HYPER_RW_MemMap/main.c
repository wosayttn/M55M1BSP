/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    This sample show HyperRAM read/write through HyperBus Interface.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>

#include "NuMicro.h"
#include "hyperram_code.h"

//------------------------------------------------------------------------------
#define FLASH_BLOCK_SIZE            (8 * 1024)     /* Flash block size. Depend on the physical flash. */
#define TEST_BLOCK_ADDR             0x10000         /* Test block address on SPI flash. */
#define BUFFER_SIZE                 2048

#define SPIM_PORT                   SPIM1
#define OTFC_PORT                   OTFC1

//------------------------------------------------------------------------------
__attribute__((aligned(32))) uint8_t g_buff[BUFFER_SIZE] = {0};

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

    /* Enable SPIM module clock */
    CLK_EnableModuleClock(OTFC0_MODULE);

    /* Enable GPIO Module clock */
    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOG_MODULE);

    /* Enable UART module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();
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

int dmm_read_write(SPIM_T *spim, uint32_t u32CipherEn)
{
    uint32_t i = 0, offset = 0;             /* variables */
    uint32_t *pData = NULL;
    uint32_t u32DMMAddr = SPIM_HYPER_GetDMMAddress(spim);

    if (u32CipherEn == SPIM_HYPER_OP_ENABLE)
    {
        SPIM_HYPER_ENABLE_CIPHER(spim);
    }

    /*
     *  Erase flash page
     */
    printf("\r\nErase and verify HyperRAM block 0x%x...", TEST_BLOCK_ADDR);
    HyperRAM_Erase(spim, TEST_BLOCK_ADDR, FLASH_BLOCK_SIZE);
    printf("done.\n");

    SPIM_HYPER_EnterDirectMapMode(spim);

    /*
     *  Program data to flash block
     */
    printf("DMM mode program sequential data to HyperRAM block 0x%x...", TEST_BLOCK_ADDR);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
            (*pData) = (i << 16) | (TEST_BLOCK_ADDR + offset + i);

        memcpy((uint8_t *)(u32DMMAddr + TEST_BLOCK_ADDR + offset), g_buff, BUFFER_SIZE);
    }

    printf("done.\n");

    /*
     *  Verify flash block data
     */
    printf("DMM mode verify HyperRAM block 0x%x data...", TEST_BLOCK_ADDR);

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
                printf("\tHyperRAM address 0x%x, read 0x%x, expect 0x%x!\n",
                       TEST_BLOCK_ADDR + i, *pData, (i << 16) | (TEST_BLOCK_ADDR + offset + i));
                return -1;
            }
        }

    }

    printf("done.\n");

    if (u32CipherEn != SPIM_HYPER_OP_ENABLE)
    {
        return 0;
    }

    memset(g_buff, 0, 64);
    printf("\n\nDump HyperRAM with command read. It's the SPIM cipher encrypted text.\n");

    for (offset = 0; offset < 64; offset += 2)
    {
        g_buff[offset] = SPIM_HYPER_Read1Word(spim, TEST_BLOCK_ADDR + offset);
    }

    DumpBufferHex(g_buff, 64);

    memset(g_buff, 0, 64);
    printf("\n\nDump HyperRAM with DMM read. It's the plain text. "
           "The cipher text was decrypted by SPIM while doing DMM read.\n");
    memcpy(g_buff, (uint8_t *)(u32DMMAddr + TEST_BLOCK_ADDR + offset), BUFFER_SIZE);
    DumpBufferHex(g_buff, 64);

    SPIM_HYPER_DISABLE_CIPHER(spim);

    SPIM_HYPER_ExitDirectMapMode(spim);

    return 0;
}

int dma_read_write(SPIM_T *spim, uint32_t u32CipherEn)
{
    uint32_t i = 0, offset = 0;             /* variables */
    uint32_t *pData = NULL;

    /*
     *  Erase flash page
     */
    printf("\r\nErase and verify HyperRAM block 0x%x...", TEST_BLOCK_ADDR);
    HyperRAM_Erase(spim, TEST_BLOCK_ADDR, FLASH_BLOCK_SIZE);
    printf("done.\n");

    if (u32CipherEn == SPIM_HYPER_OP_ENABLE)
    {
        SPIM_HYPER_ENABLE_CIPHER(spim);
    }

    /*
     *  Program data to flash block
     */
    printf("DMA mode program sequential data to HyperRAM block 0x%x...", TEST_BLOCK_ADDR);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
            (*pData) = (i << 16) | (TEST_BLOCK_ADDR + offset + i);

        SPIM_HYPER_DMAWrite(spim, TEST_BLOCK_ADDR + offset, g_buff, BUFFER_SIZE);
    }

    printf("done.\n");

    /*
     *  Verify flash block data
     */
    printf("DMA mode verify HyperRAM block 0x%x data...", TEST_BLOCK_ADDR);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        SPIM_HYPER_DMARead(spim, TEST_BLOCK_ADDR + offset, g_buff, BUFFER_SIZE);

        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if ((*pData) != ((i << 16) | (TEST_BLOCK_ADDR + offset + i)))
            {
                printf("FAILED!\n");
                printf("\tHyperRAM address 0x%x, read 0x%x, expect 0x%x!\n",
                       TEST_BLOCK_ADDR + i, *pData, (i << 16) | (TEST_BLOCK_ADDR + offset + i));
                return -1;
            }
        }

    }

    printf("done.\n");

    if (u32CipherEn != SPIM_HYPER_OP_ENABLE)
    {
        return 0;
    }

    memset(g_buff, 0, 64);
    printf("\n\nDump HyperRAM with command read. It's the SPIM cipher encrypted text.\n");

    for (offset = 0; offset < 64; offset += 2)
    {
        g_buff[offset] = SPIM_HYPER_Read1Word(spim, TEST_BLOCK_ADDR + offset);
    }

    DumpBufferHex(g_buff, 64);

    memset(g_buff, 0, 64);
    printf("\n\nDump HyperRAM with DMA read. It's the plain text. "
           "The cipher text was decrypted by SPIM while doing DMA read.\n");
    SPIM_HYPER_DMARead(spim, TEST_BLOCK_ADDR + offset, g_buff, BUFFER_SIZE);
    DumpBufferHex(g_buff, 64);

    SPIM_HYPER_DISABLE_CIPHER(spim);

    return 0;
}

int main()
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O    */
    SYS_Init();

    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

	#if 0
    CLK_EnableModuleClock(GPIOB_MODULE);
		GPIO_SetMode(PB, BIT6, GPIO_MODE_OUTPUT);
	
	  while(1)
		{
		    PB6 = 0;
			  for(volatile int i=0; i< 0x100000; i++)
			  {
			  }
				PB6 = 1;
			  for(volatile int i=0; i< 0x100000; i++)
			  {
			  }
		}
		#else
    CLK_EnableModuleClock(GPIOH_MODULE);
		GPIO_SetMode(PH, BIT13, GPIO_MODE_OUTPUT);
	
	  while(1)
		{
		    PH13 = 0;
			  for(volatile int i=0; i< 0x100000; i++)
			  {
			  }
				PH13 = 1;
			  for(volatile int i=0; i< 0x100000; i++)
			  {
			  }
		}
		#endif
		
	

    printf("+----------------------------------------+\n");
    printf("|       HyperRAM read/write sample       |\n");
    printf("+----------------------------------------+\n");

    HyperRAM_Init(SPIM_PORT);

    /* Set Cipher Key and protection region */
    OTFC_SetKeyFromKeyReg(OTFC_PORT, gau32AESKey, OTFC_PR_0,
                          TEST_BLOCK_ADDR, BUFFER_SIZE);
    OTFC_ENABLE_PR(OTFC_PORT, OTFC_PR_0);

    /* DMA mode read/write cipher disable */
    dma_read_write(SPIM_PORT, SPIM_HYPER_OP_DISABLE);

    /* DMM mode read/write cipher disable */
    dmm_read_write(SPIM_PORT, SPIM_HYPER_OP_DISABLE);

    /* DMA mode read/write cipher enable */
    dma_read_write(SPIM_PORT, SPIM_HYPER_OP_ENABLE);

    /* DMM mode read/write cipher enable */
    dmm_read_write(SPIM_PORT, SPIM_HYPER_OP_ENABLE);

    /* Lock protected registers */
    SYS_LockReg();

    printf("HyperRAM DMA/DMM Mode R/W Sample Done\r\n");

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
