/**************************************************************************//**
 * @file     descriptors.c
 * @version  V1.00
 * @brief    HSUSBD driver source file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
/*!<Includes */
#include "NuMicro.h"
#include "hid_transfer.h"

/*!<USB HID Report Descriptor */
uint8_t HID_DeviceReportDescriptor[] =
{
    0x06, 0x00, 0xFF,   // Usage Page = 0xFF00 (Vendor Defined Page 1)
    0x09, 0x01,         // Usage (Vendor Usage 1)
    0xA1, 0x01,         // Collection (Application)
    0x19, 0x01,         // Usage Minimum
    0x29, 0x40,         // Usage Maximum //64 input usages total (0x01 to 0x40)
    0x15, 0x00,         // Logical Minimum (data bytes in the report may have minimum value = 0x00)
    0x26, 0xFF, 0x00,   // Logical Maximum (data bytes in the report may have maximum value = 0x00FF = unsigned 255)
    0x75, 0x08,         // Report Size: 8-bit field size
    0x95, 0x40,         // Report Count: Make sixty-four 8-bit fields (the next time the parser hits
    // an "Input", "Output", or "Feature" item)
    0x81, 0x00,         // Input (Data, Array, Abs): Instantiates input packet fields based on the
    // above report size, count, logical min/max, and usage.
    0x19, 0x01,         // Usage Minimum
    0x29, 0x40,         // Usage Maximum //64 output usages total (0x01 to 0x40)
    0x91, 0x00,         // Output (Data, Array, Abs): Instantiates output packet fields. Uses same
    // report size and count as "Input" fields, since nothing new/different was
    // specified to the parser since the "Input" item.
    0xC0                // End Collection
};

/*----------------------------------------------------------------------------*/
/*!<USB Device Descriptor */
uint8_t gu8DeviceDescriptor[] =
{
    LEN_DEVICE,         /* bLength */
    DESC_DEVICE,        /* bDescriptorType */
    0x00, 0x02,         /* bcdUSB (USB2.0) */
    0x00,               /* bDeviceClass */
    0x00,               /* bDeviceSubClass */
    0x00,               /* bDeviceProtocol */
    CEP_MAX_PKT_SIZE,   /* bMaxPacketSize0 */
    /* idVendor */
    (USBD_VID & 0x00FF),
    ((USBD_VID & 0xFF00) >> 8),
    /* idProduct */
    (USBD_PID & 0x00FF),
    ((USBD_PID & 0xFF00) >> 8),
    0x00, 0x00,     	/* bcdDevice */
    0x01,           	/* iManufacture */
    0x02,           	/* iProduct */
    0x00,           	/* iSerialNumber - no serial */
    0x01            	/* bNumConfigurations */
};

/*!<USB Configure Descriptor */
uint8_t gu8ConfigDescriptor[] =
{
    LEN_CONFIG,     /* bLength */
    DESC_CONFIG,    /* bDescriptorType */
    /* wTotalLength */
    ((LEN_CONFIG + LEN_INTERFACE + LEN_HID + LEN_ENDPOINT * 2) & 0x00FF),
    (((LEN_CONFIG + LEN_INTERFACE + LEN_HID + LEN_ENDPOINT * 2) & 0xFF00) >> 8),
    0x01,           /* bNumInterfaces */
    0x01,           /* bConfigurationValue */
    0x00,           /* iConfiguration */
    0x80 | (USBD_SELF_POWERED << 6) | (USBD_REMOTE_WAKEUP << 5),/* bmAttributes */
    USBD_MAX_POWER,         /* MaxPower */

    /* I/F descr: HID */
    LEN_INTERFACE,  /* bLength */
    DESC_INTERFACE, /* bDescriptorType */
    0x00,           /* bInterfaceNumber */
    0x00,           /* bAlternateSetting */
    0x02,           /* bNumEndpoints */
    0x03,           /* bInterfaceClass */
    0x00,           /* bInterfaceSubClass */
    0x00,           /* bInterfaceProtocol */
    0x00,           /* iInterface */

    /* HID Descriptor */
    LEN_HID,        /* Size of this descriptor in UINT8s. */
    DESC_HID,       /* HID descriptor type. */
    0x10, 0x01,     /* HID Class Spec. release number. */
    0x00,           /* H/W target country. */
    0x01,           /* Number of HID class descriptors to follow. */
    DESC_HID_RPT,   /* Descriptor type. */
    /* Total length of report descriptor. */
    (sizeof(HID_DeviceReportDescriptor) & 0x00FF),
    ((sizeof(HID_DeviceReportDescriptor) & 0xFF00) >> 8),

    /* EP Descriptor: interrupt in. */
    LEN_ENDPOINT,   /* bLength */
    DESC_ENDPOINT,  /* bDescriptorType */
    (INT_IN_EP_NUM | EP_INPUT), /* bEndpointAddress */
    EP_INT,         /* bmAttributes */
    /* wMaxPacketSize */
    (EPA_MAX_PKT_SIZE & 0x00FF),
    ((EPA_MAX_PKT_SIZE & 0xFF00) >> 8),
    HID_DEFAULT_INT_IN_INTERVAL,        /* bInterval */

    /* EP Descriptor: interrupt out. */
    LEN_ENDPOINT,   /* bLength */
    DESC_ENDPOINT,  /* bDescriptorType */
    (INT_OUT_EP_NUM | EP_OUTPUT),   /* bEndpointAddress */
    EP_INT,         /* bmAttributes */
    /* wMaxPacketSize */
    (EPB_MAX_PKT_SIZE & 0x00FF),
    ((EPB_MAX_PKT_SIZE & 0xFF00) >> 8),
    HID_DEFAULT_INT_IN_INTERVAL     /* bInterval */
};

/*!<USB Language String Descriptor */
uint8_t gu8StringLang[4] =
{
    4,              /* bLength */
    DESC_STRING,    /* bDescriptorType */
    0x09, 0x04
};

/*!<USB Vendor String Descriptor */
uint8_t gu8VendorStringDesc[] =
{
    16,
    DESC_STRING,
    'N', 0, 'u', 0, 'v', 0, 'o', 0, 't', 0, 'o', 0, 'n', 0
};

/*!<USB Product String Descriptor */
uint8_t gu8ProductStringDesc[] =
{
    16,
    DESC_STRING,
    'W', 0, 'P', 0, 'M', 0, ' ', 0, 'U', 0, 'S', 0, 'B', 0

};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/

