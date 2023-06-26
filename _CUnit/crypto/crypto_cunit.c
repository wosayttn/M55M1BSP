/**************************************************************************//**
 * @file     crypto_cunit.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 15/09/15 4:36p $
 * @brief    NUC4xx series Crypto CUnit test function.
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
#include "crypto_cunit.h"


extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

uint32_t au32TestKey[8] =
{
    0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f,
    0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f
};

uint32_t au32TestIV[4] =
{
    0x00000000, 0x00000000, 0x00000000, 0x00000000
};

__attribute__((aligned(4))) uint8_t au8InputData[] =
{
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
};



/* TDES Key:  1e4678a17f2c8a33 800e15ac47891a4c a011453291c23340 */
uint32_t au8TDESKey[3][2] =
{
    { 0x1e4678a1, 0x7f2c8a33 },
    { 0x800e15ac, 0x47891a4c },
    { 0xa0114532, 0x91c23340 }
};


__attribute__((aligned(4))) uint8_t au8OutputData[16];

uint32_t    au32PrngData[8];

static volatile int  g_AES_done;
static volatile int  g_CHA_done;


uint8_t au8TestKeyCHA[32] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

uint8_t au8TestNonceCHA[12] =
{
    0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
	0x00, 0x4a, 0x00, 0x00, 0x00, 0x00
};

//-----------------------------------------------------------------------//
int Crypto_Tests_Init(void)
{
    NVIC_EnableIRQ(CRYPTO_IRQn);
    return 0;
}

int Crypto_Tests_Clean(void)
{
    NVIC_DisableIRQ(CRYPTO_IRQn);
    return 0;
}


