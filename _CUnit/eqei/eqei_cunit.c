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
#include "eqei_cunit.h"

extern void Enable_SelfTest(void);
extern void Disable_SelfTest(void);

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

        {"QEI API", suite_success_init, suite_success_clean, NULL, NULL, QEI_ApiTests},
        {"QEI MACRO", suite_success_init, suite_success_clean, NULL, NULL, QEI_MacroTests},

        CU_SUITE_INFO_NULL};

#define TestChNum 4
uint8_t u8TestChIdx; // QEI channel index
EQEI_T *TestCh[TestChNum] = {
    EQEI0, EQEI1, EQEI2, EQEI3};

void EQEI0_IRQHandler(void)
{
    EQEI0->STATUS = EQEI0->STATUS;
}

void EQEI1_IRQHandler(void)
{
    EQEI1->STATUS = EQEI1->STATUS;
}

void EQEI2_IRQHandler(void)
{
    EQEI2->STATUS = EQEI2->STATUS;
}

void EQEI3_IRQHandler(void)
{
    EQEI3->STATUS = EQEI3->STATUS;
}

const uint32_t au32QeiModeSel[4] = {
    EQEI_CTL_X4_FREE_COUNTING_MODE,
    EQEI_CTL_X2_FREE_COUNTING_MODE,
    EQEI_CTL_X4_COMPARE_COUNTING_MODE,
    EQEI_CTL_X2_COMPARE_COUNTING_MODE,
};

void TestFunc_QEI_Open()
{

    // 1. QEI_Stop()
    // 2. QEI_Open()
    // 3. QEI_Start()
    // 4. QEI_Close()

    uint8_t u8QeiModeSelIdx;

    Enable_SelfTest();

    // main test loop
    for (u8TestChIdx = 0; u8TestChIdx < TestChNum; u8TestChIdx++)
    {
        for (u8QeiModeSelIdx = 0; u8QeiModeSelIdx < 4; u8QeiModeSelIdx++)
        {
            // clear setting before test
            EQEI_Stop(TestCh[u8TestChIdx]);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_EQEIEN_Msk) == 0);
            EQEI_DISABLE_INPUT(TestCh[u8TestChIdx], EQEI_CTL_CHAEN_Msk | EQEI_CTL_CHBEN_Msk | EQEI_CTL_IDXEN_Msk);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & (EQEI_CTL_CHAEN_Msk | EQEI_CTL_CHBEN_Msk | EQEI_CTL_IDXEN_Msk)) == 0);

            // test function
            EQEI_Open(TestCh[u8TestChIdx], au32QeiModeSel[u8QeiModeSelIdx], 0x12345678);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_MODE_Msk) == (u8QeiModeSelIdx << EQEI_CTL_MODE_Pos));
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & (EQEI_CTL_CHAEN_Msk | EQEI_CTL_CHBEN_Msk | EQEI_CTL_IDXEN_Msk)) == (EQEI_CTL_CHAEN_Msk | EQEI_CTL_CHBEN_Msk | EQEI_CTL_IDXEN_Msk));
            CU_ASSERT((TestCh[u8TestChIdx]->CNTMAX & EQEI_CNTMAX_CNTMAX_Msk) == 0x12345678);
        }

        // test function
        EQEI_Stop(TestCh[u8TestChIdx]);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_EQEIEN_Msk) == 0);

        EQEI_Start(TestCh[u8TestChIdx]);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_EQEIEN_Msk) == EQEI_CTL_EQEIEN_Msk);

        EQEI_Close(TestCh[u8TestChIdx]);
        CU_ASSERT(TestCh[u8TestChIdx]->CTL == 0);
    }
}

const uint32_t au32QeiINTSel[] = {
    EQEI_CTL_OVUNIEN_Msk, // 16 : Direction change interrupt
    EQEI_CTL_DIRIEN_Msk,  // 17 : Counter overflow or underflow interrupt
    EQEI_CTL_CMPIEN_Msk,  // 18 : Compare-match interrupt
    EQEI_CTL_IDXIEN_Msk,  // 19 : Index detected interrupt
};

