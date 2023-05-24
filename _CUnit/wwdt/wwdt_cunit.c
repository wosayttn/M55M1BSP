/*---------------------------------------------------------------------------------------------------------*/
/* Winbond Electronics Corporation confidential                                                            */
/*                                                                                                         */
/* Copyright (c) 2016 by Nuvoton Technology Corporation                                                    */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   wwdt_cunit.c                                                                                          */
/*            The test function of WWDT for CUnit.                                                         */
/* Project:   M2355                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "wwdt_cunit.h"


#ifdef _WWDT0
#define WWDT WWDT0
#define WWDT_IRQHandler WWDT0_IRQHandler
#else
#define WWDT WWDT1
#define WWDT_IRQHandler WWDT1_IRQHandler
#endif

extern unsigned int STATE0; 
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int suite_success_init(void) { return 0; }
int suite_success_clean(void) { return 0; }

/*---------------------------------------------------------------------------------------------------------*/
/* Function: function_name                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               param - [in/out], parameter_description                                                   */
/*               param - [in/out], parameter_description                                                   */
/*                                                                                                         */
/* Returns:      the_value                                                                                 */
/* Side effects: the_effect                                                                                */
/* Description:                                                                                            */
/*               description                                                                               */
/*---------------------------------------------------------------------------------------------------------*/

CU_SuiteInfo suites[] =
{
    {"WWDT API", suite_success_init, suite_success_clean, NULL, NULL, WWDT_API},
    CU_SUITE_INFO_NULL
};

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
__attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
void Timer0_Init(uint32_t u32ClkSrc)
{
    if(u32ClkSrc == CLK_TMRSEL_TMR0SEL_LIRC) {        
        //CLK->CLKSEL1 = (CLK->CLKSEL1&~CLK_CLKSEL1_TMR0SEL_Msk) | u32ClkSrc;
        CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_LIRC, 0);
    } else {
        //CLK->CLKSEL1 = (CLK->CLKSEL1&~CLK_CLKSEL1_TMR0SEL_Msk) | CLK_CLKSEL1_TMR0SEL_HXT;
        CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC, 0);
    }
    
    CLK_EnableModuleClock(TMR0_MODULE);
    TIMER0->CNT = 0;
    TIMER0->CMP = 0xFFFFFF;
    if(u32ClkSrc == CLK_TMRSEL_TMR0SEL_LIRC) {
        /* Enable LIRC */
        CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);

        /* Waiting for clock ready */
        CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);
        TIMER0->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk;
    } else {
        TIMER0->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk | (12-1);
    }    
}
#else
extern void Timer0_Init(uint32_t u32ClkSrc);
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
__attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
void Timer0_ResetrCounter(void)
{
    TIMER0->CNT = 0UL;
}
#else
extern void Timer0_ResetrCounter(void);
#endif

volatile uint8_t gu8IntFlag;
volatile uint32_t gu32TDRTicks, gu32WWDTCnt;
void WWDT_IRQHandler(void)
{
#if _WWDT0    
    CLK_WaitModuleClockReady(WWDT0_MODULE);
#else
    CLK_WaitModuleClockReady(WWDT1_MODULE);
#endif 
    if(WWDT_GET_INT_FLAG(WWDT) == 1) {
        
        gu32TDRTicks = TIMER0->CNT;
        gu8IntFlag = 1;
        
        gu32WWDTCnt = WWDT_GET_COUNTER(WWDT);
        
        /* Clear WWDT compare match interrupt flag */
        WWDT_CLEAR_INT_FLAG(WWDT);
        WWDT_RELOAD_COUNTER(WWDT);
    }
}

