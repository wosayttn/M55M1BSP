/**************************************************************************//**
 * @file    spim.h
 * @version V1.00
 * @brief   SPIM driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#ifndef __SPIM_H__
#define __SPIM_H__

#include <stdint.h>

/*----------------------------------------------------------------------------*/
/* Include related headers                                                    */
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup SPIM_Driver SPIM Driver
  @{
*/

/** @addtogroup SPIM_EXPORTED_CONSTANTS SPIM Exported Constants
  @{
*/

#define SPIM_CACHE_EN                   (0)                   /*!< SPIM cache on/off    \hideinitializer */

#define SPIM0_DMM_MAP_SADDR             (0x80000000UL)        /*!< SPIM0 DMM mode memory map base secure address    \hideinitializer */
#define SPIM0_DMM_MAP_NSADDR            (0x90000000UL)        /*!< SPIM1 DMM mode memory map base non secure address    \hideinitializer */

#define SPIM1_DMM_MAP_SADDR             (0x82000000UL)        /*!< SPIM1 DMM mode memory map base secure address    \hideinitializer */
#define SPIM1_DMM_MAP_NSADDR            (0x92000000UL)        /*!< SPIM1 DMM mode memory map base non secure address    \hideinitializer */

#if defined (SCU_INIT_D0PNS2_VAL) && (SCU_INIT_D0PNS2_VAL & BIT2)
#define SPIM0_DMM_MAP_ADDR              SPIM0_DMM_MAP_NSADDR
#else
#define SPIM0_DMM_MAP_ADDR              SPIM0_DMM_MAP_SADDR
#endif //

#if defined (SCU_INIT_D0PNS2_VAL) && (SCU_INIT_D0PNS2_VAL & BIT3)
#define SPIM1_DMM_MAP_ADDR              SPIM1_DMM_MAP_NSADDR
#else
#define SPIM1_DMM_MAP_ADDR              SPIM1_DMM_MAP_SADDR
#endif //

#define SPIM_DMM_SIZE                   (0x2000000UL)     /*!< DMM mode memory mapping size        \hideinitializer */

#define SPIM_MAX_DLL_LATENCY            (0x05)            /*!< Maximum DLL training number        \hideinitializer */

#define SPIM_DTR_ON                     (0x01)  /* Double data rate mode enable */
#define SPIM_DTR_OFF                    (0x00)  /* Double data rate mode disable */

#define SPIM_OP_ENABLE                  (0x01UL)
#define SPIM_OP_DISABLE                 (0x00UL)

/*----------------------------------------------------------------------------*/
/* SPIM_CTL0 constant definitions                                             */
/*----------------------------------------------------------------------------*/
#define SPIM_CTL0_RW_IN(x)              ((x) ? 0UL : (0x1UL << SPIM_CTL0_QDIODIR_Pos))      /*!< SPIM_CTL0: SPI Interface Direction Select \hideinitializer */

#define SPIM_CTL0_BITMODE_SING          (0UL << SPIM_CTL0_BITMODE_Pos)  /*!< SPIM_CTL0: One bit mode (SPI Interface including DO, DI, HOLD, WP) \hideinitializer */
#define SPIM_CTL0_BITMODE_DUAL          (1UL << SPIM_CTL0_BITMODE_Pos)  /*!< SPIM_CTL0: Two bits mode (SPI Interface including D0, D1, HOLD, WP) \hideinitializer */
#define SPIM_CTL0_BITMODE_QUAD          (2UL << SPIM_CTL0_BITMODE_Pos)  /*!< SPIM_CTL0: Four bits mode (SPI Interface including D0, D1, D2, D3) \hideinitializer */
#define SPIM_CTL0_BITMODE_OCTAL         (3UL << SPIM_CTL0_BITMODE_Pos)  /*!< SPIM_CTL0: Four bits mode (SPI Interface including D0, D1, D2, D3, D4, D5, D6, D7) \hideinitializer */

#define SPIM_CTL0_OPMODE_IO             (0UL << SPIM_CTL0_OPMODE_Pos)   /*!< SPIM_CTL0: I/O Mode \hideinitializer */
#define SPIM_CTL0_OPMODE_PAGEWRITE      (1UL << SPIM_CTL0_OPMODE_Pos)   /*!< SPIM_CTL0: Page Write Mode \hideinitializer */
#define SPIM_CTL0_OPMODE_PAGEREAD       (2UL << SPIM_CTL0_OPMODE_Pos)   /*!< SPIM_CTL0: Page Read Mode \hideinitializer */
#define SPIM_CTL0_OPMODE_DIRECTMAP      (3UL << SPIM_CTL0_OPMODE_Pos)   /*!< SPIM_CTL0: Direct Map Mode \hideinitializer */

#define SPIM_CTL0_RBO_MODE0             (0UL << SPIM_CTL0_RBO_NORM_Pos) /*!< SPIM_CTL0: Read Byte Order Mode (Data format Byte0, Byte1, Byte2, Byte3) \hideinitializer */
#define SPIM_CTL0_RBO_MODE1             (1UL << SPIM_CTL0_RBO_NORM_Pos) /*!< SPIM_CTL0: Read Byte Order Mode (Data format Byte3, Byte2, Byte1, Byte0) \hideinitializer */
#define SPIM_CTL0_RBO_MODE2             (2UL << SPIM_CTL0_RBO_NORM_Pos) /*!< SPIM_CTL0: Read Byte Order Mode (Data format Byte1, Byte0, Byte3, Byte2) \hideinitializer */
#define SPIM_CTL0_RBO_MODE3             (3UL << SPIM_CTL0_RBO_NORM_Pos) /*!< SPIM_CTL0: Read Byte Order Mode (Data format Byte2, Byte3, Byte0, Byte1) \hideinitializer */

//------------------------------------------------------------------------------
// SPI Flash Write Command
//------------------------------------------------------------------------------
#define CMD_NORMAL_PAGE_PROGRAM             (0x02UL)    /*!< SPIM_CMDCODE: Page Program (Page Write Mode Use) \hideinitializer */
#define CMD_NORMAL_PAGE_PROGRAM_4B          (0x12UL)    /*!< SPIM_CMDCODE: Page Program 4 Byte Address (Page Write Mode Use) \hideinitializer */
#define CMD_QUAD_PAGE_PROGRAM_WINBOND       (0x32UL)    /*!< SPIM_CMDCODE: Quad Page program (for Winbond) (Page Write Mode Use) \hideinitializer */
#define CMD_QUAD_PAGE_PROGRAM_WINBOND_4B    (0x34UL)    /*!< SPIM_CMDCODE: Quad Page program 4 Byte Address (for Winbond) (Page Write Mode Use) \hideinitializer */
#define CMD_QUAD_PAGE_PROGRAM_MXIC          (0x38UL)    /*!< SPIM_CMDCODE: Quad Page program (for MXIC) (Page Write Mode Use) \hideinitializer */
#define CMD_QUAD_PAGE_PROGRAM_EON           (0x40UL)    /*!< SPIM_CMDCODE: Quad Page Program (for EON) (Page Write Mode Use) \hideinitializer */

#define CMD_OCTAL_PAGE_PROG_MICRON          (0x82UL)    /*!< SPIM_CMDCODE: Octal Page Program (Page Write Mode Use) \hideinitializer */
#define CMD_OCTAL_PAGE_PROG_MICRON_4B       (0x84UL)    /*!< SPIM_CMDCODE: Octal Page Program 4 Byte Address (Page Write Mode Use) \hideinitializer */
#define CMD_OCTAL_EX_PAGE_PROG_MICRON       (0xC2UL)    /*!< SPIM_CMDCODE: Octal Page Extern Program (Page Write Mode Use) \hideinitializer */
#define CMD_OCTAL_EX_PAGE_PROG_MICRON_4B    (0x8EUL)    /*!< SPIM_CMDCODE: Octal Page Extern Program 4 Byte Address (Page Write Mode Use) \hideinitializer */

//------------------------------------------------------------------------------
// SPI Flash Read Command
//------------------------------------------------------------------------------
#define CMD_DMA_NORMAL_READ                 (0x03UL)    /*!< SPIM_CMDCODE: Read Data (Page Read Mode Use) \hideinitializer */
#define CMD_DMA_NORMAL_READ_4B              (0x13UL)    /*!< SPIM_CMDCODE: Read Data 4 Byte Address( Page Read Mode Use) \hideinitializer */
#define CMD_DMA_FAST_READ                   (0x0BUL)    /*!< SPIM_CMDCODE: Fast Read (Page Read Mode Use) \hideinitializer */
#define CMD_DMA_FAST_READ_4B                (0x0CUL)    /*!< SPIM_CMDCODE: Fast Read 4 Byte Address (Page Read Mode Use) \hideinitializer */
#define CMD_DMA_NORMAL_DTR_READ             (0x0DUL)    /*!< SPIM_CMDCODE: Fast DTR Read (Page Read Mode Use) \hideinitializer */

//------------------------------------------------------------------------------
// Dual Read Command
//------------------------------------------------------------------------------
#define CMD_DMA_NORMAL_DUAL_READ            (0x3BUL)    /*!< SPIM_CMDCODE: Fast Read Dual Output (Page Read Mode Use) \hideinitializer */
#define CMD_DMA_NORMAL_DUAL_READ_4B         (0x3CUL)    /*!< SPIM_CMDCODE: Fast Read Dual Output 4 Byte Address (Page Read Mode Use) \hideinitializer */
#define CMD_DMA_FAST_DUAL_READ              (0xBBUL)    /*!< SPIM_CMDCODE: Fast Read Dual I/O (Page Read Mode Use) \hideinitializer */
#define CMD_DMA_FAST_DUAL_READ_4B           (0xBCUL)    /*!< SPIM_CMDCODE: Fast Read Dual I/O 4 Byte Address (Page Read Mode Use) \hideinitializer */
#define CMD_DMA_FAST_DUAL_DTR_READ          (0xBDUL)    /*!< SPIM_CMDCODE: DTR Fast Read Dual I/O (Page Read Mode Use) \hideinitializer */

//------------------------------------------------------------------------------
// Quad Read Command
//------------------------------------------------------------------------------
#define CMD_DMA_FAST_READ_QUAD_OUTPUT       (0x6BUL)    /*!< SPIM_CMDCODE: Fast Read Dual Output (Page Read Mode Use) \hideinitializer */
#define CMD_DMA_FAST_READ_QUAD_OUTPUT_4B    (0x6CUL)    /*!< SPIM_CMDCODE: Fast Read Dual Output 4 Byte Address (Page Read Mode Use) \hideinitializer */
#define CMD_DMA_FAST_QUAD_READ              (0xEBUL)    /*!< SPIM_CMDCODE: Fast Read Quad I/O (Page Read Mode Use) \hideinitializer */
#define CMD_DMA_FAST_QUAD_READ_4B           (0xECUL)    /*!< SPIM_CMDCODE: Fast Read Quad I/O 4 Byte Address (Page Read Mode Use) \hideinitializer */
#define CMD_DMA_FAST_QUAD_DTR_READ          (0xEDUL)    /*!< SPIM_CMDCODE: DTR Fast Read Quad I/O (Page Read Mode Use) \hideinitializer */

//------------------------------------------------------------------------------
// Micron Octal Read Command
//------------------------------------------------------------------------------
#define CMD_OCTAL_FAST_READ_OUTPUT          (0x8BUL)    /*!< SPIM_CMDCODE: Fast Read Octal Output (Page Read Mode Use) \hideinitializer */
#define CMD_OCTAL_FAST_READ_OUTPUT_4B       (0x7CUL)    /*!< SPIM_CMDCODE: Fast Read Octal Output 4 Byte Address (Page Read Mode Use) \hideinitializer */
#define CMD_OCTAL_FAST_IO_READ              (0xCBUL)    /*!< SPIM_CMDCODE: Fast Read Octal I/O (Page Read Mode Use) \hideinitializer */
#define CMD_OCTAL_FAST_IO_READ_4B           (0xCCUL)    /*!< SPIM_CMDCODE: Fast Read Octal I/O 4 Byte Address (Page Read Mode Use) \hideinitializer */
#define CMD_OCTAL_DDR_FAST_READ_OUTPUT      (0x9DUL)    /*!< SPIM_CMDCODE: DTR Fast Read Octal Output (Page Read Mode Use) \hideinitializer */
#define CMD_OCTAL_DDR_FAST_IO_READ          (0xFDUL)    /*!< SPIM_CMDCODE: DTR Fast Read Octal I/O (Page Read Mode Use) \hideinitializer */

//------------------------------------------------------------------------------
// Continue Read Mode Command
//------------------------------------------------------------------------------
#define CMD_CLEAR_MODE_DATA                 (0x00UL)    /*!< SPIM_MODE: Set output data for normal read mode \hideinitializer */
#define CMD_CONTINUE_READ_MODE              (0x20UL)    /*!< SPIM_MODE: Set output data for continue read mode. \hideinitializer */

//------------------------------------------------------------------------------
// Wrap Mode
//------------------------------------------------------------------------------
#define CMD_WRAP_8BIT                       (0x00)
#define CMD_WRAP_16BIT                      (0x20)
#define CMD_WRAP_32BIT                      (0x40)
#define CMD_WRAP_64BIT                      (0x60)

/*----------------------------------------------------------------------------*/
/* SPIM_PHDMAW, SPIM_PHDMAR, SPIM_PHDMM constant definitions                  */
/*----------------------------------------------------------------------------*/
#define PHASE_NORMAL_MODE                   (0x0)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Normal Mode for Command/Address/Data Phase. \hideinitializer */
#define PHASE_DUAL_MODE                     (0x1)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Dual Mode for Command/Address/Data Phase. \hideinitializer */
#define PHASE_QUAD_MODE                     (0x2)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Quad Mode for Command/Address/Data Phase. \hideinitializer */
#define PHASE_OCTAL_MODE                    (0x3)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Octal Mode for Command/Address/Data Phase. \hideinitializer */

#define PHASE_ORDER_MODE0                   (0x0)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Byte order of program data to SPI Flash is byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7. \hideinitializer */
#define PHASE_ORDER_MODE1                   (0x1)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Byte order of program data to SPI Flash is byte7, byte6, byte5, byte4, byte3, byte2, byte1, byte0. \hideinitializer */
#define PHASE_ORDER_MODE2                   (0x2)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Byte order of program data to SPI Flash is byte1, byte0, byte3, byte2, byte5, byte4, byte7, byte6. \hideinitializer */
#define PHASE_ORDER_MODE3                   (0x3)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Byte order of program data to SPI Flash is byte6, byte7, byte4, byte5, byte2, byte3, byte0, byte1. \hideinitializer */

#define PHASE_WIDTH_8                       (0x1)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set 8 bits are transmitted in this phase. \hideinitializer */
#define PHASE_WIDTH_16                      (0x2)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set 16 bits are transmitted in this phase. \hideinitializer */
#define PHASE_WIDTH_24                      (0x3)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set 24 bits are transmitted in this phase. \hideinitializer */
#define PHASE_WIDTH_32                      (0x4)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set 32 bits are transmitted in this phase. \hideinitializer */

/* SPIM_PHDMAR/SPIM_PHDMM read mode opcodes. */
#define PHASE_READMODE_ON                   (0x01)  /* Continue read mode enable */
#define PHASE_READMODE_OFF                  (0x00)  /* Continue read mode disable */

/* SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM DTR mode opcodes. */
#define PHASE_DTR_ON                        (0x01)  /* Double data rate mode enable */
#define PHASE_DTR_OFF                       (0x00)  /* Double data rate mode disable */

/* SPIM Operation mode. */
#define SPIM_OP_FLASH_MODE                  (0x0)   /* SPIROM operates in SPI Flash mode.  */
#define SPIM_OP_HYPER_MODE                  (0x1)   /* SPIROM operates in HYPER Device mode. */

#define SPIM_IO_WRITE_PHASE                 (0x01)  /* SPIM IO write phase mode.  */
#define SPIM_IO_READ_PHASE                  (0x00)  /* SPIM IO read phase mode.  */

/** @cond HIDDEN_SYMBOLS */

typedef enum
{
    MFGID_UNKNOW    = 0x00U,
    MFGID_SPANSION  = 0x01U,
    MFGID_EON       = 0x1CU,
    MFGID_ISSI      = 0x7FU,
    MFGID_MXIC      = 0xC2U,
    MFGID_WINBOND   = 0xEFU,
    MFGID_MICRON    = 0x2CU,
} E_MFGID;

/* Flash opcodes. */
#define OPCODE_WREN             (0x06U) /* Write enable */
#define OPCODE_RDSR             (0x05U) /* Read status register #1*/
#define OPCODE_WRSR             (0x01U) /* Write status register #1 */
#define OPCODE_RDSR2            (0x35U) /* Read status register #2*/
#define OPCODE_WRSR2            (0x31U) /* Write status register #2 */
#define OPCODE_RDSR3            (0x15U) /* Read status register #3*/
#define OPCODE_WRSR3            (0x11U) /* Write status register #3 */
#define OPCODE_PP               (0x02U) /* Page program (up to 256 bytes) */
#define OPCODE_SE_4K            (0x20U) /* Erase 4KB sector */
#define OPCODE_BE_32K           (0x52U) /* Erase 32KB block */
#define OPCODE_CHIP_ERASE       (0xc7U) /* Erase whole flash chip */
#define OPCODE_BE_64K           (0xd8U) /* Erase 64KB block */
#define OPCODE_READ_ID          (0x90U) /* Read ID */
#define OPCODE_RDID             (0x9fU) /* Read JEDEC ID */
#define OPCODE_BRRD             (0x16U) /* SPANSION flash - Bank Register Read command  */
#define OPCODE_BRWR             (0x17U) /* SPANSION flash - Bank Register write command */
#define OPCODE_NORM_READ        (0x03U) /* Read data bytes */
#define OPCODE_FAST_READ        (0x0bU) /* Read data bytes */
#define OPCODE_FAST_DUAL_READ   (0x3bU) /* Read data bytes */
#define OPCODE_FAST_QUAD_READ   (0x6bU) /* Read data bytes */

#define OPCODE_WR_NVCONFIG      (0xB1U) /* Write Non-Volatile Configuration Register*/
#define OPCODE_WR_VCONFIG       (0x81U) /* Write Volatile Configuration Register*/
#define OPCODE_RD_NVCONFIG      (0xB5U) /* Read Non-Volatile Configuration Register*/
#define OPCODE_RD_VCONFIG       (0x85U) /* Read Volatile Configuration Register*/

/* Used for SST flashes only. */
#define OPCODE_BP               (0x02U) /* Byte program */
#define OPCODE_WRDI             (0x04U) /* Write disable */
#define OPCODE_AAI_WP           (0xadU) /* Auto u32Address increment word program */

#define OPCODE_EN4B             (0xb7U) /* Enter 4-byte mode */
#define OPCODE_EX4B             (0xe9U) /* Exit 4-byte mode */

#define OPCODE_RDSCUR           (0x2bU)
#define OPCODE_WRSCUR           (0x2fU)

#define OPCODE_RSTEN            (0x66U)
#define OPCODE_RST              (0x99U)

#define OPCODE_ENQPI            (0x38U)
#define OPCODE_EXQPI            (0xFFU)

/* Used for Micron flashes. */
#define OPCODE_FAST_OCTAL_OUTPUT_READ   (0x8BU) /* Read data bytes */
#define OPCODE_FAST_OCTAL_IO_READ       (0xCBU) /* Read data bytes */

/* Status Register bits. */
#define SR_WIP                  (0x1U)  /* Write in progress */
#define SR_WEL                  (0x2U)  /* Write enable latch */
#define SR_QE                   (0x40U) /* Quad Enable for MXIC */

/* Status Register #2 bits. */
#define SR2_QE                  (0x2U)  /* Quad Enable for Winbond */

/* meaning of other SR_* bits may differ between vendors */
#define SR_BP0                  (0x4U)  /* Block protect 0 */
#define SR_BP1                  (0x8U)  /* Block protect 1 */
#define SR_BP2                  (0x10U) /* Block protect 2 */
#define SR_SRWD                 (0x80U) /* SR write protect */
#define SR3_ADR                 (0x01U) /* 4-byte u32Address mode */

#define SCUR_4BYTE              (0x04U) /* 4-byte u32Address mode */

/* SPIM Wait State Timeout Counter. */
#define SPIM_TIMEOUT            SystemCoreClock /*!< SPIM time-out counter (1 second time-out) */

/** @endcond HIDDEN_SYMBOLS */

/* SPIM Define Error Code */
#define SPIM_OK                 ( 0L)   /*!< SPIM operation OK */
#define SPIM_ERR_FAIL           (-1L)   /*!< SPIM operation failed */
#define SPIM_ERR_TIMEOUT        (-2L)   /*!< SPIM operation abort due to timeout error */

/*----------------------------------------------------------------------------*/
/*  Define Macros and functions                                               */
/*----------------------------------------------------------------------------*/
/**
 * @details    Enable cipher.
 * \hideinitializer
 */
#define SPIM_ENABLE_CIPHER(spim)                                         \
    do                                                                   \
    {                                                                    \
        (spim->CTL0 &= ~SPIM_CTL0_CIPHOFF_Msk);                          \
        spim->DMMCTL &= ~SPIM_DMMCTL_DESELTIM_Msk;                       \
        spim->DMMCTL |= (((0x12) & 0x1FUL) << SPIM_DMMCTL_DESELTIM_Pos); \
        spim->CTL0 |= SPIM_CTL0_BALEN_Msk;                               \
    } while (0)

/**
 * @details    Disable cipher.
 * \hideinitializer
 */
#define SPIM_DISABLE_CIPHER(spim)                                       \
    do                                                                  \
    {                                                                   \
        (spim->CTL0 |= SPIM_CTL0_CIPHOFF_Msk);                          \
        spim->DMMCTL &= ~SPIM_DMMCTL_DESELTIM_Msk;                      \
        spim->DMMCTL |= (((0x8) & 0x1FUL) << SPIM_DMMCTL_DESELTIM_Pos); \
        spim->CTL0 &= ~SPIM_CTL0_BALEN_Msk;                             \
    } while (0)

/**
 * @details    Enable cipher balance
 * \hideinitializer
 */
#define SPIM_ENABLE_BALEN(spim)              \
    do                                       \
    {                                        \
        (spim->CTL0 |= SPIM_CTL0_BALEN_Msk); \
    } while (0)

/**
 * @details    Disable cipher balance
 * \hideinitializer
 */
#define SPIM_DISABLE_BALEN(spim)              \
    do                                        \
    {                                         \
        (spim->CTL0 &= ~SPIM_CTL0_BALEN_Msk); \
    } while (0)

/**
 * @details     Enable Hyper Device Mode.
 * @param   spim
 * @param   x       SPIM operation Mode Bit
 *                  - \ref SPIM_OP_FLASH_MODE : SPI Flash Mode
 *                  - \ref SPIM_OP_HYPER_MODE : Hyper Device Mode
 * \hideinitializer
 */
#define SPIM_SET_OP_MODE(spim, x)                    \
    do                                               \
    {                                                \
        spim->CTL0 &= ~SPIM_CTL0_HYPER_EN_Msk;       \
        spim->CTL0 |= (x << SPIM_CTL0_HYPER_EN_Pos); \
    } while (0)

/**
 * @details     Set 4-byte address.
 * @param   x   Enable/Disable 4 bytes address.
 *              - \ref SPIM_OP_ENABLE  : Enable
 *              - \ref SPIM_OP_DISABLE : Disable
 * \hideinitializer
 */
#define SPIM_SET_4BYTE_ADDR_EN(spim, x)                              \
    do                                                               \
    {                                                                \
        spim->CTL0 &= ~(SPIM_CTL0_B4ADDREN_Msk);                     \
        spim->CTL0 |= (((x) ? 1UL : 0UL) << SPIM_CTL0_B4ADDREN_Pos); \
    } while (0)

/**
 * @details    Get 4-byte address to be enabled/disabled
 * \hideinitializer
 */
#define SPIM_GET_4BYTE_ADDR_EN(spim)    \
    ((spim->CTL0 & SPIM_CTL0_B4ADDREN_Msk) >> SPIM_CTL0_B4ADDREN_Pos)

/**
 * @details    Enable SPIM interrupt
 * \hideinitializer
 */
#define SPIM_ENABLE_INT(spim)       (spim->CTL0 |= SPIM_CTL0_IEN_Msk)

/**
 * @details    Disable SPIM interrupt
 * \hideinitializer
 */
#define SPIM_DISABLE_INT(spim)      (spim->CTL0 &= ~SPIM_CTL0_IEN_Msk)

/**
 * @details    Get SPIM interrupt to be enabled/disabled
 * \hideinitializer
 */
#define SPIM_GET_INT(spim)  \
    ((spim->CTL0 & SPIM_CTL0_IEN_Msk) >> SPIM_CTL0_IEN_Pos)

/**
 * @details    Is interrupt flag on.
 * \hideinitializer
 */
#define SPIM_IS_IF_ON(spim)   \
    ((spim->CTL0 & SPIM_CTL0_IF_Msk) >> SPIM_CTL0_IF_Pos)

/**
 * @details    Clear interrupt flag.
 * \hideinitializer
 */
#define SPIM_CLR_INT(spim)                       \
    do                                           \
    {                                            \
        spim->CTL0 |= (1UL << SPIM_CTL0_IF_Pos); \
    } while (0)

/**
 * @details Set transmit/receive bit length
 * @param   spim
 * @param   x   Transmit/Receive Bit Length
 *              - \ref 8:  8 bits
 *              - \ref 16: 16 bits
 *              - \ref 24: 24 bits
 *              - \ref 32: 32 bits
 * \hideinitializer
 */
#define SPIM_SET_DATA_WIDTH(spim, x)                      \
    do                                                    \
    {                                                     \
        spim->CTL0 &= ~(SPIM_CTL0_DWIDTH_Msk);            \
        spim->CTL0 |= (((x)-1U) << SPIM_CTL0_DWIDTH_Pos); \
    } while (0)

/**
 * @details    Get data transmit/receive bit length setting
 * \hideinitializer
 */
#define SPIM_GET_DATA_WIDTH(spim)   \
    (((spim->CTL0 & SPIM_CTL0_DWIDTH_Msk) >> SPIM_CTL0_DWIDTH_Pos) + 1U)

/**
 * @details    Set data transmit/receive burst number
 * @param   spim
 * @param   x       Transmit/Receive Burst Number
 *                  - \ref 0: one data TX/RX will be executed in one transfer
 *                  - \ref 1: two data TX/RX will be executed in one transfer
 *                  - \ref 2: three data TX/RX will be executed in one transfer
 *                  - \ref 3: four data TX/RX will be executed in one transfer
 * \hideinitializer
 */
#define SPIM_SET_BURST_DATA(spim, x)                         \
    do                                                       \
    {                                                        \
        spim->CTL0 &= ~(SPIM_CTL0_BURSTNUM_Msk);             \
        spim->CTL0 |= ((x - 1UL) << SPIM_CTL0_BURSTNUM_Pos); \
    } while (0)

/**
 * @details    Get data transmit/receive burst number
 * \hideinitializer
 */
#define SPIM_GET_BURST_DATA(spim)   \
    ((spim->CTL0 & SPIM_CTL0_BURSTNUM_Msk) >> SPIM_CTL0_BURSTNUM_Pos)

/**
 * @details    Set suspend interval.
 * @param   x  Suspend Interval
 *             - \ref 0 ~ 0xF
 * \hideinitializer
 */
#define SPIM_SET_SUSP_INTVL(spim, x)                  \
    do                                                \
    {                                                 \
        spim->CTL0 &= ~(SPIM_CTL0_SUSPITV_Msk);       \
        spim->CTL0 |= ((x) << SPIM_CTL0_SUSPITV_Pos); \
    } while (0)

/**
 * @details    Get suspend interval setting
 * \hideinitializer
 */
#define SPIM_GET_SUSP_INTVL(spim)   \
    ((spim->CTL0 & SPIM_CTL0_SUSPITV_Msk) >> SPIM_CTL0_SUSPITV_Pos)

/**
 * @details    Enable Single Input mode.
 * \hideinitializer
 */
#define SPIM_ENABLE_SING_INPUT_MODE(spim)                               \
    do                                                                  \
    {                                                                   \
        spim->CTL0 &= ~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk); \
        spim->CTL0 |= (SPIM_CTL0_BITMODE_SING | SPIM_CTL0_RW_IN(1));    \
    } while (0)

