/**************************************************************************//**
 * @file     hsusbd_user.c
 * @version  V1.00
 * @brief    HSUSBD driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "dfu_transfer.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup HSUSBD_Driver HSUSBD Driver
  @{
*/

/** @addtogroup HSUSBD_EXPORTED_FUNCTIONS HSUSBD Exported Functions
  @{
*/
/*--------------------------------------------------------------------------*/
/** @cond HIDDEN_SYMBOLS */
/* Global variables for Control Pipe */
S_HSUSBD_CMD_T gUsbCmd;

HSUSBD_VENDOR_REQ g_hsusbd_pfnVendorRequest = NULL;
HSUSBD_CLASS_REQ g_hsusbd_pfnClassRequest = NULL;
HSUSBD_SET_INTERFACE_REQ g_hsusbd_pfnSetInterface = NULL;
uint32_t g_u32HsEpStallLock = 0ul;       /* Bit map flag to lock specified EP when SET_FEATURE */

static uint8_t *g_hsusbd_CtrlInPointer = 0;
static uint32_t g_hsusbd_CtrlMaxPktSize = 64ul;
static uint8_t g_hsusbd_UsbConfig = 0ul;
static uint8_t g_hsusbd_UsbAltInterface = 0ul;
static uint8_t g_hsusbd_EnableTestMode = 0ul;
static uint8_t g_hsusbd_TestSelector = 0ul;

static uint8_t g_hsusbd_buf[12];

uint8_t volatile g_hsusbd_Configured = 0ul;
uint8_t g_hsusbd_CtrlZero = 0ul;
uint8_t g_hsusbd_UsbAddr = 0ul;
uint8_t g_hsusbd_ShortPacket = 0ul;
uint32_t volatile g_hsusbd_DmaDone = 0ul;
uint32_t g_hsusbd_CtrlInSize = 0ul;
/** @endcond HIDDEN_SYMBOLS */

/**
 * @brief       HSUSBD Initial
 *
 * @param[in]   param               Descriptor
 * @param[in]   pfnClassReq         Class Request Callback Function
 * @param[in]   pfnSetInterface     SetInterface Request Callback Function
 *
 * @retval      HSUSBD_OK           HSUSBD operation OK.
 * @retval      HSUSBD_ERR_TIMEOUT  HSUSBD operation abort due to timeout error.
 *
 * @details     This function is used to initial HSUSBD.
 */
int32_t HSUSBD_Open(S_HSUSBD_INFO_T *param, HSUSBD_CLASS_REQ pfnClassReq, HSUSBD_SET_INTERFACE_REQ pfnSetInterface)
{
    uint32_t u32TimeOutCnt = HSUSBD_TIMEOUT;

    /* Initial USB engine */
    /* Enable PHY */
    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk))
    {
        if (--u32TimeOutCnt == 0) return HSUSBD_ERR_TIMEOUT;
    }

    HSUSBD->OPER &= ~HSUSBD_OPER_HISPDEN_Msk;   /* full-speed */

    return HSUSBD_OK;
}

/**
 * @brief       HSUSBD Start
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is used to start transfer
 */
void HSUSBD_Start(void)
{
    HSUSBD->OPER = HSUSBD_OPER_HISPDEN_Msk;   /* high-speed */
    HSUSBD_CLR_SE0();
}

/**
 * @brief       Process Setup Packet
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is used to process Setup packet.
 */