void TestFunc_QEI_EnableInt()
{
    // 1. QEI_EnableInt()
    // 2. QEI_DisableInt()

    uint8_t u8QeiINTIdx; // bit

    /* test loop */
    for (u8TestChIdx = 0; u8TestChIdx < TestChNum; u8TestChIdx++)
    {
        // enable interrupt
        for (u8QeiINTIdx = 0; u8QeiINTIdx < sizeof(au32QeiINTSel) / sizeof(uint32_t); u8QeiINTIdx++)
        {
            EQEI_EnableInt(TestCh[u8TestChIdx], au32QeiINTSel[u8QeiINTIdx]);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & au32QeiINTSel[u8QeiINTIdx]) == au32QeiINTSel[u8QeiINTIdx]);
        }

        // disable interrupt
        for (u8QeiINTIdx = 0; u8QeiINTIdx < 4; u8QeiINTIdx++)
        {
            EQEI_DisableInt(TestCh[u8TestChIdx], au32QeiINTSel[u8QeiINTIdx]);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & au32QeiINTSel[u8QeiINTIdx]) == 0);
        }
    }

    NVIC_DisableIRQ(EQEI0_IRQn);
    NVIC_DisableIRQ(EQEI1_IRQn);
    NVIC_DisableIRQ(EQEI2_IRQn);
    NVIC_DisableIRQ(EQEI3_IRQn);
}

void TestFunc_QEI_TestMacro()
{

    // 1. QEI_SET_CNT_MODE()

    // 2. QEI_ENABLE_INDEX_RELOAD()
    // 3. QEI_DISABLE_INDEX_RELOAD()

    uint8_t u8QeiModeSelIdx;

    // main test loop
    for (u8TestChIdx = 0; u8TestChIdx < TestChNum; u8TestChIdx++)
    {
        for (u8QeiModeSelIdx = 0; u8QeiModeSelIdx < 4; u8QeiModeSelIdx++)
        {
            // QEI_SET_CNT_MODE
            EQEI_SET_CNT_MODE(TestCh[u8TestChIdx], au32QeiModeSel[u8QeiModeSelIdx]);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_MODE_Msk) == (u8QeiModeSelIdx << EQEI_CTL_MODE_Pos));
        }

        EQEI_ENABLE_INDEX_RELOAD(TestCh[u8TestChIdx]);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_IDXRLDEN_Msk) == EQEI_CTL_IDXRLDEN_Msk);

        EQEI_DISABLE_INDEX_RELOAD(TestCh[u8TestChIdx]);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_IDXRLDEN_Msk) == 0);
    }
}

const uint32_t au32QeiNFSel[] = {
    EQEI_CTL_NFCLKSEL_DIV1,
    EQEI_CTL_NFCLKSEL_DIV2,
    EQEI_CTL_NFCLKSEL_DIV4,
    EQEI_CTL_NFCLKSEL_DIV16,
    EQEI_CTL_NFCLKSEL_DIV32,
    EQEI_CTL_NFCLKSEL_DIV64};

void TestFunc_QEI_TestMacroNF()
{
    // 1. QEI_DISABLE_NOISE_FILTER()
    // 2. QEI_ENABLE_NOISE_FILTER()

    uint8_t u8QeiNFSelIdx;

    // main test loop
    for (u8TestChIdx = 0; u8TestChIdx < TestChNum; u8TestChIdx++)
    {
        for (u8QeiNFSelIdx = 0; u8QeiNFSelIdx < 4; u8QeiNFSelIdx++)
        {
            EQEI_DISABLE_NOISE_FILTER(TestCh[u8TestChIdx]);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_NFDIS_Msk) == EQEI_CTL_NFDIS_Msk);

            EQEI_ENABLE_NOISE_FILTER(TestCh[u8TestChIdx], au32QeiNFSel[u8QeiNFSelIdx]);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_NFDIS_Msk) == 0);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_NFCLKSEL_Msk) == (u8QeiNFSelIdx << EQEI_CTL_NFCLKSEL_Pos));
        }
    }
}

#define EQEIAPIN 0
#define EQEIBPIN 1
#define EQEIXPIN 2

volatile uint32_t *au32QeiPin[12] = {
    &PA4,
    &PA3,
    &PA5,
    &PA9,
    &PA8,
    &PA10,
    &PD0,
    &PD13,
    &PD1,
    &PA1,
    &PA0,
    &PA2,
};

