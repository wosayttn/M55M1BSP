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
#include "hyper_cunit.h"
#include "common.h"

//------------------------------------------------------------------------------
extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;

//------------------------------------------------------------------------------
static uint8_t idBuf[3] = {0};

static CMD_PHASE_T gRdCMDPhase_Windond[] =
{
    /* Commnad,  Dummy Cycle, Is4ByteAddress, Address Mode , Data Mode, Continue Mode, DTR Mode */
    {CMD_NORMAL_READ, 0, 0, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, TRANS_MODE_NOR, 0}, //0x03
    {CMD_NORMAL_READ_4B, 0, 1, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, TRANS_MODE_NOR, 0}, //0x13

    {CMD_FAST_READ, 8, 0, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, TRANS_MODE_NOR, 0}, //0x0B
    {CMD_DTR_NORMAL_READ, 6, 0, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, TRANS_MODE_NOR, 1}, //0x0D
    {CMD_FAST_READ_4B, 8, 1, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, TRANS_MODE_NOR, 0}, //0x0C

    {CMD_DUAL_FAST_READ, 8, 0, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, PHASE_DUAL_MODE, TRANS_MODE_NOR, 0}, //0x3B
    {CMD_DUAL_FAST_READ_4B, 8, 1, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, PHASE_DUAL_MODE, TRANS_MODE_NOR, 0}, //0x3C

    {CMD_QUAD_FAST_READ, 8, 0, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, PHASE_QUAD_MODE, TRANS_MODE_NOR, 0}, //0x6B
    {CMD_QUAD_FAST_READ_4B, 8, 1, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, PHASE_QUAD_MODE, TRANS_MODE_NOR, 0}, //0x6C

    //Address and Data not Normal Mode
    { CMD_QUAD_FAST_IO_READ, 4, 0, PHASE_NORMAL_MODE, PHASE_QUAD_MODE, PHASE_QUAD_MODE, TRANS_MODE_NOR, 0}, //0xEB
    { CMD_QUAD_FAST_IO_READ, 4, 0, PHASE_NORMAL_MODE, PHASE_QUAD_MODE, PHASE_QUAD_MODE, TRANS_MODE_CONT, 0}, //0xEB

    { CMD_QUAD_FAST_DTR_READ, 7, 0, PHASE_NORMAL_MODE, PHASE_QUAD_MODE, PHASE_QUAD_MODE, TRANS_MODE_NOR, 1}, //0xED
    { CMD_QUAD_FAST_DTR_READ, 7, 0, PHASE_NORMAL_MODE, PHASE_QUAD_MODE, PHASE_QUAD_MODE, TRANS_MODE_CONT, 1}, //0xED

    { CMD_QUAD_FAST_IO_READ_4B, 4, 1, PHASE_NORMAL_MODE, PHASE_QUAD_MODE, PHASE_QUAD_MODE, TRANS_MODE_NOR, 0}, //0xEC
    { CMD_QUAD_FAST_IO_READ_4B, 4, 1, PHASE_NORMAL_MODE, PHASE_QUAD_MODE, PHASE_QUAD_MODE, TRANS_MODE_CONT, 0}, //0xEC

    //Dual Mode
    { CMD_DUAL_FAST_IO_READ, 0, 0, PHASE_NORMAL_MODE, PHASE_DUAL_MODE, PHASE_DUAL_MODE, TRANS_MODE_NOR, 0}, //0xBB Continue Read
    { CMD_DUAL_FAST_IO_READ, 0, 0, PHASE_NORMAL_MODE, PHASE_DUAL_MODE, PHASE_DUAL_MODE, TRANS_MODE_CONT, 0}, //0xBB Continue Read

    { CMD_DUAL_FAST_DTR_READ, 4, 0, PHASE_NORMAL_MODE, PHASE_DUAL_MODE, PHASE_DUAL_MODE, TRANS_MODE_NOR, 1}, //0xBD Continue Read
    { CMD_DUAL_FAST_DTR_READ, 4, 0, PHASE_NORMAL_MODE, PHASE_DUAL_MODE, PHASE_DUAL_MODE, TRANS_MODE_CONT, 1}, //0xBD Continue Read

    { CMD_DUAL_FAST_IO_READ_4B, 0, 1, PHASE_NORMAL_MODE, PHASE_DUAL_MODE, PHASE_DUAL_MODE, TRANS_MODE_NOR, 0}, //0xBC
    { CMD_DUAL_FAST_IO_READ_4B, 0, 1, PHASE_NORMAL_MODE, PHASE_DUAL_MODE, PHASE_DUAL_MODE, TRANS_MODE_CONT, 0}, //0xBC

    //Quad Wrap Around Mode
    { CMD_QUAD_FAST_IO_READ, 4, 0, PHASE_NORMAL_MODE, PHASE_QUAD_MODE, PHASE_QUAD_MODE, TRANS_MODE_WRAP_16Bit, 0}, //0xEB
    { CMD_QUAD_FAST_IO_READ, 4, 0, PHASE_NORMAL_MODE, PHASE_QUAD_MODE, PHASE_QUAD_MODE, TRANS_MODE_WRAP_16Bit | TRANS_MODE_CONT, 0}, //0xEB

    { 0, 0, 0, 0, 0, 0, 0}
};

static CMD_PHASE_T gWrCMDPhase_Windond[] =
{
    /* Commnad,  Dummy Cycle, Is4ByteAddress, Address Mode , Data Mode, Continue Mode, DTR Mode */
    {CMD_NORMAL_PAGE_PROGRAM, 0, 0, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, 0, 0}, //0x02
    {CMD_NORMAL_PAGE_PROGRAM_4B, 0, 1, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, 0, 0}, //0x12
    {CMD_QUAD_PAGE_PROGRAM_WINBOND, 0, 0, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, PHASE_QUAD_MODE, 0, 0}, //0x32
    {CMD_QUAD_PAGE_PROGRAM_WINBOND_4B, 0, 1, PHASE_NORMAL_MODE, PHASE_NORMAL_MODE, PHASE_QUAD_MODE, 0, 0}, //0x34
    { 0, 0, 0, 0, 0, 0, 0}
};

//------------------------------------------------------------------------------
void *GetWriteCMDPhaseTable(uint8_t u8CmdIdx)
{
    return &gWrCMDPhase_Windond[u8CmdIdx];
}

void *GetReadCMDPhaseTable(uint8_t u8CmdIdx)
{
    return &gRdCMDPhase_Windond[u8CmdIdx];
}

/*----------------------------------------------------------------------------*/
/* Test function                                                              */
/*----------------------------------------------------------------------------*/
void SPIMIrqHandle(SPIM_T *spim)
{
    printf("INT!\n");

    CU_ASSERT_TRUE((spim->CTL0 & SPIM_CTL0_IF_Msk));

    SPIM_CLR_INT(spim);

    CU_ASSERT_FALSE((spim->CTL0 & SPIM_CTL0_IF_Msk));
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
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    // SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 10);        /* Set SPIM clock as HCLK divided by 2 */

    // SPIM_SET_RXCLKDLY_RDDLYSEL(pSPIMModule, 2);    /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */
    // SPIM_SET_RXCLKDLY_RDEDGE(pSPIMModule);         /* Use SPI input clock rising edge to sample received data. */
    // SPIM_SET_DC_DMAR(pSPIMModule, 8);              /* Set 8 dummy cycle. */

    if (SPIM_InitFlash(pSPIMModule, 1) != 0) /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");

        while (1);
    }

    SPIM_ReadJedecId(pSPIMModule, idBuf, sizeof(idBuf), 1);
    printf("SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n",
           idBuf[0],
           idBuf[1],
           idBuf[2]);

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
    //{"SPIM CONST", SPIM_Tests_Init, SPIM_Tests_Clean, NULL, NULL, SPIM_ConstantTests},
    //{"SPIM MACRO", SPIM_Tests_Init, SPIM_Tests_Clean, NULL, NULL, SPIM_MacroTests},
    {"SPIM API", SPIM_Tests_Init, SPIM_Tests_Clean, NULL, NULL, SPIM_ApiTests},

    CU_SUITE_INFO_NULL,
};

void spim_dma_read(SPIM_T *spim, uint32_t u32Addr, uint32_t u32NRx, uint8_t *pu8RxBuf)
{
    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_PAGEREAD);// Switch to Page Read mode.

    SPIM_SET_SPIM_MODE(spim, CMD_FAST_READ);    // SPIM mode.
    CU_ASSERT(SPIM_GET_SPIM_MODE(spim) == CMD_FAST_READ);

    SPIM_SET_4BYTE_ADDR_EN(spim, 0);            // Enable/disable 4-Byte Address.

    spim->SRAMADDR = (uint32_t) pu8RxBuf;       // SRAM u32Address.
    spim->DMACNT = BUFFER_SIZE;                 // Transfer length.
    spim->FADDR = 0;                            // Flash u32Address.
    SPIM_SET_GO(spim);                          // Go.
    CU_ASSERT_TRUE(SPIM_IS_BUSY(spim));

    SPIM_WAIT_FREE(spim);                       // Wait for DMA done.
}

