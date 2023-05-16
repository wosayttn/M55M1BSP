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
#include "assert.h"

#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "spim_cunit.h"

//------------------------------------------------------------------------------
extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;

//------------------------------------------------------------------------------
#define FLASH_BLOCK_SIZE            0x10000
#define BUFFER_SIZE                 2048

//#define SPIM_KEY_1                  0x391e9055
//#define SPIM_KEY_2                  0xf15da090
#define C_SPIM0                     (0)
#define C_SPIM1                     (1)
#define SPIM_FIRST_TEST             (0)

//------------------------------------------------------------------------------
#if defined (__ARMCC_VERSION)
__attribute__((aligned(4))) uint8_t g_buff[BUFFER_SIZE] = {0};
#else
__align(4) uint8_t g_buff[BUFFER_SIZE] = {0};
#endif //__ARMCC_VERSION

uint8_t idBuf[3] = {0};

SPIM_T *gpasSPIMModule[] =
{
    SPIM0,
    SPIM1,
};

//------------------------------------------------------------------------------
void *GetSPIMModule(uint32_t u32Idx)
{
    return gpasSPIMModule[u32Idx];
}

/*----------------------------------------------------------------------------*/
/* Test function                                                              */
/*----------------------------------------------------------------------------*/
void SPIMIrqHandle(SPIM_T *spim)
{
    printf("INT!\n");

    CU_ASSERT_TRUE(SPIM_IS_IF_ON(spim));

    SPIM_CLR_INT(spim);

    CU_ASSERT_FALSE(SPIM_IS_IF_ON(spim));
}

void SPIM1_IRQHandler(void)
{
    SPIM_T *pSPIMModule = (SPIM_T *)GetSPIMModule(C_SPIM1);

    //printf("INT!\n");

    //CU_ASSERT_TRUE(_SPIM_IS_IF_ON());
    //_SPIM_CLR_INT();
    //CU_ASSERT_FALSE(_SPIM_IS_IF_ON());
    SPIMIrqHandle(pSPIMModule);
}

void SPIM0_IRQHandler(void)
{
    SPIM_T *pSPIMModule = (SPIM_T *)GetSPIMModule(C_SPIM0);

    //printf("INT!\n");

    //CU_ASSERT_TRUE(_SPIM_IS_IF_ON());
    //_SPIM_CLR_INT();
    //CU_ASSERT_FALSE(_SPIM_IS_IF_ON());

    SPIMIrqHandle(pSPIMModule);
}

int SPIM_Tests_Init(void)
{
    uint32_t u32SPIMIdx = 0;
    SPIM_T *pSPIMModule = NULL;

    for (u32SPIMIdx = SPIM_FIRST_TEST; u32SPIMIdx < sizeof(gpasSPIMModule) / sizeof(SPIM_T *); u32SPIMIdx++)
    {
        pSPIMModule = (SPIM_T *)GetSPIMModule(u32SPIMIdx);

        SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 1);        /* Set SPIM clock as HCLK divided by 2 */

        SPIM_SET_RXCLKDLY_RDDLYSEL(pSPIMModule, 0);    /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */
        //SPIM_SET_RXCLKDLY_RDEDGE(pSPIMModule);         /* Use SPI input clock rising edge to sample received data. */
        SPIM_SET_DC_DMAR(pSPIMModule, 8);              /* Set 8 dummy cycle. */

        if (SPIM_InitFlash(pSPIMModule, 1) != 0)        /* Initialized SPI flash */
        {
            printf("SPIM flash initialize failed!\n");

            while (1);
        }

        SPIM_ReadJedecId(pSPIMModule, idBuf, sizeof(idBuf), 1);
        printf("SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n",
               idBuf[0],
               idBuf[1], idBuf[2]);
    }

    // NVIC_EnableIRQ(CRPT_IRQn);

    return 0;
}

int SPIM_Tests_Clean(void)
{
    //NVIC_DisableIRQ(CRPT_IRQn);

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
    {"SPIM CONST", SPIM_Tests_Init, SPIM_Tests_Clean, NULL, NULL, SPIM_ConstantTests},
    {"SPIM MACRO", SPIM_Tests_Init, SPIM_Tests_Clean, NULL, NULL, SPIM_MacroTests},
    {"SPIM API", SPIM_Tests_Init, SPIM_Tests_Clean, NULL, NULL, SPIM_ApiTests},
    CU_SUITE_INFO_NULL
};

void spim_dma_read(SPIM_T *spim, uint32_t u32Addr, uint32_t u32NRx, uint8_t *pu8RxBuf)
{
    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_PAGEREAD);// Switch to Page Read mode.

    SPIM_SET_SPIM_MODE(spim, CMD_FAST_READ);     // SPIM mode.
    CU_ASSERT(SPIM_GET_SPIM_MODE(spim) == CMD_FAST_READ);

    SPIM_SET_4BYTE_ADDR_EN(spim, 0);                 // Enable/disable 4-Byte Address.

    spim->SRAMADDR = (uint32_t) pu8RxBuf;         // SRAM u32Address.
    spim->DMACNT = BUFFER_SIZE;                 // Transfer length.
    spim->FADDR = 0;                            // Flash u32Address.
    SPIM_SET_GO(spim);                             // Go.
    CU_ASSERT_TRUE(SPIM_IS_BUSY(spim));

    SPIM_WAIT_FREE(spim);                          // Wait for DMA done.
}

void SPIM_Const_Address()
{
    uint32_t u32SPIMIdx = 0;

    for (u32SPIMIdx = SPIM_FIRST_TEST; u32SPIMIdx < sizeof(gpasSPIMModule) / sizeof(SPIM_T *); u32SPIMIdx++)
    {
        switch (u32SPIMIdx)
        {
            case C_SPIM0:
                CU_ASSERT(SPIM0_DMM_MAP_SADDR == 0x80000000UL);
                CU_ASSERT(SPIM0_DMM_MAP_NSADDR == 0x90000000UL);
                break;

            case C_SPIM1:
                CU_ASSERT(SPIM1_DMM_MAP_SADDR == 0x82000000UL);
                CU_ASSERT(SPIM1_DMM_MAP_NSADDR == 0x92000000UL);
                break;
        }
    }

    //CU_ASSERT(SPIM_DMM_MAP_ADDR == 0x8000000);
    CU_ASSERT(SPIM_DMM_SIZE == 0x2000000UL);
    //CU_ASSERT(SPIM_CCM_ADDR == 0x20020000);
    //CU_ASSERT(SPIM_CCM_SIZE == 0x8000);
}

