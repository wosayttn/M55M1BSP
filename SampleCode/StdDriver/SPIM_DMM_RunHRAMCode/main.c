/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    This example shows the subroutine when the application starts
 *           from HyperRAM.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NuMicro.h"
#include "UserImageBase.h"
#include "hyperram_code.h"

//------------------------------------------------------------------------------
#define SPIM_HRAM_PORT              SPIM0

//------------------------------------------------------------------------------
typedef void (FUNC_PTR)(void);

static FUNC_PTR *func;

//------------------------------------------------------------------------------
int SPIM_load_image_to_HyperRAM(SPIM_T *spim, uint32_t image_base, uint32_t image_limit)
{
    uint32_t u32Addr = 0, u32i, u32ImageSize = 0, *pu32Loader = NULL;
    uint8_t *pu8SrcBuf = (uint8_t *)malloc(sizeof(uint8_t) * BUFF_SIZE);
    uint8_t *pu8CmpBuf = (uint8_t *)malloc(sizeof(uint8_t) * BUFF_SIZE);

    u32ImageSize = (image_limit - image_base);
    pu32Loader = (uint32_t *)image_base;

    for (u32Addr = 0; u32Addr < u32ImageSize; u32Addr += BUFF_SIZE)
    {
        printf("\tErase ...\n");
        HyperRAM_Erase(spim, u32Addr, BUFF_SIZE);

        memset(pu8SrcBuf, 0, BUFF_SIZE);

        for (u32i = 0; u32i < BUFF_SIZE; u32i += 4, pu32Loader++)
        {
            outpw((uint32_t *)(&pu8SrcBuf[u32i]), *pu32Loader);

            if ((u32Addr + u32i) >= u32ImageSize)
                break;
        }

        printf("\tProgram ...");

        SPIM_HYPER_DMAWrite(spim, u32Addr, pu8SrcBuf, BUFF_SIZE);

        printf("\tVerify ...\r\n");

        memset(pu8CmpBuf, 0, BUFF_SIZE);

        SPIM_HYPER_DMARead(spim, u32Addr, pu8CmpBuf, BUFF_SIZE);

        if (memcmp(pu8SrcBuf, pu8CmpBuf, BUFF_SIZE))
        {
            printf("!!!\tData compare failed at block 0x%x\r\n", u32Addr);

            for (u32i = 0; u32i < 16; u32i++)
            {
                printf("Addr 0x%x - expect: 0x%02x, read: 0x%02x\r\n",
                       u32i,
                       pu8SrcBuf[u32i],
                       pu8CmpBuf[u32i]);
            }

            free(pu8SrcBuf);
            free(pu8CmpBuf);
            return -1;
        }
    }

    free(pu8SrcBuf);
    free(pu8CmpBuf);

    printf("OK.\n");

    return 0;
}

int HyperRAM_LoadCodeAndRun(SPIM_T *spim)
{
    uint32_t u32DMMAddr = SPIM_HYPER_GetDMMAddress(spim);

    if (&UserImageBase_start == &UserImageBase_finish)
    {
        printf("User image is not ready!!\r\n");

        while (1);
    }

    printf("Load user image to HyperRAM...\r\n");

    if (SPIM_load_image_to_HyperRAM(spim, (uint32_t)&UserImageBase, (uint32_t)&UserImageBase_finish) < 0)
    {
        return -1;
    }

    /* Enter direct-mapped mode to run new applications */
    SPIM_HYPER_EnterDirectMapMode(spim);

    func = (FUNC_PTR *)(u32DMMAddr + 1);
    func();

    return 0;
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

    /* Enable SPIM0 peripheral clock */
    CLK_EnableModuleClock(SPIM0_MODULE);

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /* Setup SPIM0 multi-function pins */
    SYS->GPC_MFP0 &= ~(SYS_GPC_MFP0_PC3MFP_Msk | SYS_GPC_MFP0_PC2MFP_Msk | SYS_GPC_MFP0_PC1MFP_Msk | SYS_GPC_MFP0_PC0MFP_Msk);
    SYS->GPC_MFP0 |= (SYS_GPC_MFP0_PC3MFP_SPIM0_SS | SYS_GPC_MFP0_PC2MFP_SPIM0_RESETN | SYS_GPC_MFP0_PC1MFP_SPIM0_RWDS | SYS_GPC_MFP0_PC0MFP_SPIM0_D2);
    SYS->GPC_MFP1 &= ~(SYS_GPC_MFP1_PC5MFP_Msk | SYS_GPC_MFP1_PC4MFP_Msk);
    SYS->GPC_MFP1 |= (SYS_GPC_MFP1_PC5MFP_SPIM0_CLKN | SYS_GPC_MFP1_PC4MFP_SPIM0_CLK);
    SYS->GPG_MFP2 &= ~(SYS_GPG_MFP2_PG11MFP_Msk | SYS_GPG_MFP2_PG10MFP_Msk | SYS_GPG_MFP2_PG9MFP_Msk);
    SYS->GPG_MFP2 |= (SYS_GPG_MFP2_PG11MFP_SPIM0_MOSI | SYS_GPG_MFP2_PG10MFP_SPIM0_D3 | SYS_GPG_MFP2_PG9MFP_SPIM0_D4);
    SYS->GPG_MFP3 &= ~(SYS_GPG_MFP3_PG15MFP_Msk | SYS_GPG_MFP3_PG14MFP_Msk | SYS_GPG_MFP3_PG13MFP_Msk | SYS_GPG_MFP3_PG12MFP_Msk);
    SYS->GPG_MFP3 |= (SYS_GPG_MFP3_PG15MFP_SPIM0_D7 | SYS_GPG_MFP3_PG14MFP_SPIM0_D6 | SYS_GPG_MFP3_PG13MFP_SPIM0_D5 | SYS_GPG_MFP3_PG12MFP_SPIM0_MISO);

    /* Enable SPIM clock pin schmitt trigger */
    PC->SMTEN |= (GPIO_SMTEN_SMTEN0_Msk |
                  GPIO_SMTEN_SMTEN1_Msk |
                  GPIO_SMTEN_SMTEN2_Msk |
                  GPIO_SMTEN_SMTEN3_Msk |
                  GPIO_SMTEN_SMTEN4_Msk |
                  GPIO_SMTEN_SMTEN5_Msk);
    PG->SMTEN |= (GPIO_SMTEN_SMTEN9_Msk  |
                  GPIO_SMTEN_SMTEN10_Msk |
                  GPIO_SMTEN_SMTEN11_Msk |
                  GPIO_SMTEN_SMTEN12_Msk |
                  GPIO_SMTEN_SMTEN13_Msk |
                  GPIO_SMTEN_SMTEN14_Msk |
                  GPIO_SMTEN_SMTEN15_Msk);

    /* Enable SPIM0 I/O high slew rate */
    GPIO_SetSlewCtl(PC, 0x3F, GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PG, 0x3F, GPIO_SLEWCTL_HIGH);

    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

    printf("+-------------------------------------------------------+\n");
    printf("|    M55M1 SPIM DMM mode running program on HyperRAM    |\n");
    printf("+-------------------------------------------------------+\n");

    HyperRAM_Init(SPIM_HRAM_PORT);

    HyperRAM_LoadCodeAndRun(SPIM_HRAM_PORT);

    while (1);
}

/*** (C) COPYRIGHT 2021 Nuvoton Technology Corp. ***/
