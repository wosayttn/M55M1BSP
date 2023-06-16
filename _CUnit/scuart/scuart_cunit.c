/******************************************************************************
* @file    scuart_cunit.c
* @version V1.00
* @brief   scuart CUnit Test
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
#include "scuart_cunit.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

//#define D_msg   printf
#define D_msg(...)

#define DEF_SC1     1

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
    {"SCUART MACRO & CONSTANT", suite_success_init, suite_success_clean, NULL, NULL, SCUART_MACRO_CONSTANT},
    {"SCUART API", suite_success_init, suite_success_clean, NULL, NULL, SCUART_API},
    CU_SUITE_INFO_NULL
};

SC_T *pSC[SC_INTERFACE_NUM] =
{
    SC0,
    SC1,
    SC2
};

const uint32_t au32CHARLen[] =
{
    SCUART_CHAR_LEN_5,
    SCUART_CHAR_LEN_6,
    SCUART_CHAR_LEN_7,
    SCUART_CHAR_LEN_8,
};

const uint32_t au32PARITYSel[] =
{
    SCUART_PARITY_NONE,
    SCUART_PARITY_ODD,
    SCUART_PARITY_EVEN,
};

const uint32_t au32Result[] =
{
    0x00000031 + 0x40,
    0x00000031 + 0x80,
    0x00000031 + 0x00,
    0x00000021 + 0x40,
    0x00000021 + 0x80,
    0x00000021 + 0x00,
    0x00000011 + 0x40,
    0x00000011 + 0x80,
    0x00000011 + 0x00,
    0x00000001 + 0x40,
    0x00000001 + 0x80,
    0x00000001 + 0x00,
};

int32_t SCUART_InitClock(void)
{
    /* Enable clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    CLK_SetModuleClock(SC0_MODULE, CLK_SCSEL_SC0SEL_HIRC, 0);
    CLK_SetModuleClock(SC1_MODULE, CLK_SCSEL_SC1SEL_HIRC, 0);
    CLK_SetModuleClock(SC2_MODULE, CLK_SCSEL_SC1SEL_HIRC, 0);

    CLK_EnableModuleClock(SC0_MODULE);
    CLK_EnableModuleClock(SC1_MODULE);
    CLK_EnableModuleClock(SC2_MODULE);

    return 0;
}

void MACRO_CONSTANT_Test(void)
{
    uint32_t i, j, k;
    uint8_t u8RDA[4];
    uint32_t port;

    /* Reset all SC */
    SYS_ResetModule(SYS_SC0RST);
    SYS_ResetModule(SYS_SC1RST);
    SYS_ResetModule(SYS_SC2RST);

    if (SCUART_InitClock() != 0)
    {
        CU_FAIL("SCUART Init Clock FAIL");
        return ;
    }

    /* Check CONSTANT:
        SCUART_CHAR_LEN_5, SCUART_CHAR_LEN_6, SCUART_CHAR_LEN_7, SCUART_CHAR_LEN_8,
        SCUART_PARITY_NONE, SCUART_PARITY_ODD, SCUART_PARITY_EVEN,
        SCUART_STOP_BIT_1, SCUART_STOP_BIT_2
       Check SCUART_SetLineConfig() and SCUART_Close() API
    */
    for (port = 0; port < 3; port++)
    {
        k = 0;

        for (i = 0; i < sizeof(au32CHARLen) / sizeof(au32CHARLen[0]); i++)
        {
            for (j = 0; j < sizeof(au32PARITYSel) / sizeof(au32PARITYSel[0]); j++)
            {
                SCUART_SetLineConfig(pSC[port], 115200, au32CHARLen[i], au32PARITYSel[j], SCUART_STOP_BIT_1);

                while ((pSC[port]->CTL & SC_CTL_SYNC_Msk) == SC_CTL_SYNC_Msk) {};

                //printf("REG:0x%08x, CTLREF:0x%08x\n",pSC[port]->UARTCTL,pSC[port]->CTL);
                CU_ASSERT_EQUAL(pSC[port]->CTL, 0x8001);

                CU_ASSERT_EQUAL(pSC[port]->UARTCTL, au32Result[k++]);

                SCUART_Close(pSC[port]);

                while ((pSC[port]->CTL & SC_CTL_SYNC_Msk) == SC_CTL_SYNC_Msk) {};

                CU_ASSERT_EQUAL(pSC[port]->CTL, 0x0);

                CU_ASSERT_EQUAL(pSC[port]->UARTCTL, 0x0);
            }
        }

        k = 0;

        for (i = 0; i < sizeof(au32CHARLen) / sizeof(au32CHARLen[0]); i++)
        {
            for (j = 0; j < sizeof(au32PARITYSel) / sizeof(au32PARITYSel[0]); j++)
            {
                SCUART_SetLineConfig(pSC[port], 115200, au32CHARLen[i], au32PARITYSel[j], SCUART_STOP_BIT_2);

                while ((pSC[port]->CTL & SC_CTL_SYNC_Msk) == SC_CTL_SYNC_Msk) {};

                //D_msg("BAUD:%d, REG:0x%08x\n", u32CurBAUD, pSC[port]->UARTCTL);
                //printf("REG:0x%08x, CTLREF:0x%08x\n",pSC[port]->UARTCTL,pSC[port]->CTL);
                CU_ASSERT_EQUAL(pSC[port]->CTL, 0x0001);

                CU_ASSERT_EQUAL(pSC[port]->UARTCTL, au32Result[k++]);

                SCUART_Close(pSC[port]);

                while ((pSC[port]->CTL & SC_CTL_SYNC_Msk) == SC_CTL_SYNC_Msk) {};

                CU_ASSERT_EQUAL(pSC[port]->CTL, 0x0);

                CU_ASSERT_EQUAL(pSC[port]->UARTCTL, 0x0);
            }
        }

        /* Check MACRO: SCUART_ENABLE_INT(), SCUART_DISABLE_INT() */
        SCUART_ENABLE_INT(pSC[port], SC_INTEN_RXTOIEN_Msk);
        CU_ASSERT_EQUAL(pSC[port]->INTEN, 0x200);
        SCUART_DISABLE_INT(pSC[port], SC_INTEN_RXTOIEN_Msk);
        CU_ASSERT_EQUAL(pSC[port]->INTEN, 0x000);
        SCUART_ENABLE_INT(pSC[port], SC_INTEN_TERRIEN_Msk);
        CU_ASSERT_EQUAL(pSC[port]->INTEN, 0x4);
        SCUART_DISABLE_INT(pSC[port], SC_INTEN_TERRIEN_Msk);
        CU_ASSERT_EQUAL(pSC[port]->INTEN, 0x0);
        SCUART_ENABLE_INT(pSC[port], SC_INTEN_TBEIEN_Msk);
        CU_ASSERT_EQUAL(pSC[port]->INTEN, 0x2);
        SCUART_DISABLE_INT(pSC[port], SC_INTEN_TBEIEN_Msk);
        CU_ASSERT_EQUAL(pSC[port]->INTEN, 0x0);
        SCUART_ENABLE_INT(pSC[port], SC_INTEN_RDAIEN_Msk);
        CU_ASSERT_EQUAL(pSC[port]->INTEN, 0x1);
        SCUART_DISABLE_INT(pSC[port], SC_INTEN_RDAIEN_Msk);
        CU_ASSERT_EQUAL(pSC[port]->INTEN, 0x0);

        /* Check MACRO: SCUART_GET_TX_EMPTY(), SCUART_IS_TX_FULL(), SCUART_IS_TX_EMPTY(), SCUART_GET_INT_FLAG() */
        pSC[port]->PINCTL |= BIT31; /* Enable INTERNAL Loop Back Test */
        SCUART_Open(pSC[port], 100000);
        CU_ASSERT_EQUAL((pSC[port]->STATUS & 0xFFFFDFFF), 0x00000202);
        CU_ASSERT_EQUAL(SCUART_GET_TX_EMPTY(pSC[port]), SC_STATUS_TXEMPTY_Msk);
        CU_ASSERT_EQUAL(SCUART_IS_TX_FULL(pSC[port]), 0);
        CU_ASSERT_EQUAL(SCUART_IS_TX_EMPTY(pSC[port]), 1);
        CU_ASSERT_EQUAL(SCUART_GET_INT_FLAG(pSC[port], SC_INTSTS_TERRIF_Msk), 0);
        CU_ASSERT_EQUAL(SCUART_GET_INT_FLAG(pSC[port], SC_INTSTS_TBEIF_Msk), 1);

        /* Check MACRO: SCUART_GET_RX_FULL(), SCUART_IS_RX_FULL(), SCUART_IS_RX_READY() */
        CU_ASSERT_NOT_EQUAL(SCUART_GET_RX_FULL(pSC[port]), SC_STATUS_RXFULL_Msk);
        CU_ASSERT_EQUAL(SCUART_IS_RX_FULL(pSC[port]), 0);
        CU_ASSERT_EQUAL(SCUART_IS_RX_READY(pSC[port]), 0);

        /* Check MACRO:
            SCUART_WRITE(), SCUART_WAIT_TX_EMPTY(), SCUART_GET_ERR_FLAG(), SCUART_CLR_INT_FLAG(), SCUART_CLR_ERR_FLAG(),
        */
        SCUART_WRITE(pSC[port], 0xA1);
        SCUART_WRITE(pSC[port], 0xA2);
        SCUART_WRITE(pSC[port], 0xA3);
        CU_ASSERT_EQUAL(SCUART_GET_TX_EMPTY(pSC[port]), 0);
        SCUART_WAIT_TX_EMPTY(pSC[port]) {};
        D_msg(" [3 0x%08x]\n", pSC[port]->STATUS);
        pSC[port]->ALTCTL |= (SC_ALTCTL_RXRST_Msk | SC_ALTCTL_TXRST_Msk);
        CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[port]->STATUS & 0xFFFFDFFF), 0x00000202);
        SCUART_WRITE(pSC[port], 0xB1);
        SCUART_WRITE(pSC[port], 0xB2);
        SCUART_WRITE(pSC[port], 0xB3);
        SCUART_WRITE(pSC[port], 0xB4);
        SCUART_WRITE(pSC[port], 0xB5);
        CU_ASSERT_EQUAL(SCUART_IS_TX_FULL(pSC[port]), 1);
        SCUART_WAIT_TX_EMPTY(pSC[port]) {};
        //printf("port%d, [INIT 0x%08x]\n", port,pSC[port]->INTSTS);
        CU_ASSERT_EQUAL(SCUART_GET_INT_FLAG(pSC[port], SC_INTSTS_TERRIF_Msk), 1);

        CU_ASSERT_EQUAL(SCUART_GET_ERR_FLAG(pSC[port]), 0);
        SCUART_CLR_INT_FLAG(pSC[port], SC_INTSTS_TERRIF_Msk); /* no effect */
        CU_ASSERT_EQUAL(SCUART_GET_INT_FLAG(pSC[port], SC_INTSTS_TERRIF_Msk), 1);
        SCUART_CLR_ERR_FLAG(pSC[port], SC_STATUS_TXOV_Msk);
        CU_ASSERT_EQUAL(SCUART_GET_INT_FLAG(pSC[port], SC_INTSTS_TERRIF_Msk), 0);


        while (SCUART_IS_RX_READY(pSC[port]) == 0) {};

        CLK_SysTickDelay(10000);

        //printf("port%d, [INIT 0x%08x]\n", port,pSC[port]->INTSTS);
        CU_ASSERT_EQUAL(SCUART_GET_INT_FLAG(pSC[port], SC_INTSTS_TBEIF_Msk), 1);

        i = 0;

        while (SCUART_GET_RX_EMPTY(pSC[port]) == 0)
        {
            u8RDA[i++] = SCUART_READ(pSC[port]);
        }

        CU_ASSERT_EQUAL((pSC[port]->STATUS & 0xFFFFDFFF), 0x00000202);
        CU_ASSERT_EQUAL(pSC[port]->INTSTS, 0x00000002);
        CU_ASSERT_EQUAL(u8RDA[0], 0xB1);
        CU_ASSERT_EQUAL(u8RDA[1], 0xB2);
        CU_ASSERT_EQUAL(u8RDA[2], 0xB3);
        CU_ASSERT_EQUAL(u8RDA[3], 0xB4);
        pSC[port]->PINCTL &= ~BIT31; /* Disable INTERNAL Loop Back Test */
    }

    /* UART0 RX conflicts with SC1_CLK pin */
    //    SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD0MFP_Msk);
    //    SYS->GPD_MFPL |= (SYS_GPD_MFPL_PD0MFP_UART0_RXD);
}

