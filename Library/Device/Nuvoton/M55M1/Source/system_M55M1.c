/**************************************************************************//**
 * @file     system_M55M1.c
 * @version  V1.00
 * @brief    CMSIS Device System Source File for NuMicro M55M1
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)       /* ARM Compiler 6 */
#include <arm_cmse.h>
#endif

#include "NuMicro.h"

#if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
    #include "partition_M55M1.h"
    #include "mpc_sie_drv.h"
#endif

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/
extern const VECTOR_TABLE_Type __VECTOR_TABLE[];

/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = __HSI;                /*!< System Clock Frequency (Core Clock) */
uint32_t CyclesPerUs     = (__HSI / 1000000UL);  /* Cycles per micro second */
uint32_t PllClock        = __HSI;                /*!< PLL Output Clock Frequency         */

void TZ_SAU_Setup(void);
void FMC_NSCBA_Setup(void);
void SCU_Setup(void);
void NSC_Init(uint32_t u32RegionIdx);

/*----------------------------------------------------------------------------
  System Core Clock update function
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate(void)
{    
    uint32_t u32Freq;
    uint32_t u32AclkDiv;

    u32Freq = CLK_SystemClockUpdate();
    
    u32AclkDiv = (CLK->ACLKDIV & CLK_ACLKDIV_ACLKDIV_Msk) + 1UL;

    /* Update System Core Clock */
    SystemCoreClock = u32Freq / u32AclkDiv;

    CyclesPerUs = (SystemCoreClock + 500000UL) / 1000000UL;
}

uint32_t GetSystemCoreClock(void)
{
    return SystemCoreClock;
}

#ifndef NVT_DBG_UART_OFF
/**
 * Set debug UART multi-function pins
 *
 * @param  none
 * @return none
 *
 * @brief  Set debug UART multi-function pins
 */
__WEAK void SetDebugUartMFP(void)
{
#if !defined(DEBUG_ENABLE_SEMIHOST) && !defined(OS_USE_SEMIHOSTING)

    /* Set GPB12 as UART0 RXD and GPB13 as UART0 TXD */
    SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13();

#endif /* !defined(DEBUG_ENABLE_SEMIHOST) || !defined(OS_USE_SEMIHOSTING) */
}

/**
 * Set debug UART clock
 *
 * @param    None
 * @return   None
 *
 * @brief  Set debug UART clock source and enable module clock.
 */
__WEAK void SetDebugUartCLK(void)
{
#if !defined(DEBUG_ENABLE_SEMIHOST) && !defined(OS_USE_SEMIHOSTING)

    /* Select UART clock source from HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HIRC, CLK_UARTDIV0_UART0DIV(1));

    /* Enable UART clock */
    CLK_EnableModuleClock(UART0_MODULE);

#endif /* !defined(DEBUG_ENABLE_SEMIHOST) && !defined(OS_USE_SEMIHOSTING) */
}

/**
 * @brief    Init UART
 *
 * @param    None
 * @return   None
 *
 * @details  Initialize debug UART to 115200-8n1
 */
__WEAK void InitDebugUart(void)
{
#if !defined(DEBUG_ENABLE_SEMIHOST) && !defined(OS_USE_SEMIHOSTING)

    /* Reset UART module */
    SYS_ResetModule(SYS_UART0RST);
    /* Init UART to 115200-8n1 for print message */
    UART_Open(DEBUG_PORT, 115200);

#endif /* !defined(DEBUG_ENABLE_SEMIHOST) && !defined(OS_USE_SEMIHOSTING) */
}
#endif /* NVT_DBG_UART_OFF */

