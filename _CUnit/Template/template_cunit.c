/******************************************************************************
* @file    template_cunit.c
* @version V1.00
* @brief   CUnit test template
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "template_cunit.h"

int Template_Test_Init(void)
{
    return 0;
}
int Template_Test_Clean(void)
{
    return 0;
}

void Const1_Test(void)
{
    //CU_FAIL("[Const1_Test] Failed !\n");
    CU_PASS();
}

void Const2_Test(void)
{
    CU_PASS();
}

void Macro1_Test(void)
{
    //CU_FAIL("[Macro1_Test] Failed !\n");
    CU_PASS();
}

void Macro2_Test(void)
{
    CU_PASS();
}

void Func1_Test(void)
{
    //CU_FAIL("[Func1_Test] Failed !\n");
    CU_PASS();
}

void Func2_Test(void)
{
    CU_PASS();
}

CU_SuiteInfo TemplateSuites[] =
{
    { "Template Const Test", Template_Test_Init, Template_Test_Clean, NULL, NULL, Template_ConstTest },
    { "Template Macro Test", Template_Test_Init, Template_Test_Clean, NULL, NULL, Template_MacroTest },
    { "Template Func  Test", Template_Test_Init, Template_Test_Clean, NULL, NULL, Template_FuncTest },
    CU_SUITE_INFO_NULL
};

CU_TestInfo  Template_ConstTest[] =
{
    { "Const1",  Const1_Test },
    { "Const2",  Const2_Test },
    CU_TEST_INFO_NULL
};

CU_TestInfo  Template_MacroTest[] =
{
    { "Macro1",  Macro1_Test },
    { "Macro2",  Macro2_Test },
    CU_TEST_INFO_NULL
};

CU_TestInfo  Template_FuncTest[] =
{
    { "Func1", Func1_Test },
    { "Func2", Func2_Test },
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/