void CRYPTO_IRQHandler()
{
    if (AES_GET_INT_FLAG(CRYPTO))
    {
        CU_ASSERT_TRUE(CRYPTO->INTSTS & (CRYPTO_INTSTS_AESIF_Msk | CRYPTO_INTSTS_AESEIF_Msk));
        g_AES_done = 1;
        AES_CLR_INT_FLAG(CRYPTO);
        CU_ASSERT_FALSE(CRYPTO->INTSTS & (CRYPTO_INTSTS_AESIF_Msk | CRYPTO_INTSTS_AESEIF_Msk));
    }
	
    if (CHAPOLY_GET_INT_FLAG(CRYPTO))
    {
        CU_ASSERT_TRUE(CRYPTO->INTSTS & (CRYPTO_INTSTS_CHAPOLYIF_Msk | CRYPTO_INTSTS_CHAPOLYEIF_Msk));
        g_CHA_done = 1;
        CHAPOLY_CLR_INT_FLAG(CRYPTO);
        CU_ASSERT_FALSE(CRYPTO->INTSTS & (CRYPTO_INTSTS_CHAPOLYIF_Msk | CRYPTO_INTSTS_CHAPOLYEIF_Msk));
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

CU_SuiteInfo Crypto_suites[] =
{
    {"Crypto CONST", Crypto_Tests_Init, Crypto_Tests_Clean, NULL, NULL, Crypto_ConstantTests},
    {"Crypto MACRO", Crypto_Tests_Init, Crypto_Tests_Clean, NULL, NULL, Crypto_MacroTests},
    {"Crypto API", Crypto_Tests_Init, Crypto_Tests_Clean, NULL, NULL, Crypto_ApiTests},
    CU_SUITE_INFO_NULL
};

//---------------------Const-------------------------------------//
void Crypto_Const_AES()
{
    CU_ASSERT(AES_KEY_SIZE_128 == 0);
    CU_ASSERT(AES_KEY_SIZE_192 == 1);
    CU_ASSERT(AES_KEY_SIZE_256 == 2);
    CU_ASSERT(AES_MODE_ECB == 0);
    CU_ASSERT(AES_MODE_CBC == 1);
    CU_ASSERT(AES_MODE_CFB == 2);
    CU_ASSERT(AES_MODE_OFB == 3);
    CU_ASSERT(AES_MODE_CTR == 4);
    CU_ASSERT(AES_MODE_CBC_CS1 == 0x10);
    CU_ASSERT(AES_MODE_CBC_CS2 == 0x11);
    CU_ASSERT(AES_MODE_CBC_CS3 == 0x12);
    CU_ASSERT(AES_NO_SWAP == 0);
    CU_ASSERT(AES_OUT_SWAP == 1);
    CU_ASSERT(AES_IN_SWAP == 2);
    CU_ASSERT(AES_IN_OUT_SWAP == 3);
}

void Crypto_Const_CHAPOLY()
{
	CU_ASSERT(CHAPOLY_MODE_CHACHA20 == 0);
    CU_ASSERT(CHAPOLY_MODE_POLY1305 == 1);
    CU_ASSERT(CHAPOLY_MODE_AEAD == 2);
}

void Crypto_Const_DMA()
{
    CU_ASSERT(CRYPTO_DMA_FIRST == 0x4);
    CU_ASSERT(CRYPTO_DMA_ONE_SHOT == 0x5);
    CU_ASSERT(CRYPTO_DMA_CONTINUE == 0x6);
    CU_ASSERT(CRYPTO_DMA_LAST == 0x7);
}


//---------------------Macro-------------------------------------//
void MACRO_AES_ENABLE_INT()
{
    CRYPTO->INTEN = 0;

    AES_ENABLE_INT(CRYPTO);
    CU_ASSERT_TRUE((CRYPTO->INTEN & (CRYPTO_INTEN_AESIEN_Msk | CRYPTO_INTEN_AESEIEN_Msk)) == (CRYPTO_INTEN_AESIEN_Msk | CRYPTO_INTEN_AESEIEN_Msk));

    AES_DISABLE_INT(CRYPTO);
    CU_ASSERT_FALSE(CRYPTO->INTEN & (CRYPTO_INTEN_AESIEN_Msk | CRYPTO_INTEN_AESEIEN_Msk));
}

void MACRO_AES_INT_FLAG()
{
    AES_ENABLE_INT(CRYPTO);

    AES_CLR_INT_FLAG(CRYPTO);
    CU_ASSERT_FALSE(CRYPTO->INTSTS & (CRYPTO_INTSTS_AESIF_Msk | CRYPTO_INTSTS_AESEIF_Msk));

    AES_Open(CRYPTO, 0, 1, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    AES_SetKey(CRYPTO, 0, au32TestKey, AES_KEY_SIZE_128);
    AES_SetInitVect(CRYPTO, 0, au32TestIV);
    AES_SetDMATransfer(CRYPTO, 0, (uint32_t)au8InputData, (uint32_t)au8OutputData, sizeof(au8InputData));

    g_AES_done = 0;
    AES_Start(CRYPTO, 0, CRYPTO_DMA_ONE_SHOT);

    while (!g_AES_done);
}

void MACRO_AES_KEY_PROTECT()
{
    CRYPTO->AES_CTL = 0;

    AES_ENABLE_KEY_PROTECT(CRYPTO);
    CU_ASSERT_TRUE(CRYPTO->AES_CTL & CRYPTO_AES_CTL_KEYPRT_Msk);

    AES_DISABLE_KEY_PROTECT(CRYPTO);
    CU_ASSERT_FALSE(CRYPTO->AES_CTL & CRYPTO_AES_CTL_KEYPRT_Msk);
}

void MACRO_CHAPOLY_ENABLE_INT()
{
    CRYPTO->INTEN = 0;

    CHAPOLY_ENABLE_INT(CRYPTO);
    CU_ASSERT_TRUE((CRYPTO->INTEN & (CRYPTO_INTEN_CHAPOLYIEN_Msk|CRYPTO_INTEN_CHAPOLYEIEN_Msk)) ==  (CRYPTO_INTEN_CHAPOLYIEN_Msk|CRYPTO_INTEN_CHAPOLYEIEN_Msk));
	
    CHAPOLY_DISABLE_INT(CRYPTO);
    CU_ASSERT_FALSE(CRYPTO->INTEN & (CRYPTO_INTEN_CHAPOLYIEN_Msk|CRYPTO_INTEN_CHAPOLYEIEN_Msk));
}


void MACRO_CHAPOLY_INT_FLAG()
{
    CHAPOLY_ENABLE_INT(CRYPTO);

    CHAPOLY_CLR_INT_FLAG(CRYPTO);
    CU_ASSERT_FALSE(CRYPTO->INTSTS & (CRYPTO_INTSTS_CHAPOLYIF_Msk | CRYPTO_INTSTS_CHAPOLYEIF_Msk));
    
    CHA_SetKeyandNonce(CRYPTO, au8TestKeyCHA, au8TestNonceCHA, 1);
    CHA_SetDMATransfer(CRYPTO, au8InputData, au8OutputData, sizeof(au8InputData));
    CHA_Start(CRYPTO, 1);

    while (!g_CHA_done);
	
}
//---------------------API-------------------------------------//
void API_AES_Open()
{
    uint32_t  param;

    /* u32Channel */

    //    for(param = 0; param < 4; param++)
    //    {
    //        AES_Open(CRYPTO, param, 1, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    //        CU_ASSERT(((CRYPTO->AES_CTL & CRYPTO_AES_CTL_CHANNEL_Msk) >> CRYPTO_AES_CTL_CHANNEL_Pos) == param);
    //    }

    /* u32EncDec */

    AES_Open(CRYPTO, 0, 1, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    CU_ASSERT(CRYPTO->AES_CTL & CRYPTO_AES_CTL_ENCRYPTO_Msk);

    AES_Open(CRYPTO, 0, 0, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    CU_ASSERT_FALSE(CRYPTO->AES_CTL & CRYPTO_AES_CTL_ENCRYPTO_Msk);

    /* u32OpMode */

    AES_Open(CRYPTO, 0, 0, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & CRYPTO_AES_CTL_OPMODE_Msk) >> CRYPTO_AES_CTL_OPMODE_Pos) == AES_MODE_ECB);

    AES_Open(CRYPTO, 0, 0, AES_MODE_CBC, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & CRYPTO_AES_CTL_OPMODE_Msk) >> CRYPTO_AES_CTL_OPMODE_Pos) == AES_MODE_CBC);

    AES_Open(CRYPTO, 0, 0, AES_MODE_CFB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & CRYPTO_AES_CTL_OPMODE_Msk) >> CRYPTO_AES_CTL_OPMODE_Pos) == AES_MODE_CFB);

    AES_Open(CRYPTO, 0, 0, AES_MODE_OFB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & CRYPTO_AES_CTL_OPMODE_Msk) >> CRYPTO_AES_CTL_OPMODE_Pos) == AES_MODE_OFB);

    AES_Open(CRYPTO, 0, 0, AES_MODE_CTR, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & CRYPTO_AES_CTL_OPMODE_Msk) >> CRYPTO_AES_CTL_OPMODE_Pos) == AES_MODE_CTR);

    AES_Open(CRYPTO, 0, 0, AES_MODE_CBC_CS1, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & CRYPTO_AES_CTL_OPMODE_Msk) >> CRYPTO_AES_CTL_OPMODE_Pos) == AES_MODE_CBC_CS1);

    AES_Open(CRYPTO, 0, 0, AES_MODE_CBC_CS2, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & CRYPTO_AES_CTL_OPMODE_Msk) >> CRYPTO_AES_CTL_OPMODE_Pos) == AES_MODE_CBC_CS2);

    AES_Open(CRYPTO, 0, 0, AES_MODE_CBC_CS3, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & CRYPTO_AES_CTL_OPMODE_Msk) >> CRYPTO_AES_CTL_OPMODE_Pos) == AES_MODE_CBC_CS3);

    /* u32KeySize */

    AES_Open(CRYPTO, 0, 0, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & CRYPTO_AES_CTL_KEYSZ_Msk) >> CRYPTO_AES_CTL_KEYSZ_Pos) == AES_KEY_SIZE_128);

    AES_Open(CRYPTO, 0, 0, AES_MODE_ECB, AES_KEY_SIZE_192, AES_IN_OUT_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & CRYPTO_AES_CTL_KEYSZ_Msk) >> CRYPTO_AES_CTL_KEYSZ_Pos) == AES_KEY_SIZE_192);

    AES_Open(CRYPTO, 0, 0, AES_MODE_ECB, AES_KEY_SIZE_256, AES_IN_OUT_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & CRYPTO_AES_CTL_KEYSZ_Msk) >> CRYPTO_AES_CTL_KEYSZ_Pos) == AES_KEY_SIZE_256);

    /* u32SwapType */

    AES_Open(CRYPTO, 0, 0, AES_MODE_ECB, AES_KEY_SIZE_128, AES_NO_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & 0xC00000) >> CRYPTO_AES_CTL_OUTSWAP_Pos) == AES_NO_SWAP);

    AES_Open(CRYPTO, 0, 0, AES_MODE_ECB, AES_KEY_SIZE_128, AES_OUT_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & 0xC00000) >> CRYPTO_AES_CTL_OUTSWAP_Pos) == AES_OUT_SWAP);

    AES_Open(CRYPTO, 0, 0, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & 0xC00000) >> CRYPTO_AES_CTL_OUTSWAP_Pos) == AES_IN_SWAP);

    AES_Open(CRYPTO, 0, 0, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    CU_ASSERT(((CRYPTO->AES_CTL & 0xC00000) >> CRYPTO_AES_CTL_OUTSWAP_Pos) == AES_IN_OUT_SWAP);

    CU_PASS();
}


