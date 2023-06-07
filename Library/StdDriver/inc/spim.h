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

typedef struct
{
    uint32_t u32CMDPhase;
    uint32_t u32CMDDTR;

    uint32_t u32AddrPhase;
    uint32_t u32AddrDTR;

    uint32_t u32DcNum;
    uint32_t u32DcPhase;
    uint32_t u32DcDTR;

    uint32_t u32DataPhase;
    uint32_t u32DataDTR;

    uint32_t u32Is4ByteAddr;
    uint32_t u32Sync; //for IO Write
} SPIM_IO_PHASE_T;

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
#define SPIM0_DMM_MAP_ADDR      SPIM0_DMM_MAP_NSADDR
#else
#define SPIM0_DMM_MAP_ADDR      SPIM0_DMM_MAP_SADDR
#endif //

#if defined (SCU_INIT_D0PNS2_VAL) && (SCU_INIT_D0PNS2_VAL & BIT3)
#define SPIM1_DMM_MAP_ADDR       SPIM1_DMM_MAP_NSADDR
#else
#define SPIM1_DMM_MAP_ADDR       SPIM1_DMM_MAP_SADDR
#endif //

#define SPIM_DMM_SIZE                   (0x2000000UL)     /*!< DMM mode memory mapping size        \hideinitializer */
//#define SPIM_CCM_ADDR                   (0x20020000UL)      /*!< CCM mode memory map base address    \hideinitializer */
//#define SPIM_CCM_SIZE                   (0x8000UL)          /*!< CCM mode memory size                \hideinitializer */

#define SPIM_MAX_DLL_LATENCY            (0x05)            

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
// I/O Read Command
//------------------------------------------------------------------------------
#define CMD_NORMAL_PAGE_PROGRAM             (0x02UL)    /*!< SPIM_CMDCODE: Page Program (Page Write Mode Use) \hideinitializer */
#define CMD_NORMAL_PAGE_PROGRAM_4B          (0x12UL)    /*!< SPIM_CMDCODE: Page Program 4 Byte Address (Page Write Mode Use) \hideinitializer */
#define CMD_QUAD_PAGE_PROGRAM_WINBOND       (0x32UL)    /*!< SPIM_CMDCODE: Quad Page program (for Winbond) (Page Write Mode Use) \hideinitializer */
#define CMD_QUAD_PAGE_PROGRAM_WINBOND_4B    (0x34UL)    /*!< SPIM_CMDCODE: Quad Page program 4 Byte Address (for Winbond) (Page Write Mode Use) \hideinitializer */
#define CMD_QUAD_PAGE_PROGRAM_MXIC          (0x38UL)    /*!< SPIM_CMDCODE: Quad Page program (for MXIC) (Page Write Mode Use) \hideinitializer */
#define CMD_QUAD_PAGE_PROGRAM_EON           (0x40UL)    /*!< SPIM_CMDCODE: Quad Page Program (for EON) (Page Write Mode Use) \hideinitializer */

#define CMD_OCTAL_PAGE_PROG_MICRON          (0x82UL)    /*!< SPIM_CMDCODE: Quad Page Program (for EON) (Page Write Mode Use) \hideinitializer */
#define CMD_OCTAL_PAGE_PROG_MICRON_4B       (0x84UL)    /*!< SPIM_CMDCODE: Quad Page Program (for EON) (Page Write Mode Use) \hideinitializer */
#define CMD_OCTAL_EX_PAGE_PROG_MICRON       (0xC2UL)    /*!< SPIM_CMDCODE: Quad Page Program (for EON) (Page Write Mode Use) \hideinitializer */
#define CMD_OCTAL_EX_PAGE_PROG_MICRON_4B    (0x8EUL)    /*!< SPIM_CMDCODE: Quad Page Program (for EON) (Page Write Mode Use) \hideinitializer */

#define CMD_NORMAL_READ                     (0x03UL)    /*!< SPIM_CMDCODE: Read Data (Page Read Mode Use) \hideinitializer */
#define CMD_NORMAL_READ_4B                  (0x13UL)    /*!< SPIM_CMDCODE: Read Data 4 Byte Address( Page Read Mode Use) \hideinitializer */
#define CMD_FAST_READ                       (0x0BUL)    /*!< SPIM_CMDCODE: Fast Read (Page Read Mode Use) \hideinitializer */
#define CMD_FAST_READ_4B                    (0x0CUL)    /*!< SPIM_CMDCODE: Fast Read 4 Byte Address (Page Read Mode Use) \hideinitializer */

//------------------------------------------------------------------------------
// Dual Read Command
//------------------------------------------------------------------------------
#define CMD_DUAL_FAST_READ          (0x3BUL)    /*!< SPIM_CMDCODE: Fast Read Dual Output (Page Read Mode Use) \hideinitializer */
#define CMD_DUAL_FAST_READ_4B       (0x3CUL)    /*!< SPIM_CMDCODE: Fast Read Dual Output 4 Byte Address (Page Read Mode Use) \hideinitializer */
#define CMD_DUAL_FAST_IO_READ       (0xBBUL)    /*!< SPIM_CMDCODE: Fast Read Dual Output (Page Read Mode Use) \hideinitializer */
#define CMD_DUAL_FAST_IO_READ_4B    (0xBCUL)    /*!< SPIM_CMDCODE: Fast Read Dual Output 4 Byte Address (Page Read Mode Use) \hideinitializer */
#define CMD_DUAL_FAST_DTR_READ      (0xBDUL)    /*!< SPIM_CMDCODE: Fast Read Dual Output 4 Byte Address (Page Read Mode Use) \hideinitializer */

//------------------------------------------------------------------------------
// Quad Read Command
//------------------------------------------------------------------------------
#define CMD_QUAD_FAST_READ          (0x6BUL)    /*!< SPIM_CTL0: Fast Read Dual Output (Page Read Mode Use) \hideinitializer */
#define CMD_QUAD_FAST_READ_4B       (0x6CUL)    /*!< SPIM_CTL0: Fast Read Dual Output 4 Byte Address (Page Read Mode Use) \hideinitializer */
#define CMD_QUAD_FAST_IO_READ       (0xEBUL)    /*!< SPIM_CTL0: Fast Read Quad I/O (Page Read Mode Use) \hideinitializer */
#define CMD_QUAD_FAST_IO_READ_4B    (0xECUL)    /*!< SPIM_CTL0: Fast Read Quad I/O 4 Byte Address (Page Read Mode Use) \hideinitializer */
#define CMD_QUAD_FAST_DTR_READ      (0xEDUL)    /*!< SPIM_CTL0: Fast Read Quad I/O 4 Byte Address (Page Read Mode Use) \hideinitializer */

//------------------------------------------------------------------------------
// DTR Read Command
//------------------------------------------------------------------------------
#define CMD_DTR_NORMAL_READ         (0x0DUL)    /*!< SPIM_CTL0: Fast Read Dual Output (Page Read Mode Use) \hideinitializer */
#define CMD_DTR_DUAL_READ           (0xBDUL)    /*!< SPIM_CTL0: Fast Read Dual Output (Page Read Mode Use) \hideinitializer */
#define CMD_DTR_QUAD_READ           (0xEDUL)    /*!< SPIM_CTL0: Fast Read Dual Output (Page Read Mode Use) \hideinitializer */

//------------------------------------------------------------------------------
// Micron Octal Read Command
//------------------------------------------------------------------------------
#define CMD_OCTAL_FAST_READ         (0x8BUL)    /*!< SPIM_CTL0: Fast Read Dual Output (Page Read Mode Use) \hideinitializer */
#define CMD_OCTAL_FAST_IO_READ      (0xCBUL)    /*!< SPIM_CTL0: Fast Read Dual Output (Page Read Mode Use) \hideinitializer */
#define CMD_OCTAL_FAST_READ_4B      (0x7CUL)    /*!< SPIM_CTL0: Fast Read Dual Output (Page Read Mode Use) \hideinitializer */
#define CMD_OCTAL_FAST_IO_READ_4B   (0xCCUL)    /*!< SPIM_CTL0: Fast Read Dual Output (Page Read Mode Use) \hideinitializer */
#define CMD_OCTAL_DDR_FAST_READ     (0x9DUL)    /*!< SPIM_CTL0: Fast Read Dual Output (Page Read Mode Use) \hideinitializer */
#define CMD_OCTAL_DDR_FAST_IO_READ  (0xFDUL)    /*!< SPIM_CTL0: Fast Read Dual Output (Page Read Mode Use) \hideinitializer */

//------------------------------------------------------------------------------
// Continue Read Mode Command
//------------------------------------------------------------------------------
#define CMD_CLEAR_MODE_DATA             (0x00)
#define CMD_CONTINUE_READ_MODE          (0x20)

#define CMD_WRAP_8BIT                   (0x00)
#define CMD_WRAP_16BIT                  (0x20)
#define CMD_WRAP_32BIT                  (0x40)
#define CMD_WRAP_64BIT                  (0x60)

