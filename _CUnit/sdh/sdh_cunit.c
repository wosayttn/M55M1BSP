/*----------------------------------------------------------------------------*/
/* Winbond Electronics Corporation confidential                               */
/*                                                                            */
/* Copyright (c) 2007 by Winbond Electronics Corporation                      */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   timer_cunit_test.c                                                                                    */
/*            The test function of VIC for CUnit.                                                          */
/* Project:   DA8205                                                                                       */
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
#include "sdh_cunit.h"

/*----------------------------------------------------------------------------*/
/* Includes of local headers                                                  */
/*----------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"
#include "PinConfig.h"
#include "common.h"

//------------------------------------------------------------------------------
//#define D_msg   printf
#define D_msg(...)

//#define TEST_UNKNOWN 1
#define TEST_SDHC  1
//#define TEST_SD    1
//#define TEST_eMMC  1
//#define TEST_MMC   1

#define C_SDH0          0
#define C_SDH1          1

//------------------------------------------------------------------------------
extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;

//------------------------------------------------------------------------------
SDH_T *gpasSDHModule[] =
{
    SDH0,
    SDH1,
};

const uint32_t au32CardDetectSrc[2] =
{
    CardDetect_From_GPIO,
    CardDetect_From_DAT3,
};

#ifdef __ICCARM__
__align(32) static volatile uint8_t WriteBuffer[SDH_BLOCK_SIZE] = {0};
__align(32) static volatile uint8_t ReadBuffer[SDH_BLOCK_SIZE] = {0};
#else
__attribute__((aligned(32))) static volatile uint8_t WriteBuffer[SDH_BLOCK_SIZE] = {0};
__attribute__((aligned(32))) static volatile uint8_t ReadBuffer[SDH_BLOCK_SIZE] = {0};
#endif

static uint32_t gu32SDHModuleIdx = 0;

/*----------------------------------------------------------------------------*/
/* Test function                                                              */
/*----------------------------------------------------------------------------*/
void SetSDHModuleIdx(uint32_t u32SetValue)
{
    gu32SDHModuleIdx = u32SetValue;
}

uint32_t GetSDHModuleIdx(void)
{
    return gu32SDHModuleIdx;
}

void *GetSDHModule(uint32_t u32Index)
{
    return gpasSDHModule[u32Index];
}

int suite_success_init(void)
{
    return 0;
}
int suite_success_clean(void)
{
    return 0;
}

void SDH_ModuleInit(uint32_t u32SDHModule)
{
    switch (u32SDHModule)
    {
        case C_SDH0:
            SDH0_CD_PIN_INIT();
            SDH0_CLK_PIN_INIT();
            SDH0_CMD_PIN_INIT();
            SDH0_DAT0_PIN_INIT();
            SDH0_DAT1_PIN_INIT();
            SDH0_DAT2_PIN_INIT();
            SDH0_DAT3_PIN_INIT();

#if defined __PLDM_EMU_EMMC__
            SET_GPIO_PD13();
            GPIO_SetMode(PD, BIT13, GPIO_MODE_INPUT);
            PD13 = 0;
#endif //__PLDM_EMU_EMMC__
            break;

        case C_SDH1:
            SDH1_CD_PIN_INIT();
            SDH1_CLK_PIN_INIT();
            SDH1_CMD_PIN_INIT();
            SDH1_DAT0_PIN_INIT();
            SDH1_DAT1_PIN_INIT();
            SDH1_DAT2_PIN_INIT();
            SDH1_DAT3_PIN_INIT();

#if defined __PLDM_EMU_EMMC__
            SET_GPIO_PA6();
            GPIO_SetMode(PA, BIT6, GPIO_MODE_INPUT);
            PA6 = 1;
#endif //__PLDM_EMU_EMMC__
            break;
    }
}

