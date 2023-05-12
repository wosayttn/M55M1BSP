/**************************************************************************//**
 * @file     i3c.c
 * @version  V1.00
 * @brief    I3C driver source file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup I3C_Driver I3C Driver
  @{
*/

/** @addtogroup I3C_EXPORTED_FUNCTIONS I3C Exported Functions
  @{
*/

/**
  * @brief      Open I3C with Static Address and Initial Mode
  *
  * @param[in]  i3c             The pointer of the specified I3C module.
  * @param[in]  u32MasterSlave  Decides the I3C module is operating in master mode or in slave mode. Valid values are:
  *                                 - \ref I3C_SLAVE
  *                                 - \ref I3C_MASTER
  * @param[in]  u8StaticAddr    7-bit slave address for I2C operation.
  * @param[in]  u32ModeSel      Initial mode selection to support ENTDAA CCC or Hot-Join generation. Valid values are:
  *                                 - \ref I3C_SUPPORT_ENTDAA
  *                                 - \ref I3C_SUPPORT_ADAPTIVE_HJ
  *                                 - \ref I3C_SUPPORT_IMMEDIATE_HJ
  *
  * @details    This API is used to configure I3C controller can receive ENTDAA CCC or generate a Hot-Join request.
  *             The 7-bit slave address is used for related I2C operations until I3C controller has a valid Dynamic Address.
  * @note       This API also enables all interrupt status and configures TX, RX FIFO and Response, Command Queue threshold to 0.
  */
void I3C_Open(I3C_T *i3c,
              uint32_t u32MasterSlave,
              uint8_t u8StaticAddr,
              uint32_t u32ModeSel)
{
    /* Disable I3C Controller */
    I3C_Disable(i3c);

    i3c->DEVCTLE = u32MasterSlave; // [0] 0: master, 1: slave

    if (u32MasterSlave == I3C_MASTER)
    {
        // timing setting, offset = 0xb4, 0xb8, 0xbc, 0xc0
        i3c->SCLOD = 0x00050019;
        i3c->SCLPP = 0x00050005;
        //        i3c->SCLFM = 0x01360136;
        i3c->SCLFM = 0x04000400;
        i3c->SCLFMP = 0x0021003f;
        // 0xd4, 0xd8
        i3c->BUSFAT = 0x0000003f;
        i3c->BUSIDLET = 0x00001fff;
        // 0xd0 (New for IBI Test)
        i3c->SDAHOLD = 0x00010000;
        // Threshold setting, 0x1c, 0x20
        i3c->QUETHCTL = 0x00000000;
        i3c->DBTHCTL = 0x00000003; // set rx/tx buffer size
        i3c->DEVADDR = ((u8StaticAddr << I3C_DEVADDR_SA_Pos) | I3C_DEVADDR_SAVALID_Msk)
                       | ((u8StaticAddr << I3C_DEVADDR_DA_Pos) | I3C_DEVADDR_DAVALID_Msk);

    }
    else
    {
        if (u8StaticAddr == 0)
        {
            i3c->DEVADDR &= ~I3C_DEVADDR_SAVALID_Msk;
        }
        else
        {
            i3c->DEVADDR &= ~I3C_DEVADDR_SA_Msk;
            i3c->DEVADDR |= (I3C_DEVADDR_SAVALID_Msk | u8StaticAddr);
        }

        /* Enable all interrupt status */
        i3c->INTSTSEN = 0xFFFFFFFF;

        /* Set Tx/Rx/CmdQ/ReapQ threshold to 0 */
        i3c->DBTHCTL = 0x0;
        i3c->QUETHCTL = 0x0;

        /* Select the slave ability for enter I3C mode */
        if (u32ModeSel == I3C_SUPPORT_ENTDAA)
        {
            /* HJEN disabled: Slave supports ENTDAA CCC */
            i3c->DEVCTL |= I3C_DEVCTL_ADAPTIVE_Msk;
            i3c->SLVEVNTS &= ~I3C_SLVEVNTS_HJEN_Msk;
        }
        else if (u32ModeSel == I3C_SUPPORT_ADAPTIVE_HJ)
        {
            /* Both ADAPTIVE and HJEN enabled: Slave generates a Hot-Join request until receive I3C header 7'h7E on the bus */
            i3c->DEVCTL |= I3C_DEVCTL_ADAPTIVE_Msk;
            i3c->SLVEVNTS |= I3C_SLVEVNTS_HJEN_Msk;
        }
        else if (u32ModeSel == I3C_SUPPORT_IMMEDIATE_HJ)
        {
            /* Only HJEN enabled: Slave generates a Hot-Join request immediately */
            i3c->DEVCTL &= ~I3C_DEVCTL_ADAPTIVE_Msk;
            i3c->SLVEVNTS |= I3C_SLVEVNTS_HJEN_Msk;
        }
        else
        {
            /* HJEN disabled: Slave supports ENTDAA CCC */
            i3c->DEVCTL |= I3C_DEVCTL_ADAPTIVE_Msk;
            i3c->SLVEVNTS &= ~I3C_SLVEVNTS_HJEN_Msk;
        }
    }

    /* Enable I3C Controller */
    I3C_Enable(i3c);
}

