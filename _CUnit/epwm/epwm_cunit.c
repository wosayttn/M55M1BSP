/******************************************************************************
* @file    epwm_cunit.c
* @version V1.00
* @brief   CUnit test epwm
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
#include "epwm_cunit.h"

/*---------------------------------------------------------------------------------------------------------*/
/*  Constant Definitions                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/

#define EPWM_MAX_CH EPWM_CHANNEL_NUM
uint32_t SixBitsPatternTable[4] = {0x3F, 0x2A, 0x15, 0x00}; /* 6 bits test pattern */
uint32_t EightBitsPatternTable[4] = {0xFF, 0x5A, 0xA5, 0x00}; /* 8 bits test pattern */
uint32_t TwelveBitsPatternTable[4] = {0xFFF, 0xA5A, 0x5A5, 0x000}; /* 12 bits test pattern */
uint32_t SixteenBitsPatternTable[4] = {0xFFFF, 0x5A5A, 0xA5A5, 0x0000}; /* 16 bits test pattern */

#define EPWM_MODULE_NUM 2
EPWM_T *g_apEPWMModule[EPWM_MODULE_NUM] = {EPWM0, EPWM1};

int EPWM_Test_Init(void)
{
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Enable IP module clock */
    CLK_EnableModuleClock(EPWM0_MODULE);
    CLK_EnableModuleClock(EPWM1_MODULE);

    //     /* PWM clock frequency can be set equal or double to HCLK by choosing case 1 or case 2 */
    //     /* case 1.PWM clock frequency is set equal to HCLK: select PWM module clock source as PCLK */
    CLK_SetModuleClock(EPWM0_MODULE, CLK_EPWMSEL_EPWM0SEL_PCLK0, 0);
    CLK_SetModuleClock(EPWM1_MODULE, CLK_EPWMSEL_EPWM1SEL_PCLK2, 0);

    //     /* case 2.PWM clock frequency is set double to HCLK: select PWM module clock source as PLL */
    //CLK_SetModuleClock(EPWM0_MODULE, CLK_EPWMSEL_EPWM0SEL_HCLK0, 0);
    //CLK_SetModuleClock(EPWM1_MODULE, CLK_EPWMSEL_EPWM1SEL_HCLK0, 0);

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    /* Set PA multi-function pins for EPWM0 Channel 0~5 */
    SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~(SYS_GPA_MFP0_PA0MFP_Msk)) | SYS_GPA_MFP0_PA0MFP_EPWM0_CH5;
    SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~(SYS_GPA_MFP0_PA1MFP_Msk)) | SYS_GPA_MFP0_PA1MFP_EPWM0_CH4;
    SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~(SYS_GPA_MFP0_PA2MFP_Msk)) | SYS_GPA_MFP0_PA2MFP_EPWM0_CH3;
    SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~(SYS_GPA_MFP0_PA3MFP_Msk)) | SYS_GPA_MFP0_PA3MFP_EPWM0_CH2;
    SYS->GPA_MFP1 = (SYS->GPA_MFP1 & ~(SYS_GPA_MFP1_PA4MFP_Msk)) | SYS_GPA_MFP1_PA4MFP_EPWM0_CH1;
    SYS->GPA_MFP1 = (SYS->GPA_MFP1 & ~(SYS_GPA_MFP1_PA5MFP_Msk)) | SYS_GPA_MFP1_PA5MFP_EPWM0_CH0;
    GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT1, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT2, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT3, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT4, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT5, GPIO_MODE_OUTPUT);

    /* Set PC multi-function pins for EPWM1 Channel 0~5 */
    SYS->GPB_MFP3 = (SYS->GPB_MFP3 & ~(SYS_GPB_MFP3_PB15MFP_Msk)) | SYS_GPB_MFP3_PB15MFP_EPWM1_CH0;
    SYS->GPB_MFP3 = (SYS->GPB_MFP3 & ~(SYS_GPB_MFP3_PB14MFP_Msk)) | SYS_GPB_MFP3_PB14MFP_EPWM1_CH1;
    SYS->GPB_MFP3 = (SYS->GPB_MFP3 & ~(SYS_GPB_MFP3_PB13MFP_Msk)) | SYS_GPB_MFP3_PB13MFP_EPWM1_CH2;
    SYS->GPB_MFP3 = (SYS->GPB_MFP3 & ~(SYS_GPB_MFP3_PB12MFP_Msk)) | SYS_GPB_MFP3_PB12MFP_EPWM1_CH3;
    SYS->GPA_MFP1 = (SYS->GPA_MFP1 & ~(SYS_GPA_MFP1_PA7MFP_Msk)) | SYS_GPA_MFP1_PA7MFP_EPWM1_CH4;
    SYS->GPA_MFP1 = (SYS->GPA_MFP1 & ~(SYS_GPA_MFP1_PA6MFP_Msk)) | SYS_GPA_MFP1_PA6MFP_EPWM1_CH5;
    GPIO_SetMode(PB, BIT15, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PB, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PB, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PB, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT7, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT6, GPIO_MODE_OUTPUT);

    return 0;
}
int EPWM_Test_Clean(void)
{
    /* Disable EPWM module clock */
    CLK_DisableModuleClock(EPWM0_MODULE);
    CLK_DisableModuleClock(EPWM1_MODULE);
    return 0;
}
void Func_EPWM_Start()
{
    uint32_t u32TestCh = 0, j = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (j = 0; j < EPWM_MODULE_NUM; j++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_Start(g_apEPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->CNTEN & ((1 << EPWM_CNTEN_CNTEN0_Pos) << u32TestCh), 1 << u32TestCh);

            EPWM_ForceStop(g_apEPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->CNTEN & ((1 << EPWM_CNTEN_CNTEN0_Pos) << u32TestCh), 0 << u32TestCh);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_Start(g_apEPWMModule[j], 1 << u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CNTEN, 0x0000003F);

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_ForceStop(g_apEPWMModule[j], 1 << u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CNTEN, 0);

        //test case 3: enable/disable multiple channels each time
        EPWM_Start(g_apEPWMModule[j], EPWM_CH_0_MASK | EPWM_CH_1_MASK);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CNTEN, 0x00000003);

        EPWM_ForceStop(g_apEPWMModule[j], EPWM_CH_0_MASK | EPWM_CH_1_MASK);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CNTEN, 0);

        EPWM_Start(g_apEPWMModule[j], EPWM_CH_0_MASK | EPWM_CH_1_MASK | EPWM_CH_2_MASK);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CNTEN, 0x00000007);

        EPWM_ForceStop(g_apEPWMModule[j], EPWM_CH_0_MASK | EPWM_CH_1_MASK | EPWM_CH_2_MASK);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CNTEN, 0);

        EPWM_Start(g_apEPWMModule[j], EPWM_CH_0_MASK | EPWM_CH_1_MASK | EPWM_CH_2_MASK | EPWM_CH_3_MASK);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CNTEN, 0x000000F);

        EPWM_ForceStop(g_apEPWMModule[j], EPWM_CH_0_MASK | EPWM_CH_1_MASK | EPWM_CH_2_MASK | EPWM_CH_3_MASK);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CNTEN, 0);

        EPWM_Start(g_apEPWMModule[j], EPWM_CH_0_MASK | EPWM_CH_1_MASK | EPWM_CH_2_MASK | EPWM_CH_3_MASK | EPWM_CH_4_MASK);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CNTEN, 0x000001F);

        EPWM_ForceStop(g_apEPWMModule[j], EPWM_CH_0_MASK | EPWM_CH_1_MASK | EPWM_CH_2_MASK | EPWM_CH_3_MASK | EPWM_CH_4_MASK);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CNTEN, 0);

        EPWM_Start(g_apEPWMModule[j], EPWM_CH_0_MASK | EPWM_CH_1_MASK | EPWM_CH_2_MASK | EPWM_CH_3_MASK | EPWM_CH_4_MASK | EPWM_CH_5_MASK);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CNTEN, 0x000003F);

        EPWM_ForceStop(g_apEPWMModule[j], EPWM_CH_0_MASK | EPWM_CH_1_MASK | EPWM_CH_2_MASK | EPWM_CH_3_MASK | EPWM_CH_4_MASK | EPWM_CH_5_MASK);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CNTEN, 0);
    }
}

void Func_EPWM_Stop()
{
    uint32_t u32TestCh = 0, x = 0;

    Func_EPWM_Start();

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_CNR(g_apEPWMModule[x], u32TestCh, 0x100);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PERIOD[u32TestCh], 0x100);

            EPWM_Stop(g_apEPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PERIOD[u32TestCh], 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action=> ignore
        //     for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        //     {
        //         EPWM_SET_CNR(g_apEPWMModule[x], u32TestCh, 0x100);
        //     }

        //     for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        //     {
        //         EPWM_Stop(g_apEPWMModule[x], 1 << u32TestCh);
        //     }
        //     CU_ASSERT_EQUAL(g_apEPWMModule[x]->CNR0, 0);
        //     CU_ASSERT_EQUAL(g_apEPWMModule[x]->CNR1, 0);
        //     CU_ASSERT_EQUAL(g_apEPWMModule[x]->CNR2, 0);
        //     CU_ASSERT_EQUAL(g_apEPWMModule[x]->CNR3, 0);

        //test case 3: enable/disable multiple channels each time
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_CNR(g_apEPWMModule[x], u32TestCh, 0x100);
        }

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PERIOD[u32TestCh], 0x100);
        }

        EPWM_Stop(g_apEPWMModule[x], EPWM_CH_0_MASK | EPWM_CH_1_MASK | EPWM_CH_2_MASK | EPWM_CH_3_MASK | EPWM_CH_4_MASK | EPWM_CH_5_MASK);

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PERIOD[u32TestCh], 0);
        }
    }
}

void Func_EPWM_EnableADCTrigger()
{
    uint32_t u32TestCh = 0, x = 0;

    Func_EPWM_Start();

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            if (u32TestCh < 4)
            {
                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_EVEN_ZERO);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x80 << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0 << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_EVEN_PERIOD);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x81 << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x01 << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_EVEN_ZERO_PERIOD);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x82 << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x02 << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_EVEN_COMPARE_UP);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x83 << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x03 << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_EVEN_COMPARE_DOWN);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x84 << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x04 << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_ODD_ZERO);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x85 << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x05 << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_ODD_PERIOD);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x86 << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x06 << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_ODD_ZERO_PERIOD);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x87 << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x07 << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_ODD_COMPARE_UP);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x88 << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x08 << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_ODD_COMPARE_DOWN);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x89 << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x09 << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_CH_0_FREE_CMP_UP);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x8A << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x0A << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_CH_0_FREE_CMP_DOWN);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x8B << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x0B << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_CH_2_FREE_CMP_UP);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x8C << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x0C << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_CH_2_FREE_CMP_DOWN);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x8D << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x0D << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_CH_4_FREE_CMP_UP);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x8E << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x0E << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_CH_4_FREE_CMP_DOWN);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x8F << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x0F << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_EVEN_IFA);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x90 << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x10 << (u32TestCh * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_ODD_IFA);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x91 << (u32TestCh * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0 & ((EPWM_EADCTS0_TRGEN0_Msk | EPWM_EADCTS0_TRGSEL0_Msk) << (u32TestCh * 8)), 0x11 << (u32TestCh * 8));
            }
            else
            {
                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_EVEN_ZERO);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x80 << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0 << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_EVEN_PERIOD);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x81 << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x01 << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_EVEN_ZERO_PERIOD);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x82 << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x02 << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_EVEN_COMPARE_UP);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x83 << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x03 << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_EVEN_COMPARE_DOWN);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x84 << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x04 << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_ODD_ZERO);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x85 << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x05 << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_ODD_PERIOD);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x86 << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x06 << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_ODD_ZERO_PERIOD);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x87 << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x07 << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_ODD_COMPARE_UP);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x88 << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x08 << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_ODD_COMPARE_DOWN);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x89 << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x09 << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_CH_0_FREE_CMP_UP);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x8A << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x0A << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_CH_0_FREE_CMP_DOWN);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x8B << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x0B << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_CH_2_FREE_CMP_UP);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x8C << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x0C << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_CH_2_FREE_CMP_DOWN);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x8D << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x0D << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_CH_4_FREE_CMP_UP);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x8E << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x0E << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_CH_4_FREE_CMP_DOWN);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x8F << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x0F << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_EVEN_IFA);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x90 << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x10 << ((u32TestCh - 4) * 8));

                EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_ODD_IFA);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x91 << ((u32TestCh - 4) * 8));

                EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1 & ((EPWM_EADCTS1_TRGEN4_Msk | EPWM_EADCTS1_TRGSEL4_Msk) << ((u32TestCh - 4) * 8)), 0x11 << ((u32TestCh - 4) * 8));
            }
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableADCTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRG_ADC_CH_0_FREE_CMP_UP);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0, 0x8A8A8A8A);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1, 0x8A8A);

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DisableADCTrigger(g_apEPWMModule[x], u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS0, 0x0A0A0A0A);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->EADCTS1, 0x0A0A);
    }
}
void Func_EPWM_EnableCapture()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableCapture(g_apEPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPINEN & (EPWM_CAPINEN_CAPINEN0_Msk << u32TestCh), 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPCTL & (EPWM_CAPCTL_CAPEN0_Msk << u32TestCh), 1 << u32TestCh);
            EPWM_DisableCapture(g_apEPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPINEN & (EPWM_CAPINEN_CAPINEN0_Msk << u32TestCh), 0 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPCTL & (EPWM_CAPCTL_CAPEN0_Msk << u32TestCh), 0 << u32TestCh);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableCapture(g_apEPWMModule[x], 1 << u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPINEN, 0x3F);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPCTL, 0x3F);

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DisableCapture(g_apEPWMModule[x], 1 << u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPINEN, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPCTL, 0);

        //test case 3: enable/disable multiple channels each time
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh += 3)
        {
            EPWM_EnableCapture(g_apEPWMModule[x], 7 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPINEN & (7 << u32TestCh), 7 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPCTL & (7 << u32TestCh), 7 << u32TestCh);

            EPWM_DisableCapture(g_apEPWMModule[x], 7 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPINEN, 0);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPCTL, 0);
        }
    }
}

void Func_EPWM_EnableOutput()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableOutput(g_apEPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->POEN & ((1 << EPWM_POEN_POEN0_Pos) << u32TestCh), 1 << u32TestCh);
            EPWM_DisableOutput(g_apEPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->POEN & ((1 << EPWM_POEN_POEN0_Pos) << u32TestCh), 0 << u32TestCh);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableOutput(g_apEPWMModule[x], 1 << u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->POEN, 0x3F);

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DisableOutput(g_apEPWMModule[x], 1 << u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->POEN, 0);

        //test case 3: enable/disable multiple channels each time
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh += 3)
        {
            EPWM_EnableOutput(g_apEPWMModule[x], 7 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->POEN, 7 << u32TestCh);

            EPWM_DisableOutput(g_apEPWMModule[x], 7 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->POEN, 0);
        }
    }
}