/**
 * @details    Enable Single Output mode.
 * \hideinitializer
 */
#define SPIM_ENABLE_SING_OUTPUT_MODE(spim)                              \
    do                                                                  \
    {                                                                   \
        spim->CTL0 &= ~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk); \
        spim->CTL0 |= (SPIM_CTL0_BITMODE_SING | SPIM_CTL0_RW_IN(0));    \
    } while (0)

/**
 * @details    Enable Dual Input mode.
 * \hideinitializer
 */
#define SPIM_ENABLE_DUAL_INPUT_MODE(spim)                               \
    do                                                                  \
    {                                                                   \
        spim->CTL0 &= ~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk); \
        spim->CTL0 |= (SPIM_CTL0_BITMODE_DUAL | SPIM_CTL0_RW_IN(1U));   \
    } while (0)

/**
 * @details    Enable Dual Output mode.
 * \hideinitializer
 */
#define SPIM_ENABLE_DUAL_OUTPUT_MODE(spim)                              \
    do                                                                  \
    {                                                                   \
        spim->CTL0 &= ~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk); \
        spim->CTL0 |= (SPIM_CTL0_BITMODE_DUAL | SPIM_CTL0_RW_IN(0U));   \
    } while (0)

/**
 * @details    Enable Quad Input mode.
 * \hideinitializer
 */
