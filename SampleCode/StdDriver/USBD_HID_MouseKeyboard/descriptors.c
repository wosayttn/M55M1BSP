/******************************************************************************
 * @file     descriptors.c
 * @version  V3.00
 * @brief    USBD descriptor.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
/*!<Includes */
#include "NuMicro.h"
#include "hid_mousekeyboard.h"

/*!<USB HID Report Descriptor */
static uint8_t s_au8HIDMouseReportDescriptor[] =
{
    0x05, 0x01,             /* Usage Page(Generic Desktop Controls) */
    0x09, 0x02,             /* Usage(Mouse) */
    0xA1, 0x01,             /* Collection(Application) */
    0x09, 0x01,             /* Usage(Pointer) */
    0xA1, 0x00,             /* Collection(Physical) */
    0x05, 0x09,             /* Usage Page(Button) */
    0x19, 0x01,             /* Usage Minimum(0x1) */
    0x29, 0x03,             /* Usage Maximum(0x3) */
    0x15, 0x00,             /* Logical Minimum(0x0) */
    0x25, 0x01,             /* Logical Maximum(0x1) */
    0x75, 0x01,             /* Report Size(0x1) */
    0x95, 0x03,             /* Report Count(0x3) */
    0x81, 0x02,             /* Input(3 button bit) */
    0x75, 0x05,             /* Report Size(0x5) */
    0x95, 0x01,             /* Report Count(0x1) */
    0x81, 0x01,             /* Input(5 bit padding) */
    0x05, 0x01,             /* Usage Page(Generic Desktop Controls) */
    0x09, 0x30,             /* Usage(X) */
    0x09, 0x31,             /* Usage(Y) */
    0x09, 0x38,             /* Usage(Wheel) */
    0x15, 0x81,             /* Logical Minimum(0x81)(-127) */
    0x25, 0x7F,             /* Logical Maximum(0x7F)(127) */
    0x75, 0x08,             /* Report Size(0x8) */
    0x95, 0x03,             /* Report Count(0x3) */
    0x81, 0x06,             /* Input(1 byte wheel) */
    0xC0,                   /* End Collection */
    0xC0                    /* End Collection */
};

static uint8_t s_au8HIDKeyboardReportDescriptor[] =
{
    0x05, 0x01,         /* Usage Page(Generic Desktop Controls) */
    0x09, 0x06,         /* Usage(Keyboard) */
    0xA1, 0x01,         /* Collection(Application) */
    0x05, 0x07,         /* Usage Page(Keyboard/Keypad) */
    0x19, 0xE0,         /* Usage Minimum(0xE0) */
    0x29, 0xE7,         /* Usage Maximum(0xE7) */
    0x15, 0x00,         /* Logical Minimum(0x0) */
    0x25, 0x01,         /* Logical Maximum(0x1) */
    0x75, 0x01,         /* Report Size(0x1) */
    0x95, 0x08,         /* Report Count(0x8) */
    0x81, 0x02,         /* Input (Data) => Modifier byte */
    0x95, 0x01,         /* Report Count(0x1) */
    0x75, 0x08,         /* Report Size(0x8) */
    0x81, 0x01,         /* Input (Constant) => Reserved byte */
    0x95, 0x05,         /* Report Count(0x5) */
    0x75, 0x01,         /* Report Size(0x1) */
    0x05, 0x08,         /* Usage Page(LEDs) */
    0x19, 0x01,         /* Usage Minimum(0x1) */
    0x29, 0x05,         /* Usage Maximum(0x5) */
    0x91, 0x02,         /* Output (Data) => LED report */
    0x95, 0x01,         /* Report Count(0x1) */
    0x75, 0x03,         /* Report Size(0x3) */
    0x91, 0x01,         /* Output (Constant) => LED report padding */
    0x95, 0x06,         /* Report Count(0x6) */
    0x75, 0x08,         /* Report Size(0x8) */
    0x15, 0x00,         /* Logical Minimum(0x0) */
    0x25, 0x65,         /* Logical Maximum(0x65) */
    0x05, 0x07,         /* Usage Page(Keyboard/Keypad) */
    0x19, 0x00,         /* Usage Minimum(0x0) */
    0x29, 0x65,         /* Usage Maximum(0x65) */
    0x81, 0x00,         /* Input (Data) */
    0xC0                /* End Collection */
};

