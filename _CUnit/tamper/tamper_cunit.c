/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.                                        */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   tamper_cunit.c                                                                                        */
/*            The test function of TAMPER for CUnit.                                                       */
/* Project:   M2354                                                                                        */
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
#include "tamper_cunit.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"


extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int suite_success_init(void)
{
    return 0;
}
int suite_success_clean(void)
{
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
    {"TAMPER CONSTANT & MACRO", suite_success_init, suite_success_clean, NULL, NULL, TAMPER_CONSTANT_MACRO},
    {"TAMPER API", suite_success_init, suite_success_clean, NULL, NULL, TAMPER_API},
    CU_SUITE_INFO_NULL
};

const uint32_t au32SeedChangeRate[] =
{
    TAMPER_2POW6_CLK,
    TAMPER_2POW7_CLK,
    TAMPER_2POW8_CLK,
    TAMPER_2POW9_CLK,
    TAMPER_2POW10_CLK,
    TAMPER_2POW11_CLK,
    TAMPER_2POW12_CLK,
    TAMPER_2POW13_CLK,
};

const uint32_t u32RefPattern[] =
{
    TAMPER_REF_RANDOM_PATTERN,
    TAMPER_REF_SEED,
};

void CONSTANT_TAMPER(void)
{
    volatile uint32_t i = 0;

    /* Check Snooper Detection Mode Constant;TAMPER_EnableSnooperDetection() */
    CU_ASSERT_EQUAL(TAMPER_MAX_TAMPER_PIN_NUM, 6);
    {
        uint32_t u32TamperSel = (TAMPER_TAMPER0_SELECT | TAMPER_TAMPER1_SELECT | TAMPER_TAMPER2_SELECT |
                                 TAMPER_TAMPER3_SELECT | TAMPER_TAMPER4_SELECT | TAMPER_TAMPER5_SELECT);
        uint32_t u32DetLevel = TAMPER_TAMPER_HIGH_LEVEL_DETECT;
        uint32_t u32Debounce = TAMPER_TAMPER_DEBOUNCE_ENABLE;

        TAMPER_IOSEL_TAMPER(u32TamperSel);

        TAMPER_StaticTamperDisable(u32TamperSel);

        TAMPER_StaticTamperEnable(u32TamperSel, u32DetLevel, u32Debounce);

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP0EN_Msk), (0x1 << 8));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP1EN_Msk), (0x1 << 12));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP2EN_Msk), (0x1 << 16));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP3EN_Msk), (0x1 << 20));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP4EN_Msk), (0x1 << 24));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP5EN_Msk), (0x1 << 28));

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP0LV_Msk), (0x1 << 9));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP1LV_Msk), (0x1 << 13));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP2LV_Msk), (0x1 << 17));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP3LV_Msk), (0x1 << 21));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP4LV_Msk), (0x1 << 25));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP5LV_Msk), (0x1 << 29));

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP0DBEN_Msk), (0x1 << 10));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP1DBEN_Msk), (0x1 << 14));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP2DBEN_Msk), (0x1 << 18));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP3DBEN_Msk), (0x1 << 22));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP4DBEN_Msk), (0x1 << 26));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP5DBEN_Msk), (0x1 << 30));

        TAMPER_StaticTamperDisable(u32TamperSel);

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP0EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP1EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP2EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP3EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP4EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP5EN_Msk), 0);

        u32DetLevel = TAMPER_TAMPER_LOW_LEVEL_DETECT;
        u32Debounce = TAMPER_TAMPER_DEBOUNCE_DISABLE;

        TAMPER_StaticTamperEnable(u32TamperSel, u32DetLevel, u32Debounce);

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP0LV_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP1LV_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP2LV_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP3LV_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP4LV_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP5LV_Msk), 0);

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP0DBEN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP1DBEN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP2DBEN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP3DBEN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP4DBEN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP5DBEN_Msk), 0);

        /* To check if TAMPER snooper pin interrupt occurred */
        if(TAMPER_GET_INT_FLAG() == 1)
        {
            printf("\n        * Get TAMPER FLAGs: 0x%x", TAMPER_GET_INT_STATUS());
            /* Clear TAMPER snooper pin interrupt flag */
            TAMPER_CLR_INT_STATUS(TAMPER_GET_INT_STATUS());
            CU_ASSERT_EQUAL(TAMPER_GET_INT_STATUS(), 0);
        }
        TAMPER_StaticTamperDisable(u32TamperSel);
    }

    CU_ASSERT_EQUAL(TAMPER_MAX_PAIR_NUM, 3);
    {
        uint32_t u32PairSel = (TAMPER_PAIR0_SELECT | TAMPER_PAIR1_SELECT | TAMPER_PAIR2_SELECT);
        uint32_t u32Pair1Source = 0;
        uint32_t u32Pair2Source = 0;
        uint32_t u32Debounce_ = TAMPER_TAMPER_DEBOUNCE_ENABLE;

        TAMPER_DynamicTamperEnable(u32PairSel, u32Debounce_, u32Pair1Source, u32Pair2Source);

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP0EN_Msk), (0x1 << 8));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP1EN_Msk), (0x1 << 12));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP2EN_Msk), (0x1 << 16));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP3EN_Msk), (0x1 << 20));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP4EN_Msk), (0x1 << 24));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP5EN_Msk), (0x1 << 28));

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP0DBEN_Msk), (0x1 << 10));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP1DBEN_Msk), (0x1 << 14));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP2DBEN_Msk), (0x1 << 18));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP3DBEN_Msk), (0x1 << 22));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP4DBEN_Msk), (0x1 << 26));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP5DBEN_Msk), (0x1 << 30));

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYNPR0EN_Msk), (0x1 << 15));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYNPR1EN_Msk), (0x1 << 23));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYNPR2EN_Msk), (0x1 << 31));

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYN1ISS_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYN2ISS_Msk), 0);

        /* To check if TAMPER snooper pin interrupt occurred */
        if(TAMPER_GET_INT_FLAG() == 1)
        {
            printf("\n        * Get TAMPER FLAGs: 0x%x", TAMPER_GET_INT_STATUS());
            /* Clear TAMPER snooper pin interrupt flag */
            TAMPER_CLR_INT_STATUS(TAMPER_GET_INT_STATUS());
            CU_ASSERT_EQUAL(TAMPER_GET_INT_STATUS(), 0);
        }
        TAMPER_DynamicTamperDisable(u32PairSel);

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP0EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP1EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP2EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP3EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP4EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP5EN_Msk), 0);

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYNPR0EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYNPR1EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYNPR2EN_Msk), 0);

        u32Pair1Source = 1;
        u32Pair2Source = 1;
        u32Debounce_ = TAMPER_TAMPER_DEBOUNCE_DISABLE;

        TAMPER_DynamicTamperEnable(u32PairSel, u32Debounce_, u32Pair1Source, u32Pair2Source);

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP0EN_Msk), (0x1 << 8));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP1EN_Msk), (0x1 << 12));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP2EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP3EN_Msk), (0x1 << 20));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP4EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP5EN_Msk), (0x1 << 28));

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP0DBEN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP1DBEN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP2DBEN_Msk), (0x1 << 18));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP3DBEN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP4DBEN_Msk), (0x1 << 26));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_TAMP5DBEN_Msk), 0);

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYNPR0EN_Msk), (0x1 << 15));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYNPR1EN_Msk), (0x1 << 23));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYNPR2EN_Msk), (0x1 << 31));

        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYN1ISS_Msk), (0x1 << 0));
        CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYN2ISS_Msk), (0x1 << 1));
    }

    {
        for(i = 0; i < 8; i++)
        {
            TAMPER_DynamicTamperConfig(au32SeedChangeRate[i], 0, 0, 0);

            CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYNRATE_Msk), (i << 5));
        }

