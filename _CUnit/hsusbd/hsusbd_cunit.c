/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.                                        */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   hsusbd_cunit.c                                                                                        */
/*            The test function of HSUSBD for CUnit.                                                       */
/* Project:   M466                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"
#include "hsusbd_cunit.h"

extern uint8_t g_hsusbd_SetupPacket[8];

extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

#define __PALLADIUM___

int HSUSBD_Tests_Init(void)
{
    return 0;
}
int HSUSBD_Tests_Clean(void)
{
    return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: function_name                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               param - [in/out], parameter_description                                                   */
/*               param - [in/out], parameter_description                                                   */
/*                                                                                                         */
/* Returns:      the_value                                                                                 */
/* Side effects: the_effect                                                                                */
/* Description:                                                                                            */
/*               description                                                                               */
/*---------------------------------------------------------------------------------------------------------*/

CU_SuiteInfo HSUSBD_Suites[] =
{
    {"USBD MACRO", HSUSBD_Tests_Init, HSUSBD_Tests_Clean, NULL, NULL, HSUSBD_MacroTests},
    {"USBD API", HSUSBD_Tests_Init, HSUSBD_Tests_Clean, NULL, NULL, HSUSBD_ApiTests},
    {"USBD CONSTANT", HSUSBD_Tests_Init, HSUSBD_Tests_Clean, NULL, NULL, HSUSBD_ConstantTest},
    CU_SUITE_INFO_NULL
};



void MACRO_Maximum()
{
    CU_ASSERT(Maximum(3, 5) == 5);
    CU_ASSERT(Maximum(3500, 1500) == 3500);
}