#define SPIM_ENABLE_QUAD_INPUT_MODE(spim)                               \
    do                                                                  \
    {                                                                   \
        spim->CTL0 &= ~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk); \
        spim->CTL0 |= (SPIM_CTL0_BITMODE_QUAD | SPIM_CTL0_RW_IN(1U));   \
    } while (0)

/**
 * @details    Enable Quad Output mode.
 * \hideinitializer
 */
#define SPIM_ENABLE_QUAD_OUTPUT_MODE(spim)                              \
    do                                                                  \
    {                                                                   \
        spim->CTL0 &= ~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk); \
        spim->CTL0 |= (SPIM_CTL0_BITMODE_QUAD | SPIM_CTL0_RW_IN(0U));   \
    } while (0)

/**
 * @details    Enable Octal Input mode.
 * \hideinitializer
 */
#define SPIM_ENABLE_OCTAL_INPUT_MODE(spim)                              \
    do                                                                  \
    {                                                                   \
        spim->CTL0 &= ~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk); \
        spim->CTL0 |= (SPIM_CTL0_BITMODE_OCTAL | SPIM_CTL0_RW_IN(1U));  \
    } while (0)

/**
 * @details    Enable Octal Output mode.
 * \hideinitializer
 */
#define SPIM_ENABLE_OCTAL_OUTPUT_MODE(spim)                             \
    do                                                                  \
    {                                                                   \
        spim->CTL0 &= ~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk); \
        spim->CTL0 |= (SPIM_CTL0_BITMODE_OCTAL | SPIM_CTL0_RW_IN(0U));  \
    } while (0)

/**
 * @details     Set operation mode.
 * @param   x   SPI Function Operation Mode
 *              - \ref SPIM_CTL0_OPMODE_IO          : Normal I/O mode
 *              - \ref SPIM_CTL0_OPMODE_PAGEWRITE   : DMA Write Mode
 *              - \ref SPIM_CTL0_OPMODE_PAGEREAD    : DMA Read Mode
 *              - \ref SPIM_CTL0_OPMODE_DIRECTMAP   : Direct Memory Mapping mode
 * \hideinitializer
 */
#define SPIM_SET_OPMODE(spim, x)               \
    do                                         \
    {                                          \
        spim->CTL0 &= ~(SPIM_CTL0_OPMODE_Msk); \
        spim->CTL0 |= (x);                     \
    } while (0)

/**
 * @details    Get operation mode.
 * \hideinitializer
 */
#define SPIM_GET_OPMODE(spim)   \
    ((spim->CTL0 & SPIM_CTL0_OPMODE_Msk) >> SPIM_CTL0_OPMODE_Pos)

/**
 * @details     Set DTR(Data Transfer Rate) mode.
 * @param   x   Double Transfer Rate Mode Enable Bit for Normal I/O Mode
 *              - \ref 0 : Disable
 *              - \ref 1 : Enable
 * \hideinitializer
 */
#define SPIM_SET_DTR_MODE(spim, x)                   \
    do                                               \
    {                                                \
        spim->CTL0 &= (~SPIM_CTL0_DTR_NORM_Msk);     \
        spim->CTL0 |= (x << SPIM_CTL0_DTR_NORM_Pos); \
    } while (0)

/**
 * @details     Get DTR(Data Transfer Rate) mode to be enabled/disabled.
 * \hideinitializer
 */
#define SPIM_GET_DTR_MODE(spim) \
    ((spim->CTL0 & SPIM_CTL0_DTR_NORM_Msk) >> SPIM_CTL0_DTR_NORM_Pos)

/**
 * @details     Set Read DQS Mode.
 * @param   x   Read DQS Mode Enable Bit for Normal I/O Mode
 *              - \ref 0 : Disable
 *              - \ref 1 : Enable
 * \hideinitializer
 */
#define SPIM_SET_RDQS_MODE(spim, x)                   \
    do                                                \
    {                                                 \
        spim->CTL0 &= (~SPIM_CTL0_RDQS_NORM_Msk);     \
        spim->CTL0 |= (x << SPIM_CTL0_RDQS_NORM_Pos); \
    } while (0)

/**
 * @details     Get Read DQS Mode to be enabled/disabled.
 * \hideinitializer
 */
#define SPIM_GET_RDQS_MODE(spim)    \
    ((spim->CTL0 & SPIM_CTL0_RDQS_NORM_Msk) >> SPIM_CTL0_RDQS_NORM_Pos)

/**
 * @details     Set Read Data Byte Order Mode.
 * @param   x   Received Data Byte Order of Normal I/O Mode for Octal SPI Flash
 *              width 8bits
 *              - \ref 0x0 : received data from SPI Flash is byte0.
 *              width 16bits
 *              - \ref 0x0 : received data from SPI Flash is byte0, byte1.
 *              - \ref 0x1 : received data from SPI Flash is byte1, byte0.
 *              width 24bits
 *              - \ref 0x0 : received data from SPI Flash is byte0, byte1, byte2.
 *              - \ref 0x1 : received data from SPI Flash is byte2, byte1, byte0.
 *              width 32bits
 *              - \ref 0x0 : received data from SPI Flash is byte0, byte1, byte2, byte3.
 *              - \ref 0x1 : received data from SPI Flash is byte3, byte2, byte1, byte0.
 *              - \ref 0x2 : received data from SPI Flash is byte1, byte0, byte3, byte2.
 *              - \ref 0x3 : received data from SPI Flash is byte2, byte3, byte0, byte1.
 * \hideinitializer
 */
#define SPIM_SET_RBO_MODE(spim, x)                   \
    do                                               \
    {                                                \
        spim->CTL0 &= (~SPIM_CTL0_RBO_NORM_Msk);     \
        spim->CTL0 |= (x << SPIM_CTL0_RBO_NORM_Pos); \
    } while (0)

/**
 * @details   Get Read Data Byte Order Mode.
 * \hideinitializer
 */
#define SPIM_GET_RBO_MODE(spim) \
    ((spim->CTL0 & SPIM_CTL0_RBO_NORM_Msk) >> SPIM_CTL0_RBO_NORM_Pos)

/**
 * @details     Set SPI flash commmand code.
 * @param   x   reference SPI Flash Specification.
 * \hideinitializer
 */
#define SPIM_SET_SPIM_MODE(spim, x)                                          \
    do                                                                       \
    {                                                                        \
        spim->CMDCODE = (spim->CMDCODE & (~SPIM_CMDCODE_CMDCODE_Msk)) | (x); \
    } while (0)

/**
 * @details    Get SPI flash command code.
 * \hideinitializer
 */
#define SPIM_GET_SPIM_MODE(spim)    \
    ((spim->CMDCODE & SPIM_CMDCODE_CMDCODE_Msk) >> SPIM_CMDCODE_CMDCODE_Pos)

/**
 * @details    Start operation.
 * \hideinitializer
 */
#define SPIM_SET_GO(spim)                     \
    do                                        \
    {                                         \
        (spim->CTL1 |= SPIM_CTL1_SPIMEN_Msk); \
    } while (0)

/**
 * @details    Is engine busy.
 * \hideinitializer
 */
#define SPIM_IS_BUSY(spim)  \
    ((spim->CTL1 & SPIM_CTL1_SPIMEN_Msk) >> SPIM_CTL1_SPIMEN_Pos)

/**
 * @details    Wait for free.
 * \hideinitializer
 */
#define SPIM_WAIT_FREE(spim)    \
    ((spim->CTL1 & SPIM_CTL1_SPIMEN_Msk) >> SPIM_CTL1_SPIMEN_Pos)

#if (SPIM_CACHE_EN == 1)
/**
 * @details    Enable cache.
 * \hideinitializer
 */
#define SPIM_ENABLE_CACHE(spim)                  \
    do                                           \
    {                                            \
        (spim->CTL1 &= ~SPIM_CTL1_CACHEOFF_Msk); \
    } while (0)

/**
 * @details    Disable cache.
 * \hideinitializer
 */
#define SPIM_DISABLE_CACHE(spim)                \
    do                                          \
    {                                           \
        (spim->CTL1 |= SPIM_CTL1_CACHEOFF_Msk); \
    } while (0)

/**
 * @details    Is cache enabled.
 * \hideinitializer
 */
//#define SPIM_IS_CACHE_EN(spim)    ((spim->CTL1 & SPIM_CTL1_CACHEOFF_Msk) ? 0 : 1)

/**
 * @details    Invalidate cache.
 * \hideinitializer
 */
#define SPIM_INVALID_CACHE(spim)               \
    do                                         \
    {                                          \
        (spim->CTL1 |= SPIM_CTL1_CDINVAL_Msk); \
    } while (0)

#endif //SPIM_CACHE_EN

/**
 * @details    Set SS(Select Active) to active level.
 * \hideinitializer
 */
#define SPIM_SET_SS_EN(spim, x)                                 \
    do                                                          \
    {                                                           \
        spim->CTL1 &= ~(SPIM_CTL1_SS_Msk);                      \
        spim->CTL1 |= ((!(x) ? 1UL : 0UL) << SPIM_CTL1_SS_Pos); \
    } while (0)

/**
 * @details    Is SS(Select Active) in active level.
 * \hideinitializer
 */
#define SPIM_GET_SS_EN(spim)    (!(spim->CTL1 & SPIM_CTL1_SS_Msk))

/**
 * @details    Set active level of slave select to be high/low.
 * \hideinitializer
 */
#define SPIM_SET_SS_ACTLVL(spim, x)                                    \
    do                                                                 \
    {                                                                  \
        spim->CTL1 &= ~(SPIM_CTL1_SSACTPOL_Msk);                       \
        spim->CTL1 |= ((!!(x) ? 1UL : 0UL) << SPIM_CTL1_SSACTPOL_Pos); \
    } while (0)

/**
 * @details     Set idle time interval
 * @param   x   Idle Time Interval
 *              - \ref 0 ~ 0xFFFF
 * \hideinitializer
 */
