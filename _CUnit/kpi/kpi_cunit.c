
/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "kpi_cunit.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

#if 0
#define DBG_MSG printf
#else
#define DBG_MSG(...)
#endif



/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

KPI_KEY_T queue[512] = {0};

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

CU_SuiteInfo suites[] = {
    {"KPI API", suite_success_init, suite_success_clean, NULL, NULL, KPI_ApiTests},
    CU_SUITE_INFO_NULL
};

void KPI_Reset(void)
{
    SYS_UnlockReg();
    SYS_ResetModule(SYS_KPI0RST);
}

/*
    KPI_Open
    KPI_Close
*/
void Test_API_KPI_OPEN()
{
    uint32_t u32Rows, u32Columns;
    /* KPI_Open */

    for (u32Rows = 2; u32Rows <= 6; u32Rows++) {
        for (u32Columns = 1; u32Columns <= 8; u32Columns++) {
            CU_ASSERT(0 == KPI_Open(u32Rows, u32Columns, queue, 512));
            CU_ASSERT_EQUAL(KPI->CTL & KPI_CTL_KROW_Msk, (u32Rows - 1) << KPI_CTL_KROW_Pos);
            CU_ASSERT_EQUAL(KPI->CTL & KPI_CTL_KCOL_Msk, (u32Columns - 1) << KPI_CTL_KCOL_Pos);
            KPI_Close();
            CU_ASSERT_EQUAL(0, KPI->CTL);
        }
    }
}

const uint32_t au32DBCLKSEL[] = {
    KPI_COL_SAMPLE_8CLK,
    KPI_COL_SAMPLE_16CLK,
    KPI_COL_SAMPLE_32CLK,
    KPI_COL_SAMPLE_64CLK,
    KPI_COL_SAMPLE_128CLK,
    KPI_COL_SAMPLE_256CLK,
    KPI_COL_SAMPLE_512CLK,
    KPI_COL_SAMPLE_1024CLK,
    KPI_COL_SAMPLE_2048CLK,
    KPI_COL_SAMPLE_4096CLK,
    KPI_COL_SAMPLE_8192CLK,
};

const uint32_t au32ROWDLY[] = {
    KPI_ROW_SCAN_DELAY4CLK,
    KPI_ROW_SCAN_DELAY8CLK,
    KPI_ROW_SCAN_DELAY16CLK,
    KPI_ROW_SCAN_DELAY32CLK,
};


/*
    KPI_ConfigKeyScanTiming
*/
void Test_API_KPI_CONFIG()
{
    KPI_KEY_T key;
    uint32_t i, u32RegVal;
    CU_ASSERT(0 == KPI_Open(6, 2, queue, 512));
    KPI->STATUS = KPI->STATUS;
    CU_ASSERT_EQUAL(KPI->CTL & KPI_CTL_KROW_Msk, (6 - 1) << KPI_CTL_KROW_Pos);
    CU_ASSERT_EQUAL(KPI->CTL & KPI_CTL_KCOL_Msk, (2 - 1) << KPI_CTL_KCOL_Pos);
    u32RegVal = KPI->CTL;

    for (i = 0; i < 0x100; i++) {
        KPI_ConfigKeyScanTiming(i + 1, KPI_COL_SAMPLE_8CLK, KPI_ROW_SCAN_DELAY4CLK);
        CU_ASSERT_EQUAL(KPI->CTL & KPI_CTL_PSC_Msk, i << KPI_CTL_PSC_Pos);
    }

    for (i = 0x3; i <= 0xD; i++) {
        KPI_ConfigKeyScanTiming(0x40, i << KPI_CTL_DBCLKSEL_Pos, KPI_ROW_SCAN_DELAY4CLK);
        CU_ASSERT_EQUAL(KPI->CTL & KPI_CTL_DBCLKSEL_Msk, au32DBCLKSEL[i - 0x03]);
    }

    for (i = 0; i < 4; i++) {
        KPI_ConfigKeyScanTiming(0x40, KPI_COL_SAMPLE_8CLK, i << KPI_CTL_ROWDLY_Pos);
        CU_ASSERT_EQUAL(KPI->CTL & KPI_CTL_ROWDLY_Msk, au32ROWDLY[i]);
    }

    CU_ASSERT_EQUAL(u32RegVal & ~(KPI_CTL_PSC_Msk | KPI_CTL_DBCLKSEL_Msk | KPI_CTL_ROWDLY_Msk),
                    KPI->CTL & ~(KPI_CTL_PSC_Msk | KPI_CTL_DBCLKSEL_Msk | KPI_CTL_ROWDLY_Msk));
    KPI_Close();
    CU_ASSERT_EQUAL(0, KPI->CTL);
}


/*
    KPI_IRQHandler
*/
void Test_API_KPI_PRESS()
{
    KPI_KEY_T key;
    uint32_t pressCnt = 0;
    uint32_t u32TimeOutCount = (SystemCoreClock / 10);
    //
    KPI_Reset();
    CU_ASSERT(0 == KPI_Open(6, 2, queue, 512));
    KPI->STATUS = KPI->STATUS;
    CU_ASSERT_EQUAL(KPI->CTL & KPI_CTL_KROW_Msk, (6 - 1) << KPI_CTL_KROW_Pos);
    CU_ASSERT_EQUAL(KPI->CTL & KPI_CTL_KCOL_Msk, (2 - 1) << KPI_CTL_KCOL_Pos);
    NVIC_EnableIRQ(KPI_IRQn);

    // 6 row x 2 col, there should be 12 press events.
    while (pressCnt < (6 * 2)) {
        if (KPI_kbhit()) {
            key = KPI_GetKey();
            printf("\n\t %d, %d, %s", key.x, key.y, (key.st == KPI_PRESS) ? "PRESS" : "RELEASE");
            pressCnt++;
        }

        if (--u32TimeOutCount == 0) {
            break;
        }
    }

    NVIC_DisableIRQ(KPI_IRQn);
    /*
        to pass this test, press all 6 * 2 keys if a keypad is connected to m55m1.
        or, *** remove all pull up resistor *** if there is no keypad.
        sample low(0) at column means press, sample high(1) is release.
    */
    CU_ASSERT_EQUAL(pressCnt, 6 * 2);
}

CU_TestInfo KPI_ApiTests[] = {
    {" 1: API KPI_Open_Close.", Test_API_KPI_OPEN},
    {" 2: API KPI_CONFIG.", Test_API_KPI_CONFIG},
    {" 3: API KPI_PRESS.", Test_API_KPI_PRESS},
    CU_TEST_INFO_NULL
};
