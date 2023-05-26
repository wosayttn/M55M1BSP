/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "lppdma_cunit.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"


/*---------------------------------------------------------------------------------------------------------*/
/*  Constant Definitions                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
//#define M252_D
//#define M252_G
//#define M252_C
//#define M252_E
#define M256_D
//#define M258_G

uint32_t TenBitsPatternTable[4] = {0x3FF, 0x25A, 0x1A5, 0x000}; /* 10 bits test pattern */
uint32_t SixteenBitsPatternTable[4] = {0xFFFF, 0x5A5A, 0xA5A5, 0x0000}; /* 16 bits test pattern */
uint32_t ThirtyTwoBitsPatternTable[4] = {0xFFFFFFFF, 0x5A5A5A5A, 0xA5A5A5A5, 0x00000000}; /* 32 bits test pattern */

/*---------------------------------------------------------------------------------------------------------*/
/*  Constant Definitions                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t SrcArray[256] __attribute__((aligned(8)));
uint8_t DestArray[256] __attribute__((aligned(8)));
uint32_t LPPDMA_TEST_LENGTH = 64;

typedef struct dma_desc_t {
    uint32_t ctl;
    uint32_t src;
    uint32_t dest;
    uint32_t offset;
} DMA_DESC_T;

DMA_DESC_T DMA_DESC[1];

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

void LPPDMA_Reset(void)
{
    SYS_UnlockReg();
    SYS_ResetModule(SYS_LPPDMA0RST);
    SYS_LockReg();
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
    {"LPPDMA MACRO", suite_success_init, suite_success_clean, NULL, NULL, LPPDMA_MACRO},
    {"LPPDMA API", suite_success_init, suite_success_clean, NULL, NULL, LPPDMA_API},
    {"LPPDMA CONSTANT", suite_success_init, suite_success_clean, NULL, NULL, LPPDMA_CONSTANT},
    CU_SUITE_INFO_NULL
};


void Func_LPPDMA_Open()
{
    uint32_t u32TestCh = 0;
    uint32_t u32PatternIdx = 0;
    uint32_t au32RegWrite[3] = {0x05, 0x0A, 0x0F};
    uint32_t au32RegCheck[3] = {0x05, 0x0F, 0x0F};

    //test case 1: enable/disable one channel each time, then check each action
    for (u32TestCh = 0; u32TestCh < LPPDMA_CH_MAX; u32TestCh++) {
        LPPDMA_Open(LPPDMA, 1 << u32TestCh);
        CU_ASSERT_EQUAL(LPPDMA->CHCTL, 1 << u32TestCh);
        LPPDMA_Close(LPPDMA);
        CU_ASSERT_EQUAL(LPPDMA->CHCTL, 0);
    }

    //test case 2: enable/disable one channel each time, then check at final action
    for (u32TestCh = 0; u32TestCh < LPPDMA_CH_MAX; u32TestCh++) {
        LPPDMA_Open(LPPDMA, 1 << u32TestCh);
    }

    //CU_ASSERT_EQUAL(LPPDMA->CHCTL, 0xFFF);
    CU_ASSERT_EQUAL(LPPDMA->CHCTL, pow(2, LPPDMA_CH_MAX) - 1);
    LPPDMA_Close(LPPDMA);
    CU_ASSERT_EQUAL(LPPDMA->CHCTL, 0);

    //test case 3: enable/disable multiple channels each time
    for (u32PatternIdx = 0; u32PatternIdx < sizeof(au32RegWrite) / sizeof(uint32_t); u32PatternIdx++) {
        LPPDMA_Open(LPPDMA, au32RegWrite[u32PatternIdx]);
        CU_ASSERT_EQUAL(LPPDMA->CHCTL, au32RegCheck[u32PatternIdx]);

        if (LPPDMA->CHCTL != au32RegCheck[u32PatternIdx]) {
            printf("Error LPPDMA->CHCTL = 0x%08X != 0x%08X", LPPDMA->CHCTL, au32RegCheck[u32PatternIdx]);
        }
    }

    LPPDMA_Close(LPPDMA);
    CU_ASSERT_EQUAL(LPPDMA->CHCTL, 0);
}

void Func_LPPDMA_SetTransferCnt()
{
    uint32_t u32TestCh = 0;
    uint32_t u32TransferCountPatternIdx = 0, u32TransferCountPatternCount = 0, u32TransferCountPatternData = 0;
    uint32_t au32RegWrite[4] = {0x1555, 0x2AAA, 0x3FFF, 1};
    u32TransferCountPatternCount = sizeof(au32RegWrite) / sizeof(uint32_t);

    for (u32TestCh = 0; u32TestCh < LPPDMA_CH_MAX; u32TestCh++) {
        for (u32TransferCountPatternIdx = 0; u32TransferCountPatternIdx < u32TransferCountPatternCount; u32TransferCountPatternIdx++) {
            u32TransferCountPatternData = au32RegWrite[u32TransferCountPatternIdx];
            LPPDMA_SetTransferCnt(LPPDMA, u32TestCh, LPPDMA_WIDTH_8, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_TXWIDTH_Msk, 0 << 12);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_TXCNT_Msk, (u32TransferCountPatternData - 1) << 16);
            LPPDMA_SetTransferCnt(LPPDMA, u32TestCh, LPPDMA_WIDTH_16, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_TXWIDTH_Msk, 1 << 12);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_TXCNT_Msk, (u32TransferCountPatternData - 1) << 16);
            LPPDMA_SetTransferCnt(LPPDMA, u32TestCh, LPPDMA_WIDTH_32, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_TXWIDTH_Msk, 2 << 12);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_TXCNT_Msk, (u32TransferCountPatternData - 1) << 16);
        }
    }
}

void Func_LPPDMA_SetTransferAddr()
{
    uint32_t u32TestCh = 0;
    uint32_t u32TransferAddressPatternIdx = 0, u32TransferAddressPatternCount = 0, u32TransferAddressPatternData = 0;
    u32TransferAddressPatternCount = sizeof(ThirtyTwoBitsPatternTable) / sizeof(uint32_t);
    LPPDMA_Open(LPPDMA, pow(2, LPPDMA_CH_MAX) - 1);
    CU_ASSERT_EQUAL(LPPDMA->CHCTL, pow(2, LPPDMA_CH_MAX) - 1);

    for (u32TestCh = 0; u32TestCh < LPPDMA_CH_MAX; u32TestCh++) {
        for (u32TransferAddressPatternIdx = 0; u32TransferAddressPatternIdx < u32TransferAddressPatternCount; u32TransferAddressPatternIdx++) {
            u32TransferAddressPatternData = ThirtyTwoBitsPatternTable[u32TransferAddressPatternIdx];
            LPPDMA_SetTransferAddr(LPPDMA, u32TestCh, u32TransferAddressPatternData, LPPDMA_SAR_FIX, u32TransferAddressPatternData, LPPDMA_DAR_FIX);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].SA, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].DA, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_SAINC_Msk, 3 << 8);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_DAINC_Msk, 3 << 10);
            LPPDMA_SetTransferAddr(LPPDMA, u32TestCh, u32TransferAddressPatternData, LPPDMA_SAR_INC, u32TransferAddressPatternData, LPPDMA_DAR_INC);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].SA, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].DA, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_SAINC_Msk, 0 << 8);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_DAINC_Msk, 0 << 10);
        }
    }

    LPPDMA_Close(LPPDMA);
    CU_ASSERT_EQUAL(LPPDMA->CHCTL, 0);
}

void Func_LPPDMA_SetTransferMode()
{
    uint32_t u32TestCh = 0;
    uint32_t u32PatternIdx = 0;
    uint32_t au32TransferMode[] = {
        //
        LPPDMA_MEM,
        //
        LPPDMA_LPUART0_TX, LPPDMA_LPUART0_RX,
        //
        LPPDMA_LPSPI0_TX, LPPDMA_LPSPI0_RX,
        //
        LPPDMA_LPI2C0_TX, LPPDMA_LPI2C0_RX,
        //
        LPPDMA_LPTMR0, LPPDMA_LPTMR1,
        //
        LPPDMA_TTMR0, LPPDMA_TTMR1,
        //
        LPPDMA_LPADC0_RX, LPPDMA_DMIC0_RX

    };
    uint32_t au32ReqSelRegCheck[] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 10, 11,
        13,
    };

    for (u32TestCh = 0; u32TestCh < LPPDMA_CH_MAX; u32TestCh++) {
        for (u32PatternIdx = 0; u32PatternIdx < sizeof(au32TransferMode) / sizeof(uint32_t); u32PatternIdx++) {
            LPPDMA_SetTransferMode(LPPDMA, u32TestCh, au32TransferMode[u32PatternIdx], TRUE, 0xFFFC);
            CU_ASSERT_EQUAL(*(__IO uint32_t *)(&(LPPDMA->REQSEL0_3) + ((u32TestCh) >> 2)) & (LPPDMA_REQSEL0_3_REQSRC0_Msk << ((u32TestCh % 4) * 8)), au32ReqSelRegCheck[u32PatternIdx] << ((u32TestCh % 4) * 8));
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_OPMODE_Msk, LPPDMA_OP_SCATTER);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].NEXT, 0xFFFC);
            LPPDMA_SetTransferMode(LPPDMA, u32TestCh, au32TransferMode[u32PatternIdx], TRUE, 0x1554);
            CU_ASSERT_EQUAL(*(__IO uint32_t *)(&(LPPDMA->REQSEL0_3) + ((u32TestCh) >> 2)) & (LPPDMA_REQSEL0_3_REQSRC0_Msk << ((u32TestCh % 4) * 8)), au32ReqSelRegCheck[u32PatternIdx] << ((u32TestCh % 4) * 8));
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_OPMODE_Msk, LPPDMA_OP_SCATTER);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].NEXT, 0x1554);
            LPPDMA_SetTransferMode(LPPDMA, u32TestCh, au32TransferMode[u32PatternIdx], TRUE, 0);
            CU_ASSERT_EQUAL(*(__IO uint32_t *)(&(LPPDMA->REQSEL0_3) + ((u32TestCh) >> 2)) & (LPPDMA_REQSEL0_3_REQSRC0_Msk << ((u32TestCh % 4) * 8)), au32ReqSelRegCheck[u32PatternIdx] << ((u32TestCh % 4) * 8));
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_OPMODE_Msk, LPPDMA_OP_SCATTER);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].NEXT, 0);
            LPPDMA_SetTransferMode(LPPDMA, u32TestCh, au32TransferMode[u32PatternIdx], TRUE, 0x2AA8);
            CU_ASSERT_EQUAL(*(__IO uint32_t *)(&(LPPDMA->REQSEL0_3) + ((u32TestCh) >> 2)) & (LPPDMA_REQSEL0_3_REQSRC0_Msk << ((u32TestCh % 4) * 8)), au32ReqSelRegCheck[u32PatternIdx] << ((u32TestCh % 4) * 8));
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_OPMODE_Msk, LPPDMA_OP_SCATTER);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].NEXT, 0x2AA8);
            LPPDMA_SetTransferMode(LPPDMA, u32TestCh, au32TransferMode[u32PatternIdx], FALSE, 0);
            CU_ASSERT_EQUAL(*(__IO uint32_t *)(&(LPPDMA->REQSEL0_3) + ((u32TestCh) >> 2)) & (LPPDMA_REQSEL0_3_REQSRC0_Msk << ((u32TestCh % 4) * 8)), au32ReqSelRegCheck[u32PatternIdx] << ((u32TestCh % 4) * 8));
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_OPMODE_Msk, LPPDMA_OP_BASIC);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].NEXT, 0x2AA8);
        }
    }
}

void Func_LPPDMA_SetBurstType()
{
    uint32_t u32TestCh = 0;
    uint32_t u32TransferCountPatternIdx = 0, u32TransferCountPatternCount = 0, u32TransferCountPatternData = 0;
    uint32_t au32BurstSize[8] = {LPPDMA_BURST_128, LPPDMA_BURST_64, LPPDMA_BURST_32, LPPDMA_BURST_16, LPPDMA_BURST_8, LPPDMA_BURST_4, LPPDMA_BURST_2, LPPDMA_BURST_1};
    uint32_t au32RegCheck[8] = {0, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70};
    u32TransferCountPatternCount = sizeof(au32BurstSize) / sizeof(uint32_t);

    for (u32TestCh = 0; u32TestCh < LPPDMA_CH_MAX; u32TestCh++) {
        for (u32TransferCountPatternIdx = 0; u32TransferCountPatternIdx < u32TransferCountPatternCount; u32TransferCountPatternIdx++) {
            u32TransferCountPatternData = au32BurstSize[u32TransferCountPatternIdx];
            LPPDMA_SetBurstType(LPPDMA, u32TestCh, LPPDMA_REQ_SINGLE, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_TXTYPE_Msk, 1 << 2);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_BURSIZE_Msk, au32RegCheck[u32TransferCountPatternIdx]);
            LPPDMA_SetBurstType(LPPDMA, u32TestCh, LPPDMA_REQ_BURST, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_TXTYPE_Msk, 0 << 2);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_BURSIZE_Msk, au32RegCheck[u32TransferCountPatternIdx]);
        }
    }
}

void Func_LPPDMA_EnableInt()
{
    uint32_t u32TestCh = 0;

    for (u32TestCh = 0; u32TestCh < LPPDMA_CH_MAX; u32TestCh++) {
        LPPDMA_EnableInt(LPPDMA, u32TestCh, LPPDMA_INT_TRANS_DONE);
        CU_ASSERT_EQUAL(LPPDMA->INTEN, pow(2, u32TestCh + 1) - 1);
        LPPDMA_EnableInt(LPPDMA, u32TestCh, LPPDMA_INT_TEMPTY);
        CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_TBINTDIS_Msk, 0);
    }

    for (u32TestCh = 0; u32TestCh < LPPDMA_CH_MAX; u32TestCh++) {
        LPPDMA_DisableInt(LPPDMA, u32TestCh, LPPDMA_INT_TRANS_DONE);
        CU_ASSERT_EQUAL(LPPDMA->INTEN, ((uint32_t)(pow(2, LPPDMA_CH_MAX) - 1) >> (u32TestCh + 1)) << (u32TestCh + 1));
        LPPDMA_DisableInt(LPPDMA, u32TestCh, LPPDMA_INT_TEMPTY);
        CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_TBINTDIS_Msk, 1 << 7);
    }
}

void Func_LPPDMA_GET_INT_STATUS()
{
    uint32_t u32TestCh = 0;

    for (u32TestCh = 0; u32TestCh < LPPDMA_CH_MAX; u32TestCh++) {
        LPPDMA_Open(LPPDMA, 1 << u32TestCh);
        //abort interrupt flag test
        LPPDMA_SetTransferCnt(LPPDMA, u32TestCh, LPPDMA_WIDTH_32, LPPDMA_TEST_LENGTH);
        LPPDMA_SetTransferMode(LPPDMA, u32TestCh, LPPDMA_MEM, FALSE, NULL);
        //        LPPDMA_SetTransferAddr(LPPDMA, u32TestCh, (uint32_t)SrcArray, LPPDMA_SAR_INC, (uint32_t)0, LPPDMA_DAR_INC); //it means write data to flash => abort
        //LPPDMA_SetTransferAddr(u32TestCh, (uint32_t)0, LPPDMA_SAR_INC, (uint32_t)DestArray, LPPDMA_DAR_INC); //it means read data from flash => it is supported
        LPPDMA_SetTransferAddr(LPPDMA, u32TestCh, (uint32_t)SrcArray, LPPDMA_SAR_INC, (uint32_t)0xFFFFFFF0, LPPDMA_DAR_INC); // invalid system address => abort
        LPPDMA_Trigger(LPPDMA, u32TestCh);

        while (LPPDMA_GET_INT_STATUS(LPPDMA) != LPPDMA_INTSTS_ABTIF_Msk) {};

        CU_ASSERT_EQUAL(LPPDMA_GET_ABORT_STS(LPPDMA), 1 << u32TestCh);

        CU_ASSERT_EQUAL(LPPDMA_GET_INT_STATUS(LPPDMA) & LPPDMA_INTSTS_ABTIF_Msk, 1);

        LPPDMA_CLR_ABORT_FLAG(LPPDMA, 1 << u32TestCh);

        CU_ASSERT_EQUAL(LPPDMA_GET_ABORT_STS(LPPDMA), 0);

        CU_ASSERT_EQUAL(LPPDMA_GET_INT_STATUS(LPPDMA) & LPPDMA_INTSTS_ABTIF_Msk, 0);

        /* Reset LPPDMA */
        LPPDMA_Reset();

        LPPDMA_Open(LPPDMA, 1 << u32TestCh);

        //transfer done interrupt flag test
        LPPDMA_SetTransferCnt(LPPDMA, u32TestCh, LPPDMA_WIDTH_32, LPPDMA_TEST_LENGTH);

        LPPDMA_SetTransferMode(LPPDMA, u32TestCh, LPPDMA_MEM, FALSE, NULL);

        LPPDMA_SetTransferAddr(LPPDMA, u32TestCh, (uint32_t)SrcArray, LPPDMA_SAR_INC, (uint32_t)DestArray, LPPDMA_DAR_INC);

        LPPDMA_EnableInt(LPPDMA, u32TestCh, LPPDMA_INT_TRANS_DONE);

        LPPDMA_Trigger(LPPDMA, u32TestCh);

        while (LPPDMA_IS_CH_BUSY(LPPDMA, u32TestCh) == 1) {};

        CU_ASSERT_EQUAL(LPPDMA_GET_TD_STS(LPPDMA), 1 << u32TestCh);

        CU_ASSERT_EQUAL(LPPDMA_GET_INT_STATUS(LPPDMA) & LPPDMA_INTSTS_TDIF_Msk, 1 << 1);

        LPPDMA_CLR_TD_FLAG(LPPDMA, 1 << u32TestCh);

        CU_ASSERT_EQUAL(LPPDMA_GET_TD_STS(LPPDMA), 0);

        CU_ASSERT_EQUAL(LPPDMA_GET_INT_STATUS(LPPDMA) & LPPDMA_INTSTS_TDIF_Msk, 0);

        LPPDMA_DisableInt(LPPDMA, u32TestCh, LPPDMA_INT_TRANS_DONE);

        LPPDMA_SetTransferCnt(LPPDMA, u32TestCh, LPPDMA_WIDTH_32, LPPDMA_TEST_LENGTH);

        LPPDMA_SetTransferMode(LPPDMA, u32TestCh, LPPDMA_MEM, FALSE, NULL);

        LPPDMA_SetTransferAddr(LPPDMA, u32TestCh, (uint32_t)SrcArray, LPPDMA_SAR_INC, (uint32_t)DestArray, LPPDMA_DAR_INC);

        //         //table empty flag test
        //         DMA_DESC[0].ctl = 0;
        //         DMA_DESC[0].src = 0;
        //         DMA_DESC[0].dest = 0;
        //         DMA_DESC[0].offset = 0;
        //         LPPDMA_SetTransferMode(u32TestCh, LPPDMA_MEM, TRUE, (uint32_t)&DMA_DESC[0]);
        //         LPPDMA_EnableInt(u32TestCh, LPPDMA_INT_TEMPTY);
        //         LPPDMA_Trigger(u32TestCh);
        //         while(LPPDMA_GET_INT_STATUS() != LPPDMA_INTSTS_TEIF_Msk);
        //         CU_ASSERT_EQUAL(LPPDMA_GET_EMPTY_STS(), 1 << u32TestCh);
        //         CU_ASSERT_EQUAL(LPPDMA_GET_INT_STATUS() & LPPDMA_INTSTS_TEIF_Msk, 1 << 2);
        //         LPPDMA_CLR_EMPTY_FLAG(1 << u32TestCh);
        //         CU_ASSERT_EQUAL(LPPDMA_GET_EMPTY_STS(), 0);
        //         CU_ASSERT_EQUAL(LPPDMA_GET_INT_STATUS() & LPPDMA_INTSTS_TEIF_Msk, 0);
        LPPDMA_DisableInt(LPPDMA, u32TestCh, LPPDMA_INT_TEMPTY);
    }

    LPPDMA_Close(LPPDMA);
    CU_ASSERT_EQUAL(LPPDMA->CHCTL, 0);
}

