/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 23/02/24 9:33a $
 * @brief    Implement CRC in CRC-32 mode with PDMA transfer.
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

volatile uint32_t  g_u32IsTestOver = 0;
volatile uint8_t g_u8CRCDoneFlag = 0; /* 0:No INT, 1:CRC done, 2:CRC abort, 3:CRC unknow fail */

void CRC_IRQHandler(void)
{
    volatile uint32_t reg;

    reg = CRC->DMASTS;
    if ((reg & CRC_DMASTS_ABORTED_Msk) == CRC_DMASTS_ABORTED_Msk)	/* target abort */
    {
        g_u8CRCDoneFlag = 0x2;
        printf("abort flag 0x%x\n", reg);
        CRC->DMASTS |= CRC_DMASTS_ABORTED_Msk;
    }
    if ((reg & CRC_DMASTS_CFGERR_Msk) == CRC_DMASTS_CFGERR_Msk)	/* config error */
    {
        g_u8CRCDoneFlag = 0x4;
        printf("config error 0x%x\n", reg);
        CRC->DMASTS |= CRC_DMASTS_CFGERR_Msk;
    }
    if ((reg & CRC_DMASTS_ACCERR_Msk) == CRC_DMASTS_ACCERR_Msk)	/* access error */
    {
        g_u8CRCDoneFlag = 0x8;
        printf("access error 0x%x\n", reg);
        CRC->DMASTS |= CRC_DMASTS_ACCERR_Msk;
    }
    if ((reg & CRC_DMASTS_FINISH_Msk) == CRC_DMASTS_FINISH_Msk)	/* transfer done */
    {
        g_u8CRCDoneFlag = 0x1;
        printf("*transfer done 0x%x\n", reg);
        CRC->DMASTS |= CRC_DMASTS_FINISH_Msk;
    }
}

void SYS_Init(void)
{

      /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

   /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
  
    /* Enable External RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);

    /* Waiting for External RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

   /* Enable PLL0 200MHz clock */
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
    CLK_SET_PCLK4DIV(4);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable UART0 module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable CRC0 module clock */
    CLK_EnableModuleClock(CRC0_MODULE);

		 /* Debug UART clock setting*/
     SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
     /* Set PB multi-function pins for UART0 RXD and TXD */
    SetDebugUartMFP();

}

uint32_t GetRMCChecksum(uint32_t u32Address, uint32_t u32Size)
{
    uint32_t u32CHKS;

    FMC_ENABLE_ISP();
    u32CHKS = FMC_GetChkSum(u32Address, u32Size);

    return u32CHKS;
}

uint32_t GetDMAMasterChecksum(uint32_t u32Address, uint32_t u32Size)
{
        /*Set input data address for CRC DMA Master*/
        CRC_SET_DMA_SADDR(CRC, (uint32_t)u32Address);
	
        CRC_SET_DMACNT_WORD(CRC, u32Size/4);
  
        CRC_ENABLE_DMA_INT(CRC);

        CRC_DMA_START(CRC);
	
        while(CRC->DMACTL & CRC_DMACTL_START_Msk){};        
					
        CRC_IRQHandler();

				return CRC->CHECKSUM;
}

/*---------------------------------------------------------------------------------------------------------*/
/*  MAIN function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    volatile uint32_t addr, size, u32RMCChecksum, u32CRC32Checksum, u32PDMAChecksum;

	/* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init Debug UART for printf */
    InitDebugUart();
	
    /* Lock protected registers */
    SYS_LockReg();

    size = 1024 * 2;

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    printf("+-----------------------------------------------------+\n");
    printf("|    CRC32 with PDMA Sample Code                      |\n");
    printf("|       - Get APROM first %d bytes CRC result by    |\n", size);
    printf("|          a.) FMC checksum command                   |\n");
    printf("|          b.) CPU write CRC data register directly   |\n");
    printf("|          c.) DMA Master write CRC data register           |\n");
    printf("+-----------------------------------------------------+\n\n");

    /* Unlock protected registers */
    SYS_UnlockReg();

    /*  Case a. */
    u32RMCChecksum = GetRMCChecksum(0x0, size);

    /* Lock protected registers */
    SYS_LockReg();

    /*  Case b. */
    /* Configure CRC controller for CRC-CRC32 mode */
    CRC_Open(CRC_32, (CRC_WDATA_RVS | CRC_CHECKSUM_RVS | CRC_CHECKSUM_COM), 0xFFFFFFFF, CRC_CPU_WDATA_32);
    /* Start to execute CRC-CRC32 operation */
    for(addr = 0; addr < size; addr += 4)
    {
        CRC_WRITE_DATA(inpw(addr));
    }
    u32CRC32Checksum = CRC_GetChecksum();

    /*  Case c. */
    /* Configure CRC controller for CRC-CRC32 mode */
    CRC_Open(CRC_32, (CRC_WDATA_RVS | CRC_CHECKSUM_RVS | CRC_CHECKSUM_COM), 0xFFFFFFFF, CRC_CPU_WDATA_32);
    u32PDMAChecksum = GetDMAMasterChecksum(0x0, size);

    printf("APROM first %d bytes checksum:\n", size);
    printf("   - by RMC command: 0x%x\n", u32RMCChecksum);
    printf("   - by CPU write:   0x%x\n", u32CRC32Checksum);
    printf("   - by DMA write:  0x%x\n", u32PDMAChecksum);

    if((u32RMCChecksum == u32CRC32Checksum) && (u32CRC32Checksum == u32PDMAChecksum))
    {
        if((u32RMCChecksum == 0) || (u32RMCChecksum == 0xFFFFFFFF))
        {
            printf("\n[Get checksum ... WRONG]");
        }
        else
        {
            printf("\n[Compare checksum ... PASS]");
        }
    }
    else
    {
        printf("\n[Compare checksum ... WRONG]");
    }

    /* Disable CRC function */
    CLK_DisableModuleClock(CRC0_MODULE);

    while(1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/