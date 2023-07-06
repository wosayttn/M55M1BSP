/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Show the usage of TAMPER static tamper interrupt to wake up system
 *           or clear keys in SRAM of Key Store.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Functions and variables declaration                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
static volatile uint8_t s_u8Option;

int32_t KS_TRIG(void);
void SYS_Init(void);
void UART_Init(void);
int32_t KS_Init(void);
int32_t KeyStoreSRAM(void);
/*---------------------------------------------------------------------------------------------------------*/
/* Check Debug UART TX Buffer is or not empty                                                              */
/*---------------------------------------------------------------------------------------------------------*/
int IsDebugFifoEmpty(void)
{
    return ((DEBUG_PORT->FIFOSTS & UART_FIFOSTS_TXEMPTYF_Msk) != 0U);
}
/*---------------------------------------------------------------------------------------------------------*/
/* Check Key status                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t KS_TRIG(void)
{
    uint32_t u32TimeOutCnt;
    uint32_t au32Key[8];

    u32TimeOutCnt = KS_TIMEOUT;

    /* Wait BUSY(KS_STS[7]) to 0 */
    while (KS_GET_STS(KS_STS_INITDONE_Msk))
    {
        if (--u32TimeOutCnt == 0)
        {
            printf("Wait for Key Store INIT flag time-out!\n");
            return -1;
        }
    }

    if (KS_Open() != KS_OK)
    {
        printf("Init Key Store fail !\n");
        return -1;
    }

    if (KS_Read(KS_SRAM, 0, au32Key, 8) == KS_OK)
    {
        printf("Read SRAM key from Key Store should not ok !\n");
        return -1;
    }

    if (KS_GetRemainSize(KS_SRAM) != KS_MAX_SRAM_SPACE)
    {
        printf(" Fail! Remaining 0x%X space not clear for SRAM.\n", KS_GetRemainSize(KS_SRAM));
    }

    return 0;
}
/**
 * @brief       IRQ Handler for TAMPER Interrupt
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The TAMPER_IRQHandler is default IRQ of EQEI0, declared in startup_M55M1.c.
 */
