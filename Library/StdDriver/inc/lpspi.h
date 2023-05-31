/**************************************************************************//**
 * @file     lpspi.h
 * @version  V3.00
 * @brief    M2L31 series LPSPI driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2016-2020 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#ifndef __LPSPI_H__
#define __LPSPI_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup LPSPI_Driver LPSPI Driver
  @{
*/

/** @addtogroup LPSPI_EXPORTED_CONSTANTS LPSPI Exported Constants
  @{
*/

#define LPSPI_MODE_0                        (LPSPI_CTL_TXNEG_Msk)                             /*!< CLKPOL=0; RXNEG=0; TXNEG=1 \hideinitializer */
#define LPSPI_MODE_1                        (LPSPI_CTL_RXNEG_Msk)                             /*!< CLKPOL=0; RXNEG=1; TXNEG=0 \hideinitializer */
#define LPSPI_MODE_2                        (LPSPI_CTL_CLKPOL_Msk | LPSPI_CTL_RXNEG_Msk)      /*!< CLKPOL=1; RXNEG=1; TXNEG=0 \hideinitializer */
#define LPSPI_MODE_3                        (LPSPI_CTL_CLKPOL_Msk | LPSPI_CTL_TXNEG_Msk)      /*!< CLKPOL=1; RXNEG=0; TXNEG=1 \hideinitializer */

#define LPSPI_SLAVE                         (LPSPI_CTL_SLAVE_Msk)             /*!< Set as slave \hideinitializer */
#define LPSPI_MASTER                        (0x0U)                                            /*!< Set as master \hideinitializer */

#define LPSPI_SS                            (LPSPI_SSCTL_SS_Msk)              /*!< Set SS \hideinitializer */
#define LPSPI_SS_ACTIVE_HIGH                (LPSPI_SSCTL_SSACTPOL_Msk)        /*!< SS active high \hideinitializer */
#define LPSPI_SS_ACTIVE_LOW                 (0x0U)                            /*!< SS active low \hideinitializer */

/* LPSPI Interrupt Mask */
#define LPSPI_UNIT_INT_Pos                  (0)
#define LPSPI_UNIT_INT_MASK                 (0x1ul << LPSPI_UNIT_INT_Pos)     /*!< Unit transfer interrupt mask \hideinitializer */

#define LPSPI_SSACT_INT_Pos                 (1)
#define LPSPI_SSACT_INT_MASK                (0x1ul << LPSPI_SSACT_INT_Pos)    /*!< Slave selection signal active interrupt mask \hideinitializer */

#define LPSPI_SSINACT_INT_Pos               (2)
#define LPSPI_SSINACT_INT_MASK              (0x1ul << LPSPI_SSINACT_INT_Pos)  /*!< Slave selection signal inactive interrupt mask \hideinitializer */

#define LPSPI_SLVUR_INT_Pos                 (3)
#define LPSPI_SLVUR_INT_MASK                (0x1ul << LPSPI_SLVUR_INT_Pos)    /*!< Slave under run interrupt mask \hideinitializer */

#define LPSPI_SLVBE_INT_Pos                 (4)
#define LPSPI_SLVBE_INT_MASK                (0x1ul << LPSPI_SLVBE_INT_Pos)    /*!< Slave bit count error interrupt mask \hideinitializer */

#define LPSPI_TXUF_INT_Pos                  (5)
#define LPSPI_TXUF_INT_MASK                 (0x1ul << LPSPI_TXUF_INT_Pos)     /*!< Slave TX underflow interrupt mask \hideinitializer */

#define LPSPI_FIFO_TXTH_INT_Pos             (6)
#define LPSPI_FIFO_TXTH_INT_MASK            (0x1ul << LPSPI_FIFO_TXTH_INT_Pos)  /*!< FIFO TX threshold interrupt mask \hideinitializer */

#define LPSPI_FIFO_RXTH_INT_Pos             (7)
#define LPSPI_FIFO_RXTH_INT_MASK            (0x1ul << LPSPI_FIFO_RXTH_INT_Pos)  /*!< FIFO RX threshold interrupt mask \hideinitializer */

