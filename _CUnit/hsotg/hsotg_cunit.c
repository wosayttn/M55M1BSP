/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.                                        */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   hsotg_cunit.c                                                                                         */
/*            The test function of HSOTG for CUnit.                                                        */
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
#include "hsotg_cunit.h"

extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int HSOTG_Tests_Init(void)
{
    return 0;
}
int HSOTG_Tests_Clean(void)
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
    {"HSOTG MACRO", HSOTG_Tests_Init, HSOTG_Tests_Clean, NULL, NULL, HSOTG_MacroTests},
    CU_SUITE_INFO_NULL
};



void MACRO_HSOTG_ENABLE_DISABLE()
{
    CU_ASSERT_FALSE(HSOTG->CTL & 0x10);
    HSOTG_ENABLE();
    CU_ASSERT((HSOTG->CTL & 0x10) == 0x10);
    HSOTG_DISABLE();
    CU_ASSERT_FALSE(HSOTG->CTL & 0x10);
}

void MACRO_HSOTG_ENABLE_DISABLE_PHY()
{
    CU_ASSERT_FALSE(HSOTG->PHYCTL & 0x1);
    HSOTG_ENABLE_PHY();
    CU_ASSERT((HSOTG->PHYCTL & 0x1) == 0x1);
    HSOTG_DISABLE_PHY();
    CU_ASSERT_FALSE(HSOTG->PHYCTL & 0x1);
}

void MACRO_HSOTG_ENABLE_DISABLE_ID_DETECT()
{
    CU_ASSERT_FALSE(HSOTG->PHYCTL & 0x2);
    HSOTG_ENABLE_ID_DETECT();
    CU_ASSERT((HSOTG->PHYCTL & 0x2) == 0x2);
    HSOTG_DISABLE_ID_DETECT();
    CU_ASSERT_FALSE(HSOTG->PHYCTL & 0x2);
}

void MACRO_HSOTG_ENABLE_DISABLE_WAKEUP()
{
    CU_ASSERT_FALSE(HSOTG->CTL & 0x20);
    HSOTG_ENABLE_WAKEUP();
    CU_ASSERT((HSOTG->CTL & 0x20) == 0x20);
    HSOTG_DISABLE_WAKEUP();
    CU_ASSERT_FALSE(HSOTG->CTL & 0x20);
}

void MACRO_HSOTG_SET_VBUS_EN_POL()
{
    CU_ASSERT_FALSE(HSOTG->PHYCTL & 0x10);
    HSOTG_SET_VBUS_EN_POL(HSOTG_VBUS_EN_ACTIVE_LOW);
    CU_ASSERT((HSOTG->PHYCTL & 0x10) == 0x10);
    HSOTG_SET_VBUS_EN_POL(HSOTG_VBUS_EN_ACTIVE_HIGH);
    CU_ASSERT_FALSE(HSOTG->PHYCTL & 0x10);
}

void MACRO_HSOTG_SET_VBUS_STS_POL()
{
    CU_ASSERT_FALSE(HSOTG->PHYCTL & 0x20);
    HSOTG_SET_VBUS_STS_POL(HSOTG_VBUS_ST_VALID_LOW);
    CU_ASSERT((HSOTG->PHYCTL & 0x20) == 0x20);
    HSOTG_SET_VBUS_STS_POL(HSOTG_VBUS_ST_VALID_HIGH);
    CU_ASSERT_FALSE(HSOTG->PHYCTL & 0x20);
}

void MACRO_HSOTG_ENABLE_DISABLE_INT()
{
    CU_ASSERT(HSOTG->INTEN == 0);
    HSOTG_ENABLE_INT(HSOTG_INTEN_ROLECHGIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 1);
    HSOTG_DISABLE_INT(HSOTG_INTEN_ROLECHGIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0);

    HSOTG_ENABLE_INT(HSOTG_INTEN_VBEIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 2);
    HSOTG_DISABLE_INT(HSOTG_INTEN_VBEIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0);

    HSOTG_ENABLE_INT(HSOTG_INTEN_SRPFIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 4);
    HSOTG_DISABLE_INT(HSOTG_INTEN_SRPFIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0);

    HSOTG_ENABLE_INT(HSOTG_INTEN_HNPFIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 8);
    HSOTG_DISABLE_INT(HSOTG_INTEN_HNPFIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0);

    HSOTG_ENABLE_INT(HSOTG_INTEN_GOIDLEIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0x10);
    HSOTG_DISABLE_INT(HSOTG_INTEN_GOIDLEIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0);

    HSOTG_ENABLE_INT(HSOTG_INTEN_IDCHGIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0x20);
    HSOTG_DISABLE_INT(HSOTG_INTEN_IDCHGIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0);

    HSOTG_ENABLE_INT(HSOTG_INTEN_PDEVIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0x40);
    HSOTG_DISABLE_INT(HSOTG_INTEN_PDEVIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0);

    HSOTG_ENABLE_INT(HSOTG_INTEN_HOSTIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0x80);
    HSOTG_DISABLE_INT(HSOTG_INTEN_HOSTIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0);

    HSOTG_ENABLE_INT(HSOTG_INTEN_BVLDCHGIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0x100);
    HSOTG_DISABLE_INT(HSOTG_INTEN_BVLDCHGIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0);

    HSOTG_ENABLE_INT(HSOTG_INTEN_AVLDCHGIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0x200);
    HSOTG_DISABLE_INT(HSOTG_INTEN_AVLDCHGIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0);

    HSOTG_ENABLE_INT(HSOTG_INTEN_VBCHGIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0x400);
    HSOTG_DISABLE_INT(HSOTG_INTEN_VBCHGIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0);

    HSOTG_ENABLE_INT(HSOTG_INTEN_SECHGIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0x800);
    HSOTG_DISABLE_INT(HSOTG_INTEN_SECHGIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0);

    HSOTG_ENABLE_INT(HSOTG_INTEN_SRPDETIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0x2000);
    HSOTG_DISABLE_INT(HSOTG_INTEN_SRPDETIEN_Msk);
    CU_ASSERT(HSOTG->INTEN == 0);
}