/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/
__attribute__ ((constructor)) void SystemInit(void)
{
#ifndef NVT_ICACHE_OFF
    //SCB_EnableICache();
#endif
    
#ifndef NVT_DCACHE_OFF
    //SCB_EnableDCache();
#endif

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    SCB->VTOR = (uint32_t)(&__VECTOR_TABLE[0]);
#endif

#if (defined (__FPU_USED) && (__FPU_USED == 1U)) || \
    (defined (__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE > 0U))
    SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                   (3U << 11U*2U)  );         /* enable CP11 Full Access */

    /* Set low-power state for PDEPU                */
    /*  0b00  | ON, PDEPU is not in low-power state */
    /*  0b01  | ON, but the clock is off            */
    /*  0b10  | RET(ention)                         */
    /*  0b11  | OFF                                 */

    /* Clear ELPSTATE, value is 0b11 on Cold reset */
    PWRMODCTL->CPDLPSTATE &= ~(PWRMODCTL_CPDLPSTATE_ELPSTATE_Msk << PWRMODCTL_CPDLPSTATE_ELPSTATE_Pos);

    /* Favor best FP/MVE performance by default, avoid EPU switch-ON delays */
    /* PDEPU ON, Clock OFF */
    PWRMODCTL->CPDLPSTATE |= 0x1 << PWRMODCTL_CPDLPSTATE_ELPSTATE_Pos;
#endif

#ifdef UNALIGNED_SUPPORT_DISABLE
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

    /* Enable Loop and branch info cache */
    SCB->CCR |= SCB_CCR_LOB_Msk;
    __DSB();
    __ISB();

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    TZ_SAU_Setup();
    SCU_Setup();
    FMC_NSCBA_Setup();
#endif
}

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
/**
 * @brief    Setup Non-secure boundary
 *
 * @details  This function is used to set Non-secure boundary according to
 *           the configuration of partition header file
 */
void FMC_NSCBA_Setup(void)
{
    /* Skip NSCBA Setup according config */
    if(FMC_INIT_NSCBA == 0)
        return;

    /* Check if NSCBA value with current active NSCBA */
    if(SCU->FNSADDR != FMC_SECURE_END)
    {
        /* Unlock Protected Register */
        SYS_UnlockReg();
        
        /* Switch power level to PL1 to update NSCBA */
        if (PMC->PLCTL != PMC_PLCTL_PLSEL_PL1)
            PMC_SetPowerLevel(PMC_PLCTL_PLSEL_PL1);

        /* Enable FMC ISP and config update */
        FMC->ISPCTL = FMC_ISPCTL_ISPEN_Msk | FMC_ISPCTL_CFGUEN_Msk;

        /* Config Base of NSCBA */
        FMC->ISPADDR = FMC_NSCBA_BASE ;

        /* Read Non-secure base address config */
        FMC->ISPCMD = FMC_ISPCMD_READ;
        FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;
        while(FMC->ISPTRG);

        //while(PA0);

        /* Setting NSCBA when it is empty */
        if(FMC->ISPDAT != 0xFFFFFFFFul)
        {
            /* Erase old setting */
            FMC->ISPCMD = FMC_ISPCMD_CFG_ERASE;
            FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;
            while(FMC->ISPTRG);
        }

        /* Set new base */
        FMC->ISPDAT = FMC_SECURE_END;
        FMC->ISPCMD = FMC_ISPCMD_PROGRAM;
        FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;
        while(FMC->ISPTRG);

        /* Force Chip Reset to valid new setting */
        SYS->RSTCTL = SYS_RSTCTL_CHIPRST_Msk;
    }
}

#define NVIC_ITNS_CONF(IRQn)  (NVIC->ITNS[(IRQn / 32)] |= (1 << (IRQn % 32)))

/**
  \brief   Setup SCU Configuration Unit
  \details

 */