void SPIM_Const_Address()
{
    switch (GetSPIMTestModuleIdx())
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

    CU_ASSERT(SPIM_DMM_SIZE == 0x2000000UL);
    //CU_ASSERT(SPIM_CCM_ADDR == 0x20020000);
    //CU_ASSERT(SPIM_CCM_SIZE == 0x8000);
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

void SPIM_Const_SPIM_CMDCODE()
{
    //--------------------------------------------------------------------------
    // I/O Read Command
    //--------------------------------------------------------------------------
    CU_ASSERT(CMD_NORMAL_PAGE_PROGRAM == (0x02));
    CU_ASSERT(CMD_NORMAL_PAGE_PROGRAM_4B == (0x12));
    CU_ASSERT(CMD_QUAD_PAGE_PROGRAM_WINBOND == (0x32));
    CU_ASSERT(CMD_QUAD_PAGE_PROGRAM_WINBOND_4B == (0x34));
    CU_ASSERT(CMD_QUAD_PAGE_PROGRAM_MXIC == (0x38));
    CU_ASSERT(CMD_QUAD_PAGE_PROGRAM_EON == (0x40));

    CU_ASSERT(CMD_OCTAL_PAGE_PROG_MICRON == (0x82UL));
    CU_ASSERT(CMD_OCTAL_PAGE_PROG_MICRON_4B == (0x84UL));
    CU_ASSERT(CMD_OCTAL_EX_PAGE_PROG_MICRON == (0xC2UL));
    CU_ASSERT(CMD_OCTAL_EX_PAGE_PROG_MICRON_4B == (0x8EUL));

    CU_ASSERT(CMD_NORMAL_READ == (0x03UL));
    CU_ASSERT(CMD_NORMAL_READ_4B == (0x13UL));
    CU_ASSERT(CMD_FAST_READ == (0x0BUL));
    CU_ASSERT(CMD_FAST_READ_4B == (0x0CUL));

    //--------------------------------------------------------------------------
    // Dual Read Command
    //--------------------------------------------------------------------------
    CU_ASSERT(CMD_DUAL_FAST_READ == (0x3BUL));
    CU_ASSERT(CMD_DUAL_FAST_READ_4B == (0x3CUL));
    CU_ASSERT(CMD_DUAL_FAST_IO_READ == (0xBBUL));
    CU_ASSERT(CMD_DUAL_FAST_IO_READ_4B == (0xBCUL));
    CU_ASSERT(CMD_DUAL_FAST_DTR_READ == (0xBDUL));

    //--------------------------------------------------------------------------
    // Quad Read Command
    //--------------------------------------------------------------------------
    CU_ASSERT(CMD_QUAD_FAST_READ == (0x6BUL));
    CU_ASSERT(CMD_QUAD_FAST_READ_4B == (0x6CUL));
    CU_ASSERT(CMD_QUAD_FAST_IO_READ == (0xEBUL));
    CU_ASSERT(CMD_QUAD_FAST_IO_READ_4B == (0xECUL));
    CU_ASSERT(CMD_QUAD_FAST_DTR_READ == (0xEDUL));

    //--------------------------------------------------------------------------
    // DTR Read Command
    //--------------------------------------------------------------------------
    CU_ASSERT(CMD_DTR_NORMAL_READ == (0x0DUL));
    CU_ASSERT(CMD_DTR_DUAL_READ == (0xBDUL));
    CU_ASSERT(CMD_DTR_QUAD_READ == (0xEDUL));

    //--------------------------------------------------------------------------
    // Micron Octal Read Command
    //--------------------------------------------------------------------------
    CU_ASSERT(CMD_OCTAL_FAST_READ == (0x8BUL));
    CU_ASSERT(CMD_OCTAL_FAST_IO_READ == (0xCBUL));
    CU_ASSERT(CMD_OCTAL_FAST_READ_4B == (0x7CUL));
    CU_ASSERT(CMD_OCTAL_FAST_IO_READ_4B == (0xCCUL));
    CU_ASSERT(CMD_OCTAL_DDR_FAST_READ == (0x9DUL));
    CU_ASSERT(CMD_OCTAL_DDR_FAST_IO_READ == (0xFDUL));
}

void SPIM_Const_SPIM_PHASE()
{
    //--------------------------------------------------------------------------
    // Continue Read Mode Command
    //--------------------------------------------------------------------------
    CU_ASSERT(CMD_CLEAR_MODE_DATA == (0x00));
    CU_ASSERT(CMD_CONTINUE_READ_MODE == (0x20));

    CU_ASSERT(CMD_WRAP_8BIT == (0x00));
    CU_ASSERT(CMD_WRAP_16BIT == (0x20));
    CU_ASSERT(CMD_WRAP_32BIT == (0x40));
    CU_ASSERT(CMD_WRAP_64BIT == (0x60));

    /*------------------------------------------------------------------------*/
    /* SPIM_PHDMAW, SPIM_PHDMAR, SPIM_PHDMM constant definitions              */
    /*------------------------------------------------------------------------*/
    CU_ASSERT(PHASE_NORMAL_MODE == (0x0));
    CU_ASSERT(PHASE_DUAL_MODE == (0x1));
    CU_ASSERT(PHASE_QUAD_MODE == (0x2));
    CU_ASSERT(PHASE_OCTAL_MODE == (0x3));

    CU_ASSERT(PHASE_ORDER_MODE0 == (0x0));
    CU_ASSERT(PHASE_ORDER_MODE1 == (0x1));
    CU_ASSERT(PHASE_ORDER_MODE2 == (0x2));
    CU_ASSERT(PHASE_ORDER_MODE3 == (0x3));

    CU_ASSERT(PHASE_WIDTH_8 == (0x1));
    CU_ASSERT(PHASE_WIDTH_16 == (0x2));
    CU_ASSERT(PHASE_WIDTH_24 == (0x3));
    CU_ASSERT(PHASE_WIDTH_32 == (0x4));
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
    CU_ASSERT(OPCODE_WREN == (0x06U));
    CU_ASSERT(OPCODE_RDSR == (0x05U));
    CU_ASSERT(OPCODE_WRSR == (0x01U));
    CU_ASSERT(OPCODE_RDSR2 == (0x35U));
    CU_ASSERT(OPCODE_WRSR2 == (0x31U));
    CU_ASSERT(OPCODE_RDSR3 == (0x15U));
    CU_ASSERT(OPCODE_WRSR3 == (0x11U));
    CU_ASSERT(OPCODE_PP == (0x02U));
    CU_ASSERT(OPCODE_SE_4K == (0x20U));
    CU_ASSERT(OPCODE_BE_32K == (0x52U));
    CU_ASSERT(OPCODE_CHIP_ERASE == (0xc7U));
    CU_ASSERT(OPCODE_BE_64K == (0xd8U));
    CU_ASSERT(OPCODE_READ_ID == (0x90U));
    CU_ASSERT(OPCODE_RDID == (0x9fU));
    CU_ASSERT(OPCODE_BRRD == (0x16U));
    CU_ASSERT(OPCODE_BRWR == (0x17U));
    CU_ASSERT(OPCODE_NORM_READ == (0x03U));
    CU_ASSERT(OPCODE_FAST_READ == (0x0bU));
    CU_ASSERT(OPCODE_FAST_DUAL_READ == (0x3bU));
    CU_ASSERT(OPCODE_FAST_QUAD_READ == (0x6bU));

    CU_ASSERT(OPCODE_WR_NVCONFIG == (0xB1U));
    CU_ASSERT(OPCODE_WR_VCONFIG == (0x81U));
    CU_ASSERT(OPCODE_RD_NVCONFIG == (0xB5U));
    CU_ASSERT(OPCODE_RD_VCONFIG == (0x85U));

    CU_ASSERT(OPCODE_BP == (0x02U));
    CU_ASSERT(OPCODE_WRDI == (0x04U));
    CU_ASSERT(OPCODE_AAI_WP == (0xadU));

    CU_ASSERT(OPCODE_EN4B == (0xb7U));
    CU_ASSERT(OPCODE_EX4B == (0xe9U));

    CU_ASSERT(OPCODE_RDSCUR == (0x2bU));
    CU_ASSERT(OPCODE_WRSCUR == (0x2fU));

    CU_ASSERT(OPCODE_RSTEN == (0x66U));
    CU_ASSERT(OPCODE_RST == (0x99U));

    CU_ASSERT(OPCODE_ENQPI == (0x38U));
    CU_ASSERT(OPCODE_EXQPI == (0xFFU));

    /* Used for Micron flashes. */
    CU_ASSERT(OPCODE_FAST_OCTAL_OUTPUT_READ == (0x8BU));
    CU_ASSERT(OPCODE_FAST_OCTAL_IO_READ == (0xCBU));
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
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());
    //SPIM->KEY1 = SPIM_KEY_1;
    //SPIM->KEY2 = SPIM_KEY_2;

    /*
     *  SPIM_ENABLE_CIPHER()
     */
    SPIM_ENABLE_CIPHER(pSPIMModule);
    //printf("DESELTIM = %d\r\n", SPIM_DMM_GET_DESELTIM(pSPIMModule));
    CU_ASSERT_FALSE(pSPIMModule->CTL0 & SPIM_CTL0_CIPHOFF_Msk);
    CU_ASSERT(SPIM_DMM_GET_DESELTIM(pSPIMModule) == 18);

    /*
     *  SPIM_DISABLE_CIPHER()
     */
    SPIM_DISABLE_CIPHER(pSPIMModule);
    //printf("DESELTIM = %d\r\n", SPIM_DMM_GET_DESELTIM(pSPIMModule));
    CU_ASSERT_TRUE(pSPIMModule->CTL0 & SPIM_CTL0_CIPHOFF_Msk);
    CU_ASSERT(SPIM_DMM_GET_DESELTIM(pSPIMModule) == 8);

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
    CU_ASSERT_TRUE(SPIM_GET_4BYTE_ADDR_EN(pSPIMModule));

    SPIM_SET_4BYTE_ADDR_EN(pSPIMModule, 0);
    CU_ASSERT_FALSE(pSPIMModule->CTL0 & SPIM_CTL0_B4ADDREN_Msk);
    CU_ASSERT_FALSE(SPIM_GET_4BYTE_ADDR_EN(pSPIMModule));

    /*
     *  SPIM_ENABLE_INT()
     */
    SPIM_SET_INT(pSPIMModule, 1);
    CU_ASSERT(pSPIMModule->CTL0 & SPIM_CTL0_IEN_Msk);
    CU_ASSERT_TRUE(SPIM_GET_INT(pSPIMModule));
    //SPIM_ReadJedecId(pSPIMModule, idBuf, sizeof(idBuf), 1);

    SPIM_SET_INT(pSPIMModule, 0);
    CU_ASSERT_FALSE(pSPIMModule->CTL0 & SPIM_CTL0_IEN_Msk);
    CU_ASSERT_FALSE(SPIM_GET_INT(pSPIMModule));

    /*
     *  SPIM_SET_DATA_WIDTH()
     */
    pSPIMModule->CTL0 = 0;
    SPIM_SET_DATA_WIDTH(pSPIMModule, 8);
    CU_ASSERT(SPIM_GET_DATA_WIDTH(pSPIMModule) == 8);
    SPIM_SET_DATA_WIDTH(pSPIMModule, 16);
    CU_ASSERT(SPIM_GET_DATA_WIDTH(pSPIMModule) == 16);
    SPIM_SET_DATA_WIDTH(pSPIMModule, 24);
    CU_ASSERT(SPIM_GET_DATA_WIDTH(pSPIMModule) == 24);
    SPIM_SET_DATA_WIDTH(pSPIMModule, 32);
    CU_ASSERT(SPIM_GET_DATA_WIDTH(pSPIMModule) == 32);

    /*
     *  SPIM_SET_BURST_DATA()
     */
    pSPIMModule->CTL0 = 0;
    SPIM_SET_BURST_DATA(pSPIMModule, 1);
    CU_ASSERT(SPIM_GET_BURST_DATA(pSPIMModule) == 0x0);
    SPIM_SET_BURST_DATA(pSPIMModule, 2);
    CU_ASSERT(SPIM_GET_BURST_DATA(pSPIMModule) == 0x1);
    SPIM_SET_BURST_DATA(pSPIMModule, 3);
    CU_ASSERT(SPIM_GET_BURST_DATA(pSPIMModule) == 0x2);
    SPIM_SET_BURST_DATA(pSPIMModule, 4);
    CU_ASSERT(SPIM_GET_BURST_DATA(pSPIMModule) == 0x3);

    /*
     *  SPIM_ENABLE_SING_INPUT_MODE()
     */
    pSPIMModule->CTL0 = 0;
    SPIM_ENABLE_SING_INPUT_MODE(pSPIMModule);
    CU_ASSERT(pSPIMModule->CTL0 == (0 << SPIM_CTL0_BITMODE_Pos));

    /*
     *  SPIM_ENABLE_SING_OUTPUT_MODE()
     */
    pSPIMModule->CTL0 = 0;
    SPIM_ENABLE_SING_OUTPUT_MODE(pSPIMModule);
    CU_ASSERT(pSPIMModule->CTL0 == (SPIM_CTL0_QDIODIR_Msk | (0 << SPIM_CTL0_BITMODE_Pos)));

    /*
     *  SPIM_ENABLE_DUAL_INPUT_MODE()
     */
    pSPIMModule->CTL0 = 0;
    SPIM_ENABLE_DUAL_INPUT_MODE(pSPIMModule);
    CU_ASSERT(pSPIMModule->CTL0 == (0x1 << SPIM_CTL0_BITMODE_Pos));

    /*
     *  SPIM_ENABLE_DUAL_OUTPUT_MODE()
     */
    pSPIMModule->CTL0 = 0;
    SPIM_ENABLE_DUAL_OUTPUT_MODE(pSPIMModule);
    CU_ASSERT(pSPIMModule->CTL0 == (SPIM_CTL0_QDIODIR_Msk | (0x1 << SPIM_CTL0_BITMODE_Pos)));

    /*
     *  SPIM_ENABLE_QUAD_INPUT_MODE()
     */
    pSPIMModule->CTL0 = 0;
    SPIM_ENABLE_QUAD_INPUT_MODE(pSPIMModule);
    CU_ASSERT(pSPIMModule->CTL0 == (0x2 << SPIM_CTL0_BITMODE_Pos));

    /*
     *  SPIM_ENABLE_QUAD_OUTPUT_MODE()
     */
    pSPIMModule->CTL0 = 0;
    SPIM_ENABLE_QUAD_OUTPUT_MODE(pSPIMModule);
    CU_ASSERT(pSPIMModule->CTL0 == (SPIM_CTL0_QDIODIR_Msk | (0x2 << SPIM_CTL0_BITMODE_Pos)));

    /*
     *  SPIM_ENABLE_OCTAL_INPUT_MODE()
     */
    pSPIMModule->CTL0 = 0;
    SPIM_ENABLE_OCTAL_INPUT_MODE(pSPIMModule);
    CU_ASSERT(pSPIMModule->CTL0 == (0x3 << SPIM_CTL0_BITMODE_Pos));

    /*
     *  SPIM_ENABLE_OCTAL_OUTPUT_MODE()
     */
    pSPIMModule->CTL0 = 0;
    SPIM_ENABLE_OCTAL_OUTPUT_MODE(pSPIMModule);
    CU_ASSERT(pSPIMModule->CTL0 == (SPIM_CTL0_QDIODIR_Msk | (0x3 << SPIM_CTL0_BITMODE_Pos)));

    /*
     *  _SPIM_SET_SUSP_INTVL()
     */
    pSPIMModule->CTL0 = 0;
    SPIM_SET_SUSP_INTVL(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->CTL0 & SPIM_CTL0_SUSPITV_Msk) >> SPIM_CTL0_SUSPITV_Pos) == 0x3);
    SPIM_SET_SUSP_INTVL(pSPIMModule, 0x6);
    CU_ASSERT(((pSPIMModule->CTL0 & SPIM_CTL0_SUSPITV_Msk) >> SPIM_CTL0_SUSPITV_Pos) == 0x6);

    /*
     *  _SPIM_GET_SUSP_INTVL()
     */
    pSPIMModule->CTL0 = 0;
    SPIM_SET_SUSP_INTVL(pSPIMModule, 0x2);
    CU_ASSERT(SPIM_GET_SUSP_INTVL(pSPIMModule) == 0x2);
    SPIM_SET_SUSP_INTVL(pSPIMModule, 0x5);
    CU_ASSERT(SPIM_GET_SUSP_INTVL(pSPIMModule) == 0x5);

    /*
     *  SPIM_SET_OPMODE() / SPIM_GET_OP_MODE()
     */
    pSPIMModule->CTL0 = 0;
    SPIM_SET_OPMODE(pSPIMModule, SPIM_CTL0_OPMODE_IO);
    CU_ASSERT((pSPIMModule->CTL0 & SPIM_CTL0_OPMODE_Msk) == SPIM_CTL0_OPMODE_IO);
    SPIM_SET_OPMODE(pSPIMModule, SPIM_CTL0_OPMODE_PAGEWRITE);
    CU_ASSERT((pSPIMModule->CTL0 & SPIM_CTL0_OPMODE_Msk) == SPIM_CTL0_OPMODE_PAGEWRITE);
    SPIM_SET_OPMODE(pSPIMModule, SPIM_CTL0_OPMODE_PAGEREAD);
    CU_ASSERT((pSPIMModule->CTL0 & SPIM_CTL0_OPMODE_Msk) == SPIM_CTL0_OPMODE_PAGEREAD);
    SPIM_SET_OPMODE(pSPIMModule, SPIM_CTL0_OPMODE_DIRECTMAP);
    CU_ASSERT((pSPIMModule->CTL0 & SPIM_CTL0_OPMODE_Msk) == SPIM_CTL0_OPMODE_DIRECTMAP);

    /*
    *  SPIM_SET_DTR_MODE() / SPIM_GET_DTR_MODE()
    */
    pSPIMModule->CTL0 = 0;
    SPIM_SET_DTR_MODE(pSPIMModule, 1);
    CU_ASSERT_TRUE(SPIM_GET_DTR_MODE(pSPIMModule));
    SPIM_SET_DTR_MODE(pSPIMModule, 0);
    CU_ASSERT_FALSE(SPIM_GET_DTR_MODE(pSPIMModule));

    /*
    *  SPIM_SET_RDQS_MODE() / SPIM_GET_RDQS_MODE()
    */
    pSPIMModule->CTL0 = 0;
    SPIM_SET_RDQS_MODE(pSPIMModule, 1);
    CU_ASSERT(SPIM_GET_RDQS_MODE(pSPIMModule) == 1);
    SPIM_SET_RDQS_MODE(pSPIMModule, 0);
    CU_ASSERT(SPIM_GET_RDQS_MODE(pSPIMModule) == 0);

    /*
    *  SPIM_SET_RBO_MODE() / SPIM_GET_RBO_MODE()
    */
    pSPIMModule->CTL0 = 0;
    SPIM_SET_RBO_MODE(pSPIMModule, 1);
    CU_ASSERT(SPIM_GET_RBO_MODE(pSPIMModule) == 1);
    SPIM_SET_RBO_MODE(pSPIMModule, 0);
    CU_ASSERT(SPIM_GET_RBO_MODE(pSPIMModule) == 0);
}

