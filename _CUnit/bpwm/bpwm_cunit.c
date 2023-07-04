/******************************************************************************
* @file    bpwm_cunit.c
* @version V1.00
* @brief   CUnit test BPWM
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "bpwm_cunit.h"
//#define BPWM_CUNIT_TEST_PART 2
#define BPWM_MAX_CH BPWM_CHANNEL_NUM
uint32_t SixBitsPatternTable[4] = {0x3F, 0x2A, 0x15, 0x00}; /* 6 bits test pattern */
uint32_t EightBitsPatternTable[4] = {0xFF, 0x5A, 0xA5, 0x00}; /* 8 bits test pattern */
uint32_t TwelveBitsPatternTable[4] = {0xFFF, 0xA5A, 0x5A5, 0x000}; /* 12 bits test pattern */
uint32_t SixteenBitsPatternTable[4] = {0xFFFF, 0x5A5A, 0xA5A5, 0x0000}; /* 16 bits test pattern */

#define BPWM_MODULE_NUM 2
#define BPWM_ONEPORT_SELF 0

BPWM_T *g_apPWMModule[BPWM_MODULE_NUM] = {BPWM0, BPWM1};

void ResetBPWM(void)
{
    SYS_ResetModule(SYS_BPWM0RST);
    SYS_ResetModule(SYS_BPWM1RST);
}

int BPWM_Test_Init(void)
{
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Reset PWM */
    ResetBPWM();

    //     /* PWM clock frequency can be set equal or double to HCLK by choosing case 1 or case 2 */
    //     /* case 1.PWM clock frequency is set equal to HCLK: select PWM module clock source as PCLK */
    CLK_SetModuleClock(BPWM0_MODULE, CLK_BPWMSEL_BPWM0SEL_PCLK0, 0);
    CLK_SetModuleClock(BPWM1_MODULE, CLK_BPWMSEL_BPWM1SEL_PCLK2, 0);

    //     /* case 2.PWM clock frequency is set double to HCLK: select PWM module clock source as PLL */
    //CLK_SetModuleClock(BPWM0_MODULE, CLK_BPWMSEL_BPWM0SEL_HCLK0, 0);
    //CLK_SetModuleClock(BPWM1_MODULE, CLK_BPWMSEL_BPWM1SEL_HCLK0, 0);

    /* Enable BPWM module clock */
    CLK_EnableModuleClock(BPWM0_MODULE);
    CLK_EnableModuleClock(BPWM1_MODULE);
    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);

    /* Set PA multi-function pins for BPWM0 Channel0~5 */
    SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~(SYS_GPA_MFP0_PA0MFP_Msk)) | SYS_GPA_MFP0_PA0MFP_BPWM0_CH0;
    SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~(SYS_GPA_MFP0_PA1MFP_Msk)) | SYS_GPA_MFP0_PA1MFP_BPWM0_CH1;
    SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~(SYS_GPA_MFP0_PA2MFP_Msk)) | SYS_GPA_MFP0_PA2MFP_BPWM0_CH2;
    SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~(SYS_GPA_MFP0_PA3MFP_Msk)) | SYS_GPA_MFP0_PA3MFP_BPWM0_CH3;
    SYS->GPA_MFP1 = (SYS->GPA_MFP1 & ~(SYS_GPA_MFP1_PA4MFP_Msk)) | SYS_GPA_MFP1_PA4MFP_BPWM0_CH4;
    SYS->GPA_MFP1 = (SYS->GPA_MFP1 & ~(SYS_GPA_MFP1_PA5MFP_Msk)) | SYS_GPA_MFP1_PA5MFP_BPWM0_CH5;
    GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT1, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT2, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT3, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT4, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT5, GPIO_MODE_OUTPUT);


    /* Set PB multi-function pins for BPWM1 Channel0~5 */
    SYS->GPB_MFP2 = (SYS->GPB_MFP2 & ~(SYS_GPB_MFP2_PB11MFP_Msk)) | SYS_GPB_MFP2_PB11MFP_BPWM1_CH0;
    SYS->GPB_MFP2 = (SYS->GPB_MFP2 & ~(SYS_GPB_MFP2_PB10MFP_Msk)) | SYS_GPB_MFP2_PB10MFP_BPWM1_CH1;
    SYS->GPA_MFP1 = (SYS->GPA_MFP1 & ~(SYS_GPA_MFP1_PA7MFP_Msk)) | SYS_GPA_MFP1_PA7MFP_BPWM1_CH2;
    SYS->GPA_MFP1 = (SYS->GPA_MFP1 & ~(SYS_GPA_MFP1_PA6MFP_Msk)) | SYS_GPA_MFP1_PA6MFP_BPWM1_CH3;
    SYS->GPA_MFP3 = (SYS->GPA_MFP3 & ~(SYS_GPA_MFP3_PA14MFP_Msk)) | SYS_GPA_MFP3_PA14MFP_BPWM1_CH4;
    SYS->GPA_MFP3 = (SYS->GPA_MFP3 & ~(SYS_GPA_MFP3_PA15MFP_Msk)) | SYS_GPA_MFP3_PA15MFP_BPWM1_CH5;
    GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PB, BIT10, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT7, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT6, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT15, GPIO_MODE_OUTPUT);

    return 0;
}
int BPWM_Test_Clean(void)
{
    /* Disable BPWM module clock */
    CLK_DisableModuleClock(BPWM0_MODULE);
    CLK_DisableModuleClock(BPWM1_MODULE);
    return 0;
}

void Func_BPWM_Start()
{
    uint32_t u32TestCh = 0, j = 0;  //TwoMode

    /* Reset PWM */
    ResetBPWM();

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_Start(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN & (BPWM_CNTEN_CNTEN0_Msk), 1);

            BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN & (BPWM_CNTEN_CNTEN0_Msk), 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_Start(g_apPWMModule[j], 1 << u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0x00000001);

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0);

        //test case 3: enable/disable multiple channels each time
        BPWM_Start(g_apPWMModule[j], BPWM_CH_0_MASK | BPWM_CH_1_MASK);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0x00000001);

        BPWM_ForceStop(g_apPWMModule[j], BPWM_CH_0_MASK | BPWM_CH_1_MASK);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0);

        BPWM_Start(g_apPWMModule[j], BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0x00000001);

        BPWM_ForceStop(g_apPWMModule[j], BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0);

        BPWM_Start(g_apPWMModule[j], BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK | BPWM_CH_3_MASK);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0x0000001);

        BPWM_ForceStop(g_apPWMModule[j], BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK | BPWM_CH_3_MASK);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0);

        BPWM_Start(g_apPWMModule[j], BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK | BPWM_CH_3_MASK | BPWM_CH_4_MASK);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0x0000001);

        BPWM_ForceStop(g_apPWMModule[j], BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK | BPWM_CH_3_MASK | BPWM_CH_4_MASK);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0);

        BPWM_Start(g_apPWMModule[j], BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK | BPWM_CH_3_MASK | BPWM_CH_4_MASK | BPWM_CH_5_MASK);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0x0000001);

        BPWM_ForceStop(g_apPWMModule[j], BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK | BPWM_CH_3_MASK | BPWM_CH_4_MASK | BPWM_CH_5_MASK);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0);
    }
}

void Func_BPWM_CONSTANT()
{
    uint32_t j ;
    /* Reset PWM */
    ResetBPWM();

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        g_apPWMModule[j]->CTL1 = BPWM_UP_DOWN_COUNTER;
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1, 2);
    }
}

void Func_1_BPWM_ENABLE_OUTPUT_INVERTER()
{
    uint32_t u32TestCh = 0, j = 0;

    const uint32_t au32RegWrite[7] = {0x0000003F, 0x0000001F, 0x0000000F, 0x00000007, 0x00000003, 0x00000001, 0};

    /* Reset PWM */
    ResetBPWM();

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        //test case 1: enable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_ENABLE_OUTPUT_INVERTER(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->POLCTL & (0x3ful << BPWM_POLCTL_PINV0_Pos), 1 << u32TestCh);
            g_apPWMModule[j]->POLCTL &= ~(1 << u32TestCh) ;
        }

        //test case 2: enable/disable multiple channels each time
        for (u32TestCh = 0; u32TestCh <= BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_ENABLE_OUTPUT_INVERTER(g_apPWMModule[j], au32RegWrite[u32TestCh]);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->POLCTL, au32RegWrite[u32TestCh]);
            g_apPWMModule[j]->POLCTL &= ~(au32RegWrite[u32TestCh]) ;
        }
    }
}

void Func_2_BPWM_SET_PRESCALER()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t u32PrescalerPatternIdx = 0, u32PrescalerPatternCount = 0, u32PrescalerPatternData = 0;

    /* Reset PWM */
    ResetBPWM();

    u32PrescalerPatternCount = sizeof(TwelveBitsPatternTable) / sizeof(uint32_t);

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            for (u32PrescalerPatternIdx = 0; u32PrescalerPatternIdx < u32PrescalerPatternCount; u32PrescalerPatternIdx++)
            {
                u32PrescalerPatternData = TwelveBitsPatternTable[u32PrescalerPatternIdx];

                BPWM_SET_PRESCALER(g_apPWMModule[x], u32TestCh, u32PrescalerPatternData);
                CU_ASSERT_EQUAL(*(__IO uint32_t *)(&((g_apPWMModule[x])->CLKPSC)), u32PrescalerPatternData);

                //check prescaler value by BPWM_GET_PRESCALER()
                CU_ASSERT_EQUAL(BPWM_GET_PRESCALER(g_apPWMModule[x], u32TestCh), u32PrescalerPatternData);
            }
        }
    }
}

void Func_3_BPWM_SET_CMR()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t u32PrescalerPatternIdx = 0, u32PrescalerPatternCount = 0, u32PrescalerPatternData = 0;

    /* Reset PWM */
    ResetBPWM();

    u32PrescalerPatternCount = sizeof(SixteenBitsPatternTable) / sizeof(uint32_t);

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            for (u32PrescalerPatternIdx = 0; u32PrescalerPatternIdx < u32PrescalerPatternCount; u32PrescalerPatternIdx++)
            {
                u32PrescalerPatternData = SixteenBitsPatternTable[u32PrescalerPatternIdx];

                BPWM_SET_CMR(g_apPWMModule[x], u32TestCh, u32PrescalerPatternData);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->CMPDAT[u32TestCh] & BPWM_CMPDAT_CMPDAT_Msk, u32PrescalerPatternData);
                //check comparator value by BPWM_GET_CMR()
                CU_ASSERT_EQUAL(BPWM_GET_CMR(g_apPWMModule[x], u32TestCh), u32PrescalerPatternData);

                BPWM_SET_CNR(g_apPWMModule[x], u32TestCh, u32PrescalerPatternData);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->PERIOD & BPWM_PERIOD_PERIOD_Msk, u32PrescalerPatternData);   //TwoMode
                //check period value by BPWM_GET_CNR()
                CU_ASSERT_EQUAL(BPWM_GET_CNR(g_apPWMModule[x], u32TestCh), u32PrescalerPatternData);
            }
        }
    }
}

