/*---------------------------------------------------------------------------------------------------------*/
/* Winbond Electronics Corporation confidential                                                            */
/*                                                                                                         */
/* Copyright (c) 2007 by Winbond Electronics Corporation                                                   */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   timer_cunit_test.c                                                                                    */
/*            The test function of VIC for CUnit.                                                          */
/* Project:   DA8205                                                                                       */
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
#include "dac_cunit.h"

//#define M252G       0
//#define M258G       1

//#define M25X_VERSION   M258G

#define DAC_MODULE_NUM 2
DAC_T *g_apDACModule[DAC_MODULE_NUM] = {DAC0, DAC1};

/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
const uint16_t sine[] = {2047, 2251, 2453, 2651, 2844, 3028, 3202, 3365, 3515, 3650, 3769, 3871, 3954,
                         4019, 4064, 4088, 4095, 4076, 4040, 3984, 3908, 3813, 3701, 3573, 3429, 3272,
                         3102, 2921, 2732, 2536, 2335, 2132, 1927, 1724, 1523, 1328, 1141,  962,  794,
                         639,  497,  371,  262,  171,   99,   45,   12,    0,    7,   35,   84,  151,
                         238,  343,  465,  602,  754,  919, 1095, 1281, 1475, 1674, 1876
                        };

static uint32_t index = 0;
const uint32_t array_size = sizeof(sine) / sizeof(uint16_t);

extern uint32_t I2C_SetBusClockFreq(I2C_T *i2c, uint32_t u32BusClock);
uint32_t u32UnderRunFlag = 0;



/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int suite_success_init(void)
{
    /* Enable DAC module clock */
    CLK_EnableModuleClock(DAC01_MODULE);
    return 0;
}
int suite_success_clean(void)
{
    CLK_DisableModuleClock(DAC01_MODULE) ;
    return 0;
}


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

CU_SuiteInfo DAC_suites[] =
{
    {"DAC CONST", suite_success_init, suite_success_clean, NULL, NULL, DAC_ConstTests},
    {"DAC API", suite_success_init, suite_success_clean, NULL, NULL, DAC_API_Tests},
    {"DAC API", suite_success_init, suite_success_clean, NULL, NULL, DAC_MACRO_Tests},
    CU_SUITE_INFO_NULL
};

/*---------------------------------------------------------------------------------------------------------*/
/* DAC interrupt handler                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void DAC01_IRQHandler(void)
{
    uint32_t u32DACModule = 0 ;

    if (DAC_GET_DMAUDR_FLAG(DAC0))
    {
        CU_ASSERT(DAC_GET_DMAUDR_FLAG(DAC0) == 1);
        DAC_CLR_DMAUDR_FLAG(DAC0);
        CU_ASSERT(DAC_GET_DMAUDR_FLAG(DAC0) == 0);
        u32UnderRunFlag = 0x55555555;
    }

    if (DAC_GET_DMAUDR_FLAG(DAC1))
    {
        CU_ASSERT(DAC_GET_DMAUDR_FLAG(DAC1) == 1);
        DAC_CLR_DMAUDR_FLAG(DAC1);
        CU_ASSERT(DAC_GET_DMAUDR_FLAG(DAC1) == 0);
        u32UnderRunFlag = 0x55555555;
    }


    if (DAC_GET_INT_FLAG(DAC0, 0))
    {
        /* Clear the DAC conversion complete finish flag */
        DAC_CLR_INT_FLAG(DAC0, 0);
        /* Enable Software trigger to start D/A conversion */
        DAC_START_CONV(DAC0);
    }

    if (DAC_GET_INT_FLAG(DAC1, 0))
    {
        /* Clear the DAC conversion complete finish flag */
        DAC_CLR_INT_FLAG(DAC1, 0);
        /* Enable Software trigger to start D/A conversion */
        DAC_START_CONV(DAC1);
    }

}