void API_AES_SetKey()
{
    uint32_t  u32Channel;
    uint32_t  *key_ptr;
    int       i;

    for (u32Channel = 0; u32Channel < 1; u32Channel++)
    {
        key_ptr = (uint32_t *)((uint32_t)&CRYPTO->AES_KEY[0] + (u32Channel * 0x3C));

        for (i = 0; i < 4; i++)
            key_ptr[i] = 0;

        AES_SetKey(CRYPTO, u32Channel, au32TestKey, AES_KEY_SIZE_128);

        for (i = 0; i < 4; i++)
            CU_ASSERT(key_ptr[i] == au32TestKey[i]);

        for (i = 0; i < 6; i++)
            key_ptr[i] = 0;

        AES_SetKey(CRYPTO, u32Channel, au32TestKey, AES_KEY_SIZE_192);

        for (i = 0; i < 6; i++)
            CU_ASSERT(key_ptr[i] == au32TestKey[i]);

        for (i = 0; i < 8; i++)
            key_ptr[i] = 0;

        AES_SetKey(CRYPTO, u32Channel, au32TestKey, AES_KEY_SIZE_256);

        for (i = 0; i < 8; i++)
            CU_ASSERT(key_ptr[i] == au32TestKey[i]);
    }

    CU_PASS();
}