void TestFunc_QEI_TestMacroINT()
{
    // 1. QEI_ENABLE_INT()
    // 2. QEI_DISABLE_INT()

    // 3. QEI_SET_CNT_MAX()
    // 4. QEI_SET_CNT_CMP()

    // 5. QEI_GET_INT_FLAG()
    // 6. QEI_CLR_INT_FLAG()
    // 7. QEI_GET_DIR()

    // 8. QEI_ENABLE_CNT_CMP()
    // 9. QEI_DISABLE_CNT_CMP()

    uint8_t u8QeiINTIdx; // bit
    uint32_t u32Count;

    // main test loop
    for (u8TestChIdx = 0; u8TestChIdx < TestChNum; u8TestChIdx++)
    {

        // enable interrupt
        for (u8QeiINTIdx = 0; u8QeiINTIdx < 4; u8QeiINTIdx++)
        {
            EQEI_ENABLE_INT(TestCh[u8TestChIdx], au32QeiINTSel[u8QeiINTIdx]);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & au32QeiINTSel[u8QeiINTIdx]) == au32QeiINTSel[u8QeiINTIdx]);
        }

        // disable interrupt
        for (u8QeiINTIdx = 0; u8QeiINTIdx < 4; u8QeiINTIdx++)
        {
            EQEI_DISABLE_INT(TestCh[u8TestChIdx], au32QeiINTSel[u8QeiINTIdx]);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & au32QeiINTSel[u8QeiINTIdx]) == 0);
        }

        // Setting before test
        EQEI_Stop(TestCh[u8TestChIdx]);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_EQEIEN_Msk) == 0);

        EQEI_SET_CNT_MODE(TestCh[u8TestChIdx], EQEI_CTL_X4_COMPARE_COUNTING_MODE);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_MODE_Msk) == EQEI_CTL_X4_COMPARE_COUNTING_MODE);
        EQEI_ENABLE_INPUT(TestCh[u8TestChIdx], EQEI_CTL_CHAEN_Msk | EQEI_CTL_CHBEN_Msk | EQEI_CTL_IDXEN_Msk);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & (EQEI_CTL_CHAEN_Msk | EQEI_CTL_CHBEN_Msk | EQEI_CTL_IDXEN_Msk)) == (EQEI_CTL_CHAEN_Msk | EQEI_CTL_CHBEN_Msk | EQEI_CTL_IDXEN_Msk));

        EQEI_SET_CNT_MAX(TestCh[u8TestChIdx], 0x20);
        CU_ASSERT(TestCh[u8TestChIdx]->CNTMAX == 0x20);
        EQEI_SET_CNT_CMP(TestCh[u8TestChIdx], 0x10);
        CU_ASSERT(TestCh[u8TestChIdx]->CNTCMP == 0x10);
        EQEI_ENABLE_CNT_CMP(TestCh[u8TestChIdx]);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_CMPEN_Msk) == EQEI_CTL_CMPEN_Msk);
        TestCh[u8TestChIdx]->CNT = 0;

        // init state
        *(au32QeiPin[(u8TestChIdx * 3) + EQEIAPIN]) = 0; // 00
        CLK_SysTickDelay(10);
        *(au32QeiPin[(u8TestChIdx * 3) + EQEIBPIN]) = 0;
        CLK_SysTickDelay(10);
        *(au32QeiPin[(u8TestChIdx * 3) + EQEIXPIN]) = 0;
        EQEI_Start(TestCh[u8TestChIdx]);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_EQEIEN_Msk) == EQEI_CTL_EQEIEN_Msk);

        // 1.Compare-match flag
        for (u32Count = 0; u32Count < 5; u32Count++)
        {
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIAPIN]) = 1; // 10
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIBPIN]) = 1; // 11
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIAPIN]) = 0; // 01
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIBPIN]) = 0; // 00
            CLK_SysTickDelay(10);
        }
        
        CU_ASSERT(EQEI_GET_INT_FLAG(TestCh[u8TestChIdx], EQEI_STATUS_CMPF_Msk) == 1);
        EQEI_CLR_INT_FLAG(TestCh[u8TestChIdx], EQEI_STATUS_CMPF_Msk);
        CU_ASSERT(EQEI_GET_INT_FLAG(TestCh[u8TestChIdx], EQEI_STATUS_CMPF_Msk) == 0);

        // 2.Counter overflow or underflow flag
        for (u32Count = 0; u32Count < 5; u32Count++)
        {
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIAPIN]) = 1; // 10
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIBPIN]) = 1; // 11
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIAPIN]) = 0; // 01
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIBPIN]) = 0; // 00
            CLK_SysTickDelay(10);
        }
        CU_ASSERT(EQEI_GET_INT_FLAG(TestCh[u8TestChIdx], EQEI_STATUS_OVUNF_Msk) == 1);
        EQEI_CLR_INT_FLAG(TestCh[u8TestChIdx], EQEI_STATUS_OVUNF_Msk);
        CU_ASSERT(EQEI_GET_INT_FLAG(TestCh[u8TestChIdx], EQEI_STATUS_OVUNF_Msk) == 0);

        // Counter counting direction indication
        CU_ASSERT(EQEI_GET_DIR(TestCh[u8TestChIdx]) == 1);

        // 3.Direction change flag
        for (u32Count = 0; u32Count < 8; u32Count++)
        {
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIBPIN]) = 1; // 01
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIAPIN]) = 1; // 11
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIBPIN]) = 0; // 10
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIAPIN]) = 0; // 00
            CLK_SysTickDelay(10);
        }

        // Counter counting direction indication
        CU_ASSERT(EQEI_GET_DIR(TestCh[u8TestChIdx]) == 0);
        CU_ASSERT(EQEI_GET_INT_FLAG(TestCh[u8TestChIdx], EQEI_STATUS_DIRCHGF_Msk) == 1);
        EQEI_CLR_INT_FLAG(TestCh[u8TestChIdx], EQEI_STATUS_DIRCHGF_Msk);
        CU_ASSERT(EQEI_GET_INT_FLAG(TestCh[u8TestChIdx], EQEI_STATUS_DIRCHGF_Msk) == 0);

        // 4.Index detected flag
        *(au32QeiPin[(u8TestChIdx * 3) + EQEIXPIN]) = 1;
        CLK_SysTickDelay(10);
        *(au32QeiPin[(u8TestChIdx * 3) + EQEIXPIN]) = 0;
        CLK_SysTickDelay(10);
        CU_ASSERT(EQEI_GET_INT_FLAG(TestCh[u8TestChIdx], EQEI_STATUS_IDXF_Msk) == 0x1);
        EQEI_CLR_INT_FLAG(TestCh[u8TestChIdx], EQEI_STATUS_IDXF_Msk);
        CU_ASSERT(EQEI_GET_INT_FLAG(TestCh[u8TestChIdx], EQEI_STATUS_IDXF_Msk) == 0x0);

        // Disable compare function after test
        EQEI_DISABLE_CNT_CMP(TestCh[u8TestChIdx]);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_CMPEN_Msk) == 0);

        EQEI_Close(TestCh[u8TestChIdx]);
        CU_ASSERT(TestCh[u8TestChIdx]->CTL == 0);
        TestCh[u8TestChIdx]->STATUS = TestCh[u8TestChIdx]->STATUS;
    }
}