void Func_4_BPWM_SET_ALIGNED_TYPE()
{
    uint32_t u32TestCh = 0, j = 0;
    uint32_t au32ChMsk[6] = {0x00000001, 0x00000003, 0x00000007, 0x0000000F, 0x0000001F, 0x0000003F};
#if 0
    uint32_t au32RegCheck1[6] = {0x00000001, 0x00000005, 0x00000015, 0x00000055, 0x00000155, 0x00000555};
    uint32_t au32RegCheck2[6] = {0x00000556, 0x0000055A, 0x0000056A, 0x000005AA, 0x000006AA, 0x00000AAA};
#else //TwoMode
    uint32_t au32RegCheck1[6] = {0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001};
    uint32_t au32RegCheck2[6] = {0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002};
#endif
    /* Reset PWM */
    ResetBPWM();

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        //test case 1: enable one channel each time, then check each action //TwoMode
        BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], BPWM_CH_0_MASK, BPWM_CENTER_ALIGNED);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, 2);
        BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], BPWM_CH_0_MASK, BPWM_EDGE_ALIGNED);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, 1);

        BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], BPWM_CH_1_MASK, BPWM_CENTER_ALIGNED);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, 2);
        BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], BPWM_CH_1_MASK, BPWM_EDGE_ALIGNED);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, 1);

        BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], BPWM_CH_2_MASK, BPWM_CENTER_ALIGNED);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, 2);
        BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], BPWM_CH_2_MASK, BPWM_EDGE_ALIGNED);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, 1);

        BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], BPWM_CH_3_MASK, BPWM_CENTER_ALIGNED);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, 2);
        BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], BPWM_CH_3_MASK, BPWM_EDGE_ALIGNED);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, 1);

        BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], BPWM_CH_4_MASK, BPWM_CENTER_ALIGNED);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, 2);
        BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], BPWM_CH_4_MASK, BPWM_EDGE_ALIGNED);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, 1);

        BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], BPWM_CH_5_MASK, BPWM_CENTER_ALIGNED);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, 2);
        BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], BPWM_CH_5_MASK, BPWM_EDGE_ALIGNED);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, 1);

        //test case 2: enable one channel each time, then check at final action //SixMode
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], 1 << u32TestCh, BPWM_EDGE_ALIGNED);
        }

        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1, 0x1);

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], 1 << u32TestCh, BPWM_CENTER_ALIGNED);
        }

        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1, 0x2);

        /* clear CTL1 register value */
        g_apPWMModule[j]->CTL1 = 0;

        //test case 3: enable/disable multiple channels each time
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], au32ChMsk[u32TestCh], BPWM_EDGE_ALIGNED);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1, au32RegCheck1[u32TestCh]);
        }

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], au32ChMsk[u32TestCh], BPWM_CENTER_ALIGNED);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1, au32RegCheck2[u32TestCh]);
        }
    }
}

void Func_7_BPWM_ENABLE_TIMER_SYNC()
{
    uint32_t u32TestCh = 0, j = 0;
    uint32_t au32RegCheck[6] = {0x1, 0x1, 0x1, 0x1, 0x1, 0x1};
    uint32_t au32RegReverseCheck[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

#if (BPWM_MODULE_NUM>=2)

    CLK_EnableModuleClock(EPWM0_MODULE);
    CLK_EnableModuleClock(EPWM1_MODULE);
    /* Reset PWM */
    ResetBPWM();

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
#if 1 //M252 open this

        //synchrouous start source comes from EPWM1
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_ENABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh, BPWM_SSCTL_SSRC_EPWM1);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, 1  | BPWM_SSCTL_SSRC_EPWM1);

            //trigger sync start event from EPWM1, and check if the counter of selected channel(s) is starting counting
            EPWM_TRIGGER_SYNC_START(EPWM1);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 1);
            //disable counter for next test
            BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0);

            BPWM_DISABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, BPWM_SSCTL_SSRC_EPWM1);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_ENABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh, BPWM_SSCTL_SSRC_EPWM1);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, au32RegCheck[u32TestCh] | BPWM_SSCTL_SSRC_EPWM1);
        }

        //trigger sync start event from EPWM1, and check if the counter of selected channel(s) is starting counting
        EPWM_TRIGGER_SYNC_START(EPWM1);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, BPWM_CH_0_MASK);
        //disable counter for next test
        BPWM_ForceStop(g_apPWMModule[j], BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK | BPWM_CH_3_MASK | BPWM_CH_4_MASK | BPWM_CH_5_MASK);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0);

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_DISABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, au32RegReverseCheck[u32TestCh] | BPWM_SSCTL_SSRC_EPWM1);
        }

#endif
#if 1 //M252 open this

        //synchrouous start source comes from EPWM0
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_ENABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh, BPWM_SSCTL_SSRC_EPWM0);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, 1 | BPWM_SSCTL_SSRC_EPWM0);

            //trigger sync start event from EPWM0, and check if the counter of selected channel(s) is starting counting
            EPWM_TRIGGER_SYNC_START(EPWM0);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 1);
            //disable counter for next test
            BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0);

            BPWM_DISABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, 0 | BPWM_SSCTL_SSRC_EPWM0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_ENABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh, BPWM_SSCTL_SSRC_EPWM0);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, au32RegCheck[u32TestCh] | BPWM_SSCTL_SSRC_EPWM0);
        }

        //trigger sync start event from EPWM0, and check if the counter of selected channel(s) is starting counting
        EPWM_TRIGGER_SYNC_START(EPWM0);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, BPWM_CH_0_MASK);
        //disable counter for next test
        BPWM_ForceStop(g_apPWMModule[j], BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK | BPWM_CH_3_MASK | BPWM_CH_4_MASK | BPWM_CH_5_MASK);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0);

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_DISABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, au32RegReverseCheck[u32TestCh] | BPWM_SSCTL_SSRC_EPWM0);
        }

#endif
#if 1

        //synchrouous start source comes from BPWM1
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_ENABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh, BPWM_SSCTL_SSRC_BPWM1);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, 1  | BPWM_SSCTL_SSRC_BPWM1);

            //trigger sync start event from PWM1, and check if the counter of selected channel(s) is starting counting
            BPWM_TRIGGER_SYNC_START(BPWM1);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 1);
            //disable counter for next test
            BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0);

            BPWM_DISABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, BPWM_SSCTL_SSRC_BPWM1);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_ENABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh, BPWM_SSCTL_SSRC_BPWM1);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, au32RegCheck[u32TestCh] | BPWM_SSCTL_SSRC_BPWM1);
        }

        //trigger sync start event from PWM1, and check if the counter of selected channel(s) is starting counting
        BPWM_TRIGGER_SYNC_START(BPWM1);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, BPWM_CH_0_MASK);
        //disable counter for next test
        BPWM_ForceStop(g_apPWMModule[j], BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK | BPWM_CH_3_MASK | BPWM_CH_4_MASK | BPWM_CH_5_MASK);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0);

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_DISABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, au32RegReverseCheck[u32TestCh] | BPWM_SSCTL_SSRC_BPWM1);
        }

#endif
#if 1

        //synchrouous start source comes from BPWM0
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_ENABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh, BPWM_SSCTL_SSRC_BPWM0);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, 1 | BPWM_SSCTL_SSRC_BPWM0);

            //trigger sync start event from PWM1, and check if the counter of selected channel(s) is starting counting
            BPWM_TRIGGER_SYNC_START(BPWM0);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 1);
            //disable counter for next test
            BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0);

            BPWM_DISABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, BPWM_SSCTL_SSRC_BPWM0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_ENABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh, BPWM_SSCTL_SSRC_BPWM0);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, au32RegCheck[u32TestCh] | BPWM_SSCTL_SSRC_BPWM0);
        }

        //trigger sync start event from PWM1, and check if the counter of selected channel(s) is starting counting
        BPWM_TRIGGER_SYNC_START(BPWM0);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, BPWM_CH_0_MASK);
        //disable counter for next test
        BPWM_ForceStop(g_apPWMModule[j], BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK | BPWM_CH_3_MASK | BPWM_CH_4_MASK | BPWM_CH_5_MASK);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CNTEN, 0);

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_DISABLE_TIMER_SYNC(g_apPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->SSCTL, au32RegReverseCheck[u32TestCh] | BPWM_SSCTL_SSRC_BPWM0);
        }

#endif
    }

    CLK_DisableModuleClock(EPWM0_MODULE);
    CLK_DisableModuleClock(EPWM1_MODULE);
#endif
}

void Func_8_BPWM_MASK_OUTPUT()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset PWM */
    ResetBPWM();

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_MASK_OUTPUT(g_apPWMModule[x], 1 << u32TestCh, 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->MSKEN, 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->MSK, 1 << u32TestCh);
            g_apPWMModule[x]->MSKEN &= ~(1 << u32TestCh) ;
            g_apPWMModule[x]->MSK &= ~(1 << u32TestCh) ;
        }
    }
}

void Func_12_BPWM_CLR_COUNTER()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset PWM */
    ResetBPWM();

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_SET_CNR(g_apPWMModule[x], u32TestCh, 0xFFFF);
            BPWM_SET_CMR(g_apPWMModule[x], u32TestCh, 500);
            BPWM_Start(g_apPWMModule[x], 1 << u32TestCh);

            while ((g_apPWMModule[x]->INTSTS & (BPWM_INTSTS_CMPUIF0_Msk << u32TestCh)) != BPWM_INTSTS_CMPUIF0_Msk << u32TestCh);

            BPWM_ForceStop(g_apPWMModule[x], 1 << u32TestCh);
            BPWM_CLR_COUNTER(g_apPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CNT, 0x00010000);
        }
    }
}

