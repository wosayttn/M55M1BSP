/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   To configure memory region as write-back or write-through region
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NuMicro.h"

/*
 * Reference: https://www.keil.com/pack/doc/CMSIS/Core/html/group__mpu8__functions.html
 */

/* Base address and size of cacheable buffer must be 32 byte aligned */
#define TEST_BUF_SIZE       (1024)
uint32_t g_au32CachedBuf_WT[DCACHE_ALIGN_LINE_SIZE(TEST_BUF_SIZE / 4)] __ALIGNED(DCACHE_LINE_SIZE);
uint32_t g_au32CachedBuf_WB[DCACHE_ALIGN_LINE_SIZE(TEST_BUF_SIZE / 4)] __ALIGNED(DCACHE_LINE_SIZE);
uint32_t g_au32PDMA_DstBuf [DCACHE_ALIGN_LINE_SIZE(TEST_BUF_SIZE / 4)] __ALIGNED(DCACHE_LINE_SIZE);

NVT_ITCM void MemManage_Handler(void)
{
    uint32_t u32LR = 0;
    uint32_t *pu32SP;

    __ASM volatile("mov %0, lr\n" : "=r"(u32LR));

    if (u32LR & BIT2)
        pu32SP = (uint32_t *)__get_PSP();
    else
        pu32SP = (uint32_t *)__get_MSP();

    /* NOTE1: Disable MPU to allow simple return from MemManage_Handler().
              MemManage fault typically indicates code failure, and would
              be resolved by reset or terminating faulty thread in OS.
       NOTE2: Call ARM_MPU_Disable() will allow the code touch
              illegal address to be executed after return from
              HardFault_Handler(). If this line is comment out, this code
              will keep enter MemManage_Handler(). */
    ARM_MPU_Disable();
    printf("\n  Memory Fault !\n");

    if (SCB->CFSR & SCB_CFSR_IACCVIOL_Msk)
    {
        printf("  Instruction access violation flag is raised.");
        /* Check disassembly code of MemManage_Handler to get stack offset of return address */
        printf("  Fault address: 0x%08X\n", pu32SP[10]);
    }

    if (SCB->CFSR & SCB_CFSR_DACCVIOL_Msk)
    {
        printf("  Data access violation flag is raised.");

        if (SCB->CFSR & SCB_CFSR_MMARVALID_Msk)
            printf("  Fault address: 0x%08X\n", SCB->MMFAR);
    }

    /* Clear MemManage fault status register */
    SCB->CFSR = SCB_CFSR_MEMFAULTSR_Msk;
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable PLL0 180MHz clock from HIRC and switch SCLK clock source to PLL0 */
    CLK_SetBusClock(CLK_SCLKSEL_SCLKSEL_APLL0, FREQ_180MHZ);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable module clock */
    CLK_EnableModuleClock(PDMA0_MODULE);

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /* Lock protected registers */
    SYS_LockReg();
}

int32_t PDMA_Transfer(uint32_t u32Chan, uint32_t *pu32SrcBuf, uint32_t u32TranByteSize)
{
    /* Open Channel */
    PDMA_Open(PDMA0, (1 << u32Chan));
    /* Transfer count is u32TranByteSize / 4, transfer width is 32 bits(one word) */
    PDMA_SetTransferCnt(PDMA0, u32Chan, PDMA_WIDTH_32, u32TranByteSize / 4);
    /* Set source address is pu32SrcBuf, destination address is g_au32PDMA_DstBuf, Source/Destination increment size is 32 bits(one word) */
    PDMA_SetTransferAddr(PDMA0, u32Chan, (uint32_t)pu32SrcBuf, PDMA_SAR_INC, (uint32_t)g_au32PDMA_DstBuf, PDMA_DAR_INC);
    /* Request source is memory to memory */
    PDMA_SetTransferMode(PDMA0, u32Chan, PDMA_MEM, FALSE, 0);
    /* Transfer type is burst transfer and burst size is 4 */
    PDMA_SetBurstType(PDMA0, u32Chan, PDMA_REQ_BURST, PDMA_BURST_4);

    /* Generate a software request to trigger transfer with PDMA channel */
    PDMA_Trigger(PDMA0, u32Chan);

    while ((PDMA_GET_INT_STATUS(PDMA0) & (PDMA_INTSTS_TDIF_Msk | PDMA_INTSTS_ABTIF_Msk)) == 0)
        ;

    if (PDMA_GET_TD_STS(PDMA0) & (PDMA_TDSTS_TDIF0_Msk << u32Chan))
        PDMA_CLR_TD_FLAG(PDMA0, (PDMA_TDSTS_TDIF0_Msk << u32Chan));

    if (PDMA_GET_ABORT_STS(PDMA0) & (PDMA_ABTSTS_ABTIF0_Msk << u32Chan))
    {
        /* Clear abort flag of channel */
        PDMA_CLR_ABORT_FLAG(PDMA0, (PDMA_ABTSTS_ABTIF0_Msk << u32Chan));
        printf("PDMA transfer abort !\n");
        return -1;
    }

    PDMA_Close(PDMA0);
    SCB_InvalidateDCache_by_Addr(g_au32PDMA_DstBuf, u32TranByteSize);
    return 0;
}

