/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "pdma_cunit.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"


/*---------------------------------------------------------------------------------------------------------*/
/*  Constant Definitions                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define M256_D

#define PDMA_MAX_TIMEOUT_CH PDMA_CH_MAX

uint32_t TenBitsPatternTable[4] = {0x3FF, 0x25A, 0x1A5, 0x000}; /* 10 bits test pattern */
uint32_t SixteenBitsPatternTable[4] = {0xFFFF, 0x5A5A, 0xA5A5, 0x0000}; /* 16 bits test pattern */
uint32_t ThirtyTwoBitsPatternTable[4] = {0xFFFFFFFF, 0x5A5A5A5A, 0xA5A5A5A5, 0x00000000}; /* 32 bits test pattern */

/*---------------------------------------------------------------------------------------------------------*/
/*  Constant Definitions                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t SrcArray[256] __attribute__((aligned(8)));
uint8_t DestArray[256] __attribute__((aligned(8)));
uint32_t PDMA_TEST_LENGTH = 64;

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

void PDMA_Reset(void)
{
    SYS_UnlockReg();
    SYS_ResetModule(SYS_PDMA0RST);
    SYS_ResetModule(SYS_PDMA1RST);
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
    {"PDMA MACRO", suite_success_init, suite_success_clean, NULL, NULL, PDMA_MACRO},
    {"PDMA API", suite_success_init, suite_success_clean, NULL, NULL, PDMA_API},
    {"PDMA CONSTANT", suite_success_init, suite_success_clean, NULL, NULL, PDMA_CONSTANT},
    CU_SUITE_INFO_NULL
};


void Func_PDMA_Open()
{
    PDMA_T *pdma = PDMA0;
    uint32_t u32TestCh = 0;
    uint32_t u32PatternIdx = 0;
    uint32_t au32RegWrite[3] = {0x5555, 0xAAAA, 0xFFFF};
    uint32_t au32RegCheck[3] = {0x5555, 0xFFFF, 0xFFFF};
test_main:

    //test case 1: enable/disable one channel each time, then check each action
    for (u32TestCh = 0; u32TestCh < PDMA_CH_MAX; u32TestCh++) {
        PDMA_Open(pdma, 1 << u32TestCh);
        CU_ASSERT_EQUAL(pdma->CHCTL, 1 << u32TestCh);
        PDMA_Close(pdma);
        CU_ASSERT_EQUAL(pdma->CHCTL, 0);
    }

    //test case 2: enable/disable one channel each time, then check at final action
    for (u32TestCh = 0; u32TestCh < PDMA_CH_MAX; u32TestCh++) {
        PDMA_Open(pdma, 1 << u32TestCh);
    }

    CU_ASSERT_EQUAL(pdma->CHCTL, pow(2, PDMA_CH_MAX) - 1);
    PDMA_Close(pdma);
    CU_ASSERT_EQUAL(pdma->CHCTL, 0);

    //test case 3: enable/disable multiple channels each time
    for (u32PatternIdx = 0; u32PatternIdx < sizeof(au32RegWrite) / sizeof(uint32_t); u32PatternIdx++) {
        PDMA_Open(pdma, au32RegWrite[u32PatternIdx]);
        CU_ASSERT_EQUAL(pdma->CHCTL, au32RegCheck[u32PatternIdx]);
    }

    PDMA_Close(pdma);
    CU_ASSERT_EQUAL(pdma->CHCTL, 0);

    if (pdma == PDMA0) {
        pdma = PDMA1;
        goto test_main;
    }
}

void Func_PDMA_SetTransferCnt()
{
    PDMA_T *pdma = PDMA0;
    uint32_t u32TestCh = 0;
    uint32_t u32TransferCountPatternIdx = 0, u32TransferCountPatternCount = 0, u32TransferCountPatternData = 0;
    uint32_t au32RegWrite[4] = {0x1555, 0x2AAA, 0x3FFF, 1};
    u32TransferCountPatternCount = sizeof(au32RegWrite) / sizeof(uint32_t);
test_main:

    for (u32TestCh = 0; u32TestCh < PDMA_CH_MAX; u32TestCh++) {
        for (u32TransferCountPatternIdx = 0; u32TransferCountPatternIdx < u32TransferCountPatternCount; u32TransferCountPatternIdx++) {
            u32TransferCountPatternData = au32RegWrite[u32TransferCountPatternIdx];
            PDMA_SetTransferCnt(pdma, u32TestCh, PDMA_WIDTH_8, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_TXWIDTH_Msk, 0 << 12);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_TXCNT_Msk, (u32TransferCountPatternData - 1) << 16);
            PDMA_SetTransferCnt(pdma, u32TestCh, PDMA_WIDTH_16, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_TXWIDTH_Msk, 1 << 12);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_TXCNT_Msk, (u32TransferCountPatternData - 1) << 16);
            PDMA_SetTransferCnt(pdma, u32TestCh, PDMA_WIDTH_32, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_TXWIDTH_Msk, 2 << 12);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_TXCNT_Msk, (u32TransferCountPatternData - 1) << 16);
        }
    }

    if (pdma == PDMA0) {
        pdma = PDMA1;
        goto test_main;
    }
}

void Func_PDMA_SetTransferAddr()
{
    PDMA_T *pdma = PDMA0;
    uint32_t u32TestCh = 0;
    uint32_t u32TransferAddressPatternIdx = 0, u32TransferAddressPatternCount = 0, u32TransferAddressPatternData = 0;
    u32TransferAddressPatternCount = sizeof(ThirtyTwoBitsPatternTable) / sizeof(uint32_t);
test_main:
    PDMA_Open(pdma, pow(2, PDMA_CH_MAX) - 1);
    CU_ASSERT_EQUAL(pdma->CHCTL, pow(2, PDMA_CH_MAX) - 1);

    for (u32TestCh = 0; u32TestCh < PDMA_CH_MAX; u32TestCh++) {
        for (u32TransferAddressPatternIdx = 0; u32TransferAddressPatternIdx < u32TransferAddressPatternCount; u32TransferAddressPatternIdx++) {
            u32TransferAddressPatternData = ThirtyTwoBitsPatternTable[u32TransferAddressPatternIdx];
            PDMA_SetTransferAddr(pdma, u32TestCh, u32TransferAddressPatternData, PDMA_SAR_FIX, u32TransferAddressPatternData, PDMA_DAR_FIX);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].SA, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].DA, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_SAINC_Msk, 3 << 8);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_DAINC_Msk, 3 << 10);
            PDMA_SetTransferAddr(pdma, u32TestCh, u32TransferAddressPatternData, PDMA_SAR_INC, u32TransferAddressPatternData, PDMA_DAR_INC);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].SA, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].DA, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_SAINC_Msk, 0 << 8);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_DAINC_Msk, 0 << 10);
        }
    }

    PDMA_Close(pdma);
    CU_ASSERT_EQUAL(pdma->CHCTL, 0);

    if (pdma == PDMA0) {
        pdma = PDMA1;
        goto test_main;
    }
}

void Func_PDMA_SetTransferMode()
{
    PDMA_T *pdma = PDMA0;
    uint32_t u32TestCh = 0;
    uint32_t u32PatternIdx = 0;
    uint32_t au32TransferMode[] = {
        //
        PDMA_MEM, PDMA_USB_TX, PDMA_USB_RX,
        //
        PDMA_UART0_TX, PDMA_UART0_RX, PDMA_UART1_TX, PDMA_UART1_RX,
        PDMA_UART2_TX, PDMA_UART2_RX, PDMA_UART3_TX, PDMA_UART3_RX,
        PDMA_UART4_TX, PDMA_UART4_RX, PDMA_UART5_TX, PDMA_UART5_RX,
        PDMA_UART6_TX, PDMA_UART6_RX, PDMA_UART7_TX, PDMA_UART7_RX,
        PDMA_UART8_TX, PDMA_UART8_RX, PDMA_UART9_TX, PDMA_UART9_RX,
        //
        PDMA_USCI0_TX, PDMA_USCI0_RX,
        //
        PDMA_QSPI0_TX, PDMA_QSPI0_RX, PDMA_QSPI1_TX, PDMA_QSPI1_RX,
        //
        PDMA_SPI0_TX, PDMA_SPI0_RX, PDMA_SPI1_TX, PDMA_SPI1_RX,
        PDMA_SPI2_TX, PDMA_SPI2_RX, PDMA_SPI3_TX, PDMA_SPI3_RX,
        //
        PDMA_ACMP0, PDMA_ACMP1, PDMA_ACMP2, PDMA_ACMP3,
        //
        PDMA_EPWM0_P1_RX, PDMA_EPWM0_P2_RX, PDMA_EPWM0_P3_RX,
        PDMA_EPWM1_P1_RX, PDMA_EPWM1_P2_RX, PDMA_EPWM1_P3_RX,
        //
        PDMA_I2C0_TX, PDMA_I2C0_RX, PDMA_I2C1_TX, PDMA_I2C1_RX,
        PDMA_I2C2_TX, PDMA_I2C2_RX, PDMA_I2C3_TX, PDMA_I2C3_RX,
        //
        PDMA_I2S0_TX, PDMA_I2S0_RX, PDMA_I2S1_TX, PDMA_I2S1_RX,
        //
        PDMA_TMR0, PDMA_TMR1, PDMA_TMR2, PDMA_TMR3,
        //
        PDMA_EADC0_RX, PDMA_EADC1_RX, PDMA_DAC0_TX, PDMA_DAC1_TX,
        //
        PDMA_EPWM0_CH0_TX, PDMA_EPWM0_CH1_TX, PDMA_EPWM0_CH2_TX,
        PDMA_EPWM0_CH3_TX, PDMA_EPWM0_CH4_TX, PDMA_EPWM0_CH5_TX,
        PDMA_EPWM1_CH0_TX, PDMA_EPWM1_CH1_TX, PDMA_EPWM1_CH2_TX,
        PDMA_EPWM1_CH3_TX, PDMA_EPWM1_CH4_TX, PDMA_EPWM1_CH5_TX,
        //
        PDMA_EINT0, PDMA_EINT1, PDMA_EINT2, PDMA_EINT3,
        PDMA_EINT4, PDMA_EINT5, PDMA_EINT6, PDMA_EINT7,
        //
        PDMA_PSIO_TX, PDMA_PSIO_RX,
        PDMA_I3C0_TX, PDMA_I3C0_RX,
        PDMA_ETMC, PDMA_SWODEC,
    };
    uint32_t au32ReqSelRegCheck[] = {
        // MEM, USB TX, RX
        0, 2, 3,
        // UART0 ~ UART9
        4, 5, 6, 7,
        8, 9, 10, 11,
        12, 13, 14, 15,
        16, 17, 18, 19,
        20, 21, 22, 23,
        // USCI0
        24, 25,
        // QSPI0/1
        26, 27, 28, 29,
        // SPI0~3
        30, 31, 32, 33, 34, 35, 36, 37,
        // ACMP0~3
        38, 39, 40, 41,
        // EPWM0/1
        42, 43, 44, 45, 46, 47,
        // I2C0~4
        48, 49, 50, 51, 52, 53, 54, 55,
        // I2S0/1
        56, 57, 58, 59,
        // TMR0~3
        60, 61, 62, 63,
        // EADC0/1, DAC0/1
        64, 65, 66, 67,
        // EPWM0/1
        68, 69, 70, 71, 72, 73,
        74, 75, 76, 77, 78, 79,
        // EINT0~7
        80, 81, 82, 83, 84, 85, 86, 87,
        // PSIO, I3C
        90, 91, 92, 93,
        // ETMC, SWODEC
        94, 95,
    };
test_main:

    for (u32TestCh = 0; u32TestCh < PDMA_CH_MAX; u32TestCh++) {
        for (u32PatternIdx = 0; u32PatternIdx < sizeof(au32TransferMode) / sizeof(uint32_t); u32PatternIdx++) {
            PDMA_SetTransferMode(pdma, u32TestCh, au32TransferMode[u32PatternIdx], TRUE, 0xFFFC);
            CU_ASSERT_EQUAL(*(__IO uint32_t *)(&(pdma->REQSEL0_3) + ((u32TestCh) >> 2)) & (PDMA_REQSEL0_3_REQSRC0_Msk << ((u32TestCh % 4) * 8)), au32ReqSelRegCheck[u32PatternIdx] << ((u32TestCh % 4) * 8));
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_OPMODE_Msk, PDMA_OP_SCATTER);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].NEXT, 0xFFFC);
            PDMA_SetTransferMode(pdma, u32TestCh, au32TransferMode[u32PatternIdx], TRUE, 0x1554);
            CU_ASSERT_EQUAL(*(__IO uint32_t *)(&(pdma->REQSEL0_3) + ((u32TestCh) >> 2)) & (PDMA_REQSEL0_3_REQSRC0_Msk << ((u32TestCh % 4) * 8)), au32ReqSelRegCheck[u32PatternIdx] << ((u32TestCh % 4) * 8));
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_OPMODE_Msk, PDMA_OP_SCATTER);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].NEXT, 0x1554);
            PDMA_SetTransferMode(pdma, u32TestCh, au32TransferMode[u32PatternIdx], TRUE, 0);
            CU_ASSERT_EQUAL(*(__IO uint32_t *)(&(pdma->REQSEL0_3) + ((u32TestCh) >> 2)) & (PDMA_REQSEL0_3_REQSRC0_Msk << ((u32TestCh % 4) * 8)), au32ReqSelRegCheck[u32PatternIdx] << ((u32TestCh % 4) * 8));
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_OPMODE_Msk, PDMA_OP_SCATTER);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].NEXT, 0);
            PDMA_SetTransferMode(pdma, u32TestCh, au32TransferMode[u32PatternIdx], TRUE, 0x2AA8);
            CU_ASSERT_EQUAL(*(__IO uint32_t *)(&(pdma->REQSEL0_3) + ((u32TestCh) >> 2)) & (PDMA_REQSEL0_3_REQSRC0_Msk << ((u32TestCh % 4) * 8)), au32ReqSelRegCheck[u32PatternIdx] << ((u32TestCh % 4) * 8));
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_OPMODE_Msk, PDMA_OP_SCATTER);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].NEXT, 0x2AA8);
            PDMA_SetTransferMode(pdma, u32TestCh, au32TransferMode[u32PatternIdx], FALSE, 0);
            CU_ASSERT_EQUAL(*(__IO uint32_t *)(&(pdma->REQSEL0_3) + ((u32TestCh) >> 2)) & (PDMA_REQSEL0_3_REQSRC0_Msk << ((u32TestCh % 4) * 8)), au32ReqSelRegCheck[u32PatternIdx] << ((u32TestCh % 4) * 8));
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_OPMODE_Msk, PDMA_OP_BASIC);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].NEXT, 0x2AA8);
        }
    }

    if (pdma == PDMA0) {
        pdma = PDMA1;
        goto test_main;
    }
}

void Func_PDMA_SetBurstType()
{
    PDMA_T *pdma = PDMA0;
    uint32_t u32TestCh = 0;
    uint32_t u32TransferCountPatternIdx = 0, u32TransferCountPatternCount = 0, u32TransferCountPatternData = 0;
    uint32_t au32BurstSize[8] = {PDMA_BURST_128, PDMA_BURST_64, PDMA_BURST_32, PDMA_BURST_16, PDMA_BURST_8, PDMA_BURST_4, PDMA_BURST_2, PDMA_BURST_1};
    uint32_t au32RegCheck[8] = {0, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70};
    u32TransferCountPatternCount = sizeof(au32BurstSize) / sizeof(uint32_t);
test_main:

    for (u32TestCh = 0; u32TestCh < PDMA_CH_MAX; u32TestCh++) {
        for (u32TransferCountPatternIdx = 0; u32TransferCountPatternIdx < u32TransferCountPatternCount; u32TransferCountPatternIdx++) {
            u32TransferCountPatternData = au32BurstSize[u32TransferCountPatternIdx];
            PDMA_SetBurstType(pdma, u32TestCh, PDMA_REQ_SINGLE, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_TXTYPE_Msk, 1 << 2);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_BURSIZE_Msk, au32RegCheck[u32TransferCountPatternIdx]);
            PDMA_SetBurstType(pdma, u32TestCh, PDMA_REQ_BURST, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_TXTYPE_Msk, 0 << 2);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_BURSIZE_Msk, au32RegCheck[u32TransferCountPatternIdx]);
        }
    }

    if (pdma == PDMA0) {
        pdma = PDMA1;
        goto test_main;
    }
}

void Func_PDMA_EnableTimeout()
{
    PDMA_T *pdma = PDMA0;
    uint32_t u32TestCh = 0;
    uint32_t u32PatternIdx = 0;
    uint32_t au32EnableRegCheck[3] = {0x0000, 0x5555, 0xFFFF};
    uint32_t au32DisableRegCheck[3] = {0xFFFF, 0xAAAA, 0x0000};
test_main:

    //test case 1: enable/disable one channel each time, then check each action
    for (u32TestCh = 0; u32TestCh < PDMA_MAX_TIMEOUT_CH; u32TestCh++) {
        PDMA_EnableTimeout(pdma, 1 << u32TestCh);
        CU_ASSERT_EQUAL(pdma->TOUTEN, 1 << u32TestCh);
        PDMA_DisableTimeout(pdma, 1 << u32TestCh);
        CU_ASSERT_EQUAL(pdma->TOUTEN, 0);
    }

    //test case 2: enable/disable one channel each time, then check at final action
    for (u32TestCh = 0; u32TestCh < PDMA_MAX_TIMEOUT_CH; u32TestCh++) {
        PDMA_EnableTimeout(pdma, 1 << u32TestCh);
    }

    CU_ASSERT_EQUAL(pdma->TOUTEN, pow(2, PDMA_MAX_TIMEOUT_CH) - 1);

    for (u32TestCh = 0; u32TestCh < PDMA_MAX_TIMEOUT_CH; u32TestCh++) {
        PDMA_DisableTimeout(pdma, 1 << u32TestCh);
    }

    CU_ASSERT_EQUAL(pdma->TOUTEN, 0);

    //test case 3: enable/disable multiple channels each time
    for (u32PatternIdx = 0; u32PatternIdx < sizeof(au32EnableRegCheck) / sizeof(uint32_t); u32PatternIdx++) {
        PDMA_EnableTimeout(pdma, au32EnableRegCheck[u32PatternIdx]);
        CU_ASSERT_EQUAL(pdma->TOUTEN, au32EnableRegCheck[u32PatternIdx]);
//        if (pdma->TOUTEN != au32EnableRegCheck[u32PatternIdx]) {
//            printf("Eanble Error: pdma->TOUTEN = 0x%08X != 0x%08X\n", pdma->TOUTEN, au32EnableRegCheck[u32PatternIdx]);
//        }
    }

    for (u32PatternIdx = 0; u32PatternIdx < sizeof(au32EnableRegCheck) / sizeof(uint32_t); u32PatternIdx++) {
        PDMA_DisableTimeout(pdma, au32EnableRegCheck[u32PatternIdx]);
        CU_ASSERT_EQUAL(pdma->TOUTEN, au32DisableRegCheck[u32PatternIdx]);
//        if (pdma->TOUTEN != au32DisableRegCheck[u32PatternIdx]) {
//            printf("Disable Error: pdma->TOUTEN = 0x%08X != 0x%08X\n", pdma->TOUTEN, au32EnableRegCheck[u32PatternIdx]);
//        }
    }

    if (pdma == PDMA0) {
        pdma = PDMA1;
        goto test_main;
    }
}

void Func_PDMA_SetTimeOut()
{
    PDMA_T *pdma = PDMA0;
    uint32_t u32TestCh = 0;
    uint32_t u32PatternIdx = 0, u32PatternCount = 0, u32PatternData = 0;
    u32PatternCount = sizeof(SixteenBitsPatternTable) / sizeof(uint32_t);
test_main:

    for (u32TestCh = 0; u32TestCh < 2; u32TestCh++) {
        for (u32PatternIdx = 0; u32PatternIdx < u32PatternCount; u32PatternIdx++) {
            u32PatternData = SixteenBitsPatternTable[u32PatternIdx];
            PDMA_SetTimeOut(pdma, u32TestCh, NULL, u32PatternData);
            CU_ASSERT_EQUAL(*(__IO uint32_t *)(&(pdma->TOC0_1) + ((u32TestCh) >> 1)) & (PDMA_TOC0_1_TOC0_Msk << ((u32TestCh % 2) * 16)), u32PatternData << ((u32TestCh % 2) * 16));
        }
    }

    if (pdma == PDMA0) {
        pdma = PDMA1;
        goto test_main;
    }
}

void Func_PDMA_EnableInt()
{
    PDMA_T *pdma = PDMA0;
    uint32_t u32TestCh = 0;
test_main:

    for (u32TestCh = 0; u32TestCh < PDMA_CH_MAX; u32TestCh++) {
        PDMA_EnableInt(pdma, u32TestCh, PDMA_INT_TRANS_DONE);
        CU_ASSERT_EQUAL(pdma->INTEN, pow(2, u32TestCh + 1) - 1);
        PDMA_EnableInt(pdma, u32TestCh, PDMA_INT_TEMPTY);
        CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_TBINTDIS_Msk, 0);
    }

    for (u32TestCh = 0; u32TestCh < PDMA_CH_MAX; u32TestCh++) {
        PDMA_DisableInt(pdma, u32TestCh, PDMA_INT_TRANS_DONE);
        CU_ASSERT_EQUAL(pdma->INTEN, ((uint32_t)(pow(2, PDMA_CH_MAX) - 1) >> (u32TestCh + 1)) << (u32TestCh + 1));
        PDMA_DisableInt(pdma, u32TestCh, PDMA_INT_TEMPTY);
        CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_TBINTDIS_Msk, 1 << 7);
    }

    for (u32TestCh = 0; u32TestCh < PDMA_MAX_TIMEOUT_CH; u32TestCh++) {
        PDMA_EnableInt(pdma, u32TestCh, PDMA_INT_TIMEOUT);
        CU_ASSERT_EQUAL(pdma->TOUTIEN, pow(2, u32TestCh + 1) - 1);
    }

    for (u32TestCh = 0; u32TestCh < PDMA_MAX_TIMEOUT_CH; u32TestCh++) {
        PDMA_DisableInt(pdma, u32TestCh, PDMA_INT_TIMEOUT);
        CU_ASSERT_EQUAL(pdma->TOUTIEN, ((uint32_t)(pow(2, PDMA_MAX_TIMEOUT_CH) - 1) >> (u32TestCh + 1)) << (u32TestCh + 1));
    }

    if (pdma == PDMA0) {
        pdma = PDMA1;
        goto test_main;
    }
}

void Func_PDMA_GET_INT_STATUS()
{
    PDMA_T *pdma = PDMA0;
    uint32_t u32TestCh = 0;
test_main:

    for (u32TestCh = 0; u32TestCh < PDMA_CH_MAX; u32TestCh++) {
        PDMA_Open(pdma, 1 << u32TestCh);
        //abort interrupt flag test
        PDMA_SetTransferCnt(pdma, u32TestCh, PDMA_WIDTH_32, PDMA_TEST_LENGTH);
        PDMA_SetTransferMode(pdma, u32TestCh, PDMA_MEM, FALSE, NULL);
        PDMA_SetTransferAddr(pdma, u32TestCh, (uint32_t)SrcArray, PDMA_SAR_INC, (uint32_t)0xFFFFFFF0, PDMA_DAR_INC); // invalid system address => abort
        PDMA_Trigger(pdma, u32TestCh);

        while (PDMA_GET_INT_STATUS(pdma) != PDMA_INTSTS_ABTIF_Msk) {};

        CU_ASSERT_EQUAL(PDMA_GET_ABORT_STS(pdma), 1 << u32TestCh);

        CU_ASSERT_EQUAL(PDMA_GET_INT_STATUS(pdma) & PDMA_INTSTS_ABTIF_Msk, 1);

        PDMA_CLR_ABORT_FLAG(pdma, 1 << u32TestCh);

        CU_ASSERT_EQUAL(PDMA_GET_ABORT_STS(pdma), 0);

        CU_ASSERT_EQUAL(PDMA_GET_INT_STATUS(pdma) & PDMA_INTSTS_ABTIF_Msk, 0);

        /* Reset PDMA */
        PDMA_Reset();

        PDMA_Open(pdma, 1 << u32TestCh);

        //transfer done interrupt flag test
        PDMA_SetTransferCnt(pdma, u32TestCh, PDMA_WIDTH_32, PDMA_TEST_LENGTH);

        PDMA_SetTransferMode(pdma, u32TestCh, PDMA_MEM, FALSE, NULL);

        PDMA_SetTransferAddr(pdma, u32TestCh, (uint32_t)SrcArray, PDMA_SAR_INC, (uint32_t)DestArray, PDMA_DAR_INC);

        PDMA_EnableInt(pdma, u32TestCh, PDMA_INT_TRANS_DONE);

        PDMA_Trigger(pdma, u32TestCh);

        while (PDMA_IS_CH_BUSY(pdma, u32TestCh) == 1) {};

        CU_ASSERT_EQUAL(PDMA_GET_TD_STS(pdma), 1 << u32TestCh);

        CU_ASSERT_EQUAL(PDMA_GET_INT_STATUS(pdma) & PDMA_INTSTS_TDIF_Msk, 1 << 1);

        PDMA_CLR_TD_FLAG(pdma, 1 << u32TestCh);

        CU_ASSERT_EQUAL(PDMA_GET_TD_STS(pdma), 0);

        CU_ASSERT_EQUAL(PDMA_GET_INT_STATUS(pdma) & PDMA_INTSTS_TDIF_Msk, 0);

        PDMA_DisableInt(pdma, u32TestCh, PDMA_INT_TRANS_DONE);

        PDMA_Close(pdma);

        CU_ASSERT_EQUAL(pdma->CHCTL, 0);
    }

    if (pdma == PDMA0) {
        pdma = PDMA1;
        goto test_main;
    }
}

