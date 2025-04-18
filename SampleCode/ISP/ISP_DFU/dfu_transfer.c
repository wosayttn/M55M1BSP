/******************************************************************************//**
 * @file     dfu_transfer.c
 * @version  V1.00
 * @brief    USBD DFU transfer sample file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

/*!<Includes */
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "dfu_transfer.h"
#include "fmc_user.h"

#define APROM_BLOCK_NUM         ((g_u32ApromSize/TRANSFER_SIZE)-1)

static uint32_t s_u32CommandCount = 0;

static dfu_status_struct dfu_status;
static s_prog_struct prog_struct __attribute__((aligned(4))) = { {0}, 0, 0, FMC_APROM_BASE };

NVT_ITCM void USBD_IRQHandler(void)
{
    uint32_t u32IntSts = USBD_GET_INT_FLAG();
    uint32_t u32State = USBD_GET_BUS_STATE();

    //------------------------------------------------------------------
    if (u32IntSts & USBD_INTSTS_FLDET)
    {
        // Floating detect
        USBD_CLR_INT_FLAG(USBD_INTSTS_FLDET);

        if (USBD_IS_ATTACHED())
        {
            /* USB Plug In */
            USBD_ENABLE_USB();
        }
        else
        {
            /* USB Un-plug */
            USBD_DISABLE_USB();
        }
    }

    //------------------------------------------------------------------
    if (u32IntSts & USBD_INTSTS_BUS)
    {
        /* Clear event flag */
        USBD_CLR_INT_FLAG(USBD_INTSTS_BUS);

        if (u32State & USBD_STATE_USBRST)
        {
            /* Bus reset */
            USBD_ENABLE_USB();
            USBD_SwReset();
        }

        if (u32State & USBD_STATE_SUSPEND)
        {
            /* Enable USB but disable PHY */
            USBD_DISABLE_PHY();
        }

        if (u32State & USBD_STATE_RESUME)
        {
            /* Enable USB and enable PHY */
            USBD_ENABLE_USB();
        }

    }

    if (u32IntSts & USBD_INTSTS_NEVWKIF_Msk)
    {
        /*Clear no-event wake up interrupt */
        USBD_CLR_INT_FLAG(USBD_INTSTS_NEVWKIF_Msk);
        /*
           TODO: Implement the function that will be executed when device is woken by non-USB event.
        */
    }

    //------------------------------------------------------------------
    if (u32IntSts & USBD_INTSTS_USB)
    {
        // USB event
        if (u32IntSts & USBD_INTSTS_SETUP)
        {
            // Setup packet
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_SETUP);

            /* Clear the data IN/OUT ready flag of control end-points */
            USBD_STOP_TRANSACTION(EP0);
            USBD_STOP_TRANSACTION(EP1);

            USBD_ProcessSetupPacket();
        }

        // EP events
        if (u32IntSts & USBD_INTSTS_EP0)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP0);
            // control IN
            USBD_CtrlIn();
        }

        if (u32IntSts & USBD_INTSTS_EP1)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP1);

            // control OUT
            USBD_CtrlOut();
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
  * @brief  USBD Endpoint Config.
  * @param  None.
  * @retval None.
  */

void DFU_Init(void)
{
    /* Init setup packet buffer */
    /* Buffer range for setup packet -> [0 ~ 0x7] */
    USBD->STBUFSEG = SETUP_BUF_BASE;

    /*****************************************************/
    /* EP0 ==> control IN endpoint, address 0 */
    USBD_CONFIG_EP(EP0, USBD_CFG_CSTALL | USBD_CFG_EPMODE_IN | 0);
    /* Buffer range for EP0 */
    USBD_SET_EP_BUF_ADDR(EP0, EP0_BUF_BASE);

    /* EP1 ==> control OUT endpoint, address 0 */
    USBD_CONFIG_EP(EP1, USBD_CFG_CSTALL | USBD_CFG_EPMODE_OUT | 0);
    /* Buffer range for EP1 */
    USBD_SET_EP_BUF_ADDR(EP1, EP1_BUF_BASE);

    /* restore device default state */
    dfu_status.bStatus = STATUS_OK;
    dfu_status.bState = STATE_dfuIDLE;

    prog_struct.block_num = 0;
    prog_struct.data_len = 0;

}

