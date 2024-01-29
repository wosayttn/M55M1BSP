/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Secure sample code for TrustZone
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <arm_cmse.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "partition_M55M1.h"
#include "nsclib.h"
#include "dpm_cunit.h"
#include "../../pldm_emu.h"

#ifndef DEBUG_PORT
    #define DEBUG_PORT UART0
#endif

#define LOOP_HERE       0xE7FEE7FF      /* Instruction Code of "B ." */

void SysTick_Handler(void)
{
    static uint32_t u32Ticks;

    if ((u32Ticks++ % 1000) == 0)
    {
        printf("Secure SysTick\n");
    }
}

void SCU_IRQHandler(void)
{
    //    if (DPM->STS & DPM_STS_PWCERR_Msk)
    //        DPM->STS = DPM_STS_PWCERR_Msk;
    //
    //    if (DPM_NS->NSSTS & DPM_NSSTS_PWCERR_Msk)
    //        DPM_NS->NSSTS = DPM_NSSTS_PWCERR_Msk;
}

/*----------------------------------------------------------------------------
  Secure function exported to Non-Secure application
  Must decalre with __NONSECURE_ENTRY and place in Non-Secure Callable region
 *----------------------------------------------------------------------------*/
__NONSECURE_ENTRY
int32_t SecureFunc(void)
{
    printf("Secure function call by Non-Secure.\n");

    return 1;
}

__NONSECURE_ENTRY
uint32_t Secure_GetSystemCoreClock(void)
{
    printf("System core clock = %d.\n", SystemCoreClock);
    return SystemCoreClock;
}

/*----------------------------------------------------------------------------*/
/*  Boot_NonSecure function is used to jump to Non-Secure boot code.          */
/*----------------------------------------------------------------------------*/
void Boot_NonSecure(uint32_t u32NonSecureBase)
{
    PFN_NON_SECURE_FUNC pfnNonSecureEntry;

    /* SCB_NS.VTOR points to the Non-Secure vector table base address. */
    SCB_NS->VTOR = u32NonSecureBase;

    /* 1st entry in the vector table is the Non-Secure Main Stack Pointer. */
    __TZ_set_MSP_NS(*((uint32_t *)SCB_NS->VTOR));      /* Set up MSP in Non-Secure code */

    /* 2nd entry contains the address of the Reset_Handler (CMSIS-CORE) function */
    pfnNonSecureEntry = ((PFN_NON_SECURE_FUNC)(*(((uint32_t *)SCB_NS->VTOR) + 1)));

    /* Clear the LSB of the function address to indicate the function-call
       will cause a state switch from Secure to Non-Secure */
    pfnNonSecureEntry = cmse_nsfptr_create(pfnNonSecureEntry);

    /* Check if the Reset_Handler address is in Non-Secure space */
    if (cmse_is_nsfptr(pfnNonSecureEntry) && (((uint32_t)pfnNonSecureEntry & NS_OFFSET) == NS_OFFSET))
    {
        printf("Execute Non-Secure code at 0x%08X\n", u32NonSecureBase);
        pfnNonSecureEntry(0); /* Non-Secure function entry */
    }
    else
    {
        /* Something went wrong */
        printf("No code in Non-Secure region !\n");
        printf("CPU will halted at Non-Secure state.\n");

        /* Set Non-Secure MSP in Non-Secure region */
        __TZ_set_MSP_NS(NON_SECURE_SRAM_BASE + 512);

        /* Try to halted in Non-Secure state (SRAM) */
        M32(NON_SECURE_SRAM_BASE) = LOOP_HERE;
        pfnNonSecureEntry = (PFN_NON_SECURE_FUNC)(NON_SECURE_SRAM_BASE + 1);
        pfnNonSecureEntry(0);

        while (1);
    }
}

static void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
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
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable module clock */
    CLK_EnableModuleClock(ISP0_MODULE);

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /*----------------------------------------------------------------------*/
    /* Init UART1 for Non-Secure console                                    */
    /* (Config UART1 to Non-Secure in partition_TC8263.h)                   */
    /*----------------------------------------------------------------------*/
    CLK_EnableModuleClock(UART1_MODULE);
    CLK_SetModuleClock(UART1_MODULE, CLK_UARTSEL0_UART1SEL_HIRC, CLK_UARTDIV0_UART1DIV(1));
    SET_UART1_RXD_PA2();
    SET_UART1_TXD_PA3();

    /* Lock protected registers */
    SYS_LockReg();
}

void exit(int32_t code)
{
    if (code)
        while (1);  // Fail
    else
        while (1);  // Success
}

void AddTests(void)
{
    assert((NULL != CU_get_registry()));
    assert(!CU_is_test_running());

    if (CUE_SUCCESS != CU_register_suites(DPM_Suites))
    {
        fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }
}

void CUnitTest(void)
{
    printf("\n\n");
    printf("+--------------------------------------+\n");
    printf("|      M55M1 DPM Secure CUnit Test     |\n");
    printf("+--------------------------------------+\n");

    if (CU_initialize_registry())
    {
        fprintf(stderr, " Initialization of Test Registry failed. ");
        exit(EXIT_FAILURE);
    }
    else
    {
        AddTests();
        CU_console_run_tests();
        CU_cleanup_registry();
    }
}

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main(void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

    printf("+-----------------------------------------+\n");
    printf("|           Execute Secure code           |\n");
    printf("+-----------------------------------------+\n");

    SYS_UnlockReg();
    FMC_Open();
    printf("SAU->CTRL:    0x%08X\n", SAU->CTRL);
    printf("SCU->FNSADDR: 0x%08X, NSCBA:        0x%08X\n", SCU->FNSADDR, FMC_Read(FMC_NSCBA_BASE));
    printf("SRAM0MPCLUT0: 0x%08X, SRAM1MPCLUT0: 0x%08X\n", SCU->SRAM0MPCLUT0, SCU->SRAM1MPCLUT0);
    printf("SRAM2MPCLUT0: 0x%08X\n", SCU->SRAM2MPCLUT0);

    /* Generate Systick interrupt each 10 ms */
    SysTick_Config(SystemCoreClock / 100);
    NVIC_DisableIRQ(SCU_IRQn);
    CUnitTest();
    printf("\n\nPress any key to continue Non-Secure test\n");
    getchar();

    /* Init and jump to Non-Secure code */
    Boot_NonSecure(FMC_NON_SECURE_BASE);

    do
    {
        __WFI();
    }

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
