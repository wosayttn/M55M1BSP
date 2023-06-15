/**************************************************************************//**
 * @file    spim.c
 * @version V1.00
 * @brief   SPIM driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup SPIM_Driver SPIM Driver
  @{
*/

/** @addtogroup SPIM_EXPORTED_FUNCTIONS SPIM Exported Functions
  @{
*/

/** @cond HIDDEN_SYMBOLS */

#define ENABLE_DEBUG    0

#if ENABLE_DEBUG
    #define SPIM_DBGMSG   printf
#else
    #define SPIM_DBGMSG(...)   do { } while (0)      /* disable debug */
#endif

//------------------------------------------------------------------------------
static volatile uint8_t  g_Supported_List[] =
{
    MFGID_WINBOND,
    MFGID_MXIC,
    MFGID_EON,
    MFGID_ISSI,
    MFGID_SPANSION,
    MFGID_MICRON,
};

//------------------------------------------------------------------------------
static void N_delay(int n);
static void SwitchNBitOutput(SPIM_T *spim, uint32_t u32NBit);
static void SwitchNBitInput(SPIM_T *spim, uint32_t u32NBit);
static int32_t spim_write(SPIM_T *spim, uint8_t pu8TxBuf[], uint32_t u32NTx);
static int32_t spim_read(SPIM_T *spim, uint8_t pu8RxBuf[], uint32_t u32NRx);
static void SPIM_WriteStatusRegister(SPIM_T *spim, uint8_t dataBuf[], uint32_t u32NTx, uint32_t u32NBit);
static void SPIM_ReadStatusRegister(SPIM_T *spim, uint8_t dataBuf[], uint32_t u32NRx, uint32_t u32NBit);
static void SPIM_ReadStatusRegister2(SPIM_T *spim, uint8_t dataBuf[], uint32_t u32NRx, uint32_t u32NBit);
static void SPIM_WriteStatusRegister2(SPIM_T *spim, uint8_t dataBuf[], uint32_t u32NTx, uint32_t u32NBit);
static void SPIM_ReadStatusRegister3(SPIM_T *spim, uint8_t dataBuf[], uint32_t u32NRx, uint32_t u32NBit);
static void SPIM_ReadSecurityRegister(SPIM_T *spim, uint8_t dataBuf[], uint32_t u32NRx, uint32_t u32NBit);
static int spim_is_write_done(SPIM_T *spim, uint32_t u32NBit);
static void spim_enable_spansion_quad_mode(SPIM_T *spim, int isEn);
static void SPIM_SPANSION_4Bytes_Enable(SPIM_T *spim, int isEn, uint32_t u32NBit);
static void SPIM_WriteInPageDataByIO(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr,
                                     uint32_t u32NTx, uint8_t pu8TxBuf[], uint8_t wrCmd,
                                     uint32_t u32NBitCmd, uint32_t u32NBitAddr, uint32_t u32NBitDat,
                                     uint32_t u32DTREn, int isSync);
static int32_t SPIM_WriteInPageDataByPageWrite(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr,
                                               uint32_t u32NTx, uint8_t pu8TxBuf[], uint32_t wrCmd,
                                               int isSync);
#if 0 /* not used */
    static void spim_eon_set_qpi_mode(SPIM_T *spim, int isEn);
#endif //0

//------------------------------------------------------------------------------
static void N_delay(int n)
{
    while (n-- > 0)
    {
        __NOP();
    }
}

static void SwitchNBitOutput(SPIM_T *spim, uint32_t u32NBit)
{
    switch (u32NBit)
    {
        case 1UL:
            SPIM_ENABLE_SING_OUTPUT_MODE(spim);     /* 1-bit, Output. */
            break;

        case 2UL:
            SPIM_ENABLE_DUAL_OUTPUT_MODE(spim);     /* 2-bit, Output. */
            break;

        case 4UL:
            SPIM_ENABLE_QUAD_OUTPUT_MODE(spim);     /* 4-bit, Output. */
            break;

        case 8UL:
            SPIM_ENABLE_OCTAL_OUTPUT_MODE(spim);    /* 8-bit, Output. */
            break;

        default:
            break;
    }
}

static void SwitchNBitInput(SPIM_T *spim, uint32_t u32NBit)
{
    switch (u32NBit)
    {
        case 1UL:
            SPIM_ENABLE_SING_INPUT_MODE(spim);      /* 1-bit, Input. */
            break;

        case 2UL:
            SPIM_ENABLE_DUAL_INPUT_MODE(spim);      /* 2-bit, Input. */
            break;

        case 4UL:
            SPIM_ENABLE_QUAD_INPUT_MODE(spim);      /* 4-bit, Input. */
            break;

        case 8UL:
            SPIM_ENABLE_OCTAL_INPUT_MODE(spim);     /* 8-bit, Input. */
            break;

        default:
            break;
    }
}

/**
  * @brief      Write data to SPI slave.
  * @param      pu8TxBuf    Transmit buffer.
  * @param      u32NTx      Number of bytes to transmit.
  * @return     SPIM_OK          SPIM write done.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  */
static int32_t spim_write(SPIM_T *spim, uint8_t pu8TxBuf[], uint32_t u32NTx)
{
    uint32_t buf_idx = 0UL;

    while (u32NTx)
    {
        uint32_t dataNum = 0UL, dataNum2;

        if (u32NTx >= 16UL)
        {
            dataNum = 4UL;
        }
        else if (u32NTx >= 12UL)
        {
            dataNum = 3UL;
        }
        else if (u32NTx >= 8UL)
        {
            dataNum = 2UL;
        }
        else if (u32NTx >= 4UL)
        {
            dataNum = 1UL;
        }

        dataNum2 = dataNum;

        while (dataNum2)
        {
            uint32_t tmp;

            memcpy(&tmp, &pu8TxBuf[buf_idx], 4U);
            buf_idx += 4UL;
            u32NTx -= 4UL;

            dataNum2--;
            /*  *((__O uint32_t *) &SPIM->TX0 + dataNum2) = tmp; */
            spim->TX[dataNum2] = tmp;
        }

        if (dataNum)
        {
            SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_IO);     /* Switch to Normal mode. */
            SPIM_SET_DATA_WIDTH(spim, 32UL);
            SPIM_SET_BURST_DATA(spim, dataNum);

            if (SPIM_WaitSPIMENDone(spim, 1) == SPIM_ERR_TIMEOUT)
            {
                return SPIM_ERR_TIMEOUT;
            }
        }

        if (u32NTx && (u32NTx < 4UL))
        {
            uint32_t  rnm, tmp;

            rnm = u32NTx;
            memcpy(&tmp, &pu8TxBuf[buf_idx], u32NTx);
            buf_idx += u32NTx;
            u32NTx = 0UL;
            spim->TX[0] = tmp;

            SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_IO);     /* Switch to Normal mode. */
            SPIM_SET_DATA_WIDTH(spim, (rnm * 8UL));
            SPIM_SET_BURST_DATA(spim, 1UL);

            if (SPIM_WaitSPIMENDone(spim, 1) == SPIM_ERR_TIMEOUT)
            {
                return SPIM_ERR_TIMEOUT;
            }
        }
    }

    return SPIM_OK;
}

/**
  * @brief      Read data from SPI slave.
  * @param      pu8TxBuf    Receive buffer.
  * @param      u32NRx      Size of receive buffer in bytes.
  * @return     SPIM_OK          SPIM write done.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  */
static int32_t spim_read(SPIM_T *spim, uint8_t pu8RxBuf[], uint32_t u32NRx)
{
    uint32_t  buf_idx = 0UL;

    while (u32NRx)
    {
        uint32_t dataNum = 0UL;     /* number of words */

        if (u32NRx >= 16UL)
        {
            dataNum = 4UL;
        }
        else if (u32NRx >= 12UL)
        {
            dataNum = 3UL;
        }
        else if (u32NRx >= 8UL)
        {
            dataNum = 2UL;
        }
        else if (u32NRx >= 4UL)
        {
            dataNum = 1UL;
        }

        if (dataNum)
        {
            SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_IO);     /* Switch to Normal mode. */
            SPIM_SET_DATA_WIDTH(spim, 32UL);
            SPIM_SET_BURST_DATA(spim, dataNum);

            if (SPIM_WaitSPIMENDone(spim, 1) == SPIM_ERR_TIMEOUT)
            {
                return SPIM_ERR_TIMEOUT;
            }
        }

        while (dataNum)
        {
            uint32_t tmp;

            tmp = spim->RX[dataNum - 1UL];
            memcpy(&pu8RxBuf[buf_idx], &tmp, 4U);
            buf_idx += 4UL;
            dataNum --;
            u32NRx -= 4UL;
        }

        if (u32NRx && (u32NRx < 4UL))
        {
            uint32_t tmp;

            SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_IO);     /* Switch to Normal mode. */
            SPIM_SET_DATA_WIDTH(spim, (u32NRx * 8UL));
            SPIM_SET_BURST_DATA(spim, 1UL);

            if (SPIM_WaitSPIMENDone(spim, 1) == SPIM_ERR_TIMEOUT)
            {
                return SPIM_ERR_TIMEOUT;
            }

            tmp = spim->RX[0];
            memcpy(&pu8RxBuf[buf_idx], &tmp, u32NRx);
            buf_idx += u32NRx;
            u32NRx = 0UL;
        }
    }

    return SPIM_OK;
}

/**
  * @brief      Issue Read Status Register #1 command.
  * @param      dataBuf     Receive buffer.
  * @param      u32NRx      Size of receive buffer.
  * @param      u32NBit     N-bit transmit/receive.
  * @return     None.
  */
static void SPIM_ReadStatusRegister(SPIM_T *spim, uint8_t dataBuf[], uint32_t u32NRx, uint32_t u32NBit)
{
    uint8_t cmdBuf[] = {OPCODE_RDSR};       /* 1-byte Read Status Register #1 command. */

    SPIM_SET_SS_EN(spim, 1);                /* CS activated. */

    SwitchNBitOutput(spim, u32NBit);
    spim_write(spim, cmdBuf, sizeof(cmdBuf));
    SwitchNBitInput(spim, u32NBit);
    spim_read(spim, dataBuf, u32NRx);

    SPIM_SET_SS_EN(spim, 0);                /* CS deactivated. */
}

/**
  * @brief      Issue Write Status Register #1 command.
  * @param      dataBuf     Transmit buffer.
  * @param      u32NTx      Size of transmit buffer.
  * @param      u32NBit     N-bit transmit/receive.
  * @return     None.
  */
static void SPIM_WriteStatusRegister(SPIM_T *spim, uint8_t dataBuf[], uint32_t u32NTx, uint32_t u32NBit)
{
    uint8_t cmdBuf[] = {OPCODE_WRSR, 0x00U};        /* 1-byte Write Status Register #1 command + 1-byte data. */

    cmdBuf[1] = dataBuf[0];

    SPIM_SET_SS_EN(spim, 1);                        /* CS activated. */

    SwitchNBitOutput(spim, u32NBit);
    spim_write(spim, cmdBuf, sizeof(cmdBuf));

    SPIM_SET_SS_EN(spim, 0);                        /* CS deactivated. */
}

/**
  * @brief      Issue Read Status Register #2 command.
  * @param      dataBuf     Receive buffer.
  * @param      u32NRx      Size of receive buffer.
 * @param       u32NBit     N-bit transmit/receive.
  * @return     None.
  */
static void SPIM_ReadStatusRegister2(SPIM_T *spim, uint8_t dataBuf[], uint32_t u32NRx, uint32_t u32NBit)
{
    uint8_t cmdBuf[] = {OPCODE_RDSR2};      /* 1-byte Read Status Register #1 command. */

    SPIM_SET_SS_EN(spim, 1);                /* CS activated. */

    SwitchNBitOutput(spim, u32NBit);
    spim_write(spim, cmdBuf, sizeof(cmdBuf));
    SwitchNBitInput(spim, u32NBit);
    spim_read(spim, dataBuf, u32NRx);

    SPIM_SET_SS_EN(spim, 0);                /* CS deactivated. */
}

/**
  * @brief      Issue Winbond Write Status Register command. This command write both Status Register-1
  *             and Status Register-2.
  * @param      dataBuf     Transmit buffer.
  * @param      u32NTx      Size of transmit buffer.
  * @param      u32NBit     N-bit transmit/receive.
  * @return     None.
  */
static void SPIM_WriteStatusRegister2(SPIM_T *spim, uint8_t dataBuf[], uint32_t u32NTx, uint32_t u32NBit)
{
    uint8_t cmdBuf[3] = {OPCODE_WRSR, 0U, 0U};

    cmdBuf[1] = dataBuf[0];
    cmdBuf[2] = dataBuf[1];

    SPIM_SET_SS_EN(spim, 1);        /* CS activated. */

    SwitchNBitOutput(spim, u32NBit);
    spim_write(spim, cmdBuf, sizeof(cmdBuf));

    SPIM_SET_SS_EN(spim, 0);        /* CS deactivated. */
}

#if 0  /* not used */
/**
  * @brief      Issue Write Status Register #3 command.
  * @param      dataBuf     Transmit buffer.
  * @param      u32NTx      Size of transmit buffer.
  * @param      u32NBit     N-bit transmit/receive.
  * @return     None.
  */
static void SPIM_WriteStatusRegister3(uint8_t dataBuf[], uint32_t u32NTx, uint32_t u32NBit)
{
    uint8_t cmdBuf[] = {OPCODE_WRSR3, 0x00U};    /* 1-byte Write Status Register #2 command + 1-byte data. */
    cmdBuf[1] = dataBuf[0];

    SPIM_SET_SS_EN(1);                          /* CS activated. */
    SwitchNBitOutput(u32NBit);
    spim_write(cmdBuf, sizeof(cmdBuf));
    SPIM_SET_SS_EN(0);                          /* CS deactivated. */
}
#endif

/**
  * @brief      Issue Read Config Register #3 command.
  * @param      dataBuf     Receive buffer.
  * @param      u32NRx      Size of receive buffer.
  * @param      u32NBit     N-bit transmit/receive.
  * @return     None.
  */