void Test_CONST_DAC_CTL(void)
{
    CU_ASSERT(DAC_CTL_LALIGN_RIGHT_ALIGN == 0);
    CU_ASSERT(DAC_CTL_LALIGN_LEFT_ALIGN == 0x400);
    CU_ASSERT(DAC_WRITE_DAT_TRIGGER == 0x00);
    CU_ASSERT(DAC_SOFTWARE_TRIGGER == (0x00 | 0x10));
    CU_ASSERT(DAC_LOW_LEVEL_TRIGGER == (0x0000 | 0x20 | 0x10));
    CU_ASSERT(DAC_HIGH_LEVEL_TRIGGER == (0x1000 | 0x20 | 0x10));
    CU_ASSERT(DAC_FALLING_EDGE_TRIGGER == (0x2000 | 0x20 | 0x10));
    CU_ASSERT(DAC_RISING_EDGE_TRIGGER == (0x3000 | 0x20 | 0x10));
    CU_ASSERT(DAC_TIMER0_TRIGGER == (0x40 | 0x10));
    CU_ASSERT(DAC_TIMER1_TRIGGER == (0x60 | 0x10));
    CU_ASSERT(DAC_TIMER2_TRIGGER == (0x80 | 0x10));
    CU_ASSERT(DAC_TIMER3_TRIGGER == (0xA0 | 0x10));
    CU_ASSERT(DAC_EPWM0_TRIGGER == (0xC0 | 0x10));
    CU_ASSERT(DAC_EPWM1_TRIGGER == (0xE0 | 0x10));
    CU_ASSERT(DAC_TRIGGER_MODE_ENABLE == 0x10);
    CU_ASSERT(DAC_TRIGGER_MODE_DISABLE == 0x00);

}
void ResetPDMA(PDMA_T *psPdma)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    if (psPdma == PDMA0)
        SYS_ResetModule(SYS_PDMA0RST);
    else
        SYS_ResetModule(SYS_PDMA1RST);

    /* Lock protected registers */
    SYS_LockReg();
}


void ResetDAC(uint32_t u32Module)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
    SYS_ResetModule(SYS_DAC01RST);
    /* Lock protected registers */
    SYS_LockReg();
}
void Test_API_DAC_Open_Close(void)
{
    uint32_t u32DACModule = 0 ;

#if 0
    DAC_Open(DAC0, 0, DAC_SOFTWARE_TRIGGER);
    CU_ASSERT((DAC0->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGSEL_Msk) == (0 << DAC_CTL_TRGSEL_Pos)); //SWTRG
    CU_ASSERT((DAC0->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC0, 0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_DACEN_Msk) == 0);
    DAC_Open(DAC1, 0, DAC_SOFTWARE_TRIGGER);
    CU_ASSERT((DAC1->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGSEL_Msk) == (0 << DAC_CTL_TRGSEL_Pos)); //SWTRG
    CU_ASSERT((DAC1->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC1, 0);
#else

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_SOFTWARE_TRIGGER);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGSEL_Msk) == (0 << DAC_CTL_TRGSEL_Pos)); //SWTRG
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

#endif

#if 0
    CU_ASSERT((DAC0->CTL & DAC_CTL_DACEN_Msk) == 0);
    DAC_Open(DAC0, 0, DAC_LOW_LEVEL_TRIGGER);
    CU_ASSERT((DAC0->CTL & DAC_CTL_ETRGSEL_Msk) == (0 << DAC_CTL_ETRGSEL_Pos)); //Low Level trigger
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGSEL_Msk) == (1 << DAC_CTL_TRGSEL_Pos));   //External STDAC
    CU_ASSERT((DAC0->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC0, 0);
    CU_ASSERT((DAC1->CTL & DAC_CTL_DACEN_Msk) == 0);
    DAC_Open(DAC1, 0, DAC_LOW_LEVEL_TRIGGER);
    CU_ASSERT((DAC1->CTL & DAC_CTL_ETRGSEL_Msk) == (0 << DAC_CTL_ETRGSEL_Pos)); //Low Level trigger
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGSEL_Msk) == (1 << DAC_CTL_TRGSEL_Pos));   //External STDAC
    CU_ASSERT((DAC1->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC1, 0);
#else

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACEN_Msk) == 0);
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_LOW_LEVEL_TRIGGER);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_ETRGSEL_Msk) == (0 << DAC_CTL_ETRGSEL_Pos)); //Low Level trigger
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGSEL_Msk) == (1 << DAC_CTL_TRGSEL_Pos));   //External STDAC
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

#endif
#if 0
    DAC_Open(DAC0, 0, DAC_HIGH_LEVEL_TRIGGER);
    CU_ASSERT((DAC0->CTL & DAC_CTL_ETRGSEL_Msk) == (1 << DAC_CTL_ETRGSEL_Pos)); //Hi Level trigger
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGSEL_Msk) == (1 << DAC_CTL_TRGSEL_Pos));   //External STDAC
    CU_ASSERT((DAC0->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC0, 0);
    DAC_Open(DAC1, 0, DAC_HIGH_LEVEL_TRIGGER);
    CU_ASSERT((DAC1->CTL & DAC_CTL_ETRGSEL_Msk) == (1 << DAC_CTL_ETRGSEL_Pos)); //Hi Level trigger
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGSEL_Msk) == (1 << DAC_CTL_TRGSEL_Pos));   //External STDAC
    CU_ASSERT((DAC1->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC1, 0);
#else

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_HIGH_LEVEL_TRIGGER);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_ETRGSEL_Msk) == (1 << DAC_CTL_ETRGSEL_Pos)); //Hi Level trigger
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGSEL_Msk) == (1 << DAC_CTL_TRGSEL_Pos));   //External STDAC
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