/*----------------------------------------------------------------------------*/
/* SPIM_PHDMAW, SPIM_PHDMAR, SPIM_PHDMM constant definitions                  */
/*----------------------------------------------------------------------------*/
#define PHASE_NORMAL_MODE               (0x0)
#define PHASE_DUAL_MODE                 (0x1)
#define PHASE_QUAD_MODE                 (0x2)
#define PHASE_OCTAL_MODE                (0x3)

#define PHASE_ORDER_MODE0               (0x0)
#define PHASE_ORDER_MODE1               (0x1)
#define PHASE_ORDER_MODE2               (0x2)
#define PHASE_ORDER_MODE3               (0x3)

#define PHASE_WIDTH_8                   (0x1)
#define PHASE_WIDTH_16                  (0x2)
#define PHASE_WIDTH_24                  (0x3)
#define PHASE_WIDTH_32                  (0x4)

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
}E_MFGID;

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

/** @endcond HIDDEN_SYMBOLS */

/* SPIM Define Error Code */
#define SPIM_TIMEOUT            SystemCoreClock /*!< SPIM time-out counter (1 second time-out) */
#define SPIM_OK                 ( 0L)           /*!< SPIM operation OK */
#define SPIM_ERR_FAIL           (-1L)           /*!< SPIM operation failed */
#define SPIM_ERR_TIMEOUT        (-2L)           /*!< SPIM operation abort due to timeout error */

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
 * @details    Set Hyper Chip Mode.
 * \hideinitializer
 */
#define SPIM_SET_HYPER_MODE(spim, x)                                              \
    do                                                                            \
    {                                                                             \
        if (((spim->CTL1 & SPIM_CTL1_DIVIDER_Msk) >> SPIM_CTL1_DIVIDER_Pos) <= 0) \
        {                                                                         \
            spim->CTL1 = (spim->CTL1 & ~SPIM_CTL1_DIVIDER_Msk) |                  \
                         (1 << SPIM_CTL1_DIVIDER_Pos);                            \
        }                                                                         \
        if (((spim->CTL1 & SPIM_CTL1_DIVIDER_Msk) >> SPIM_CTL1_DIVIDER_Pos) > 2)  \
        {                                                                         \
            spim->CTL1 = (spim->CTL1 & ~SPIM_CTL1_DIVIDER_Msk) |                  \
                         (2 << SPIM_CTL1_DIVIDER_Pos);                            \
        }                                                                         \
        spim->CTL0 &= ~SPIM_CTL0_HYPER_EN_Msk;                                    \
        spim->CTL0 |= (x << SPIM_CTL0_HYPER_EN_Pos);                              \
    } while (0)

/**
 * @details    Set 4-byte address to be enabled/disabled, 0: Disable, 1: Enable
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
#define SPIM_GET_4BYTE_ADDR_EN(spim)    (spim->CTL0 & SPIM_CTL0_B4ADDREN_Msk)

/**
 * @details    Enable SPIM interrupt
 * \hideinitializer
 */
//#define SPIM_ENABLE_INT(spim)       (spim->CTL0 |= SPIM_CTL0_IEN_Msk)

/**
 * @details    Disable SPIM interrupt
 * \hideinitializer
 */
//#define SPIM_DISABLE_INT(spim)      (spim->CTL0 &= ~SPIM_CTL0_IEN_Msk)

/**
 * @details    Set SPIM interrupt, 0: Disable, 1: Enable
 * \hideinitializer
 */
#define SPIM_SET_INT(spim, x)                                   \
    do                                                          \
    {                                                           \
        spim->CTL0 &= ~(SPIM_CTL0_IEN_Msk);                     \
        spim->CTL0 |= (((x) ? 1UL : 0UL) << SPIM_CTL0_IEN_Pos); \
    } while (0)

#define SPIM_GET_INT(spim)  ((spim->CTL0 & SPIM_CTL0_IEN_Msk) >> SPIM_CTL0_IEN_Pos)

/**
 * @details    Is interrupt flag on.
 * \hideinitializer
 */
#define SPIM_WAIT_IF_ON(spim)   while((spim->CTL0 & SPIM_CTL0_IF_Msk) >> SPIM_CTL0_IF_Pos)

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
 * @details    Set transmit/receive bit length
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
#define SPIM_GET_DATA_WIDTH(spim)   (((spim->CTL0 & SPIM_CTL0_DWIDTH_Msk) >> SPIM_CTL0_DWIDTH_Pos) + 1U)

/**
 * @details    Set data transmit/receive burst number, x = 0: one data TX/RX will be executed in one transfer
 *                                                         2: two data TX/RX will be executed in one transfer
 * *                                                       3: three data TX/RX will be executed in one transfer
 * *                                                       4: four data TX/RX will be executed in one transfer
 * \hideinitializer
 */
#define SPIM_SET_BURST_DATA(spim, x)                   \
    do                                                 \
    {                                                  \
        spim->CTL0 &= ~(SPIM_CTL0_BURSTNUM_Msk);       \
        spim->CTL0 |= ((x - 1UL) << SPIM_CTL0_BURSTNUM_Pos); \
    } while (0)

/**
 * @details    Get data transmit/receive burst number
 * \hideinitializer
 */
#define SPIM_GET_BURST_DATA(spim)    ((spim->CTL0 & SPIM_CTL0_BURSTNUM_Msk) >> SPIM_CTL0_BURSTNUM_Pos)

/**
 * @details    Set suspend interval which ranges between 0 and 15.
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
#define SPIM_GET_SUSP_INTVL(spim)       ((spim->CTL0 & SPIM_CTL0_SUSPITV_Msk) >> SPIM_CTL0_SUSPITV_Pos)

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
 * @details    Set operation mode.
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
#define SPIM_GET_OPMODE(spim)      (spim->CTL0 & SPIM_CTL0_OPMODE_Msk)

/**
 * @details    Set DTR(Data Transfer Rate) mode.
 * \hideinitializer
 */
#define SPIM_SET_DTR_MODE(spim, x)                   \
    do                                               \
    {                                                \
        spim->CTL0 &= (~SPIM_CTL0_DTR_NORM_Msk);     \
        spim->CTL0 |= (x << SPIM_CTL0_DTR_NORM_Pos); \
    } while (0)

#define SPIM_GET_DTR_MODE(spim)         ((spim->CTL0 & SPIM_CTL0_DTR_NORM_Msk) >> SPIM_CTL0_DTR_NORM_Pos)

/**
 * @details    Set Read DQS Mode.
 * \hideinitializer
 */
#define SPIM_SET_RDQS_MODE(spim, x)                   \
    do                                                \
    {                                                 \
        spim->CTL0 &= (~SPIM_CTL0_RDQS_NORM_Msk);     \
        spim->CTL0 |= (x << SPIM_CTL0_RDQS_NORM_Pos); \
    } while (0)

#define SPIM_GET_RDQS_MODE(spim)        ((spim->CTL0 & SPIM_CTL0_RDQS_NORM_Msk) >> SPIM_CTL0_RDQS_NORM_Pos)

/**
 * @details    Set Read Data Byte Order Mode.
 * \hideinitializer
 */
#define SPIM_SET_RBO_MODE(spim, x)                   \
    do                                               \
    {                                                \
        spim->CTL0 &= (~SPIM_CTL0_RBO_NORM_Msk);     \
        spim->CTL0 |= (x << SPIM_CTL0_RBO_NORM_Pos); \
    } while (0)

#define SPIM_GET_RBO_MODE(spim)        ((spim->CTL0 & SPIM_CTL0_RBO_NORM_Msk) >> SPIM_CTL0_RBO_NORM_Pos)

/**
 * @details    Set SPIM mode.
 * \hideinitializer
 */
#define SPIM_SET_SPIM_MODE(spim, x)                                          \
    do                                                                       \
    {                                                                        \
        spim->CMDCODE = (spim->CMDCODE & (~SPIM_CMDCODE_CMDCODE_Msk)) | (x); \
    } while (0)

/**
 * @details    Get SPIM mode.
 * \hideinitializer
 */
#define SPIM_GET_SPIM_MODE(spim)    ((spim->CMDCODE & SPIM_CMDCODE_CMDCODE_Msk) >> SPIM_CMDCODE_CMDCODE_Pos)

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
#define SPIM_IS_BUSY(spim)          ((spim->CTL1 & SPIM_CTL1_SPIMEN_Msk) >> SPIM_CTL1_SPIMEN_Pos)

/**
 * @details    Wait for free.
 * \hideinitializer
 */
#define SPIM_WAIT_FREE(spim)        while((spim->CTL1 & SPIM_CTL1_SPIMEN_Msk) >> SPIM_CTL1_SPIMEN_Pos)

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
 * @details    Enable CCM (Core Coupled Memory Mode).
 * \hideinitializer
 */
//#define SPIM_ENABLE_CCM(spim)          (spim->CTL1 |= SPIM_CTL1_CCMEN_Msk)

/**
 * @details    Disable CCM (Core Coupled Memory Mode).
 * \hideinitializer
 */