void HSUSBD_ProcessSetupPacket(void)
{
    /* Setup packet process */
    gUsbCmd.bmRequestType = (uint8_t)(HSUSBD->SETUP1_0 & 0xfful);
    gUsbCmd.bRequest = (uint8_t)((HSUSBD->SETUP1_0 >> 8) & 0xfful);
    gUsbCmd.wValue = (uint16_t)HSUSBD->SETUP3_2;
    gUsbCmd.wIndex = (uint16_t)HSUSBD->SETUP5_4;
    gUsbCmd.wLength = (uint16_t)HSUSBD->SETUP7_6;

    /* USB device request in setup packet: offset 0, D[6..5]: 0=Standard, 1=Class, 2=Vendor, 3=Reserved */
    switch (gUsbCmd.bmRequestType & 0x60ul)
    {
        case REQ_STANDARD:
        {
            HSUSBD_StandardRequest();
            break;
        }

        case REQ_CLASS:
        {
            DFU_ClassRequest();
            break;
        }

        case REQ_VENDOR:
        {
            if (g_hsusbd_pfnVendorRequest != NULL)
            {
                g_hsusbd_pfnVendorRequest();
            }

            break;
        }

        default:
        {
            /* Setup error, stall the device */
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
            break;
        }
    }
}

/**
 * @brief       Get Descriptor request
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is used to process GetDescriptor request.
 */
int HSUSBD_GetDescriptor(void)
{
    uint32_t u32Len;
    int val = 0;
    u32Len = gUsbCmd.wLength;
    g_hsusbd_CtrlZero = (uint8_t)0ul;

    switch ((gUsbCmd.wValue & 0xff00ul) >> 8)
    {
        /* Get Device Descriptor */
        case DESC_DEVICE:
        {
            u32Len = Minimum(u32Len, LEN_DEVICE);
            HSUSBD_PrepareCtrlIn((uint8_t *)gu8DeviceDescriptor, u32Len);
            break;
        }

        /* Get Configuration Descriptor */
        case DESC_CONFIG:
        {
            uint32_t u32TotalLen;
            u32TotalLen = gu8ConfigDescriptor[3];
            u32TotalLen = gu8ConfigDescriptor[2] + (u32TotalLen << 8);
            u32Len = Minimum(u32Len, u32TotalLen);

            if ((u32Len % g_hsusbd_CtrlMaxPktSize) == 0ul)
            {
                g_hsusbd_CtrlZero = (uint8_t)1ul;
            }

            HSUSBD_PrepareCtrlIn((uint8_t *)gu8ConfigDescriptor, u32Len);
            break;
        }

        /* Get String Descriptor */
        case DESC_STRING:
        {
            switch (gUsbCmd.wValue & 0xfful)
            {
                case 0:
                    u32Len = Minimum(u32Len, gu8StringLang[0]);
                    HSUSBD_PrepareCtrlIn((uint8_t *)gu8StringLang, u32Len);
                    break;

                case 1:
                    u32Len = Minimum(u32Len, gu8VendorStringDesc[0]);
                    HSUSBD_PrepareCtrlIn((uint8_t *)gu8VendorStringDesc, u32Len);
                    break;

                case 2:
                    u32Len = Minimum(u32Len, gu8ProductStringDesc[0]);
                    HSUSBD_PrepareCtrlIn((uint8_t *)gu8ProductStringDesc, u32Len);
                    break;

                case 3:
                    u32Len = Minimum(u32Len, gu8StringSerial[0]);
                    HSUSBD_PrepareCtrlIn((uint8_t *)gu8StringSerial, u32Len);
                    break;

                default:
                    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
                    val = 1;
            }

            break;
        }

        default:
            /* Not support. Reply STALL. */
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
            val = 1;
            break;
    }

    return val;
}

/**
 * @brief       Process USB standard request
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is used to process USB Standard Request.
 */