void DFU_ClassRequest(void)
{
    uint8_t buf[8];
    uint32_t wValue, wLength;

    USBD_GetSetupPacket(buf);

    wValue  = (uint32_t)(buf[3] << 8 | buf[2]);
    wLength = (uint32_t)(buf[7] << 8 | buf[6]);

    if (buf[0] & 0x80)   /* request data transfer direction */
    {
        switch (buf[1])
        {
            case DFU_GETSTATUS:
            {
                if (dfu_status.bState == STATE_dfuDNLOAD_SYNC)
                {
                    s_u32CommandCount++;

                    if (s_u32CommandCount == 5)
                    {
                        uint32_t u32ProgAddr = (prog_struct.base_addr + (prog_struct.block_num * TRANSFER_SIZE));

                        if ((u32ProgAddr & (FMC_FLASH_PAGE_SIZE - 1)) == 0)
                            FMC_Erase_User(u32ProgAddr);

                        dfu_status.bState = STATE_dfuDNLOAD_IDLE;
                        WriteData(
                            u32ProgAddr,
                            (u32ProgAddr + prog_struct.data_len),
                            (uint32_t *)(uint32_t)prog_struct.buf);
                        //dfu_status.bStatus = STATUS_errWRITE;

                        s_u32CommandCount = 0;
                    }
                }

                if (dfu_status.bState == STATE_dfuDNLOAD_IDLE)
                {
                    s_u32CommandCount++;

                    if (s_u32CommandCount == 5)
                    {
                        dfu_status.bState = STATE_dfuMANIFEST_SYNC;
                        s_u32CommandCount = 0;
                    }
                }

                if (dfu_status.bState == STATE_dfuMANIFEST_SYNC)
                {
                    s_u32CommandCount++;

                    if (s_u32CommandCount == 5)
                    {
                        dfu_status.bState = STATE_dfuIDLE;
                        s_u32CommandCount = 0;
                    }
                }

                USBD_PrepareCtrlIn((uint8_t *)&dfu_status.bStatus, 6);
                USBD_PrepareCtrlOut(0, 0);
                break;
            }

            case DFU_GETSTATE:
            {
                USBD_PrepareCtrlIn((uint8_t *)&dfu_status.bState, 1);
                USBD_PrepareCtrlOut(0, 0);
                break;
            }

            case DFU_UPLOAD:
            {
                if (dfu_status.bState == STATE_dfuIDLE || dfu_status.bState == STATE_dfuUPLOAD_IDLE)
                {
                    if (wLength <= 0)
                    {
                        dfu_status.bState = STATE_dfuIDLE;
                        return;
                    }
                    else
                    {
                        if (dfu_status.bState == STATE_dfuIDLE)
                        {
                            dfu_status.bState = STATE_dfuUPLOAD_IDLE;
                        }

                        if (wValue > APROM_BLOCK_NUM)
                        {
                            dfu_status.bState = STATE_dfuIDLE;
                            USBD_PrepareCtrlIn(0, 0);
                            USBD_PrepareCtrlOut(0, 0);
                            break;
                        }

                        ReadData(
                            (prog_struct.base_addr + (wValue * TRANSFER_SIZE)),
                            (prog_struct.base_addr + (wValue * TRANSFER_SIZE) + wLength),
                            (uint32_t *)(uint32_t)prog_struct.buf);
                        USBD_PrepareCtrlIn((uint8_t *)prog_struct.buf, wLength);
                    }

                    USBD_PrepareCtrlOut(0, 0);
                }

                break;
            }

            default:
            {
                // Stall
                /* Setup error, stall the device */
                USBD_SetStall(0);
                break;
            }
        }
    }
    else
    {
        switch (buf[1])
        {
            case DFU_DETACH:
            {
                switch (dfu_status.bState)
                {
                    case STATE_dfuIDLE:
                    case STATE_dfuDNLOAD_SYNC:
                    case STATE_dfuDNLOAD_IDLE:
                    case STATE_dfuMANIFEST_SYNC:
                    case STATE_dfuUPLOAD_IDLE:
                        dfu_status.bStatus = STATUS_OK;
                        dfu_status.bState = STATE_dfuIDLE;
                        dfu_status.iString = 0; /* iString */
                        prog_struct.block_num = 0;
                        prog_struct.data_len = 0;
                        break;

                    default:
                        break;
                }

            }

            case DFU_DNLOAD:
            {
                switch (dfu_status.bState)
                {
                    case STATE_dfuIDLE:
                    case STATE_dfuDNLOAD_IDLE:
                        if (wLength > 0)
                        {
                            /* update the global length and block number */
                            prog_struct.block_num = (uint16_t)wValue;
                            prog_struct.data_len = (uint16_t)wLength;
                            dfu_status.bState = STATE_dfuDNLOAD_SYNC;
                        }
                        else
                        {
                            dfu_status.bState = STATE_dfuMANIFEST_SYNC;
                        }

                        /* enable EP0 prepare receive the buffer */
                        USBD_PrepareCtrlOut((uint8_t *)prog_struct.buf, wLength);
                        USBD_PrepareCtrlIn(0, 0);
                        break;
                }

                break;
            }

            case DFU_CLRSTATUS:
            {
                //  if (STATE_dfuERROR == dfu_status.bState) {
                dfu_status.bStatus = STATUS_OK;
                dfu_status.bState = STATE_dfuIDLE;
                // } //else {
                /* state Error */
                // dfu_status.bStatus = STATUS_errUNKNOWN;
                // dfu_status.bState = STATE_dfuERROR;
                // }

                dfu_status.iString = 0; /* iString: index = 0 */
                break;
            }

            case DFU_ABORT:
            {
                switch (dfu_status.bState)
                {
                    case STATE_dfuIDLE:
                    case STATE_dfuDNLOAD_SYNC:
                    case STATE_dfuDNLOAD_IDLE:
                    case STATE_dfuMANIFEST_SYNC:
                    case STATE_dfuUPLOAD_IDLE:
                        dfu_status.bStatus = STATUS_OK;
                        dfu_status.bState = STATE_dfuIDLE;
                        dfu_status.iString = 0; /* iString: index = 0 */

                        prog_struct.block_num = 0;
                        prog_struct.data_len = 0;
                        break;

                    default:
                        break;
                }
            }

            default:
            {
                /* Setup error, stall the device */
                USBD_SetStall(0);
                break;
            }

        }

    }
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/

