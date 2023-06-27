/**************************************************************************//**
 * @file     spim_cunit.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 15/09/15 4:36p $
 * @brief    NUC4xx series SPIM CUnit test function.
 *
 * @note
 * Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

//* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "spim_cunit.h"
#include "octal_cunit.h"
#include "common.h"

//------------------------------------------------------------------------------
/* 0x82 : CMD_OCTAL_PAGE_PROG_MICRON Command Phase Table */
static SPIM_PHASE_T gMU_82h_WrCMD =
{
    CMD_OCTAL_PAGE_PROG_MICRON,                            //Command Code
    PHASE_NORMAL_MODE, PHASE_WIDTH_8, PHASE_DISABLE_DTR,       //Command Phase
    PHASE_NORMAL_MODE, PHASE_WIDTH_32, PHASE_DISABLE_DTR,        //Address Phase
    PHASE_OCTAL_MODE, PHASE_ORDER_MODE0, PHASE_DISABLE_DTR,    //Data Phase
};

/* 0xCB : CMD_OCTAL_FAST_IO_READ Command Phase Table */
static SPIM_PHASE_T gMU_CBh_RdCMD =
{
    CMD_OCTAL_FAST_IO_READ,                            // Command Code
    PHASE_OCTAL_MODE, PHASE_WIDTH_16, PHASE_ENABLE_DTR,    // Command Phase
    PHASE_OCTAL_MODE, PHASE_WIDTH_32, PHASE_ENABLE_DTR,    // Address Phase
    PHASE_OCTAL_MODE, PHASE_ORDER_MODE0, PHASE_ENABLE_DTR, // Data Phase
    16,                                                // Dummy Cycle Number
};

//------------------------------------------------------------------------------
int SPIM_Tests_OcatlInit(void)
{
    uint8_t idBuf[3] = {0};
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 2);

    if (SPIM_InitFlash(pSPIMModule, 1) != 0)        /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        CU_FAIL();
    }

    SPIM_ReadJedecId(pSPIMModule, idBuf, sizeof(idBuf), 1, 0, 0);
    printf("SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n", idBuf[0], idBuf[1], idBuf[2]);

    //if ((idBuf[0] != MFGID_WINBOND) &&
    //        (idBuf[0] != MFGID_MXIC) &&
    //        (idBuf[0] != MFGID_EON) &&
    //        (idBuf[0] != MFGID_ISSI) &&
    //        (idBuf[0] != MFGID_SPANSION) &&
    //        (idBuf[0] != MFGID_MICRON))
    //{
    //    CU_FAIL();
    //}
    //else
    //{
    //    CU_PASS();
    //}

    return 0;
}

int SPIM_Tests_OctalClean(void)
{
    return 0;
}

void MT35x_EnterOctalDDRMode(SPIM_T *pSPIMx)
{
    uint8_t u8CMDBuf[1] = {0xE7};

    SPIM_MT35x_4Bytes_Enable(pSPIMx, 0, 1, 0);

    //Enter Octal DDR Mode
    SPIM_IO_SendCMDPhase(pSPIMx, SPIM_IO_WRITE_PHASE, OPCODE_WR_VCONFIG, PHASE_NORMAL_MODE, 0);
    SPIM_IO_SendAddrPhase(pSPIMx, 0, 0x00, PHASE_NORMAL_MODE, 0);
    SPIM_IO_SendDataPhase(pSPIMx, SPIM_IO_WRITE_PHASE, u8CMDBuf, 1, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, 0);

    //Read volatile configure register value
    SPIM_MT35x_4Bytes_Enable(pSPIMx, 1, 8, 1);
    SPIM_IO_SendCMDPhase(pSPIMx, SPIM_IO_READ_PHASE, OPCODE_RD_VCONFIG, PHASE_OCTAL_MODE, 1);
    SPIM_IO_SendAddrPhase(pSPIMx, 1, 0x00, PHASE_OCTAL_MODE, 1);
    SPIM_IO_SendDummyCycle(pSPIMx, 16);
    SPIM_IO_SendDataPhase(pSPIMx, SPIM_IO_READ_PHASE, u8CMDBuf, 1, PHASE_NORMAL_MODE, PHASE_OCTAL_MODE, 1);

    printf("Octal Mode = 0x%X\r\n", u8CMDBuf[0]);
}

