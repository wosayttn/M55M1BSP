/**************************************************************************//**
 * @file    spim_hyper.h
 * @version V1.00
 * @brief   SPIM_HYPER driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#ifndef __SPIM_HYPER_H__
#define __SPIM_HYPER_H__

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

/** @addtogroup SPI_HYPER_Driver SPI_HYPER Driver
  @{
*/

/** @addtogroup SPI_HYPER_EXPORTED_CONSTANTS SPI_HYPER Exported Constants
  @{
*/

#define SPIM0_HYPER_DMM_SADDR               (0x80000000UL)  /*!< SPIM0 DMM mode memory map base secure address    \hideinitializer */
#define SPIM0_HYPER_DMM_NSADDR              (0x90000000UL)  /*!< SPIM1 DMM mode memory map base non secure address    \hideinitializer */

#define SPIM1_HYPER_DMM_SADDR               (0x82000000UL)  /*!< SPIM1 DMM mode memory map base secure address    \hideinitializer */
#define SPIM1_HYPER_DMM_NSADDR              (0x92000000UL)  /*!< SPIM1 DMM mode memory map base non secure address    \hideinitializer */

#if defined (SCU_INIT_D0PNS2_VAL) && (SCU_INIT_D0PNS2_VAL & SCU_D0PNS2_SPIM0_Msk)
#define SPIM0_HYPER_DMM_ADDR                SPIM0_HYPER_DMM_NSADDR
#else
#define SPIM0_HYPER_DMM_ADDR                SPIM0_HYPER_DMM_SADDR
#endif //

#if defined (SCU_INIT_D0PNS2_VAL) && (SCU_INIT_D0PNS2_VAL & SCU_D0PNS2_SPIM1_Msk)
#define SPIM1_HYPER_DMM_ADDR                SPIM1_HYPER_DMM_NSADDR
#else
#define SPIM1_HYPER_DMM_ADDR                SPIM1_HYPER_DMM_SADDR
#endif //

#define SPIM_HYPER_DMM_SIZE                 (0x2000000UL)       /*!< DMM mode memory mapping size        \hideinitializer */

#define SPIM_HYPER_MAX_LATENCY              (0x05)              /*!< Maximum DLL training number        \hideinitializer */

#define SPIM_HYPER_ENABLE                   (0x01UL)            /* SPIM_HYPER Operation Enable */
#define SPIM_HYPER_DISABLE                  (0x00UL)            /* SPIM_HYPER Operation Disable */

#define SPIM_HYPER_CACHE_EN                 (0)                 /*!< SPIM_HYPER cache on/off    \hideinitializer */

/* SPIM_HYPER Wait State Timeout Counter. */
#define SPIM_HYPER_TIMEOUT                  SystemCoreClock /*!< SPIM_HYPER time-out counter (1 second time-out) */

/** @endcond HIDDEN_SYMBOLS */

/* SPIM_HYPER Define Error Code */
#define SPIM_HYPER_OK                       ( 0L)   /*!< SPIM_HYPER operation OK */
#define SPIM_HYPER_ERR_FAIL                 (-1L)   /*!< SPIM_HYPER operation failed */
#define SPIM_HYPER_ERR_TIMEOUT              (-2L)   /*!< SPIM_HYPER operation abort due to timeout error */

/* HyperRAM Register Address */
#define HYPER_RAM_ID_REG0                   (0x00000000)    /* Hyper RAM Identification Register 0. */
#define HYPER_RAM_ID_REG1                   (0x00000002)    /* Hyper RAM Identification Register 1. */
#define HYPER_RAM_CONFIG_REG0               (0x00001000)    /* Hyper RAM Configuration Register 0. */
#define HYPER_RAM_CONFIG_REG1               (0x00001002)    /* Hyper RAM Configuration Register 1. */

/* SPIM Hyper Mode Command */
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

/** @} end of group SPIM_HYPER_EXPORTED_CONSTANTS */


/** @addtogroup SPIM_HYPER_EXPORTED_FUNCTIONS SPI Exported Constants
  @{
*/
/*----------------------------------------------------------------------------*/
/*  Define Macros and functions                                               */
/*----------------------------------------------------------------------------*/
/**
 * @brief   Enable cipher.
 * \hideinitializer
 */