const uint32_t au32QeiInputSel[3] = {
    EQEI_CTL_CHAEN_Msk,
    EQEI_CTL_CHBEN_Msk,
    EQEI_CTL_IDXEN_Msk,
};

const uint32_t au32QeiInvSel[3] = {
    EQEI_CTL_CHAINV_Msk,
    EQEI_CTL_CHAINV_Msk,
    EQEI_CTL_IDXINV_Msk,
};

void TestFunc_QEI_TestMacroINPUT()
{
    // 1. QEI_ENABLE_INPUT()
    // 2. QEI_DISABLE_INPUT()
    // 3. QEI_ENABLE_INPUT_INV()
    // 4. QEI_DISABLE_INPUT_INV()

    uint8_t u8QeiInputSelIdx;

    // main test loop
    for (u8TestChIdx = 0; u8TestChIdx < TestChNum; u8TestChIdx++)
    {
        for (u8QeiInputSelIdx = 0; u8QeiInputSelIdx < 3; u8QeiInputSelIdx++)
        {

            EQEI_ENABLE_INPUT(TestCh[u8TestChIdx], au32QeiInputSel[u8QeiInputSelIdx]);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & au32QeiInputSel[u8QeiInputSelIdx]) == au32QeiInputSel[u8QeiInputSelIdx]);

            EQEI_DISABLE_INPUT(TestCh[u8TestChIdx], au32QeiInputSel[u8QeiInputSelIdx]);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & au32QeiInputSel[u8QeiInputSelIdx]) == 0);

            EQEI_ENABLE_INPUT_INV(TestCh[u8TestChIdx], au32QeiInvSel[u8QeiInputSelIdx]);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & au32QeiInvSel[u8QeiInputSelIdx]) == au32QeiInvSel[u8QeiInputSelIdx]);

            EQEI_DISABLE_INPUT_INV(TestCh[u8TestChIdx], au32QeiInvSel[u8QeiInputSelIdx]);
            CU_ASSERT((TestCh[u8TestChIdx]->CTL & au32QeiInvSel[u8QeiInputSelIdx]) == 0);
        }
    }
}

