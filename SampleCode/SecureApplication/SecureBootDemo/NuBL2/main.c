/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   Demonstrate how to generate the first booting image, NuBL2.
 *          After NuBL2 runs, NuBL2 will authenticate NuBL32 and NuBL33 then jump to execute in NuBL32.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "NuBL2.h"

void SYS_Init(void);
void UART_Init(void);

static int32_t CheckROTPKStatus(void)
{
    /* Configure module clock */
    CLK_EnableModuleClock(KS0_MODULE);

    KS_Open();

    if ((KS->OTPSTS & 0x3) == 0)
    {
        printf("ROTPK absent! NuBL2 execution via direct boot.\n\n");

        // TODO: User must set ROTPK in KS OTP0 and OTP1
        //       and then the Secure Bootloader (NuBL1) can perform trusted boot to execute NuBL2.

        /*
            The NuBL2 ROTPK is a ECC P-256 public key pair.
            ECC P-256 private key file of this sample is FwKey/ECC-P256.pem.
            The public key pair can be generated with command - "imgtool.exe getpub -k ECC-P256.pem"
            const unsigned char ecdsa_pub_key[] = {
                0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86,
                0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a,
                0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03,
                0x42, 0x00, 0x04, 0xd2, 0xc3, 0xe0, 0x3e, 0x54,
                0xc9, 0xcd, 0x32, 0xaa, 0x84, 0x70, 0xb6, 0x7e,
                0xe3, 0xaa, 0x81, 0x39, 0x2c, 0x2f, 0xa6, 0x71,
                0xea, 0x8e, 0xc3, 0x00, 0xa1, 0xd1, 0x9b, 0x1e,
                0xad, 0xbe, 0x65, 0x0e, 0x32, 0xb3, 0x3d, 0x83,
                0x28, 0x88, 0x09, 0x6a, 0xbf, 0x71, 0x0c, 0x08,
                0x2e, 0x46, 0xa2, 0x13, 0x16, 0x6f, 0x55, 0x76,
                0x49, 0xc7, 0x85, 0x9b, 0x0b, 0x60, 0x7a, 0x8d,
                0x34, 0x71, 0xc2,
            };

            Public key 1 starts from ecdsa_pub_key[27] and public key 2 starts from ecdsa_pub_key[59]
              Public key 1 = D2C3E03E 54C9CD32 AA8470B6 7EE3AA81 392C2FA6 71EA8EC3 00A1D19B 1EADBE65
              Public key 2 = 0E32B33D 83288809 6ABF710C 082E46A2 13166F55 7649C785 9B0B607A 8D3471C2

            Notes of programming NuBL2 ECC ROTPK:
            * The ROTPK arrary for KS_WriteOTP API
              !!! Please take care inversed order of public key in g_au32ROTPK !!!
                const uint32_t g_au32ROTPK[16] =
                {
                    // Public key 1
                    0x1EADBE65, 0x00A1D19B, 0x71EA8EC3, 0x392C2FA6,
                    0x7EE3AA81, 0xAA8470B6, 0x54C9CD32, 0xD2C3E03E,
                    // Public key 2
                    0x8D3471C2, 0x9B0B607A, 0x7649C785, 0x13166F55,
                    0x082E46A2, 0x6ABF710C, 0x83288809, 0x0E32B33D,
                };

            * An example to program KS OPT0 and OPT1
                // Enable module clock
                CLK_EnableModuleClock(KS_MODULE);
                KS_Open();
                KS_WriteOTP(0, (KS_META_256 | KS_META_ECC | KS_META_BOOT), (uint32_t *)&g_au32ROTPK[0]);
                KS_WriteOTP(1, (KS_META_256 | KS_META_ECC | KS_META_BOOT), (uint32_t *)&g_au32ROTPK[8]);
        */
    }
    else
    {
        printf("ROTPK present. NuBL2 execution via secure boot.\n\n");
    }

    return 0;
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

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /* Lock protected registers */
    SYS_LockReg();
}

/*---------------------------------------------------------------------------------------------------------*/
/*  MAIN function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    uint32_t u32TimeOutCnt, u32NuBL32Base, u32NuBL33Base;

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART for print message */
    InitDebugUart();

    printf("\n\nCPU @ %d Hz (Non-secure Flash Base Address: 0x%x)\n", SystemCoreClock, SCU->FNSADDR);
    printf("+------------------------------------------+\n");
    printf("|    SecureBootDemo - NuBL2 Sample Code    |\n");
    printf("+------------------------------------------+\n\n");

    CheckROTPKStatus();

    /* Authenticate NuBL32 FW */
    if (VerifyNuBL3x(NUBL32_FW_IMG_BASE, NUBL32_FW_IMG_SIZE, &u32NuBL32Base) != 0)
    {
        printf("\n\nNuBL2 verified NuBL32 FAIL !\n");
        goto ErrorExit;
    }

    printf("\nNuBL2 verified NuBL32 FW PASS.\n");

    /* Authenticate NuBL33 FW */
    if (VerifyNuBL3x(NUBL33_FW_IMG_BASE, NUBL33_FW_IMG_SIZE, &u32NuBL33Base) != 0)
    {
        printf("\n\nNuBL2 verified NuBL33 FAIL !\n");
        goto ErrorExit;
    }

    printf("\nNuBL2 verified NuBL33 FW PASS.\n");

    /* Jump to execute NuBL32 FW */
    printf("\nJump to execute NuBL32...\n");
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
    UART_WAIT_TX_EMPTY((UART_T *)DEBUG_PORT)

    if (--u32TimeOutCnt == 0) break;

    /* Disable all interrupt */
    __set_PRIMASK(1);

    /* Unlock protected registers */
    SYS_UnlockReg();
    FMC_Open();
    FMC_SetVectorPageAddr(u32NuBL32Base);

    /* Reset to execute NuBL32 FW */
    NVIC_SystemReset();

ErrorExit:

    while (1) {}
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/