static void SPIM_ReadStatusRegister3(SPIM_T *spim, uint8_t dataBuf[], uint32_t u32NRx, uint32_t u32NBit)
{
    uint8_t cmdBuf[] = {OPCODE_RDSR3};              /* 1-byte Read Status Register #1 command. */

    SPIM_SET_SS_EN(spim, 1);                        /* CS activated. */

    SwitchNBitOutput(spim, u32NBit);
    spim_write(spim, cmdBuf, sizeof(cmdBuf));
    SwitchNBitInput(spim, u32NBit);
    spim_read(spim, dataBuf, u32NRx);

    SPIM_SET_SS_EN(spim, 0);                        /* CS deactivated. */
}

/**
  * @brief      Read Micron MT35X Octal SPI Flash Flag Status Register.
  * @param      spim
  * @param      dataBuf     Receive buffer.
  * @param      u32NRx      Receive Length.
  * @param      u32NBit     N-bit transmit/receive.
  * @return     None.
  */
static void SPIM_ReadMT35XFlagRegister(SPIM_T *spim, uint8_t dataBuf[], uint32_t u32NRx, uint32_t u32NBit)
{
    uint8_t cmdBuf[] = {0x70};           /* 1-byte Read Status Register #1 command. */

    SPIM_SET_SS_EN(spim, 1);                          /* CS activated. */

    SwitchNBitOutput(spim, u32NBit);
    spim_write(spim, cmdBuf, sizeof(cmdBuf));
    SwitchNBitInput(spim, u32NBit);
    spim_read(spim, dataBuf, u32NRx);

    SPIM_SET_SS_EN(spim, 0);                          /* CS deactivated. */
}


/**
  * @brief      SPIM Start Transfer And Wait Busy Status.
  * @param      spim
  * @param      u32IsSync   Wait Busy Status
  * @return     SPIM_OK          SPIM write done.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  */
int32_t SPIM_WaitSPIMENDone(SPIM_T *spim, uint32_t u32IsSync)
{
    volatile int32_t i32TimeOutCount = SPIM_TIMEOUT;

    SPIM_SET_GO(spim);

    if (u32IsSync)
    {
        if (SPIM_GET_INT(spim))
        {
            while (SPIM_IS_IF_ON(spim))
            {
                if (--i32TimeOutCount <= 0)
                {
                    return SPIM_ERR_TIMEOUT;
                }
            }
        }
        else if (SPIM_GET_HYPER_INT(spim))
        {
            while (SPIM_GET_HYPER_INTSTS(spim))
            {
                if (--i32TimeOutCount <= 0)
                {
                    return SPIM_ERR_TIMEOUT;
                }
            }
        }
        else
        {
            while (SPIM_WAIT_FREE(spim))
            {
                if (--i32TimeOutCount <= 0)
                {
                    return SPIM_ERR_TIMEOUT;
                }
            }
        }
    }

    return SPIM_OK;
}

/**
  * @brief      Issue Winbond Write Config Register command. This command write both Status Register-1
  *             and Status Register-2.
  * @param      dataBuf     Transmit buffer.
  * @param      u32NTx      Size of transmit buffer.
  * @param      u32NBit     N-bit transmit/receive.
  * @return     None.
  */
void SPIM_WriteConfigRegister(SPIM_T *spim, uint8_t u8CMD, uint32_t u32Addr, uint8_t u8Data, uint32_t u32NBit)
{
    uint8_t cmdBuf[4] = {0x0U};
    uint32_t u32BufIdx = 0;

    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_IO); /* Switch back to Normal mode. */

    SPIM_Set_Write_Enable(spim, 1, 1);

    SPIM_SET_SS_EN(spim, 1);                    /* CS activated.    */

    SwitchNBitOutput(spim, u32NBit);

    cmdBuf[0] = u8CMD;
    spim_write(spim, cmdBuf, 1UL);

    //cmdBuf[u32BufIdx++] = (uint8_t)(u32Addr >> 24);
    cmdBuf[u32BufIdx++] = (uint8_t)(u32Addr >> 16);
    cmdBuf[u32BufIdx++] = (uint8_t)(u32Addr >> 8);
    cmdBuf[u32BufIdx++] = (uint8_t) u32Addr;
    spim_write(spim, cmdBuf, u32BufIdx);

    cmdBuf[0] = u8Data;
    spim_write(spim, cmdBuf, 1UL);

    SPIM_SET_SS_EN(spim, 0);                    /* CS deactivated.  */

    SPIM_Wait_Write_Done(spim, 1UL);
}

/**
  * @brief      Issue Read Status Register #3 command.
  * @param      pu8DataBuf  Receive buffer.
  * @param      u32NRx      Size of receive buffer.
  * @param      u32NBit     N-bit transmit/receive.
  * @return     None.
  */
void SPIM_ReadConfigRegister(SPIM_T *spim, uint8_t u8RdCMD, uint32_t u32Addr, uint8_t *pu8DataBuf, uint32_t u32NRx, uint32_t u32NBit)
{
    uint8_t cmdBuf[8] = {0};
    uint32_t buf_idx = 0;

    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_IO); /* Switch back to Normal mode.  */

    SPIM_SET_SS_EN(spim, 1);                        /* CS activated. */

    cmdBuf[0] = u8RdCMD;
    SwitchNBitOutput(spim, u32NBit);
    spim_write(spim, cmdBuf, 1UL);                  /* Write out command. */

    buf_idx = 0;
    cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 24);
    cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 16);
    cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 8);
    cmdBuf[buf_idx++] = (uint8_t) u32Addr;
    buf_idx++;//for dummy cycle
    spim_write(spim, cmdBuf, buf_idx);              /* Write out Address. */

    SwitchNBitInput(spim, u32NBit);
    spim_read(spim, pu8DataBuf, u32NRx);               /* Read back data. */

    SPIM_SET_SS_EN(spim, 0);                        /* CS deactivated. */
}

#if 0  /* not used */
/**
  * @brief      Issue Write Security Register command.
  * @param      dataBuf     Transmit buffer.
  * @param      u32NTx      Size of transmit buffer.
  * @param      u32NBit     N-bit transmit/receive.
  * @return     None.
  */
static void SPIM_WriteSecurityRegister(uint8_t dataBuf[], uint32_t u32NTx, uint32_t u32NBit)
{
    uint8_t cmdBuf[] = {OPCODE_WRSCUR, 0x00U};   /* 1-byte Write Status Register #2 command + 1-byte data. */
    cmdBuf[1] = dataBuf[0];

    SPIM_SET_SS_EN(1);                          /* CS activated. */
    SwitchNBitOutput(u32NBit);
    spim_write(cmdBuf, sizeof(cmdBuf));
    SPIM_SET_SS_EN(0);                          /* CS deactivated. */
}
#endif

/**
  * @brief      Issue Read Security Register command.
  * @param      dataBuf     Receive buffer.
  * @param      u32NRx      Size of receive buffer.
  * @param      u32NBit     N-bit transmit/receive.
  * @return     None.
  */
static void SPIM_ReadSecurityRegister(SPIM_T *spim, uint8_t dataBuf[], uint32_t u32NRx, uint32_t u32NBit)
{
    uint8_t cmdBuf[] = {OPCODE_RDSCUR};     /* 1-byte Read Status Register #1 command. */

    SPIM_SET_SS_EN(spim, 1);                /* CS activated. */

    SwitchNBitOutput(spim, u32NBit);
    spim_write(spim, cmdBuf, sizeof(cmdBuf));
    SwitchNBitInput(spim, u32NBit);
    spim_read(spim, dataBuf, u32NRx);

    SPIM_SET_SS_EN(spim, 0);                /* CS deactivated. */
}

/**
 * @brief Set SPI flash exit continue read mode.
 *
 * @param spim
 */
void SPIM_ExitContReadMode(SPIM_T *spim)
{
    uint8_t cmdBuf[4] = {0xFF, 0xFF, 0xFF, 0xFF};       /* 4-byte Exit Continue Read Data. */

    SPIM_SetQuadEnable(spim, 1, 1);

    SPIM_SET_SS_EN(spim, 1);                /* CS activated. */

    SwitchNBitOutput(spim, 4);
    spim_write(spim, cmdBuf, sizeof(cmdBuf));

    SPIM_SET_SS_EN(spim, 0);                /* CS deactivated. */

    SPIM_SetQuadEnable(spim, 0, 1);
}

/**
  * @brief      Check if Erase/Write is done.
  * @return     0: Not done. 1: Done.
  */
static int spim_is_write_done(SPIM_T *spim, uint32_t u32NBit)
{
    uint8_t status[1];
    SPIM_ReadStatusRegister(spim, status, sizeof(status), u32NBit);
    return !(status[0] & SR_WIP);
}

/**
  * @brief      Wait until Erase/Write done.
  * @param      u32NBit     N-bit transmit/receive.
  * @return     SPIM_OK          SPIM write done.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  */
int32_t SPIM_Wait_Write_Done(SPIM_T *spim, uint32_t u32NBit)
{
    uint32_t count;
    int32_t ret = SPIM_ERR_TIMEOUT;

    for (count = 0UL; count < SystemCoreClock / 1000UL; count++)
    {
        if (spim_is_write_done(spim, u32NBit))
        {
            ret = SPIM_OK;
            break;
        }
    }

    if (ret != 0)
    {
        SPIM_DBGMSG("SPIM_Wait_Write_Done time-out!!\n");
    }

    return ret;
}

/**
  * @brief      Issue Write Enable/disable command.
  * @param      isEn        Enable/disable.
  * @param      u32NBit     N-bit transmit/receive.
  * @return     None.
  */
void SPIM_Set_Write_Enable(SPIM_T *spim, int isEn, uint32_t u32NBit)
{
    uint8_t cmdBuf[] = {0U};            /* 1-byte Write Enable command. */
    cmdBuf[0] = isEn ? OPCODE_WREN : OPCODE_WRDI;

    SPIM_SET_SS_EN(spim, 1);            /* CS activated. */

    SwitchNBitOutput(spim, u32NBit);
    spim_write(spim, cmdBuf, sizeof(cmdBuf));

    SPIM_SET_SS_EN(spim, 0);            /* CS deactivated. */
}

/** @endcond HIDDEN_SYMBOLS */

/**
  * @brief      Get SPIM serial clock.
  * @return     SPI serial clock.
  * @details    This function calculates the serial clock of SPI in Hz.
  */
uint32_t SPIM_GetSClkFreq(SPIM_T *spim)
{
    uint32_t clkDiv = SPIM_GET_CLOCK_DIVIDER(spim);

    return clkDiv ? SystemCoreClock / (clkDiv * 2U) : SystemCoreClock;
}

/**
  * @brief      Initialize SPIM flash.
  * @param      clrWP           Clear Write Protect or not.
  * @return     SPIM_OK         Success.
  *             SPIM_ERR_FAIL   Unrecognized manufacture ID or failed on reading manufacture ID.
  */
int32_t SPIM_InitFlash(SPIM_T *spim, int clrWP)
{
    uint8_t   idBuf[3];
    uint8_t   cmdBuf[1];
    uint32_t  i = 0;
    int32_t   ret = SPIM_ERR_FAIL;

    SPIM_SET_OP_MODE(spim, SPIM_OP_FLASH_MODE); /* Enable SPIM Hyper Bus Mode */

    SPIM_SET_SS_ACTLVL(spim, 0);

    /*
     * Because not sure in SPI or QPI mode, do QPI reset and then SPI reset.
     */
    /* QPI Reset Enable */
    cmdBuf[0] = OPCODE_RSTEN;
    SPIM_SET_SS_EN(spim, 1);                          /* CS activated.    */
    SPIM_ENABLE_QUAD_OUTPUT_MODE(spim);               /* 1-bit, Output.   */
    spim_write(spim, cmdBuf, sizeof(cmdBuf));
    SPIM_SET_SS_EN(spim, 0);                          /* CS deactivated.  */

    /* QPI Reset */
    cmdBuf[0] = OPCODE_RST;
    SPIM_SET_SS_EN(spim, 1);                          /* CS activated.    */
    SPIM_ENABLE_QUAD_OUTPUT_MODE(spim);               /* 1-bit, Output.   */
    spim_write(spim, cmdBuf, sizeof(cmdBuf));
    SPIM_SET_SS_EN(spim, 0);                          /* CS deactivated.  */

    /* SPI ResetEnable */
    cmdBuf[0] = OPCODE_RSTEN;
    SPIM_SET_SS_EN(spim, 1);                          /* CS activated.    */
    SPIM_ENABLE_SING_OUTPUT_MODE(spim);               /* 1-bit, Output.   */
    spim_write(spim, cmdBuf, sizeof(cmdBuf));
    SPIM_SET_SS_EN(spim, 0);                          /* CS deactivated.  */

    /* SPI Reset */
    cmdBuf[0] = OPCODE_RST;
    SPIM_SET_SS_EN(spim, 1);                          /* CS activated.    */
    SPIM_ENABLE_SING_OUTPUT_MODE(spim);               /* 1-bit, Output.   */
    spim_write(spim, cmdBuf, sizeof(cmdBuf));
    SPIM_SET_SS_EN(spim, 0);                          /* CS deactivated.  */

    if (clrWP)
    {
        uint8_t dataBuf[] = {0x00U};

        SPIM_Set_Write_Enable(spim, 1, 1UL);          /* Clear Block Protect. */
        SPIM_WriteStatusRegister(spim, dataBuf, sizeof(dataBuf), 1U);
        SPIM_Wait_Write_Done(spim, 1UL);
    }

    SPIM_ReadJedecId(spim, idBuf, sizeof(idBuf), 1UL, 0, DISABLE);

    /* printf("ID: 0x%x, 0x%x, px%x\n", idBuf[0], idBuf[1], idBuf[2]); */

    for (i = 0UL; i < sizeof(g_Supported_List) / sizeof(g_Supported_List[0]); i++)
    {
        if (idBuf[0] == g_Supported_List[i])
        {
            ret = SPIM_OK;
        }
    }

    if (ret != 0)
    {
        SPIM_DBGMSG("Flash initialize failed!! 0x%x\n", idBuf[0]);
    }

    return ret;
}

/**
 * @brief   Issue JEDEC ID command.
 *
 * @param spim
 * @param pu8IdBuf          ID buffer.
 * @param u32NRx            Size of ID buffer.
 * @param u32NBit           N-bit transmit/receive.
 * @param u32DummyCycle     Dummy cycle counter
 * @param u32DTREn          Double Data Rate
 *                          - \ref SPIM_OP_ENABLE  : Enable
 *                          - \ref SPIM_OP_DISABLE : disable
 * @return                  none
 */