void Func_EPWM_EnablePDMA()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            uint32_t u32IsOddCh;
            u32IsOddCh = u32TestCh % 2;
            EPWM_EnablePDMA(g_apEPWMModule[x], u32TestCh, TRUE, EPWM_CAPTURE_PDMA_RISING_FALLING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PDMACTL & ((EPWM_PDMACTL_CHSEL0_1_Msk | EPWM_PDMACTL_CAPORD0_1_Msk | EPWM_PDMACTL_CAPMOD0_1_Msk | EPWM_PDMACTL_CHEN0_1_Msk) << (u32TestCh / 2 * 8)),
                            ((u32IsOddCh << 4) | (1 << 3) | (3 << 1) | (1 << 0)) << (u32TestCh / 2 * 8));
            EPWM_DisablePDMA(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PDMACTL & ((EPWM_PDMACTL_CHSEL0_1_Msk | EPWM_PDMACTL_CAPORD0_1_Msk | EPWM_PDMACTL_CAPMOD0_1_Msk | EPWM_PDMACTL_CHEN0_1_Msk) << (u32TestCh / 2 * 8)),
                            ((u32IsOddCh << 4) | (1 << 3) | (3 << 1) | (0 << 0)) << (u32TestCh / 2 * 8));

            EPWM_EnablePDMA(g_apEPWMModule[x], u32TestCh, FALSE, EPWM_CAPTURE_PDMA_RISING_FALLING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PDMACTL & ((EPWM_PDMACTL_CHSEL0_1_Msk | EPWM_PDMACTL_CAPORD0_1_Msk | EPWM_PDMACTL_CAPMOD0_1_Msk | EPWM_PDMACTL_CHEN0_1_Msk) << (u32TestCh / 2 * 8)),
                            ((u32IsOddCh << 4) | (0 << 3) | (3 << 1) | (1 << 0)) << (u32TestCh / 2 * 8));
            EPWM_DisablePDMA(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PDMACTL & ((EPWM_PDMACTL_CHSEL0_1_Msk | EPWM_PDMACTL_CAPORD0_1_Msk | EPWM_PDMACTL_CAPMOD0_1_Msk | EPWM_PDMACTL_CHEN0_1_Msk) << (u32TestCh / 2 * 8)),
                            ((u32IsOddCh << 4) | (0 << 3) | (3 << 1) | (0 << 0)) << (u32TestCh / 2 * 8));

            EPWM_EnablePDMA(g_apEPWMModule[x], u32TestCh, TRUE, EPWM_CAPTURE_PDMA_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PDMACTL & ((EPWM_PDMACTL_CHSEL0_1_Msk | EPWM_PDMACTL_CAPORD0_1_Msk | EPWM_PDMACTL_CAPMOD0_1_Msk | EPWM_PDMACTL_CHEN0_1_Msk) << (u32TestCh / 2 * 8)),
                            ((u32IsOddCh << 4) | (1 << 3) | (1 << 1) | (1 << 0)) << (u32TestCh / 2 * 8));
            EPWM_DisablePDMA(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PDMACTL & ((EPWM_PDMACTL_CHSEL0_1_Msk | EPWM_PDMACTL_CAPORD0_1_Msk | EPWM_PDMACTL_CAPMOD0_1_Msk | EPWM_PDMACTL_CHEN0_1_Msk) << (u32TestCh / 2 * 8)),
                            ((u32IsOddCh << 4) | (1 << 3) | (1 << 1) | (0 << 0)) << (u32TestCh / 2 * 8));

            EPWM_EnablePDMA(g_apEPWMModule[x], u32TestCh, FALSE, EPWM_CAPTURE_PDMA_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PDMACTL & ((EPWM_PDMACTL_CHSEL0_1_Msk | EPWM_PDMACTL_CAPORD0_1_Msk | EPWM_PDMACTL_CAPMOD0_1_Msk | EPWM_PDMACTL_CHEN0_1_Msk) << (u32TestCh / 2 * 8)),
                            ((u32IsOddCh << 4) | (0 << 3) | (1 << 1) | (1 << 0)) << (u32TestCh / 2 * 8));
            EPWM_DisablePDMA(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PDMACTL & ((EPWM_PDMACTL_CHSEL0_1_Msk | EPWM_PDMACTL_CAPORD0_1_Msk | EPWM_PDMACTL_CAPMOD0_1_Msk | EPWM_PDMACTL_CHEN0_1_Msk) << (u32TestCh / 2 * 8)),
                            ((u32IsOddCh << 4) | (0 << 3) | (1 << 1) | (0 << 0)) << (u32TestCh / 2 * 8));

            EPWM_EnablePDMA(g_apEPWMModule[x], u32TestCh, TRUE, EPWM_CAPTURE_PDMA_FALLING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PDMACTL & ((EPWM_PDMACTL_CHSEL0_1_Msk | EPWM_PDMACTL_CAPORD0_1_Msk | EPWM_PDMACTL_CAPMOD0_1_Msk | EPWM_PDMACTL_CHEN0_1_Msk) << (u32TestCh / 2 * 8)),
                            ((u32IsOddCh << 4) | (1 << 3) | (2 << 1) | (1 << 0)) << (u32TestCh / 2 * 8));
            EPWM_DisablePDMA(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PDMACTL & ((EPWM_PDMACTL_CHSEL0_1_Msk | EPWM_PDMACTL_CAPORD0_1_Msk | EPWM_PDMACTL_CAPMOD0_1_Msk | EPWM_PDMACTL_CHEN0_1_Msk) << (u32TestCh / 2 * 8)),
                            ((u32IsOddCh << 4) | (1 << 3) | (2 << 1) | (0 << 0)) << (u32TestCh / 2 * 8));

            EPWM_EnablePDMA(g_apEPWMModule[x], u32TestCh, FALSE, EPWM_CAPTURE_PDMA_FALLING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PDMACTL & ((EPWM_PDMACTL_CHSEL0_1_Msk | EPWM_PDMACTL_CAPORD0_1_Msk | EPWM_PDMACTL_CAPMOD0_1_Msk | EPWM_PDMACTL_CHEN0_1_Msk) << (u32TestCh / 2 * 8)),
                            ((u32IsOddCh << 4) | (0 << 3) | (2 << 1) | (1 << 0)) << (u32TestCh / 2 * 8));
            EPWM_DisablePDMA(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->PDMACTL & ((EPWM_PDMACTL_CHSEL0_1_Msk | EPWM_PDMACTL_CAPORD0_1_Msk | EPWM_PDMACTL_CAPMOD0_1_Msk | EPWM_PDMACTL_CHEN0_1_Msk) << (u32TestCh / 2 * 8)),
                            ((u32IsOddCh << 4) | (0 << 3) | (2 << 1) | (0 << 0)) << (u32TestCh / 2 * 8));
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnablePDMA(g_apEPWMModule[x], u32TestCh, TRUE, EPWM_CAPTURE_PDMA_RISING_FALLING_LATCH);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->PDMACTL, 0x1F1F1F);

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DisablePDMA(g_apEPWMModule[x], u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->PDMACTL, 0x1E1E1E);

    }
}

void Func_EPWM_EnableDeadZone()
{
    uint32_t x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    /* unlock protected registers */
    SYS_UnlockReg();

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        EPWM_EnableFallingDeadZone(g_apEPWMModule[x], 0, 0xFFF);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, BIT8);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[0], 0xFFF);
        EPWM_DisableFallingDeadZone(g_apEPWMModule[x], 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[0], 0xFFF);
        EPWM_EnableRisingDeadZone(g_apEPWMModule[x], 0, 0xFFF);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, BIT0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[0], 0xFFF);
        EPWM_DisableRisingDeadZone(g_apEPWMModule[x], 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[0], 0xFFF);

        EPWM_EnableFallingDeadZone(g_apEPWMModule[x], 1, 0xFFF);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, BIT8);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[0], 0xFFF);
        EPWM_DisableFallingDeadZone(g_apEPWMModule[x], 1);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[0], 0xFFF);
        EPWM_EnableRisingDeadZone(g_apEPWMModule[x], 1, 0xFFF);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, BIT0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[0], 0xFFF);
        EPWM_DisableRisingDeadZone(g_apEPWMModule[x], 1);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[0], 0xFFF);

        EPWM_EnableFallingDeadZone(g_apEPWMModule[x], 2, 0xFFF);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, BIT9);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[1], 0xFFF);
        EPWM_DisableFallingDeadZone(g_apEPWMModule[x], 2);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[1], 0xFFF);
        EPWM_EnableRisingDeadZone(g_apEPWMModule[x], 2, 0xFFF);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, BIT1);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[1], 0xFFF);
        EPWM_DisableRisingDeadZone(g_apEPWMModule[x], 2);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[1], 0xFFF);

        EPWM_EnableFallingDeadZone(g_apEPWMModule[x], 3, 0xFFF);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, BIT9);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[1], 0xFFF);
        EPWM_DisableFallingDeadZone(g_apEPWMModule[x], 3);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[1], 0xFFF);
        EPWM_EnableRisingDeadZone(g_apEPWMModule[x], 3, 0xFFF);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, BIT1);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[1], 0xFFF);
        EPWM_DisableRisingDeadZone(g_apEPWMModule[x], 3);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[1], 0xFFF);

        EPWM_EnableFallingDeadZone(g_apEPWMModule[x], 4, 0xFFF);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, BIT10);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[2], 0xFFF);
        EPWM_DisableFallingDeadZone(g_apEPWMModule[x], 4);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[2], 0xFFF);
        EPWM_EnableRisingDeadZone(g_apEPWMModule[x], 4, 0xFFF);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, BIT2);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[2], 0xFFF);
        EPWM_DisableRisingDeadZone(g_apEPWMModule[x], 4);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[2], 0xFFF);

        EPWM_EnableFallingDeadZone(g_apEPWMModule[x], 5, 0xFFF);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, BIT10);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[2], 0xFFF);
        EPWM_DisableFallingDeadZone(g_apEPWMModule[x], 5);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[2], 0xFFF);
        EPWM_EnableRisingDeadZone(g_apEPWMModule[x], 5, 0xFFF);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, BIT2);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[2], 0xFFF);
        EPWM_DisableRisingDeadZone(g_apEPWMModule[x], 5);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[2], 0xFFF);

        //test case 2: enable/disable one channel each time, then check at final action
        EPWM_EnableFallingDeadZone(g_apEPWMModule[x], 0, 0x5A);
        EPWM_EnableFallingDeadZone(g_apEPWMModule[x], 1, 0x5A);
        EPWM_EnableFallingDeadZone(g_apEPWMModule[x], 2, 0x5A);
        EPWM_EnableFallingDeadZone(g_apEPWMModule[x], 3, 0x5A);
        EPWM_EnableFallingDeadZone(g_apEPWMModule[x], 4, 0x5A);
        EPWM_EnableFallingDeadZone(g_apEPWMModule[x], 5, 0x5A);
        EPWM_EnableRisingDeadZone(g_apEPWMModule[x], 0, 0xA5);
        EPWM_EnableRisingDeadZone(g_apEPWMModule[x], 1, 0xA5);
        EPWM_EnableRisingDeadZone(g_apEPWMModule[x], 2, 0xA5);
        EPWM_EnableRisingDeadZone(g_apEPWMModule[x], 3, 0xA5);
        EPWM_EnableRisingDeadZone(g_apEPWMModule[x], 4, 0xA5);
        EPWM_EnableRisingDeadZone(g_apEPWMModule[x], 5, 0xA5);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, BIT10 | BIT9 | BIT8 | BIT2 | BIT1 | BIT0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[0], 0x5A);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[1], 0x5A);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[2], 0x5A);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[0], 0xA5);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[1], 0xA5);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[2], 0xA5);

        EPWM_DisableRisingDeadZone(g_apEPWMModule[x], 0);
        EPWM_DisableRisingDeadZone(g_apEPWMModule[x], 1);
        EPWM_DisableRisingDeadZone(g_apEPWMModule[x], 2);
        EPWM_DisableRisingDeadZone(g_apEPWMModule[x], 3);
        EPWM_DisableRisingDeadZone(g_apEPWMModule[x], 4);
        EPWM_DisableRisingDeadZone(g_apEPWMModule[x], 5);
        EPWM_DisableFallingDeadZone(g_apEPWMModule[x], 0);
        EPWM_DisableFallingDeadZone(g_apEPWMModule[x], 1);
        EPWM_DisableFallingDeadZone(g_apEPWMModule[x], 2);
        EPWM_DisableFallingDeadZone(g_apEPWMModule[x], 3);
        EPWM_DisableFallingDeadZone(g_apEPWMModule[x], 4);
        EPWM_DisableFallingDeadZone(g_apEPWMModule[x], 5);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[0], 0x5A);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[1], 0x5A);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FDTCNT[2], 0x5A);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[0], 0xA5);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[1], 0xA5);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->RDTCNT[2], 0xA5);
    }
}

void Func_EPWM_EnableCaptureInt()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableCaptureInt(g_apEPWMModule[x], u32TestCh, EPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPIEN, 1 << u32TestCh);
            EPWM_DisableCaptureInt(g_apEPWMModule[x], u32TestCh, EPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPIEN, 0);

            EPWM_EnableCaptureInt(g_apEPWMModule[x], u32TestCh, EPWM_CAPTURE_INT_FALLING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPIEN, 1 << (u32TestCh + 8));
            EPWM_DisableCaptureInt(g_apEPWMModule[x], u32TestCh, EPWM_CAPTURE_INT_FALLING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPIEN, 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableCaptureInt(g_apEPWMModule[x], u32TestCh, EPWM_CAPTURE_INT_RISING_LATCH | EPWM_CAPTURE_INT_FALLING_LATCH);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPIEN, 0x3F3F);

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DisableCaptureInt(g_apEPWMModule[x], u32TestCh, EPWM_CAPTURE_INT_RISING_LATCH | EPWM_CAPTURE_INT_FALLING_LATCH);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPIEN, 0);
    }
}

void Func_EPWM_EnableDutyInt()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t au32RegCheck[6] = {0x01010000, 0x03030000, 0x07070000, 0x0F0F0000, 0x1F1F0000, 0x3F3F0000};
    uint32_t au32RegReverseCheck[6] = {0x3E3E0000, 0x3C3C0000, 0x38380000, 0x30300000, 0x20200000, 0x00000000};

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableDutyInt(g_apEPWMModule[x], u32TestCh, EPWM_DUTY_INT_DOWN_COUNT_MATCH_CMP);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN0 & ((1 << EPWM_INTEN0_CMPDIEN0_Pos) << u32TestCh), 1 << (24 + u32TestCh));
            EPWM_DisableDutyInt(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN0 & ((1 << EPWM_INTEN0_CMPDIEN0_Pos) << u32TestCh), 0);

            EPWM_EnableDutyInt(g_apEPWMModule[x], u32TestCh, EPWM_DUTY_INT_UP_COUNT_MATCH_CMP);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN0 & ((1 << EPWM_INTEN0_CMPUIEN0_Pos) << u32TestCh), 1 << (16 + u32TestCh));
            EPWM_DisableDutyInt(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN0 & ((1 << EPWM_INTEN0_CMPUIEN0_Pos) << u32TestCh), 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableDutyInt(g_apEPWMModule[x], u32TestCh, EPWM_DUTY_INT_DOWN_COUNT_MATCH_CMP | EPWM_DUTY_INT_UP_COUNT_MATCH_CMP);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN0, au32RegCheck[u32TestCh]);
        }

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DisableDutyInt(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN0, au32RegReverseCheck[u32TestCh]);
        }
    }
}

void Func_EPWM_EnablePeriodInt()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t au32RegCheck[6] = {0x00000100, 0x00000300, 0x00000700, 0x00000F00, 0x00001F00, 0x00003F00};
    uint32_t au32RegReverseCheck[6] = {0x00003E00, 0x00003C00, 0x00003800, 0x00003000, 0x00002000, 0x00000000};

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnablePeriodInt(g_apEPWMModule[x], u32TestCh, NULL);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN0 & ((1 << EPWM_INTEN0_PIEN0_Pos) << u32TestCh), 1 << (8 + u32TestCh));
            EPWM_DisablePeriodInt(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN0, 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnablePeriodInt(g_apEPWMModule[x], u32TestCh, NULL);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN0, au32RegCheck[u32TestCh]);
        }

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DisablePeriodInt(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN0, au32RegReverseCheck[u32TestCh]);
        }
    }
}

void Func_EPWM_ENABLE_OUTPUT_INVERTER()
{
    uint32_t u32TestCh = 0, j = 0;
    uint32_t au32RegWrite[7] = {0x0000003F, 0x0000001F, 0x0000000F, 0x00000007, 0x00000003, 0x00000001, 0};

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (j = 0; j < EPWM_MODULE_NUM; j++)
    {
        //test case 1: enable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_ENABLE_OUTPUT_INVERTER(g_apEPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->POLCTL & (1 << u32TestCh), 1 << u32TestCh);
        }

        //test case 2: enable/disable multiple channels each time
        for (u32TestCh = 0; u32TestCh <= EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_ENABLE_OUTPUT_INVERTER(g_apEPWMModule[j], au32RegWrite[u32TestCh]);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->POLCTL, au32RegWrite[u32TestCh]);
        }
    }
}

