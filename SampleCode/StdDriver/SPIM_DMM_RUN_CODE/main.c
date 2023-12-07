/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    This sample shows how to make an application booting from APROM
 *           with a sub-routine resided on SPIM flash.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>

#include "NuMicro.h"

//------------------------------------------------------------------------------
#define USE_4_BYTES_MODE            0   /* W25Q20 does not support 4-bytes address mode. */
#define SPIM_CIPHER_ON              0

//------------------------------------------------------------------------------
void spim_routine(void);

//------------------------------------------------------------------------------
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

    /* Enable UART module clock */
    SetDebugUartCLK();

    /* Enable GPIO Module clock */
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOG_MODULE);

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

int main()
{
    uint8_t idBuf[3] = {0};
    /* 0xEB: CMD_DMA_FAST_QUAD_READ Command Phase Table */
    SPIM_PHASE_T sWbEBhRdCMD =
    {
        CMD_DMA_FAST_QUAD_READ,                                                     // Command Code
        PHASE_NORMAL_MODE, PHASE_WIDTH_8, PHASE_DISABLE_DTR,                        // Command Phase
        PHASE_QUAD_MODE, PHASE_WIDTH_24, PHASE_DISABLE_DTR,                         // Address Phase
        PHASE_QUAD_MODE, PHASE_ORDER_MODE0, PHASE_DISABLE_DTR,                      // Data Phase
        4,                                                                          // Dummy Cycle Phase
        PHASE_ENABLE_CONT_READ, PHASE_QUAD_MODE, PHASE_WIDTH_8, PHASE_DISABLE_DTR,  // Read Mode Phase
    };

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O    */
    SYS_Init();

    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

    SPIM_SetDMMAddrNonCacheable();

    printf("+--------------------------------------------------+\n");
    printf("|      SPIM DMM mode running program on flash      |\n");
    printf("+--------------------------------------------------+\n");

    SPIM_SET_CLOCK_DIVIDER(SPIM0, 2);       /* Set SPIM clock as HCLK divided by 4 */

    SPIM_SET_RXCLKDLY_RDDLYSEL(SPIM0, 0);   /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */

    SPIM_DISABLE_CIPHER(SPIM0);             /* Disable SPIM Cipher */

    if (SPIM_InitFlash(SPIM0, 1) != 0)      /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        goto lexit;
    }

    SPIM_ReadJedecId(SPIM0, idBuf, sizeof(idBuf), 1, 0);
    printf("SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n",
           idBuf[0], idBuf[1], idBuf[2]);

#if (SPIM_REG_CACHE == 1) //TESTCHIP_ONLY not support
    SPIM_ENABLE_CACHE(SPIM0);
    SPIM0->CTL1 |= SPIM_CTL1_CDINVAL_Msk;        // invalid cache
#endif

    SPIM_DMADMM_InitPhase(SPIM0, &sWbEBhRdCMD, SPIM_CTL0_OPMODE_DIRECTMAP);
    SPIM_EnterDirectMapMode(SPIM0, USE_4_BYTES_MODE, sWbEBhRdCMD.u32CMDCode, 8);

    while (1)
    {
        printf("\n\nProgram is currently running on APROM flash.\n");
        printf("Press any key to branch to sub-routine on SPIM flash...\n");

        getchar();

        spim_routine();
    }

lexit:

    /* Lock protected registers */
    SYS_LockReg();

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