void SPIM_ReadJedecId(SPIM_T *spim, uint8_t *pu8IdBuf, uint32_t u32NRx,
                      uint32_t u32NBit, uint32_t u32DummyCycle, uint32_t u32DTREn)
{
    uint8_t cmdBuf[2] = { OPCODE_RDID, OPCODE_RDID };   /* 2-byte JEDEC ID command. */
    uint8_t u8CmdSize = 1;
    uint32_t u32i = 0;

    SPIM_SET_DTR_MODE(spim, u32DTREn);      /* DTR Activated. */

    SPIM_SET_SS_EN(spim, 1);                /* CS activated. */

    SwitchNBitOutput(spim, u32NBit);

    if (u32DTREn == 1)
    {
        u8CmdSize = 2;
    }

    spim_write(spim, cmdBuf, u8CmdSize);

    if (u32DummyCycle != 0)
    {
        cmdBuf[0] = 0x0;
        cmdBuf[1] = 0x0;

        SwitchNBitOutput(spim, 8);

        for (u32i = 0; u32i < u32DummyCycle; u32i++)
        {
            spim_write(spim, cmdBuf, u8CmdSize);
        }

        SwitchNBitOutput(spim, u32NBit);
    }

    SwitchNBitInput(spim, u32NBit);
    spim_read(spim, pu8IdBuf, u32NRx);
    SPIM_SET_SS_EN(spim, 0);                /* CS deactivated. */

    SPIM_SET_DTR_MODE(spim, 0);     /* DTR deactivated. */
}

/** @cond HIDDEN_SYMBOLS */

static void spim_enable_spansion_quad_mode(SPIM_T *spim, int isEn)
{
    uint8_t cmdBuf[3];
    uint8_t dataBuf[1], status1;

    cmdBuf[0] = 0x5U;                            /* Read Status Register-1 */

    SPIM_SET_SS_EN(spim, 1);
    SwitchNBitOutput(spim, 1UL);
    spim_write(spim, cmdBuf, sizeof(cmdBuf));
    SwitchNBitInput(spim, 1UL);
    spim_read(spim, dataBuf, sizeof(dataBuf));
    SPIM_SET_SS_EN(spim, 0);
    /* SPIM_DBGMSG("SR1 = 0x%x\n", dataBuf[0]); */

    status1 = dataBuf[0];

    cmdBuf[0] = 0x35U;                           /* Read Configuration Register-1 */

    SPIM_SET_SS_EN(spim, 1);
    SwitchNBitOutput(spim, 1UL);
    spim_write(spim, cmdBuf, sizeof(cmdBuf));
    SwitchNBitInput(spim, 1UL);
    spim_read(spim, dataBuf, sizeof(dataBuf));
    SPIM_SET_SS_EN(spim, 0);
    /* SPIM_DBGMSG("CR1 = 0x%x\n", dataBuf[0]); */

    SPIM_Set_Write_Enable(spim, 1, 1UL);

    cmdBuf[0] =  0x1U;                           /* Write register */
    cmdBuf[1] =  status1;

    if (isEn)
    {
        cmdBuf[2] = dataBuf[0] | 0x2U;           /* set QUAD */
    }
    else
    {
        cmdBuf[2] = dataBuf[0] & ~0x2U;          /* clear QUAD */
    }

    SPIM_SET_SS_EN(spim, 1);
    SwitchNBitOutput(spim, 1UL);
    spim_write(spim, cmdBuf, 3UL);
    SPIM_SET_SS_EN(spim, 0);

    SPIM_Set_Write_Enable(spim, 0, 1UL);


    cmdBuf[0] = 0x35U;                           /* Read Configuration Register-1 */

    SPIM_SET_SS_EN(spim, 1);
    SwitchNBitOutput(spim, 1UL);
    spim_write(spim, cmdBuf, sizeof(cmdBuf));
    SwitchNBitInput(spim, 1UL);
    spim_read(spim, dataBuf, sizeof(dataBuf));
    SPIM_SET_SS_EN(spim, 0);

    /* SPIM_DBGMSG("CR1 = 0x%x\n", dataBuf[0]); */
    N_delay(10000);
}

/** @endcond HIDDEN_SYMBOLS */

/**
  * @brief      Set Wrap Around Enable/disable.
  * @param      isEn    Wrap Mode.
 *                      - \ref SPIM_OP_ENABLE  : Enable
 *                      - \ref SPIM_OP_DISABLE : disable
  * @param      u32NBit N-bit transmit/receive.
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  */
int32_t SPIM_SetWrapAroundEnable(SPIM_T *spim, int isEn, uint32_t u32WaBit)
{
    uint32_t u32CmdBuf[2] = {0x00000000, 0x11011101};

    SPIM_SetQuadEnable(spim, 1, 1);

    SPIM_SET_SS_EN(spim, 1);

    spim->TX[0] = (u32CmdBuf[0] | (1 << 28));

    if (isEn == 1)
    {
        spim->TX[0] = ((u32CmdBuf[0] & ~(1 << 28)) | (1 << 29));
    }

    spim->TX[1] = 0x11011101;

    SwitchNBitOutput(spim, 4UL);
    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_IO);    /* Switch to Normal mode. */
    SPIM_SET_DATA_WIDTH(spim, 32UL);
    SPIM_SET_BURST_DATA(spim, 2);

    if (SPIM_WaitSPIMENDone(spim, 1) == SPIM_ERR_TIMEOUT)
    {
        return SPIM_ERR_TIMEOUT;
    }

    SPIM_SET_SS_EN(spim, 0);

    SPIM_SetQuadEnable(spim, 0, 1);

    return SPIM_OK;
}

/**
  * @brief      Set Quad Enable/disable.
  * @param      isEn    Quad Mode.
 *                      - \ref SPIM_OP_ENABLE  : Enable
 *                      - \ref SPIM_OP_DISABLE : disable
  * @param      u32NBit N-bit transmit/receive.
  * @return     None.
  */
void SPIM_SetQuadEnable(SPIM_T *spim, int isEn, uint32_t u32NBit)
{
    uint8_t  idBuf[3];
    uint8_t  dataBuf[2];

    SPIM_ReadJedecId(spim, idBuf, sizeof(idBuf), u32NBit, 0, DISABLE);

    SPIM_DBGMSG("SPIM_SetQuadEnable - Flash ID is 0x%x\n", idBuf[0]);

    switch (idBuf[0])
    {
        case MFGID_WINBOND:                      /* Winbond SPI flash  */
            SPIM_ReadStatusRegister(spim, &dataBuf[0], 1UL, u32NBit);
            SPIM_ReadStatusRegister2(spim, &dataBuf[1], 1UL, u32NBit);
            SPIM_DBGMSG("Status Register: 0x%x - 0x%x\n", dataBuf[0], dataBuf[1]);

            if (isEn)
            {
                dataBuf[1] |= SR2_QE;
            }
            else
            {
                dataBuf[1] &= ~SR2_QE;
            }

            SPIM_Set_Write_Enable(spim, 1, u32NBit);   /* Write Enable.    */
            SPIM_WriteStatusRegister2(spim, dataBuf, sizeof(dataBuf), u32NBit);
            SPIM_Wait_Write_Done(spim, u32NBit);

            SPIM_ReadStatusRegister(spim, &dataBuf[0], 1UL, u32NBit);
            SPIM_ReadStatusRegister2(spim, &dataBuf[1], 1UL, u32NBit);
            SPIM_DBGMSG("Status Register: 0x%x - 0x%x\n", dataBuf[0], dataBuf[1]);
            break;

        case MFGID_MXIC:                         /* MXIC SPI flash.  */
        case MFGID_EON:
        case MFGID_ISSI:                               /* ISSI SPI flash.  */
            SPIM_Set_Write_Enable(spim, 1, u32NBit);   /* Write Enable.    */
            dataBuf[0] = isEn ? SR_QE : 0U;
            SPIM_WriteStatusRegister(spim, dataBuf, sizeof(dataBuf), u32NBit);
            SPIM_Wait_Write_Done(spim, u32NBit);
            break;

        case MFGID_SPANSION:
            spim_enable_spansion_quad_mode(spim, isEn);
            break;

        default:
            break;
    }
}

#if 0 /* not used */
/**
  * @brief      Enter/exit QPI mode.
  * @param      isEn    QPI Mode
 *                      - \ref SPIM_OP_ENABLE  : Enable
 *                      - \ref SPIM_OP_DISABLE : disable
  * @return     None.
  */
static void spim_eon_set_qpi_mode(SPIM_T *spim, int isEn)
{
    uint8_t cmdBuf[1];                                  /* 1-byte command. */

    uint8_t status[1];
    SPIM_ReadStatusRegister(spim, status, sizeof(status), 1UL);
    SPIM_DBGMSG("Status: 0x%x\n", status[0]);

    if (isEn)                                           /* Assume in SPI mode. */
    {
        cmdBuf[0] = OPCODE_ENQPI;

        SPIM_SET_SS_EN(spim, 1);                        /* CS activated. */
        SwitchNBitOutput(spim, 1UL);
        spim_write(spim, cmdBuf, sizeof(cmdBuf));
        SPIM_SET_SS_EN(spim, 0);                        /* CS deactivated. */
    }
    else                                                /* Assume in QPI mode. */
    {
        cmdBuf[0] = OPCODE_EXQPI;

        SPIM_SET_SS_EN(spim, 1);                        /* CS activated. */
        SwitchNBitOutput(spim, 4UL);
        spim_write(spim, cmdBuf, sizeof(cmdBuf));
        SPIM_SET_SS_EN(spim, 0);                        /* CS deactivated. */
    }

    SPIM_ReadStatusRegister(spim, status, sizeof(status), 1UL);
    SPIM_DBGMSG("Status: 0x%x\n", status[0]);
}
#endif //0

static void SPIM_SPANSION_4Bytes_Enable(SPIM_T *spim, int isEn, uint32_t u32NBit)
{
    uint8_t cmdBuf[2];
    uint8_t dataBuf[1];

    cmdBuf[0] = OPCODE_BRRD;

    SPIM_SET_SS_EN(spim, 1);                     /* CS activated. */

    SwitchNBitOutput(spim, u32NBit);
    spim_write(spim, cmdBuf, 1UL);
    SwitchNBitInput(spim, 1UL);
    spim_read(spim, dataBuf, 1UL);

    SPIM_SET_SS_EN(spim, 0);                     /* CS deactivated. */

    SPIM_DBGMSG("Bank Address register= 0x%x\n", dataBuf[0]);

    cmdBuf[0] =  OPCODE_BRWR;

    if (isEn)
    {
        cmdBuf[1] = dataBuf[0] | 0x80U;          /* set EXTADD */
    }
    else
    {
        cmdBuf[1] = dataBuf[0] & ~0x80U;         /* clear EXTADD */
    }

    SPIM_SET_SS_EN(spim, 1);                     /* CS activated. */

    SwitchNBitOutput(spim, 1UL);
    spim_write(spim, cmdBuf, 2UL);

    SPIM_SET_SS_EN(spim, 0);                     /* CS deactivated. */
}

/**
 * @brief   Micron Chip Enter/Exit 4-byte address mode.
 *
 * @param spim
 * @param isEn          Enable/disable.
 * @param u32NBit       N-bit transmit/receive.
 * @param u32DTREn      DTR Mode.
 *                      - \ref SPIM_OP_ENABLE  : Enable
 *                      - \ref SPIM_OP_DISABLE : disable
 * @return              None.
 */
void SPIM_MT35x_4Bytes_Enable(SPIM_T *spim, int isEn, uint32_t u32NBit, uint32_t u32DTREn)
{
    uint8_t cmdBuf[2] = {0};
    uint8_t u8CmdSize = 1;

    SPIM_SET_DTR_MODE(spim, u32DTREn); /* DTR Activated. */

    cmdBuf[0] = isEn ? OPCODE_EN4B : OPCODE_EX4B;

    if (u32DTREn == 1)
    {
        //cmdBuf[0] = isEn ? OPCODE_EN4B : OPCODE_EX4B;
        cmdBuf[u8CmdSize++] = isEn ? OPCODE_EN4B : OPCODE_EX4B;
    }

    SPIM_SET_SS_EN(spim, 1);                      /* CS activated.    */

    SwitchNBitOutput(spim, u32NBit);
    spim_write(spim, cmdBuf, u8CmdSize);

    SPIM_SET_SS_EN(spim, 0);

    SPIM_SET_DTR_MODE(spim, 0); /* DTR Deactivated. */
}

/** @cond HIDDEN_SYMBOLS */

/**
  * @brief      Query 4-byte address mode enabled or not.
  * @param      u32NBit     N-bit transmit/receive.
  * @return     0: 4-byte address mode disabled. 1: 4-byte address mode enabled.
  */
int32_t SPIM_Is4ByteModeEnable(SPIM_T *spim, uint32_t u32NBit)
{
    int32_t isEn = 0;
    int32_t isSupt = 0;
    uint8_t idBuf[3];
    uint8_t dataBuf[1];

    SPIM_ReadJedecId(spim, idBuf, sizeof(idBuf), u32NBit, 0, DISABLE);

    /* Based on Flash size, check if 4-byte address mode is supported.  */
    switch (idBuf[0])
    {
        case MFGID_WINBOND:
        case MFGID_MXIC:
        case MFGID_EON:
            isSupt = (idBuf[2] < 0x19U) ? 0L : 1L;
            break;

        case MFGID_ISSI:
            isSupt = (idBuf[2] < 0x49U) ? 0L : 1L;
            break;

        case MFGID_MICRON:
            SPIM_ReadMT35XFlagRegister(spim, dataBuf, sizeof(dataBuf), u32NBit);
            isEn = !!(dataBuf[0] & SR3_ADR);
            break;

        default:
            break;
    }

    if (isSupt != 0)
    {
        if (idBuf[0] == MFGID_WINBOND)
        {
            /* Winbond SPI flash. */
            SPIM_ReadStatusRegister3(spim, dataBuf, sizeof(dataBuf), u32NBit);
            isEn = !!(dataBuf[0] & SR3_ADR);
        }
        else if ((idBuf[0] == MFGID_MXIC) || (idBuf[0] == MFGID_EON))
        {
            /* MXIC/EON SPI flash. */
            SPIM_ReadSecurityRegister(spim, dataBuf, sizeof(dataBuf), u32NBit);
            isEn = !!(dataBuf[0] & SCUR_4BYTE);
        }
    }

    return isEn;
}