void Func_LPPDMA_SET_SRC_ADDR()
{
    uint32_t u32TestCh = 0;
    uint32_t u32TransferAddressPatternIdx = 0, u32TransferAddressPatternCount = 0, u32TransferAddressPatternData = 0;
    u32TransferAddressPatternCount = sizeof(ThirtyTwoBitsPatternTable) / sizeof(uint32_t);
    LPPDMA_Open(LPPDMA, pow(2, LPPDMA_CH_MAX) - 1);
    CU_ASSERT_EQUAL(LPPDMA->CHCTL, pow(2, LPPDMA_CH_MAX) - 1);

    for (u32TestCh = 0; u32TestCh < LPPDMA_CH_MAX; u32TestCh++) {
        for (u32TransferAddressPatternIdx = 0; u32TransferAddressPatternIdx < u32TransferAddressPatternCount; u32TransferAddressPatternIdx++) {
            u32TransferAddressPatternData = ThirtyTwoBitsPatternTable[u32TransferAddressPatternIdx];
            LPPDMA_SET_SRC_ADDR(LPPDMA, u32TestCh, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].SA, u32TransferAddressPatternData);
            LPPDMA_SET_DST_ADDR(LPPDMA, u32TestCh, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].DA, u32TransferAddressPatternData);
        }
    }

    LPPDMA_Close(LPPDMA);
    CU_ASSERT_EQUAL(LPPDMA->CHCTL, 0);
}

