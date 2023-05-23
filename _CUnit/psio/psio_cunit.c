/*----------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                */
/*                                                                            */
/* Copyright (c) 2014 by Nuvoton Technology Corporation                       */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   pdma_cunit.c                                                                                           */
/*            The test function of PDMA for CUnit.                                                          */
/* Project:   FA8256                                                                                       */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Includes of system headers                                                 */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "psio_cunit.h"

/*----------------------------------------------------------------------------*/
/* Includes of local headers                                                  */
/*----------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

//------------------------------------------------------------------------------
//#define M252_G
//#define M252_E
//#define M252_D
#define TC8263

//------------------------------------------------------------------------------
extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;

//------------------------------------------------------------------------------
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    #define MAX_PIN_NUM 8
    #define MAX_SC_NUM 4

#elif defined(M252_D)
    #define MAX_PIN_NUM 4
    #define MAX_SC_NUM 2

#else
    #define MAX_PIN_NUM 8
    #define MAX_SC_NUM 4

#endif

/*---------------------------------------------------------------------------------------------------------*/
/*  Global variable                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*  Constant Definitions                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
const uint32_t au32SlotCNT[MAX_SC_NUM] = {PSIO_SC0, PSIO_SC1, PSIO_SC2, PSIO_SC3};
const uint32_t au32Slot[8] = {PSIO_SLOT0, PSIO_SLOT1, PSIO_SLOT2, PSIO_SLOT3, PSIO_SLOT4, PSIO_SLOT5, PSIO_SLOT6, PSIO_SLOT7};
const uint32_t au32PIN[MAX_PIN_NUM] = {PSIO_PIN0, PSIO_PIN1, PSIO_PIN2, PSIO_PIN3, PSIO_PIN4, PSIO_PIN5, PSIO_PIN6, PSIO_PIN7};
const uint32_t au32CP[8] = {PSIO_CP0, PSIO_CP1, PSIO_CP2, PSIO_CP3, PSIO_CP4, PSIO_CP5, PSIO_CP6, PSIO_CP7};

#elif defined(M252_D)
const uint32_t au32SlotCNT[MAX_SC_NUM] = {PSIO_SC0, PSIO_SC1};
const uint32_t au32Slot[8] = {PSIO_SLOT0, PSIO_SLOT1, PSIO_SLOT2, PSIO_SLOT3, PSIO_SLOT4, PSIO_SLOT5, PSIO_SLOT6, PSIO_SLOT7};
const uint32_t au32PIN[MAX_PIN_NUM] = {PSIO_PIN0, PSIO_PIN1, PSIO_PIN2, PSIO_PIN3};
const uint32_t au32CP[8] = {PSIO_CP0, PSIO_CP1, PSIO_CP2, PSIO_CP3, PSIO_CP4, PSIO_CP5, PSIO_CP6, PSIO_CP7};

#else
const uint32_t au32SlotCNT[MAX_SC_NUM] = {PSIO_SC0, PSIO_SC1, PSIO_SC2, PSIO_SC3};
const uint32_t au32Slot[8] = {PSIO_SLOT0, PSIO_SLOT1, PSIO_SLOT2, PSIO_SLOT3, PSIO_SLOT4, PSIO_SLOT5, PSIO_SLOT6, PSIO_SLOT7};
const uint32_t au32PIN[MAX_PIN_NUM] = {PSIO_PIN0, PSIO_PIN1, PSIO_PIN2, PSIO_PIN3, PSIO_PIN4, PSIO_PIN5, PSIO_PIN6, PSIO_PIN7};
const uint32_t au32CP[8] = {PSIO_CP0, PSIO_CP1, PSIO_CP2, PSIO_CP3, PSIO_CP4, PSIO_CP5, PSIO_CP6, PSIO_CP7};
#endif

#define PSIO_IPRESET()                               \
    do                                               \
    {                                                \
        /* Unlock protected registers */             \
        SYS_UnlockReg();                             \
        SYS->PSIORST |= SYS_PSIORST_PSIO0RST_Msk;    \
        SYS->PSIORST &= (~SYS_PSIORST_PSIO0RST_Msk); \
    } while (0)

#define PSIO_INTCTL_LOOPBACK_Msk (1 << 31)
//PSIO_INTCTL   LOOPBACK
#define PSIO_ENABLE_LOOPBACK    (PSIO->INTCTL |= PSIO_INTCTL_LOOPBACK_Msk)

#define PSIO_DISABLE_LOOPBACK   (PSIO->INTCTL &= (~PSIO_INTCTL_LOOPBACK_Msk))

#undef PDMA
#define PDMA                        PDMA0

/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int suite_success_init(void)
{
    /* Enable Internal RC 22.1184MHz clock */
    //CLK->PWRCTL |= CLK_PWRCTL_HIRCEN_Msk;

    /* Waiting for Internal RC clock ready */
    //while (!(CLK->STATUS & CLK_STATUS_HIRCSTB_Msk));

    /* Enable PSIO module clock */
    CLK_EnableModuleClock(PSIO0_MODULE);

    /* Reset PSIO */
    PSIO_IPRESET();

#if defined(M252_G)
    printf("\nVersion: %s, PIN Number:%d, Slot Counter Number:%d\n", "M252_G", MAX_PIN_NUM, MAX_SC_NUM);
#elif defined(M252_E)
    printf("\nVersion: %s, PIN Number:%d, Slot Counter Number:%d\n", "M252_E", MAX_PIN_NUM, MAX_SC_NUM);
#elif defined(M252_D)
    printf("\nVersion: %s, PIN Number:%d, Slot Counter Number:%d\n", "M252_D", MAX_PIN_NUM, MAX_SC_NUM);
#elif defined(TC8263)
    printf("\nVersion: %s, PIN Number:%d, Slot Counter Number:%d\n", "TC8263", MAX_PIN_NUM, MAX_SC_NUM);
#endif
    return 0;
}

int suite_success_clean(void)
{
    /* Disable PSIO module clock */
    CLK_DisableModuleClock(PSIO0_MODULE);

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

CU_SuiteInfo suites[] =
{
    {"PSIO MACRO", suite_success_init, suite_success_clean, NULL, NULL, PSIO_MACRO},
    {"PSIO API", suite_success_init, suite_success_clean, NULL, NULL, PSIO_API},
    {"PSIO CONSTANT", suite_success_init, suite_success_clean, NULL, NULL, PSIO_CONSTANT},
    CU_SUITE_INFO_NULL
};

void Func_PSIO_ENABLE_INT()
{
    /* Reset PSIO */
    PSIO_IPRESET();

    PSIO_ENABLE_INT(PSIO, PSIO_INTEN_CON0IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x1);

    PSIO_DISABLE_INT(PSIO, PSIO_INTEN_CON0IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x0);

    PSIO_ENABLE_INT(PSIO, PSIO_INTEN_CON1IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x1 << 1);

    PSIO_DISABLE_INT(PSIO, PSIO_INTEN_CON1IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x0);

    PSIO_ENABLE_INT(PSIO, PSIO_INTEN_MISMATIE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x1 << 2);

    PSIO_DISABLE_INT(PSIO, PSIO_INTEN_MISMATIE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x0);

    PSIO_ENABLE_INT(PSIO, PSIO_INTEN_TERRIE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x1 << 3);

    PSIO_DISABLE_INT(PSIO, PSIO_INTEN_TERRIE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x0);

    PSIO_ENABLE_INT(PSIO, PSIO_INTEN_SC0IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x1 << 4);

    PSIO_DISABLE_INT(PSIO, PSIO_INTEN_SC0IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x0);

    PSIO_ENABLE_INT(PSIO, PSIO_INTEN_SC1IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x1 << 5);

    PSIO_DISABLE_INT(PSIO, PSIO_INTEN_SC1IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x0);

#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    PSIO_ENABLE_INT(PSIO, PSIO_INTEN_SC2IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x1 << 6);

    PSIO_DISABLE_INT(PSIO, PSIO_INTEN_SC2IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x0);

    PSIO_ENABLE_INT(PSIO, PSIO_INTEN_SC3IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x1 << 7);

    PSIO_DISABLE_INT(PSIO, PSIO_INTEN_SC3IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x0);
#endif

#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    PSIO_ENABLE_INT(PSIO, PSIO_INTEN_CON0IE_Msk | PSIO_INTEN_CON1IE_Msk | PSIO_INTEN_MISMATIE_Msk | PSIO_INTEN_TERRIE_Msk
                    | PSIO_INTEN_SC0IE_Msk | PSIO_INTEN_SC1IE_Msk | PSIO_INTEN_SC2IE_Msk | PSIO_INTEN_SC3IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0xFF);

    PSIO_DISABLE_INT(PSIO, PSIO_INTEN_CON1IE_Msk | PSIO_INTEN_TERRIE_Msk | PSIO_INTEN_SC1IE_Msk | PSIO_INTEN_SC3IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x55);

    PSIO_DISABLE_INT(PSIO, PSIO_INTEN_CON0IE_Msk | PSIO_INTEN_MISMATIE_Msk | PSIO_INTEN_SC0IE_Msk | PSIO_INTEN_SC2IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x0);

#elif defined(M252_D)
    PSIO_ENABLE_INT(PSIO, PSIO_INTEN_CON0IE_Msk | PSIO_INTEN_CON1IE_Msk | PSIO_INTEN_MISMATIE_Msk | PSIO_INTEN_TERRIE_Msk
                    | PSIO_INTEN_SC0IE_Msk | PSIO_INTEN_SC1IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x3F);

    PSIO_DISABLE_INT(PSIO, PSIO_INTEN_CON1IE_Msk | PSIO_INTEN_TERRIE_Msk | PSIO_INTEN_SC1IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x15);

    PSIO_DISABLE_INT(PSIO, PSIO_INTEN_CON0IE_Msk | PSIO_INTEN_MISMATIE_Msk | PSIO_INTEN_SC0IE_Msk);
    CU_ASSERT_EQUAL(PSIO->INTEN, 0x0);

#else

    while (1);

#endif
}