#define LPSPI_FIFO_RXOV_INT_Pos             (8)
#define LPSPI_FIFO_RXOV_INT_MASK            (0x1ul << LPSPI_FIFO_RXOV_INT_Pos)  /*!< FIFO RX overrun interrupt mask \hideinitializer */

#define LPSPI_FIFO_RXTO_INT_Pos             (9)
#define LPSPI_FIFO_RXTO_INT_MASK            (0x1ul << LPSPI_FIFO_RXTO_INT_Pos)  /*!< FIFO RX time-out interrupt mask \hideinitializer */

/* LPSPI Status Mask */
#define LPSPI_BUSY_Pos                      (0)
#define LPSPI_BUSY_MASK                     (0x1ul << LPSPI_BUSY_Pos)         /*!< Busy status mask \hideinitializer */

#define LPSPI_RX_EMPTY_Pos                  (1)
#define LPSPI_RX_EMPTY_MASK                 (0x1ul << LPSPI_RX_EMPTY_Pos)     /*!< RX empty status mask \hideinitializer */

#define LPSPI_RX_FULL_Pos                   (2)
#define LPSPI_RX_FULL_MASK                  (0x1ul << LPSPI_RX_FULL_Pos)      /*!< RX full status mask \hideinitializer */

#define LPSPI_TX_EMPTY_Pos                  (3)
#define LPSPI_TX_EMPTY_MASK                 (0x1ul << LPSPI_TX_EMPTY_Pos)     /*!< TX empty status mask \hideinitializer */

#define LPSPI_TX_FULL_Pos                   (4)
#define LPSPI_TX_FULL_MASK                  (0x1ul << LPSPI_TX_FULL_Pos)      /*!< TX full status mask \hideinitializer */

#define LPSPI_TXRX_RESET_Pos                (5)
#define LPSPI_TXRX_RESET_MASK               (0x1ul << LPSPI_TXRX_RESET_Pos)   /*!< TX or RX reset status mask \hideinitializer */

#define LPSPI_SPIEN_STS_Pos                 (6)
#define LPSPI_SPIEN_STS_MASK                (0x1ul << LPSPI_SPIEN_STS_Pos)    /*!< SPIEN status mask \hideinitializer */

#define LPSPI_SSLINE_STS_Pos                (7)
#define LPSPI_SSLINE_STS_MASK               (0x1ul << LPSPI_SSLINE_STS_Pos)   /*!< SPIx_SS line status mask \hideinitializer */

/* LPSPI Auto Trigger Source */
#define LPSPI_TRIGGER_LPTMER0               (0x00UL)
#define LPSPI_TRIGGER_LPTMER1               (0x01UL)
#define LPSPI_TRIGGER_TTMR0                 (0x02UL)
#define LPSPI_TRIGGER_TTMR1                 (0x03UL)
#define LPSPI_TRIGGER_GPIO0                 (0x04UL)
#define LPSPI_TRIGGER_GPIO1                 (0x05UL)
#define LPSPI_TRIGGER_GPIO2                 (0x06UL)
#define LPSPI_TRIGGER_GPIO3                 (0x07UL)

/*@}*/ /* end of group LPSPI_EXPORTED_CONSTANTS */


/** @addtogroup LPSPI_EXPORTED_FUNCTIONS LPSPI Exported Functions
  @{
*/

/**
  * @brief      Clear the unit transfer interrupt flag.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Write 1 to UNITIF bit of LPSPI_STATUS register to clear the unit transfer interrupt flag.
  * \hideinitializer
  */
#define LPSPI_CLR_UNIT_TRANS_INT_FLAG(lpspi)  \
  do                                          \
  {                                           \
    lpspi->STATUS |= LPSPI_STATUS_UNITIF_Msk; \
  } while (0)

/**
  * @brief      Trigger RX PDMA function.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Set RXPDMAEN bit of LPSPI_PDMACTL register to enable RX PDMA transfer function.
  * \hideinitializer
  */
#define LPSPI_TRIGGER_RX_PDMA(lpspi)              \
  do                                              \
  {                                               \
    lpspi->PDMACTL |= LPSPI_PDMACTL_RXPDMAEN_Msk; \
  } while (0)