/*----------------------------------------------------------------------------*/
/*!<USB Device Descriptor */
static uint8_t s_au8DeviceDescriptor[] =
{
    LEN_DEVICE,     /* bLength */
    DESC_DEVICE,    /* bDescriptorType */
#ifdef SUPPORT_LPM
    0x01, 0x02,     /* bcdUSB >= 0x0201 to support LPM */
#else
    0x10, 0x01,     /* bcdUSB */
#endif
    0x00,           /* bDeviceClass */
    0x00,           /* bDeviceSubClass */
    0x00,           /* bDeviceProtocol */
    EP0_MAX_PKT_SIZE,   /* bMaxPacketSize0 */
    /* idVendor */
    USBD_VID & 0x00FF,
    (USBD_VID & 0xFF00) >> 8,
                        /* idProduct */
                        USBD_PID & 0x00FF,
                        (USBD_PID & 0xFF00) >> 8,
                        0x00, 0x00,     /* bcdDevice */
                        0x01,           /* iManufacture */
                        0x02,           /* iProduct */
                        0x00,           /* iSerialNumber - no serial */
                        0x01            /* bNumConfigurations */
};

/*!<USB Configure Descriptor */
static uint8_t s_au8ConfigDescriptor[] =
{
    LEN_CONFIG,     /* bLength */
    DESC_CONFIG,    /* bDescriptorType */
    // wTotalLength
    0x3B,
    0x00,
    0x02,           /* bNumInterfaces */
    0x01,           /* bConfigurationValue */
    0x00,           /* iConfiguration */
    0x80 | (USBD_SELF_POWERED << 6) | (USBD_REMOTE_WAKEUP << 5),/* bmAttributes */
    USBD_MAX_POWER,         /* MaxPower */

    /* I/F descr: HID - Mouse */
    LEN_INTERFACE,  /* bLength */
    DESC_INTERFACE, /* bDescriptorType */
    0x00,           /* bInterfaceNumber */
    0x00,           /* bAlternateSetting */
    0x01,           /* bNumEndpoints */
    0x03,           /* bInterfaceClass */
    0x00,           /* bInterfaceSubClass */
    HID_MOUSE,      /* bInterfaceProtocol */
    0x00,           /* iInterface */

    /* HID Descriptor */
    LEN_HID,        /* Size of this descriptor in UINT8s. */
    DESC_HID,       /* HID descriptor type. */
    0x10, 0x01,     /* HID Class Spec. release number. */
    0x00,           /* H/W target country. */
    0x01,           /* Number of HID class descriptors to follow. */
    DESC_HID_RPT,   /* Descriptor type. */
    /* Total length of report descriptor. */
    sizeof(s_au8HIDMouseReportDescriptor) & 0x00FF,
    (sizeof(s_au8HIDMouseReportDescriptor) & 0xFF00) >> 8,

                                                     /* EP Descriptor: interrupt in. */
                                                     LEN_ENDPOINT,   /* bLength */
                                                     DESC_ENDPOINT,  /* bDescriptorType */
                                                     (HID_MOUSE_EP_NUM | EP_INPUT), /* bEndpointAddress */
                                                     EP_INT,         /* bmAttributes */
                                                     /* wMaxPacketSize */
                                                     EP2_MAX_PKT_SIZE & 0x00FF,
                                                     (EP2_MAX_PKT_SIZE & 0xFF00) >> 8,
                                                     HID_DEFAULT_INT_IN_INTERVAL,     /* bInterval */

                                                     /* I/F descr: HID - Keyboard */
                                                     LEN_INTERFACE,  /* bLength */
                                                     DESC_INTERFACE, /* bDescriptorType */
                                                     0x01,           /* bInterfaceNumber */
                                                     0x00,           /* bAlternateSetting */
                                                     0x01,           /* bNumEndpoints */
                                                     0x03,           /* bInterfaceClass */
                                                     0x00,           /* bInterfaceSubClass */
                                                     HID_KEYBOARD,   /* bInterfaceProtocol */
                                                     0x00,           /* iInterface */

                                                     /* HID Descriptor */
                                                     LEN_HID,        /* Size of this descriptor in UINT8s. */
                                                     DESC_HID,       /* HID descriptor type. */
                                                     0x10, 0x01,     /* HID Class Spec. release number. */
                                                     0x00,           /* H/W target country. */
                                                     0x01,           /* Number of HID class descriptors to follow. */
                                                     DESC_HID_RPT,   /* Descriptor type. */
                                                     /* Total length of report descriptor. */
                                                     sizeof(s_au8HIDKeyboardReportDescriptor) & 0x00FF,
                                                     (sizeof(s_au8HIDKeyboardReportDescriptor) & 0xFF00) >> 8,

                                                     /* EP Descriptor: interrupt in. */
                                                     LEN_ENDPOINT,   /* bLength */
                                                     DESC_ENDPOINT,  /* bDescriptorType */
                                                     (HID_KB_EP_NUM | EP_INPUT), /* bEndpointAddress */
                                                     EP_INT,         /* bmAttributes */
                                                     /* wMaxPacketSize */
                                                     EP3_MAX_PKT_SIZE & 0x00FF,
                                                     (EP3_MAX_PKT_SIZE & 0xFF00) >> 8,
                                                     HID_DEFAULT_INT_IN_INTERVAL     /* bInterval */
};