void MACRO_SPIM_CTL1()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());
#if (SPIM_CACHE_EN == 1)
    /*
     *  SPIM_ENABLE_CACHE()
     */
    pSPIMModule->CTL1 = 0;
    SPIM_ENABLE_CACHE(pSPIMModule);
    CU_ASSERT_FALSE(pSPIMModule->CTL1 & SPIM_CTL1_CACHEOFF_Msk);

    /*
     *  _SPIM_DISABLE_CACHE()
     */
    pSPIMModule->CTL1 = 0;
    SPIM_DISABLE_CACHE(pSPIMModule);
    CU_ASSERT_TRUE(pSPIMModule->CTL1 & SPIM_CTL1_CACHEOFF_Msk);

    /*
     *  SPIM_IS_CACHE_EN()
     */
    pSPIMModule->CTL1 = 0;
    SPIM_ENABLE_CACHE(pSPIMModule);
    CU_ASSERT_TRUE(SPIM_IS_CACHE_EN(pSPIMModule));
    SPIM_DISABLE_CACHE();
    CU_ASSERT_FALSE(SPIM_IS_CACHE_EN(pSPIMModule));

    /*
     *  SPIM_INVALID_CACHE()
     */
    pSPIMModule->CTL1 = 0;
    SPIM_INVALID_CACHE(pSPIMModule);
    CU_ASSERT_TRUE((pSPIMModule->CTL1 & SPIM_CTL1_CDINVAL_Msk) == 1);

#endif //SPIM_CACHE_EN

    /*
     *  _SPIM_ENABLE_CCM()
     */
    //SPIM->CTL1 = 0;
    //_SPIM_ENABLE_CCM();
    //_SPIM_INVALID_CACHE();
    //CU_ASSERT_TRUE(SPIM->CTL1 & SPIM_CTL1_CCMEN_Msk);

    /*
     *  _SPIM_DISABLE_CCM()
     */
    //_SPIM_DISABLE_CCM();
    //CU_ASSERT_FALSE(SPIM->CTL1 & SPIM_CTL1_CCMEN_Msk);

    /*
     *  _SPIM_IS_CCM_EN()
     */
    //SPIM->CTL1 = 0;
    //_SPIM_ENABLE_CCM();
    //CU_ASSERT_TRUE(_SPIM_IS_CCM_EN());
    //_SPIM_DISABLE_CCM();
    //CU_ASSERT_FALSE(_SPIM_IS_CCM_EN());

    /*
     *  _SPIM_SET_SS_EN(x)
     */
    pSPIMModule->CTL1 = 0;
    SPIM_SET_SS_EN(pSPIMModule, 1);
    CU_ASSERT_FALSE(pSPIMModule->CTL1 & SPIM_CTL1_SS_Msk);
    SPIM_SET_SS_EN(pSPIMModule, 0);
    CU_ASSERT_TRUE(pSPIMModule->CTL1 & SPIM_CTL1_SS_Msk);

    /*
     *  _SPI_GET_SS_EN()
     */
    pSPIMModule->CTL1 = 0;
    SPIM_SET_SS_EN(pSPIMModule, 1);
    CU_ASSERT_TRUE(SPIM_GET_SS_EN(pSPIMModule));
    SPIM_SET_SS_EN(pSPIMModule, 0);
    CU_ASSERT_FALSE(SPIM_GET_SS_EN(pSPIMModule));

    /*
     *  _SPI_SET_SS_ACTLVL(x)
     */
    pSPIMModule->CTL1 = 0;
    SPIM_SET_SS_ACTLVL(pSPIMModule, 0);
    CU_ASSERT_FALSE(pSPIMModule->CTL1 & SPIM_CTL1_SSACTPOL_Msk);
    SPIM_SET_SS_ACTLVL(pSPIMModule, 1);
    CU_ASSERT_TRUE(pSPIMModule->CTL1 & SPIM_CTL1_SSACTPOL_Msk);

    /*
     *  SPIM_SET_IDL_INTVL()
     */
    pSPIMModule->CTL1 = 0;
    SPIM_SET_IDL_INTVL(pSPIMModule, 3);
    CU_ASSERT(((pSPIMModule->CTL1 & SPIM_CTL1_IDLETIME_Msk) >> SPIM_CTL1_IDLETIME_Pos) == 0x03);
    SPIM_SET_IDL_INTVL(pSPIMModule, 6);
    CU_ASSERT(((pSPIMModule->CTL1 & SPIM_CTL1_IDLETIME_Msk) >> SPIM_CTL1_IDLETIME_Pos) == 0x06);

    /*
     *  SPIM_GET_IDL_INTVL()
     */
    pSPIMModule->CTL1 = 0;
    SPIM_SET_IDL_INTVL(pSPIMModule, 2);
    CU_ASSERT(SPIM_GET_IDL_INTVL(pSPIMModule) == 0x02);
    SPIM_SET_IDL_INTVL(pSPIMModule, 4);
    CU_ASSERT(SPIM_GET_IDL_INTVL(pSPIMModule) == 0x04);

    /*
     *  SPIM_SET_CLOCK_DIVIDER(x)
     */
    pSPIMModule->CTL1 = 0;
    SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 3);
    CU_ASSERT(((pSPIMModule->CTL1 & SPIM_CTL1_DIVIDER_Msk) >> SPIM_CTL1_DIVIDER_Pos) == 0x03);
    SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 6);
    CU_ASSERT(((pSPIMModule->CTL1 & SPIM_CTL1_DIVIDER_Msk) >> SPIM_CTL1_DIVIDER_Pos) == 0x06);

    /*
     *  SPIM_GET_CLOCK_DIVIDER()
     */
    pSPIMModule->CTL1 = 0;
    SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 2);
    CU_ASSERT(SPIM_GET_CLOCK_DIVIDER(pSPIMModule) == 0x02);
    SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 4);
    CU_ASSERT(SPIM_GET_CLOCK_DIVIDER(pSPIMModule) == 0x04);

    CU_PASS();
}