void Func_13_BPWM_SET_OUTPUT_LEVEL()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t au32PatternData[4] = {BPWM_OUTPUT_NOTHING, BPWM_OUTPUT_LOW, BPWM_OUTPUT_HIGH, BPWM_OUTPUT_TOGGLE};
    uint32_t u32ZeroIdx = 0, u32CmpuIdx = 0, u32PeriodIdx = 0, u32CmpdIdx = 0;

    /* Reset PWM */
    ResetBPWM();

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            for (u32ZeroIdx = 0; u32ZeroIdx < sizeof(au32PatternData) / sizeof(uint32_t); u32ZeroIdx++)
            {
                for (u32CmpuIdx = 0; u32CmpuIdx < sizeof(au32PatternData) / sizeof(uint32_t); u32CmpuIdx++)
                {
                    for (u32PeriodIdx = 0; u32PeriodIdx < sizeof(au32PatternData) / sizeof(uint32_t); u32PeriodIdx++)
                    {
                        for (u32CmpdIdx = 0; u32CmpdIdx < sizeof(au32PatternData) / sizeof(uint32_t); u32CmpdIdx++)
                        {
                            BPWM_SET_OUTPUT_LEVEL(g_apPWMModule[x], 1 << u32TestCh, au32PatternData[u32ZeroIdx], au32PatternData[u32CmpuIdx], au32PatternData[u32PeriodIdx], au32PatternData[u32CmpdIdx]);
                            CU_ASSERT_EQUAL(g_apPWMModule[x]->WGCTL0 & (BPWM_WGCTL0_ZPCTL0_Msk << u32TestCh << u32TestCh), au32PatternData[u32ZeroIdx] << u32TestCh << u32TestCh);
                            CU_ASSERT_EQUAL(g_apPWMModule[x]->WGCTL0 & (BPWM_WGCTL0_PRDPCTL0_Msk << u32TestCh << u32TestCh), au32PatternData[u32PeriodIdx] << u32TestCh << u32TestCh << 16);
                            CU_ASSERT_EQUAL(g_apPWMModule[x]->WGCTL1 & (BPWM_WGCTL1_CMPUCTL0_Msk << u32TestCh << u32TestCh), au32PatternData[u32CmpuIdx] << u32TestCh << u32TestCh);
                            CU_ASSERT_EQUAL(g_apPWMModule[x]->WGCTL1 & (BPWM_WGCTL1_CMPDCTL0_Msk << u32TestCh << u32TestCh), au32PatternData[u32CmpdIdx] << u32TestCh << u32TestCh << 16);
                        }
                    }
                }
            }

        }
    }
}

void Func_15_BPWM_Stop()
{
    uint32_t u32TestCh = 0, x = 0;

    Func_BPWM_Start();

    /* Reset PWM */
    ResetBPWM();

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_SET_CNR(g_apPWMModule[x], u32TestCh, 0x100);

            BPWM_Stop(g_apPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->PERIOD, 0);   //TwoMode
        }

        //test case 2: enable/disable one channel each time, then check at final action=> ignore
        //     for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        //     {
        //         BPWM_SET_CNR(g_apPWMModule[x], u32TestCh, 0x100);
        //     }

        //     for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        //     {
        //         BPWM_Stop(g_apPWMModule[x], 1 << u32TestCh);
        //     }
        //     CU_ASSERT_EQUAL(g_apPWMModule[x]->CNR0, 0);
        //     CU_ASSERT_EQUAL(g_apPWMModule[x]->CNR1, 0);
        //     CU_ASSERT_EQUAL(g_apPWMModule[x]->CNR2, 0);
        //     CU_ASSERT_EQUAL(g_apPWMModule[x]->CNR3, 0);

        //test case 3: enable/disable multiple channels each time
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_SET_CNR(g_apPWMModule[x], u32TestCh, 0x100);
        }

        BPWM_Stop(g_apPWMModule[x], BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK | BPWM_CH_3_MASK | BPWM_CH_4_MASK | BPWM_CH_5_MASK);

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            CU_ASSERT_EQUAL(g_apPWMModule[x]->PERIOD, 0);
        }
    }
}

