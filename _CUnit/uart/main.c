
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "uart_cunit.h"
//#include "../pldm_emu.h"

#ifndef DEBUG_PORT
    #define DEBUG_PORT UART0
#endif

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Enable External RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HXTEN_Msk);

    /* Waiting for External RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);


    /* Enable External RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRC48MEN_Msk);

    /* Waiting for External RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRC48MSTB_Msk);	
	
    /* Enable PLL0 200MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HXT, FREQ_180MHZ, CLK_APLL0_SELECT);

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

    /* Enable UART clock */
    SetDebugUartCLK();
    SYS_ResetModule(SYS_UART0RST);

    /* Enable UART module clock */
 //   CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(UART1_MODULE);
    CLK_EnableModuleClock(UART2_MODULE);
    CLK_EnableModuleClock(UART3_MODULE);
    CLK_EnableModuleClock(UART4_MODULE);
    CLK_EnableModuleClock(UART5_MODULE);
    CLK_EnableModuleClock(UART6_MODULE);
    CLK_EnableModuleClock(UART7_MODULE);
    CLK_EnableModuleClock(UART8_MODULE);
    CLK_EnableModuleClock(UART9_MODULE);

    /* Select UART module clock source as HXT and UART module clock divider as 1 */
//    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HIRC, CLK_UARTDIV0_UART0DIV(1));
    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART1_MODULE, CLK_UARTSEL0_UART1SEL_HIRC, CLK_UARTDIV0_UART1DIV(1));
    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART2_MODULE, CLK_UARTSEL0_UART2SEL_HIRC, CLK_UARTDIV0_UART2DIV(1));
    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART3_MODULE, CLK_UARTSEL0_UART3SEL_HIRC, CLK_UARTDIV0_UART3DIV(1));
    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART4_MODULE, CLK_UARTSEL0_UART4SEL_HIRC, CLK_UARTDIV0_UART4DIV(1));
    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART5_MODULE, CLK_UARTSEL0_UART5SEL_HIRC, CLK_UARTDIV0_UART5DIV(1));
    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART6_MODULE, CLK_UARTSEL0_UART6SEL_HIRC, CLK_UARTDIV0_UART6DIV(1));
    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART7_MODULE, CLK_UARTSEL0_UART7SEL_HIRC, CLK_UARTDIV0_UART7DIV(1));
    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART8_MODULE, CLK_UARTSEL1_UART8SEL_HIRC, CLK_UARTDIV1_UART8DIV(1));
    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART9_MODULE, CLK_UARTSEL1_UART9SEL_HIRC, CLK_UARTDIV1_UART9DIV(1));
    
		SYS_ResetModule(SYS_UART1RST);
    /* Enable GPA module clock */
    CLK_EnableModuleClock(GPIOA_MODULE);
    /* Enable GPB module clock */
    CLK_EnableModuleClock(GPIOB_MODULE);
    /* Enable GPA module clock */
    CLK_EnableModuleClock(GPIOD_MODULE);
    /* Enable GPB module clock */
    CLK_EnableModuleClock(GPIOE_MODULE);
     /* Enable GPA module clock */
    CLK_EnableModuleClock(GPIOG_MODULE);
    /* Enable GPB module clock */
    CLK_EnableModuleClock(GPIOH_MODULE);


   /* Update System Core Clock */

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
//    SetDebugUartMFP();
    SET_UART0_TXD_PA1();
    SET_UART0_RXD_PA0(); 
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
 
    if (CUE_SUCCESS != CU_register_suites(UART_suites))
    {
        fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }

}

int main(int argc, char *argv[])
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init DEBUG_PORT to 115200-8N1 for printf */
//    UART_Open(DEBUG_PORT, 115200);
//    DEBUG_PORT_Init(DEBUG_PORT, 115200);
      InitDebugUart();
    printf("\n\n");
    printf("+--------------------------------------+\n");
    printf("|       M55M1 UART CUnit Test          |\n");
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

    while (1) ;
}