#define SPIM_HYPER_ENABLE_CIPHER(spim)                                  \
    do                                                                  \
    {                                                                   \
        (spim->CTL0 &= ~SPIM_CTL0_CIPHOFF_Msk);                         \
        spim->DMMCTL = (spim->DMMCTL & ~(SPIM_DMMCTL_DESELTIM_Msk)) |   \
                       (((0x12) & 0x1FUL) << SPIM_DMMCTL_DESELTIM_Pos); \
        spim->CTL0 |= SPIM_CTL0_BALEN_Msk;                              \
    } while (0)

/**
 * @brief   Disable cipher.
 * \hideinitializer
 */
#define SPIM_HYPER_DISABLE_CIPHER(spim)                                \
    do                                                                 \
    {                                                                  \
        (spim->CTL0 |= SPIM_CTL0_CIPHOFF_Msk);                         \
        spim->DMMCTL = (spim->DMMCTL & ~SPIM_DMMCTL_DESELTIM_Msk) |    \
                       (((0x8) & 0x1FUL) << SPIM_DMMCTL_DESELTIM_Pos); \
        spim->CTL0 &= ~(SPIM_CTL0_BALEN_Msk);                          \
    } while (0)

/**
 * @brief       Enable Hyper Device Mode.
 * \hideinitializer
 */
#define SPIM_HYPER_ENABLE_HYPMODE(spim)   \
    (spim->CTL0 = (spim->CTL0 & ~(SPIM_CTL0_HYPER_EN_Msk)) | (SPIM_CTL0_HYPER_EN_Msk))

/**
 * @brief       Set operation mode.
 * @param[in]   x   SPI Function Operation Mode
 *                  - \ref SPIM_CTL0_OPMODE_IO
 *                  - \ref SPIM_CTL0_OPMODE_PAGEWRITE
 *                  - \ref SPIM_CTL0_OPMODE_PAGEREAD
 *                  - \ref SPIM_CTL0_OPMODE_DIRECTMAP
 * \hideinitializer
 */
#define SPIM_HYPER_SET_OPMODE(spim, x)    \
    (spim->CTL0 = (spim->CTL0 & ~(SPIM_CTL0_OPMODE_Msk)) | (x))

/**
 * @brief   Get operation mode.
 * \hideinitializer
 */
#define SPIM_HYPER_GET_OPMODE(spim)   \
    ((spim->CTL0 & SPIM_CTL0_OPMODE_Msk) >> SPIM_CTL0_OPMODE_Pos)

/**
 * @brief   Start operation.
 * \hideinitializer
 */
#define SPIM_HYPER_SET_GO(spim)   (spim->CTL1 |= SPIM_CTL1_SPIMEN_Msk)

/**
 * @brief   Is engine busy.
 * \hideinitializer
 */
#define SPIM_HYPER_IS_BUSY(spim)  \
    ((spim->CTL1 & SPIM_CTL1_SPIMEN_Msk) >> SPIM_CTL1_SPIMEN_Pos)

#if (SPIM_HYPER_CACHE_EN == 1) // TESTCHIP_ONLY not support
/**
 * @brief   Enable cache.
 * \hideinitializer
 */
#define SPIM_HYPER_ENABLE_CACHE(spim) (spim->CTL1 &= ~(SPIM_CTL1_CACHEOFF_Msk))

/**
 * @brief   Disable cache.
 * \hideinitializer
 */
#define SPIM_HYPER_DISABLE_CACHE(spim)    (spim->CTL1 |= SPIM_CTL1_CACHEOFF_Msk)

/**
 * @brief   Is cache enabled.
 * \hideinitializer
 */
//#define SPIM_HYPER_IS_CACHE_EN(spim)    ((spim->CTL1 & SPIM_CTL1_CACHEOFF_Msk) ? 0 : 1)

/**
 * @brief   Invalidate cache.
 * \hideinitializer
 */
#define SPIM_HYPER_INVALID_CACHE(spim)    (spim->CTL1 |= SPIM_CTL1_CDINVAL_Msk)

#endif //SPIM_HYPER_CACHE_EN

/**
 * @brief       Set SPIM clock divider.
 * @param[in]   x   Clock Divider Register
 *                  SPI Flash For DTR commands
 *                  - \ref only 1, 2, 4, 8, 16, 32,….
 *                  Hyper Device Mode
 *                  - \ref only support 1 or 2
 * \hideinitializer
 */
