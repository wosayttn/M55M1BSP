/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    This example shows the subroutine when the application starts
 *           from HyperFlash.
 *
 * @copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NuMicro.h"
#include "UserImageBase.h"
#include "hyperflash_code.h"

//------------------------------------------------------------------------------
#define SPIM_HFLASH_PORT            SPIM0

#define BUFF_SIZE                   0x200

//------------------------------------------------------------------------------
#if defined (__ARMCC_VERSION)
__attribute__((aligned(32))) uint8_t g_au8SrcArray[BUFF_SIZE] = {0};
__attribute__((aligned(32))) uint8_t g_au8DestArray[BUFF_SIZE] = {0};
#else
__align(32) uint8_t g_au8SrcArray[BUFF_SIZE] = {0};
__align(32) uint8_t g_au8DestArray[BUFF_SIZE] = {0};
#endif //__ARMCC_VERSION

typedef void (FUNC_PTR)(void);

static FUNC_PTR *func;

//------------------------------------------------------------------------------
static void __set_SP(uint32_t _sp)
{
    __asm__ __volatile__("MSR MSP, r0");
    __asm__ __volatile__("BX lr");
}

int SPIM_load_image_to_HyperFlash(SPIM_T *spim, uint32_t image_base, uint32_t image_limit)
{
    volatile uint32_t u32Addr = 0, u32i = 0, u32ImageSize = 0, *pu32Loader = NULL;
    volatile uint32_t u32EraseCount = 0;

    u32ImageSize = (image_limit - image_base);
    pu32Loader = (uint32_t *)image_base;

    printf("\tErase Secotr...\n");

    /* Erase HyperFlash, 1 sector = 256KB */
    HyperFlash_EraseSector(spim, 0);

    for (u32Addr = 0; u32Addr < u32ImageSize; u32Addr += BUFF_SIZE)
    {
        memset(g_au8SrcArray, 0, BUFF_SIZE);

        for (u32i = 0; u32i < BUFF_SIZE; u32i += 4, pu32Loader++)
        {
            outpw((uint32_t *)(&g_au8SrcArray[u32i]), *pu32Loader);

            if ((u32Addr + u32i) >= u32ImageSize)
                break;
        }

        printf("\tProgram...");

        HyperFlash_DMAWrite(spim, u32Addr, g_au8SrcArray, BUFF_SIZE);

        printf("\tVerify...\r\n");

        memset(g_au8DestArray, 0, BUFF_SIZE);

        HyperFlash_DMARead(spim, u32Addr, g_au8DestArray, BUFF_SIZE);

        if (memcmp(g_au8SrcArray, g_au8DestArray, BUFF_SIZE))
        {
            printf("!!!\tData compare failed at block 0x%x\r\n", u32Addr);

            for (u32i = 0; u32i < 16; u32i++)
            {
                printf("Addr 0x%x - expect: 0x%02x, read: 0x%02x\r\n",
                       u32i,
                       g_au8SrcArray[u32i],
                       g_au8DestArray[u32i]);
            }

            return -1;
        }
    }

    printf("OK.\n");

    return 0;
}

int HyperFlash_LoadCodeAndRun(SPIM_T *spim)
{
    uint32_t u32DMMAddr = SPIM_GetDirectMapAddress(spim);

    if (&UserImageBase_start == &UserImageBase_finish)
    {
        printf("User image is not ready!!\r\n");

        while (1);
    }

    printf("Load user image to HyperFlash...\r\n");

    if (SPIM_load_image_to_HyperFlash(spim, (uint32_t)&UserImageBase, (uint32_t)&UserImageBase_finish) < 0)
    {
        return -1;
    }

    /* Enter direct-mapped mode to run new applications */
    SPIM_Hyper_EnterDirectMapMode(spim);

    func = (void *)(u32DMMAddr + 1);
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
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0, CLK_ACLKDIV_ACLKDIV(1));

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

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /* Enable SPIM0 peripheral clock */
    CLK_EnableModuleClock(SPIM0_MODULE);

    /* Set PB multi-function pins for UART0 RXD=PB.12 and TXD=PB.13 */
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

    /* Init UART to 115200-8n1 for print message */
    InitDebugUart();

    printf("+-------------------------------------------------------+\n");
    printf("|    M55M1 SPIM DMM mode running program on HyperFlash  |\n");
    printf("+-------------------------------------------------------+\n");

    HyperFlash_Init(SPIM_HFLASH_PORT);

    HyperFlash_LoadCodeAndRun(SPIM_HFLASH_PORT);

    while (1);
}

/*** (C) COPYRIGHT 2021 Nuvoton Technology Corp. ***/