void SPIM_Const_SPIM_CMDCODE()
{
    CU_ASSERT(CMD_NORMAL_PAGE_PROGRAM == (0x02));
    CU_ASSERT(CMD_NORMAL_PAGE_PROGRAM_4B == (0x12));
    CU_ASSERT(CMD_QUAD_PAGE_PROGRAM_WINBOND == (0x32));
    CU_ASSERT(CMD_QUAD_PAGE_PROGRAM_MXIC == (0x38));
    CU_ASSERT(CMD_QUAD_PAGE_PROGRAM_EON == (0x40));

    CU_ASSERT(CMD_DMA_NORMAL_READ == (0x03));
    CU_ASSERT(CMD_DMA_FAST_READ == (0x0B));
    CU_ASSERT(CMD_DMA_NORMAL_DUAL_READ == (0x3B));
    CU_ASSERT(CMD_DMA_FAST_READ_DUAL_OUTPUT == (0x3B));
    CU_ASSERT(CMD_DMA_FAST_READ_QUAD_OUTPUT == (0x6B));
    CU_ASSERT(CMD_DMA_FAST_DUAL_READ == (0xBBUL));
    CU_ASSERT(CMD_DMA_NORMAL_QUAD_READ == (0xE7UL));
    CU_ASSERT(CMD_DMA_FAST_QUAD_READ == (0xEBUL));
}

void SPIM_Const_SPIM_CTL0()
{
    CU_ASSERT(SPIM_CTL0_RW_IN(0) == SPIM_CTL0_QDIODIR_Msk);

    CU_ASSERT(SPIM_CTL0_BITMODE_SING == (0UL << SPIM_CTL0_BITMODE_Pos));
    CU_ASSERT(SPIM_CTL0_BITMODE_DUAL == (1UL << SPIM_CTL0_BITMODE_Pos));
    CU_ASSERT(SPIM_CTL0_BITMODE_QUAD == (2UL << SPIM_CTL0_BITMODE_Pos));
    CU_ASSERT(SPIM_CTL0_BITMODE_OCTAL == (3UL << SPIM_CTL0_BITMODE_Pos));

    CU_ASSERT(SPIM_CTL0_OPMODE_IO == (0UL << SPIM_CTL0_OPMODE_Pos));
    CU_ASSERT(SPIM_CTL0_OPMODE_PAGEWRITE == (1UL << SPIM_CTL0_OPMODE_Pos));
    CU_ASSERT(SPIM_CTL0_OPMODE_PAGEREAD == (2UL << SPIM_CTL0_OPMODE_Pos));
    CU_ASSERT(SPIM_CTL0_OPMODE_DIRECTMAP == (3UL << SPIM_CTL0_OPMODE_Pos));

    //Read Byte Order Mode
    CU_ASSERT(SPIM_CTL0_RBO_MODE0 == (0UL << SPIM_CTL0_RBO_NORM_Pos));
    CU_ASSERT(SPIM_CTL0_RBO_MODE1 == (1UL << SPIM_CTL0_RBO_NORM_Pos));
    CU_ASSERT(SPIM_CTL0_RBO_MODE2 == (2UL << SPIM_CTL0_RBO_NORM_Pos));
    CU_ASSERT(SPIM_CTL0_RBO_MODE3 == (3UL << SPIM_CTL0_RBO_NORM_Pos));
}

void SPIM_Const_MFGID()
{
    CU_ASSERT(MFGID_UNKNOW == 0x00);
    CU_ASSERT(MFGID_SPANSION == 0x01);
    CU_ASSERT(MFGID_EON == 0x1C);
    CU_ASSERT(MFGID_ISSI == 0x7F);
    CU_ASSERT(MFGID_MXIC == 0xC2);
    CU_ASSERT(MFGID_WINBOND == 0xEF);
    CU_ASSERT(MFGID_MICRON == 0x2CU);
}

void SPIM_Const_OPCODE()
{
    CU_ASSERT(OPCODE_WREN == 0x06);
    CU_ASSERT(OPCODE_RDSR == 0x05);
    CU_ASSERT(OPCODE_WRSR == 0x01);
    CU_ASSERT(OPCODE_RDSR2 == 0x35);
    CU_ASSERT(OPCODE_WRSR2 == 0x31);
    CU_ASSERT(OPCODE_RDSR3 == 0x15);
    CU_ASSERT(OPCODE_WRSR3 == 0x11);
    CU_ASSERT(OPCODE_NORM_READ == 0x03);
    CU_ASSERT(OPCODE_FAST_READ == 0x0b);
    CU_ASSERT(OPCODE_FAST_DUAL_READ == 0x3b);
    CU_ASSERT(OPCODE_FAST_QUAD_READ == 0x6b);
    CU_ASSERT(OPCODE_PP == 0x02);
    CU_ASSERT(OPCODE_SE_4K == 0x20);
    CU_ASSERT(OPCODE_BE_32K == 0x52);
    CU_ASSERT(OPCODE_CHIP_ERASE == 0xc7);
    CU_ASSERT(OPCODE_BE_64K == 0xd8);
    CU_ASSERT(OPCODE_READ_ID == 0x90);
    CU_ASSERT(OPCODE_RDID == 0x9f);
    CU_ASSERT(OPCODE_BRRD == 0x16);
    CU_ASSERT(OPCODE_BRWR == 0x17);

    CU_ASSERT(OPCODE_BP == 0x02);
    CU_ASSERT(OPCODE_WRDI == 0x04);
    CU_ASSERT(OPCODE_AAI_WP == 0xad);

    CU_ASSERT(OPCODE_EN4B == 0xb7);
    CU_ASSERT(OPCODE_EX4B == 0xe9);

    CU_ASSERT(OPCODE_RDSCUR == 0x2b);
    CU_ASSERT(OPCODE_WRSCUR == 0x2f);

    CU_ASSERT(OPCODE_RSTEN == 0x66);
    CU_ASSERT(OPCODE_RST == 0x99);

    CU_ASSERT(OPCODE_ENQPI == 0x38);
    CU_ASSERT(OPCODE_EXQPI == 0xFF);
}

void SPIM_Const_Register()
{
    CU_ASSERT(SR_WIP == 1);
    CU_ASSERT(SR_WEL == 2);
    CU_ASSERT(SR_QE == 0x40);

    CU_ASSERT(SR2_QE == 2);

    CU_ASSERT(SR_BP0 == 4);
    CU_ASSERT(SR_BP1 == 8);
    CU_ASSERT(SR_BP2 == 0x10);
    CU_ASSERT(SR_SRWD == 0x80);
    CU_ASSERT(SR3_ADR == 0x01);

    CU_ASSERT(SCUR_4BYTE == 0x04);
}

