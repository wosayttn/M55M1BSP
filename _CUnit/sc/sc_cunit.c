/******************************************************************************
* @file    sc_cunit.c
* @version V1.00
* @brief   sc CUnit Test
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "sc_cunit.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

//#define D_msg   printf
#define D_msg(...)


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
    {"SC MACRO & CONSTANT", suite_success_init, suite_success_clean, NULL, NULL, SC_MACRO_CONSTANT},
    {"SC API", suite_success_init, suite_success_clean, NULL, NULL, SC_API},
    CU_SUITE_INFO_NULL
};

SC_T *pSC[SC_INTERFACE_NUM] =
{
    SC0,
    SC1,
    SC2,
};

const uint32_t au32TMROPMode[] =
{
    SC_TMR_MODE_0,
    SC_TMR_MODE_1,
    SC_TMR_MODE_2,
    SC_TMR_MODE_3,
    SC_TMR_MODE_4,
    SC_TMR_MODE_5,
    SC_TMR_MODE_6,
    SC_TMR_MODE_7,
    SC_TMR_MODE_8,
    SC_TMR_MODE_F,
};

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
void NSC_SYS_ResetModule(uint32_t u32ModuleIndex)
{
    SYS_ResetModule(u32ModuleIndex);
}
#else
extern void NSC_SYS_ResetModule(uint32_t u32ModuleIndex);
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
int32_t SC_InitClock(void)
{
    volatile uint32_t u32Timeout = SystemCoreClock / 1000;

    /* Enable HXT */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Waiting for clock ready */
    CLK_SetModuleClock(SC0_MODULE, CLK_SCSEL_SC0SEL_HIRC, CLK_SCDIV_SC0DIV(12));
    CLK_SetModuleClock(SC1_MODULE, CLK_SCSEL_SC1SEL_HIRC, CLK_SCDIV_SC1DIV(12));
    CLK_SetModuleClock(SC2_MODULE, CLK_SCSEL_SC2SEL_HIRC, CLK_SCDIV_SC2DIV(12));

    CLK_EnableModuleClock(SC0_MODULE);
    CLK_EnableModuleClock(SC1_MODULE);
    CLK_EnableModuleClock(SC2_MODULE);


    SYS_ResetModule(SYS_SC0RST);
    SYS_ResetModule(SYS_SC1RST);
    SYS_ResetModule(SYS_SC2RST);


    return 0;
}
#else
extern int32_t SC_InitClock(void);
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)  /* Pre-defined in TrustZone-Secure */
    __attribute__((cmse_nonsecure_entry))
#endif
#if !defined (TRUSTZONE_NON_SECURE)                             /* Pre-defined in TrustZone-NonSecure */
void SCModule_DeInit(void)
{
    uint32_t port;
    //    uint32_t NS_idx;
    //
    //    if((__PC()&NS_OFFSET) == NS_OFFSET)
    //        NS_idx = 3;
    //    else
    //        NS_idx = 0;
    CLK_SetModuleClock(SC0_MODULE, CLK_SCSEL_SC0SEL_PCLK1, CLK_SCDIV_SC0DIV(12));
    CLK_SetModuleClock(SC1_MODULE, CLK_SCSEL_SC1SEL_PCLK3, CLK_SCDIV_SC1DIV(12));
    CLK_SetModuleClock(SC2_MODULE, CLK_SCSEL_SC2SEL_PCLK1, CLK_SCDIV_SC2DIV(12));

    for (port = 0; port < SC_INTERFACE_NUM; port++)
    {
        if ((port == 0) || (port == 2))
        {
            if (SC_GetInterfaceClock(pSC[port]) != ((CLK_GetPCLK1Freq() / 12) / 1000))
            {
                //printf("port%d,%d,%d\n",port,SC_GetInterfaceClock(pSC[port]),((CLK_GetPCLK1Freq()/12)/1000));
                printf("\nL: %d failed.\n", __LINE__);

                while (1) {}
            }
        }
        else
        {
            if (SC_GetInterfaceClock(pSC[port]) != ((CLK_GetPCLK3Freq() / 12) / 1000))
            {
                //printf("port%d,%d,%d\n",port,SC_GetInterfaceClock(pSC[port]),((CLK_GetPCLK3Freq()/12)/1000));
                printf("\nL: %d failed.\n", __LINE__);

                while (1) {}
            }
        }
    }

    CLK_SetModuleClock(SC0_MODULE, CLK_SCSEL_SC0SEL_HIRC48M_DIV4, CLK_SCDIV_SC0DIV(12));
    CLK_SetModuleClock(SC1_MODULE, CLK_SCSEL_SC1SEL_HIRC48M_DIV4, CLK_SCDIV_SC1DIV(12));
    CLK_SetModuleClock(SC2_MODULE, CLK_SCSEL_SC2SEL_HIRC48M_DIV4, CLK_SCDIV_SC2DIV(12));

    for (port = 0; port < SC_INTERFACE_NUM; port++)
    {
        if (SC_GetInterfaceClock(pSC[port]) != 1000)
        {
            printf("\nL: %d failed.\n", __LINE__);

            while (1) {}
        }
    }

    /* Reset all SC */
    SYS_ResetModule(SYS_SC0RST);
    SYS_ResetModule(SYS_SC1RST);
    SYS_ResetModule(SYS_SC2RST);
}
#else
extern void SCModule_DeInit(void);
#endif