/**
  * @brief      Trigger TX PDMA function.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Set TXPDMAEN bit of LPSPI_PDMACTL register to enable TX PDMA transfer function.
  * \hideinitializer
  */
#define LPSPI_TRIGGER_TX_PDMA(lpspi)              \
  do                                              \
  {                                               \
    lpspi->PDMACTL |= LPSPI_PDMACTL_TXPDMAEN_Msk; \
  } while (0)

/**
  * @brief      Trigger TX and RX PDMA function.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Set TXPDMAEN bit and RXPDMAEN bit of LPSPI_PDMACTL register to enable TX and RX PDMA transfer function.
  * \hideinitializer
  */
#define LPSPI_TRIGGER_TX_RX_PDMA(lpspi)             \
  do                                                \
  {                                                 \
    lpspi->PDMACTL |= (LPSPI_PDMACTL_TXPDMAEN_Msk | \
                       LPSPI_PDMACTL_RXPDMAEN_Msk); \
  } while (0)

/**
  * @brief      Disable RX PDMA transfer.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Clear RXPDMAEN bit of LPSPI_PDMACTL register to disable RX PDMA transfer function.
  * \hideinitializer
  */
#define LPSPI_DISABLE_RX_PDMA(lpspi)                 \
  do                                                 \
  {                                                  \
    lpspi->PDMACTL &= ~(LPSPI_PDMACTL_RXPDMAEN_Msk); \
  } while (0)

/**
  * @brief      Disable TX PDMA transfer.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Clear TXPDMAEN bit of LPSPI_PDMACTL register to disable TX PDMA transfer function.
  * \hideinitializer
  */
#define LPSPI_DISABLE_TX_PDMA(lpspi)                 \
  do                                                 \
  {                                                  \
    lpspi->PDMACTL &= ~(LPSPI_PDMACTL_TXPDMAEN_Msk); \
  } while (0)

/**
  * @brief      Disable TX and RX PDMA transfer.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Clear TXPDMAEN bit and RXPDMAEN bit of LPSPI_PDMACTL register to disable TX and RX PDMA transfer function.
  * \hideinitializer
  */
#define LPSPI_DISABLE_TX_RX_PDMA(lpspi)              \
  do                                                 \
  {                                                  \
    lpspi->PDMACTL &= ~(LPSPI_PDMACTL_TXPDMAEN_Msk | \
                        LPSPI_PDMACTL_RXPDMAEN_Msk); \
  } while (0)

/**
  * @brief      Get the count of available data in RX FIFO.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     The count of available data in RX FIFO.
  * @details    Read RXCNT (LPSPI_STATUS[27:24]) to get the count of available data in RX FIFO.
  * \hideinitializer
  */
#define LPSPI_GET_RX_FIFO_COUNT(lpspi)  ((lpspi->STATUS & LPSPI_STATUS_RXCNT_Msk) >> LPSPI_STATUS_RXCNT_Pos)

/**
  * @brief      Get the RX FIFO empty flag.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @retval     0 RX FIFO is not empty.
  * @retval     1 RX FIFO is empty.
  * @details    Read RXEMPTY bit of LPSPI_STATUS register to get the RX FIFO empty flag.
  * \hideinitializer
  */
#define LPSPI_GET_RX_FIFO_EMPTY_FLAG(lpspi) ((lpspi->STATUS & LPSPI_STATUS_RXEMPTY_Msk) >> LPSPI_STATUS_RXEMPTY_Pos)

/**
  * @brief      Get the TX FIFO empty flag.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @retval     0 TX FIFO is not empty.
  * @retval     1 TX FIFO is empty.
  * @details    Read TXEMPTY bit of LPSPI_STATUS register to get the TX FIFO empty flag.
  * \hideinitializer
  */
#define LPSPI_GET_TX_FIFO_EMPTY_FLAG(lpspi) ((lpspi->STATUS & LPSPI_STATUS_TXEMPTY_Msk) >> LPSPI_STATUS_TXEMPTY_Pos)