#endif
#if 0
    DAC_Open(DAC0, 0, DAC_FALLING_EDGE_TRIGGER);
    CU_ASSERT((DAC0->CTL & DAC_CTL_ETRGSEL_Msk) == (2 << DAC_CTL_ETRGSEL_Pos)); //Falling edge trigger
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGSEL_Msk) == (1 << DAC_CTL_TRGSEL_Pos));   //External STDAC
    CU_ASSERT((DAC0->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC0, 0);
    DAC_Open(DAC1, 1, DAC_FALLING_EDGE_TRIGGER);
    CU_ASSERT((DAC1->CTL & DAC_CTL_ETRGSEL_Msk) == (2 << DAC_CTL_ETRGSEL_Pos)); //Falling edge trigger
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGSEL_Msk) == (1 << DAC_CTL_TRGSEL_Pos));   //External STDAC
    CU_ASSERT((DAC1->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC1, 0);
#else

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_FALLING_EDGE_TRIGGER);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_ETRGSEL_Msk) == (2 << DAC_CTL_ETRGSEL_Pos)); //Falling edge trigger
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGSEL_Msk) == (1 << DAC_CTL_TRGSEL_Pos));   //External STDAC
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

#endif
#if 0
    DAC_Open(DAC0, 0, DAC_RISING_EDGE_TRIGGER);
    CU_ASSERT((DAC0->CTL & DAC_CTL_ETRGSEL_Msk) == (3 << DAC_CTL_ETRGSEL_Pos)); //Rising edge trigger
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGSEL_Msk) == (1 << DAC_CTL_TRGSEL_Pos));   //External STDAC
    CU_ASSERT((DAC0->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC0, 0);
    DAC_Open(DAC1, 1, DAC_RISING_EDGE_TRIGGER);
    CU_ASSERT((DAC1->CTL & DAC_CTL_ETRGSEL_Msk) == (3 << DAC_CTL_ETRGSEL_Pos)); //Rising edge trigger
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGSEL_Msk) == (1 << DAC_CTL_TRGSEL_Pos));   //External STDAC
    CU_ASSERT((DAC1->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC1, 0);
#else

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_RISING_EDGE_TRIGGER);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_ETRGSEL_Msk) == (3 << DAC_CTL_ETRGSEL_Pos)); //Rising edge trigger
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGSEL_Msk) == (1 << DAC_CTL_TRGSEL_Pos));   //External STDAC
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

#endif
#if 0
    DAC_Open(DAC0, 0, DAC_TIMER0_TRIGGER);
    CU_ASSERT((DAC0->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGSEL_Msk) == (2 << DAC_CTL_TRGSEL_Pos)); //Timer0TRG
    CU_ASSERT((DAC0->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC0, 0);
    DAC_Open(DAC1, 1, DAC_TIMER0_TRIGGER);
    CU_ASSERT((DAC1->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGSEL_Msk) == (2 << DAC_CTL_TRGSEL_Pos)); //Timer0TRG
    CU_ASSERT((DAC1->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC1, 0);
#else

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_TIMER0_TRIGGER);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGSEL_Msk) == (2 << DAC_CTL_TRGSEL_Pos)); //Timer0TRG
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

#endif
#if 0
    DAC_Open(DAC0, 0, DAC_TIMER1_TRIGGER);
    CU_ASSERT((DAC0->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGSEL_Msk) == (3 << DAC_CTL_TRGSEL_Pos)); //Timer1TRG
    CU_ASSERT((DAC0->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC0, 0);
    DAC_Open(DAC1, 0, DAC_TIMER1_TRIGGER);
    CU_ASSERT((DAC1->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGSEL_Msk) == (3 << DAC_CTL_TRGSEL_Pos)); //Timer1TRG
    CU_ASSERT((DAC1->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC1, 0);
#else

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_TIMER1_TRIGGER);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGSEL_Msk) == (3 << DAC_CTL_TRGSEL_Pos)); //Timer1TRG
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

#endif
#if 0
    DAC_Open(DAC0, 0, DAC_TIMER2_TRIGGER);
    CU_ASSERT((DAC0->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGSEL_Msk) == (4 << DAC_CTL_TRGSEL_Pos)); //Timer2TRG
    CU_ASSERT((DAC0->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC0, 0);
    DAC_Open(DAC1, 0, DAC_TIMER2_TRIGGER);
    CU_ASSERT((DAC1->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGSEL_Msk) == (4 << DAC_CTL_TRGSEL_Pos)); //Timer2TRG
    CU_ASSERT((DAC1->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC1, 0);
#else

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_TIMER2_TRIGGER);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGSEL_Msk) == (4 << DAC_CTL_TRGSEL_Pos)); //Timer2TRG
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