#define SPIM_SET_IDL_INTVL(spim, x)                    \
    do                                                 \
    {                                                  \
        spim->CTL1 &= ~(SPIM_CTL1_IDLETIME_Msk);       \
        spim->CTL1 |= ((x) << SPIM_CTL1_IDLETIME_Pos); \
    } while (0)

/**
 * @details    Get idle time interval setting
 * \hideinitializer
 */
#define SPIM_GET_IDL_INTVL(spim)    \
    ((spim->CTL1 & SPIM_CTL1_IDLETIME_Msk) >> SPIM_CTL1_IDLETIME_Pos)

/**
 * @details     Set SPIM clock divider
 * @parem   x   Clock Divider Register
 *              SPI Flash For DTR commands
 *              - \ref only 1,2,4,8,16,32,….
 *              Hyper Device Mode
 *              - \ref only 1 or 2
 * \hideinitializer
 */
#define SPIM_SET_CLOCK_DIVIDER(spim, x)               \
    do                                                \
    {                                                 \
        spim->CTL1 &= ~(SPIM_CTL1_DIVIDER_Msk);       \
        spim->CTL1 |= ((x) << SPIM_CTL1_DIVIDER_Pos); \
    } while (0)

/**
 * @details    Get SPIM current clock divider setting
 * \hideinitializer
 */
#define SPIM_GET_CLOCK_DIVIDER(spim)    \
    ((spim->CTL1 & SPIM_CTL1_DIVIDER_Msk) >> SPIM_CTL1_DIVIDER_Pos)

/**
 * @details    Set sampling clock delay selection for received data
 * \hideinitializer
 */
#define SPIM_SET_RXCLKDLY_RDDLYSEL(spim, x)                     \
    do                                                          \
    {                                                           \
        spim->RXCLKDLY &= (~SPIM_RXCLKDLY_RDDLYSEL_Msk);        \
        spim->RXCLKDLY |= ((x) << SPIM_RXCLKDLY_RDDLYSEL_Pos);  \
    } while (0)

/**
 * @details    Get sampling clock delay selection for received data
 * \hideinitializer
 */
#define SPIM_GET_RXCLKDLY_RDDLYSEL(spim)    \
    ((spim->RXCLKDLY & SPIM_RXCLKDLY_RDDLYSEL_Msk) >> SPIM_RXCLKDLY_RDDLYSEL_Pos)

/**
 * @details    Set DMA/DMM mode SPI flash active SCLK time
 * \hideinitializer
 */
#define SPIM_SET_DMM_ACTSCLKT(spim, x)                            \
    do                                                             \
    {                                                              \
        spim->DMMCTL &= ~SPIM_DMMCTL_ACTSCLKT_Msk;                 \
        spim->DMMCTL |= (((x)&0xFUL) << SPIM_DMMCTL_ACTSCLKT_Pos); \
        spim->DMMCTL |= SPIM_DMMCTL_UACTSCLK_Msk;                  \
    } while (0)

/**
 * @details    Reset SPI flash active SCLK time
 * \hideinitializer
 */
#define SPIM_RESET_DMM_ACTSCLK(spim)                   \
    do                                                 \
    {                                                  \
        (spim->DMMCTL &= ~(SPIM_DMMCTL_UACTSCLK_Msk)); \
    } while (0)

/**
 * @details    Set DMM mode SPI flash deselect time
 * \hideinitializer
 */
#define SPIM_SET_DMM_DESELTIM(spim, x)                              \
    do                                                              \
    {                                                               \
        spim->DMMCTL &= ~SPIM_DMMCTL_DESELTIM_Msk;                  \
        spim->DMMCTL |= (((x)&0x1FUL) << SPIM_DMMCTL_DESELTIM_Pos); \
    } while (0)

/**
 * @details    Get current DMM mode SPI flash deselect time setting
 * \hideinitializer
 */
#define SPIM_GET_DMM_DESELTIM(spim) \
    ((spim->DMMCTL & SPIM_DMMCTL_DESELTIM_Msk) >> SPIM_DMMCTL_DESELTIM_Pos)

/**
 * @details    Enable DMM mode burst wrap mode
 * \hideinitializer
 */
#define SPIM_ENABLE_DMM_BWEN(spim)              \
    do                                          \
    {                                           \
        (spim->DMMCTL |= SPIM_DMMCTL_BWEN_Msk); \
    } while (0)

/**
 * @details    Disable DMM mode burst wrap mode
 * \hideinitializer
 */
#define SPIM_DISABLE_DMM_BWEN(spim)                \
    do                                             \
    {                                              \
        (spim->DMMCTL &= ~(SPIM_DMMCTL_BWEN_Msk)); \
    } while (0)

/**
 * @details    Enable DMM mode continuous read mode
 * \hideinitializer
 */
#define SPIM_ENABLE_DMM_CREN(spim)              \
    do                                          \
    {                                           \
        (spim->DMMCTL |= SPIM_DMMCTL_CREN_Msk); \
    } while (0)

/**
 * @details    Disable DMM mode continuous read mode
 * \hideinitializer
 */
#define SPIM_DISABLE_DMM_CREN(spim)                \
    do                                             \
    {                                              \
        (spim->DMMCTL &= ~(SPIM_DMMCTL_CREN_Msk)); \
    } while (0)

/**
 * @details    Stop DMM mode Transfer
 * \hideinitializer
 */
#define SPIM_ENABLE_DMM_HYPDONE(spim)            \
    do                                           \
    {                                            \
        spim->DMMCTL |= SPIM_DMMCTL_HYPDONE_Msk; \
    } while (0)

/**
 * @details    Wait DMM mode complete to stop TX/RX
 * \hideinitializer
 */
#define SPIM_WAIT_DMM_HYPDONE(spim) \
    ((spim->DMMCTL & SPIM_DMMCTL_HYPDONE_Msk) >> SPIM_DMMCTL_HYPDONE_Pos)

/**
 * @details    Set dummy cycle number (Only DMA Command Mode)
 * \hideinitializer
 */
#define SPIM_SET_DMAR_DC(spim, x)                     \
    do                                                \
    {                                                 \
        spim->CTL2 &= ~(SPIM_CTL2_DC_DMAR_Msk);       \
        spim->CTL2 |= ((x) << SPIM_CTL2_DC_DMAR_Pos); \
    } while (0)

/**
 * @details    Clear dummy cycle number (Only DMA Command Mode)
 * \hideinitializer
 */
#define SPIM_CLEAR_DMAR_DC(spim)                \
    do                                          \
    {                                           \
        spim->CTL2 &= ~(SPIM_CTL2_DC_DMAR_Msk); \
    } while (0)

/**
 * @details    Set dummy cycle number (Only DMM Command Mode)
 * \hideinitializer
 */
#define SPIM_SET_DMM_DC(spim, x)                              \
    do                                                        \
    {                                                         \
        spim->CTL2 &= ~(SPIM_CTL2_DC_DMM_Msk);                \
        spim->CTL2 |= (((x)&0xFFUL) << SPIM_CTL2_DC_DMM_Pos); \
    } while (0)

/**
 * @details    Clear dummy cycle number (Only DMA Command Mode)
 * \hideinitializer
 */
#define SPIM_CLEAR_DMM_DC(spim)                \
    do                                         \
    {                                          \
        spim->CTL2 &= ~(SPIM_CTL2_DC_DMM_Msk); \
    } while (0)

/**
 * @details     Set output data for mode phase
 * @param x     SPI Flash Continue Read Mode
 *              - \ref CMD_CLEAR_MODE_DATA     : Disable continue read.
 *              - \ref CMD_CONTINUE_READ_MODE  : Enable continue read.
 * \hideinitializer
 */
#define SPIM_SET_MODE_DATA(spim, x)                                    \
    do                                                                 \
    {                                                                  \
        spim->MODE = ((spim->MODE & (~SPIM_MODE_MODEDATA_Msk)) | (x)); \
    } while (0)

/**
 * @brief Get Read Data Mode
 *
 */
#define SPIM_GET_MODE_DATA(spim) \
    ((spim->MODE & SPIM_MODE_MODEDATA_Msk) >> SPIM_MODE_MODEDATA_Pos)

/**
 * @details    Clear output data for mode phase
 * \hideinitializer
 */
#define SPIM_CLEAR_MODE_DATA(spim)               \
    do                                           \
    {                                            \
        spim->MODE &= ~(SPIM_MODE_MODEDATA_Msk); \
    } while (0)

/*----------------------------------------------------------------------------*/
/* SPIM_PHDMAW constant definitions                                           */
/*----------------------------------------------------------------------------*/
/**
 * @details    Clear Phase DMA Write Setting
 * \hideinitializer
 */
#define SPIM_CLEAR_PHDMAW(spim)          \
    do                                   \
    {                                    \
        (spim->PHDMAW &= ~(0xFFFFFFFF)); \
    } while (0)

/**
 * @details Set Write Data Width for Command Phase
 * @param x Command Phase Witdh
 *          - \ref PHASE_WIDTH_8  : 8 bits
 *          - \ref PHASE_WIDTH_16 : 16 bits
 *          - \ref PHASE_WIDTH_24 : 24 bits
 *          - \ref PHASE_WIDTH_32 : 32 bits
 * \hideinitializer
 */
#define SPIM_SET_PHDMAW_CMD_WIDTH(spim, x)             \
    do                                                 \
    {                                                  \
        spim->PHDMAW &= ~(SPIM_PHDMAW_DW_CMD_Msk);     \
        spim->PHDMAW |= (x << SPIM_PHDMAW_DW_CMD_Pos); \
    } while (0)

/**
 * @details     Get Write Data Width for Command Phase
 * \hideinitializer
 */
#define SPIM_GET_PHDMAW_CMD_WIDTH(spim) \
    ((spim->PHDMAW & SPIM_PHDMAW_DW_CMD_Msk) >> SPIM_PHDMAW_DW_CMD_Pos)

/**
 * @details    Set DTR Mode Enable Bit for Command Phase
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAW_CMD_DTR(spim)           \
    do                                             \
    {                                              \
        (spim->PHDMAW |= SPIM_PHDMAW_DTR_CMD_Msk); \
    } while (0)

/**
 * @details    Set DTR Mode Disable Bit for Command Phase
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAW_CMD_DTR(spim)             \
    do                                                \
    {                                                 \
        (spim->PHDMAW &= ~(SPIM_PHDMAW_DTR_CMD_Msk)); \
    } while (0)

/**
 * @details    Get DTR Mode Disable Bit for Command Phase
 * \hideinitializer
 */
#define SPIM_GET_PHDMAW_CMD_DTR(spim)   \
    ((spim->PHDMAW & SPIM_PHDMAW_DTR_CMD_Msk) >> SPIM_PHDMAW_DTR_CMD_Pos)

/**
 * @details Set Bit Mode for Command Phase
 * @param x Command Phase Bits Mode
 *          - \ref PHASE_NORMAL_MODE : Standard Mode
 *          - \ref PHASE_DUAL_MODE : Dual Mode
 *          - \ref PHASE_QUAD_MODE : Quad Mode
 *          - \ref PHASE_OCTAL_MODE : Octal Mode
 * \hideinitializer
 */
#define SPIM_SET_PHDMAW_CMD_BIT_MODE(spim, x)          \
    do                                                 \
    {                                                  \
        spim->PHDMAW &= ~(SPIM_PHDMAW_BM_CMD_Msk);     \
        spim->PHDMAW |= (x << SPIM_PHDMAW_BM_CMD_Pos); \
    } while (0)

/**
 * @details Set Data Width for Address Phase.
 * @param x Address Phase Witdh
 *          - \ref PHASE_WIDTH_8  : 8 bits
 *          - \ref PHASE_WIDTH_16 : 16 bits
 *          - \ref PHASE_WIDTH_24 : 24 bits
 *          - \ref PHASE_WIDTH_32 : 32 bits
 * \hideinitializer
 */