void MT35x_ExitOctalDDRMode(SPIM_T *pSPIMx)
{
    uint8_t u8CMDBuf[1] = {0xFF};

    //Exit Octal DDR Mode
    SPIM_IO_SendCMDPhase(pSPIMx, SPIM_IO_WRITE_PHASE, OPCODE_WR_VCONFIG, PHASE_OCTAL_MODE, 1);
    SPIM_IO_SendAddrPhase(pSPIMx, 1, 0x00, PHASE_OCTAL_MODE, 1);
    SPIM_IO_SendDataPhase(pSPIMx, SPIM_IO_WRITE_PHASE, u8CMDBuf, 1, PHASE_OCTAL_MODE, PHASE_OCTAL_MODE, 1);

    SPIM_MT35x_4Bytes_Enable(pSPIMx, 0, 1, 0);

    //Read Config Reg
    SPIM_IO_SendCMDPhase(pSPIMx, SPIM_IO_READ_PHASE, OPCODE_RD_VCONFIG, PHASE_NORMAL_MODE, 0);
    SPIM_IO_SendAddrPhase(pSPIMx, 0, 0x00, PHASE_NORMAL_MODE, 0);
    //SPIM_IO_DCPhase(pSPIMx, 1, PHASE_NORMAL_MODE, 0);
    SPIM_IO_SendDummyCycle(pSPIMx, 8);
    SPIM_IO_SendDataPhase(pSPIMx, SPIM_IO_READ_PHASE, u8CMDBuf, 1, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, 0);
    printf("Octal Mode = 0x%X\r\n", u8CMDBuf[0]);
}

void SPIM_OctalTrainingDllLatency()
{
    uint32_t u32i = 0;
    uint32_t u32j = 0;
    uint8_t u8RdDelay = 0;
    uint8_t u8Temp = 0;
    uint8_t u8RdDelayIdx = 0;
    uint8_t u8RdDelayRes[SPIM_MAX_DLL_LATENCY] = {0};
    uint32_t u32SrcAddr = 0;
    uint32_t u32TestSize = 32;
    SPIM_T *pSPIMx = NULL;
    __attribute__((aligned(32))) uint8_t u8TstBuf1[32] = {0};
    __attribute__((aligned(32))) uint8_t u8TstBuf2[32] = {0};

    pSPIMx = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    SPIM_EraseAddrRange(pSPIMx, 0, 0, FLASH_PAGE_SIZE, OPCODE_BE_64K, FLASH_PAGE_SIZE, 1);

    popDat(u8TstBuf1, u32TestSize);

    SPIM_IO_WritePhase(pSPIMx, &gMU_82h_WrCMD, 0, u8TstBuf1, u32TestSize);

    MT35x_EnterOctalDDRMode(pSPIMx);

    for (u8RdDelay = 0; u8RdDelay <= SPIM_MAX_DLL_LATENCY; u8RdDelay++)
    {
        SPIM_CtrlDLLDelayTime(pSPIMx, u8RdDelay);

        memset(u8TstBuf2, 0, u32TestSize);

        SPIM_DMA_ReadPhase(pSPIMx, &gMU_CBh_RdCMD, 0, 0, 32, u8TstBuf2, 1);
        CU_ASSERT(pSPIMx->PHDMAR == 0x33003C3A);

        if (memcmp(u8TstBuf1, u8TstBuf2, u32TestSize))
        {
            printf("!!!\t%d Latency number failed\n", u8RdDelay);
            //CU_FAIL();
        }
        else
        {
            printf("RX Delay: %d = Pass\r\n", u8RdDelay);
            u8RdDelayRes[u8RdDelayIdx++] = u8RdDelay;
        }
    }

    //[sort]
    for (u32i = 0; u32i <= u8RdDelayIdx; u32i = u32i + 1)
    {
        for (u32j = u32i + 1; u32j < u8RdDelayIdx; u32j = u32j + 1)
        {
            if (u8RdDelayRes[u32i] > u8RdDelayRes[u32j])
            {
                u8Temp = u8RdDelayRes[u32i];
                u8RdDelayRes[u32i] = u8RdDelayRes[u32j];
                u8RdDelayRes[u32j] = u8Temp;
            }
        }
    }

    if (u8RdDelayIdx > 2)
    {
        u8RdDelayIdx = (u8RdDelayIdx / 2) - 1;
    }
    else
    {
        u8RdDelayIdx = 0;
    }

    MT35x_ExitOctalDDRMode(pSPIMx);

    printf("\r\nDLL Delay Time Num = %d\r\n\r\n", u8RdDelayRes[u8RdDelayIdx]);

    SPIM_CtrlDLLDelayTime(pSPIMx, u8RdDelayRes[u8RdDelayIdx]);
    CU_ASSERT(((pSPIMx->DLL0 & SPIM_DLL0_DLL_DNUM_Msk) >> SPIM_DLL0_DLL_DNUM_Pos) == u8RdDelayRes[u8RdDelayIdx]);

    CU_PASS();
}