#endif
#if 0
    DAC_Open(DAC0, 0, DAC_TIMER3_TRIGGER);
    CU_ASSERT((DAC0->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC0->CTL & DAC_CTL_TRGSEL_Msk) == (5 << DAC_CTL_TRGSEL_Pos)); //Timer3TRG
    CU_ASSERT((DAC0->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC0, 0);
    DAC_Open(DAC1, 0, DAC_TIMER3_TRIGGER);
    CU_ASSERT((DAC1->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
    CU_ASSERT((DAC1->CTL & DAC_CTL_TRGSEL_Msk) == (5 << DAC_CTL_TRGSEL_Pos)); //Timer3TRG
    CU_ASSERT((DAC1->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
    DAC_Close(DAC1, 0);
#else

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_TIMER3_TRIGGER);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGSEL_Msk) == (5 << DAC_CTL_TRGSEL_Pos)); //Timer3TRG
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

#endif
#if 1 //TC8237 DAC is not supported PWM trigger

    for (u32DACModule = 0 ; u32DACModule ; u32DACModule++)
    {
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_EPWM0_TRIGGER);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGSEL_Msk) == (6 << DAC_CTL_TRGSEL_Pos)); //PWM0TRG
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

    for (u32DACModule = 0 ; u32DACModule ; u32DACModule++)
    {
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_EPWM1_TRIGGER);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_ETRGSEL_Msk) == 0);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGEN_Msk) == DAC_CTL_TRGEN_Msk);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_TRGSEL_Msk) == (7 << DAC_CTL_TRGSEL_Pos)); //PWM1TRG
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACEN_Msk) == DAC_CTL_DACEN_Msk);
        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

#endif

}

void Test_API_DAC_DelayTime(void)
{
    int i, u32DACModule;
#if 0
    DAC_Open(DAC0, 0, DAC_SOFTWARE_TRIGGER);

    //Note: 160M, DAC0->TCTL MAX value is 6
    for (i = 0; i < 6; i++)
    {
        CU_ASSERT(DAC_SetDelayTime(DAC0, i) == i);
    }

    DAC_Close(DAC0, 0);
    DAC_Open(DAC1, 0, DAC_SOFTWARE_TRIGGER);

    //Note: 160M, DAC0->TCTL MAX value is 6
    for (i = 0; i < 6; i++)
    {
        CU_ASSERT(DAC_SetDelayTime(DAC1, i) == i);
    }

    DAC_Close(DAC1, 0);
#else

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_SOFTWARE_TRIGGER);

        for (i = 0; i < 6; i++)
        {
            CU_ASSERT(DAC_SetDelayTime(g_apDACModule[u32DACModule], i) == i);
        }

        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

#endif
}

void Test_MACRO_DAC_Align(void)
{
    uint32_t u32DACModule ;
#if 0
    DAC_Open(DAC0, 0, DAC_SOFTWARE_TRIGGER);
    DAC_ENABLE_LEFT_ALIGN(DAC0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_LALIGN_Msk) == DAC_CTL_LALIGN_Msk);
    DAC_ENABLE_RIGHT_ALIGN(DAC0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_LALIGN_Msk) == 0);
    DAC_Close(DAC0, 0);
    DAC_Open(DAC1, 0, DAC_SOFTWARE_TRIGGER);
    DAC_ENABLE_LEFT_ALIGN(DAC1);
    CU_ASSERT((DAC1->CTL & DAC_CTL_LALIGN_Msk) == DAC_CTL_LALIGN_Msk);
    DAC_ENABLE_RIGHT_ALIGN(DAC1);
    CU_ASSERT((DAC1->CTL & DAC_CTL_LALIGN_Msk) == 0);
    DAC_Close(DAC1, 0);
#else

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_SOFTWARE_TRIGGER);
        DAC_ENABLE_LEFT_ALIGN(g_apDACModule[u32DACModule]);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_LALIGN_Msk) == DAC_CTL_LALIGN_Msk);
        DAC_ENABLE_RIGHT_ALIGN(g_apDACModule[u32DACModule]);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_LALIGN_Msk) == 0);
        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

#endif
}