/**
  * @brief      Get the TX FIFO full flag.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @retval     0 TX FIFO is not full.
  * @retval     1 TX FIFO is full.
  * @details    Read TXFULL bit of LPSPI_STATUS register to get the TX FIFO full flag.
  * \hideinitializer
  */
#define LPSPI_GET_TX_FIFO_FULL_FLAG(lpspi)  ((lpspi->STATUS & LPSPI_STATUS_TXFULL_Msk) >> LPSPI_STATUS_TXFULL_Pos)

/**
  * @brief      Get the datum read from RX register.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     Data in RX register.
  * @details    Read LPSPI_RX register to get the received datum.
  * \hideinitializer
  */
#define LPSPI_READ_RX(lpspi)  (lpspi->RX)

/**
  * @brief      Write datum to TX register.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @param[in]  u32TxData The datum which user attempt to transfer through LPSPI bus.
  * @return     None.
  * @details    Write u32TxData to LPSPI_TX register.
  * \hideinitializer
  */
#define LPSPI_WRITE_TX(lpspi, u32TxData) \
  do                                     \
  {                                      \
    lpspi->TX = (u32TxData);             \
  } while (0)

/**
  * @brief      Set SPIx_SS pin to high state.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Disable automatic slave selection function and set SPIx_SS pin to high state.
  * \hideinitializer
  */
#define LPSPI_SET_SS_HIGH(lpspi)                                     \
  do                                                                 \
  {                                                                  \
    lpspi->SSCTL &= ~(LPSPI_SSCTL_AUTOSS_Msk);                       \
    lpspi->SSCTL |= (LPSPI_SSCTL_SSACTPOL_Msk | LPSPI_SSCTL_SS_Msk); \
  } while (0)

/**
  * @brief      Set SPIx_SS pin to low state.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Disable automatic slave selection function and set SPIx_SS pin to low state.
  * \hideinitializer
  */
#define LPSPI_SET_SS_LOW(lpspi)                                           \
  do                                                                      \
  {                                                                       \
    lpspi->SSCTL &= ~(LPSPI_SSCTL_AUTOSS_Msk | LPSPI_SSCTL_SSACTPOL_Msk); \
    lpspi->SSCTL |= LPSPI_SSCTL_SS_Msk;                                   \
  } while (0)

/**
  * @brief      Enable Byte Reorder function.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Enable Byte Reorder function. The suspend interval depends on the setting of SUSPITV (LPSPI_CTL[7:4]).
  * \hideinitializer
  */
#define LPSPI_ENABLE_BYTE_REORDER(lpspi) \
  do                                     \
  {                                      \
    lpspi->CTL |= LPSPI_CTL_REORDER_Msk; \
  } while (0)

/**
  * @brief      Disable Byte Reorder function.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Clear REORDER bit field of LPSPI_CTL register to disable Byte Reorder function.
  * \hideinitializer
  */
#define LPSPI_DISABLE_BYTE_REORDER(lpspi)   \
  do                                        \
  {                                         \
    lpspi->CTL &= ~(LPSPI_CTL_REORDER_Msk); \
  } while (0)

/**
  * @brief      Set the length of suspend interval.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @param[in]  u32SuspCycle Decides the length of suspend interval. It could be 0 ~ 15.
  * @return     None.
  * @details    Set the length of suspend interval according to u32SuspCycle.
  *             The length of suspend interval is ((u32SuspCycle + 0.5) * the length of one LPSPI bus clock cycle).
  * \hideinitializer
  */
#define LPSPI_SET_SUSPEND_CYCLE(lpspi, u32SuspCycle)         \
  do                                                         \
  {                                                          \
    lpspi->CTL &= ~(LPSPI_CTL_SUSPITV_Msk);                  \
    lpspi->CTL |= ((u32SuspCycle) << LPSPI_CTL_SUSPITV_Pos); \
  } while (0)

/**
  * @brief      Set the LPSPI transfer sequence with LSB first.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Set LSB bit of LPSPI_CTL register to set the LPSPI transfer sequence with LSB first.
  * \hideinitializer
  */
