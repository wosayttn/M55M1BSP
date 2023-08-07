/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   To configure memory region as non-executable region
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

/* Base address and size must be 32 byte aligned */
#define TEST_REGION_1_BASE_ADDR     (FMC_APROM_BASE + 0x10000)
#define TEST_REGION_1_SIZE          (0x10000)
#define TEST_REGION_1_END_ADDR      (TEST_REGION_1_BASE_ADDR + TEST_REGION_1_SIZE - 1)

#define TEST_REGION_2_BASE_ADDR     (SRAM0_BASE)
#define TEST_REGION_2_SIZE          (0x10000)
#define TEST_REGION_2_END_ADDR      (TEST_REGION_2_BASE_ADDR + TEST_REGION_2_SIZE - 1)

#define TEST_REGION_3_BASE_ADDR     (SRAM0_BASE + 0x20000)
#define TEST_REGION_3_SIZE          (0x10000)
#define TEST_REGION_3_END_ADDR      (TEST_REGION_3_BASE_ADDR + TEST_REGION_3_SIZE - 1)

/* ExeInRegion1 located in TEST_REGION_1 */
uint32_t ExeInRegion1(void) __attribute__((section(".ARM.__at_0x110000")));
/* ExeInRegion2 located in TEST_REGION_2 */
uint32_t ExeInRegion2(void);
/* ExeInRegion3 located in TEST_REGION_3 */
uint32_t ExeInRegion3(void);

void MemManage_Handler(void)
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
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Enable PLL0 180MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_180MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to PLL0 */
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
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /* Lock protected registers */
    SYS_LockReg();
}

void MPU_TestExecutable(void)
{
    /* Disable I-Cache and D-Cache */
    SCB_DisableICache();
    SCB_DisableDCache();

    /* Configure MPU memory attribute */
    /*
     * Attribute 1
     * Memory Type = Normal
     * Attribute = Outer Non-cacheable, Inner Non-cacheable
     */
    ARM_MPU_SetMemAttr(1UL, ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));

    /* Configure MPU memory regions */
    ARM_MPU_SetRegion(1UL,
                      ARM_MPU_RBAR(TEST_REGION_1_BASE_ADDR, ARM_MPU_SH_NON, 1, 0, 1),     /* Non-shareable, read-only, privileged, non-executable */
                      ARM_MPU_RLAR(TEST_REGION_1_END_ADDR, 1)                             /* Use Attr 1 */
                     );

    ARM_MPU_SetRegion(2UL,
                      ARM_MPU_RBAR(TEST_REGION_2_BASE_ADDR, ARM_MPU_SH_NON, 1, 0, 0),     /* Non-shareable, read-only, privileged, executable */
                      ARM_MPU_RLAR(TEST_REGION_2_END_ADDR, 1)                             /* Use Attr 1 */
                     );

    ARM_MPU_SetRegion(3UL,
                      ARM_MPU_RBAR(TEST_REGION_3_BASE_ADDR, ARM_MPU_SH_NON, 1, 0, 1),     /* Non-shareable, read-only, privileged, non-executable */
                      ARM_MPU_RLAR(TEST_REGION_3_END_ADDR, 1)                             /* Use Attr 1 */
                     );

    /* Enable MPU */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
    /* Enable I-Cache and D-Cache */
    SCB_EnableDCache();
    SCB_EnableICache();

    printf("\n==============================================\n");
    printf(" Memory Region 1 (Flash Memory) configuration:\n");
    printf("==============================================\n");
    printf(" Start address: 0x%08X\n", (uint32_t)TEST_REGION_1_BASE_ADDR);
    printf(" End address  : 0x%08X\n", (uint32_t)TEST_REGION_1_END_ADDR);
    printf(" Size         : %d KB\n", (uint32_t)(TEST_REGION_1_SIZE / 1024));
    printf(" Memory Type  : Normal\n");
    printf(" Permission   : Non-executable\n");
    printf("----------------------------------------------\n");
    printf("Press any key to test execute code from memory region 1 (Flash Memory).\n");

    printf("(It should trigger a memory fault exception.)\n");
    getchar();
    ExeInRegion1();
    /* Becuase MPU is disabled in MemManage_Handler, enable MPU again here */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

    printf("\n==============================================\n");
    printf("Memory Region 2 (SRAM Memory) configuration:\n");
    printf("==============================================\n");
    printf(" Start address: 0x%08X\n", (uint32_t)TEST_REGION_2_BASE_ADDR);
    printf(" End address  : 0x%08X\n", (uint32_t)TEST_REGION_2_END_ADDR);
    printf(" Size         : %d KB\n", (uint32_t)(TEST_REGION_2_SIZE / 1024));
    printf(" Memory Type  : Normal\n");
    printf(" Permission   : Executable\n");
    printf("----------------------------------------------\n");

    printf("Press any key to test execute code from memory region 2 (SRAM Memory).\n");
    getchar();
    ExeInRegion2();

    printf("\n==============================================\n");
    printf("Memory Region 3 (SRAM Memory) configuration:\n");
    printf("==============================================\n");
    printf(" Start address: 0x%08X\n", (uint32_t)TEST_REGION_3_BASE_ADDR);
    printf(" End address  : 0x%08X\n", (uint32_t)TEST_REGION_3_END_ADDR);
    printf(" Size         : %d KB\n", (uint32_t)(TEST_REGION_3_SIZE / 1024));
    printf(" Memory Type  : Normal\n");
    printf(" Permission   : Non-executable\n");
    printf("----------------------------------------------\n");

    printf("Press any key to test execute code from memory region 3 (SRAM Memory).\n");
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