const uint32_t gu32PeriodSel[] = {
    WWDT_PRESCALER_1,
    WWDT_PRESCALER_2,
    WWDT_PRESCALER_4,
    WWDT_PRESCALER_8,
    WWDT_PRESCALER_16,
    WWDT_PRESCALER_32,
    WWDT_PRESCALER_64,
    WWDT_PRESCALER_128,
    WWDT_PRESCALER_192,
    WWDT_PRESCALER_256,
    WWDT_PRESCALER_384,
    WWDT_PRESCALER_512,
    WWDT_PRESCALER_768,
    WWDT_PRESCALER_1024,
    WWDT_PRESCALER_1536,
    WWDT_PRESCALER_2048,
};

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
__attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
#define TEST_MARKER         (0x20003FFC)
void API_WWDT_Open(void) 
{
    volatile uint32_t u32Timeout = 0;
    uint32_t u32MarkerCnt;
            
    /* Enable LIRC */
    CLK_EnableXtalRC(CLK_SRCCTL_LIRCEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);
    
#if _WWDT0    
    CLK_SetModuleClock(WWDT0_MODULE, CLK_WWDTSEL_WWDT0SEL_LIRC, 0);
    
    CLK_EnableModuleClock(WWDT0_MODULE);
#else
    CLK_SetModuleClock(WWDT1_MODULE, CLK_WWDTSEL_WWDT1SEL_LIRC, 0);
    
    CLK_EnableModuleClock(WWDT1_MODULE);
#endif
    if(WWDT_GET_RESET_FLAG(WWDT)) {
        printf("\nLast status. [WWDT reset occurred]\n\n");
        WWDT_CLEAR_RESET_FLAG(WWDT);
        if(WWDT_GET_RESET_FLAG(WWDT) != 0)
            CU_FAIL("API_WWDT_Open - Clear RESET flag fail");
    } else {
        printf("\nLast status. [No WWDT reset]\n\n");
    }        
    UART_WAIT_TX_EMPTY(DEBUG_PORT);
    
    u32MarkerCnt = inpw(TEST_MARKER);
    if(u32MarkerCnt > 15) {
        u32MarkerCnt = 0;
    } else {
        u32MarkerCnt++;
    }
    outpw(TEST_MARKER, u32MarkerCnt);
    printf("[Current Test marker is %d. Please hit [R]un to check test result when Test marker is 7.]\n", u32MarkerCnt);
    UART_WAIT_TX_EMPTY(DEBUG_PORT);
    
    if(u32MarkerCnt == 7) {
        /* Enable WWDT NVIC */
#if _WWDT0        
        NVIC_EnableIRQ(WWDT0_IRQn);
#else
        NVIC_EnableIRQ(WWDT1_IRQn);
#endif        
        Timer0_Init(CLK_TMRSEL_TMR0SEL_HIRC);
        Timer0_ResetrCounter();
        
        /* If WWDT clock source is HCLK/2048 (72 MHz/2048), prescaler 128 ... period time is around 7281 us */
        /* If WWDT clock source is LIRC, prescaler 128 ... period time is around 25.6 ms */
        WWDT_Open(WWDT,WWDT_PRESCALER_128, 0x3E, TRUE);
        
        gu8IntFlag = 0;
        while(gu8IntFlag == 0) {
            if((TIMER0->CNT > __HXT/12) || (u32Timeout > SystemCoreClock/100)) {
                
                /* Disable WWDT module clock */
#if _WWDT0                
                CLK->WWDTCTL &= ~(CLK_WWDTCTL_WWDT0CKEN_Msk);
#else
                CLK->WWDTCTL &= ~(CLK_WWDTCTL_WWDT1CKEN_Msk);
#endif                
                CU_FAIL("API_WWDT_Open INT flag fail");
                return ;
            }         
            
            u32Timeout++;
        }
        
        if((gu32WWDTCnt == 0x3E) || (gu32WWDTCnt == 0x3D)) {
            ;
        } else {
#if _WWDT0                
                CLK->WWDTCTL &= ~(CLK_WWDTCTL_WWDT0CKEN_Msk);
#else
                CLK->WWDTCTL &= ~(CLK_WWDTCTL_WWDT1CKEN_Msk);
#endif 
            CU_FAIL("API_WWDT_Open Counter fail");
            return ;
        }
        
//        if((gu32TDRTicks > (25600+(2560*2))) || (gu32TDRTicks < (25600-(2560*2)))) {
        if((gu32TDRTicks > (8000+(800*2))) || (gu32TDRTicks < (8000-(800*2)))) {
#if _WWDT0                
                CLK->WWDTCTL &= ~(CLK_WWDTCTL_WWDT0CKEN_Msk);
#else
                CLK->WWDTCTL &= ~(CLK_WWDTCTL_WWDT1CKEN_Msk);
#endif 
            CU_FAIL("API_WWDT_Open period time fail");
            return ;
        }
    } else {
        WWDT_Open(WWDT,gu32PeriodSel[u32MarkerCnt], 0x20, FALSE);
        WWDT_RELOAD_COUNTER(WWDT);
    }    
}
#else
extern void API_WWDT_Open(void);
#endif

CU_TestInfo WWDT_API[] =
{
    {"Check WWDT_Open API with Interrupt Status and Reload/Get Counter MACRO ", API_WWDT_Open}, 
    CU_TEST_INFO_NULL
};