void MACRO_SPIM_CTL0()
{
    uint8_t idBuf[3] = {0};
    uint32_t u32SPIMIdx = 0;
    SPIM_T *pSPIMModule = NULL;

    for (u32SPIMIdx = SPIM_FIRST_TEST; u32SPIMIdx < sizeof(gpasSPIMModule) / sizeof(SPIM_T *); u32SPIMIdx++)
    {
        pSPIMModule = (SPIM_T *)GetSPIMModule(u32SPIMIdx);
        //SPIM->KEY1 = SPIM_KEY_1;
        //SPIM->KEY2 = SPIM_KEY_2;

        /*
         *  SPIM_ENABLE_CIPHER()
         */
        SPIM_ENABLE_CIPHER(pSPIMModule);
        CU_ASSERT_FALSE(pSPIMModule->CTL0 & SPIM_CTL0_CIPHOFF_Msk);
        CU_ASSERT((pSPIMModule->CTL0 & SPIM_DMMCTL_DESELTIM_Msk) == 0x12);

        /*
         *  SPIM_DISABLE_CIPHER()
         */
        SPIM_DISABLE_CIPHER(pSPIMModule);
        CU_ASSERT_TRUE(pSPIMModule->CTL0 & SPIM_CTL0_CIPHOFF_Msk);
        CU_ASSERT((pSPIMModule->CTL0 & SPIM_DMMCTL_DESELTIM_Msk) == 0x08);

        /*
         *  SPIM_SET_BAL_EN(x)
         */
        pSPIMModule->CTL0 = 0;
        SPIM_ENABLE_BALEN(pSPIMModule);
        CU_ASSERT_TRUE(pSPIMModule->CTL0 & SPIM_CTL0_BALEN_Msk);
        SPIM_DISABLE_BALEN(pSPIMModule);
        CU_ASSERT_FALSE(pSPIMModule->CTL0 & SPIM_CTL0_BALEN_Msk);
        SPIM_ENABLE_BALEN(pSPIMModule);
        CU_ASSERT_TRUE(pSPIMModule->CTL0 & SPIM_CTL0_BALEN_Msk);

        /*
         *  SPIM_SET_HYPER_MODE(x)
         */
        SPIM_SET_HYPER_MODE(pSPIMModule, 1);
        CU_ASSERT_TRUE(pSPIMModule->CTL0 & SPIM_CTL0_HYPER_EN_Msk);
        SPIM_SET_HYPER_MODE(pSPIMModule, 0);
        CU_ASSERT_FALSE(pSPIMModule->CTL0 & SPIM_CTL0_HYPER_EN_Msk);

        /*
         *  _SPIM_SET_4BYTE_ADDR_EN(x)
         */
        pSPIMModule->CTL0 = 0;
        SPIM_SET_4BYTE_ADDR_EN(pSPIMModule, 1);
        CU_ASSERT_TRUE(pSPIMModule->CTL0 & SPIM_CTL0_B4ADDREN_Msk);
        SPIM_SET_4BYTE_ADDR_EN(pSPIMModule, 0);
        CU_ASSERT_FALSE(pSPIMModule->CTL0 & SPIM_CTL0_B4ADDREN_Msk);

        /*
         *  _SPIM_ENABLE_INT()
         */
        SPIM_SET_INT(pSPIMModule, 1);
        CU_ASSERT(pSPIMModule->CTL0 & SPIM_CTL0_IEN_Msk);
        SPIM_ReadJedecId(pSPIMModule, idBuf, sizeof(idBuf), 1);

        /*
         *  _SPIM_DISABLE_INT()
         */
        SPIM_SET_INT(pSPIMModule, 0);
        CU_ASSERT_FALSE(pSPIMModule->CTL0 & SPIM_CTL0_IEN_Msk);

        /*
         *  _SPIM_SET_DATA_WIDTH()
         */
        pSPIMModule->CTL0 = 0;
        SPIM_SET_DATA_WIDTH(pSPIMModule, 8);
        CU_ASSERT(((pSPIMModule->CTL0 & SPIM_CTL0_DWIDTH_Msk) >> SPIM_CTL0_DWIDTH_Pos) == 0x7);
        SPIM_SET_DATA_WIDTH(pSPIMModule, 16);
        CU_ASSERT(((pSPIMModule->CTL0 & SPIM_CTL0_DWIDTH_Msk) >> SPIM_CTL0_DWIDTH_Pos) == 0xF);
        SPIM_SET_DATA_WIDTH(pSPIMModule, 24);
        CU_ASSERT(((pSPIMModule->CTL0 & SPIM_CTL0_DWIDTH_Msk) >> SPIM_CTL0_DWIDTH_Pos) == 0x17);
        SPIM_SET_DATA_WIDTH(pSPIMModule, 32);
        CU_ASSERT(((pSPIMModule->CTL0 & SPIM_CTL0_DWIDTH_Msk) >> SPIM_CTL0_DWIDTH_Pos) == 0x1F);

        /*
         *  _SPIM_GET_DATA_WIDTH()
         */
        pSPIMModule->CTL0 = (pSPIMModule->CTL0 & ~SPIM_CTL0_DWIDTH_Msk) | (8 << SPIM_CTL0_DWIDTH_Pos);
        CU_ASSERT(SPIM_GET_DATA_WIDTH(pSPIMModule) == 0x7);
        pSPIMModule->CTL0 = (pSPIMModule->CTL0 & ~SPIM_CTL0_DWIDTH_Msk) | (16 << SPIM_CTL0_DWIDTH_Pos);
        CU_ASSERT(SPIM_GET_DATA_WIDTH(pSPIMModule) == 0xF);
        pSPIMModule->CTL0 = (pSPIMModule->CTL0 & ~SPIM_CTL0_DWIDTH_Msk) | (24 << SPIM_CTL0_DWIDTH_Pos);
        CU_ASSERT(SPIM_GET_DATA_WIDTH(pSPIMModule) == 0x17);
        pSPIMModule->CTL0 = (pSPIMModule->CTL0 & ~SPIM_CTL0_DWIDTH_Msk) | (32 << SPIM_CTL0_DWIDTH_Pos);
        CU_ASSERT(SPIM_GET_DATA_WIDTH(pSPIMModule) == 0x1F);

        /*
         *  _SPIM_SET_DATA_NUM()
         */
        SPIM->CTL0 = 0;
        SPIM_SET_BURST_NUM(pSPIMModule, 0x0);
        CU_ASSERT(((pSPIMModule->CTL0 & SPIM_CTL0_BURSTNUM_Msk) >> SPIM_CTL0_BURSTNUM_Pos) == 0x0);
        SPIM_SET_BURST_NUM(pSPIMModule, 0x1);
        CU_ASSERT(((pSPIMModule->CTL0 & SPIM_CTL0_BURSTNUM_Msk) >> SPIM_CTL0_BURSTNUM_Pos) == 0x1);
        SPIM_SET_BURST_NUM(pSPIMModule, 0x2);
        CU_ASSERT(((pSPIMModule->CTL0 & SPIM_CTL0_BURSTNUM_Msk) >> SPIM_CTL0_BURSTNUM_Pos) == 0x2);
        SPIM_SET_BURST_NUM(pSPIMModule, 0x3);
        CU_ASSERT(((pSPIMModule->CTL0 & SPIM_CTL0_BURSTNUM_Msk) >> SPIM_CTL0_BURSTNUM_Pos) == 0x3);

        /*
         *  _SPIM_GET_DATA_NUM()
         */
        SPIM->CTL0 = (SPIM->CTL0 & (~SPIM_CTL0_BURSTNUM_Msk)) | (0x1 << SPIM_CTL0_BURSTNUM_Pos);
        CU_ASSERT(_SPIM_GET_DATA_NUM() == 0x2);
        SPIM->CTL0 = (SPIM->CTL0 & (~SPIM_CTL0_BURSTNUM_Msk)) | (0x3 << SPIM_CTL0_BURSTNUM_Pos);
        CU_ASSERT(_SPIM_GET_DATA_NUM() == 0x4);

        /*
         *  _SPIM_ENABLE_SING_INPUT_MODE()
         */
        SPIM->CTL0 = 0;
        _SPIM_ENABLE_SING_INPUT_MODE();
        CU_ASSERT(SPIM->CTL0 == (0 << SPIM_CTL0_BITMODE_Pos));

        /*
         *  _SPIM_ENABLE_SING_OUTPUT_MODE()
         */
        SPIM->CTL0 = 0;
        _SPIM_ENABLE_SING_OUTPUT_MODE();
        CU_ASSERT(SPIM->CTL0 == (SPIM_CTL0_QDIODIR_Msk | (0 << SPIM_CTL0_BITMODE_Pos)));

        /*
         *  _SPIM_ENABLE_DUAL_INPUT_MODE()
         */
        SPIM->CTL0 = 0;
        _SPIM_ENABLE_DUAL_INPUT_MODE();
        CU_ASSERT(SPIM->CTL0 == (0x1 << SPIM_CTL0_BITMODE_Pos));

        /*
         *  _SPIM_ENABLE_DUAL_OUTPUT_MODE()
         */
        SPIM->CTL0 = 0;
        _SPIM_ENABLE_DUAL_OUTPUT_MODE();
        CU_ASSERT(SPIM->CTL0 == (SPIM_CTL0_QDIODIR_Msk | (0x1 << SPIM_CTL0_BITMODE_Pos)));

        /*
         *  _SPIM_ENABLE_QUAD_INPUT_MODE()
         */
        SPIM->CTL0 = 0;
        _SPIM_ENABLE_QUAD_INPUT_MODE();
        CU_ASSERT(SPIM->CTL0 == (0x2 << SPIM_CTL0_BITMODE_Pos));

        /*
         *  _SPIM_ENABLE_QUAD_OUTPUT_MODE()
         */
        SPIM->CTL0 = 0;
        _SPIM_ENABLE_QUAD_OUTPUT_MODE();
        CU_ASSERT(SPIM->CTL0 == (SPIM_CTL0_QDIODIR_Msk | (0x2 << SPIM_CTL0_BITMODE_Pos)));

        /*
         *  _SPIM_SET_SUSP_INTVL()
         */
        SPIM->CTL0 = 0;
        _SPIM_SET_SUSP_INTVL(0x3);
        CU_ASSERT(((SPIM->CTL0 & SPIM_CTL0_SUSPITV_Msk) >> SPIM_CTL0_SUSPITV_Pos) == 0x3);
        _SPIM_SET_SUSP_INTVL(0x6);
        CU_ASSERT(((SPIM->CTL0 & SPIM_CTL0_SUSPITV_Msk) >> SPIM_CTL0_SUSPITV_Pos) == 0x6);

        /*
         *  _SPIM_GET_SUSP_INTVL()
         */
        SPIM->CTL0 = 0;
        _SPIM_SET_SUSP_INTVL(0x2);
        CU_ASSERT(_SPIM_GET_SUSP_INTVL() == 0x2);
        _SPIM_SET_SUSP_INTVL(0x5);
        CU_ASSERT(_SPIM_GET_SUSP_INTVL() == 0x5);

        /*
         *  _SPIM_SET_OPMODE() / _SPIM_GET_OP_MODE()
         */
        SPIM->CTL0 = 0;
        _SPIM_SET_OPMODE(SPIM_CTL0_OPMODE_IO);
        CU_ASSERT(_SPIM_GET_OP_MODE() == SPIM_CTL0_OPMODE_IO);
        _SPIM_SET_OPMODE(SPIM_CTL0_OPMODE_PAGEWRITE);
        CU_ASSERT(_SPIM_GET_OP_MODE() == SPIM_CTL0_OPMODE_PAGEWRITE);
        _SPIM_SET_OPMODE(SPIM_CTL0_OPMODE_PAGEREAD);
        CU_ASSERT(_SPIM_GET_OP_MODE() == SPIM_CTL0_OPMODE_PAGEREAD);
        _SPIM_SET_OPMODE(SPIM_CTL0_OPMODE_DIRECTMAP);
        CU_ASSERT(_SPIM_GET_OP_MODE() == SPIM_CTL0_OPMODE_DIRECTMAP);
    }
}