void MACRO_HSOTG_GET_CLR_INT_FLAG()
{
    /* Clear all interrupt flags */
    HSOTG->INTSTS = HSOTG->INTSTS;
    CU_ASSERT_FALSE(HSOTG_GET_INT_FLAG(HSOTG_INTSTS_ROLECHGIF_Msk));
    HSOTG_CLR_INT_FLAG(HSOTG_INTSTS_ROLECHGIF_Msk);
    CU_ASSERT(HSOTG->INTSTS == 0);
    CU_ASSERT_FALSE(HSOTG_GET_INT_FLAG(HSOTG_INTSTS_VBEIF_Msk));
    HSOTG_CLR_INT_FLAG(HSOTG_INTSTS_VBEIF_Msk);
    CU_ASSERT(HSOTG->INTSTS == 0);
    CU_ASSERT_FALSE(HSOTG_GET_INT_FLAG(HSOTG_INTSTS_SRPFIF_Msk));
    HSOTG_CLR_INT_FLAG(HSOTG_INTSTS_SRPFIF_Msk);
    CU_ASSERT(HSOTG->INTSTS == 0);
    CU_ASSERT_FALSE(HSOTG_GET_INT_FLAG(HSOTG_INTSTS_HNPFIF_Msk));
    HSOTG_CLR_INT_FLAG(HSOTG_INTSTS_HNPFIF_Msk);
    CU_ASSERT(HSOTG->INTSTS == 0);
    CU_ASSERT_FALSE(HSOTG_GET_INT_FLAG(HSOTG_INTSTS_GOIDLEIF_Msk));
    HSOTG_CLR_INT_FLAG(HSOTG_INTSTS_GOIDLEIF_Msk);
    CU_ASSERT(HSOTG->INTSTS == 0);
    CU_ASSERT_FALSE(HSOTG_GET_INT_FLAG(HSOTG_INTSTS_IDCHGIF_Msk));
    HSOTG_CLR_INT_FLAG(HSOTG_INTSTS_IDCHGIF_Msk);
    CU_ASSERT(HSOTG->INTSTS == 0);
    CU_ASSERT_FALSE(HSOTG_GET_INT_FLAG(HSOTG_INTSTS_PDEVIF_Msk));
    HSOTG_CLR_INT_FLAG(HSOTG_INTSTS_PDEVIF_Msk);
    CU_ASSERT(HSOTG->INTSTS == 0);
    CU_ASSERT_FALSE(HSOTG_GET_INT_FLAG(HSOTG_INTSTS_HOSTIF_Msk));
    HSOTG_CLR_INT_FLAG(HSOTG_INTSTS_HOSTIF_Msk);
    CU_ASSERT(HSOTG->INTSTS == 0);
    CU_ASSERT_FALSE(HSOTG_GET_INT_FLAG(HSOTG_INTSTS_BVLDCHGIF_Msk));
    HSOTG_CLR_INT_FLAG(HSOTG_INTSTS_BVLDCHGIF_Msk);
    CU_ASSERT(HSOTG->INTSTS == 0);
    CU_ASSERT_FALSE(HSOTG_GET_INT_FLAG(HSOTG_INTSTS_AVLDCHGIF_Msk));
    HSOTG_CLR_INT_FLAG(HSOTG_INTSTS_AVLDCHGIF_Msk);
    CU_ASSERT(HSOTG->INTSTS == 0);
    CU_ASSERT_FALSE(HSOTG_GET_INT_FLAG(HSOTG_INTSTS_VBCHGIF_Msk));
    HSOTG_CLR_INT_FLAG(HSOTG_INTSTS_VBCHGIF_Msk);
    CU_ASSERT(HSOTG->INTSTS == 0);
    CU_ASSERT_FALSE(HSOTG_GET_INT_FLAG(HSOTG_INTSTS_SECHGIF_Msk));
    HSOTG_CLR_INT_FLAG(HSOTG_INTSTS_SECHGIF_Msk);
    CU_ASSERT(HSOTG->INTSTS == 0);
    CU_ASSERT_FALSE(HSOTG_GET_INT_FLAG(HSOTG_INTSTS_SRPDETIF_Msk));
    HSOTG_CLR_INT_FLAG(HSOTG_INTSTS_SRPDETIF_Msk);
    CU_ASSERT(HSOTG->INTSTS == 0);
}