#define TXSIZE 16
void Func_PSIO_GET_INT_FLAG()
{
    uint8_t u8Channel = 0;
    uint8_t au8TxData[TXSIZE] = {0};
    /*******************************************************************/
    /****                PSIO_INTSTS_SC0IF_Msk                      ****/
    /*******************************************************************/
    /* Reset PSIO */
    PSIO_IPRESET();

    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_ENABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_SLOT0);

    PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT0, 0xF);

    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_OUT_LOW);

    //Enable PIN loopback function
    PSIO->INTCTL |= PSIO_INTCTL_LOOPBACK_Msk;

    PSIO_SET_TRIGSRC(PSIO, PSIO_SC0, PSIO_SW_TRIGGER);
    PSIO_START_SC(PSIO, PSIO_SC0);

    while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC0)) {}

    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTSTS_SC0IF_Msk), 0x1);
    PSIO_CLEAR_INT_FLAG(PSIO, PSIO_INTSTS_SC0IF_Msk);
    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTSTS_SC0IF_Msk), 0x0);

    //Disable PIN loopback function
    PSIO->INTCTL &= ~PSIO_INTCTL_LOOPBACK_Msk;

    //Disable PIN
    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_DISABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

    /*******************************************************************/
    /****                PSIO_INTSTS_SC1IF_Msk                      ****/
    /*******************************************************************/
    /* Reset PSIO */
    PSIO_IPRESET();

    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_ENABLE, PSIO_SC1, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_SLOT0);

    PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC1, PSIO_SLOT0, 0xF);

    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_OUT_LOW);

    //Enable PIN loopback function
    PSIO->INTCTL |= PSIO_INTCTL_LOOPBACK_Msk;

    PSIO_SET_TRIGSRC(PSIO, PSIO_SC1, PSIO_SW_TRIGGER);
    PSIO_START_SC(PSIO, PSIO_SC1);

    while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC1)) {}

    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTSTS_SC1IF_Msk), 0x1);
    PSIO_CLEAR_INT_FLAG(PSIO, PSIO_INTSTS_SC1IF_Msk);
    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTSTS_SC1IF_Msk), 0x0);

    //Disable PIN loopback function
    PSIO->INTCTL &= ~PSIO_INTCTL_LOOPBACK_Msk;

    //Disable PIN
    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_DISABLE, PSIO_SC1, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    /*******************************************************************/
    /****                PSIO_INTSTS_SC2IF_Msk                      ****/
    /*******************************************************************/
    /* Reset PSIO */
    PSIO_IPRESET();

    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_ENABLE, PSIO_SC2, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_SLOT0);

    PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC2, PSIO_SLOT0, 0xF);

    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_OUT_LOW);

    //Enable PIN loopback function
    PSIO->INTCTL |= PSIO_INTCTL_LOOPBACK_Msk;

    PSIO_SET_TRIGSRC(PSIO, PSIO_SC2, PSIO_SW_TRIGGER);
    PSIO_START_SC(PSIO, PSIO_SC2);

    while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC2)) {}

    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTSTS_SC2IF_Msk), 0x1);
    PSIO_CLEAR_INT_FLAG(PSIO, PSIO_INTSTS_SC2IF_Msk);
    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTSTS_SC2IF_Msk), 0x0);

    //Disable PIN loopback function
    PSIO->INTCTL &= ~PSIO_INTCTL_LOOPBACK_Msk;

    //Disable PIN
    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_DISABLE, PSIO_SC2, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

    /*******************************************************************/
    /****                PSIO_INTSTS_SC3IF_Msk                      ****/
    /*******************************************************************/
    /* Reset PSIO */
    PSIO_IPRESET();

    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_ENABLE, PSIO_SC3, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_SLOT0);

    PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC3, PSIO_SLOT0, 0xF);

    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_OUT_LOW);

    //Enable PIN loopback function
    PSIO->INTCTL |= PSIO_INTCTL_LOOPBACK_Msk;

    PSIO_SET_TRIGSRC(PSIO, PSIO_SC3, PSIO_SW_TRIGGER);
    PSIO_START_SC(PSIO, PSIO_SC3);

    while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC3)) {}

    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTSTS_SC3IF_Msk), 0x1);
    PSIO_CLEAR_INT_FLAG(PSIO, PSIO_INTSTS_SC3IF_Msk);
    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTSTS_SC3IF_Msk), 0x0);

    //Disable PIN loopback function
    PSIO->INTCTL &= ~PSIO_INTCTL_LOOPBACK_Msk;

    //Disable PIN
    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_DISABLE, PSIO_SC3, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

#endif

    /*******************************************************************/
    /****                PSIO_INTSTS_CON0IF_Msk                     ****/
    /*******************************************************************/
    /* Reset PSIO */
    PSIO_IPRESET();

    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_ENABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_SLOT0);

    PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT0, 0xF);

    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_OUT_LOW);

    //Enable PIN loopback function
    PSIO->INTCTL |= PSIO_INTCTL_LOOPBACK_Msk;

    //Set interrupt control
    PSIO_SET_INTCTL(PSIO, PSIO_SC0, PSIO_INT0, PSIO_SLOT0);

    PSIO_SET_TRIGSRC(PSIO, PSIO_SC0, PSIO_SW_TRIGGER);
    PSIO_START_SC(PSIO, PSIO_SC0);

    while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC0)) {}

    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTSTS_CON0IF_Msk), 0x1);
    PSIO_CLEAR_INT_FLAG(PSIO, PSIO_INTSTS_CON0IF_Msk);
    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTSTS_CON0IF_Msk), 0x0);

    //Disable PIN loopback function
    PSIO->INTCTL &= ~PSIO_INTCTL_LOOPBACK_Msk;

    //Disable PIN
    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_DISABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

    /*******************************************************************/
    /****                PSIO_INTSTS_CON1IF_Msk                     ****/
    /*******************************************************************/
    /* Reset PSIO */
    PSIO_IPRESET();

    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_ENABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_SLOT0);

    PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT0, 0xF);

    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_OUT_LOW);

    //Enable PIN loopback function
    PSIO->INTCTL |= PSIO_INTCTL_LOOPBACK_Msk;

    //Set interrupt control
    PSIO_SET_INTCTL(PSIO, PSIO_SC0, PSIO_INT1, PSIO_SLOT0);

    PSIO_SET_TRIGSRC(PSIO, PSIO_SC0, PSIO_SW_TRIGGER);
    PSIO_START_SC(PSIO, PSIO_SC0);

    while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC0)) {}

    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTSTS_CON1IF_Msk), 0x1);
    PSIO_CLEAR_INT_FLAG(PSIO, PSIO_INTSTS_CON1IF_Msk);
    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTSTS_CON1IF_Msk), 0x0);

    //Disable PIN loopback function
    PSIO->INTCTL &= ~PSIO_INTCTL_LOOPBACK_Msk;

    //Disable PIN
    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_DISABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

    /*******************************************************************/
    /****                PSIO_INTSTS_TERRIF_Msk                     ****/
    /*******************************************************************/
    /* Reset PSIO */
    PSIO_IPRESET();

    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_ENABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_SLOT0);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP1, PSIO_SLOT1);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP2, PSIO_SLOT2);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP3, PSIO_SLOT3);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP4, PSIO_SLOT4);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP5, PSIO_SLOT5);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP6, PSIO_SLOT6);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP7, PSIO_SLOT7);

    PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT0, 0xF);
    PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT1, 0xF);
    PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT2, 0xF);
    PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT3, 0xF);
    PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT4, 0xF);
    PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT5, 0xF);
    PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT6, 0xF);
    PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT7, 0xF);

    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP1, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP2, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP3, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP4, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP5, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP6, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP7, PSIO_OUT_BUFFER);

    PSIO_SET_WIDTH(PSIO, PSIO_PIN0, 1, 4);
    PSIO_SET_OUTPUT_DEPTH(PSIO, PSIO_PIN0, PSIO_DEPTH1);

    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN0, 0x01010101);

    //Enable PIN loopback function
    PSIO->INTCTL |= PSIO_INTCTL_LOOPBACK_Msk;

    PSIO_SET_TRIGSRC(PSIO, PSIO_SC0, PSIO_SW_TRIGGER);
    PSIO_START_SC(PSIO, PSIO_SC0);

    while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC0)) {}

    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTSTS_TERRIF_Msk), 0x1);
    PSIO_CLEAR_TRANSFER_STATUS(PSIO, PSIO_TRANSTS_OUTUF0_Msk);
    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTSTS_TERRIF_Msk), 0x0);

    //Disable PIN loopback function
    PSIO->INTCTL &= ~PSIO_INTCTL_LOOPBACK_Msk;

    //Disable PIN
    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_DISABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

    /*******************************************************************/
    /****                PSIO_INTEN_MISMATIE_Msk                    ****/
    /*******************************************************************/
    /* Reset PSIO */
    PSIO_IPRESET();

    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_ENABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);
    PSIO_SET_GENCTL(PSIO, PSIO_PIN1, PSIO_PIN_ENABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_SLOT0);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP1, PSIO_SLOT1);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP2, PSIO_SLOT2);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP3, PSIO_SLOT3);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP4, PSIO_SLOT4);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP5, PSIO_SLOT5);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP6, PSIO_SLOT6);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN0, PSIO_CP7, PSIO_SLOT7);

    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN1, PSIO_CP0, PSIO_SLOT0);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN1, PSIO_CP1, PSIO_SLOT1);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN1, PSIO_CP2, PSIO_SLOT2);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN1, PSIO_CP3, PSIO_SLOT3);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN1, PSIO_CP4, PSIO_SLOT4);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN1, PSIO_CP5, PSIO_SLOT5);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN1, PSIO_CP6, PSIO_SLOT6);
    PSIO_SET_CHECKPOINT(PSIO, PSIO_PIN1, PSIO_CP7, PSIO_SLOT7);

    PSIO_SCSLOT_SET_ALL_SLOT(PSIO, PSIO_SC0, 0xF);

    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP0, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP1, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP2, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP3, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP4, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP5, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP6, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN0, PSIO_CP7, PSIO_OUT_BUFFER);

    PSIO_SET_ACTION(PSIO, PSIO_PIN1, PSIO_CP0, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN1, PSIO_CP1, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN1, PSIO_CP2, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN1, PSIO_CP3, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN1, PSIO_CP4, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN1, PSIO_CP5, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN1, PSIO_CP6, PSIO_OUT_BUFFER);
    PSIO_SET_ACTION(PSIO, PSIO_PIN1, PSIO_CP7, PSIO_OUT_BUFFER);

    PSIO_SET_WIDTH(PSIO, PSIO_PIN0, 0, 4);
    PSIO_SET_OUTPUT_DEPTH(PSIO, PSIO_PIN0, PSIO_DEPTH1);

    PSIO_SET_WIDTH(PSIO, PSIO_PIN1, 0, 1);
    PSIO_SET_OUTPUT_DEPTH(PSIO, PSIO_PIN1, PSIO_DEPTH1);

    PSIO_SET_PDMA_OUTPUT(PSIO, PSIO_SC0, PSIO_PDMACTL_OPIN0EN_Msk);
    PSIO_SET_PDMA_OUTPUT(PSIO, PSIO_SC0, PSIO_PDMACTL_OPIN1EN_Msk);

    //Set slot as repeat mode
    PSIO_SET_SCCTL(PSIO, PSIO_SC0, 0, 0, 0, PSIO_REPEAT_ENABLE);

    /* PDMA Clock Enable */
    CLK_EnableModuleClock(PDMA0_MODULE);

    PDMA->DSCT[u8Channel].CTL =
        PDMA_OP_BASIC |
        PDMA_REQ_SINGLE |
        PDMA_SAR_INC |             /* source address -> incremented */
        PDMA_DAR_FIX |             /* destination address -> fixed(UART) */
        PDMA_WIDTH_8 |          /* transfer width -> 8-bit */
        //PDMA_TBINT_DISABLE |
        ((TXSIZE / 2 - 1) << 16);
    PDMA->DSCT[u8Channel].SA = (uint32_t)au8TxData;
    PDMA->DSCT[u8Channel].DA = (uint32_t)(&(PSIO->PODAT));

    /* Enable PDMA channel x */
    PDMA->CHCTL |= (1 << u8Channel);

    //Set request source
    PDMA_SetTransferMode(PDMA, u8Channel, PDMA_PSIO_TX, 0, 0);


    PSIO_SET_TRIGSRC(PSIO, PSIO_SC0, PSIO_SW_TRIGGER);
    PSIO_START_SC(PSIO, PSIO_SC0);

    while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC0)) {}

    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTEN_MISMATIE_Msk), 0x1);
    PSIO_CLEAR_INT_FLAG(PSIO, PSIO_INTEN_MISMATIE_Msk);
    CU_ASSERT_EQUAL(PSIO_GET_INT_FLAG(PSIO, PSIO_INTEN_MISMATIE_Msk), 0x0);

    //Test idle flag macro
    CU_ASSERT_EQUAL(PSIO_GET_IDLE_FLAG(PSIO, PSIO_SC0), 0x0);
    PSIO_SET_IDLE_FLAG(PSIO, PSIO_SC0);
    CU_ASSERT_EQUAL(PSIO_GET_IDLE_FLAG(PSIO, PSIO_SC0), 0x1);

    //Disable PIN
    PSIO_SET_GENCTL(PSIO, PSIO_PIN0, PSIO_PIN_DISABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);
    PSIO_SET_GENCTL(PSIO, PSIO_PIN1, PSIO_PIN_DISABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);
}