//#define SPIM_DISABLE_CCM(spim)         (spim->CTL1 &= ~SPIM_CTL1_CCMEN_Msk)

/**
 * @details    Is CCM (Core Coupled Memory Mode) enabled.
 * \hideinitializer
 */
//#define SPIM_IS_CCM_EN(spim)           ((spim->CTL1 & SPIM_CTL1_CCMEN_Msk) >> SPIM_CTL1_CCMEN_Pos)

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
 * @details    Set idle time interval
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
#define SPIM_GET_IDL_INTVL(spim)    ((spim->CTL1 & SPIM_CTL1_IDLETIME_Msk) >> SPIM_CTL1_IDLETIME_Pos)

/**
 * @details    Set SPIM clock divider
 * @note SPIM_Hyper_EN
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
#define SPIM_GET_CLOCK_DIVIDER(spim)    ((spim->CTL1 & SPIM_CTL1_DIVIDER_Msk) >> SPIM_CTL1_DIVIDER_Pos)

/**
 * @details    Set SPI flash deselect time interval of DMA write mode
 * \hideinitializer
 */
//#define SPIM_SET_RXCLKDLY_DWDELSEL(spim, x)                         \
//    do                                                              \
//    {                                                               \
//        spim->RXCLKDLY &= (~SPIM_RXCLKDLY_DWDELSEL_Msk);            \
//        spim->RXCLKDLY |= ((x) << SPIM_RXCLKDLY_DWDELSEL_Pos);      \
//    } while (0)

/**
 * @details    Get SPI flash deselect time interval of DMA write mode
 * \hideinitializer
 */
//#define SPIM_GET_RXCLKDLY_DWDELSEL(spim) \
//    ((spim->RXCLKDLY & SPIM_RXCLKDLY_DWDELSEL_Msk) >> SPIM_RXCLKDLY_DWDELSEL_Pos)

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
 * @details    Set sampling clock edge selection for received data
 * \hideinitializer
 */
//#define SPIM_SET_RXCLKDLY_RDEDGE(spim) \
//    (spim->RXCLKDLY |= SPIM_RXCLKDLY_RDEDGE_Msk)

/**
 * @details    Get sampling clock edge selection for received data
 * \hideinitializer
 */
//#define SPIM_CLR_RXCLKDLY_RDEDGE(spim) \
//    (spim->RXCLKDLY &= ~SPIM_RXCLKDLY_RDEDGE_Msk)

/**
 * @details    Set mode bits data for continuous read mode
 * \hideinitializer
 */
//#define SPIM_SET_DMMCTL_CRMDAT(spim, x)                                         \
//    do                                                                          \
//    {                                                                           \
//        spim->DMMCTL &= (~SPIM_DMMCTL_CRMDAT_Msk);                              \
//        spim->DMMCTL |= ((x) << SPIM_DMMCTL_CRMDAT_Pos) | SPIM_DMMCTL_CREN_Msk; \
//    } while (0)

/**
 * @details    Get mode bits data for continuous read mode
 * \hideinitializer
 */
//#define SPIM_GET_DMMCTL_CRMDAT(spim) \
//    ((spim->DMMCTL & SPIM_DMMCTL_CRMDAT_Msk) >> SPIM_DMMCTL_CRMDAT_Pos)

/**
 * @details    Set DMA/DMM mode SPI flash active SCLK time
 * \hideinitializer
 */
#define SPIM_DMAM_SET_ACTSCLKT(spim, x)                            \
    do                                                             \
    {                                                              \
        spim->DMMCTL &= ~SPIM_DMMCTL_ACTSCLKT_Msk;                 \
        spim->DMMCTL |= (((x)&0xFUL) << SPIM_DMMCTL_ACTSCLKT_Pos); \
        spim->DMMCTL |= SPIM_DMMCTL_UACTSCLK_Msk;                  \
    } while (0)

/**
 * @details    Set SPI flash active SCLK time as SPIM default
 * \hideinitializer
 */
#define SPIM_DMM_SET_DEFAULT_ACTSCLK(spim)           \
    do                                               \
    {                                                \
        (spim->DMMCTL &= ~(SPIM_DMMCTL_UACTSCLK_Msk)); \
    } while { 0 }

/**
 * @details    Set DMM mode SPI flash deselect time
 * \hideinitializer
 */
#define SPIM_DMM_SET_DESELTIM(spim, x)                              \
    do                                                              \
    {                                                               \
        spim->DMMCTL &= ~SPIM_DMMCTL_DESELTIM_Msk;                  \
        spim->DMMCTL |= (((x)&0x1FUL) << SPIM_DMMCTL_DESELTIM_Pos); \
    } while (0)

/**
 * @details    Get current DMM mode SPI flash deselect time setting
 * \hideinitializer
 */
#define SPIM_DMM_GET_DESELTIM(spim) \
    ((spim->DMMCTL & SPIM_DMMCTL_DESELTIM_Msk) >> SPIM_DMMCTL_DESELTIM_Pos)

/**
 * @details    Enable DMM mode burst wrap mode
 * \hideinitializer
 */
#define SPIM_DMM_ENABLE_BWEN(spim)              \
    do                                          \
    {                                           \
        (spim->DMMCTL |= SPIM_DMMCTL_BWEN_Msk); \
    } while (0)

/**
 * @details    Disable DMM mode burst wrap mode
 * \hideinitializer
 */
#define SPIM_DMM_DISABLE_BWEN(spim)                \
    do                                             \
    {                                              \
        (spim->DMMCTL &= ~(SPIM_DMMCTL_BWEN_Msk)); \
    } while (0)

/**
 * @details    Enable DMM mode continuous read mode
 * \hideinitializer
 */
#define SPIM_DMM_ENABLE_CREN(spim)              \
    do                                          \
    {                                           \
        (spim->DMMCTL |= SPIM_DMMCTL_CREN_Msk); \
    } while (0)

/**
 * @details    Disable DMM mode continuous read mode
 * \hideinitializer
 */
#define SPIM_DMM_DISABLE_CREN(spim)                \
    do                                             \
    {                                              \
        (spim->DMMCTL &= ~(SPIM_DMMCTL_CREN_Msk)); \
    } while (0)

/**
 * @details    DMM mode complete to stop TX/RX
 * \hideinitializer
 */
#define SPIM_DMM_HYPDONE(spim)                   \
    do                                           \
    {                                            \
        spim->DMMCTL |= SPIM_DMMCTL_HYPDONE_Msk; \
    } while (0)

/**
 * @details    Wait DMM mode complete to stop TX/RX
 * \hideinitializer
 */
#define SPIM_WAIT_HYPDONE(spim)       \
    while((spim->DMMCTL & SPIM_DMMCTL_HYPDONE_Msk) >> SPIM_DMMCTL_HYPDONE_Pos)

/**
 * @details    Set dummy cycle number (Only for DMM mode and DMA mode)
 * \hideinitializer
 */
//#define SPIM_SET_DCNUM(spim, x)                                                       \
//    do                                                                                \
//    {                                                                                 \
//        spim->CTL2 &= ~SPIM_CTL2_DCNUM_Msk;                                           \
//        spim->CTL2 |= (((x)&0x1FUL) << SPIM_CTL2_DCNUM_Pos) | SPIM_CTL2_USETEN_Msk;   \
//    } while (0)

/**
 * @details    Set dummy cycle number (Only for DMM mode and DMA mode) as SPIM default
 * \hideinitializer
 */
//#define SPIM_SET_DEFAULT_DCNUM(spim, x)  \
//    (spim->CTL2 &= ~SPIM_CTL2_USETEN_Msk)

/**
 * @details    Set dummy cycle number (Only DMA Command Mode)
 * \hideinitializer
 */
#define SPIM_SET_DC_DMAR(spim, x)                              \
    do                                                         \
    {                                                          \
        spim->CTL2 &= ~(SPIM_CTL2_DC_DMAR_Msk);                \
        spim->CTL2 |= (((x) & 0xFFUL) << SPIM_CTL2_DC_DMAR_Pos); \
    } while (0)

/**
 * @details    Clear dummy cycle number (Only DMA Command Mode)
 * \hideinitializer
 */
#define SPIM_CLEAR_DC_DMAR(spim)                  \
    do                                            \
    {                                             \
        (spim->CTL2 &= ~(SPIM_CTL2_DC_DMAR_Msk)); \
    } while (0)

/**
 * @details    Set dummy cycle number (Only DMM Command Mode)
 * \hideinitializer
 */
#define SPIM_SET_DC_DMM(spim, x)                              \
    do                                                        \
    {                                                         \
        spim->CTL2 &= ~(SPIM_CTL2_DC_DMM_Msk);                \
        spim->CTL2 |= (((x) & 0xFFUL) << SPIM_CTL2_DC_DMM_Pos); \
    } while (0)

/**
 * @details    Clear dummy cycle number (Only DMA Command Mode)
 * \hideinitializer
 */
#define SPIM_CLEAR_DC_DMM(spim)                  \
    do                                           \
    {                                            \
        (spim->CTL2 &= ~(SPIM_CTL2_DC_DMM_Msk)); \
    } while (0)

