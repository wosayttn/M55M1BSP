/******************************************************************************
* @file    otg_cunit.c
* @version V1.00
* @brief   otg CUnit Test
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

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
#include "otg_cunit.h"

extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int OTG_Tests_Init(void)
{
    return 0;
}
int OTG_Tests_Clean(void)
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
    {"OTG MACRO", OTG_Tests_Init, OTG_Tests_Clean, NULL, NULL, OTG_MacroTests},
    CU_SUITE_INFO_NULL
};



void MACRO_OTG_ENABLE_DISABLE()
{
    CU_ASSERT_FALSE(OTG->CTL & 0x10);
    OTG_ENABLE();
    CU_ASSERT((OTG->CTL & 0x10) == 0x10);
    OTG_DISABLE();
    CU_ASSERT_FALSE(OTG->CTL & 0x10);
}

void MACRO_OTG_ENABLE_DISABLE_PHY()
{
    CU_ASSERT_FALSE(OTG->PHYCTL & 0x1);
    OTG_ENABLE_PHY();
    CU_ASSERT((OTG->PHYCTL & 0x1) == 0x1);
    OTG_DISABLE_PHY();
    CU_ASSERT_FALSE(OTG->PHYCTL & 0x1);
}

void MACRO_OTG_ENABLE_DISABLE_ID_DETECT()
{
    CU_ASSERT_FALSE(OTG->PHYCTL & 0x2);
    OTG_ENABLE_ID_DETECT();
    CU_ASSERT((OTG->PHYCTL & 0x2) == 0x2);
    OTG_DISABLE_ID_DETECT();
    CU_ASSERT_FALSE(OTG->PHYCTL & 0x2);
}

void MACRO_OTG_ENABLE_DISABLE_WAKEUP()
{
    CU_ASSERT_FALSE(OTG->CTL & 0x20);
    OTG_ENABLE_WAKEUP();
    CU_ASSERT((OTG->CTL & 0x20) == 0x20);
    OTG_DISABLE_WAKEUP();
    CU_ASSERT_FALSE(OTG->CTL & 0x20);
}

void MACRO_OTG_SET_VBUS_EN_POL()
{
    CU_ASSERT_FALSE(OTG->PHYCTL & 0x10);
    OTG_SET_VBUS_EN_POL(OTG_VBUS_EN_ACTIVE_LOW);
    CU_ASSERT((OTG->PHYCTL & 0x10) == 0x10);
    OTG_SET_VBUS_EN_POL(OTG_VBUS_EN_ACTIVE_HIGH);
    CU_ASSERT_FALSE(OTG->PHYCTL & 0x10);
}

void MACRO_OTG_SET_VBUS_STS_POL()
{
    CU_ASSERT_FALSE(OTG->PHYCTL & 0x20);
    OTG_SET_VBUS_STS_POL(OTG_VBUS_ST_VALID_LOW);
    CU_ASSERT((OTG->PHYCTL & 0x20) == 0x20);
    OTG_SET_VBUS_STS_POL(OTG_VBUS_ST_VALID_HIGH);
    CU_ASSERT_FALSE(OTG->PHYCTL & 0x20);
}

void MACRO_OTG_ENABLE_DISABLE_INT()
{
    CU_ASSERT(OTG->INTEN == 0);
    OTG_ENABLE_INT(OTG_INTEN_ROLECHGIEN_Msk);
    CU_ASSERT(OTG->INTEN == 1);
    OTG_DISABLE_INT(OTG_INTEN_ROLECHGIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0);

    OTG_ENABLE_INT(OTG_INTEN_VBEIEN_Msk);
    CU_ASSERT(OTG->INTEN == 2);
    OTG_DISABLE_INT(OTG_INTEN_VBEIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0);

    OTG_ENABLE_INT(OTG_INTEN_SRPFIEN_Msk);
    CU_ASSERT(OTG->INTEN == 4);
    OTG_DISABLE_INT(OTG_INTEN_SRPFIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0);

    OTG_ENABLE_INT(OTG_INTEN_HNPFIEN_Msk);
    CU_ASSERT(OTG->INTEN == 8);
    OTG_DISABLE_INT(OTG_INTEN_HNPFIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0);

    OTG_ENABLE_INT(OTG_INTEN_GOIDLEIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0x10);
    OTG_DISABLE_INT(OTG_INTEN_GOIDLEIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0);

    OTG_ENABLE_INT(OTG_INTEN_IDCHGIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0x20);
    OTG_DISABLE_INT(OTG_INTEN_IDCHGIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0);

    OTG_ENABLE_INT(OTG_INTEN_PDEVIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0x40);
    OTG_DISABLE_INT(OTG_INTEN_PDEVIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0);

    OTG_ENABLE_INT(OTG_INTEN_HOSTIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0x80);
    OTG_DISABLE_INT(OTG_INTEN_HOSTIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0);

    OTG_ENABLE_INT(OTG_INTEN_BVLDCHGIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0x100);
    OTG_DISABLE_INT(OTG_INTEN_BVLDCHGIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0);

    OTG_ENABLE_INT(OTG_INTEN_AVLDCHGIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0x200);
    OTG_DISABLE_INT(OTG_INTEN_AVLDCHGIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0);

    OTG_ENABLE_INT(OTG_INTEN_VBCHGIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0x400);
    OTG_DISABLE_INT(OTG_INTEN_VBCHGIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0);

    OTG_ENABLE_INT(OTG_INTEN_SECHGIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0x800);
    OTG_DISABLE_INT(OTG_INTEN_SECHGIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0);

    OTG_ENABLE_INT(OTG_INTEN_SRPDETIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0x2000);
    OTG_DISABLE_INT(OTG_INTEN_SRPDETIEN_Msk);
    CU_ASSERT(OTG->INTEN == 0);
}

void MACRO_OTG_GET_CLR_INT_FLAG()
{
    /* Clear all interrupt flags */
    OTG->INTSTS = OTG->INTSTS;
    CU_ASSERT_FALSE(OTG_GET_INT_FLAG(OTG_INTSTS_ROLECHGIF_Msk));
    OTG_CLR_INT_FLAG(OTG_INTSTS_ROLECHGIF_Msk);
    CU_ASSERT(OTG->INTSTS == 0);
    CU_ASSERT_FALSE(OTG_GET_INT_FLAG(OTG_INTSTS_VBEIF_Msk));
    OTG_CLR_INT_FLAG(OTG_INTSTS_VBEIF_Msk);
    CU_ASSERT(OTG->INTSTS == 0);
    CU_ASSERT_FALSE(OTG_GET_INT_FLAG(OTG_INTSTS_SRPFIF_Msk));
    OTG_CLR_INT_FLAG(OTG_INTSTS_SRPFIF_Msk);
    CU_ASSERT(OTG->INTSTS == 0);
    CU_ASSERT_FALSE(OTG_GET_INT_FLAG(OTG_INTSTS_HNPFIF_Msk));
    OTG_CLR_INT_FLAG(OTG_INTSTS_HNPFIF_Msk);
    CU_ASSERT(OTG->INTSTS == 0);
    CU_ASSERT_FALSE(OTG_GET_INT_FLAG(OTG_INTSTS_GOIDLEIF_Msk));
    OTG_CLR_INT_FLAG(OTG_INTSTS_GOIDLEIF_Msk);
    CU_ASSERT(OTG->INTSTS == 0);
    CU_ASSERT_FALSE(OTG_GET_INT_FLAG(OTG_INTSTS_IDCHGIF_Msk));
    OTG_CLR_INT_FLAG(OTG_INTSTS_IDCHGIF_Msk);
    CU_ASSERT(OTG->INTSTS == 0);
    CU_ASSERT_FALSE(OTG_GET_INT_FLAG(OTG_INTSTS_PDEVIF_Msk));
    OTG_CLR_INT_FLAG(OTG_INTSTS_PDEVIF_Msk);
    CU_ASSERT(OTG->INTSTS == 0);
    CU_ASSERT_FALSE(OTG_GET_INT_FLAG(OTG_INTSTS_HOSTIF_Msk));
    OTG_CLR_INT_FLAG(OTG_INTSTS_HOSTIF_Msk);
    CU_ASSERT(OTG->INTSTS == 0);
    CU_ASSERT_FALSE(OTG_GET_INT_FLAG(OTG_INTSTS_BVLDCHGIF_Msk));
    OTG_CLR_INT_FLAG(OTG_INTSTS_BVLDCHGIF_Msk);
    CU_ASSERT(OTG->INTSTS == 0);
    CU_ASSERT_FALSE(OTG_GET_INT_FLAG(OTG_INTSTS_AVLDCHGIF_Msk));
    OTG_CLR_INT_FLAG(OTG_INTSTS_AVLDCHGIF_Msk);
    CU_ASSERT(OTG->INTSTS == 0);
    CU_ASSERT_FALSE(OTG_GET_INT_FLAG(OTG_INTSTS_VBCHGIF_Msk));
    OTG_CLR_INT_FLAG(OTG_INTSTS_VBCHGIF_Msk);
    CU_ASSERT(OTG->INTSTS == 0);
    CU_ASSERT_FALSE(OTG_GET_INT_FLAG(OTG_INTSTS_SECHGIF_Msk));
    OTG_CLR_INT_FLAG(OTG_INTSTS_SECHGIF_Msk);
    CU_ASSERT(OTG->INTSTS == 0);
    CU_ASSERT_FALSE(OTG_GET_INT_FLAG(OTG_INTSTS_SRPDETIF_Msk));
    OTG_CLR_INT_FLAG(OTG_INTSTS_SRPDETIF_Msk);
    CU_ASSERT(OTG->INTSTS == 0);
}