void Func_PSIO_GET_TRANSFER_STATUS()
{
    uint32_t u32Index = 0;

#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    uint32_t au32INFULL[MAX_PIN_NUM] = {PSIO_TRANSTS_INFULL0_Msk, PSIO_TRANSTS_INFULL1_Msk, PSIO_TRANSTS_INFULL2_Msk, PSIO_TRANSTS_INFULL3_Msk
                                        , PSIO_TRANSTS_INFULL4_Msk, PSIO_TRANSTS_INFULL5_Msk, PSIO_TRANSTS_INFULL6_Msk, PSIO_TRANSTS_INFULL7_Msk
                                       };
    uint32_t au32INOVER[MAX_PIN_NUM] = {PSIO_TRANSTS_INOVER0_Msk, PSIO_TRANSTS_INOVER1_Msk, PSIO_TRANSTS_INOVER2_Msk, PSIO_TRANSTS_INOVER3_Msk
                                        , PSIO_TRANSTS_INOVER4_Msk, PSIO_TRANSTS_INOVER5_Msk, PSIO_TRANSTS_INOVER6_Msk, PSIO_TRANSTS_INOVER7_Msk
                                       };
    uint32_t au32OUTEPY[MAX_PIN_NUM] = {PSIO_TRANSTS_OUTEPY0_Msk, PSIO_TRANSTS_OUTEPY1_Msk, PSIO_TRANSTS_OUTEPY2_Msk, PSIO_TRANSTS_OUTEPY3_Msk
                                        , PSIO_TRANSTS_OUTEPY4_Msk, PSIO_TRANSTS_OUTEPY5_Msk, PSIO_TRANSTS_OUTEPY6_Msk, PSIO_TRANSTS_OUTEPY7_Msk
                                       };
    uint32_t au32OUTUF[MAX_PIN_NUM] = {PSIO_TRANSTS_OUTUF0_Msk, PSIO_TRANSTS_OUTUF1_Msk, PSIO_TRANSTS_OUTUF2_Msk, PSIO_TRANSTS_OUTUF3_Msk
                                       , PSIO_TRANSTS_OUTUF4_Msk, PSIO_TRANSTS_OUTUF5_Msk, PSIO_TRANSTS_OUTUF6_Msk, PSIO_TRANSTS_OUTUF7_Msk
                                      };
#elif defined(M252_D)
    uint32_t au32INFULL[MAX_PIN_NUM] = {PSIO_TRANSTS_INFULL0_Msk, PSIO_TRANSTS_INFULL1_Msk, PSIO_TRANSTS_INFULL2_Msk, PSIO_TRANSTS_INFULL3_Msk};
    uint32_t au32INOVER[MAX_PIN_NUM] = {PSIO_TRANSTS_INOVER0_Msk, PSIO_TRANSTS_INOVER1_Msk, PSIO_TRANSTS_INOVER2_Msk, PSIO_TRANSTS_INOVER3_Msk};
    uint32_t au32OUTEPY[MAX_PIN_NUM] = {PSIO_TRANSTS_OUTEPY0_Msk, PSIO_TRANSTS_OUTEPY1_Msk, PSIO_TRANSTS_OUTEPY2_Msk, PSIO_TRANSTS_OUTEPY3_Msk};
    uint32_t au32OUTUF[MAX_PIN_NUM] = {PSIO_TRANSTS_OUTUF0_Msk, PSIO_TRANSTS_OUTUF1_Msk, PSIO_TRANSTS_OUTUF2_Msk, PSIO_TRANSTS_OUTUF3_Msk};
#else
    uint32_t au32INFULL[MAX_PIN_NUM] = {PSIO_TRANSTS_INFULL0_Msk, PSIO_TRANSTS_INFULL1_Msk, PSIO_TRANSTS_INFULL2_Msk, PSIO_TRANSTS_INFULL3_Msk
                                        , PSIO_TRANSTS_INFULL4_Msk, PSIO_TRANSTS_INFULL5_Msk, PSIO_TRANSTS_INFULL6_Msk, PSIO_TRANSTS_INFULL7_Msk
                                       };
    uint32_t au32INOVER[MAX_PIN_NUM] = {PSIO_TRANSTS_INOVER0_Msk, PSIO_TRANSTS_INOVER1_Msk, PSIO_TRANSTS_INOVER2_Msk, PSIO_TRANSTS_INOVER3_Msk
                                        , PSIO_TRANSTS_INOVER4_Msk, PSIO_TRANSTS_INOVER5_Msk, PSIO_TRANSTS_INOVER6_Msk, PSIO_TRANSTS_INOVER7_Msk
                                       };
    uint32_t au32OUTEPY[MAX_PIN_NUM] = {PSIO_TRANSTS_OUTEPY0_Msk, PSIO_TRANSTS_OUTEPY1_Msk, PSIO_TRANSTS_OUTEPY2_Msk, PSIO_TRANSTS_OUTEPY3_Msk
                                        , PSIO_TRANSTS_OUTEPY4_Msk, PSIO_TRANSTS_OUTEPY5_Msk, PSIO_TRANSTS_OUTEPY6_Msk, PSIO_TRANSTS_OUTEPY7_Msk
                                       };
    uint32_t au32OUTUF[MAX_PIN_NUM] = {PSIO_TRANSTS_OUTUF0_Msk, PSIO_TRANSTS_OUTUF1_Msk, PSIO_TRANSTS_OUTUF2_Msk, PSIO_TRANSTS_OUTUF3_Msk
                                       , PSIO_TRANSTS_OUTUF4_Msk, PSIO_TRANSTS_OUTUF5_Msk, PSIO_TRANSTS_OUTUF6_Msk, PSIO_TRANSTS_OUTUF7_Msk
                                      };

#endif

    /*******************************************************************/
    /****                PSIO_GET_TRANSFER_STATUS                   ****/
    /*******************************************************************/
    for (u32Index = 0; u32Index < MAX_PIN_NUM; u32Index++) //Output empty, output under flow
    {
        /* Reset PSIO */
        PSIO_IPRESET();

        PSIO_SET_GENCTL(PSIO, au32PIN[u32Index], PSIO_PIN_ENABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP0, PSIO_SLOT0);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP1, PSIO_SLOT1);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP2, PSIO_SLOT2);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP3, PSIO_SLOT3);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP4, PSIO_SLOT4);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP5, PSIO_SLOT5);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP6, PSIO_SLOT6);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP7, PSIO_SLOT7);

        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT0, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT1, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT2, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT3, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT4, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT5, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT6, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT7, 0xF);

        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP0, PSIO_OUT_BUFFER);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP1, PSIO_OUT_BUFFER);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP2, PSIO_OUT_BUFFER);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP3, PSIO_OUT_BUFFER);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP4, PSIO_OUT_BUFFER);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP5, PSIO_OUT_BUFFER);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP6, PSIO_OUT_BUFFER);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP7, PSIO_OUT_BUFFER);

        PSIO_SET_WIDTH(PSIO, au32PIN[u32Index], 0, 4);
        PSIO_SET_OUTPUT_DEPTH(PSIO, au32PIN[u32Index], PSIO_DEPTH1);

        CU_ASSERT_EQUAL(PSIO_GET_TRANSFER_STATUS(PSIO, au32OUTEPY[u32Index]), 0x1);
        PSIO_SET_OUTPUT_DATA(PSIO, au32PIN[u32Index], 0x01010101);
        CU_ASSERT_EQUAL(PSIO_GET_TRANSFER_STATUS(PSIO, au32OUTEPY[u32Index]), 0x0);

        PSIO_SET_TRIGSRC(PSIO, PSIO_SC0, PSIO_SW_TRIGGER);
        PSIO_START_SC(PSIO, PSIO_SC0);

        while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC0)) {}

        CU_ASSERT_EQUAL(PSIO_GET_TRANSFER_STATUS(PSIO, au32OUTUF[u32Index]), 0x1);
        PSIO_CLEAR_TRANSFER_STATUS(PSIO, au32OUTUF[u32Index]);
        CU_ASSERT_EQUAL(PSIO_GET_TRANSFER_STATUS(PSIO, au32OUTUF[u32Index]), 0x0);

        //Disable PIN
        PSIO_SET_GENCTL(PSIO, au32PIN[u32Index], PSIO_PIN_DISABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);
    }

    for (u32Index = 0; u32Index < MAX_PIN_NUM; u32Index++) //Input full, input over flow
    {
        /* Reset PSIO */
        PSIO_IPRESET();

        PSIO_SET_GENCTL(PSIO, au32PIN[u32Index], PSIO_PIN_ENABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP0, PSIO_SLOT0);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP1, PSIO_SLOT1);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP2, PSIO_SLOT2);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP3, PSIO_SLOT3);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP4, PSIO_SLOT4);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP5, PSIO_SLOT5);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP6, PSIO_SLOT6);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP7, PSIO_SLOT7);

        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT0, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT1, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT2, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT3, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT4, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT5, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT6, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT7, 0xF);

        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP0, PSIO_IN_BUFFER);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP1, PSIO_IN_BUFFER);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP2, PSIO_IN_BUFFER);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP3, PSIO_IN_BUFFER);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP4, PSIO_IN_BUFFER);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP5, PSIO_IN_BUFFER);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP6, PSIO_IN_BUFFER);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP7, PSIO_IN_BUFFER);

        PSIO_SET_WIDTH(PSIO, au32PIN[u32Index], 4, 0);
        PSIO_SET_INPUT_DEPTH(PSIO, au32PIN[u32Index], PSIO_DEPTH1);

        CU_ASSERT_EQUAL(PSIO_GET_TRANSFER_STATUS(PSIO, au32OUTEPY[u32Index]), 0x1);
        PSIO_SET_OUTPUT_DATA(PSIO, au32PIN[u32Index], 0x01010101);
        CU_ASSERT_EQUAL(PSIO_GET_TRANSFER_STATUS(PSIO, au32OUTEPY[u32Index]), 0x0);

        PSIO_SET_TRIGSRC(PSIO, PSIO_SC0, PSIO_SW_TRIGGER);
        PSIO_START_SC(PSIO, PSIO_SC0);

        while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC0)) {}

        CU_ASSERT_EQUAL(PSIO_GET_TRANSFER_STATUS(PSIO, au32INFULL[u32Index]), 0x1);
        PSIO_GET_INPUT_DATA(PSIO, au32PIN[u32Index]);
        CU_ASSERT_EQUAL(PSIO_GET_TRANSFER_STATUS(PSIO, au32INFULL[u32Index]), 0x0);

        CU_ASSERT_EQUAL(PSIO_GET_TRANSFER_STATUS(PSIO, au32INOVER[u32Index]), 0x1);
        PSIO_CLEAR_TRANSFER_STATUS(PSIO, au32INOVER[u32Index]);
        CU_ASSERT_EQUAL(PSIO_GET_TRANSFER_STATUS(PSIO, au32INOVER[u32Index]), 0x0);

        //Disable PIN
        PSIO_SET_GENCTL(PSIO, au32PIN[u32Index], PSIO_PIN_DISABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);
    }
}

