/**************************************************************************//**
 * @file     lpspi.h
 * @version  V1.00
 * @brief    LPSPI driver header file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
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

#define LPSPI_MODE_0        (LPSPI_CTL_TXNEG_Msk)                           /*!< CLKPOL=0; RXNEG=0; TXNEG=1 \hideinitializer */
#define LPSPI_MODE_1        (LPSPI_CTL_RXNEG_Msk)                           /*!< CLKPOL=0; RXNEG=1; TXNEG=0 \hideinitializer */
#define LPSPI_MODE_2        (LPSPI_CTL_CLKPOL_Msk | LPSPI_CTL_RXNEG_Msk)    /*!< CLKPOL=1; RXNEG=1; TXNEG=0 \hideinitializer */
#define LPSPI_MODE_3        (LPSPI_CTL_CLKPOL_Msk | LPSPI_CTL_TXNEG_Msk)    /*!< CLKPOL=1; RXNEG=0; TXNEG=1 \hideinitializer */

#define LPSPI_SLAVE         (LPSPI_CTL_SLAVE_Msk)                           /*!< Set as slave \hideinitializer */
#define LPSPI_MASTER        (0x0U)                                          /*!< Set as master \hideinitializer */

#define LPSPI_SS                (LPSPI_SSCTL_SS_Msk)                        /*!< Set SS \hideinitializer */
#define LPSPI_SS_ACTIVE_HIGH    (LPSPI_SSCTL_SSACTPOL_Msk)                  /*!< SS active high \hideinitializer */
#define LPSPI_SS_ACTIVE_LOW     (0x0U)                                      /*!< SS active low \hideinitializer */

/* SPI Interrupt Mask */
#define LPSPI_UNIT_INT_MASK                (0x001U)                         /*!< Unit transfer interrupt mask \hideinitializer */
#define LPSPI_SSACT_INT_MASK               (0x002U)                         /*!< Slave selection signal active interrupt mask \hideinitializer */
#define LPSPI_SSINACT_INT_MASK             (0x004U)                         /*!< Slave selection signal inactive interrupt mask \hideinitializer */
#define LPSPI_SLVUR_INT_MASK               (0x008U)                         /*!< Slave under run interrupt mask \hideinitializer */
#define LPSPI_SLVBE_INT_MASK               (0x010U)                         /*!< Slave bit count error interrupt mask \hideinitializer */
#define LPSPI_TXUF_INT_MASK                (0x040U)                         /*!< Slave TX underflow interrupt mask \hideinitializer */
#define LPSPI_FIFO_TXTH_INT_MASK           (0x080U)                         /*!< FIFO TX threshold interrupt mask \hideinitializer */
#define LPSPI_FIFO_RXTH_INT_MASK           (0x100U)                         /*!< FIFO RX threshold interrupt mask \hideinitializer */
#define LPSPI_FIFO_RXOV_INT_MASK           (0x200U)                         /*!< FIFO RX overrun interrupt mask \hideinitializer */
#define LPSPI_FIFO_RXTO_INT_MASK           (0x400U)                         /*!< FIFO RX time-out interrupt mask \hideinitializer */

/* SPI Status Mask */
#define LPSPI_BUSY_MASK                    (0x01U)                          /*!< Busy status mask \hideinitializer */
#define LPSPI_RX_EMPTY_MASK                (0x02U)                          /*!< RX empty status mask \hideinitializer */
#define LPSPI_RX_FULL_MASK                 (0x04U)                          /*!< RX full status mask \hideinitializer */
#define LPSPI_TX_EMPTY_MASK                (0x08U)                          /*!< TX empty status mask \hideinitializer */
#define LPSPI_TX_FULL_MASK                 (0x10U)                          /*!< TX full status mask \hideinitializer */
#define LPSPI_TXRX_RESET_MASK              (0x20U)                          /*!< TX or RX reset status mask \hideinitializer */
#define LPSPI_SPIEN_STS_MASK               (0x40U)                          /*!< SPIEN status mask \hideinitializer */
#define LPSPI_SSLINE_STS_MASK              (0x80U)                          /*!< SPIx_SS line status mask \hideinitializer */