void Func_EPWM_SET_PRESCALER()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t u32PrescalerPatternIdx = 0, u32PrescalerPatternCount = 0, u32PrescalerPatternData = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    u32PrescalerPatternCount = sizeof(TwelveBitsPatternTable) / sizeof(uint32_t);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            for (u32PrescalerPatternIdx = 0; u32PrescalerPatternIdx < u32PrescalerPatternCount; u32PrescalerPatternIdx++)
            {
                u32PrescalerPatternData = TwelveBitsPatternTable[u32PrescalerPatternIdx];

                EPWM_SET_PRESCALER(g_apEPWMModule[x], u32TestCh, u32PrescalerPatternData);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->CLKPSC[u32TestCh], u32PrescalerPatternData);
            }
        }
    }
}

void Func_EPWM_SET_CMR()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t u32PrescalerPatternIdx = 0, u32PrescalerPatternCount = 0, u32PrescalerPatternData = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    u32PrescalerPatternCount = sizeof(SixteenBitsPatternTable) / sizeof(uint32_t);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            for (u32PrescalerPatternIdx = 0; u32PrescalerPatternIdx < u32PrescalerPatternCount; u32PrescalerPatternIdx++)
            {
                u32PrescalerPatternData = SixteenBitsPatternTable[u32PrescalerPatternIdx];

                EPWM_SET_CMR(g_apEPWMModule[x], u32TestCh, u32PrescalerPatternData);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->CMPDAT[u32TestCh] & EPWM_CMPDAT0_CMP_Msk, u32PrescalerPatternData);

                EPWM_SET_CNR(g_apEPWMModule[x], u32TestCh, u32PrescalerPatternData);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->PERIOD[u32TestCh] & EPWM_PERIOD0_PERIOD_Msk, u32PrescalerPatternData);
            }
        }
    }
}

void Func_EPWM_SET_ALIGNED_TYPE()
{
    uint32_t u32TestCh = 0, j = 0;
    uint32_t au32ChMsk[6] = {0x00000001, 0x00000003, 0x00000007, 0x0000000F, 0x0000001F, 0x0000003F};
    uint32_t au32RegCheck1[6] = {0x00000001, 0x00000005, 0x00000015, 0x00000055, 0x00000155, 0x00000555};
    uint32_t au32RegCheck2[6] = {0x00000556, 0x0000055A, 0x0000056A, 0x000005AA, 0x000006AA, 0x00000AAA};

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (j = 0; j < EPWM_MODULE_NUM; j++)
    {
        //test case 1: enable one channel each time, then check each action
        EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], EPWM_CH_0_MASK, EPWM_CENTER_ALIGNED);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1 & 2, 2);
        EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], EPWM_CH_0_MASK, EPWM_EDGE_ALIGNED);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1 & 1, 1);

        EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], EPWM_CH_1_MASK, EPWM_CENTER_ALIGNED);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1 & (2 << 2), 2 << 2);
        EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], EPWM_CH_1_MASK, EPWM_EDGE_ALIGNED);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1 & (1 << 2), 1 << 2);

        EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], EPWM_CH_2_MASK, EPWM_CENTER_ALIGNED);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1 & (2 << 4), 2 << 4);
        EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], EPWM_CH_2_MASK, EPWM_EDGE_ALIGNED);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1 & (1 << 4), 1 << 4);

        EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], EPWM_CH_3_MASK, EPWM_CENTER_ALIGNED);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1 & (2 << 6), 2 << 6);
        EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], EPWM_CH_3_MASK, EPWM_EDGE_ALIGNED);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1 & (1 << 6), 1 << 6);

        EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], EPWM_CH_4_MASK, EPWM_CENTER_ALIGNED);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1 & (2 << 8), 2 << 8);
        EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], EPWM_CH_4_MASK, EPWM_EDGE_ALIGNED);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1 & (1 << 8), 1 << 8);

        EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], EPWM_CH_5_MASK, EPWM_CENTER_ALIGNED);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1 & (2 << 10), 2 << 10);
        EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], EPWM_CH_5_MASK, EPWM_EDGE_ALIGNED);
        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1 & (1 << 10), 1 << 10);

        //test case 2: enable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], 1 << u32TestCh, EPWM_EDGE_ALIGNED);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1, 0x555);

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], 1 << u32TestCh, EPWM_CENTER_ALIGNED);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1, 0xAAA);

        /* clear CTL1 register value */
        g_apEPWMModule[j]->CTL1 = 0;

        //test case 3: enable/disable multiple channels each time
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], au32ChMsk[u32TestCh], EPWM_EDGE_ALIGNED);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1, au32RegCheck1[u32TestCh]);
        }

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], au32ChMsk[u32TestCh], EPWM_CENTER_ALIGNED);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1, au32RegCheck2[u32TestCh]);
        }
    }
}

void Func_EPWM_ENABLE_COMPLEMENTARY_MODE()
{
    uint32_t x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        EPWM_ENABLE_COMPLEMENTARY_MODE(g_apEPWMModule[x]);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->CTL1 & (0x7ul << EPWM_CTL1_OUTMODE0_Pos), 0x07000000);
        EPWM_DISABLE_COMPLEMENTARY_MODE(g_apEPWMModule[x]);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->CTL1 & (0x7ul << EPWM_CTL1_OUTMODE0_Pos), 0);
    }
}

void Func_EPWM_ENABLE_GROUP_MODE()
{
    uint32_t x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        EPWM_ENABLE_GROUP_MODE(g_apEPWMModule[x]);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->CTL0 & EPWM_CTL0_GROUPEN_Msk, 1 << 24);
        EPWM_DISABLE_GROUP_MODE(g_apEPWMModule[x]);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->CTL0 & EPWM_CTL0_GROUPEN_Msk, 0);
    }
}

void Func_EPWM_ENABLE_TIMER_SYNC()
{
    uint32_t u32TestCh = 0, j = 0;
    uint32_t au32RegCheck[6] = {0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F};
    uint32_t au32RegReverseCheck[6] = {0x3E, 0x3C, 0x38, 0x30, 0x20, 0x0};

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (j = 0; j < EPWM_MODULE_NUM; j++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_ENABLE_TIMER_SYNC(g_apEPWMModule[j], 1 << u32TestCh, EPWM_SSCTL_SSRC_EPWM0);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->SSCTL, 1 << u32TestCh);
            EPWM_DISABLE_TIMER_SYNC(g_apEPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->SSCTL, 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_ENABLE_TIMER_SYNC(g_apEPWMModule[j], 1 << u32TestCh, EPWM_SSCTL_SSRC_EPWM0);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->SSCTL, au32RegCheck[u32TestCh]);
        }

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DISABLE_TIMER_SYNC(g_apEPWMModule[j], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->SSCTL, au32RegReverseCheck[u32TestCh]);
        }
    }
}

void Func_EPWM_MASK_OUTPUT()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_MASK_OUTPUT(g_apEPWMModule[x], 1 << u32TestCh, 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->MSKEN, 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->MSK, 1 << u32TestCh);
        }
    }
}

void Func_EPWM_SET_FTCMR()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t u32PatternIdx = 0, u32PatternCount = 0, u32PatternData = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    u32PatternCount = sizeof(SixteenBitsPatternTable) / sizeof(uint32_t);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            for (u32PatternIdx = 0; u32PatternIdx < u32PatternCount; u32PatternIdx++)
            {
                u32PatternData = SixteenBitsPatternTable[u32PatternIdx];

                EPWM_SET_FTCMR(g_apEPWMModule[x], u32TestCh, u32PatternData);
                CU_ASSERT_EQUAL(*(__IO uint32_t *)(&(g_apEPWMModule[x]->FTCMPDAT[0]) + ((u32TestCh) >> 1)) & EPWM_CMPDAT0_CMP_Msk, u32PatternData);
            }
        }
    }
}

void Func_EPWM_SET_LOAD_WINDOW()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_CNR(g_apEPWMModule[x], u32TestCh, 1000);
            EPWM_SET_CMR(g_apEPWMModule[x], u32TestCh, 600);
            EPWM_SET_PRESCALER(g_apEPWMModule[x], u32TestCh, 500);
            EPWM_EnableLoadMode(g_apEPWMModule[x], u32TestCh, EPWM_LOAD_MODE_WINDOW);
        }

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            g_apEPWMModule[x]->INTSTS0 = 0x3F3F3F3F;
            EPWM_Start(g_apEPWMModule[x], 1 << u32TestCh);
            EPWM_SET_LOAD_WINDOW(g_apEPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->LOAD, 1 << u32TestCh);   // added

            while ((g_apEPWMModule[x]->INTSTS0 & ((1 << EPWM_INTSTS0_CMPUIF0_Pos) << u32TestCh)) == 0) {};

            ////    while(EPWM_GetDutyIntFlag(g_apEPWMModule[x], u32TestCh) == 0);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->LOAD, 1 << u32TestCh);

            //The load bit by hardware cleared when current EPWM period end
            while ((g_apEPWMModule[x]->INTSTS0 & ((1 << EPWM_INTSTS0_PIF0_Pos) << u32TestCh)) != ((1 << EPWM_INTSTS0_PIF0_Pos) << u32TestCh));

            EPWM_Stop(g_apEPWMModule[x], 1 << u32TestCh);
            EPWM_ForceStop(g_apEPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->LOAD, 0);
        }
    }
}

void Func_EPWM_SET_DEADZONE_CLK_SRC()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    /* unlock protected registers */
    SYS_UnlockReg();

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_DEADZONE_CLK_SRC(g_apEPWMModule[x], u32TestCh, TRUE);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, (EPWM_DTCTL_DTCKSEL0_Msk << (u32TestCh >> 1)));
            EPWM_SET_DEADZONE_CLK_SRC(g_apEPWMModule[x], u32TestCh, FALSE);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DTCTL, 0);
        }
    }
}

void Func_EPWM_CLR_COUNTER()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_CNR(g_apEPWMModule[x], u32TestCh, 0xFFFF);
            EPWM_SET_CMR(g_apEPWMModule[x], u32TestCh, 500);
            EPWM_SET_PRESCALER(EPWM0, u32TestCh, 0x400);
            EPWM_Start(g_apEPWMModule[x], 1 << u32TestCh);

            while ((g_apEPWMModule[x]->INTSTS0 & ((1 << EPWM_INTSTS0_CMPUIF0_Pos) << u32TestCh)) != (1 << EPWM_INTSTS0_CMPUIF0_Pos) << u32TestCh);

            EPWM_ForceStop(g_apEPWMModule[x], 1 << u32TestCh);
            EPWM_CLR_COUNTER(g_apEPWMModule[x], 1 << u32TestCh);

            while ((g_apEPWMModule[x])->CNTCLR & (1 << u32TestCh)) {};

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CNTCLR, 0);
        }
    }
}

void Func_EPWM_SET_OUTPUT_LEVEL()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t au32PatternData[4] = {EPWM_OUTPUT_NOTHING, EPWM_OUTPUT_LOW, EPWM_OUTPUT_HIGH, EPWM_OUTPUT_TOGGLE};
    uint32_t u32ZeroIdx = 0, u32CmpuIdx = 0, u32PeriodIdx = 0, u32CmpdIdx = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            for (u32ZeroIdx = 0; u32ZeroIdx < sizeof(au32PatternData) / sizeof(uint32_t); u32ZeroIdx++)
            {
                for (u32CmpuIdx = 0; u32CmpuIdx < sizeof(au32PatternData) / sizeof(uint32_t); u32CmpuIdx++)
                {
                    for (u32PeriodIdx = 0; u32PeriodIdx < sizeof(au32PatternData) / sizeof(uint32_t); u32PeriodIdx++)
                    {
                        for (u32CmpdIdx = 0; u32CmpdIdx < sizeof(au32PatternData) / sizeof(uint32_t); u32CmpdIdx++)
                        {
                            EPWM_SET_OUTPUT_LEVEL(g_apEPWMModule[x], 1 << u32TestCh, au32PatternData[u32ZeroIdx], au32PatternData[u32CmpuIdx], au32PatternData[u32PeriodIdx], au32PatternData[u32CmpdIdx]);
                            CU_ASSERT_EQUAL(g_apEPWMModule[x]->WGCTL0 & (3 << (EPWM_WGCTL0_ZPCTL0_Pos + (u32TestCh << 1))), au32PatternData[u32ZeroIdx] << (u32TestCh << 1));
                            CU_ASSERT_EQUAL(g_apEPWMModule[x]->WGCTL0 & (3 << (EPWM_WGCTL0_PRDPCTL0_Pos + (u32TestCh << 1))), au32PatternData[u32PeriodIdx] << (16 + (u32TestCh << 1)));
                            CU_ASSERT_EQUAL(g_apEPWMModule[x]->WGCTL1 & (3 << (EPWM_WGCTL1_CMPUCTL0_Pos + (u32TestCh << 1))), au32PatternData[u32CmpuIdx] << (u32TestCh << 1));
                            CU_ASSERT_EQUAL(g_apEPWMModule[x]->WGCTL1 & (3 << (EPWM_WGCTL1_CMPDCTL0_Pos + (u32TestCh << 1))), au32PatternData[u32CmpdIdx] << (16 + (u32TestCh << 1)));
                        }
                    }
                }
            }

        }
    }
}

void Func_EPWM_GetADCTriggerFlag()
{
    uint32_t u32TestCh = 0, j = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (j = 0; j < EPWM_MODULE_NUM; j++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_CNR(g_apEPWMModule[j], u32TestCh, 10);
            EPWM_SET_CMR(g_apEPWMModule[j], u32TestCh, 5);
            EPWM_SET_PRESCALER(g_apEPWMModule[j], u32TestCh, 2);
            EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], 1 << u32TestCh, EPWM_CENTER_ALIGNED);

            if (u32TestCh % 2)
            {
                EPWM_EnableADCTrigger(g_apEPWMModule[j], u32TestCh, EPWM_TRG_ADC_ODD_PERIOD);
            }
            else
            {
                EPWM_EnableADCTrigger(g_apEPWMModule[j], u32TestCh, EPWM_TRG_ADC_EVEN_PERIOD);
            }

            EPWM_EnableOutput(g_apEPWMModule[j], 1 << u32TestCh);
            EPWM_Start(g_apEPWMModule[j], 1 << u32TestCh);

            while (EPWM_GetADCTriggerFlag(g_apEPWMModule[j], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(EPWM_GetADCTriggerFlag(g_apEPWMModule[j], u32TestCh), 1);

            EPWM_DisableADCTrigger(g_apEPWMModule[j], u32TestCh);

            EPWM_ForceStop(g_apEPWMModule[j], 1 << u32TestCh);

            EPWM_ClearADCTriggerFlag(g_apEPWMModule[j], u32TestCh, NULL);

            CU_ASSERT_EQUAL(g_apEPWMModule[j]->STATUS & ((1 << EPWM_STATUS_EADCTRGF0_Pos) << u32TestCh), 0);
        }
    }
}

void Func_EPWM_GetDutyIntFlag()
{
    uint32_t u32TestCh = 0, j = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (j = 0; j < EPWM_MODULE_NUM; j++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_CNR(g_apEPWMModule[j], u32TestCh, 10);
            EPWM_SET_CMR(g_apEPWMModule[j], u32TestCh, 5);
            EPWM_SET_PRESCALER(g_apEPWMModule[j], u32TestCh, 2);
            EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], 1 << u32TestCh, EPWM_EDGE_ALIGNED);
            EPWM_EnableDutyInt(g_apEPWMModule[j], u32TestCh, EPWM_DUTY_INT_DOWN_COUNT_MATCH_CMP);
            EPWM_Start(g_apEPWMModule[j], 1 << u32TestCh);

            while (EPWM_GetDutyIntFlag(g_apEPWMModule[j], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(EPWM_GetDutyIntFlag(g_apEPWMModule[j], u32TestCh), 1);

            EPWM_DisableDutyInt(g_apEPWMModule[j], u32TestCh);

            EPWM_ForceStop(g_apEPWMModule[j], 1 << u32TestCh);

            EPWM_ClearDutyIntFlag(g_apEPWMModule[j], u32TestCh);

            CU_ASSERT_EQUAL(g_apEPWMModule[j]->INTSTS0 & ((1 << EPWM_INTSTS0_CMPDIF0_Pos) << u32TestCh), 0);

            /* change EPWM operates at up counter type */
            EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[j], 1 << u32TestCh, EPWM_UP_COUNTER);

            EPWM_EnableDutyInt(g_apEPWMModule[j], u32TestCh, EPWM_DUTY_INT_UP_COUNT_MATCH_CMP);

            EPWM_Start(g_apEPWMModule[j], 1 << u32TestCh);

            while (EPWM_GetDutyIntFlag(g_apEPWMModule[j], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(EPWM_GetDutyIntFlag(g_apEPWMModule[j], u32TestCh), 1);

            EPWM_DisableDutyInt(g_apEPWMModule[j], u32TestCh);

            EPWM_ForceStop(g_apEPWMModule[j], 1 << u32TestCh);

            EPWM_ClearDutyIntFlag(g_apEPWMModule[j], u32TestCh);

            CU_ASSERT_EQUAL(g_apEPWMModule[j]->INTSTS0 & ((1 << EPWM_INTSTS0_CMPDIF0_Pos) << u32TestCh), 0);
        }
    }
}

