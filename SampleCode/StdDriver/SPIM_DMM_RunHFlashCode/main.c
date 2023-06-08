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

/**
 * @brief Training DLL component delay stop number
 *
 * @param spim
 */
void SPIM_TrainingDLLDelayTime(SPIM_T *spim)
{
    volatile uint32_t u32i = 0;
    uint32_t u32j = 0;
    volatile uint8_t u8RdDelay = 0;
    uint8_t u8Temp = 0;
    uint8_t u8RdDelayIdx = 0;
    uint8_t u8RdDelayRes[SPIM_MAX_DLL_LATENCY] = {0};
    uint32_t u32SrcAddr = 0;
    uint32_t u32TestSize = 32;

    /* Erase HyperFlash */
    HyperFlash_EraseSector(spim, 0); //one sector = 256KB

    HyperFlash_DMAWrite(spim, u32SrcAddr, g_au8SrcArray, u32TestSize);

    for (u8RdDelay = 0; u8RdDelay <= SPIM_MAX_DLL_LATENCY; u8RdDelay++)
    {
        /* Set DLL calibration to select the valid delay step number */
        SPIM_CtrlDLLDelayTime(spim, 0, 0, 0, 0, u8RdDelay);

        /* Read Data from HyperFlash */
        HyperFlash_DMARead(spim, u32SrcAddr, g_au8DestArray, u32TestSize);

        /* Verify the data and save the number of successful delay steps */
        if (memcmp(g_au8SrcArray, g_au8DestArray, u32TestSize))
        {
            printf("!!!\tData compare failed at block 0x%x\n", u32SrcAddr);
        }
        else
        {
            printf("RX Delay: %d = Pass\r\n", u8RdDelay);
            u8RdDelayRes[u8RdDelayIdx++] = u8RdDelay;
        }
    }

    /* Sort delay step number */
    for (u32i = 0 ; u32i <= u8RdDelayIdx ; u32i = u32i + 1)
    {
        for (u32j = u32i + 1 ; u32j < u8RdDelayIdx ; u32j = u32j + 1)
        {
            if (u8RdDelayRes[u32i] > u8RdDelayRes[u32j])
            {
                u8Temp = u8RdDelayRes[u32i];
                u8RdDelayRes[u32i] = u8RdDelayRes[u32j];
                u8RdDelayRes[u32j] = u8Temp;
            }
        }
    }

    if (u8RdDelayIdx > 2)
    {
        u8RdDelayIdx = (u8RdDelayIdx / 2) - 1;
    }
    else
    {
        u8RdDelayIdx = 0;
    }

    /* Set the number of intermediate delay steps */
    SPIM_CtrlDLLDelayTime(spim, 0, 0, 0, 0, u8RdDelayRes[u8RdDelayIdx]);
}

/**
  * @brief      SPIM Default Config HyperBus Access Module Parameters.
  * @param      spim
  * @param      u32CSMaxLT Chip Select Maximum Low Time 0 ~ 0xFFFF, Default Set 0x02ED
  * @param      u32AcctRD Initial Read Access Time 1 ~ 0x1F, Default Set 0x04
  * @param      u32AcctWR Initial Write Access Time 1 ~ 0x1F, Default Set 0x04
  * @return     None.
  */