void Func_PSIO_GET_INPUT_STATUS_STATE()
{
    uint32_t u32Index = 0;
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    uint32_t au32VALID[MAX_PIN_NUM] = {PSIO_ISSTS_VALID0_Msk, PSIO_ISSTS_VALID1_Msk, PSIO_ISSTS_VALID2_Msk, PSIO_ISSTS_VALID3_Msk
                                       , PSIO_ISSTS_VALID4_Msk, PSIO_ISSTS_VALID5_Msk, PSIO_ISSTS_VALID6_Msk, PSIO_ISSTS_VALID7_Msk
                                      };
    uint32_t au32INSTSOV[MAX_PIN_NUM] = {PSIO_ISSTS_INSTSOV0_Msk, PSIO_ISSTS_INSTSOV1_Msk, PSIO_ISSTS_INSTSOV2_Msk, PSIO_ISSTS_INSTSOV3_Msk
                                         , PSIO_ISSTS_INSTSOV4_Msk, PSIO_ISSTS_INSTSOV5_Msk, PSIO_ISSTS_INSTSOV6_Msk, PSIO_ISSTS_INSTSOV7_Msk
                                        };
#elif defined(M252_D)
    uint32_t au32VALID[MAX_PIN_NUM] = {PSIO_ISSTS_VALID0_Msk, PSIO_ISSTS_VALID1_Msk, PSIO_ISSTS_VALID2_Msk, PSIO_ISSTS_VALID3_Msk};
    uint32_t au32INSTSOV[MAX_PIN_NUM] = {PSIO_ISSTS_INSTSOV0_Msk, PSIO_ISSTS_INSTSOV1_Msk, PSIO_ISSTS_INSTSOV2_Msk, PSIO_ISSTS_INSTSOV3_Msk};

#else
    uint32_t au32VALID[MAX_PIN_NUM] = {PSIO_ISSTS_VALID0_Msk, PSIO_ISSTS_VALID1_Msk, PSIO_ISSTS_VALID2_Msk, PSIO_ISSTS_VALID3_Msk
                                       , PSIO_ISSTS_VALID4_Msk, PSIO_ISSTS_VALID5_Msk, PSIO_ISSTS_VALID6_Msk, PSIO_ISSTS_VALID7_Msk
                                      };
    uint32_t au32INSTSOV[MAX_PIN_NUM] = {PSIO_ISSTS_INSTSOV0_Msk, PSIO_ISSTS_INSTSOV1_Msk, PSIO_ISSTS_INSTSOV2_Msk, PSIO_ISSTS_INSTSOV3_Msk
                                         , PSIO_ISSTS_INSTSOV4_Msk, PSIO_ISSTS_INSTSOV5_Msk, PSIO_ISSTS_INSTSOV6_Msk, PSIO_ISSTS_INSTSOV7_Msk
                                        };
#endif

    /*******************************************************************/
    /****                PSIO_GET_TRANSFER_STATUS                   ****/
    /*******************************************************************/
    for (u32Index = 0; u32Index < MAX_PIN_NUM; u32Index++) //Output empty, output under flow
    {
        /* Reset PSIO */
        PSIO_IPRESET();

        PSIO_SET_GENCTL(PSIO, au32PIN[u32Index], PSIO_PIN_ENABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP0, PSIO_SLOT0);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32Index], PSIO_CP1, PSIO_SLOT1);

        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT0, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT1, 0xF);

        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP0, PSIO_IN_STATUS_UPDATE);
        PSIO_SET_ACTION(PSIO, au32PIN[u32Index], PSIO_CP1, PSIO_IN_STATUS_UPDATE);

        PSIO_SET_TRIGSRC(PSIO, PSIO_SC0, PSIO_SW_TRIGGER);
        PSIO_START_SC(PSIO, PSIO_SC0);

        while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC0)) {}

        CU_ASSERT_EQUAL(PSIO_GET_INPUT_STATUS_STATE(PSIO, au32VALID[u32Index]), 0x1);
        PSIO_GET_INPUT_STATUS(PSIO, au32PIN[u32Index]);
        CU_ASSERT_EQUAL(PSIO_GET_INPUT_STATUS_STATE(PSIO, au32VALID[u32Index]), 0x0);

        CU_ASSERT_EQUAL(PSIO_GET_INPUT_STATUS_STATE(PSIO, au32INSTSOV[u32Index]), 0x1);
        PSIO_CLEAR_INPUT_STATUS_STATE(PSIO, au32INSTSOV[u32Index]);
        CU_ASSERT_EQUAL(PSIO_GET_INPUT_STATUS_STATE(PSIO, au32INSTSOV[u32Index]), 0x0);

        //Disable PIN
        PSIO_SET_GENCTL(PSIO, au32PIN[u32Index], PSIO_PIN_DISABLE, PSIO_SC0, PSIO_OUTPUT_MODE, PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);
    }
}

void Func_PSIO_PDMA_MACRO()
{
    uint32_t u32SlotCNTIndex = 0, u32PDMACNTIndex = 0;
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    uint32_t au32IPIN[MAX_PIN_NUM] = {PSIO_PDMACTL_IPIN0EN_Msk, PSIO_PDMACTL_IPIN1EN_Msk, PSIO_PDMACTL_IPIN2EN_Msk, PSIO_PDMACTL_IPIN3EN_Msk
                                      , PSIO_PDMACTL_IPIN4EN_Msk, PSIO_PDMACTL_IPIN5EN_Msk, PSIO_PDMACTL_IPIN6EN_Msk, PSIO_PDMACTL_IPIN7EN_Msk
                                     };
    uint32_t au32OPIN[MAX_PIN_NUM] = {PSIO_PDMACTL_OPIN0EN_Msk, PSIO_PDMACTL_OPIN1EN_Msk, PSIO_PDMACTL_OPIN2EN_Msk, PSIO_PDMACTL_OPIN3EN_Msk
                                      , PSIO_PDMACTL_OPIN4EN_Msk, PSIO_PDMACTL_OPIN5EN_Msk, PSIO_PDMACTL_OPIN6EN_Msk, PSIO_PDMACTL_OPIN7EN_Msk
                                     };

#elif defined(M252_D)
    uint32_t au32IPIN[MAX_PIN_NUM] = {PSIO_PDMACTL_IPIN0EN_Msk, PSIO_PDMACTL_IPIN1EN_Msk, PSIO_PDMACTL_IPIN2EN_Msk, PSIO_PDMACTL_IPIN3EN_Msk};
    uint32_t au32OPIN[MAX_PIN_NUM] = {PSIO_PDMACTL_OPIN0EN_Msk, PSIO_PDMACTL_OPIN1EN_Msk, PSIO_PDMACTL_OPIN2EN_Msk, PSIO_PDMACTL_OPIN3EN_Msk};

#else
    uint32_t au32IPIN[MAX_PIN_NUM] = {PSIO_PDMACTL_IPIN0EN_Msk, PSIO_PDMACTL_IPIN1EN_Msk, PSIO_PDMACTL_IPIN2EN_Msk, PSIO_PDMACTL_IPIN3EN_Msk
                                      , PSIO_PDMACTL_IPIN4EN_Msk, PSIO_PDMACTL_IPIN5EN_Msk, PSIO_PDMACTL_IPIN6EN_Msk, PSIO_PDMACTL_IPIN7EN_Msk
                                     };
    uint32_t au32OPIN[MAX_PIN_NUM] = {PSIO_PDMACTL_OPIN0EN_Msk, PSIO_PDMACTL_OPIN1EN_Msk, PSIO_PDMACTL_OPIN2EN_Msk, PSIO_PDMACTL_OPIN3EN_Msk
                                      , PSIO_PDMACTL_OPIN4EN_Msk, PSIO_PDMACTL_OPIN5EN_Msk, PSIO_PDMACTL_OPIN6EN_Msk, PSIO_PDMACTL_OPIN7EN_Msk
                                     };

#endif

    /* Reset PSIO */
    PSIO_IPRESET();

    for (u32SlotCNTIndex = 0; u32SlotCNTIndex < MAX_SC_NUM; u32SlotCNTIndex++)
    {
        for (u32PDMACNTIndex = 0; u32PDMACNTIndex < MAX_PIN_NUM; u32PDMACNTIndex++)
        {
            //Input
            PSIO_SET_PDMA_INPUT(PSIO, au32SlotCNT[u32SlotCNTIndex], au32IPIN[u32PDMACNTIndex]);
            CU_ASSERT_EQUAL(PSIO->PDMACTL, au32SlotCNT[u32SlotCNTIndex] << 28 | au32IPIN[u32PDMACNTIndex]);

            PSIO_CLEAR_PDMA_INPUT(PSIO, au32IPIN[u32PDMACNTIndex]);
            CU_ASSERT_EQUAL(PSIO->PDMACTL, 0);

            //Output
            PSIO_SET_PDMA_OUTPUT(PSIO, au32SlotCNT[u32SlotCNTIndex], au32OPIN[u32PDMACNTIndex]);
            CU_ASSERT_EQUAL(PSIO->PDMACTL, au32SlotCNT[u32SlotCNTIndex] << 20 | au32OPIN[u32PDMACNTIndex]);

            PSIO_CLEAR_PDMA_OUTPUT(PSIO, au32OPIN[u32PDMACNTIndex]);
            CU_ASSERT_EQUAL(PSIO->PDMACTL, 0);
        }

#if defined(M252_G) | defined(M252_E) | defined(TC8263)
        //Input
        PSIO_SET_PDMA_INPUT(PSIO, au32SlotCNT[u32SlotCNTIndex],
                            PSIO_PDMACTL_IPIN0EN_Msk | PSIO_PDMACTL_IPIN1EN_Msk | PSIO_PDMACTL_IPIN2EN_Msk | PSIO_PDMACTL_IPIN3EN_Msk
                            | PSIO_PDMACTL_IPIN4EN_Msk | PSIO_PDMACTL_IPIN5EN_Msk | PSIO_PDMACTL_IPIN6EN_Msk | PSIO_PDMACTL_IPIN7EN_Msk);
        CU_ASSERT_EQUAL(PSIO->PDMACTL, au32SlotCNT[u32SlotCNTIndex] << 28 | 0xFF00);
        PSIO_CLEAR_PDMA_INPUT(PSIO, PSIO_PDMACTL_IPIN0EN_Msk | PSIO_PDMACTL_IPIN1EN_Msk | PSIO_PDMACTL_IPIN2EN_Msk | PSIO_PDMACTL_IPIN3EN_Msk
                              | PSIO_PDMACTL_IPIN4EN_Msk | PSIO_PDMACTL_IPIN5EN_Msk | PSIO_PDMACTL_IPIN6EN_Msk | PSIO_PDMACTL_IPIN7EN_Msk);
        CU_ASSERT_EQUAL(PSIO->PDMACTL, 0);

        //Output
        PSIO_SET_PDMA_OUTPUT(PSIO, au32SlotCNT[u32SlotCNTIndex],
                             PSIO_PDMACTL_OPIN0EN_Msk | PSIO_PDMACTL_OPIN1EN_Msk | PSIO_PDMACTL_OPIN2EN_Msk | PSIO_PDMACTL_OPIN3EN_Msk
                             | PSIO_PDMACTL_OPIN4EN_Msk | PSIO_PDMACTL_OPIN5EN_Msk | PSIO_PDMACTL_OPIN6EN_Msk | PSIO_PDMACTL_OPIN7EN_Msk);
        CU_ASSERT_EQUAL(PSIO->PDMACTL, au32SlotCNT[u32SlotCNTIndex] << 20 | 0xFF);
        PSIO_CLEAR_PDMA_OUTPUT(PSIO, PSIO_PDMACTL_OPIN0EN_Msk | PSIO_PDMACTL_OPIN1EN_Msk | PSIO_PDMACTL_OPIN2EN_Msk | PSIO_PDMACTL_OPIN3EN_Msk
                               | PSIO_PDMACTL_OPIN4EN_Msk | PSIO_PDMACTL_OPIN5EN_Msk | PSIO_PDMACTL_OPIN6EN_Msk | PSIO_PDMACTL_OPIN7EN_Msk);
        CU_ASSERT_EQUAL(PSIO->PDMACTL, 0);

#elif defined(M252_D)
        //Input
        PSIO_SET_PDMA_INPUT(PSIO, au32SlotCNT[u32SlotCNTIndex],
                            PSIO_PDMACTL_IPIN0EN_Msk | PSIO_PDMACTL_IPIN1EN_Msk | PSIO_PDMACTL_IPIN2EN_Msk | PSIO_PDMACTL_IPIN3EN_Msk);
        CU_ASSERT_EQUAL(PSIO->PDMACTL, au32SlotCNT[u32SlotCNTIndex] << 28 | 0xF00);
        PSIO_CLEAR_PDMA_INPUT(PSIO, PSIO_PDMACTL_IPIN0EN_Msk | PSIO_PDMACTL_IPIN1EN_Msk | PSIO_PDMACTL_IPIN2EN_Msk | PSIO_PDMACTL_IPIN3EN_Msk);
        CU_ASSERT_EQUAL(PSIO->PDMACTL, 0);

        //Output
        PSIO_SET_PDMA_OUTPUT(PSIO, au32SlotCNT[u32SlotCNTIndex],
                             PSIO_PDMACTL_OPIN0EN_Msk | PSIO_PDMACTL_OPIN1EN_Msk | PSIO_PDMACTL_OPIN2EN_Msk | PSIO_PDMACTL_OPIN3EN_Msk);
        CU_ASSERT_EQUAL(PSIO->PDMACTL, au32SlotCNT[u32SlotCNTIndex] << 20 | 0xF);
        PSIO_CLEAR_PDMA_OUTPUT(PSIO, PSIO_PDMACTL_OPIN0EN_Msk | PSIO_PDMACTL_OPIN1EN_Msk | PSIO_PDMACTL_OPIN2EN_Msk | PSIO_PDMACTL_OPIN3EN_Msk);
        CU_ASSERT_EQUAL(PSIO->PDMACTL, 0);

#else
        //Input
        PSIO_SET_PDMA_INPUT(PSIO, au32SlotCNT[u32SlotCNTIndex],
                            PSIO_PDMACTL_IPIN0EN_Msk | PSIO_PDMACTL_IPIN1EN_Msk | PSIO_PDMACTL_IPIN2EN_Msk | PSIO_PDMACTL_IPIN3EN_Msk
                            | PSIO_PDMACTL_IPIN4EN_Msk | PSIO_PDMACTL_IPIN5EN_Msk | PSIO_PDMACTL_IPIN6EN_Msk | PSIO_PDMACTL_IPIN7EN_Msk);
        CU_ASSERT_EQUAL(PSIO->PDMACTL, au32SlotCNT[u32SlotCNTIndex] << 28 | 0xFF00);
        PSIO_CLEAR_PDMA_INPUT(PSIO, PSIO_PDMACTL_IPIN0EN_Msk | PSIO_PDMACTL_IPIN1EN_Msk | PSIO_PDMACTL_IPIN2EN_Msk | PSIO_PDMACTL_IPIN3EN_Msk
                              | PSIO_PDMACTL_IPIN4EN_Msk | PSIO_PDMACTL_IPIN5EN_Msk | PSIO_PDMACTL_IPIN6EN_Msk | PSIO_PDMACTL_IPIN7EN_Msk);
        CU_ASSERT_EQUAL(PSIO->PDMACTL, 0);

        //Output
        PSIO_SET_PDMA_OUTPUT(PSIO, au32SlotCNT[u32SlotCNTIndex],
                             PSIO_PDMACTL_OPIN0EN_Msk | PSIO_PDMACTL_OPIN1EN_Msk | PSIO_PDMACTL_OPIN2EN_Msk | PSIO_PDMACTL_OPIN3EN_Msk
                             | PSIO_PDMACTL_OPIN4EN_Msk | PSIO_PDMACTL_OPIN5EN_Msk | PSIO_PDMACTL_OPIN6EN_Msk | PSIO_PDMACTL_OPIN7EN_Msk);
        CU_ASSERT_EQUAL(PSIO->PDMACTL, au32SlotCNT[u32SlotCNTIndex] << 20 | 0xFF);
        PSIO_CLEAR_PDMA_OUTPUT(PSIO, PSIO_PDMACTL_OPIN0EN_Msk | PSIO_PDMACTL_OPIN1EN_Msk | PSIO_PDMACTL_OPIN2EN_Msk | PSIO_PDMACTL_OPIN3EN_Msk
                               | PSIO_PDMACTL_OPIN4EN_Msk | PSIO_PDMACTL_OPIN5EN_Msk | PSIO_PDMACTL_OPIN6EN_Msk | PSIO_PDMACTL_OPIN7EN_Msk);
        CU_ASSERT_EQUAL(PSIO->PDMACTL, 0);

#endif
    }
}