void Func_16_BPWM_EnableADCTrigger()
{
    uint32_t u32TestCh = 0, x = 0;

    Func_BPWM_Start();

    /* Reset PWM */
    ResetBPWM();

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            if (u32TestCh < 4)
            {
                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_EVEN_ZERO_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0 & ((BPWM_EADCTS0_TRGEN0_Msk | BPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x80 << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0 & ((BPWM_EADCTS0_TRGEN0_Msk | BPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0 << (u32TestCh * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_EVEN_PERIOD_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0 & ((BPWM_EADCTS0_TRGEN0_Msk | BPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x81 << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0 & ((BPWM_EADCTS0_TRGEN0_Msk | BPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x01 << (u32TestCh * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_EVEN_ZERO_OR_PERIOD_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0 & ((BPWM_EADCTS0_TRGEN0_Msk | BPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x82 << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0 & ((BPWM_EADCTS0_TRGEN0_Msk | BPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x02 << (u32TestCh * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_EVEN_CMP_UP_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0 & ((BPWM_EADCTS0_TRGEN0_Msk | BPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x83 << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0 & ((BPWM_EADCTS0_TRGEN0_Msk | BPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x03 << (u32TestCh * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_EVEN_CMP_DOWN_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0 & ((BPWM_EADCTS0_TRGEN0_Msk | BPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x84 << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0 & ((BPWM_EADCTS0_TRGEN0_Msk | BPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x04 << (u32TestCh * 8));
#if 0
                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_ODD_ZERO_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x85 << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x05 << (u32TestCh * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_ODD_PERIOD_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x86 << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x06 << (u32TestCh * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_ODD_ZERO_OR_PERIOD_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x87 << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x07 << (u32TestCh * 8));
#endif
                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_ODD_CMP_UP_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0 & ((BPWM_EADCTS0_TRGEN0_Msk | BPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x88 << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0 & ((BPWM_EADCTS0_TRGEN0_Msk | BPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x08 << (u32TestCh * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_ODD_CMP_DOWN_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0 & ((BPWM_EADCTS0_TRGEN0_Msk | BPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x89 << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0 & ((BPWM_EADCTS0_TRGEN0_Msk | BPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x09 << (u32TestCh * 8));
#if 0
                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_CH_0_FREE_CMP_UP_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x8A << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x0A << (u32TestCh * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_CH_0_FREE_CMP_DOWN_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x8B << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x0B << (u32TestCh * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_CH_2_FREE_CMP_UP_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x8C << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x0C << (u32TestCh * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_CH_2_FREE_CMP_DOWN_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x8D << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x0D << (u32TestCh * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_CH_4_FREE_CMP_UP_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x8E << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x0E << (u32TestCh * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_CH_4_FREE_CMP_DOWN_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x8F << (u32TestCh * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS0 & ((BPWM_ADCTS0_TRGEN0_Msk | BPWM_ADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x0F << (u32TestCh * 8));
#endif
            }
            else
            {
                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_EVEN_ZERO_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1 & ((BPWM_EADCTS1_TRGEN4_Msk | BPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x80 << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1 & ((BPWM_EADCTS1_TRGEN4_Msk | BPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0 << ((u32TestCh - 4) * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_EVEN_PERIOD_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1 & ((BPWM_EADCTS1_TRGEN4_Msk | BPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x81 << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1 & ((BPWM_EADCTS1_TRGEN4_Msk | BPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x01 << ((u32TestCh - 4) * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_EVEN_ZERO_OR_PERIOD_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1 & ((BPWM_EADCTS1_TRGEN4_Msk | BPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x82 << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1 & ((BPWM_EADCTS1_TRGEN4_Msk | BPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x02 << ((u32TestCh - 4) * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_EVEN_CMP_UP_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1 & ((BPWM_EADCTS1_TRGEN4_Msk | BPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x83 << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1 & ((BPWM_EADCTS1_TRGEN4_Msk | BPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x03 << ((u32TestCh - 4) * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_EVEN_CMP_DOWN_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1 & ((BPWM_EADCTS1_TRGEN4_Msk | BPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x84 << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1 & ((BPWM_EADCTS1_TRGEN4_Msk | BPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x04 << ((u32TestCh - 4) * 8));
#if 0
                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_ODD_ZERO_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x85 << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x05 << ((u32TestCh - 4) * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_ODD_PERIOD_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x86 << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x06 << ((u32TestCh - 4) * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_ODD_ZERO_OR_PERIOD_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x87 << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x07 << ((u32TestCh - 4) * 8));
#endif
                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_ODD_CMP_UP_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1 & ((BPWM_EADCTS1_TRGEN4_Msk | BPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x88 << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1 & ((BPWM_EADCTS1_TRGEN4_Msk | BPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x08 << ((u32TestCh - 4) * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_ODD_CMP_DOWN_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1 & ((BPWM_EADCTS1_TRGEN4_Msk | BPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x89 << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1 & ((BPWM_EADCTS1_TRGEN4_Msk | BPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x09 << ((u32TestCh - 4) * 8));
#if 0
                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_CH_0_FREE_CMP_UP_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x8A << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x0A << ((u32TestCh - 4) * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_CH_0_FREE_CMP_DOWN_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x8B << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x0B << ((u32TestCh - 4) * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_CH_2_FREE_CMP_UP_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x8C << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x0C << ((u32TestCh - 4) * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_CH_2_FREE_CMP_DOWN_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x8D << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x0D << ((u32TestCh - 4) * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_CH_4_FREE_CMP_UP_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x8E << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x0E << ((u32TestCh - 4) * 8));

                BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_CH_4_FREE_CMP_DOWN_COUNT_POINT);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x8F << ((u32TestCh - 4) * 8));

                BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->ADCTS1 & ((BPWM_ADCTS1_TRGEN4_Msk | BPWM_ADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x0F << ((u32TestCh - 4) * 8));
#endif
            }
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnableADCTrigger(g_apPWMModule[x], u32TestCh, BPWM_TRIGGER_ADC_EVEN_CMP_UP_COUNT_POINT);
        }

        CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0, 0x83838383);
        CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1, 0x8383);

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_DisableADCTrigger(g_apPWMModule[x], u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS0, 0x03030303);
        CU_ASSERT_EQUAL(g_apPWMModule[x]->EADCTS1, 0x0303);

    }
}

void Func_17_BPWM_EnableCapture()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset PWM */
    ResetBPWM();

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnableCapture(g_apPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPINEN & (BPWM_CAPINEN_CAPINEN0_Msk << u32TestCh), 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPCTL & (BPWM_CAPCTL_CAPEN0_Msk << u32TestCh), 1 << u32TestCh);
            BPWM_DisableCapture(g_apPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPINEN & (BPWM_CAPINEN_CAPINEN0_Msk << u32TestCh), 0 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPCTL & (BPWM_CAPCTL_CAPEN0_Msk << u32TestCh), 0 << u32TestCh);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnableCapture(g_apPWMModule[x], 1 << u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPINEN, 0x3F);
        CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPCTL, 0x3F);

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_DisableCapture(g_apPWMModule[x], 1 << u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPINEN, 0);
        CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPCTL, 0);

        //test case 3: enable/disable multiple channels each time
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh += 3)
        {
            BPWM_EnableCapture(g_apPWMModule[x], 7 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPINEN & (BPWM_CAPINEN_CAPINEN0_Msk | BPWM_CAPINEN_CAPINEN1_Msk | BPWM_CAPINEN_CAPINEN2_Msk) << u32TestCh, 7 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPCTL & (BPWM_CAPCTL_CAPEN0_Msk | BPWM_CAPCTL_CAPEN1_Msk | BPWM_CAPCTL_CAPEN2_Msk) << u32TestCh, 7 << u32TestCh);

            BPWM_DisableCapture(g_apPWMModule[x], 7 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPINEN, 0);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPCTL, 0);
        }
    }
}

void Func_18_BPWM_EnableOutput()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset PWM */
    ResetBPWM();

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnableOutput(g_apPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->POEN & (BPWM_POEN_POEN0_Msk << u32TestCh), 1 << u32TestCh);
            BPWM_DisableOutput(g_apPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->POEN & (BPWM_POEN_POEN0_Msk << u32TestCh), 0 << u32TestCh);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnableOutput(g_apPWMModule[x], 1 << u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apPWMModule[x]->POEN, 0x3F);

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_DisableOutput(g_apPWMModule[x], 1 << u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apPWMModule[x]->POEN, 0);

        //test case 3: enable/disable multiple channels each time
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh += 3)
        {
            BPWM_EnableOutput(g_apPWMModule[x], 7 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->POEN, 7 << u32TestCh);

            BPWM_DisableOutput(g_apPWMModule[x], 7 << u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->POEN, 0);
        }
    }
}

void Func_21_BPWM_EnableCaptureInt()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset PWM */
    ResetBPWM();

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnableCaptureInt(g_apPWMModule[x], u32TestCh, BPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPIEN, 1 << u32TestCh);
            BPWM_DisableCaptureInt(g_apPWMModule[x], u32TestCh, BPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPIEN, 0);

            BPWM_EnableCaptureInt(g_apPWMModule[x], u32TestCh, BPWM_CAPTURE_INT_FALLING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPIEN, 1 << (u32TestCh + 8));
            BPWM_DisableCaptureInt(g_apPWMModule[x], u32TestCh, BPWM_CAPTURE_INT_FALLING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPIEN, 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnableCaptureInt(g_apPWMModule[x], u32TestCh, BPWM_CAPTURE_INT_RISING_LATCH | BPWM_CAPTURE_INT_FALLING_LATCH);
        }

        CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPIEN, 0x3F3F);

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_DisableCaptureInt(g_apPWMModule[x], u32TestCh, BPWM_CAPTURE_INT_RISING_LATCH | BPWM_CAPTURE_INT_FALLING_LATCH);
        }

        CU_ASSERT_EQUAL(g_apPWMModule[x]->CAPIEN, 0);
    }
}

void Func_22_BPWM_EnableDutyInt()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t au32RegCheck[6] = {0x01010000, 0x03030000, 0x07070000, 0x0F0F0000, 0x1F1F0000, 0x3F3F0000};
    uint32_t au32RegReverseCheck[6] = {0x3E3E0000, 0x3C3C0000, 0x38380000, 0x30300000, 0x20200000, 0x00000000};

    /* Reset PWM */
    ResetBPWM();

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnableDutyInt(g_apPWMModule[x], u32TestCh, BPWM_DUTY_INT_DOWN_COUNT_MATCH_CMP);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->INTEN & (0x3ful << BPWM_INTEN_CMPDIEN0_Pos), 1 << (24 + u32TestCh));
            BPWM_DisableDutyInt(g_apPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->INTEN & (BPWM_INTEN_CMPDIEN0_Msk << u32TestCh), 0);

            BPWM_EnableDutyInt(g_apPWMModule[x], u32TestCh, BPWM_DUTY_INT_UP_COUNT_MATCH_CMP);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->INTEN & (0x3ful << BPWM_INTEN_CMPUIEN0_Pos), 1 << (16 + u32TestCh));
            BPWM_DisableDutyInt(g_apPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->INTEN & (BPWM_INTEN_CMPUIEN0_Msk << u32TestCh), 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnableDutyInt(g_apPWMModule[x], u32TestCh, BPWM_DUTY_INT_DOWN_COUNT_MATCH_CMP | BPWM_DUTY_INT_UP_COUNT_MATCH_CMP);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->INTEN, au32RegCheck[u32TestCh]);
        }

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_DisableDutyInt(g_apPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->INTEN, au32RegReverseCheck[u32TestCh]);
        }
    }
}

void Func_23_BPWM_EnablePeriodInt()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t au32RegCheck[6] = {0x00000100, 0x00000100, 0x00000100, 0x00000100, 0x00000100, 0x00000100};
    uint32_t au32RegReverseCheck[6] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};

    /* Reset PWM */
    ResetBPWM();

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnablePeriodInt(g_apPWMModule[x], u32TestCh, NULL);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->INTEN & (0x3ful << BPWM_INTEN_PIEN0_Pos), 1 << (8));
            BPWM_DisablePeriodInt(g_apPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->INTEN, 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnablePeriodInt(g_apPWMModule[x], u32TestCh, NULL);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->INTEN, au32RegCheck[u32TestCh]);
        }

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_DisablePeriodInt(g_apPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->INTEN, au32RegReverseCheck[u32TestCh]);
        }
    }
}

void Func_24_BPWM_GetADCTriggerFlag()
{
    uint32_t u32TestCh = 0, j = 0;

    /* Reset PWM */
    ResetBPWM();

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_SET_CNR(g_apPWMModule[j], u32TestCh, 10);
            BPWM_SET_CMR(g_apPWMModule[j], u32TestCh, 5);
            BPWM_SET_PRESCALER(g_apPWMModule[j], u32TestCh, 2);
            BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], 1 << u32TestCh, BPWM_CENTER_ALIGNED);

            if (u32TestCh % 2)
            {
                BPWM_EnableADCTrigger(g_apPWMModule[j], u32TestCh, BPWM_TRIGGER_ADC_ODD_CMP_UP_COUNT_POINT);
            }
            else
            {
                BPWM_EnableADCTrigger(g_apPWMModule[j], u32TestCh, BPWM_TRIGGER_ADC_EVEN_CMP_UP_COUNT_POINT);
            }

            BPWM_EnableOutput(g_apPWMModule[j], 1 << u32TestCh);
            BPWM_Start(g_apPWMModule[j], 1 << u32TestCh);

            while (BPWM_GetADCTriggerFlag(g_apPWMModule[j], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(BPWM_GetADCTriggerFlag(g_apPWMModule[j], u32TestCh), 1);

            BPWM_DisableADCTrigger(g_apPWMModule[j], u32TestCh);

            BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);

            BPWM_ClearADCTriggerFlag(g_apPWMModule[j], u32TestCh, NULL);

            CU_ASSERT_EQUAL(g_apPWMModule[j]->STATUS & (BPWM_STATUS_EADCTRG0_Msk << u32TestCh), 0);
        }
    }
}

void Func_25_BPWM_GetDutyIntFlag()
{
    uint32_t u32TestCh = 0, j = 0;

    /* Reset PWM */
    ResetBPWM();

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_SET_CNR(g_apPWMModule[j], u32TestCh, 10);
            BPWM_SET_CMR(g_apPWMModule[j], u32TestCh, 5);
            BPWM_SET_PRESCALER(g_apPWMModule[j], u32TestCh, 2);
            BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], 1 << u32TestCh, BPWM_EDGE_ALIGNED);
            BPWM_EnableDutyInt(g_apPWMModule[j], u32TestCh, BPWM_DUTY_INT_DOWN_COUNT_MATCH_CMP);
            BPWM_Start(g_apPWMModule[j], 1 << u32TestCh);

            while (BPWM_GetDutyIntFlag(g_apPWMModule[j], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(BPWM_GetDutyIntFlag(g_apPWMModule[j], u32TestCh), 1);

            BPWM_DisableDutyInt(g_apPWMModule[j], u32TestCh);

            BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);

            BPWM_ClearDutyIntFlag(g_apPWMModule[j], u32TestCh);

            CU_ASSERT_EQUAL(g_apPWMModule[j]->INTSTS & (BPWM_INTSTS_CMPDIF0_Msk << u32TestCh), 0);

            /* change PWM operates at up counter type */
            BPWM_SET_ALIGNED_TYPE(g_apPWMModule[j], 1 << u32TestCh, BPWM_UP_COUNTER);

            BPWM_EnableDutyInt(g_apPWMModule[j], u32TestCh, BPWM_DUTY_INT_UP_COUNT_MATCH_CMP);

            BPWM_Start(g_apPWMModule[j], 1 << u32TestCh);

            while (BPWM_GetDutyIntFlag(g_apPWMModule[j], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(BPWM_GetDutyIntFlag(g_apPWMModule[j], u32TestCh), 1);

            BPWM_DisableDutyInt(g_apPWMModule[j], u32TestCh);

            BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);

            BPWM_ClearDutyIntFlag(g_apPWMModule[j], u32TestCh);

            CU_ASSERT_EQUAL(g_apPWMModule[j]->INTSTS & (BPWM_INTSTS_CMPDIF0_Msk << u32TestCh), 0);
        }
    }
}

void Func_26_BPWM_GetPeriodIntFlag()
{
    uint32_t u32TestCh = 0, j = 0;

    /* Reset PWM */
    ResetBPWM();

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_SET_CNR(g_apPWMModule[j], u32TestCh, 10);
            BPWM_SET_CMR(g_apPWMModule[j], u32TestCh, 5);
            BPWM_SET_PRESCALER(g_apPWMModule[j], u32TestCh, 2);
            BPWM_EnablePeriodInt(g_apPWMModule[j], u32TestCh, NULL);
            BPWM_Start(g_apPWMModule[j], 1 << u32TestCh);

            while (BPWM_GetPeriodIntFlag(g_apPWMModule[j], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(BPWM_GetPeriodIntFlag(g_apPWMModule[j], u32TestCh), 1);

            BPWM_DisablePeriodInt(g_apPWMModule[j], u32TestCh);

            BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);

            BPWM_ClearPeriodIntFlag(g_apPWMModule[j], u32TestCh);

            CU_ASSERT_EQUAL(g_apPWMModule[j]->INTSTS & (BPWM_INTSTS_PIF0_Msk << u32TestCh), 0);
        }
    }
}

void Func_27_BPWM_GetCaptureIntFlag()
{
    volatile uint32_t u32Loop = 0, j = 0, k = 0;
    uint32_t u32TestCh = 0, u32CapCh = 0;

#if (BPWM_MODULE_NUM>=2 && BPWM_ONEPORT_SELF == 0)
    /* Reset PWM */
    ResetBPWM();

    //enable self-test for PWM
    SYS_UnlockReg();
    outpw(BPWM0_BASE + 0x300, 0x1ul << 31); //TwoMode
    outpw(BPWM1_BASE + 0x300, 0x1ul << 31);

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        //case 1: PWM0: output, PWM1: capture(input), PWM2: output, PWM3: capture(input) and PWM4: output, PWM5: capture(input)
        //case 2: PWM0: capture(input), PWM1: output, PWM2: capture(input), PWM3: output and PWM4: capture(input), PWM5: output
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            if (j == 0)
                k = 1 ;
            else
                k = 0 ;

            /*Set Pwm mode*/
#if 0// Auto-reload
            g_apPWMModule[k]->CTL1 &= ~(BPWM_CTL1_CNTMODE1_Msk << u32TestCh);
#endif
            BPWM_SET_CNR(g_apPWMModule[k], u32TestCh, 1000);
            BPWM_SET_CMR(g_apPWMModule[k], u32TestCh, 500);
            BPWM_SET_PRESCALER(g_apPWMModule[k], u32TestCh, 2);
            BPWM_SET_OUTPUT_LEVEL(g_apPWMModule[k], 1 << u32TestCh, BPWM_OUTPUT_HIGH, BPWM_OUTPUT_LOW, BPWM_OUTPUT_NOTHING, BPWM_OUTPUT_NOTHING);
            BPWM_EnableOutput(g_apPWMModule[k], 1 << u32TestCh);
            BPWM_Start(g_apPWMModule[k], 1 << u32TestCh);

            u32CapCh = u32TestCh/*(u32TestCh % 2)? (u32TestCh - 1):(u32TestCh + 1)*/;


            /*Set Pwm mode*/
#if 0// Auto-reload
            g_apPWMModule[j]->CTL1 &= ~(BPWM_CTL1_CNTMODE1_Msk << u32CapCh);
#endif
            BPWM_SET_CNR(g_apPWMModule[j], u32CapCh, 1000);
            //BPWM_SET_CNR(g_apPWMModule[j], u32CapCh, 0xFFFF);
            BPWM_SET_CMR(g_apPWMModule[j], u32CapCh, 500);
            BPWM_SET_PRESCALER(g_apPWMModule[j], u32CapCh, 2);
            BPWM_EnableCaptureInt(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH | BPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CAPIEN, 0x00000101 << u32CapCh);

            BPWM_EnableCapture(g_apPWMModule[j], 1 << u32CapCh);
            BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH | BPWM_CAPTURE_INT_RISING_LATCH);
            BPWM_Start(g_apPWMModule[j], 1 << u32CapCh);

            u32Loop = 0;

            while (BPWM_GetCaptureIntFlag(g_apPWMModule[j], u32CapCh) < 3)
            {
                if (u32Loop++ > (SystemCoreClock / 1000))
                {
                    if (g_apPWMModule[j] == BPWM0)
                    {
                        CU_FAIL("Check BPWM0_CH0(PC.0)<->BPWM0_CH1(PC.1)");
                    }
                    else if (g_apPWMModule[j] == BPWM1)
                    {
                        CU_FAIL("Check BPWM1_CH0(PC.6)<->BPWM1_CH1(PC.7)");
                    }

                    return;
                    //break;
                }
            }

            CU_ASSERT_EQUAL(BPWM_GetCaptureIntFlag(g_apPWMModule[j], u32CapCh), 3);

            BPWM_DisableCapture(g_apPWMModule[j], 1 << u32CapCh);
            BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH | BPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CAPIF, 0);

            CU_ASSERT_EQUAL(BPWM_GET_CAPTURE_RISING_DATA(g_apPWMModule[j], u32CapCh), g_apPWMModule[j]->CAPDAT[1].RCAPDAT);
            CU_ASSERT_EQUAL(BPWM_GET_CAPTURE_FALLING_DATA(g_apPWMModule[j], u32CapCh), g_apPWMModule[j]->CAPDAT[1].FCAPDAT);

            //         BPWM_DisableOutput(g_apPWMModule[j], 1 << u32TestCh);
            //         BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);
            BPWM_ForceStop(g_apPWMModule[j], 1 << u32CapCh);

            BPWM_DisableCaptureInt(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_RISING_LATCH);
            BPWM_EnableCaptureInt(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CAPIEN, 0x00000100 << u32CapCh);

            BPWM_EnableCapture(g_apPWMModule[j], 1 << u32CapCh);
            BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH | BPWM_CAPTURE_INT_RISING_LATCH);
            //BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH);
            BPWM_Start(g_apPWMModule[j], 1 << u32CapCh);

            u32Loop = 0;

            while (BPWM_GetCaptureIntFlag(g_apPWMModule[j], u32CapCh) < 3)
            {
                if (u32Loop++ > (SystemCoreClock / 1000))
                {
                    break;
                }
            }

            //CU_ASSERT_EQUAL(BPWM_GetCaptureIntFlag(g_apPWMModule[j], 1), 2);
            CU_ASSERT_EQUAL(BPWM_GetCaptureIntFlag(g_apPWMModule[j], u32CapCh), 3);

            BPWM_DisableCapture(g_apPWMModule[j], 1 << u32CapCh);
            BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CAPIF, 0x00000001 << u32CapCh);

            //         BPWM_DisableOutput(g_apPWMModule[j], 1 << u32TestCh);
            //         BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);
            BPWM_ForceStop(g_apPWMModule[j], 1 << u32CapCh);

            BPWM_DisableCaptureInt(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH);
            BPWM_EnableCaptureInt(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CAPIEN, 0x000000001 << u32CapCh);

            BPWM_EnableCapture(g_apPWMModule[j], 1 << u32CapCh);
            BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH | BPWM_CAPTURE_INT_RISING_LATCH);
            //BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_RISING_LATCH);
            BPWM_Start(g_apPWMModule[j], 1 << u32CapCh);

            u32Loop = 0;

            while (BPWM_GetCaptureIntFlag(g_apPWMModule[j], u32CapCh) < 3)
            {
                if (u32Loop++ > (SystemCoreClock / 1000))
                {
                    break;
                }
            }

            //CU_ASSERT_EQUAL(BPWM_GetCaptureIntFlag(g_apPWMModule[j], u32CapCh), 2);
            CU_ASSERT_EQUAL(BPWM_GetCaptureIntFlag(g_apPWMModule[j], u32CapCh), 3);

            BPWM_DisableCapture(g_apPWMModule[j], 1 << u32CapCh);
            BPWM_DisableCaptureInt(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_RISING_LATCH);
            BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CAPIF, 0x00000100 << u32CapCh);

            BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH);

            BPWM_DisableOutput(g_apPWMModule[j], 1 << u32TestCh);
            BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);
            BPWM_ForceStop(g_apPWMModule[j], 1 << u32CapCh);
        }
    }

#else
    /* Reset PWM */
    ResetBPWM();

    //enable self-test for PWM
    SYS_UnlockReg();
    outpw(BPWM0_BASE + 0x300, 0x1ul << 31); //TwoMode
    outpw(BPWM1_BASE + 0x300, 0x1ul << 31);

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        for (u32CapCh = 0; u32CapCh < BPWM_MAX_CH; u32CapCh++)
        {
            u32TestCh = (u32CapCh + 1) % BPWM_MAX_CH;

            /*Set Pwm out mode*/
#if 0// Auto-reload
            g_apPWMModule[k]->CTL1 &= ~(BPWM_CTL1_CNTMODE1_Msk << u32TestCh);
#endif
            BPWM_SET_CNR(g_apPWMModule[j], u32TestCh, 10000);
            BPWM_SET_CMR(g_apPWMModule[j], u32TestCh, 5000);
            BPWM_SET_PRESCALER(g_apPWMModule[j], u32TestCh, 2);
            BPWM_SET_OUTPUT_LEVEL(g_apPWMModule[j], 1 << u32TestCh, BPWM_OUTPUT_HIGH, BPWM_OUTPUT_LOW, BPWM_OUTPUT_NOTHING, BPWM_OUTPUT_NOTHING);
            BPWM_EnableOutput(g_apPWMModule[j], 1 << u32TestCh);
            BPWM_Start(g_apPWMModule[j], 1 << u32TestCh);


            /*Set Pwm cap mode*/
#if 0// Auto-reload
            g_apPWMModule[j]->CTL1 &= ~(BPWM_CTL1_CNTMODE1_Msk << u32CapCh);
#endif
            BPWM_SET_CNR(g_apPWMModule[j], u32CapCh, 10000);
            //BPWM_SET_CNR(g_apPWMModule[j], u32CapCh, 0xFFFF);
            BPWM_SET_CMR(g_apPWMModule[j], u32CapCh, 5000);
            BPWM_SET_PRESCALER(g_apPWMModule[j], u32CapCh, 2);
            BPWM_EnableCaptureInt(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH | BPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CAPIEN, 0x00000101 << u32CapCh);

            BPWM_EnableCapture(g_apPWMModule[j], 1 << u32CapCh);
            BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH | BPWM_CAPTURE_INT_RISING_LATCH);
            BPWM_Start(g_apPWMModule[j], 1 << u32CapCh);

            u32Loop = 0;

            while (BPWM_GetCaptureIntFlag(g_apPWMModule[j], u32CapCh) < 3)
            {
                if (u32Loop++ > (SystemCoreClock / 1000))
                {
                    if (g_apPWMModule[j] == BPWM0)
                    {
                        CU_FAIL("Check BPWM0_CH0(PA.0)<->BPWM0_CH1(PA.1)");
                    }
                    else if (g_apPWMModule[j] == BPWM1)
                    {
                        CU_FAIL("Check BPWM1_CH0(PB.11)<->BPWM1_CH1(PB.10)");
                    }

                    return;
                    //break;
                }
            }

            CU_ASSERT_EQUAL(BPWM_GetCaptureIntFlag(g_apPWMModule[j], u32CapCh), 3);

            BPWM_DisableCapture(g_apPWMModule[j], 1 << u32CapCh);
            BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH | BPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CAPIF, 0);

            CU_ASSERT_EQUAL(BPWM_GET_CAPTURE_RISING_DATA(g_apPWMModule[j], u32CapCh), g_apPWMModule[j]->CAPDAT[1].RCAPDAT);
            CU_ASSERT_EQUAL(BPWM_GET_CAPTURE_FALLING_DATA(g_apPWMModule[j], u32CapCh), g_apPWMModule[j]->CAPDAT[1].FCAPDAT);

            //         BPWM_DisableOutput(g_apPWMModule[j], 1 << u32TestCh);
            //         BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);
            BPWM_ForceStop(g_apPWMModule[j], 1 << u32CapCh);

            BPWM_DisableCaptureInt(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_RISING_LATCH);
            BPWM_EnableCaptureInt(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CAPIEN, 0x00000100 << u32CapCh);

            BPWM_EnableCapture(g_apPWMModule[j], 1 << u32CapCh);
            BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH | BPWM_CAPTURE_INT_RISING_LATCH);
            //BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH);
            BPWM_Start(g_apPWMModule[j], 1 << u32CapCh);

            u32Loop = 0;

            while (BPWM_GetCaptureIntFlag(g_apPWMModule[j], u32CapCh) < 3)
            {
                if (u32Loop++ > (SystemCoreClock / 1000))
                {
                    break;
                }
            }

            //CU_ASSERT_EQUAL(BPWM_GetCaptureIntFlag(g_apPWMModule[j], 1), 2);
            CU_ASSERT_EQUAL(BPWM_GetCaptureIntFlag(g_apPWMModule[j], u32CapCh), 3);

            BPWM_DisableCapture(g_apPWMModule[j], 1 << u32CapCh);
            BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CAPIF, 0x00000001 << u32CapCh);

            //         BPWM_DisableOutput(g_apPWMModule[j], 1 << u32TestCh);
            //         BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);
            BPWM_ForceStop(g_apPWMModule[j], 1 << u32CapCh);

            BPWM_DisableCaptureInt(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH);
            BPWM_EnableCaptureInt(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CAPIEN, 0x000000001 << u32CapCh);

            BPWM_EnableCapture(g_apPWMModule[j], 1 << u32CapCh);
            BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH | BPWM_CAPTURE_INT_RISING_LATCH);
            //BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_RISING_LATCH);
            BPWM_Start(g_apPWMModule[j], 1 << u32CapCh);

            u32Loop = 0;

            while (BPWM_GetCaptureIntFlag(g_apPWMModule[j], u32CapCh) < 3)
            {
                if (u32Loop++ > (SystemCoreClock / 1000))
                {
                    break;
                }
            }

            //CU_ASSERT_EQUAL(BPWM_GetCaptureIntFlag(g_apPWMModule[j], u32CapCh), 2);
            CU_ASSERT_EQUAL(BPWM_GetCaptureIntFlag(g_apPWMModule[j], u32CapCh), 3);

            BPWM_DisableCapture(g_apPWMModule[j], 1 << u32CapCh);
            BPWM_DisableCaptureInt(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_RISING_LATCH);
            BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apPWMModule[j]->CAPIF, 0x00000100 << u32CapCh);

            BPWM_ClearCaptureIntFlag(g_apPWMModule[j], u32CapCh, BPWM_CAPTURE_INT_FALLING_LATCH);

            BPWM_DisableOutput(g_apPWMModule[j], 1 << u32TestCh);
            BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);
            BPWM_ForceStop(g_apPWMModule[j], 1 << u32CapCh);
        }
    }

#endif
}

void Func_28_BPWM_ConfigOutputChannel()
{
    uint32_t j = 0, u32TestCh = 0, u32DutyIndex = 0, u32PeriodIndex = 0;
    uint32_t au32Period[2] = {1000, 100};
    uint32_t au32Duty[3] = {50, 0, 100}; /* 50%, 0%, 100% */
    uint32_t rPeriod ;

    /*                                {{PERIOD1, PERIOD2},{Duty1, Duty2},{Psc1, Psc2}} */
    uint32_t au32PCLKSample[3][3][2] = {{{0xC34F, 0xF423}, {0x61A8, 0x7A12}, {1, 0xF}},
        {{0xC34F, 0xF423}, {0, 0}, {1, 0xF}},
        {{0xC34F, 0xF423}, {0xC34F, 0xF423}, {1, 0xF}}
    };
    uint32_t au32HXTSample[3][3][2] = {{{11999, 59999}, {6000, 30000}, {0, 1}},
        {{11999, 59999}, {0, 0}, {0, 1}},
        {{11999, 59999}, {11999, 59999}, {0, 1}}
    };
#if 0
    //22M
    uint32_t au32HIRCSample[3][3][2] = {{{22117, 55295}, {11059, 27648}, {0, 3}},
        {{22117, 55295}, {0, 0}, {0, 3}},
        {{22117, 55295}, {22118, 55296}, {0, 3}}
    };
#endif
    //48M
    uint32_t au32HIRCSample[3][3][2] = {{{47999, 59999}, {24000, 30000}, {0, 7}},
        {{47999, 59999}, {0, 0}, {0, 7}},
        {{47999, 59999}, {47999, 59999}, {0, 7}}
    };


    uint32_t au32LIRCSample[3][3][2] = {{{31, 326}, {16, 163}, {0, 0}},
        {{31, 326}, {0, 0}, {0, 0}},
        {{31, 326}, {31, 326}, {0, 0}}
    };

#if 0
    /* PLL clock is 72MHz */
    uint32_t au32PLLSample[3][3][2] = {{{35999, 65453}, {18000, 32727}, {1, 10}},
        {{35999, 65453}, {0, 0}, {1, 10}},
        {{35999, 65453}, {36000, 65454}, {1, 10}}
    };
#endif
    /* Reset PWM */
    ResetBPWM();

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        for (u32DutyIndex = 0; u32DutyIndex < sizeof(au32Duty) / sizeof(uint32_t); u32DutyIndex++)
        {
            for (u32PeriodIndex = 0; u32PeriodIndex < sizeof(au32Period) / sizeof(uint32_t); u32PeriodIndex++)
            {
                for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
                {
                    CU_ASSERT_EQUAL(BPWM_ConfigOutputChannel(g_apPWMModule[j], u32TestCh, au32Period[u32PeriodIndex], au32Duty[u32DutyIndex]), au32Period[u32PeriodIndex]);
                    CU_ASSERT_EQUAL(g_apPWMModule[j]->PERIOD, au32PCLKSample[u32DutyIndex][0][u32PeriodIndex]);
                    CU_ASSERT_EQUAL(g_apPWMModule[j]->CMPDAT[u32TestCh], au32PCLKSample[u32DutyIndex][1][u32PeriodIndex]);
                    CU_ASSERT_EQUAL((*(__IO uint32_t *)(&(g_apPWMModule[j]->CLKPSC))), au32PCLKSample[u32DutyIndex][2][u32PeriodIndex]);
#if 0  //auto-reload mode
                    CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & (BPWM_CTL1_CNTMODE0_Msk << u32TestCh), 0 << (16 + u32TestCh));//auto-reload mode
#endif
                    CU_ASSERT_EQUAL((g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk), BPWM_UP_COUNTER); //edge-aligned type
                }
            }
        }
    }


    //----------------------------
    //PWM clock is HIRC: 48MHz
    //----------------------------
    //    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
    //
    //    for (j = 0; j < BPWM_MODULE_NUM; j++)
    //    {
    //        for (u32DutyIndex = 0; u32DutyIndex < sizeof(au32Duty)/sizeof(uint32_t); u32DutyIndex++)
    //        {
    //            for (u32PeriodIndex = 0; u32PeriodIndex < sizeof(au32Period)/sizeof(uint32_t); u32PeriodIndex++)
    //            {
    //                for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
    //                {
    //                    CU_ASSERT_EQUAL(BPWM_ConfigOutputChannel(g_apPWMModule[j], u32TestCh, au32Period[u32PeriodIndex], au32Duty[u32DutyIndex]), au32Period[u32PeriodIndex]);
    //                    CU_ASSERT_EQUAL(g_apPWMModule[j]->PERIOD, au32HIRCSample[u32DutyIndex][0][u32PeriodIndex]);
    //                    CU_ASSERT_EQUAL(g_apPWMModule[j]->CMPDAT[u32TestCh], au32HIRCSample[u32DutyIndex][1][u32PeriodIndex]);
    //                    CU_ASSERT_EQUAL((*(__IO uint32_t *) (&(g_apPWMModule[j]->CLKPSC))), au32HIRCSample[u32DutyIndex][2][u32PeriodIndex]);
    //#if 0  //auto-reload mode
    //                    CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & (BPWM_CTL1_CNTMODE0_Msk << u32TestCh), 0 << (16 + u32TestCh));//auto-reload mode
    //#endif
    //                    CU_ASSERT_EQUAL((g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk ), BPWM_DOWN_COUNTER);//edge-aligned type
    //                }
    //            }
    //        }
    //    }

#if 0
    //----------------------------
    //PWM clock is LIRC: 10KHz
    //----------------------------
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_LIRC, CLK_CLKDIV0_HCLK(1));;
    UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__LIRC, 14400);

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        for (u32DutyIndex = 0; u32DutyIndex < sizeof(au32Duty) / sizeof(uint32_t); u32DutyIndex++)
        {
            for (u32PeriodIndex = 0; u32PeriodIndex < sizeof(au32Period) / sizeof(uint32_t); u32PeriodIndex++)
            {
                for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
                {
#if 0
                    CU_ASSERT_EQUAL(BPWM_ConfigOutputChannel(g_apPWMModule[j], u32TestCh, au32Period[u32PeriodIndex], au32Duty[u32DutyIndex]), au32Period[u32PeriodIndex]);
#else
                    rPeriod = BPWM_ConfigOutputChannel(g_apPWMModule[j], u32TestCh, au32Period[u32PeriodIndex], au32Duty[u32DutyIndex]) ;
                    CU_ASSERT_EQUAL(rPeriod, __LXT / (au32LIRCSample[u32DutyIndex][0][u32PeriodIndex] + 1)) ;

#endif
                    CU_ASSERT_EQUAL(g_apPWMModule[j]->PERIOD, au32LIRCSample[u32DutyIndex][0][u32PeriodIndex]);
                    CU_ASSERT_EQUAL(g_apPWMModule[j]->CMPDAT[u32TestCh], au32LIRCSample[u32DutyIndex][1][u32PeriodIndex]);
                    CU_ASSERT_EQUAL((*(__IO uint32_t *)(&(g_apPWMModule[j]->CLKPSC))), au32LIRCSample[u32DutyIndex][2][u32PeriodIndex]);
#if 0  //auto-reload mode
                    CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & (BPWM_CTL1_CNTMODE0_Msk << u32TestCh), 0 << (16 + u32TestCh));//auto-reload mode
#endif
                    CU_ASSERT_EQUAL((g_apPWMModule[j]->CTL1 & (BPWM_CTL1_CNTTYPE0_Msk)), BPWM_DOWN_COUNTER);//edge-aligned type
                }
            }
        }
    }

    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~(CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_HXT);
    UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HXT, 115200);
#endif
#if 0
#if 0
    //----------------------------
    //PWM clock is PLL: PLL_CLOCK = 72000000
    //----------------------------
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~(CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_PLL);

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        for (u32DutyIndex = 0; u32DutyIndex < sizeof(au32Duty) / sizeof(uint32_t); u32DutyIndex++)
        {
            for (u32PeriodIndex = 0; u32PeriodIndex < sizeof(au32Period) / sizeof(uint32_t); u32PeriodIndex++)
            {
                for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
                {
                    CU_ASSERT_EQUAL(BPWM_ConfigOutputChannel(g_apPWMModule[j], u32TestCh, au32Period[u32PeriodIndex], au32Duty[u32DutyIndex]), au32Period[u32PeriodIndex]);
                    CU_ASSERT_EQUAL(g_apPWMModule[j]->PERIOD[u32TestCh], au32PLLSample[u32DutyIndex][0][u32PeriodIndex]);
                    CU_ASSERT_EQUAL(g_apPWMModule[j]->CMPDAT[u32TestCh], au32PLLSample[u32DutyIndex][1][u32PeriodIndex]);
                    CU_ASSERT_EQUAL((*(__IO uint32_t *)(&(g_apPWMModule[j]->CLKPSC))), au32PLLSample[u32DutyIndex][2][u32PeriodIndex]);
#if 0  //auto-reload mode
                    CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & (BPWM_CTL1_CNTMODE0_Msk << u32TestCh), 0 << (16 + u32TestCh));//auto-reload mode
#endif
                    CU_ASSERT_EQUAL((g_apPWMModule[j]->CTL1 & (BPWM_CTL1_CNTTYPE0_Msk << (2 * u32TestCh))), BPWM_UP_COUNTER << (2 * u32TestCh));//edge-aligned type
                }
            }
        }
    }

#endif
#endif
}

void Func_29_BPWM_ConfigCaptureChannel()
{
    uint32_t j = 0, u32TestCh = 0, u32CapTimeIndex = 0;
    uint32_t au32CapTime[2] = {1000, 100};
    uint32_t rUnitTime ;

    /*                             {{RealCapTime1, RealCapTime2},{PERIOD1, PERIOD2},{Psc1, Psc2}} */
    uint32_t au32PCLKSample[3][2] = {{0x3E8, 0x64}, {0xFFFF, 0xFFFF}, {0x63, 9}};
    uint32_t au32HXTSample[3][2] = {{1000, 166}, {0xFFFF, 0xFFFF}, {0xB, 1}};

#if 0
    uint32_t au32HIRCSample[3][2] = {{1039, 135}, {0xFFFF, 0xFFFF}, {0x16, 2}}; //22M
#else
    uint32_t au32HIRCSample[3][2] = {{1000, 104}, {0xFFFF, 0xFFFF}, {0x2F, 4}}; //48M
#endif


    uint32_t au32LIRCSample[3][2] = {{100000, 100000}, {0xFFFF, 0xFFFF}, {0, 0}};

#if 0
    /* PLL clock is 72MHz */
    uint32_t au32PLLSample[3][2] = {{1000, 111}, {0xFFFF, 0xFFFF}, {0x47, 7}};
#endif
    /* Reset PWM */
    ResetBPWM();

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        for (u32CapTimeIndex = 0; u32CapTimeIndex < sizeof(au32CapTime) / sizeof(uint32_t); u32CapTimeIndex++)
        {
            for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
            {
                CU_ASSERT_EQUAL(BPWM_ConfigCaptureChannel(g_apPWMModule[j], u32TestCh, au32CapTime[u32CapTimeIndex], NULL), au32PCLKSample[0][u32CapTimeIndex]);
                CU_ASSERT_EQUAL(g_apPWMModule[j]->PERIOD, au32PCLKSample[1][u32CapTimeIndex]);
                CU_ASSERT_EQUAL(g_apPWMModule[j]->CMPDAT[u32TestCh], 0);
                CU_ASSERT_EQUAL((*(__IO uint32_t *)(&(g_apPWMModule[j]->CLKPSC))), au32PCLKSample[2][u32CapTimeIndex]);
#if 0  //auto-reload mode
                CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & (BPWM_CTL1_CNTMODE0_Msk << u32TestCh), 0 << (16 + u32TestCh));//auto-reload mode
#endif
                CU_ASSERT_EQUAL((g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk), BPWM_DOWN_COUNTER); //edge-aligned type
            }
        }
    }


    //----------------------------
    //PWM clock is HIRC: 22.1184MHz
    //----------------------------
    //    CLK->CLKSEL0 = ((CLK->CLKSEL0 & ~(CLK_CLKSEL0_HCLKSEL_Msk)) | CLK_CLKSEL0_HCLKSEL_HIRC);

    //    for (j = 0; j < BPWM_MODULE_NUM; j++)
    //    {
    //        #if 0
    //        CU_ASSERT_EQUAL(BPWM_ConfigCaptureChannel(g_apPWMModule[j], 0, 1000, NULL), 1039);
    //        CU_ASSERT_EQUAL(g_apPWMModule[j]->PERIOD[0], 0xFFFF);
    //        CU_ASSERT_EQUAL(g_apPWMModule[j]->CLKPSC0_1, 0x16);//prescaler = 12
    //#if 0  //auto-reload mode
    //        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTMODE0_Msk, 0 << 16);//auto-reload mode
    //#endif
    //        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, BPWM_DOWN_COUNTER);//edge-aligned type

    //        CU_ASSERT_EQUAL(BPWM_ConfigCaptureChannel(g_apPWMModule[j], 1, 100, NULL), 135);
    //        CU_ASSERT_EQUAL(g_apPWMModule[j]->PERIOD[1], 0xFFFF);
    //        CU_ASSERT_EQUAL(g_apPWMModule[j]->CLKPSC0_1, 0x2);//prescaler = 2
    //#if 0  //auto-reload mode
    //        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTMODE0_Msk, 0 << 17);//auto-reload mode
    //#endif
    //        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, BPWM_DOWN_COUNTER);//edge-aligned type
    //        #endif
    //        for (u32CapTimeIndex = 0; u32CapTimeIndex < sizeof(au32CapTime)/sizeof(uint32_t); u32CapTimeIndex++)
    //        {
    //            for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
    //            {
    //                CU_ASSERT_EQUAL(BPWM_ConfigCaptureChannel(g_apPWMModule[j], u32TestCh, au32CapTime[u32CapTimeIndex], NULL), au32HIRCSample[0][u32CapTimeIndex]);
    //                CU_ASSERT_EQUAL(g_apPWMModule[j]->PERIOD, au32HIRCSample[1][u32CapTimeIndex]);
    //                CU_ASSERT_EQUAL(g_apPWMModule[j]->CMPDAT[u32TestCh], 0);
    //                CU_ASSERT_EQUAL((*(__IO uint32_t *) (&(g_apPWMModule[j]->CLKPSC))), au32HIRCSample[2][u32CapTimeIndex]);
    //#if 0  //auto-reload mode
    //                CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & (BPWM_CTL1_CNTMODE0_Msk << u32TestCh), 0 << (16 + u32TestCh));//auto-reload mode
    //#endif
    //                CU_ASSERT_EQUAL((g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk ), BPWM_DOWN_COUNTER);//edge-aligned type
    //            }
    //        }
    //    }

#if 0
    //----------------------------
    //PWM clock is LIRC: 10KHz
    //----------------------------
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~(CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_LIRC);

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
#if 0
        CU_ASSERT_EQUAL(BPWM_ConfigCaptureChannel(g_apPWMModule[j], 0, 1000, NULL), 100000);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->PERIOD[0], 0xFFFF);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CLKPSC0_1, 0);//prescaler = 12
#if 0  //auto-reload mode
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTMODE0_Msk, 0 << 16);//auto-reload mode
#endif
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, BPWM_DOWN_COUNTER);//edge-aligned type

        CU_ASSERT_EQUAL(BPWM_ConfigCaptureChannel(g_apPWMModule[j], 1, 100, NULL), 100000);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->PERIOD[1], 0xFFFF);
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CLKPSC0_1, 0);//prescaler = 2
#if 0  //auto-reload mode
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTMODE0_Msk, 0 << 17);//auto-reload mode
#endif
        CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk, BPWM_DOWN_COUNTER);//edge-aligned type
#endif

        for (u32CapTimeIndex = 0; u32CapTimeIndex < sizeof(au32CapTime) / sizeof(uint32_t); u32CapTimeIndex++)
        {
            for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
            {
#if 0
                CU_ASSERT_EQUAL(BPWM_ConfigCaptureChannel(g_apPWMModule[j], u32TestCh, au32CapTime[u32CapTimeIndex], NULL), au32LIRCSample[0][u32CapTimeIndex]);
#else
                rUnitTime = BPWM_ConfigCaptureChannel(g_apPWMModule[j], u32TestCh, au32CapTime[u32CapTimeIndex], NULL) ;
                CU_ASSERT_EQUAL(rUnitTime, 1000000 / (__LXT / 1000)) ;
#endif
                CU_ASSERT_EQUAL(g_apPWMModule[j]->PERIOD, au32LIRCSample[1][u32CapTimeIndex]);
                CU_ASSERT_EQUAL(g_apPWMModule[j]->CMPDAT[u32TestCh], 0);
                CU_ASSERT_EQUAL((*(__IO uint32_t *)(&(g_apPWMModule[j]->CLKPSC))), au32LIRCSample[2][u32CapTimeIndex]);
#if 0  //auto-reload mode
                CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & (BPWM_CTL1_CNTMODE0_Msk << u32TestCh), 0 << (16 + u32TestCh));//auto-reload mode
#endif
                CU_ASSERT_EQUAL((g_apPWMModule[j]->CTL1 & BPWM_CTL1_CNTTYPE0_Msk), BPWM_DOWN_COUNTER); //edge-aligned type
            }
        }
    }

    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~(CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_HXT);