#define SPIM_HYPER_SET_CLKDIV(spim, x) \
    (spim->CTL1 = (spim->CTL1 & ~(SPIM_CTL1_DIVIDER_Msk)) | ((x) << SPIM_CTL1_DIVIDER_Pos))

/**
 * @brief   Stop DMM mode Transfer.
 * \hideinitializer
 */
#define SPIM_HYPER_ENABLE_DMMDONE(spim)   (spim->DMMCTL |= SPIM_DMMCTL_HYPDONE_Msk)

/**
 * @brief   Get DMM mode complete to stop TX/RX.
 * \hideinitializer
 */
#define SPIM_HYPER_GET_DMMDONE(spim)  \
    ((spim->DMMCTL & SPIM_DMMCTL_HYPDONE_Msk) >> SPIM_DMMCTL_HYPDONE_Pos)

/*----------------------------------------------------------------------------*/
/* SPIM_DLLx constant definitions                                            */
/*----------------------------------------------------------------------------*/
/**
 * @brief   Set DLL0 OLDO Enable Bit
 * @param x is DLL circuit power mode.
 *          - \ref SPIM_HYPER_ENABLE
 *          - \ref SPIM_HYPER_DISABLE
 * \hideinitializer
 */
#define SPIM_HYPER_ENABLE_DLL0OLDO(spim, x)  \
    (spim->DLL0 = (spim->DLL0 & ~(SPIM_DLL0_DLLOLDO_Msk)) | (((x) ? 1UL : 0UL) << SPIM_DLL0_DLLOLDO_Pos))

/**
 * @brief   Set DLL0 Output Valid Counter Reset.
 * @param x is starts to count from 0x0 to DLLOVNUM
 *          - \ref SPIM_HYPER_ENABLE
 *          - \ref SPIM_HYPER_DISABLE
 * \hideinitializer
 */
#define SPIM_HYPER_ENABLE_DLL0OVRST(spim, x) \
    (spim->DLL0 = (spim->DLL0 & ~(SPIM_DLL0_DLLOVRST_Msk)) | (((x) ? 1UL : 0UL) << SPIM_DLL0_DLLOVRST_Pos))

/**
 * @brief   Get DLL0 Output Valid Counter Reset Done.
 * \hideinitializer
 */
#define SPIM_HYPER_GET_DLL0OVRST(spim)  \
    ((spim->DLL0 & SPIM_DLL0_DLLOVRST_Msk) >> SPIM_DLL0_DLLOVRST_Pos)

/**
 * @brief   Get DLL0 Clock Divider Circuit Status Bit.
 * \hideinitializer
 */
#define SPIM_HYPER_GET_DLL0CLKON(spim)  \
    ((spim->DLL0 & SPIM_DLL0_DLLCLKON_Msk) >> SPIM_DLL0_DLLCLKON_Pos)

/**
 * @brief   Get DLL0 Lock Status Bit.
 * \hideinitializer
 */
#define SPIM_HYPER_GET_DLL0LOCK(spim)   \
    ((spim->DLL0 & SPIM_DLL0_DLLLOCK_Msk) >> SPIM_DLL0_DLLLOCK_Pos)

/**
 * @brief   Get DLL0 Output Ready Status.
 * \hideinitializer
 */
#define SPIM_HYPER_GET_DLL0READY(spim)  \
    ((spim->DLL0 & SPIM_DLL0_DLLREADY_Msk) >> SPIM_DLL0_DLLREADY_Pos)

/**
 * @brief   Get DLL0 Refresh Status Bit.
 * \hideinitializer
 */
#define SPIM_HYPER_GET_DLL0REF(spim)    \
    ((spim->DLL0 & SPIM_DLL0_DLL_REF_Msk) >> SPIM_DLL0_DLL_REF_Pos)

/**
 * @brief   Set DLL0 Delay Step Number. It could be 0 ~ 0x1F.
 * \hideinitializer
 */
#define SPIM_HYPER_SET_DLL0DNUM(spim, x)    \
    (spim->DLL0 = (spim->DLL0 & ~(SPIM_DLL0_DLL_DNUM_Msk)) | ((x) << SPIM_DLL0_DLL_DNUM_Pos))

