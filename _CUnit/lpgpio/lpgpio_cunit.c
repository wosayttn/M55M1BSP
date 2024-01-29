#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "lpgpio_cunit.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int suite_success_init(void)
{
    return 0;
}

int suite_success_clean(void)
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
    {"LPGPIO API", suite_success_init, suite_success_clean, NULL, NULL, LPGPIO_ApiTests},
    {"LPGPIO MACRO", suite_success_init, suite_success_clean, NULL, NULL, LPGPIO_MacroTests},
    CU_SUITE_INFO_NULL
};

/*
    LPGPIO_SetMode
*/
void TestFunc_LPGPIO_SetMode()
{
    uint8_t u8TestPin;          //pin

    for (u8TestPin = 0; u8TestPin < LPGPIO_PIN_MAX; u8TestPin++)
    {
        LPGPIO_SetMode(LPGPIO, BIT0 << u8TestPin, LPGPIO_MODE_OUTPUT);
        CU_ASSERT((LPGPIO->MODE & (BIT0 << u8TestPin)) == (BIT0 << u8TestPin));
        LPGPIO_SetMode(LPGPIO, BIT0 << u8TestPin, LPGPIO_MODE_INPUT);
        CU_ASSERT((LPGPIO->MODE & (BIT0 << u8TestPin)) == 0);
    }
}

/*
    LPGPIO_SetMode
*/

void TestFunc_LPGPIO_TestMacro()
{
    uint8_t u8TestPin;              //pin
    uint32_t u32PinData;

    /* test pin value */
    for (u8TestPin = 0; u8TestPin < LPGPIO_PIN_MAX; u8TestPin++)
    {
        //set LPGPIO mode
        LPGPIO_SetMode(LPGPIO, 1 << u8TestPin, LPGPIO_MODE_OUTPUT);
        CU_ASSERT((LPGPIO->MODE & (1 << u8TestPin)) == (1 << u8TestPin));
        //output 0
        LPGPIO_SET_OUT_DATA(LPGPIO, (0 << u8TestPin));
        u32PinData = LPGPIO_GET_IN_DATA(LPGPIO);
        CU_ASSERT((u32PinData & (1 << u8TestPin)) == 0);
        //output 1
        LPGPIO_SET_OUT_DATA(LPGPIO, (1 << u8TestPin));
        u32PinData = LPGPIO_GET_IN_DATA(LPGPIO);
        CU_ASSERT((u32PinData & (1 << u8TestPin)) == (1 << u8TestPin));

        if ((u32PinData & (1 << u8TestPin)) == 0)
        {
            printf("Error: LPIO%d \n", u8TestPin);
        }

        LPGPIO_SetMode(LPGPIO, 1 << u8TestPin, LPGPIO_MODE_INPUT);
        CU_ASSERT((LPGPIO->MODE & (1 << u8TestPin)) == 0);
    }
}

CU_TestInfo LPGPIO_ApiTests[] =
{
    {"Testing GPIO_SetMode Function:", TestFunc_LPGPIO_SetMode},
    CU_TEST_INFO_NULL
};

CU_TestInfo LPGPIO_MacroTests[] =
{
    {"Testing GPIO_TestMacro Function:", TestFunc_LPGPIO_TestMacro},
    CU_TEST_INFO_NULL
};