void Func_PSIO_SET_SCCTL()
{
    uint32_t u32SlotCNTIndex = 0, u32SrcIndex = 0, u32SlotIndex = 0, u32LoopCNT = 0;
    uint32_t au32TRIGSRC[4] = {PSIO_SW_TRIGGER, PSIO_FALLING_TRIGGER, PSIO_RISING_TRIGGER, PSIO_BOTH_EDGE_TRIGGER};

    for (u32SlotCNTIndex = 0; u32SlotCNTIndex < MAX_SC_NUM; u32SlotCNTIndex++)
    {
        for (u32SrcIndex = 0; u32SrcIndex < 4; u32SrcIndex++)
        {
            PSIO_SET_TRIGSRC(PSIO, au32SlotCNT[u32SlotCNTIndex], au32TRIGSRC[u32SrcIndex]);
            CU_ASSERT_EQUAL(PSIO->SCCT[au32SlotCNT[u32SlotCNTIndex]].SCCTL & PSIO_SCCT_SCCTL_TRIGSRC_Msk, au32TRIGSRC[u32SrcIndex]);
        }
    }

    for (u32SlotCNTIndex = 0; u32SlotCNTIndex < MAX_SC_NUM; u32SlotCNTIndex++)
    {
        for (u32SlotIndex = 0; u32SlotIndex < 8; u32SlotIndex++)
        {
            for (u32LoopCNT = 0; u32LoopCNT < 0x40; u32LoopCNT++)
            {
                /* Reset PSIO */
                PSIO_IPRESET();

                PSIO_SET_SCCTL(PSIO, au32SlotCNT[u32SlotCNTIndex], au32Slot[u32SlotIndex], au32Slot[u32SlotIndex], u32LoopCNT, PSIO_REPEAT_ENABLE);
                CU_ASSERT_EQUAL(PSIO->SCCT[au32SlotCNT[u32SlotCNTIndex]].SCCTL, \
                                PSIO_SCCT_SCCTL_IDLE_Msk | PSIO_SCCT_SCCTL_REPEAT_Msk | (u32LoopCNT << PSIO_SCCT_SCCTL_SPLCNT_Pos) \
                                | ((au32Slot[u32SlotIndex]) << PSIO_SCCT_SCCTL_ENDSLOT_Pos) | au32Slot[u32SlotIndex]);
            }
        }
    }

    for (u32SlotCNTIndex = 0; u32SlotCNTIndex < MAX_SC_NUM; u32SlotCNTIndex++)
    {
        for (u32SlotIndex = 0; u32SlotIndex < 8; u32SlotIndex++)
        {
            for (u32LoopCNT = 0; u32LoopCNT < 0x40; u32LoopCNT++)
            {
                /* Reset PSIO */
                PSIO_IPRESET();

                PSIO_SET_SCCTL(PSIO, au32SlotCNT[u32SlotCNTIndex], au32Slot[u32SlotIndex], au32Slot[u32SlotIndex], u32LoopCNT, PSIO_REPEAT_DISABLE);
                CU_ASSERT_EQUAL(PSIO->SCCT[au32SlotCNT[u32SlotCNTIndex]].SCCTL, \
                                PSIO_SCCT_SCCTL_IDLE_Msk | (u32LoopCNT << PSIO_SCCT_SCCTL_SPLCNT_Pos) \
                                | ((au32Slot[u32SlotIndex]) << PSIO_SCCT_SCCTL_ENDSLOT_Pos) | au32Slot[u32SlotIndex]);
            }
        }
    }
}

void Func_PSIO_SET_GENCTL()
{
    uint32_t u32SlotCNTIndex = 0, u32PinIndex = 0, u32ModeIndex = 0, u32ActionIndex = 0, u32CPIndex = 0;
    uint32_t au32IOMode[4] = {PSIO_INPUT_MODE, PSIO_OUTPUT_MODE, PSIO_OPENDRAIN_MODE, PSIO_QUASI_MODE};
    uint32_t au32Action[4] = {PSIO_LOW_LEVEL, PSIO_HIGH_LEVEL, PSIO_LAST_OUTPUT, PSIO_Toggle};

    //Enable pin
    for (u32SlotCNTIndex = 0; u32SlotCNTIndex < MAX_SC_NUM; u32SlotCNTIndex++)
    {
        for (u32PinIndex = 0; u32PinIndex < 8; u32PinIndex++)
        {
            for (u32ModeIndex = 0; u32ModeIndex < 4; u32ModeIndex++)
            {
                for (u32ActionIndex = 0; u32ActionIndex < 4; u32ActionIndex++)
                {
                    /* Reset PSIO */
                    PSIO_IPRESET();

                    PSIO_SET_GENCTL(PSIO, au32PIN[u32PinIndex], PSIO_PIN_ENABLE, au32SlotCNT[u32SlotCNTIndex], au32IOMode[u32ModeIndex], au32Action[u32ActionIndex], au32Action[u32ActionIndex]);
                    CU_ASSERT_EQUAL(PSIO->GNCT[au32PIN[u32PinIndex]].GENCTL,
                                    PSIO_GNCT_GENCTL_PINEN_Msk | au32SlotCNT[u32SlotCNTIndex] << PSIO_GNCT_GENCTL_SCSEL_Pos | au32Action[u32ActionIndex] << PSIO_GNCT_GENCTL_INTERVAL_Pos
                                    | au32Action[u32ActionIndex] << PSIO_GNCT_GENCTL_INITIAL_Pos | au32IOMode[u32ModeIndex]);
                }
            }
        }
    }

    //Disable pin
    for (u32SlotCNTIndex = 0; u32SlotCNTIndex < MAX_SC_NUM; u32SlotCNTIndex++)
    {
        for (u32PinIndex = 0; u32PinIndex < 8; u32PinIndex++)
        {
            for (u32ModeIndex = 0; u32ModeIndex < 4; u32ModeIndex++)
            {
                for (u32ActionIndex = 0; u32ActionIndex < 4; u32ActionIndex++)
                {
                    /* Reset PSIO */
                    PSIO_IPRESET();

                    PSIO_SET_GENCTL(PSIO, au32PIN[u32PinIndex], PSIO_PIN_DISABLE, au32SlotCNT[u32SlotCNTIndex], au32IOMode[u32ModeIndex], au32Action[u32ActionIndex], au32Action[u32ActionIndex]);
                    CU_ASSERT_EQUAL(PSIO->GNCT[au32PIN[u32PinIndex]].GENCTL,
                                    au32SlotCNT[u32SlotCNTIndex] << PSIO_GNCT_GENCTL_SCSEL_Pos | au32Action[u32ActionIndex] << PSIO_GNCT_GENCTL_INTERVAL_Pos
                                    | au32Action[u32ActionIndex] << PSIO_GNCT_GENCTL_INITIAL_Pos | au32IOMode[u32ModeIndex]);
                }
            }
        }
    }

    //Switch point 0
    for (u32PinIndex = 0; u32PinIndex < MAX_PIN_NUM; u32PinIndex++)
    {
        for (u32ModeIndex = 0; u32ModeIndex < 4; u32ModeIndex++)
        {
            for (u32CPIndex = 0; u32CPIndex < 8; u32CPIndex++)
            {
                /* Reset PSIO */
                PSIO_IPRESET();

                PSIO_SWITCH_MODE(PSIO, au32PIN[u32PinIndex], PSIO_SWITCH_P0, au32IOMode[u32ModeIndex], au32CP[u32CPIndex]);
                CU_ASSERT_EQUAL(PSIO->GNCT[au32PIN[u32PinIndex]].GENCTL, au32IOMode[u32ModeIndex] << PSIO_GNCT_GENCTL_MODESW0_Pos | (au32CP[u32CPIndex] + 1) << PSIO_GNCT_GENCTL_SW0CP_Pos);
            }
        }
    }

    //Switch point 1
    for (u32PinIndex = 0; u32PinIndex < MAX_PIN_NUM; u32PinIndex++)
    {
        for (u32ModeIndex = 0; u32ModeIndex < 4; u32ModeIndex++)
        {
            for (u32CPIndex = 0; u32CPIndex < 8; u32CPIndex++)
            {
                /* Reset PSIO */
                PSIO_IPRESET();

                PSIO_SWITCH_MODE(PSIO, au32PIN[u32PinIndex], PSIO_SWITCH_P1, au32IOMode[u32ModeIndex], au32CP[u32CPIndex]);
                CU_ASSERT_EQUAL(PSIO->GNCT[au32PIN[u32PinIndex]].GENCTL, au32IOMode[u32ModeIndex] << PSIO_GNCT_GENCTL_MODESW1_Pos | (au32CP[u32CPIndex] + 1) << PSIO_GNCT_GENCTL_SW1CP_Pos);
            }
        }
    }
}