#endif
#if 0
#if 0
    //----------------------------
    //PWM clock is PLL: PLL_CLOCK = 72000000
    //----------------------------
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~(CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_PLL);

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        for (u32CapTimeIndex = 0; u32CapTimeIndex < sizeof(au32CapTime) / sizeof(uint32_t); u32CapTimeIndex++)
        {
            for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
            {
                CU_ASSERT_EQUAL(BPWM_ConfigCaptureChannel(g_apPWMModule[j], u32TestCh, au32CapTime[u32CapTimeIndex], NULL), au32PLLSample[0][u32CapTimeIndex]);
                CU_ASSERT_EQUAL(g_apPWMModule[j]->PERIOD, au32PLLSample[1][u32CapTimeIndex]);
                CU_ASSERT_EQUAL(g_apPWMModule[j]->CMPDAT[u32TestCh], 0);
                CU_ASSERT_EQUAL((*(__IO uint32_t *)(&(g_apPWMModule[j]->CLKPSC))), au32PLLSample[2][u32CapTimeIndex]);
#if 0  //auto-reload mode
                CU_ASSERT_EQUAL(g_apPWMModule[j]->CTL1 & (BPWM_CTL1_CNTMODE0_Msk << u32TestCh), 0 << (16 + u32TestCh));//auto-reload mode
#endif
                CU_ASSERT_EQUAL((g_apPWMModule[j]->CTL1 & (BPWM_CTL1_CNTTYPE0_Msk << (2 * u32TestCh))), BPWM_DOWN_COUNTER << (2 * u32TestCh));//edge-aligned type
            }
        }
    }

