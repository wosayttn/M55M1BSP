/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief    Demonstrate how to implement 1-Wire protocol by PSIO.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "DS18B20_driver_thermometer.h"

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

    /* Enable PLL0 180MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_180MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to PLL0 and divide 1 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0);

    /* Set HCLK2 divide 2 */
    CLK_SET_HCLK2DIV(2);

    /* Set PCLKx divide 2 */
    CLK_SET_PCLK0DIV(2);
    CLK_SET_PCLK2DIV(2);
    CLK_SET_PCLK3DIV(2);
    CLK_SET_PCLK4DIV(2);

    /* Set PCLK1 to HCLK/8 */
    CLK_SET_PCLK1DIV(8);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and cyclesPerUs automatically. */
    SystemCoreClockUpdate();

    /* Enable PSIO module clock */
    CLK_EnableModuleClock(PSIO0_MODULE);

    /* Select PSIO module clock source as PCLK1 and PSIO module clock divider as 250 */
    CLK_SetModuleClock(PSIO0_MODULE, CLK_PSIOSEL_PSIO0SEL_PCLK1, CLK_PSIODIV_PSIO0DIV(250));

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
    S_PSIO_DS18B20_CFG sConfig;
    uint8_t au8Data[2] = {0};

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
    printf("|      DS18B20 ONE WIRE Thermometer  Test Code         | \n");
    printf("|      Please connected PSIO_CH0(PB.15) to device      | \n");
    printf("+------------------------------------------------------+ \n");

    /* Use slot controller 0 and pin 0 */
    sConfig.u8SlotCtrl   = PSIO_SC0;
    sConfig.u8DataPin    = PSIO_PIN0;

    /* Reset PSIO */
    SYS_ResetModule(SYS_PSIO0RST);

    /* Initialize PSIO setting for DS18B20 */
    PSIO_DS18B20_Open(&sConfig);

    do
    {
        /* Reset DS18B20 */
        PSIO_DS18B20_Reset(&sConfig);

        /* Write command to DS18B20 */
        PSIO_DS18B20_Write_Command(&sConfig, ONEWIRE_SKIP_ROM);
        PSIO_DS18B20_Write_Command(&sConfig, ONEWIRE_CONVT);
        /* Reset DS18B20 */
        PSIO_DS18B20_Reset(&sConfig);

        /* Write command to DS18B20 */
        PSIO_DS18B20_Write_Command(&sConfig, ONEWIRE_SKIP_ROM);
        PSIO_DS18B20_Write_Command(&sConfig, ONEWIRE_RDSCRATCH_PAD);

        /* Read data from DS18B20 */
        PSIO_DS18B20_Read_Data(&sConfig, &au8Data[0]);

        printf("Temperature= %f degrees C\n", (au8Data[0] | ((au8Data[1] & 0x07) << 8)) * 0.0625);

        /* Initialize data */
        au8Data[0] = 0;
        au8Data[1] = 0;

        /* Delay 50000 us */
        CLK_SysTickDelay(50000);

    } while (1);
}