void API_AES_SetInitVect()
{
    uint32_t  u32Channel;
    uint32_t  *key_ptr;
    int       i;

    for (u32Channel = 0; u32Channel < 1; u32Channel++)
    {
        key_ptr = (uint32_t *)((uint32_t)&CRYPTO->AES_IV[0] + (u32Channel * 0x3C));

        for (i = 0; i < 4; i++)
            key_ptr[i] = 0;

        AES_SetInitVect(CRYPTO, u32Channel, au32TestKey);

        for (i = 0; i < 4; i++)
            CU_ASSERT(key_ptr[i] == au32TestKey[i]);
    }

    CU_PASS();
}

void API_AES_SetDMATransfer()
{
    uint32_t  u32Channel;
    uint32_t  *key_ptr;

    for (u32Channel = 0; u32Channel < 1; u32Channel++)
    {
        AES_SetDMATransfer(CRYPTO, u32Channel, 0x20001200, 0x20002600, 64);

        key_ptr = (uint32_t *)((uint32_t)&CRYPTO->AES_SADDR + (u32Channel * 0x3C));
        CU_ASSERT(*key_ptr == 0x20001200);
        key_ptr = (uint32_t *)((uint32_t)&CRYPTO->AES_DADDR + (u32Channel * 0x3C));
        CU_ASSERT(*key_ptr == 0x20002600);
        key_ptr = (uint32_t *)((uint32_t)&CRYPTO->AES_CNT + (u32Channel * 0x3C));
        CU_ASSERT(*key_ptr == 64);

        AES_SetDMATransfer(CRYPTO, u32Channel, 0x20000E00, 0x20001C00, 160);

        key_ptr = (uint32_t *)((uint32_t)&CRYPTO->AES_SADDR + (u32Channel * 0x3C));
        CU_ASSERT(*key_ptr == 0x20000E00);
        key_ptr = (uint32_t *)((uint32_t)&CRYPTO->AES_DADDR + (u32Channel * 0x3C));
        CU_ASSERT(*key_ptr == 0x20001C00);
        key_ptr = (uint32_t *)((uint32_t)&CRYPTO->AES_CNT + (u32Channel * 0x3C));
        CU_ASSERT(*key_ptr == 160);
    }

    CU_PASS();
}