/** @endcond HIDDEN_SYMBOLS  */

/**
  * @brief      Enter/Exit 4-byte address mode.
  * @param      isEn        Enable/disable.
  *                         - \ref SPIM_OP_ENABLE  : Enable
  *                         - \ref SPIM_OP_DISABLE : disable
  * @param      u32NBit     N-bit transmit/receive.
  * @param      u32DTREn    Enable/Disable SPIM DTR Mode.
  *                         - \ref SPIM_OP_ENABLE  : Enable
  *                         - \ref SPIM_OP_DISABLE : disable
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_FAIL    SPIM operation Fail.
  */
int32_t SPIM_Enable_4Bytes_Mode(SPIM_T *spim, int isEn, uint32_t u32NBit,  uint32_t u32DTREn)
{
    int32_t  isSupt = 0L, ret = SPIM_ERR_FAIL;
    uint8_t idBuf[3];
    uint8_t cmdBuf[1];                           /* 1-byte Enter/Exit 4-Byte Mode command. */
    volatile int32_t i32TimeOutCount = 0;

    SPIM_ReadJedecId(spim, idBuf, sizeof(idBuf), u32NBit, 0, DISABLE);

    /* Based on Flash size, check if 4-byte address mode is supported. */
    switch (idBuf[0])
    {
        case MFGID_WINBOND:
            isSupt = (idBuf[2] < 0x16U) ? 0L : 1L;
            break;

        case MFGID_MXIC:
        case MFGID_EON:
            isSupt = (idBuf[2] < 0x19U) ? 0L : 1L;
            break;

        case MFGID_ISSI:
            isSupt = (idBuf[2] < 0x49U) ? 0L : 1L;
            break;

        case MFGID_SPANSION:
            SPIM_SPANSION_4Bytes_Enable(spim, isEn, u32NBit);
            isSupt = 1L;
            ret = SPIM_OK;
            break;

        case MFGID_MICRON:
            SPIM_Set_Write_Enable(spim, 1, 1);
            isSupt = 1;
            break;

        default:
            break;
    }

    if ((isSupt) && (idBuf[0] != MFGID_SPANSION))
    {
        cmdBuf[0] = isEn ? OPCODE_EN4B : OPCODE_EX4B;

        //SPIM_SET_DTR_MODE(spim, 1);

        SPIM_SET_SS_EN(spim, 1);                      /* CS activated.    */
        SwitchNBitOutput(spim, u32NBit);
        spim_write(spim, cmdBuf, sizeof(cmdBuf));
        SPIM_SET_SS_EN(spim, 0);                      /* CS deactivated.  */

        /*
         * FIXME: Per test, 4BYTE Indicator bit doesn't set after EN4B, which
         * doesn't match spec(MX25L25635E), so skip the check below.
         */
        ret = SPIM_OK;

        if (idBuf[0] != MFGID_MXIC)
        {
            /*
             *  About over 100 instrucsions executed, just want to give
             *  a time-out about 1 seconds to avoid infinite loop
             */
            i32TimeOutCount = (SystemCoreClock) / 100;

            if (isEn)
            {
                while ((i32TimeOutCount-- > 0) && !SPIM_Is4ByteModeEnable(spim, u32NBit)) { }
            }
            else
            {
                while ((i32TimeOutCount-- > 0) && SPIM_Is4ByteModeEnable(spim, u32NBit)) { }
            }

            if (i32TimeOutCount <= 0)
                ret = SPIM_ERR_FAIL;
        }

        SPIM_SET_DTR_MODE(spim, 0); /* DTR Deactivated. */
    }

    return ret;
}

void SPIM_WinbondUnlock(SPIM_T *spim, uint32_t u32NBit)
{
    uint8_t idBuf[3];
    uint8_t dataBuf[4];

    SPIM_ReadJedecId(spim, idBuf, sizeof(idBuf), u32NBit, 0, DISABLE);

    if ((idBuf[0] != MFGID_WINBOND) || (idBuf[1] != 0x40) || (idBuf[2] != 0x16))
    {
        SPIM_DBGMSG("SPIM_WinbondUnlock - Not W25Q32, do nothing.\n");
        return;
    }

    SPIM_ReadStatusRegister(spim, &dataBuf[0], 1UL, u32NBit);
    SPIM_ReadStatusRegister2(spim, &dataBuf[1], 1UL, u32NBit);
    SPIM_DBGMSG("Status Register: 0x%x - 0x%x\n", dataBuf[0], dataBuf[1]);
    dataBuf[1] &= ~0x40;    /* clear Status Register-1 SEC bit */

    SPIM_Set_Write_Enable(spim, 1, u32NBit);   /* Write Enable.    */
    SPIM_WriteStatusRegister2(spim, dataBuf, sizeof(dataBuf), u32NBit);
    SPIM_Wait_Write_Done(spim, u32NBit);

    SPIM_ReadStatusRegister(spim, &dataBuf[0], 1UL, u32NBit);
    SPIM_ReadStatusRegister2(spim, &dataBuf[1], 1UL, u32NBit);
    SPIM_DBGMSG("Status Register (after unlock): 0x%x - 0x%x\n", dataBuf[0], dataBuf[1]);
}

/**
  * @brief      Erase whole chip.
  * @param      u32NBit     N-bit transmit/receive.
  * @param      isSync      Block or not.
  * @return     None.
  */
void SPIM_ChipErase(SPIM_T *spim, uint32_t u32NBit, int isSync)
{
    uint8_t cmdBuf[] = { OPCODE_CHIP_ERASE };    /* 1-byte Chip Erase command. */

    SPIM_Set_Write_Enable(spim, 1, u32NBit);           /* Write Enable.    */

    SPIM_SET_SS_EN(spim, 1);                          /* CS activated.    */
    SwitchNBitOutput(spim, u32NBit);
    spim_write(spim, cmdBuf, sizeof(cmdBuf));
    SPIM_SET_SS_EN(spim, 0);                          /* CS deactivated.  */

    if (isSync)
    {
        SPIM_Wait_Write_Done(spim, u32NBit);
    }
}

/**
  * @brief      Erase one block.
  * @param      u32Addr         Block to erase which contains the u32Addr.
  * @param      is4ByteAddr     4-byte u32Address or not.
  * @param      u8ErsCmd        Erase command.
  * @param      u32NBit         N-bit transmit/receive.
  * @param      isSync          Block or not.
  * @return     None.
  */
void SPIM_EraseBlock(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint8_t u8ErsCmd, uint32_t u32NBit, int isSync)
{
    uint8_t  cmdBuf[16];
    uint32_t  buf_idx = 0UL;

    SPIM_Set_Write_Enable(spim, 1, u32NBit);           /* Write Enable.    */

    cmdBuf[buf_idx++] = u8ErsCmd;

    if (is4ByteAddr)
    {
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 24);
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 16);
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 8);
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr & 0xFFUL);
    }
    else
    {
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 16);
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 8);
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr & 0xFFUL);
    }

    SPIM_SET_SS_EN(spim, 1);                      /* CS activated.    */
    SwitchNBitOutput(spim, u32NBit);
    spim_write(spim, cmdBuf, buf_idx);
    SPIM_SET_SS_EN(spim, 0);                      /* CS deactivated.  */

    if (isSync)
    {
        SPIM_Wait_Write_Done(spim, u32NBit);
    }
}

/** @cond HIDDEN_SYMBOLS */
/**
 * @brief Swich DMA/DMM Phase Setting Register
 * @param      spim
 * @param u32OPMode SPI Function Operation Mode
 *                  - \ref SPIM_CTL0_OPMODE_PAGEWRITE : DMA Write mode
 *                  - \ref SPIM_CTL0_OPMODE_PAGEREAD  : DMA Read mode
 *                  - \ref SPIM_CTL0_OPMODE_DIRECTMAP : Direct Memory Mapping mode
 * @return     PHDMAW/PHDMAR/PHDMM Register Address
 */
static void *SPIM_SwitchPhaseRegister(SPIM_T *spim, uint32_t u32OPMode)
{
    switch (u32OPMode)
    {
        case SPIM_CTL0_OPMODE_PAGEWRITE:
            return (uint32_t *)&spim->PHDMAW;
            break;

        case SPIM_CTL0_OPMODE_PAGEREAD:
            return (uint32_t *)&spim->PHDMAR;
            break;

        case SPIM_CTL0_OPMODE_DIRECTMAP:
            return (uint32_t *)&spim->PHDMM;
            break;

        default:
            break;
    }

    return NULL;
}

/**
  * @brief      Write data in the same page by I/O mode.
  * @param      u32Addr     Start u32Address to write.
  * @param      is4ByteAddr 4-byte u32Address or not.
  * @param      u32NTx      Number of bytes to write.
  * @param      pu8TxBuf    Transmit buffer.
  * @param      wrCmd       Write command.
  * @param      u32NBitCmd  N-bit transmit command.
  * @param      u32NBitAddr N-bit transmit u32Address.
  * @param      u32NBitDat  N-bit transmit/receive data.
  * @param      isSync      Block or not.
  * @return     None.
  */
static void SPIM_WriteInPageDataByIO(SPIM_T *spim, uint32_t u32Addr,
                                     int is4ByteAddr, uint32_t u32NTx,
                                     uint8_t pu8TxBuf[], uint8_t wrCmd,
                                     uint32_t u32NBitCmd, uint32_t u32NBitAddr,
                                     uint32_t u32NBitDat, uint32_t u32DTREn, int isSync)
{
    uint8_t cmdBuf[16];
    uint32_t buf_idx;
    uint32_t u32TxSize = 1UL;

    SPIM_Set_Write_Enable(spim, 1, u32NBitCmd);         /* Write Enable. */

    SPIM_SET_DTR_MODE(spim, u32DTREn);                  /* DTR Activated. */

    SPIM_SET_SS_EN(spim, SPIM_OP_ENABLE);               /* CS activated. */

    SwitchNBitOutput(spim, u32NBitCmd);
    cmdBuf[0] = wrCmd;

    if (u32DTREn == SPIM_OP_ENABLE)
    {
        cmdBuf[1] = wrCmd;
        u32TxSize = 2UL;
    }

    spim_write(spim, cmdBuf, u32TxSize);                /* Write out command. */

    buf_idx = 0UL;

    if (is4ByteAddr)
    {
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 24);
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 16);
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 8);
        cmdBuf[buf_idx++] = (uint8_t) u32Addr;
    }
    else
    {
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 16);
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 8);
        cmdBuf[buf_idx++] = (uint8_t) u32Addr;
    }

    SwitchNBitOutput(spim, u32NBitAddr);
    spim_write(spim, cmdBuf, buf_idx);                  /* Write out u32Address. */

    SwitchNBitOutput(spim, u32NBitDat);
    spim_write(spim, pu8TxBuf, u32NTx);                 /* Write out data.  */

    SPIM_SET_SS_EN(spim, SPIM_OP_DISABLE);              /* CS deactivated.  */

    SPIM_SET_DTR_MODE(spim, SPIM_OP_DISABLE);           /* DTR Deactivated. */

    if (isSync)
    {
        SPIM_Wait_Write_Done(spim, u32NBitCmd);
    }
}

/**
  * @brief      Write data in the same page by Page Write mode.
  * @param      u32Addr     Start u32Address to write.
  * @param      is4ByteAddr 4-byte u32Address or not.
  * @param      u32NTx      Number of bytes to write.
  * @param      pu8TxBuf    Transmit buffer.
  * @param      wrCmd       Write command.
  * @param      isSync      Block or not.
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  */
static int32_t SPIM_WriteInPageDataByPageWrite(SPIM_T *spim, uint32_t u32Addr,
                                               int is4ByteAddr, uint32_t u32NTx,
                                               uint8_t pu8TxBuf[], uint32_t wrCmd,
                                               int isSync)
{
    SPIM_CLEAR_MODE_DATA(spim);                         /* Disable continue read */
    SPIM_DISABLE_DMM_BWEN(spim);                        /* Disable burst wrap */

    SPIM_Set_Write_Enable(spim, 1, 1);                  /* Write Enable. */

    SPIM_SET_4BYTE_ADDR_EN(spim, is4ByteAddr);          /* Enable/disable 4-Byte Address. */

    SPIM_SET_SPIM_MODE(spim, wrCmd);                    /* SPIM mode. */
    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_PAGEWRITE);  /* Switch to Page Write mode. */

    spim->SRAMADDR = (uint32_t)pu8TxBuf;                /* SRAM u32Address. */
    spim->DMACNT = u32NTx;                              /* Transfer length. */
    spim->FADDR = u32Addr;                              /* Flash u32Address.*/

    if (SPIM_WaitSPIMENDone(spim, isSync) == SPIM_ERR_TIMEOUT)
    {
        return SPIM_ERR_TIMEOUT;
    }

    SPIM_Wait_Write_Done(spim, 1);

    return SPIM_OK;
}

/**
 * @brief Calculate I/O phase bit mode size.
 *
 * @param u32Phase
 *          - \ref PHASE_NORMAL_MODE : 1 Bit Mode
 *          - \ref PHASE_DUAL_MODE   : 2 Bit Mode
 *          - \ref PHASE_QUAD_MODE   : 4 Bit Mode
 *          - \ref PHASE_OCTAL_MODE  : 8 Bit Mode
 * @return uint32_t Phase Size
 */
static uint32_t SPIM_GetIOPhaseSize(uint32_t u32Phase)
{
    uint32_t u32PhaseSize = 0;

    switch (u32Phase)
    {
        case PHASE_DUAL_MODE:
            u32PhaseSize = 2;
            break;

        case PHASE_QUAD_MODE:
            u32PhaseSize = 4;
            break;

        case PHASE_OCTAL_MODE:
            u32PhaseSize = 8;
            break;

        default:
        case PHASE_NORMAL_MODE:
            u32PhaseSize = 1;
            break;
    }

    return u32PhaseSize;
}

/** @endcond HIDDEN_SYMBOLS */