#endif
#endif
}

void Func_33_BPWM_EnableZeroInt()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t au32RegCheck[6] = {0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001};
    uint32_t au32RegReverseCheck[6] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};

    /* Reset PWM */
    ResetBPWM();

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnableZeroInt(g_apPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->INTEN & (0x3ful << BPWM_INTEN_ZIEN0_Pos), 1);
            BPWM_DisableZeroInt(g_apPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->INTEN, 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnableZeroInt(g_apPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->INTEN, au32RegCheck[u32TestCh]);
        }

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_DisableZeroInt(g_apPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->INTEN, au32RegReverseCheck[u32TestCh]);
        }
    }
}

void Func_34_BPWM_GetZeroIntFlag()
{
    uint32_t u32TestCh = 0, j = 0;

    /* Reset PWM */
    ResetBPWM();

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_SET_CNR(g_apPWMModule[j], u32TestCh, 10);
            BPWM_SET_CMR(g_apPWMModule[j], u32TestCh, 5);
            BPWM_SET_PRESCALER(g_apPWMModule[j], u32TestCh, 2);
            BPWM_EnableZeroInt(g_apPWMModule[j], u32TestCh);
            BPWM_Start(g_apPWMModule[j], 1 << u32TestCh);

            while (BPWM_GetZeroIntFlag(g_apPWMModule[j], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(BPWM_GetZeroIntFlag(g_apPWMModule[j], u32TestCh), 1);

            BPWM_DisableZeroInt(g_apPWMModule[j], u32TestCh);

            BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);

            BPWM_ClearZeroIntFlag(g_apPWMModule[j], u32TestCh);

            CU_ASSERT_EQUAL(g_apPWMModule[j]->INTSTS & (BPWM_INTSTS_ZIF0_Msk << u32TestCh), 0);
        }
    }
}