/**
 * @brief   Set Cycle Number of between DLL Lock and DLL Output Valid.
 *          It could be 0 ~ 0xFFFF.
 * \hideinitializer
 */
#define SPIM_HYPER_SET_DLL1OVNUM(spim, x)    \
    (spim->DLL1 = (spim->DLL1 & ~(SPIM_DLL1_DLLOVNUM_Msk)) | ((x) << SPIM_DLL1_DLLOVNUM_Pos))

/**
 * @brief   Get Cycle Number of between DLL Lock and DLL Output Valid.
 * \hideinitializer
 */
#define SPIM_HYPER_GET_DLL1OVNUM(spim) \
    ((spim->DLL1 & SPIM_DLL1_DLLOVNUM_Msk) >> SPIM_DLL1_DLLOVNUM_Pos)

/**
 * @brief   Set Cycle Number between DLL Clock Divider Enable and DLL Lock Valid.
 *          It could be 0 ~ 0xFFFF.
 * \hideinitializer
 */
#define SPIM_HYPER_SET_DLL1LKNUM(spim, x)  \
    (spim->DLL1 = (spim->DLL1 & ~(SPIM_DLL1_DLLLKNUM_Msk)) | ((x) << SPIM_DLL1_DLLLKNUM_Pos))

/**
 * @brief   Get Cycle Number between DLL Clock Divider Enable and DLL Lock Valid.
 * \hideinitializer
 */
#define SPIM_HYPER_GET_DLL1LKNUM(spim) \
    ((spim->DLL1 & SPIM_DLL1_DLLLKNUM_Msk) >> SPIM_DLL1_DLLLKNUM_Pos)

/**
 * @brief   Set Cycle Number of between DLL Output Valid and DLL Auto Trim Enable Time.
 *          It could be 0 ~ 0xFFFF.
 * \hideinitializer
 */
#define SPIM_HYPER_SET_DLL2TRIMNUM(spim, x) \
    (spim->DLL2 = (spim->DLL2 & ~(SPIM_DLL2_TRIMNUM_Msk)) | ((x) << SPIM_DLL2_TRIMNUM_Pos))

/**
 * @brief   Set Cycle Number of between DLL OLDO Enable and DLL Clock Divider Enable Time.
 *          It could be 0 ~ 0xFFFF.
 * \hideinitializer
 */
#define SPIM_HYPER_SET_DLL2CLKONNUM(spim, x)    \
    (spim->DLL2 = (spim->DLL2 & ~(SPIM_DLL2_CLKONNUM_Msk)) | ((x) << SPIM_DLL2_CLKONNUM_Pos))

/**
  * @brief      Set Hyper Chip Select Setup Time to Next CK Rising Edge.
  * @param[in]  x   Chip Select Setup Time to Next CK Rising Edge.
  *                 - \ref SPIM_HYPER_CONFIG1_CSST_1_5_HCLK : 1.5 HCLK cycles
  *                 - \ref SPIM_HYPER_CONFIG1_CSST_2_5_HCLK : 2.5 HCLK cycles
  *                 - \ref SPIM_HYPER_CONFIG1_CSST_3_5_HCLK : 3.5 HCLK cycles
  *                 - \ref SPIM_HYPER_CONFIG1_CSST_4_5_HCLK : 4.5 HCLK cycles
  * \hideinitializer
  */
#define SPIM_HYPER_SET_CFG1CSST(spim, x)    \
    (spim->HYPER_CONFIG1 = (spim->HYPER_CONFIG1 & ~(SPIM_HYPER_CONFIG1_CSST_Msk)) | \
                           ((x) << SPIM_HYPER_CONFIG1_CSST_Pos))

/**
  * @brief      Set Hyper Chip Select Hold Time After CK Falling Edge.
  * @param[in]  x   Chip Select Hold Time After CK Falling Edge.
  *                 - \ref SPIM_HYPER_CONFIG1_CSH_0_5_HCLK : 0.5 HCLK cycles
  *                 - \ref SPIM_HYPER_CONFIG1_CSH_1_5_HCLK : 1.5 HCLK cycles
  *                 - \ref SPIM_HYPER_CONFIG1_CSH_2_5_HCLK : 2.5 HCLK cycles
  *                 - \ref SPIM_HYPER_CONFIG1_CSH_3_5_HCLK : 3.5 HCLK cycles
  * \hideinitializer
  */