void MACRO_CONSTANT_Test(void)
{
    uint32_t port;
    uint32_t NS_idx;

    if (SC_InitClock() != 0)
    {
        CU_FAIL("SC Init Clock FAIL");
        return ;
    }

#if 0

    if ((__PC()&NS_OFFSET) == NS_OFFSET)
        NS_idx = 3;
    else
        NS_idx = 0;

#else
    NS_idx = 0;
#endif

    for (port = 0; port < SC_INTERFACE_NUM; port++)
    {
        /* Check SC_ENABLE_INT(), SC_DISABLE_INT */
        SC_ENABLE_INT(pSC[port + NS_idx], (SC_INTEN_ACERRIEN_Msk | SC_INTEN_RXTOIEN_Msk | SC_INTEN_INITIEN_Msk | SC_INTEN_CDIEN_Msk));
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->INTEN, 0x780);
        SC_DISABLE_INT(pSC[port + NS_idx], (SC_INTEN_ACERRIEN_Msk | SC_INTEN_RXTOIEN_Msk | SC_INTEN_INITIEN_Msk | SC_INTEN_CDIEN_Msk));
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->INTEN, 0x0);

        /* SC_SET_ETUDIV() */
        SC_SET_ETUDIV(pSC[port + NS_idx], 11);
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->ETUCTL, 0x0B);
        SC_SET_ETUDIV(pSC[port + NS_idx], 371);
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->ETUCTL, 0x173);

        /* Check SC_SET_VCC_PIN() */
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->PINCTL & SC_PINCTL_PWREN_Msk), 0x0);
        SC_SET_VCC_PIN(pSC[port + NS_idx], SC_PIN_STATE_HIGH);
        CLK_SysTickDelay(100);
        CU_ASSERT_NOT_EQUAL((pSC[port + NS_idx]->PINCTL & SC_PINCTL_PWREN_Msk), 0x0);
        SC_SET_VCC_PIN(pSC[port + NS_idx], SC_PIN_STATE_LOW);
        CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->PINCTL & SC_PINCTL_PWREN_Msk), 0x0);

        /* Check SC_SET_CLK_PIN() */
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->PINCTL & SC_PINCTL_CLKKEEP_Msk), 0x0);
        SC_SET_CLK_PIN(pSC[port + NS_idx], SC_CLK_ON);
        CLK_SysTickDelay(100);
        CU_ASSERT_NOT_EQUAL((pSC[port + NS_idx]->PINCTL & SC_PINCTL_CLKKEEP_Msk), 0x0);
        SC_SET_CLK_PIN(pSC[port + NS_idx], SC_CLK_OFF);
        CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->PINCTL & SC_PINCTL_CLKKEEP_Msk), 0x0);

        /* Check SC_SET_IO_PIN() */
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->PINCTL & SC_PINCTL_SCDATA_Msk), 0x0);
        SC_SET_IO_PIN(pSC[port + NS_idx], SC_PIN_STATE_HIGH);
        CLK_SysTickDelay(100);
        CU_ASSERT_NOT_EQUAL((pSC[port + NS_idx]->PINCTL & SC_PINCTL_SCDATA_Msk), 0x0);
        SC_SET_IO_PIN(pSC[port + NS_idx], SC_PIN_STATE_LOW);
        CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->PINCTL & SC_PINCTL_SCDATA_Msk), 0x0);

        /* Check SC_SET_RST_PIN() */
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->PINCTL & SC_PINCTL_RSTEN_Msk), 0x0);
        SC_SET_RST_PIN(pSC[port + NS_idx], SC_PIN_STATE_HIGH);
        CLK_SysTickDelay(100);
        CU_ASSERT_NOT_EQUAL((pSC[port + NS_idx]->PINCTL & SC_PINCTL_RSTEN_Msk), 0x0);
        SC_SET_RST_PIN(pSC[port + NS_idx], SC_PIN_STATE_LOW);
        CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->PINCTL & SC_PINCTL_RSTEN_Msk), 0x0);

        /* Check SC_SET_STOP_BIT_LEN() */
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->CTL & SC_CTL_NSB_Msk), 0x0);
        SC_SET_STOP_BIT_LEN(pSC[port + NS_idx], 1);
        CLK_SysTickDelay(100);
        CU_ASSERT_NOT_EQUAL((pSC[port + NS_idx]->CTL & SC_CTL_NSB_Msk), 0x0);
        SC_SET_STOP_BIT_LEN(pSC[port + NS_idx], 2);
        CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->CTL & SC_CTL_NSB_Msk), 0x0);

        /* Check SC_SET_STOP_BIT_LEN() */
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->CTL & SC_CTL_NSB_Msk), 0x0);
        SC_SET_STOP_BIT_LEN(pSC[port + NS_idx], 1);
        CLK_SysTickDelay(100);
        CU_ASSERT_NOT_EQUAL((pSC[port + NS_idx]->CTL & SC_CTL_NSB_Msk), 0x0);
        SC_SET_STOP_BIT_LEN(pSC[port + NS_idx], 2);
        CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->CTL & SC_CTL_NSB_Msk), 0x0);

        /* Check SC_SetTxRetry() */
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->CTL & (SC_CTL_TXRTYEN_Msk | SC_CTL_TXRTY_Msk)), 0x0);
        SC_SetTxRetry(pSC[port + NS_idx], 5);

        while (pSC[port + NS_idx]->CTL & SC_CTL_SYNC_Msk) {};

        CU_ASSERT_EQUAL((pSC[port + NS_idx]->CTL & (SC_CTL_TXRTYEN_Msk | SC_CTL_TXRTY_Msk)), 0x00C00000);

        SC_SetTxRetry(pSC[port + NS_idx], 0);

        while (pSC[port + NS_idx]->CTL & SC_CTL_SYNC_Msk) {}

        CU_ASSERT_EQUAL((pSC[port + NS_idx]->CTL & (SC_CTL_TXRTYEN_Msk | SC_CTL_TXRTY_Msk)), 0x0);

        /* Check SC_SetRxRetry() */
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->CTL & (SC_CTL_RXRTYEN_Msk | SC_CTL_RXRTY_Msk)), 0x0);
        SC_SetRxRetry(pSC[port + NS_idx], 4);

        while (pSC[port + NS_idx]->CTL & SC_CTL_SYNC_Msk) {};

        CU_ASSERT_EQUAL((pSC[port + NS_idx]->CTL & (SC_CTL_RXRTYEN_Msk | SC_CTL_RXRTY_Msk)), 0x000B0000);

        SC_SetRxRetry(pSC[port + NS_idx], 0);

        while (pSC[port + NS_idx]->CTL & SC_CTL_SYNC_Msk) {};

        CU_ASSERT_EQUAL((pSC[port + NS_idx]->CTL & (SC_CTL_RXRTYEN_Msk | SC_CTL_RXRTY_Msk)), 0x0);

        /* Check SC_WRITE(), SC_READ() by INTERNAL Loop Back Test */
        SC_Open(pSC[port + NS_idx], SC_PIN_STATE_IGNORE, SC_PIN_STATE_HIGH);

        CLK_SysTickDelay(100);

        pSC[port + NS_idx]->PINCTL |= BIT31;

        CLK_SysTickDelay(100);

        SC_WRITE(pSC[port + NS_idx], 0x5A);

        while ((pSC[port + NS_idx]->STATUS & SC_STATUS_RXEMPTY_Msk) == SC_STATUS_RXEMPTY_Msk) {};

        CU_ASSERT_EQUAL(SC_READ(pSC[port + NS_idx]), 0x5A);

        SC_WRITE(pSC[port + NS_idx], 0x25);

        while ((pSC[port + NS_idx]->STATUS & SC_STATUS_RXEMPTY_Msk) == SC_STATUS_RXEMPTY_Msk) {};

        CU_ASSERT_EQUAL(SC_READ(pSC[port + NS_idx]), 0x25);

        pSC[port + NS_idx]->PINCTL &= ~BIT31;

        CLK_SysTickDelay(100);

        SC_Close(pSC[port + NS_idx]);

        CLK_SysTickDelay(100);
    }

    /* Reset all SC */
    SYS_ResetModule(SYS_SC0RST);
    SYS_ResetModule(SYS_SC1RST);
    SYS_ResetModule(SYS_SC2RST);
}