void MACRO_SPIM_RXCLKDLY()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());
#if 0
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
#endif //0

    /*
     *  SPIM_SET_RXCLKDLY_RDDLYSEL(x)
     */
    pSPIMModule->RXCLKDLY = 0;
    SPIM_SET_RXCLKDLY_RDDLYSEL(pSPIMModule, 2);
    CU_ASSERT(((pSPIMModule->RXCLKDLY & SPIM_RXCLKDLY_RDDLYSEL_Msk) >> SPIM_RXCLKDLY_RDDLYSEL_Pos) == 0x02);
    SPIM_SET_RXCLKDLY_RDDLYSEL(pSPIMModule, 3);
    CU_ASSERT(((pSPIMModule->RXCLKDLY & SPIM_RXCLKDLY_RDDLYSEL_Msk) >> SPIM_RXCLKDLY_RDDLYSEL_Pos) == 0x03);

    /*
     *  _SPIM_GET_RXCLKDLY_RDDLYSEL()
     */
    pSPIMModule->RXCLKDLY = 0;
    SPIM_SET_RXCLKDLY_RDDLYSEL(pSPIMModule, 3);
    CU_ASSERT(SPIM_GET_RXCLKDLY_RDDLYSEL(pSPIMModule) == 0x03);
    SPIM_SET_RXCLKDLY_RDDLYSEL(pSPIMModule, 1);
    CU_ASSERT(SPIM_GET_RXCLKDLY_RDDLYSEL(pSPIMModule) == 0x01);

#if 0
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
#endif //0

    CU_PASS();
}

void MACRO_SPIM_DMMCTL()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());
#if 0
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
#endif //0

    /*
     *  SPIM_DMAM_SET_ACTSCLKT(x)
     */
    pSPIMModule->DMMCTL = 0;
    SPIM_DMAM_SET_ACTSCLKT(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->DMMCTL & SPIM_DMMCTL_ACTSCLKT_Msk) >> SPIM_DMMCTL_ACTSCLKT_Pos) == 0x3);
    CU_ASSERT_TRUE(pSPIMModule->DMMCTL & SPIM_DMMCTL_UACTSCLK_Msk);

    pSPIMModule->DMMCTL = 0;
    SPIM_DMAM_SET_ACTSCLKT(pSPIMModule, 0xcul);
    CU_ASSERT(((pSPIMModule->DMMCTL & SPIM_DMMCTL_ACTSCLKT_Msk) >> SPIM_DMMCTL_ACTSCLKT_Pos) == 0xc);
    CU_ASSERT_TRUE(pSPIMModule->DMMCTL & SPIM_DMMCTL_UACTSCLK_Msk);

    /*
     *  SPIM_DMAM_SET_ACTSCLKT(x)
     */
    pSPIMModule->DMMCTL = 0;
    SPIM_DMAM_SET_ACTSCLKT(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->DMMCTL & SPIM_DMMCTL_ACTSCLKT_Msk) >> SPIM_DMMCTL_ACTSCLKT_Pos) == 0x3);
    CU_ASSERT_TRUE(pSPIMModule->DMMCTL & SPIM_DMMCTL_UACTSCLK_Msk);

    /*
     *  SPIM_DMM_SET_DESELTIM()
     */
    pSPIMModule->DMMCTL |= 0;
    SPIM_DMM_SET_DESELTIM(pSPIMModule, 0x03);
    CU_ASSERT(SPIM_DMM_GET_DESELTIM(pSPIMModule) == 0x3);

    pSPIMModule->DMMCTL |= 0;
    SPIM_DMM_SET_DESELTIM(pSPIMModule, 0xcul);
    CU_ASSERT(SPIM_DMM_GET_DESELTIM(pSPIMModule) == 0xcul);

    /*
     *  SPIM_DMM_ENABLE_BWEN() & SPIM_DMM_DISABLE_BWEN()
     */
    pSPIMModule->DMMCTL = 0;
    SPIM_DMM_ENABLE_BWEN(pSPIMModule);
    CU_ASSERT_TRUE(pSPIMModule->DMMCTL & SPIM_DMMCTL_BWEN_Msk);
    SPIM_DMM_DISABLE_BWEN(pSPIMModule);
    CU_ASSERT_FALSE(pSPIMModule->DMMCTL & SPIM_DMMCTL_BWEN_Msk);
    SPIM_DMM_ENABLE_BWEN(pSPIMModule);
    CU_ASSERT_TRUE(pSPIMModule->DMMCTL & SPIM_DMMCTL_BWEN_Msk);
    SPIM_DMM_DISABLE_BWEN(pSPIMModule);
    CU_ASSERT_FALSE(pSPIMModule->DMMCTL & SPIM_DMMCTL_BWEN_Msk);

    /*
     *  SPIM_DMM_ENABLE_CREN() & SPIM_DMM_DISABLE_CREN()
     */
    pSPIMModule->DMMCTL = 0;
    SPIM_DMM_ENABLE_CREN(pSPIMModule);
    CU_ASSERT_TRUE(pSPIMModule->DMMCTL & SPIM_DMMCTL_CREN_Msk);
    SPIM_DMM_DISABLE_CREN(pSPIMModule);
    CU_ASSERT_FALSE(pSPIMModule->DMMCTL & SPIM_DMMCTL_CREN_Msk);
    SPIM_DMM_ENABLE_CREN(pSPIMModule);
    CU_ASSERT_TRUE(pSPIMModule->DMMCTL & SPIM_DMMCTL_CREN_Msk);
    SPIM_DMM_DISABLE_CREN(pSPIMModule);
    CU_ASSERT_FALSE(pSPIMModule->DMMCTL & SPIM_DMMCTL_CREN_Msk);

    /*
     *  SPIM_DMM_HYPDONE()
     */
    pSPIMModule->DMMCTL |= 0;
    SPIM_DMM_HYPDONE(pSPIMModule);
    SPIM_WAIT_HYPDONE(pSPIMModule) {}
    CU_ASSERT_FALSE((pSPIMModule->DMMCTL & SPIM_DMMCTL_HYPDONE_Msk) >> SPIM_DMMCTL_HYPDONE_Pos);

    CU_PASS();
}

void MACRO_SPIM_CTL2()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());
#if 0
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
#endif //0
    /*
     *  SPIM_SET_DC_DMAR(x)
     */
    pSPIMModule->CTL2 = 0;
    SPIM_SET_DC_DMAR(pSPIMModule, 0x13);
    CU_ASSERT(((pSPIMModule->CTL2 & SPIM_CTL2_DC_DMAR_Msk) >> SPIM_CTL2_DC_DMAR_Pos) == 0x13);
    SPIM_CLEAR_DC_DMAR(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->CTL2 & SPIM_CTL2_DC_DMAR_Msk) >> SPIM_CTL2_DC_DMAR_Pos));

    pSPIMModule->CTL2 = 0;
    SPIM_SET_DC_DMAR(pSPIMModule, 0x0c);
    CU_ASSERT(((pSPIMModule->CTL2 & SPIM_CTL2_DC_DMAR_Msk) >> SPIM_CTL2_DC_DMAR_Pos) == 0x0c);
    SPIM_CLEAR_DC_DMAR(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->CTL2 & SPIM_CTL2_DC_DMAR_Msk) >> SPIM_CTL2_DC_DMAR_Pos));

    /*
     *  SPIM_SET_DC_DMAR(x)
     */
    pSPIMModule->CTL2 = 0;
    SPIM_SET_DC_DMM(pSPIMModule, 0x13);
    CU_ASSERT(((pSPIMModule->CTL2 & SPIM_CTL2_DC_DMM_Msk) >> SPIM_CTL2_DC_DMM_Pos) == 0x13);
    SPIM_CLEAR_DC_DMM(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->CTL2 & SPIM_CTL2_DC_DMM_Msk) >> SPIM_CTL2_DC_DMM_Pos));

    pSPIMModule->CTL2 = 0;
    SPIM_SET_DC_DMM(pSPIMModule, 0x0c);
    CU_ASSERT(((pSPIMModule->CTL2 & SPIM_CTL2_DC_DMM_Msk) >> SPIM_CTL2_DC_DMM_Pos) == 0x0c);
    SPIM_CLEAR_DC_DMM(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->CTL2 & SPIM_CTL2_DC_DMM_Msk) >> SPIM_CTL2_DC_DMM_Pos));

    CU_PASS();
}

