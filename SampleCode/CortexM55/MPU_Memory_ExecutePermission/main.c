/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Configure memory region as non-executable region
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "NuMicro.h"

/*
 * Reference: https://www.keil.com/pack/doc/CMSIS/Core/html/group__mpu8__functions.html
 */

/* Base address and size must be 32-byte aligned */
#define REGION_FLASH_XN_BASE_ADDR     (FMC_APROM_BASE + 0x10000)
#define REGION_FLASH_XN_SIZE          (0x10000)
#define REGION_FLASH_XN_END_ADDR      (REGION_FLASH_XN_BASE_ADDR + REGION_FLASH_XN_SIZE - 1)

#define REGION_SRAM_nXN_BASE_ADDR     (SRAM0_BASE)
#define REGION_SRAM_nXN_SIZE          (0x10000)
#define REGION_SRAM_nXN_END_ADDR      (REGION_SRAM_nXN_BASE_ADDR + REGION_SRAM_nXN_SIZE - 1)

#define REGION_SRAM_XN_BASE_ADDR     (SRAM0_BASE + 0x20000)
#define REGION_SRAM_XN_SIZE          (0x10000)
#define REGION_SRAM_XN_END_ADDR      (REGION_SRAM_XN_BASE_ADDR + REGION_SRAM_XN_SIZE - 1)

/* ExeInRegion1 located in REGION_FLASH_XN */
uint32_t ExeInRegion1(void) __attribute__((section(".ARM.__at_0x110000")));
/* ExeInRegion2 located in REGION_SRAM_nXN */
uint32_t ExeInRegion2(void);
/* ExeInRegion3 located in REGION_SRAM_XN */
uint32_t ExeInRegion3(void);