#define SPIM_SET_PHDMAW_ADDR_WIDTH(spim, x)             \
    do                                                  \
    {                                                   \
        spim->PHDMAW &= ~(SPIM_PHDMAW_DW_ADDR_Msk);     \
        spim->PHDMAW |= (x << SPIM_PHDMAW_DW_ADDR_Pos); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Enable Bit for Address Phase
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAW_ADDR_DTR(spim)             \
    do                                                \
    {                                                 \
        (spim->PHDMAW |= (SPIM_PHDMAW_DTR_ADDR_Msk)); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Disable Bit for Address Phase
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAW_ADDR_DTR(spim)             \
    do                                                 \
    {                                                  \
        (spim->PHDMAW &= ~(SPIM_PHDMAW_DTR_ADDR_Msk)); \
    } while (0)

/**
 * @details Set SPI Interface Bit Mode for Address Phase.
 * @param x Address Phase Bits Mode
 *          - \ref PHASE_NORMAL_MODE : Standard Mode
 *          - \ref PHASE_DUAL_MODE   : Dual Mode
 *          - \ref PHASE_QUAD_MODE   : Quad Mode
 *          - \ref PHASE_OCTAL_MODE  : Octal Mode
 * \hideinitializer
 */
#define SPIM_SET_PHDMAW_ADDR_BIT_MODE(spim, x)          \
    do                                                  \
    {                                                   \
        spim->PHDMAW &= ~(SPIM_PHDMAW_BM_ADDR_Msk);     \
        spim->PHDMAW |= (x << SPIM_PHDMAW_BM_ADDR_Pos); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Enable Bit for Data Phase
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAW_DATA_DTR(spim)             \
    do                                                \
    {                                                 \
        (spim->PHDMAW |= (SPIM_PHDMAW_DTR_DATA_Msk)); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Disable Bit for Data Phase
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAW_DATA_DTR(spim)             \
    do                                                 \
    {                                                  \
        (spim->PHDMAW &= ~(SPIM_PHDMAW_DTR_DATA_Msk)); \
    } while (0)

/**
 * @details Set Program Data Byte Order of Program Data Phase for Octal SPI Flash,
 * @param x Data Byte Order
 *          - \ref PHASE_ORDER_MODE0 : Byte order byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7.
 *          - \ref PHASE_ORDER_MODE1 : Byte order byte7, byte6, byte5, byte4, byte3, byte2, byte1, byte0.
 *          - \ref PHASE_ORDER_MODE2 : Byte order byte1, byte0, byte3, byte2, byte5, byte4, byte7, byte6.
 *          - \ref PHASE_ORDER_MODE3 : Byte order byte6, byte7, byte4, byte5, byte2, byte3, byte0, byte1.
 * \hideinitializer
 */
#define SPIM_SET_PHDMAW_PBO_DATA(spim, x)                \
    do                                                   \
    {                                                    \
        spim->PHDMAW &= ~(SPIM_PHDMAW_PBO_DATA_Msk);     \
        spim->PHDMAW |= (x << SPIM_PHDMAW_PBO_DATA_Pos); \
    } while (0)

/**
 * @details Set SPI Interface Bit Mode for Data Phase.
 * @param x Data Phase Witdh
 *          - \ref PHASE_WIDTH_8  : 8 bits
 *          - \ref PHASE_WIDTH_16 : 16 bits
 *          - \ref PHASE_WIDTH_24 : 24 bits
 *          - \ref PHASE_WIDTH_32 : 32 bits
 * \hideinitializer
 */
#define SPIM_SET_PHDMAW_DATA_BIT_MODE(spim, x)          \
    do                                                  \
    {                                                   \
        spim->PHDMAW &= ~(SPIM_PHDMAW_BM_DATA_Msk);     \
        spim->PHDMAW |= (x << SPIM_PHDMAW_BM_DATA_Pos); \
    } while (0)

/*----------------------------------------------------------------------------*/
/* SPIM_PHDMAR constant definitions                                           */
/*----------------------------------------------------------------------------*/
#define SPIM_CLEAR_PHDMAR(spim)          \
    do                                   \
    {                                    \
        (spim->PHDMAR &= ~(0xFFFFFFFF)); \
    } while (0)

/**
 * @details Set Read Data Width for Command Phase.
 * @param x Command Phase Witdh
 *          - \ref PHASE_WIDTH_8  : 8 bits
 *          - \ref PHASE_WIDTH_16 : 16 bits
 *          - \ref PHASE_WIDTH_24 : 24 bits
 *          - \ref PHASE_WIDTH_32 : 32 bits
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_CMD_WIDTH(spim, x)             \
    do                                                 \
    {                                                  \
        spim->PHDMAR &= ~(SPIM_PHDMAR_DW_CMD_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_DW_CMD_Pos); \
    } while (0)

/**
 * @details    Get Read Data Width for Command Phase.
 * \hideinitializer
 */
#define SPIM_GET_PHDMAR_CMD_WIDTH(spim) \
    ((spim->PHDMAR & SPIM_PHDMAR_DW_CMD_Msk) >> SPIM_PHDMAR_DW_CMD_Pos)

/**
 * @details    Enable Double Transfer Rate Mode for Command Phase
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAR_CMD_DTR(spim)             \
    do                                               \
    {                                                \
        (spim->PHDMAR |= (SPIM_PHDMAR_DTR_CMD_Msk)); \
    } while (0)

/**
 * @details    Disable Double Transfer Rate Mode Bit for Command Phase
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAR_CMD_DTR(spim)             \
    do                                                \
    {                                                 \
        (spim->PHDMAR &= ~(SPIM_PHDMAR_DTR_CMD_Msk)); \
    } while (0)

/**
 * @details     Get Double Transfer Rate Mode Disable Bit for Command Phase
 * \hideinitializer
 */
#define SPIM_GET_PHDMAR_CMD_DTR(spim)   \
    ((spim->PHDMAR & SPIM_PHDMAR_DTR_CMD_Msk) >> SPIM_PHDMAR_DTR_CMD_Pos)

/**
 * @details Set SPI Interface Bit Mode for Command Phase.
 * @param x Command Phase Bits Mode
 *          - \ref PHASE_NORMAL_MODE : Standard Mode
 *          - \ref PHASE_DUAL_MODE   : Dual Mode
 *          - \ref PHASE_QUAD_MODE   : Quad Mode
 *          - \ref PHASE_OCTAL_MODE  : Octal Mode
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_CMD_BIT_MODE(spim, x)          \
    do                                                 \
    {                                                  \
        spim->PHDMAR &= ~(SPIM_PHDMAR_BM_CMD_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_BM_CMD_Pos); \
    } while (0)

/**
 * @details    Get SPI Interface Bit Mode for Command Phase.
 * \hideinitializer
 */
#define SPIM_GET_PHDMAR_CMD_BIT_MODE(spim)  \
    ((spim->PHDMAR & SPIM_PHDMAR_BM_CMD_Msk) >> SPIM_PHDMAR_BM_CMD_Pos)

/**
 * @details Set Data Width for Address Phase.
 * @param x Addres Phase Witdh
 *          - \ref PHASE_WIDTH_8  : 8 bits
 *          - \ref PHASE_WIDTH_16 : 16 bits
 *          - \ref PHASE_WIDTH_24 : 24 bits
 *          - \ref PHASE_WIDTH_32 : 32 bits
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_ADDR_WIDTH(spim, x)             \
    do                                                  \
    {                                                   \
        spim->PHDMAR &= ~(SPIM_PHDMAR_DW_ADDR_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_DW_ADDR_Pos); \
    } while (0)

/**
 * @details    Enable Double Transfer Rate Mode for Address Phase
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAR_ADDR_DTR(spim)             \
    do                                                \
    {                                                 \
        (spim->PHDMAR |= (SPIM_PHDMAR_DTR_ADDR_Msk)); \
    } while (0)

/**
 * @details    Disable Double Transfer Rate Mode for Address Phase
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAR_ADDR_DTR(spim)             \
    do                                                 \
    {                                                  \
        (spim->PHDMAR &= ~(SPIM_PHDMAR_DTR_ADDR_Msk)); \
    } while (0)

/**
 * @details Set SPI Interface Bit Mode for Address Phase.
 * @param x Addess Phase Bits Mode
 *          - \ref PHASE_NORMAL_MODE : Standard Mode
 *          - \ref PHASE_DUAL_MODE   : Dual Mode
 *          - \ref PHASE_QUAD_MODE   : Quad Mode
 *          - \ref PHASE_OCTAL_MODE  : Octal Mode
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_ADDR_BIT_MODE(spim, x)          \
    do                                                  \
    {                                                   \
        spim->PHDMAR &= ~(SPIM_PHDMAR_BM_ADDR_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_BM_ADDR_Pos); \
    } while (0)

/**
 * @details Set Data Width for Mode Phase.
 * @param x Read Mode Phase Witdh
 *          - \ref PHASE_WIDTH_8  : 8 bits
 *          - \ref PHASE_WIDTH_16 : 16 bits
 *          - \ref PHASE_WIDTH_24 : 24 bits
 *          - \ref PHASE_WIDTH_32 : 32 bits
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_READ_DATA_WIDTH(spim, x)        \
    do                                                  \
    {                                                   \
        spim->PHDMAR &= ~(SPIM_PHDMAR_DW_MODE_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_DW_MODE_Pos); \
    } while (0)

/**
 * @details    Enable Double Transfer Rate Mode for Read Mode Phase
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAR_READ_DTR(spim)             \
    do                                                \
    {                                                 \
        (spim->PHDMAR |= (SPIM_PHDMAR_DTR_MODE_Msk)); \
    } while (0)

/**
 * @details    Disable Double Transfer Rate Mode for Read Mode Phase
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAR_READ_DTR(spim)             \
    do                                                 \
    {                                                  \
        (spim->PHDMAR &= ~(SPIM_PHDMAR_DTR_MODE_Msk)); \
    } while (0)

/**
 * @details Set SPI Interface Bit Mode for Read Mode Phase.
 * @param x Read Mode Phase Bits Mode
 *          - \ref PHASE_NORMAL_MODE : Standard Mode
 *          - \ref PHASE_DUAL_MODE   : Dual Mode
 *          - \ref PHASE_QUAD_MODE   : Quad Mode
 *          - \ref PHASE_OCTAL_MODE  : Octal Mode
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_READ_BIT_MODE(spim, x)          \
    do                                                  \
    {                                                   \
        spim->PHDMAR &= ~(SPIM_PHDMAR_BM_MODE_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_BM_MODE_Pos); \
    } while (0)

/**
 * @details    Enable Double Transfer Rate Mode for Data Phase
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAR_DATA_DTR(spim)             \
    do                                                \
    {                                                 \
        (spim->PHDMAR |= (SPIM_PHDMAR_DTR_DATA_Msk)); \
    } while (0)

/**
 * @details    Disable Double Transfer Rate Mode for Data Phase
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAR_DATA_DTR(spim)             \
    do                                                 \
    {                                                  \
        (spim->PHDMAR &= ~(SPIM_PHDMAR_DTR_DATA_Msk)); \
    } while (0)

/**
 * @details    Enable Read DQS Mode for Data Phase
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAR_DATA_RDQS(spim)             \
    do                                                 \
    {                                                  \
        (spim->PHDMAR |= (SPIM_PHDMAR_RDQS_DATA_Msk)); \
    } while (0)

/**
 * @details    Disable Read DQS Mode for Data Phase
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAR_DATA_RDQS(spim)             \
    do                                                  \
    {                                                   \
        (spim->PHDMAR &= ~(SPIM_PHDMAR_RDQS_DATA_Msk)); \
    } while (0)

/**
 * @details Set Received Data Byte Order of Received Data Phase for Octal SPI Flash
 * @param x Data Byte Order
 *          - \ref PHASE_ORDER_MODE0 : Byte order byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7.
 *          - \ref PHASE_ORDER_MODE1 : Byte order byte7, byte6, byte5, byte4, byte3, byte2, byte1, byte0.
 *          - \ref PHASE_ORDER_MODE2 : Byte order byte1, byte0, byte3, byte2, byte5, byte4, byte7, byte6.
 *          - \ref PHASE_ORDER_MODE3 : Byte order byte6, byte7, byte4, byte5, byte2, byte3, byte0, byte1.
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_RBO_DATA(spim, x)                \
    do                                                   \
    {                                                    \
        spim->PHDMAR &= ~(SPIM_PHDMAR_RBO_DATA_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_RBO_DATA_Pos); \
    } while (0)

/**
 * @details Set SPI Interface Bit Mode for Data Phase.
 * @param x Data Phase Bits Mode
 *          - \ref PHASE_NORMAL_MODE : Standard Mode
 *          - \ref PHASE_DUAL_MODE   : Dual Mode
 *          - \ref PHASE_QUAD_MODE   : Quad Mode
 *          - \ref PHASE_OCTAL_MODE  : Octal Mode
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_DATA_BIT_MODE(spim, x)          \
    do                                                  \
    {                                                   \
        spim->PHDMAR &= ~(SPIM_PHDMAR_BM_DATA_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_BM_DATA_Pos); \
    } while (0)

/*----------------------------------------------------------------------------*/
/* SPIM_PHDMM constant definitions                                            */
/*----------------------------------------------------------------------------*/
#define SPIM_CLEAR_PHDMM(spim)          \
    do                                  \
    {                                   \
        (spim->PHDMM &= ~(0xFFFFFFFF)); \
    } while (0)

/**
 * @details Set DMM Mode Data Width for Command Phase.
 * @param x Command Phase Witdh
 *          - \ref PHASE_WIDTH_8  : 8 bits
 *          - \ref PHASE_WIDTH_16 : 16 bits
 *          - \ref PHASE_WIDTH_24 : 24 bits
 *          - \ref PHASE_WIDTH_32 : 32 bits
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_CMD_WIDTH(spim, x)            \
    do                                               \
    {                                                \
        spim->PHDMM &= ~(SPIM_PHDMM_DW_CMD_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_DW_CMD_Pos); \
    } while (0)

/**
 * @details    Get DMM Mode Data Width for Command Phase.
 * \hideinitializer
 */
#define SPIM_GET_PHDMM_CMD_WIDTH(spim)  \
    ((spim->PHDMM & SPIM_PHDMM_DW_CMD_Msk) >> SPIM_PHDMM_DW_CMD_Pos)

/**
 * @details    Enable DMM Mode Double Transfer Rate Mode for Command Phase
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMM_CMD_DTR(spim)            \
    do                                             \
    {                                              \
        (spim->PHDMM |= (SPIM_PHDMM_DTR_CMD_Msk)); \
    } while (0)

/**
 * @details    Disable DMM Mode Double Transfer Rate Mode for Command Phase
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMM_CMD_DTR(spim)            \
    do                                              \
    {                                               \
        (spim->PHDMM &= ~(SPIM_PHDMM_DTR_CMD_Msk)); \
    } while (0)

/**
 * @details    Get DMM Mode Double Transfer Rate Mode Enable Bit for Command Phase
 * \hideinitializer
 */
#define SPIM_GET_PHDMM_CMD_DTR(spim)    \
    ((spim->PHDMM & SPIM_PHDMM_DTR_CMD_Msk) >> SPIM_PHDMM_DTR_CMD_Pos)

/**
 * @details Set SPI Interface Bit Mode for Command Phase,
 * @param x Command Phase Bits Mode
 *          - \ref PHASE_NORMAL_MODE : Standard Mode
 *          - \ref PHASE_DUAL_MODE   : Dual Mode
 *          - \ref PHASE_QUAD_MODE   : Quad Mode
 *          - \ref PHASE_OCTAL_MODE  : Octal Mode
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_CMD_BIT_MODE(spim, x)         \
    do                                               \
    {                                                \
        spim->PHDMM &= ~(SPIM_PHDMM_BM_CMD_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_BM_CMD_Pos); \
    } while (0)

/**
 * @details     Get SPI Interface Bit Mode for Command Phase,
 * \hideinitializer
 */
#define SPIM_GET_PHDMM_CMD_BIT_MODE(spim)   \
    ((spim->PHDMM & SPIM_PHDMM_BM_CMD_Msk) >> SPIM_PHDMM_BM_CMD_Pos)

/**
 * @details Set Data Width for Address Phase.
 * @param spim
 * @param x Set Address Width
 *          - \ref PHASE_WIDTH_8  : 8 bits
 *          - \ref PHASE_WIDTH_16 : 16 bits
 *          - \ref PHASE_WIDTH_24 : 24 bits
 *          - \ref PHASE_WIDTH_32 : 32 bits
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_ADDR_WIDTH(spim, x)            \
    do                                                \
    {                                                 \
        spim->PHDMM &= ~(SPIM_PHDMM_DW_ADDR_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_DW_ADDR_Pos); \
    } while (0)

/**
 * @details    Enable Double Transfer Rate Mode for Address Phase
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMM_ADDR_DTR(spim)            \
    do                                              \
    {                                               \
        (spim->PHDMM |= (SPIM_PHDMM_DTR_ADDR_Msk)); \
    } while (0)

/**
 * @details    Disable Double Transfer Rate Mode for Address Phase
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMM_ADDR_DTR(spim)            \
    do                                               \
    {                                                \
        (spim->PHDMM &= ~(SPIM_PHDMM_DTR_ADDR_Msk)); \
    } while (0)

/**
 * @details Set SPI Interface Bit Mode for Address Phase.
 * @param spim
 * @param x Address Phase Bit Mode
 *          - \ref PHASE_NORMAL_MODE : Standard Mode
 *          - \ref PHASE_DUAL_MODE   : Dual Mode
 *          - \ref PHASE_QUAD_MODE   : Quad Mode
 *          - \ref PHASE_OCTAL_MODE  : Octal Mod
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_ADDR_BIT_MODE(spim, x)         \
    do                                                \
    {                                                 \
        spim->PHDMM &= ~(SPIM_PHDMM_BM_ADDR_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_BM_ADDR_Pos); \
    } while (0)

/**
 * @details Set Data Width for Mode Phase.
 * @param x Read mode phase width
 *          - \ref PHASE_WIDTH_8  : 8 bits
 *          - \ref PHASE_WIDTH_16 : 16 bits
 *          - \ref PHASE_WIDTH_24 : 24 bits
 *          - \ref PHASE_WIDTH_32 : 32 bits
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_READ_DATA_WIDTH(spim, x)       \
    do                                                \
    {                                                 \
        spim->PHDMM &= ~(SPIM_PHDMM_DW_MODE_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_DW_MODE_Pos); \
    } while (0)

/**
 * @details Enable Double Transfer Rate Mode for Mode Phase
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMM_READ_DTR(spim)            \
    do                                              \
    {                                               \
        (spim->PHDMM |= (SPIM_PHDMM_DTR_MODE_Msk)); \
    } while (0)

/**
 * @details Disable Double Transfer Rate Mode for Mode Phase
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMM_READ_DTR(spim)            \
    do                                               \
    {                                                \
        (spim->PHDMM &= ~(SPIM_PHDMM_DTR_MODE_Msk)); \
    } while (0)

/**
 * @details Set SPI Interface Bit Mode for Mode Phase.
 * @param x Read Mode Phase Bit Mode
 *          - \ref PHASE_NORMAL_MODE : Standard Mode
 *          - \ref PHASE_DUAL_MODE   : Dual Mode
 *          - \ref PHASE_QUAD_MODE   : Quad Mode
 *          - \ref PHASE_OCTAL_MODE  : Octal Mod
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_READ_BIT_MODE(spim, x)         \
    do                                                \
    {                                                 \
        spim->PHDMM &= ~(SPIM_PHDMM_BM_MODE_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_BM_MODE_Pos); \
    } while (0)

/**
 * @details Enable Double Transfer Rate Mode for Data Phase
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMM_DATA_DTR(spim)            \
    do                                              \
    {                                               \
        (spim->PHDMM |= (SPIM_PHDMM_DTR_DATA_Msk)); \
    } while (0)

/**
 * @details Disable Double Transfer Rate Mode for Data Phase
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMM_DATA_DTR(spim)            \
    do                                               \
    {                                                \
        (spim->PHDMM &= ~(SPIM_PHDMM_DTR_DATA_Msk)); \
    } while (0)

/**
 * @details Enable Read DQS Mode for Data Phase
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMM_DATA_RDQS(spim)            \
    do                                               \
    {                                                \
        (spim->PHDMM |= (SPIM_PHDMM_RDQS_DATA_Msk)); \
    } while (0)

/**
 * @details Disable Read DQS Mode for Data Phase
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMM_DATA_RDQS(spim)            \
    do                                                \
    {                                                 \
        (spim->PHDMM &= ~(SPIM_PHDMM_RDQS_DATA_Msk)); \
    } while (0)

/**
 * @details Set Received Data Byte Order of Received Data Phase for Octal SPI Flash
 * @param x Data Byte Order
 *          - \ref PHASE_ORDER_MODE0 : Byte order byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7.
 *          - \ref PHASE_ORDER_MODE1 : Byte order byte7, byte6, byte5, byte4, byte3, byte2, byte1, byte0.
 *          - \ref PHASE_ORDER_MODE2 : Byte order byte1, byte0, byte3, byte2, byte5, byte4, byte7, byte6.
 *          - \ref PHASE_ORDER_MODE3 : Byte order byte6, byte7, byte4, byte5, byte2, byte3, byte0, byte1.
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_RBO_DATA(spim, x)               \
    do                                                 \
    {                                                  \
        spim->PHDMM &= ~(SPIM_PHDMM_RBO_DATA_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_RBO_DATA_Pos); \
    } while (0)

/**
 * @details Set SPI Interface Bit Mode for Data Phase.
 * @param x Read mode phase width
 *          - \ref PHASE_WIDTH_8  : 8 bits
 *          - \ref PHASE_WIDTH_16 : 16 bits
 *          - \ref PHASE_WIDTH_24 : 24 bits
 *          - \ref PHASE_WIDTH_32 : 32 bits
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_DATA_BIT_MODE(spim, x)         \
    do                                                \
    {                                                 \
        spim->PHDMM &= ~(SPIM_PHDMM_BM_DATA_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_BM_DATA_Pos); \
    } while (0)

/*----------------------------------------------------------------------------*/
/* SPIM_DLLx constant definitions                                            */
/*----------------------------------------------------------------------------*/
/**
 * @details Set DLL0 OLDO Enable Bit, 0: Disable, 1: Enable
 * \hideinitializer
 */
#define SPIM_ENABLE_DLL0_OLDO(spim, x)                               \
    do                                                              \
    {                                                               \
        spim->DLL0 &= ~(SPIM_DLL0_DLLOLDO_Msk);                     \
        spim->DLL0 |= (((x) ? 1UL : 0UL) << SPIM_DLL0_DLLOLDO_Pos); \
    } while (0)

/**
 * @details Set DLL0 Output Valid Counter Reset
 * \hideinitializer
 */
#define SPIM_ENABLE_DLL0_OVRST(spim, x)                              \
    do                                                               \
    {                                                                \
        spim->DLL0 &= ~(SPIM_DLL0_DLLOVRST_Msk);                     \
        spim->DLL0 |= (((x) ? 1UL : 0UL) << SPIM_DLL0_DLLOVRST_Pos); \
    } while (0)

/**
 * @details Wait DLL0 Output Valid Counter Reset Done
 * \hideinitializer
 */
#define SPIM_WAIT_DLL0_OVRST(spim)  \
    ((spim->DLL0 & SPIM_DLL0_DLLOVRST_Msk) >> SPIM_DLL0_DLLOVRST_Pos)

/**
 * @details Wait DLL0 Clock Divider Circuit Status Bit
 * \hideinitializer
 */
#define SPIM_WAIT_DLL0_CLKON(spim)  \
    ((spim->DLL0 & SPIM_DLL0_DLLCLKON_Msk) >> SPIM_DLL0_DLLCLKON_Pos)

/**
 * @details Wait DLL0 Lock Status Bit
 * \hideinitializer
 */
#define SPIM_WAIT_DLL0_LOCK(spim)   \
    ((spim->DLL0 & SPIM_DLL0_DLLLOCK_Msk) >> SPIM_DLL0_DLLLOCK_Pos)

/**
 * @details Wait DLL0 Output Ready Status
 * \hideinitializer
 */
#define SPIM_WAIT_DLL0_READY(spim)  \
    ((spim->DLL0 & SPIM_DLL0_DLLREADY_Msk) >> SPIM_DLL0_DLLREADY_Pos)

/**
 * @details Wait DLL0 Refresh Status Bit
 * \hideinitializer
 */
#define SPIM_WAIT_DLL0_REFRESH(spim)    \
    ((spim->DLL0 & SPIM_DLL0_DLL_REF_Msk) >> SPIM_DLL0_DLL_REF_Pos)

/**
 * @details Set DLL0 Delay Step Number 0 ~ 1F
 * \hideinitializer
 */
#define SPIM_SET_DLL0_DELAY_NUM(spim, x)               \
    do                                                 \
    {                                                  \
        spim->DLL0 &= ~(SPIM_DLL0_DLL_DNUM_Msk);       \
        spim->DLL0 |= ((x) << SPIM_DLL0_DLL_DNUM_Pos); \
    } while (0)

/**
 * @details Set Cycle Number of between DLL Lock and DLL Output Valid 0 ~ 0xFFFF, Default 0x1388
 * \hideinitializer
 */
#define SPIM_SET_DLL1_OUT_VALID(spim, x)               \
    do                                                 \
    {                                                  \
        spim->DLL1 &= ~(SPIM_DLL1_DLLOVNUM_Msk);       \
        spim->DLL1 |= ((x) << SPIM_DLL1_DLLOVNUM_Pos); \
    } while (0)

/**
 * @details Get Cycle Number of DLL1 Ouput Valid 0 ~ 0xFFFF, Default 0x07D0
 * \hideinitializer
 */
#define SPIM_GET_DLL1_LOCKK_VALID(spim) \
    ((spim->DLL1 & SPIM_DLL1_DLLLKNUM_Msk) >> SPIM_DLL1_DLLLKNUM_Pos)

/**
 * @details Set Cycle Number between DLL Clock Divider Enable and DLL Lock Valid 0 ~ 0xFFFF, Default 0x07D0
 * \hideinitializer
 */
#define SPIM_SET_DLL1_LOCKK_VALID(spim, x)             \
    do                                                 \
    {                                                  \
        spim->DLL1 &= ~(SPIM_DLL1_DLLLKNUM_Msk);       \
        spim->DLL1 |= ((x) << SPIM_DLL1_DLLLKNUM_Pos); \
    } while (0)

/**
 * @details Get Cycle Number between DLL Clock Divider Enable and DLL Lock Valid 0 ~ 0xFFFF, Default 0x07D0
 * \hideinitializer
 */
#define SPIM_GET_DLL1_LOCKK_VALID(spim) \
    ((spim->DLL1 & SPIM_DLL1_DLLLKNUM_Msk) >> SPIM_DLL1_DLLLKNUM_Pos)

/**
 * @details Get Cycle Number of DLL1 Ouput Valid 0 ~ 0xFFFF, Default 0x07D0
 * \hideinitializer
 */
#define SPIM_GET_DLL1_OUT_VALID(spim)   (spim->DLL1 & SPIM_DLL1_DLLOVNUM_Msk)

/**
 * @details Set Cycle Number of DLL1 Lock Time 0 ~ 0xFFFF, Default 0x1388
 * \hideinitializer
 */
#define SPIM_SET_DLL1_LOCK_TIME(spim, x)               \
    do                                                 \
    {                                                  \
        spim->DLL1 &= ~(SPIM_DLL1_DLLLKNUM_Msk);       \
        spim->DLL1 |= ((x) << SPIM_DLL1_DLLLKNUM_Pos); \
    } while (0)

/**
 * @details Set Cycle Number of between DLL OLDO Enable and DLL Clock Divider Enable Time 0 ~ 0xFFFF, Default 0x07D0
 * \hideinitializer
 */
#define SPIM_SET_DLL2_CLKON_NUM(spim, x)               \
    do                                                 \
    {                                                  \
        spim->DLL2 &= ~(SPIM_DLL2_CLKONNUM_Msk);       \
        spim->DLL2 |= ((x) << SPIM_DLL2_CLKONNUM_Pos); \
    } while (0)

/**
 * @details Set Cycle Number of between DLL Output Valid and DLL Auto Trim Enable Time 0 ~ 0xFFFF, Default 0x07D0
 * \hideinitializer
 */
#define SPIM_SET_DLL2_TRIM_NUM(spim, x)               \
    do                                                \
    {                                                 \
        spim->DLL2 &= ~(SPIM_DLL2_TRIMNUM_Msk);       \
        spim->DLL2 |= ((x) << SPIM_DLL2_TRIMNUM_Pos); \
    } while (0)

/** @addtogroup SPIM_EXPORTED_TYPEDEF SPIM Exported Type Defines
  @{
*/
typedef struct
{
    uint32_t u32CMDCode;        /*!< Page Program Command Code */

    uint32_t u32CMDPhase;       /*!< Command phase mode */
    uint32_t u32CMDWidth;       /*!< Command Width */
    uint32_t u32CMDDTR;         /*!< Command use DTR mode */

    uint32_t u32AddrPhase;      /*!< Address phase mode */
    uint32_t u32AddrWidth;      /*!< Address Width */
    uint32_t u32AddrDTR;        /*!< Address use DTR mode */

    uint32_t u32DataPhase;      /*!< Data phase mode */
    uint32_t u32ByteOrder;      /*!< Data Byte Order */
    uint32_t u32DataDTR;        /*!< Data use DTR mode */

    uint32_t u32DcNum;          /*!< Dummy cycle count */
    //uint32_t u32DcPhase;        /*!< Dummy cycle phase mode */
    //uint32_t u32DcDTR;          /*!< Dummy cycle use DTR mode */

    /* Continue read mode only support 0xBB, 0xEB, 0xE7, 0x0D, 0xBD, and 0xED */
    uint32_t u32ContRdEn;       /*!< Enable Continue Read mode */
    uint32_t u32RdModePhase;    /*!< Read mode phase mode */
    uint32_t u32RdModeWidth;    /*!< Read mode phase mode */
    uint32_t u32RdModeDTR;      /*!< Read mode use DTR mode */

    //uint32_t u32Is4ByteAddr;    /*!< 4 bytes address mode */
    //uint32_t u32Sync;           /*!< Wait device ready */
} PHASE_SET_T;  /*!< Structure holds SPIM IO phase info */


/** @addtogroup SPIM HYPER_EXPORTED_CONSTANTS HYPER Exported Constants
  @{
*/
#define HYPER_RAM_ID_REG0                  (0x00000000)    /* Hyper RAM Identification Register 0. */
#define HYPER_RAM_ID_REG1                  (0x00000002)    /* Hyper RAM Identification Register 1. */
#define HYPER_RAM_CONFIG_REG0              (0x00001000)    /* Hyper RAM Configuration Register 0. */
#define HYPER_RAM_CONFIG_REG1              (0x00001002)    /* Hyper RAM Configuration Register 1. */

#define SPIM_HYPER_CMD_IDLE                 (0x00000000)    /* Hyper Bus interface is Idle. */
#define SPIM_HYPER_CMD_RESET                (0x00000001)    /* Reset Hyper Bus Devices. */
#define SPIM_HYPER_CMD_READ_HRAM_REGISTER   (0x00000002)    /* Read Hyper RAM Regsiter (16-Bit, Read Data[15:0]. */
#define SPIM_HYPER_CMD_EXIT_HS_PD           (0x00000005)    /* Exit From Hybrid Sleep and deep power down. */
#define SPIM_HYPER_CMD_WRITE_HRAM_REGISTER  (0x00000007)    /* Write Hyper RAM Regsiter (16-Bit, Write Data[15:0]. */
#define SPIM_HYPER_CMD_READ_1_WORD          (0x00000008)    /* Read 1 word (Read Data[15:0]) from Hyper Bus Devices. */
#define SPIM_HYPER_CMD_READ_2_WORD          (0x00000009)    /* Read 2 word (Read Data[31:0]) from Hyper Bus Devices. */
#define SPIM_HYPER_CMD_WRITE_1_BYTE         (0x0000000C)    /* Write 1 Byte (Write Data[7:0]) to Hyper Bus Devices. */
#define SPIM_HYPER_CMD_WRITE_2_BYTE         (0x0000000D)    /* Write 2 Byte (Write Data[15:0]) to Hyper Bus Devices. */
#define SPIM_HYPER_CMD_WRITE_3_BYTE         (0x0000000E)    /* Write 3 Byte (Write Data[23:0]) to Hyper Bus Devices. */
#define SPIM_HYPER_CMD_WRITE_4_BYTE         (0x0000000F)    /* Write 4 Byte (Write Data[31:0]) to Hyper Bus Devices. */

/*----------------------------------------------------------------------------*/
/* SPIM_HYPER_CONFIG1: Chip Select Setup Time to Next CK Rising Edge
    00 = 1.5 HCLK cycles.
    01 = 2.5 HCLK cycles.
    10 = 3.5 HCLK cycles.
    11 = 4.5 HCLK cycles.
*/
/*----------------------------------------------------------------------------*/
#define SPIM_HYPER_CONFIG1_CSST_1_5_HCLK    (0x0)
#define SPIM_HYPER_CONFIG1_CSST_2_5_HCLK    (0x1)
#define SPIM_HYPER_CONFIG1_CSST_3_5_HCLK    (0x2)
#define SPIM_HYPER_CONFIG1_CSST_4_5_HCLK    (0x3)

/*----------------------------------------------------------------------------*/
/* SPIM_HYPER_CONFIG1: Chip Select Hold Time After CK Falling Edge
    00 = 0.5 HCLK cycles.
    01 = 1.5 HCLK cycles.
    10 = 2.5 HCLK cycles.
    11 = 3.5 HCLK cycles.
*/
/*----------------------------------------------------------------------------*/
#define SPIM_HYPER_CONFIG1_CSH_0_5_HCLK    (0x0)
#define SPIM_HYPER_CONFIG1_CSH_1_5_HCLK    (0x1)
#define SPIM_HYPER_CONFIG1_CSH_2_5_HCLK    (0x2)
#define SPIM_HYPER_CONFIG1_CSH_3_5_HCLK    (0x3)

/*----------------------------------------------------------------------------*/
/* SPIM_HYPER_CONFIG1: Chip Select High between Transaction
    10 = 0.5 HCLK cycles.
    ...
    1111 = 3.5 HCLK cycles.
*/
/*----------------------------------------------------------------------------*/
#define SPIM_HYPER_CONFIG1_CSHI_2_HCLK    (0x02)
#define SPIM_HYPER_CONFIG1_CSHI_3_HCLK    (0x03)
#define SPIM_HYPER_CONFIG1_CSHI_4_HCLK    (0x04)
#define SPIM_HYPER_CONFIG1_CSHI_5_HCLK    (0x05)
#define SPIM_HYPER_CONFIG1_CSHI_6_HCLK    (0x06)
#define SPIM_HYPER_CONFIG1_CSHI_7_HCLK    (0x07)
#define SPIM_HYPER_CONFIG1_CSHI_8_HCLK    (0x08)
#define SPIM_HYPER_CONFIG1_CSHI_9_HCLK    (0x09)
#define SPIM_HYPER_CONFIG1_CSHI_10_HCLK   (0x0A)
#define SPIM_HYPER_CONFIG1_CSHI_11_HCLK   (0x0B)
#define SPIM_HYPER_CONFIG1_CSHI_12_HCLK   (0x0C)
#define SPIM_HYPER_CONFIG1_CSHI_13_HCLK   (0x0D)
#define SPIM_HYPER_CONFIG1_CSHI_14_HCLK   (0x0E)
#define SPIM_HYPER_CONFIG1_CSHI_15_HCLK   (0x0F)

/*----------------------------------------------------------------------------*/
/*  Define SPIM HYPER Macros and functions                                    */
/*----------------------------------------------------------------------------*/
/**
  * @brief  Set Hyper Chip Select Setup Time to Next CK Rising Edge
  * @param[in]  x Chip Select Setup Time to Next CK Rising Edge.
  *               - \ref SPIM_HYPER_CONFIG1_CSST_1_5_HCLK : 1.5 HCLK cycles
  *               - \ref SPIM_HYPER_CONFIG1_CSST_2_5_HCLK : 2.5 HCLK cycles
  *               - \ref SPIM_HYPER_CONFIG1_CSST_3_5_HCLK : 3.5 HCLK cycles
  *               - \ref SPIM_HYPER_CONFIG1_CSST_4_5_HCLK : 4.5 HCLK cycles
  * @details    This Macro Set Hyper Chip Select Setup Time to Next CK Rising Edge
  * \hideinitializer
  */
#define SPIM_SET_HYPER_CONFIG1_CSST(spim, x)                       \
    do                                                             \
    {                                                              \
        spim->HYPER_CONFIG1 &= ~(SPIM_HYPER_CONFIG1_CSST_Msk);     \
        spim->HYPER_CONFIG1 |= (x << SPIM_HYPER_CONFIG1_CSST_Pos); \
    } while (0)

/**
  * @brief  Set Hyper Chip Select Hold Time After CK Falling Edge
  * @param[in]  x   Chip Select Hold Time After CK Falling Edge.
  *                 - \ref SPIM_HYPER_CONFIG1_CSH_0_5_HCLK : 0.5 HCLK cycles
  *                 - \ref SPIM_HYPER_CONFIG1_CSH_1_5_HCLK : 1.5 HCLK cycles
  *                 - \ref SPIM_HYPER_CONFIG1_CSH_2_5_HCLK : 2.5 HCLK cycles
  *                 - \ref SPIM_HYPER_CONFIG1_CSH_3_5_HCLK : 3.5 HCLK cycles
  * @return     None
  * @details    This Macro Set Hyper Chip Select Hold Time After CK Falling Edge
  * \hideinitializer
  */
#define SPIM_SET_HYPER_CONFIG1_CSH(spim, x)                         \
    do                                                              \
    {                                                               \
        spim->HYPER_CONFIG1 &= ~(SPIM_HYPER_CONFIG1_CSH_Msk);       \
        spim->HYPER_CONFIG1 |= ((x) << SPIM_HYPER_CONFIG1_CSH_Pos); \
    } while (0)

/**
  * @brief  Set Hyper Chip Select High between Transaction
  * @param[in]  x   Set Chip Select High between Transaction as u8Value HCLK cycles
                    u8Value must be 2 ~ 16
  * @return     None
  * @details    This Macro Set Hyper Chip Select High between Transaction.
  * \hideinitializer
  */
#define SPIM_SET_HYPER_CONFIG1_CSHI(spim, x)                              \
    do                                                                    \
    {                                                                     \
        uint32_t u32Value = 0;                                            \
        if (x <= 1)                                                       \
        {                                                                 \
            u32Value = 2;                                                 \
        }                                                                 \
        else                                                              \
        {                                                                 \
            u32Value = x;                                                 \
        }                                                                 \
        spim->HYPER_CONFIG1 &= ~(SPIM_HYPER_CONFIG1_CSHI_Msk);            \
        spim->HYPER_CONFIG1 |= (u32Value << SPIM_HYPER_CONFIG1_CSHI_Pos); \
    } while (0)

/**
  * @brief  Set Hyper Chip Select Maximum Low Time
  * @param[in]  u32CsMaxLT  Set Hyper Chip Select Maximum Low Time as u32CsMaxLT HCLK cycles.
  *                         u32CsMaxLT must be 1 ~ 2048
  * @return     None
  * @details    This Macro Set Hyper Chip Select Maximum Low Time.
  * \hideinitializer
  */
#define SPIMS_SET_HYPER_CONFIG1_CSMAXLT(spim, u32CsMaxLT)                            \
    do                                                                               \
    {                                                                                \
        spim->HYPER_CONFIG1 &= ~(SPIM_HYPER_CONFIG1_CSMAXLT_Msk);                    \
        spim->HYPER_CONFIG1 |= ((u32CsMaxLT - 1) << SPIM_HYPER_CONFIG1_CSMAXLT_Pos); \
    } while (0)

/**
  * @brief  Get Hyper Chip Select Maximum Low Time
  * \hideinitializer
  */
#define SPIM_GET_HYPER_CONFIG1_CSMAXLT(spim)    \
    (((spim->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSMAXLT_Msk) >> SPIM_HYPER_CONFIG1_CSMAXLT_Pos) + 1UL)

/**
  * @brief  Set Hyper Chip Initial Read Access Time
  * @param[in]  x   Initial Access Time 1 ~ 31 Clock Cycle.
  * @return     None
  * @details    This Macro Set Hyper Chip Initial Access Time
  * \hideinitializer
  */
#define SPIM_SET_HYPER_CONFIG2_ACCTWR(spim, x)                       \
    do                                                               \
    {                                                                \
        spim->HYPER_CONFIG2 &= ~(SPIM_HYPER_CONFIG2_ACCTWR_Msk);     \
        spim->HYPER_CONFIG2 |= (x << SPIM_HYPER_CONFIG2_ACCTWR_Pos); \
    } while (0)

/**
  * @brief      Get Hyper Chip Initial Read Access Time
  * @details    This Macro Set Hyper Chip Initial Access Time
  * \hideinitializer
  */
#define SPIM_GET_HYPER_CONFIG2_ACCTWR(spim) \
    ((spim->HYPER_CONFIG2 & SPIM_HYPER_CONFIG2_ACCTWR_Msk) >> SPIM_HYPER_CONFIG2_ACCTWR_Pos)

/**
  * @brief      Set Hyper Device RESETN Low Time
  * @param[in]  u8Value   Initial Device RESETN Low Time 0 ~ 255.
  * @return     None
  * @details    This Macro Set Hyper Device RESETN Low Time
  * \hideinitializer
  */
#define SPIM_SET_HYPER_CONFIG2_RSTNLT(spim, u8Value)                       \
    do                                                                     \
    {                                                                      \
        spim->HYPER_CONFIG2 &= ~(SPIM_HYPER_CONFIG2_RSTNLT_Msk);           \
        spim->HYPER_CONFIG2 |= (u8Value << SPIM_HYPER_CONFIG2_RSTNLT_Pos); \
    } while (0)

/**
  * @brief      Get Hyper Device RESETN Low Time
  * @details    This Macro Set Hyper Device RESETN Low Time
  * \hideinitializer
  */
#define SPIM_GET_HYPER_CONFIG2_RSTNLT(spim) \
    ((spim->HYPER_CONFIG2 & SPIM_HYPER_CONFIG2_RSTNLT_Msk) >> SPIM_HYPER_CONFIG2_RSTNLT_Pos)

/**
  * @brief      Set Hyper Chip Initial Read Access Time
  * @param[in]  x   Initial Access Time 1 ~ 31 Clock Cycle.
  * @return     None
  * @details    This Macro Set Hyper Chip Initial Access Time
  * \hideinitializer
  */
#define SPIM_SET_HYPER_CONFIG2_ACCTRD(spim, x)                       \
    do                                                               \
    {                                                                \
        spim->HYPER_CONFIG2 &= ~(SPIM_HYPER_CONFIG2_ACCTRD_Msk);     \
        spim->HYPER_CONFIG2 |= (x << SPIM_HYPER_CONFIG2_ACCTRD_Pos); \
    } while (0)

/**
  * @brief      Get Hyper Chip Initial Read Access Time
  * @details    This Macro Set Hyper Chip Initial Access Time
  * \hideinitializer
  */
#define SPIM_GET_HYPER_CONFIG2_ACCTRD(spim) \
    ((spim->HYPER_CONFIG2 & SPIM_HYPER_CONFIG2_ACCTRD_Msk) >> SPIM_HYPER_CONFIG2_ACCTRD_Pos)

/**
 * @brief      Enable Hyper Chip Operation Done Interrupt
 * @param[in]  None
 * @return     None
 * @details    This macro enable Hyper Chip Operation Done Interrupt.
 * \hideinitializer
 */
#define SPIM_ENABLE_HYPER_INT(spim)                        \
    do                                                     \
    {                                                      \
        spim->HYPER_INTEN |= SPIM_HYPER_INTEN_OPINTEN_Msk; \
    } while (0)

/**
 * @brief      Disable Hyper Chip Operation Done Interrupt
 * @param[in]  None
 * @return     None
 * @details    This macro disable Hyper Chip Operation Done Interrupt.
 * \hideinitializer
 */
#define SPIM_DISABLE_HYPER_INT(spim)                        \
    do                                                      \
    {                                                       \
        spim->HYPER_INTEN &= ~SPIM_HYPER_INTEN_OPINTEN_Msk; \
    } while (0)

/**
 * @brief      Get Hyper Bus Operation Done Interrupt
 * @param[in]  None
 * @return     None
 * @details    This macro enable Hyper Chip Operation Done Interrupt.
 * \hideinitializer
 */
#define SPIM_GET_HYPER_INT(spim)    \
    ((spim->HYPER_INTEN & SPIM_HYPER_INTEN_OPINTEN_Msk) >> SPIM_HYPER_INTEN_OPINTEN_Pos)

/**
 * @brief      Get Hyper Chip Operation Done Interrupt
 * @param[in]  None
 * @return     0 = Hyper Chip operation is busy.
 *             1 = Hyper Chip operation is done.
 * @details    This macro Get Hyper Chip Operation Done Interrupt.
 * \hideinitializer
 */
#define SPIM_GET_HYPER_INTSTS(spim) \
    ((spim->HYPER_INTEN & SPIM_HYPER_INTEN_OPINTEN_Msk) >> SPIM_HYPER_INTEN_OPINTEN_Pos)

/** @} end of group SPIM_EXPORTED_CONSTANTS */

/** @addtogroup SPIM_EXPORTED_FUNCTIONS SPIM Exported Functions
  @{
*/

/* Octal SPI flash and hyper device training DLL API */
int32_t SPIM_CtrlDLLDelayTime(SPIM_T *spim, uint32_t u32ClkOnNum, uint32_t u32TrimNum, uint32_t u32LKNum, uint32_t u32OVNum, uint32_t u32DelayNum);

/*----------------------------------------------------------------------------*/
/* SPIM Define Function Prototypes                                            */
/*----------------------------------------------------------------------------*/
int32_t SPIM_InitFlash(SPIM_T *spim, int clrWP);
uint32_t SPIM_GetSClkFreq(SPIM_T *spim);
void SPIM_ReadJedecId(SPIM_T *spim, uint8_t *idBuf, uint32_t u32NRx, uint32_t u32NBit, uint32_t u32DummyCycle, uint32_t u32DTREn);
int32_t SPIM_Enable_4Bytes_Mode(SPIM_T *spim, int isEn, uint32_t u32NBit, uint32_t u32DTREn);
int32_t SPIM_Is4ByteModeEnable(SPIM_T *spim, uint32_t u32NBit);
int32_t SPIM_SetWrapAroundEnable(SPIM_T *spim, int isEn, uint32_t u32WaBit);
int32_t SPIM_Wait_Write_Done(SPIM_T *spim, uint32_t u32NBit);
void SPIM_Set_Write_Enable(SPIM_T *spim, int isEn, uint32_t u32NBit);

void SPIM_ReadConfigRegister(SPIM_T *spim, uint8_t u8RdCMD, uint32_t u32Addr, uint8_t *pu8DataBuf, uint32_t u32NRx, uint32_t u32NBit);
void SPIM_WriteConfigRegister(SPIM_T *spim, uint8_t u8CMD, uint32_t u32Addr, uint8_t u8Data, uint32_t u32NBit);

void SPIM_ChipErase(SPIM_T *spim, uint32_t u32NBit, int isSync);
void SPIM_EraseBlock(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint8_t u8ErsCmd, uint32_t u32NBit, int isSync);

void SPIM_IO_Write(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint32_t u32NTx,
                   uint8_t *pu8TxBuf, uint8_t wrCmd, uint32_t u32NBitCmd,
                   uint32_t u32NBitAddr, uint32_t u32NBitDat, uint32_t u32DTREn);
void SPIM_IO_Read(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint32_t u32RdSize,
                  uint8_t *pu8RxBuf, uint8_t rdCmd, uint32_t u32NBitCmd,
                  uint32_t u32NBitAddr, uint32_t u32NBitDat, int u32NDummy,
                  uint32_t u32DTREn);

void SPIM_DMA_Write(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint32_t u32NTx, uint8_t *pu8TxBuf, uint32_t wrCmd);
int SPIM_DMA_Read(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint32_t u32NRx, uint8_t *pu8RxBuf, uint32_t u32RdCmd, int isSync);

uint32_t SPIM_GetDirectMapAddress(SPIM_T *spim);
void SPIM_EnterDirectMapMode(SPIM_T *spim, int is4ByteAddr, uint32_t u32RdCmd, uint32_t u32IdleIntvl);
int32_t SPIM_ExitDirectMapMode(SPIM_T *spim);

void SPIM_ExitContReadMode(SPIM_T *spim);
void SPIM_SetQuadEnable(SPIM_T *spim, int isEn, uint32_t u32NBit);

void SPIM_WinbondUnlock(SPIM_T *spim, uint32_t u32NBit);

/* PHDMAW/PHDMAR/PHDMM Register Setting */
int32_t SPIM_DMADMM_ClearPhaseSetting(SPIM_T *spim, uint32_t u32OPMode);
int32_t SPIM_DMADMM_SetCMDPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32NBit, uint32_t u32Width, uint32_t u32DTREn);
int32_t SPIM_DMADMM_SetAddrPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32NBit, uint32_t u32Width, uint32_t u32DTREn);
int32_t SPIM_DMADMM_SetContReadPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32NBit, uint32_t u32Width, uint32_t u32ContEn, uint32_t u32DTREn);
int32_t SPIM_DMADMM_SetDataPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32NBit, uint32_t u32ByteOrder, uint32_t u32RdDQS, uint32_t u32DTREn);