void SDH_ClockDisable(uint32_t u32SDHModule)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    switch (u32SDHModule)
    {
        case C_SDH0:
            //CLK->SDHCTL &= ~(CLK_SDHCTL_SDH0CKEN_Msk);
            CLK_DisableModuleClock(SDH0_MODULE);
            break;

        case C_SDH1:
            //CLK->SDHCTL &= ~(CLK_SDHCTL_SDH1CKEN_Msk);
            CLK_DisableModuleClock(SDH1_MODULE);
            break;
    }
}

void SDH_ClockEnable(uint32_t u32SDHModule)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    SDH_ClockDisable(u32SDHModule);

    switch (u32SDHModule)
    {
        case C_SDH0:
            //CLK->SDHCTL |= (CLK_SDHCTL_SDH0CKEN_Msk);
            CLK_EnableModuleClock(SDH0_MODULE);
            break;

        case C_SDH1:
            //CLK->SDHCTL |= (CLK_SDHCTL_SDH1CKEN_Msk);
            CLK_EnableModuleClock(SDH1_MODULE);
            break;
    }
}

void SelectSDHClockSrc(uint32_t u32SDHModule)
{
#if 0
    int i32Index = 0;
    struct
    {
        char *chName;
    } S_SDHClkSrc[] =
    {
        {"HXT"},
        {"PLLx_DIV2"},
        {"HCLK"},
        {"HIRC"},
        {"HIRC 48Mhz"},
    };

    printf("===========================================================\r\n");
    printf("Set SDH Clock Source :\r\n");
    printf("===========================================================\r\n");

    for (i32Index = 0; i32Index < sizeof(S_SDHClkSrc) / sizeof(S_SDHClkSrc[0]); i32Index++)
    {
        printf("[%d]%s\r\n",
               i32Index,
               S_SDHClkSrc[i32Index].chName);
    }

    printf("\r\n");

    i32Index = sysGetNum();

    if (i32Index > sizeof(S_SDHClkSrc) / sizeof(S_SDHClkSrc[0]))
    {
        i32Index = 0;
    }

    /* Enable SDH clock */
    SDH_ClockEnable(u32SDHModule);

    /* Unlock protected registers */
    SYS_UnlockReg();

    switch (u32SDHModule)
    {
        case C_SDH0:
            CLK_SetModuleClock(SDH0_MODULE,
                               (i32Index << CLK_SDHSEL_SDH0SEL_Pos),
                               CLK_SDHDIV_SDH0DIV(10));
            break;

        case C_SDH1:
            CLK_SetModuleClock(SDH1_MODULE,
                               (i32Index << CLK_SDHSEL_SDH1SEL_Pos),
                               CLK_SDHDIV_SDH1DIV(10));
            break;
    }

#endif //0

    /* Enable SDH clock */
    SDH_ClockEnable(u32SDHModule);

    switch (u32SDHModule)
    {
        case C_SDH0:
            CLK_SetModuleClock(SDH0_MODULE,
                               (0 << CLK_SDHSEL_SDH0SEL_Pos),
                               CLK_SDHDIV_SDH0DIV(10));
            break;

        case C_SDH1:
            CLK_SetModuleClock(SDH1_MODULE,
                               (0 << CLK_SDHSEL_SDH1SEL_Pos),
                               CLK_SDHDIV_SDH1DIV(10));
            break;
    }
}