#if 0 /* seed reload bit will auto-clear, can not check this bit */
        for(i = 0; i < 2; i++)
        {
            TAMPER_DynamicTamperConfig(au32SeedChangeRate[0], i, 0, 0);

            CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_SEEDRLD_Msk), (0x0 << 4));
        }
#endif

        for(i = 0; i < 2; i++)
        {
            TAMPER_DynamicTamperConfig(au32SeedChangeRate[0], 0, u32RefPattern[i], 0);

            if(u32RefPattern[i] == TAMPER_REF_SEED)
            {
                CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYNSRC_Msk), (0x1 << 3));
            }

            if(u32RefPattern[i] == TAMPER_REF_RANDOM_PATTERN)
            {
                CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_DYNSRC_Msk), (0x0 << 3));
            }
        }

        for(i = 0; i < 5; i++)
        {
            TAMPER_DynamicTamperConfig(au32SeedChangeRate[0], 0, 0, (i * 5));

            CU_ASSERT_EQUAL((TAMPER->SEED & TAMPER_SEED_SEED_Msk), (i * 5));
        }
    }

    /* Check Active Shield */
    /* has not support     */
#if 0    
    CU_ASSERT_EQUAL(TAMPER_MAX_PAIR_NUM, 3);
    {
        uint32_t u32PairSel1 = (TAMPER_PAIR0_SELECT | TAMPER_PAIR1_SELECT | TAMPER_PAIR2_SELECT);
        uint32_t u32Pair1Source1 = 0;
        uint32_t u32PairSel2 = (TAMPER_PAIR0_SELECT | TAMPER_PAIR1_SELECT | TAMPER_PAIR2_SELECT);
        uint32_t u32Pair1Source2 = 0;

        TAMPER_ActiveShieldDynamicTamperEnable(u32PairSel1, u32Pair1Source1, u32PairSel2, u32Pair1Source2);

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP0EN_Msk), (0x1 << 8));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP1EN_Msk), (0x1 << 12));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP2EN_Msk), (0x1 << 16));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP3EN_Msk), (0x1 << 20));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP4EN_Msk), (0x1 << 24));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP5EN_Msk), (0x1 << 28));

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ADYNPR0EN_Msk), (0x1 << 15));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ADYNPR1EN_Msk), (0x1 << 23));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ADYNPR2EN_Msk), (0x1 << 31));

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ADYN1ISS_Msk), 0);

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP0EN2_Msk), (0x1 << 8));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP1EN2_Msk), (0x1 << 12));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP2EN2_Msk), (0x1 << 16));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP3EN2_Msk), (0x1 << 20));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP4EN2_Msk), (0x1 << 24));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP5EN2_Msk), (0x1 << 28));

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ADYNPR0EN2_Msk), (0x1 << 15));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ADYNPR1EN2_Msk), (0x1 << 23));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ADYNPR2EN2_Msk), (0x1 << 31));

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ADYN1ISS2_Msk), 0);

        /* To check if TAMPER snooper pin interrupt occurred */
        if(TAMPER_GET_INT_FLAG() == 1)
        {
            printf("\n        * Get TAMPER FLAGs: 0x%x", TAMPER_GET_INT_STATUS());
            /* Clear TAMPER snooper pin interrupt flag */
            TAMPER_CLR_INT_STATUS(TAMPER_GET_INT_STATUS());
            CU_ASSERT_EQUAL(TAMPER_GET_INT_STATUS(), 0);
        }
        TAMPER_ActiveShieldDynamicTamperDisable(u32PairSel1, u32PairSel2);

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP0EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP1EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP2EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP3EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP4EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP5EN_Msk), 0);

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ADYNPR0EN_Msk), (0x1 << 15)); // keep high
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ADYNPR1EN_Msk), (0x1 << 23)); // keep high
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ADYNPR2EN_Msk), (0x1 << 31)); // keep high

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP0EN2_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP1EN2_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP2EN2_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP3EN2_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP4EN2_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP5EN2_Msk), 0);

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ADYNPR0EN2_Msk), (0x1 << 15)); // keep high
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ADYNPR1EN2_Msk), (0x1 << 23)); // keep high
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ADYNPR2EN2_Msk), (0x1 << 31)); // keep high

        u32Pair1Source1 = 1;
        u32Pair1Source2 = 1;

        TAMPER_ActiveShieldDynamicTamperEnable(u32PairSel1, u32Pair1Source1, u32PairSel2, u32Pair1Source2);

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP0EN_Msk), (0x1 << 8));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP1EN_Msk), (0x1 << 12));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP2EN_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP3EN_Msk), (0x1 << 20));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP4EN_Msk), (0x1 << 24));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ATAMP5EN_Msk), (0x1 << 28));

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ADYNPR0EN_Msk), (0x1 << 15));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ADYNPR1EN_Msk), (0x1 << 23));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ADYNPR2EN_Msk), (0x1 << 31));

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ADYN1ISS_Msk), (0x1 << 0));

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP0EN2_Msk), (0x1 << 8));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP1EN2_Msk), (0x1 << 12));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP2EN2_Msk), 0);
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP3EN2_Msk), (0x1 << 20));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP4EN2_Msk), (0x1 << 24));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ATAMP5EN2_Msk), (0x1 << 28));

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ADYNPR0EN2_Msk), (0x1 << 15));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ADYNPR1EN2_Msk), (0x1 << 23));
        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ADYNPR2EN2_Msk), (0x1 << 31));

        CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ADYN1ISS2_Msk), (0x1 << 0));
    }

    {
        for(i = 0; i < 8; i++)
        {
            TAMPER_ActiveShieldDynamicTamperConfig(au32ACTSSeedChangeRate[i], 0, 0, 0, au32ACTSSeedChangeRate[i], 0, 0, 0);

            CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ADYNRATE_Msk), (i << 5));
            CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ADYNRATE2_Msk), (i << 5));
        }