void Func_PSIO_SET_DATCTL()
{
    uint32_t u32PinIndex = 0, u32InWidth = 0, u32OutWidth = 0, u32DepthIndex = 0;
    uint32_t au32IOMode[4] = {PSIO_INPUT_MODE, PSIO_OUTPUT_MODE, PSIO_OPENDRAIN_MODE, PSIO_QUASI_MODE};
    uint32_t au32Depth[4] = {PSIO_DEPTH1, PSIO_DEPTH2, PSIO_DEPTH3, PSIO_DEPTH4};

    //Width:1~32
    for (u32PinIndex = 0; u32PinIndex < MAX_PIN_NUM; u32PinIndex++)
    {
        for (u32InWidth = 1; u32InWidth < 33; u32InWidth++)
        {
            for (u32OutWidth = 1; u32OutWidth < 33; u32OutWidth++)
            {
                /* Reset PSIO */
                PSIO_IPRESET();

                PSIO_SET_WIDTH(PSIO, au32PIN[u32PinIndex], u32InWidth, u32OutWidth);
                CU_ASSERT_EQUAL(PSIO->GNCT[au32PIN[u32PinIndex]].DATCTL & (PSIO_GNCT_DATCTL_INDATWD_Msk | PSIO_GNCT_DATCTL_OUTDATWD_Msk)
                                , (u32InWidth - 1) << PSIO_GNCT_DATCTL_INDATWD_Pos | (u32OutWidth - 1));
            }
        }
    }

    //Width:0
    for (u32PinIndex = 0; u32PinIndex < MAX_PIN_NUM; u32PinIndex++)
    {
        /* Reset PSIO */
        PSIO_IPRESET();

        PSIO_SET_WIDTH(PSIO, au32PIN[u32PinIndex], 0, 0);
        CU_ASSERT_EQUAL(PSIO->GNCT[au32PIN[u32PinIndex]].DATCTL & (PSIO_GNCT_DATCTL_INDATWD_Msk | PSIO_GNCT_DATCTL_OUTDATWD_Msk), 0);
    }

    //Depth
    for (u32PinIndex = 0; u32PinIndex < MAX_PIN_NUM; u32PinIndex++)
    {
        /* Reset PSIO */
        PSIO_IPRESET();

        CU_ASSERT_EQUAL(PSIO->GNCT[au32PIN[u32PinIndex]].DATCTL, PSIO_GNCT_DATCTL_OUTDEPTH_Msk | PSIO_GNCT_DATCTL_INDEPTH_Msk);
        PSIO_SET_OUTPUT_DEPTH(PSIO, au32PIN[u32PinIndex], au32Depth[u32DepthIndex]);
        PSIO_SET_INPUT_DEPTH(PSIO, au32PIN[u32PinIndex], au32Depth[u32DepthIndex]);
        CU_ASSERT_EQUAL(PSIO->GNCT[au32PIN[u32PinIndex]].DATCTL,
                        au32Depth[u32DepthIndex] << PSIO_GNCT_DATCTL_OUTDEPTH_Pos | au32Depth[u32DepthIndex] << PSIO_GNCT_DATCTL_INDEPTH_Pos);
    }

    //Order
    for (u32PinIndex = 0; u32PinIndex < MAX_PIN_NUM; u32PinIndex++)
    {
        /* Reset PSIO */
        PSIO_IPRESET();

        CU_ASSERT_EQUAL(PSIO->GNCT[au32PIN[u32PinIndex]].DATCTL, PSIO_GNCT_DATCTL_OUTDEPTH_Msk | PSIO_GNCT_DATCTL_INDEPTH_Msk);
        PSIO_SET_ORDER(PSIO, au32PIN[u32PinIndex], PSIO_LSB);
        CU_ASSERT_EQUAL(PSIO->GNCT[au32PIN[u32PinIndex]].DATCTL, 0 << PSIO_GNCT_DATCTL_ORDER_Pos | PSIO_GNCT_DATCTL_OUTDEPTH_Msk | PSIO_GNCT_DATCTL_INDEPTH_Msk);

        PSIO_SET_ORDER(PSIO, au32PIN[u32PinIndex], PSIO_MSB);
        CU_ASSERT_EQUAL(PSIO->GNCT[au32PIN[u32PinIndex]].DATCTL, 1 << PSIO_GNCT_DATCTL_ORDER_Pos | PSIO_GNCT_DATCTL_OUTDEPTH_Msk | PSIO_GNCT_DATCTL_INDEPTH_Msk);
    }

}

void Func_PSIO_SET_CHECKPOINT()
{
    uint32_t u32PinIndex = 0, u32CPIndex = 0, u32SlotIndex = 0;

    for (u32PinIndex = 0; u32PinIndex < MAX_PIN_NUM; u32PinIndex++)
    {
        for (u32CPIndex = 0; u32CPIndex < 8; u32CPIndex++)
        {
            for (u32SlotIndex = 0; u32SlotIndex < 8; u32SlotIndex++)
            {
                /* Reset PSIO */
                PSIO_IPRESET();

                PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], au32CP[u32CPIndex], au32Slot[u32SlotIndex]);
                CU_ASSERT_EQUAL(PSIO->GNCT[au32PIN[u32PinIndex]].CPCTL0, (au32Slot[u32SlotIndex]) << (au32CP[u32CPIndex] * 4));

                PSIO_CLEAR_CHECKPOINT(PSIO, au32PIN[u32PinIndex], au32CP[u32CPIndex]);
                CU_ASSERT_EQUAL(PSIO->GNCT[au32PIN[u32PinIndex]].CPCTL0, 0);
            }
        }
    }
}

void Func_PSIO_SCSLOT_SET_SLOT()
{
    uint32_t u32SCIndex = 0, u32SlotIndex = 0, u32Cnt = 0;

    for (u32SCIndex = 0; u32SCIndex < MAX_SC_NUM; u32SCIndex++)
    {
        for (u32Cnt = 0; u32Cnt < 16; u32Cnt++)
        {
            for (u32SlotIndex = 0; u32SlotIndex < 8; u32SlotIndex++)
            {
                /* Reset PSIO */
                PSIO_IPRESET();

                PSIO_SCSLOT_SET_SLOT(PSIO, au32SlotCNT[u32SCIndex], au32Slot[u32SlotIndex], u32Cnt);
                CU_ASSERT_EQUAL(PSIO->SCCT[au32SlotCNT[u32SCIndex]].SCSLOT, u32Cnt << ((au32Slot[u32SlotIndex] - 1) * 4));
            }

            /* Reset PSIO */
            PSIO_IPRESET();

            PSIO_SCSLOT_SET_ALL_SLOT(PSIO, au32SlotCNT[u32SCIndex], u32Cnt);
            CU_ASSERT_EQUAL(PSIO->SCCT[au32SlotCNT[u32SCIndex]].SCSLOT, u32Cnt | u32Cnt << 4 | u32Cnt << 8 | u32Cnt << 12 | u32Cnt << 16 | u32Cnt << 20 | u32Cnt << 24 | u32Cnt << 28);
        }
    }
}

void Func_PSIO_SET_ACTION()
{
    uint32_t u32PinIndex = 0, u32CPIndex = 0, u32ActionIndex = 0;
    uint32_t au32Action[7] = {PSIO_OUT_LOW, PSIO_OUT_HIGH, PSIO_OUT_BUFFER, PSIO_OUT_TOGGLE, PSIO_IN_BUFFER, PSIO_IN_STATUS, PSIO_IN_STATUS_UPDATE};

    for (u32PinIndex = 0; u32PinIndex < MAX_PIN_NUM; u32PinIndex++)
    {
        for (u32CPIndex = 0; u32CPIndex < 8; u32CPIndex++)
        {
            for (u32ActionIndex = 0; u32ActionIndex < 7; u32ActionIndex++)
            {
                /* Reset PSIO */
                PSIO_IPRESET();

                PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], au32CP[u32CPIndex], au32Action[u32ActionIndex]);
                CU_ASSERT_EQUAL(PSIO->GNCT[au32PIN[u32PinIndex]].CPCTL1, au32Action[u32ActionIndex] << au32CP[u32CPIndex] * 4);
            }
        }
    }

}

void Func_PSIO_SET_INTCTL()
{
    uint32_t u32SlotCNTIndex = 0, u32SlotIndex = 0;

    for (u32SlotCNTIndex = 0; u32SlotCNTIndex < MAX_SC_NUM; u32SlotCNTIndex++)
    {
        for (u32SlotIndex = 0; u32SlotIndex < 8; u32SlotIndex++)
        {
            /* Reset PSIO */
            PSIO_IPRESET();

            PSIO_SET_INTCTL(PSIO, au32SlotCNT[u32SlotCNTIndex], PSIO_INT0, au32Slot[u32SlotIndex]);
            CU_ASSERT_EQUAL(PSIO->INTCTL, au32SlotCNT[u32SlotCNTIndex] << 8 | au32Slot[u32SlotIndex]);
        }
    }

    for (u32SlotCNTIndex = 0; u32SlotCNTIndex < MAX_SC_NUM; u32SlotCNTIndex++)
    {
        for (u32SlotIndex = 0; u32SlotIndex < 8; u32SlotIndex++)
        {
            /* Reset PSIO */
            PSIO_IPRESET();

            PSIO_SET_INTCTL(PSIO, au32SlotCNT[u32SlotCNTIndex], PSIO_INT1, au32Slot[u32SlotIndex]);
            CU_ASSERT_EQUAL(PSIO->INTCTL, au32SlotCNT[u32SlotCNTIndex] << 12 | (au32Slot[u32SlotIndex]) << 4);
        }
    }
}

void Func_PSIO_GET_INPUT()
{
    uint32_t u32PinIndex = 0, u32Mode = 0;
    uint32_t au32Mode[2] = {PSIO_OUTPUT_MODE, PSIO_INPUT_MODE};
    uint32_t au32Action[2] = {PSIO_OUT_BUFFER, PSIO_IN_BUFFER};

    /* Pin0, 2, 4, 6 as output, Pin1 ,3 ,5 ,7 as input*/
    /* Reset PSIO */
    PSIO_IPRESET();

    //Enable PIN loopback function
    PSIO->INTCTL |= PSIO_INTCTL_LOOPBACK_Msk;

    for (u32PinIndex = 0; u32PinIndex < MAX_PIN_NUM; u32PinIndex++)
    {
        PSIO_SET_GENCTL(PSIO, au32PIN[u32PinIndex], PSIO_PIN_ENABLE, PSIO_SC0, au32Mode[u32PinIndex % 2], PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP0, PSIO_SLOT0);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP1, PSIO_SLOT1);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP2, PSIO_SLOT2);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP3, PSIO_SLOT3);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP4, PSIO_SLOT4);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP5, PSIO_SLOT5);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP6, PSIO_SLOT6);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP7, PSIO_SLOT7);

        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT0, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT1, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT2, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT3, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT4, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT5, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT6, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT7, 0xF);

        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP0, au32Action[u32PinIndex % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP1, au32Action[u32PinIndex % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP2, au32Action[u32PinIndex % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP3, au32Action[u32PinIndex % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP4, au32Action[u32PinIndex % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP5, au32Action[u32PinIndex % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP6, au32Action[u32PinIndex % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP7, au32Action[u32PinIndex % 2]);

        PSIO_SET_WIDTH(PSIO, au32PIN[u32PinIndex], 32, 32);

        PSIO_SET_SCCTL(PSIO, PSIO_SC0, PSIO_SLOT0, PSIO_SLOT7, 3, PSIO_REPEAT_DISABLE);
    }

    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN0, 0xA5A5A5A5);
    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN2, 0xA5A5A5A5);
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN4, 0xA5A5A5A5);
    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN6, 0xA5A5A5A5);
#endif

    PSIO_SET_TRIGSRC(PSIO, PSIO_SC0, PSIO_SW_TRIGGER);
    PSIO_START_SC(PSIO, PSIO_SC0);

    while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC0)) {}


    CU_ASSERT_EQUAL(PSIO_GET_INPUT_DATA(PSIO, PSIO_PIN1), 0xA5A5A5A5);
    CU_ASSERT_EQUAL(PSIO_GET_INPUT_DATA(PSIO, PSIO_PIN3), 0xA5A5A5A5);
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    CU_ASSERT_EQUAL(PSIO_GET_INPUT_DATA(PSIO, PSIO_PIN5), 0xA5A5A5A5);
    CU_ASSERT_EQUAL(PSIO_GET_INPUT_DATA(PSIO, PSIO_PIN7), 0xA5A5A5A5);