void API_AES_Start()
{
    uint32_t  u32Channel;

    AES_ENABLE_INT(CRYPTO);

    for (u32Channel = 0; u32Channel < 1; u32Channel++)
    {
        AES_Open(CRYPTO, u32Channel, 1, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
        AES_SetKey(CRYPTO, u32Channel, au32TestKey, AES_KEY_SIZE_128);
        AES_SetInitVect(CRYPTO, u32Channel, au32TestIV);
        AES_SetDMATransfer(CRYPTO, u32Channel, (uint32_t)au8InputData, (uint32_t)au8OutputData, sizeof(au8InputData));

        g_AES_done = 0;
        AES_Start(CRYPTO, 0, CRYPTO_DMA_ONE_SHOT);

        while (!g_AES_done);
    }

    CU_PASS();
}


void API_CHAPOLY_SetKeyNounceCounter()
{
    uint8_t  *key_ptr;
	uint8_t  *nonce_ptr;
	uint8_t counter =1;
	int       i;

    key_ptr = (uint8_t *)((uint32_t)&CRYPTO->CHAPOLY_KEY[0]);
    nonce_ptr = (uint8_t *)((uint32_t)&CRYPTO->CHAPOLY_NONCE[0]);
    
	for (i = 0; i < 32; i++)
        key_ptr[i] = 0;

    CHA_SetKeyandNonce(CRYPTO, au8TestKeyCHA, au8TestNonceCHA, counter);

    for (i = 0; i < 32; i++)
        CU_ASSERT(key_ptr[i] == au8TestKeyCHA[i]);

    for (i = 0; i < 12; i++)
        CU_ASSERT(nonce_ptr[i] == au8TestNonceCHA[i]);
	
	CU_ASSERT((CRYPTO->CHAPOLY_CNT)==counter);

    CU_PASS();
}


void API_CHAPOLY_Start()
{

    CHAPOLY_ENABLE_INT(CRYPTO);
    CHAPOLY_CLR_INT_FLAG(CRYPTO);
        
    CHA_SetKeyandNonce(CRYPTO, au8TestKeyCHA, au8TestNonceCHA, 1);
    CHA_SetDMATransfer(CRYPTO, au8InputData, au8OutputData, sizeof(au8InputData));
	
	CHA_Start(CRYPTO, 1);
    CU_ASSERT(CRYPTO->CHAPOLY_CTL & CRYPTO_CHAPOLY_CTL_ENCRYPTO_Msk);

	  while (!g_CHA_done);
	
    CU_PASS();
}

//-------------------------CU_TesInfo Table-----------------------//
CU_TestInfo Crypto_ConstantTests[] =
{
    {"Crypto CONST AES", Crypto_Const_AES},
    {"Crypto CONST AES", Crypto_Const_DMA},
	{"Crypto CONST CHAPOLY", Crypto_Const_CHAPOLY},
    CU_TEST_INFO_NULL
};

CU_TestInfo Crypto_MacroTests[] =
{
    {"Enable/Disable AES interrupt", MACRO_AES_ENABLE_INT},
    {"Get/Clear AES interrupt flag", MACRO_AES_INT_FLAG},		
    {"Enable/Disable AES key protect", MACRO_AES_KEY_PROTECT},
	{"Enable/Disable CHAPOLY interrupt", MACRO_CHAPOLY_ENABLE_INT},
    {"Get/Clear CHAPOLY interrupt flag", MACRO_CHAPOLY_INT_FLAG},
    CU_TEST_INFO_NULL
};

CU_TestInfo Crypto_ApiTests[] =
{
    {"AES_Open", API_AES_Open},
    {"AES_SetKey", API_AES_SetKey},
    {"AES_SetInitVect", API_AES_SetInitVect},
    {"AES_SetDMATransfer", API_AES_SetDMATransfer},
    {"AES_Start", API_AES_Start},
	{"CHAPOLY_Start", API_CHAPOLY_Start},		
	{"CHAPOLY_SetKeyNonceCounter", API_CHAPOLY_SetKeyNounceCounter},
		
    CU_TEST_INFO_NULL
};