#if 0 /* seed reload bit will auto-clear, can not check this bit */
        for(i = 0; i < 2; i++)
        {
            TAMPER_ActiveShieldDynamicTamperConfig(au32ACTSSeedChangeRate[0], i, 0, 0, au32ACTSSeedChangeRate[0], i, 0, 0);

            CU_ASSERT_EQUAL((TAMPER->TIOCTL & TAMPER_TIOCTL_SEEDRLD_Msk), (0x0 << 4));
            CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_SEEDRLD2_Msk), (0x0 << 4));
        }
#endif

        for(i = 0; i < 2; i++)
        {
            TAMPER_ActiveShieldDynamicTamperConfig(au32ACTSSeedChangeRate[0], 0, u32RefPattern[i], 0, au32ACTSSeedChangeRate[0], 0, u32RefPattern[i], 0);

            if(u32RefPattern[i] == TAMPER_REF_SEED)
            {
                CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ADYNSRC_Msk), (0x1 << 3));
                CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ADYNSRC2_Msk), (0x1 << 3));
            }

            if(u32RefPattern[i] == TAMPER_REF_RANDOM_PATTERN)
            {
                CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL1 & TAMPER_ACTSTIOCTL1_ADYNSRC_Msk), (0x0 << 3));
                CU_ASSERT_EQUAL((TAMPER->ACTSTIOCTL2 & TAMPER_ACTSTIOCTL2_ADYNSRC2_Msk), (0x0 << 3));
            }
        }

        for(i = 0; i < 5; i++)
        {
            TAMPER_ActiveShieldDynamicTamperConfig(au32ACTSSeedChangeRate[0], 0, 0, (i * 5), au32ACTSSeedChangeRate[0], 0, 0, (i * 5));

            CU_ASSERT_EQUAL((TAMPER->SEED & TAMPER_SEED_SEED_Msk), (i * 5));
            CU_ASSERT_EQUAL((TAMPER->SEED2 & TAMPER_SEED2_SEED2_Msk), (i * 5));
        }
    }