/*!<USB Language String Descriptor */
static uint8_t s_au8StringLang[4] =
{
    4,              /* bLength */
    DESC_STRING,    /* bDescriptorType */
    0x09, 0x04
};

/*!<USB Vendor String Descriptor */
static uint8_t s_au8VendorStringDesc[] =
{
    16,
    DESC_STRING,
    'N', 0, 'u', 0, 'v', 0, 'o', 0, 't', 0, 'o', 0, 'n', 0
};

/*!<USB Product String Descriptor */
static uint8_t s_au8ProductStringDesc[] =
{
    24,
    DESC_STRING,
    'N', 0, 'u', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, ' ', 0, 'H', 0, 'I', 0, 'D', 0
};

#ifdef SUPPORT_LPM
/*!<USB BOS Descriptor */
static uint8_t s_au8BOSDescriptor[] =
{
    LEN_BOS,        /* bLength */
    DESC_BOS,       /* bDescriptorType */
    /* wTotalLength */
    0x0C & 0x00FF,
    (0x0C & 0xFF00) >> 8,
                    0x01,           /* bNumDeviceCaps */

                    /* Device Capability */
                    LEN_BOSCAP,     /* bLength */
                    DESC_CAPABILITY,/* bDescriptorType */
                    CAP_USB20_EXT,  /* bDevCapabilityType, 0x02 is USB 2.0 Extension */
                    0x06, 0x04, 0x00, 0x00  /* bmAttributes, 32 bits */
                    /* bit 0 : Reserved. Must 0. */
                    /* bit 1 : 1 to support LPM. */
                    /* bit 2 : 1 to support BSL & Alternat HIRD. */
                    /* bit 3 : 1 to recommend Baseline BESL. */
                    /* bit 4 : 1 to recommand Deep BESL. */
                    /* bit 11:8 : Recommend Baseline BESL value. Ignore by bit3 is zero. */
                    /* bit 15:12 : Recommend Deep BESL value. Ignore by bit4 is zero. */
                    /* bit 31:16 : Reserved. Must 0. */
};
#endif

static uint8_t *s_apu8UsbString[4] =
{
    s_au8StringLang,
    s_au8VendorStringDesc,
    s_au8ProductStringDesc,
    NULL
};

static uint8_t *s_apu8UsbHidReport[3] =
{
    s_au8HIDMouseReportDescriptor,
    s_au8HIDKeyboardReportDescriptor,
    NULL
};

static uint32_t s_au32UsbHidReportLen[3] =
{
    sizeof(s_au8HIDMouseReportDescriptor),
    sizeof(s_au8HIDKeyboardReportDescriptor),
    0
};

static uint32_t s_au32ConfigHidDescIdx[3] =
{
    (LEN_CONFIG + LEN_INTERFACE),
    (sizeof(s_au8ConfigDescriptor) - LEN_HID - LEN_ENDPOINT),
    0
};

const S_USBD_INFO_T gsInfo =
{
    (uint8_t *)s_au8DeviceDescriptor,
    (uint8_t *)s_au8ConfigDescriptor,
    (uint8_t **)s_apu8UsbString,
    (uint8_t **)s_apu8UsbHidReport,
#ifdef SUPPORT_LPM
    (uint8_t *)s_au8BOSDescriptor,
#else
    NULL,
#endif
    (uint32_t *)s_au32UsbHidReportLen,
    (uint32_t *)s_au32ConfigHidDescIdx
};