#define LPSPI_SET_LSB_FIRST(lpspi)   \
  do                                 \
  {                                  \
    lpspi->CTL |= LPSPI_CTL_LSB_Msk; \
  } while (0)

/**
  * @brief      Set the LPSPI transfer sequence with MSB first.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Clear LSB bit of LPSPI_CTL register to set the LPSPI transfer sequence with MSB first.
  * \hideinitializer
  */
#define LPSPI_SET_MSB_FIRST(lpspi)      \
  do                                    \
  {                                     \
    lpspi->CTL &= ~(LPSPI_CTL_LSB_Msk); \
  } while (0)

/**
  * @brief      Set the data width of a LPSPI transaction.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @param[in]  u32Width The bit width of one transaction.
  * @return     None.
  * @details    The data width can be 8 ~ 32 bits.
  * \hideinitializer
  */
#define LPSPI_SET_DATA_WIDTH(lpspi, u32Width)                    \
  do                                                             \
  {                                                              \
    lpspi->CTL &= ~(LPSPI_CTL_DWIDTH_Msk);                       \
    (lpspi->CTL |= (((u32Width)&0x1F) << LPSPI_CTL_DWIDTH_Pos)); \
  } while (0)

/**
  * @brief      Get the LPSPI busy state.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @retval     0 LPSPI controller is not busy.
  * @retval     1 LPSPI controller is busy.
  * @details    This macro will return the busy state of LPSPI controller.
  * \hideinitializer
  */
#define LPSPI_IS_BUSY(lpspi)  ((lpspi->STATUS & LPSPI_STATUS_BUSY_Msk) >> LPSPI_STATUS_BUSY_Pos)

/**
  * @brief      Enable LPSPI controller.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Set SPIEN (LPSPI_CTL[0]) to enable LPSPI controller.
  * \hideinitializer
  */
#define LPSPI_ENABLE(lpspi)            \
  do                                   \
  {                                    \
    lpspi->CTL |= LPSPI_CTL_SPIEN_Msk; \
  } while (0)

/**
  * @brief      Disable LPSPI controller.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * @details    Clear SPIEN (LPSPI_CTL[0]) to disable LPSPI controller.
  * \hideinitializer
  */
#define LPSPI_DISABLE(lpspi)              \
  do                                      \
  {                                       \
    lpspi->CTL &= ~(LPSPI_CTL_SPIEN_Msk); \
  } while (0)

/**
  * @brief      Set Automatic Operation Trigger Source Select.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @param[in]  x Enable/Disable Auto Operation
  *               - \ref LPSPI_TRIGGER_LPTMER0
  *               - \ref LPSPI_TRIGGER_LPTMER1
  *               - \ref LPSPI_TRIGGER_TTMR0
  *               - \ref LPSPI_TRIGGER_TTMR1
  *               - \ref LPSPI_TRIGGER_GPIO0
  *               - \ref LPSPI_TRIGGER_GPIO1
  *               - \ref LPSPI_TRIGGER_GPIO2
  *               - \ref LPSPI_TRIGGER_GPIO3
  * @return     None.
  * \hideinitializer
  */
#define LPSPI_SET_TRIGSRC(lpspi, x)                     \
  do                                                    \
  {                                                     \
    lpspi->AUTOCTL &= ~(LPSPI_AUTOCTL_TRIGSEL_Msk);     \
    lpspi->AUTOCTL |= (x << LPSPI_AUTOCTL_TRIGSEL_Pos); \
  } while (0)

/**
* @brief      Get Automatic Operation Trigger Source Select.
* @param[in]  lpspi The pointer of the specified LPSPI module.
* @return     None.
* \hideinitializer
*/
#define LPSPI_GET_TRIGSRC(lpspi)  ((lpspi->AUTOCTL & LPSPI_AUTOCTL_TRIGSEL_Msk) >> LPSPI_AUTOCTL_TRIGSEL_Pos)

/**
  * @brief      Automatic Operation Trigger Enable.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @param[in]  x Enable/Disable Auto Operation
  *               - \ref 0 : Disable
  *               - \ref 1 : Enable
  * @return     None.
  * \hideinitializer
  */
