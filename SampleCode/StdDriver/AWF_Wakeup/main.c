/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Template for M55M1 series MCU
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "NuMicro.h"

void LPTMR_Init(void)
{
    /* Set LPTMR mode and frequency */
    LPTMR_Open(LPTMR0, LPTMR_PERIODIC_MODE, 2);

    /* Set LPTMR timeout event to trigger LPPDMA */
    LPTMR0->TRGCTL = (LPTMR0->TRGCTL & ~LPTMR_TRGCTL_TRGSSEL_Msk) |
                     (LPTMR_TRGSRC_TIMEOUT_EVENT);

    LPTMR0->TRGCTL = (LPTMR0->TRGCTL & ~(LPTMR_TRGCTL_TRGLPPDMA_Msk)) |
                     (LPTMR_TRG_TO_LPPDMA);

    /* Enable LPTMR clock in power-down mode */
    LPTMR0->CTL |= (LPTMR_CTL_PDCLKEN_Msk);
}

void LPPDMA_Init(void)
{
    uint32_t ch = 1;

    /* Open Channel 1 */
    LPPDMA_Open(LPPDMA, BIT0 << ch);

    /* Transfer count is 8, transfer width is 16 bits */
    LPPDMA_SetTransferCnt(LPPDMA, ch, LPPDMA_WIDTH_16, 8);

    /* Set source address is LPSRAM_BASE, destination address is AWF->DAT */
    LPPDMA_SetTransferAddr(LPPDMA, ch, LPSRAM_BASE, LPPDMA_SAR_INC, (uint32_t) & (AWF->DAT), LPPDMA_DAR_FIX);

    /* Set transfer mode to LPPDMA_LPTMR0 */
    LPPDMA_SetTransferMode(LPPDMA, ch, LPPDMA_LPTMR0, FALSE, 0);

    /* Transfer type is burst transfer and burst size is 1 */
    LPPDMA_SetBurstType(LPPDMA, ch, LPPDMA_REQ_SINGLE, 0);

    /* Enable interrupt */
    LPPDMA_CLR_TD_FLAG(LPPDMA, LPPDMA_TDSTS_TDIF0_Msk << ch);
}

void AWF_Wakeup_Test(void)
{
    uint32_t u32ACCCount = 8;
    uint32_t u32HTHValue = 20;
    uint32_t u32LTHValue = 0;
    uint32_t u32WBINITValue = 0;

    /* Set AWF function */
    AWF_Open(AWF_HTINT_ENABLE, AWF_HTWK_ENABLE, u32HTHValue, u32LTHValue, u32WBINITValue, u32ACCCount);

    /* Low power timer initialize */
    LPTMR_Init();

    /* Low power PDMA initialize */
    LPPDMA_Init();

    /* Select power-down mode and power level */
    PMC_SetPowerDownMode(PMC_NPD3, PMC_PLCTL_PLSEL_PL0);

    printf("Enter Power-down...\n");

    /* Start LPTMR */
    LPTMR_Start(LPTMR0);

    PMC_PowerDown();

    printf("Wake-up!!\n");

    /* LPTMR and LPPDMA close */
    LPTMR_Stop(LPTMR0);
    LPTMR_ClearIntFlag(LPTMR0);
    NVIC_DisableIRQ(LPPDMA_IRQn);
    LPPDMA_Close(LPPDMA);
}

void AWF_IRQHandler(void)
{
    uint32_t u32HTH_Flag,u32LTH_Flag,u32AccumulationValue;
    
    u32HTH_Flag = AWF_GET_HTH_INTFLAG();
    u32LTH_Flag = AWF_GET_LTH_INTFLAG();
    u32AccumulationValue = AWF_GET_ACUVAL();
    /* Enable AWF0 module clock */
    CLK_EnableModuleClock(AWF0_MODULE);

    if (AWF_GET_HTH_INTFLAG())
    {
        printf("AWF HTH Interrupt occured!!!, HTH_INT = %d, LTH_INT = %d\n", u32HTH_Flag, u32LTH_Flag);
        printf("AWFHTH = %d, ACUVAL = %d\n", (uint32_t)((AWF->HTH & AWF_HTH_AWFHTH_Msk) >> AWF_HTH_AWFHTH_Pos), u32AccumulationValue);
        AWF_Close();
        printf("HTH Flag clear!, HTH_INT = %d, LTH_INT = %d\n", u32HTH_Flag, u32LTH_Flag);
    }

    if (AWF_GET_LTH_INTFLAG())
    {
        printf("AWF LTH Interrupt occured!!!, HTH_INT = %d, LTH_INT = %d\n", u32HTH_Flag, u32LTH_Flag);
        printf("AWFLTH = %d, ACUVAL = %d\n", (uint32_t)((AWF->LTH & AWF_LTH_AWFLTH_Msk) >> AWF_LTH_AWFLTH_Pos), u32AccumulationValue);
        AWF_Close();
        printf("LTH Flag clear!, HTH_INT = %d, LTH_INT = %d\n", u32HTH_Flag, u32LTH_Flag);
    }
}

static void SYS_Init(void)
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

    /* Set SCLK  */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_HIRC);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /* Enable LPSRAM0 module clock */
    CLK_EnableModuleClock(LPSRAM0_MODULE);

    /* Enable LPPDMA0 module clock */
    CLK_EnableModuleClock(LPPDMA0_MODULE);

    /* Enable AWF0 module clock */
    CLK_EnableModuleClock(AWF0_MODULE);

    /* Enable LPTMR0 module clock */
    CLK_EnableModuleClock(LPTMR0_MODULE);

    /* Select LPTMR0 clock source to HIRC */
    CLK_SetModuleClock(LPTMR0_MODULE, CLK_LPTMRSEL_LPTMR0SEL_HIRC, 0);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    uint32_t u32AddrOffset = 0x0;
    uint32_t u32AddrOffsetEnd = 0x0e;
    uint32_t u32WriteVlaue = 0;

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    initialise_monitor_handles();
#endif

    /* Unlock protected registers */
    SYS_UnlockReg();

    NVIC_EnableIRQ(AWF_IRQn);

    /* Pre-load data in LPSRAM */
    for (u32AddrOffset = 0; u32AddrOffset <= u32AddrOffsetEnd; u32AddrOffset += 0x2)
    {
        u32WriteVlaue += 1;
        outpw(LPSRAM_BASE + u32AddrOffset, u32WriteVlaue);
    }

    AWF_Wakeup_Test();

    /* Got no where to go, just loop forever */
    while (1) ;
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/