void Func_EPWM_GetPeriodIntFlag()
{
    uint32_t u32TestCh = 0, j = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (j = 0; j < EPWM_MODULE_NUM; j++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_CNR(g_apEPWMModule[j], u32TestCh, 10);
            EPWM_SET_CMR(g_apEPWMModule[j], u32TestCh, 5);
            EPWM_SET_PRESCALER(g_apEPWMModule[j], u32TestCh, 2);
            EPWM_EnablePeriodInt(g_apEPWMModule[j], u32TestCh, NULL);
            EPWM_Start(g_apEPWMModule[j], 1 << u32TestCh);

            while (EPWM_GetPeriodIntFlag(g_apEPWMModule[j], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(EPWM_GetPeriodIntFlag(g_apEPWMModule[j], u32TestCh), 1);

            EPWM_DisablePeriodInt(g_apEPWMModule[j], u32TestCh);

            EPWM_ForceStop(g_apEPWMModule[j], 1 << u32TestCh);

            EPWM_ClearPeriodIntFlag(g_apEPWMModule[j], u32TestCh);

            CU_ASSERT_EQUAL(g_apEPWMModule[j]->INTSTS0 & ((1 << EPWM_INTSTS0_PIF0_Pos) << u32TestCh), 0);
        }
    }
}

void Func_EPWM_GetCaptureIntFlag()
{
    volatile uint32_t u32Loop = 0, j = 0;
    uint32_t u32TestCh = 0, u32CapCh = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    //enable self-test for EPWM
    SYS_UnlockReg();
    outpw(0x40242300, 0x1ul << 31);//=> it can't work
    outpw(0x40282300, 0x1ul << 31);//=> it can't work

    for (j = 0; j < EPWM_MODULE_NUM; j++)
    {
        //case 1: EPWM0: output, EPWM1: capture(input), EPWM2: output, EPWM3: capture(input) and EPWM4: output, EPWM5: capture(input)
        //case 2: EPWM0: capture(input), EPWM1: output, EPWM2: capture(input), EPWM3: output and EPWM4: capture(input), EPWM5: output
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            /*Set Pwm mode*/
            g_apEPWMModule[j]->CTL1 &= ~((1 << EPWM_CTL1_CNTMODE0_Pos) << u32TestCh);
            EPWM_SET_CNR(g_apEPWMModule[j], u32TestCh, 10000);
            EPWM_SET_CMR(g_apEPWMModule[j], u32TestCh, 5000);
            EPWM_SET_PRESCALER(g_apEPWMModule[j], u32TestCh, 2);
            EPWM_SET_OUTPUT_LEVEL(g_apEPWMModule[j], 1 << u32TestCh, EPWM_OUTPUT_HIGH, EPWM_OUTPUT_LOW, EPWM_OUTPUT_NOTHING, EPWM_OUTPUT_NOTHING);
            EPWM_EnableOutput(g_apEPWMModule[j], 1 << u32TestCh);
            EPWM_Start(g_apEPWMModule[j], 1 << u32TestCh);

            u32CapCh = (u32TestCh % 2) ? (u32TestCh - 1) : (u32TestCh + 1);

            /*Set Pwm mode*/
            g_apEPWMModule[j]->CTL1 &= ~((1 << EPWM_CTL1_CNTMODE0_Pos) << u32CapCh);
            EPWM_SET_CNR(g_apEPWMModule[j], u32CapCh, 10000);
            //EPWM_SET_CNR(g_apEPWMModule[j], u32CapCh, 0xFFFF);
            EPWM_SET_CMR(g_apEPWMModule[j], u32CapCh, 5000);
            EPWM_SET_PRESCALER(g_apEPWMModule[j], u32CapCh, 2);
            EPWM_EnableCaptureInt(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_FALLING_LATCH | EPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->CAPIEN, 0x00000101 << u32CapCh);

            EPWM_EnableCapture(g_apEPWMModule[j], 1 << u32CapCh);
            EPWM_ClearCaptureIntFlag(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_FALLING_LATCH | EPWM_CAPTURE_INT_RISING_LATCH);
            EPWM_Start(g_apEPWMModule[j], 1 << u32CapCh);

            u32Loop = 0;

            while (EPWM_GetCaptureIntFlag(g_apEPWMModule[j], u32CapCh) < 3)
            {
                if (u32Loop++ > (SystemCoreClock / 1000))
                {
                    if (g_apEPWMModule[j] == EPWM0)
                    {
                        CU_FAIL("Check EPWM0_CH0(PC.0)<->EPWM0_CH1(PC.1)");
                    }
                    else if (g_apEPWMModule[j] == EPWM1)
                    {
                        CU_FAIL("Check EPWM1_CH0(PC.6)<->EPWM1_CH1(PC.7)");
                    }

                    return;
                    //break;
                }
            }

            CU_ASSERT_EQUAL(EPWM_GetCaptureIntFlag(g_apEPWMModule[j], u32CapCh), 3);

            EPWM_DisableCapture(g_apEPWMModule[j], 1 << u32CapCh);
            EPWM_ClearCaptureIntFlag(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_FALLING_LATCH | EPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->CAPIF, 0);

            CU_ASSERT_EQUAL(EPWM_GET_CAPTURE_RISING_DATA(g_apEPWMModule[j], u32CapCh), g_apEPWMModule[j]->CAPDAT[1].RCAPDAT);
            CU_ASSERT_EQUAL(EPWM_GET_CAPTURE_FALLING_DATA(g_apEPWMModule[j], u32CapCh), g_apEPWMModule[j]->CAPDAT[1].FCAPDAT);

            //            CU_ASSERT_EQUAL(EPWM_GET_CAPTURE_RISING_DATA(g_apEPWMModule[j], u32CapCh), g_apEPWMModule[j]->RCAPDAT1);
            //            CU_ASSERT_EQUAL(EPWM_GET_CAPTURE_FALLING_DATA(g_apEPWMModule[j], u32CapCh), g_apEPWMModule[j]->FCAPDAT1);

            //         EPWM_DisableOutput(g_apEPWMModule[j], 1 << u32TestCh);
            //         EPWM_ForceStop(g_apEPWMModule[j], 1 << u32TestCh);
            EPWM_ForceStop(g_apEPWMModule[j], 1 << u32CapCh);

            EPWM_DisableCaptureInt(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_RISING_LATCH);
            EPWM_EnableCaptureInt(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_FALLING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->CAPIEN, 0x00000100 << u32CapCh);

            EPWM_EnableCapture(g_apEPWMModule[j], 1 << u32CapCh);
            EPWM_ClearCaptureIntFlag(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_FALLING_LATCH | EPWM_CAPTURE_INT_RISING_LATCH);
            //EPWM_ClearCaptureIntFlag(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_FALLING_LATCH);
            EPWM_Start(g_apEPWMModule[j], 1 << u32CapCh);

            u32Loop = 0;

            while (EPWM_GetCaptureIntFlag(g_apEPWMModule[j], u32CapCh) < 3)
            {
                if (u32Loop++ > (SystemCoreClock / 1000))
                {
                    break;
                }
            }

            //CU_ASSERT_EQUAL(EPWM_GetCaptureIntFlag(g_apEPWMModule[j], 1), 2);
            CU_ASSERT_EQUAL(EPWM_GetCaptureIntFlag(g_apEPWMModule[j], u32CapCh), 3);

            EPWM_DisableCapture(g_apEPWMModule[j], 1 << u32CapCh);
            EPWM_ClearCaptureIntFlag(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_FALLING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->CAPIF, 0x00000001 << u32CapCh);

            //         EPWM_DisableOutput(g_apEPWMModule[j], 1 << u32TestCh);
            //         EPWM_ForceStop(g_apEPWMModule[j], 1 << u32TestCh);
            EPWM_ForceStop(g_apEPWMModule[j], 1 << u32CapCh);

            EPWM_DisableCaptureInt(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_FALLING_LATCH);
            EPWM_EnableCaptureInt(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->CAPIEN, 0x000000001 << u32CapCh);

            EPWM_EnableCapture(g_apEPWMModule[j], 1 << u32CapCh);
            EPWM_ClearCaptureIntFlag(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_FALLING_LATCH | EPWM_CAPTURE_INT_RISING_LATCH);
            //EPWM_ClearCaptureIntFlag(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_RISING_LATCH);
            EPWM_Start(g_apEPWMModule[j], 1 << u32CapCh);

            u32Loop = 0;

            while (EPWM_GetCaptureIntFlag(g_apEPWMModule[j], u32CapCh) < 3)
            {
                if (u32Loop++ > (SystemCoreClock / 1000))
                {
                    break;
                }
            }

            //CU_ASSERT_EQUAL(EPWM_GetCaptureIntFlag(g_apEPWMModule[j], u32CapCh), 2);
            CU_ASSERT_EQUAL(EPWM_GetCaptureIntFlag(g_apEPWMModule[j], u32CapCh), 3);

            EPWM_DisableCapture(g_apEPWMModule[j], 1 << u32CapCh);
            EPWM_DisableCaptureInt(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_RISING_LATCH);
            EPWM_ClearCaptureIntFlag(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_RISING_LATCH);
            CU_ASSERT_EQUAL(g_apEPWMModule[j]->CAPIF, 0x00000100 << u32CapCh);

            EPWM_ClearCaptureIntFlag(g_apEPWMModule[j], u32CapCh, EPWM_CAPTURE_INT_FALLING_LATCH);

            EPWM_DisableOutput(g_apEPWMModule[j], 1 << u32TestCh);
            EPWM_ForceStop(g_apEPWMModule[j], 1 << u32TestCh);
            EPWM_ForceStop(g_apEPWMModule[j], 1 << u32CapCh);
        }
    }
}

void Func_EPWM_ConfigOutputChannel()
{
    uint32_t j = 0, u32TestCh = 0, u32DutyIndex = 0, u32PeriodIndex = 0;
    uint32_t au32Period[2] = {1000, 100};
    uint32_t au32Duty[3] = {50, 0, 100}; /* 50%, 0%, 100% */

    /*                                {{PERIOD1, PERIOD2},{Duty1, Duty2},{Psc1, Psc2}} */

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (j = 0; j < EPWM_MODULE_NUM; j++)
    {
        for (u32DutyIndex = 0; u32DutyIndex < sizeof(au32Duty) / sizeof(uint32_t); u32DutyIndex++)
        {
            for (u32PeriodIndex = 0; u32PeriodIndex < sizeof(au32Period) / sizeof(uint32_t); u32PeriodIndex++)
            {
                for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
                {
                    CU_ASSERT_EQUAL(EPWM_ConfigOutputChannel(g_apEPWMModule[j], u32TestCh, au32Period[u32PeriodIndex], au32Duty[u32DutyIndex]), au32Period[u32PeriodIndex]);
                    CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1 & ((1 << EPWM_CTL1_CNTMODE0_Pos) << u32TestCh), 0 << (16 + u32TestCh));//auto-reload mode
                    CU_ASSERT_EQUAL((g_apEPWMModule[j]->CTL1 & ((1 << EPWM_CTL1_CNTTYPE0_Pos) << (2 * u32TestCh))), 0 << (2 * u32TestCh)); //edge-aligned type
                }
            }
        }
    }
}

void Func_EPWM_ConfigCaptureChannel()
{
    uint32_t j = 0, u32TestCh = 0, u32CapTimeIndex = 0;
    uint32_t au32CapTime[2];

    /*                             {{RealCapTime1, RealCapTime2},{PERIOD1, PERIOD2},{Psc1, Psc2}} */

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (j = 0; j < EPWM_MODULE_NUM; j++)
    {
        for (u32CapTimeIndex = 0; u32CapTimeIndex < sizeof(au32CapTime) / sizeof(uint32_t); u32CapTimeIndex++)
        {
            for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
            {
                CU_ASSERT_EQUAL(g_apEPWMModule[j]->CTL1 & ((1 << EPWM_CTL1_CNTMODE0_Pos) << u32TestCh), 0 << (16 + u32TestCh));//auto-reload mode
                CU_ASSERT_EQUAL((g_apEPWMModule[j]->CTL1 & ((1 << EPWM_CTL1_CNTTYPE0_Pos) << (2 * u32TestCh))), 0 << (2 * u32TestCh));//edge-aligned type
            }
        }
    }
}

void Func_EPWM_EnableDACTrigger()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableDACTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRIGGER_DAC_ZERO);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN & ((1 << EPWM_DACTRGEN_ZTE0_Pos) << u32TestCh), 1 << u32TestCh);
            EPWM_DisableDACTrigger(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN, 0);

            EPWM_EnableDACTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRIGGER_DAC_PERIOD);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN & ((1 << EPWM_DACTRGEN_PTE0_Pos) << u32TestCh), 0x0100 << u32TestCh);
            EPWM_DisableDACTrigger(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN, 0);

            EPWM_EnableDACTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRIGGER_DAC_COMPARE_UP);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN & ((1 << EPWM_DACTRGEN_CUTRGE0_Pos) << u32TestCh), 0x010000 << u32TestCh);
            EPWM_DisableDACTrigger(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN, 0);

            EPWM_EnableDACTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRIGGER_DAC_COMPARE_DOWN);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN & ((1 << EPWM_DACTRGEN_CDTRGE0_Pos) << u32TestCh), 0x01000000 << u32TestCh);
            EPWM_DisableDACTrigger(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN, 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableDACTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRIGGER_DAC_ZERO);
            EPWM_EnableDACTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRIGGER_DAC_PERIOD);
            EPWM_EnableDACTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRIGGER_DAC_COMPARE_UP);
            EPWM_EnableDACTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRIGGER_DAC_COMPARE_DOWN);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN, 0x3F3F3F3F);

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DisableDACTrigger(g_apEPWMModule[x], u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN, 0);

        //test case 3: enable/disable multiple conditions each time
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableDACTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRIGGER_DAC_ZERO | EPWM_TRIGGER_DAC_COMPARE_UP);
            EPWM_EnableDACTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRIGGER_DAC_PERIOD | EPWM_TRIGGER_DAC_COMPARE_DOWN);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN, 0x01010101 << u32TestCh);

            EPWM_DisableDACTrigger(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN, 0);
        }
    }
}

