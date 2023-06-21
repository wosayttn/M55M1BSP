/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://aws.amazon.com/freertos
 *
 */
#include <stdio.h>
#include <stdlib.h>

/* Use CMSE intrinsics */
#include <arm_cmse.h>
//#include "RTE_Components.h"
//#include CMSIS_device_header
#include "NuMicro.h"
/* FreeRTOS includes. */
#include "secure_port_macros.h"

/* Start address of non-secure application. */
#define mainNONSECURE_APP_START_ADDRESS		( FMC_NON_SECURE_BASE )

#define LOOP_HERE       0xE7FEE7FF      /* Instruction Code of "B ." */

/* typedef for non-secure Reset Handler. */
typedef void ( *NonSecureResetHandler_t )	( void ) __attribute__( ( cmse_nonsecure_call ) );


/*-----------------------------------------------------------*/

/* Boot into the non-secure code. */
void BootNonSecure( uint32_t u32NonSecureBase );
/*-----------------------------------------------------------*/

void SYS_Init(void)
{
    /*----------------------------------------------------------------------*/
    /* Init System Clock                                                    */
    /*----------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Switch SCLK clock source to HIRC before PLL setting */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_HIRC);
    /* Enable PLL0 200MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_180MHZ, CLK_APLL0_SELECT);    

    /* Switch SCLK clock source to PLL0 and divide 1 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0);

    /* Set HCLK0 divide 1 */
    CLK_SET_HCLK0DIV(1);
    
    /* Set HCLK1 divide 1 */
    CLK_SET_HCLK1DIV(1);
    
    /* Set HCLK2 divide 1 */
    CLK_SET_HCLK2DIV(2);

    /* Set PCLK0/1/2/3/4 divide 2 */
    CLK_SET_PCLK0DIV(2);
    CLK_SET_PCLK1DIV(2);
    CLK_SET_PCLK2DIV(2);
    CLK_SET_PCLK3DIV(2);
    CLK_SET_PCLK4DIV(2);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable UART clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(SCU0_MODULE);
    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);

    /* Select UART clock source from HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HIRC, CLK_UARTDIV0_UART0DIV(1));    

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13();

    /*----------------------------------------------------------------------*/
    /* Enable Non-Secure IP/GPIO clock                                      */
    /* (Config UART1/GPIO to Non-Secure in partition_M55M1.h)          */
    /*----------------------------------------------------------------------*/
    /* Enable FMC0 module clock */
	CLK_EnableModuleClock(FMC0_MODULE);

	/* Enable UART1 module clock */
	CLK_EnableModuleClock(UART1_MODULE);

	/* Enable GPIO module clock */
	CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOD_MODULE);
    CLK_EnableModuleClock(GPIOE_MODULE);
    CLK_EnableModuleClock(GPIOF_MODULE);
    CLK_EnableModuleClock(GPIOG_MODULE);
    CLK_EnableModuleClock(GPIOH_MODULE);
    CLK_EnableModuleClock(GPIOI_MODULE);
    CLK_EnableModuleClock(GPIOJ_MODULE);

	/* Set UART1 TXD/RXD pins for non-secure code */
	CLK_SetModuleClock(UART1_MODULE, CLK_UARTSEL0_UART1SEL_HIRC, CLK_UARTDIV0_UART1DIV(1));
    SET_UART1_RXD_PA2();
    SET_UART1_TXD_PA3();

    /* Lock protected registers */
    SYS_LockReg();
}

void DEBUG_PORT_Init(void)
{
    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);
}

/*
 *	Instructions to Build and Run:
 *	 - The Keil multi-project workspace FreeRTOSDemo.uvmpw contains projects for
 *	   both the secure project, and non secure project.
 *	 - Set the FreeRTOSDemo_s project as Active - Right click on
 *	   "Project: FreeRTOSDemo_s" and select "Set as Active Project".
 *	 - Build the FreeRTOSDemo_s project using "Project --> Build" or by pressing
 *	   "F7".
 *	 - Set the FreeRTOSDemo_ns project as Active - Right click on
 *	   "Project: FreeRTOSDemo_ns" and select "Set as Active Project".
 *	 - Build the FreeRTOSDemo_ns project using "Project --> Build" or by
 *	   pressing "F7".
 *	 - Start Debug Session using "Debug -> Start/Stop Debug Session" or by
 *	   pressing "Ctrl+F5".
 */

/* Secure main() */
int main( void )
{
    SYS_Init();
    /* Configure debug port */
    DEBUG_PORT_Init();

    printf("\nThis sample code test trustzone with FreeRTOS.\n");
    SCB->CCR |= SCB_CCR_BFHFNMIGN_Msk;

    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk |
                  SCB_SHCSR_SECUREFAULTENA_Msk; // enable Usage-/Bus-/MPU-/Secure Fault

    /* Boot the non-secure code. */
	BootNonSecure( mainNONSECURE_APP_START_ADDRESS );

	/* Non-secure software does not return, this code is not executed. */
	for( ; ; )
	{
		/* Should not reach here. */
	}
}
/*-----------------------------------------------------------*/


void BootNonSecure( uint32_t u32NonSecureBase )
{
    NonSecureResetHandler_t pfnNonSecureEntry;

    printf("u32NonSecureBase: 0x%08X\n", u32NonSecureBase);
    printf("NON_SECURE_SRAM_BASE: 0x%08X\n", NON_SECURE_SRAM_BASE);

    /* SCB_NS.VTOR points to the Non-Secure vector table base address. */
    SCB_NS->VTOR = u32NonSecureBase;

    /* 1st Entry in the vector table is the Non-Secure Main Stack Pointer. */
    __TZ_set_MSP_NS(*((uint32_t *)SCB_NS->VTOR));      /* Set up MSP in Non-Secure code */

	
    /* Enable features */
#if (defined(__FPU_USED) && (__FPU_USED == 1U)) || (defined(__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE > 0U))
    SCB_NS->CPACR |= ((3U << 10U * 2U) | /* enable CP10 Full Access */
                      (3U << 11U * 2U)); /* enable CP11 Full Access */
#endif

#ifdef UNALIGNED_SUPPORT_DISABLE
    SCB_NS->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

    // Enable Loop and branch info cache
    SCB_NS->CCR |= SCB_CCR_LOB_Msk;
    __ISB();
	
    /* 2nd entry contains the address of the Reset_Handler (CMSIS-CORE) function */
    pfnNonSecureEntry = ((NonSecureResetHandler_t)(*(((uint32_t *)SCB_NS->VTOR) + 1)));

    /* Clear the LSB of the function address to indicate the function-call
       will cause a state switch from Secure to Non-Secure */
    pfnNonSecureEntry = cmse_nsfptr_create(pfnNonSecureEntry);

    /* Check if the Reset_Handler address is in Non-Secure space */
    if(cmse_is_nsfptr(pfnNonSecureEntry) && (((uint32_t)pfnNonSecureEntry & NS_OFFSET) == NS_OFFSET))
    {
        printf("Execute Non-Secure code at %p ...\n", pfnNonSecureEntry);
        pfnNonSecureEntry(); /* Non-Secure function entry */
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
        pfnNonSecureEntry = (NonSecureResetHandler_t)(NON_SECURE_SRAM_BASE + 1);
        pfnNonSecureEntry();

        while(1);
    }
}
/*-----------------------------------------------------------*/

