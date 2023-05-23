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

//------------------------------------------------------------------------------
//#define D_msg   printf
#define D_msg(...)

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

/*----------------------------------------------------------------------------*/
/* Test function                                                              */
/*----------------------------------------------------------------------------*/
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

CU_SuiteInfo suites[] =
{
    {"SD CONSTANT & MACRO", suite_success_init, suite_success_clean, NULL, NULL, SDH_CONSTANT_MACRO},
    {"SD API", suite_success_init, suite_success_clean, NULL, NULL, SDH_API},
    CU_SUITE_INFO_NULL
};

//#define TEST_UNKNOWN 1
#define TEST_SDHC  1
//#define TEST_SD    1
//#define TEST_eMMC  1
//#define TEST_MMC   1

uint32_t WriteBuffer[128];  // 512 bytes
uint32_t ReadBuffer[128]; // 512 bytes

void BasicTestISRHandle(SDH_T *pSDH, SDH_INFO_T *pSDH_Info)
{
    unsigned int volatile isr;
    unsigned int volatile ier;

    // FMI data abort interrupt
    if (pSDH->DMAINTSTS & SDH_DMAINTSTS_ABORTIF_Msk)
    {
        /* ResetAllEngine() */
        pSDH->DMACTL |= SDH_DMACTL_DMARST_Msk;
        pSDH->DMAINTSTS = SDH_DMAINTSTS_ABORTIF_Msk;
    }

    //----- SD interrupt status
    isr = pSDH->INTSTS;

    if (isr & SDH_INTSTS_BLKDIF_Msk)     // block down
    {
        pSDH_Info->DataReadyFlag = (uint8_t)TRUE;

        pSDH->INTSTS = SDH_INTSTS_BLKDIF_Msk;
    }

    if (isr & SDH_INTSTS_CDIF_Msk)   // port 0 card detect
    {
        //----- SD interrupt status
        // it is work to delay 50 times for SDH_CLK = 200KHz
        {
            volatile int i;         // delay 30 fail, 50 OK

            for (i = 0; i < 500; i++); // delay to make sure got updated value from REG_SDISR.

            isr = pSDH->INTSTS;
        }

        if (pSDH->INTEN & SDH_INTEN_CDSRC_Msk) // from GPIO
        {
            if (isr & SDH_INTSTS_CDSTS_Msk)
            {
                pSDH_Info->IsCardInsert = FALSE;   // SDISR_CD_Card = 1 means card remove for GPIO mode
                memset(pSDH_Info, 0, sizeof(SDH_INFO_T));
            }
            else
            {
                SDH_Open(pSDH, CardDetect_From_GPIO);
                SDH_Probe(pSDH);
            }
        }

        //        else    // from DAT3
        //        {
        //            if (!(isr & SDH_INTSTS_CDSTS0_Msk)) {
        //                SD0.IsCardInsert = FALSE;
        //                memset(&SD0, 0, sizeof(SDH_INFO_T));
        //            } else {
        //                SDH_Open(pSDH, CardDetect_From_DAT3);
        //                SDH_Probe(pSDH);
        //            }
        //        }

        CU_ASSERT((pSDH->INTSTS & SDH_INTSTS_CDIF_Msk) == SDH_INTSTS_CDIF_Msk);
        pSDH->INTSTS = SDH_INTSTS_CDIF_Msk;
        CU_ASSERT_EQUAL((pSDH->INTSTS & SDH_INTSTS_CDIF_Msk), 0);
    }

    // CRC error interrupt
    if (isr & SDH_INTSTS_CRCIF_Msk)
    {
        if (!(isr & SDH_INTSTS_CRC16_Msk))
        {
            //printf("***** ISR sdioIntHandler(): CRC_16 error !\n");
            // handle CRC error
        }
        else if (!(isr & SDH_INTSTS_CRC7_Msk))
        {
            extern uint32_t _sd_uR3_CMD;

            if (! _sd_uR3_CMD)
            {
                //printf("***** ISR sdioIntHandler(): CRC_7 error !\n");
                // handle CRC error
            }
        }

        pSDH->INTSTS = SDH_INTSTS_CRCIF_Msk;      // clear interrupt flag
    }
}

void SDH0_IRQHandler()
{
    SDH_INFO_T *pSDH0Info = (SDH_INFO_T *)GetSDHInfoMsg(SDH0);

    BasicTestISRHandle(SDH0, pSDH0Info);
}