uint8_t au8TxBuf[] = "ABC";

void API_SCUART_Test(void)
{
    uint32_t u32Baud[3];
    uint8_t au8RxBuf[3];
    uint32_t port;

    /* Reset all SC */
    SYS_ResetModule(SYS_SC0RST);
    SYS_ResetModule(SYS_SC1RST);
    SYS_ResetModule(SYS_SC2RST);

    if (SCUART_InitClock() != 0)
    {
        CU_FAIL("SCUART Init Clock FAIL");
        return ;
    }

    for (port = 0; port < 3; port++)
    {
        /* Check SCUART_SetTimeoutCnt() */
        SCUART_SetTimeoutCnt(pSC[port], 0x5A);
        CU_ASSERT_EQUAL(pSC[port]->RXTOUT, 0x5A);
        SCUART_SetTimeoutCnt(pSC[port], 0x1FF);
        CU_ASSERT_EQUAL(pSC[port]->RXTOUT, 0x1FF);
        SCUART_SetTimeoutCnt(pSC[port], 0x0);
        CU_ASSERT_EQUAL(pSC[port]->RXTOUT, 0x0);

        /* Check SCUART_Open() */
        u32Baud[0] = SCUART_Open(pSC[port], 115200);

        if ((u32Baud[0] > (115200 + 2000)) || (u32Baud[0] < (115200 - 2000)))
        {
            CU_FAIL("Baudrate calculate FAIL");
        }

        while ((pSC[port]->CTL & SC_CTL_SYNC_Msk) == SC_CTL_SYNC_Msk) {};

        CU_ASSERT_EQUAL(pSC[port]->CTL,    0x8001);

        CU_ASSERT_EQUAL(pSC[port]->UARTCTL,  0x41);

        CU_ASSERT_EQUAL(pSC[port]->ETUCTL, (__HXT / 115200) - 1);

        /* Check SCUART_Write(), SCUART_Read(), SCUART_GET_TX_FULL() and SCUART_GET_RX_EMPTY() */
        pSC[port]->PINCTL |= BIT31; /* Enable INTERNAL Loop Back Test */

        SCUART_Write(pSC[port], au8TxBuf, sizeof(au8TxBuf)); /* Include SCUART_GET_TX_FULL() */

        //            CU_ASSERT_EQUAL(SCUART_IS_TX_ACTIVE(pSC[port]), 0);
        while ((pSC[port]->STATUS & (SC_STATUS_RXPOINT_Msk | 0x7)) != 0x00030000) {};

        //            CU_ASSERT_EQUAL(SCUART_IS_TX_ACTIVE(pSC[port]), 0);
        CU_ASSERT_EQUAL(SCUART_GET_TX_FULL(pSC[port]), 0);

        SCUART_Read(pSC[port], au8RxBuf, sizeof(au8RxBuf)); /* Include SCUART_GET_RX_EMPTY() */

        CU_ASSERT_EQUAL(au8RxBuf[0], 'A');

        CU_ASSERT_EQUAL(au8RxBuf[1], 'B');

        CU_ASSERT_EQUAL(au8RxBuf[2], 'C');

        while ((pSC[port]->STATUS & (SC_STATUS_TXACT_Msk | SC_STATUS_RXACT_Msk)) != 0x0);

        while (SCUART_GET_RX_EMPTY(pSC[port]) == 0)
        {
            SCUART_READ(pSC[port]);
        }

        SCUART_WRITE(pSC[port], 0xA5);
        //        printf("b=0x%08x\n", pSC[port]->STATUS);
        pSC[port]->PINCTL &= ~BIT31; /* Disable INTERNAL Loop Back Test */
    }

    /* UART0 RX conflicts with SC1_CLK pin */
    //    SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD0MFP_Msk);
    //    SYS->GPD_MFPL |= (SYS_GPD_MFPL_PD0MFP_UART0_RXD);
}

CU_TestInfo SCUART_MACRO_CONSTANT[] =
{
    {"Check SCUART MACRO and CONSTANT ", MACRO_CONSTANT_Test},
    CU_TEST_INFO_NULL
};

CU_TestInfo SCUART_API[] =
{
    {"Check SCUART API ", API_SCUART_Test},
    CU_TEST_INFO_NULL
};