void Test_MACRO_DAC_BYPASS_BUF(void)
{

#if 0
    DAC_Open(DAC0, 0, DAC_SOFTWARE_TRIGGER);
    DAC_ENABLE_BYPASS_BUFFER(DAC0);
    CU_ASSERT(((DAC0->CTL)&DAC_CTL_BYPASS_Msk) == DAC_CTL_BYPASS_Msk);
    DAC_DISABLE_BYPASS_BUFFER(DAC0);
    CU_ASSERT(((DAC0->CTL)&DAC_CTL_BYPASS_Msk) == 0);
    DAC_Close(DAC0, 0);
    DAC_Open(DAC1, 0, DAC_SOFTWARE_TRIGGER);
    DAC_ENABLE_BYPASS_BUFFER(DAC1);
    CU_ASSERT(((DAC1->CTL)&DAC_CTL_BYPASS_Msk) == DAC_CTL_BYPASS_Msk);
    DAC_DISABLE_BYPASS_BUFFER(DAC1);
    CU_ASSERT(((DAC1->CTL)&DAC_CTL_BYPASS_Msk) == 0);
    DAC_Close(DAC1, 0);
#else
    uint32_t u32DACModule ;

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_SOFTWARE_TRIGGER);
        DAC_ENABLE_BYPASS_BUFFER(g_apDACModule[u32DACModule]);
        CU_ASSERT(((g_apDACModule[u32DACModule]->CTL)&DAC_CTL_BYPASS_Msk) == DAC_CTL_BYPASS_Msk);
        DAC_DISABLE_BYPASS_BUFFER(g_apDACModule[u32DACModule]);
        CU_ASSERT(((g_apDACModule[u32DACModule]->CTL)&DAC_CTL_BYPASS_Msk) == 0);
        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

#endif
}

void Test_MACRO_DAC_DMA_INT(void)
{
#if 0
    DAC_Open(DAC0, 0, DAC_SOFTWARE_TRIGGER);

    CU_ASSERT((DAC0->CTL & DAC_CTL_DMAURIEN_Msk) == 0);
    DAC_ENABLE_DMAUDR_INT(DAC0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_DMAURIEN_Msk) == DAC_CTL_DMAURIEN_Msk);
    DAC_DISABLE_DMAUDR_INT(DAC0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_DMAURIEN_Msk) == 0);

    CU_ASSERT((DAC0->CTL & DAC_CTL_DMAEN_Msk) == 0);
    DAC_ENABLE_PDMA(DAC0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_DMAEN_Msk) == DAC_CTL_DMAEN_Msk);
    DAC_DISABLE_PDMA(DAC0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_DMAEN_Msk) == 0);

    CU_ASSERT((DAC0->CTL & DAC_CTL_DACIEN_Msk) == 0);
    DAC_ENABLE_INT(DAC0, 0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_DACIEN_Msk) == DAC_CTL_DACIEN_Msk);
    DAC_DISABLE_INT(DAC0, 0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_DACIEN_Msk) == 0);

    DAC_Close(DAC0, 0);

    DAC_Open(DAC1, 0, DAC_SOFTWARE_TRIGGER);

    CU_ASSERT((DAC1->CTL & DAC_CTL_DMAURIEN_Msk) == 0);
    DAC_ENABLE_DMAUDR_INT(DAC1);
    CU_ASSERT((DAC1->CTL & DAC_CTL_DMAURIEN_Msk) == DAC_CTL_DMAURIEN_Msk);
    DAC_DISABLE_DMAUDR_INT(DAC1);
    CU_ASSERT((DAC1->CTL & DAC_CTL_DMAURIEN_Msk) == 0);

    CU_ASSERT((DAC1->CTL & DAC_CTL_DMAEN_Msk) == 0);
    DAC_ENABLE_PDMA(DAC1);
    CU_ASSERT((DAC1->CTL & DAC_CTL_DMAEN_Msk) == DAC_CTL_DMAEN_Msk);
    DAC_DISABLE_PDMA(DAC1);
    CU_ASSERT((DAC1->CTL & DAC_CTL_DMAEN_Msk) == 0);

    CU_ASSERT((DAC1->CTL & DAC_CTL_DACIEN_Msk) == 0);
    DAC_ENABLE_INT(DAC1, 0);
    CU_ASSERT((DAC1->CTL & DAC_CTL_DACIEN_Msk) == DAC_CTL_DACIEN_Msk);
    DAC_DISABLE_INT(DAC1, 0);
    CU_ASSERT((DAC1->CTL & DAC_CTL_DACIEN_Msk) == 0);

    DAC_Close(DAC1, 0);