/* SPI flash command phase table init API */
void SPIM_DMADMM_InitPhase(SPIM_T *spim, PHASE_SET_T *psPhaseTable, uint32_t u32OPMode);
void SPIM_DMA_WritePhase(SPIM_T *spim, PHASE_SET_T *psPhaseTable, uint32_t u32Addr, int is4ByteAddr, uint32_t u32WrSize, uint8_t *pu8TxBuf);
int32_t SPIM_DMA_ReadPhase(SPIM_T *spim, PHASE_SET_T *psPhaseTable, uint32_t u32Addr, int is4ByteAddr, uint32_t u32RdSize, uint8_t *pu8RxBuf, int isSync);
void SPIM_DMM_ReadPhase(SPIM_T *spim, PHASE_SET_T *psPhaseTable, int is4ByteAddr, uint32_t u32IdleIntvl);

/* Use Normal I/O mode send phase data */
void SPIM_IO_SendCMDPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32OpCMD, uint32_t u32CMDPhase, uint32_t u32DTREn);
void SPIM_IO_SendAddrPhase(SPIM_T *spim, uint32_t u32Is4ByteAddr, uint32_t u32Addr, uint32_t u32AddrPhase, uint32_t u32DTREn);
int32_t SPIM_IO_SendDummyCycle(SPIM_T *spim, int u32NDummy);
void SPIM_IO_SendDataPhase(SPIM_T *spim, uint32_t u32OPMode, uint8_t *pu8TRxBuf, uint32_t u32TRxSize, uint32_t u32DataPhae, uint32_t u32DTREn);