void Func_39_BPWM_EnableLoadMode()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset PWM */
    ResetBPWM();

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnableLoadMode(g_apPWMModule[x], u32TestCh, BPWM_LOAD_MODE_IMMEDIATE);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CTL0 & 0x3F0000, 1 << u32TestCh << 16);
            BPWM_DisableLoadMode(g_apPWMModule[x], u32TestCh, BPWM_LOAD_MODE_IMMEDIATE);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CTL0 & 0x3F0000, 0);
#if 0
            BPWM_EnableLoadMode(g_apPWMModule[x], u32TestCh, BPWM_LOAD_MODE_WINDOW);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CTL0 & 0x3F00, 1 << u32TestCh << 8);
            BPWM_DisableLoadMode(g_apPWMModule[x], u32TestCh, BPWM_LOAD_MODE_WINDOW);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CTL0 & 0x3F00, 0);
#endif
            BPWM_EnableLoadMode(g_apPWMModule[x], u32TestCh, BPWM_LOAD_MODE_CENTER);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CTL0 & 0x3F, 1 << u32TestCh);
            BPWM_DisableLoadMode(g_apPWMModule[x], u32TestCh, BPWM_LOAD_MODE_CENTER);
            CU_ASSERT_EQUAL(g_apPWMModule[x]->CTL0 & 0x3F, 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_EnableLoadMode(g_apPWMModule[x], u32TestCh, BPWM_LOAD_MODE_IMMEDIATE);
#if 0
            BPWM_EnableLoadMode(g_apPWMModule[x], u32TestCh, BPWM_LOAD_MODE_WINDOW);
#endif
            BPWM_EnableLoadMode(g_apPWMModule[x], u32TestCh, BPWM_LOAD_MODE_CENTER);
        }

        CU_ASSERT_EQUAL(g_apPWMModule[x]->CTL0, 0x003F003F);

        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_DisableLoadMode(g_apPWMModule[x], u32TestCh, BPWM_LOAD_MODE_IMMEDIATE);