#else
    uint32_t u32DACModule ;

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_SOFTWARE_TRIGGER);

        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DMAURIEN_Msk) == 0);
        DAC_ENABLE_DMAUDR_INT(g_apDACModule[u32DACModule]);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DMAURIEN_Msk) == DAC_CTL_DMAURIEN_Msk);
        DAC_DISABLE_DMAUDR_INT(g_apDACModule[u32DACModule]);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DMAURIEN_Msk) == 0);

        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DMAEN_Msk) == 0);
        DAC_ENABLE_PDMA(g_apDACModule[u32DACModule]);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DMAEN_Msk) == DAC_CTL_DMAEN_Msk);
        DAC_DISABLE_PDMA(g_apDACModule[u32DACModule]);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DMAEN_Msk) == 0);

        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACIEN_Msk) == 0);
        DAC_ENABLE_INT(g_apDACModule[u32DACModule], 0);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACIEN_Msk) == DAC_CTL_DACIEN_Msk);
        DAC_DISABLE_INT(g_apDACModule[u32DACModule], 0);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DACIEN_Msk) == 0);
        DAC_Close(g_apDACModule[u32DACModule], 0);
        ResetDAC(u32DACModule) ;
    }

#endif
}

void Test_MACRO_DAC_RW_Data(void)
{
#if 0
    index = 0;
    NVIC_DisableIRQ(DAC_IRQn);
    DAC_Open(DAC0, 0, DAC_SOFTWARE_TRIGGER);
    DAC_SetDelayTime(DAC0, 6);

    DAC_WRITE_DATA(DAC0, 0, sine[index]);
    CU_ASSERT(DAC0->DAT == sine[index]);
    CU_ASSERT(DAC_READ_DATA(DAC0, 0) == sine[index]);
    DAC_CLR_INT_FLAG(DAC0, 0);
    CU_ASSERT((DAC0->STATUS & DAC_STATUS_FINISH_Msk) == 0);
    DAC_START_CONV(DAC0);

    /* Below test code maybe cannot get SWTRG=1, it is too fast */
    //CU_ASSERT((DAC0->SWTRG & DAC_SWTRG_SWTRG_Msk)==DAC_SWTRG_SWTRG_Msk);

    CU_ASSERT(DAC_IS_BUSY(DAC0, 0) == 1);
    CU_ASSERT((DAC0->STATUS & DAC_STATUS_BUSY_Msk) == DAC_STATUS_BUSY_Msk);

    while (DAC_GET_INT_FLAG(DAC0, 0) == 0);

    CU_ASSERT(DAC_GET_INT_FLAG(DAC0, 0) == DAC_STATUS_FINISH_Msk);
    CU_ASSERT((DAC0->STATUS & DAC_STATUS_FINISH_Msk) == DAC_STATUS_FINISH_Msk);
    CU_ASSERT((DAC0->SWTRG & DAC_SWTRG_SWTRG_Msk) == 0);

    DAC_CLR_INT_FLAG(DAC0, 0);
    CU_ASSERT((DAC0->STATUS & DAC_STATUS_FINISH_Msk) == 0);

    DAC_Close(DAC0, 0);
    SYS_ResetModule(DAC_RST);
#else
    uint32_t u32DACModule ;

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        index = 0;
        NVIC_DisableIRQ(DAC01_IRQn);
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_SOFTWARE_TRIGGER);
        DAC_SetDelayTime(g_apDACModule[u32DACModule], 6);

        DAC_WRITE_DATA(g_apDACModule[u32DACModule], 0, sine[index]);
        CU_ASSERT(g_apDACModule[u32DACModule]->DAT == sine[index]);
        CU_ASSERT(DAC_READ_DATA(g_apDACModule[u32DACModule], 0) == sine[index]);
        DAC_CLR_INT_FLAG(g_apDACModule[u32DACModule], 0);
        CU_ASSERT((g_apDACModule[u32DACModule]->STATUS & DAC_STATUS_FINISH_Msk) == 0);
        DAC_START_CONV(g_apDACModule[u32DACModule]);

        /* Below test code maybe cannot get SWTRG=1, it is too fast */
        //CU_ASSERT((DAC0->SWTRG & DAC_SWTRG_SWTRG_Msk)==DAC_SWTRG_SWTRG_Msk);

        CU_ASSERT(DAC_IS_BUSY(g_apDACModule[u32DACModule], 0) == 1);
        CU_ASSERT((g_apDACModule[u32DACModule]->STATUS & DAC_STATUS_BUSY_Msk) == DAC_STATUS_BUSY_Msk);

        while (DAC_GET_INT_FLAG(g_apDACModule[u32DACModule], 0) == 0) {};

        CU_ASSERT(DAC_GET_INT_FLAG(g_apDACModule[u32DACModule], 0) == DAC_STATUS_FINISH_Msk);

        CU_ASSERT((g_apDACModule[u32DACModule]->STATUS & DAC_STATUS_FINISH_Msk) == DAC_STATUS_FINISH_Msk);

        CU_ASSERT((g_apDACModule[u32DACModule]->SWTRG & DAC_SWTRG_SWTRG_Msk) == 0);

        DAC_CLR_INT_FLAG(g_apDACModule[u32DACModule], 0);

        CU_ASSERT((g_apDACModule[u32DACModule]->STATUS & DAC_STATUS_FINISH_Msk) == 0);

        DAC_Close(g_apDACModule[u32DACModule], 0);

        ResetDAC(u32DACModule) ;
    }