void MACRO_SPIM_DMA_WRITE_PHASE()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());
    /*
     *  SPIM_PHDMAW_CMD_WIDTH(x)
     */
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_CMD_WIDTH(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_DW_CMD_Msk) >> SPIM_PHDMAW_DW_CMD_Pos) == 0x1);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_CMD_WIDTH(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_DW_CMD_Msk) >> SPIM_PHDMAW_DW_CMD_Pos) == 0x2);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_CMD_WIDTH(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_DW_CMD_Msk) >> SPIM_PHDMAW_DW_CMD_Pos) == 0x3);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_CMD_WIDTH(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_DW_CMD_Msk) >> SPIM_PHDMAW_DW_CMD_Pos) == 0x4);

    /*
     *  SPIM_PHDMAW_CMD_DTR_ENABLE(x)
     */
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_CMD_DTR_ENABLE(pSPIMModule);
    CU_ASSERT_TRUE(((pSPIMModule->PHDMAW & SPIM_PHDMAW_DTR_CMD_Msk) >> SPIM_PHDMAW_DTR_CMD_Pos));
    SPIM_PHDMAW_CMD_DTR_DISABLE(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->PHDMAW & SPIM_PHDMAW_DTR_CMD_Msk) >> SPIM_PHDMAW_DTR_CMD_Pos));

    /*
     *  SPIM_PHDMAW_CMD_BIT_MODE(x)
     */
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_CMD_BIT_MODE(pSPIMModule, 0);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_BM_CMD_Msk) >> SPIM_PHDMAW_BM_CMD_Pos) == 0x0);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_CMD_BIT_MODE(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_BM_CMD_Msk) >> SPIM_PHDMAW_BM_CMD_Pos) == 0x1);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_CMD_BIT_MODE(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_BM_CMD_Msk) >> SPIM_PHDMAW_BM_CMD_Pos) == 0x2);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_CMD_BIT_MODE(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_BM_CMD_Msk) >> SPIM_PHDMAW_BM_CMD_Pos) == 0x3);

    /*
      *  SPIM_PHDMAW_ADDR_WIDTH(x)
      */
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_ADDR_WIDTH(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_DW_ADDR_Msk) >> SPIM_PHDMAW_DW_ADDR_Pos) == 0x1);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_ADDR_WIDTH(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_DW_ADDR_Msk) >> SPIM_PHDMAW_DW_ADDR_Pos) == 0x2);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_ADDR_WIDTH(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_DW_ADDR_Msk) >> SPIM_PHDMAW_DW_ADDR_Pos) == 0x3);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_ADDR_WIDTH(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_DW_ADDR_Msk) >> SPIM_PHDMAW_DW_ADDR_Pos) == 0x4);

    /*
     *  SPIM_PHDMAW_ADDR_DTR_ENABLE(x)/ SPIM_PHDMAW_ADDR_DTR_DISABLE(x)
     */
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_ADDR_DTR_ENABLE(pSPIMModule);
    CU_ASSERT_TRUE(((pSPIMModule->PHDMAW & SPIM_PHDMAW_DTR_ADDR_Msk) >> SPIM_PHDMAW_DTR_ADDR_Pos));
    SPIM_PHDMAW_ADDR_DTR_DISABLE(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->PHDMAW & SPIM_PHDMAW_DTR_ADDR_Msk) >> SPIM_PHDMAW_DTR_ADDR_Pos));

    /*
      *  SPIM_PHDMAW_ADDR_BIT_MODE(x)
      */
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_ADDR_BIT_MODE(pSPIMModule, 0x0);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_BM_ADDR_Msk) >> SPIM_PHDMAW_BM_ADDR_Pos) == 0x0);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_ADDR_BIT_MODE(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_BM_ADDR_Msk) >> SPIM_PHDMAW_BM_ADDR_Pos) == 0x1);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_ADDR_BIT_MODE(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_BM_ADDR_Msk) >> SPIM_PHDMAW_BM_ADDR_Pos) == 0x2);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_ADDR_BIT_MODE(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_BM_ADDR_Msk) >> SPIM_PHDMAW_BM_ADDR_Pos) == 0x3);

    /*
     *  SPIM_PHDMAW_DATA_DTR_ENABLE(x)/ SPIM_PHDMAW_DATA_DTR_DISABLE(x)
     */
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_DATA_DTR_ENABLE(pSPIMModule);
    CU_ASSERT_TRUE(((pSPIMModule->PHDMAW & SPIM_PHDMAW_DTR_DATA_Msk) >> SPIM_PHDMAW_DTR_DATA_Pos));
    SPIM_PHDMAW_DATA_DTR_DISABLE(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->PHDMAW & SPIM_PHDMAW_DTR_DATA_Msk) >> SPIM_PHDMAW_DTR_DATA_Pos));

    /*
      *  SPIM_PHDMAW_PBO_DATA(x)
      */
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_PBO_DATA(pSPIMModule, 0x0);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_PBO_DATA_Msk) >> SPIM_PHDMAW_PBO_DATA_Pos) == 0x0);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_PBO_DATA(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_PBO_DATA_Msk) >> SPIM_PHDMAW_PBO_DATA_Pos) == 0x1);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_PBO_DATA(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_PBO_DATA_Msk) >> SPIM_PHDMAW_PBO_DATA_Pos) == 0x2);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_PBO_DATA(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_PBO_DATA_Msk) >> SPIM_PHDMAW_PBO_DATA_Pos) == 0x3);

    /*
      *  SPIM_PHDMAW_DATA_BIT_MODE(x)
      */
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_DATA_BIT_MODE(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_BM_DATA_Msk) >> SPIM_PHDMAW_BM_DATA_Pos) == 0x1);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_DATA_BIT_MODE(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_BM_DATA_Msk) >> SPIM_PHDMAW_BM_DATA_Pos) == 0x2);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_DATA_BIT_MODE(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_BM_DATA_Msk) >> SPIM_PHDMAW_BM_DATA_Pos) == 0x3);
    pSPIMModule->PHDMAW = 0;
    SPIM_PHDMAW_DATA_BIT_MODE(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMAW & SPIM_PHDMAW_BM_DATA_Msk) >> SPIM_PHDMAW_BM_DATA_Pos) == 0x4);

    SPIM_CLEAR_PHDMAW(pSPIMModule);
    CU_ASSERT(pSPIMModule->PHDMAW == 0x0);

    CU_PASS();
}

void MACRO_SPIM_DMA_READ_PHASE()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());
    /*
     *  SPIM_PHDMAR_CMD_WIDTH(x)
     */
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_CMD_WIDTH(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DW_CMD_Msk) >> SPIM_PHDMAR_DW_CMD_Pos) == 0x1);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_CMD_WIDTH(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DW_CMD_Msk) >> SPIM_PHDMAR_DW_CMD_Pos) == 0x2);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_CMD_WIDTH(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DW_CMD_Msk) >> SPIM_PHDMAR_DW_CMD_Pos) == 0x3);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_CMD_WIDTH(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DW_CMD_Msk) >> SPIM_PHDMAR_DW_CMD_Pos) == 0x4);

    /*
     *  SPIM_PHDMAR_CMD_DTR_ENABLE(x) / SPIM_PHDMAR_CMD_DTR_DISABLE(x)
     */
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_CMD_DTR_ENABLE(pSPIMModule);
    CU_ASSERT_TRUE(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DTR_CMD_Msk) >> SPIM_PHDMAR_DTR_CMD_Pos));
    SPIM_PHDMAR_CMD_DTR_DISABLE(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DTR_CMD_Msk) >> SPIM_PHDMAR_DTR_CMD_Pos));

    /*
     *  SPIM_PHDMAR_CMD_BIT_MODE(x)
     */
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_CMD_BIT_MODE(pSPIMModule, 0);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_CMD_Msk) >> SPIM_PHDMAR_BM_CMD_Pos) == 0x0);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_CMD_BIT_MODE(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_CMD_Msk) >> SPIM_PHDMAR_BM_CMD_Pos) == 0x1);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_CMD_BIT_MODE(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_CMD_Msk) >> SPIM_PHDMAR_BM_CMD_Pos) == 0x2);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_CMD_BIT_MODE(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_CMD_Msk) >> SPIM_PHDMAR_BM_CMD_Pos) == 0x3);

    /*
      *  SPIM_PHDMAR_ADDR_WIDTH(x)
      */
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_ADDR_WIDTH(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DW_ADDR_Msk) >> SPIM_PHDMAR_DW_ADDR_Pos) == 0x1);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_ADDR_WIDTH(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DW_ADDR_Msk) >> SPIM_PHDMAR_DW_ADDR_Pos) == 0x2);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_ADDR_WIDTH(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DW_ADDR_Msk) >> SPIM_PHDMAR_DW_ADDR_Pos) == 0x3);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_ADDR_WIDTH(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DW_ADDR_Msk) >> SPIM_PHDMAR_DW_ADDR_Pos) == 0x4);

    /*
     *  SPIM_PHDMAR_ADDR_DTR_ENABLE(x)/ SPIM_PHDMAR_ADDR_DTR_DISABLE(x)
     */
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_ADDR_DTR_ENABLE(pSPIMModule);
    CU_ASSERT_TRUE(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DTR_ADDR_Msk) >> SPIM_PHDMAR_DTR_ADDR_Pos));
    SPIM_PHDMAR_ADDR_DTR_DISABLE(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DTR_ADDR_Msk) >> SPIM_PHDMAR_DTR_ADDR_Pos));

    /*
      *  SPIM_PHDMAR_ADDR_BIT_MODE(x)
      */
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_ADDR_BIT_MODE(pSPIMModule, 0x0);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_ADDR_Msk) >> SPIM_PHDMAR_BM_ADDR_Pos) == 0x0);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_ADDR_BIT_MODE(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_ADDR_Msk) >> SPIM_PHDMAR_BM_ADDR_Pos) == 0x1);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_ADDR_BIT_MODE(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_ADDR_Msk) >> SPIM_PHDMAR_BM_ADDR_Pos) == 0x2);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_ADDR_BIT_MODE(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_ADDR_Msk) >> SPIM_PHDMAR_BM_ADDR_Pos) == 0x3);

    /*
      *  SPIM_PHDMAR_READ_DATA_WIDTH(x)
      */
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_READ_DATA_WIDTH(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DW_MODE_Msk) >> SPIM_PHDMAR_DW_MODE_Pos) == 0x1);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_READ_DATA_WIDTH(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DW_MODE_Msk) >> SPIM_PHDMAR_DW_MODE_Pos) == 0x2);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_READ_DATA_WIDTH(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DW_MODE_Msk) >> SPIM_PHDMAR_DW_MODE_Pos) == 0x3);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_READ_DATA_WIDTH(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DW_MODE_Msk) >> SPIM_PHDMAR_DW_MODE_Pos) == 0x4);

    /*
     *  SPIM_PHDMAR_READ_DTR_ENABLE(x)/ SPIM_PHDMAR_READ_DTR_DISABLE(x)
     */
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_READ_DTR_ENABLE(pSPIMModule);
    CU_ASSERT_TRUE(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DTR_MODE_Msk) >> SPIM_PHDMAR_DTR_MODE_Pos));
    SPIM_PHDMAR_READ_DTR_DISABLE(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DTR_MODE_Msk) >> SPIM_PHDMAR_DTR_MODE_Pos));

    /*
      *  SPIM_PHDMAR_READ_BIT_MODE(x)
      */
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_READ_BIT_MODE(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_MODE_Msk) >> SPIM_PHDMAR_BM_MODE_Pos) == 0x1);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_READ_BIT_MODE(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_MODE_Msk) >> SPIM_PHDMAR_BM_MODE_Pos) == 0x2);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_READ_BIT_MODE(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_MODE_Msk) >> SPIM_PHDMAR_BM_MODE_Pos) == 0x3);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_READ_BIT_MODE(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_MODE_Msk) >> SPIM_PHDMAR_BM_MODE_Pos) == 0x4);

    /*
     *  SPIM_PHDMAR_DATA_DTR_ENABLE(x)/ SPIM_PHDMAR_DATA_DTR_DISABLE(x)
     */
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_DATA_DTR_ENABLE(pSPIMModule);
    CU_ASSERT_TRUE(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DTR_DATA_Msk) >> SPIM_PHDMAR_DTR_DATA_Pos));
    SPIM_PHDMAR_DATA_DTR_DISABLE(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->PHDMAR & SPIM_PHDMAR_DTR_DATA_Msk) >> SPIM_PHDMAR_DTR_DATA_Pos));

    /*
     *  SPIM_PHDMAR_DATA_RDQS_ENABLE(x)/ SPIM_PHDMAR_DATA_RDQS_DISABLE(x)
     */
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_DATA_RDQS_ENABLE(pSPIMModule);
    CU_ASSERT_TRUE(((pSPIMModule->PHDMAR & SPIM_PHDMAR_RDQS_DATA_Msk) >> SPIM_PHDMAR_RDQS_DATA_Pos));
    SPIM_PHDMAR_DATA_RDQS_DISABLE(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->PHDMAR & SPIM_PHDMAR_RDQS_DATA_Msk) >> SPIM_PHDMAR_RDQS_DATA_Pos));

    /*
      *  SPIM_PHDMAR_RBO_DATA(x)
      */
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_RBO_DATA(pSPIMModule, 0x0);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_RBO_DATA_Msk) >> SPIM_PHDMAR_RBO_DATA_Pos) == 0x0);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_RBO_DATA(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_RBO_DATA_Msk) >> SPIM_PHDMAR_RBO_DATA_Pos) == 0x1);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_RBO_DATA(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_RBO_DATA_Msk) >> SPIM_PHDMAR_RBO_DATA_Pos) == 0x2);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_RBO_DATA(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_RBO_DATA_Msk) >> SPIM_PHDMAR_RBO_DATA_Pos) == 0x3);

    /*
      *  SPIM_PHDMAR_DATA_BIT_MODE(x)
      */
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_DATA_BIT_MODE(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_DATA_Msk) >> SPIM_PHDMAR_BM_DATA_Pos) == 0x1);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_DATA_BIT_MODE(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_DATA_Msk) >> SPIM_PHDMAR_BM_DATA_Pos) == 0x2);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_DATA_BIT_MODE(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_DATA_Msk) >> SPIM_PHDMAR_BM_DATA_Pos) == 0x3);
    pSPIMModule->PHDMAR = 0;
    SPIM_PHDMAR_DATA_BIT_MODE(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMAR & SPIM_PHDMAR_BM_DATA_Msk) >> SPIM_PHDMAR_BM_DATA_Pos) == 0x4);

    SPIM_CLEAR_PHDMAR(pSPIMModule);
    CU_ASSERT(pSPIMModule->PHDMAR == 0x0);

    CU_PASS();
}