void HSUSBD_StandardRequest(void)
{
    /* clear global variables for new request */
    g_hsusbd_CtrlInPointer = 0;
    g_hsusbd_CtrlInSize = 0ul;

    if ((gUsbCmd.bmRequestType & 0x80ul) == 0x80ul)   /* request data transfer direction */
    {
        /* Device to host */
        switch (gUsbCmd.bRequest)
        {
            case GET_CONFIGURATION:
            {
                /* Return current configuration setting */
                HSUSBD_PrepareCtrlIn((uint8_t *)&g_hsusbd_UsbConfig, 1ul);
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);
                break;
            }

            case GET_DESCRIPTOR:
            {
                if (!HSUSBD_GetDescriptor())
                {
                    HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);
                }

                break;
            }

            case GET_INTERFACE:
            {
                /* Return current interface setting */
                HSUSBD_PrepareCtrlIn((uint8_t *)&g_hsusbd_UsbAltInterface, 1ul);
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);
                break;
            }

            case GET_STATUS:
            {
                /* Device */
                if (gUsbCmd.bmRequestType == 0x80ul)
                {
                    if ((gu8ConfigDescriptor[7] & 0x40ul) == 0x40ul)
                    {
                        g_hsusbd_buf[0] = (uint8_t)1ul; /* Self-Powered */
                    }
                    else
                    {
                        g_hsusbd_buf[0] = (uint8_t)0ul; /* bus-Powered */
                    }
                }
                /* Interface */
                else if (gUsbCmd.bmRequestType == 0x81ul)
                {
                    g_hsusbd_buf[0] = (uint8_t)0ul;
                }
                /* Endpoint */
                else if (gUsbCmd.bmRequestType == 0x82ul)
                {
                    uint8_t ep = (uint8_t)(gUsbCmd.wIndex & 0xFul);
                    g_hsusbd_buf[0] = (uint8_t)HSUSBD_GetStall((uint32_t)ep) ? (uint8_t)1 : (uint8_t)0;
                }

                g_hsusbd_buf[1] = (uint8_t)0ul;
                HSUSBD_PrepareCtrlIn(g_hsusbd_buf, 2ul);
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);
                break;
            }

            default:
            {
                /* Setup error, stall the device */
                HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
                break;
            }
        }
    }
    else
    {
        /* Host to device */
        switch (gUsbCmd.bRequest)
        {
            case CLEAR_FEATURE:
            {
                if ((gUsbCmd.wValue & 0xfful) == FEATURE_ENDPOINT_HALT)
                {
                    uint32_t epNum, i;
                    /* EP number stall is not allow to be clear in MSC class "Error Recovery Test".
                       a flag: g_u32HsEpStallLock is added to support it */
                    epNum = (uint32_t)(gUsbCmd.wIndex & 0xFul);

                    for (i = 0ul; i < HSUSBD_MAX_EP; i++)
                    {
                        if ((((HSUSBD->EP[i].EPCFG & 0xf0ul) >> 4) == epNum) && ((g_u32HsEpStallLock & (1ul << i)) == 0ul))
                        {
                            HSUSBD->EP[i].EPRSPCTL = (HSUSBD->EP[i].EPRSPCTL & 0xeful) | HSUSBD_EP_RSPCTL_TOGGLE;
                        }
                    }
                }

                /* Status stage */
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
                HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_STSDONEIEN_Msk);
                break;
            }

            case SET_ADDRESS:
            {
                g_hsusbd_UsbAddr = (uint8_t)gUsbCmd.wValue;
                /* Status Stage */
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
                HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_STSDONEIEN_Msk);
                break;
            }

            case SET_CONFIGURATION:
            {
                g_hsusbd_UsbConfig = (uint8_t)gUsbCmd.wValue;
                g_hsusbd_Configured = (uint8_t)1ul;
                /* Status stage */
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
                HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_STSDONEIEN_Msk);
                break;
            }

            case SET_FEATURE:
            {
                if ((gUsbCmd.wValue & 0x3ul) == 2ul)    /* TEST_MODE */
                {
                    g_hsusbd_EnableTestMode = (uint8_t)1ul;
                    g_hsusbd_TestSelector = (uint8_t)(gUsbCmd.wIndex >> 8);
                }

                if ((gUsbCmd.wValue & 0x3ul) == 3ul)    /* HNP ebable */
                {
                    HSOTG->CTL |= (HSOTG_CTL_HNPREQEN_Msk | HSOTG_CTL_BUSREQ_Msk);
                }

                /* Status stage */
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
                HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_STSDONEIEN_Msk);
                break;
            }

            case SET_INTERFACE:
            {
                g_hsusbd_UsbAltInterface = (uint8_t)gUsbCmd.wValue;

                if (g_hsusbd_pfnSetInterface != NULL)
                {
                    g_hsusbd_pfnSetInterface((uint32_t)g_hsusbd_UsbAltInterface);
                }

                /* Status stage */
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
                HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_STSDONEIEN_Msk);
                break;
            }

            default:
            {
                /* Setup error, stall the device */
                HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
                break;
            }
        }
    }
}