#endif
}

void Test_MACRO_DAC_SW_PDMA(void)
{

    uint32_t u32ExitCnt;
    uint32_t u32DACModule ;

    for (u32DACModule = 0 ; u32DACModule < DAC_MODULE_NUM ; u32DACModule++)
    {
        /* Reset DAC module */
        ResetDAC(u32DACModule) ;
        /* Enable PDMA clock source */
        CLK_EnableModuleClock(PDMA0_MODULE);
        /* Reset PDMA module */
        ResetPDMA(PDMA0);
        /* Open PDMA Channel 0 */
        PDMA_Open(PDMA0, 0x1);
        /* Set transfer data width, and tranfer count */
        PDMA_SetTransferCnt(PDMA0, 0, PDMA_WIDTH_16, array_size);
        /* transfer width is one word(32 bit) */
        PDMA_SetTransferAddr(PDMA0, 0, (uint32_t)&sine[index], PDMA_SAR_INC, (uint32_t)&g_apDACModule[u32DACModule]->DAT, PDMA_DAR_FIX);


        if (u32DACModule == 0)
            /* Select channel 0 request source from DAC */
            PDMA_SetTransferMode(PDMA0, 0, PDMA_DAC0_TX, FALSE, 0);
        else
            /* Select channel 0 request source from DAC */
            PDMA_SetTransferMode(PDMA0, 0, PDMA_DAC1_TX, FALSE, 0);


        /* Set transfer type and burst size */
        PDMA_SetBurstType(PDMA0, 0, PDMA_REQ_SINGLE, PDMA_BURST_128);
        /* Set the software trigger,enable DAC even trigger mode and enable D/A converter */
        DAC_Open(g_apDACModule[u32DACModule], 0, DAC_SOFTWARE_TRIGGER);
        /* The DAC conversion settling time is 8us */
        DAC_SetDelayTime(g_apDACModule[u32DACModule], 6);
        /* Clear the DAC conversion complete finish flag for safe */
        DAC_CLR_INT_FLAG(g_apDACModule[u32DACModule], 0);
        /* Enable the PDMA Mode */
        DAC_ENABLE_PDMA(g_apDACModule[u32DACModule]);
        /* Enable the DAC interrupt.  */
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DMAURIEN_Msk) == 0);
        DAC_ENABLE_DMAUDR_INT(g_apDACModule[u32DACModule]);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DMAURIEN_Msk) == DAC_CTL_DMAURIEN_Msk);
        DAC_ENABLE_INT(g_apDACModule[u32DACModule], 0);
        NVIC_EnableIRQ(DAC01_IRQn);

        CLK_SysTickDelay(1000);
        u32ExitCnt = 0;
        u32UnderRunFlag = 0;
        /* Enable Software trigger to start D/A conversion */
        DAC_START_CONV(g_apDACModule[u32DACModule]);

        while (1)
        {
            if (PDMA_GET_TD_STS(PDMA0) == 0x1)
            {
                u32ExitCnt++;
                /* Re-Set transfer count and basic operation mode */
                PDMA_SetTransferCnt(PDMA0, 0, PDMA_WIDTH_16, array_size);
                PDMA_SetTransferMode(PDMA0, 0, PDMA_DAC0_TX, FALSE, 0);

                /* Clear PDMA channel 0 transfer done flag */
                PDMA_CLR_TD_FLAG(PDMA0, 0x1);
            }
            else
            {
                DAC_START_CONV(g_apDACModule[u32DACModule]);
            }

            if ((u32ExitCnt > 100) | (u32UnderRunFlag == 0x55555555))
            {
                break;
            }
        }

        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DMAURIEN_Msk) == DAC_CTL_DMAURIEN_Msk);
        DAC_DISABLE_DMAUDR_INT(g_apDACModule[u32DACModule]);
        CU_ASSERT((g_apDACModule[u32DACModule]->CTL & DAC_CTL_DMAURIEN_Msk) == 0);
        /* Disable External Interrupt */
        NVIC_DisableIRQ(DAC01_IRQn);

        /* Reset PDMA module */
        ResetPDMA(PDMA0);

        /* Reset DAC module */
        ResetDAC(u32DACModule) ;

        /* Disable PDMA IP clock */
        CLK_DisableModuleClock(PDMA0_MODULE);
    }


}