void SPIM_OctalDMA_Func()
{
    SPIM_T *pSPIMModule = NULL;
    uint32_t offset = 0;
    uint32_t is4ByteAddr = 1;
    uint8_t *u8TstBuf1 = (uint8_t *)GetTestBuffer1();
    uint8_t *u8TstBuf2 = (uint8_t *)GetTestBuffer2();

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    SPIM_Enable_4Bytes_Mode(pSPIMModule, is4ByteAddr, 1, 0);

    if (is4ByteAddr)
    {
        CU_ASSERT(SPIM_Is4ByteModeEnable(pSPIMModule, 1) == 1);
    }
    else
    {
        CU_ASSERT(SPIM_Is4ByteModeEnable(pSPIMModule, 1) == 0);
    }

    popDat(u8TstBuf1, BUFFER_SIZE);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        SPIM_DMA_WritePhase(pSPIMModule,
                            &gMU_82h_WrCMD,
                            is4ByteAddr,
                            offset,
                            BUFFER_SIZE,
                            u8TstBuf1);

        //printf("pSPIMx->PHDMAW = 0x%08X\r\n", pSPIMModule->PHDMAW);
        CU_ASSERT(pSPIMModule->PHDMAW == 0x30000401);
    }

    MT35x_EnterOctalDDRMode(pSPIMModule);

    popDat(u8TstBuf1, BUFFER_SIZE);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(u8TstBuf2, 0, BUFFER_SIZE);
        SPIM_DMA_ReadPhase(pSPIMModule,
                           &gMU_CBh_RdCMD,
                           is4ByteAddr,
                           offset,
                           BUFFER_SIZE,
                           u8TstBuf2,
                           1);

        //printf("pSPIMx->PHDMAR = 0x%08X\r\n", pSPIMModule->PHDMAR);
        CU_ASSERT(pSPIMModule->PHDMAR == 0x33003C3A);

        // Compare.
        if (memcmp(u8TstBuf1, u8TstBuf2, BUFFER_SIZE))
        {
            printf("FAILED!\n");
            dump_compare_error(offset, u8TstBuf1, u8TstBuf2, BUFFER_SIZE);
            CU_FAIL();
        }
    }

    MT35x_ExitOctalDDRMode(pSPIMModule);

    CU_PASS();
}