/**
 * @brief       Update Device State
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is used to update Device state when Setup packet complete
 */
/** @cond HIDDEN_SYMBOLS */
#define TEST_J                  0x01ul
#define TEST_K                  0x02ul
#define TEST_SE0_NAK            0x03ul
#define TEST_PACKET             0x04ul
#define TEST_FORCE_ENABLE       0x05ul
/** @endcond HIDDEN_SYMBOLS */

void HSUSBD_UpdateDeviceState(void)
{
    switch (gUsbCmd.bRequest)
    {
        case SET_ADDRESS:
        {
            HSUSBD_SET_ADDR(g_hsusbd_UsbAddr);
            break;
        }

        case SET_CONFIGURATION:
        {
            if (g_hsusbd_UsbConfig == 0ul)
            {
                uint32_t volatile i;

                /* Reset PID DATA0 */
                for (i = 0ul; i < HSUSBD_MAX_EP; i++)
                {
                    if ((HSUSBD->EP[i].EPCFG & 0x1ul) == 0x1ul)
                    {
                        HSUSBD->EP[i].EPRSPCTL = HSUSBD_EP_RSPCTL_TOGGLE;
                    }
                }
            }

            break;
        }

        case SET_FEATURE:
        {
            if (gUsbCmd.wValue == FEATURE_ENDPOINT_HALT)
            {
                uint32_t idx;
                idx = (uint32_t)(gUsbCmd.wIndex & 0xFul);
                HSUSBD_SetStall(idx);
            }
            else if (g_hsusbd_EnableTestMode)
            {
                g_hsusbd_EnableTestMode = (uint8_t)0ul;

                if (g_hsusbd_TestSelector == TEST_J)
                {
                    HSUSBD->TEST = TEST_J;
                }
                else if (g_hsusbd_TestSelector == TEST_K)
                {
                    HSUSBD->TEST = TEST_K;
                }
                else if (g_hsusbd_TestSelector == TEST_SE0_NAK)
                {
                    HSUSBD->TEST = TEST_SE0_NAK;
                }
                else if (g_hsusbd_TestSelector == TEST_PACKET)
                {
                    HSUSBD->TEST = TEST_PACKET;
                }
                else if (g_hsusbd_TestSelector == TEST_FORCE_ENABLE)
                {
                    HSUSBD->TEST = TEST_FORCE_ENABLE;
                }
            }

            break;
        }

        case CLEAR_FEATURE:
        {
            if (gUsbCmd.wValue == FEATURE_ENDPOINT_HALT)
            {
                uint32_t idx;
                idx = (uint32_t)(gUsbCmd.wIndex & 0xFul);
                HSUSBD_ClearStall(idx);
            }

            break;
        }

        default:
            break;
    }
}

/**
 * @brief       Prepare Control IN transaction
 *
 * @param[in]   pu8Buf      Control IN data pointer
 * @param[in]   u32Size     IN transfer size
 *
 * @return      None
 *
 * @details     This function is used to prepare Control IN transfer
 */
void HSUSBD_PrepareCtrlIn(uint8_t pu8Buf[], uint32_t u32Size)
{
    g_hsusbd_CtrlInPointer = pu8Buf;
    g_hsusbd_CtrlInSize = u32Size;
}