/** @} end of group LPSPI_EXPORTED_CONSTANTS */


/** @addtogroup LPSPI_EXPORTED_FUNCTIONS SPI Exported Functions
  @{
*/

/**
  * @brief      Clear the unit transfer interrupt flag.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Write 1 to UNITIF bit of LPSPI_STATUS register to clear the unit transfer interrupt flag.
  * \hideinitializer
  */
#define LPSPI_CLR_UNIT_TRANS_INT_FLAG(lpspi)   (lpspi->STATUS = LPSPI_STATUS_UNITIF_Msk)

/**
  * @brief      Trigger RX PDMA function.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Set RXPDMAEN bit of LPSPI_PDMACTL register to enable RX PDMA transfer function.
  * \hideinitializer
  */
#define LPSPI_TRIGGER_RX_PDMA(lpspi)   (lpspi->PDMACTL |= LPSPI_PDMACTL_RXPDMAEN_Msk)

/**
  * @brief      Trigger TX PDMA function.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Set TXPDMAEN bit of LPSPI_PDMACTL register to enable TX PDMA transfer function.
  * \hideinitializer
  */
#define LPSPI_TRIGGER_TX_PDMA(lpspi)   (lpspi->PDMACTL |= LPSPI_PDMACTL_TXPDMAEN_Msk)

/**
  * @brief      Trigger TX and RX PDMA function.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Set TXPDMAEN bit and RXPDMAEN bit of LPSPI_PDMACTL register to enable TX and RX PDMA transfer function.
  * \hideinitializer
  */
#define LPSPI_TRIGGER_TX_RX_PDMA(lpspi)   (lpspi->PDMACTL |= (LPSPI_PDMACTL_TXPDMAEN_Msk | LPSPI_PDMACTL_RXPDMAEN_Msk))

/**
  * @brief      Disable RX PDMA transfer.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Clear RXPDMAEN bit of LPSPI_PDMACTL register to disable RX PDMA transfer function.
  * \hideinitializer
  */
#define LPSPI_DISABLE_RX_PDMA(lpspi) ( lpspi->PDMACTL &= ~LPSPI_PDMACTL_RXPDMAEN_Msk )

/**
  * @brief      Disable TX PDMA transfer.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Clear TXPDMAEN bit of LPSPI_PDMACTL register to disable TX PDMA transfer function.
  * \hideinitializer
  */
#define LPSPI_DISABLE_TX_PDMA(lpspi) ( lpspi->PDMACTL &= ~LPSPI_PDMACTL_TXPDMAEN_Msk )

/**
  * @brief      Disable TX and RX PDMA transfer.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Clear TXPDMAEN bit and RXPDMAEN bit of LPSPI_PDMACTL register to disable TX and RX PDMA transfer function.
  * \hideinitializer
  */
#define LPSPI_DISABLE_TX_RX_PDMA(lpspi) ( lpspi->PDMACTL &= ~(LPSPI_PDMACTL_TXPDMAEN_Msk | LPSPI_PDMACTL_RXPDMAEN_Msk) )

/**
  * @brief      Get the count of available data in RX FIFO.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     The count of available data in RX FIFO.
  * @details    Read RXCNT (LPSPI_STATUS[27:24]) to get the count of available data in RX FIFO.
  * \hideinitializer
  */
#define LPSPI_GET_RX_FIFO_COUNT(lpspi)   ((lpspi->STATUS & LPSPI_STATUS_RXCNT_Msk) >> LPSPI_STATUS_RXCNT_Pos)

/**
  * @brief      Get the RX FIFO empty flag.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @retval     0 RX FIFO is not empty.
  * @retval     1 RX FIFO is empty.
  * @details    Read RXEMPTY bit of LPSPI_STATUS register to get the RX FIFO empty flag.
  * \hideinitializer
  */