#endif

    /* Pin0, 2, 4, 6 as input, Pin1 ,3 ,5 ,7 as output*/
    /* Reset PSIO */
    PSIO_IPRESET();

    //Enable PIN loopback function
    PSIO->INTCTL |= PSIO_INTCTL_LOOPBACK_Msk;

    for (u32PinIndex = 0; u32PinIndex < MAX_PIN_NUM; u32PinIndex++)
    {
        PSIO_SET_GENCTL(PSIO, au32PIN[u32PinIndex], PSIO_PIN_ENABLE, PSIO_SC0, au32Mode[(u32PinIndex + 1) % 2], PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP0, PSIO_SLOT0);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP1, PSIO_SLOT1);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP2, PSIO_SLOT2);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP3, PSIO_SLOT3);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP4, PSIO_SLOT4);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP5, PSIO_SLOT5);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP6, PSIO_SLOT6);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP7, PSIO_SLOT7);

        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT0, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT1, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT2, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT3, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT4, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT5, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT6, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT7, 0xF);

        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP0, au32Action[(u32PinIndex + 1) % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP1, au32Action[(u32PinIndex + 1) % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP2, au32Action[(u32PinIndex + 1) % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP3, au32Action[(u32PinIndex + 1) % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP4, au32Action[(u32PinIndex + 1) % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP5, au32Action[(u32PinIndex + 1) % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP6, au32Action[(u32PinIndex + 1) % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP7, au32Action[(u32PinIndex + 1) % 2]);

        PSIO_SET_WIDTH(PSIO, au32PIN[u32PinIndex], 32, 32);

        PSIO_SET_SCCTL(PSIO, PSIO_SC0, PSIO_SLOT0, PSIO_SLOT7, 3, PSIO_REPEAT_DISABLE);
    }

    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN1, 0x5A5A5A5A);
    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN3, 0x5A5A5A5A);
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN5, 0x5A5A5A5A);
    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN7, 0x5A5A5A5A);
#endif

    PSIO_SET_TRIGSRC(PSIO, PSIO_SC0, PSIO_SW_TRIGGER);
    PSIO_START_SC(PSIO, PSIO_SC0);

    while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC0)) {}


    CU_ASSERT_EQUAL(PSIO_GET_INPUT_DATA(PSIO, PSIO_PIN0), 0x5A5A5A5A);
    CU_ASSERT_EQUAL(PSIO_GET_INPUT_DATA(PSIO, PSIO_PIN2), 0x5A5A5A5A);
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    CU_ASSERT_EQUAL(PSIO_GET_INPUT_DATA(PSIO, PSIO_PIN4), 0x5A5A5A5A);
    CU_ASSERT_EQUAL(PSIO_GET_INPUT_DATA(PSIO, PSIO_PIN6), 0x5A5A5A5A);
#endif
}

void Func_PSIO_GET_INPUT_STATUS()
{
    uint32_t u32PinIndex = 0, u32Mode = 0;
    uint32_t au32Mode[2] = {PSIO_OUTPUT_MODE, PSIO_INPUT_MODE};
    uint32_t au32Action[2] = {PSIO_OUT_BUFFER, PSIO_IN_STATUS};

    /* Pin0, 2, 4, 6 as output, Pin1 ,3 ,5 ,7 as input*/
    /* Reset PSIO */
    PSIO_IPRESET();

    //Enable PIN loopback function
    PSIO->INTCTL |= PSIO_INTCTL_LOOPBACK_Msk;

    for (u32PinIndex = 0; u32PinIndex < MAX_PIN_NUM; u32PinIndex++)
    {
        PSIO_SET_GENCTL(PSIO, au32PIN[u32PinIndex], PSIO_PIN_ENABLE, PSIO_SC0, au32Mode[u32PinIndex % 2], PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP0, PSIO_SLOT0);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP1, PSIO_SLOT1);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP2, PSIO_SLOT2);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP3, PSIO_SLOT3);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP4, PSIO_SLOT4);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP5, PSIO_SLOT5);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP6, PSIO_SLOT6);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP7, PSIO_SLOT7);

        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT0, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT1, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT2, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT3, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT4, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT5, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT6, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT7, 0xF);

        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP0, au32Action[u32PinIndex % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP1, au32Action[u32PinIndex % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP2, au32Action[u32PinIndex % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP3, au32Action[u32PinIndex % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP4, au32Action[u32PinIndex % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP5, au32Action[u32PinIndex % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP6, au32Action[u32PinIndex % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP7, u32PinIndex % 2 ? PSIO_IN_STATUS_UPDATE : PSIO_OUT_BUFFER);

        PSIO_SET_WIDTH(PSIO, au32PIN[u32PinIndex], 8, 8);

        //        PSIO_SET_SCCTL(PSIO, PSIO_SC0, PSIO_SLOT0, PSIO_SLOT7, 3, PSIO_REPEAT_DISABLE);
    }

    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN0, 0xA5A5A5A5);
    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN2, 0xA5A5A5A5);
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN4, 0xA5A5A5A5);
    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN6, 0xA5A5A5A5);
#endif

    PSIO_SET_TRIGSRC(PSIO, PSIO_SC0, PSIO_SW_TRIGGER);
    PSIO_START_SC(PSIO, PSIO_SC0);

    while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC0)) {}


    CU_ASSERT_EQUAL(PSIO_GET_INPUT_STATUS(PSIO, PSIO_PIN1), 0xA5);
    CU_ASSERT_EQUAL(PSIO_GET_INPUT_STATUS(PSIO, PSIO_PIN3), 0xA5);
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    CU_ASSERT_EQUAL(PSIO_GET_INPUT_STATUS(PSIO, PSIO_PIN5), 0xA5);
    CU_ASSERT_EQUAL(PSIO_GET_INPUT_STATUS(PSIO, PSIO_PIN7), 0xA5);
#endif

    /* Pin0, 2, 4, 6 as input, Pin1 ,3 ,5 ,7 as output*/
    /* Reset PSIO */
    PSIO_IPRESET();

    //Enable PIN loopback function
    PSIO->INTCTL |= PSIO_INTCTL_LOOPBACK_Msk;

    for (u32PinIndex = 0; u32PinIndex < MAX_PIN_NUM; u32PinIndex++)
    {
        PSIO_SET_GENCTL(PSIO, au32PIN[u32PinIndex], PSIO_PIN_ENABLE, PSIO_SC0, au32Mode[(u32PinIndex + 1) % 2], PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP0, PSIO_SLOT0);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP1, PSIO_SLOT1);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP2, PSIO_SLOT2);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP3, PSIO_SLOT3);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP4, PSIO_SLOT4);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP5, PSIO_SLOT5);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP6, PSIO_SLOT6);
        PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP7, PSIO_SLOT7);

        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT0, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT1, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT2, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT3, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT4, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT5, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT6, 0xF);
        PSIO_SCSLOT_SET_SLOT(PSIO, PSIO_SC0, PSIO_SLOT7, 0xF);

        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP0, au32Action[(u32PinIndex + 1) % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP1, au32Action[(u32PinIndex + 1) % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP2, au32Action[(u32PinIndex + 1) % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP3, au32Action[(u32PinIndex + 1) % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP4, au32Action[(u32PinIndex + 1) % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP5, au32Action[(u32PinIndex + 1) % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP6, au32Action[(u32PinIndex + 1) % 2]);
        PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP7, (u32PinIndex + 1) % 2 ? PSIO_IN_STATUS_UPDATE : PSIO_OUT_BUFFER);

        PSIO_SET_WIDTH(PSIO, au32PIN[u32PinIndex], 8, 8);

        //        PSIO_SET_SCCTL(PSIO, PSIO_SC0, PSIO_SLOT0, PSIO_SLOT7, 3, PSIO_REPEAT_DISABLE);
    }

    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN1, 0x5A5A5A5A);
    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN3, 0x5A5A5A5A);
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN5, 0x5A5A5A5A);
    PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN7, 0x5A5A5A5A);
#endif

    PSIO_SET_TRIGSRC(PSIO, PSIO_SC0, PSIO_SW_TRIGGER);
    PSIO_START_SC(PSIO, PSIO_SC0);

    while (PSIO_GET_BUSY_FLAG(PSIO, PSIO_SC0)) {}


    CU_ASSERT_EQUAL(PSIO_GET_INPUT_STATUS(PSIO, PSIO_PIN0), 0x5A);
    CU_ASSERT_EQUAL(PSIO_GET_INPUT_STATUS(PSIO, PSIO_PIN2), 0x5A);
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
    CU_ASSERT_EQUAL(PSIO_GET_INPUT_STATUS(PSIO, PSIO_PIN4), 0x5A);
    CU_ASSERT_EQUAL(PSIO_GET_INPUT_STATUS(PSIO, PSIO_PIN6), 0x5A);
#endif
}

void Func_PSIO_STOP_SC()
{
    uint32_t u32PinIndex = 0, u32Mode = 0, u32LoopCNT = 0, u32SC = 0;
    uint32_t au32Mode[2] = {PSIO_OUTPUT_MODE, PSIO_INPUT_MODE};
    uint32_t au32Action[2] = {PSIO_OUT_BUFFER, PSIO_IN_BUFFER};

    /* Unlock protected registers */
    SYS_UnlockReg();

    //CLK->CLKDIV1 = (CLK->CLKDIV1 & (~CLK_CLKDIV1_PSIODIV_Msk)) | ((2) << CLK_CLKDIV1_PSIODIV_Pos);
    CLK_SetModuleClock(PSIO0_MODULE, CLK_PSIOSEL_PSIO0SEL_HIRC, CLK_PSIODIV_PSIO0DIV(2));

    /* Lock protected registers */
    SYS_LockReg();

    for (u32SC = 0; u32SC < MAX_SC_NUM; u32SC++)
    {
        /* Reset PSIO */
        PSIO_IPRESET();

        //Enable PIN loopback function
        PSIO->INTCTL |= PSIO_INTCTL_LOOPBACK_Msk;

        for (u32PinIndex = 0; u32PinIndex < MAX_PIN_NUM; u32PinIndex++)
        {
            PSIO_SET_GENCTL(PSIO, au32PIN[u32PinIndex], PSIO_PIN_ENABLE, au32SlotCNT[u32SC], au32Mode[u32PinIndex % 2], PSIO_LOW_LEVEL, PSIO_LOW_LEVEL);

            PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP0, PSIO_SLOT0);
            PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP1, PSIO_SLOT1);
            PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP2, PSIO_SLOT2);
            PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP3, PSIO_SLOT3);
            PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP4, PSIO_SLOT4);
            PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP5, PSIO_SLOT5);
            PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP6, PSIO_SLOT6);
            PSIO_SET_CHECKPOINT(PSIO, au32PIN[u32PinIndex], PSIO_CP7, PSIO_SLOT7);

            PSIO_SCSLOT_SET_SLOT(PSIO, au32SlotCNT[u32SC], PSIO_SLOT0, 0xF);
            PSIO_SCSLOT_SET_SLOT(PSIO, au32SlotCNT[u32SC], PSIO_SLOT1, 0xF);
            PSIO_SCSLOT_SET_SLOT(PSIO, au32SlotCNT[u32SC], PSIO_SLOT2, 0xF);
            PSIO_SCSLOT_SET_SLOT(PSIO, au32SlotCNT[u32SC], PSIO_SLOT3, 0xF);
            PSIO_SCSLOT_SET_SLOT(PSIO, au32SlotCNT[u32SC], PSIO_SLOT4, 0xF);
            PSIO_SCSLOT_SET_SLOT(PSIO, au32SlotCNT[u32SC], PSIO_SLOT5, 0xF);
            PSIO_SCSLOT_SET_SLOT(PSIO, au32SlotCNT[u32SC], PSIO_SLOT6, 0xF);
            PSIO_SCSLOT_SET_SLOT(PSIO, au32SlotCNT[u32SC], PSIO_SLOT7, 0xF);

            PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP0, au32Action[u32PinIndex % 2]);
            PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP1, au32Action[u32PinIndex % 2]);
            PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP2, au32Action[u32PinIndex % 2]);
            PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP3, au32Action[u32PinIndex % 2]);
            PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP4, au32Action[u32PinIndex % 2]);
            PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP5, au32Action[u32PinIndex % 2]);
            PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP6, au32Action[u32PinIndex % 2]);
            PSIO_SET_ACTION(PSIO, au32PIN[u32PinIndex], PSIO_CP7, au32Action[u32PinIndex % 2]);

            PSIO_SET_WIDTH(PSIO, au32PIN[u32PinIndex], 32, 32);

            PSIO_SET_SCCTL(PSIO, au32SlotCNT[u32SC], 0, 0, 0, PSIO_REPEAT_ENABLE);
        }

        PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN0, 0xA5A5A5A5);
        PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN2, 0xA5A5A5A5);
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
        PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN4, 0xA5A5A5A5);
        PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN6, 0xA5A5A5A5);