void SDH1_IRQHandler()
{
    SDH_INFO_T *pSDH1Info = (SDH_INFO_T *)GetSDHInfoMsg(SDH1);

    BasicTestISRHandle(SDH1, pSDH1Info);
}

const uint32_t au32CardDetectSrc[2] =
{
    CardDetect_From_GPIO,
    CardDetect_From_DAT3,
};

void API_SDH_Function(void)
{
    uint32_t j, k;
    uint32_t u32ModuleIdx = 0;
    SDH_T *pSDHModule = NULL;
    SDH_INFO_T *pSDHInfo = NULL;

    for (u32ModuleIdx = 0; u32ModuleIdx < sizeof(gpasSDHModule) / sizeof(SDH_T *); u32ModuleIdx++)
    {
        pSDHModule = (SDH_T *)GetSDHModule(u32ModuleIdx);
        pSDHInfo = (SDH_INFO_T *)GetSDHInfoMsg(pSDHModule);

        for (j = 0; j < 2; j++)
        {
            k = 0;

            while (k < 512)
            {
                WriteBuffer[k / 4] = (k << 0) |
                                     ((k + 1) << 8) |
                                     ((k + 2) << 16) |
                                     ((k + 3) << 24);
                ReadBuffer[k / 4] = 0x0;

                k = k + 4;
            }

            memset(pSDHInfo, 0, sizeof(SDH_INFO_T));
            //memset(&SD1, 0, sizeof(SDH_INFO_T));

            SDH_Open(pSDHModule, au32CardDetectSrc[j]);
            SDH_Probe(pSDHModule);

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

            SDH_Write(pSDHModule, (uint8_t *)WriteBuffer, 2, 1);
            SDH_Read(pSDHModule, (uint8_t *)ReadBuffer, 2, 1);

            CU_ASSERT_EQUAL(memcmp(WriteBuffer, ReadBuffer, sizeof(WriteBuffer)), 0);
        }
    }
}

void CONSTANT_SD(void)
{
    CU_ASSERT_EQUAL(SDH_ERR_ID, 0xFFFF0100);
    CU_ASSERT_EQUAL(SDH_TIMEOUT, 0xFFFF0101);
    CU_ASSERT_EQUAL(SDH_NO_MEMORY, 0xFFFF0102);

    CU_ASSERT_EQUAL(Successful, 0);
    CU_ASSERT_EQUAL(Fail, 1);
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
    uint32_t u32ModuleIdx = 0;
    SDH_T *pSDHModule = NULL;
    SDH_INFO_T *pSDHInfo = NULL;

    for (u32ModuleIdx = 0; u32ModuleIdx < sizeof(gpasSDHModule) / sizeof(SDH_T *); u32ModuleIdx++)
    {
        pSDHModule = (SDH_T *)GetSDHModule(u32ModuleIdx);
        pSDHInfo = (SDH_INFO_T *)GetSDHInfoMsg(pSDHModule);

        memset(pSDHInfo, 0, sizeof(SDH_INFO_T));
        //memset(&SD1, 0, sizeof(SDH_INFO_T));

        SDH_Open(pSDHModule, CardDetect_From_GPIO);
        SDH_Probe(pSDHModule);

        CU_ASSERT_EQUAL(SDH_GET_INT_FLAG(pSDHModule, SDH_INTSTS_CDSTS_Msk), 0);

        for (i = 0; i < 11; i++)
        {
            SDH_ENABLE_INT(pSDHModule, au32SINTEN[i]);
            CU_ASSERT_EQUAL((pSDHModule->INTEN & au32SINTEN[i]), au32SINTEN[i]);

            SDH_DISABLE_INT(pSDHModule, au32SINTEN[i]);
            CU_ASSERT_EQUAL((pSDHModule->INTEN & au32SINTEN[i]), 0);
        }
    }
}

CU_TestInfo SDH_CONSTANT_MACRO[] =
{
    {"Check RTC CONSTANT ", CONSTANT_SD},
    {"Check RTC CONSTANT ", MACRO_SD},

    CU_TEST_INFO_NULL
};

CU_TestInfo SDH_API[] =
{
    {"Check SD Probe Function", API_SDH_Function},

    CU_TEST_INFO_NULL
};