#endif    
}

void MACRO_TAMPER(void)
{
    /* Reset/Release Tamper Coreblock */
    TAMPER->FUNEN = 0x44;
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_LXTDETEN_Msk) == TAMPER_FUNEN_LXTDETEN_Msk);
    TAMPER_CORE_RESET();
    CU_ASSERT((TAMPER->INIT & TAMPER_INIT_TCORERST_Msk) == TAMPER_INIT_TCORERST_Msk);
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_LXTDETEN_Msk) == 0x0);
    TAMPER_CORE_RELEASE();
    CU_ASSERT((TAMPER->INIT & TAMPER_INIT_TCORERST_Msk) == 0x0);
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_LXTDETEN_Msk) == 0x0);

    /* Get the TLDO Power Ready Status */
    CU_ASSERT(TAMPER_TLDO_IS_READY() == 1);
    CU_ASSERT((TAMPER->INIT & TAMPER_INIT_TLDORDY_Msk) == TAMPER_INIT_TLDORDY_Msk);

    /* Enable/Disable LXT Clock Detection */
    TAMPER_ENABLE_LXTDET();
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_LXTDETEN_Msk) == TAMPER_FUNEN_LXTDETEN_Msk);
    TAMPER_DISABLE_LXTDET();
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_LXTDETEN_Msk) == 0x0);

