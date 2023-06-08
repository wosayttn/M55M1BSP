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
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int IsDebugFifoEmpty(void)
{
    return ((DEBUG_PORT->FIFOSTS & UART_FIFOSTS_TXEMPTYF_Msk) != 0U);
}

static volatile uint8_t s_u8Option;

int32_t KS_TRIG(void);
void TAMPER_IRQHandler(void);
void SYS_Init(void);
void UART_Init(void);
int32_t KS_Init(void);
int32_t KeyStoreSRAM(void);

int32_t KS_TRIG(void)
{
    uint32_t u32TimeOutCnt;
     
    u32TimeOutCnt = KS_TIMEOUT;
    /* Wait BUSY(KS_STS[7]) to 0 */
    u32TimeOutCnt = KS_TIMEOUT;
    while(KS->STS & KS_STS_INITDONE_Msk)
    {
        if(--u32TimeOutCnt == 0)
        {
            printf("Wait for Key Store INIT flag time-out!\n");
            return -1;
        }
    }
    /* Claer EIF and IF STS */
    KS->STS = (KS_STS_EIF_Msk | KS_STS_IF_Msk);
    /* Check BUSY(KS_STS[2]) is 0 and EIF(KS_STS[1]) is 0 */
    if(KS->STS & (KS_STS_BUSY_Msk | KS_STS_EIF_Msk))
    {
        printf("Key store status is wrong!\n");
    }

    /* Set OPMODE(KS_CTL[3:1]) to 000 */
    KS->CTL = (KS->CTL & (~KS_CTL_OPMODE_Msk)) | (0 << KS_CTL_OPMODE_Pos);

    /* Set START(KS_CTL[0]) to 1 */
    KS->CTL |= (1 << KS_CTL_START_Pos);

    /* Wait BUSY(KS_STS[2]) to 0 */
    u32TimeOutCnt = KS_TIMEOUT;
    while(KS->STS & KS_STS_BUSY_Msk)
    {
        if(--u32TimeOutCnt == 0)
        {
            printf("Wait for Key Store busy flag time-out!\n");
            return -1;
        }
    }

    /* Read key from KS_KEY[0]~[7] registers if EIF(KS_STS[1]) is 1 */
    u32TimeOutCnt = KS_TIMEOUT;
    while(!(KS->STS & KS_STS_EIF_Msk))
    {
        if(--u32TimeOutCnt == 0)
        {
            printf("Wait for Key Store time-out!\n");
            return -1;
        }
    }

    if(KS->REMAIN != 0x20001000)
    {
        printf(" Fail! Remaining 0x%X space not clear for SRAM.\n", KS->REMAIN & 0x1FFF);
    }

    return 0;
}