/* SPI flash command phase table use normal I/O mode R/W API */
void SPIM_IO_WritePhase(SPIM_T *spim, PHASE_SET_T *psPhaseTable, uint32_t u32Addr, int is4ByteAddr, uint8_t *pu8RxBuf, uint32_t u32WrSize);
void SPIM_IO_ReadPhase(SPIM_T *spim, PHASE_SET_T *psPhaseTable, uint32_t u32Addr, int is4ByteAddr, uint8_t *pu8RxBuf, uint32_t u32RdSize);

/* Wait SPI flash write operation done */
int32_t SPIM_WaitSPIMENDone(SPIM_T *spim, uint32_t u32IsSync);

/* Set Micron MT35X SPI flash 4 bytes address access */
void SPIM_MT35x_4Bytes_Enable(SPIM_T *spim, int isEn, uint32_t u32NBit, uint32_t u32DTREn);

/*----------------------------------------------------------------------------*/
/* SPIM Hyper Device Define Function Prototypes                               */
/*----------------------------------------------------------------------------*/
/* HyperRAM */
int32_t SPIM_ExitHSAndDPD(SPIM_T *spim);
int SPIM_ReadHyperRAMReg(SPIM_T *spim, uint32_t u32Addr);
int SPIM_WriteHyperRAMReg(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Value);