/* M55 has not support */
//    /* Enable/Disable HXT Clock Detection */
//    TAMPER_ENABLE_HXTDET();
//    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_HXTDETEN_Msk) == TAMPER_FUNEN_HXTDETEN_Msk);
//    TAMPER_DISABLE_HXTDET();
//    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_HXTDETEN_Msk) == 0x0);

    /* Tamper I/O TAMPER/RTC Block Detection Selection */
    TAMPER_IOSEL_TAMPER(TAMPER_TAMPER0_SELECT);
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_TMPIOSEL_Msk) == 0x100);
    TAMPER_IOSEL_RTC(TAMPER_TAMPER0_SELECT);
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_TMPIOSEL_Msk) == 0x0);
    TAMPER_IOSEL_TAMPER(TAMPER_TAMPER1_SELECT);
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_TMPIOSEL_Msk) == 0x200);
    TAMPER_IOSEL_RTC(TAMPER_TAMPER1_SELECT);
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_TMPIOSEL_Msk) == 0x0);
    TAMPER_IOSEL_TAMPER(TAMPER_TAMPER2_SELECT);
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_TMPIOSEL_Msk) == 0x400);
    TAMPER_IOSEL_RTC(TAMPER_TAMPER2_SELECT);
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_TMPIOSEL_Msk) == 0x0);
    TAMPER_IOSEL_TAMPER(TAMPER_TAMPER3_SELECT);
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_TMPIOSEL_Msk) == 0x800);
    TAMPER_IOSEL_RTC(TAMPER_TAMPER3_SELECT);
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_TMPIOSEL_Msk) == 0x0);
    TAMPER_IOSEL_TAMPER(TAMPER_TAMPER4_SELECT);
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_TMPIOSEL_Msk) == 0x1000);
    TAMPER_IOSEL_RTC(TAMPER_TAMPER4_SELECT);
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_TMPIOSEL_Msk) == 0x0);
    TAMPER_IOSEL_TAMPER(TAMPER_TAMPER5_SELECT);
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_TMPIOSEL_Msk) == 0x2000);
    TAMPER_IOSEL_RTC(TAMPER_TAMPER5_SELECT);
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_TMPIOSEL_Msk) == 0x0);

    /* Enable/Disable HIRC48M */
    TAMPER_ENABLE_HIRC48M();
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_HIRC48MEN_Msk) == 0x0);
    TAMPER_DISABLE_HIRC48M();
    CU_ASSERT((TAMPER->FUNEN & TAMPER_FUNEN_HIRC48MEN_Msk) == 0x5A0000);

    /* Power Glitch Sampling Rate Selectionh */
    TAMPER_VG_SAMPLE_SEL(TAMPER_VG_96M_SAMPLE);
    CU_ASSERT((TAMPER->FUNEN & 0xF000000) == 0x3000000);
    TAMPER_VG_SAMPLE_SEL(TAMPER_VG_192M_SAMPLE);
    CU_ASSERT((TAMPER->FUNEN & 0xF000000) == 0xF000000);
    TAMPER_VG_SAMPLE_SEL(TAMPER_VG_96M_SAMPLE);
    CU_ASSERT((TAMPER->FUNEN & 0xF000000) == 0x3000000);
    TAMPER_VG_SAMPLE_SEL(TAMPER_VG_48M_SAMPLE);
    CU_ASSERT((TAMPER->FUNEN & 0xF000000) == 0x1000000);

    /* Enable/Disable Trigger Test */