void Func_LPPDMA_SET_TRANS_CNT()
{
    uint32_t u32TestCh = 0;
    uint32_t u32TransferCountPatternIdx = 0, u32TransferCountPatternCount = 0, u32TransferCountPatternData = 0;
    uint32_t au32RegWrite[4] = {0x1555, 0x2AAA, 0x3FFF, 1};
    u32TransferCountPatternCount = sizeof(au32RegWrite) / sizeof(uint32_t);

    for (u32TestCh = 0; u32TestCh < LPPDMA_CH_MAX; u32TestCh++) {
        for (u32TransferCountPatternIdx = 0; u32TransferCountPatternIdx < u32TransferCountPatternCount; u32TransferCountPatternIdx++) {
            u32TransferCountPatternData = au32RegWrite[u32TransferCountPatternIdx];
            LPPDMA_SET_TRANS_CNT(LPPDMA, u32TestCh, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(LPPDMA->LPDSCT[u32TestCh].CTL & LPPDMA_DSCT_CTL_TXCNT_Msk, (u32TransferCountPatternData - 1) << 16);
        }
    }
}

void Func_LPPDMA_STOP()
{
    uint32_t u32TestCh = 0;

    for (u32TestCh = 0; u32TestCh < LPPDMA_CH_MAX; u32TestCh++) {
        LPPDMA_Open(LPPDMA, 1 << u32TestCh);
        CU_ASSERT_EQUAL(LPPDMA->CHCTL, 1 << u32TestCh);
        LPPDMA_Trigger(LPPDMA, u32TestCh);
        LPPDMA_STOP(LPPDMA, u32TestCh);
        CU_ASSERT_EQUAL(LPPDMA->CHCTL, 0);
        //         LPPDMA_Close();
        //         CU_ASSERT_EQUAL(LPPDMA->CHCTL, 0);
    }
}

void Func_LPPDMA_CONSTANT()
{
    /* Reset LPPDMA */
    LPPDMA_Reset();
    LPPDMA->LPDSCT[0].CTL = LPPDMA_OP_BASIC;
    CU_ASSERT_EQUAL(LPPDMA->LPDSCT[0].CTL, LPPDMA_OP_BASIC);
    LPPDMA->LPDSCT[0].CTL = LPPDMA_OP_SCATTER;
    CU_ASSERT_EQUAL(LPPDMA->LPDSCT[0].CTL, LPPDMA_OP_SCATTER);
    LPPDMA->LPDSCT[0].CTL = LPPDMA_OP_STOP;
    CU_ASSERT_EQUAL(LPPDMA->LPDSCT[0].CTL, LPPDMA_OP_STOP);
}

CU_TestInfo LPPDMA_MACRO[] = {
    {
        "Test LPPDMA_GET_INT_STATUS/LPPDMA_GET_ABORT_STS/LPPDMA_CLR_ABORT_FLAG/LPPDMA_IS_CH_BUSY\n\t\t /LPPDMA_GET_TD_STS/LPPDMA_CLR_TD_FLAG/LPPDMA_CLR_ABORT_FLAG/LPPDMA_GET_EMPTY_STS/LPPDMA_CLR_EMPTY_FLAG\n\t\t /Func_LPPDMA_Trigger:", \
        Func_LPPDMA_GET_INT_STATUS
    },

    {"Test LPPDMA_SET_SRC_ADDR/LPPDMA_SET_DST_ADDR:", Func_LPPDMA_SET_SRC_ADDR},
    {"Test LPPDMA_STOP:", Func_LPPDMA_STOP},

    CU_TEST_INFO_NULL
};

CU_TestInfo LPPDMA_API[] = {
    {"Test LPPDMA_Open/LPPDMA_Close:", Func_LPPDMA_Open},
    {"Test LPPDMA_SetTransferCnt:", Func_LPPDMA_SetTransferCnt},
    {"Test LPPDMA_SetTransferAddr:", Func_LPPDMA_SetTransferAddr},
    {"Test LPPDMA_SetTransferMode:", Func_LPPDMA_SetTransferMode},
    {"Test LPPDMA_SetBurstType:", Func_LPPDMA_SetBurstType},
    {"Test LPPDMA_EnableInt/LPPDMA_DisableInt:", Func_LPPDMA_EnableInt},
    CU_TEST_INFO_NULL
};

CU_TestInfo LPPDMA_CONSTANT[] = {
    {"Test LPPDMA_CONSTANT:", Func_LPPDMA_CONSTANT},

    CU_TEST_INFO_NULL
};