void MACRO_SPIM_CTL1()
{
    /*
     *  _SPIM_ENABLE_CACHE()
     */
    SPIM->CTL1 = 0;
    _SPIM_ENABLE_CACHE();
    CU_ASSERT_FALSE(SPIM->CTL1 & SPIM_CTL1_CACHEOFF_Msk);

    /*
     *  _SPIM_DISABLE_CACHE()
     */
    SPIM->CTL1 = 0;
    _SPIM_DISABLE_CACHE();
    CU_ASSERT_TRUE(SPIM->CTL1 & SPIM_CTL1_CACHEOFF_Msk);

    /*
     *  _SPIM_IS_CACHE_EN()
     */
    SPIM->CTL1 = 0;
    _SPIM_ENABLE_CACHE();
    CU_ASSERT_TRUE(_SPIM_IS_CACHE_EN());
    _SPIM_DISABLE_CACHE();
    CU_ASSERT_FALSE(_SPIM_IS_CACHE_EN());

    /*
     *  _SPIM_ENABLE_CCM()
     */
    SPIM->CTL1 = 0;
    _SPIM_ENABLE_CCM();
    _SPIM_INVALID_CACHE();
    CU_ASSERT_TRUE(SPIM->CTL1 & SPIM_CTL1_CCMEN_Msk);

    /*
     *  _SPIM_DISABLE_CCM()
     */
    _SPIM_DISABLE_CCM();
    CU_ASSERT_FALSE(SPIM->CTL1 & SPIM_CTL1_CCMEN_Msk);

    /*
     *  _SPIM_IS_CCM_EN()
     */
    SPIM->CTL1 = 0;
    _SPIM_ENABLE_CCM();
    CU_ASSERT_TRUE(_SPIM_IS_CCM_EN());
    _SPIM_DISABLE_CCM();
    CU_ASSERT_FALSE(_SPIM_IS_CCM_EN());

    /*
     *  _SPIM_SET_SS_EN(x)
     */
    SPIM->CTL1 = 0;
    _SPIM_SET_SS_EN(1);
    CU_ASSERT_FALSE(SPIM->CTL1 & SPIM_CTL1_SS_Msk);
    _SPIM_SET_SS_EN(0);
    CU_ASSERT_TRUE(SPIM->CTL1 & SPIM_CTL1_SS_Msk);

    /*
     *  _SPI_GET_SS_EN()
     */
    SPIM->CTL1 = 0;
    _SPIM_SET_SS_EN(1);
    CU_ASSERT_TRUE(_SPI_GET_SS_EN());
    _SPIM_SET_SS_EN(0);
    CU_ASSERT_FALSE(_SPI_GET_SS_EN());

    /*
     *  _SPI_SET_SS_ACTLVL(x)
     */
    SPIM->CTL1 = 0;
    _SPI_SET_SS_ACTLVL(0);
    CU_ASSERT_FALSE(SPIM->CTL1 & SPIM_CTL1_SSACTPOL_Msk);
    _SPI_SET_SS_ACTLVL(1);
    CU_ASSERT_TRUE(SPIM->CTL1 & SPIM_CTL1_SSACTPOL_Msk);

    /*
     *  _SPI_SET_IDL_INTVL()
     */
    SPIM->CTL1 = 0;
    _SPI_SET_IDL_INTVL(3);
    CU_ASSERT(((SPIM->CTL1 & SPIM_CTL1_IDLETIME_Msk) >> SPIM_CTL1_IDLETIME_Pos) == 0x03);
    _SPI_SET_IDL_INTVL(6);
    CU_ASSERT(((SPIM->CTL1 & SPIM_CTL1_IDLETIME_Msk) >> SPIM_CTL1_IDLETIME_Pos) == 0x06);

    /*
     *  _SPI_GET_IDL_INTVL()
     */
    SPIM->CTL1 = 0;
    _SPI_SET_IDL_INTVL(2);
    CU_ASSERT(_SPI_GET_IDL_INTVL() == 0x02);
    _SPI_SET_IDL_INTVL(4);
    CU_ASSERT(_SPI_GET_IDL_INTVL() == 0x04);

    /*
     *  _SPIM_SET_CLOCK_DIVIDER(x)
     */
    SPIM->CTL1 = 0;
    _SPIM_SET_CLOCK_DIVIDER(3);
    CU_ASSERT(((SPIM->CTL1 & SPIM_CTL1_DIVIDER_Msk) >> SPIM_CTL1_DIVIDER_Pos) == 0x03);
    _SPIM_SET_CLOCK_DIVIDER(6);
    CU_ASSERT(((SPIM->CTL1 & SPIM_CTL1_DIVIDER_Msk) >> SPIM_CTL1_DIVIDER_Pos) == 0x06);

    /*
     *  _SPIM_GET_CLOCK_DIVIDER()
     */
    SPIM->CTL1 = 0;
    _SPIM_SET_CLOCK_DIVIDER(2);
    CU_ASSERT(_SPIM_GET_CLOCK_DIVIDER() == 0x02);
    _SPIM_SET_CLOCK_DIVIDER(4);
    CU_ASSERT(_SPIM_GET_CLOCK_DIVIDER() == 0x04);

    CU_PASS();
}