void MACRO_SPIM_DMM_PHASE()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());
    /*
     *  SPIM_PHDMM_CMD_WIDTH(x)
     */
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_CMD_WIDTH(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_DW_CMD_Msk) >> SPIM_PHDMM_DW_CMD_Pos) == 0x1);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_CMD_WIDTH(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_DW_CMD_Msk) >> SPIM_PHDMM_DW_CMD_Pos) == 0x2);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_CMD_WIDTH(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_DW_CMD_Msk) >> SPIM_PHDMM_DW_CMD_Pos) == 0x3);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_CMD_WIDTH(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_DW_CMD_Msk) >> SPIM_PHDMM_DW_CMD_Pos) == 0x4);

    /*
     *  SPIM_PHDMM_CMD_DTR_ENABLE(x)/ SPIM_PHDMM_CMD_DTR_DISABLE(x)
     */
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_CMD_DTR_ENABLE(pSPIMModule);
    CU_ASSERT_TRUE(((pSPIMModule->PHDMM & SPIM_PHDMM_DTR_CMD_Msk) >> SPIM_PHDMM_DTR_CMD_Pos));
    SPIM_PHDMM_CMD_DTR_DISABLE(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->PHDMM & SPIM_PHDMM_DTR_CMD_Msk) >> SPIM_PHDMM_DTR_CMD_Pos));

    /*
      *  SPIM_PHDMM_CMD_BIT_MODE(x)
      */
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_CMD_BIT_MODE(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_CMD_Msk) >> SPIM_PHDMM_BM_CMD_Pos) == 0x1);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_CMD_BIT_MODE(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_CMD_Msk) >> SPIM_PHDMM_BM_CMD_Pos) == 0x2);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_CMD_BIT_MODE(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_CMD_Msk) >> SPIM_PHDMM_BM_CMD_Pos) == 0x3);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_CMD_BIT_MODE(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_CMD_Msk) >> SPIM_PHDMM_BM_CMD_Pos) == 0x4);

    /*
     *  SPIM_PHDMM_ADDR_WIDTH(x)
     */
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_ADDR_WIDTH(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_DW_ADDR_Msk) >> SPIM_PHDMM_DW_ADDR_Pos) == 0x1);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_ADDR_WIDTH(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_DW_ADDR_Msk) >> SPIM_PHDMM_DW_ADDR_Pos) == 0x2);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_ADDR_WIDTH(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_DW_ADDR_Msk) >> SPIM_PHDMM_DW_ADDR_Pos) == 0x3);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_ADDR_WIDTH(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_DW_ADDR_Msk) >> SPIM_PHDMM_DW_ADDR_Pos) == 0x4);

    /*
     *  SPIM_PHDMM_ADDR_DTR_ENABLE(x)/ SPIM_PHDMM_ADDR_DTR_DISABLE(x)
     */
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_ADDR_DTR_ENABLE(pSPIMModule);
    CU_ASSERT_TRUE(((pSPIMModule->PHDMM & SPIM_PHDMM_DTR_ADDR_Msk) >> SPIM_PHDMM_DTR_ADDR_Pos));
    SPIM_PHDMM_ADDR_DTR_DISABLE(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->PHDMM & SPIM_PHDMM_DTR_ADDR_Msk) >> SPIM_PHDMM_DTR_ADDR_Pos));

    /*
      *  SPIM_PHDMM_ADDR_BIT_MODE(x)
      */
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_ADDR_BIT_MODE(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_ADDR_Msk) >> SPIM_PHDMM_BM_ADDR_Pos) == 0x1);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_ADDR_BIT_MODE(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_ADDR_Msk) >> SPIM_PHDMM_BM_ADDR_Pos) == 0x2);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_ADDR_BIT_MODE(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_ADDR_Msk) >> SPIM_PHDMM_BM_ADDR_Pos) == 0x3);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_ADDR_BIT_MODE(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_ADDR_Msk) >> SPIM_PHDMM_BM_ADDR_Pos) == 0x4);

    /*
      *  SPIM_PHDMM_READ_DATA_WIDTH(x)
      */
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_READ_DATA_WIDTH(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_DW_MODE_Msk) >> SPIM_PHDMM_DW_MODE_Pos) == 0x1);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_READ_DATA_WIDTH(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_DW_MODE_Msk) >> SPIM_PHDMM_DW_MODE_Pos) == 0x2);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_READ_DATA_WIDTH(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_DW_MODE_Msk) >> SPIM_PHDMM_DW_MODE_Pos) == 0x3);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_READ_DATA_WIDTH(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_DW_MODE_Msk) >> SPIM_PHDMM_DW_MODE_Pos) == 0x4);

    /*
     *  SPIM_PHDMM_READ_DTR_ENABLE(x)/ SPIM_PHDMM_READ_DTR_DISABLE(x)
     */
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_READ_DTR_ENABLE(pSPIMModule);
    CU_ASSERT_TRUE(((pSPIMModule->PHDMM & SPIM_PHDMM_DTR_MODE_Msk) >> SPIM_PHDMM_DTR_MODE_Pos));
    SPIM_PHDMM_READ_DTR_DISABLE(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->PHDMM & SPIM_PHDMM_DTR_MODE_Msk) >> SPIM_PHDMM_DTR_MODE_Pos));

    /*
      *  SPIM_PHDMM_READ_BIT_MODE(x)
      */
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_READ_BIT_MODE(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_MODE_Msk) >> SPIM_PHDMM_BM_MODE_Pos) == 0x1);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_READ_BIT_MODE(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_MODE_Msk) >> SPIM_PHDMM_BM_MODE_Pos) == 0x2);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_READ_BIT_MODE(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_MODE_Msk) >> SPIM_PHDMM_BM_MODE_Pos) == 0x3);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_READ_BIT_MODE(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_MODE_Msk) >> SPIM_PHDMM_BM_MODE_Pos) == 0x4);

    /*
     *  SPIM_PHDMM_DATA_DTR_ENABLE(x)/ SPIM_PHDMM_DATA_DTR_DISABLE(x)
     */
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_DATA_DTR_ENABLE(pSPIMModule);
    CU_ASSERT_TRUE(((pSPIMModule->PHDMM & SPIM_PHDMM_DTR_DATA_Msk) >> SPIM_PHDMM_DTR_DATA_Pos));
    SPIM_PHDMM_DATA_DTR_DISABLE(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->PHDMM & SPIM_PHDMM_DTR_DATA_Msk) >> SPIM_PHDMM_DTR_DATA_Pos));

    /*
     *  SPIM_PHDMM_DATA_RDQS_ENABLE(x)/ SPIM_PHDMM_DATA_RDQS_DISABLE(x)
     */
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_DATA_RDQS_ENABLE(pSPIMModule);
    CU_ASSERT_TRUE(((pSPIMModule->PHDMM & SPIM_PHDMM_RDQS_DATA_Msk) >> SPIM_PHDMM_RDQS_DATA_Pos));
    SPIM_PHDMM_DATA_RDQS_DISABLE(pSPIMModule);
    CU_ASSERT_FALSE(((pSPIMModule->PHDMM & SPIM_PHDMM_RDQS_DATA_Msk) >> SPIM_PHDMM_RDQS_DATA_Pos));

    /*
      *  SPIM_PHDMM_RBO_DATA(x)
      */
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_RBO_DATA(pSPIMModule, 0x0);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_RBO_DATA_Msk) >> SPIM_PHDMM_RBO_DATA_Pos) == 0x0);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_RBO_DATA(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_RBO_DATA_Msk) >> SPIM_PHDMM_RBO_DATA_Pos) == 0x1);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_RBO_DATA(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_RBO_DATA_Msk) >> SPIM_PHDMM_RBO_DATA_Pos) == 0x2);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_RBO_DATA(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_RBO_DATA_Msk) >> SPIM_PHDMM_RBO_DATA_Pos) == 0x3);

    /*
      *  SPIM_PHDMM_DATA_BIT_MODE(x)
      */
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_DATA_BIT_MODE(pSPIMModule, 0x1);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_DATA_Msk) >> SPIM_PHDMM_BM_DATA_Pos) == 0x1);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_DATA_BIT_MODE(pSPIMModule, 0x2);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_DATA_Msk) >> SPIM_PHDMM_BM_DATA_Pos) == 0x2);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_DATA_BIT_MODE(pSPIMModule, 0x3);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_DATA_Msk) >> SPIM_PHDMM_BM_DATA_Pos) == 0x3);
    pSPIMModule->PHDMM = 0;
    SPIM_PHDMM_DATA_BIT_MODE(pSPIMModule, 0x4);
    CU_ASSERT(((pSPIMModule->PHDMM & SPIM_PHDMM_BM_DATA_Msk) >> SPIM_PHDMM_BM_DATA_Pos) == 0x4);

    SPIM_CLEAR_PHDMM(pSPIMModule);
    CU_ASSERT(pSPIMModule->PHDMM == 0x0);

    CU_PASS();
}

void MACRO_SPIM_DLL0()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());
    /*
     *  SPIM_ENABLE_DLL0_OLDO(x)
     */
    pSPIMModule->DLL0 = 0;
    SPIM_ENABLE_DLL0_OLDO(pSPIMModule, 1);
    CU_ASSERT_TRUE(((pSPIMModule->DLL0 & SPIM_DLL0_DLLOLDO_Msk) >> SPIM_DLL0_DLLOLDO_Pos));
    pSPIMModule->DLL0 = 0;
    SPIM_ENABLE_DLL0_OLDO(pSPIMModule, 0);
    CU_ASSERT_FALSE(((pSPIMModule->DLL0 & SPIM_DLL0_DLLOLDO_Msk) >> SPIM_DLL0_DLLOLDO_Pos));

    /*
     *  SPIM_ENABLE_DLL0_OVRST(x)
     */
    pSPIMModule->DLL0 = 0;
    SPIM_ENABLE_DLL0_OVRST(pSPIMModule, 1);
    CU_ASSERT_FALSE(((pSPIMModule->DLL0 & SPIM_DLL0_DLLOVRST_Msk) >> SPIM_DLL0_DLLOVRST_Pos));
    pSPIMModule->DLL0 = 0;
    SPIM_ENABLE_DLL0_OVRST(pSPIMModule, 0);
    CU_ASSERT_FALSE(((pSPIMModule->DLL0 & SPIM_DLL0_DLLOVRST_Msk) >> SPIM_DLL0_DLLOVRST_Pos));

    /*
     *  SPIM_SET_DLL0_DELAY_NUM(x)
     */
    pSPIMModule->DLL0 = 0;
    SPIM_SET_DLL0_DELAY_NUM(pSPIMModule, 1);
    CU_ASSERT(((pSPIMModule->DLL0 & SPIM_DLL1_DLLOVNUM_Msk) >> SPIM_DLL0_DLL_DNUM_Pos) == 1);
    pSPIMModule->DLL0 = 0;
    SPIM_SET_DLL0_DELAY_NUM(pSPIMModule, 7);
    CU_ASSERT(((pSPIMModule->DLL0 & SPIM_DLL0_DLL_DNUM_Msk) >> SPIM_DLL0_DLL_DNUM_Pos) == 7);
    pSPIMModule->DLL0 = 0;
    SPIM_SET_DLL0_DELAY_NUM(pSPIMModule, 15);
    CU_ASSERT(((pSPIMModule->DLL0 & SPIM_DLL0_DLL_DNUM_Msk) >> SPIM_DLL0_DLL_DNUM_Pos) == 15);

    CU_PASS();
}