//    TAMPER_ENABLE_FMC_TRIG();
//    CU_ASSERT((TAMPER->TRIEN & 0x3F) == TAMPER_TRIEN_FMCTRIGEN_Msk);
//    TAMPER_DISABLE_FMC_TRIG();
    CU_ASSERT((TAMPER->TRIEN & 0x3F) == 0x0);
    TAMPER_ENABLE_KS_TRIG();
    CU_ASSERT((TAMPER->TRIEN & 0x3F) == TAMPER_TRIEN_KSTRIGEN_Msk);
    TAMPER_DISABLE_KS_TRIG();
    CU_ASSERT((TAMPER->TRIEN & 0x3F) == 0x0);
    TAMPER_ENABLE_WAKEUP();
    CU_ASSERT((TAMPER->TRIEN & 0x3F) == TAMPER_TRIEN_WAKEUPEN_Msk);
    TAMPER_DISABLE_WAKEUP();
    CU_ASSERT((TAMPER->TRIEN & 0x3F) == 0x0);
    TAMPER_ENABLE_CRYPTO();
    CU_ASSERT((TAMPER->TRIEN & 0x3F) == TAMPER_TRIEN_CRYPTOEN_Msk);
    TAMPER_DISABLE_CRYPTO();
    CU_ASSERT((TAMPER->TRIEN & 0x3F) == 0x0);
    TAMPER_ENABLE_CHIPRST();
    CU_ASSERT((TAMPER->TRIEN & 0x3F) == TAMPER_TRIEN_CHIPRSTEN_Msk);
    TAMPER_DISABLE_CHIPRST();
    CU_ASSERT((TAMPER->TRIEN & 0x3F) == 0x0);
    TAMPER_ENABLE_RTCSPCLR();
    CU_ASSERT((TAMPER->TRIEN & 0x3F) == TAMPER_TRIEN_RTCSPCLREN_Msk);
    TAMPER_DISABLE_RTCSPCLR();
    CU_ASSERT((TAMPER->TRIEN & 0x3F) == 0x0);
}

void API_TAMPER(void)
{
    volatile uint32_t i = 0, j = 0;
    volatile uint32_t u32Value, u32Timeout;

    /* Check TAMPER_EnableInt(), TAMPER_DisableInt() */
    TAMPER_EnableInt(TAMPER_INTEN_TAMP0IEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_TAMP0IEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_TAMP0IEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_TAMP1IEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_TAMP1IEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_TAMP1IEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_TAMP2IEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_TAMP2IEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_TAMP2IEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_TAMP3IEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_TAMP3IEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_TAMP3IEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_TAMP4IEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_TAMP4IEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_TAMP4IEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_TAMP5IEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_TAMP5IEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_TAMP5IEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_CLKFIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_CLKFIEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_CLKFIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_CLKSTOPIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_CLKSTOPIEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_CLKSTOPIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_OVPIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_OVPIEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_OVPIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_VGPIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_VGPIEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_VGPIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_VGNIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_VGNIEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_VGNIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_RTCLVRIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_RTCLVRIEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_RTCLVRIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_RTCIOIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_RTCIOIEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_RTCIOIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_RTCLKIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_RTCLKIEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_RTCLKIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_VLOSSIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_VLOSSIEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_VLOSSIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_WDTIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_WDTIEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_WDTIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
    TAMPER_EnableInt(TAMPER_INTEN_BODIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, TAMPER_INTEN_BODIEN_Msk);
    TAMPER_DisableInt(TAMPER_INTEN_BODIEN_Msk);
    CU_ASSERT_EQUAL(TAMPER->INTEN, 0);
}

CU_TestInfo TAMPER_CONSTANT_MACRO[] =
{
    {"Check TAMPER CONSTANT ", CONSTANT_TAMPER},
    {"Check TAMPER MACRO ",    MACRO_TAMPER},
    CU_TEST_INFO_NULL
};

CU_TestInfo TAMPER_API[] =
{
    {"Check TAMPER API ",    API_TAMPER},
    CU_TEST_INFO_NULL
};