#if 0
            BPWM_DisableLoadMode(g_apPWMModule[x], u32TestCh, BPWM_LOAD_MODE_WINDOW);
#endif
            BPWM_DisableLoadMode(g_apPWMModule[x], u32TestCh, BPWM_LOAD_MODE_CENTER);
        }

        CU_ASSERT_EQUAL(g_apPWMModule[x]->CTL0, 0);
    }
}

void Func_44_BPWM_SetClockSource()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t u32PatternIdx = 0;
    uint32_t au32RegPattern[5] = {BPWM_CLKSRC_BPWM_CLK, BPWM_CLKSRC_TIMER0, BPWM_CLKSRC_TIMER1, BPWM_CLKSRC_TIMER2, BPWM_CLKSRC_TIMER3};
    uint32_t au32RegCheck[5 * 6] = {0x0, 0x1, 0x2, 0x3, 0x4, \
                                    0x0, 0x1, 0x2, 0x3, 0x4, \
                                    0x4, 0x104, 0x204, 0x304, 0x404, \
                                    0x4, 0x104, 0x204, 0x304, 0x404, \
                                    0x404, 0x10404, 0x20404, 0x30404, 0x40404, \
                                    0x404, 0x10404, 0x20404, 0x30404, 0x40404
                                   };
    /* Reset PWM */
    ResetBPWM();

    for (x = 0; x < BPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            for (u32PatternIdx = 0; u32PatternIdx < sizeof(au32RegPattern) / sizeof(uint32_t); u32PatternIdx++)
            {
                BPWM_SetClockSource(g_apPWMModule[x], u32TestCh, au32RegPattern[u32PatternIdx]);
                CU_ASSERT_EQUAL(g_apPWMModule[x]->CLKSRC, au32RegCheck[u32PatternIdx]);
            }
        }
    }
}

void Func_49_BPWM_GetWrapAroundFlag()
{
    uint32_t u32TestCh = 0, j = 0;

    /* Reset PWM */
    ResetBPWM();

    for (j = 0; j < BPWM_MODULE_NUM; j++)
    {
        for (u32TestCh = 0; u32TestCh < BPWM_MAX_CH; u32TestCh++)
        {
            BPWM_SET_CNR(g_apPWMModule[j], u32TestCh, 0xFFFF);
            BPWM_SET_CMR(g_apPWMModule[j], u32TestCh, 5);
            BPWM_SET_PRESCALER(g_apPWMModule[j], u32TestCh, 0);
            BPWM_Start(g_apPWMModule[j], 1 << u32TestCh);

            while (BPWM_GetWrapAroundFlag(g_apPWMModule[j], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(BPWM_GetWrapAroundFlag(g_apPWMModule[j], u32TestCh), 1);

            BPWM_ForceStop(g_apPWMModule[j], 1 << u32TestCh);

            BPWM_ClearWrapAroundFlag(g_apPWMModule[j], u32TestCh);

            CU_ASSERT_EQUAL(g_apPWMModule[j]->STATUS & (BPWM_STATUS_CNTMAX0_Msk << u32TestCh), 0);
        }
    }
}

CU_SuiteInfo BPWMSuites[] =
{
    { "BPWM Const Test", BPWM_Test_Init, BPWM_Test_Clean, NULL, NULL, BPWM_ConstTest },
    { "BPWM Macro Test", BPWM_Test_Init, BPWM_Test_Clean, NULL, NULL, BPWM_MacroTest },
    { "BPWM Func  Test", BPWM_Test_Init, BPWM_Test_Clean, NULL, NULL, BPWM_FuncTest },
    CU_SUITE_INFO_NULL
};

CU_TestInfo  BPWM_ConstTest[] =
{
    { "Test BPWM_CONSTANT",  Func_BPWM_CONSTANT },
    CU_TEST_INFO_NULL
};

CU_TestInfo  BPWM_MacroTest[] =
{
//    {"1. BPWM_ENABLE_OUTPUT_INVERTER():", Func_1_BPWM_ENABLE_OUTPUT_INVERTER},
//    {"2. BPWM_SET_PRESCALER()/BPWM_GET_PRESCALER():", Func_2_BPWM_SET_PRESCALER},
//    {"3. BPWM_SET_CMR()/BPWM_SET_CNR()/BPWM_GET_CMR()/BPWM_GET_CNR():", Func_3_BPWM_SET_CMR},
//    {"4. BPWM_SET_ALIGNED_TYPE():", Func_4_BPWM_SET_ALIGNED_TYPE},
//    {"7. BPWM_ENABLE_TIMER_SYNC()/BPWM_DISABLE_TIMER_SYNC()/BPWM_TRIGGER_SYNC_START():", Func_7_BPWM_ENABLE_TIMER_SYNC},
//    {"8. BPWM_MASK_OUTPUT():", Func_8_BPWM_MASK_OUTPUT},
//    {"12. BPWM_CLR_COUNTER():", Func_12_BPWM_CLR_COUNTER},
//    {"13. BPWM_SET_OUTPUT_LEVEL():", Func_13_BPWM_SET_OUTPUT_LEVEL},
    CU_TEST_INFO_NULL
};

CU_TestInfo  BPWM_FuncTest[] =
{
//    {"15. BPWM_Start()/BPWM_ForceStop()/BPWM_Stop():", Func_15_BPWM_Stop},
//    {"16. BPWM_EnableADCTrigger()/BPWM_DisableADCTrigger():", Func_16_BPWM_EnableADCTrigger},
//    {"17. BPWM_EnableCapture()/BPWM_DisableCapture():", Func_17_BPWM_EnableCapture},
//    {"18. BPWM_EnableOutput()/BPWM_DisableOutput():", Func_18_BPWM_EnableOutput},
//    {"21. BPWM_EnableCaptureInt()/BPWM_DisableCaptureInt():", Func_21_BPWM_EnableCaptureInt},
//    {"22. BPWM_EnableDutyInt()/BPWM_DisableDutyInt():", Func_22_BPWM_EnableDutyInt},
//    {"23. BPWM_EnablePeriodInt()/BPWM_DisablePeriodInt():", Func_23_BPWM_EnablePeriodInt},
//    {"24. BPWM_GetADCTriggerFlag()/BPWM_ClearADCTriggerFlag():", Func_24_BPWM_GetADCTriggerFlag},
//    {"25. BPWM_GetDutyIntFlag()/BPWM_ClearDutyIntFlag():", Func_25_BPWM_GetDutyIntFlag},
//    {"26. BPWM_GetPeriodIntFlag()/BPWM_ClearPeriodIntFlag():", Func_26_BPWM_GetPeriodIntFlag},
//    {"27. BPWM_GetCaptureIntFlag()/BPWM_ClearCaptureIntFlag()/BPWM_GET_CAPTURE_RISING_DATA()/BPWM_GET_CAPTURE_FALLING_DATA():", Func_27_BPWM_GetCaptureIntFlag},

    {"28. BPWM_ConfigOutputChannel():", Func_28_BPWM_ConfigOutputChannel},
    {"29. BPWM_ConfigCaptureChannel():", Func_29_BPWM_ConfigCaptureChannel},
    {"33. BPWM_EnableZeroInt()/BPWM_DisableZeroInt():", Func_33_BPWM_EnableZeroInt},
    {"34. BPWM_GetZeroIntFlag()/BPWM_ClearZeroIntFlag():", Func_34_BPWM_GetZeroIntFlag},
    {"39. BPWM_EnableLoadMode()/BPWM_DisableLoadMode():", Func_39_BPWM_EnableLoadMode},
    {"44. BPWM_SetClockSource():", Func_44_BPWM_SetClockSource},
    {"49. BPWM_GetWrapAroundFlag()/BPWM_ClearWrapAroundFlag():", Func_49_BPWM_GetWrapAroundFlag},
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