/**
 * @brief Clear DMAW/DMAR/DMM Phase Config
 * @param      spim
 * @param u32OPMode SPI Function Operation Mode
 *                  - \ref SPIM_CTL0_OPMODE_PAGEWRITE : DMA Write mode
 *                  - \ref SPIM_CTL0_OPMODE_PAGEREAD  : DMA Read mode
 *                  - \ref SPIM_CTL0_OPMODE_DIRECTMAP : Direct Memory Mapping mode
 * @return     SPIM_OK          SPIM operation OK.
 *             SPIM_ERR_FAIL    SPIM operation Fail.
 */
int32_t SPIM_DMADMM_ClearPhaseSetting(SPIM_T *spim, uint32_t u32OPMode)
{
    uint32_t *pu32PhaseReg = (uint32_t *)SPIM_SwitchPhaseRegister(spim, u32OPMode);

    if (pu32PhaseReg == NULL)
    {
        return SPIM_ERR_FAIL;
    }

    *pu32PhaseReg &= ~(0xFFFFFFFF);    /* clear phase setting. */

    return SPIM_OK;
}

/**
 * @brief Set DMA/DMM Command Phase
 * @param spim
 * @param u32OPMode SPI Function Operation Mode
 *                  - \ref SPIM_CTL0_OPMODE_PAGEWRITE : DMA Write mode
 *                  - \ref SPIM_CTL0_OPMODE_PAGEREAD  : DMA Read mode
 *                  - \ref SPIM_CTL0_OPMODE_DIRECTMAP : Direct Memory Mapping mode
 * @param u32NBit   Command Bit Mode
 *                  - \ref PHASE_NORMAL_MODE : Send Command Use Standard Mode
 *                  - \ref PHASE_DUAL_MODE   : Send Command Use Dual Mode
 *                  - \ref PHASE_QUAD_MODE   : Send Command Use Quad Mode
 *                  - \ref PHASE_OCTAL_MODE  : Send Command Use Octal Mode
 * @param u32Width  Command Width
 *                  - \ref PHASE_WIDTH_8    : Set Command Width 8bit
 *                  - \ref PHASE_WIDTH_16   : Set Command Width 16bit
 *                  - \ref PHASE_WIDTH_24   : Set Command Width 24bit
 *                  - \ref PHASE_WIDTH_32   : Set Command Width 32bit
 * @param u32DTREn  Set Enable/Disable DTR(Double Transfer Rate) Mode 0 or 1
 */
int32_t SPIM_DMADMM_SetCMDPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32NBit, uint32_t u32Width, uint32_t u32DTREn)
{
    uint32_t *pu32PhaseReg = (uint32_t *)SPIM_SwitchPhaseRegister(spim, u32OPMode);

    if (pu32PhaseReg == NULL)
    {
        return SPIM_ERR_FAIL;
    }

    *pu32PhaseReg &= ~(0xFF);  /* clear Command phase setting. */

    *pu32PhaseReg |= (u32Width << SPIM_PHDMAW_DW_CMD_Pos);
    *pu32PhaseReg |= (u32DTREn << SPIM_PHDMAR_DTR_CMD_Pos);
    *pu32PhaseReg |= (u32NBit << SPIM_PHDMAW_BM_CMD_Pos);

    return SPIM_OK;
}

/**
 * @brief Set DMA/DMM Address Phase
 * @param spim
 * @param u32OPMode SPI Function Operation Mode
 *                  - \ref SPIM_CTL0_OPMODE_PAGEWRITE : DMA Write mode
 *                  - \ref SPIM_CTL0_OPMODE_PAGEREAD  : DMA Read mode
 *                  - \ref SPIM_CTL0_OPMODE_DIRECTMAP : Direct Memory Mapping mode
 * @param u32NBit   Command Bit Mode
 *                  - \ref PHASE_NORMAL_MODE : Send Command Use Standard Mode
 *                  - \ref PHASE_DUAL_MODE   : Send Command Use Dual Mode
 *                  - \ref PHASE_QUAD_MODE   : Send Command Use Quad Mode
 *                  - \ref PHASE_OCTAL_MODE  : Send Command Use Octal Mode
 * @param u32Width  Command Width
 *                  - \ref PHASE_WIDTH_8    : Set Command Width 8bit
 *                  - \ref PHASE_WIDTH_16   : Set Command Width 16bit
 *                  - \ref PHASE_WIDTH_24   : Set Command Width 24bit
 *                  - \ref PHASE_WIDTH_32   : Set Command Width 32bit
 * @param u32DTREn  Set Enable/Disable DTR(Double Transfer Rate) Mode 0 or 1
 * @return     SPIM_OK          SPIM operation OK.
 *             SPIM_ERR_FAIL    SPIM operation Fail.
 */
int32_t SPIM_DMADMM_SetAddrPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32NBit, uint32_t u32Width, uint32_t u32DTREn)
{
    uint32_t *pu32PhaseReg = (uint32_t *)SPIM_SwitchPhaseRegister(spim, u32OPMode);

    if (pu32PhaseReg == NULL)
    {
        return SPIM_ERR_FAIL;
    }

    *pu32PhaseReg &= ~(0xFF << 8); /* clear Address phase setting. */

    *pu32PhaseReg |= (u32Width << SPIM_PHDMAW_DW_ADDR_Pos);
    *pu32PhaseReg |= (u32NBit << SPIM_PHDMAW_BM_ADDR_Pos);
    *pu32PhaseReg |= (u32DTREn << SPIM_PHDMAR_DTR_ADDR_Pos);

    return SPIM_OK;
}

/**
 * @brief Set DMA/DMM Continue Read Mode Phase
 * @param spim
 * @param u32OPMode SPI Function Operation Mode
 *                  - \ref SPIM_CTL0_OPMODE_PAGEREAD  : DMA Read mode
 *                  - \ref SPIM_CTL0_OPMODE_DIRECTMAP : Direct Memory Mapping mode
 * @param u32NBit   Command Bit Mode
 *                  - \ref PHASE_NORMAL_MODE : Send Command Use Standard Mode
 *                  - \ref PHASE_DUAL_MODE   : Send Command Use Dual Mode
 *                  - \ref PHASE_QUAD_MODE   : Send Command Use Quad Mode
 *                  - \ref PHASE_OCTAL_MODE  : Send Command Use Octal Mode
 * @param u32Width  Command Width
 *                  - \ref PHASE_WIDTH_8    : Set Command Width 8bit
 *                  - \ref PHASE_WIDTH_16   : Set Command Width 16bit
 *                  - \ref PHASE_WIDTH_24   : Set Command Width 24bit
 *                  - \ref PHASE_WIDTH_32   : Set Command Width 32bit
 * @param u32DTREn  Set Enable/Disable DTR(Double Transfer Rate) Mode 0 or 1
 * @return     SPIM_OK          SPIM operation OK.
 *             SPIM_ERR_FAIL    SPIM operation Fail.
 */
int32_t SPIM_DMADMM_SetContReadPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32NBit, uint32_t u32Width, uint32_t u32ContEn, uint32_t u32DTREn)
{
    uint32_t *pu32PhaseReg = (uint32_t *)SPIM_SwitchPhaseRegister(spim, u32OPMode);

    if (pu32PhaseReg == NULL)
    {
        return SPIM_ERR_FAIL;
    }

    *pu32PhaseReg &= ~(0xFF << 16);    /* clear Read Mode phase setting. */

    *pu32PhaseReg |= (u32Width << SPIM_PHDMAR_DW_MODE_Pos);
    *pu32PhaseReg |= (u32NBit << SPIM_PHDMAR_BM_MODE_Pos);
    *pu32PhaseReg |= (u32DTREn << SPIM_PHDMAR_DTR_MODE_Pos);

    if ((u32ContEn == PHASE_READMODE_ON) &&
            (u32OPMode == SPIM_CTL0_OPMODE_DIRECTMAP))
    {
        SPIM_ENABLE_DMM_CREN(spim);

        /* Enable Contiue Read Mode */
        SPIM_SET_MODE_DATA(spim, CMD_CONTINUE_READ_MODE);
    }
    else
    {
        SPIM_CLEAR_MODE_DATA(spim);
    }

    return SPIM_OK;
}

/**
 * @brief Set DMA/DMM Data Phase
 * @param spim
 * @param u32OPMode SPI Function Operation Mode
 *                  - \ref SPIM_CTL0_OPMODE_PAGEWRITE : DMA Write mode
 *                  - \ref SPIM_CTL0_OPMODE_PAGEREAD  : DMA Read mode
 *                  - \ref SPIM_CTL0_OPMODE_DIRECTMAP : Direct Memory Mapping mode
 * @param u32NBit   Command Bit Mode
 *                  - \ref PHASE_NORMAL_MODE : Send Command Use Standard Mode
 *                  - \ref PHASE_DUAL_MODE   : Send Command Use Dual Mode
 *                  - \ref PHASE_QUAD_MODE   : Send Command Use Quad Mode
 *                  - \ref PHASE_OCTAL_MODE  : Send Command Use Octal Mode
 * @param u32ByteOrder Set Received Data Byte Order of Received Data Phase for Octal SPI Flash
 *                  - \ref PHASE_ORDER_MODE0 : Reveice Data Format Ref. {byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7}
 *                  - \ref PHASE_ORDER_MODE1 : Reveice Data Format Ref. {byte7, byte6, byte5, byte4, byte3, byte2, byte1, byte0}
 *                  - \ref PHASE_ORDER_MODE2 : Reveice Data Format Ref. {byte1, byte0, byte3, byte2, byte5, byte4, byte7, byte6}
 *                  - \ref PHASE_ORDER_MODE3 : Reveice Data Format Ref. {byte6, byte7, byte4, byte5, byte2, byte3, byte0, byte1}
 * @param u32RdDQS Set Enable/Disable Read DQS Mode Enable Bit for Data Phase 0 or 1
 * @param u32DTREn Set Enable/Disable DTR(Double Transfer Rate) Mode 0 or 1
 * @return     SPIM_OK          SPIM operation OK.
 *             SPIM_ERR_FAIL    SPIM operation Fail.
 */
int32_t SPIM_DMADMM_SetDataPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32NBit, uint32_t u32ByteOrder, uint32_t u32RdDQS, uint32_t u32DTREn)
{
    uint32_t *pu32PhaseReg = (uint32_t *)SPIM_SwitchPhaseRegister(spim, u32OPMode);

    if (pu32PhaseReg == NULL)
    {
        return SPIM_ERR_FAIL;
    }

    *pu32PhaseReg &= ~(0xFF << 24);     /* clear Data phase setting. */

    *pu32PhaseReg |= (u32DTREn << SPIM_PHDMAR_DTR_DATA_Pos);
    *pu32PhaseReg |= (u32RdDQS << SPIM_PHDMAR_RDQS_DATA_Pos);
    *pu32PhaseReg |= (u32ByteOrder << SPIM_PHDMAW_PBO_DATA_Pos);
    *pu32PhaseReg |= (u32NBit << SPIM_PHDMAW_BM_DATA_Pos);

    return SPIM_OK;
}

/**
 * @brief Init DMA/DMM SPI Flash Read/Wirte Command Phase
 * @param spim
 * @param psPhaseTable  Check SPI Flash specifications for support command codes,
 *                      and create command phase table.
 * @param u32OPMode     SPI Function Operation Mode
 *                      - \ref SPIM_CTL0_OPMODE_PAGEWRITE : DMA Write mode
 *                      - \ref SPIM_CTL0_OPMODE_PAGEREAD  : DMA Read mode
 *                      - \ref SPIM_CTL0_OPMODE_DIRECTMAP : Direct Memory Mapping mode
 */
void SPIM_DMADMM_InitPhase(SPIM_T *spim,
                           PHASE_SET_T *psPhaseTable,
                           uint32_t u32OPMode)
{
    uint32_t u32RDQSOn = 0;

    SPIM_DISABLE_DMM_BWEN(spim);
    SPIM_DISABLE_DMM_CREN(spim);

    /* Set Flash Command Phase */
    SPIM_DMADMM_SetCMDPhase(spim,
                            u32OPMode,
                            psPhaseTable->u32CMDPhase,
                            psPhaseTable->u32CMDWidth,
                            psPhaseTable->u32CMDDTR);

    /* Set Flash Address Phase */
    SPIM_DMADMM_SetAddrPhase(spim,
                             u32OPMode,
                             psPhaseTable->u32AddrPhase,
                             psPhaseTable->u32AddrWidth,
                             psPhaseTable->u32AddrDTR);

    /* Set DMA/DMM Continue Read Phase */
    SPIM_DMADMM_SetContReadPhase(spim,
                                 u32OPMode,
                                 psPhaseTable->u32RdModePhase,
                                 psPhaseTable->u32RdModeWidth,
                                 psPhaseTable->u32ContRdEn,
                                 psPhaseTable->u32RdModeDTR);

    if (psPhaseTable->u32DataPhase == PHASE_OCTAL_MODE)
    {
        u32RDQSOn = 1;
    }

    /* Set Flash Data Phase */
    SPIM_DMADMM_SetDataPhase(spim,
                             u32OPMode,
                             psPhaseTable->u32DataPhase,
                             psPhaseTable->u32ByteOrder,
                             u32RDQSOn,
                             psPhaseTable->u32DataDTR);

    /* Set Dummy Cycle After Address. */
    if (u32OPMode == SPIM_CTL0_OPMODE_PAGEREAD)
    {
        SPIM_SET_DMAR_DC(spim, psPhaseTable->u32DcNum);
    }
    else if (u32OPMode == SPIM_CTL0_OPMODE_DIRECTMAP)
    {
        SPIM_SET_DMM_DC(spim, psPhaseTable->u32DcNum);
    }
}

/**
 * @brief   Set DMA write phase and DMA write data
 *
 * @param spim
 * @param psPhaseTable  Check SPI Flash specifications for support command codes,
 *                      and create command phase table.
 * @param u32Addr       Write Address.
 * @param is4ByteAddr   Enable 4 Bytes Address Mode.
 * @param u32WrSize     Write Data Size.
 * @param pu8TxBuf      Write Data Buffer.
 */