void Test_MACRO_DAC_GROUP_MODE(void)
{
    // TC8237 DAC is not supported this function
    DAC_ENABLE_GROUP_MODE(DAC0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_GRPEN_Msk) == DAC_CTL_GRPEN_Msk);
    DAC_DISABLE_GROUP_MODE(DAC0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_GRPEN_Msk) == 0);
    DAC_ENABLE_GROUP_MODE(DAC0);
    DAC_GROUP_WRITE_DATA(0xAAAA, 0x5555);
    CU_ASSERT((DAC0->CTL & DAC_CTL_GRPEN_Msk) == DAC_CTL_GRPEN_Msk);
    CU_ASSERT((DAC0->GRPDAT & DAC_GPRDAT_DAC1DAT_Msk) == 0x55550000);
    CU_ASSERT((DAC0->GRPDAT & DAC_GPRDAT_DAC0DAT_Msk) == 0xAAAA);
    DAC_GROUP_WRITE_DATA(0x0, 0x0);
    CU_ASSERT((DAC0->GRPDAT & DAC_GPRDAT_DAC1DAT_Msk) == 0x0);
    CU_ASSERT((DAC0->GRPDAT & DAC_GPRDAT_DAC0DAT_Msk) == 0x0);
    DAC_DISABLE_GROUP_MODE(DAC0);
    CU_ASSERT((DAC0->CTL & DAC_CTL_GRPEN_Msk) == 0);
    DAC_GROUP_WRITE_DATA(0x5555, 0xAAAA);
    CU_ASSERT((DAC0->GRPDAT & DAC_GPRDAT_DAC1DAT_Msk) == 0x0);
    CU_ASSERT((DAC0->GRPDAT & DAC_GPRDAT_DAC0DAT_Msk) == 0x0);

    DAC_ENABLE_GROUP_MODE(DAC1);
    CU_ASSERT((DAC0->CTL & DAC_CTL_GRPEN_Msk) == DAC_CTL_GRPEN_Msk);
    DAC_DISABLE_GROUP_MODE(DAC1);
    CU_ASSERT((DAC0->CTL & DAC_CTL_GRPEN_Msk) == 0);
    DAC_ENABLE_GROUP_MODE(DAC1);
    DAC_GROUP_WRITE_DATA(0x5555, 0xAAAA);
    CU_ASSERT((DAC0->CTL & DAC_CTL_GRPEN_Msk) == DAC_CTL_GRPEN_Msk);
    CU_ASSERT((DAC0->GRPDAT & DAC_GPRDAT_DAC1DAT_Msk) == 0xAAAA0000);
    CU_ASSERT((DAC0->GRPDAT & DAC_GPRDAT_DAC0DAT_Msk) == 0x5555);
    DAC_GROUP_WRITE_DATA(0x0, 0x0);
    CU_ASSERT((DAC0->GRPDAT & DAC_GPRDAT_DAC1DAT_Msk) == 0x0);
    CU_ASSERT((DAC0->GRPDAT & DAC_GPRDAT_DAC0DAT_Msk) == 0x0);
    DAC_DISABLE_GROUP_MODE(DAC1);
    CU_ASSERT((DAC0->CTL & DAC_CTL_GRPEN_Msk) == 0);
    DAC_GROUP_WRITE_DATA(0x5555, 0xAAAA);
    CU_ASSERT((DAC0->GRPDAT & DAC_GPRDAT_DAC1DAT_Msk) == 0x0);
    CU_ASSERT((DAC0->GRPDAT & DAC_GPRDAT_DAC0DAT_Msk) == 0x0);



}


CU_TestInfo DAC_ConstTests[] =
{
    {" 1: CONST DAC_CTL.", Test_CONST_DAC_CTL},
    CU_TEST_INFO_NULL
};

CU_TestInfo DAC_API_Tests[] =
{
    {" 1: API DAC_Open&Close", Test_API_DAC_Open_Close},
    {" 2: API DAC_DelayTime", Test_API_DAC_DelayTime},
    CU_TEST_INFO_NULL
};

CU_TestInfo DAC_MACRO_Tests[] =
{
    {" 1: MACRO DAC_Open&Close", Test_MACRO_DAC_Align},
    {" 2: MACRO DAC_BPASS_BUF", Test_MACRO_DAC_BYPASS_BUF},
    {" 3: MACRO DAC_INT", Test_MACRO_DAC_DMA_INT},
    {" 4: MACRO DAC_Read/Write", Test_MACRO_DAC_RW_Data},
    {" 5: MACRO DAC_SW_PDMA", Test_MACRO_DAC_SW_PDMA},
    {" 6: MACRO Group Mode", Test_MACRO_DAC_GROUP_MODE},

    CU_TEST_INFO_NULL
};