void TAMPER_IRQHandler(void)
{
    /* Tamper interrupt occurred */
    if (TAMPER_GET_INT_FLAG())
    {
        uint32_t i;
        uint32_t u32FlagStatus;
        uint32_t u32TimeOutCnt;

        /* Get tamper interrupt status */
        u32FlagStatus = TAMPER_GET_INT_STATUS();

        for (i = 0; i < 6; i++)
        {
            if (u32FlagStatus & (0x1UL << (i + TAMPER_INTSTS_TAMP0IF_Pos)))
                printf(" Tamper %u Detected!!\n", i);
        }

        if (s_u8Option == '0')
        {
            /* Clear tamper interrupt status */
            TAMPER_CLR_INT_STATUS(u32FlagStatus);

            printf(" System wake-up!\n");
        }
        else if (s_u8Option == '1')
        {
            /* Clear tamper interrupt status */
            TAMPER_CLR_INT_STATUS(u32FlagStatus);

            printf(" Check keys are all zero ...");

            if (KS_TRIG() == 0)
                printf(" Pass!\n");
        }

        /* To check if all the debug messages are finished */
        u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

        while (IsDebugFifoEmpty() == 0)
            if (--u32TimeOutCnt == 0) break;

        SYS_ResetChip();
    }
}
/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void)
{
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);
    /* Enable Internal RC 32KHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    /* Waiting for Low speed Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);

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
    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK_EnableModuleClock(GPIOF_MODULE);
    CLK_EnableModuleClock(TAMPER0_MODULE);
    CLK_EnableModuleClock(KS0_MODULE);
    CLK_EnableModuleClock(FMC0_MODULE);
    CLK_EnableModuleClock(ISP0_MODULE);

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /* Set PF multi-function pins for GPIO*/
    SET_TAMPER2_PF8();
    SET_TAMPER3_PF9();

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
/* KEY Stroe SRAM                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t KeyStoreSRAM(void)
{
    uint32_t u32TimeOutCnt;
    uint32_t au32KSKeyArary[8];

    /* Write key to KS_KEY[0]~[7] registers */
    au32KSKeyArary[0] = 0x12345678;
    au32KSKeyArary[1] = 0x12345678;
    au32KSKeyArary[2] = 0x12345678;
    au32KSKeyArary[3] = 0x12345678;
    au32KSKeyArary[4] = 0x12345678;
    au32KSKeyArary[5] = 0x12345678;
    au32KSKeyArary[6] = 0x12345678;
    au32KSKeyArary[7] = 0x12345678;

    if (KS_Write(KS_SRAM, (KS_META_READABLE | KS_META_CPU | KS_META_256), au32KSKeyArary) != KS_OK)
    {
        return -1;
    }

    return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
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
    printf("+--------------------------------------+\n");
    printf("|     Tamper Detection Sample Code     |\n");
    printf("+--------------------------------------+\n\n");

    /* Reset tamper coreblock */
    TAMPER_CORE_RESET();
    TAMPER_CORE_RELEASE();

    printf("# Please connect TAMPER2/3(PF.8/9) pins to High first.\n");
    printf("\nSelect:\n");
    printf("    [0] Wake-up test\n");
    printf("    [1] Clear keys in SRAM of Key Store test\n");

    s_u8Option = (uint8_t)getchar();
    printf("\n");
    printf("Select item [%c]\n", s_u8Option);

    TAMPER_CLR_INT_STATUS(TAMPER_INTSTS_TAMP2IF_Msk | TAMPER_INTSTS_TAMP3IF_Msk);

    TAMPER_IOSEL_TAMPER(TAMPER_TAMPER2_SELECT | TAMPER_TAMPER3_SELECT);

    TAMPER_StaticTamperEnable(TAMPER_TAMPER2_SELECT | TAMPER_TAMPER3_SELECT, TAMPER_TAMPER_HIGH_LEVEL_DETECT,
                              TAMPER_TAMPER_DEBOUNCE_ENABLE);

    /* Enable tamper I/O interrupt */
    TAMPER_EnableInt(TAMPER_INTEN_TAMP2IEN_Msk | TAMPER_INTEN_TAMP3IEN_Msk);
    NVIC_EnableIRQ(TAMPER_IRQn);

    if (s_u8Option == '0')
    {
        printf("# Please connect TAMPER2/3(PF.8/9) pins to Low to generate TAMPER event.\n");
        printf("# Press any key to start test:\n\n");
        getchar();

        /* Enable wake-up function */
        TAMPER_ENABLE_WAKEUP();

        /* System enter to Power-down */
        /* To program PWRCTL register, it needs to disable register protection first. */
        SYS_UnlockReg();
        PMC_SetPowerDownMode(PMC_NPD0, PMC_PLCTL_PLSEL_PL1);
        printf("# System enter to power-down mode ...\n");
        /* To check if all the debug messages are finished */
        uint32_t u32TimeOutCnt;
        u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

        while (IsDebugFifoEmpty() == 0)
            if (--u32TimeOutCnt == 0) break;

        PMC_PowerDown();
    }
    else if (s_u8Option == '1')
    {
        /* Enable to trigger Key Store */
        TAMPER_ENABLE_KS_TRIG();

        /* Unlock protected registers */
        SYS_UnlockReg();

        /* Init Key Store */
        if (KS_Open() != KS_OK)
        {
            printf("Init Key Store fail !\n");

            while (1);
        }

        printf("# Write keys ...");

        if (KeyStoreSRAM() != KS_OK)
        {
            printf("Creta key fail !\n");

            while (1);
        }

        printf(" Done!\n\n");

        printf("# Please connect TAMPER2/3(PF.8/9) pins to Low to generate TAMPER event.\n");
        printf("# Press any key to start test:\n\n");
        getchar();

        printf("# Check keys in SRAM of Key Store when tamper event occurred:\n");

        /* Wait for tamper event interrupt happened */
        while (1);
    }
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