void MACRO_SPIM_RXCLKDLY()
{
    /*
     *  _SPIM_SET_RXCLKDLY_DWDELSEL(x)
     */
    SPIM->RXCLKDLY = 0;
    _SPIM_SET_RXCLKDLY_DWDELSEL(3);
    CU_ASSERT(((SPIM->RXCLKDLY & SPIM_RXCLKDLY_DWDELSEL_Msk) >> SPIM_RXCLKDLY_DWDELSEL_Pos) == 0x03);
    CU_ASSERT(_SPIM_GET_RXCLKDLY_DWDELSEL() == 0x3);
    _SPIM_SET_RXCLKDLY_DWDELSEL(6);
    CU_ASSERT(((SPIM->RXCLKDLY & SPIM_RXCLKDLY_DWDELSEL_Msk) >> SPIM_RXCLKDLY_DWDELSEL_Pos) == 0x06);
    CU_ASSERT(_SPIM_GET_RXCLKDLY_DWDELSEL() == 0x6);

    /*
     *  _SPIM_SET_RXCLKDLY_DWDELSEL(x)
     */
    SPIM->RXCLKDLY = 0;
    _SPIM_SET_RXCLKDLY_DWDELSEL(3);
    CU_ASSERT(((SPIM->RXCLKDLY & SPIM_RXCLKDLY_DWDELSEL_Msk) >> SPIM_RXCLKDLY_DWDELSEL_Pos) == 0x03);
    _SPIM_SET_RXCLKDLY_DWDELSEL(6);
    CU_ASSERT(((SPIM->RXCLKDLY & SPIM_RXCLKDLY_DWDELSEL_Msk) >> SPIM_RXCLKDLY_DWDELSEL_Pos) == 0x06);

    /*
     *  _SPIM_SET_RXCLKDLY_RDDLYSEL(x)
     */
    SPIM->RXCLKDLY = 0;
    _SPIM_SET_RXCLKDLY_RDDLYSEL(2);
    CU_ASSERT(((SPIM->RXCLKDLY & SPIM_RXCLKDLY_RDDLYSEL_Msk) >> SPIM_RXCLKDLY_RDDLYSEL_Pos) == 0x02);
    _SPIM_SET_RXCLKDLY_RDDLYSEL(3);
    CU_ASSERT(((SPIM->RXCLKDLY & SPIM_RXCLKDLY_RDDLYSEL_Msk) >> SPIM_RXCLKDLY_RDDLYSEL_Pos) == 0x03);

    /*
     *  _SPIM_GET_RXCLKDLY_RDDLYSEL()
     */
    SPIM->RXCLKDLY = 0;
    _SPIM_SET_RXCLKDLY_RDDLYSEL(3);
    CU_ASSERT(_SPIM_GET_RXCLKDLY_RDDLYSEL() == 0x03);
    _SPIM_SET_RXCLKDLY_RDDLYSEL(1);
    CU_ASSERT(_SPIM_GET_RXCLKDLY_RDDLYSEL() == 0x01);

    /*
     *  _SPIM_SET_RXCLKDLY_RDEDGE() / _SPIM_CLR_RXCLKDLY_RDEDGE()
     */
    SPIM->RXCLKDLY = 0;
    _SPIM_SET_RXCLKDLY_RDEDGE();
    CU_ASSERT_TRUE(SPIM->RXCLKDLY & SPIM_RXCLKDLY_RDEDGE_Msk);
    _SPIM_CLR_RXCLKDLY_RDEDGE();
    CU_ASSERT_FALSE(SPIM->RXCLKDLY & SPIM_RXCLKDLY_RDEDGE_Msk);
    _SPIM_SET_RXCLKDLY_RDEDGE();
    CU_ASSERT_TRUE(SPIM->RXCLKDLY & SPIM_RXCLKDLY_RDEDGE_Msk);

    CU_PASS();
}

