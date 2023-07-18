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

//------------------------------------------------------------------------------
NVT_NONCACHEABLE __attribute__((aligned(32))) uint8_t g_buff[BUFFER_SIZE] = {0};

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

    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOD_MODULE);
    CLK_EnableModuleClock(GPIOE_MODULE);
    CLK_EnableModuleClock(GPIOF_MODULE);
    CLK_EnableModuleClock(GPIOG_MODULE);
    CLK_EnableModuleClock(GPIOH_MODULE);
    CLK_EnableModuleClock(GPIOI_MODULE);
    CLK_EnableModuleClock(GPIOJ_MODULE);

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

int hyperram_read_write(SPIM_T *spim)
{
    uint32_t i = 0, offset = 0;             /* variables */
    uint32_t *pData = NULL;

    /*
     *  Erase flash page
     */
    printf("\tErase and verify HyperRAM block 0x%x...", TEST_BLOCK_ADDR);
    HyperRAM_Erase(spim, TEST_BLOCK_ADDR, FLASH_BLOCK_SIZE);
    printf("done.\n");

    /*
     *  Program data to flash block
     */
    printf("\tProgram sequential data to HyperRAM block 0x%x...", TEST_BLOCK_ADDR);

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
    printf("\tVerify HyperRAM block 0x%x data...", TEST_BLOCK_ADDR);

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

    //SPIM_SetDMMAddrNonCacheable();

    printf("+----------------------------------------+\n");
    printf("|    M55M1 HyperRAM read/write sample    |\n");
    printf("+----------------------------------------+\n");

    SYS_UnlockReg();                        /* Unlock protected registers                      */

    HyperRAM_Init(SPIM0);

    hyperram_read_write(SPIM0);

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