NVT_ITCM void MemManage_Handler(void)
{
    uint32_t u32LR = 0;
    uint32_t *pu32SP;

    __ASM volatile("mov %0, lr\n" : "=r"(u32LR));

    if (u32LR & BIT2)
        pu32SP = (uint32_t *)__get_PSP();
    else
        pu32SP = (uint32_t *)__get_MSP();

    /*
     * 1. Disable MPU to allow simple return from MemManage_Handler().
     *    MemManage fault typically indicates code failure, and would
     *    be resolved by reset or terminating faulty thread in OS.
     * 2: Call ARM_MPU_Disable() will allow the code touch
     *    illegal address to be executed after return from
     *    HardFault_Handler(). If this line is comment out, this code
     *    will keep enter MemManage_Handler().
     */
    ARM_MPU_Disable();
    printf("\n  Memory Fault !\n");

    if (SCB->CFSR & SCB_CFSR_IACCVIOL_Msk)
    {
        printf("  Instruction access violation flag is raised.\n");
        /* Check disassembly code of MemManage_Handler to get stack offset of return address */
        printf("  Fault address: 0x%08X\n", pu32SP[10]);
    }

    if (SCB->CFSR & SCB_CFSR_DACCVIOL_Msk)
    {
        printf("  Data access violation flag is raised.\n");

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

    /* Enable UART module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /* Lock protected registers */
    SYS_LockReg();
}

#define RO(set)     (set)   /* Read-Only: Set to 1 for a read-only memory region.                   */
#define NP(set)     (set)   /* Non-Privileged: Set to 1 for a non-privileged memory region.         */
#define XN(set)     (set)   /* eXecute Never: Set to 1 for a non-executable memory region.          */

void MPU_TestExecutable(void)
{
    uint8_t     u8AttrIdxNonCache = 1;
    uint32_t    u32RegionFlash_XN = 1,
                u32RegionSRAM_nXN = 2,
                u32RegionSRAM_XN  = 3;

    /* Disable I-Cache and D-Cache before config MPU */
    SCB_DisableICache();
    SCB_DisableDCache();

    /* Configure MPU memory attribute */
    /*
     * Attribute Non-cacheable
     * Memory Type = Normal
     * Attribute = Outer Non-cacheable, Inner Non-cacheable
     */
    ARM_MPU_SetMemAttr(1UL, ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));

    /* Configure MPU memory regions */
    ARM_MPU_SetRegion(u32RegionFlash_XN,
                      /*           Base address               Non-shareable,  read-only, privileged, non-executable   */
                      ARM_MPU_RBAR(REGION_FLASH_XN_BASE_ADDR, ARM_MPU_SH_NON, RO(TRUE),  NP(FALSE),  XN(TRUE)),
                      /*           Limit address             Attribute index */
                      ARM_MPU_RLAR(REGION_FLASH_XN_END_ADDR, u8AttrIdxNonCache)
                     );

    ARM_MPU_SetRegion(u32RegionSRAM_nXN,
                      /*           Base address               Non-shareable,  read-only, privileged, executable        */
                      ARM_MPU_RBAR(REGION_SRAM_nXN_BASE_ADDR, ARM_MPU_SH_NON, RO(TRUE),  NP(FALSE),  XN(FALSE)),
                      /*           Limit address             Attribute index */
                      ARM_MPU_RLAR(REGION_SRAM_nXN_END_ADDR, u8AttrIdxNonCache)
                     );

    ARM_MPU_SetRegion(u32RegionSRAM_XN,
                      /*           Base address              Non-shareable,  read-only, privileged, non-executable   */
                      ARM_MPU_RBAR(REGION_SRAM_XN_BASE_ADDR, ARM_MPU_SH_NON, RO(TRUE),  NP(FALSE),  XN(TRUE)),
                      /*           Limit address            Attribute index */
                      ARM_MPU_RLAR(REGION_SRAM_XN_END_ADDR, u8AttrIdxNonCache)
                     );

    /* Enable MPU */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
    /* Enable I-Cache and D-Cache */
    SCB_EnableDCache();
    SCB_EnableICache();

    printf("\n==============================================\n");
    printf(" Memory Region %d (Flash Memory) configuration:\n", u32RegionFlash_XN);
    printf("==============================================\n");
    printf(" Start address: 0x%08X\n", (uint32_t)REGION_FLASH_XN_BASE_ADDR);
    printf(" End address  : 0x%08X\n", (uint32_t)REGION_FLASH_XN_END_ADDR);
    printf(" Size         : %d KB\n", (uint32_t)(REGION_FLASH_XN_SIZE / 1024));
    printf(" Memory Type  : Normal\n");
    printf(" Permission   : Non-executable\n");
    printf("----------------------------------------------\n");
    printf("Press any key to test execute code from memory region %d (Flash Memory).\n", u32RegionFlash_XN);

    printf("(It should trigger a memory fault exception.)\n");
    getchar();
    ExeInRegion1();
    /* Becuase MPU is disabled in MemManage_Handler, enable MPU again here */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

    printf("\n==============================================\n");
    printf("Memory Region %d (SRAM Memory) configuration:\n", u32RegionSRAM_nXN);
    printf("==============================================\n");
    printf(" Start address: 0x%08X\n", (uint32_t)REGION_SRAM_nXN_BASE_ADDR);
    printf(" End address  : 0x%08X\n", (uint32_t)REGION_SRAM_nXN_END_ADDR);
    printf(" Size         : %d KB\n", (uint32_t)(REGION_SRAM_nXN_SIZE / 1024));
    printf(" Memory Type  : Normal\n");
    printf(" Permission   : Executable\n");
    printf("----------------------------------------------\n");

    printf("Press any key to test execute code from memory region %d (SRAM Memory).\n", u32RegionSRAM_nXN);
    getchar();
    ExeInRegion2();

    printf("\n==============================================\n");
    printf("Memory Region %d (SRAM Memory) configuration:\n", u32RegionSRAM_XN);
    printf("==============================================\n");
    printf(" Start address: 0x%08X\n", (uint32_t)REGION_SRAM_XN_BASE_ADDR);
    printf(" End address  : 0x%08X\n", (uint32_t)REGION_SRAM_XN_END_ADDR);
    printf(" Size         : %d KB\n", (uint32_t)(REGION_SRAM_XN_SIZE / 1024));
    printf(" Memory Type  : Normal\n");
    printf(" Permission   : Non-executable\n");
    printf("----------------------------------------------\n");

    printf("Press any key to test execute code from memory region %d (SRAM Memory).\n", u32RegionSRAM_XN);
    printf("(It should trigger a memory fault exception.)\n");
    getchar();
    ExeInRegion3();
}

int main()
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART for print message */
    InitDebugUart();

    printf("+------------------------------------------+\n");
    printf("| M55M1 MPU Execute Permission Sample Code |\n");
    printf("+------------------------------------------+\n");
    printf("MPU region count: %d\n", (uint32_t)(MPU->TYPE & MPU_TYPE_DREGION_Msk) >> MPU_TYPE_DREGION_Pos);
    MPU_TestExecutable();

    printf("\nDone\n");

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