#define SPIM_HYPER_SET_CFG1CSH(spim, x) \
    (spim->HYPER_CONFIG1 = (spim->HYPER_CONFIG1 & ~(SPIM_HYPER_CONFIG1_CSH_Msk)) | \
                           ((x) << SPIM_HYPER_CONFIG1_CSH_Pos))

/**
  * @brief      Set Hyper Chip Select High between Transaction.
  * @param[in]  x   Set Chip Select High between Transaction as u8Value HCLK cycles.
                    It could be 2 ~ 16.
  * \hideinitializer
  */
#define SPIM_HYPER_SET_CFG1CSHI(spim, x)    \
    (spim->HYPER_CONFIG1 = (spim->HYPER_CONFIG1 & ~(SPIM_HYPER_CONFIG1_CSHI_Msk)) | \
                           ((x) << SPIM_HYPER_CONFIG1_CSHI_Pos))

/**
  * @brief      Set Hyper Chip Select Maximum Low Time.
  * @param[in]  u32CsMaxLT  Set Hyper Chip Select Maximum Low Time as u32CsMaxLT HCLK cycles.
  *                         It could be 1 ~ 2048.
  * \hideinitializer
  */
#define SPIM_HYPER_SET_CFG1CSMAXLT(spim, u32CsMaxLT)   \
    (spim->HYPER_CONFIG1 = (spim->HYPER_CONFIG1 & ~(SPIM_HYPER_CONFIG1_CSMAXLT_Msk)) | \
                           (((u32CsMaxLT) - 1) << SPIM_HYPER_CONFIG1_CSMAXLT_Pos))
/**
  * @brief  Get Hyper Chip Select Maximum Low Time.
  * \hideinitializer
  */
#define SPIM_HYPER_GET_CFG1CSMAXLT(spim)    \
    (((spim->HYPER_CONFIG1 & SPIM_HYPER_CONFIG1_CSMAXLT_Msk) >> SPIM_HYPER_CONFIG1_CSMAXLT_Pos) + 1UL)

/**
  * @brief  Set Hyper Chip Initial Read Access Time.
  * @param[in]  x   Initial Access Time. It could be 1 ~ 31.
  * \hideinitializer
  */
#define SPIM_HYPER_SET_CFG2ACCTWR(spim, x)  \
    (spim->HYPER_CONFIG2 = (spim->HYPER_CONFIG2 & ~(SPIM_HYPER_CONFIG2_ACCTWR_Msk)) | \
                           ((x) << SPIM_HYPER_CONFIG2_ACCTWR_Pos))

/**
  * @brief  Get Hyper Chip Initial Read Access Time.
  * \hideinitializer
  */
#define SPIM_HYPER_GET_CFG2ACCTWR(spim) \
    ((spim->HYPER_CONFIG2 & SPIM_HYPER_CONFIG2_ACCTWR_Msk) >> SPIM_HYPER_CONFIG2_ACCTWR_Pos)

/**
  * @brief      Set Hyper Device RESETN Low Time.
  * @param[in]  u8Value Initial Device RESETN Low Time. It could be 0 ~ 255.
  * \hideinitializer
  */
#define SPIM_HYPER_SET_CFG2RSTNLT(spim, u8Value)    \
    (spim->HYPER_CONFIG2 = (spim->HYPER_CONFIG2 & ~(SPIM_HYPER_CONFIG2_RSTNLT_Msk)) |\
                           ((u8Value) << SPIM_HYPER_CONFIG2_RSTNLT_Pos))

/**
  * @brief  Get Hyper Device RESETN Low Time.
  * \hideinitializer
  */
#define SPIM_HYPER_GET_CFG2RSTNLT(spim) \
    ((spim->HYPER_CONFIG2 & SPIM_HYPER_CONFIG2_RSTNLT_Msk) >> SPIM_HYPER_CONFIG2_RSTNLT_Pos)

/**
  * @brief      Set Hyper Chip Initial Read Access Time.
  * @param[in]  x   Initial Access Time. It could be 1 ~ 31.
  * \hideinitializer
  */