void API_SC_Test(void)
{
    uint32_t i, j, port;
    uint32_t NS_idx;

    if (SC_InitClock() != 0)
    {
        CU_FAIL("SC Init Clock FAIL");
        return ;
    }

#if 0

    if ((__PC()&NS_OFFSET) == NS_OFFSET)
        NS_idx = 3;
    else
        NS_idx = 0;

#else
    NS_idx = 0;
#endif

    for (port = 0; port < SC_INTERFACE_NUM; port++)
    {
        /* Check SC_ClearFIFO() by INTERNAL Loop Back Test */
        SC_Open(pSC[port + NS_idx], SC_PIN_STATE_IGNORE, SC_PIN_STATE_HIGH);
        CLK_SysTickDelay(100);
        pSC[port + NS_idx]->PINCTL |= BIT31;
        CLK_SysTickDelay(100);
        SC_WRITE(pSC[port + NS_idx], 0x5A);
        SC_WRITE(pSC[port + NS_idx], 0x5A);
        SC_WRITE(pSC[port + NS_idx], 0x5A);
        SC_WRITE(pSC[port + NS_idx], 0x5A);
        SC_WRITE(pSC[port + NS_idx], 0x5A);

        while ((pSC[port + NS_idx]->STATUS & SC_STATUS_RXEMPTY_Msk) == SC_STATUS_RXEMPTY_Msk)
            ;

        CU_ASSERT_EQUAL(SC_READ(pSC[port + NS_idx]), 0x5A);

        while ((pSC[port + NS_idx]->STATUS & SC_STATUS_RXEMPTY_Msk) == SC_STATUS_RXEMPTY_Msk)
            ;

        CU_ASSERT_NOT_EQUAL(pSC[port + NS_idx]->STATUS & 0x707, 0x00000202);
        SC_ClearFIFO(pSC[port + NS_idx]);
        CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL(SC_READ(pSC[port + NS_idx]), 0x0);
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->STATUS & 0x707, 0x00000202);
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->ALTCTL & (SC_ALTCTL_TXRST_Msk | SC_ALTCTL_RXRST_Msk), 0x0);
        pSC[port + NS_idx]->PINCTL &= ~BIT31;
        CLK_SysTickDelay(100);
        SC_Close(pSC[port + NS_idx]);
        CLK_SysTickDelay(100);

        /* Check SC_Open(), SC_Close() and SC_IsCardInserted() */
        SC_Open(pSC[port + NS_idx], SC_PIN_STATE_IGNORE, SC_PIN_STATE_HIGH);
        CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->PINCTL & SC_PINCTL_PWRINV_Msk, 0x0);
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->CTL & SC_CTL_SCEN_Msk, SC_CTL_SCEN_Msk);
        CU_ASSERT_EQUAL(SC_IsCardInserted(pSC[port + NS_idx]), TRUE);
        SC_Close(pSC[port + NS_idx]);
        CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->INTEN, 0x0);
        //CU_ASSERT_EQUAL(pSC[port+NS_idx]->PINCTL&~SC_PINCTL_CD_PIN_ST_Msk, 0x0);
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->ALTCTL, 0x0);
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->CTL, 0x0);

        SC_Open(pSC[port + NS_idx], SC_PIN_STATE_IGNORE, SC_PIN_STATE_LOW);
        CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->PINCTL & SC_PINCTL_PWRINV_Msk, SC_PINCTL_PWRINV_Msk);
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->CTL & SC_CTL_SCEN_Msk, SC_CTL_SCEN_Msk);
        SC_Close(pSC[port + NS_idx]);
        CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->INTEN, 0x0);
        //CU_ASSERT_EQUAL(pSC[port+NS_idx]->PINCTL&~SC_PINCTL_CD_PIN_ST_Msk, 0x0);
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->ALTCTL, 0x0);
        CU_ASSERT_EQUAL(pSC[port + NS_idx]->CTL, 0x0);

        /* Check SC_SetBlockGuardTime(), SC_SetCharGuardTime() */
        for (i = 1; i <= 32; i++)
        {
            SC_SetBlockGuardTime(pSC[port + NS_idx], i);
            CU_ASSERT_EQUAL(((pSC[port + NS_idx]->CTL & SC_CTL_BGT_Msk) >> SC_CTL_BGT_Pos), (i - 1));
        }

        for (i = 12; i <= 32; i++)
        {
            SC_SetCharGuardTime(pSC[port + NS_idx], i);
            CU_ASSERT_EQUAL(pSC[port + NS_idx]->EGT, (i - 12));
        }

        /* Check SC_StartTimer(), SC_StopTimer() and SC_StopAllTimer() */
        SC_Open(pSC[port + NS_idx], SC_PIN_STATE_IGNORE, SC_PIN_STATE_HIGH);
        CLK_SysTickDelay(100);
        pSC[port + NS_idx]->CTL |= SC_CTL_TMRSEL_Msk; /* Enable all TIMER function */

        for (j = 0; j < sizeof(au32TMROPMode) / sizeof(au32TMROPMode[0]); j++)
        {
            if (au32TMROPMode[j] == SC_TMR_MODE_F)
            {
                SC_StartTimer(pSC[port + NS_idx], 0, au32TMROPMode[j], 100);

                while ((pSC[port + NS_idx]->TMRCTL0 & SC_TMRCTL0_SYNC_Msk)) {};

                CU_ASSERT_EQUAL(pSC[port + NS_idx]->TMRCTL0, (0xF << SC_TMRCTL0_OPMODE_Pos) | (100 - 1));
            }
            else
            {
                SC_StartTimer(pSC[port + NS_idx], 0, au32TMROPMode[j], 100);

                while ((pSC[port + NS_idx]->TMRCTL0 & SC_TMRCTL0_SYNC_Msk)) {};

                CU_ASSERT_EQUAL(pSC[port + NS_idx]->TMRCTL0, (j << SC_TMRCTL0_OPMODE_Pos) | (100 - 1));
            }

            CU_ASSERT_EQUAL((pSC[port + NS_idx]->ALTCTL & SC_ALTCTL_CNTEN0_Msk), SC_ALTCTL_CNTEN0_Msk);
            SC_StopTimer(pSC[port + NS_idx], 0);
            CU_ASSERT_EQUAL((pSC[port + NS_idx]->ALTCTL & SC_ALTCTL_CNTEN0_Msk), 0x0);
        }

        for (j = 0; j < sizeof(au32TMROPMode) / sizeof(au32TMROPMode[0]); j++)
        {
            if (j == 3)
                continue;

            if (au32TMROPMode[j] == SC_TMR_MODE_F)
            {
                SC_StartTimer(pSC[port + NS_idx], 1, au32TMROPMode[j], 90);

                while ((pSC[port + NS_idx]->TMRCTL1 & SC_TMRCTL1_SYNC_Msk)) {};

                CU_ASSERT_EQUAL(pSC[port + NS_idx]->TMRCTL1, (0xF << SC_TMRCTL1_OPMODE_Pos) | (90 - 1));
            }
            else
            {
                SC_StartTimer(pSC[port + NS_idx], 1, au32TMROPMode[j], 90);

                while ((pSC[port + NS_idx]->TMRCTL1 & SC_TMRCTL1_SYNC_Msk)) {};

                CU_ASSERT_EQUAL(pSC[port + NS_idx]->TMRCTL1, (j << SC_TMRCTL1_OPMODE_Pos) | (90 - 1));
            }

            CU_ASSERT_EQUAL((pSC[port + NS_idx]->ALTCTL & SC_ALTCTL_CNTEN1_Msk), SC_ALTCTL_CNTEN1_Msk);
            SC_StopTimer(pSC[port + NS_idx], 1);
            CU_ASSERT_EQUAL((pSC[port + NS_idx]->ALTCTL & SC_ALTCTL_CNTEN1_Msk), 0x0);
        }

        for (j = 0; j < sizeof(au32TMROPMode) / sizeof(au32TMROPMode[0]); j++)
        {
            if (j == 3)
                continue;

            if (au32TMROPMode[j] == SC_TMR_MODE_F)
            {
                SC_StartTimer(pSC[port + NS_idx], 2, au32TMROPMode[j], 55);

                while ((pSC[port + NS_idx]->TMRCTL2 & SC_TMRCTL2_SYNC_Msk)) {};

                CU_ASSERT_EQUAL(pSC[port + NS_idx]->TMRCTL2, (0xF << SC_TMRCTL2_OPMODE_Pos) | (55 - 1));
            }
            else
            {
                SC_StartTimer(pSC[port + NS_idx], 2, au32TMROPMode[j], 55);

                while ((pSC[port + NS_idx]->TMRCTL2 & SC_TMRCTL2_SYNC_Msk)) {};

                CU_ASSERT_EQUAL(pSC[port + NS_idx]->TMRCTL2, (j << SC_TMRCTL2_OPMODE_Pos) | (55 - 1));
            }

            CU_ASSERT_EQUAL((pSC[port + NS_idx]->ALTCTL & SC_ALTCTL_CNTEN2_Msk), SC_ALTCTL_CNTEN2_Msk);
            SC_StopTimer(pSC[port + NS_idx], 2);
            CU_ASSERT_EQUAL((pSC[port + NS_idx]->ALTCTL & SC_ALTCTL_CNTEN2_Msk), 0x0);
        }

        SC_StartTimer(pSC[port + NS_idx], 0, SC_TMR_MODE_0, 100);
        SC_StartTimer(pSC[port + NS_idx], 1, SC_TMR_MODE_0, 100);
        SC_StartTimer(pSC[port + NS_idx], 2, SC_TMR_MODE_0, 100);
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->ALTCTL & 0xE0), 0xE0);
        SC_StopAllTimer(pSC[port + NS_idx]);
        CU_ASSERT_EQUAL((pSC[port + NS_idx]->ALTCTL & 0xE0), 0x0);

        /* Check SC_ResetReader() */
        pSC[port + NS_idx]->CTL = 0x0;
        pSC[port + NS_idx]->RXTOUT = 0xA;
        pSC[port + NS_idx]->ETUCTL = 0x0;
        pSC[port + NS_idx]->PINCTL = 0x0;
        pSC[port + NS_idx]->INTEN = 0x0;
        SC_ResetReader(pSC[port + NS_idx]);

        while ((pSC[port + NS_idx]->CTL & SC_CTL_SYNC_Msk)) {};

        while ((pSC[port + NS_idx]->PINCTL & SC_PINCTL_SYNC_Msk)) {};

        CU_ASSERT_EQUAL(pSC[port + NS_idx]->CTL,    0x00006008);

        CU_ASSERT_EQUAL(pSC[port + NS_idx]->RXTOUT,  0x0);

        CU_ASSERT_EQUAL(pSC[port + NS_idx]->ETUCTL,  0x00000173);

        CU_ASSERT_EQUAL(pSC[port + NS_idx]->INTEN,    0x0000047D);

        //CU_ASSERT_EQUAL((pSC[port+NS_idx]->PINCTL&SC_PINCTL_ADAC_CD_EN_Msk), SC_PINCTL_ADAC_CD_EN_Msk);
    }

    /* Check SC_GetInterfaceClock() */
    for (port = 0; port < SC_INTERFACE_NUM; port++)
    {
        CU_ASSERT_EQUAL(SC_GetInterfaceClock(pSC[port + NS_idx]), 1000);
    }

    //printf("sc%d clock%d,%d,%d\r\n",0,SC_GetInterfaceClock(pSC[0]),CLK_GetModuleClockSource(SC0_MODULE),CLK_GetModuleClockSource(SC0_MODULE));
    //printf("sc%d clock%d,%d,%d\r\n",1,SC_GetInterfaceClock(SC1),CLK_GetModuleClockSource(SC1_MODULE),CLK_GetModuleClockSource(SC1_MODULE));
    //printf("sc%d clock%d\r\n",2,SC_GetInterfaceClock(pSC[2]));

    /* Disable all SC functions */
    SCModule_DeInit();
}

CU_TestInfo SC_MACRO_CONSTANT[] =
{
    {"Check SC MACRO and CONSTANT ", MACRO_CONSTANT_Test},
    CU_TEST_INFO_NULL
};

CU_TestInfo SC_API[] =
{
    {"Check SC API ", API_SC_Test},
    CU_TEST_INFO_NULL
};
