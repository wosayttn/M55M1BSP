/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief    Demonstrate how to implement NEC IR protocol by PSIO.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "NEC_IR_driver.h"

void SYS_Init(void);
void UART0_Init(void);

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Enable Internal LXT clock */
    CLK_EnableXtalRC(CLK_SRCCTL_LXTEN_Msk);

    /* Waiting for Internal LXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);

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

    /* Enable PSIO module clock */
    CLK_EnableModuleClock(PSIO0_MODULE);

    /* Select PSIO module clock source as LXT and PSIO module clock divider as 2 */
    CLK_SetModuleClock(PSIO0_MODULE, CLK_PSIOSEL_PSIO0SEL_LXT, CLK_PSIODIV_PSIO0DIV(2));

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /* Set PSIO multi-function pin CH0(PB.15) */
    SET_PSIO0_CH0_PB15();

}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    S_PSIO_NEC_CFG sConfig;
    uint32_t u32TimeOutCnt;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
    printf("+------------------------------------------------------+ \n");
    printf("|               NEC IR sample code                     | \n");
    printf("|      Please check waveform on PSIO_CH0(PB.15)        | \n");
    printf("+------------------------------------------------------+ \n");

    /* Use slot controller 0 and pin 0 */
    sConfig.u8SlotCtrl   = PSIO_SC0;
    sConfig.u8TxPin      = PSIO_PIN0;

    /* Initialize PSIO setting for NEC IR protocol */
    PSIO_NEC_Open(&sConfig);

    /* Send  0x1(Address), ~0x1(/Address), 0x2(Command), ~0x2(/Command) */
    PSIO_NEC_Send(&sConfig, 0x1, ~0x1, 0x2, ~0x2);

    /* Wait transfer done */
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

    while (PSIO_NEC_TransferDone(&sConfig))
    {
        if (--u32TimeOutCnt == 0)
        {
            printf("Wait for PSIO transfer done time-out!\n");
            goto lexit;
        }
    }

    /* Send  Repeat signal */
    PSIO_NEC_Repeat(&sConfig);

    /* Wait transfer done */
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

    while (PSIO_NEC_TransferDone(&sConfig))
    {
        if (--u32TimeOutCnt == 0)
        {
            printf("Wait for PSIO transfer done time-out!\n");
            goto lexit;
        }
    }

    /* Release PSIO setting */
    PSIO_NEC_Close(&sConfig);

    printf("Complete!\n");

lexit:

    while (1);
}