void Func_PDMA_CLR_TMOUT_FLAG()
{
    PDMA_T *pdma = PDMA0;
    uint32_t u32TestCh = 0;
    SYS_UnlockReg();
    /* Select UART1 clock source is HIRC */
    CLK_SetModuleClock(UART1_MODULE, CLK_UARTSEL0_UART1SEL_HIRC, CLK_UARTDIV0_UART1DIV(1));
    /* Enable UART1 peripheral clock */
    CLK_EnableModuleClock(UART1_MODULE);
    SYS_LockReg();
    SET_UART1_RXD_PA2();
    SET_UART1_TXD_PA3();
    // Must connect PA2 to PA3
test_main:

    for (u32TestCh = 0; u32TestCh < PDMA_MAX_TIMEOUT_CH; u32TestCh++) {
        UART_Open(UART1, 115200);
        UART1->INTEN |= UART_INTEN_RXPDMAEN_Msk;
        PDMA_Open(pdma, 1 << u32TestCh);
        PDMA_EnableTimeout(pdma, 1 << u32TestCh);
        //transfer done interrupt flag test
        PDMA_SetTransferCnt(pdma, u32TestCh, PDMA_WIDTH_8, PDMA_TEST_LENGTH);
        PDMA_SetTransferAddr(pdma, u32TestCh, (uint32_t)UART1_BASE, PDMA_SAR_FIX, (uint32_t)DestArray, PDMA_DAR_FIX);
        PDMA_SetBurstType(pdma, u32TestCh, PDMA_REQ_SINGLE, 0);
        PDMA_SetTimeOut(pdma, u32TestCh, TRUE, 1);
        PDMA_EnableInt(pdma, u32TestCh, PDMA_INT_TIMEOUT);
        PDMA_SetTransferMode(pdma, u32TestCh, PDMA_UART1_RX, FALSE, NULL);
        UART1->DAT = 0x55;

        while ((pdma->INTSTS & (BYTE1_Msk | BYTE2_Msk)) != BIT8 << u32TestCh) {};

        // printf("pdma->INTSTS = 0x%08X\n", pdma->INTSTS);
        CU_ASSERT_EQUAL(PDMA_GET_INT_STATUS(pdma) & (BYTE1_Msk | BYTE2_Msk), BIT8 << u32TestCh);

        PDMA_SetTimeOut(pdma, u32TestCh, FALSE, 10);

        PDMA_CLR_TMOUT_FLAG(pdma, u32TestCh);

        CU_ASSERT_EQUAL(PDMA_GET_INT_STATUS(pdma) & (BYTE1_Msk | BYTE2_Msk), 0);

        SYS_UnlockReg();

        SYS_ResetModule(SYS_UART1RST);

        SYS_LockReg();

        PDMA_DisableInt(pdma, u32TestCh, PDMA_INT_TIMEOUT);

        PDMA_SetTransferMode(pdma, u32TestCh, 0, FALSE, NULL);
    }

    PDMA_Close(pdma);
    CU_ASSERT_EQUAL(pdma->CHCTL, 0);

    if (pdma == PDMA0) {
        pdma = PDMA1;
        goto test_main;
    }
}