void SPIM_DMA_WritePhase(SPIM_T *spim, PHASE_SET_T *psPhaseTable,
                         uint32_t u32Addr, int is4ByteAddr, uint32_t u32WrSize,
                         uint8_t *pu8TxBuf)
{
    SPIM_DMADMM_InitPhase(spim, psPhaseTable, SPIM_CTL0_OPMODE_PAGEWRITE);

    SPIM_DMA_Write(spim,
                   u32Addr,
                   is4ByteAddr,
                   u32WrSize,
                   pu8TxBuf,
                   psPhaseTable->u32CMDCode);
}

/**
 * @brief   Set DMA read phase andd DMA read data
 *
 * @param spim
 * @param psPhaseTable  Check SPI Flash specifications for support command codes,
 *                      and create command phase table.
 * @param u32Addr       Read SPI Flash Address.
 * @param is4ByteAddr   Enable 4 Bytes Address.
 * @param u32RdSize     Read Data Size.
 * @param pu8RxBuf      Read Data Buffer.
 * @param isSync        Wait SPIM Enable Done
 * @return int32_t
 */
int32_t SPIM_DMA_ReadPhase(SPIM_T *spim, PHASE_SET_T *psPhaseTable,
                           uint32_t u32Addr, int is4ByteAddr, uint32_t u32RdSize,
                           uint8_t *pu8RxBuf, int isSync)
{
    SPIM_DMADMM_InitPhase(spim, psPhaseTable, SPIM_CTL0_OPMODE_PAGEREAD);

    return SPIM_DMA_Read(spim,
                         u32Addr,
                         is4ByteAddr,
                         u32RdSize,
                         pu8RxBuf,
                         psPhaseTable->u32CMDCode,
                         isSync);
}

/**
 * @brief   Set Direct Map phase and enter direct map mode
 *
 * @param spim
 * @param psPhaseTable  Check SPI Flash specifications for support command codes,
 *                      and create command phase table.
 * @param is4ByteAddr   Enable 4 Bytes Address.
 * @param u32IdleIntvl  Direct Map Mode Idle Interval Time.
 */
void SPIM_DMM_ReadPhase(SPIM_T *spim, PHASE_SET_T *psPhaseTable, int is4ByteAddr, uint32_t u32IdleIntvl)
{
    SPIM_DMADMM_InitPhase(spim, psPhaseTable, SPIM_CTL0_OPMODE_DIRECTMAP);

    SPIM_EnterDirectMapMode(spim, is4ByteAddr, psPhaseTable->u32CMDCode, u32IdleIntvl);
}

/**
 * @brief Normal mode data phase
 *
 * @param spim
 * @param u32OPMode     Normal I/O read or wirte mode.
 *                      - \ref SPIM_IO_WRITE_PHASE
 *                      - \ref SPIM_IO_READ_PHASE
 * @param pu8TRxBuf     Read/Write Data Buffer
 * @param u32TRxSize    Read/Write Data Size
 * @param u32DataPhase  Command Bit Mode
 *                      - \ref PHASE_NORMAL_MODE : Send Command Use Standard Mode
 *                      - \ref PHASE_DUAL_MODE   : Send Command Use Dual Mode
 *                      - \ref PHASE_QUAD_MODE   : Send Command Use Quad Mode
 *                      - \ref PHASE_OCTAL_MODE  : Send Command Use Octal Mode
 * @param u32DTREn      DTR mode
 *                      - \ref SPIM_OP_ENABLE  : Enable
 *                      - \ref SPIM_OP_DISABLE : Disable
 * @param isSync        sync write done
 *                      - \ref 0 : read mode
 *                      - \ref 1 : write mode and wait write done
 */
void SPIM_IO_SendDataPhase(SPIM_T *spim, uint32_t u32OPMode, uint8_t *pu8TRxBuf,
                           uint32_t u32TRxSize, uint32_t u32DataPhase, uint32_t u32DTREn)
{
    SPIM_SET_DTR_MODE(spim, u32DTREn); /* DTR Activated. */
    //SPIM_SET_RDQS_MODE(spim, u32DTREn);

    if (u32OPMode == SPIM_IO_WRITE_PHASE)
    {
        SwitchNBitOutput(spim, SPIM_GetIOPhaseSize(u32DataPhase));
        spim_write(spim, pu8TRxBuf, u32TRxSize);     /* Write out data.  */
    }
    else
    {
        SwitchNBitInput(spim, SPIM_GetIOPhaseSize(u32DataPhase));
        spim_read(spim, pu8TRxBuf, u32TRxSize);      /* Read back data.  */
    }

    SPIM_SET_SS_EN(spim, SPIM_OP_DISABLE);          /* CS Deactivated. */
    SPIM_SET_DTR_MODE(spim, SPIM_OP_DISABLE);       /* DTR Deactivated. */
    SPIM_SET_RDQS_MODE(spim, SPIM_OP_DISABLE);

    if (u32OPMode == SPIM_IO_WRITE_PHASE)
    {
        SPIM_Wait_Write_Done(spim, u32DataPhase);
    }
}

/**
 * @brief Send dummy cycle clock through normal mode
 *
 * @param spim
 * @param u32NDummy dummy clocks, reference SPI flash command specification
 * @return int32_t
 */
int32_t SPIM_IO_SendDummyCycle(SPIM_T *spim, int u32NDummy)
{
    if (u32NDummy == 0)
    {
        return SPIM_OK;
    }

    SPIM_SET_DTR_MODE(spim, SPIM_OP_DISABLE);       /* DTR Deactivated. */
    SwitchNBitOutput(spim, 1);
    spim->TX[0] = 0x00;

    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_IO);     /* Switch to Normal mode. */
    SPIM_SET_DATA_WIDTH(spim, u32NDummy);
    SPIM_SET_BURST_DATA(spim, 1UL);

    if (SPIM_WaitSPIMENDone(spim, 1) == SPIM_ERR_TIMEOUT)
    {
        return SPIM_ERR_TIMEOUT;
    }

    return SPIM_OK;
}

void SPIM_IO_SendAddrPhase(SPIM_T *spim,
                           uint32_t u32Is4ByteAddr,
                           uint32_t u32Addr,
                           uint32_t u32AddrPhase,
                           uint32_t u32DTREn)
{
    uint8_t u8CmdBuf[8] = {0};
    uint8_t u8BufIdx = 0;

    SPIM_SET_DTR_MODE(spim, u32DTREn); /* DTR Activated. */

    if (u32Is4ByteAddr)
    {
        u8CmdBuf[u8BufIdx++] = (uint8_t)(u32Addr >> 24);
        u8CmdBuf[u8BufIdx++] = (uint8_t)(u32Addr >> 16);
        u8CmdBuf[u8BufIdx++] = (uint8_t)(u32Addr >> 8);
        u8CmdBuf[u8BufIdx++] = (uint8_t) u32Addr;
    }
    else
    {
        u8CmdBuf[u8BufIdx++] = (uint8_t)(u32Addr >> 16);
        u8CmdBuf[u8BufIdx++] = (uint8_t)(u32Addr >> 8);
        u8CmdBuf[u8BufIdx++] = (uint8_t) u32Addr;
    }

    if ((SPIM_GetIOPhaseSize(u32AddrPhase) == 2) ||
            (SPIM_GetIOPhaseSize(u32AddrPhase) == 4))
    {
        u8BufIdx++;
    }

    SwitchNBitOutput(spim, SPIM_GetIOPhaseSize(u32AddrPhase));
    spim_write(spim, u8CmdBuf, u8BufIdx);       /* Write out u32Address. */

    SPIM_SET_DTR_MODE(spim, SPIM_OP_DISABLE);   /* DTR Deactivated. */
}

/**
 * @brief Normal I/O mode sends commands using a phase table.
 *
 * @param spim
 * @param u32OPMode     Normal I/O read or wirte mode.
 *                      - \ref SPIM_IO_WRITE_PHASE
 *                      - \ref SPIM_IO_READ_PHASE
 * @param u32OpCMD      Read/Write Command reference SPI Flash specification.
 * @param u32CMDPhase   Send Command phase
 * @param u32DTREn      Double data rate
 *                      - \ref SPIM_OP_ENABLE
 *                      - \ref SPIM_OP_DISABLE
 */
void SPIM_IO_SendCMDPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32OpCMD,
                          uint32_t u32CMDPhase, uint32_t u32DTREn)
{
    uint8_t u8CmdBuf[2] = {0};
    uint8_t u8CmdSize = 1UL;

    if (u32OPMode == SPIM_IO_WRITE_PHASE)
    {
        /* Write Enable. */
        SPIM_Set_Write_Enable(spim, 1, SPIM_GetIOPhaseSize(u32CMDPhase));
    }

    SPIM_SET_DTR_MODE(spim, u32DTREn);          /* DTR Activated. */

    SPIM_SET_SS_EN(spim, SPIM_OP_ENABLE);       /* CS activated. */

    u8CmdBuf[0] = (u32OpCMD & 0xFF);

    if (u32DTREn == 1)
    {
        u8CmdBuf[1] = (u32OpCMD & 0xFF);
        u8CmdSize = 2UL;
    }

    SwitchNBitOutput(spim, SPIM_GetIOPhaseSize(u32CMDPhase));

    spim_write(spim, u8CmdBuf, u8CmdSize);      /* Write out command. */

    SPIM_SET_DTR_MODE(spim, SPIM_OP_DISABLE);   /* DTR Deactivated. */
}


void SPIM_WriteInPageDataByPhaseIO(SPIM_T *spim, PHASE_SET_T *psPhaseTable,
                                   uint32_t u32Addr, int is4ByteAddr,
                                   uint8_t *pu8RxBuf, uint32_t u32RdSize)
{
    SPIM_IO_SendCMDPhase(spim,
                         SPIM_IO_WRITE_PHASE,
                         psPhaseTable->u32CMDCode,
                         psPhaseTable->u32CMDPhase,
                         psPhaseTable->u32CMDDTR);

    SPIM_IO_SendAddrPhase(spim,
                          is4ByteAddr,
                          u32Addr,
                          psPhaseTable->u32AddrPhase,
                          psPhaseTable->u32AddrDTR);

    /* Write out dummy bytes. */
    SPIM_IO_SendDummyCycle(spim, psPhaseTable->u32DcNum);

    SPIM_IO_SendDataPhase(spim,
                          SPIM_IO_WRITE_PHASE,
                          pu8RxBuf,
                          u32RdSize,
                          psPhaseTable->u32DataPhase,
                          psPhaseTable->u32DataDTR);
}

void SPIM_IO_WritePhase(SPIM_T *spim, PHASE_SET_T *psPhaseTable,
                        uint32_t u32Addr, int is4ByteAddr,
                        uint8_t *pu8TxBuf, uint32_t u32WrSize)
{
    uint32_t  pageOffset, toWr;
    uint32_t  buf_idx = 0UL;

    pageOffset = u32Addr % 256UL;

    if ((pageOffset + u32WrSize) <= 256UL)          /* Do all the bytes fit onto one page ? */
    {
        SPIM_WriteInPageDataByPhaseIO(spim, psPhaseTable, u32Addr, is4ByteAddr, &pu8TxBuf[buf_idx], u32WrSize);
    }
    else
    {
        toWr = 256UL - pageOffset;               /* Size of data remaining on the first page. */

        SPIM_WriteInPageDataByPhaseIO(spim, psPhaseTable, u32Addr, is4ByteAddr, &pu8TxBuf[buf_idx], toWr);
        u32Addr += toWr;                         /* Advance indicator.  */
        u32WrSize -= toWr;
        buf_idx += toWr;

        while (u32WrSize)
        {
            toWr = 256UL;

            if (toWr > u32WrSize)
            {
                toWr = u32WrSize;
            }

            SPIM_WriteInPageDataByPhaseIO(spim, psPhaseTable, u32Addr, is4ByteAddr, &pu8TxBuf[buf_idx], toWr);
            u32Addr += toWr;                 /* Advance indicator.  */
            u32WrSize -= toWr;
            buf_idx += toWr;
        }
    }
}

/**
  * @brief      Read data from SPI Flash by sending commands manually (I/O mode).
  * @param      u32Addr     Start u32Address to read.
  * @param      is4ByteAddr 4-byte u32Address or not.
  * @param      u32RdSize   Number of bytes to read.
  * @param      pu8RxBuf    Receive buffer.
  * @param      rdCmd       Read command.
  * @param      u32NBitCmd  N-bit transmit command.
  * @param      u32NBitAddr N-bit transmit u32Address.
  * @param      u32NBitDat  N-bit transmit/receive data.
  * @param      u32NDummy   Number of dummy bytes following address.
  * @param      u32DTREn    DTR mode
  *                         - \ref SPIM_OP_ENABLE  : Enable
  *                         - \ref SPIM_OP_DISABLE : disable
  * @return     None.
  */
void SPIM_IO_ReadPhase(SPIM_T *spim, PHASE_SET_T *psPhaseTable,
                       uint32_t u32Addr, int is4ByteAddr,
                       uint8_t *pu8RxBuf, uint32_t u32RdSize)
{
    SPIM_IO_SendCMDPhase(spim,
                         SPIM_IO_READ_PHASE,
                         psPhaseTable->u32CMDCode,
                         psPhaseTable->u32CMDPhase,
                         psPhaseTable->u32CMDDTR);

    SPIM_IO_SendAddrPhase(spim,
                          is4ByteAddr,
                          u32Addr,
                          psPhaseTable->u32AddrPhase,
                          psPhaseTable->u32AddrDTR);

    /* Write out dummy bytes. */
    SPIM_IO_SendDummyCycle(spim, psPhaseTable->u32DcNum);

    SPIM_IO_SendDataPhase(spim,
                          SPIM_IO_READ_PHASE,
                          pu8RxBuf,
                          u32RdSize,
                          psPhaseTable->u32DataPhase,
                          psPhaseTable->u32DataDTR);
}

/**
  * @brief      Write data to SPI Flash by sending commands manually (I/O mode).
  * @param      u32Addr     Start u32Address to write.
  * @param      is4ByteAddr 4-byte u32Address or not.
  * @param      u32NTx      Number of bytes to write.
  * @param      pu8TxBuf    Transmit buffer.
  * @param      wrCmd       Write command.
  * @param      u32NBitCmd  N-bit transmit command.
  * @param      u32NBitAddr N-bit transmit u32Address.
  * @param      u32NBitDat  N-bit transmit/receive data.
  * @return     None.
  */