void Func_EPWM_GetDACTriggerFlag()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    //     /* Enable DAC module clock */ //=> it just check EPWM flag, so needn't enable DAC module clock
    //     CLK->APBCLK1 |= CLK_APBCLK1_DACCKEN_Msk;
    //     DAC->CTL |= (DAC_EPWM0_TRIGGER | DAC_CTL_DACEN_Msk);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[x], 1 << u32TestCh, NULL); //up counter
            EPWM_SET_CNR(g_apEPWMModule[x], u32TestCh, 100);
            EPWM_SET_CMR(g_apEPWMModule[x], u32TestCh, 30);

            //EPWM trigger DAC at zero point
            EPWM_EnableDACTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRIGGER_DAC_ZERO);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN & ((1 << EPWM_DACTRGEN_ZTE0_Pos) << u32TestCh), 1 << u32TestCh);
            EPWM_ClearZeroIntFlag(g_apEPWMModule[x], u32TestCh);
            EPWM_Start(g_apEPWMModule[x], 1 << u32TestCh);

            while (EPWM_GetZeroIntFlag(g_apEPWMModule[x], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(EPWM_GetDACTriggerFlag(g_apEPWMModule[x], u32TestCh), 1);

            EPWM_ForceStop(g_apEPWMModule[x], 1 << u32TestCh);

            EPWM_ClearDACTriggerFlag(g_apEPWMModule[x], u32TestCh, NULL);

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->STATUS & EPWM_STATUS_DACTRGF_Msk, 0);

            EPWM_DisableDACTrigger(g_apEPWMModule[x], u32TestCh);

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN, 0);

            //EPWM trigger DAC at period point
            EPWM_EnableDACTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRIGGER_DAC_PERIOD);

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN & ((1 << EPWM_DACTRGEN_PTE0_Pos) << u32TestCh), 0x0100 << u32TestCh);

            EPWM_ClearPeriodIntFlag(g_apEPWMModule[x], u32TestCh);

            EPWM_Start(g_apEPWMModule[x], 1 << u32TestCh);

            while (EPWM_GetPeriodIntFlag(g_apEPWMModule[x], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(EPWM_GetDACTriggerFlag(g_apEPWMModule[x], u32TestCh), 1);

            EPWM_ForceStop(g_apEPWMModule[x], 1 << u32TestCh);

            EPWM_ClearDACTriggerFlag(g_apEPWMModule[x], u32TestCh, NULL);

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->STATUS & EPWM_STATUS_DACTRGF_Msk, 0);

            EPWM_DisableDACTrigger(g_apEPWMModule[x], u32TestCh);

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN, 0);

            //EPWM trigger DAC at compare up point
            EPWM_EnableDACTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRIGGER_DAC_COMPARE_UP);

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN & ((1 << EPWM_DACTRGEN_CUTRGE0_Pos) << u32TestCh), 0x010000 << u32TestCh);

            EPWM_ClearDutyIntFlag(g_apEPWMModule[x], u32TestCh);

            EPWM_Start(g_apEPWMModule[x], 1 << u32TestCh);

            while (EPWM_GetDutyIntFlag(g_apEPWMModule[x], u32TestCh) == 0);

            //CU_ASSERT_EQUAL(EPWM_GetDACTriggerFlag(g_apEPWMModule[x], u32TestCh), 1);
            EPWM_ForceStop(g_apEPWMModule[x], 1 << u32TestCh);
            EPWM_ClearDACTriggerFlag(g_apEPWMModule[x], u32TestCh, NULL);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->STATUS & EPWM_STATUS_DACTRGF_Msk, 0);
            EPWM_DisableDACTrigger(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN, 0);

            //EPWM trigger DAC at compare down point
            EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[x], 1 << u32TestCh, EPWM_EDGE_ALIGNED);
            EPWM_EnableDACTrigger(g_apEPWMModule[x], u32TestCh, EPWM_TRIGGER_DAC_COMPARE_DOWN);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN & ((1 << EPWM_DACTRGEN_CDTRGE0_Pos) << u32TestCh), 0x01000000 << u32TestCh);
            EPWM_ClearDutyIntFlag(g_apEPWMModule[x], u32TestCh);
            EPWM_Start(g_apEPWMModule[x], 1 << u32TestCh);

            while (EPWM_GetDutyIntFlag(g_apEPWMModule[x], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(EPWM_GetDACTriggerFlag(g_apEPWMModule[x], u32TestCh), 1);

            EPWM_ForceStop(g_apEPWMModule[x], 1 << u32TestCh);

            EPWM_ClearDACTriggerFlag(g_apEPWMModule[x], u32TestCh, NULL);

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->STATUS & EPWM_STATUS_DACTRGF_Msk, 0);

            EPWM_DisableDACTrigger(g_apEPWMModule[x], u32TestCh);

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->DACTRGEN, 0);
        }
    }

    /* Disable EPWM module clock */
    //     CLK->APBCLK1 &= ~CLK_APBCLK1_DACCKEN_Msk;
}

void Func_EPWM_EnableFaultBrake()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t u32PatternIdx = 0, u32PatternCount = 0, u32ChMskIdx = 0;
    uint32_t au32PatternData[16] = \
    {
        EPWM_FB_EDGE_ACMP0, EPWM_FB_EDGE_ACMP1, EPWM_FB_EDGE_BKP0, EPWM_FB_EDGE_BKP1, EPWM_FB_EDGE_SYS_CSS, EPWM_FB_EDGE_SYS_BOD, EPWM_FB_EDGE_SYS_RAM, EPWM_FB_EDGE_SYS_COR, \
        EPWM_FB_LEVEL_ACMP0, EPWM_FB_LEVEL_ACMP1, EPWM_FB_LEVEL_BKP0, EPWM_FB_LEVEL_BKP1, EPWM_FB_LEVEL_SYS_CSS, EPWM_FB_LEVEL_SYS_BOD, EPWM_FB_LEVEL_SYS_RAM, EPWM_FB_LEVEL_SYS_COR
    };

    uint32_t au32BRKCTLRegMsk[16] = \
    {
        EPWM_BRKCTL0_1_CPO0EBEN_Msk, EPWM_BRKCTL0_1_CPO1EBEN_Msk, EPWM_BRKCTL0_1_BRKP0EEN_Msk, EPWM_BRKCTL0_1_BRKP1EEN_Msk, \
        EPWM_BRKCTL0_1_SYSEBEN_Msk, EPWM_BRKCTL0_1_SYSEBEN_Msk, EPWM_BRKCTL0_1_SYSEBEN_Msk, EPWM_BRKCTL0_1_SYSEBEN_Msk, \
        EPWM_BRKCTL0_1_CPO0LBEN_Msk, EPWM_BRKCTL0_1_CPO1LBEN_Msk, EPWM_BRKCTL0_1_BRKP0LEN_Msk, EPWM_BRKCTL0_1_BRKP1LEN_Msk, \
        EPWM_BRKCTL0_1_SYSLBEN_Msk, EPWM_BRKCTL0_1_SYSLBEN_Msk, EPWM_BRKCTL0_1_SYSLBEN_Msk, EPWM_BRKCTL0_1_SYSLBEN_Msk
    };

    uint32_t au32BRKCTLRegCheck[16] = \
    {
        0x1, 0x2, 0x10, 0x20, \
        0x80, 0x80, 0x80, 0x80, \
        0x100, 0x200, 0x1000, 0x2000, \
        0x8000, 0x8000, 0x8000, 0x8000
    };

    uint32_t au32FAILBRKRegCheck[16] = \
    {
        0, 0, 0, 0, \
        0x1, 0x2, 0x4, 0x8, \
        0, 0, 0, 0, \
        0x1, 0x2, 0x4, 0x8
    };

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    u32PatternCount = sizeof(au32PatternData) / sizeof(uint32_t);

    /* unlock protected registers */
    SYS_UnlockReg();

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            for (u32PatternIdx = 0; u32PatternIdx < u32PatternCount; u32PatternIdx++)
            {
                EPWM_EnableFaultBrake(g_apEPWMModule[x], 1 << u32TestCh, 1 << u32TestCh, au32PatternData[u32PatternIdx]);
                CU_ASSERT_EQUAL(*(__IO uint32_t *)(&(g_apEPWMModule[x]->BRKCTL[0]) + (u32TestCh >> 1)) & (au32BRKCTLRegMsk[u32PatternIdx]), au32BRKCTLRegCheck[u32PatternIdx]);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->FAILBRK, au32FAILBRKRegCheck[u32PatternIdx]);

                if (u32TestCh % 2 == 0)
                {
                    //CU_ASSERT_EQUAL((*(__IO uint32_t *)(&(g_apEPWMModule[x]->BRKCTL0_1) + (u32TestCh >> 1))) & (EPWM_BRKCTL0_1_BRKAEVEN_Msk), 3 << 16);
                    CU_ASSERT_EQUAL((*(__IO uint32_t *)(&(g_apEPWMModule[x]->BRKCTL[0]) + (u32TestCh >> 1))) & (EPWM_BRKCTL0_1_BRKAEVEN_Msk | EPWM_BRKCTL0_1_BRKAODD_Msk), 0xB << 16);
                }
                else
                {
                    //CU_ASSERT_EQUAL((*(__IO uint32_t *)(&(g_apEPWMModule[x]->BRKCTL0_1) + (u32TestCh >> 1))) & (EPWM_BRKCTL0_1_BRKAODD_Msk), 3 << 18);
                    CU_ASSERT_EQUAL((*(__IO uint32_t *)(&(g_apEPWMModule[x]->BRKCTL[0]) + (u32TestCh >> 1))) & (EPWM_BRKCTL0_1_BRKAEVEN_Msk | EPWM_BRKCTL0_1_BRKAODD_Msk), 0xE << 16);
                }

                g_apEPWMModule[x]->FAILBRK = 0;
            }
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            for (u32PatternIdx = 0; u32PatternIdx < u32PatternCount; u32PatternIdx++)
            {
                EPWM_EnableFaultBrake(g_apEPWMModule[x], 1 << u32TestCh, 1 << u32TestCh, au32PatternData[u32PatternIdx]);
            }
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[0], 0x000AB3B3);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[1], 0x000AB3B3);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[2], 0x000EB3B3);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->FAILBRK, 0xF);

        //test case 3: enable/disable multiple conditions each time
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            for (u32ChMskIdx = 0; u32ChMskIdx < sizeof(SixBitsPatternTable) / sizeof(uint32_t); u32ChMskIdx++)
            {
                EPWM_EnableFaultBrake(g_apEPWMModule[x], 1 << u32TestCh, 0x3F, au32PatternData[u32PatternIdx]);
            }

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[0], 0x000FB3B3);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[1], 0x000FB3B3);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[2], 0x000FB3B3);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->FAILBRK, 0xF);

            for (u32ChMskIdx = 0; u32ChMskIdx < sizeof(SixBitsPatternTable) / sizeof(uint32_t); u32ChMskIdx++)
            {
                EPWM_EnableFaultBrake(g_apEPWMModule[x], 1 << u32TestCh, 0x2A, au32PatternData[u32PatternIdx]);
            }

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[0], 0x000EB3B3);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[1], 0x000EB3B3);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[2], 0x000EB3B3);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->FAILBRK, 0xF);

            for (u32ChMskIdx = 0; u32ChMskIdx < sizeof(SixBitsPatternTable) / sizeof(uint32_t); u32ChMskIdx++)
            {
                EPWM_EnableFaultBrake(g_apEPWMModule[x], 1 << u32TestCh, 0x15, au32PatternData[u32PatternIdx]);
            }

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[0], 0x000BB3B3);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[1], 0x000BB3B3);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[2], 0x000BB3B3);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->FAILBRK, 0xF);

            for (u32ChMskIdx = 0; u32ChMskIdx < sizeof(SixBitsPatternTable) / sizeof(uint32_t); u32ChMskIdx++)
            {
                EPWM_EnableFaultBrake(g_apEPWMModule[x], 1 << u32TestCh, 0x00, au32PatternData[u32PatternIdx]);
            }

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[0], 0x000AB3B3);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[1], 0x000AB3B3);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BRKCTL[2], 0x000AB3B3);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->FAILBRK, 0xF);
        }
    }
}

void Func_EPWM_EnableFaultBrakeInt()
{
    uint32_t x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        EPWM_EnableFaultBrakeInt(g_apEPWMModule[x], EPWM_FB_EDGE);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN1, 0x7);
        EPWM_DisableFaultBrakeInt(g_apEPWMModule[x], EPWM_FB_EDGE);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN1, 0);
        EPWM_EnableFaultBrakeInt(g_apEPWMModule[x], EPWM_FB_LEVEL);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN1, 0x700);
        EPWM_DisableFaultBrakeInt(g_apEPWMModule[x], EPWM_FB_LEVEL);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN1, 0);

        //test case 2: enable/disable one channel each time, then check at final action
        EPWM_EnableFaultBrakeInt(g_apEPWMModule[x], EPWM_FB_EDGE);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN1, 0x7);
        EPWM_EnableFaultBrakeInt(g_apEPWMModule[x], EPWM_FB_LEVEL);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN1, 0x707);
        EPWM_DisableFaultBrakeInt(g_apEPWMModule[x], EPWM_FB_LEVEL);
        EPWM_DisableFaultBrakeInt(g_apEPWMModule[x], EPWM_FB_EDGE);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN1, 0);
    }
}

void Func_EPWM_EnableZeroInt()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t au32RegCheck[6] = {0x00000001, 0x00000003, 0x00000007, 0x0000000F, 0x0000001F, 0x0000003F};
    uint32_t au32RegReverseCheck[6] = {0x0000003E, 0x0000003C, 0x00000038, 0x00000030, 0x00000020, 0x00000000};

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableZeroInt(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN0 & (1 << u32TestCh), 1 << u32TestCh);
            EPWM_DisableZeroInt(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN0, 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableZeroInt(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN0, au32RegCheck[u32TestCh]);
        }

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DisableZeroInt(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTEN0, au32RegReverseCheck[u32TestCh]);
        }
    }
}

void Func_EPWM_GetZeroIntFlag()
{
    uint32_t u32TestCh = 0, j = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (j = 0; j < EPWM_MODULE_NUM; j++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_CNR(g_apEPWMModule[j], u32TestCh, 10);
            EPWM_SET_CMR(g_apEPWMModule[j], u32TestCh, 5);
            EPWM_SET_PRESCALER(g_apEPWMModule[j], u32TestCh, 2);
            EPWM_EnableZeroInt(g_apEPWMModule[j], u32TestCh);
            EPWM_Start(g_apEPWMModule[j], 1 << u32TestCh);

            while (EPWM_GetZeroIntFlag(g_apEPWMModule[j], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(EPWM_GetZeroIntFlag(g_apEPWMModule[j], u32TestCh), 1);

            EPWM_DisableZeroInt(g_apEPWMModule[j], u32TestCh);

            EPWM_ForceStop(g_apEPWMModule[j], 1 << u32TestCh);

            EPWM_ClearZeroIntFlag(g_apEPWMModule[j], u32TestCh);

            CU_ASSERT_EQUAL(g_apEPWMModule[j]->INTSTS0 & ((1 << EPWM_INTSTS0_ZIF0_Pos) << u32TestCh), 0);
        }
    }
}

void Func_EPWM_EnableAcc()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t u32PatternIdx = 0, u32PatternCount = 0;
    uint32_t au32PatternData[4] = \
    {EPWM_IFA_ZERO_POINT, EPWM_IFA_PERIOD_POINT, EPWM_IFA_COMPARE_UP_COUNT_POINT, EPWM_IFA_COMPARE_DOWN_COUNT_POINT};
    uint32_t au32EnableRegCheck[4] = {0x80000000, 0x90000000, 0xA0000000, 0xB0000000};
    uint32_t au32DisableRegCheck[4] = {0x0, 0x10000000, 0x20000000, 0x30000000};

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    u32PatternCount = sizeof(au32PatternData) / sizeof(uint32_t);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            for (u32PatternIdx = 0; u32PatternIdx < u32PatternCount; u32PatternIdx++)
            {
                EPWM_EnableAcc(g_apEPWMModule[x], u32TestCh, 0xF, au32PatternData[u32PatternIdx]);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[u32TestCh], (au32EnableRegCheck[u32PatternIdx] | 0xF));
                EPWM_DisableAcc(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[u32TestCh], (au32DisableRegCheck[u32PatternIdx] | 0xF));

                EPWM_EnableAcc(g_apEPWMModule[x], u32TestCh, 0xA, au32PatternData[u32PatternIdx]);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[u32TestCh], (au32EnableRegCheck[u32PatternIdx] | 0xA));
                EPWM_DisableAcc(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[u32TestCh], (au32DisableRegCheck[u32PatternIdx] | 0xA));

                EPWM_EnableAcc(g_apEPWMModule[x], u32TestCh, 0x5, au32PatternData[u32PatternIdx]);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[u32TestCh], (au32EnableRegCheck[u32PatternIdx] | 0x5));
                EPWM_DisableAcc(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[u32TestCh], (au32DisableRegCheck[u32PatternIdx] | 0x5));

                EPWM_EnableAcc(g_apEPWMModule[x], u32TestCh, 0, au32PatternData[u32PatternIdx]);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[u32TestCh], (au32EnableRegCheck[u32PatternIdx] | 0));
                EPWM_DisableAcc(g_apEPWMModule[x], u32TestCh);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[u32TestCh], (au32DisableRegCheck[u32PatternIdx] | 0));
                g_apEPWMModule[x]->IFA[u32TestCh] = 0;
            }
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            for (u32PatternIdx = 0; u32PatternIdx < u32PatternCount; u32PatternIdx++)
            {
                EPWM_EnableAcc(g_apEPWMModule[x], u32TestCh, 0xF, au32PatternData[u32PatternIdx]);
                EPWM_DisableAcc(g_apEPWMModule[x], u32TestCh);
                EPWM_EnableAcc(g_apEPWMModule[x], u32TestCh, 0xA, au32PatternData[u32PatternIdx]);
                EPWM_DisableAcc(g_apEPWMModule[x], u32TestCh);
            }
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[5], 0x3000000A);

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DisableAcc(g_apEPWMModule[x], u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[5], 0x3000000A);
    }
}