int SDH_ModuleSelect(void)
{
    uint32_t u32Index = 0;
    S_TestOption sTestPort[] =
    {
        {"SDH0"},
        {"SDH1"},
    };

    u32Index = GetRequireOptions(sTestPort, sizeof(sTestPort) / sizeof(sTestPort[0]));

    SetSDHModuleIdx(u32Index);

    SDH_ModuleInit(u32Index);
    SelectSDHClockSrc(u32Index);

    return (int)u32Index;
}
/*----------------------------------------------------------------------------*/
/* Function: function_name                                                    */
/*                                                                            */
/* Parameters:                                                                */
/*               param - [in/out], parameter_description                      */
/*               param - [in/out], parameter_description                      */
/*                                                                            */
/* Returns:      the_value                                                    */
/* Side effects: the_effect                                                   */
/* Description:                                                               */
/*               description                                                  */
/*----------------------------------------------------------------------------*/
void BasicTestISRHandle(SDH_T *pSDHx, SDH_INFO_T *pSDHxInfo)
{
    volatile uint32_t isr = 0;

    // FMI data abort interrupt
    if (pSDHx->GINTSTS & SDH_GINTSTS_DTAIF_Msk)
    {
        /* ResetAllEngine() */
        pSDHx->GCTL |= SDH_GCTL_GCTLRST_Msk;
        pSDHx->GINTSTS |= SDH_GINTSTS_DTAIF_Msk;
        printf("SDH Data Abort\r\n");
    }

    //----- SD interrupt status
    isr = pSDHx->INTSTS;

    if (isr & SDH_INTSTS_BLKDIF_Msk)
    {
        // block down
        //         extern uint8_t volatile _SDH0_SDDataReady;
        //         _SDH0_SDDataReady = TRUE;
        pSDHxInfo->DataReadyFlag = (uint8_t)TRUE;
        pSDHx->INTSTS |= SDH_INTSTS_BLKDIF_Msk;

        printf("SDH Block transfer done\r\n");
    }

    if (isr & SDH_INTSTS_CDIF_Msk)   // port 0 card detect
    {
        //----- SD interrupt status
        // it is work to delay 50 times for SD_CLK = 200KHz
        {
            int volatile i;         // delay 30 fail, 50 OK

            for (i = 0; i < 0x500; i++); // delay to make sure got updated value from REG_SDISR.

            isr = pSDHx->INTSTS;
        }

        if (((pSDHx->INTEN & SDH_INTEN_CDSRC_Msk) >> SDH_INTEN_CDSRC_Pos) == 0)
        {
            if ((isr & ~SDH_INTSTS_CDSTS_Msk) == ~SDH_INTSTS_CDSTS_Msk)
            {
                pSDHxInfo->IsCardInsert = FALSE;
                memset(pSDHxInfo, 0, sizeof(SDH_INFO_T));
            }
        }
        else
        {
            if (isr & SDH_INTSTS_CDSTS_Msk)
            {
                pSDHxInfo->IsCardInsert = FALSE;
                memset(pSDHxInfo, 0, sizeof(SDH_INFO_T));
            }
        }

        //#ifdef _USE_DAT3_DETECT_
        //
        //        if ((isr & ~SDH_INTSTS_CDSTS_Msk) == ~SDH_INTSTS_CDSTS_Msk)
        //        {
        //#else
        //
        //        if (isr & SDH_INTSTS_CDSTS_Msk)
        //        {
        //#endif // _USE_DAT3_DETECT_
        //            pSDHxInfo->IsCardInsert = FALSE;
        //            memset(pSDHxInfo, 0, sizeof(SDH_INFO_T));
        //        }

        //        else
        //        {
        //#ifdef _USE_DAT3_DETECT_
        //            SDH_Open(pSDHx, CardDetect_From_DAT3);
        //#else
        //            SDH_Open(pSDHx, CardDetect_From_GPIO);
        //#endif //_USE_DAT3_DETECT_
        //#if defined __PLDM_EMU_EMMC__
        //            SDH_eMMCProbe(pSDHx);
        //#else
        //            SDH_Probe(pSDHx);
        //#endif //__PLDM_EMU_EMMC__
        //        }

        pSDHx->INTSTS |= SDH_INTSTS_CDIF_Msk;
    }

    // CRC error interrupt
    if (isr & SDH_INTSTS_CRCIF_Msk)
    {
        if (!(isr & SDH_INTSTS_CRC16_Msk))
        {
            printf("***** ISR sdioIntHandler(): CRC_16 error !\n");
            // handle CRC error
            pSDHx->INTSTS |= SDH_INTSTS_CRC16_Msk;
        }
        else if (!(isr & SDH_INTSTS_CRC7_Msk))
        {
            //extern uint32_t _SDH0_uR3_CMD;

            //if (!_SDH0_uR3_CMD)
            if (!pSDHxInfo->R3Flag)
            {
                printf("***** ISR sdioIntHandler(): CRC_7 error !\n");
                // handle CRC error
            }
        }

        pSDHx->INTSTS |= SDH_INTSTS_CRCIF_Msk;      // clear interrupt flag
    }

    if (isr & SDH_INTSTS_DITOIF_Msk)
    {
        printf("***** ISR: data in timeout !\n");
        pSDHx->INTSTS |= SDH_INTSTS_DITOIF_Msk;
    }

    // Response in timeout interrupt
    if (isr & SDH_INTSTS_RTOIF_Msk)
    {
        printf("***** ISR: response in timeout !\n");
        pSDHx->INTSTS |= SDH_INTSTS_RTOIF_Msk;
    }
}