#define LPSPI_GET_RX_FIFO_EMPTY_FLAG(lpspi)   ((lpspi->STATUS & LPSPI_STATUS_RXEMPTY_Msk) >> LPSPI_STATUS_RXEMPTY_Pos)

/**
  * @brief      Get the TX FIFO empty flag.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @retval     0 TX FIFO is not empty.
  * @retval     1 TX FIFO is empty.
  * @details    Read TXEMPTY bit of LPSPI_STATUS register to get the TX FIFO empty flag.
  * \hideinitializer
  */
#define LPSPI_GET_TX_FIFO_EMPTY_FLAG(lpspi)   ((lpspi->STATUS & LPSPI_STATUS_TXEMPTY_Msk) >> LPSPI_STATUS_TXEMPTY_Pos)

/**
  * @brief      Get the TX FIFO full flag.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @retval     0 TX FIFO is not full.
  * @retval     1 TX FIFO is full.
  * @details    Read TXFULL bit of LPSPI_STATUS register to get the TX FIFO full flag.
  * \hideinitializer
  */
#define LPSPI_GET_TX_FIFO_FULL_FLAG(lpspi)   ((lpspi->STATUS & LPSPI_STATUS_TXFULL_Msk) >> LPSPI_STATUS_TXFULL_Pos)

/**
  * @brief      Get the datum read from RX register.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     Data in RX register.
  * @details    Read LPSPI_RX register to get the received datum.
  * \hideinitializer
  */
#define LPSPI_READ_RX(lpspi)   (lpspi->RX)

/**
  * @brief      Write datum to TX register.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @param[in]  u32TxData The datum which user attempt to transfer through SPI bus.
  * @return     None.
  * @details    Write u32TxData to LPSPI_TX register.
  * \hideinitializer
  */
#define LPSPI_WRITE_TX(spi, u32TxData)   (lpspi->TX = (u32TxData))

/**
  * @brief      Set SPIx_SS pin to high state.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Disable automatic slave selection function and set SPIx_SS pin to high state.
  * \hideinitializer
  */
#define LPSPI_SET_SS_HIGH(lpspi)   (lpspi->SSCTL = (lpspi->SSCTL & (~LPSPI_SSCTL_AUTOSS_Msk)) | (LPSPI_SSCTL_SSACTPOL_Msk | SPI_SSCTL_SS_Msk))

/**
  * @brief      Set SPIx_SS pin to low state.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Disable automatic slave selection function and set SPIx_SS pin to low state.
  * \hideinitializer
  */
#define LPSPI_SET_SS_LOW(lpspi)   (lpspi->SSCTL = (lpspi->SSCTL & (~(LPSPI_SSCTL_AUTOSS_Msk | LPSPI_SSCTL_SSACTPOL_Msk))) | LPSPI_SSCTL_SS_Msk)

/**
  * @brief      Enable Byte Reorder function.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Enable Byte Reorder function. The suspend interval depends on the setting of SUSPITV (LPSPI_CTL[7:4]).
  * \hideinitializer
  */
#define LPSPI_ENABLE_BYTE_REORDER(lpspi)   (lpspi->CTL |=  LPSPI_CTL_REORDER_Msk)

/**
  * @brief      Disable Byte Reorder function.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Clear REORDER bit field of LPSPI_CTL register to disable Byte Reorder function.
  * \hideinitializer
  */
#define LPSPI_DISABLE_BYTE_REORDER(lpspi)   (lpspi->CTL &= ~LPSPI_CTL_REORDER_Msk)

/**
  * @brief      Set the length of suspend interval.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @param[in]  u32SuspCycle Decides the length of suspend interval. It could be 0 ~ 15.
  * @return     None.
  * @details    Set the length of suspend interval according to u32SuspCycle.
  *             The length of suspend interval is ((u32SuspCycle + 0.5) * the length of one SPI bus clock cycle).
  * \hideinitializer
  */
#define LPSPI_SET_SUSPEND_CYCLE(spi, u32SuspCycle)   (lpspi->CTL = (lpspi->CTL & ~LPSPI_CTL_SUSPITV_Msk) | ((u32SuspCycle) << LPSPI_CTL_SUSPITV_Pos))