void MACRO_Minimum()
{
    CU_ASSERT(Minimum(3, 5) == 3);
    CU_ASSERT(Minimum(3500, 1500) == 1500);
    /* Reset HSUSBD */
    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_ENABLE_DISABLE_USB()
{
    CU_ASSERT_FALSE(HSUSBD->PHYCTL & 0x300);
    HSUSBD_ENABLE_USB();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk)) {};

    CU_ASSERT((HSUSBD->PHYCTL & 0x300) == (HSUSBD_PHYCTL_PHYEN_Msk | HSUSBD_PHYCTL_DPPUEN_Msk));

    HSUSBD_DISABLE_USB();

    CU_ASSERT((HSUSBD->PHYCTL & 0x300) == 0x200);

    /* Reset HSUSBD */
    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_ENABLE_DISABLE_PHY()
{
    HSUSBD_DISABLE_PHY();
    CU_ASSERT_FALSE(HSUSBD->PHYCTL & 0x200);
    HSUSBD_ENABLE_PHY();
    CU_ASSERT((HSUSBD->PHYCTL & 0x300) == 0x200);
    HSUSBD_DISABLE_PHY();
    CU_ASSERT_FALSE(HSUSBD->PHYCTL & 0x200);
    /* Reset HSUSBD */
    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_SET_CLEAR_SE0()
{
    CU_ASSERT_FALSE(HSUSBD->PHYCTL & 0x100);
    HSUSBD_CLR_SE0();
    CU_ASSERT(HSUSBD->PHYCTL & 0x100);
    HSUSBD_SET_SE0();
    CU_ASSERT_FALSE(HSUSBD->PHYCTL & 0x100);

    /* Reset HSUSBD */
    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_SET_GET_ADDR()
{
    int volatile i;
    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    for (i = 0; i < 0x1000; i++) {};

    CU_ASSERT_FALSE(HSUSBD->FADDR);

    HSUSBD_SET_ADDR(0x55);

    CU_ASSERT((HSUSBD->FADDR & 0x7F) == 0x55);

    CU_ASSERT(HSUSBD_GET_ADDR() == 0x55);

    HSUSBD_SET_ADDR(0xAA);

    CU_ASSERT((HSUSBD->FADDR & 0x7F) == 0x2A);

    CU_ASSERT(HSUSBD_GET_ADDR() == 0x2A);

    /* Reset HSUSBD */

    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_ENABLE_INT()
{
    uint32_t i;

    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    for (i = 0; i < (HSUSBD_MAX_EP + 2); i++)
    {
        HSUSBD_ENABLE_USB_INT(1 << i);
        CU_ASSERT(HSUSBD->GINTEN & (1 << i));
    }

    for (i = 0; i < 6; i++)
    {
        HSUSBD_ENABLE_BUS_INT(1 << i);
        CU_ASSERT(HSUSBD->BUSINTEN & (1 << i));
    }

    HSUSBD_ENABLE_BUS_INT(0x100);
    CU_ASSERT(HSUSBD->BUSINTEN & 0x100);

    HSUSBD_ENABLE_BUS_INT(0x200);
    CU_ASSERT(HSUSBD->BUSINTEN & 0x200);
}

void MACRO_HSUSBD_GET_CLR_INT_FLAG()
{
    CU_ASSERT(HSUSBD_GET_BUS_INT_FLAG() == 0x100); // Plug-in to test
    HSUSBD_CLR_BUS_INT_FLAG(0x37f);
    CU_ASSERT(HSUSBD_GET_BUS_INT_FLAG() == 0x00);
}

void MACRO_HSUSBD_ENABLE_CEP_INT()
{
    uint32_t i;

    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    for (i = 0; i < 13; i++)
    {
        HSUSBD_ENABLE_CEP_INT(1 << i);
        CU_ASSERT(HSUSBD->CEPINTEN & (1 << i));
    }

    /* Reset HSUSBD */
    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_CLR_CEP_INT_FLAG()
{
    CU_ASSERT(HSUSBD->CEPINTSTS == 0x1800);
    HSUSBD_CLR_CEP_INT_FLAG(0x1fff);
    CU_ASSERT(HSUSBD->CEPINTSTS == 0x1800);
}

void MACRO_HSUSBD_SET_CEP_STATE()
{
    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
    CU_ASSERT((HSUSBD->CEPCTL & HSUSBD_CEPCTL_NAKCLR_Msk) == HSUSBD_CEPCTL_NAKCLR);
    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALL);
    CU_ASSERT((HSUSBD->CEPCTL & HSUSBD_CEPCTL_STALLEN_Msk) == HSUSBD_CEPCTL_STALL);
    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_ZEROLEN);
    CU_ASSERT((HSUSBD->CEPCTL & HSUSBD_CEPCTL_ZEROLEN_Msk) == HSUSBD_CEPCTL_ZEROLEN);
    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_FLUSH);
    CU_ASSERT((HSUSBD->CEPCTL & HSUSBD_CEPCTL_FLUSH_Msk) == 0);
    /* Reset HSUSBD */

    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_START_CEP_IN()
{
    HSUSBD_ENABLE_PHY();
    /* wait PHY clock ready */

    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    HSUSBD_START_CEP_IN(0xff);
    CU_ASSERT(HSUSBD->CEPTXCNT == 0xff);
    /* Reset HSUSBD */
    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_SET_MAX_PAYLOAD()
{
    uint32_t volatile i;

    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    for (i = 0; i < HSUSBD_MAX_EP; i++)
    {
        CU_ASSERT(HSUSBD->EP[i].EPMPS == 0);
        HSUSBD_SET_MAX_PAYLOAD(i, 0x7ff);
        CU_ASSERT(HSUSBD->EP[i].EPMPS == 0x7ff);
    }

    /* Reset HSUSBD */

    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_ENABLE_EP_INT()
{
    uint32_t volatile i, j;

    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    for (i = 0; i < HSUSBD_MAX_EP; i++)
    {
        for (j = 0; j < 13; j++)
        {
            HSUSBD_ENABLE_EP_INT(i, 1 << j);
            CU_ASSERT(HSUSBD->EP[i].EPINTEN & (1 << j));
        }
    }

    /* Reset HSUSBD */
    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_GET_CLR_EP_INT_FLAG()
{
    uint32_t volatile i;

    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    for (i = 0; i < HSUSBD_MAX_EP; i++)
    {
        CU_ASSERT(HSUSBD_GET_EP_INT_FLAG(i) == 0x3);
        HSUSBD_CLR_EP_INT_FLAG(i, 0x1fff);
        CU_ASSERT(HSUSBD_GET_EP_INT_FLAG(i) == 0x3);
    }

    /* Reset HSUSBD */

    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_SET_DMA_LEN()
{
    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    HSUSBD_SET_DMA_LEN(0xfffff);
    CU_ASSERT(HSUSBD->DMACNT == 0xfffff);
    HSUSBD_ResetDMA();
    CU_ASSERT(HSUSBD->DMACNT == 0x0);
    /* Reset HSUSBD */

    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_SET_DMA_ADDR()
{
    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    HSUSBD_SET_DMA_ADDR(0x20001000);
    CU_ASSERT(HSUSBD->DMAADDR == 0x20001000);
    HSUSBD_ResetDMA();
    CU_ASSERT(HSUSBD->DMAADDR == 0x20001000);
    /* Reset HSUSBD */

    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_SET_DMA_READ()
{
    uint32_t volatile i;

    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    for (i = 0; i < 0x1000; i++) {};

    for (i = 0; i < 0x10; i++)
    {
        HSUSBD_SET_DMA_READ(i);
        CU_ASSERT(HSUSBD->DMACTL == (0x110 | i));
    }

    HSUSBD_ResetDMA();
    CU_ASSERT(HSUSBD->DMACTL == 0);
    /* Reset HSUSBD */
    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_SET_DMA_WRITE()
{
    uint32_t volatile i;

    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    for (i = 0; i < 0x1000; i++) {};

    for (i = 0; i < 0x10; i++)
    {
        HSUSBD_SET_DMA_WRITE(i);
        CU_ASSERT(HSUSBD->DMACTL == (0x000 | i));
    }

    HSUSBD_ResetDMA();
    CU_ASSERT(HSUSBD->DMACTL == 0);
    /* Reset HSUSBD */
    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_ENABLE_DMA()
{
    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk)) {};

    CU_ASSERT_FALSE(HSUSBD->DMACTL == HSUSBD_DMACTL_DMAEN_Msk);

    HSUSBD_ENABLE_DMA();

    CU_ASSERT(HSUSBD->DMACTL & HSUSBD_DMACTL_DMAEN_Msk);

    HSUSBD_ResetDMA();

    CU_ASSERT(HSUSBD->DMACTL == 0);

    /* Reset HSUSBD */
    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_IS_ATTACHED()
{
    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk)) {};

    CU_ASSERT(HSUSBD_IS_ATTACHED() == (HSUSBD->PHYCTL & 0x80000000)); // Plug-in to test

    /* Reset HSUSBD */

    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_ENABLE_DISABLE_BCD()
{
    HSUSBD_DISABLE_BCD();
    CU_ASSERT_FALSE(HSUSBD->BCDC & 0x1);
    HSUSBD_ENABLE_BCD();
    CU_ASSERT((HSUSBD->BCDC & 0x1) == 0x1);
    HSUSBD_DISABLE_BCD();
    CU_ASSERT_FALSE(HSUSBD->BCDC & 0x1);
    /* Reset HSUSBD */
    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_ENABLE_DISABLE_LPM()
{
    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    HSUSBD_DISABLE_LPM();
    CU_ASSERT_FALSE(HSUSBD->LPMCSR & 0x1000);
    HSUSBD_ENABLE_LPM();
    CU_ASSERT((HSUSBD->LPMCSR & 0x1000) == 0x1000);
    HSUSBD_DISABLE_LPM();
    CU_ASSERT_FALSE(HSUSBD->LPMCSR & 0x1000);
    /* Reset HSUSBD */

    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_MemCopy()
{
    uint8_t u8Src[20];
    uint8_t u8Dest[20];
    uint32_t u32Count;

    for (u32Count = 0; u32Count < 20; u32Count++)
    {
        u8Src[u32Count] = u32Count + 1;
        u8Dest[u32Count] = 0;
    }

    HSUSBD_MemCopy(u8Dest, u8Src, 20);

    for (u32Count = 0; u32Count < 20; u32Count++)
    {
        CU_ASSERT(u8Src[u32Count] == u8Dest[u32Count])
    }
}

void MACRO_HSUSBD_Set_Clear_Get_Ep_Stall()
{
    uint32_t u32EpCount;

    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    HSUSBD_SetEpStall(0xff);
    CU_ASSERT(HSUSBD->CEPCTL & 0x2);
    CU_ASSERT(HSUSBD->CEPCTL == HSUSBD_CEPCTL_STALL);

    for (u32EpCount = 0; u32EpCount < HSUSBD_MAX_EP; u32EpCount++)
    {
        HSUSBD_SetEpStall(u32EpCount);
        CU_ASSERT(HSUSBD->EP[u32EpCount].EPRSPCTL == 0x10);
        CU_ASSERT(HSUSBD->EP[u32EpCount].EPRSPCTL == HSUSBD_EP_RSPCTL_HALT);
        CU_ASSERT_TRUE(HSUSBD_GetEpStall(u32EpCount));
        HSUSBD_ClearEpStall(u32EpCount);
        CU_ASSERT(HSUSBD->EP[u32EpCount].EPRSPCTL == 0);
        CU_ASSERT_FALSE(HSUSBD_GetEpStall(u32EpCount));
    }

    /* Reset HSUSBD */

    SYS_ResetModule(SYS_HSUSBD0RST);
}

void MACRO_HSUSBD_SetStall_ClearStall_GetStall()
{
    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (!(HSUSBD->PHYCTL & HSUSBD_PHYCTL_PHYCLKSTB_Msk));

    HSUSBD_SetStall(0x0);
    CU_ASSERT(HSUSBD->CEPCTL & 0x2);
    CU_ASSERT(HSUSBD->CEPCTL == HSUSBD_CEPCTL_STALL);

    HSUSBD->EP[EPB].EPCFG = 0xf0;
    HSUSBD_SetStall(0xF);
    CU_ASSERT(HSUSBD->EP[EPB].EPRSPCTL & 0x10);
    CU_ASSERT_TRUE(HSUSBD_GetStall(0xF));

    HSUSBD->EP[EPG].EPCFG = 0x20;
    HSUSBD_SetStall(0x2);
    CU_ASSERT(HSUSBD->EP[EPG].EPRSPCTL & 0x10);
    CU_ASSERT_TRUE(HSUSBD_GetStall(0x2));

    HSUSBD->EP[EPL].EPCFG = 0x70;
    HSUSBD_SetStall(0x7);
    CU_ASSERT(HSUSBD->EP[EPL].EPRSPCTL & 0x10);
    CU_ASSERT_TRUE(HSUSBD_GetStall(0x7));

    HSUSBD_ClearStall(0xF);
    CU_ASSERT((HSUSBD->EP[EPB].EPRSPCTL & 0x10) == 0);
    CU_ASSERT_FALSE(HSUSBD_GetStall(0xF));

    HSUSBD_ClearStall(0x2);
    CU_ASSERT((HSUSBD->EP[EPG].EPRSPCTL & 0x10) == 0);
    CU_ASSERT_FALSE(HSUSBD_GetStall(0x2));

    HSUSBD_ClearStall(0x7);
    CU_ASSERT((HSUSBD->EP[EPL].EPRSPCTL & 0x10) == 0);
    CU_ASSERT_FALSE(HSUSBD_GetStall(0x7));

    /* Reset HSUSBD */
    SYS_ResetModule(SYS_HSUSBD0RST);
}



#define MAX_PACKET_SIZE 20

#define CEP_BUF_BASE    0
#define CEP_BUF_LEN     64
#define EPA_BUF_BASE    0x200
#define EPA_BUF_LEN     512

#define EP0_BUF_OFFSET 0x80
#define EP1_BUF_OFFSET 0x100
#define EP2_BUF_OFFSET 0x180
#define DEVICE_ADDRESS 0x25
#define CONFIGURATION_VALUE 5
#define ALT_INTERFACE 2

/*!<USB Device Descriptor */
__attribute__((aligned(4))) uint8_t gu8DeviceDescriptor[] =
{
    LEN_DEVICE,     /* bLength */
    DESC_DEVICE,    /* bDescriptorType */
    0x10, 0x01,     /* bcdUSB */
    0x00,           /* bDeviceClass */
    0x00,           /* bDeviceSubClass */
    0x00,           /* bDeviceProtocol */
    MAX_PACKET_SIZE,   /* bMaxPacketSize0 */
    /* idVendor */
    0x0416 & 0x00FF,
    (0x0416 & 0xFF00) >> 8,
                      /* idProduct */
                      0x8248 & 0x00FF,
                      (0x8248 & 0xFF00) >> 8,
                      0x00, 0x00,     /* bcdDevice */
                      0x01,           /* iManufacture */
                      0x02,           /* iProduct */
                      0x03,           /* iSerialNumber */
                      0x01            /* bNumConfigurations */
};

__attribute__((aligned(4))) uint8_t gu8QualifierDescriptor[] =
{
    LEN_QUALIFIER,  /* bLength */
    DESC_QUALIFIER, /* bDescriptorType */
    0x10, 0x01,     /* bcdUSB */
    0x00,           /* bDeviceClass */
    0x00,           /* bDeviceSubClass */
    0x00,           /* bDeviceProtocol */
    0x40,           /* bMaxPacketSize0 */
    0x01,           /* bNumConfigurations */
    0x00
};



/*!<USB HID Report Descriptor */
__attribute__((aligned(4))) uint8_t HID_MouseReportDescriptor[] =
{
    0x05, 0x01,     /* Usage Page(Generic Desktop Controls) */
    0x09, 0x02,     /* Usage(Mouse) */
    0xA1, 0x01,     /* Collection(Application) */
    0x09, 0x01,     /* Usage(Pointer) */
    0xA1, 0x00,     /* Collection(Physical) */
    0x05, 0x09,     /* Usage Page(Button) */
    0x19, 0x01,     /* Usage Minimum(0x1) */
    0x29, 0x03,     /* Usage Maximum(0x3) */
    0x15, 0x00,     /* Logical Minimum(0x0) */
    0x25, 0x01,     /* Logical Maximum(0x1) */
    0x75, 0x01,     /* Report Size(0x1) */
    0x95, 0x03,     /* Report Count(0x3) */
    0x81, 0x02,     /* Input(3 button bit) */
    0x75, 0x05,     /* Report Size(0x5) */
    0x95, 0x01,     /* Report Count(0x1) */
    0x81, 0x01,     /* Input(5 bit padding) */
    0x05, 0x01,     /* Usage Page(Generic Desktop Controls) */
    0x09, 0x30,     /* Usage(X) */
    0x09, 0x31,     /* Usage(Y) */
    0x09, 0x38,     /* Usage(Wheel) */
    0x15, 0x81,     /* Logical Minimum(0x81)(-127) */
    0x25, 0x7F,     /* Logical Maximum(0x7F)(127) */
    0x75, 0x08,     /* Report Size(0x8) */
    0x95, 0x03,     /* Report Count(0x3) */
    0x81, 0x06,     /* Input(1 byte wheel) */
    0xC0,           /* End Collection */
    0xC0            /* End Collection */
};

/* Constant definitions for HID mouse */
#define LEN_CONFIG_AND_SUBORDINATE      (LEN_CONFIG+LEN_INTERFACE+LEN_HID+LEN_ENDPOINT)
#define HID_DEFAULT_INT_IN_INTERVAL     20
#define HSUSBD_SELF_POWERED               0
#define HSUSBD_REMOTE_WAKEUP              0
#define HSUSBD_MAX_POWER                  50  /* The unit is in 2mA. ex: 50 * 2mA = 100mA */
#define INT_IN_EP_NUM   0x01    /* Define the interrupt In EP number */
#define EP2_MAX_PKT_SIZE    8
/* USB HID Interface Class protocol */
#define HID_NONE            0x00
#define HID_KEYBOARD        0x01
#define HID_MOUSE           0x02

/*!<USB Configuration Descriptor for HID */
__attribute__((aligned(4))) uint8_t gu8ConfigDescriptorHID[] =
{
    LEN_CONFIG,     /* bLength */
    DESC_CONFIG,    /* bDescriptorType */
    /* wTotalLength */
    LEN_CONFIG_AND_SUBORDINATE & 0x00FF,
    (LEN_CONFIG_AND_SUBORDINATE & 0xFF00) >> 8,
                                          0x01,           /* bNumInterfaces */
                                          0x01,           /* bConfigurationValue */
                                          0x00,           /* iConfiguration */
                                          0x80 | (HSUSBD_SELF_POWERED << 6) | (HSUSBD_REMOTE_WAKEUP << 5),/* bmAttributes */
                                          HSUSBD_MAX_POWER,         /* MaxPower */

                                          /* I/F descr: HID */
                                          LEN_INTERFACE,  /* bLength */
                                          DESC_INTERFACE, /* bDescriptorType */
                                          0x00,           /* bInterfaceNumber */
                                          0x00,           /* bAlternateSetting */
                                          0x01,           /* bNumEndpoints */
                                          0x03,           /* bInterfaceClass */
                                          0x01,           /* bInterfaceSubClass */
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
                                          sizeof(HID_MouseReportDescriptor) & 0x00FF,
                                          (sizeof(HID_MouseReportDescriptor) & 0xFF00) >> 8,

                                          /* EP Descriptor: interrupt in. */
                                          LEN_ENDPOINT,   /* bLength */
                                          DESC_ENDPOINT,  /* bDescriptorType */
                                          (INT_IN_EP_NUM | EP_INPUT), /* bEndpointAddress */
                                          EP_INT,         /* bmAttributes */
                                          /* wMaxPacketSize */
                                          EP2_MAX_PKT_SIZE & 0x00FF,
                                          (EP2_MAX_PKT_SIZE & 0xFF00) >> 8,
                                          HID_DEFAULT_INT_IN_INTERVAL     /* bInterval */
};

__attribute__((aligned(4))) uint8_t gu8ConfigDescriptorFS_HID[] =
{
    LEN_CONFIG,     /* bLength */
    DESC_CONFIG,    /* bDescriptorType */
    /* wTotalLength */
    LEN_CONFIG_AND_SUBORDINATE & 0x00FF,
    (LEN_CONFIG_AND_SUBORDINATE & 0xFF00) >> 8,
                                          0x01,           /* bNumInterfaces */
                                          0x01,           /* bConfigurationValue */
                                          0x00,           /* iConfiguration */
                                          0x80 | (HSUSBD_SELF_POWERED << 6) | (HSUSBD_REMOTE_WAKEUP << 5),/* bmAttributes */
                                          HSUSBD_MAX_POWER,         /* MaxPower */

                                          /* I/F descr: HID */
                                          LEN_INTERFACE,  /* bLength */
                                          DESC_INTERFACE, /* bDescriptorType */
                                          0x00,           /* bInterfaceNumber */
                                          0x00,           /* bAlternateSetting */
                                          0x01,           /* bNumEndpoints */
                                          0x03,           /* bInterfaceClass */
                                          0x01,           /* bInterfaceSubClass */
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
                                          sizeof(HID_MouseReportDescriptor) & 0x00FF,
                                          (sizeof(HID_MouseReportDescriptor) & 0xFF00) >> 8,

                                          /* EP Descriptor: interrupt in. */
                                          LEN_ENDPOINT,   /* bLength */
                                          DESC_ENDPOINT,  /* bDescriptorType */
                                          (INT_IN_EP_NUM | EP_INPUT), /* bEndpointAddress */
                                          EP_INT,         /* bmAttributes */
                                          /* wMaxPacketSize */
                                          EP2_MAX_PKT_SIZE & 0x00FF,
                                          (EP2_MAX_PKT_SIZE & 0xFF00) >> 8,
                                          HID_DEFAULT_INT_IN_INTERVAL     /* bInterval */
};

__attribute__((aligned(4))) uint8_t gu8OtherConfigDescriptorHS_HID[] =
{
    LEN_CONFIG,     /* bLength */
    DESC_CONFIG,    /* bDescriptorType */
    /* wTotalLength */
    LEN_CONFIG_AND_SUBORDINATE & 0x00FF,
    (LEN_CONFIG_AND_SUBORDINATE & 0xFF00) >> 8,
                                          0x01,           /* bNumInterfaces */
                                          0x01,           /* bConfigurationValue */
                                          0x00,           /* iConfiguration */
                                          0x80 | (HSUSBD_SELF_POWERED << 6) | (HSUSBD_REMOTE_WAKEUP << 5),/* bmAttributes */
                                          HSUSBD_MAX_POWER,         /* MaxPower */

                                          /* I/F descr: HID */
                                          LEN_INTERFACE,  /* bLength */
                                          DESC_INTERFACE, /* bDescriptorType */
                                          0x00,           /* bInterfaceNumber */
                                          0x00,           /* bAlternateSetting */
                                          0x01,           /* bNumEndpoints */
                                          0x03,           /* bInterfaceClass */
                                          0x01,           /* bInterfaceSubClass */
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
                                          sizeof(HID_MouseReportDescriptor) & 0x00FF,
                                          (sizeof(HID_MouseReportDescriptor) & 0xFF00) >> 8,

                                          /* EP Descriptor: interrupt in. */
                                          LEN_ENDPOINT,   /* bLength */
                                          DESC_ENDPOINT,  /* bDescriptorType */
                                          (INT_IN_EP_NUM | EP_INPUT), /* bEndpointAddress */
                                          EP_INT,         /* bmAttributes */
                                          /* wMaxPacketSize */
                                          EP2_MAX_PKT_SIZE & 0x00FF,
                                          (EP2_MAX_PKT_SIZE & 0xFF00) >> 8,
                                          HID_DEFAULT_INT_IN_INTERVAL     /* bInterval */
};

__attribute__((aligned(4))) uint8_t gu8OtherConfigDescriptorFS_HID[] =
{
    LEN_CONFIG,     /* bLength */
    DESC_CONFIG,    /* bDescriptorType */
    /* wTotalLength */
    LEN_CONFIG_AND_SUBORDINATE & 0x00FF,
    (LEN_CONFIG_AND_SUBORDINATE & 0xFF00) >> 8,
                                          0x01,           /* bNumInterfaces */
                                          0x01,           /* bConfigurationValue */
                                          0x00,           /* iConfiguration */
                                          0x80 | (HSUSBD_SELF_POWERED << 6) | (HSUSBD_REMOTE_WAKEUP << 5),/* bmAttributes */
                                          HSUSBD_MAX_POWER,         /* MaxPower */

                                          /* I/F descr: HID */
                                          LEN_INTERFACE,  /* bLength */
                                          DESC_INTERFACE, /* bDescriptorType */
                                          0x00,           /* bInterfaceNumber */
                                          0x00,           /* bAlternateSetting */
                                          0x01,           /* bNumEndpoints */
                                          0x03,           /* bInterfaceClass */
                                          0x01,           /* bInterfaceSubClass */
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
                                          sizeof(HID_MouseReportDescriptor) & 0x00FF,
                                          (sizeof(HID_MouseReportDescriptor) & 0xFF00) >> 8,

                                          /* EP Descriptor: interrupt in. */
                                          LEN_ENDPOINT,   /* bLength */
                                          DESC_ENDPOINT,  /* bDescriptorType */
                                          (INT_IN_EP_NUM | EP_INPUT), /* bEndpointAddress */
                                          EP_INT,         /* bmAttributes */
                                          /* wMaxPacketSize */
                                          EP2_MAX_PKT_SIZE & 0x00FF,
                                          (EP2_MAX_PKT_SIZE & 0xFF00) >> 8,
                                          HID_DEFAULT_INT_IN_INTERVAL     /* bInterval */
};

/*!<USB Configuration Descriptor for MSC */
__attribute__((aligned(4))) uint8_t gu8ConfigDescriptorMSC[] =
{
    LEN_CONFIG,                                         // bLength
    DESC_CONFIG,                                        // bDescriptorType
    (LEN_CONFIG + LEN_INTERFACE + LEN_ENDPOINT * 2), 0x00, // wTotalLength
    0x01,                                               // bNumInterfaces
    0x01,                                               // bConfigurationValue
    0x00,                                               // iConfiguration
    0xC0,                                               // bmAttributes
    0x32,                                               // MaxPower

    /* const BYTE cbyInterfaceDescriptor[LEN_INTERFACE] = */
    LEN_INTERFACE,  // bLength
    DESC_INTERFACE, // bDescriptorType
    0x00,     // bInterfaceNumber
    0x00,     // bAlternateSetting
    0x02,     // bNumEndpoints
    0x08,     // bInterfaceClass
    0x05,     // bInterfaceSubClass
    0x50,     // bInterfaceProtocol
    0x00,     // iInterface

    /* const BYTE cbyEndpointDescriptor1[LEN_ENDPOINT] = */
    LEN_ENDPOINT,           // bLength
    DESC_ENDPOINT,          // bDescriptorType
    0x82,                   // bEndpointAddress
    EP_BULK,                // bmAttributes
    MAX_PACKET_SIZE, 0x00,  // wMaxPacketSize
    0x00,                   // bInterval

    /* const BYTE cbyEndpointDescriptor2[LEN_ENDPOINT] = */
    LEN_ENDPOINT,           // bLength
    DESC_ENDPOINT,          // bDescriptorType
    0x03,                   // bEndpointAddress
    EP_BULK,                // bmAttributes
    MAX_PACKET_SIZE, 0x00,  // wMaxPacketSize
    0x00                    // bInterval
};

__attribute__((aligned(4))) uint8_t gu8ConfigDescriptorFS_MSC[] =
{
    LEN_CONFIG,                                         // bLength
    DESC_CONFIG,                                        // bDescriptorType
    (LEN_CONFIG + LEN_INTERFACE + LEN_ENDPOINT * 2), 0x00, // wTotalLength
    0x01,                                               // bNumInterfaces
    0x01,                                               // bConfigurationValue
    0x00,                                               // iConfiguration
    0xC0,                                               // bmAttributes
    0x32,                                               // MaxPower

    /* const BYTE cbyInterfaceDescriptor[LEN_INTERFACE] = */
    LEN_INTERFACE,  // bLength
    DESC_INTERFACE, // bDescriptorType
    0x00,     // bInterfaceNumber
    0x00,     // bAlternateSetting
    0x02,     // bNumEndpoints
    0x08,     // bInterfaceClass
    0x05,     // bInterfaceSubClass
    0x50,     // bInterfaceProtocol
    0x00,     // iInterface

    /* const BYTE cbyEndpointDescriptor1[LEN_ENDPOINT] = */
    LEN_ENDPOINT,           // bLength
    DESC_ENDPOINT,          // bDescriptorType
    0x82,                   // bEndpointAddress
    EP_BULK,                // bmAttributes
    MAX_PACKET_SIZE, 0x00,  // wMaxPacketSize
    0x00,                   // bInterval

    /* const BYTE cbyEndpointDescriptor2[LEN_ENDPOINT] = */
    LEN_ENDPOINT,           // bLength
    DESC_ENDPOINT,          // bDescriptorType
    0x03,                   // bEndpointAddress
    EP_BULK,                // bmAttributes
    MAX_PACKET_SIZE, 0x00,  // wMaxPacketSize
    0x00                    // bInterval
};

__attribute__((aligned(4))) uint8_t gu8OtherConfigDescriptorHS_MSC[] =
{
    LEN_CONFIG,                                         // bLength
    DESC_CONFIG,                                        // bDescriptorType
    (LEN_CONFIG + LEN_INTERFACE + LEN_ENDPOINT * 2), 0x00, // wTotalLength
    0x01,                                               // bNumInterfaces
    0x01,                                               // bConfigurationValue
    0x00,                                               // iConfiguration
    0xC0,                                               // bmAttributes
    0x32,                                               // MaxPower

    /* const BYTE cbyInterfaceDescriptor[LEN_INTERFACE] = */
    LEN_INTERFACE,  // bLength
    DESC_INTERFACE, // bDescriptorType
    0x00,     // bInterfaceNumber
    0x00,     // bAlternateSetting
    0x02,     // bNumEndpoints
    0x08,     // bInterfaceClass
    0x05,     // bInterfaceSubClass
    0x50,     // bInterfaceProtocol
    0x00,     // iInterface

    /* const BYTE cbyEndpointDescriptor1[LEN_ENDPOINT] = */
    LEN_ENDPOINT,           // bLength
    DESC_ENDPOINT,          // bDescriptorType
    0x82,                   // bEndpointAddress
    EP_BULK,                // bmAttributes
    MAX_PACKET_SIZE, 0x00,  // wMaxPacketSize
    0x00,                   // bInterval

    /* const BYTE cbyEndpointDescriptor2[LEN_ENDPOINT] = */
    LEN_ENDPOINT,           // bLength
    DESC_ENDPOINT,          // bDescriptorType
    0x03,                   // bEndpointAddress
    EP_BULK,                // bmAttributes
    MAX_PACKET_SIZE, 0x00,  // wMaxPacketSize
    0x00                    // bInterval
};

__attribute__((aligned(4))) uint8_t gu8OtherConfigDescriptorFS_MSC[] =
{
    LEN_CONFIG,                                         // bLength
    DESC_CONFIG,                                        // bDescriptorType
    (LEN_CONFIG + LEN_INTERFACE + LEN_ENDPOINT * 2), 0x00, // wTotalLength
    0x01,                                               // bNumInterfaces
    0x01,                                               // bConfigurationValue
    0x00,                                               // iConfiguration
    0xC0,                                               // bmAttributes
    0x32,                                               // MaxPower

    /* const BYTE cbyInterfaceDescriptor[LEN_INTERFACE] = */
    LEN_INTERFACE,  // bLength
    DESC_INTERFACE, // bDescriptorType
    0x00,     // bInterfaceNumber
    0x00,     // bAlternateSetting
    0x02,     // bNumEndpoints
    0x08,     // bInterfaceClass
    0x05,     // bInterfaceSubClass
    0x50,     // bInterfaceProtocol
    0x00,     // iInterface

    /* const BYTE cbyEndpointDescriptor1[LEN_ENDPOINT] = */
    LEN_ENDPOINT,           // bLength
    DESC_ENDPOINT,          // bDescriptorType
    0x82,                   // bEndpointAddress
    EP_BULK,                // bmAttributes
    MAX_PACKET_SIZE, 0x00,  // wMaxPacketSize
    0x00,                   // bInterval

    /* const BYTE cbyEndpointDescriptor2[LEN_ENDPOINT] = */
    LEN_ENDPOINT,           // bLength
    DESC_ENDPOINT,          // bDescriptorType
    0x03,                   // bEndpointAddress
    EP_BULK,                // bmAttributes
    MAX_PACKET_SIZE, 0x00,  // wMaxPacketSize
    0x00                    // bInterval
};

/*!<USB Language String Descriptor */
__attribute__((aligned(4))) uint8_t gu8StringLang[4] =
{
    4,              /* bLength */
    DESC_STRING,    /* bDescriptorType */
    0x09, 0x04
};

/*!<USB Vendor String Descriptor */
__attribute__((aligned(4))) uint8_t gu8VendorStringDesc[] =
{
    16,
    DESC_STRING,
    'N', 0, 'u', 0, 'v', 0, 'o', 0, 't', 0, 'o', 0, 'n', 0
};

/*!<USB Product String Descriptor */
__attribute__((aligned(4))) uint8_t gu8ProductStringDesc[] =
{
    22,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    'U', 0, 'S', 0, 'B', 0, ' ', 0, 'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0
};

__attribute__((aligned(4))) uint8_t gu8StringSerial[26] =
{
    26,             // bLength
    DESC_STRING,    // bDescriptorType
    'A', 0, '0', 0, '2', 0, '0', 0, '0', 0, '8', 0, '0', 0, '4', 0, '0', 0, '1', 0, '1', 0, '4', 0
};

uint8_t *gpu8UsbString[4] =
{
    gu8StringLang,         /*!< Pointer for USB Language String Descriptor */
    gu8VendorStringDesc,   /*!< Pointer for USB Vendor String Descriptor   */
    gu8ProductStringDesc,  /*!< Pointer for USB Product String Descriptor  */
    gu8StringSerial
};

uint8_t *gpu8UsbHidReport[3] =
{
    HID_MouseReportDescriptor,
    NULL,
    NULL
};

uint32_t gu32UsbHidReportLen[3] =
{
    sizeof(HID_MouseReportDescriptor),
    0,
    0
};

S_HSUSBD_INFO_T gsHSInfo_HID =
{
    gu8DeviceDescriptor,      /*!< Pointer for USB Device Descriptor          */
    gu8ConfigDescriptorHID,   /*!< Pointer for USB Configuration Descriptor   */
    gpu8UsbString,            /*!< String Descriptors */
    gu8QualifierDescriptor,
    gu8ConfigDescriptorFS_HID,
    gu8OtherConfigDescriptorHS_HID,
    gu8OtherConfigDescriptorFS_HID,
    0,
    gpu8UsbHidReport,         /*!< Pointer for USB HID Report Descriptor      */
    gu32UsbHidReportLen,
    0
};

S_HSUSBD_INFO_T gsHSInfo_MSC =
{
    gu8DeviceDescriptor,      /*!< Pointer for USB Device Descriptor          */
    gu8ConfigDescriptorMSC,   /*!< Pointer for USB Configuration Descriptor   */
    gpu8UsbString,            /*!< String Descriptors */
    gu8QualifierDescriptor,
    gu8ConfigDescriptorFS_MSC,
    gu8OtherConfigDescriptorHS_MSC,
    gu8OtherConfigDescriptorFS_MSC,
    NULL,
    NULL,
    NULL
};



void API_HSUSBD_Open()
{
    HSUSBD_Open(&gsHSInfo_MSC, NULL, NULL);
    CU_ASSERT(HSUSBD->OPER == 0x0);
    CU_ASSERT(HSUSBD->PHYCTL == 0x08000200);
    /* Reset HSUSBD */
    SYS_ResetModule(SYS_HSUSBD0RST);
}

void API_HSUSBD_Start()
{
    HSUSBD_Start();
    CU_ASSERT(HSUSBD->OPER == 0x2);
    CU_ASSERT(HSUSBD->PHYCTL == 0x00000100);
    /* Reset HSUSBD */
    SYS_ResetModule(SYS_HSUSBD0RST);
}

void API_HSUSBD_CtrlIn()
{
    uint32_t u32Count, u32ErrorCount = 0;
    uint8_t u8CtrlInBuffer[20];

    for (u32Count = 0; u32Count < 20; u32Count++)
        u8CtrlInBuffer[u32Count] = u32Count;

    HSUSBD_Open(&gsHSInfo_MSC, NULL, NULL);
    HSUSBD_Start();
    HSUSBD_SetEpBufAddr(CEP, CEP_BUF_BASE, CEP_BUF_LEN);

    HSUSBD_PrepareCtrlIn(u8CtrlInBuffer, 20);

    HSUSBD_CtrlIn();
    CU_ASSERT(HSUSBD->CEPTXCNT == 20);

    for (u32Count = 0; u32Count < 20; u32Count++)
    {
        if (HSUSBD->CEPDAT_BYTE != u32Count)
            u32ErrorCount++;
    }

    CU_ASSERT_EQUAL(u32ErrorCount, 0);

    for (u32Count = 0; u32Count < 3; u32Count++)
        u8CtrlInBuffer[u32Count] = 0xF | u32Count;

    HSUSBD_PrepareCtrlIn(u8CtrlInBuffer, 3);
    HSUSBD_CtrlIn();
    CU_ASSERT(HSUSBD->CEPTXCNT == 3);

    for (u32Count = 0; u32Count < 3; u32Count++)
    {
        if (HSUSBD->CEPDAT_BYTE != (0xF | u32Count))
            u32ErrorCount++;
    }

    HSUSBD_SwReset();
    /* Reset HSUSBD */

    SYS_ResetModule(SYS_HSUSBD0RST);
}


void API_HSUSBD_SwReset()
{

    HSUSBD->FADDR = 0x55;

    HSUSBD_SwReset();

    CU_ASSERT(HSUSBD->FADDR == 0);
}

void VendorRequestCB()
{
    __NOP();
}

extern VENDOR_REQ g_hsusbd_pfnVendorRequest;

void API_HSUSBD_SetVendorRequest()
{
    HSUSBD_SetVendorRequest((VENDOR_REQ)VendorRequestCB);
    CU_ASSERT(g_hsusbd_pfnVendorRequest == VendorRequestCB);
}


void CONST_HSUSBD_Constants()
{
    /* The following standard request types are defined in USB 2.0 specification. */
    CU_ASSERT(REQ_CLASS == 0x20); /* USBD request type check (bmRequestType[6:5]) */
    CU_ASSERT(REQ_VENDOR == 0x40); /* USBD request type check (bmRequestType[6:5]) */

    /* The following standard request codes are defined in USB 2.0 specification. */
    CU_ASSERT(SET_DESCRIPTOR == 7); /* This standard request does not be implemented in USB device driver so far. */
    CU_ASSERT(SYNC_FRAME == 12); /* This standard request does not be implemented in USB device driver so far. */

    /* The following standard USBD descriptor types are defined in USB 2.0 specification. */
    CU_ASSERT(DESC_QUALIFIER == 6);
    CU_ASSERT(DESC_OTHERSPEED == 7);

    /* The length of HID Class Descriptor is defined in HID specification. */
    CU_ASSERT(LEN_HID == 0x9);

    /* Transfer type constant definition test for bmAttributes field of endpoint descriptor */
    CU_ASSERT(EP_ISO == 1);
    CU_ASSERT(EP_BULK == 2);
    CU_ASSERT(EP_INT == 3);

    /* Direction constant definition test for bEndpointAddress field of endpoint descriptor */
    CU_ASSERT(EP_INPUT == 0x80);
    CU_ASSERT(EP_OUTPUT == 0);

    /* USB event constant definition test */
    CU_ASSERT(HSUSBD_MAX_EP == 18);
    CU_ASSERT(CEP           == 0xff);
    CU_ASSERT(EPA           == 0);
    CU_ASSERT(EPB           == 1);
    CU_ASSERT(EPC           == 2);
    CU_ASSERT(EPD           == 3);
    CU_ASSERT(EPE           == 4);
    CU_ASSERT(EPF           == 5);
    CU_ASSERT(EPG           == 6);
    CU_ASSERT(EPH           == 7);
    CU_ASSERT(EPI           == 8);
    CU_ASSERT(EPJ           == 9);
    CU_ASSERT(EPK           == 10);
    CU_ASSERT(EPL           == 11);
    CU_ASSERT(EPM           == 12);
    CU_ASSERT(EPN           == 13);
    CU_ASSERT(EPO           == 14);
    CU_ASSERT(EPP           == 15);
    CU_ASSERT(EPQ           == 16);
    CU_ASSERT(EPR           == 17);
    /* HSUSBD_EPxRSPCTL register check */
    CU_ASSERT(HSUSBD_EP_RSPCTL_FLUSH       == 0x00000001);
    CU_ASSERT(HSUSBD_EP_RSPCTL_MODE_AUTO   == 0x00000000);
    CU_ASSERT(HSUSBD_EP_RSPCTL_MODE_MANUAL == 0x00000002);
    CU_ASSERT(HSUSBD_EP_RSPCTL_MODE_FLY    == 0x00000004);
    CU_ASSERT(HSUSBD_EP_RSPCTL_MODE_MASK   == 0x00000006);
    CU_ASSERT(HSUSBD_EP_RSPCTL_TOGGLE      == 0x00000008);
    CU_ASSERT(HSUSBD_EP_RSPCTL_HALT        == 0x00000010);
    CU_ASSERT(HSUSBD_EP_RSPCTL_ZEROLEN     == 0x00000020);
    CU_ASSERT(HSUSBD_EP_RSPCTL_SHORTTXEN   == 0x00000040);
    CU_ASSERT(HSUSBD_EP_RSPCTL_DISBUF      == 0x00000080);

    /* HSUSBD_EPxCFG register check */
    CU_ASSERT(HSUSBD_EP_CFG_VALID     == 0x00000001);
    CU_ASSERT(HSUSBD_EP_CFG_TYPE_BULK == 0x00000002);
    CU_ASSERT(HSUSBD_EP_CFG_TYPE_INT  == 0x00000004);
    CU_ASSERT(HSUSBD_EP_CFG_TYPE_ISO  == 0x00000006);
    CU_ASSERT(HSUSBD_EP_CFG_TYPE_MASK == 0x00000006);
    CU_ASSERT(HSUSBD_EP_CFG_DIR_OUT   == 0x00000000);
    CU_ASSERT(HSUSBD_EP_CFG_DIR_IN    == 0x00000008);

    /* HSUSBD_BCDC register check */
    CU_ASSERT(HSUSBD_BCDC_DETMOD_IDLE               == 0x00000000);
    CU_ASSERT(HSUSBD_BCDC_DETMOD_VBUS               == 0x00000002);
    CU_ASSERT(HSUSBD_BCDC_DETMOD_DCD                == 0x00000004);
    CU_ASSERT(HSUSBD_BCDC_DETMOD_PD                 == 0x00000006);
    CU_ASSERT(HSUSBD_BCDC_DETMOD_SD                 == 0x00000008);
    CU_ASSERT(HSUSBD_BCDC_DETSTS_VBUS_UNREACH       == 0x00000000);
    CU_ASSERT(HSUSBD_BCDC_DETSTS_VBUS_REACH         == 0x00000010);
    CU_ASSERT(HSUSBD_BCDC_DETSTS_DCD_DATA_UNCONTACT == 0x00000000);
    CU_ASSERT(HSUSBD_BCDC_DETSTS_DCD_DATA_CONTACT   == 0x00000010);
    CU_ASSERT(HSUSBD_BCDC_DETSTS_PD_SDP_NUSP        == 0x00000000);
    CU_ASSERT(HSUSBD_BCDC_DETSTS_PD_DCP_CDP         == 0x00000010);
    CU_ASSERT(HSUSBD_BCDC_DETSTS_SD_CDP             == 0x00000000);
    CU_ASSERT(HSUSBD_BCDC_DETSTS_SD_DCP             == 0x00000010);
}



CU_TestInfo HSUSBD_MacroTests[] =
{

    {"Get maximum value", MACRO_Maximum},
    {"Get minimum value", MACRO_Minimum},
    {"Enable/Disable USB", MACRO_HSUSBD_ENABLE_DISABLE_USB},
    {"Enable/Disable USB PHY", MACRO_HSUSBD_ENABLE_DISABLE_PHY},
    {"Set/Clear USB DRVSE0 bit", MACRO_HSUSBD_SET_CLEAR_SE0},
    {"Set/Get USB device address", MACRO_HSUSBD_SET_GET_ADDR},
    //    {"Enable USBD interrupt function", MACRO_HSUSBD_ENABLE_INT},
    //    {"Get/Clear USB interrupt flag", MACRO_HSUSBD_GET_CLR_INT_FLAG},
    {"Enable CEP interrupt function", MACRO_HSUSBD_ENABLE_CEP_INT},
    //    {"Clear CEP interrupt flag", MACRO_HSUSBD_CLR_CEP_INT_FLAG},
    //    {"Set CEP state", MACRO_HSUSBD_SET_CEP_STATE},
    //    {"Start CEP in transfer", MACRO_HSUSBD_START_CEP_IN},
    //    {"Set EP maximum packet size", MACRO_HSUSBD_SET_MAX_PAYLOAD},
    //    {"Enable EP interrupt function", MACRO_HSUSBD_ENABLE_EP_INT},
    //    {"Get/Clear EP interrupt flag", MACRO_HSUSBD_GET_CLR_EP_INT_FLAG},
    //    {"Set DMA length", MACRO_HSUSBD_SET_DMA_LEN},
    {"Set DMA address", MACRO_HSUSBD_SET_DMA_ADDR},
    {"Set DMA read", MACRO_HSUSBD_SET_DMA_READ},
    {"Set DMA write", MACRO_HSUSBD_SET_DMA_WRITE},
    {"Enable DMA", MACRO_HSUSBD_ENABLE_DMA},
    {"Get USB bus connection state", MACRO_HSUSBD_IS_ATTACHED},
    {"Enable/Disable BCD", MACRO_HSUSBD_ENABLE_DISABLE_BCD},
    {"Enable/Disable LPM", MACRO_HSUSBD_ENABLE_DISABLE_LPM},
    {"Memory copy test", MACRO_HSUSBD_MemCopy},
    {"Set/Clear/Get STALL state of the specified endpoint ID", MACRO_HSUSBD_Set_Clear_Get_Ep_Stall},
    {"Set/Clear/Get STALL", MACRO_HSUSBD_SetStall_ClearStall_GetStall},

    CU_TEST_INFO_NULL
};

CU_TestInfo HSUSBD_ApiTests[] =
{

    {"USB open", API_HSUSBD_Open},
    {"USB device start", API_HSUSBD_Start},
    {"USB control IN test", API_HSUSBD_CtrlIn},
    {"USB software reset", API_HSUSBD_SwReset},
    {"Set USBD vendor request callback function", API_HSUSBD_SetVendorRequest},

    CU_TEST_INFO_NULL
};

CU_TestInfo HSUSBD_ConstantTest[] =
{
    {"Endpoint ID constant test", CONST_HSUSBD_Constants},

    CU_TEST_INFO_NULL
};