void SPIM_Hyper_DefaultConfig(SPIM_T *spim, uint32_t u32CSMaxLow, uint32_t u32AcctRD, uint32_t u32AcctWR)
{
    /* Chip Select Setup Time 2.5 */
    SPIM_HYPER_CONFIG1_SET_CSST(spim, SPIM_HYPER_CONFIG1_CSST_2_5_HCLK);

    /* Chip Select Hold Time 3.5 HCLK */
    SPIM_HYPER_CONFIG1_SET_CSH(spim, SPIM_HYPER_CONFIG1_CSH_3_5_HCLK);

    /* Chip Select High between Transaction as 2 HCLK cycles */
    SPIM_HYPER_CONFIG1_SET_CSHI(spim, 2);

    /* Chip Select Masximum low time HCLK */
    SPIM_HYPER_CONFIG1_SET_CSMAXLT(spim, u32CSMaxLow);

    /* Initial Device RESETN Low Time 255 */
    SPIM_HYPER_CONFIG2_SET_RSTNLT(spim, 0xFF);

    /* Initial Read Access Time Clock cycle*/
    SPIM_HYPER_CONFIG2_SET_ACCTRD(spim, u32AcctRD);

    /* Initial Write Access Time Clock cycle*/
    SPIM_HYPER_CONFIG2_SET_ACCTWR(spim, u32AcctWR);
}

static uint32_t HyperFlash_GetLatencyNum(uint32_t u32Latency)
{
    if (u32Latency < 5)
    {
        u32Latency = 5;
    }
    else if (u32Latency > 16)
    {
        u32Latency = 16;
    }

    return (u32Latency - 5);
}

void HyperFlash_SetReadLatency(SPIM_T *spim, uint32_t u32Latency)
{
    uint32_t u32NVCRData = 0;

    SPIM_Hyper_DefaultConfig(spim, HFLH_MAX_CS_LOW, 16, HFLH_WR_ACCTIME);

    SPIM_CtrlDLLDelayTime(spim, 0, 0, 0, 0, 1);

    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, READ_NVCR_REG);

    u32NVCRData = SPIM_Hyper_Read1Word(spim, CMD_NOOP_CODE);
    printf("NVCR Reg = %x\r\n", u32NVCRData);

    u32NVCRData &= ~(0xF << 4);
    u32NVCRData |= (HyperFlash_GetLatencyNum(u32Latency) << 4); //Latency

    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, CMD_COMMON_AA);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_2AA, CMD_COMMON_55);
    HyperFlash_WriteOPCMD(spim, CMD_COMMON_555, WRITE_NVCR_REG);
    SPIM_Hyper_Write2Byte(spim, CMD_NOOP_CODE, u32NVCRData);

    SPIM_Hyper_DefaultConfig(spim, HFLH_MAX_CS_LOW, u32Latency, HFLH_WR_ACCTIME);

    HyperFlash_WaitBusBusy(spim);

    u32NVCRData = HyperFlash_ReadData4CmdSets(spim, READ_NVCR_REG, CMD_NOOP_CODE);
    printf("NVCR Verify NVCR Reg = %x\r\n", u32NVCRData);

    //u32NVCRData = HyperFlash_ReadData4CmdSets(spim, READ_VCR_REG, CMD_NOOP_CODE);
    //printf("VCR Reg = %x\r\n", u32NVCRData);
}

void SPIM_HyperFlash_Init(SPIM_T *spim)
{
    /* Enable SPIM Hyper Bus Mode */
    SPIM_SET_HYPER_MODE(spim, 1);

    /* Set SPIM clock as HCLK divided by 1 or 2 */
    SPIM_SET_CLOCK_DIVIDER(spim, 2);

#if (SPIM_CACHE_EN == 1)
    /* Enable SPIM Cache */
    SPIM_DISABLE_CACHE(spim);
#endif //SPIM_CACHE_EN

    /* SPIM Def. Enable Cipher, First Disable the test. */
    SPIM_DISABLE_CIPHER(spim);

    SPIM_Hyper_Reset(spim);

    HyperFlash_SetReadLatency(spim, 9);

    SPIM_TrainingDLLDelayTime(spim);
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

    SPIM_HyperFlash_Init(SPIM_HFLASH_PORT);

    SPIM_Hyper_EnterDirectMapMode(SPIM_HFLASH_PORT);

    HyperFlash_LoadCodeAndRun(SPIM_HFLASH_PORT);

    while (1);
}

/*** (C) COPYRIGHT 2021 Nuvoton Technology Corp. ***/