void MACRO_SPIM_DLL1()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());
    /*
    *  SPIM_SET_DLL1_OUT_VALID(x)
    */
    pSPIMModule->DLL1 = 0;
    SPIM_SET_DLL1_OUT_VALID(pSPIMModule, 0x55AA);
    CU_ASSERT(((pSPIMModule->DLL1 & SPIM_DLL1_DLLOVNUM_Msk) >> SPIM_DLL1_DLLOVNUM_Pos) == 0x55AA);
    pSPIMModule->DLL1 = 0;
    SPIM_SET_DLL1_OUT_VALID(pSPIMModule, 0xAA55);
    CU_ASSERT(((pSPIMModule->DLL1 & SPIM_DLL1_DLLOVNUM_Msk) >> SPIM_DLL1_DLLOVNUM_Pos) == 0xAA55);

    /*
    *  SPIM_SET_DLL1_LOCKK_VALID(x)
    */
    pSPIMModule->DLL1 = 0;
    SPIM_SET_DLL1_LOCKK_VALID(pSPIMModule, 0x55AA);
    CU_ASSERT(SPIM_GET_DLL1_LOCKK_VALID(pSPIMModule) == 0x55AA);
    pSPIMModule->DLL1 = 0;
    SPIM_SET_DLL1_LOCKK_VALID(pSPIMModule, 0xAA55);
    CU_ASSERT(SPIM_GET_DLL1_LOCKK_VALID(pSPIMModule) == 0xAA55);

    /*
    *  SPIM_SET_DLL1_LOCK_TIME(x)
    */
    pSPIMModule->DLL1 = 0;
    SPIM_SET_DLL1_LOCK_TIME(pSPIMModule, 0x55AA);
    CU_ASSERT(((pSPIMModule->DLL1 & SPIM_DLL1_DLLLKNUM_Msk) >> SPIM_DLL1_DLLLKNUM_Pos) == 0x55AA);
    pSPIMModule->DLL1 = 0;
    SPIM_SET_DLL1_LOCK_TIME(pSPIMModule, 0xAA55);
    CU_ASSERT(((pSPIMModule->DLL1 & SPIM_DLL1_DLLLKNUM_Msk) >> SPIM_DLL1_DLLLKNUM_Pos) == 0xAA55);

    CU_PASS();
}

void MACRO_SPIM_DLL2()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());
    /*
    *  SPIM_SET_DLL2_CLKON_NUM(x)
    */
    pSPIMModule->DLL2 = 0;
    SPIM_SET_DLL2_CLKON_NUM(pSPIMModule, 0x55AA);
    CU_ASSERT(((pSPIMModule->DLL2 & SPIM_DLL2_CLKONNUM_Msk) >> SPIM_DLL2_CLKONNUM_Pos) == 0x55AA);
    pSPIMModule->DLL2 = 0;
    SPIM_SET_DLL2_CLKON_NUM(pSPIMModule, 0xAA55);
    CU_ASSERT(((pSPIMModule->DLL2 & SPIM_DLL2_CLKONNUM_Msk) >> SPIM_DLL2_CLKONNUM_Pos) == 0xAA55);

    /*
    *  SPIM_SET_DLL2_TRIM_NUM(x)
    */
    pSPIMModule->DLL2 = 0;
    SPIM_SET_DLL2_TRIM_NUM(pSPIMModule, 0x55AA);
    CU_ASSERT(((pSPIMModule->DLL2 & SPIM_DLL2_TRIMNUM_Msk) >> SPIM_DLL2_TRIMNUM_Pos) == 0x55AA);
    pSPIMModule->DLL2 = 0;
    SPIM_SET_DLL2_TRIM_NUM(pSPIMModule, 0xAA55);
    CU_ASSERT(((pSPIMModule->DLL2 & SPIM_DLL2_TRIMNUM_Msk) >> SPIM_DLL2_TRIMNUM_Pos) == 0xAA55);

    CU_PASS();
}

void MACRO_SPIM_GetSClkFreq()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 0);

    CU_ASSERT(SPIM_GetSClkFreq(pSPIMModule) == SystemCoreClock);

    SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 1);

    CU_ASSERT(SPIM_GetSClkFreq(pSPIMModule) == SystemCoreClock / 2);

    SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 2);

    CU_ASSERT(SPIM_GetSClkFreq(pSPIMModule) == SystemCoreClock / 4);

    CU_PASS();
}

int dma_read_write(SPIM_T *spim, int is4ByteAddr, uint32_t u32RdCmd, uint32_t WrCmd, int dc_num)
{
    uint32_t i, offset;             /* variables */
    uint32_t *pData;
    uint8_t *u8TstBuf1 = (uint8_t *)GetTestBuffer1();
    uint8_t *u8TstBuf2 = (uint8_t *)GetTestBuffer2();

    SPIM_Enable_4Bytes_Mode(spim, is4ByteAddr, 1);

    if (is4ByteAddr)
    {
        CU_ASSERT(SPIM_Is4ByteModeEnable(spim, 1) == 1);
    }
    else
    {
        CU_ASSERT(SPIM_Is4ByteModeEnable(spim, 1) == 0);
    }

    /*
     *  Erase flash page
     */
    //SPIM_EraseBlock(spim, 0, is4ByteAddr, OPCODE_BE_64K, 1, 1);
    SPIM_EraseAddrRange(spim, 0, is4ByteAddr, FLASH_PAGE_SIZE, OPCODE_BE_64K, FLASH_PAGE_SIZE, 1);

    /*
     *  Verify flash page be erased
     */
    spim->PHDMAR = (spim->PHDMAR & 0xFFFFFFFF) | 0x00000301;
    printf("pSPIMx->PHDMAR = 0x%08X\r\n", spim->PHDMAR);
    // Set Dummy Cycle After Address.
    SPIM_SET_DC_DMAR(spim, 0);

    for (offset = 0; offset < FLASH_PAGE_SIZE; offset += BUFFER_SIZE)
    {
        memset(u8TstBuf1, 0, BUFFER_SIZE);

        SPIM_DMA_Read(spim, offset, is4ByteAddr, BUFFER_SIZE, u8TstBuf1, u32RdCmd, 1);

        pData = (uint32_t *)u8TstBuf1;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != 0xFFFFFFFF)
            {
                printf("FAILED!\n");
                printf("Erase DMA R/W - Flash address 0x%x, read 0x%x!\n", i, *pData);
                return -1;
            }
        }
    }

    /*
     *  Program data to flash block
     */
    spim->PHDMAW = (spim->PHDMAW & 0xFFFFFFFF) | 0x00000301;
    printf("pSPIMx->PHDMAW = 0x%08X\r\n", spim->PHDMAW);
    SPIM_SET_DC_DMAR(spim, 0);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        popDat(u8TstBuf1, BUFFER_SIZE);
        //pData = (uint32_t *)u8TstBuf1;

        //for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        //    (*pData) = (i << 16) | (offset + i);

        SPIM_DMA_Write(spim, offset, is4ByteAddr, BUFFER_SIZE, u8TstBuf1, WrCmd);
    }

    /*
     *  Verify flash block data
     */
    spim->PHDMAR = (spim->PHDMAR & 0xFFFFFFFF) | 0x00000301;
    printf("pSPIMx->PHDMAR = 0x%08X\r\n", spim->PHDMAR);
    // Set Dummy Cycle After Address.
    SPIM_SET_DC_DMAR(spim, 0);

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        popDat(u8TstBuf1, BUFFER_SIZE);

        memset(u8TstBuf2, 0, BUFFER_SIZE);
        SPIM_DMA_Read(spim, offset, is4ByteAddr, BUFFER_SIZE, u8TstBuf2, u32RdCmd, 1);

        //pData = (uint32_t *)u8TstBuf1;

        // Compare.
        if (memcmp(&u8TstBuf1, &u8TstBuf2, BUFFER_SIZE))
        {
            printf("FAILED!\n");
            dump_compare_error(offset, u8TstBuf1, u8TstBuf2, BUFFER_SIZE);
            CU_FAIL();
            return -1;
        }

        //for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        //{
        //    if (*pData != ((i << 16) | (offset + i)))
        //    {
        //        printf("FAILED!\n");
        //        printf("DMA R/W - Flash address 0x%x, read 0x%x, expect 0x%x!\n",
        //               i,
        //               *pData,
        //               (i << 16) | (offset + i));
        //        CU_FAIL();
        //    }
        //}

    }

    printf("done.\n");
    return 0;
}

void SPIM_Basic_Func()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    /* Unlock protected registers */
    //SYS_UnlockReg();

    //if (GetSPIMTestModuleIdx() == C_SPIM0)
    //{
    //    SYS_ResetModule(SYS_SPIM0RST);
    //}
    //else if (GetSPIMTestModuleIdx() == C_SPIM1)
    //{
    //    SYS_ResetModule(SYS_SPIM1RST);
    //}

    //SPIM_DISABLE_CIPHER(pSPIMModule);

    //SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 10);        /* Set SPIM clock as HCLK divided by 2 */
    //SPIM_SET_DCNUM(pSPIMModule, 8);                /* 8 is the default value. */

    if (SPIM_InitFlash(pSPIMModule, 1) != 0)        /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        CU_FAIL();
    }

    SPIM_ReadJedecId(pSPIMModule, idBuf, sizeof(idBuf), 1);
    printf("SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n", idBuf[0], idBuf[1], idBuf[2]);

    if ((idBuf[0] != MFGID_WINBOND) &&
            (idBuf[0] != MFGID_MXIC) &&
            (idBuf[0] != MFGID_EON) &&
            (idBuf[0] != MFGID_ISSI) &&
            (idBuf[0] != MFGID_SPANSION) &&
            (idBuf[0] != MFGID_MICRON))
    {
        CU_FAIL();
    }
    else
    {
        CU_PASS();
    }
}

void SPIM_DMA_Func()
{
    SPIM_T *pSPIMModule = NULL;

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    //SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 10);        /* Set SPIM clock as HCLK divided by 2 */

    //SPIM_SET_DCNUM(8);                /* 8 is the default value. */

    if (SPIM_InitFlash(pSPIMModule, 1) != 0)        /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        CU_FAIL();
    }

    SPIM_ReadJedecId(pSPIMModule, idBuf, sizeof(idBuf), 1);
    printf("SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n", idBuf[0], idBuf[1], idBuf[2]);

    if (dma_read_write(pSPIMModule, 0, CMD_NORMAL_READ, CMD_NORMAL_PAGE_PROGRAM, 8) < 0)
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
    SPIM_T *pSPIMModule = NULL;
    uint8_t *u8TstBuf1 = (uint8_t *)GetTestBuffer1();

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 10);       /* Set SPIM clock as HCLK divided by 2 */

    //SPIM_SET_DCNUM(pSPIMModule, 8);                /* 8 is the default value. */

    if (SPIM_InitFlash(pSPIMModule, 1) != 0)        /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        CU_FAIL();
    }

    if (dma_read_write(pSPIMModule, 0, CMD_FAST_READ, CMD_NORMAL_PAGE_PROGRAM, 8) < 0)
    {
        printf("dma_read_write FAILED!!\n");
        CU_FAIL();
    }

    printf("Doing Chip Erase...\n");
    SPIM_ChipErase(pSPIMModule, 1, 1);
    printf("Chip Erase done.\n");

    /*
     *  Verify flash page be erased
     */
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(u8TstBuf1, 0, BUFFER_SIZE);
        SPIM_DMA_Read(pSPIMModule, offset, 0, BUFFER_SIZE, u8TstBuf1, CMD_FAST_READ, 1);

        pData = (uint32_t *)u8TstBuf1;

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

    printf("DMA Test done.\n");
    CU_PASS();
}