void SPIM_IO_Write(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr,
                   uint32_t u32NTx, uint8_t *pu8TxBuf, uint8_t wrCmd,
                   uint32_t u32NBitCmd, uint32_t u32NBitAddr, uint32_t u32NBitDat,
                   uint32_t u32DTREn)
{
    uint32_t  pageOffset, toWr;
    uint32_t  buf_idx = 0UL;

    pageOffset = u32Addr % 256UL;

    if ((pageOffset + u32NTx) <= 256UL)          /* Do all the bytes fit onto one page ? */
    {
        SPIM_WriteInPageDataByIO(spim, u32Addr, is4ByteAddr, u32NTx, &pu8TxBuf[buf_idx],
                                 wrCmd, u32NBitCmd, u32NBitAddr, u32NBitDat, u32DTREn, 1);
    }
    else
    {
        toWr = 256UL - pageOffset;               /* Size of data remaining on the first page. */

        SPIM_WriteInPageDataByIO(spim, u32Addr, is4ByteAddr, toWr, &pu8TxBuf[buf_idx],
                                 wrCmd, u32NBitCmd, u32NBitAddr, u32NBitDat, u32DTREn, 1);
        u32Addr += toWr;                         /* Advance indicator.  */
        u32NTx -= toWr;
        buf_idx += toWr;

        while (u32NTx)
        {
            toWr = 256UL;

            if (toWr > u32NTx)
            {
                toWr = u32NTx;
            }

            SPIM_WriteInPageDataByIO(spim, u32Addr, is4ByteAddr, toWr, &pu8TxBuf[buf_idx],
                                     wrCmd, u32NBitCmd, u32NBitAddr, u32NBitDat, u32DTREn, 1);
            u32Addr += toWr;                 /* Advance indicator.  */
            u32NTx -= toWr;
            buf_idx += toWr;
        }
    }
}

/**
  * @brief      Read data from SPI Flash by sending commands manually (I/O mode).
  * @param      u32Addr     Start u32Address to read.
  * @param      is4ByteAddr 4-byte u32Address or not.
  * @param      u32NRx      Number of bytes to read.
  * @param      pu8RxBuf    Receive buffer.
  * @param      rdCmd       Read command.
  * @param      u32NBitCmd  N-bit transmit command.
  * @param      u32NBitAddr N-bit transmit u32Address.
  * @param      u32NBitDat  N-bit transmit/receive data.
  * @param      u32NDummy   Number of dummy bytes following address.
  * @return     None.
  */
void SPIM_IO_Read(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint32_t u32NRx, uint8_t *pu8RxBuf, uint8_t rdCmd,
                  uint32_t u32NBitCmd, uint32_t u32NBitAddr, uint32_t u32NBitDat, int u32NDummy, uint32_t u32DTREn)
{
    uint8_t   cmdBuf[16];
    uint32_t  buf_idx;

    SPIM_SET_SS_EN(spim, 1);                          /* CS activated.    */

    cmdBuf[0] = rdCmd;
    SwitchNBitOutput(spim, u32NBitCmd);
    spim_write(spim, cmdBuf, 1UL);                    /* Write out command. */

    buf_idx = 0UL;

    if (is4ByteAddr)
    {
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 24);
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 16);
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 8);
        cmdBuf[buf_idx++] = (uint8_t) u32Addr;
    }
    else
    {
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 16);
        cmdBuf[buf_idx++] = (uint8_t)(u32Addr >> 8);
        cmdBuf[buf_idx++] = (uint8_t) u32Addr;
    }

    /* Add a byte for the address phase for dual or quad mode for read mode */
    if ((u32NBitAddr == 2) ||
            (u32NBitAddr == 4))
    {
        buf_idx++;
    }

    SwitchNBitOutput(spim, u32NBitAddr);
    spim_write(spim, cmdBuf, buf_idx);                 /* Write out u32Address. */

    buf_idx = 0UL;

    while (u32NDummy --)
    {
        cmdBuf[buf_idx++] = 0x00U;
    }

    /* Same bit mode as above. */
    spim_write(spim, cmdBuf, buf_idx);                 /* Write out dummy bytes. */

    SwitchNBitInput(spim, u32NBitDat);
    spim_read(spim, pu8RxBuf, u32NRx);                 /* Read back data.  */

    SPIM_SET_SS_EN(spim, 0);                           /* CS deactivated.  */
}

/**
  * @brief      Write data to SPI Flash by Page Write mode.
  * @param      u32Addr     Start address to write.
  * @param      is4ByteAddr 4-byte address or not.
  * @param      u32NTx      Number of bytes to write.
  * @param      pu8TxBuf    Transmit buffer.
  * @param      wrCmd       Write command.
  * @return     None.
  */
void SPIM_DMA_Write(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr,
                    uint32_t u32NTx, uint8_t *pu8TxBuf, uint32_t wrCmd)
{
    uint32_t   pageOffset, toWr;
    uint32_t   buf_idx = 0UL;

    /* DTR mode and 16-bit set to dual commands */
    if ((SPIM_GET_PHDMAW_CMD_WIDTH(spim) == PHASE_WIDTH_16) &&
            (SPIM_GET_PHDMAW_CMD_DTR(spim) == 1))
    {
        wrCmd = ((wrCmd << 8) | wrCmd);
    }

    pageOffset = u32Addr % 256UL;

    if ((pageOffset + u32NTx) <= 256UL)
    {
        /* Do all the bytes fit onto one page ? */
        SPIM_WriteInPageDataByPageWrite(spim, u32Addr, is4ByteAddr, u32NTx, pu8TxBuf, wrCmd, 1);
    }
    else
    {
        toWr = 256UL - pageOffset;               /* Size of data remaining on the first page. */

        SPIM_WriteInPageDataByPageWrite(spim, u32Addr, is4ByteAddr, toWr, &pu8TxBuf[buf_idx], wrCmd, 1);

        u32Addr += toWr;                         /* Advance indicator. */
        u32NTx -= toWr;
        buf_idx += toWr;

        while (u32NTx)
        {
            toWr = 256UL;

            if (toWr > u32NTx)
            {
                toWr = u32NTx;
            }

            SPIM_WriteInPageDataByPageWrite(spim, u32Addr, is4ByteAddr, toWr, &pu8TxBuf[buf_idx], wrCmd, 1);

            u32Addr += toWr;                 /* Advance indicator. */
            u32NTx -= toWr;
            buf_idx += toWr;
        }
    }
}

/**
  * @brief      Read data from SPI Flash by Page Read mode.
  * @param      u32Addr     Start address to read.
  * @param      is4ByteAddr 4-byte u32Address or not.
  * @param      u32NRx      Number of bytes to read.
  * @param      pu8RxBuf    Receive buffer.
  * @param      u32RdCmd    Read command.
  * @param      isSync      Block or not.
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  *             if waiting SPIM time-out.
  */
int SPIM_DMA_Read(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr,
                  uint32_t u32NRx, uint8_t *pu8RxBuf, uint32_t u32RdCmd, int isSync)
{
    /* DTR mode and 16-bit set to dual commands */
    if ((SPIM_GET_PHDMAR_CMD_WIDTH(spim) == PHASE_WIDTH_16) &&
            (SPIM_GET_PHDMAR_CMD_DTR(spim) == 1))
    {
        u32RdCmd = ((u32RdCmd << 8) | u32RdCmd);
    }

    SPIM_SET_4BYTE_ADDR_EN(spim, is4ByteAddr);          /* Enable/disable 4-Byte Address. */

    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_PAGEREAD);   /* Switch to Page Read mode. */

    SPIM_SET_SPIM_MODE(spim, u32RdCmd);                 /* SPIM mode. */

    spim->SRAMADDR = (uint32_t) pu8RxBuf;               /* SRAM u32Address. */
    spim->DMACNT = u32NRx;                              /* Transfer length. */
    spim->FADDR = u32Addr;                              /* Flash u32Address. */

    if (SPIM_WaitSPIMENDone(spim, isSync) != SPIM_OK)
    {
        return SPIM_ERR_TIMEOUT;
    }

    return SPIM_OK;
}

/**
  * @brief      Get Direct Map Address.
  * @param      spim
  * @return     Direct Mapping Address
  */
uint32_t SPIM_GetDirectMapAddress(SPIM_T *spim)
{
    uint32_t u32DMMAddr = 0;

    if (spim == SPIM0)
    {
        u32DMMAddr = SPIM0_DMM_MAP_ADDR;
    }
    else if (spim == SPIM1)
    {
        u32DMMAddr = SPIM1_DMM_MAP_ADDR;
    }

    return u32DMMAddr;
}

/**
  * @brief      Enter Direct Map mode.
  * @param      is4ByteAddr     4-byte u32Address or not.
  * @param      u32RdCmd        Read command.
  * @param      u32IdleIntvl    Idle interval.
  * @return     None.
  */
void SPIM_EnterDirectMapMode(SPIM_T *spim, int is4ByteAddr,
                             uint32_t u32RdCmd, uint32_t u32IdleIntvl)
{
    /* DTR mode and 16-bit set to dual commands */
    if ((SPIM_GET_PHDMM_CMD_WIDTH(spim) == PHASE_WIDTH_16) &&
            (SPIM_GET_PHDMM_CMD_DTR(spim) == 1))
    {
        u32RdCmd = ((u32RdCmd << 8) | u32RdCmd);
    }

    SPIM_SET_4BYTE_ADDR_EN(spim, is4ByteAddr);           /* Enable/disable 4-byte u32Address. */
    SPIM_SET_SPIM_MODE(spim, u32RdCmd);                  /* SPIM mode. */
    SPIM_SET_IDL_INTVL(spim, u32IdleIntvl);              /* Idle interval. */
    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_DIRECTMAP);   /* Switch to Direct Map mode. */
}

/**
  * @brief      Exit Direct Map mode.
  * @param      spim
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  */
int32_t SPIM_ExitDirectMapMode(SPIM_T *spim)
{
    uint32_t u32CmdBuf = 0xFFFFFFFF;

    spim->TX[0] = u32CmdBuf;

    SPIM_SetQuadEnable(spim, 1, 1);

    SPIM_SET_SS_EN(spim, 1);

    SwitchNBitOutput(spim, 4UL);
    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_IO);    /* Switch to Normal mode. */
    SPIM_SET_DATA_WIDTH(spim, 32UL);
    SPIM_SET_BURST_DATA(spim, 1);

    if (SPIM_WaitSPIMENDone(spim, 1) == SPIM_ERR_TIMEOUT)
    {
        return SPIM_ERR_TIMEOUT;
    }

    SPIM_SET_SS_EN(spim, 0);

    SPIM_SetQuadEnable(spim, 0, 1);

    spim->TX[0] = u32CmdBuf;

    SPIM_SET_SS_EN(spim, 1);

    SwitchNBitOutput(spim, 2UL);
    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_IO);    /* Switch to Normal mode. */
    SPIM_SET_DATA_WIDTH(spim, 32UL);
    SPIM_SET_BURST_DATA(spim, 1);

    if (SPIM_WaitSPIMENDone(spim, 1) == SPIM_ERR_TIMEOUT)
    {
        return SPIM_ERR_TIMEOUT;
    }

    SPIM_SET_SS_EN(spim, 0);

    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_IO); /* Switch back to Normal mode.  */

    return SPIM_OK;
}

/**
  * @brief      SPIM used to delay the read data strobe (DQS/RWDS) from Octal SPI Flash and Hyper bus device
  * @param      spim
  * @param      u32ClkOnNum Clock Cycle Number between DLL OLDO Enable and DLL Clock Divider Enable
  * @param      u32TrimNum  Clock Cycle Number between DLL Output Valid and DLL Auto Trim Enable
  * @param      u32LKNum    Clock Cycle Number between DLL Clock Divider Enable and DLL Lock
  * @param      u32OVNum    Clock Cycle Number between DLL Lock and DLL Output Valid
  * @param      u32DelayNum DLL Delay Step Number
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note       This function sets SPIM_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  * @note       First fix DIVIDER to set the frequency of SPIM output bus clock.
  */
int32_t SPIM_CtrlDLLDelayTime(SPIM_T *spim, uint32_t u32ClkOnNum, uint32_t u32TrimNum,
                              uint32_t u32LKNum, uint32_t u32OVNum, uint32_t u32DelayNum)
{
    int i32Timeout = SPIM_TIMEOUT;

    /* The default value of CLKONNUM is 0x07D0 when frequency of DLL reference clock
       is 100 MHz and enable time of DLL clock divider is 20us */
    if (u32ClkOnNum != 0)
    {
        SPIM_SET_DLL2_CLKON_NUM(spim, u32ClkOnNum);
    }

    if (u32TrimNum != 0)
    {
        SPIM_SET_DLL2_TRIM_NUM(spim, u32TrimNum);
    }

    /* The default value of DLLLKNUM is 0x07D0 when frequency of DLL reference clock
       is 100 MHz and DLL lock time is 20us. */
    if (u32LKNum != 0)
    {
        SPIM_SET_DLL1_LOCKK_VALID(spim, u32LKNum);
    }

    if (u32OVNum != 0)
    {
        SPIM_SET_DLL1_OUT_VALID(spim, u32OVNum);
    }

    /* SPIM starts to send DLL reference clock to DLL circuit
       that the frequency is the same as the SPIM output bus clock. */
    SPIM_ENABLE_DLL0_OLDO(spim, 1);

    /* User asserts this control register to 0x1,
       the DLL circuit begins searching for lock with DLL reference clock. */
    SPIM_ENABLE_DLL0_OVRST(spim, 1);

    i32Timeout = SPIM_TIMEOUT;

    /* Polling the DLL status register DLLCKON to 0x1,
       and the value 0x1 indicates that clock divider circuit inside DLL is enabled. */
    while (SPIM_WAIT_DLL0_CLKON(spim) != 1)
    {
        if (i32Timeout-- <= 0)
        {
            return SPIM_ERR_TIMEOUT;
        }
    }

    i32Timeout = SPIM_TIMEOUT;

    /* Polling the DLL status register DLLLOCK to 0x1,
       and the value 0x1 indicates that DLL circuit is in lock state */
    while (SPIM_WAIT_DLL0_LOCK(spim) != 1)
    {
        if (i32Timeout-- <= 0)
        {
            return SPIM_ERR_TIMEOUT;
        }
    }

    i32Timeout = SPIM_TIMEOUT;

    /* Polling the DLL status register DLLREADY to 0x1,
       and the value 0x1 indicates that output of DLL circuit is ready. */
    while (SPIM_WAIT_DLL0_READY(spim) != 1)
    {
        if (i32Timeout-- <= 0)
        {
            return SPIM_ERR_TIMEOUT;
        }
    }

    /* Set this valid delay number to control register DLL_DNUM. */
    SPIM_SET_DLL0_DELAY_NUM(spim, u32DelayNum);

    i32Timeout = SPIM_TIMEOUT;

    /* Polling DLL status register DLL_REF to 1
       to know the updating flow of DLL delay step number is finish or not. */
    while (SPIM_WAIT_DLL0_REFRESH(spim) != 0)
    {
        if (i32Timeout-- <= 0)
        {
            return SPIM_ERR_TIMEOUT;
        }
    }

    return SPIM_OK;
}