/**
 * @brief       Start Control IN transfer
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is used to start Control IN
 */
void HSUSBD_CtrlIn(void)
{
    uint32_t i, cnt;
    uint8_t u8Value;

    if (g_hsusbd_CtrlInSize >= g_hsusbd_CtrlMaxPktSize)
    {
        /* Data size > MXPLD */
        cnt = g_hsusbd_CtrlMaxPktSize >> 2;

        for (i = 0ul; i < cnt; i++)
        {
            HSUSBD->CEPDAT = *(uint32_t *)g_hsusbd_CtrlInPointer;
            g_hsusbd_CtrlInPointer = (uint8_t *)(g_hsusbd_CtrlInPointer + 4ul);
        }

        HSUSBD_START_CEP_IN(g_hsusbd_CtrlMaxPktSize);
        g_hsusbd_CtrlInSize -= g_hsusbd_CtrlMaxPktSize;
    }
    else
    {
        /* Data size <= MXPLD */
        cnt = g_hsusbd_CtrlInSize;

        for (i = 0ul; i < cnt; i++)
        {
            u8Value = *(uint8_t *)(g_hsusbd_CtrlInPointer + i);
            outpb(&HSUSBD->CEPDAT, u8Value);
        }

        HSUSBD_START_CEP_IN(g_hsusbd_CtrlInSize);
        g_hsusbd_CtrlInPointer = 0;
        g_hsusbd_CtrlInSize = 0ul;
    }
}

/**
 * @brief       Start Control OUT transaction
 *
 * @param[in]   pu8Buf      Control OUT data pointer
 * @param[in]   u32Size     OUT transfer size
 *
 * @retval      HSUSBD_OK           HSUSBD operation OK.
 * @retval      HSUSBD_ERR_TIMEOUT  HSUSBD operation abort due to timeout error.
 *
 * @details     This function is used to start Control OUT transfer
 */
int32_t HSUSBD_CtrlOut(uint8_t pu8Buf[], uint32_t u32Size)
{
    uint32_t i;
    uint32_t u32TimeOutCnt = HSUSBD_TIMEOUT;

    while (1)
    {
        if ((HSUSBD->CEPINTSTS & HSUSBD_CEPINTSTS_RXPKIF_Msk) == HSUSBD_CEPINTSTS_RXPKIF_Msk)
        {
            for (i = 0ul; i < u32Size; i++)
            {
                pu8Buf[i] = inpb(&HSUSBD->CEPDAT);
            }

            HSUSBD->CEPINTSTS = HSUSBD_CEPINTSTS_RXPKIF_Msk;
            break;
        }

        if (--u32TimeOutCnt == 0) return HSUSBD_ERR_TIMEOUT;
    }

    return HSUSBD_OK;
}

/**
 * @brief       Clear all software flags
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is used to clear all software control flag
 */
void HSUSBD_SwReset(void)
{
    /* Reset all variables for protocol */
    g_hsusbd_UsbAddr = (uint8_t)0ul;
    g_hsusbd_DmaDone = 0ul;
    g_hsusbd_ShortPacket = (uint8_t)0ul;
    g_hsusbd_Configured = (uint8_t)0ul;
    /* Reset USB device address */
    HSUSBD_SET_ADDR(0ul);
}

/**
 * @brief       HSUSBD Set Vendor Request
 *
 * @param[in]   pfnVendorReq         Vendor Request Callback Function
 *
 * @return      None
 *
 * @details     This function is used to set HSUSBD vendor request callback function
 */
void HSUSBD_SetVendorRequest(HSUSBD_VENDOR_REQ pfnVendorReq)
{
    g_hsusbd_pfnVendorRequest = pfnVendorReq;
}

/*@}*/ /* end of group HSUSBD_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group HSUSBD_Driver */

/*@}*/ /* end of group Standard_Driver */

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/