void SDH0_IRQHandler()
{
    SDH_INFO_T *pSDH0Info = (SDH_INFO_T *)SDH_GetSDInfoMsg(SDH0);

    printf("112");
    BasicTestISRHandle(SDH0, pSDH0Info);
}

void SDH1_IRQHandler()
{
    SDH_INFO_T *pSDH1Info = (SDH_INFO_T *)SDH_GetSDInfoMsg(SDH1);
    printf("113");
    BasicTestISRHandle(SDH1, pSDH1Info);
}

void SDH_Show_info(SDH_T *pSDHx)
{
    SDH_INFO_T *pSDHxInfo = NULL;//(SDH_INFO_T *)GetSDHInfo(u32SDHModule);

    pSDHxInfo = (SDH_INFO_T *)SDH_GetSDInfoMsg(pSDHx);

    // get information about SD card
    SDH_Get_SD_info(pSDHx);

    // show card type
    switch (pSDHxInfo->CardType)
    {
        case SDH_TYPE_SD_HIGH:
            printf("    SDHC card insert to SD port!\n");
            break;

        case SDH_TYPE_SD_LOW:
            printf("    SD1.1 card insert to SD port!\n");
            break;

        case SDH_TYPE_MMC:
            printf("    MMC card insert to SD port!\n");
            break;

        default:
            printf("    Unknown card insert to SD port!\n");
            break;
    }

    printf("    SD Card size = %d Kbytes\n",
           pSDHxInfo->diskSize);
    printf("                 = %d bytes * %d sectors\n",
           pSDHxInfo->sectorSize,
           pSDHxInfo->totalSectorN);

    return;
}

unsigned int is_sd_card_insert(SDH_T *pSDHx)
{
    if (((pSDHx->INTEN & 0x40000000) >> 30) == 0)  // for DAT3 mode
    {
        return ((pSDHx->INTSTS & 0x10000) ? TRUE : FALSE);
    }
    else    // for GPIO mode
    {
        return ((pSDHx->INTSTS & 0x10000) ? FALSE : TRUE);
    }
}

void SDH_ClearCardDetect(SDH_T *pSDHx)
{
    SDH_INFO_T *pSDHxInfo = NULL;

    pSDHxInfo = (SDH_INFO_T *)SDH_GetSDInfoMsg(pSDHx);

    if (is_sd_card_insert(pSDHx))
    {
        // ACMD42 to disconnect pull-up resistor inside the card for DAT3
        if (pSDHxInfo->CardType != SDH_TYPE_MMC) // MMC card don't accept ACMD42
        {
            //pSDHx->CTL &= ~SDH_CTL_SDPORT_Msk;
            //printf("send ACMD42 to SDPort!\n");
            SDH_SDCmdAndRsp(pSDHx, 7, pSDHxInfo->RCA, 0);
            SDH_CheckRB(pSDHx);
            SDH_SDCmdAndRsp(pSDHx, 55, pSDHxInfo->RCA, 0);
            SDH_SDCmdAndRsp(pSDHx, 42, 0, 0);
            SDH_SDCommand(pSDHx, 7, 0);
            //printf("send ACMD42 Done!\n");
        }
    }
    else
    {
        printf("\nSD card removed ! Do nothing !\n");
        return;
    }
}

