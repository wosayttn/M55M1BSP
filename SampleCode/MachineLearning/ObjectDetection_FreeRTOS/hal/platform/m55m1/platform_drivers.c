/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "platform_drivers.h"

#include <string.h>

#if defined(ARM_NPU)
#include "ethosu_npu_init.h"
#endif /* ARM_NPU */

#include "hyperflash_code.h"
#include "hyperram_code.h"

#define HYPERRAM_SPIM_PORT SPIM0
#define HYPERFLASH_SPIM_PORT SPIM1

static void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

//    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);

    /* Waiting for Internal RC clock ready */
//    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

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

    CLK_PCLKDIV_PCLK0DIV(2);
    CLK_PCLKDIV_PCLK1DIV(2);
    CLK_PCLKDIV_PCLK2DIV(2);
    CLK_PCLKDIV_PCLK3DIV(2);
    CLK_PCLKDIV_PCLK4DIV(2);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable UART0 module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable GPIO module clock */
    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOD_MODULE);
    CLK_EnableModuleClock(GPIOE_MODULE);
    CLK_EnableModuleClock(GPIOF_MODULE);
    CLK_EnableModuleClock(GPIOG_MODULE);
    CLK_EnableModuleClock(GPIOH_MODULE);
    CLK_EnableModuleClock(GPIOI_MODULE);
    CLK_EnableModuleClock(GPIOJ_MODULE);

    /* Enable FMC0 module clock to keep FMC clock when CPU idle but NPU running*/
	CLK_EnableModuleClock(FMC0_MODULE);

    /* Enable NPU module clock */
    CLK_EnableModuleClock(NPU0_MODULE);

    /* Enable NPU module clock */
    CLK_EnableModuleClock(CCAP0_MODULE);

	/* Select UART0 module clock source as HIRC and UART0 module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HIRC, CLK_UARTDIV0_UART0DIV(1));

	/* Enable SRAM2 module clock */
	CLK_EnableModuleClock(SRAM2_MODULE);
	/* Enable SPIM module clock */
    CLK_EnableModuleClock(SPIM0_MODULE);
	/* Enable SPIM module clock */
    CLK_EnableModuleClock(SPIM1_MODULE);

	/*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set multi-function pins for UART0 RXD and TXD */
    SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13();

	HyperRAM_PinConfig(HYPERRAM_SPIM_PORT);
	HyperFlash_PinConfig(HYPERFLASH_SPIM_PORT);	
}

static void UART0_Init(void)
{
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

int platform_init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    SYS_Init();
    /* UART init - will enable valid use of printf (stdout
     * re-directed at this UART (UART0) */
    UART0_Init();

    SYS_LockReg();                   /* Unlock register lock protect */

    HyperRAM_Init(HYPERRAM_SPIM_PORT);
    HyperFlash_Init(HYPERFLASH_SPIM_PORT);

    info("%s: complete\n", __FUNCTION__);

#if defined(ARM_NPU)

    int state;

    /* If Arm Ethos-U NPU is to be used, we initialise it here */
    if (0 != (state = arm_ethosu_npu_init())) {
        return state;
    }

#endif /* ARM_NPU */

    /* Print target design info */
    info("Target system design: %s\n", DESIGN_NAME);

    return 0;
}

void platform_release(void)
{
    __disable_irq();
}

void platform_name(char* name, size_t size)
{
    strncpy(name, DESIGN_NAME, size);
}