#endif

        PSIO_SET_TRIGSRC(PSIO, au32SlotCNT[u32SC], PSIO_SW_TRIGGER);
        PSIO_START_SC(PSIO, au32SlotCNT[u32SC]);

        while (1)
        {
            while (PSIO_GET_TRANSFER_STATUS(PSIO, PSIO_TRANSTS_OUTEPY0_Msk) == 0);

            while (PSIO_GET_TRANSFER_STATUS(PSIO, PSIO_TRANSTS_OUTEPY2_Msk) == 0);

#if defined(M252_G) | defined(M252_E) | defined(TC8263)

            while (PSIO_GET_TRANSFER_STATUS(PSIO, PSIO_TRANSTS_OUTEPY4_Msk) == 0);

            while (PSIO_GET_TRANSFER_STATUS(PSIO, PSIO_TRANSTS_OUTEPY6_Msk) == 0);

#endif

            PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN0, 0xA5A5A5A5);
            PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN2, 0xA5A5A5A5);
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
            PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN4, 0xA5A5A5A5);
            PSIO_SET_OUTPUT_DATA(PSIO, PSIO_PIN6, 0xA5A5A5A5);
#endif

            while (PSIO_GET_TRANSFER_STATUS(PSIO, PSIO_TRANSTS_INFULL1_Msk) == 0) {}

            while (PSIO_GET_TRANSFER_STATUS(PSIO, PSIO_TRANSTS_INFULL3_Msk) == 0) {}

#if defined(M252_G) | defined(M252_E) | defined(TC8263)

            while (PSIO_GET_TRANSFER_STATUS(PSIO, PSIO_TRANSTS_INFULL5_Msk) == 0) {}

            while (PSIO_GET_TRANSFER_STATUS(PSIO, PSIO_TRANSTS_INFULL7_Msk) == 0) {}

#endif

            CU_ASSERT_EQUAL(PSIO_GET_INPUT_DATA(PSIO, PSIO_PIN1), 0xA5A5A5A5);
            CU_ASSERT_EQUAL(PSIO_GET_INPUT_DATA(PSIO, PSIO_PIN3), 0xA5A5A5A5);
#if defined(M252_G) | defined(M252_E) | defined(TC8263)
            CU_ASSERT_EQUAL(PSIO_GET_INPUT_DATA(PSIO, PSIO_PIN5), 0xA5A5A5A5);
            CU_ASSERT_EQUAL(PSIO_GET_INPUT_DATA(PSIO, PSIO_PIN7), 0xA5A5A5A5);
#endif

            if (u32LoopCNT > 10)
            {
                PSIO_STOP_SC(PSIO, au32SlotCNT[u32SC]);
                CU_ASSERT_EQUAL(PSIO->SCCT[au32SlotCNT[u32SC]].SCCTL & PSIO_SCCT_SCCTL_START_Msk, 0x0);
                u32LoopCNT = 0;
                break;
            }

            u32LoopCNT++;
        }
    }

    /* Unlock protected registers */
    SYS_UnlockReg();

    //CLK->CLKDIV1 = (CLK->CLKDIV1 & (~CLK_CLKDIV1_PSIODIV_Msk)) | ((0) << CLK_CLKDIV1_PSIODIV_Pos);
    CLK_SetModuleClock(PSIO0_MODULE, CLK_PSIOSEL_PSIO0SEL_HIRC, CLK_PSIODIV_PSIO0DIV(0));

    /* Lock protected registers */
    SYS_LockReg();
}

void Func_PSIO_CONSTANT()
{
    /* Slot controller */
    CU_ASSERT_EQUAL(PSIO_SC0, 0);
    CU_ASSERT_EQUAL(PSIO_SC1, 1);
    CU_ASSERT_EQUAL(PSIO_SC2, 2);
    CU_ASSERT_EQUAL(PSIO_SC3, 3);

    /* Interrupt */
    CU_ASSERT_EQUAL(PSIO_INT0, 0);
    CU_ASSERT_EQUAL(PSIO_INT1, 1);

    /* Slot number */
    CU_ASSERT_EQUAL(PSIO_SLOT_DISABLE, 0);
    CU_ASSERT_EQUAL(PSIO_SLOT0, 1);
    CU_ASSERT_EQUAL(PSIO_SLOT1, 2);
    CU_ASSERT_EQUAL(PSIO_SLOT2, 3);
    CU_ASSERT_EQUAL(PSIO_SLOT3, 4);
    CU_ASSERT_EQUAL(PSIO_SLOT4, 5);
    CU_ASSERT_EQUAL(PSIO_SLOT5, 6);
    CU_ASSERT_EQUAL(PSIO_SLOT6, 7);
    CU_ASSERT_EQUAL(PSIO_SLOT7, 8);

    /* Pin number */
    CU_ASSERT_EQUAL(PSIO_PIN0, 0);
    CU_ASSERT_EQUAL(PSIO_PIN1, 1);
    CU_ASSERT_EQUAL(PSIO_PIN2, 2);
    CU_ASSERT_EQUAL(PSIO_PIN3, 3);
    CU_ASSERT_EQUAL(PSIO_PIN4, 4);
    CU_ASSERT_EQUAL(PSIO_PIN5, 5);
    CU_ASSERT_EQUAL(PSIO_PIN6, 6);
    CU_ASSERT_EQUAL(PSIO_PIN7, 7);

    /* Check point number */
    CU_ASSERT_EQUAL(PSIO_PIN0, 0);
    CU_ASSERT_EQUAL(PSIO_PIN1, 1);
    CU_ASSERT_EQUAL(PSIO_PIN2, 2);
    CU_ASSERT_EQUAL(PSIO_PIN3, 3);
    CU_ASSERT_EQUAL(PSIO_PIN4, 4);
    CU_ASSERT_EQUAL(PSIO_PIN5, 5);
    CU_ASSERT_EQUAL(PSIO_PIN6, 6);
    CU_ASSERT_EQUAL(PSIO_PIN7, 7);

    /* Check point number */
    CU_ASSERT_EQUAL(PSIO_PIN_ENABLE, 1);
    CU_ASSERT_EQUAL(PSIO_PIN_DISABLE, 0);

    /* Trigger source */
    CU_ASSERT_EQUAL(PSIO_SW_TRIGGER, 0x0);
    CU_ASSERT_EQUAL(PSIO_FALLING_TRIGGER, 0x4000);
    CU_ASSERT_EQUAL(PSIO_RISING_TRIGGER, 0x8000);
    CU_ASSERT_EQUAL(PSIO_BOTH_EDGE_TRIGGER, 0xc000);

    /* Repeate mode enable */
    CU_ASSERT_EQUAL(PSIO_REPEAT_ENABLE, 1);
    CU_ASSERT_EQUAL(PSIO_REPEAT_DISABLE, 0);

    /* IO mode */
    CU_ASSERT_EQUAL(PSIO_INPUT_MODE, 0);
    CU_ASSERT_EQUAL(PSIO_OUTPUT_MODE, 1);
    CU_ASSERT_EQUAL(PSIO_OPENDRAIN_MODE, 2);
    CU_ASSERT_EQUAL(PSIO_QUASI_MODE, 3);

    /* Pin status */
    CU_ASSERT_EQUAL(PSIO_LOW_LEVEL, 0);
    CU_ASSERT_EQUAL(PSIO_HIGH_LEVEL, 1);
    CU_ASSERT_EQUAL(PSIO_LAST_OUTPUT, 2);
    CU_ASSERT_EQUAL(PSIO_Toggle, 3);

    /* Switch point */
    CU_ASSERT_EQUAL(PSIO_SWITCH_P0, 0);
    CU_ASSERT_EQUAL(PSIO_SWITCH_P1, 1);

    /* Order */
    CU_ASSERT_EQUAL(PSIO_LSB, 0);
    CU_ASSERT_EQUAL(PSIO_MSB, 0x10000);

    /* Depth */
    CU_ASSERT_EQUAL(PSIO_DEPTH1, 0);
    CU_ASSERT_EQUAL(PSIO_DEPTH2, 1);
    CU_ASSERT_EQUAL(PSIO_DEPTH3, 2);
    CU_ASSERT_EQUAL(PSIO_DEPTH4, 3);

    /* Action */
    CU_ASSERT_EQUAL(PSIO_OUT_LOW, 0);
    CU_ASSERT_EQUAL(PSIO_OUT_HIGH, 1);
    CU_ASSERT_EQUAL(PSIO_OUT_BUFFER, 2);
    CU_ASSERT_EQUAL(PSIO_OUT_TOGGLE, 3);
    CU_ASSERT_EQUAL(PSIO_IN_BUFFER, 4);
    CU_ASSERT_EQUAL(PSIO_IN_STATUS, 5);
    CU_ASSERT_EQUAL(PSIO_IN_STATUS_UPDATE, 6);
}

CU_TestInfo PSIO_MACRO[] =
{
    {"Test PSIO_ENABLE_INT /PSIO_DISABLE_INT:", Func_PSIO_ENABLE_INT},
    {
        "Test PSIO_GET_INT_FLAG /PSIO_CLEAR_INT_FLAG /PSIO_GET_BUSY_FLAG /PSIO_GET_IDLE_FLAG /PSIO_SET_IDLE_FLAG /PSIO_CLEAR_IDLE_FLAG :"
        , Func_PSIO_GET_INT_FLAG
    },
    {"Test PSIO_GET_TRANSFER_STATUS /PSIO_CLEAR_TRANSFER_STATUS /PSIO_START_SC:", Func_PSIO_GET_TRANSFER_STATUS},
    {"Test PSIO_GET_INPUT_STATUS_STATE /PSIO_CLEAR_INPUT_STATUS_STATE:", Func_PSIO_GET_INPUT_STATUS_STATE},
    {"Test PSIO_CLEAR_PDMA_INPUT /PSIO_SET_PDMA_OUTPUT /PSIO_CLEAR_PDMA_OUTPUT:", Func_PSIO_PDMA_MACRO},
    {"Test PSIO_SET_TRIGSRC /PSIO_SET_SCCTL :", Func_PSIO_SET_SCCTL},
    {"Test PSIO_SET_GENCTL /PSIO_SWITCH_MODE :", Func_PSIO_SET_GENCTL},
    {"Test PSIO_SET_WIDTH /PSIO_SET_OUTPUT_DEPTH /PSIO_SET_INPUT_DEPTH/PSIO_SET_ORDER :", Func_PSIO_SET_DATCTL},
    {"Test PSIO_SET_CHECKPOINT /PSIO_CLEAR_CHECKPOINT:", Func_PSIO_SET_CHECKPOINT},
    {"Test PSIO_SCSLOT_SET_SLOT /PSIO_SCSLOT_SET_ALL_SLOT:", Func_PSIO_SCSLOT_SET_SLOT},
    {"Test PSIO_SET_ACTION:", Func_PSIO_SET_ACTION},
    {"Test PSIO_SET_INTCTL:", Func_PSIO_SET_INTCTL},
    {"Test PSIO_SET_OUTPUT_DATA/PSIO_GET_INPUT_DATA:", Func_PSIO_GET_INPUT},
    {"Test PSIO_GET_INPUT_STATUS:", Func_PSIO_GET_INPUT_STATUS},
    {"Test PSIO_STOP_SC:", Func_PSIO_STOP_SC},

    CU_TEST_INFO_NULL
};

CU_TestInfo PSIO_API[] =
{

    CU_TEST_INFO_NULL
};

CU_TestInfo PSIO_CONSTANT[] =
{
    {"Test PSIO_CONSTANT:", Func_PSIO_CONSTANT},

    CU_TEST_INFO_NULL
};