/**
  * @brief      Reset and Resume I3C Controller
  *
  * @param[in]  i3c             The pointer of the specified I3C module.
  * @param[in]  u32ResetMask    Software reset operation of I3C module. Valid values are:
  *                                 - \ref I3C_RESET_CMD_QUEUE
  *                                 - \ref I3C_RESET_RESP_QUEUE
  *                                 - \ref I3C_RESET_TX_BUF
  *                                 - \ref I3C_RESET_RX_BUF
  *                                 - \ref I3C_RESET_ALL_QUEUE_AND_BUF
  * @param[in]  u32EnableResume Enable resume I3C Slave controller. Valid values are TRUE and FALSE.
  *
  * @retval     I3C_STS_NO_ERR      No error
  * @retval     I3C_TIMEOUT_ERR     Function time-out
  *
  * @details    This API is used to reset specified FIFO and Queue or resume the I3C Slave controller from the halt state.
  * @note       THe application can reset Queues and FIFO only when the Slave controller is disabled or
  *             when the Slave controller is in Halt state(I3C_CCCDEVS[9] SLVBUSY = 1) after any error occurred from the I3C Master request.
  */
int32_t I3C_ResetAndResume(I3C_T *i3c, uint32_t u32ResetMask, uint32_t u32EnableResume)
{
    uint8_t u8InHaltState = 0;
    volatile uint32_t u32Timeout;

    if (I3C_IS_SLAVE_BUSY(i3c))
    {
        u8InHaltState = 1;
    }

    if (u32ResetMask)
    {
        /* Reset specify source */
        i3c->RSTCTL = u32ResetMask;
        u32Timeout = (SystemCoreClock / 1000);

        while ((i3c->RSTCTL != 0) && (--u32Timeout)) {}

        if (u32Timeout == 0)
        {
            return I3C_TIMEOUT_ERR;
        }
    }

    if (u32EnableResume || u8InHaltState)
    {
        /* The application has to take necessary action to handle the error condition and
            then set RESUME bit to resume the controller. */
        /* Slave will receive GETSTATUS CCC to clear specify status in I3C_CCCDEVS register. */
        i3c->DEVCTL |= I3C_DEVCTL_RESUME_Msk;

        while ((i3c->DEVCTL & I3C_DEVCTL_RESUME_Msk) == I3C_DEVCTL_RESUME_Msk) {}

        /* RESUME bit is auto-cleared once the controller is ready to accept new transfers. */
    }

    return I3C_STS_NO_ERR;
}

/**
  * @brief      Get Resopnse Status and Receive Data
  *
  * @param[in]  i3c             The pointer of the specified I3C module.
  * @param[out] pRespQ          The response data structure to get the response data.
  *
  * @retval     I3C_STS_NO_ERR          No error
  * @retval     I3C_STS_INVALID_INPUT   Invalid input parameter
  * @retval     I3C_STS_RESPQ_EMPTY     No Response Queue data
  * @return     Response error status
  *
  * @details    This API is used to get the response data and the received data on Master write operation.
  */