/**
 * @details    Set output data for mode phase
 * \hideinitializer
 */
#define SPIM_SET_MODE_DATA(spim, x)                                  \
    do                                                               \
    {                                                                \
        (spim->MODE = (spim->MODE & (~SPIM_MODE_MODEDATA_Msk)) | x); \
    } while (0)

/**
 * @details    Clear output data for mode phase
 * \hideinitializer
 */
#define SPIM_CLEAR_MODE_DATA(spim)                 \
    do                                             \
    {                                              \
        (spim->MODE &= ~(SPIM_MODE_MODEDATA_Msk)); \
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
 * @details    Set Write Data Width for Command Phase, x = 0: No command phase
 *                                                         1: 8 bits
 *                                                         2: 16 bits
 *                                                         3: 24 bits
 *                                                         4: 32 bits
 * \hideinitializer
 */
#define SPIM_PHDMAW_CMD_WIDTH(spim, x)                 \
    do                                                 \
    {                                                  \
        spim->PHDMAW &= ~(SPIM_PHDMAW_DW_CMD_Msk);     \
        spim->PHDMAW |= (x << SPIM_PHDMAW_DW_CMD_Pos); \
    } while (0)

/**
 * @details    Set DTR Mode Enable Bit for Command Phase
 * \hideinitializer
 */
#define SPIM_PHDMAW_CMD_DTR_ENABLE(spim)           \
    do                                             \
    {                                              \
        (spim->PHDMAW |= SPIM_PHDMAW_DTR_CMD_Msk); \
    } while (0)

/**
 * @details    Set DTR Mode Disable Bit for Command Phase
 * \hideinitializer
 */
#define SPIM_PHDMAW_CMD_DTR_DISABLE(spim)             \
    do                                                \
    {                                                 \
        (spim->PHDMAW &= ~(SPIM_PHDMAW_DTR_CMD_Msk)); \
    } while (0)

/**
 * @details    Set Bit Mode for Command Phase, x = 0: Standard Mode
 *                                                 1: Dual Mode
 *                                                 2: Quad Mode
 *                                                 3: Octal Mode
 * \hideinitializer
 */
#define SPIM_PHDMAW_CMD_BIT_MODE(spim, x)              \
    do                                                 \
    {                                                  \
        spim->PHDMAW &= ~(SPIM_PHDMAW_BM_CMD_Msk);     \
        spim->PHDMAW |= (x << SPIM_PHDMAW_BM_CMD_Pos); \
    } while (0)

/**
 * @details    Set Data Width for Address Phase, x = 0: No command phase
 *                                                   1: 8 bits
 *                                                   2: 16 bits
 *                                                   3: 24 bits
 *                                                   4: 32 bits
 * \hideinitializer
 */
#define SPIM_PHDMAW_ADDR_WIDTH(spim, x)                 \
    do                                                  \
    {                                                   \
        spim->PHDMAW &= ~(SPIM_PHDMAW_DW_ADDR_Msk);     \
        spim->PHDMAW |= (x << SPIM_PHDMAW_DW_ADDR_Pos); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Enable Bit for Address Phase
 * \hideinitializer
 */
#define SPIM_PHDMAW_ADDR_DTR_ENABLE(spim)             \
    do                                                \
    {                                                 \
        (spim->PHDMAW |= (SPIM_PHDMAW_DTR_ADDR_Msk)); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Disable Bit for Address Phase
 * \hideinitializer
 */
#define SPIM_PHDMAW_ADDR_DTR_DISABLE(spim)             \
    do                                                 \
    {                                                  \
        (spim->PHDMAW &= ~(SPIM_PHDMAW_DTR_ADDR_Msk)); \
    } while (0)

/**
 * @details    Set SPI Interface Bit Mode for Address Phase, x = 0: Standard Mode
 *                                                               1: Dual Mode
 *                                                               2: Quad Mode
 *                                                               3: Octal Mode
 * \hideinitializer
 */
#define SPIM_PHDMAW_ADDR_BIT_MODE(spim, x)              \
    do                                                  \
    {                                                   \
        spim->PHDMAW &= ~(SPIM_PHDMAW_BM_ADDR_Msk);     \
        spim->PHDMAW |= (x << SPIM_PHDMAW_BM_ADDR_Pos); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Enable Bit for Data Phase
 * \hideinitializer
 */
#define SPIM_PHDMAW_DATA_DTR_ENABLE(spim)             \
    do                                                \
    {                                                 \
        (spim->PHDMAW |= (SPIM_PHDMAW_DTR_DATA_Msk)); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Disable Bit for Data Phase
 * \hideinitializer
 */
#define SPIM_PHDMAW_DATA_DTR_DISABLE(spim)             \
    do                                                 \
    {                                                  \
        (spim->PHDMAW &= ~(SPIM_PHDMAW_DTR_DATA_Msk)); \
    } while (0)

/**
 * @details    Set Program Data Byte Order of Program Data Phase for Octal SPI Flash,
 *             x = 0: Byte order byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7.
 *                 1: Byte order byte7, byte6, byte5, byte4, byte3, byte2, byte1, byte0.
 *                 2: Byte order byte1, byte0, byte3, byte2, byte5, byte4, byte7, byte6.
 *                 3: Byte order byte6, byte7, byte4, byte5, byte2, byte3, byte0, byte1.
 * \hideinitializer
 */
#define SPIM_PHDMAW_PBO_DATA(spim, x)                    \
    do                                                   \
    {                                                    \
        spim->PHDMAW &= ~(SPIM_PHDMAW_PBO_DATA_Msk);     \
        spim->PHDMAW |= (x << SPIM_PHDMAW_PBO_DATA_Pos); \
    } while (0)

/**
 * @details    Set SPI Interface Bit Mode for Data Phase, x = 0: No command phase
 *                                                            1: 8 bits
 *                                                            2: 16 bits
 *                                                            3: 24 bits
 *                                                            4: 32 bits
 * \hideinitializer
 */
#define SPIM_PHDMAW_DATA_BIT_MODE(spim, x)              \
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
 * @details    Set Read Data Width for Command Phase, x = 0: No command phase
 *                                                        1: 8 bits
 *                                                        2: 16 bits
 *                                                        3: 24 bits
 *                                                        4: 32 bits
 * \hideinitializer
 */
#define SPIM_PHDMAR_CMD_WIDTH(spim, x)                 \
    do                                                 \
    {                                                  \
        spim->PHDMAR &= ~(SPIM_PHDMAR_DW_CMD_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_DW_CMD_Pos); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Enable Bit for Command Phase
 * \hideinitializer
 */
#define SPIM_PHDMAR_CMD_DTR_ENABLE(spim)             \
    do                                               \
    {                                                \
        (spim->PHDMAR |= (SPIM_PHDMAR_DTR_CMD_Msk)); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Disable Bit for Command Phase
 * \hideinitializer
 */
#define SPIM_PHDMAR_CMD_DTR_DISABLE(spim)             \
    do                                                \
    {                                                 \
        (spim->PHDMAR &= ~(SPIM_PHDMAR_DTR_CMD_Msk)); \
    } while (0)

/**
 * @details    Set SPI Interface Bit Mode for Command Phase, x = 0: Standard Mode
 *                                                               1: Dual Mode
 *                                                               2: Quad Mode
 *                                                               3: Octal Mode
 * \hideinitializer
 */
#define SPIM_PHDMAR_CMD_BIT_MODE(spim, x)              \
    do                                                 \
    {                                                  \
        spim->PHDMAR &= ~(SPIM_PHDMAR_BM_CMD_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_BM_CMD_Pos); \
    } while (0)

/**
 * @details    Set Data Width for Address Phase, x = 0: No command phase
 *                                                   1: 8 bits
 *                                                   2: 16 bits
 *                                                   3: 24 bits
 *                                                   4: 32 bits
 * \hideinitializer
 */
#define SPIM_PHDMAR_ADDR_WIDTH(spim, x)                 \
    do                                                  \
    {                                                   \
        spim->PHDMAR &= ~(SPIM_PHDMAR_DW_ADDR_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_DW_ADDR_Pos); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Enable Bit for Address Phase
 * \hideinitializer
 */
#define SPIM_PHDMAR_ADDR_DTR_ENABLE(spim)             \
    do                                                \
    {                                                 \
        (spim->PHDMAR |= (SPIM_PHDMAR_DTR_ADDR_Msk)); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Disable Bit for Address Phase
 * \hideinitializer
 */
#define SPIM_PHDMAR_ADDR_DTR_DISABLE(spim)             \
    do                                                 \
    {                                                  \
        (spim->PHDMAR &= ~(SPIM_PHDMAR_DTR_ADDR_Msk)); \
    } while (0)

/**
 * @details    Set SPI Interface Bit Mode for Address Phase, x = 0: Standard Mode
 *                                                               1: Dual Mode
 *                                                               2: Quad Mode
 *                                                               3: Octal Mode
 * \hideinitializer
 */
#define SPIM_PHDMAR_ADDR_BIT_MODE(spim, x)              \
    do                                                  \
    {                                                   \
        spim->PHDMAR &= ~(SPIM_PHDMAR_BM_ADDR_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_BM_ADDR_Pos); \
    } while (0)

/**
 * @details    Set Data Width for Mode Phase, x = 0: No command phase
 *                                                1: 8 bits
 *                                                2: 16 bits
 *                                                3: 24 bits
 *                                                4: 32 bits
 * \hideinitializer
 */
#define SPIM_PHDMAR_READ_DATA_WIDTH(spim, x)            \
    do                                                  \
    {                                                   \
        spim->PHDMAR &= ~(SPIM_PHDMAR_DW_MODE_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_DW_MODE_Pos); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Enable Bit for Read Mode Phase
 * \hideinitializer
 */
#define SPIM_PHDMAR_READ_DTR_ENABLE(spim)             \
    do                                                \
    {                                                 \
        (spim->PHDMAR |= (SPIM_PHDMAR_DTR_MODE_Msk)); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Disable Bit for Read Mode Phase
 * \hideinitializer
 */
#define SPIM_PHDMAR_READ_DTR_DISABLE(spim)             \
    do                                                 \
    {                                                  \
        (spim->PHDMAR &= ~(SPIM_PHDMAR_DTR_MODE_Msk)); \
    } while (0)

/**
 * @details    Set SPI Interface Bit Mode for Read Mode Phase, x = 0: Standard Mode
 *                                                                 1: Dual Mode
 *                                                                 2: Quad Mode
 *                                                                 3: Octal Mode
 * \hideinitializer
 */
#define SPIM_PHDMAR_READ_BIT_MODE(spim, x)              \
    do                                                  \
    {                                                   \
        spim->PHDMAR &= ~(SPIM_PHDMAR_BM_MODE_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_BM_MODE_Pos); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Enable Bit for Data Phase
 * \hideinitializer
 */
#define SPIM_PHDMAR_DATA_DTR_ENABLE(spim)             \
    do                                                \
    {                                                 \
        (spim->PHDMAR |= (SPIM_PHDMAR_DTR_DATA_Msk)); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode DIsable Bit for Data Phase
 * \hideinitializer
 */
#define SPIM_PHDMAR_DATA_DTR_DISABLE(spim)             \
    do                                                 \
    {                                                  \
        (spim->PHDMAR &= ~(SPIM_PHDMAR_DTR_DATA_Msk)); \
    } while (0)

/**
 * @details    Set Read DQS Mode Enable Bit for Data Phase
 * \hideinitializer
 */
#define SPIM_PHDMAR_DATA_RDQS_ENABLE(spim)             \
    do                                                 \
    {                                                  \
        (spim->PHDMAR |= (SPIM_PHDMAR_RDQS_DATA_Msk)); \
    } while (0)

/**
 * @details    Set Read DQS Mode Disable Bit for Data Phase
 * \hideinitializer
 */
#define SPIM_PHDMAR_DATA_RDQS_DISABLE(spim)             \
    do                                                  \
    {                                                   \
        (spim->PHDMAR &= ~(SPIM_PHDMAR_RDQS_DATA_Msk)); \
    } while (0)

/**
 * @details    Set Received Data Byte Order of Received Data Phase for Octal SPI Flash
 *             x = 0: Byte order byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7.
 *                 1: Byte order byte7, byte6, byte5, byte4, byte3, byte2, byte1, byte0.
 *                 2: Byte order byte1, byte0, byte3, byte2, byte5, byte4, byte7, byte6.
 *                 3: Byte order byte6, byte7, byte4, byte5, byte2, byte3, byte0, byte1.
 * \hideinitializer
 */
#define SPIM_PHDMAR_RBO_DATA(spim, x)                    \
    do                                                   \
    {                                                    \
        spim->PHDMAR &= ~(SPIM_PHDMAR_RBO_DATA_Msk);     \
        spim->PHDMAR |= (x << SPIM_PHDMAR_RBO_DATA_Pos); \
    } while (0)

/**
 * @details    Set SPI Interface Bit Mode for Data Phase, x = 0: Standard Mode
 *                                                            1: Dual Mode
 *                                                            2: Quad Mode
 *                                                            3: Octal Mode
 * \hideinitializer
 */
#define SPIM_PHDMAR_DATA_BIT_MODE(spim, x)              \
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
 * @details    Set DMM Mode Data Width for Command Phase, x = 0: No command phase
 *                                                            1: 8 bits
 *                                                            2: 16 bits
 *                                                            3: 24 bits
 *                                                            4: 32 bits
 * \hideinitializer
 */
#define SPIM_PHDMM_CMD_WIDTH(spim, x)                \
    do                                               \
    {                                                \
        spim->PHDMM &= ~(SPIM_PHDMM_DW_CMD_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_DW_CMD_Pos); \
    } while (0)

/**
 * @details    Set DMM Mode Double Transfer Rate Mode Enable Bit for Command Phase
 * \hideinitializer
 */
#define SPIM_PHDMM_CMD_DTR_ENABLE(spim)            \
    do                                             \
    {                                              \
        (spim->PHDMM |= (SPIM_PHDMM_DTR_CMD_Msk)); \
    } while (0)

/**
 * @details    Set DMM Mode Double Transfer Rate Mode Enable Bit for Command Phase
 * \hideinitializer
 */
#define SPIM_PHDMM_CMD_DTR_DISABLE(spim)            \
    do                                              \
    {                                               \
        (spim->PHDMM &= ~(SPIM_PHDMM_DTR_CMD_Msk)); \
    } while (0)

/**
 * @details    Set SPI Interface Bit Mode for Command Phase, x = 0: Standard Mode
 *                                                               1: Dual Mode
 *                                                               2: Quad Mode
 *                                                               3: Octal Mod
 * \hideinitializer
 */
#define SPIM_PHDMM_CMD_BIT_MODE(spim, x)             \
    do                                               \
    {                                                \
        spim->PHDMM &= ~(SPIM_PHDMM_BM_CMD_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_BM_CMD_Pos); \
    } while (0)

/**
 * @details    Set Data Width for Address Phase, x = 0: No command phase
 *                                                   1: 8 bits
 *                                                   2: 16 bits
 *                                                   3: 24 bits
 *                                                   4: 32 bits
 * \hideinitializer
 */
#define SPIM_PHDMM_ADDR_WIDTH(spim, x)                \
    do                                                \
    {                                                 \
        spim->PHDMM &= ~(SPIM_PHDMM_DW_ADDR_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_DW_ADDR_Pos); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Enable Bit for Address Phase
 * \hideinitializer
 */
#define SPIM_PHDMM_ADDR_DTR_ENABLE(spim)            \
    do                                              \
    {                                               \
        (spim->PHDMM |= (SPIM_PHDMM_DTR_ADDR_Msk)); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Disable Bit for Address Phase
 * \hideinitializer
 */
#define SPIM_PHDMM_ADDR_DTR_DISABLE(spim)            \
    do                                               \
    {                                                \
        (spim->PHDMM &= ~(SPIM_PHDMM_DTR_ADDR_Msk)); \
    } while (0)

/**
 * @details    Set SPI Interface Bit Mode for Address Phase, x = 0: Standard Mode
 *                                                               1: Dual Mode
 *                                                               2: Quad Mode
 *                                                               3: Octal Mod
 * \hideinitializer
 */
#define SPIM_PHDMM_ADDR_BIT_MODE(spim, x)             \
    do                                                \
    {                                                 \
        spim->PHDMM &= ~(SPIM_PHDMM_BM_ADDR_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_BM_ADDR_Pos); \
    } while (0)

/**
 * @details    Set Data Width for Mode Phase, x = 0: No command phase
 *                                                1: 8 bits
 *                                                2: 16 bits
 *                                                3: 24 bits
 *                                                4: 32 bits
 * \hideinitializer
 */
#define SPIM_PHDMM_READ_DATA_WIDTH(spim, x)           \
    do                                                \
    {                                                 \
        spim->PHDMM &= ~(SPIM_PHDMM_DW_MODE_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_DW_MODE_Pos); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Enable Bit for Mode Phase
 * \hideinitializer
 */
#define SPIM_PHDMM_READ_DTR_ENABLE(spim)            \
    do                                              \
    {                                               \
        (spim->PHDMM |= (SPIM_PHDMM_DTR_MODE_Msk)); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Disable Bit for Mode Phase
 * \hideinitializer
 */
#define SPIM_PHDMM_READ_DTR_DISABLE(spim)            \
    do                                               \
    {                                                \
        (spim->PHDMM &= ~(SPIM_PHDMM_DTR_MODE_Msk)); \
    } while (0)

/**
 * @details    Set SPI Interface Bit Mode for Mode Phase, x = 0: Standard Mode
 *                                                            1: Dual Mode
 *                                                            2: Quad Mode
 *                                                            3: Octal Mod
 * \hideinitializer
 */
#define SPIM_PHDMM_READ_BIT_MODE(spim, x)             \
    do                                                \
    {                                                 \
        spim->PHDMM &= ~(SPIM_PHDMM_BM_MODE_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_BM_MODE_Pos); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Enable Bit for Data Phase
 * \hideinitializer
 */
#define SPIM_PHDMM_DATA_DTR_ENABLE(spim)            \
    do                                              \
    {                                               \
        (spim->PHDMM |= (SPIM_PHDMM_DTR_DATA_Msk)); \
    } while (0)

/**
 * @details    Set Double Transfer Rate Mode Disable Bit for Data Phase
 * \hideinitializer
 */
#define SPIM_PHDMM_DATA_DTR_DISABLE(spim)            \
    do                                               \
    {                                                \
        (spim->PHDMM &= ~(SPIM_PHDMM_DTR_DATA_Msk)); \
    } while (0)

/**
 * @details    Set Read DQS Mode Enable Bit for Data Phase
 * \hideinitializer
 */
#define SPIM_PHDMM_DATA_RDQS_ENABLE(spim)            \
    do                                               \
    {                                                \
        (spim->PHDMM |= (SPIM_PHDMM_RDQS_DATA_Msk)); \
    } while (0)

/**
 * @details    Set Read DQS Mode Disable Bit for Data Phase
 * \hideinitializer
 */
#define SPIM_PHDMM_DATA_RDQS_DISABLE(spim)            \
    do                                                \
    {                                                 \
        (spim->PHDMM &= ~(SPIM_PHDMM_RDQS_DATA_Msk)); \
    } while (0)

/**
 * @details    Set Received Data Byte Order of Received Data Phase for Octal SPI Flash
 *             x = 0: Byte order byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7.
 *                 1: Byte order byte7, byte6, byte5, byte4, byte3, byte2, byte1, byte0.
 *                 2: Byte order byte1, byte0, byte3, byte2, byte5, byte4, byte7, byte6.
 *                 3: Byte order byte6, byte7, byte4, byte5, byte2, byte3, byte0, byte1.
 * \hideinitializer
 */
#define SPIM_PHDMM_RBO_DATA(spim, x)                   \
    do                                                 \
    {                                                  \
        spim->PHDMM &= ~(SPIM_PHDMM_RBO_DATA_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_RBO_DATA_Pos); \
    } while (0)

/**
 * @details    Set SPI Interface Bit Mode for Data Phase, x = 0: No command phase
 *                                                            1: 8 bits
 *                                                            2: 16 bits
 *                                                            3: 24 bits
 *                                                            4: 32 bits
 * \hideinitializer
 */
#define SPIM_PHDMM_DATA_BIT_MODE(spim, x)             \
    do                                                \
    {                                                 \
        spim->PHDMM &= ~(SPIM_PHDMM_BM_DATA_Msk);     \
        spim->PHDMM |= (x << SPIM_PHDMM_BM_DATA_Pos); \
    } while (0)

/*----------------------------------------------------------------------------*/
/* SPIM_DLLx constant definitions                                            */
/*----------------------------------------------------------------------------*/
/**
 * @details    Set DLL0 OLDO Enable Bit, 0: Disable, 1: Enable
 * \hideinitializer
 */
#define SPIM_ENABLE_DLL0_OLDO(spim, x)                               \
    do                                                              \
    {                                                               \
        spim->DLL0 &= ~(SPIM_DLL0_DLLOLDO_Msk);                     \
        spim->DLL0 |= (((x) ? 1UL : 0UL) << SPIM_DLL0_DLLOLDO_Pos); \
    } while (0)

/**
 * @details    Set DLL0 Output Valid Counter Reset
 * \hideinitializer
 */
#define SPIM_ENABLE_DLL0_OVRST(spim, x)                              \
    do                                                               \
    {                                                                \
        spim->DLL0 &= ~(SPIM_DLL0_DLLOVRST_Msk);                     \
        spim->DLL0 |= (((x) ? 1UL : 0UL) << SPIM_DLL0_DLLOVRST_Pos); \
    } while (0)

/**
 * @details    Wait DLL0 Output Valid Counter Reset Done
 * \hideinitializer
 */
#define SPIM_WAIT_DLL0_OVRST(spim)       ((spim->DLL0 & SPIM_DLL0_DLLOVRST_Msk) >> SPIM_DLL0_DLLOVRST_Pos)

/**
 * @details    Wait DLL0 Clock Divider Circuit Status Bit
 * \hideinitializer
 */
#define SPIM_WAIT_DLL0_CLKON(spim)  ((spim->DLL0 & SPIM_DLL0_DLLCLKON_Msk) >> SPIM_DLL0_DLLCLKON_Pos)

/**
 * @details    Wait DLL0 Lock Status Bit
 * \hideinitializer
 */
#define SPIM_WAIT_DLL0_LOCK(spim)        ((spim->DLL0 & SPIM_DLL0_DLLLOCK_Msk) >> SPIM_DLL0_DLLLOCK_Pos)

/**
 * @details    Wait DLL0 Output Ready Status
 * \hideinitializer
 */
#define SPIM_WAIT_DLL0_READY(spim)       ((spim->DLL0 & SPIM_DLL0_DLLREADY_Msk) >> SPIM_DLL0_DLLREADY_Pos)

/**
 * @details    Wait DLL0 Refresh Status Bit
 * \hideinitializer
 */
#define SPIM_WAIT_DLL0_REFRESH(spim)     ((spim->DLL0 & SPIM_DLL0_DLL_REF_Msk) >> SPIM_DLL0_DLL_REF_Pos)

/**
 * @details    Set DLL0 Delay Step Number 0 ~ 1F
 * \hideinitializer
 */
#define SPIM_SET_DLL0_DELAY_NUM(spim, x)               \
    do                                                 \
    {                                                  \
        spim->DLL0 &= ~(SPIM_DLL0_DLL_DNUM_Msk);       \
        spim->DLL0 |= ((x) << SPIM_DLL0_DLL_DNUM_Pos); \
    } while (0)

/**
 * @details    Set Cycle Number of between DLL Lock and DLL Output Valid 0 ~ 0xFFFF, Default 0x1388
 * \hideinitializer
 */
#define SPIM_SET_DLL1_OUT_VALID(spim, x)               \
    do                                                 \
    {                                                  \
        spim->DLL1 &= ~(SPIM_DLL1_DLLOVNUM_Msk);       \
        spim->DLL1 |= ((x) << SPIM_DLL1_DLLOVNUM_Pos); \
    } while (0)

/**
 * @details    Get Cycle Number of DLL1 Ouput Valid 0 ~ 0xFFFF, Default 0x07D0
 * \hideinitializer
 */
#define SPIM_GET_DLL1_LOCKK_VALID(spim)   \
    ((spim->DLL1 & SPIM_DLL1_DLLLKNUM_Msk) >> SPIM_DLL1_DLLLKNUM_Pos)

/**
 * @details    Set Cycle Number between DLL Clock Divider Enable and DLL Lock Valid 0 ~ 0xFFFF, Default 0x07D0
 * \hideinitializer
 */
#define SPIM_SET_DLL1_LOCKK_VALID(spim, x)             \
    do                                                 \
    {                                                  \
        spim->DLL1 &= ~(SPIM_DLL1_DLLLKNUM_Msk);       \
        spim->DLL1 |= ((x) << SPIM_DLL1_DLLLKNUM_Pos); \
    } while (0)

/**
 * @details    Get Cycle Number between DLL Clock Divider Enable and DLL Lock Valid 0 ~ 0xFFFF, Default 0x07D0
 * \hideinitializer
 */
#define SPIM_GET_DLL1_LOCKK_VALID(spim)   \
    ((spim->DLL1 & SPIM_DLL1_DLLLKNUM_Msk) >> SPIM_DLL1_DLLLKNUM_Pos)

/**
 * @details    Get Cycle Number of DLL1 Ouput Valid 0 ~ 0xFFFF, Default 0x07D0
 * \hideinitializer
 */
#define SPIM_GET_DLL1_OUT_VALID(spim)     (spim->DLL1 & SPIM_DLL1_DLLOVNUM_Msk)

/**
 * @details    Set Cycle Number of DLL1 Lock Time 0 ~ 0xFFFF, Default 0x1388
 * \hideinitializer
 */
#define SPIM_SET_DLL1_LOCK_TIME(spim, x)               \
    do                                                 \
    {                                                  \
        spim->DLL1 &= ~(SPIM_DLL1_DLLLKNUM_Msk);       \
        spim->DLL1 |= ((x) << SPIM_DLL1_DLLLKNUM_Pos); \
    } while (0)

/**
 * @details    Set Cycle Number of between DLL OLDO Enable and DLL Clock Divider Enable Time 0 ~ 0xFFFF, Default 0x07D0
 * \hideinitializer
 */
#define SPIM_SET_DLL2_CLKON_NUM(spim, x)               \
    do                                                 \
    {                                                  \
        spim->DLL2 &= ~(SPIM_DLL2_CLKONNUM_Msk);       \
        spim->DLL2 |= ((x) << SPIM_DLL2_CLKONNUM_Pos); \
    } while (0)

/**
 * @details    Set Cycle Number of between DLL Output Valid and DLL Auto Trim Enable Time 0 ~ 0xFFFF, Default 0x07D0
 * \hideinitializer
 */
#define SPIM_SET_DLL2_TRIM_NUM(spim, x)               \
    do                                                 \
    {                                                  \
        spim->DLL2 &= ~(SPIM_DLL2_TRIMNUM_Msk);       \
        spim->DLL2 |= ((x) << SPIM_DLL2_TRIMNUM_Pos); \
    } while (0)

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
/** @addtogroup SPIM HyperBus Interface Driver
  @{
*/


/** @addtogroup SPIM HYPER_EXPORTED_CONSTANTS HYPER Exported Constants
  @{
*/

/*----------------------------------------------------------------------------*/
/* Hyper Chip Register Space constant definitions
Register Space Range:
   0x0000_0000 = Identification Register 0
   0x0000_0002 = Identification Register 1
   0x0000_1000 = Configuration Register 0
   0x0000_1002 = Configuration Register 1
*/
#define HYPER_CHIP_ID_REG0            (0x00000000)
#define HYPER_CHIP_ID_REG1            (0x00000002)
#define HYPER_CHIP_CONFIG_REG0        (0x00001000)
#define HYPER_CHIP_CONFIG_REG1        (0x00001002)

/*----------------------------------------------------------------------------*/
/* SPIM HYPER_CMD constant definitions
    0001 = Reset Hyper Chip
    0010 = Read Hyper Chip regsiter (16-Bit, Read Data[15:0]
    0101 = Exit From Hybrid Sleep and deep power down
    0111 = Write Hyper Chip regsiter (16-Bit, Write Data[15:0]
    1000 = Read 1 word (Read Data[15:0]) from Hyper Chip
    1001 = Read 2 word (Read Data[31:0]) from Hyper Chip
    1100 = Write 1 Byte (Write Data[7:0]) to Hyper Chip
    1101 = Write 2 Bytes (Write Data[15:0]) to Hyper Chip
    1110 = Write 3 Byte (Write Data[23:0]) to Hyper Chip
    1111 = Write 4 Byte (Write Data[31:0]) to Hyper Chip
*/
/*----------------------------------------------------------------------------*/
#define SPIM_HYPER_CMD_HRAM_IDLE            (0x00000)
#define SPIM_HYPER_CMD_RESET_HRAM           (0x00001)
#define SPIM_HYPER_CMD_READ_HRAM_REGISTER   (0x00002)
#define SPIM_HYPER_CMD_EXIT_HS_PD           (0x00005)
#define SPIM_HYPER_CMD_WRITE_HRAM_REGISTER  (0x00007)
#define SPIM_HYPER_CMD_READ_HRAM_1_WORD     (0x00008)  // 16bit
#define SPIM_HYPER_CMD_READ_HRAM_2_WORD     (0x00009)  // 32bit
#define SPIM_HYPER_CMD_WRITE_HRAM_1_BYTE    (0x0000C)
#define SPIM_HYPER_CMD_WRITE_HRAM_2_BYTE    (0x0000D)
#define SPIM_HYPER_CMD_WRITE_HRAM_3_BYTE    (0x0000E)
#define SPIM_HYPER_CMD_WRITE_HRAM_4_BYTE    (0x0000F)

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
  * @brief      Set Hyper Chip Select Setup Time to Next CK Rising Edge
  * @param[in]  x Chip Select Setup Time to Next CK Rising Edge.
  *               - \ref SPIM_HYPER_CONFIG1_CSST_1_5_HCLK : 1.5 HCLK cycles
  *               - \ref SPIM_HYPER_CONFIG1_CSST_2_5_HCLK : 2.5 HCLK cycles
  *               - \ref SPIM_HYPER_CONFIG1_CSST_3_5_HCLK : 3.5 HCLK cycles
  *               - \ref SPIM_HYPER_CONFIG1_CSST_4_5_HCLK : 4.5 HCLK cycles
  * @return     None
  * @details    This Macro Set Hyper Chip Select Setup Time to Next CK Rising Edge
  * \hideinitializer
  */
#define SPIM_HYPER_CONFIG1_SET_CSST(spim, x)                       \
    do                                                             \
    {                                                              \
        spim->HYPER_CONFIG1 &= ~(SPIM_HYPER_CONFIG1_CSST_Msk);     \
        spim->HYPER_CONFIG1 |= (x << SPIM_HYPER_CONFIG1_CSST_Pos); \
    } while (0)

/**
  * @brief      Set Hyper Chip Select Hold Time After CK Falling Edge
  * @param[in]  x   Chip Select Hold Time After CK Falling Edge.
  *                 - \ref SPIM_HYPER_CONFIG1_CSH_0_5_HCLK : 0.5 HCLK cycles
  *                 - \ref SPIM_HYPER_CONFIG1_CSH_1_5_HCLK : 1.5 HCLK cycles
  *                 - \ref SPIM_HYPER_CONFIG1_CSH_2_5_HCLK : 2.5 HCLK cycles
  *                 - \ref SPIM_HYPER_CONFIG1_CSH_3_5_HCLK : 3.5 HCLK cycles
  * @return     None
  * @details    This Macro Set Hyper Chip Select Hold Time After CK Falling Edge
  * \hideinitializer
  */
#define SPIM_HYPER_CONFIG1_SET_CSH(spim, x)                         \
    do                                                              \
    {                                                               \
        spim->HYPER_CONFIG1 &= ~(SPIM_HYPER_CONFIG1_CSH_Msk);       \
        spim->HYPER_CONFIG1 |= ((x) << SPIM_HYPER_CONFIG1_CSH_Pos); \
    } while (0)

/**
  * @brief      Set Hyper Chip Select High between Transaction
  * @param[in]  x     Set Chip Select High between Transaction as u8Value HCLK cycles
                            u8Value must be 2 ~ 16
  * @return     None
  * @details    This Macro Set Hyper Chip Select High between Transaction.
  * \hideinitializer
  */
#define SPIM_HYPER_CONFIG1_SET_CSHI(spim, x)                              \
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
  * @brief      Set Hyper Chip Select Maximum Low Time
  * @param[in]  u32CsMaxLT      Set Hyper Chip Select Maximum Low Time as u32CsMaxLT HCLK cycles.
                                u32CsMaxLT must be 1 ~ 2048
  * @return     None
  * @details    This Macro Set Hyper Chip Select Maximum Low Time.
  * \hideinitializer
  */
#define SPIM_HYPER_CONFIG1_SET_CSMAXLT(spim, u32CsMaxLT)                             \
    do                                                                               \
    {                                                                                \
        spim->HYPER_CONFIG1 &= ~(SPIM_HYPER_CONFIG1_CSMAXLT_Msk);                    \
        spim->HYPER_CONFIG1 |= ((u32CsMaxLT - 1) << SPIM_HYPER_CONFIG1_CSMAXLT_Pos); \
    } while (0)

/**
  * @brief      Get Hyper Chip Select Maximum Low Time
  * \hideinitializer
  */
#define SPIM_HYPER_CONFIG1_GET_CSMAXLT(spim)    \
        (((spim->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSMAXLT_Msk) >> SPIM_HYPER_CONFIG1_CSMAXLT_Pos) + 1UL)

/**
  * @brief      Set Hyper Chip Initial Read Access Time
  * @param[in]  x   Initial Access Time 1 ~ 31 Clock Cycle.
  * @return     None
  * @details    This Macro Set Hyper Chip Initial Access Time
  * \hideinitializer
  */
#define SPIM_HYPER_CONFIG2_SET_ACCTWR(spim, x)                       \
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
#define SPIM_HYPER_CONFIG2_GET_ACCTWR(spim) \
            ((spim->HYPER_CONFIG2 & SPIM_HYPER_CONFIG2_ACCTWR_Msk) >> SPIM_HYPER_CONFIG2_ACCTWR_Pos)

/**
  * @brief      Set Hyper Device RESETN Low Time
  * @param[in]  u8Value   Initial Device RESETN Low Time 0 ~ 255.
  * @return     None
  * @details    This Macro Set Hyper Device RESETN Low Time
  * \hideinitializer
  */
#define SPIM_HYPER_CONFIG2_SET_RSTNLT(spim, u8Value)                       \
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
#define SPIM_HYPER_CONFIG2_GET_RSTNLT(spim) \
            ((spim->HYPER_CONFIG2 & SPIM_HYPER_CONFIG2_RSTNLT_Msk) >> SPIM_HYPER_CONFIG2_RSTNLT_Pos)

/**
  * @brief      Set Hyper Chip Initial Read Access Time
  * @param[in]  x   Initial Access Time 1 ~ 31 Clock Cycle.
  * @return     None
  * @details    This Macro Set Hyper Chip Initial Access Time
  * \hideinitializer
  */
#define SPIM_HYPER_CONFIG2_SET_ACCTRD(spim, x)                       \
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
#define SPIM_HYPER_CONFIG2_GET_ACCTRD(spim) \
            ((spim->HYPER_CONFIG2 & SPIM_HYPER_CONFIG2_ACCTRD_Msk) >> SPIM_HYPER_CONFIG2_ACCTRD_Pos)

/**
 * @brief      Enable Hyper Chip Operation Done Interrupt
 * @param[in]  None
 * @return     None
 * @details    This macro enable Hyper Chip Operation Done Interrupt.
 * \hideinitializer
 */
#define SPIM_HYPER_ENABLE_INT(spim)                        \
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
#define SPIM_HYPER_DISABLE_INT(spim)                        \
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
#define SPIM_HYPER_GET_INT(spim)    ((spim->HYPER_INTEN & SPIM_HYPER_INTEN_OPINTEN_Msk) >> SPIM_HYPER_INTEN_OPINTEN_Pos)

/**
 * @brief      Get Hyper Chip Operation Done Interrupt
 * @param[in]  None
 * @return     0 = Hyper Chip operation is busy.
 *             1 = Hyper Chip operation is done.
 * @details    This macro Get Hyper Chip Operation Done Interrupt.
 * \hideinitializer
 */
#define SPIM_HYPER_GET_INTSTS(spim)    while(spim->HYPER_INTEN & SPIM_HYPER_INTEN_OPINTEN_Msk)

/** @} end of group SPIM_EXPORTED_CONSTANTS */

/** @addtogroup SPIM_EXPORTED_FUNCTIONS SPIM Exported Functions
  @{
*/

/*----------------------------------------------------------------------------*/
/* SPIM Define Function Prototypes                                            */
/*----------------------------------------------------------------------------*/
int SPIM_InitFlash(SPIM_T *spim, int clrWP);
uint32_t SPIM_GetSClkFreq(SPIM_T *spim);
void SPIM_ReadJedecId(SPIM_T *spim, uint8_t idBuf[], uint32_t u32NRx, uint32_t u32NBit);
int SPIM_Enable_4Bytes_Mode(SPIM_T *spim, int isEn, uint32_t u32NBit);
int SPIM_Is4ByteModeEnable(SPIM_T *spim, uint32_t u32NBit);
int SPIM_SetWrapAroundEnable(SPIM_T *spim, int isEn, uint32_t u32WaBit);
int SPIM_Wait_Write_Done(SPIM_T *spim, uint32_t u32NBit);
void SPIM_Set_Write_Enable(SPIM_T *spim, int isEn, uint32_t u32NBit);

void SPIM_ReadConfigRegister(SPIM_T *spim, uint8_t u8RdCMD, uint32_t u32Addr, uint8_t dataBuf[], uint32_t u32NRx, uint32_t u32NBit);
void SPIM_WriteConfigRegister(SPIM_T *spim, uint8_t u8CMD, uint32_t u32Addr, uint8_t u8Data, uint32_t u32NBit);

void SPIM_ChipErase(SPIM_T *spim, uint32_t u32NBit, int isSync);
void SPIM_EraseBlock(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint8_t u8ErsCmd, uint32_t u32NBit, int isSync);

void SPIM_IO_Write(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint32_t u32NTx, uint8_t pu8TxBuf[], uint8_t wrCmd, uint32_t u32NBitCmd, uint32_t u32NBitAddr, uint32_t u32NBitDat);
void SPIM_IO_Read(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint32_t u32NRx, uint8_t pu8RxBuf[], uint8_t rdCmd, uint32_t u32NBitCmd, uint32_t u32NBitAddr, uint32_t u32NBitDat, int u32NDummy,uint32_t u32DTROn);

void SPIM_DMA_Write(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint32_t u32NTx, uint8_t pu8TxBuf[], uint32_t wrCmd);
int32_t SPIM_DMA_Read(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint32_t u32NRx, uint8_t pu8RxBuf[], uint32_t u32RdCmd, int isSync);

void SPIM_EnterDirectMapMode(SPIM_T *spim, int is4ByteAddr, uint32_t u32RdCmd, uint32_t u32IdleIntvl);
int SPIM_ExitDirectMapMode(SPIM_T *spim);

void SPIM_SetQuadEnable(SPIM_T *spim, int isEn, uint32_t u32NBit);

void SPIM_WinbondUnlock(SPIM_T *spim, uint32_t u32NBit);

void SPIM_DMAM_ClearPhaseConfig(SPIM_T *spim, uint32_t u32OPMode);
void SPIM_DMAM_SetCmdPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32NBit, uint32_t u32Width, uint32_t u32DTREn);
void SPIM_DMAM_SetAddrPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32NBit, uint32_t u32Width, uint32_t u32DTREn);
void SPIM_DMAM_SetContReadPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32NBit, uint32_t u32Width, uint32_t u32DTREn);
void SPIM_DMAM_SetDataPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32NBit, uint32_t u32ByteOrder, uint32_t u32RdDQS, uint32_t u32DTREn);

void SPIM_IO_CMDPhase(SPIM_T *spim, uint32_t u32RdCMD, uint32_t u32CmdPhase, uint32_t u32DTRMode, uint32_t isSync);
void SPIM_IO_AddrPhase(SPIM_T *spim, uint32_t u32Is4ByteAddr, uint32_t u32Addr, uint32_t u32AddrPhase, uint32_t u32DTRMode);
void SPIM_IO_DCPhase(SPIM_T *spim, uint32_t u32NDummy, uint32_t u32DcPhase, uint32_t u32DTRMode);
void SPIM_IO_DataPhase(SPIM_T *spim, uint8_t u8TRxBuf[], uint32_t u32TRxSize, uint32_t u32DataPhae, uint32_t u32DTRMode, uint32_t isSync);
uint32_t SPIM_GetIOPhaseSize(uint32_t u32Phase);
int SPIM_WaitSPIMENDone(SPIM_T *spim, uint32_t u32IsSync);

void SPIM_ODTRReadJedecId(SPIM_T *spim, uint8_t idBuf[], uint32_t u32NRx, uint32_t u32NBit, uint32_t u32DTRMode);
void SPIM_MT35x_4Bytes_Enable(SPIM_T *spim, int isEn, uint32_t u32NBit, uint32_t u32DTRMode);

/*----------------------------------------------------------------------------*/
/* SPIM Hyper Define Function Prototypes                                      */
/*----------------------------------------------------------------------------*/
int SPIM_Hyper_GetErrCode(void);
void SPIM_Hyper_SetErrCode(int i32SetValue);
void SPIM_ForceDLLDelayTime(SPIM_T *spim, uint32_t u32OutValid, uint32_t u32LockTime, uint32_t u32ClkOnNum, uint32_t u32DelayNum);
void SPIM_Hyper_Config(SPIM_T *spim, uint32_t u32CSMaxLow, uint32_t u32AcctRD, uint32_t u32AcctWR);

/* HyperRAM */
int SPIM_Hyper_ResetHyperRAM(SPIM_T *spim);
int SPIM_HyperRAM_ExitHSAndDPD(SPIM_T *spim);
int SPIM_HyperRAM_ReadReg(SPIM_T *spim, uint32_t u32Addr);
int SPIM_HyperRAM_WriteReg(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Value);

uint16_t SPIM_Hyper_Read1Word(SPIM_T *spim, uint32_t u32Addr);
uint32_t SPIM_Hyper_Read2Word(SPIM_T *spim, uint32_t u32Addr);
int SPIM_Hyper_Write1Byte(SPIM_T *spim, uint32_t u32Addr, uint8_t u8Data);
int SPIM_Hyper_Write2Byte(SPIM_T *spim, uint32_t u32Addr, uint16_t u16Data);
int SPIM_Hyper_Write3Byte(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Data);
int SPIM_Hyper_Write4Byte(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Data);

int SPIM_Hyper_DMAWrite(SPIM_T *spim, uint32_t u32Addr, void *pvWrBuf, uint32_t u32NTx);
int SPIM_Hyper_DMARead(SPIM_T *spim, uint32_t u32Addr, void *pvRdBuf, uint32_t u32NRx);

int SPIM_Hyper_EnterDirectMapMode(SPIM_T *spim);
void SPIM_Hyper_ExitDirectMapMode(SPIM_T *spim);
int SPIM_Hyper_WaitDMMDone(SPIM_T *spim);

void SPIM_Hyper_DefConfig(SPIM_T *spim, uint32_t u32CSMaxLow, uint32_t u32AcctRD, uint32_t u32AcctWR);

/*----------------------------------------------------------------------------*/
/* SPIM DLL Define Function Prototypes                                      */
/*----------------------------------------------------------------------------*/
void SPIM_CtrlDLLDelayTime(SPIM_T *spim, uint32_t u32ClkOnNum, uint32_t u32TrimNum, uint32_t u32LKNum, uint32_t u32OVNum, uint32_t u32DelayNum);

/** @} end of group SPIM_EXPORTED_FUNCTIONS */
/** @} end of group SPIM_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __SPIM_H__ */
