/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.                                        */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   usbd_cunit.c                                                                                          */
/*            The test function of USBD for CUnit.                                                         */
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
#include "usbd_cunit.h"

extern uint8_t g_usbd_SetupPacket[8];

extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int USBD_Tests_Init(void)
{
    return 0;
}
int USBD_Tests_Clean(void)
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

CU_SuiteInfo suites[] =
{
    {"USBD MACRO", USBD_Tests_Init, USBD_Tests_Clean, NULL, NULL, USBD_MacroTests},
    {"USBD API", USBD_Tests_Init, USBD_Tests_Clean, NULL, NULL, USBD_ApiTests},
    {"USBD CONSTANT", USBD_Tests_Init, USBD_Tests_Clean, NULL, NULL, USBD_ConstantTest},
    CU_SUITE_INFO_NULL
};



void MACRO_USBD_Maximum()
{
    CU_ASSERT(USBD_Maximum(3, 5) == 5);
    CU_ASSERT(USBD_Maximum(3500, 1500) == 3500);
}

void MACRO_USBD_Minimum()
{
    CU_ASSERT(USBD_Minimum(3, 5) == 3);
    CU_ASSERT(USBD_Minimum(3500, 1500) == 1500);
}

void MACRO_USBD_ENABLE_DISABLE_USB()
{
    CU_ASSERT_FALSE(USBD->ATTR & 0x90);
    USBD_ENABLE_USB();
    CU_ASSERT((USBD->ATTR & 0x90) == (USBD_USB_EN | USBD_PHY_EN));

    USBD_DISABLE_USB();
    CU_ASSERT((USBD->ATTR & 0x90) == 0x10);

    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void MACRO_USBD_ENABLE_DISABLE_PHY()
{
    CU_ASSERT_FALSE(USBD->ATTR & 0x10);
    USBD_ENABLE_PHY();
    CU_ASSERT((USBD->ATTR & 0x90) == 0x10);
    USBD_DISABLE_PHY();
    CU_ASSERT_FALSE(USBD->ATTR & 0x10);
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void MACRO_USBD_SET_CLEAR_SE0()
{
    CU_ASSERT((USBD->SE0 & 1) == USBD_DRVSE0);
    USBD_CLR_SE0();
    CU_ASSERT_FALSE(USBD->SE0 & 1);
    USBD_SET_SE0();
    CU_ASSERT_TRUE(USBD->SE0 & 1);
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void MACRO_USBD_SET_GET_ADDR()
{
    CU_ASSERT_FALSE(USBD->FADDR);
    USBD_SET_ADDR(0x55);
    CU_ASSERT((USBD->FADDR & 0x7F) == 0x55);
    CU_ASSERT(USBD_GET_ADDR() == 0x55);
    USBD_SET_ADDR(0xAA);
    CU_ASSERT((USBD->FADDR & 0x7F) == 0x2A);
    CU_ASSERT(USBD_GET_ADDR() == 0x2A);
     /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void MACRO_USBD_ENABLE_INT()
{
    USBD_ENABLE_INT(USBD_INT_SOF);
    CU_ASSERT(USBD->INTEN & 0x10);
    USBD_ENABLE_INT(USBD_INT_WAKEUP);
    CU_ASSERT(USBD->INTEN & 8);
    USBD_ENABLE_INT(USBD_INT_FLDET);
    CU_ASSERT(USBD->INTEN & 4);
    USBD_ENABLE_INT(USBD_INT_USB);
    CU_ASSERT(USBD->INTEN & 2);
    USBD_ENABLE_INT(USBD_INT_BUS);
    CU_ASSERT(USBD->INTEN & 1);
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void MACRO_USBD_GET_CLR_INT_FLAG()
{
    CU_ASSERT(USBD_GET_INT_FLAG() == 0);
    USBD_CLR_INT_FLAG(USBD_INTSTS_WAKEUP | USBD_INTSTS_FLDET | USBD_INTSTS_BUS | USBD_INTSTS_USB);
    CU_ASSERT(USBD_GET_INT_FLAG() == 0);
}

void MACRO_USBD_GET_CLR_EP_INT_FLAG()
{
    CU_ASSERT(USBD_GET_EP_INT_FLAG() == 0);
    USBD_CLR_EP_INT_FLAG(0x1FFFFFF);
    CU_ASSERT(USBD_GET_EP_INT_FLAG() == 0);
}

void MACRO_USBD_GET_EP_FLAG()
{
    CU_ASSERT(USBD_GET_EP_FLAG() == 0);
}


void MACRO_USBD_GET_BUS_STATE()
{
    CU_ASSERT(USBD_GET_BUS_STATE() == 0);
}

void MACRO_USBD_IS_ATTACHED()
{
    CU_ASSERT(USBD_IS_ATTACHED() == 0);
}

void MACRO_USBD_STOP_TRANSACTION()
{
    USBD_STOP_TRANSACTION(EP0);
    CU_ASSERT(USBD->EP[0].CFGP == 0);
    USBD_STOP_TRANSACTION(EP1);
    CU_ASSERT(USBD->EP[1].CFGP == 0);
    USBD_STOP_TRANSACTION(EP2);
    CU_ASSERT(USBD->EP[2].CFGP == 0);
    USBD_STOP_TRANSACTION(EP3);
    CU_ASSERT(USBD->EP[3].CFGP == 0);
    USBD_STOP_TRANSACTION(EP4);
    CU_ASSERT(USBD->EP[4].CFGP == 0);
    USBD_STOP_TRANSACTION(EP5);
    CU_ASSERT(USBD->EP[5].CFGP == 0);
    USBD_STOP_TRANSACTION(EP6);
    CU_ASSERT(USBD->EP[6].CFGP == 0);
    USBD_STOP_TRANSACTION(EP7);
    CU_ASSERT(USBD->EP[7].CFGP == 0);
    USBD_STOP_TRANSACTION(EP8);
    CU_ASSERT(USBD->EP[8].CFGP == 0);
    USBD_STOP_TRANSACTION(EP9);
    CU_ASSERT(USBD->EP[9].CFGP == 0);
    USBD_STOP_TRANSACTION(EP10);
    CU_ASSERT(USBD->EP[10].CFGP == 0);
    USBD_STOP_TRANSACTION(EP11);
    CU_ASSERT(USBD->EP[11].CFGP == 0);
    USBD_STOP_TRANSACTION(EP12);
    CU_ASSERT(USBD->EP[12].CFGP == 0);
    USBD_STOP_TRANSACTION(EP13);
    CU_ASSERT(USBD->EP[13].CFGP == 0);
    USBD_STOP_TRANSACTION(EP14);
    CU_ASSERT(USBD->EP[14].CFGP == 0);
    USBD_STOP_TRANSACTION(EP15);
    CU_ASSERT(USBD->EP[15].CFGP == 0);
    USBD_STOP_TRANSACTION(EP16);
    CU_ASSERT(USBD->EP[16].CFGP == 0);
    USBD_STOP_TRANSACTION(EP17);
    CU_ASSERT(USBD->EP[17].CFGP == 0);
    USBD_STOP_TRANSACTION(EP18);
    CU_ASSERT(USBD->EP[18].CFGP == 0);
    USBD_STOP_TRANSACTION(EP19);
    CU_ASSERT(USBD->EP[19].CFGP == 0);
    USBD_STOP_TRANSACTION(EP20);
    CU_ASSERT(USBD->EP[20].CFGP == 0);
    USBD_STOP_TRANSACTION(EP21);
    CU_ASSERT(USBD->EP[21].CFGP == 0);
    USBD_STOP_TRANSACTION(EP22);
    CU_ASSERT(USBD->EP[22].CFGP == 0);
    USBD_STOP_TRANSACTION(EP23);
    CU_ASSERT(USBD->EP[23].CFGP == 0);
    USBD_STOP_TRANSACTION(EP24);
    CU_ASSERT(USBD->EP[24].CFGP == 0);
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void MACRO_USBD_SET_DATA0_DATA1()
{
    USBD_SET_DATA1(0);
    CU_ASSERT((USBD->EP[0].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(0);
    CU_ASSERT((USBD->EP[0].CFG & 0x80) == 0);
    USBD_SET_DATA1(1);
    CU_ASSERT((USBD->EP[1].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(1);
    CU_ASSERT((USBD->EP[1].CFG & 0x80) == 0);
    USBD_SET_DATA1(2);
    CU_ASSERT((USBD->EP[2].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(2);
    CU_ASSERT((USBD->EP[2].CFG & 0x80) == 0);
    USBD_SET_DATA1(3);
    CU_ASSERT((USBD->EP[3].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(3);
    CU_ASSERT((USBD->EP[3].CFG & 0x80) == 0);
    USBD_SET_DATA1(4);
    CU_ASSERT((USBD->EP[4].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(4);
    CU_ASSERT((USBD->EP[4].CFG & 0x80) == 0);
    USBD_SET_DATA1(5);
    CU_ASSERT((USBD->EP[5].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(5);
    CU_ASSERT((USBD->EP[5].CFG & 0x80) == 0);
    USBD_SET_DATA1(6);
    CU_ASSERT((USBD->EP[6].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(6);
    CU_ASSERT((USBD->EP[6].CFG & 0x80) == 0);
    USBD_SET_DATA1(7);
    CU_ASSERT((USBD->EP[7].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(7);
    CU_ASSERT((USBD->EP[7].CFG & 0x80) == 0);
    USBD_SET_DATA1(8);
    CU_ASSERT((USBD->EP[8].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(8);
    CU_ASSERT((USBD->EP[8].CFG & 0x80) == 0);
    USBD_SET_DATA1(9);
    CU_ASSERT((USBD->EP[9].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(9);
    CU_ASSERT((USBD->EP[9].CFG & 0x80) == 0);
    USBD_SET_DATA1(10);
    CU_ASSERT((USBD->EP[10].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(10);
    CU_ASSERT((USBD->EP[10].CFG & 0x80) == 0);
    USBD_SET_DATA1(11);
    CU_ASSERT((USBD->EP[11].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(11);
    CU_ASSERT((USBD->EP[11].CFG & 0x80) == 0);
    USBD_SET_DATA1(12);
    CU_ASSERT((USBD->EP[12].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(12);
    CU_ASSERT((USBD->EP[12].CFG & 0x80) == 0);
    USBD_SET_DATA1(13);
    CU_ASSERT((USBD->EP[13].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(13);
    CU_ASSERT((USBD->EP[13].CFG & 0x80) == 0);
    USBD_SET_DATA1(14);
    CU_ASSERT((USBD->EP[14].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(14);
    CU_ASSERT((USBD->EP[14].CFG & 0x80) == 0);
    USBD_SET_DATA1(15);
    CU_ASSERT((USBD->EP[15].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(15);
    CU_ASSERT((USBD->EP[15].CFG & 0x80) == 0);
    USBD_SET_DATA1(16);
    CU_ASSERT((USBD->EP[16].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(16);
    CU_ASSERT((USBD->EP[16].CFG & 0x80) == 0);
    USBD_SET_DATA1(17);
    CU_ASSERT((USBD->EP[17].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(17);
    CU_ASSERT((USBD->EP[17].CFG & 0x80) == 0);
    USBD_SET_DATA1(18);
    CU_ASSERT((USBD->EP[18].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(18);
    CU_ASSERT((USBD->EP[18].CFG & 0x80) == 0);
    USBD_SET_DATA1(19);
    CU_ASSERT((USBD->EP[19].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(19);
    CU_ASSERT((USBD->EP[19].CFG & 0x80) == 0);
    USBD_SET_DATA1(20);
    CU_ASSERT((USBD->EP[20].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(20);
    CU_ASSERT((USBD->EP[20].CFG & 0x80) == 0);
    USBD_SET_DATA1(21);
    CU_ASSERT((USBD->EP[21].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(21);
    CU_ASSERT((USBD->EP[21].CFG & 0x80) == 0);
    USBD_SET_DATA1(22);
    CU_ASSERT((USBD->EP[22].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(22);
    CU_ASSERT((USBD->EP[22].CFG & 0x80) == 0);
    USBD_SET_DATA1(23);
    CU_ASSERT((USBD->EP[23].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(23);
    CU_ASSERT((USBD->EP[23].CFG & 0x80) == 0);
    USBD_SET_DATA1(24);
    CU_ASSERT((USBD->EP[24].CFG & 0x80) == 0x80);
    USBD_SET_DATA0(24);
    CU_ASSERT((USBD->EP[24].CFG & 0x80) == 0);
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void MACRO_USBD_SET_GET_PAYLOAD_LEN()
{
    uint32_t u32EpNum, u32ReturnValue;

    for(u32EpNum = 0; u32EpNum < USBD_MAX_EP; u32EpNum++)
    {
        USBD_CONFIG_EP(u32EpNum, USBD_CFG_EPMODE_IN);
    }

    for(u32EpNum = 0; u32EpNum < USBD_MAX_EP; u32EpNum++)
    {
        USBD_SET_PAYLOAD_LEN(u32EpNum, 0);
        CU_ASSERT(USBD->EP[u32EpNum].MXPLD == 0);
        u32ReturnValue = USBD_GET_PAYLOAD_LEN(u32EpNum);
        CU_ASSERT(u32ReturnValue == 0);

        USBD_SET_PAYLOAD_LEN(u32EpNum, 0x80);
        CU_ASSERT(USBD->EP[u32EpNum].MXPLD == 0x80);
        u32ReturnValue = USBD_GET_PAYLOAD_LEN(u32EpNum);
        CU_ASSERT(u32ReturnValue == 0x80);

        USBD_SET_PAYLOAD_LEN(u32EpNum, 0x1FF);
        CU_ASSERT(USBD->EP[u32EpNum].MXPLD == 0x1FF);
        u32ReturnValue = USBD_GET_PAYLOAD_LEN(u32EpNum);
        CU_ASSERT(u32ReturnValue == 0x1FF);
    }
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void MACRO_USBD_CONFIG_EP()
{
    uint32_t u32EpNum;

    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
    for(u32EpNum = 0; u32EpNum < USBD_MAX_EP; u32EpNum++)
    {
        CU_ASSERT(USBD->EP[u32EpNum].CFG == USBD_CFG_EPMODE_DISABLE);
        USBD_CONFIG_EP(u32EpNum, USBD_CFG_CSTALL | USBD_CFG_DSQSYNC_Msk | USBD_CFG_EPMODE_OUT | USBD_CFG_TYPE_ISO | 0xF);
        CU_ASSERT(USBD->EP[u32EpNum].CFG == 0x2BF);
    }
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
    for(u32EpNum = 0; u32EpNum < USBD_MAX_EP; u32EpNum++)
    {
        CU_ASSERT(USBD->EP[u32EpNum].CFG == USBD_CFG_EPMODE_DISABLE);
        USBD_CONFIG_EP(u32EpNum, USBD_CFG_DB_EN | USBD_CFG_DBTGACTIVE | USBD_CFG_CSTALL | USBD_CFG_DSQSYNC_Msk | USBD_CFG_EPMODE_IN | USBD_CFG_TYPE_ISO | 0xF);
        CU_ASSERT(USBD->EP[u32EpNum].CFG == 0xE5F);
    }
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
    for(u32EpNum = 0; u32EpNum < USBD_MAX_EP; u32EpNum++)
    {
        CU_ASSERT(USBD->EP[u32EpNum].CFG == USBD_CFG_EPMODE_DISABLE);
        USBD_CONFIG_EP(u32EpNum, USBD_CFG_DB_EN | USBD_CFG_CSTALL | USBD_CFG_DSQSYNC_Msk | USBD_CFG_EPMODE_OUT | USBD_CFG_TYPE_ISO | 0xF);
        CU_ASSERT(USBD->EP[u32EpNum].CFG == 0xABF);
    }
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
    
}

void MACRO_USBD_SET_GET_EP_BUF_ADDR()
{
    uint32_t u32EpNum;

    for(u32EpNum = 0; u32EpNum < USBD_MAX_EP; u32EpNum++)
    {
        USBD_SET_EP_BUF_ADDR(u32EpNum, 0x1F8);
        CU_ASSERT(USBD->EP[u32EpNum].BUFSEG == 0x1F8);
        CU_ASSERT(USBD_GET_EP_BUF_ADDR(u32EpNum) == 0x1F8);
    }
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
    
}

void MACRO_USBD_SET_CLR_GET_EP_STALL()
{
    uint32_t u32EpCount;

    for(u32EpCount = 0; u32EpCount < USBD_MAX_EP; u32EpCount++)
    {
        USBD_SET_EP_STALL(u32EpCount);
        CU_ASSERT(USBD->EP[u32EpCount].CFGP == 2);
        CU_ASSERT(USBD->EP[u32EpCount].CFGP == USBD_CFGP_SSTALL);
        CU_ASSERT_TRUE(USBD_GET_EP_STALL(u32EpCount));
        USBD_CLR_EP_STALL(u32EpCount);
        CU_ASSERT(USBD->EP[u32EpCount].CFGP == 0);
        CU_ASSERT_FALSE(USBD_GET_EP_STALL(u32EpCount));
    }
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void MACRO_USBD_SET_DB_SB_MODE_IS_DB_MODE()
{
    uint32_t u32EpCount;

    for(u32EpCount = 0; u32EpCount < USBD_MAX_EP; u32EpCount++)
    {
        USBD_SET_DB_MODE(u32EpCount);
        CU_ASSERT(USBD->EP[u32EpCount].CFG == (0x800 | USBD_CFG_DSQSYNC_Msk));
        CU_ASSERT(USBD->EP[u32EpCount].CFG == (USBD_CFG_DB_EN | USBD_CFG_DSQSYNC_Msk));
        CU_ASSERT_TRUE(USBD_IS_DB_MODE(u32EpCount));
        USBD_SET_SB_MODE(u32EpCount);
        CU_ASSERT(USBD->EP[u32EpCount].CFG == (0 | USBD_CFG_DSQSYNC_Msk));
        CU_ASSERT_FALSE(USBD_IS_DB_MODE(u32EpCount));
    }
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void MACRO_USBD_SET_DB_ACTIVE_INACTIVE()
{
    uint32_t u32EpCount;

    for(u32EpCount = 0; u32EpCount < USBD_MAX_EP; u32EpCount++)
    {
        USBD_SET_DB_ACTIVE(u32EpCount);
        CU_ASSERT(USBD->EP[u32EpCount].CFG == 0x400);
        CU_ASSERT(USBD->EP[u32EpCount].CFG == USBD_CFG_DBTGACTIVE);
        USBD_SET_DB_INACTIVE(u32EpCount);
        CU_ASSERT(USBD->EP[u32EpCount].CFG == 0);
    }
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void MACRO_USBD_MemCopy()
{
    uint8_t u8Src[20];
    uint8_t u8Dest[20];
    uint32_t u32Count;

    for(u32Count = 0; u32Count < 20; u32Count++)
    {
        u8Src[u32Count] = u32Count + 1;
        u8Dest[u32Count] = 0;
    }
    USBD_MemCopy(u8Dest, u8Src, 20);
    for(u32Count = 0; u32Count < 20; u32Count++)
    {
        CU_ASSERT(u8Src[u32Count] == u8Dest[u32Count])
    }
}

void MACRO_USBD_SetStall_ClearStall_GetStall()
{
    USBD->EP[2].CFG = 0xF;
    USBD_SetStall(0xF);
    CU_ASSERT(USBD->EP[2].CFGP == 2);
    CU_ASSERT_TRUE(USBD_GetStall(0xF));

    USBD->EP[7].CFG = 0x2;
    USBD_SetStall(0x2);
    CU_ASSERT(USBD->EP[7].CFGP == 2);
    CU_ASSERT_TRUE(USBD_GetStall(0x2));

    USBD_ClearStall(0xF);
    CU_ASSERT(USBD->EP[2].CFGP == 0);
    CU_ASSERT_FALSE(USBD_GetStall(0xF));

    USBD_ClearStall(0x2);
    CU_ASSERT(USBD->EP[7].CFGP == 0);
    CU_ASSERT_FALSE(USBD_GetStall(0x2));

    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}



#define MAX_PACKET_SIZE 20
#define EP0_BUF_OFFSET 0x80
#define EP1_BUF_OFFSET 0x100
#define EP2_BUF_OFFSET 0x180
#define DEVICE_ADDRESS 0x25
#define CONFIGURATION_VALUE 5
#define ALT_INTERFACE 2

/*!<USB Device Descriptor */
uint8_t gu8DeviceDescriptor[] =
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

/*!<USB HID Report Descriptor */
uint8_t HID_MouseReportDescriptor[] =
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
#define USBD_SELF_POWERED               0
#define USBD_REMOTE_WAKEUP              0
#define USBD_MAX_POWER                  50  /* The unit is in 2mA. ex: 50 * 2mA = 100mA */
#define INT_IN_EP_NUM   0x01    /* Define the interrupt In EP number */
#define EP2_MAX_PKT_SIZE    8
/* USB HID Interface Class protocol */
#define HID_NONE            0x00
#define HID_KEYBOARD        0x01
#define HID_MOUSE           0x02

/*!<USB Configuration Descriptor for HID */
const uint8_t gu8ConfigDescriptorHID[] =
{
    LEN_CONFIG,     /* bLength */
    DESC_CONFIG,    /* bDescriptorType */
    /* wTotalLength */
    LEN_CONFIG_AND_SUBORDINATE & 0x00FF,
    (LEN_CONFIG_AND_SUBORDINATE & 0xFF00) >> 8,
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
uint8_t gu8ConfigDescriptorMSC[] =
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
    22,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    'U', 0, 'S', 0, 'B', 0, ' ', 0, 'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0
};

const uint8_t gu8StringSerial[26] =
{
    26,             // bLength
    DESC_STRING,    // bDescriptorType
    'A', 0, '0', 0, '2', 0, '0', 0, '0', 0, '8', 0, '0', 0, '4', 0, '0', 0, '1', 0, '1', 0, '4', 0
};

const uint8_t *gpu8UsbString[4] =
{
    gu8StringLang,         /*!< Pointer for USB Language String Descriptor */
    gu8VendorStringDesc,   /*!< Pointer for USB Vendor String Descriptor   */
    gu8ProductStringDesc,  /*!< Pointer for USB Product String Descriptor  */
    gu8StringSerial
};

const uint8_t *gpu8UsbHidReport[2] =
{
    HID_MouseReportDescriptor,
    NULL
};

const uint32_t gu32UsbHidReportLen[2] =
{
    sizeof(HID_MouseReportDescriptor),
    0
};

const uint32_t gu32ConfigHidDescIdx[2] =
{
    (LEN_CONFIG + LEN_INTERFACE),
    0
};

const S_USBD_INFO_T gsInfo_HID =
{
    (uint8_t *)gu8DeviceDescriptor,
    (uint8_t *)gu8ConfigDescriptorHID,
    (uint8_t **)gpu8UsbString,
    (uint8_t **)gpu8UsbHidReport,
    NULL,
    (uint32_t *)gu32UsbHidReportLen,
    (uint32_t *)gu32ConfigHidDescIdx
};

const S_USBD_INFO_T gsInfo_MSC =
{
    (uint8_t *)gu8DeviceDescriptor,      /*!< Pointer for USB Device Descriptor          */
    (uint8_t *)gu8ConfigDescriptorMSC,   /*!< Pointer for USB Configuration Descriptor   */
    (uint8_t **)gpu8UsbString,            /*!< String Descriptors */
    NULL,
    NULL,
    NULL
};


void API_USBD_Open()
{
    USBD_Open(&gsInfo_MSC, NULL, NULL);
    //CU_ASSERT( g_usbd_CtrlMaxPktSize==20);
#ifdef SUPPORT_LPM
    CU_ASSERT(USBD->ATTR == (0x800 | 0x7D0));
    CU_ASSERT(USBD->ATTR == (USBD_LPMACK | USBD_BYTEM | 0x200 | USBD_DPPU_EN | USBD_USB_EN | 0x40 | USBD_PHY_EN));
#else
    CU_ASSERT(USBD->ATTR == (0x7D0));
    CU_ASSERT(USBD->ATTR == (USBD_BYTEM | 0x200 | USBD_DPPU_EN | USBD_USB_EN | 0x40 | USBD_PHY_EN));
#endif
    CU_ASSERT(USBD->SE0 == 1);
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void API_USBD_Start()
{
    USBD_Start();
    CU_ASSERT(USBD->INTEN == 0x010F);
    CU_ASSERT(USBD->SE0 == 0);
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void API_USBD_GetSetupPacket()
{
    uint32_t u32Count, u32ErrorCount = 0;
    uint8_t u8Buffer[8];

    for(u32Count = 0; u32Count < 8; u32Count++)
        g_usbd_SetupPacket[u32Count] = u32Count;

    USBD_GetSetupPacket(u8Buffer);

    for(u32Count = 0; u32Count < 8; u32Count++)
    {
        if(g_usbd_SetupPacket[u32Count] != u8Buffer[u32Count])
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);
    USBD_SwReset();
}

void API_USBD_ProcessSetupPacket()
{
    uint32_t u32Count, u32ErrorCount = 0;

    USBD_Open(&gsInfo_MSC, NULL, NULL);
    USBD_CONFIG_EP(0, USBD_CFG_EPMODE_IN);
    USBD_CONFIG_EP(1, USBD_CFG_EPMODE_IN); /* Also set Endpoint buffer 1 as IN endpoint for control OUT endpoint. Just for CUnit test. */
//    outp32(0x400C0500, EP0_BUF_OFFSET); /* EP[0]->BUFSEG */
//    outp32(0x400C0510, EP1_BUF_OFFSET); /* EP[1]->BUFSEG */
    outp32(0x40250500, EP0_BUF_OFFSET); /* EP[0]->BUFSEG */
    outp32(0x40250510, EP1_BUF_OFFSET); /* EP[1]->BUFSEG */
  	/* EP2 ==> Bulk IN endpoint, device endpoint number 2. */
    USBD_CONFIG_EP(EP2, USBD_CFG_EPMODE_IN | 2);
    /* Buffer range for EP2 */
    USBD_SET_EP_BUF_ADDR(EP2, EP2_BUF_OFFSET);

    /* --- Standard request "SET_ADDRESS" test --- */
    //outp8(0x400C0800, 0x00); /* Host to device; standard request. */
    outp8(USBD_BUF_BASE, 0x00); /* Host to device; standard request. */
    outp8(0x40250801, 5);    /* SET_ADDRESS */
    outp8(0x40250802, DEVICE_ADDRESS); /* wValue: USB device address */
    outp8(0x40250803, 0);
    outp8(0x40250804, 0); /* wIndex */
    outp8(0x40250805, 0);
    outp8(0x40250806, 0); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 0);
    USBD_CtrlIn();
    CU_ASSERT(USBD->EP[0].MXPLD == 0);
    CU_ASSERT(USBD->FADDR == DEVICE_ADDRESS);


    /* --- Standard request "SET_CONFIGURATION" test --- */
    outp8(0x40250800, REQ_STANDARD); /* Host to device; standard request. */
    outp8(0x40250801, 9);    /* SET_CONFIGURATION */
    outp8(0x40250802, CONFIGURATION_VALUE); /* wValue: Configuration value */
    outp8(0x40250803, 0);
    outp8(0x40250804, 0); /* wIndex */
    outp8(0x40250805, 0);
    outp8(0x40250806, 0); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 0);
    USBD_CtrlIn();
    CU_ASSERT(USBD->EP[0].MXPLD == 0);


    /* --- Standard request "GET_CONFIGURATION" test --- */
    outp8(0x40250800, 0x80); /* Device to host; standard request. */
    outp8(0x40250801, 0x8);  /* GET_CONFIGURATION */
    outp8(0x40250802, 0); /* wValue */
    outp8(0x40250803, 0);
    outp8(0x40250804, 0); /* wIndex */
    outp8(0x40250805, 0);
    outp8(0x40250806, 0x01); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 1);
    CU_ASSERT(inp8(0x40250800 + EP0_BUF_OFFSET) == CONFIGURATION_VALUE);
    CU_ASSERT(USBD->EP[1].MXPLD == 20); /* MAX_PACKET_SIZE: 20 */
    //CU_ASSERT(USBD->EP[1].MXPLD == 0);

    /* --- Standard request "SET_FEATURE" & "CLEAR_FEATURE" test --- */
    outp8(0x40250800, 0x00); /* Host to device; standard request. */
    outp8(0x40250801, 0x3);  /* SET_FEATURE */
    outp8(0x40250802, FEATURE_ENDPOINT_HALT); /* wValue: ENDPOINT_HALT standard feature selector */
    outp8(0x40250803, 0);
    outp8(0x40250804, 2); /* wIndex: device endpoint number 2 (bulk IN) */
    outp8(0x40250805, 0);
    outp8(0x40250806, 0x0); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[2].CFGP & 2); /* SSTALL */
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 0);

    outp8(0x40250800, 0x00); /* Host to device; standard request. */
    outp8(0x40250801, 0x1);  /* CLEAR_FEATURE */
    outp8(0x40250802, 0); /* wValue: ENDPOINT_HALT standard feature selector */
    outp8(0x40250803, 0);
    outp8(0x40250804, 2); /* wIndex: device endpoint number 2 (bulk IN) */
    outp8(0x40250805, 0);
    outp8(0x40250806, 0x0); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT((USBD->EP[2].CFGP & 2) == 0); /* SSTALL */
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 0);

    outp8(0x40250800, 0x00); /* Host to device; standard request. */
    outp8(0x40250801, 0x3);  /* SET_FEATURE */
    outp8(0x40250802, FEATURE_DEVICE_REMOTE_WAKEUP); /* wValue: DEVICE_REMOTE_WAKEUP standard feature selector */
    outp8(0x40250803, 0);
    outp8(0x40250804, 0); /* wIndex */
    outp8(0x40250805, 0);
    outp8(0x40250806, 0x0); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(g_usbd_RemoteWakeupEn == 1);
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 0);

    outp8(0x40250800, 0x00); /* Host to device; standard request. */
    outp8(0x40250801, 0x1);  /* CLEAR_FEATURE */
    outp8(0x40250802, 1); /* wValue: DEVICE_REMOTE_WAKEUP standard feature selector */
    outp8(0x40250803, 0);
    outp8(0x40250804, 0); /* wIndex */
    outp8(0x40250805, 0);
    outp8(0x40250806, 0x0); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(g_usbd_RemoteWakeupEn == 0);
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 0);


    /* --- Standard request "SET_INTERFACE" test --- */
    outp8(0x40250800, 0x01); /* Host to device; standard request. */
    outp8(0x40250801, 0xB);  /* SET_INTERFACE */
    outp8(0x40250802, ALT_INTERFACE); /* wValue: alternate interface setting */
    outp8(0x40250803, 0);
    outp8(0x40250804, 0); /* wIndex: interface number */
    outp8(0x40250805, 0);
    outp8(0x40250806, 0x0); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 0);

    /* --- Standard request "GET_INTERFACE" test --- */
    outp8(0x40250800, 0x81); /* Device to host; standard request. */
    outp8(0x40250801, 0xA);  /* GET_INTERFACE */
    outp8(0x40250802, 0); /* wValue */
    outp8(0x40250803, 0);
    outp8(0x40250804, 0); /* wIndex: interface number */
    outp8(0x40250805, 0);
    outp8(0x40250806, 0x01); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 1);
    CU_ASSERT(inp8(0x40250800 + EP0_BUF_OFFSET) == ALT_INTERFACE);
    CU_ASSERT(USBD->EP[1].MXPLD == 20); /* MAX_PACKET_SIZE: 20 */


    /* --- Standard request "GET_DESCRIPTOR" test --- */
    outp8(0x40250800, 0x80); /* Device to host; standard request. */
    outp8(0x40250801, 0x6);  /* GET_DESCRIPTOR */
    outp8(0x40250802, 0); /* wValue: descriptor type (high byte) and descriptor index (low byte). */
    outp8(0x40250803, 1); /* 1: device descriptor */
    outp8(0x40250804, 0); /* wIndex */
    outp8(0x40250805, 0);
    outp8(0x40250806, LEN_DEVICE); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == LEN_DEVICE);
    /* Check EP0 buffer */
    for(u32Count = 0; u32Count < LEN_DEVICE; u32Count++)
    {
        if(inp8(0x40250800 + EP0_BUF_OFFSET + u32Count) != gu8DeviceDescriptor[u32Count])
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);
    CU_ASSERT(USBD->EP[1].MXPLD == MAX_PACKET_SIZE);

    outp8(0x40250800, 0x80); /* Device to host; standard request. */
    outp8(0x40250801, 0x6);  /* GET_DESCRIPTOR */
    outp8(0x40250802, 0); /* wValue: descriptor type (high byte) and descriptor index (low byte). */
    outp8(0x40250803, 2); /* 2: configuration descriptor */
    outp8(0x40250804, 0); /* wIndex */
    outp8(0x40250805, 0);
    outp8(0x40250806, LEN_CONFIG + LEN_INTERFACE + LEN_ENDPOINT * 2); /* wLength (9+9+7*2) */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == MAX_PACKET_SIZE);
    /* Check EP0 buffer */
    for(u32Count = 0; u32Count < MAX_PACKET_SIZE; u32Count++)
    {
        if(inp8(0x40250800 + EP0_BUF_OFFSET + u32Count) != gu8ConfigDescriptorMSC[u32Count])
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);
    CU_ASSERT(USBD->EP[1].MXPLD == MAX_PACKET_SIZE);

    USBD_CtrlIn();
    CU_ASSERT(USBD->EP[0].MXPLD == ((LEN_CONFIG + LEN_INTERFACE + LEN_ENDPOINT * 2) - MAX_PACKET_SIZE));
    /* Check EP0 buffer */
    for(u32Count = 0; u32Count < (LEN_CONFIG + LEN_INTERFACE + LEN_ENDPOINT * 2) - MAX_PACKET_SIZE; u32Count++)
    {
        if(inp8(0x40250800 + EP0_BUF_OFFSET + u32Count) != gu8ConfigDescriptorMSC[MAX_PACKET_SIZE + u32Count])
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);


    USBD_Open(&gsInfo_HID, NULL, NULL);
    outp8(0x40250800, 0x80); /* Device to host; standard request. */
    outp8(0x40250801, 0x6);  /* GET_DESCRIPTOR */
    outp8(0x40250802, 0); /* wValue: descriptor type (high byte) and descriptor index (low byte). */
    outp8(0x40250803, DESC_HID_RPT); /* DESC_HID_RPT (0x22): HID report descriptor */
    outp8(0x40250804, 0); /* wIndex */
    outp8(0x40250805, 0);
    outp8(0x40250806, 52); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == MAX_PACKET_SIZE);
    /* Check EP0 buffer */
    for(u32Count = 0; u32Count < MAX_PACKET_SIZE; u32Count++)
    {
        if(inp8(0x40250800 + EP0_BUF_OFFSET + u32Count) != HID_MouseReportDescriptor[u32Count])
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);
    CU_ASSERT(USBD->EP[1].MXPLD == MAX_PACKET_SIZE);

    USBD_CtrlIn();
    CU_ASSERT(USBD->EP[0].MXPLD == MAX_PACKET_SIZE);
    /* Check EP0 buffer */
    for(u32Count = 0; u32Count < MAX_PACKET_SIZE; u32Count++)
    {
        if(inp8(0x40250800 + EP0_BUF_OFFSET + u32Count) != HID_MouseReportDescriptor[MAX_PACKET_SIZE + u32Count])
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);

    USBD_CtrlIn();
    CU_ASSERT(USBD->EP[0].MXPLD == (52 - MAX_PACKET_SIZE * 2));
    /* Check EP0 buffer */
    for(u32Count = 0; u32Count < (52 - MAX_PACKET_SIZE * 2); u32Count++)
    {
        if(inp8(0x40250800 + EP0_BUF_OFFSET + u32Count) != HID_MouseReportDescriptor[MAX_PACKET_SIZE * 2 + u32Count])
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);


    USBD_Open(&gsInfo_MSC, NULL, NULL);
    outp8(0x40250800, 0x80); /* Device to host; standard request. */
    outp8(0x40250801, 0x6);  /* GET_DESCRIPTOR */
    outp8(0x40250802, 0); /* wValue: descriptor type (high byte) and descriptor index (low byte). */
    outp8(0x40250803, 3); /* 3: string descriptor */
    outp8(0x40250804, 0); /* wIndex */
    outp8(0x40250805, 0);
    outp8(0x40250806, 4); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 4);
    /* Check EP0 buffer */
    for(u32Count = 0; u32Count < 4; u32Count++)
    {
        if(inp8(0x40250800 + EP0_BUF_OFFSET + u32Count) != gu8StringLang[u32Count])
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);
    CU_ASSERT(USBD->EP[1].MXPLD == MAX_PACKET_SIZE);

    outp8(0x40250800, 0x80); /* Device to host; standard request. */
    outp8(0x40250801, 0x6);  /* GET_DESCRIPTOR */
    outp8(0x40250802, 1); /* wValue: descriptor type (high byte) and descriptor index (low byte). */
    outp8(0x40250803, 3); /* 3: string descriptor */
    outp8(0x40250804, 0); /* wIndex */
    outp8(0x40250805, 0);
    outp8(0x40250806, 16); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 16);
    /* Check EP0 buffer */
    for(u32Count = 0; u32Count < 16; u32Count++)
    {
        if(inp8(0x40250800 + EP0_BUF_OFFSET + u32Count) != gu8VendorStringDesc[u32Count])
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);
    CU_ASSERT(USBD->EP[1].MXPLD == MAX_PACKET_SIZE);

    outp8(0x40250800, 0x80); /* Device to host; standard request. */
    outp8(0x40250801, 0x6);  /* GET_DESCRIPTOR */
    outp8(0x40250802, 2); /* wValue: descriptor type (high byte) and descriptor index (low byte). */
    outp8(0x40250803, 3); /* 3: string descriptor */
    outp8(0x40250804, 0); /* wIndex */
    outp8(0x40250805, 0);
    outp8(0x40250806, 22); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == MAX_PACKET_SIZE);
    /* Check EP0 buffer */
    for(u32Count = 0; u32Count < MAX_PACKET_SIZE; u32Count++)
    {
        if(inp8(0x40250800 + EP0_BUF_OFFSET + u32Count) != gu8ProductStringDesc[u32Count])
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);
    CU_ASSERT(USBD->EP[1].MXPLD == MAX_PACKET_SIZE);

    USBD_CtrlIn();
    CU_ASSERT(USBD->EP[0].MXPLD == (gu8ProductStringDesc[0] - MAX_PACKET_SIZE));
    /* Check EP0 buffer */
    for(u32Count = 0; u32Count < gu8ProductStringDesc[0] - MAX_PACKET_SIZE; u32Count++)
    {
        if(inp8(0x40250800 + EP0_BUF_OFFSET + u32Count) != gu8ProductStringDesc[MAX_PACKET_SIZE + u32Count])
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);


    /* --- Standard request "GET_STATUS" test --- */
    outp8(0x40250800, 0x80); /* Device to host; standard request. */
    outp8(0x40250801, 0x0);  /* GET_STATUS */
    outp8(0x40250802, 0); /* wValue */
    outp8(0x40250803, 0);
    outp8(0x40250804, 0); /* wIndex: 0 for device */
    outp8(0x40250805, 0);
    outp8(0x40250806, 0x2); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 2);
    CU_ASSERT(inp8(0x40250800 + EP0_BUF_OFFSET) == 1);
    CU_ASSERT(inp8(0x40250800 + EP0_BUF_OFFSET + 1) == 0);
    CU_ASSERT(USBD->EP[1].MXPLD == 20); /* MAX_PACKET_SIZE: 20 */

    outp8(0x40250800, 0x81); /* Device to host; standard request. */
    outp8(0x40250801, 0x0);  /* GET_STATUS */
    outp8(0x40250802, 0); /* wValue */
    outp8(0x40250803, 0);
    outp8(0x40250804, 0); /* wIndex: interface number */
    outp8(0x40250805, 0);
    outp8(0x40250806, 0x2); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 2);
    CU_ASSERT(inp8(0x40250800 + EP0_BUF_OFFSET) == 0);
    CU_ASSERT(inp8(0x40250800 + EP0_BUF_OFFSET + 1) == 0);
    CU_ASSERT(USBD->EP[1].MXPLD == 20); /* MAX_PACKET_SIZE: 20 */

    outp8(0x40250800, 0x82); /* Device to host; standard request. */
    outp8(0x40250801, 0x0);  /* GET_STATUS */
    outp8(0x40250802, 0); /* wValue */
    outp8(0x40250803, 0);
    outp8(0x40250804, 0); /* wIndex: endpoint number */
    outp8(0x40250805, 0);
    outp8(0x40250806, 0x2); /* wLength */
    outp8(0x40250807, 0);

    USBD_ProcessSetupPacket();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 2);
    CU_ASSERT(inp8(0x40250800 + EP0_BUF_OFFSET) == 0);
    CU_ASSERT(inp8(0x40250800 + EP0_BUF_OFFSET + 1) == 0);
    CU_ASSERT(USBD->EP[1].MXPLD == 20); /* MAX_PACKET_SIZE: 20 */

    outp8(0x40250800, 0x82); /* Device to host; standard request. */
    outp8(0x40250801, 0x0);  /* GET_STATUS */
    outp8(0x40250802, 0); /* wValue */
    outp8(0x40250803, 0);
    outp8(0x40250804, 2); /* wIndex: endpoint number */
    outp8(0x40250805, 0);
    outp8(0x40250806, 0x2); /* wLength */
    outp8(0x40250807, 0);
    USBD_SetStall(2);

    //USBD_ProcessSetupPacket();
    /* Get SETUP packet from USB buffer */
    USBD_MemCopy(g_usbd_SetupPacket, (uint8_t *)USBD_BUF_BASE, 8);
    USBD_StandardRequest();
    CU_ASSERT(USBD->EP[0].CFG & 0x80); /* DATA1 */
    CU_ASSERT(USBD->EP[0].MXPLD == 2);
    CU_ASSERT(inp8(0x40250800 + EP0_BUF_OFFSET) == 1);
    CU_ASSERT(inp8(0x40250800 + EP0_BUF_OFFSET + 1) == 0);
    CU_ASSERT(USBD->EP[1].MXPLD == 20); /* MAX_PACKET_SIZE: 20 */


    USBD_SwReset();
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void API_USBD_CtrlIn()
{
    uint32_t u32Count, u32ErrorCount = 0;
    uint8_t u8CtrlInBuffer[50];

    for(u32Count = 0; u32Count < 50; u32Count++)
        u8CtrlInBuffer[u32Count] = u32Count;

    USBD_Open(&gsInfo_MSC, NULL, NULL);
    USBD_CONFIG_EP(0, USBD_CFG_EPMODE_IN);
    outp32(0x40250500, 0x100); /* EP[0]->BUFSEG */

    USBD_PrepareCtrlIn(u8CtrlInBuffer, 50);
    CU_ASSERT(USBD->EP[0].CFG & 0x80);
    CU_ASSERT(USBD->EP[0].MXPLD == 20);
    for(u32Count = 0; u32Count < 20; u32Count++)
    {
        if(inp8(0x40250900 + u32Count) != u32Count)
            u32ErrorCount++;
    }

    USBD_CtrlIn();
    CU_ASSERT(USBD->EP[0].MXPLD == 20);
    for(u32Count = 0; u32Count < 20; u32Count++)
    {
        if(inp8(0x40250900 + u32Count) != u32Count + 20)
            u32ErrorCount++;
    }

    USBD_CtrlIn();
    CU_ASSERT(USBD->EP[0].MXPLD == 10);
    for(u32Count = 0; u32Count < 10; u32Count++)
    {
        if(inp8(0x40250900 + u32Count) != u32Count + 40)
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);

    for(u32Count = 0; u32Count < 3; u32Count++)
        u8CtrlInBuffer[u32Count] = 0xF | u32Count;

    USBD_PrepareCtrlIn(u8CtrlInBuffer, 3);
    CU_ASSERT(USBD->EP[0].CFG & 0x80);
    CU_ASSERT(USBD->EP[0].MXPLD == 3);
    for(u32Count = 0; u32Count < 3; u32Count++)
    {
        if(inp8(0x40250900 + u32Count) != (0xF | u32Count))
            u32ErrorCount++;
    }

    USBD_SwReset();
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void API_USBD_CtrlOut()
{
    uint32_t u32Count, u32ErrorCount = 0;
    uint8_t u8CtrlOutBuffer[50];

    for(u32Count = 0; u32Count < 50; u32Count++)
        u8CtrlOutBuffer[u32Count] = 0;

    USBD_Open(&gsInfo_MSC, NULL, NULL);
    USBD_CONFIG_EP(1, USBD_CFG_EPMODE_IN);
    outp32(0x40250510, 0x100); /* EP[1]->BUFSEG */
    USBD_PrepareCtrlOut(u8CtrlOutBuffer, 50);

    for(u32Count = 0; u32Count < 20; u32Count++)
        outp8(0x40250900 + u32Count, u32Count);
    USBD_CtrlOut();

    for(u32Count = 0; u32Count < 20; u32Count++)
        outp8(0x40250900 + u32Count, u32Count + 20);
    USBD_CtrlOut();

    outp32(0x40250514, 10); /* EP[1]->MXPLD */
    for(u32Count = 0; u32Count < 10; u32Count++)
        outp8(0x40250900 + u32Count, u32Count + 40);
    USBD_CtrlOut();

    for(u32Count = 0; u32Count < 50; u32Count++)
    {
        if(u8CtrlOutBuffer[u32Count] != u32Count)
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);

    USBD_SwReset();
    /* Reset USBD */
  SYS_ResetModule(SYS_USBD0RST);
}

void API_USBD_SwReset()
{
    uint32_t u32Count, u32ErrorCount = 0;

    USBD->FADDR = 0x55;

    for(u32Count = 0; u32Count < 8; u32Count++)
        g_usbd_SetupPacket[u32Count] = u32Count;

    USBD_SwReset();

    CU_ASSERT(USBD->FADDR == 0);
    for(u32Count = 0; u32Count < 8; u32Count++)
    {
        if(g_usbd_SetupPacket[u32Count] != 0)
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);

    u32ErrorCount = 0;
    for(u32Count = 0; u32Count < 8; u32Count++)
    {
        if((USBD->EP[u32Count].CFG & USBD_CFG_DSQSYNC_Msk) != 0)
            u32ErrorCount++;
    }
    CU_ASSERT_EQUAL(u32ErrorCount, 0);
}

void VendorRequestCB()
{
    __NOP();
}

extern VENDOR_REQ g_usbd_pfnVendorRequest;

void API_USBD_SetVendorRequest()
{
    USBD_SetVendorRequest((VENDOR_REQ)VendorRequestCB);
    CU_ASSERT(g_usbd_pfnVendorRequest == VendorRequestCB);
}


void SetConfigCB()
{
    __NOP();
}

extern SET_CONFIG_CB g_usbd_pfnSetConfigCallback;

void API_USBD_SetConfigCallback()
{
    USBD_SetConfigCallback((SET_CONFIG_CB)SetConfigCB);
    CU_ASSERT(g_usbd_pfnSetConfigCallback == SetConfigCB);
}

extern uint32_t g_u32EpStallLock;
void API_USBD_LockEpStall()
{
    USBD_LockEpStall(0x11);
    CU_ASSERT(g_u32EpStallLock == 0x11);
    USBD_LockEpStall(0x22);
    CU_ASSERT(g_u32EpStallLock == 0x22);
}



void CONST_USBD_Constants()
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

    /* The length of Smart Card Device Class Descriptor is defined in Smart-Card CCID specification. */
    CU_ASSERT(LEN_CCID == 0x36);

    /* Transfer type constant definition test for bmAttributes field of endpoint descriptor */
    CU_ASSERT(EP_ISO == 1);
    CU_ASSERT(EP_BULK == 2);
    CU_ASSERT(EP_INT == 3);

    /* Direction constant definition test for bEndpointAddress field of endpoint descriptor */
    CU_ASSERT(EP_INPUT == 0x80);
    CU_ASSERT(EP_OUTPUT == 0);

    USBD->INTEN = USBD_WAKEUP_EN;
    CU_ASSERT(USBD->INTEN == 0x100);

    /* USB event constant definition test */
    CU_ASSERT(USBD_INTSTS_SETUP == 0x80000000);
    CU_ASSERT(USBD_INTSTS_EP0 == 0x00010000);
    CU_ASSERT(USBD_INTSTS_EP1 == 0x00020000);
    CU_ASSERT(USBD_INTSTS_EP2 == 0x00040000);
    CU_ASSERT(USBD_INTSTS_EP3 == 0x00080000);
    CU_ASSERT(USBD_INTSTS_EP4 == 0x00100000);
    CU_ASSERT(USBD_INTSTS_EP5 == 0x00200000);
    CU_ASSERT(USBD_INTSTS_EP6 == 0x00400000);
    CU_ASSERT(USBD_INTSTS_EP7 == 0x00800000);
    CU_ASSERT(USBD_INTSTS_EP8 == 0x01000000);
    CU_ASSERT(USBD_INTSTS_EP9 == 0x02000000);
    CU_ASSERT(USBD_INTSTS_EP10 == 0x04000000);
    CU_ASSERT(USBD_INTSTS_EP11 == 0x08000000);

    CU_ASSERT(USBD_EPINTSTS_EP0 == 0x00000001);
    CU_ASSERT(USBD_EPINTSTS_EP1 == 0x00000002);
    CU_ASSERT(USBD_EPINTSTS_EP2 == 0x00000004);
    CU_ASSERT(USBD_EPINTSTS_EP3 == 0x00000008);
    CU_ASSERT(USBD_EPINTSTS_EP4 == 0x00000010);
    CU_ASSERT(USBD_EPINTSTS_EP5 == 0x00000020);
    CU_ASSERT(USBD_EPINTSTS_EP6 == 0x00000040);
    CU_ASSERT(USBD_EPINTSTS_EP7 == 0x00000080);
    CU_ASSERT(USBD_EPINTSTS_EP8 == 0x00000100);
    CU_ASSERT(USBD_EPINTSTS_EP9 == 0x00000200);
    CU_ASSERT(USBD_EPINTSTS_EP10 == 0x00000400);
    CU_ASSERT(USBD_EPINTSTS_EP11 == 0x00000800);
    CU_ASSERT(USBD_EPINTSTS_EP12 == 0x00001000);
    CU_ASSERT(USBD_EPINTSTS_EP13 == 0x00002000);
    CU_ASSERT(USBD_EPINTSTS_EP14 == 0x00004000);
    CU_ASSERT(USBD_EPINTSTS_EP15 == 0x00008000);
    CU_ASSERT(USBD_EPINTSTS_EP16 == 0x00010000);
    CU_ASSERT(USBD_EPINTSTS_EP17 == 0x00020000);
    CU_ASSERT(USBD_EPINTSTS_EP18 == 0x00040000);
    CU_ASSERT(USBD_EPINTSTS_EP19 == 0x00080000);
    CU_ASSERT(USBD_EPINTSTS_EP20 == 0x00100000);
    CU_ASSERT(USBD_EPINTSTS_EP21 == 0x00200000);
    CU_ASSERT(USBD_EPINTSTS_EP22 == 0x00400000);
    CU_ASSERT(USBD_EPINTSTS_EP23 == 0x00800000);
    CU_ASSERT(USBD_EPINTSTS_EP24 == 0x01000000);

    /* USBD_ATTR status flag check */
    CU_ASSERT(USBD_STATE_USBRST == 1);
    CU_ASSERT(USBD_STATE_SUSPEND == 2);
    CU_ASSERT(USBD_STATE_RESUME == 4);
    CU_ASSERT(USBD_STATE_TIMEOUT == 8);
    CU_ASSERT(USBD_STATE_L1SUSPEND == 0x1000);
    CU_ASSERT(USBD_STATE_L1RESUME == 0x2000);
}



CU_TestInfo USBD_MacroTests[] =
{

    {"Get maximum value", MACRO_USBD_Maximum},
    {"Get minimum value", MACRO_USBD_Minimum},
    {"Enable/Disable USB", MACRO_USBD_ENABLE_DISABLE_USB},
    {"Enable/Disable USB PHY", MACRO_USBD_ENABLE_DISABLE_PHY},
    {"Set/Clear USB DRVSE0 bit", MACRO_USBD_SET_CLEAR_SE0},
    {"Set/Get USB device address", MACRO_USBD_SET_GET_ADDR},
    {"Enable USBD interrupt function", MACRO_USBD_ENABLE_INT},
    {"Get/Clear USB interrupt flag", MACRO_USBD_GET_CLR_INT_FLAG},
    {"Get/Clear USB endpoint interrupt flag", MACRO_USBD_GET_CLR_EP_INT_FLAG},
    {"Get USB endpoint status", MACRO_USBD_GET_EP_FLAG},
    {"Get USB bus state", MACRO_USBD_GET_BUS_STATE},
    {"Get USB bus connection state", MACRO_USBD_IS_ATTACHED},
    {"Stop USB transaction", MACRO_USBD_STOP_TRANSACTION},
    {"Set data sequence synchronization", MACRO_USBD_SET_DATA0_DATA1},
    {"Set/Get USB payload size", MACRO_USBD_SET_GET_PAYLOAD_LEN},
    {"Configure endpoint", MACRO_USBD_CONFIG_EP},
    {"Set/Get endpoint buffer address", MACRO_USBD_SET_GET_EP_BUF_ADDR},
    {"Set/Clear/Get STALL state of the specified endpoint ID", MACRO_USBD_SET_CLR_GET_EP_STALL},
    {"Set/Get buffer mode of the specified endpoint ID", MACRO_USBD_SET_DB_SB_MODE_IS_DB_MODE},
    {"Set to active/inactive double buffer mode of the specified endpoint ID", MACRO_USBD_SET_DB_ACTIVE_INACTIVE},
    {"Memory copy test", MACRO_USBD_MemCopy},
    {"Set/Clear/Get STALL", MACRO_USBD_SetStall_ClearStall_GetStall},

    CU_TEST_INFO_NULL
};

CU_TestInfo USBD_ApiTests[] =
{

    {"USB open", API_USBD_Open},
    {"USB device start", API_USBD_Start},
    {"Get SETUP packet", API_USBD_GetSetupPacket},
    {"Process USB SETUP packet", API_USBD_ProcessSetupPacket},
    {"USB control IN test", API_USBD_CtrlIn},
    {"USB control OUT test", API_USBD_CtrlOut},
    {"USB software reset", API_USBD_SwReset},
    {"Set USBD vendor request callback function", API_USBD_SetVendorRequest},
    {"Set USBD callback function for SET_CONFIGURATION request", API_USBD_SetConfigCallback},
    {"Lock endpoint to avoid stall clear", API_USBD_LockEpStall},

    CU_TEST_INFO_NULL
};

CU_TestInfo USBD_ConstantTest[] =
{
    {"Endpoint ID constant test", CONST_USBD_Constants},

    CU_TEST_INFO_NULL
};