//------------------------------------------------------------------------------
// SPIM Hyper Define Function Prototypes
//------------------------------------------------------------------------------
/**
 * @brief   Set Hyper Bus Mode
 *
 * @param spim
 * @param u32Div Hyper bus device the setting values of DIVIDER are only 1 and 2.
 * @return int
 */
void SPIM_InitHyper(SPIM_T *spim, uint32_t u32Div)
{
    SPIM_SET_OP_MODE(spim, SPIM_OP_HYPER_MODE); /* Enable SPIM Hyper Bus Mode */

    /* Check if clock divider is 1 or 2 */
    if (u32Div <= 0)
    {
        u32Div = 1;
    }
    else if (u32Div > 2)
    {
        u32Div = 2;
    }

    SPIM_SET_CLOCK_DIVIDER(spim, u32Div);
}

/**
 * @brief Wait Hyper Bus interface is Idle
 *
 * @param spim
 * @return     SPIM_OK          SPIM operation OK.
 *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
 * @note       This function sets SPIM_ERR_TIMEOUT, if waiting Hyper Chip time-out.
 */
static int32_t SPIM_IsCMDIdle(SPIM_T *spim)
{
    volatile int32_t i32TimeOutCnt = SPIM_TIMEOUT;

    while (spim->HYPER_CMD != SPIM_HYPER_CMD_IDLE)
    {
        if (i32TimeOutCnt-- <= 0)
        {
            return SPIM_ERR_TIMEOUT;
        }
    }

    return SPIM_OK;
}

/**
  * @brief      Reset hyper chip function
  * @param      spim
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note       if waiting Hyper Chip time-out.
  */
int32_t SPIM_ResetHyper(SPIM_T *spim)
{
    spim->HYPER_CMD = SPIM_HYPER_CMD_RESET;

    if (SPIM_IsCMDIdle(spim) == SPIM_ERR_TIMEOUT)
    {
        return SPIM_ERR_TIMEOUT;
    }

    return SPIM_OK;
}

/**
  * @brief      Exit from Hybrid sleep and deep Power down function
  * @param      spim
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note       This function sets SPIM_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_ExitHSAndDPD(SPIM_T *spim)
{
    spim->HYPER_CMD = SPIM_HYPER_CMD_EXIT_HS_PD;

    if (SPIM_IsCMDIdle(spim) != SPIM_OK)
    {
        return SPIM_ERR_TIMEOUT;
    }

    return SPIM_OK;
}

/**
  * @brief      Read hyper chip register space
  * @param      spim
  * @param[in]  u32Addr  Address of hyper chip register space
  *                 - \ref HYPER_CHIP_ID_REG0       : 0x0000_0000 = Identification Register 0
  *                 - \ref HYPER_CHIP_ID_REG1       : 0x0000_0002 = Identification Register 1
  *                 - \ref HYPER_CHIP_CONFIG_REG0   : 0x0000_1000 = Configuration Register 0
  *                 - \ref HYPER_CHIP_CONFIG_REG1   : 0x0000_1002 = Configuration Register 1
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_FAIL    SPIM operation Fail.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note       This function sets SPIM_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_ReadHyperRAMReg(SPIM_T *spim, uint32_t u32Addr)
{
    if ((u32Addr != HYPER_RAM_ID_REG0) &&
            (u32Addr != HYPER_RAM_ID_REG1) &&
            (u32Addr != HYPER_RAM_CONFIG_REG0) &&
            (u32Addr != HYPER_RAM_CONFIG_REG1))
    {
        return SPIM_ERR_FAIL;
    }

    spim->HYPER_ADR = u32Addr;

    spim->HYPER_CMD = SPIM_HYPER_CMD_READ_HRAM_REGISTER;

    if (SPIM_IsCMDIdle(spim) != SPIM_OK)
    {
        return SPIM_ERR_TIMEOUT;
    }

    return (spim->HYPER_RDATA & 0xFFFF);
}

/**
  * @brief      Write Hyper Chip register space
  * @param      spim
  * @param[in]  u32Addr  Address of Hyper Chip register space
  *                 - \ref HYPER_CHIP_ID_REG0       : 0x0000_0000 = Identification Register 0
  *                 - \ref HYPER_CHIP_ID_REG1       : 0x0000_0002 = Identification Register 1
  *                 - \ref HYPER_CHIP_CONFIG_REG0   : 0x0000_1000 = Configuration Register 0
  *                 - \ref HYPER_CHIP_CONFIG_REG1   : 0x0000_1002 = Configuration Register 1
  * @param[in]
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_FAIL    SPIM operation Fail.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note       This function sets SPIM_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_WriteHyperRAMReg(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Value)
{
    if ((u32Addr != HYPER_RAM_ID_REG0) &&
            (u32Addr != HYPER_RAM_ID_REG1) &
            (u32Addr != HYPER_RAM_CONFIG_REG0) &&
            (u32Addr != HYPER_RAM_CONFIG_REG1))
    {
        return SPIM_ERR_FAIL;
    }

    spim->HYPER_WDATA &= ~0xFFFFFFFF;

    spim->HYPER_ADR = u32Addr;
    spim->HYPER_WDATA = u32Value;

    spim->HYPER_CMD = SPIM_HYPER_CMD_WRITE_HRAM_REGISTER;

    if (SPIM_IsCMDIdle(spim) != SPIM_OK)
    {
        return SPIM_ERR_TIMEOUT;
    }

    return SPIM_OK;
}

/**
  * @brief      Read 1 word from hyper chip space
  * @param      spim
  * @param[in]  u32Addr  Address of hyper chip space
  * @return     The 16 bit data of hyper chip space.
  * @note       This function sets SPIM_ERR_TIMEOUT if waiting Hyper Chip time-out.
  */
int16_t SPIM_Read1Word(SPIM_T *spim, uint32_t u32Addr)
{
    spim->HYPER_ADR = u32Addr;

    spim->HYPER_CMD = SPIM_HYPER_CMD_READ_1_WORD;

    if (SPIM_IsCMDIdle(spim) != SPIM_OK)
    {
        return SPIM_ERR_TIMEOUT;
    }

    return (spim->HYPER_RDATA & 0xFFFF);
}

/**
  * @brief      Read 2 word from hyper chip space
  * @param      spim
  * @param[in]  u32Addr  Address of hyper chip space
  * @return     The 32bit data of hyper chip space.
  */
int32_t SPIM_Read2Word(SPIM_T *spim, uint32_t u32Addr)
{
    spim->HYPER_ADR = u32Addr;

    spim->HYPER_CMD = SPIM_HYPER_CMD_READ_2_WORD;

    if (SPIM_IsCMDIdle(spim) != SPIM_OK)
    {
        return SPIM_ERR_TIMEOUT;
    }

    return (spim->HYPER_RDATA & 0xFFFFFFFF);
}

/**
  * @brief      Write 1 byte to hyper chip space
  * @param      spim
  * @param[in]  u32Addr  Address of hyper chip space
  * @param[in]  u8Data   8 bits data to be written to hyper chip space
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note       This function sets SPIM_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_Write1Byte(SPIM_T *spim, uint32_t u32Addr, uint8_t u8Data)
{
    spim->HYPER_WDATA &= ~0xFFFFFFFF;

    spim->HYPER_ADR = u32Addr;
    spim->HYPER_WDATA = u8Data;

    spim->HYPER_CMD = SPIM_HYPER_CMD_WRITE_1_BYTE;

    if (SPIM_IsCMDIdle(spim) != SPIM_OK)
    {
        return SPIM_ERR_TIMEOUT;
    }

    return SPIM_OK;
}

/**
  * @brief      Write 2 bytes to Hyper Chip space
  * @param      spim
  * @param[in]  u32Addr  Address of Hyper Chip space
  * @param[in]  u16Data  16 bits data to be written to Hyper Chip space
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note       This function sets SPIM_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_Write2Byte(SPIM_T *spim, uint32_t u32Addr, uint16_t u16Data)
{
    spim->HYPER_WDATA &= ~0xFFFFFFFF;

    spim->HYPER_ADR = u32Addr;
    spim->HYPER_WDATA = u16Data;

    spim->HYPER_CMD = SPIM_HYPER_CMD_WRITE_2_BYTE;

    if (SPIM_IsCMDIdle(spim) != SPIM_OK)
    {
        return SPIM_ERR_TIMEOUT;
    }

    return SPIM_OK;
}

/**
  * @brief      Write 3 bytes to Hyper Chip space
  * @param      spim
  * @param[in]  u32Addr  Address of Hyper Chip space
  * @param[in]  u32Data  24 bits data to be written to Hyper Chip space
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note       This function sets SPIM_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_Write3Byte(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Data)
{
    spim->HYPER_WDATA &= ~0xFFFFFFFF;

    spim->HYPER_ADR = u32Addr;
    spim->HYPER_WDATA = u32Data;

    spim->HYPER_CMD = SPIM_HYPER_CMD_WRITE_3_BYTE;

    if (SPIM_IsCMDIdle(spim) != SPIM_OK)
    {
        return SPIM_ERR_TIMEOUT;
    }

    return SPIM_OK;
}

/**
  * @brief      Write 4 byte to hyper chip space
  * @param      spim
  * @param[in]  u32Addr  Address of hyper chip space
  * @param[in]  u32Data  32 bits data to be written to hyper chip space
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note       This function sets SPIM_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_Write4Byte(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Data)
{
    spim->HYPER_WDATA &= ~0xFFFFFFFF;

    spim->HYPER_ADR = u32Addr;
    spim->HYPER_WDATA = u32Data;

    spim->HYPER_CMD = SPIM_HYPER_CMD_WRITE_4_BYTE;

    if (SPIM_IsCMDIdle(spim) != SPIM_OK)
    {
        return SPIM_ERR_TIMEOUT;
    }

    return SPIM_OK;
}

/**
  * @brief      Write data to HyperBus Module.
  * @param      u32Addr     Start address to write.
  * @param      pu8WrBuf    Transmit buffer.
  * @param      u32NTx      Number of bytes to write.
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note       This function sets SPIM_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_DMAWrite_Hyper(SPIM_T *spim, uint32_t u32Addr, uint8_t *pu8WrBuf, uint32_t u32NTx)
{
    if (pu8WrBuf == NULL)
    {
        return SPIM_ERR_FAIL;
    }

    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_PAGEWRITE);  /* Switch to DMA Write mode.   */

    spim->SRAMADDR = (uint32_t) pu8WrBuf;                /* SRAM u32Address.  */
    spim->DMACNT = u32NTx;                              /* Transfer length.  */
    spim->FADDR = u32Addr;                              /* Flash u32Address. */

    if (SPIM_WaitSPIMENDone(spim, 1) != SPIM_OK)
    {
        return SPIM_ERR_TIMEOUT;
    }

    return SPIM_OK;
}

/**
  * @brief      Read data from HyperBus Module.
  * @param      u32Addr     Start address to read.
  * @param      pu8RdBuf    Receive buffer.
  * @param      u32NRx      Number of bytes to read.
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note       This function sets SPIM_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_DMARead_Hyper(SPIM_T *spim, uint32_t u32Addr, uint8_t *pu8RdBuf, uint32_t u32NRx)
{
    if (pu8RdBuf == NULL)
    {
        return SPIM_ERR_FAIL;
    }

    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_PAGEREAD);   /* Switch to DMA Write mode.   */

    spim->SRAMADDR = (uint32_t) pu8RdBuf;                /* SRAM u32Address. */
    spim->DMACNT = u32NRx;                              /* Transfer length. */
    spim->FADDR = u32Addr;                              /* Flash u32Address. */

    if (SPIM_WaitSPIMENDone(spim, 1) != SPIM_OK)
    {
        return SPIM_ERR_TIMEOUT;
    }

    return SPIM_OK;
}

/**
  * @brief      SPIM Hyper Mode Enter DMM Mode
  * @param      spim
  * @return     None.
  * @note       This function sets SPIM_ERR_TIMEOUT if waiting Hyper Chip time-out.
  */
void SPIM_EnterDirectMapMode_Hyper(SPIM_T *spim)
{
    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_DIRECTMAP);  /* Switch to DMA Write mode.   */
}

/**
  * @brief      SPIM Hyper Mode Exit DMM Mode
  * @param      spim
  * @return     None.
  * @note       This function sets SPIM_ERR_TIMEOUT if waiting Hyper Chip time-out.
  */
void SPIM_ExitDirectMapMode_Hyper(SPIM_T *spim)
{
    SPIM_SET_OPMODE(spim, SPIM_CTL0_OPMODE_IO);       /* Switch back to Normal mode.  */
}

/**
  * @brief      Wait Hyper Direct Map Mode Read/Write Done.
  * @param      spim
  * @return     SPIM_OK          SPIM operation OK.
  *             SPIM_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note       This function sets SPIM_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_IsDMMDone_Hyper(SPIM_T *spim)
{
    volatile int32_t u32TimeOutCount = SPIM_TIMEOUT;

    SPIM_ENABLE_DMM_HYPDONE(spim);       /* HyperBus DMM Mode Done.  */

    while (SPIM_WAIT_DMM_HYPDONE(spim))
    {
        if (--u32TimeOutCount <= 0)
        {
            return SPIM_ERR_TIMEOUT;
        }
    }

    return SPIM_OK;
}

/** @} end of group SPIM_EXPORTED_FUNCTIONS */
/** @} end of group SPIM_Driver */
/** @} end of group Standard_Driver */
