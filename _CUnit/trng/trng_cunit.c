/**************************************************************************//**
 * @file     trng_cunit.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 15/09/15 4:36p $
 * @brief    M55 series Crypto CUnit test function.
 *
 * @note
 * Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

//* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "trng_cunit.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
__attribute__((aligned(4))) uint8_t au8InputData[] =
{
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
};


__attribute__((aligned(4))) uint8_t au8OutputData[16];

uint32_t    au32PrngData[8];

static volatile int  g_TRNG_done;



//-----------------------------------------------------------------------//
int Trng_Tests_Init(void)
{
    NVIC_EnableIRQ(TRNG_IRQn);
    return 0;
}

int Trng_Tests_Clean(void)
{
    NVIC_DisableIRQ(TRNG_IRQn);
    return 0;
}

void TRNG_IRQHandler(void)
{
    uint32_t c, d;
	
	if(TRNG_GET_INT_FLAG(TRNG))
    {
        c = TRNG->DATA_OUT[0];
	    g_TRNG_done = 1;
	}
 
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

CU_SuiteInfo Trng_suites[] =
{
    {"Trng CONST", Trng_Tests_Init, Trng_Tests_Clean, NULL, NULL, Trng_ConstantTests},
    {"Trng MACRO", Trng_Tests_Init, Trng_Tests_Clean, NULL, NULL, Trng_MacroTests},
    {"Trng API", Trng_Tests_Init, Trng_Tests_Clean, NULL, NULL, Trng_ApiTests},
    CU_SUITE_INFO_NULL
};


//---------------------Const-------------------------------------//
void TRNG_Const()
{
    CU_ASSERT(TRNG_CTL_KATSEL_INSTANTIATION_TESTING == 0);
    CU_ASSERT(TRNG_CTL_KATSEL_RESEED_TESTING == 1);  
	CU_ASSERT(TRNG_CTL_KATSEL_GENERATION_TESTING == 2);
    CU_ASSERT(TRNG_CTL_MODE_OUTPUT_ENTROPY == 0);
    CU_ASSERT(TRNG_CTL_MODE_OUTPUT_NRBG == 1);
    CU_ASSERT(TRNG_CTL_MODE_OUTPUT_DRBG == 2);
}

//---------------------Macro-------------------------------------//
void MACRO_TRNG_ENABLE_INT()
{
    TRNG_ENABLE_INT(TRNG);
    
	CU_ASSERT_TRUE((TRNG->STS & (TRNG_STS_DVIF_Msk)) == (TRNG_STS_DVIF_Msk));

    TRNG_ENABLE_INT(TRNG);
    CU_ASSERT_FALSE(TRNG->STS & (TRNG_STS_DVIF_Msk));
}

void MACRO_TRNG_INT_FLAG()
{
    TRNG_ENABLE_INT(TRNG);
    
    CU_ASSERT_FALSE(TRNG->STS & (TRNG_STS_DVIF_Msk));

    TRNG_Open();
	
	g_TRNG_done = 0;
	
	TRNG->CTL |= TRNG_CTL_START_Msk;
	  
    while (!g_TRNG_done);
	  
	CU_ASSERT((TRNG->STS &TRNG_STS_DVIF_Msk) == 0);
}

//---------------------API-------------------------------------//
void API_TRNG_Open()
{
    TRNG_Open();
	
	CU_ASSERT_TRUE((TRNG->CTL & (TRNG_CTL_LDOEN_Msk|TRNG_CTL_NRST_Msk|TRNG_CTL_TRNGEN_Msk)) == (TRNG_CTL_LDOEN_Msk|TRNG_CTL_NRST_Msk|TRNG_CTL_TRNGEN_Msk));
	
	CU_ASSERT_FALSE(TRNG->STS & TRNG_STS_LDORDY_Msk);
	
	CU_PASS();
}


//-------------------------CU_TesInfo Table-----------------------//
CU_TestInfo Trng_ConstantTests[] =
{
    {"TRNG CONST", TRNG_Const},
    CU_TEST_INFO_NULL
};

CU_TestInfo Trng_MacroTests[] =
{
    {"Enable/Disable TRNG data valid interrupt", MACRO_TRNG_ENABLE_INT},
    {"Get TRNG data valid interrupt flag", MACRO_TRNG_INT_FLAG},		
    CU_TEST_INFO_NULL
};

CU_TestInfo Trng_ApiTests[] =
{
    {"TRNG_Opent", API_TRNG_Open},
	CU_TEST_INFO_NULL
};