void Func_EPWM_EnableAccInt()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        EPWM_EnableAccInt(g_apEPWMModule[x], 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->AINTEN, 1 << 0);
        EPWM_DisableAccInt(g_apEPWMModule[x], 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->AINTEN, 0);

        EPWM_EnableAccInt(g_apEPWMModule[x], 1);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->AINTEN & EPWM_AINTSTS_IFAIF1_Msk, EPWM_AINTSTS_IFAIF1_Msk);
        EPWM_DisableAccInt(g_apEPWMModule[x], 1);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->AINTEN, 0);

        EPWM_EnableAccInt(g_apEPWMModule[x], 2);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->AINTEN & EPWM_AINTSTS_IFAIF2_Msk, EPWM_AINTSTS_IFAIF2_Msk);
        EPWM_DisableAccInt(g_apEPWMModule[x], 2);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->AINTEN, 0);

        EPWM_EnableAccInt(g_apEPWMModule[x], 3);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->AINTEN & EPWM_AINTSTS_IFAIF3_Msk, EPWM_AINTSTS_IFAIF3_Msk);
        EPWM_DisableAccInt(g_apEPWMModule[x], 3);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->AINTEN, 0);

        EPWM_EnableAccInt(g_apEPWMModule[x], 4);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->AINTEN & EPWM_AINTSTS_IFAIF4_Msk, EPWM_AINTSTS_IFAIF4_Msk);
        EPWM_DisableAccInt(g_apEPWMModule[x], 4);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->AINTEN, 0);

        EPWM_EnableAccInt(g_apEPWMModule[x], 5);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->AINTEN & EPWM_AINTSTS_IFAIF5_Msk, EPWM_AINTSTS_IFAIF5_Msk);
        EPWM_DisableAccInt(g_apEPWMModule[x], 5);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->AINTEN, 0);

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableAccInt(g_apEPWMModule[x], u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->AINTEN, 0x3F);

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DisableAccInt(g_apEPWMModule[x], u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->AINTEN, 0);
    }
}

void Func_EPWM_EnableAccStopMode()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        EPWM_EnableAccStopMode(g_apEPWMModule[x], 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[0] & EPWM_IFA0_STPMOD_Msk, EPWM_IFA0_STPMOD_Msk);
        EPWM_DisableAccStopMode(g_apEPWMModule[x], 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[0] & EPWM_IFA0_STPMOD_Msk, 0);

        EPWM_EnableAccStopMode(g_apEPWMModule[x], 1);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[1] & EPWM_IFA0_STPMOD_Msk, EPWM_IFA0_STPMOD_Msk);
        EPWM_DisableAccStopMode(g_apEPWMModule[x], 1);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[1] & EPWM_IFA0_STPMOD_Msk, 0);

        EPWM_EnableAccStopMode(g_apEPWMModule[x], 2);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[2] & EPWM_IFA0_STPMOD_Msk, EPWM_IFA0_STPMOD_Msk);
        EPWM_DisableAccStopMode(g_apEPWMModule[x], 2);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[2] & EPWM_IFA0_STPMOD_Msk, 0);

        EPWM_EnableAccStopMode(g_apEPWMModule[x], 3);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[3] & EPWM_IFA0_STPMOD_Msk, EPWM_IFA0_STPMOD_Msk);
        EPWM_DisableAccStopMode(g_apEPWMModule[x], 3);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[3] & EPWM_IFA0_STPMOD_Msk, 0);

        EPWM_EnableAccStopMode(g_apEPWMModule[x], 4);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[4] & EPWM_IFA0_STPMOD_Msk, EPWM_IFA0_STPMOD_Msk);
        EPWM_DisableAccStopMode(g_apEPWMModule[x], 4);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[4] & EPWM_IFA0_STPMOD_Msk, 0);

        EPWM_EnableAccStopMode(g_apEPWMModule[x], 5);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[5] & EPWM_IFA0_STPMOD_Msk, EPWM_IFA0_STPMOD_Msk);
        EPWM_DisableAccStopMode(g_apEPWMModule[x], 5);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[5] & EPWM_IFA0_STPMOD_Msk, 0);

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableAccStopMode(g_apEPWMModule[x], u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[0] & EPWM_IFA0_STPMOD_Msk, EPWM_IFA0_STPMOD_Msk);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[1] & EPWM_IFA0_STPMOD_Msk, EPWM_IFA0_STPMOD_Msk);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[2] & EPWM_IFA0_STPMOD_Msk, EPWM_IFA0_STPMOD_Msk);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[3] & EPWM_IFA0_STPMOD_Msk, EPWM_IFA0_STPMOD_Msk);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[4] & EPWM_IFA0_STPMOD_Msk, EPWM_IFA0_STPMOD_Msk);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[5] & EPWM_IFA0_STPMOD_Msk, EPWM_IFA0_STPMOD_Msk);

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DisableAccStopMode(g_apEPWMModule[x], u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[0] & EPWM_IFA0_STPMOD_Msk, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[1] & EPWM_IFA0_STPMOD_Msk, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[2] & EPWM_IFA0_STPMOD_Msk, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[3] & EPWM_IFA0_STPMOD_Msk, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[4] & EPWM_IFA0_STPMOD_Msk, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->IFA[5] & EPWM_IFA0_STPMOD_Msk, 0);
    }
}

void Func_EPWM_GetAccInt()
{
    uint32_t u32TestCh = 0, x = 0;
    //uint32_t au32RegCheck[6] = {0x81, 0x83, 0x8007, 0x800F, 0x80001F, 0x80003F};
    uint32_t au32ClearRegCheck[6] = {0x010101, 0x030303, 0x070707, 0x0F0F0F, 0x1F1F1F, 0x3F3F3F};

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh += 2)
        {
            EPWM_SET_CNR(g_apEPWMModule[x], u32TestCh, 10);
            EPWM_SET_CMR(g_apEPWMModule[x], u32TestCh, 5);
            EPWM_SET_PRESCALER(g_apEPWMModule[x], u32TestCh, 2);
            EPWM_EnableAcc(g_apEPWMModule[x], u32TestCh, 0xF, EPWM_IFA_ZERO_POINT);
            EPWM_Start(g_apEPWMModule[x], 1 << u32TestCh);

            while (EPWM_GetAccInt(g_apEPWMModule[x], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(EPWM_GetAccInt(g_apEPWMModule[x], u32TestCh), 1);

            EPWM_ForceStop(g_apEPWMModule[x], 1 << u32TestCh);

            CU_ASSERT_EQUAL(EPWM_GetAccCounter(g_apEPWMModule[x], u32TestCh), 0xF + 1);

            EPWM_ClearAccInt(g_apEPWMModule[x], u32TestCh);

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTSTS0, au32ClearRegCheck[u32TestCh]);

            EPWM_SET_CNR(g_apEPWMModule[x], u32TestCh + 1, 10);

            EPWM_SET_CMR(g_apEPWMModule[x], u32TestCh + 1, 5);

            EPWM_SET_PRESCALER(g_apEPWMModule[x], u32TestCh + 1, 2);

            EPWM_EnableAcc(g_apEPWMModule[x], u32TestCh + 1, 0xF, EPWM_IFA_PERIOD_POINT);

            EPWM_Start(g_apEPWMModule[x], 1 << (u32TestCh + 1));

            while (EPWM_GetAccInt(g_apEPWMModule[x], u32TestCh + 1) == 0) {};

            CU_ASSERT_EQUAL(EPWM_GetAccInt(g_apEPWMModule[x], u32TestCh + 1), 1);

            EPWM_ForceStop(g_apEPWMModule[x], 1 << (u32TestCh + 1));

            CU_ASSERT_EQUAL(EPWM_GetAccCounter(g_apEPWMModule[x], u32TestCh + 1), 0xF + 1);

            EPWM_ClearAccInt(g_apEPWMModule[x], u32TestCh + 1);

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTSTS0, au32ClearRegCheck[u32TestCh + 1]);
        }
    }
}

void Func_EPWM_GetFTDutyIntFlag()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
            //for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh+=2)
        {
            EPWM_SET_CNR(g_apEPWMModule[x], u32TestCh, 10);
            EPWM_SET_CMR(g_apEPWMModule[x], u32TestCh, 5);
            EPWM_SET_FTCMR(g_apEPWMModule[x], u32TestCh, 5);
            EPWM_SET_PRESCALER(g_apEPWMModule[x], u32TestCh, 2);
            EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[x], 1 << u32TestCh, EPWM_EDGE_ALIGNED);
            EPWM_Start(g_apEPWMModule[x], 1 << u32TestCh);

            while (EPWM_GetFTDutyIntFlag(g_apEPWMModule[x], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(EPWM_GetFTDutyIntFlag(g_apEPWMModule[x], u32TestCh), 1);

            EPWM_ForceStop(g_apEPWMModule[x], 1 << u32TestCh);

            EPWM_ClearFTDutyIntFlag(g_apEPWMModule[x], u32TestCh);

            CU_ASSERT_EQUAL(EPWM_GetFTDutyIntFlag(g_apEPWMModule[x], u32TestCh), 0);

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->FTCI & ((0x7ul << EPWM_FTCI_FTCMD0_Pos) << (u32TestCh >> 1)), 0);

            /* change EPWM operates at up counter type */
            EPWM_SET_ALIGNED_TYPE(g_apEPWMModule[x], 1 << u32TestCh, EPWM_UP_COUNTER);

            EPWM_Start(g_apEPWMModule[x], 1 << u32TestCh);

            while (EPWM_GetFTDutyIntFlag(g_apEPWMModule[x], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(EPWM_GetFTDutyIntFlag(g_apEPWMModule[x], u32TestCh), 1);

            EPWM_ForceStop(g_apEPWMModule[x], 1 << u32TestCh);

            EPWM_ClearFTDutyIntFlag(g_apEPWMModule[x], u32TestCh);

            CU_ASSERT_EQUAL(EPWM_GetFTDutyIntFlag(g_apEPWMModule[x], u32TestCh), 0);

            CU_ASSERT_EQUAL(g_apEPWMModule[x]->FTCI & ((0x7ul << EPWM_FTCI_FTCMU0_Pos) << (u32TestCh >> 1)), 0);
        }
    }
}

void Func_EPWM_EnableLoadMode()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableLoadMode(g_apEPWMModule[x], u32TestCh, EPWM_LOAD_MODE_IMMEDIATE);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CTL0 & 0x3F0000, 1 << u32TestCh << 16);
            EPWM_DisableLoadMode(g_apEPWMModule[x], u32TestCh, EPWM_LOAD_MODE_IMMEDIATE);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CTL0 & 0x3F0000, 0);

            EPWM_EnableLoadMode(g_apEPWMModule[x], u32TestCh, EPWM_LOAD_MODE_WINDOW);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CTL0 & 0x3F00, 1 << u32TestCh << 8);
            EPWM_DisableLoadMode(g_apEPWMModule[x], u32TestCh, EPWM_LOAD_MODE_WINDOW);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CTL0 & 0x3F00, 0);

            EPWM_EnableLoadMode(g_apEPWMModule[x], u32TestCh, EPWM_LOAD_MODE_CENTER);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CTL0 & 0x3F, 1 << u32TestCh);
            EPWM_DisableLoadMode(g_apEPWMModule[x], u32TestCh, EPWM_LOAD_MODE_CENTER);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CTL0 & 0x3F, 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableLoadMode(g_apEPWMModule[x], u32TestCh, EPWM_LOAD_MODE_IMMEDIATE);
            EPWM_EnableLoadMode(g_apEPWMModule[x], u32TestCh, EPWM_LOAD_MODE_WINDOW);
            EPWM_EnableLoadMode(g_apEPWMModule[x], u32TestCh, EPWM_LOAD_MODE_CENTER);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->CTL0, 0x003F3F3F);

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DisableLoadMode(g_apEPWMModule[x], u32TestCh, EPWM_LOAD_MODE_IMMEDIATE);
            EPWM_DisableLoadMode(g_apEPWMModule[x], u32TestCh, EPWM_LOAD_MODE_WINDOW);
            EPWM_DisableLoadMode(g_apEPWMModule[x], u32TestCh, EPWM_LOAD_MODE_CENTER);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->CTL0, 0);
    }
}

void Func_EPWM_ConfigSyncPhase()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t u32PatternIdx = 0, u32PatternCount = 0;
    uint32_t au32PatternData[4] = {EPWM_SYNC_OUT_FROM_SYNCIN_SWSYNC, EPWM_SYNC_OUT_FROM_COUNT_TO_ZERO, EPWM_SYNC_OUT_FROM_COUNT_TO_COMPARATOR, EPWM_SYNC_OUT_DISABLE};
    uint32_t au32RegCheck[4] = {0, 0x100, 0x200, 0x300};

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    u32PatternCount = sizeof(au32PatternData) / sizeof(uint32_t);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            for (u32PatternIdx = 0; u32PatternIdx < u32PatternCount; u32PatternIdx++)
            {
                EPWM_ConfigSyncPhase(g_apEPWMModule[x], u32TestCh, au32PatternData[u32PatternIdx], EPWM_PHS_DIR_DECREMENT, SixteenBitsPatternTable[u32PatternIdx]);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC & ((3 << EPWM_SYNC_SINSRC0_Pos) << ((u32TestCh >> 1) * 2)), au32RegCheck[u32PatternIdx] << ((u32TestCh >> 1) * 2));
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->PHS[(u32TestCh / 2)], SixteenBitsPatternTable[u32PatternIdx]);

                EPWM_ConfigSyncPhase(g_apEPWMModule[x], u32TestCh, au32PatternData[u32PatternIdx], EPWM_PHS_DIR_INCREMENT, SixteenBitsPatternTable[u32PatternIdx]);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC & ((0x3ful << EPWM_SYNC_SINSRC0_Pos) << ((u32TestCh >> 1) * 2)), (au32RegCheck[u32PatternIdx] << ((u32TestCh >> 1) * 2)));
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC & ((1 << EPWM_SYNC_PHSDIR0_Pos) << (u32TestCh >> 1)), 0x1000000 << (u32TestCh >> 1));
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->PHS[(u32TestCh / 2)], SixteenBitsPatternTable[u32PatternIdx]);
            }
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 0x07003F00);
    }
}

void Func_EPWM_EnableSyncPhase()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t u32PatternIdx = 0;
    uint32_t au32RegWrite[4] = {0x2A, 0x15, 0x0, 0x3F};
    uint32_t au32RegEnableCheck[4] = {0x7, 0x7, 0x7, 0x7};
    uint32_t au32RegDisableCheck[4] = {0, 0, 0, 0};

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableSyncPhase(g_apEPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 1 << (u32TestCh >> 1));
            EPWM_DisableSyncPhase(g_apEPWMModule[x], 1 << u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 0);
        }

        //test case 2: enable/disable one channel each time, then check at final action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableSyncPhase(g_apEPWMModule[x], 1 << u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 0x7);

        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_DisableSyncPhase(g_apEPWMModule[x], 1 << u32TestCh);
        }

        CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 0);

        //test case 3: enable/disable multiple conditions each time
        for (u32PatternIdx = 0; u32PatternIdx < sizeof(au32RegWrite) / sizeof(uint32_t); u32PatternIdx++)
        {
            EPWM_EnableSyncPhase(g_apEPWMModule[x], au32RegWrite[u32PatternIdx]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, au32RegEnableCheck[u32PatternIdx]);
        }

        for (u32PatternIdx = 0; u32PatternIdx < sizeof(au32RegWrite) / sizeof(uint32_t); u32PatternIdx++)
        {
            EPWM_DisableSyncPhase(g_apEPWMModule[x], au32RegWrite[u32PatternIdx]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, au32RegDisableCheck[u32PatternIdx]);
        }
    }
}