void TestFunc_QEI_TestMacroLATCH()
{
    // 1. QEI_ENABLE_INDEX_LATCH()
    // 2. QEI_DISABLE_INDEX_LATCH

    // 3. QEI_GET_INDEX_LATCH_VALUE()
    // 4. QEI_SET_INDEX_LATCH_VALUE()

    uint32_t u32Count;

    // main test loop
    for (u8TestChIdx = 0; u8TestChIdx < TestChNum; u8TestChIdx++)
    {

        // Setting before test
        EQEI_Stop(TestCh[u8TestChIdx]);
        EQEI_SET_CNT_MODE(TestCh[u8TestChIdx], EQEI_CTL_X4_FREE_COUNTING_MODE);
        EQEI_ENABLE_INPUT(TestCh[u8TestChIdx], EQEI_CTL_CHAEN_Msk | EQEI_CTL_CHBEN_Msk | EQEI_CTL_IDXEN_Msk);

        EQEI_ENABLE_INDEX_LATCH(TestCh[u8TestChIdx]);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_IDXLATEN_Msk) == EQEI_CTL_IDXLATEN_Msk);
        TestCh[u8TestChIdx]->CNT = 0;

        // init signal state and start counting
        *(au32QeiPin[(u8TestChIdx * 3) + EQEIAPIN]) = 0; // 00
        CLK_SysTickDelay(10);
        *(au32QeiPin[(u8TestChIdx * 3) + EQEIBPIN]) = 0;
        CLK_SysTickDelay(10);
        *(au32QeiPin[(u8TestChIdx * 3) + EQEIXPIN]) = 0;
        EQEI_Start(TestCh[u8TestChIdx]);

        // counting
        for (u32Count = 0; u32Count < 5; u32Count++)
        {
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIAPIN]) = 1; // 10
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIBPIN]) = 1; // 11
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIAPIN]) = 0; // 01
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIBPIN]) = 0; // 00
            CLK_SysTickDelay(10);
        }

        // index
        *(au32QeiPin[(u8TestChIdx * 3) + EQEIXPIN]) = 1;
        CLK_SysTickDelay(10);
        *(au32QeiPin[(u8TestChIdx * 3) + EQEIXPIN]) = 0;
        CLK_SysTickDelay(10);

        // latch value
        CU_ASSERT(EQEI_GET_INDEX_LATCH_VALUE(TestCh[u8TestChIdx]) == 20);
        CU_ASSERT(EQEI_GET_CNT_VALUE(TestCh[u8TestChIdx]) == 20);

        // Disable index latch function after test
        EQEI_DISABLE_INDEX_LATCH(TestCh[u8TestChIdx]);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_IDXLATEN_Msk) == 0);

        // End test
        EQEI_Close(TestCh[u8TestChIdx]);
        TestCh[u8TestChIdx]->STATUS = TestCh[u8TestChIdx]->STATUS;
    }

    EQEI_SET_INDEX_LATCH_VALUE(EQEI0, 40);
    CU_ASSERT(EQEI_GET_INDEX_LATCH_VALUE(EQEI0) == 40);
}