void SCU_Setup(void)
{
    int32_t i;
    
    SYS_ResetModule(SYS_SCU0RST);
    SCU->D0PNS0 = SCU_INIT_D0PNS0_VAL;
    SCU->D0PNS2 = SCU_INIT_D0PNS2_VAL;
    SCU->D1PNS0 = SCU_INIT_D1PNS0_VAL;
    SCU->D1PNS1 = SCU_INIT_D1PNS1_VAL;
    SCU->D1PNS2 = SCU_INIT_D1PNS2_VAL;
    SCU->D1PNS4 = SCU_INIT_D1PNS4_VAL;
    SCU->D2PNS0 = SCU_INIT_D2PNS0_VAL;
    SCU->D2PNS2 = SCU_INIT_D2PNS2_VAL;

    SCU->IONS[0] = SCU_INIT_IONS0_VAL;
    SCU->IONS[1] = SCU_INIT_IONS1_VAL;
    SCU->IONS[2] = SCU_INIT_IONS2_VAL;
    SCU->IONS[3] = SCU_INIT_IONS3_VAL;
    SCU->IONS[4] = SCU_INIT_IONS4_VAL;
    SCU->IONS[5] = SCU_INIT_IONS5_VAL;
    SCU->IONS[6] = SCU_INIT_IONS6_VAL;
    SCU->IONS[7] = SCU_INIT_IONS7_VAL;
    SCU->IONS[8] = SCU_INIT_IONS8_VAL;
    SCU->IONS[9] = SCU_INIT_IONS9_VAL;
    
    /* Set Secure/Non-secure SRAM region through MPC */
    if (SCU_SECURE_SRAM_SIZE <= SRAM0_SIZE)
    {
        SetupMPC(SRAM0MPC_BASE, SRAM0_BASE, SRAM0_SIZE, SRAM0_BASE, SCU_SECURE_SRAM_SIZE, NON_SECURE_SRAM_BASE,   SRAM0_SIZE - SCU_SECURE_SRAM_SIZE);
        SetupMPC(SRAM1MPC_BASE, SRAM1_BASE, SRAM1_SIZE, SRAM1_BASE, 0, SRAM1_BASE + NS_OFFSET, SRAM1_SIZE);
    }
    else
    {
        SetupMPC(SRAM0MPC_BASE, SRAM0_BASE, SRAM0_SIZE, SRAM0_BASE, SRAM0_SIZE, SRAM0_BASE + NS_OFFSET, 0);
        SetupMPC(SRAM1MPC_BASE, SRAM1_BASE, SRAM1_SIZE, SRAM1_BASE, SCU_SECURE_SRAM_SIZE - SRAM0_SIZE, NON_SECURE_SRAM_BASE, (SRAM0_SIZE + SRAM1_SIZE) - SCU_SECURE_SRAM_SIZE);
    }

    /* Set interrupt to non-secure according to DxPNSy settings */
    /* SCU_D0PNS0 */
    if (SCU_INIT_D0PNS0_VAL & SCU_D0PNS0_NPU_Msk)
        NVIC_ITNS_CONF( NPU_IRQn );
    
    /* SCU_D0PNS2 */
    if (SCU_INIT_D0PNS2_VAL & SCU_D0PNS2_SPIM0_Msk)
        NVIC_ITNS_CONF( SPIM0_IRQn );
    if (SCU_INIT_D0PNS2_VAL & SCU_D0PNS2_SPIM1_Msk)
        NVIC_ITNS_CONF( SPIM1_IRQn );
    if (SCU_INIT_D1PNS0_VAL & SCU_D1PNS0_PDMA0_Msk)
        NVIC_ITNS_CONF( PDMA0_IRQn );
    if (SCU_INIT_D1PNS0_VAL & SCU_D1PNS0_PDMA1_Msk)
        NVIC_ITNS_CONF( PDMA1_IRQn );
    
    /* SCU_D1PNS0 */
    if (SCU_INIT_D1PNS0_VAL & SCU_D1PNS0_USBH0_Msk)
        NVIC_ITNS_CONF( USBH0_IRQn );
    if (SCU_INIT_D1PNS0_VAL & SCU_D1PNS0_USBH1_Msk)
        NVIC_ITNS_CONF( USBH1_IRQn );
    if (SCU_INIT_D1PNS0_VAL & SCU_D1PNS0_HSUSBH_Msk)
        NVIC_ITNS_CONF( HSUSBH_IRQn );
    if (SCU_INIT_D1PNS0_VAL & SCU_D1PNS0_HSUSBD_Msk)
        NVIC_ITNS_CONF( HSUSBD_IRQn );
    if (SCU_INIT_D1PNS0_VAL & SCU_D1PNS0_SDH0_Msk)
        NVIC_ITNS_CONF( SDH0_IRQn );
    if (SCU_INIT_D1PNS0_VAL & SCU_D1PNS0_SDH1_Msk)
        NVIC_ITNS_CONF( SDH1_IRQn );
    if (SCU_INIT_D1PNS0_VAL & SCU_D1PNS0_EMAC0_Msk)
        NVIC_ITNS_CONF( EMAC0_IRQn );
    if (SCU_INIT_D1PNS0_VAL & SCU_D1PNS0_CRYPTO_Msk)
        NVIC_ITNS_CONF( CRYPTO_IRQn );
    if (SCU_INIT_D1PNS0_VAL & SCU_D1PNS0_CRC_Msk)
        NVIC_ITNS_CONF( CRC_IRQn );
    
    /* SCU_D1PNS1 */
    if (SCU_INIT_D1PNS1_VAL & SCU_D1PNS1_CANFD0_Msk)
    {
        NVIC_ITNS_CONF( CANFD00_IRQn );
        NVIC_ITNS_CONF( CANFD01_IRQn );
    }
    if (SCU_INIT_D1PNS1_VAL & SCU_D1PNS1_CANFD1_Msk)
    {
        NVIC_ITNS_CONF( CANFD10_IRQn );
        NVIC_ITNS_CONF( CANFD11_IRQn );
    }
    
    /* SCU_D1PNS2 */
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_WWDT0_Msk)
        NVIC_ITNS_CONF( WWDT0_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_EADC0_Msk)
    {
        NVIC_ITNS_CONF( EADC00_IRQn );
        NVIC_ITNS_CONF( EADC01_IRQn );
        NVIC_ITNS_CONF( EADC02_IRQn );
        NVIC_ITNS_CONF( EADC03_IRQn );
    }
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_EPWM0_Msk)
    {
        NVIC_ITNS_CONF( BRAKE0_IRQn  );
        NVIC_ITNS_CONF( EPWM0P0_IRQn );
        NVIC_ITNS_CONF( EPWM0P1_IRQn );
        NVIC_ITNS_CONF( EPWM0P2_IRQn );
    }
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_BPWM0_Msk)
        NVIC_ITNS_CONF( BPWM0_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_EQEI0_Msk)
        NVIC_ITNS_CONF( EQEI0_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_EQEI2_Msk)
        NVIC_ITNS_CONF( EQEI2_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_ECAP0_Msk)
        NVIC_ITNS_CONF( ECAP0_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_ECAP2_Msk)
        NVIC_ITNS_CONF( ECAP2_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_I2C0_Msk)
        NVIC_ITNS_CONF( I2C0_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_I2C2_Msk)
        NVIC_ITNS_CONF( I2C2_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_QSPI0_Msk)
        NVIC_ITNS_CONF( QSPI0_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_SPI0_Msk)
        NVIC_ITNS_CONF( SPI0_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_SPI2_Msk)
        NVIC_ITNS_CONF( SPI2_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_UART0_Msk)
        NVIC_ITNS_CONF( UART0_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_UART2_Msk)
        NVIC_ITNS_CONF( UART2_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_UART4_Msk)
        NVIC_ITNS_CONF( UART4_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_UART6_Msk)
        NVIC_ITNS_CONF( UART6_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_UART8_Msk)
        NVIC_ITNS_CONF( UART8_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_USCI0_Msk)
        NVIC_ITNS_CONF( USCI0_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_SC0_Msk)
        NVIC_ITNS_CONF( SC0_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_SC2_Msk)
        NVIC_ITNS_CONF( SC2_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_PSIO_Msk)
        NVIC_ITNS_CONF( PSIO_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_TMR01_Msk)
    {
        NVIC_ITNS_CONF( TIMER0_IRQn );
        NVIC_ITNS_CONF( TIMER1_IRQn );
    }
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_DAC01_Msk)
        NVIC_ITNS_CONF( DAC01_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_HSOTG_Msk)
        NVIC_ITNS_CONF( HSOTG_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_I2S0_Msk)
        NVIC_ITNS_CONF( I2S0_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_ACMP01_Msk)
        NVIC_ITNS_CONF( ACMP01_IRQn );
    if (SCU_INIT_D1PNS2_VAL & SCU_D1PNS2_USBD_Msk)
        NVIC_ITNS_CONF( USBD_IRQn );
    
    /* SCU_D1PNS4 */
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_WWDT1_Msk)
        NVIC_ITNS_CONF( WWDT1_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_EADC1_Msk)
    {
        NVIC_ITNS_CONF( EADC10_IRQn );
        NVIC_ITNS_CONF( EADC11_IRQn );
        NVIC_ITNS_CONF( EADC12_IRQn );
        NVIC_ITNS_CONF( EADC13_IRQn );
    }
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_EPWM1_Msk)
    {
        NVIC_ITNS_CONF( BRAKE1_IRQn  );
        NVIC_ITNS_CONF( EPWM1P0_IRQn );
        NVIC_ITNS_CONF( EPWM1P1_IRQn );
        NVIC_ITNS_CONF( EPWM1P2_IRQn );
    }
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_BPWM1_Msk)
        NVIC_ITNS_CONF( BPWM1_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_EQEI1_Msk)
        NVIC_ITNS_CONF( EQEI1_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_EQEI3_Msk)
        NVIC_ITNS_CONF( EQEI3_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_ECAP1_Msk)
        NVIC_ITNS_CONF( ECAP1_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_ECAP3_Msk)
        NVIC_ITNS_CONF( ECAP3_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_I2C1_Msk)
        NVIC_ITNS_CONF( I2C1_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_I2C3_Msk)
        NVIC_ITNS_CONF( I2C3_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_QSPI1_Msk)
        NVIC_ITNS_CONF( QSPI1_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_SPI1_Msk)
        NVIC_ITNS_CONF( SPI1_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_SPI3_Msk)
        NVIC_ITNS_CONF( SPI3_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_UART1_Msk)
        NVIC_ITNS_CONF( UART1_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_UART3_Msk)
        NVIC_ITNS_CONF( UART3_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_UART5_Msk)
        NVIC_ITNS_CONF( UART5_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_UART7_Msk)
        NVIC_ITNS_CONF( UART7_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_UART9_Msk)
        NVIC_ITNS_CONF( UART9_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_SC1_Msk)
        NVIC_ITNS_CONF( SC1_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_OTG_Msk)
        NVIC_ITNS_CONF( USBOTG_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_KPI_Msk)
        NVIC_ITNS_CONF( KPI_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_TMR23_Msk)
    {
        NVIC_ITNS_CONF( TIMER2_IRQn );
        NVIC_ITNS_CONF( TIMER3_IRQn );
    }
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_TRNG_Msk)
        NVIC_ITNS_CONF( TRNG_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_RTC_Msk)
        NVIC_ITNS_CONF( RTC_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_I2S1_Msk)
        NVIC_ITNS_CONF( I2S1_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_ACMP23_Msk)
        NVIC_ITNS_CONF( ACMP23_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_I3C0_Msk)
        NVIC_ITNS_CONF( I3C0_IRQn );
    if (SCU_INIT_D1PNS4_VAL & SCU_D1PNS4_UTCPD_Msk)
        NVIC_ITNS_CONF( UTCPD_IRQn );
    
    /* SCU_D2PNS0 */
    if (SCU_INIT_D2PNS0_VAL & SCU_D2PNS0_LPPDMA_Msk)
        NVIC_ITNS_CONF( LPPDMA_IRQn );
    if (SCU_INIT_D2PNS0_VAL & SCU_D2PNS0_CCAP_Msk)
        NVIC_ITNS_CONF( CCAP_IRQn );
    
    /* SCU_D2PNS2 */
    if (SCU_INIT_D2PNS2_VAL & SCU_D2PNS2_LPTMR01_Msk)
    {
        NVIC_ITNS_CONF( LPTMR0_IRQn );
        NVIC_ITNS_CONF( LPTMR1_IRQn );
    }
    if (SCU_INIT_D2PNS2_VAL & SCU_D2PNS2_TTMR01_Msk)
    {
        NVIC_ITNS_CONF( TTMR0_IRQn );
        NVIC_ITNS_CONF( TTMR1_IRQn );
    }
    if (SCU_INIT_D2PNS2_VAL & SCU_D2PNS2_LPADC0_Msk)
        NVIC_ITNS_CONF( LPADC0_IRQn );
    if (SCU_INIT_D2PNS2_VAL & SCU_D2PNS2_LPI2C0_Msk)
        NVIC_ITNS_CONF( LPI2C0_IRQn );
    if (SCU_INIT_D2PNS2_VAL & SCU_D2PNS2_LPSPI0_Msk)
        NVIC_ITNS_CONF( LPSPI0_IRQn );
    if (SCU_INIT_D2PNS2_VAL & SCU_D2PNS2_DMIC0_Msk)
    {
        NVIC_ITNS_CONF( DMIC0_IRQn );
        NVIC_ITNS_CONF( DMIC0VAD_IRQn );
    }
    if (SCU_INIT_D2PNS2_VAL & SCU_D2PNS2_LPUART0_Msk)
        NVIC_ITNS_CONF( LPUART0_IRQn );
    if (SCU_INIT_D2PNS2_VAL & SCU_D2PNS2_WDT0_Msk)
        NVIC_ITNS_CONF( WDT0_IRQn );
    if (SCU_INIT_D2PNS2_VAL & SCU_D2PNS2_WDT1_Msk)
        NVIC_ITNS_CONF( WDT1_IRQn );
    if (SCU_INIT_D2PNS2_VAL & SCU_D2PNS2_AWF_Msk)
        NVIC_ITNS_CONF( AWF_IRQn );
    
    /* Set interrupt to non-secure according to SCU_INIT_IO_ITNS_VAL settings */
    if (SCU_INIT_IO_ITNS_VAL & BIT0 ) NVIC_ITNS_CONF( GPA_IRQn    );
    if (SCU_INIT_IO_ITNS_VAL & BIT1 ) NVIC_ITNS_CONF( GPB_IRQn    );
    if (SCU_INIT_IO_ITNS_VAL & BIT2 ) NVIC_ITNS_CONF( GPC_IRQn    );
    if (SCU_INIT_IO_ITNS_VAL & BIT3 ) NVIC_ITNS_CONF( GPD_IRQn    );
    if (SCU_INIT_IO_ITNS_VAL & BIT4 ) NVIC_ITNS_CONF( GPE_IRQn    );
    if (SCU_INIT_IO_ITNS_VAL & BIT5 ) NVIC_ITNS_CONF( GPF_IRQn    );
    if (SCU_INIT_IO_ITNS_VAL & BIT6 ) NVIC_ITNS_CONF( GPG_IRQn    );
    if (SCU_INIT_IO_ITNS_VAL & BIT7 ) NVIC_ITNS_CONF( GPH_IRQn    );
    if (SCU_INIT_IO_ITNS_VAL & BIT8 ) NVIC_ITNS_CONF( GPI_IRQn    );
    if (SCU_INIT_IO_ITNS_VAL & BIT9 ) NVIC_ITNS_CONF( GPJ_IRQn    );
    if (SCU_INIT_IO_ITNS_VAL & BIT10) NVIC_ITNS_CONF( EINT0_IRQn  );
    if (SCU_INIT_IO_ITNS_VAL & BIT11) NVIC_ITNS_CONF( EINT1_IRQn  );
    if (SCU_INIT_IO_ITNS_VAL & BIT12) NVIC_ITNS_CONF( EINT2_IRQn  );
    if (SCU_INIT_IO_ITNS_VAL & BIT13) NVIC_ITNS_CONF( EINT3_IRQn  );
    if (SCU_INIT_IO_ITNS_VAL & BIT14) NVIC_ITNS_CONF( EINT4_IRQn  );
    if (SCU_INIT_IO_ITNS_VAL & BIT15) NVIC_ITNS_CONF( EINT5_IRQn  );
    if (SCU_INIT_IO_ITNS_VAL & BIT16) NVIC_ITNS_CONF( EINT6_IRQn  );
    if (SCU_INIT_IO_ITNS_VAL & BIT17) NVIC_ITNS_CONF( EINT7_IRQn  );

    /* Enable SCU Int status */
    SCU->SVIEN0 = (uint32_t)(-1);
    SCU->SVIEN1 = (uint32_t)(-1);
    NVIC_EnableIRQ(SCU_IRQn);
}