void Func_EPWM_EnableSyncNoiseFilter()
{
    uint32_t x = 0;
    uint32_t u32PatternIdx = 0;
    uint32_t au32RegPattern[8] = {EPWM_NF_CLK_DIV_1, EPWM_NF_CLK_DIV_2, EPWM_NF_CLK_DIV_4, EPWM_NF_CLK_DIV_8, \
                                  EPWM_NF_CLK_DIV_16, EPWM_NF_CLK_DIV_32, EPWM_NF_CLK_DIV_64, EPWM_NF_CLK_DIV_128
                                 };

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32PatternIdx = 0; u32PatternIdx < sizeof(au32RegPattern) / sizeof(uint32_t); u32PatternIdx++)
        {
            EPWM_EnableSyncNoiseFilter(g_apEPWMModule[x], 7, au32RegPattern[u32PatternIdx]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 0x710000 | au32RegPattern[u32PatternIdx] << 17);
            EPWM_DisableSyncNoiseFilter(g_apEPWMModule[x]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 0x700000 | au32RegPattern[u32PatternIdx] << 17);

            EPWM_EnableSyncNoiseFilter(g_apEPWMModule[x], 5, au32RegPattern[u32PatternIdx]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 0x510000 | au32RegPattern[u32PatternIdx] << 17);
            EPWM_DisableSyncNoiseFilter(g_apEPWMModule[x]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 0x500000 | au32RegPattern[u32PatternIdx] << 17);

            EPWM_EnableSyncNoiseFilter(g_apEPWMModule[x], 2, au32RegPattern[u32PatternIdx]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 0x210000 | au32RegPattern[u32PatternIdx] << 17);
            EPWM_DisableSyncNoiseFilter(g_apEPWMModule[x]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 0x200000 | au32RegPattern[u32PatternIdx] << 17);

            EPWM_EnableSyncNoiseFilter(g_apEPWMModule[x], 0, au32RegPattern[u32PatternIdx]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 0x10000 | au32RegPattern[u32PatternIdx] << 17);
            EPWM_DisableSyncNoiseFilter(g_apEPWMModule[x]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 0 | au32RegPattern[u32PatternIdx] << 17);
        }
    }
}

void Func_EPWM_EnableSyncPinInverse()
{
    uint32_t x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        EPWM_EnableSyncPinInverse(g_apEPWMModule[x]);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 1 << 23);
        EPWM_DisableSyncPinInverse(g_apEPWMModule[x]);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->SYNC, 0);
    }
}

void Func_EPWM_SetClockSource()
{
    uint32_t u32TestCh = 0, x = 0;
    uint32_t u32PatternIdx = 0;
    uint32_t au32RegPattern[5] = {EPWM_CLKSRC_EPWM_CLK, EPWM_CLKSRC_TIMER0, EPWM_CLKSRC_TIMER1, EPWM_CLKSRC_TIMER2, EPWM_CLKSRC_TIMER3};
    uint32_t au32RegCheck[5 * 6] = {0x0, 0x1, 0x2, 0x3, 0x4, \
                                    0x0, 0x1, 0x2, 0x3, 0x4, \
                                    0x4, 0x104, 0x204, 0x304, 0x404, \
                                    0x4, 0x104, 0x204, 0x304, 0x404, \
                                    0x404, 0x10404, 0x20404, 0x30404, 0x40404, \
                                    0x404, 0x10404, 0x20404, 0x30404, 0x40404
                                   };
    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            for (u32PatternIdx = 0; u32PatternIdx < sizeof(au32RegPattern) / sizeof(uint32_t); u32PatternIdx++)
            {
                EPWM_SetClockSource(g_apEPWMModule[x], u32TestCh, au32RegPattern[u32PatternIdx]);
                CU_ASSERT_EQUAL(g_apEPWMModule[x]->CLKSRC, au32RegCheck[u32TestCh * 5 + u32PatternIdx]);
            }
        }
    }
}

void Func_EPWM_EnableBrakeNoiseFilter()
{
    uint32_t x = 0;
    uint32_t u32PatternIdx = 0;
    uint32_t au32RegPattern[8] = {EPWM_NF_CLK_DIV_1, EPWM_NF_CLK_DIV_2, EPWM_NF_CLK_DIV_4, EPWM_NF_CLK_DIV_8, \
                                  EPWM_NF_CLK_DIV_16, EPWM_NF_CLK_DIV_32, EPWM_NF_CLK_DIV_64, EPWM_NF_CLK_DIV_128
                                 };

    uint32_t u32PreReg = 0;
    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32PatternIdx = 0; u32PatternIdx < sizeof(au32RegPattern) / sizeof(uint32_t); u32PatternIdx++)
        {
            EPWM_EnableBrakeNoiseFilter(g_apEPWMModule[x], 0, 7, au32RegPattern[u32PatternIdx]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0x71 | au32RegPattern[u32PatternIdx] << 1);
            EPWM_DisableBrakeNoiseFilter(g_apEPWMModule[x], 0);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0x70 | au32RegPattern[u32PatternIdx] << 1);

            EPWM_EnableBrakeNoiseFilter(g_apEPWMModule[x], 0, 5, au32RegPattern[u32PatternIdx]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0x51 | au32RegPattern[u32PatternIdx] << 1);
            EPWM_DisableBrakeNoiseFilter(g_apEPWMModule[x], 0);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0x50 | au32RegPattern[u32PatternIdx] << 1);

            EPWM_EnableBrakeNoiseFilter(g_apEPWMModule[x], 0, 2, au32RegPattern[u32PatternIdx]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0x21 | au32RegPattern[u32PatternIdx] << 1);
            EPWM_DisableBrakeNoiseFilter(g_apEPWMModule[x], 0);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0x20 | au32RegPattern[u32PatternIdx] << 1);

            EPWM_EnableBrakeNoiseFilter(g_apEPWMModule[x], 0, 0, au32RegPattern[u32PatternIdx]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0x1 | au32RegPattern[u32PatternIdx] << 1);
            EPWM_DisableBrakeNoiseFilter(g_apEPWMModule[x], 0);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0 | au32RegPattern[u32PatternIdx] << 1);
        }

        u32PreReg = g_apEPWMModule[x]->BNF;

        for (u32PatternIdx = 0; u32PatternIdx < sizeof(au32RegPattern) / sizeof(uint32_t); u32PatternIdx++)
        {
            EPWM_EnableBrakeNoiseFilter(g_apEPWMModule[x], 1, 7, au32RegPattern[u32PatternIdx]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0x7100 | au32RegPattern[u32PatternIdx] << 9 | u32PreReg);
            EPWM_DisableBrakeNoiseFilter(g_apEPWMModule[x], 1);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0x7000 | au32RegPattern[u32PatternIdx] << 9 | u32PreReg);

            EPWM_EnableBrakeNoiseFilter(g_apEPWMModule[x], 1, 5, au32RegPattern[u32PatternIdx]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0x5100 | au32RegPattern[u32PatternIdx] << 9 | u32PreReg);
            EPWM_DisableBrakeNoiseFilter(g_apEPWMModule[x], 1);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0x5000 | au32RegPattern[u32PatternIdx] << 9 | u32PreReg);

            EPWM_EnableBrakeNoiseFilter(g_apEPWMModule[x], 1, 2, au32RegPattern[u32PatternIdx]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0x2100 | au32RegPattern[u32PatternIdx] << 9 | u32PreReg);
            EPWM_DisableBrakeNoiseFilter(g_apEPWMModule[x], 1);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0x2000 | au32RegPattern[u32PatternIdx] << 9 | u32PreReg);

            EPWM_EnableBrakeNoiseFilter(g_apEPWMModule[x], 1, 0, au32RegPattern[u32PatternIdx]);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0x100 | au32RegPattern[u32PatternIdx] << 9 | u32PreReg);
            EPWM_DisableBrakeNoiseFilter(g_apEPWMModule[x], 1);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0 | au32RegPattern[u32PatternIdx] << 9 | u32PreReg);
        }
    }
}

void Func_EPWM_EnableBrakePinInverse()
{
    uint32_t x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        EPWM_EnableBrakePinInverse(g_apEPWMModule[x], 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 1 << 7);
        EPWM_EnableBrakePinInverse(g_apEPWMModule[x], 1);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 1 << 7 | 1 << 15);
        EPWM_DisableBrakePinInverse(g_apEPWMModule[x], 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 1 << 15);
        EPWM_DisableBrakePinInverse(g_apEPWMModule[x], 1);
        CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0);
    }
}

void Func_EPWM_SetBrakePinSource()
{
    uint32_t u32TestCh = 0, x = 0;

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        //test case 1: enable/disable one channel each time, then check each action
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SetBrakePinSource(g_apEPWMModule[x], 0, TRUE);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 1 << 16);
            EPWM_SetBrakePinSource(g_apEPWMModule[x], 1, TRUE);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 1 << 16 | 1 << 24);
            EPWM_SetBrakePinSource(g_apEPWMModule[x], 0, FALSE);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 1 << 24);
            EPWM_SetBrakePinSource(g_apEPWMModule[x], 1, FALSE);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->BNF, 0);
        }
    }
}

void Func_EPWM_CaptureInputNoiseFilter()
{
    uint32_t x = 0;
    uint32_t u32TestCh = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableCaptureInputNoiseFilter(g_apEPWMModule[x], u32TestCh, 1, EPWM_NF_CLK_DIV_2);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPNF[u32TestCh], (EPWM_CAPNF_CAPNFEN_Msk | (1 << EPWM_CAPNF_CAPNFCNT_Pos) | (1 << EPWM_CAPNF_CAPNFSEL_Pos)));

            EPWM_EnableCaptureInputNoiseFilter(g_apEPWMModule[x], u32TestCh, 2, EPWM_NF_CLK_DIV_4);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPNF[u32TestCh], (EPWM_CAPNF_CAPNFEN_Msk | (2 << EPWM_CAPNF_CAPNFCNT_Pos) | (2 << EPWM_CAPNF_CAPNFSEL_Pos)));

            EPWM_EnableCaptureInputNoiseFilter(g_apEPWMModule[x], u32TestCh, 3, EPWM_NF_CLK_DIV_8);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPNF[u32TestCh], (EPWM_CAPNF_CAPNFEN_Msk | (3 << EPWM_CAPNF_CAPNFCNT_Pos) | (3 << EPWM_CAPNF_CAPNFSEL_Pos)));

            EPWM_EnableCaptureInputNoiseFilter(g_apEPWMModule[x], u32TestCh, 4, EPWM_NF_CLK_DIV_16);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPNF[u32TestCh], (EPWM_CAPNF_CAPNFEN_Msk | (4 << EPWM_CAPNF_CAPNFCNT_Pos) | (4 << EPWM_CAPNF_CAPNFSEL_Pos)));

            EPWM_EnableCaptureInputNoiseFilter(g_apEPWMModule[x], u32TestCh, 5, EPWM_NF_CLK_DIV_32);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPNF[u32TestCh], (EPWM_CAPNF_CAPNFEN_Msk | (5 << EPWM_CAPNF_CAPNFCNT_Pos) | (5 << EPWM_CAPNF_CAPNFSEL_Pos)));

            EPWM_EnableCaptureInputNoiseFilter(g_apEPWMModule[x], u32TestCh, 6, EPWM_NF_CLK_DIV_64);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPNF[u32TestCh], (EPWM_CAPNF_CAPNFEN_Msk | (6 << EPWM_CAPNF_CAPNFCNT_Pos) | (6 << EPWM_CAPNF_CAPNFSEL_Pos)));

            EPWM_EnableCaptureInputNoiseFilter(g_apEPWMModule[x], u32TestCh, 7, EPWM_NF_CLK_DIV_128);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPNF[u32TestCh], (EPWM_CAPNF_CAPNFEN_Msk | (7 << EPWM_CAPNF_CAPNFCNT_Pos) | (7 << EPWM_CAPNF_CAPNFSEL_Pos)));

            EPWM_EnableCaptureInputNoiseFilter(g_apEPWMModule[x], u32TestCh, 0, EPWM_NF_CLK_DIV_1);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPNF[u32TestCh], (EPWM_CAPNF_CAPNFEN_Msk | (0 << EPWM_CAPNF_CAPNFCNT_Pos) | (0 << EPWM_CAPNF_CAPNFSEL_Pos)));

            EPWM_DisableCaptureInputNoiseFilter(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->CAPNF[u32TestCh], 0x0);
        }
    }
}

void Func_EPWM_ExtEventTrigger()
{
    uint32_t x = 0;
    uint32_t u32TestCh = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableExtEventTrigger(g_apEPWMModule[x], u32TestCh, EPWM_EXT_TGR_PIN_INT1, EPWM_EXT_TGR_COUNTER_RESET);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->EXTETCTL[u32TestCh], (EPWM_EXTETCTL_EXTETEN_Msk | (1 << EPWM_EXTETCTL_EXTTRGS_Pos) | (0 << EPWM_EXTETCTL_CNTACTS_Pos)));

            EPWM_EnableExtEventTrigger(g_apEPWMModule[x], u32TestCh, EPWM_EXT_TGR_PIN_INT2, EPWM_EXT_TGR_COUNTER_START);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->EXTETCTL[u32TestCh], (EPWM_EXTETCTL_EXTETEN_Msk | (2 << EPWM_EXTETCTL_EXTTRGS_Pos) | (1 << EPWM_EXTETCTL_CNTACTS_Pos)));

            EPWM_EnableExtEventTrigger(g_apEPWMModule[x], u32TestCh, EPWM_EXT_TGR_PIN_INT3, EPWM_EXT_TGR_COUNTER_RESET_AND_START);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->EXTETCTL[u32TestCh], (EPWM_EXTETCTL_EXTETEN_Msk | (3 << EPWM_EXTETCTL_EXTTRGS_Pos) | (2 << EPWM_EXTETCTL_CNTACTS_Pos)));

            EPWM_EnableExtEventTrigger(g_apEPWMModule[x], u32TestCh, EPWM_EXT_TGR_PIN_INT4, EPWM_EXT_TGR_COUNTER_RESET);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->EXTETCTL[u32TestCh], (EPWM_EXTETCTL_EXTETEN_Msk | (4 << EPWM_EXTETCTL_EXTTRGS_Pos) | (0 << EPWM_EXTETCTL_CNTACTS_Pos)));

            EPWM_EnableExtEventTrigger(g_apEPWMModule[x], u32TestCh, EPWM_EXT_TGR_PIN_INT5, EPWM_EXT_TGR_COUNTER_START);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->EXTETCTL[u32TestCh], (EPWM_EXTETCTL_EXTETEN_Msk | (5 << EPWM_EXTETCTL_EXTTRGS_Pos) | (1 << EPWM_EXTETCTL_CNTACTS_Pos)));

            EPWM_EnableExtEventTrigger(g_apEPWMModule[x], u32TestCh, EPWM_EXT_TGR_PIN_INT6, EPWM_EXT_TGR_COUNTER_RESET_AND_START);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->EXTETCTL[u32TestCh], (EPWM_EXTETCTL_EXTETEN_Msk | (6 << EPWM_EXTETCTL_EXTTRGS_Pos) | (2 << EPWM_EXTETCTL_CNTACTS_Pos)));

            EPWM_EnableExtEventTrigger(g_apEPWMModule[x], u32TestCh, EPWM_EXT_TGR_PIN_INT7, EPWM_EXT_TGR_COUNTER_START);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->EXTETCTL[u32TestCh], (EPWM_EXTETCTL_EXTETEN_Msk | (7 << EPWM_EXTETCTL_EXTTRGS_Pos) | (1 << EPWM_EXTETCTL_CNTACTS_Pos)));

            EPWM_EnableExtEventTrigger(g_apEPWMModule[x], u32TestCh, EPWM_EXT_TGR_PIN_INT0, EPWM_EXT_TGR_COUNTER_RESET);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->EXTETCTL[u32TestCh], (EPWM_EXTETCTL_EXTETEN_Msk | (0 << EPWM_EXTETCTL_EXTTRGS_Pos) | (0 << EPWM_EXTETCTL_CNTACTS_Pos)));

            EPWM_DisableExtEventTrigger(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->EXTETCTL[u32TestCh], 0x0);
        }
    }
}