int32_t TestCacheWrite(uint32_t *pu32BaseAddr, uint32_t u32ByteSize, uint32_t bWriteBack, uint32_t bClean)
{
    uint8_t  au8TestPattern[] = { 0x55, 0xAA };
    int32_t  i;
    uint32_t u32Offset, u32DataErrCnt;

    memset(g_au32PDMA_DstBuf, 0x0, sizeof(g_au32PDMA_DstBuf));
    SCB_CleanDCache_by_Addr(g_au32PDMA_DstBuf, sizeof(g_au32PDMA_DstBuf));

    for (i = 0; i < (sizeof(au8TestPattern) / sizeof(au8TestPattern[0])); i++)
    {
        memset(pu32BaseAddr, 0x0, u32ByteSize);
        /* Write data with CPU */
        /* Write back mode   : data is write to cache
         * Write through mode: data is write to cache and memory
         */
        printf("CPU write (addr + 0x%02X)\n", au8TestPattern[i]);

        for (u32Offset = 0; u32Offset < u32ByteSize; u32Offset += 4)
        {
            pu32BaseAddr[u32Offset / 4] = (u32Offset + au8TestPattern[i]);
        }

        /* Check with CPU */
        printf("CPU check ");

        for (u32Offset = 0; u32Offset < u32ByteSize; u32Offset += 4)
        {
            if (pu32BaseAddr[u32Offset / 4] != (u32Offset + au8TestPattern[i]))
            {
                printf("[Error] CPU read (0x%08X != 0x%08X) !\n", pu32BaseAddr[u32Offset / 4], (u32Offset + au8TestPattern[i]));
                return -1;
            }
        }

        printf("Pass\n");

        if (bClean)
        {
            printf("Clean data cache\n");
            SCB_CleanDCache_by_Addr(pu32BaseAddr, u32ByteSize);
        }

        /* Check with PDMA */
        printf("PDMA transfer\n");

        if (PDMA_Transfer(0, pu32BaseAddr, u32ByteSize) != 0)
        {
            printf("[Error] Failed to do PDMA transfer !\n");
            return -2;
        }

        u32DataErrCnt = 0;

        for (u32Offset = 0; u32Offset < u32ByteSize; u32Offset += 4)
        {
            if (g_au32PDMA_DstBuf[u32Offset / 4] != pu32BaseAddr[u32Offset / 4])
            {
                if (u32DataErrCnt == 0)
                    printf("  Check (0x%08X != 0x%08X).\n", g_au32PDMA_DstBuf[u32Offset / 4], pu32BaseAddr[u32Offset / 4]);

                u32DataErrCnt++;
            }
        }

        if (bWriteBack && (bClean == FALSE))
        {
            /* Write back mode
             *   If CPU did not use SCB_CleanDCache to flush dirty data from cache to memory,
             *   data in memory is inconsistent with data in cache and PDMA copied old data.
             */
            printf("  [Write Back] Data error count: %d\n", u32DataErrCnt);

            if (u32DataErrCnt == 0)
                printf("  [Error] Check PDMA copied old data - Failed !\n");
            else
                printf("  Check PDMA copied old data - Pass.\n");
        }
        else
        {
            if (bWriteBack && (bClean == TRUE))
            {
                printf("  [Write Back & CleanDCache] Data error count: %d\n", u32DataErrCnt);
            }
            else
            {
                /* Write through mode
                *   CPU always write to cache and memory directly,
                *   data in memory is consistent with data in cache and PDMA copied new data.
                */
                printf("  [Write Through] Data error count: %d\n", u32DataErrCnt);
            }

            if (u32DataErrCnt > 0)
                printf("  [Error] Check PDMA copied new data - Failed !\n");
            else
                printf("  Check PDMA copied new data - Pass.\n");
        }
    }

    return 0;
}