#define LPSPI_AUTO_TRIGEN(lpspi, x)                    \
  do                                                   \
  {                                                    \
    lpspi->AUTOCTL &= ~(LPSPI_AUTOCTL_TRIGEN_Msk);     \
    lpspi->AUTOCTL |= (x << LPSPI_AUTOCTL_TRIGEN_Pos); \
  } while (0)

/**
  * @brief      TCNT Count Match Interrupt Enable
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @param[in]  x Enable/Disable TCNT Count Match Interrupt
  *               - \ref 0 : Disable
  *               - \ref 1 : Enable
  * @return     None.
  * \hideinitializer
  */
#define LPSPI_AUTO_TCNTINT(lpspi, x)                   \
  do                                                   \
  {                                                    \
    lpspi->AUTOCTL &= ~(LPSPI_AUTOCTL_CNTIEN_Msk);     \
    lpspi->AUTOCTL |= (x << LPSPI_AUTOCTL_CNTIEN_Pos); \
  } while (0)

/**
  * @brief      Full RX Data Acception Enable Bit.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @param[in]  x Enable/Disable Full RX Data
  *               - \ref 0 : Disable
  *               - \ref 1 : Enable
  * @return     None.
  * \hideinitializer
  */
#define LPSPI_AUTO_FULLRX(lpspi, x)                      \
  do                                                     \
  {                                                      \
    lpspi->AUTOCTL &= ~(LPSPI_AUTOCTL_FULLRXEN_Msk);     \
    lpspi->AUTOCTL |= (x << LPSPI_AUTOCTL_FULLRXEN_Pos); \
  } while (0)

/**
  * @brief      Slave Select Wake Up Enable Bit.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @param[in]  x Enable/Disable Slave Select Wake Up
  *               - \ref 0 : Disable
  *               - \ref 1 : Enable
  * @return     None.
  * \hideinitializer
  */
#define LPSPI_AUTO_SSWKEN(lpspi, x)                    \
  do                                                   \
  {                                                    \
    lpspi->AUTOCTL &= ~(LPSPI_AUTOCTL_SSWKEN_Msk);     \
    lpspi->AUTOCTL |= (x << LPSPI_AUTOCTL_SSWKEN_Pos); \
  } while (0)

/**
  * @brief      Automatic Operation Mode Enable Bit.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @param[in]  x Enable/Disable Automatic Operation Mode
  *               - \ref 0 : Disable
  *               - \ref 1 : Enable
  * @return     None.
  * \hideinitializer
  */
#define LPSPI_AUTO_EN(lpspi, x)                        \
  do                                                   \
  {                                                    \
    lpspi->AUTOCTL &= ~(LPSPI_AUTOCTL_AUTOEN_Msk);     \
    lpspi->AUTOCTL |= (x << LPSPI_AUTOCTL_AUTOEN_Pos); \
  } while (0)

/**
  * @brief      Software Trigger.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @param[in]  x Enable/Disable Software Trigger.
  *               - \ref 0 : Disable
  *               - \ref 1 : Enable
  * @return     None.
  * \hideinitializer
  */
#define LPSPI_AUTO_SWTRIG(lpspi, x)                   \
  do                                                  \
  {                                                   \
    lpspi->AUTOCTL &= ~(LPSPI_AUTOCTL_SWTRG_Msk);     \
    lpspi->AUTOCTL |= (x << LPSPI_AUTOCTL_SWTRG_Pos); \
  } while (0)

/**
  * @brief      TCNT Count Match Wake Up Enable Bit.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @param[in]  x Enable/Disable TCNT Count Match Wake Up.
  *               - \ref 0 : Disable
  *               - \ref 1 : Enable
  * @return     None.
  * \hideinitializer
  */
#define LPSPI_AUTO_TCNTWKEN(lpspi, x)                   \
  do                                                    \
  {                                                     \
    lpspi->AUTOCTL &= ~(LPSPI_AUTOCTL_CNTWKEN_Msk);     \
    lpspi->AUTOCTL |= (x << LPSPI_AUTOCTL_CNTWKEN_Pos); \
  } while (0)