void MACRO_SPIM_DMMCTL()
{
    /*
     *  _SPIM_SET_DMMCTL_CRMDAT(x)
     */
    SPIM->DMMCTL = 0;
    _SPIM_SET_DMMCTL_CRMDAT(0x7);
    CU_ASSERT(((SPIM->DMMCTL & SPIM_DMMCTL_CRMDAT_Msk) >> SPIM_DMMCTL_CRMDAT_Pos) == 0x7);
    _SPIM_SET_DMMCTL_CRMDAT(0xf1);
    CU_ASSERT(((SPIM->DMMCTL & SPIM_DMMCTL_CRMDAT_Msk) >> SPIM_DMMCTL_CRMDAT_Pos) == 0xf1);

    /*
     *  _SPIM_GET_DMMCTL_CRMDAT(x)
     */
    SPIM->DMMCTL = 0;
    _SPIM_SET_DMMCTL_CRMDAT(0x31);
    CU_ASSERT(_SPIM_GET_DMMCTL_CRMDAT() == 0x31);
    _SPIM_SET_DMMCTL_CRMDAT(0x8e);
    CU_ASSERT(_SPIM_GET_DMMCTL_CRMDAT() == 0x8e);

    /*
     *  _SPIM_DMM_SET_DESELTIM(x)
     */
    SPIM->DMMCTL = 0;
    _SPIM_DMM_SET_DESELTIM(0xe);
    CU_ASSERT(((SPIM->DMMCTL & SPIM_DMMCTL_DESELTIM_Msk) >> SPIM_DMMCTL_DESELTIM_Pos) == 0xe);
    _SPIM_DMM_SET_DESELTIM(0x11);
    CU_ASSERT(((SPIM->DMMCTL & SPIM_DMMCTL_DESELTIM_Msk) >> SPIM_DMMCTL_DESELTIM_Pos) == 0x11);

    /*
     *  _SPIM_DMM_GET_DESELTIM()
     */
    SPIM->DMMCTL = 0;
    _SPIM_DMM_SET_DESELTIM(0x13);
    CU_ASSERT(_SPIM_DMM_GET_DESELTIM() == 0x13);
    _SPIM_DMM_SET_DESELTIM(0xd);
    CU_ASSERT(_SPIM_DMM_GET_DESELTIM() == 0xd);

    /*
     *  _SPIM_DMM_ENABLE_BWEN() & _SPIM_DMM_DISABLE_BWEN()
     */
    SPIM->DMMCTL = 0;
    _SPIM_DMM_ENABLE_BWEN();
    CU_ASSERT_TRUE(SPIM->DMMCTL & SPIM_DMMCTL_BWEN_Msk);
    _SPIM_DMM_DISABLE_BWEN();
    CU_ASSERT_FALSE(SPIM->DMMCTL & SPIM_DMMCTL_BWEN_Msk);
    _SPIM_DMM_ENABLE_BWEN();
    CU_ASSERT_TRUE(SPIM->DMMCTL & SPIM_DMMCTL_BWEN_Msk);
    _SPIM_DMM_DISABLE_BWEN();
    CU_ASSERT_FALSE(SPIM->DMMCTL & SPIM_DMMCTL_BWEN_Msk);

    /*
     *  _SPIM_DMM_ENABLE_CREN() & _SPIM_DMM_DISABLE_CREN()
     */
    SPIM->DMMCTL = 0;
    _SPIM_DMM_ENABLE_CREN();
    CU_ASSERT_TRUE(SPIM->DMMCTL & SPIM_DMMCTL_CREN_Msk);
    _SPIM_DMM_DISABLE_CREN();
    CU_ASSERT_FALSE(SPIM->DMMCTL & SPIM_DMMCTL_CREN_Msk);
    _SPIM_DMM_ENABLE_CREN();
    CU_ASSERT_TRUE(SPIM->DMMCTL & SPIM_DMMCTL_CREN_Msk);
    _SPIM_DMM_DISABLE_CREN();
    CU_ASSERT_FALSE(SPIM->DMMCTL & SPIM_DMMCTL_CREN_Msk);

    /*
     *  _SPIM_DMM_SET_ACTSCLKT(x)
     */
    SPIM->DMMCTL = 0;
    _SPIM_DMM_SET_ACTSCLKT(0x3);
    CU_ASSERT(((SPIM->DMMCTL & SPIM_DMMCTL_ACTSCLKT_Msk) >> SPIM_DMMCTL_ACTSCLKT_Pos) == 0x3);
    CU_ASSERT_TRUE(SPIM->DMMCTL & SPIM_DMMCTL_UACTSCLK_Msk);
    SPIM->DMMCTL = 0;
    _SPIM_DMM_SET_ACTSCLKT(0xcul);
    CU_ASSERT(((SPIM->DMMCTL & SPIM_DMMCTL_ACTSCLKT_Msk) >> SPIM_DMMCTL_ACTSCLKT_Pos) == 0xc);
    CU_ASSERT_TRUE(SPIM->DMMCTL & SPIM_DMMCTL_UACTSCLK_Msk);

    /*
     *  _SPIM_DMM_SET_DEFAULT_ACTSCLK()
     */
    SPIM->DMMCTL |= SPIM_DMMCTL_UACTSCLK_Msk;
    _SPIM_DMM_SET_DEFAULT_ACTSCLK();
    CU_ASSERT_FALSE(SPIM->DMMCTL & SPIM_DMMCTL_UACTSCLK_Msk);

    CU_PASS();
}