void MPU_TestWB_WT(void)
{
    /* Disable I-Cache and D-Cache */
    SCB_DisableICache();
    SCB_DisableDCache();

    /* Configure MPU memory attribute */
    /* Attribute 1
     * Memory Type = Normal
     * Attribute   = Innre NT, WT, RA, WA
     */
    ARM_MPU_SetMemAttr(1UL, ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(1, 0, 1, 1), ARM_MPU_ATTR_MEMORY_(1, 0, 1, 1)));
    /*
     * Attribute 2
     * Memory Type = Normal
     * Attribute   = Innre NT, WB, RA, WA
     */
    ARM_MPU_SetMemAttr(2UL, ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(1, 1, 1, 1), ARM_MPU_ATTR_MEMORY_(1, 1, 1, 1)));

    /* Configure MPU memory regions */
    /*
     * Region 1 (SRAM Memory Space)
     * Start address = g_au32CachedBuf_WT
     * Shareablity   = Non-shareable
     * Size          = TEST_BUF_SIZE
     * Permission    = Cacheable with Write Through
     */
    ARM_MPU_SetRegion(1UL,                                                       /* Region 0 */
                      ARM_MPU_RBAR((uint32_t)g_au32CachedBuf_WT, ARM_MPU_SH_NON, 0, 0, 1),     /* Non-shareable, read/write, privileged, non-executable */
                      ARM_MPU_RLAR((uint32_t)g_au32CachedBuf_WT + TEST_BUF_SIZE - 1, 1)        /* Use Attr 1 */
                     );

    /*
     * Region 2 (SRAM Memory Space)
     * Start address = g_au32CachedBuf_WB
     * Shareablity   = Non-shareable
     * Size          = TEST_BUF_SIZE
     * Permission    = Cacheable with Write Back
     */
    ARM_MPU_SetRegion(2UL,                                                       /* Region 1 */
                      ARM_MPU_RBAR((uint32_t)g_au32CachedBuf_WB, ARM_MPU_SH_NON, 0, 0, 1),     /* Non-shareable, read/write, privileged, non-executable */
                      ARM_MPU_RLAR((uint32_t)g_au32CachedBuf_WB + TEST_BUF_SIZE - 1, 2)        /* Use Attr 2 */
                     );

    /* Enable MPU */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
    /* Enable I-Cache and D-Cache */
    SCB_EnableDCache();
    SCB_EnableICache();

    printf("\n==============================================\n");
    printf(" Memory Region 1 (SRAM Memory) configuration:\n");
    printf("==============================================\n");
    printf(" Start address: 0x%08X\n", (uint32_t)g_au32CachedBuf_WT);
    printf(" Size         : %d KB\n", (uint32_t)(TEST_BUF_SIZE / 1024));
    printf(" Memory Type  : Normal\n");
    printf(" Permission   : Cacheable with Write Through\n");
    printf("----------------------------------------------\n");

    printf("Press any key to test write through with memory region 0.\n");
    getchar();

    if (TestCacheWrite(g_au32CachedBuf_WT, TEST_BUF_SIZE, FALSE, FALSE) != 0)
        printf("[Error] !\n");

    printf("\n==============================================\n");
    printf(" Memory Region 2 (SRAM Memory) configuration:\n");
    printf("==============================================\n");
    printf(" Start address: 0x%08X\n", (uint32_t)g_au32CachedBuf_WB);
    printf(" Size         : %d KB\n", (uint32_t)(TEST_BUF_SIZE / 1024));
    printf(" Memory Type  : Normal\n");
    printf(" Permission   : Cacheable with Write Back\n");
    printf("----------------------------------------------\n");

    printf("Press any key to test write back with memory region 1.\n");
    getchar();

    if (TestCacheWrite(g_au32CachedBuf_WB, TEST_BUF_SIZE, TRUE, FALSE) != 0)
        printf("[Error] !\n");

    printf("\n----------------------------------------------\n");
    printf(" CleanDCache after PDMA transfer\n");
    printf("----------------------------------------------\n");

    if (TestCacheWrite(g_au32CachedBuf_WB, TEST_BUF_SIZE, TRUE, TRUE) != 0)
        printf("[Error] !\n");
}

int main()
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART for print message */
    InitDebugUart();

    printf("+------------------------------------------+\n");
    printf("|    M55M1 MPU Cache Update Sample Code    |\n");
    printf("+------------------------------------------+\n");
    printf("MPU region count: %d\n", (uint32_t)(MPU->TYPE & MPU_TYPE_DREGION_Msk) >> MPU_TYPE_DREGION_Pos);
    printf("Address and size of cacheable buffer must be 32 byte alignment.\n");
    MPU_TestWB_WT();

    printf("\nDone\n");

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