/**
  \brief   Setup a Nonsecure callable Region
  \details The base and limit of Nonsecure callable region is dependent on the
           application code size.
 */
void NSC_Init(uint32_t u32RegionIdx)
{
    uint32_t u32Base = 0, 
             u32Limit = 0;

#if defined (__ICCARM__)
# pragma section = "NSC"
    u32Base  = (uint32_t)__section_begin("NSC");
    u32Limit = (uint32_t)__section_end("NSC");
#elif defined(__ARMCC_VERSION)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
    extern uint32_t Image$$NSC_ROM$$XO$$Base [];
    extern uint32_t Image$$NSC_ROM$$XO$$Limit[];
    u32Base  = (uint32_t)Image$$NSC_ROM$$XO$$Base;
    u32Limit = (uint32_t)Image$$NSC_ROM$$XO$$Limit;
#pragma clang diagnostic pop
#else
    extern uint32_t __start_NSC[];
    extern uint32_t __end_NSC[];
    u32Base  = (uint32_t)__start_NSC;
    u32Limit = (uint32_t)__end_NSC;
#endif
    
    if (u32Limit > 0)
    {
        SAU->RNR  = (u32RegionIdx & SAU_RNR_REGION_Msk);
        SAU->RBAR = (u32Base & SAU_RBAR_BADDR_Msk);
        SAU->RLAR = (u32Limit & SAU_RLAR_LADDR_Msk) |
                    (SAU_RLAR_NSC_Msk | 1ul);
    }
}