void MACRO_HSOTG_GET_STATUS()
{
    CU_ASSERT_FALSE(HSOTG_GET_STATUS(HSOTG_STATUS_OVERCUR_Msk));
    CU_ASSERT_FALSE(HSOTG_GET_STATUS(HSOTG_STATUS_IDSTS_Msk));
    CU_ASSERT(HSOTG_GET_STATUS(HSOTG_STATUS_SESSEND_Msk));
    CU_ASSERT_FALSE(HSOTG_GET_STATUS(HSOTG_STATUS_BVLD_Msk));
    CU_ASSERT_FALSE(HSOTG_GET_STATUS(HSOTG_STATUS_AVLD_Msk));
    CU_ASSERT_FALSE(HSOTG_GET_STATUS(HSOTG_STATUS_VBUSVLD_Msk));
    CU_ASSERT_FALSE(HSOTG_GET_STATUS(HSOTG_STATUS_ASPERI_Msk));
    CU_ASSERT_FALSE(HSOTG_GET_STATUS(HSOTG_STATUS_ASHOST_Msk));
}

//void MACRO_HSOTG_SET_PHY_REF_CLK()
//{
//    CU_ASSERT((HSOTG->PHYCTL & 0x700) == 0x200);
//    HSOTG_SET_PHY_REF_CLK(HSOTG_PHYCTL_FSEL_9_6M);
//    CU_ASSERT((HSOTG->PHYCTL & 0x700) == 0);
//    HSOTG_SET_PHY_REF_CLK(HSOTG_PHYCTL_FSEL_10_0M);
//    CU_ASSERT((HSOTG->PHYCTL & 0x700) == 0x100);
//    HSOTG_SET_PHY_REF_CLK(HSOTG_PHYCTL_FSEL_19_2M);
//    CU_ASSERT((HSOTG->PHYCTL & 0x700) == 0x300);
//    HSOTG_SET_PHY_REF_CLK(HSOTG_PHYCTL_FSEL_20_0M);
//    CU_ASSERT((HSOTG->PHYCTL & 0x700) == 0x400);
//    HSOTG_SET_PHY_REF_CLK(HSOTG_PHYCTL_FSEL_24_0M);
//    CU_ASSERT((HSOTG->PHYCTL & 0x700) == 0x500);
//    HSOTG_SET_PHY_REF_CLK(HSOTG_PHYCTL_FSEL_50_0M);
//    CU_ASSERT((HSOTG->PHYCTL & 0x700) == 0x700);
//    HSOTG_SET_PHY_REF_CLK(HSOTG_PHYCTL_FSEL_12_0M);
//    CU_ASSERT((HSOTG->PHYCTL & 0x700) == 0x200);
//}

CU_TestInfo HSOTG_MacroTests[] =
{

    {"Enable/Disable HSOTG", MACRO_HSOTG_ENABLE_DISABLE},
    {"Enable/Disable USB PHY", MACRO_HSOTG_ENABLE_DISABLE_PHY},
    {"Enable/Disable HSOTG ID detection function", MACRO_HSOTG_ENABLE_DISABLE_ID_DETECT},
    {"Enable/Disable HSOTG wake-up function", MACRO_HSOTG_ENABLE_DISABLE_WAKEUP},
    {"Set the polarity of USB_VBUS_EN pin", MACRO_HSOTG_SET_VBUS_EN_POL},
    {"Set the polarity of USB_VBUS_ST pin", MACRO_HSOTG_SET_VBUS_STS_POL},
    {"Enable/Disable HSOTG related interrupts", MACRO_HSOTG_ENABLE_DISABLE_INT},
    {"Get/Clear HSOTG related interrupt flags", MACRO_HSOTG_GET_CLR_INT_FLAG},
    {"Get HSOTG related status", MACRO_HSOTG_GET_STATUS},
    //{"Set OTG PHY reference clock frequency", MACRO_HSOTG_SET_PHY_REF_CLK},

    CU_TEST_INFO_NULL
};