void Func_EPWM_SWEventOutput()
{
    uint32_t x = 0;
    uint32_t u32TestCh = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_EnableSWEventOutput(g_apEPWMModule[x], u32TestCh, EPWM_OUTPUT_LOW);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SWEOFTRG, 0x1 << (u32TestCh));
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SWEOFCTL, ((1UL) << (u32TestCh << 1U)));

            EPWM_EnableSWEventOutput(g_apEPWMModule[x], u32TestCh, EPWM_OUTPUT_HIGH);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SWEOFTRG, 0x1 << (u32TestCh));
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SWEOFCTL, ((2UL) << (u32TestCh << 1U)));

            EPWM_EnableSWEventOutput(g_apEPWMModule[x], u32TestCh, EPWM_OUTPUT_TOGGLE);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SWEOFTRG, 0x1 << (u32TestCh));
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SWEOFCTL, ((3UL) << (u32TestCh << 1U)));

            EPWM_EnableSWEventOutput(g_apEPWMModule[x], u32TestCh, EPWM_OUTPUT_NOTHING);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SWEOFTRG, 0x1 << (u32TestCh));
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SWEOFCTL, ((0UL) << (u32TestCh << 1U)));

            EPWM_DisableSWEventOutput(g_apEPWMModule[x], u32TestCh);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->SWEOFTRG, 0x0);
        }
    }
}

void Func_EPWM_GetWrapAroundFlag()
{
    uint32_t u32TestCh = 0, j = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (j = 0; j < EPWM_MODULE_NUM; j++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh++)
        {
            EPWM_SET_CNR(g_apEPWMModule[j], u32TestCh, 0xFFFF);
            EPWM_SET_CMR(g_apEPWMModule[j], u32TestCh, 5);
            EPWM_SET_PRESCALER(g_apEPWMModule[j], u32TestCh, 1);
            EPWM_Start(g_apEPWMModule[j], 1 << u32TestCh);

            while (EPWM_GetWrapAroundFlag(g_apEPWMModule[j], u32TestCh) == 0) {};

            CU_ASSERT_EQUAL(EPWM_GetWrapAroundFlag(g_apEPWMModule[j], u32TestCh), 1);

            EPWM_ForceStop(g_apEPWMModule[j], 1 << u32TestCh);

            EPWM_ClearWrapAroundFlag(g_apEPWMModule[j], u32TestCh);

            CU_ASSERT_EQUAL(g_apEPWMModule[j]->STATUS & ((1 << EPWM_STATUS_CNTMAXF0_Pos) << u32TestCh), 0);
        }
    }
}

void Func_EPWM_GetFaultBrakeIntFlag()
{
    uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        for (u32TestCh = 0; u32TestCh < EPWM_MAX_CH; u32TestCh += 2)
        {
            EPWM_TRIGGER_BRAKE(g_apEPWMModule[x], 1 << (u32TestCh >> 1), EPWM_FB_LEVEL);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTSTS1, 0x300 << u32TestCh);
            CU_ASSERT_EQUAL(EPWM_GetFaultBrakeIntFlag(g_apEPWMModule[x], EPWM_FB_LEVEL), 1);
            EPWM_ClearFaultBrakeIntFlag(g_apEPWMModule[x], EPWM_FB_LEVEL);
            CU_ASSERT_EQUAL(EPWM_GetFaultBrakeIntFlag(g_apEPWMModule[x], EPWM_FB_LEVEL), 0);

            EPWM_TRIGGER_BRAKE(g_apEPWMModule[x], 1 << (u32TestCh >> 1), EPWM_FB_EDGE);
            CU_ASSERT_EQUAL(g_apEPWMModule[x]->INTSTS1, 0x30003 << u32TestCh);
            CU_ASSERT_EQUAL(EPWM_GetFaultBrakeIntFlag(g_apEPWMModule[x], EPWM_FB_EDGE), 1);
            EPWM_ClearFaultBrakeIntFlag(g_apEPWMModule[x], EPWM_FB_EDGE);
            CU_ASSERT_EQUAL(EPWM_GetFaultBrakeIntFlag(g_apEPWMModule[x], EPWM_FB_EDGE), 0);
        }
    }
}

void Func_EPWM_TRIGGER_SYNC()
{
    //     uint32_t u32TestCh = 0, x = 0;

    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    //     for (x = 0; x < EPWM_MODULE_NUM; x++)
    {
        EPWM_TRIGGER_SYNC(g_apEPWMModule[0], 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[0]->STATUS, 0x600);
        CU_ASSERT_EQUAL(g_apEPWMModule[1]->STATUS, 0x700);
        g_apEPWMModule[0]->STATUS = 0x600;
        CU_ASSERT_EQUAL(g_apEPWMModule[0]->STATUS, 0);
        g_apEPWMModule[1]->STATUS = 0x700;
        CU_ASSERT_EQUAL(g_apEPWMModule[1]->STATUS, 0);

        EPWM_TRIGGER_SYNC(g_apEPWMModule[0], 2);
        CU_ASSERT_EQUAL(g_apEPWMModule[0]->STATUS, 0x400);
        CU_ASSERT_EQUAL(g_apEPWMModule[1]->STATUS, 0x700);
        g_apEPWMModule[0]->STATUS = 0x400;
        CU_ASSERT_EQUAL(g_apEPWMModule[0]->STATUS, 0);
        g_apEPWMModule[1]->STATUS = 0x700;
        CU_ASSERT_EQUAL(g_apEPWMModule[1]->STATUS, 0);

        EPWM_TRIGGER_SYNC(g_apEPWMModule[0], 4);
        CU_ASSERT_EQUAL(g_apEPWMModule[0]->STATUS, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[1]->STATUS, 0x700);
        g_apEPWMModule[0]->STATUS = 0;
        CU_ASSERT_EQUAL(g_apEPWMModule[0]->STATUS, 0);
        g_apEPWMModule[1]->STATUS = 0x700;
        CU_ASSERT_EQUAL(g_apEPWMModule[1]->STATUS, 0);

        EPWM_TRIGGER_SYNC(g_apEPWMModule[1], 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[0]->STATUS, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[1]->STATUS, 0x600);
        g_apEPWMModule[1]->STATUS = 0x600;
        CU_ASSERT_EQUAL(g_apEPWMModule[1]->STATUS, 0);

        EPWM_TRIGGER_SYNC(g_apEPWMModule[1], 2);
        CU_ASSERT_EQUAL(g_apEPWMModule[0]->STATUS, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[1]->STATUS, 0x400);
        g_apEPWMModule[1]->STATUS = 0x400;
        CU_ASSERT_EQUAL(g_apEPWMModule[1]->STATUS, 0);

        EPWM_TRIGGER_SYNC(g_apEPWMModule[1], 4);
        CU_ASSERT_EQUAL(g_apEPWMModule[0]->STATUS, 0);
        CU_ASSERT_EQUAL(g_apEPWMModule[1]->STATUS, 0);
        g_apEPWMModule[1]->STATUS = 0;
        CU_ASSERT_EQUAL(g_apEPWMModule[1]->STATUS, 0);
    }
}

void Func_EPWM_CONSTANT()
{
    /* Reset EPWM */
    SYS_ResetModule(SYS_EPWM0RST);
    SYS_ResetModule(SYS_EPWM1RST);

    EPWM0->CTL1 = EPWM_UP_DOWN_COUNTER;
    CU_ASSERT_EQUAL(EPWM0->CTL1, 2);

}

CU_SuiteInfo EPWMSuites[] =
{
    { "EPWM Const Test", EPWM_Test_Init, EPWM_Test_Clean, NULL, NULL, EPWM_ConstTest },
    { "EPWM Macro Test", EPWM_Test_Init, EPWM_Test_Clean, NULL, NULL, EPWM_MacroTest },
    { "EPWM Func  Test", EPWM_Test_Init, EPWM_Test_Clean, NULL, NULL, EPWM_FuncTest },
    CU_SUITE_INFO_NULL
};

CU_TestInfo  EPWM_ConstTest[] =
{
    {"Test EPWM_CONSTANT:", Func_EPWM_CONSTANT},
    CU_TEST_INFO_NULL
};

CU_TestInfo  EPWM_MacroTest[] =
{
    {"Test EPWM_ENABLE_OUTPUT_INVERTER():", Func_EPWM_ENABLE_OUTPUT_INVERTER},
    {"Test EPWM_SET_PRESCALER():", Func_EPWM_SET_PRESCALER},
    {"Test EPWM_SET_CMR()/EPWM_SET_CNR():", Func_EPWM_SET_CMR},
    {"Test EPWM_SET_ALIGNED_TYPE():", Func_EPWM_SET_ALIGNED_TYPE},
    {"Test EPWM_ENABLE_COMPLEMENTARY_MODE()/EPWM_DISABLE_COMPLEMENTARY_MODE():", Func_EPWM_ENABLE_COMPLEMENTARY_MODE},
    {"Test EPWM_ENABLE_GROUP_MODE()/EPWM_DISABLE_GROUP_MODE():", Func_EPWM_ENABLE_GROUP_MODE},
    {"Test EPWM_ENABLE_TIMER_SYNC()/EPWM_DISABLE_TIMER_SYNC():", Func_EPWM_ENABLE_TIMER_SYNC},
    {"Test EPWM_MASK_OUTPUT():", Func_EPWM_MASK_OUTPUT},
    {"Test EPWM_SET_FTCMR():", Func_EPWM_SET_FTCMR},
    {"Test EPWM_TRIGGER_SYNC():", Func_EPWM_TRIGGER_SYNC},
    {"Test EPWM_SET_LOAD_WINDOW():", Func_EPWM_SET_LOAD_WINDOW},
    {"Test EPWM_CLR_COUNTER():", Func_EPWM_CLR_COUNTER},
    {"Test EPWM_SET_OUTPUT_LEVEL():", Func_EPWM_SET_OUTPUT_LEVEL},
    {"Test EPWM_SET_DEADZONE_CLK_SRC():", Func_EPWM_SET_DEADZONE_CLK_SRC}, //This function is only supported in M45xD/M45xC.
    CU_TEST_INFO_NULL
};

CU_TestInfo  EPWM_FuncTest[] =
{
    {"Test EPWM_Start()/EPWM_ForceStop()/EPWM_Stop():", Func_EPWM_Stop},
    {"Test EPWM_EnableADCTrigger()/EPWM_DisableADCTrigger():", Func_EPWM_EnableADCTrigger},
    {"Test EPWM_EnableCapture()/EPWM_DisableCapture():", Func_EPWM_EnableCapture},
    {"Test EPWM_EnableOutput()/EPWM_DisableOutput():", Func_EPWM_EnableOutput},
    {"Test EPWM_EnablePDMA()/EPWM_DisablePDMA():", Func_EPWM_EnablePDMA},
    {"Test EPWM_EnableDeadZone()/EPWM_DisableDeadZone():", Func_EPWM_EnableDeadZone},
    {"Test EPWM_EnableCaptureInt()/EPWM_DisableCaptureInt():", Func_EPWM_EnableCaptureInt},
    {"Test EPWM_EnableDutyInt()/EPWM_DisableDutyInt():", Func_EPWM_EnableDutyInt},
    {"Test EPWM_EnablePeriodInt()/EPWM_DisablePeriodInt():", Func_EPWM_EnablePeriodInt},
    {"Test EPWM_GetADCTriggerFlag()/EPWM_ClearADCTriggerFlag():", Func_EPWM_GetADCTriggerFlag},
    {"Test EPWM_GetDutyIntFlag()/EPWM_ClearDutyIntFlag():", Func_EPWM_GetDutyIntFlag},
    {"Test EPWM_GetPeriodIntFlag()/EPWM_ClearPeriodIntFlag():", Func_EPWM_GetPeriodIntFlag},
    {"Test EPWM_GetCaptureIntFlag()/EPWM_ClearCaptureIntFlag()/EPWM_GET_CAPTURE_RISING_DATA()/EPWM_GET_CAPTURE_FALLING_DATA():", Func_EPWM_GetCaptureIntFlag},
    {"Test EPWM_ConfigOutputChannel():", Func_EPWM_ConfigOutputChannel},
    {"Test EPWM_ConfigCaptureChannel():", Func_EPWM_ConfigCaptureChannel},
    {"Test EPWM_EnableDACTrigger()/EPWM_DisableDACTrigger():", Func_EPWM_EnableDACTrigger},
    {"Test EPWM_GetDACTriggerFlag()/EPWM_ClearDACTriggerFlag():", Func_EPWM_GetDACTriggerFlag},
    {"Test EPWM_EnableFaultBrake:", Func_EPWM_EnableFaultBrake},
    {"Test EPWM_EnableFaultBrakeInt()/EPWM_DisableFaultBrakeInt():", Func_EPWM_EnableFaultBrakeInt},
    {"Test EPWM_GetFaultBrakeIntFlag()/EPWM_ClearFaultBrakeIntFlag()/EPWM_TRIGGER_BRAKE():", Func_EPWM_GetFaultBrakeIntFlag},
    {"Test EPWM_EnableZeroInt()/EPWM_DisableZeroInt():", Func_EPWM_EnableZeroInt},
    {"Test EPWM_GetZeroIntFlag()/EPWM_ClearZeroIntFlag():", Func_EPWM_GetZeroIntFlag},
    {"Test EPWM_EnableAcc()/EPWM_DisableAcc():", Func_EPWM_EnableAcc},
    {"Test EPWM_EnableAccInt()/EPWM_DisableAccInt():", Func_EPWM_EnableAccInt},
    {"Test EPWM_EnableAccInt()/EPWM_DisableAccInt():", Func_EPWM_EnableAccStopMode},
    {"Test EPWM_GetAccInt()/EPWM_ClearAccInt():", Func_EPWM_GetAccInt},
    {"Test EPWM_GetFTDutyIntFlag()/EPWM_ClearFTDutyIntFlag():", Func_EPWM_GetFTDutyIntFlag},
    {"Test EPWM_EnableLoadMode()/EPWM_DisableLoadMode():", Func_EPWM_EnableLoadMode},
    {"Test EPWM_ConfigSyncPhase():", Func_EPWM_ConfigSyncPhase},
    {"Test EPWM_EnableSyncPhase()/EPWM_DisableSyncPhase():", Func_EPWM_EnableSyncPhase},
    {"Test EPWM_EnableSyncNoiseFilter()/EPWM_DisableSyncNoiseFilter():", Func_EPWM_EnableSyncNoiseFilter},
    {"Test EPWM_EnableSyncPinInverse()/EPWM_DisableSyncPinInverse():", Func_EPWM_EnableSyncPinInverse},
    {"Test EPWM_SetClockSource():", Func_EPWM_SetClockSource},
    {"Test EPWM_EnableBrakeNoiseFilter()/EPWM_DisableBrakeNoiseFilter():", Func_EPWM_EnableBrakeNoiseFilter},
    {"Test EPWM_EnableBrakePinInverse()/EPWM_DisableBrakePinInverse():", Func_EPWM_EnableBrakePinInverse},
    {"Test EPWM_SetBrakePinSource():", Func_EPWM_SetBrakePinSource},
    {"Test EPWM_GetWrapAroundFlag()/EPWM_ClearWrapAroundFlag():", Func_EPWM_GetWrapAroundFlag},
    {"Test EPWM_EnableCaptureInputNoiseFilter()/EPWM_DisableCaptureInputNoiseFilter():", Func_EPWM_CaptureInputNoiseFilter},
    {"Test EPWM_EnableExtEventTrigger()/EPWM_DisableExtEventTrigger():", Func_EPWM_ExtEventTrigger},
    {"Test EPWM_EnableSWEventOutput()/EPWM_DisableSWEventOutput():", Func_EPWM_SWEventOutput},
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/