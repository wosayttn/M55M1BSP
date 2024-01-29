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
#include "rng_cunit.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

uint32_t au32TestKey[8] =
{
    0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f,
    0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f
};

uint32_t au32TestECC_N[18] =
{
    0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f,
    0x10111213, 0x14151617, 0x18191a1b, 0x1c1d1e1f,
    0x20212223, 0x24252627, 0x28292a2b, 0x2c2d2e2f,
    0x20212233, 0x34353537, 0x38393a3b, 0x3c3d3e3f,
    0x40414243, 0x44454647
};

__attribute__((aligned(4))) uint8_t au8InputData[] =
{
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
};


__attribute__((aligned(4))) uint8_t au8OutputData[16];

uint32_t    au32PrngData[8];

static volatile int  g_PRNG_done;


//-----------------------------------------------------------------------//
int Rng_Tests_Init(void)
{
    NVIC_EnableIRQ(CRYPTO_IRQn);
    return 0;
}

int Rng_Tests_Clean(void)
{
    NVIC_DisableIRQ(CRYPTO_IRQn);
    return 0;
}

void CRYPTO_IRQHandler(void)
{

    if (PRNG_GET_INT_FLAG(CRYPTO))
    {
        g_PRNG_done = 1;
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

CU_SuiteInfo Rng_suites[] =
{
    {"Rng CONST", Rng_Tests_Init, Rng_Tests_Clean, NULL, NULL, Rng_ConstantTests},
    {"Rng MACRO", Rng_Tests_Init, Rng_Tests_Clean, NULL, NULL, Rng_MacroTests},
    {"Rng API", Rng_Tests_Init, Rng_Tests_Clean, NULL, NULL, Rng_ApiTests},
    CU_SUITE_INFO_NULL
};


//---------------------Const-------------------------------------//
void RNG_Const()
{
    CU_ASSERT(PRNG_KEY_SIZE_128 == 0);
    CU_ASSERT(PRNG_KEY_SIZE_163 == 1);
    CU_ASSERT(PRNG_KEY_SIZE_192 == 2);
    CU_ASSERT(PRNG_KEY_SIZE_224 == 3);
    CU_ASSERT(PRNG_KEY_SIZE_233 == 4);
    CU_ASSERT(PRNG_KEY_SIZE_255 == 5);
    CU_ASSERT(PRNG_KEY_SIZE_256 == 6);
    CU_ASSERT(PRNG_KEY_SIZE_283 == 7);
    CU_ASSERT(PRNG_KEY_SIZE_384 == 8);
    CU_ASSERT(PRNG_KEY_SIZE_409 == 9);
    CU_ASSERT(PRNG_KEY_SIZE_512 == 10);
    CU_ASSERT(PRNG_KEY_SIZE_521 == 11);
    CU_ASSERT(PRNG_KEY_SIZE_571 == 12);

    CU_ASSERT(PRNG_SEED_CONT == 0);
    CU_ASSERT(PRNG_SEED_RELOAD == 1);
}
//---------------------Macro-------------------------------------//
void MACRO_PRNG_ENABLE_INT()
{
    PRNG_ENABLE_INT(CRYPTO);

    CU_ASSERT_TRUE((TRNG->STS & (TRNG_STS_DVIF_Msk)) == (TRNG_STS_DVIF_Msk));

    PRNG_DISABLE_INT(CRYPTO);
    CU_ASSERT_FALSE(TRNG->STS & (TRNG_STS_DVIF_Msk));
}


void MACRO_PRNG_INT_FLAG()
{
    PRNG_ENABLE_INT(CRYPTO);

    g_PRNG_done = 0;

    RNG_Open();

    while (!g_PRNG_done);

    CU_ASSERT((CRYPTO->INTSTS & CRYPTO_INTSTS_PRNGIF_Msk) == 1);

    PRNG_CLR_INT_FLAG(CRYPTO);

    CU_ASSERT((CRYPTO->INTSTS & CRYPTO_INTSTS_PRNGIF_Msk) == 0);

    PRNG_DISABLE_INT(CRYPTO);

}
//---------------------API-------------------------------------//
void API_RNG_Open()
{
    RNG_Open();

    CU_ASSERT_TRUE((TRNG->CTL & (TRNG_CTL_LDOEN_Msk | TRNG_CTL_NRST_Msk | TRNG_CTL_TRNGEN_Msk)) == (TRNG_CTL_LDOEN_Msk | TRNG_CTL_NRST_Msk | TRNG_CTL_TRNGEN_Msk));

    CU_ASSERT_TRUE(TRNG->STS & TRNG_STS_TRNGRDY_Msk);

    CU_ASSERT_FALSE(CRYPTO->PRNG_CTL & CRYPTO_PRNG_CTL_BUSY_Msk);

    CU_PASS();
}

void API_RNG_Random()
{

    uint32_t len_set = 1;
    uint32_t len;

    len = RNG_Random(au32TestKey, len_set);

    CU_ASSERT_TRUE((CRYPTO->PRNG_CTL & CRYPTO_PRNG_CTL_KEYSZ_Msk) == (PRNG_KEY_SIZE_256 << CRYPTO_PRNG_CTL_KEYSZ_Pos)) ;

    CU_ASSERT_TRUE(len_set == len);

    CU_PASS();
}

void API_RNG_ECDSA_Init()
{
    int i;

    RNG_ECDSA_Init(1, au32TestECC_N);

    for (i = 0; i < 18; i++)
        CU_ASSERT_TRUE(CRYPTO->ECC_N[i] = au32TestECC_N[i]);

    CU_ASSERT_TRUE((CRYPTO->PRNG_KSCTL) == (KS_OWNER_ECC << CRYPTO_PRNG_KSCTL_OWNER_Pos) |
                   CRYPTO_PRNG_KSCTL_ECDSA_Msk |
                   (CRYPTO_PRNG_KSCTL_WDST_Msk) |
                   (KS_SRAM << CRYPTO_PRNG_KSCTL_WSDST_Pos));
    CU_PASS();
}

void API_RNG_ECDSA()
{

    uint32_t key_num;

    key_num = RNG_ECDSA(PRNG_KEY_SIZE_128);

    CU_ASSERT_TRUE((CRYPTO->PRNG_CTL & (CRYPTO_PRNG_CTL_KEYSZ_Msk | CRYPTO_PRNG_CTL_START_Msk | CRYPTO_PRNG_CTL_SEEDSRC_Msk)) == ((PRNG_KEY_SIZE_128 << CRYPTO_PRNG_CTL_KEYSZ_Pos) |
                   CRYPTO_PRNG_CTL_START_Msk | (0 << CRYPTO_PRNG_CTL_SEEDSRC_Pos)));

    CU_PASS();
}

void API_RNG_ECDH_Init()
{
    int i;
    RNG_ECDH_Init(PRNG_KEY_SIZE_128, au32TestECC_N);

    for (i = 0; i < 18; i++)
        CU_ASSERT_TRUE(CRYPTO->ECC_N[i] = au32TestECC_N[i]);

    CU_ASSERT_TRUE((CRYPTO->PRNG_KSCTL) == (KS_OWNER_ECC << CRYPTO_PRNG_KSCTL_OWNER_Pos) |
                   CRYPTO_PRNG_KSCTL_ECDH_Msk |
                   (CRYPTO_PRNG_KSCTL_WDST_Msk) |
                   (KS_SRAM << CRYPTO_PRNG_KSCTL_WSDST_Pos));
    CU_PASS();
}


void API_RNG_ECDH()
{
    uint32_t key_num;

    key_num = RNG_ECDH(PRNG_KEY_SIZE_128);

    CU_ASSERT_TRUE((CRYPTO->PRNG_CTL & (CRYPTO_PRNG_CTL_KEYSZ_Msk | CRYPTO_PRNG_CTL_START_Msk | CRYPTO_PRNG_CTL_SEEDSRC_Msk)) == ((PRNG_KEY_SIZE_128 << CRYPTO_PRNG_CTL_KEYSZ_Pos) |
                   CRYPTO_PRNG_CTL_START_Msk | (0 << CRYPTO_PRNG_CTL_SEEDSRC_Pos)));

    CU_PASS();
}


//-------------------------CU_TesInfo Table-----------------------//
CU_TestInfo Rng_ConstantTests[] =
{
    {"PRNG CONST", RNG_Const},
    CU_TEST_INFO_NULL
};

CU_TestInfo Rng_MacroTests[] =
{
    {"Enable/Disable RNG data valid interrupt", MACRO_PRNG_ENABLE_INT},
    {"Get RNG data valid interrupt flag", MACRO_PRNG_INT_FLAG},
    CU_TEST_INFO_NULL
};

CU_TestInfo Rng_ApiTests[] =
{
    {"RNG_Open", API_RNG_Open},
    {"RNG_Random", API_RNG_Random},
    {"RNG_Open", API_RNG_ECDSA_Init},
    {"RNG_Random", API_RNG_ECDSA},
    {"RNG_Open", API_RNG_ECDH_Init},
    {"RNG_Random", API_RNG_ECDH},
    CU_TEST_INFO_NULL
};