int32_t I3C_ParseRespQueue(I3C_T *i3c, uint32_t *pu32RespQ)
{
    uint8_t qn, u8RespQCnt;

    /* Check if RespQ buffer is empty */
    if (pu32RespQ == NULL)
    {
        return I3C_STS_INVALID_INPUT;
    }

    /* Check if RespQ is empty */
    if (I3C_IS_RESPQ_EMPTY(i3c))
    {
        return I3C_STS_RESPQ_EMPTY;
    }

    u8RespQCnt = I3C_GET_RESPQ_THLD(i3c) + 1;
    qn = 0; // Queue number

    do
    {
        pu32RespQ[qn] = I3C_GET_RESP_DATA(i3c);

        if (!I3C_IS_RESP_NO_ERR(pu32RespQ[qn]))
        {
            return (pu32RespQ[qn] & I3C_RESPQUE_ERRSTS_Msk);
        }

        qn++;
        u8RespQCnt--;
    } while (u8RespQCnt);

    return I3C_STS_NO_ERR;
}

/**
  * @brief      Set Command Queue and Transmit Data for Master Read
  *
  * @param[in]  i3c             The pointer of the specified I3C module.
  * @param[in]  u8TID           Specified Transmit Transaction ID in Command Queue.
  * @param[in]  pu32TxBuf       The buffer to send the data to transmit FIFO.
  * @param[in]  u16WriteBytes   The byte number of TX data.
  *
  * @retval     I3C_STS_NO_ERR          No error
  * @retval     I3C_STS_INVALID_INPUT   Invalid input parameter
  * @retval     I3C_STS_CMDQ_FULL       Command Queue is full
  * @retval     I3C_STS_TX_FULL         TX FIFO is full
  *
  * @details    This API is used to prepare a Command Queue and TX response data for Master read operation.
  */
int32_t I3C_SetCmdQueueAndData(I3C_T *i3c, uint8_t u8TID, uint32_t *pu32TxBuf, uint16_t u16WriteBytes)
{
    uint32_t i;

    /* Check if write bytes is exceeded */
    if (u16WriteBytes > (I3C_DEVICE_TX_BUF_CNT * 4))
    {
        return I3C_STS_INVALID_INPUT;
    }

    /* Check if CmdQ is full */
    if (I3C_IS_CMDQ_FULL(i3c))
    {
        return I3C_STS_CMDQ_FULL;
    }

    if (pu32TxBuf != NULL)
    {
        for (i = 0; i < ((u16WriteBytes + 3) / 4); i++)
        {
            /* Check if TX buffer is full */
            if (I3C_IS_TX_FULL(i3c))
            {
                return I3C_STS_TX_FULL;
            }

            i3c->TXRXDAT = pu32TxBuf[i];
        }
    }

    i3c->CMDQUE = ((u8TID << I3C_CMDQUE_TID_Pos) | (u16WriteBytes << I3C_CMDQUE_DATLEN_Pos));
    return I3C_STS_NO_ERR;
}

/**
  * @brief      Generate IBI Request
  *
  * @param[in]  i3c             The pointer of the specified I3C module.
  * @param[in]  u8MandatoryData The mandatory data byte.
  * @param[in]  u32PayloadData  The payload data.
  * @param[in]  u8PayloadLen    The byte number of payload data. The maximum length is 4 bytes.
  *
  * @retval     I3C_STS_NO_ERR              No error, IBI request accepted by the Master
  * @retval     I3C_STS_INVALID_STATE       Invalid state
  * @retval     I3C_STS_INVALID_INPUT       Invalid input parameter
  *
  * @details    This API is used to generate an IBI request on the bus.
  */
