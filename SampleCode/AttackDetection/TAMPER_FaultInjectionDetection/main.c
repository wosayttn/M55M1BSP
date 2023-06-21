/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Show the usage of TAMPER voltage glitch positive and negative detection function.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Functions and variables declaration                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
static uint32_t au32MyAESKey[8] =
{
    0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f,
    0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f
};

static uint32_t au32MyAESIV[4] =
{
    0x00000000, 0x00000000, 0x00000000, 0x00000000
};

#ifdef __ICCARM__
#pragma data_alignment=4
uint8_t au8InputData[] =
{
#else
__attribute__((aligned(4))) static uint8_t au8InputData[] =
{
#endif
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
};

#ifdef __ICCARM__
    #pragma data_alignment=4
    uint8_t au8OutputData[1024];
#else
    __attribute__((aligned(4))) static uint8_t au8OutputData[1024];
#endif

static volatile int32_t  s_AES_done;

void DumpBuffHex(uint8_t *pucBuff, int nBytes);
void SYS_Init(void);
void UART_Init(void);
/**
 * @brief       IRQ Handler for TAMPER Interrupt
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The TAMPER_IRQHandler is default IRQ of TAMPER, declared in startup_M55M1.c.
 */
void TAMPER_IRQHandler(void)
{
    uint32_t u32FlagStatus;
    uint32_t u32AESIndex;
    uint32_t u32AESClearStatus = 0;

    /* Tamper interrupt occurred */
    if (TAMPER_GET_INT_FLAG())
    {
        /* Get tamper interrupt status */
        u32FlagStatus = TAMPER_GET_INT_STATUS();

        if (u32FlagStatus & TAMPER_INTSTS_VGPEVIF_Msk)
        {
            /* Clear tamper interrupt status */
            TAMPER_CLR_INT_STATUS(u32FlagStatus);

            printf("LDO_CAP positive glitch is detected!\n\n");
        }

        if (u32FlagStatus & TAMPER_INTSTS_VGNEVIF_Msk)
        {
            /* Clear tamper interrupt status */
            TAMPER_CLR_INT_STATUS(u32FlagStatus);

            printf("LDO_CAP negative glitch is detected!\n\n");
        }

        /* Check Crypto AES KEY status */
        for (u32AESIndex = 0 ; u32AESIndex < 8 ; u32AESIndex++)
        {
            if (CRYPTO->AES_KEY[u32AESIndex] != 0)
            {
                u32AESClearStatus ++;
            }
        }

        if (u32AESClearStatus == 0)
        {
            printf("Crypto settign has been cleared!\n\n");
        }
        else
        {
            printf("Crypto settign clear fail!\n\n");
        }
    }
}
/**
 * @brief       IRQ Handler for CRYPTO Interrupt
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The CRYPTO_IRQHandler is default IRQ of CRYPTO, declared in startup_M55M1.c.
 */
void CRYPTO_IRQHandler(void)
{
    if (AES_GET_INT_FLAG(CRYPTO))
    {
        s_AES_done = 1;
        AES_CLR_INT_FLAG(CRYPTO);
    }
}

void DumpBuffHex(uint8_t *pucBuff, int nBytes)
{
    int32_t i32Idx, i;

    i32Idx = 0;

    while (nBytes > 0)
    {
        printf("0x%04X  ", i32Idx);

        for (i = 0; i < 16; i++)
            printf("%02x ", pucBuff[i32Idx + i]);

        printf("  ");

        for (i = 0; i < 16; i++)
        {
            if ((pucBuff[i32Idx + i] >= 0x20) && (pucBuff[i32Idx + i] < 127))
                printf("%c", pucBuff[i32Idx + i]);
            else
                printf(".");

            nBytes--;
        }

        i32Idx += 16;
        printf("\n");
    }

    printf("\n");
}
/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void)
{
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
    /* set power level to level 1 */
    PMC_SetPowerLevel(PMC_PLCTL_PLSEL_PL1);

    /* Enable module clock */
    CLK_EnableModuleClock(TAMPER0_MODULE);
    CLK_EnableModuleClock(CRYPTO0_MODULE);
    CLK_EnableModuleClock(FMC0_MODULE);
    CLK_EnableModuleClock(ISP0_MODULE);

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

}
/*---------------------------------------------------------------------------------------------------------*/
/* Init UART                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void UART_Init(void)
{
    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    uint32_t u32TimeOutCnt;
    uint32_t u32Confi3Read;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART for printf */
    UART_Init();

    /* Enable FMC ISP function */
    FMC_Open();

    /* Enable APROM update function */
    FMC_ENABLE_AP_UPDATE();

    /* Enable User Configuration update function */
    FMC_ENABLE_CFG_UPDATE();

    /* Enable Tamper Domain */
    u32Confi3Read = FMC_Read(FMC_USER_CONFIG_3);

    printf("CONFI3=0x%8X\n", u32Confi3Read);

    if (u32Confi3Read == 0x5aa5ffff)
    {
        FMC_ENABLE_ISP();
        FMC_ENABLE_CFG_UPDATE();
        FMC_WriteConfig(FMC_USER_CONFIG_3, 0x5a5affff);
        SYS_ResetChip();

        while (1);
    }

    /* Lock protected registers */
    SYS_LockReg();

    printf("\n\nCPU @ %uHz\n", SystemCoreClock);
    printf("+--------------------------------------------------+\n");
    printf("|   TAMPER Fault Injection Detection Sample Code   |\n");
    printf("+--------------------------------------------------+\n\n");
    printf("The encryption result will be zero if any voltage fault injection is detected before AES encryption is completed.\n\n");

    /* Reset tamper coreblock */
    TAMPER_CORE_RESET();
    TAMPER_CORE_RELEASE();

    /* Enable voltage glitch detection clock source and select sampling rate */
    TAMPER_ENABLE_HIRC48M();
    TAMPER_VG_SAMPLE_SEL(TAMPER_VG_192M_SAMPLE);

    /* Initialize a reference trim value according to the power level of the system */
    TAMPER_VG_TRIM_INIT();

    /* Enable voltage glitch positive/negative detection interrupt */
    TAMPER_EnableInt(TAMPER_INTEN_VGPIEN_Msk | TAMPER_INTEN_VGNIEN_Msk);

    /* Clear voltage glitch positive/negative interrupt flag */
    TAMPER_CLR_INT_STATUS(TAMPER_INTSTS_VGPEVIF_Msk | TAMPER_INTSTS_VGNEVIF_Msk);

    NVIC_EnableIRQ(TAMPER_IRQn);

    /* Enable to clear crypto function */
    TAMPER_ENABLE_CRYPTO();

    NVIC_EnableIRQ(CRYPTO_IRQn);
    AES_ENABLE_INT(CRYPTO);

    /*---------------------------------------
     *  AES-128 ECB mode encrypt
     *---------------------------------------*/
    AES_Open(CRYPTO, 0, 1, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    AES_SetKey(CRYPTO, 0, au32MyAESKey, AES_KEY_SIZE_128);
    AES_SetInitVect(CRYPTO, 0, au32MyAESIV);
    AES_SetDMATransfer(CRYPTO, 0, (uint32_t)au8InputData, (uint32_t)au8OutputData, sizeof(au8InputData));

    s_AES_done = 0;
    /* Start AES Encrypt */
    AES_Start(CRYPTO, 0, CRYPTO_DMA_ONE_SHOT);
    /* Waiting for AES calculation */
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

    while (!s_AES_done)
    {
        if (--u32TimeOutCnt == 0)
        {
            printf("Wait for AES encrypt done time-out!\n");
            return -1;
        }
    }

    printf("AES encrypt done.\n\n");
    DumpBuffHex(au8OutputData, sizeof(au8InputData));

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/