/* Hyper Device API */
void SPIM_InitHyper(SPIM_T *spim, uint32_t u32Div);
int32_t SPIM_ResetHyper(SPIM_T *spim);
int16_t SPIM_Read1Word(SPIM_T *spim, uint32_t u32Addr);
int32_t SPIM_Read2Word(SPIM_T *spim, uint32_t u32Addr);
int32_t SPIM_Write1Byte(SPIM_T *spim, uint32_t u32Addr, uint8_t u8Data);
int32_t SPIM_Write2Byte(SPIM_T *spim, uint32_t u32Addr, uint16_t u16Data);
int32_t SPIM_Write3Byte(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Data);
int32_t SPIM_Write4Byte(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Data);

int32_t SPIM_DMAWrite_Hyper(SPIM_T *spim, uint32_t u32Addr, uint8_t *pu8WrBuf, uint32_t u32NTx);
int32_t SPIM_DMARead_Hyper(SPIM_T *spim, uint32_t u32Addr, uint8_t *pu8RdBuf, uint32_t u32NRx);

void SPIM_EnterDirectMapMode_Hyper(SPIM_T *spim);
void SPIM_ExitDirectMapMode_Hyper(SPIM_T *spim);
int32_t SPIM_IsDMMDone_Hyper(SPIM_T *spim);

/** @} end of group SPIM_EXPORTED_FUNCTIONS */
/** @} end of group SPIM_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __SPIM_H__ */