void SPIM_OctalDMM_Func()
{
    int        i, offset;
    uint32_t   *pData;
    SPIM_T *pSPIMModule = NULL;
    uint32_t u32DMMAddress = 0;
    uint32_t is4ByteAddr = 1;
    uint8_t *u8TstBuf1 = (uint8_t *)GetTestBuffer1();
    uint8_t *u8TstBuf2 = (uint8_t *)GetTestBuffer2();

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    if (SPIM_InitFlash(pSPIMModule, 1) != 0)        /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        CU_FAIL();
    }

    SPIM_Enable_4Bytes_Mode(pSPIMModule, is4ByteAddr, 1, 0);

    if (is4ByteAddr)
    {
        CU_ASSERT(SPIM_Is4ByteModeEnable(pSPIMModule, 1) == 1);
    }
    else
    {
        CU_ASSERT(SPIM_Is4ByteModeEnable(pSPIMModule, 1) == 0);
    }

    //for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    //{
    //    popDat(u8TstBuf1, BUFFER_SIZE);
    //    SPIM_IO_Write(pSPIMModule,
    //                  offset,
    //                  is4ByteAddr,
    //                  BUFFER_SIZE,
    //                  u8TstBuf1,
    //                  CMD_OCTAL_PAGE_PROG_MICRON,
    //                  1,
    //                  1,
    //                  8;
    //}

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        popDat(u8TstBuf1, BUFFER_SIZE);

        SPIM_DMA_WritePhase(pSPIMModule,
                            &gMU_82h_WrCMD,
                            is4ByteAddr,
                            offset,
                            BUFFER_SIZE,
                            u8TstBuf1);

        //printf("pSPIMx->PHDMAW = 0x%08X\r\n", pSPIMModule->PHDMAW);
        CU_ASSERT(pSPIMModule->PHDMAW == 0x30000401);
    }

    MT35x_EnterOctalDDRMode(pSPIMModule);

    SPIM_DMM_ReadPhase(pSPIMModule, &gMU_CBh_RdCMD, is4ByteAddr, 0);
    printf("pSPIMx->PHDMM = 0x%08X\r\n", pSPIMModule->PHDMM);
    CU_ASSERT(pSPIMModule->PHDMM == 0x33003C3A);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        if (GetSPIMTestModuleIdx() == C_SPIM0)
        {
            u32DMMAddress = SPIM0_DMM_MAP_ADDR;
        }
        else if (GetSPIMTestModuleIdx() == C_SPIM1)
        {
            u32DMMAddress = SPIM1_DMM_MAP_ADDR;
        }

        memcpy(u8TstBuf1, (uint8_t *)(u32DMMAddress + offset), BUFFER_SIZE);

        popDat(u8TstBuf1, BUFFER_SIZE);

        pData = (uint32_t *)u8TstBuf1;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (inpw(u32DMMAddress + offset + i) != *pData)
            {
                printf("FAILED!\n");
                printf("DMM - Flash address 0x%x, read 0x%x, expect 0x%x!\n",
                       i,
                       *pData,
                       (i << 16) | (offset + i));
                SPIM_ExitDirectMapMode(pSPIMModule);
                MT35x_ExitOctalDDRMode(pSPIMModule);
                CU_FAIL();
            }
        }
    }

    SPIM_ExitDirectMapMode(pSPIMModule);

    MT35x_ExitOctalDDRMode(pSPIMModule);

    SPIM_SetQuadEnable(pSPIMModule, 0, 1);

    printf("DMM Test done.\n");
    CU_PASS();
}

CU_TestInfo SPIM_OctalApiTests[] =
{
    {"SPIM Octal DLL Training Function", SPIM_OctalTrainingDllLatency},
    {"SPIM Octal DMA Function", SPIM_OctalDMA_Func},
    {"SPIM Octal DMM Function", SPIM_OctalDMM_Func},

    CU_TEST_INFO_NULL
};

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
CU_SuiteInfo spim_octal_suites[] =
{
    {"SPIM HYPER API", SPIM_Tests_OcatlInit, SPIM_Tests_OctalClean, NULL, NULL, SPIM_OctalApiTests},

    CU_SUITE_INFO_NULL,
};
