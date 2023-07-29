/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    This sample shows how to make an application booting from APROM
 *           with a sub-routine resided on HyperRAM.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>

#include "NuMicro.h"
#include "hyperram_code.h"

void spim_routine(void);

void SPIM_SetDMMAddrNonCacheable(void)
{
    uint32_t u32DMMAddr = SPIM_GetDMMAddress(SPIM0);

    /* Disable D-Cache */
    SCB_DisableDCache();

    /* Configure MPU memory attribute */
    /*
     * Attribute 0
     * Memory Type = Normal
     * Attribute   = Outer Non-cacheable, Inner Non-cacheable
     */
    ARM_MPU_SetMemAttr(0UL, ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));

    /* Configure MPU memory regions */
    ARM_MPU_SetRegion(0UL,                                                          /* Region 0 */
                      ARM_MPU_RBAR((uint32_t)u32DMMAddr, ARM_MPU_SH_NON, 0, 0, 0),  /* Non-shareable, read/write, privileged, non-executable */
                      ARM_MPU_RLAR((uint32_t)u32DMMAddr + 0x10000, 0)               /* Use Attr 0 */
                     );
    /* Enable MPU */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

    /* Enable D-Cache */
    SCB_EnableDCache();
}

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

int main()
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O    */
    SYS_Init();

    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

    //SPIM_SetDMMAddrNonCacheable();

    printf("+-------------------------------------------------------+\n");
    printf("|       SPIM DMM mode running program on HyperRAM       |\n");
    printf("+-------------------------------------------------------+\n");

    SYS_UnlockReg();                        /* Unlock protected registers                      */

    HyperRAM_Init(SPIM0);

    SPIM_HYPER_EnterDirectMapMode(SPIM0);

    while (1)
    {
        printf("\n\nProgram is currently running on APROM flash.\n");
        printf("Press any key to branch to sub-routine on HyperRAM...\n");

        getchar();

        spim_routine();
    }

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