void MACRO_SPIM_CTL2()
{
    /*
     *  _SPIM_SET_DCNUM(x)
     */
    SPIM->CTL2 = 0;
    _SPIM_SET_DCNUM(0x13);
    CU_ASSERT(((SPIM->CTL2 & SPIM_CTL2_DCNUM_Msk) >> SPIM_CTL2_DCNUM_Pos) == 0x13);
    CU_ASSERT_TRUE(SPIM->CTL2 & SPIM_CTL2_USETEN_Msk);
    SPIM->CTL2 = 0;
    _SPIM_SET_DCNUM(0xc);
    CU_ASSERT(((SPIM->CTL2 & SPIM_CTL2_DCNUM_Msk) >> SPIM_CTL2_DCNUM_Pos) == 0xc);
    CU_ASSERT_TRUE(SPIM->CTL2 & SPIM_CTL2_USETEN_Msk);

    /*
     *  _SPIM_SET_DEFAULT_DCNUM()
     */
    SPIM->CTL2 |= SPIM_CTL2_USETEN_Msk;
    _SPIM_SET_DEFAULT_DCNUM();
    CU_ASSERT_FALSE(SPIM->CTL2 & SPIM_CTL2_USETEN_Msk);

    CU_PASS();
}

void MACRO_SPIM_GetSClkFreq()
{
    _SPIM_SET_CLOCK_DIVIDER(0);

    CU_ASSERT(SPIM_GetSClkFreq() == SystemCoreClock);

    _SPIM_SET_CLOCK_DIVIDER(1);

    CU_ASSERT(SPIM_GetSClkFreq() == SystemCoreClock / 2);

    _SPIM_SET_CLOCK_DIVIDER(2);

    CU_ASSERT(SPIM_GetSClkFreq() == SystemCoreClock / 4);

    CU_PASS();
}

int  dma_read_write(int is4ByteAddr, uint32_t u32RdCmd, uint32_t WrCmd, int dc_num)
{
    uint32_t    i, offset;             /* variables */
    uint32_t    *pData;

    SPIM_Enable_4Bytes_Mode(is4ByteAddr, 1);

    if (is4ByteAddr)
    {
        CU_ASSERT(SPIM_Is4ByteModeEnable(1) == 1);
    }
    else
    {
        CU_ASSERT(SPIM_Is4ByteModeEnable(1) == 0);
    }

    _SPIM_SET_DCNUM(dc_num);

    /*
     *  Erase flash page
     */
    SPIM_EraseBlock(0, is4ByteAddr, OPCODE_BE_64K, 1, 1);

    /*
     *  Verify flash page be erased
     */
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        SPIM_DMA_Read(offset, is4ByteAddr, BUFFER_SIZE, g_buff, u32RdCmd, 1);

        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != 0xFFFFFFFF)
            {
                printf("FAILED!\n");
                printf("DMA R/W - Flash address 0x%x, read 0x%x!\n", i, *pData);
                return -1;
            }
        }
    }

    /*
     *  Program data to flash block
     */
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
            * pData = (i << 16) | (offset + i);

        SPIM_DMA_Write(offset, is4ByteAddr, BUFFER_SIZE, g_buff, WrCmd);
    }

    /*
     *  Verify flash block data
     */
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        SPIM_DMA_Read(offset, is4ByteAddr, BUFFER_SIZE, g_buff, u32RdCmd, 1);

        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != ((i << 16) | (offset + i)))
            {
                printf("FAILED!\n");
                printf("DMA R/W - Flash address 0x%x, read 0x%x, expect 0x%x!\n", i, *pData, (i << 16) | (offset + i));
                CU_FAIL();
            }
        }

    }

    printf("done.\n");
    return 0;
}

void SPIM_Basic_Func()
{
    SYS->IPRST0 = SYS_IPRST0_SPIMRST_Msk;
    SYS->IPRST0 &= ~SYS_IPRST0_SPIMRST_Msk;

    SPIM->KEY1 = SPIM_KEY_1;
    SPIM->KEY2 = SPIM_KEY_2;

    _SPIM_DISABLE_CIPHER();

    _SPIM_SET_CLOCK_DIVIDER(10);        /* Set SPIM clock as HCLK divided by 2 */
    _SPIM_SET_DCNUM(8);                /* 8 is the default value. */

    if (SPIM_InitFlash(1) != 0)        /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        CU_FAIL();
    }

    SPIM_ReadJedecId(idBuf, sizeof(idBuf), 1);
    printf("SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n", idBuf[0], idBuf[1], idBuf[2]);

    if ((idBuf[0] != MFGID_WINBOND) && (idBuf[0] != MFGID_MXIC) &&
            (idBuf[0] != MFGID_EON) && (idBuf[0] != MFGID_ISSI) && (idBuf[0] != MFGID_SPANSION))
    {
        CU_FAIL();
    }

    CU_PASS();
}

void SPIM_DMA_Func()
{
    _SPIM_SET_CLOCK_DIVIDER(10);        /* Set SPIM clock as HCLK divided by 2 */

    _SPIM_SET_DCNUM(8);                /* 8 is the default value. */

    if (SPIM_InitFlash(1) != 0)        /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        CU_FAIL();
    }

    SPIM_ReadJedecId(idBuf, sizeof(idBuf), 1);
    printf("SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n", idBuf[0], idBuf[1], idBuf[2]);

    if (dma_read_write(0, CMD_DMA_FAST_READ, CMD_NORMAL_PAGE_PROGRAM, 8) < 0)
    {
        printf("dma_read_write FAILED!!\n");
        CU_FAIL();
    }