void Func_PDMA_SET_SRC_ADDR()
{
    PDMA_T *pdma = PDMA0;
    uint32_t u32TestCh = 0;
    uint32_t u32TransferAddressPatternIdx = 0, u32TransferAddressPatternCount = 0, u32TransferAddressPatternData = 0;
    u32TransferAddressPatternCount = sizeof(ThirtyTwoBitsPatternTable) / sizeof(uint32_t);
test_main:
    PDMA_Open(pdma, pow(2, PDMA_CH_MAX) - 1);
    CU_ASSERT_EQUAL(pdma->CHCTL, pow(2, PDMA_CH_MAX) - 1);

    for (u32TestCh = 0; u32TestCh < PDMA_CH_MAX; u32TestCh++) {
        for (u32TransferAddressPatternIdx = 0; u32TransferAddressPatternIdx < u32TransferAddressPatternCount; u32TransferAddressPatternIdx++) {
            u32TransferAddressPatternData = ThirtyTwoBitsPatternTable[u32TransferAddressPatternIdx];
            PDMA_SET_SRC_ADDR(pdma, u32TestCh, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].SA, u32TransferAddressPatternData);
            PDMA_SET_DST_ADDR(pdma, u32TestCh, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].DA, u32TransferAddressPatternData);
        }
    }

    PDMA_Close(pdma);
    CU_ASSERT_EQUAL(pdma->CHCTL, 0);

    if (pdma == PDMA0) {
        pdma = PDMA1;
        goto test_main;
    }
}