void MACRO_OTG_GET_STATUS()
{
    CU_ASSERT_FALSE(OTG_GET_STATUS(OTG_STATUS_OVERCUR_Msk));
    CU_ASSERT_FALSE(OTG_GET_STATUS(OTG_STATUS_IDSTS_Msk));
    CU_ASSERT_TRUE(OTG_GET_STATUS(OTG_STATUS_SESSEND_Msk));
    CU_ASSERT_FALSE(OTG_GET_STATUS(OTG_STATUS_BVLD_Msk));
    CU_ASSERT_FALSE(OTG_GET_STATUS(OTG_STATUS_AVLD_Msk));
    CU_ASSERT_FALSE(OTG_GET_STATUS(OTG_STATUS_VBUSVLD_Msk));
    CU_ASSERT_FALSE(OTG_GET_STATUS(OTG_STATUS_ASPERI_Msk));
    CU_ASSERT_FALSE(OTG_GET_STATUS(OTG_STATUS_ASHOST_Msk));
}

CU_TestInfo OTG_MacroTests[] =
{

    {"Enable/Disable OTG", MACRO_OTG_ENABLE_DISABLE},
    {"Enable/Disable USB PHY", MACRO_OTG_ENABLE_DISABLE_PHY},
    {"Enable/Disable OTG ID detection function", MACRO_OTG_ENABLE_DISABLE_ID_DETECT},
    {"Enable/Disable OTG wake-up function", MACRO_OTG_ENABLE_DISABLE_WAKEUP},
    {"Set the polarity of USB_VBUS_EN pin", MACRO_OTG_SET_VBUS_EN_POL},
    {"Set the polarity of USB_VBUS_ST pin", MACRO_OTG_SET_VBUS_STS_POL},
    {"Enable/Disable OTG related interrupts", MACRO_OTG_ENABLE_DISABLE_INT},
    {"Get/Clear OTG related interrupt flags", MACRO_OTG_GET_CLR_INT_FLAG},
    {"Get OTG related status", MACRO_OTG_GET_STATUS},

    CU_TEST_INFO_NULL
};