/**
  \brief   Setup a SAU Region
  \details Writes the region information contained in SAU_Region to the
           registers SAU_RNR, SAU_RBAR, and SAU_RLAR
 */
void TZ_SAU_Setup(void)
{
#if defined (__SAUREGION_PRESENT) && (__SAUREGION_PRESENT == 1U)

#if defined (SAU_INIT_REGION0) && (SAU_INIT_REGION0 == 1U)
    SAU_INIT_REGION(0);
#endif

#if defined (SAU_INIT_REGION1) && (SAU_INIT_REGION1 == 1U)
    SAU_INIT_REGION(1);
#endif

#if defined (SAU_INIT_REGION2) && (SAU_INIT_REGION2 == 1U)
    SAU_INIT_REGION(2);
#endif

#if defined (SAU_INIT_REGION3) && (SAU_INIT_REGION3 == 1U)
    SAU_INIT_REGION(3);
#endif

#if defined (SAU_INIT_REGION4) && (SAU_INIT_REGION4 == 1U)
    SAU_INIT_REGION(4);
#endif

#if defined (SAU_INIT_REGION5) && (SAU_INIT_REGION5 == 1U)
    SAU_INIT_REGION(5);
#endif

#if defined (SAU_INIT_REGION6) && (SAU_INIT_REGION6 == 1U)
    SAU_INIT_REGION(6);
#endif

#if defined (SAU_INIT_REGION7) && (SAU_INIT_REGION7 == 1U)
    SAU_INIT_REGION(7);
#endif
#endif /* defined (__SAUREGION_PRESENT) && (__SAUREGION_PRESENT == 1U) */

#if defined (SAU_INIT_CTRL) && (SAU_INIT_CTRL == 1U)
    SAU->CTRL = ((SAU_INIT_CTRL_ENABLE << SAU_CTRL_ENABLE_Pos) & SAU_CTRL_ENABLE_Msk) |
                ((SAU_INIT_CTRL_ALLNS  << SAU_CTRL_ALLNS_Pos)  & SAU_CTRL_ALLNS_Msk)   ;
#endif

#if defined (SCB_CSR_AIRCR_INIT) && (SCB_CSR_AIRCR_INIT == 1U)
    SCB->SCR   = (SCB->SCR   & ~(SCB_SCR_SLEEPDEEPS_Msk)) |
                 ((SCB_CSR_DEEPSLEEPS_VAL     << SCB_SCR_SLEEPDEEPS_Pos)     & SCB_SCR_SLEEPDEEPS_Msk);

    SCB->AIRCR = (0x05FA << 16) |
                 ((SCB_AIRCR_SYSRESETREQS_VAL << SCB_AIRCR_SYSRESETREQS_Pos) & SCB_AIRCR_SYSRESETREQS_Msk) |
                 ((SCB_AIRCR_BFHFNMINS_VAL    << SCB_AIRCR_BFHFNMINS_Pos)    & SCB_AIRCR_BFHFNMINS_Msk)    |
                 ((SCB_AIRCR_PRIS_VAL         << SCB_AIRCR_PRIS_Pos)         & SCB_AIRCR_PRIS_Msk);

#endif /* defined (SCB_CSR_AIRCR_INIT) && (SCB_CSR_AIRCR_INIT == 1U) */

#if (((defined (__FPU_USED) && (__FPU_USED == 1U))              || \
      (defined (__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE > 0))) && \
      (defined (TZ_FPU_NS_USAGE) && (TZ_FPU_NS_USAGE == 1U)))

    SCB->NSACR = (SCB->NSACR & ~(SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk)) |
                    ((SCB_NSACR_CP10_11_VAL << SCB_NSACR_CP10_Pos) & (SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk));

    FPU->FPCCR = (FPU->FPCCR & ~(FPU_FPCCR_TS_Msk | FPU_FPCCR_CLRONRETS_Msk | FPU_FPCCR_CLRONRET_Msk)) |
                    ((FPU_FPCCR_TS_VAL        << FPU_FPCCR_TS_Pos       ) & FPU_FPCCR_TS_Msk       ) |
                    ((FPU_FPCCR_CLRONRETS_VAL << FPU_FPCCR_CLRONRETS_Pos) & FPU_FPCCR_CLRONRETS_Msk) |
                    ((FPU_FPCCR_CLRONRET_VAL  << FPU_FPCCR_CLRONRET_Pos ) & FPU_FPCCR_CLRONRET_Msk );
#endif

#if defined (SCB_ICSR_INIT) && (SCB_ICSR_INIT == 1U)
    SCB->ICSR  = (SCB->ICSR  & ~(SCB_ICSR_STTNS_Msk)) |
               ((SCB_ICSR_STTNS_VAL         << SCB_ICSR_STTNS_Pos)         & SCB_ICSR_STTNS_Msk);
#endif /* defined (SCB_ICSR_INIT) && (SCB_ICSR_INIT == 1U) */

    /* repeat this for all possible ITNS elements */

    /* Initial NonSecure Callable region */
    NSC_Init(4);    /* SAU region 4 is dedicated for NSC */
}

#endif /* defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) */