/**
  * @brief      Set Auomatic Operation RX Transfer Count.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @param[in]  x Auomatic Operation RX Transfer Count 1 ~ 255.
  * @return     None.
  * \hideinitializer
  */
#define LPSPI_SET_AUTO_TCNT(lpspi, x)                \
  do                                                 \
  {                                                  \
    lpspi->AUTOCTL &= ~(LPSPI_AUTOCTL_TCNT_Msk);     \
    lpspi->AUTOCTL |= (x << LPSPI_AUTOCTL_TCNT_Pos); \
  } while (0)

/**
* @brief      Get Auomatic Operation RX Transfer Count.
* @param[in]  lpspi The pointer of the specified LPSPI module.
* @return     None.
* \hideinitializer
*/
#define LPSPI_GET_AUTO_TCNT(lpspi) ((lpspi->AUTOCTL & LPSPI_AUTOCTL_TCNT_Msk) >> LPSPI_AUTOCTL_TCNT_Pos)

/**
  * @brief      TCNT Count Match Interrupt Flag.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * \hideinitializer
  */
#define LPSPI_GET_AUTOSTS_CNTIF(lpspi) ((lpspi->AUTOCTL & LPSPI_AUTOSTS_CNTIF_Msk) >> LPSPI_AUTOSTS_CNTIF_Pos)

/**
  * @brief      Slave Select Wake Up Flag.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * \hideinitializer
  */
#define LPSPI_GET_AUTOSTS_SSWKF(lpspi) ((lpspi->AUTOCTL & LPSPI_AUTOSTS_SSWKF_Msk) >> LPSPI_AUTOSTS_SSWKF_Pos)

/**
  * @brief      Automatic Operation Busy Flag.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * \hideinitializer
  */
#define LPSPI_GET_AUTOSTS_BUSY(lpspi) ((lpspi->AUTOCTL & LPSPI_AUTOSTS_AOBUSY_Msk) >> LPSPI_AUTOSTS_AOBUSY_Pos)

/**
  * @brief      TCNT Count Match Wake Up Flag.
  * @param[in]  lpspi The pointer of the specified LPSPI module.
  * @return     None.
  * \hideinitializer
  */
#define LPSPI_GET_AUTOSTS_CNTWKF(lpspi) ((lpspi->AUTOCTL & LPSPI_AUTOSTS_CNTWKF_Msk) >> LPSPI_AUTOSTS_CNTWKF_Pos)

/* Function prototype declaration */
uint32_t LPSPI_Open(LPSPI_T *lpspi, uint32_t u32MasterSlave, uint32_t u32SPIMode, uint32_t u32DataWidth, uint32_t u32BusClock);
void LPSPI_Close(LPSPI_T *lpspi);
void LPSPI_ClearRxFIFO(LPSPI_T *lpspi);
void LPSPI_ClearTxFIFO(LPSPI_T *lpspi);
void LPSPI_DisableAutoSS(LPSPI_T *lpspi);
void LPSPI_EnableAutoSS(LPSPI_T *lpspi, uint32_t u32SSPinMask, uint32_t u32ActiveLevel);
uint32_t LPSPI_SetBusClock(LPSPI_T *lpspi, uint32_t u32BusClock);
void LPSPI_SetFIFO(LPSPI_T *lpspi, uint32_t u32TxThreshold, uint32_t u32RxThreshold);
uint32_t LPSPI_GetBusClock(LPSPI_T *lpspi);
void LPSPI_EnableInt(LPSPI_T *lpspi, uint32_t u32Mask);
void LPSPI_DisableInt(LPSPI_T *lpspi, uint32_t u32Mask);
uint32_t LPSPI_GetIntFlag(LPSPI_T *lpspi, uint32_t u32Mask);
void LPSPI_ClearIntFlag(LPSPI_T *lpspi, uint32_t u32Mask);
uint32_t LPSPI_GetStatus(LPSPI_T *lpspi, uint32_t u32Mask);

/*@}*/ /* end of group LPSPI_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group LPSPI_Driver */

/*@}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