int32_t I3C_SendIBIRequest(I3C_T *i3c, uint8_t u8MandatoryData, uint32_t u32PayloadData, uint8_t u8PayloadLen)
{
    /* Check if Controller is in busy state */
    if (I3C_IS_SLAVE_BUSY(i3c))
    {
        return I3C_STS_INVALID_STATE;
    }

    /* Check if SIR function enabled */
    if (!(i3c->SLVEVNTS & I3C_SLVEVNTS_SIREN_Msk))
    {
        return I3C_STS_INVALID_STATE;
    }

    /* Clear CmdQ and Tx buffer */
//    if (I3C_ResetAndResume(i3c, (I3C_RESET_CMD_QUEUE | I3C_RESET_TX_BUF), FALSE) != 0)
//    {
//        return I3C_STS_INVALID_STATE;
//    }

    /* Check if payload length > 4-bytes */
    if (u8PayloadLen > 4)
    {
        return I3C_STS_INVALID_INPUT;
    }
    
    PB2 = 1;

    /* Program IBI payload data, payload length and MDB */
    i3c->SIR = (u8PayloadLen << I3C_SIR_DATLEN_Pos) | (u8MandatoryData << I3C_SIR_MDB_Pos) | (0 << I3C_SIR_CTL_Pos);
    i3c->SIRDAT = u32PayloadData;
    /* Trigger IBI request */
    /* SIR_EN bit be cleared automatically after the Master accepts the IBI request or Slave unable to issue the IBI request */
    i3c->SIR |= I3C_SIR_EN_Msk;
    PB2 = 0;
    return I3C_STS_NO_ERR;
}

/**
  * @brief      Enable HJ Generation
  *
  * @param[in]  i3c             The pointer of the specified I3C module.
  * @param[in]  u32ModeSel      Select the Hot-Join generation method. Valid values are:
  *                                 - \ref I3C_SUPPORT_ADAPTIVE_HJ
  *                                 - \ref I3C_SUPPORT_IMMEDIATE_HJ
  *
  * @retval     I3C_STS_NO_ERR          No error
  * @retval     I3C_TIMEOUT_ERR         Enable/Disable I3C time-out
  * @retval     I3C_STS_INVALID_INPUT   Invalid input parameter
  *
  * @details    This API is used to enable the specified HJ generation after enable I3C controller again.
  * @note       I3C Slave controller does not recognize the ENTDAA CCC after enabling the HJ generation.
  */
int32_t I3C_EnableHJRequest(I3C_T *i3c, uint32_t u32ModeSel)
{
    volatile uint32_t u32Timeout;
    I3C_ResetAndResume(i3c, I3C_RESET_ALL_QUEUE_AND_BUF, FALSE);

    /* Disable I3C controller */
    if (I3C_Disable(i3c) != I3C_STS_NO_ERR)
    {
        return I3C_TIMEOUT_ERR;
    }

    /* Select the slave ability for enter I3C mode */
    if (u32ModeSel == I3C_SUPPORT_ADAPTIVE_HJ)
    {
        /* Both ADAPTIVE and HJEN enabled: Slave generates a Hot-Join request until receive I3C header 7'h7E on the bus */
        i3c->DEVCTL |= I3C_DEVCTL_ADAPTIVE_Msk;
        i3c->SLVEVNTS |= I3C_SLVEVNTS_HJEN_Msk;
    }
    else if (u32ModeSel == I3C_SUPPORT_IMMEDIATE_HJ)
    {
        /* Only HJEN enabled: Slave generates a Hot-Join request immediately */
        i3c->DEVCTL &= ~I3C_DEVCTL_ADAPTIVE_Msk;
        i3c->SLVEVNTS |= I3C_SLVEVNTS_HJEN_Msk;
    }
    else
    {
        /* HJEN disabled: Slave supports ENTDAA CCC */
        i3c->DEVCTL |= I3C_DEVCTL_ADAPTIVE_Msk;
        i3c->SLVEVNTS &= ~I3C_SLVEVNTS_HJEN_Msk;

        /* Enable I3C controller */
        if (I3C_Enable(i3c) != I3C_STS_NO_ERR)
        {
            return I3C_TIMEOUT_ERR;
        }

        return I3C_STS_INVALID_INPUT;
    }

    /* Enable I3C controller */
    if (I3C_Enable(i3c) != I3C_STS_NO_ERR)
    {
        return I3C_TIMEOUT_ERR;
    }

    return I3C_STS_NO_ERR;
}

/**
  * @brief      Disable HJ Generation
  *
  * @param[in]  i3c             The pointer of the specified I3C module.
  *
  * @retval     I3C_STS_NO_ERR      No error
  * @retval     I3C_TIMEOUT_ERR     Enable/Disable I3C time-out
  *
  * @details    This API is used to disable the HJ generation.
  * @note       I3C Slave controller can recognize the ENTDAA CCC after disabling the HJ generation.
  */