void Func_PDMA_SET_TRANS_CNT()
{
    PDMA_T *pdma = PDMA0;
    uint32_t u32TestCh = 0;
    uint32_t u32TransferCountPatternIdx = 0, u32TransferCountPatternCount = 0, u32TransferCountPatternData = 0;
    uint32_t au32RegWrite[4] = {0x1555, 0x2AAA, 0x3FFF, 1};
    u32TransferCountPatternCount = sizeof(au32RegWrite) / sizeof(uint32_t);
test_main:

    for (u32TestCh = 0; u32TestCh < PDMA_CH_MAX; u32TestCh++) {
        for (u32TransferCountPatternIdx = 0; u32TransferCountPatternIdx < u32TransferCountPatternCount; u32TransferCountPatternIdx++) {
            u32TransferCountPatternData = au32RegWrite[u32TransferCountPatternIdx];
            PDMA_SET_TRANS_CNT(pdma, u32TestCh, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(pdma->DSCT[u32TestCh].CTL & PDMA_DSCT_CTL_TXCNT_Msk, (u32TransferCountPatternData - 1) << 16);
        }
    }

    if (pdma == PDMA0) {
        pdma = PDMA1;
        goto test_main;
    }
}

void Func_PDMA_STOP()
{
    PDMA_T *pdma = PDMA0;
    uint32_t u32TestCh = 0;
test_main:

    for (u32TestCh = 0; u32TestCh < PDMA_CH_MAX; u32TestCh++) {
        PDMA_Open(pdma, 1 << u32TestCh);
        CU_ASSERT_EQUAL(pdma->CHCTL, 1 << u32TestCh);
        PDMA_Trigger(pdma, u32TestCh);
        PDMA_STOP(pdma, u32TestCh);
        CU_ASSERT_EQUAL(pdma->CHCTL, 0);
        PDMA_Close(pdma);
        CU_ASSERT_EQUAL(pdma->CHCTL, 0);
    }

    if (pdma == PDMA0) {
        pdma = PDMA1;
        goto test_main;
    }
}

void Func_PDMA_CONSTANT()
{
    /* Reset PDMA */
    PDMA_Reset();
    PDMA0->DSCT[0].CTL = PDMA_OP_BASIC;
    CU_ASSERT_EQUAL(PDMA0->DSCT[0].CTL, PDMA_OP_BASIC);
    PDMA0->DSCT[0].CTL = PDMA_OP_SCATTER;
    CU_ASSERT_EQUAL(PDMA0->DSCT[0].CTL, PDMA_OP_SCATTER);
    PDMA0->DSCT[0].CTL = PDMA_OP_STOP;
    CU_ASSERT_EQUAL(PDMA0->DSCT[0].CTL, PDMA_OP_STOP);
    //
    PDMA1->DSCT[0].CTL = PDMA_OP_BASIC;
    CU_ASSERT_EQUAL(PDMA1->DSCT[0].CTL, PDMA_OP_BASIC);
    PDMA1->DSCT[0].CTL = PDMA_OP_SCATTER;
    CU_ASSERT_EQUAL(PDMA1->DSCT[0].CTL, PDMA_OP_SCATTER);
    PDMA1->DSCT[0].CTL = PDMA_OP_STOP;
    CU_ASSERT_EQUAL(PDMA1->DSCT[0].CTL, PDMA_OP_STOP);
}

CU_TestInfo PDMA_MACRO[] = {
    {
        "Test PDMA_GET_INT_STATUS/PDMA_GET_ABORT_STS/PDMA_CLR_ABORT_FLAG/PDMA_IS_CH_BUSY\n\t\t /PDMA_GET_TD_STS/PDMA_CLR_TD_FLAG/PDMA_CLR_ABORT_FLAG/PDMA_GET_EMPTY_STS/PDMA_CLR_EMPTY_FLAG\n\t\t /Func_PDMA_Trigger:", \
        Func_PDMA_GET_INT_STATUS
    },
    {"Test PDMA_GET_INT_STATUS/PDMA_CLR_TMOUT_FLAG:", Func_PDMA_CLR_TMOUT_FLAG},
    {"Test PDMA_SET_SRC_ADDR/PDMA_SET_DST_ADDR:", Func_PDMA_SET_SRC_ADDR},
    {"Test PDMA_STOP:", Func_PDMA_STOP},

    CU_TEST_INFO_NULL
};

CU_TestInfo PDMA_API[] = {
    {"Test PDMA_Open/PDMA_Close:", Func_PDMA_Open},
    {"Test PDMA_SetTransferCnt:", Func_PDMA_SetTransferCnt},
    {"Test PDMA_SetTransferAddr:", Func_PDMA_SetTransferAddr},
    {"Test PDMA_SetTransferMode:", Func_PDMA_SetTransferMode},
    {"Test PDMA_SetBurstType:", Func_PDMA_SetBurstType},
    {"Test PDMA_EnableTimeout/PDMA_DisableTimeout:", Func_PDMA_EnableTimeout},
    {"Test PDMA_SetTimeOut:", Func_PDMA_SetTimeOut},
    {"Test PDMA_EnableInt/PDMA_DisableInt:", Func_PDMA_EnableInt},
    CU_TEST_INFO_NULL
};

CU_TestInfo PDMA_CONSTANT[] = {
    {"Test PDMA_CONSTANT:", Func_PDMA_CONSTANT},

    CU_TEST_INFO_NULL
};