void API_SDH_Function(void)
{
    uint32_t j = 0, k = 0;
    volatile uint32_t u32Delay = 0;
    uint8_t *pu8WriteBuf = NULL;
    uint8_t *pu8ReadBuf = NULL;
    SDH_T *pSDHModule = NULL;
    SDH_INFO_T *pSDHInfo = NULL;

    pSDHModule = (SDH_T *)GetSDHModule(GetSDHModuleIdx());
    pSDHInfo = (SDH_INFO_T *)SDH_GetSDInfoMsg(pSDHModule);

    pu8WriteBuf = (uint8_t *)&WriteBuffer[0];
    pu8ReadBuf = (uint8_t *)&ReadBuffer[0];

    for (k = 0; k < SDH_BLOCK_SIZE; k++)
    {
        pu8WriteBuf[k] = (k & 0xFF); //rand() & 0xFF;
    }

    for (j = 0; j < 2; j++)
    {
        memset(pSDHInfo, 0, sizeof(SDH_INFO_T));

        SDH_Enable_Int(pSDHModule);

        SDH_Open(pSDHModule, au32CardDetectSrc[j]);
        SDH_Probe(pSDHModule);

        SDH_Show_info(pSDHModule);

        SDH_ClearCardDetect(pSDHModule);

        pSDHModule->INTEN &= ~SDH_INTEN_CDIEN_Msk;

        CU_ASSERT_EQUAL((pSDHModule->GCTL & SDH_GCTL_SDEN_Msk), (1 << 1));
        CU_ASSERT_EQUAL(SDH_IS_CARD_PRESENT(pSDHModule), 1);
        CU_ASSERT(SDH_GET_CARD_CAPACITY(pSDHModule) > 0);

#ifdef TEST_SDHC
        CU_ASSERT_EQUAL(pSDHInfo->CardType, SDH_TYPE_SD_HIGH);
#elif defined TEST_SD
        CU_ASSERT_EQUAL(pSDHInfo->CardType, SDH_TYPE_SD_LOW);
#elif defined TEST_eMMC
        CU_ASSERT_EQUAL(pSDHInfo->CardType, SDH_TYPE_EMMC);
#elif defined TEST_MMC
        CU_ASSERT_EQUAL(pSDHInfo->CardType, SDH_TYPE_MMC);
#elif define TEST_UNKNOWN
        CU_ASSERT_EQUAL(pSDHInfo->CardType, SDH_TYPE_UNKNOWN);
#endif

        CU_ASSERT(pSDHInfo->sectorSize > 0);
        CU_ASSERT(pSDHInfo->totalSectorN > 0);

        CU_ASSERT(SDH_Write(pSDHModule, (uint8_t *)pu8WriteBuf, 1, 1) == 0);

        memset(pu8ReadBuf, 0x11, SDH_BLOCK_SIZE);

        CU_ASSERT(SDH_Read(pSDHModule, (uint8_t *)pu8ReadBuf, 1, 1) == 0);

        CU_ASSERT(memcmp((uint8_t *)pu8WriteBuf, (uint8_t *)pu8ReadBuf, 128) == 0);
    }
}