void TAMPER_IRQHandler(void)
{
    uint32_t i;
    uint32_t u32FlagStatus;
    uint32_t u32TimeOutCnt;

    /* Tamper interrupt occurred */
    if(TAMPER_GET_INT_FLAG())
    {
        /* Get tamper interrupt status */
        u32FlagStatus = TAMPER_GET_INT_STATUS();

        for(i = 0; i < 6; i++)
        {
            if(u32FlagStatus & (0x1UL << (i + TAMPER_INTSTS_TAMP0IF_Pos)))
                printf(" Tamper %d Detected!!\n", i);
        }

        if(s_u8Option == '0')
        {
            /* Clear tamper interrupt status */
            TAMPER_CLR_INT_STATUS(u32FlagStatus);

            printf(" System wake-up!\n");
        }
        else if(s_u8Option == '1')
        {
            /* Clear tamper interrupt status */
            TAMPER_CLR_INT_STATUS(u32FlagStatus);

            printf(" Check keys are all zero ...");
            if( KS_TRIG() == 0 )
                printf(" Pass!\n");
        }

        /* To check if all the debug messages are finished */
        u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
        while(IsDebugFifoEmpty() == 0)
            if(--u32TimeOutCnt == 0) break;
        SYS_ResetChip();
    }
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/  
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
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();
    /* set power level to level 1 */
    PMC_SetPowerLevel(PMC_PLCTL_PLSEL_PL1);
    
    /* Set module clock*/
    CLK_SetModuleClock(WDT0_MODULE,CLK_WDTSEL_WDT0SEL_LIRC,0);
        
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

void UART_Init(void)
{

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

int32_t KS_Init(void)
{
    uint32_t u32TimeOutCnt;

    /* Key store initialization */
    KS->CTL = KS_CTL_INIT_Msk | KS_CTL_START_Msk;

    /* Waiting for initialization was done */
    u32TimeOutCnt = KS_TIMEOUT;
    while(!(KS->STS & KS_STS_INITDONE_Msk))
    {
        if(--u32TimeOutCnt == 0)
        {
            printf("Wait for Key Store initialization done time-out!\n");
            return -1;
        }
    }

    return 0;
}

int32_t KeyStoreSRAM(void)
{
    uint32_t u32TimeOutCnt;

    u32TimeOutCnt = SystemCoreClock;
    /* Check BUSY(KS_STS[2]), EIF(KS_STS[1]) and SRAMFULL(KS_STS[3]) is 0 */
    if(KS->STS & (KS_STS_BUSY_Msk | KS_STS_EIF_Msk | KS_STS_SRAMFULL_Msk))
    {
        printf("Key store status is wrong!\n");
    }

    /* Configure according key's metadata */
    KS->METADATA = ((0x1 << KS_METADATA_READABLE_Pos) | (0x6 << KS_METADATA_SIZE_Pos) | (0x5 << KS_METADATA_OWNER_Pos) | (0x0 << KS_METADATA_DST_Pos));

    /* Set OPMODE(KS_CTL[3:1]) to 001. */
    KS->CTL = (KS->CTL & (~KS_CTL_OPMODE_Msk)) | (1 << KS_CTL_OPMODE_Pos);

    /* Write key to KS_KEY[0]~[7] registers */
    KS->KEY[0] = 0x12345678;
    KS->KEY[1] = 0x12345678;
    KS->KEY[2] = 0x12345678;
    KS->KEY[3] = 0x12345678;
    KS->KEY[4] = 0x12345678;
    KS->KEY[5] = 0x12345678;
    KS->KEY[6] = 0x12345678;
    KS->KEY[7] = 0x12345678;
    
    /* Clear Status */
    KS->STS = KS_STS_EIF_Msk | KS_STS_IF_Msk; 

    /* Set START(KS_CTL[0]) to 1 */
    KS->CTL |= (1 << KS_CTL_START_Pos);

    /* Wait BUSY(KS_STS[2]) to 0 */
    while(KS->STS & KS_STS_BUSY_Msk)
    {
        if(--u32TimeOutCnt == 0)
        {
            printf("Wait for Key Store busy flag time-out!\n");
            return -1;
        }
    }

    /* Check EIF(KS_STS[1]) is 0 */
    if(KS->STS & KS_STS_EIF_Msk)
    {
        printf("EIF(KS_STS[1]) is not 0.\n");
    }

    return 0;
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
    
    if(u32Confi3Read == 0x5aa5ffff){
        FMC_ENABLE_ISP();
        FMC_ENABLE_CFG_UPDATE();
        FMC_WriteConfig(FMC_USER_CONFIG_3, 0x5a5affff);
        FMC_WriteConfig(FMC_USER_CONFIG_3, 0x5a5affff);
        SYS_ResetChip();
        while(1);
    }

    /* Lock protected registers */
    SYS_LockReg();

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
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

    if(s_u8Option == '0')
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
        u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
        while(IsDebugFifoEmpty() == 0)
            if(--u32TimeOutCnt == 0) break;
        PMC_PowerDown();
    }
    else if(s_u8Option == '1')
    {
        /* Enable to trigger Key Store */
        TAMPER_ENABLE_KS_TRIG();

        /* Unlock protected registers */
        SYS_UnlockReg();

        /* Init Key Store */
        if( KS_Init() <0 ) return -1;

        printf("# Write keys ...");
        if( KeyStoreSRAM() < 0 ) return -1;
        printf(" Done!\n\n");

        printf("# Please connect TAMPER2/3(PF.8/9) pins to Low to generate TAMPER event.\n");
        printf("# Press any key to start test:\n\n");
        getchar();

        printf("# Check keys in SRAM of Key Store when tamper event occurred:\n");

        /* Wait for tamper event interrupt happened */
        while(1);
    }
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/