void SPIM_IO_WriteFunc()
{
    uint32_t offset = 0;
    SPIM_T *pSPIMModule = NULL;
    uint8_t *u8TstBuf1 = (uint8_t *)GetTestBuffer1();

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    SPIM_SetQuadEnable(pSPIMModule, 1, 1);

    /*
     *  Program data to flash block
     */
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        popDat(u8TstBuf1, BUFFER_SIZE);
        //pData = (uint32_t *)g_buff;

        //for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        //    * pData = (i << 16) | (offset + i);

        SPIM_IO_Write(pSPIMModule,
                      offset,
                      0,
                      BUFFER_SIZE,
                      u8TstBuf1,
                      CMD_QUAD_PAGE_PROGRAM_WINBOND,
                      SPIM_GetIOPhaseSize(PHASE_NORMAL_MODE),
                      SPIM_GetIOPhaseSize(PHASE_NORMAL_MODE),
                      SPIM_GetIOPhaseSize(PHASE_QUAD_MODE));
        //SPIM_IO_Write(pSPIMModule, offset, 0, BUFFER_SIZE, g_buff, OPCODE_PP, 1, 1, 1);
    }

    SPIM_SetQuadEnable(pSPIMModule, 0, 1);
}

void SPIM_IO_RW_Func()
{
    int        i, offset;
    uint32_t   *pData;
    SPIM_T *pSPIMModule = NULL;
    uint8_t *u8TstBuf1 = (uint8_t *)GetTestBuffer1();
    uint8_t *u8TstBuf2 = (uint8_t *)GetTestBuffer2();

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    //SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 10);       /* Set SPIM clock as HCLK divided by 4 */

    if (SPIM_InitFlash(pSPIMModule, 1) != 0)        /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        CU_FAIL();
    }

    SPIM_Enable_4Bytes_Mode(pSPIMModule, 0, 1);

    CU_ASSERT(SPIM_Is4ByteModeEnable(pSPIMModule, 1) == 0);

    /*
     *  Erase flash page
     */
    //SPIM_EraseBlock(pSPIMModule, 0, 0, OPCODE_BE_64K, 1, 1);
    SPIM_EraseAddrRange(pSPIMModule, 0, 0, FLASH_PAGE_SIZE, OPCODE_BE_64K, FLASH_PAGE_SIZE, 1);

    SPIM_SetQuadEnable(pSPIMModule, 1, 1);

    /*
     *  Verify flash page be erased
     */
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        memset(u8TstBuf1, 0, BUFFER_SIZE);
        //SPIM_IO_Read(pSPIMModule, offset, 0, BUFFER_SIZE, u8TstBuf1, 0xeb, 1, 4, 4, 3, 0);
        SPIM_IO_CMDPhase(pSPIMModule, 0xEB, PHASE_NORMAL_MODE, 0, 0);
        SPIM_IO_AddrPhase(pSPIMModule, 0, offset, PHASE_QUAD_MODE, 0);
        SPIM_IO_DCPhase(pSPIMModule, 2, PHASE_QUAD_MODE, 0);
        SPIM_IO_DataPhase(pSPIMModule, u8TstBuf1, BUFFER_SIZE, PHASE_QUAD_MODE, 0, 0);

        pData = (uint32_t *)u8TstBuf1;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != 0xFFFFFFFF)
            {
                printf("FAILED!\n");
                printf("Erase I/O RW - Flash address 0x%x, read 0x%x!\n", i, *pData);
                CU_FAIL();
            }
        }
    }

    /*
     *  Program data to flash block
     */
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        popDat(u8TstBuf1, BUFFER_SIZE);
        //pData = (uint32_t *)u8TstBuf1;

        //for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        //    * pData = (i << 16) | (offset + i);

        SPIM_IO_Write(pSPIMModule,
                      offset,
                      0,
                      BUFFER_SIZE,
                      u8TstBuf1,
                      CMD_QUAD_PAGE_PROGRAM_WINBOND,
                      SPIM_GetIOPhaseSize(PHASE_NORMAL_MODE),
                      SPIM_GetIOPhaseSize(PHASE_NORMAL_MODE),
                      SPIM_GetIOPhaseSize(PHASE_QUAD_MODE));
        //SPIM_IO_Write(pSPIMModule, offset, 0, BUFFER_SIZE, g_buff, OPCODE_PP, 1, 1, 1);
    }

    /*
     *  Read and compare flash data
     */
    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        popDat(u8TstBuf1, BUFFER_SIZE);

        memset(u8TstBuf2, 0, BUFFER_SIZE);
        //SPIM_IO_Read(pSPIMModule, offset, 0, BUFFER_SIZE, u8TstBuf2, 0xeb, 1, 4, 4, 3, 0);
        SPIM_IO_CMDPhase(pSPIMModule, 0xEB, PHASE_NORMAL_MODE, 0, 0);
        SPIM_IO_AddrPhase(pSPIMModule, 0, offset, PHASE_QUAD_MODE, 0);
        SPIM_IO_DCPhase(pSPIMModule, 2, PHASE_QUAD_MODE, 0);
        SPIM_IO_DataPhase(pSPIMModule, u8TstBuf2, BUFFER_SIZE, PHASE_QUAD_MODE, 0, 0);

        // Compare.
        if (memcmp(&u8TstBuf1, &u8TstBuf2, BUFFER_SIZE))
        {
            printf("FAILED!\n");
            dump_compare_error(offset, u8TstBuf1, u8TstBuf2, BUFFER_SIZE);
            CU_FAIL();
        }

        //pData = (uint32_t *)g_buff;

        //for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        //{
        //    if (*pData != ((i << 16) | (offset + i)))
        //    {
        //        printf("FAILED!\n");
        //        printf("I/O RW - Flash address 0x%x, read 0x%x, expect 0x%x!\n", i, *pData, (i << 16) | (offset + i));
        //        CU_FAIL();
        //    }
        //}
    }

    SPIM_SetQuadEnable(pSPIMModule, 0, 1);

    printf("IO Test done.\n");
    CU_PASS();
}

void SPIM_DMM_Func()
{
    int        i, offset;
    uint32_t   *pData;
    SPIM_T *pSPIMModule = NULL;
    uint8_t *u8TstBuf1 = (uint8_t *)GetTestBuffer1();
    uint8_t *u8TstBuf2 = (uint8_t *)GetTestBuffer2();

    pSPIMModule = (SPIM_T *)GetSPIMModule(GetSPIMTestModuleIdx());

    //SPIM_SET_CLOCK_DIVIDER(pSPIMModule, 10);        /* Set SPIM clock as HCLK divided by 2 */

    if (SPIM_InitFlash(pSPIMModule, 1) != 0)        /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        CU_FAIL();
    }

    //if (dma_read_write(pSPIMModule, 0, CMD_FAST_READ, CMD_NORMAL_PAGE_PROGRAM, 8) < 0)
    //{
    //    printf("dma_read_write FAILED!!\n");
    //    CU_FAIL();
    //}
    SPIM_IO_WriteFunc();

    //SPIM_EnterDirectMapMode(pSPIMModule, 0, CMD_FAST_READ, 8);
    pSPIMModule->PHDMM = (pSPIMModule->PHDMM & 0xFFFFFFFF) | 0x20212301;
    printf("pSPIMx->PHDMM = 0x%08X\r\n", pSPIMModule->PHDMM);
    // Set Dummy Cycle After Address.
    SPIM_SET_DC_DMM(pSPIMModule, 4);

    SPIM_SetQuadEnable(pSPIMModule, 1, 1);

    SPIM_EnterDirectMapMode(pSPIMModule, 0, 0xEB, 0);  // Switch to Direct Map mode.cmd_idx].u32CMD, idlIntvl);     // Switch to Direct Map mode.

    for (offset = 0; offset < FLASH_BLOCK_SIZE; offset += BUFFER_SIZE)
    {
        if (GetSPIMTestModuleIdx() == C_SPIM0)
        {
            memcpy(u8TstBuf1, (uint8_t *)(SPIM0_DMM_MAP_ADDR + offset), BUFFER_SIZE);
        }
        else if (GetSPIMTestModuleIdx() == C_SPIM1)
        {
            memcpy(u8TstBuf1, (uint8_t *)(SPIM1_DMM_MAP_ADDR + offset), BUFFER_SIZE);
        }

        popDat(u8TstBuf1, BUFFER_SIZE);

        pData = (uint32_t *)u8TstBuf1;

        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (inpw(SPIM0_DMM_MAP_ADDR + offset + i) != *pData)
            {
                printf("FAILED!\n");
                printf("DMM - Flash address 0x%x, read 0x%x, expect 0x%x!\n",
                       i,
                       *pData,
                       (i << 16) | (offset + i));
                SPIM_ExitDirectMapMode(pSPIMModule);
                CU_FAIL();
            }

            //if (*pData != ((i << 16) | (offset + i)))
            //{
            //    printf("FAILED!\n");
            //    printf("DMM - Flash address 0x%x, read 0x%x, expect 0x%x!\n", i, *pData, (i << 16) | (offset + i));
            //    SPIM_ExitDirectMapMode(pSPIMModule);
            //    CU_FAIL();
            //}
        }
    }

    SPIM_ExitDirectMapMode(pSPIMModule);

    SPIM_SetQuadEnable(pSPIMModule, 0, 1);

    printf("DMM Test done.\n");
    CU_PASS();
}

CU_TestInfo SPIM_ConstantTests[] =
{
    {"SPIM CONST Address", SPIM_Const_Address},
    {"SPIM CONST SPIM_CTL0", SPIM_Const_SPIM_CTL0},
    {"SPIM CONST SPIM_CMDCODE", SPIM_Const_SPIM_CMDCODE},
    {"SPIM CONST SPIM_PHASE", SPIM_Const_SPIM_PHASE},
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
    {"SPIM_PHDMAW Macros", MACRO_SPIM_DMA_WRITE_PHASE},
    {"SPIM_PHDMAR Macros", MACRO_SPIM_DMA_READ_PHASE},
    {"SPIM_PHDMM Macros", MACRO_SPIM_DMM_PHASE},
    {"SPIM_DLL0 Macros", MACRO_SPIM_DLL0},
    {"SPIM_DLL1 Macros", MACRO_SPIM_DLL1},
    {"SPIM_DLL2 Macros", MACRO_SPIM_DLL2},
    {"SPIM_GetSClkFreq Macros", MACRO_SPIM_GetSClkFreq},
    CU_TEST_INFO_NULL,
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