/**
  * @brief      Set the SPI transfer sequence with LSB first.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Set LSB bit of LPSPI_CTL register to set the SPI transfer sequence with LSB first.
  * \hideinitializer
  */
#define LPSPI_SET_LSB_FIRST(lpspi)   (lpspi->CTL |= LPSPI_CTL_LSB_Msk)

/**
  * @brief      Set the SPI transfer sequence with MSB first.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Clear LSB bit of LPSPI_CTL register to set the SPI transfer sequence with MSB first.
  * \hideinitializer
  */
#define LPSPI_SET_MSB_FIRST(lpspi)   (lpspi->CTL &= ~LPSPI_CTL_LSB_Msk)

/**
  * @brief      Set the data width of a SPI transaction.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @param[in]  u32Width The bit width of one transaction.
  * @return     None.
  * @details    The data width can be 8 ~ 32 bits.
  * \hideinitializer
  */
#define LPSPI_SET_DATA_WIDTH(spi, u32Width)   (lpspi->CTL = (lpspi->CTL & ~LPSPI_CTL_DWIDTH_Msk) | (((u32Width)&0x1F) << LPSPI_CTL_DWIDTH_Pos))

/**
  * @brief      Get the SPI busy state.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @retval     0 SPI controller is not busy.
  * @retval     1 SPI controller is busy.
  * @details    This macro will return the busy state of SPI controller.
  * \hideinitializer
  */
#define LPSPI_IS_BUSY(lpspi)   ( (lpspi->STATUS & LPSPI_STATUS_BUSY_Msk)>>LPSPI_STATUS_BUSY_Pos )

/**
  * @brief      Enable SPI controller.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Set SPIEN (LPSPI_CTL[0]) to enable SPI controller.
  * \hideinitializer
  */
#define LPSPI_ENABLE(lpspi)   (lpspi->CTL |= LPSPI_CTL_SPIEN_Msk)

/**
  * @brief      Disable SPI controller.
  * @param[in]  lpspi The pointer of the specified SPI module.
  * @return     None.
  * @details    Clear SPIEN (LPSPI_CTL[0]) to disable SPI controller.
  * \hideinitializer
  */
#define LPSPI_DISABLE(lpspi)   (lpspi->CTL &= ~LPSPI_CTL_SPIEN_Msk)


/* Function prototype declaration */
uint32_t LPSPI_Open(LPSPI_T *spi, uint32_t u32MasterSlave, uint32_t u32SPIMode, uint32_t u32DataWidth, uint32_t u32BusClock);
void LPSPI_Close(LPSPI_T *spi);
void LPSPI_ClearRxFIFO(LPSPI_T *spi);
void LPSPI_ClearTxFIFO(LPSPI_T *spi);
void LPSPI_DisableAutoSS(LPSPI_T *spi);
void LPSPI_EnableAutoSS(LPSPI_T *spi, uint32_t u32SSPinMask, uint32_t u32ActiveLevel);
uint32_t LPSPI_SetBusClock(LPSPI_T *spi, uint32_t u32BusClock);
void LPSPI_SetFIFO(LPSPI_T *spi, uint32_t u32TxThreshold, uint32_t u32RxThreshold);
uint32_t LPSPI_GetBusClock(LPSPI_T *spi);
void LPSPI_EnableInt(LPSPI_T *spi, uint32_t u32Mask);
void LPSPI_DisableInt(LPSPI_T *spi, uint32_t u32Mask);
uint32_t LPSPI_GetIntFlag(LPSPI_T *spi, uint32_t u32Mask);
void LPSPI_ClearIntFlag(LPSPI_T *spi, uint32_t u32Mask);
uint32_t LPSPI_GetStatus(LPSPI_T *spi, uint32_t u32Mask);

/** @} end of group LPSPI_EXPORTED_FUNCTIONS */
/** @} end of group LPSPI_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __LPSPI_H__ */