void CONSTANT_SD(void)
{
    CU_ASSERT(SDH_ERR_ID == 0xFFFF0100ul);
    CU_ASSERT(SDH_TIMEOUT == (SDH_ERR_ID | 0x01ul));
    CU_ASSERT(SDH_NO_MEMORY == (SDH_ERR_ID | 0x02ul));

    CU_ASSERT(Successful == 0ul);
    CU_ASSERT(Fail == 1ul);

    CU_ASSERT(SDH_TYPE_UNKNOWN == (0ul));
    CU_ASSERT(SDH_TYPE_SD_HIGH == (1ul));
    CU_ASSERT(SDH_TYPE_SD_LOW == (2ul));
    CU_ASSERT(SDH_TYPE_MMC == (3ul));
    CU_ASSERT(SDH_TYPE_EMMC == (4ul));

    CU_ASSERT(SDH_NO_SD_CARD == (SDH_ERR_ID | 0x10ul));
    CU_ASSERT(SDH_ERR_DEVICE == (SDH_ERR_ID | 0x11ul));
    CU_ASSERT(SDH_INIT_TIMEOUT == (SDH_ERR_ID | 0x12ul));
    CU_ASSERT(SDH_SELECT_ERROR == (SDH_ERR_ID | 0x13ul));
    CU_ASSERT(SDH_WRITE_PROTECT == (SDH_ERR_ID | 0x14ul));
    CU_ASSERT(SDH_INIT_ERROR == (SDH_ERR_ID | 0x15ul));
    CU_ASSERT(SDH_CRC7_ERROR == (SDH_ERR_ID | 0x16ul));
    CU_ASSERT(SDH_CRC16_ERROR == (SDH_ERR_ID | 0x17ul));
    CU_ASSERT(SDH_CRC_ERROR == (SDH_ERR_ID | 0x18ul));
    CU_ASSERT(SDH_CMD8_ERROR == (SDH_ERR_ID | 0x19ul));

    CU_ASSERT(MMC_FREQ == (20000ul));
    CU_ASSERT(SD_FREQ == (25000ul));
    CU_ASSERT(SDHC_FREQ == (50000ul));

    CU_ASSERT(SD_PORT0 == (1UL << 0));
    CU_ASSERT(SD_PORT1 == (1UL << 2));
    CU_ASSERT(CardDetect_From_GPIO == (1ul << 8));
    CU_ASSERT(CardDetect_From_DAT3 == (1ul << 9));

    CU_ASSERT(SDH_OK == (0L));
    CU_ASSERT(SDH_ERR_FAIL == (-1L));
    CU_ASSERT(SDH_ERR_TIMEOUT == (-2L));

    CU_ASSERT(SDH_BLOCK_SIZE == (512ul));
}

const uint32_t au32SINTEN[11] =
{
    SDH_INTEN_BLKDIEN_Msk,
    SDH_INTEN_CRCIEN_Msk,
    SDH_INTEN_CDIEN_Msk,
    SDH_INTEN_RTOIEN_Msk,
    SDH_INTEN_DITOIEN_Msk,
    SDH_INTEN_WKIEN_Msk,
    SDH_INTEN_CDSRC_Msk,
};

void MACRO_SD(void)
{
    uint32_t i;
    SDH_T *pSDHModule = NULL;
    SDH_INFO_T *pSDHInfo = NULL;

    pSDHModule = (SDH_T *)GetSDHModule(GetSDHModuleIdx());
    pSDHInfo = (SDH_INFO_T *)SDH_GetSDInfoMsg(pSDHModule);

    memset(pSDHInfo, 0, sizeof(SDH_INFO_T));

    SDH_Open(pSDHModule, CardDetect_From_GPIO);
    SDH_Enable_Int(pSDHModule);

    SDH_Probe(pSDHModule);

    CU_ASSERT(SDH_GET_INT_FLAG(pSDHModule, SDH_INTSTS_CDSTS_Msk) == 0);

    for (i = 0; i < 11; i++)
    {
        SDH_ENABLE_INT(pSDHModule, au32SINTEN[i]);
        CU_ASSERT_EQUAL((pSDHModule->INTEN & au32SINTEN[i]), au32SINTEN[i]);

        SDH_DISABLE_INT(pSDHModule, au32SINTEN[i]);
        CU_ASSERT_EQUAL((pSDHModule->INTEN & au32SINTEN[i]), 0);
    }
}

CU_TestInfo SDH_CONSTANT_MACRO[] =
{
    {"Check SDH CONSTANT ", CONSTANT_SD},
    {"Check SDH MACRO ", MACRO_SD},

    CU_TEST_INFO_NULL
};

CU_TestInfo SDH_API[] =
{
    {"Check SD Probe Function", API_SDH_Function},

    CU_TEST_INFO_NULL
};

CU_SuiteInfo suites[] =
{
    {"SD CONSTANT & MACRO", suite_success_init, suite_success_clean, NULL, NULL, SDH_CONSTANT_MACRO},
    {"SD API", suite_success_init, suite_success_clean, NULL, NULL, SDH_API},
    CU_SUITE_INFO_NULL
};