int32_t I3C_DisableHJRequest(I3C_T *i3c)
{
    /* Disable I3C controller */
    if (I3C_Disable(i3c) != I3C_STS_NO_ERR)
    {
        return I3C_TIMEOUT_ERR;
    }

    /* HJEN disabled: Slave supports ENTDAA CCC */
    i3c->DEVCTL |= I3C_DEVCTL_ADAPTIVE_Msk;
    i3c->SLVEVNTS &= ~I3C_SLVEVNTS_HJEN_Msk;

    /* Enable I3C controller */
    if (I3C_Enable(i3c) != I3C_STS_NO_ERR)
    {
        return I3C_TIMEOUT_ERR;
    }

    return I3C_STS_NO_ERR;
}

/**
  * @brief      I3C Response Error Recovery
  *
  * @param[in]  i3c             The pointer of the specified I3C module.
  * @param[in]  u32RespStatus   Response error status from the response queue.
  *
  * @return     I3C_STS_NO_ERR
  *
  * @details    This API is used to perform error recovery then the I3C Slave controller can leave Halt(Busy) state.
  * @note       The RESUME operation is completed after a GETSTATUS CCC is received or the specified Master operation is successfully.
  */
int32_t I3C_RespErrorRecovery(I3C_T *i3c, uint32_t u32RespStatus)
{
    if (u32RespStatus != I3C_STS_NO_ERR)
    {
        if (I3C_IS_SLAVE_BUSY(i3c))
        {
            switch (u32RespStatus)
            {
                case I3C_RESP_CRC_ERR:
                case I3C_RESP_PARITY_ERR:
                case I3C_RESP_FRAME_ERR:
                case I3C_RESP_FLOW_ERR:
                    /* Reset RX FIFO -> apply resume */
                    I3C_ResetAndResume(i3c, I3C_RESET_RX_BUF, TRUE);
                    break;

                case I3C_RESP_MASTER_TERMINATE_ERR:
                    /* Reset TX FIFO and CMDQ Queue -> apply resume */
                    I3C_ResetAndResume(i3c, (I3C_RESET_TX_BUF | I3C_RESET_CMD_QUEUE), TRUE);
                    break;

                default:
                    /* Reset all FIFO and Queue */
                    I3C_ResetAndResume(i3c, I3C_RESET_ALL_QUEUE_AND_BUF, FALSE);
                    break;
            }
        }
    }

    return I3C_STS_NO_ERR;
}

/**
  * @brief      Write a byte to Slave
  *
  * @param[in]  *i3c            Point to I3C peripheral
  * @param[in]  u32IsI3cDev     Type of device is I3C or not
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  data            Write a byte data to Slave
  *
  * @retval     0               Write data success
  * @retval     1               Write data fail, or bus occurs error events
  *
  * @details    The function is used for I3C Master write a byte data to Slave.
  *
  * @note
  *
  */

uint8_t I3C_WriteByte(I3C_T *i3c, uint32_t u32IsI3cDev, uint8_t u8SlaveAddr, uint8_t data)
{
    uint8_t u8Xfering = 1u, u8Err = 0u, u8Ctrl = 0u;
    uint32_t u32TimeOutCount = 0u;

    if (u32IsI3cDev)
    {
    }
    else
    {
        i3c->DEV1ADR = (I3C_DEVADR_DEVICE_Msk | u8SlaveAddr);
        i3c->TXRXDAT = data;
        i3c->CMDQUE = (I3C_DATLEN(8) | I3C_CMD_ATTR_TRANSFER_ARG);
        i3c->CMDQUE = (I3C_CMDQUE_TOC_Msk | I3C_CMDQUE_ROC_Msk | I3C_CMD_ATTR_TRANSFER_CMD);
    }

    return (u8Err | u8Xfering);                                  /* return (Success)/(Fail) status */
}

/** @} end of group I3C_EXPORTED_FUNCTIONS */
/** @} end of group I3C_Driver */
/** @} end of group Standard_Driver */