void TestFunc_QEI_TestMacroHOLD()
{
    // 1. QEI_SET_HOLD_TRG_SRC()
    // 2. QEI_GET_HOLD_VALUE()
    // 3. QEI_GET_CNT_VALUE()
    // 4. QEI_SET_CNT_VALUE()

    uint32_t u32Count;

    // main test loop
    for (u8TestChIdx = 0; u8TestChIdx < 2; u8TestChIdx++)
    {

        // Setting before test
        EQEI_Stop(TestCh[u8TestChIdx]);
        EQEI_SET_CNT_MODE(TestCh[u8TestChIdx], EQEI_CTL_X4_FREE_COUNTING_MODE);
        EQEI_ENABLE_INPUT(TestCh[u8TestChIdx], EQEI_CTL_CHAEN_Msk | EQEI_CTL_CHBEN_Msk | EQEI_CTL_IDXEN_Msk);
        TestCh[u8TestChIdx]->CNT = 0;

        // init signal state and start counting
        *(au32QeiPin[(u8TestChIdx * 3) + EQEIAPIN]) = 0; // 00
        CLK_SysTickDelay(10);
        *(au32QeiPin[(u8TestChIdx * 3) + EQEIBPIN]) = 0;
        CLK_SysTickDelay(10);
        *(au32QeiPin[(u8TestChIdx * 3) + EQEIXPIN]) = 0;
        EQEI_Start(TestCh[u8TestChIdx]);

        // counting
        for (u32Count = 0; u32Count < 5; u32Count++)
        {
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIAPIN]) = 1; // 10
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIBPIN]) = 1; // 11
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIAPIN]) = 0; // 01
            CLK_SysTickDelay(10);
            *(au32QeiPin[(u8TestChIdx * 3) + EQEIBPIN]) = 0; // 00
            CLK_SysTickDelay(10);
        }

        // hold value
        EQEI_ENABLE_HOLD_TRG_SRC(TestCh[u8TestChIdx], EQEI_CTL_HOLDTMR0_Msk);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_HOLDTMR0_Msk) == EQEI_CTL_HOLDTMR0_Msk);
        EQEI_DISABLE_HOLD_TRG_SRC(TestCh[u8TestChIdx], EQEI_CTL_HOLDTMR0_Msk);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_HOLDTMR0_Msk) == 0);

        EQEI_ENABLE_HOLD_TRG_SRC(TestCh[u8TestChIdx], EQEI_CTL_HOLDTMR1_Msk);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_HOLDTMR1_Msk) == EQEI_CTL_HOLDTMR1_Msk);

        EQEI_DISABLE_HOLD_TRG_SRC(TestCh[u8TestChIdx], EQEI_CTL_HOLDTMR1_Msk);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_HOLDTMR1_Msk) == 0);

        EQEI_ENABLE_HOLD_TRG_SRC(TestCh[u8TestChIdx], EQEI_CTL_HOLDTMR2_Msk);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_HOLDTMR2_Msk) == EQEI_CTL_HOLDTMR2_Msk);

        EQEI_DISABLE_HOLD_TRG_SRC(TestCh[u8TestChIdx], EQEI_CTL_HOLDTMR2_Msk);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_HOLDTMR2_Msk) == 0);

        EQEI_ENABLE_HOLD_TRG_SRC(TestCh[u8TestChIdx], EQEI_CTL_HOLDTMR3_Msk);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_HOLDTMR3_Msk) == EQEI_CTL_HOLDTMR3_Msk);

        EQEI_DISABLE_HOLD_TRG_SRC(TestCh[u8TestChIdx], EQEI_CTL_HOLDTMR3_Msk);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_HOLDTMR3_Msk) == 0);

        EQEI_ENABLE_HOLD_TRG_SRC(TestCh[u8TestChIdx], EQEI_CTL_HOLDCNT_Msk);
        CU_ASSERT(EQEI_GET_HOLD_VALUE(TestCh[u8TestChIdx]) == 20);
        CU_ASSERT(EQEI_GET_CNT_VALUE(TestCh[u8TestChIdx]) == 20);

        EQEI_DISABLE_HOLD_TRG_SRC(TestCh[u8TestChIdx], EQEI_CTL_HOLDCNT_Msk);
        CU_ASSERT((TestCh[u8TestChIdx]->CTL & EQEI_CTL_HOLDCNT_Msk) == 0);

        // End test
        EQEI_Close(TestCh[u8TestChIdx]);
        TestCh[u8TestChIdx]->STATUS = TestCh[u8TestChIdx]->STATUS;
    }

    EQEI_SET_CNT_VALUE(EQEI0, 50);
    CU_ASSERT(EQEI_GET_CNT_VALUE(EQEI0) == 50);

    Disable_SelfTest();
}

CU_TestInfo QEI_ApiTests[] =
    {
        {"Testing QEI_Open Function.", TestFunc_QEI_Open},
        {"Testing QEI_EnableInt Function.", TestFunc_QEI_EnableInt},

        CU_TEST_INFO_NULL};

CU_TestInfo QEI_MacroTests[] =
    {

        {"Testing Macro.", TestFunc_QEI_TestMacro},
        {"Testing Macro about noise filter.", TestFunc_QEI_TestMacroNF},
        {"Testing Macro about interrupt.", TestFunc_QEI_TestMacroINT},
        {"Testing Macro about input.", TestFunc_QEI_TestMacroINPUT},
        {"Testing Macro about latch.", TestFunc_QEI_TestMacroLATCH},
        {"Testing Macro about hold.", TestFunc_QEI_TestMacroHOLD},

        CU_TEST_INFO_NULL};