#if 0

    if (dma_read_write(1, CMD_DMA_FAST_READ, CMD_NORMAL_PAGE_PROGRAM_4B, 8) < 0)
    {
        printf("dma_read_write FAILED!!\n");
        CU_FAIL();
    }

#endif

    CU_PASS();
}

void SPIM_Chip_Erase_Func()
{
    int        i, offset;
    uint32_t   *pData;

    _SPIM_SET_CLOCK_DIVIDER(10);       /* Set SPIM clock as HCLK divided by 2 */

    _SPIM_SET_DCNUM(8);                /* 8 is the default value. */

    if (SPIM_InitFlash(1) != 0)        /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        CU_FAIL();
    }

    if (dma_read_write(0, CMD_DMA_FAST_READ, CMD_NORMAL_PAGE_PROGRAM, 8) < 0)
    {
        printf("dma_read_write FAILED!!\n");
        CU_FAIL();
    }

    printf("Doing Chip Erase...\n");
    SPIM_ChipErase(1, 1);
    printf("Chip Erase done.\n");

    /*
     *  Verify flash page be erased
     */
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        SPIM_DMA_Read(offset, 0, BUFFER_SIZE, g_buff, CMD_DMA_FAST_READ, 1);

        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != 0xFFFFFFFF)
            {
                printf("FAILED!\n");
                printf("Chip Erase - Flash address 0x%x, read 0x%x!\n", i, *pData);
                CU_FAIL();
            }
        }
    }

    printf("done.\n");
    CU_PASS();
}

void SPIM_IO_RW_Func()
{
    int        i, offset;
    uint32_t   *pData;

    _SPIM_SET_CLOCK_DIVIDER(10);       /* Set SPIM clock as HCLK divided by 4 */

    if (SPIM_InitFlash(1) != 0)        /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        CU_FAIL();
    }

    SPIM_Enable_4Bytes_Mode(0, 1);

    CU_ASSERT(SPIM_Is4ByteModeEnable(1) == 0);

    /*
     *  Erase flash page
     */
    SPIM_EraseBlock(0, 0, OPCODE_BE_64K, 1, 1);

    SPIM_SetQuadEnable(1, 1);

    /*
     *  Verify flash page be erased
     */
    for (offset = 0; offset < 64 * 1024; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        SPIM_IO_Read(offset, 0, BUFFER_SIZE, g_buff, 0xeb, 1, 4, 4, 3);

        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != 0xFFFFFFFF)
            {
                printf("FAILED!\n");
                printf("I/O RW - Flash address 0x%x, read 0x%x!\n", i, *pData);
                CU_FAIL();
            }
        }
    }

    /*
     *  Program data to flash block
     */
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
            * pData = (i << 16) | (offset + i);

        SPIM_IO_Write(offset, 0, BUFFER_SIZE, g_buff, OPCODE_PP, 1, 1, 1);
    }

    /*
     *  Read and compare flash data
     */
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(g_buff, 0, BUFFER_SIZE);
        SPIM_IO_Read(offset, 0, BUFFER_SIZE, g_buff, 0xeb, 1, 4, 4, 3);

        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != ((i << 16) | (offset + i)))
            {
                printf("FAILED!\n");
                printf("I/O RW - Flash address 0x%x, read 0x%x, expect 0x%x!\n", i, *pData, (i << 16) | (offset + i));
                CU_FAIL();
            }
        }
    }

    SPIM_SetQuadEnable(0, 1);

    CU_PASS();
}

void SPIM_DMM_Func()
{
    int        i, offset;
    uint32_t   *pData;

    _SPIM_SET_CLOCK_DIVIDER(10);        /* Set SPIM clock as HCLK divided by 2 */

    if (SPIM_InitFlash(1) != 0)        /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        CU_FAIL();
    }

    if (dma_read_write(0, CMD_DMA_FAST_READ, CMD_NORMAL_PAGE_PROGRAM, 8) < 0)
    {
        printf("dma_read_write FAILED!!\n");
        CU_FAIL();
    }

    SPIM_EnterDirectMapMode(0, CMD_DMA_FAST_READ, 8);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memcpy(g_buff, (uint8_t *)(SPIM_DMM_MAP_ADDR + offset), BUFFER_SIZE);

        pData = (uint32_t *)g_buff;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != ((i << 16) | (offset + i)))
            {
                printf("FAILED!\n");
                printf("DMM - Flash address 0x%x, read 0x%x, expect 0x%x!\n", i, *pData, (i << 16) | (offset + i));
                SPIM_ExitDirectMapMode();
                CU_FAIL();
            }
        }
    }

    SPIM_ExitDirectMapMode();
    CU_PASS();
}

CU_TestInfo SPIM_ConstantTests[] =
{
    {"SPIM CONST Address", SPIM_Const_Address},
    {"SPIM CONST SPIM_CTL0", SPIM_Const_SPIM_CTL0},
    {"SPIM CONST SPIM_CMDCODE", SPIM_Const_SPIM_CMDCODE},
    {"SPIM CONST MFGID", SPIM_Const_MFGID},
    {"SPIM CONST OPCODE", SPIM_Const_OPCODE},
    {"SPIM CONST Register", SPIM_Const_Register},
    CU_TEST_INFO_NULL
};

CU_TestInfo SPIM_MacroTests[] =
{
    {"SPIM_CTL0 Macros", MACRO_SPIM_CTL0},
    {"SPIM_CTL1 Macros", MACRO_SPIM_CTL1},
    {"SPIM_RXCLKDLY Macros", MACRO_SPIM_RXCLKDLY},
    {"SPIM_DMMCTL Macros", MACRO_SPIM_DMMCTL},
    {"SPIM_CTL2 Macros", MACRO_SPIM_CTL2},
    {"SPIM_GetSClkFreq Macros", MACRO_SPIM_GetSClkFreq},
    CU_TEST_INFO_NULL
};

CU_TestInfo SPIM_ApiTests[] =
{
    {"SPIM Init/Read ID Function", SPIM_Basic_Func},
    {"SPIM DMA Function", SPIM_DMA_Func},
    {"SPIM I/O Read/Write Function", SPIM_IO_RW_Func},
    {"SPIM DMM Function", SPIM_DMM_Func},
    //   {"SPIM Chip Erase", SPIM_Chip_Erase_Func},
    CU_TEST_INFO_NULL
};