#define SPIM_HYPER_SET_CFG2ACCTRD(spim, x)  \
    (spim->HYPER_CONFIG2 = (spim->HYPER_CONFIG2 & ~(SPIM_HYPER_CONFIG2_ACCTRD_Msk)) |   \
                           ((x) << SPIM_HYPER_CONFIG2_ACCTRD_Pos))

/**
  * @brief  Get Hyper Chip Initial Read Access Time.
  * \hideinitializer
  */
#define SPIM_HYPER_GET_CFG2ACCTRD(spim) \
    ((spim->HYPER_CONFIG2 & SPIM_HYPER_CONFIG2_ACCTRD_Msk) >> SPIM_HYPER_CONFIG2_ACCTRD_Pos)

/**
 * @brief Clear Hyper Bus Write DATA
 * \hideinitializer
 */
#define SPIM_HYPER_CLEAR_WDATA(spim)    (spim->HYPER_WDATA &= ~(0xFFFFFFFF))

/**
 * @brief   Enable Hyper Chip Operation Done Interrupt.
 * \hideinitializer
 */
#define SPIM_HYPER_ENABLE_INT(spim) (spim->HYPER_INTEN |= SPIM_HYPER_INTEN_OPINTEN_Msk)

/**
 * @brief   Disable Hyper Chip Operation Done Interrupt.
 * \hideinitializer
 */
#define SPIM_HYPER_DISABLE_INT(spim)    (spim->HYPER_INTEN &= ~(SPIM_HYPER_INTEN_OPINTEN_Msk))

/**
 * @brief   Get Hyper Bus Operation Done Interrupt.
 * \hideinitializer
 */
#define SPIM_HYPER_GET_INT(spim)    \
    ((spim->HYPER_INTEN & SPIM_HYPER_INTEN_OPINTEN_Msk) >> SPIM_HYPER_INTEN_OPINTEN_Pos)

/**
 * @brief   Get Hyper Chip Operation Done Interrupt.
 * \hideinitializer
 */
#define SPIM_HYPER_GET_INTSTS(spim) \
    ((spim->HYPER_INTEN & SPIM_HYPER_INTEN_OPINTEN_Msk) >> SPIM_HYPER_INTEN_OPINTEN_Pos)


/* Octal SPI flash and hyper device training DLL API */
int32_t SPIM_HYPER_CtrlDLLDelayTime(SPIM_T *spim, uint32_t u32DelayNum);
uint32_t SPIM_HYPER_GetDMMAddress(SPIM_T *spim);

/* HyperRAM */
int32_t SPIM_HYPER_ExitHSAndDPD(SPIM_T *spim);
int32_t SPIM_HYPER_ReadHyperRAMReg(SPIM_T *spim, uint32_t u32Addr);
int32_t SPIM_HYPER_WriteHyperRAMReg(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Value);

/* Hyper Device API */
void SPIM_HYPER_Init(SPIM_T *spim, uint32_t u32Div);
int32_t SPIM_HYPER_Reset(SPIM_T *spim);
int16_t SPIM_HYPER_Read1Word(SPIM_T *spim, uint32_t u32Addr);
int32_t SPIM_HYPER_Read2Word(SPIM_T *spim, uint32_t u32Addr);
int32_t SPIM_HYPER_Write1Byte(SPIM_T *spim, uint32_t u32Addr, uint8_t u8Data);
int32_t SPIM_HYPER_Write2Byte(SPIM_T *spim, uint32_t u32Addr, uint16_t u16Data);
int32_t SPIM_HYPER_Write3Byte(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Data);
int32_t SPIM_HYPER_Write4Byte(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Data);

int32_t SPIM_HYPER_DMAWrite(SPIM_T *spim, uint32_t u32Addr, uint8_t *pu8WrBuf, uint32_t u32NTx);
int32_t SPIM_HYPER_DMARead(SPIM_T *spim, uint32_t u32Addr, uint8_t *pu8RdBuf, uint32_t u32NRx);

void SPIM_HYPER_EnterDirectMapMode(SPIM_T *spim);
void SPIM_HYPER_ExitDirectMapMode(SPIM_T *spim);
int32_t SPIM_HYPER_IsDMMDone(SPIM_T *spim);

/** @} end of group SPIM_HYPER_EXPORTED_FUNCTIONS */
/** @} end of group SPIM_HYPER_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __SPIM_HYPER_H__ */