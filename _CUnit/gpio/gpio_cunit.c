#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "gpio_cunit.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

#if 0 // test all port @ Palladium
    #define GPIO_PORT_MAX           10UL /*!< Specify Maximum Ports for TC8263 */
#else
    #define GPIO_PORT_MAX           1    /*!< Specify Maximum Ports for TC8263 */
#endif

#ifndef GPIO_PIN_DATA
    #define GPIO_PIN_DATA GPIO_PIN_DATA_S
#endif


#define GPIO_SET_DEBOUNCE_ICLKON(port)    ((port)->DBCTL |= GPIO_DBCTL_ICLK_ON)
#define GPIO_CLR_DEBOUNCE_ICLKON(port)    ((port)->DBCTL &= ~GPIO_DBCTL_ICLK_ON)

extern volatile uint32_t  *au32GpioPortPin[GPIO_PORT_MAX * GPIO_PIN_MAX];

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
    {"GPIO API", suite_success_init, suite_success_clean, NULL, NULL, GPIO_ApiTests},
    {"GPIO MACRO", suite_success_init, suite_success_clean, NULL, NULL, GPIO_MacroTests},


    CU_SUITE_INFO_NULL
};

GPIO_T *GpioPort[GPIO_PORT_MAX] =
{
    (GPIO_T *)GPIOA_BASE, (GPIO_T *)GPIOB_BASE, (GPIO_T *)GPIOC_BASE,
    (GPIO_T *)GPIOD_BASE, (GPIO_T *)GPIOE_BASE, (GPIO_T *)GPIOF_BASE,
    (GPIO_T *)GPIOG_BASE, (GPIO_T *)GPIOH_BASE, (GPIO_T *)GPIOI_BASE,
    (GPIO_T *)GPIOJ_BASE,
};


/*
    TC8263 Pin No n for each Port.
    Max. n=15 for port A/B/E/G/H.
    Max. n=14 for port C/D.
    Max. n=13 for port J.
    Max. n=11 for port F.
    n=6~15 for port I.
*/
/*
    remove AF NC pin: PD1~4, PB13~15 from GPIO
    PG.7/8 pin is replaced by DCDC
    remove PC14
*/
#if (GPIO_PORT_MAX == 1)
volatile uint32_t  *au32GpioPortPin[GPIO_PORT_MAX * GPIO_PIN_MAX] =
{
    &PA0, &PA1, &PA2, &PA3, &PA4, &PA5, &PA6, &PA7, &PA8, &PA9, &PA10, &PA11, &PA12, &PA13, &PA14, &PA15,
};

#else
volatile uint32_t  *au32GpioPortPin[GPIO_PORT_MAX * GPIO_PIN_MAX] =
{
    &PA0, &PA1, &PA2, &PA3, &PA4, &PA5, &PA6, &PA7, &PA8, &PA9, &PA10, &PA11, &PA12, &PA13, &PA14, &PA15,
    &PB0, &PB1, &PB2, &PB3, &PB4, &PB5, &PB6, &PB7, &PB8, &PB9, &PB10, &PB11, &PB12,     0,     0,     0,
    &PC0, &PC1, &PC2, &PC3, &PC4, &PC5, &PC6, &PC7, &PC8, &PC9, &PC10, &PC11, &PC12, &PC13,     0,     0,
    &PD0,    0,    0,    0,    0, &PD5, &PD6, &PD7, &PD8, &PD9, &PD10, &PD11, &PD12, &PD13, &PD14,     0,
    &PE0, &PE1, &PE2, &PE3, &PE4, &PE5, &PE6, &PE7, &PE8, &PE9, &PE10, &PE11, &PE12, &PE13, &PE14, &PE15,
    &PF0, &PF1, &PF2, &PF3, &PF4, &PF5, &PF6, &PF7, &PF8, &PF9, &PF10, &PF11,     0,     0,     0,     0,
    &PG0, &PG1, &PG2, &PG3, &PG4, &PG5, &PG6,    0,    0, &PG9, &PG10, &PG11, &PG12, &PG13, &PG14, &PG15,
    &PH0, &PH1, &PH2, &PH3, &PH4, &PH5, &PH6, &PH7, &PH8, &PH9, &PH10, &PH11, &PH12, &PH13, &PH14, &PH15,
    0,    0,    0,    0,    0,    0, &PI6, &PI7, &PI8, &PI9, &PI10, &PI11, &PI12, &PI13, &PI14, &PI15,
    &PJ0, &PJ1, &PJ2, &PJ3, &PJ4, &PJ5, &PJ6, &PJ7, &PJ8, &PJ9, &PJ10, &PJ11, &PJ12, &PJ13,     0,     0,
};
#endif

uint32_t CheckPortPinReserved(uint32_t u32GpioPort, uint32_t u32GpioPin)
{
    // PH4, PH5 are reserved for UART6
    if (u32GpioPort == 7)   // PORT H
    {
        if ((u32GpioPin == 4) || (u32GpioPin == 5))
        {
            return 1;
        }
    }
    else if (u32GpioPort == 5)     // PORT F
    {
        if ((u32GpioPin == 0) || (u32GpioPin == 1))
        {
            return 1;
        }
        else if ((u32GpioPin == 2) || (u32GpioPin == 3))
        {
            return 1;
        }
        else if ((u32GpioPin == 4) || (u32GpioPin == 5))
        {
            return 1;
        }
    }

    // SKIP pins which is not available
    if (au32GpioPortPin[u32GpioPort * GPIO_PIN_MAX + u32GpioPin] == 0)
    {
        return 1;
    }

    return 0;
}

const uint32_t au32GpioModeSel[] =
{
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_OPEN_DRAIN,
    GPIO_MODE_QUASI,
};

/*
    GPIO_SetMode
*/
void TestFunc_GPIO_SetMode()
{
    uint32_t u32TestPort;       //port
    uint8_t u8TestPin;          //pin
    uint8_t u8GpioModeSelIdx;   //I/O mode
    uint32_t mode;

    /* test loop */
    for (u32TestPort = 0; u32TestPort < GPIO_PORT_MAX; u32TestPort++)
    {
        for (u8TestPin = 0; u8TestPin < GPIO_PIN_MAX; u8TestPin++)
        {
            //skip reserved pin
            if (CheckPortPinReserved(u32TestPort, u8TestPin))
            {
                continue;
            }

            //            printf("GPIO_SetMode P%c.%d ---\n", 'A' + u32TestPort, u8TestPin);

            for (u8GpioModeSelIdx = 0; u8GpioModeSelIdx < 4; u8GpioModeSelIdx++)
            {
                GPIO_SetMode(GpioPort[u32TestPort], 1 << u8TestPin, au32GpioModeSel[u8GpioModeSelIdx]);
                CU_ASSERT(((GpioPort[u32TestPort]->MODE) & (3 << (u8TestPin << 1))) == (au32GpioModeSel[u8GpioModeSelIdx] << (u8TestPin << 1)));

                if (((GpioPort[u32TestPort]->MODE) & (3 << (u8TestPin << 1))) != (au32GpioModeSel[u8GpioModeSelIdx] << (u8TestPin << 1)))
                {
                    printf("[Error] port: %d, pin: %d, mode: 0x%08X\n", u32TestPort, u8TestPin, GpioPort[u32TestPort]->MODE);
                }
            }
        }
    }
}

const uint32_t au32GpioPullSel[] =
{
    GPIO_PUSEL_DISABLE,
    GPIO_PUSEL_PULL_UP,
    GPIO_PUSEL_PULL_DOWN,
};

/*
    GPIO_SetPullCtl
*/
void TestFunc_GPIO_SetPullCtl()
{
    uint32_t u32TestPort;       //port
    uint8_t u8TestPin;          //pin
    uint8_t u8GpioModeSelIdx;   //I/O mode

    /* test loop */
    for (u32TestPort = 0; u32TestPort < GPIO_PORT_MAX; u32TestPort++)
    {
        for (u8TestPin = 0; u8TestPin < GPIO_PIN_MAX; u8TestPin++)
        {
            //skip reserved pin
            if (CheckPortPinReserved(u32TestPort, u8TestPin))
            {
                continue;
            }

            for (u8GpioModeSelIdx = 0; u8GpioModeSelIdx < 3; u8GpioModeSelIdx++)
            {
                GPIO_SetPullCtl(GpioPort[u32TestPort], 1 << u8TestPin, au32GpioPullSel[u8GpioModeSelIdx]);
                CU_ASSERT(((GpioPort[u32TestPort]->PUSEL) & (3 << (u8TestPin << 1))) == (au32GpioPullSel[u8GpioModeSelIdx] << (u8TestPin << 1)));
            }
        }
    }
}

const uint32_t au32GpioSlewSel[] =
{
    GPIO_SLEWCTL_NORMAL,
    GPIO_SLEWCTL_HIGH,
};

/*
    GPIO_SetSlewCtl
*/
void TestFunc_GPIO_SetSlewCtl()
{
    uint32_t u32TestPort;       //port
    uint8_t u8TestPin;          //pin
    uint8_t u8GpioModeSelIdx;   //I/O mode

    /* test loop */
    for (u32TestPort = 0; u32TestPort < GPIO_PORT_MAX; u32TestPort++)
    {
        for (u8TestPin = 0; u8TestPin < GPIO_PIN_MAX; u8TestPin++)
        {
            //skip reserved pin
            if (CheckPortPinReserved(u32TestPort, u8TestPin))
            {
                continue;
            }

            for (u8GpioModeSelIdx = 0; u8GpioModeSelIdx < (sizeof(au32GpioSlewSel) / sizeof(uint32_t)); u8GpioModeSelIdx++)
            {
                GPIO_SetSlewCtl(GpioPort[u32TestPort], 1 << u8TestPin, au32GpioSlewSel[u8GpioModeSelIdx]);
                CU_ASSERT(((GpioPort[u32TestPort]->SLEWCTL) & (3 << (u8TestPin << 1))) == (au32GpioSlewSel[u8GpioModeSelIdx] << (u8TestPin << 1)));
            }
        }
    }
}

const uint32_t au32GpioINTSel[20] =   //5*4
{
    //interrupt mode, IMD setting, IEN rising setting, IEN falling setting
    GPIO_INT_RISING, GPIO_INTTYPE_EDGE, 1, 0,
    GPIO_INT_FALLING, GPIO_INTTYPE_EDGE, 0, 1,
    GPIO_INT_BOTH_EDGE, GPIO_INTTYPE_EDGE, 1, 1,
    GPIO_INT_HIGH, GPIO_INTTYPE_LEVEL, 1, 0,
    GPIO_INT_LOW, GPIO_INTTYPE_LEVEL, 0, 1,
};


/*
    GPIO_EnableInt / GPIO_DisableInt
    GPIO_EnableEINT / GPIO_DisableEINT
*/
void TestFunc_GPIO_EnableInt()
{
    // Internal Interrupt
    uint32_t u32TestPort;       //PORT A ~ PORT J
    uint8_t u8TestPin;          //pin
    uint8_t u8GpioINTSelIdx;    //interrupt mode
    // External Interrupt
    uint32_t u32EINTn;          //EINT0 ~ EINT7
    uint32_t u32IntAttribs;     //interrupt mode

    /* test loop */
    for (u32TestPort = 0; u32TestPort < GPIO_PORT_MAX; u32TestPort++)
    {
        for (u8TestPin = 0; u8TestPin < GPIO_PIN_MAX; u8TestPin++)
        {
            //skip reserved pin
            if (CheckPortPinReserved(u32TestPort, u8TestPin))
            {
                continue;
            }

            for (u8GpioINTSelIdx = 0; u8GpioINTSelIdx < 20; u8GpioINTSelIdx += 4)
            {
                //GPIO_EnableInt
                GPIO_EnableInt(GpioPort[u32TestPort], u8TestPin, au32GpioINTSel[u8GpioINTSelIdx]);
                CU_ASSERT(((GpioPort[u32TestPort]->INTTYPE) & (au32GpioINTSel[u8GpioINTSelIdx + 1] << u8TestPin))
                          == (au32GpioINTSel[u8GpioINTSelIdx + 1] << u8TestPin));
                CU_ASSERT(((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx + 2] << (u8TestPin + 16)))
                          == (au32GpioINTSel[u8GpioINTSelIdx + 2] << (u8TestPin + 16)));
                CU_ASSERT(((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx + 3] << u8TestPin))
                          == (au32GpioINTSel[u8GpioINTSelIdx + 3] << u8TestPin));
                //GPIO_DisableInt
                GPIO_DisableInt(GpioPort[u32TestPort], u8TestPin);
                CU_ASSERT(((GpioPort[u32TestPort]->INTTYPE) & (au32GpioINTSel[u8GpioINTSelIdx + 1] << u8TestPin)) == 0);
                CU_ASSERT(((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx + 2] << (u8TestPin + 16))) == 0);
                CU_ASSERT(((GpioPort[u32TestPort]->INTEN) & (au32GpioINTSel[u8GpioINTSelIdx + 3] << u8TestPin)) == 0);
            }
        }
    }

    /* test loop */
    for (u32EINTn = 0; u32EINTn < 8; u32EINTn++)
    {
        for (u32IntAttribs = GPIO_INT_EDETCTL_DISABLE; u32IntAttribs <= GPIO_INT_EDETCTL_BOTH_EDGE; u32IntAttribs ++)
        {
            //
            GPIO_EnableEINT(u32EINTn, u32IntAttribs);
            CU_ASSERT(inpw(GPIO_INT_BASE + 0x20 * u32EINTn + 4) == (u32IntAttribs & GPIO_INT_EDETCTL_EDETCTL_Msk));
            CU_ASSERT(inpw(GPIO_INT_BASE + 0x20 * u32EINTn + 8) == GPIO_INT_EDINTEN_EDIEN_Msk);
            //
            GPIO_DisableEINT(u32EINTn);
            CU_ASSERT(inpw(GPIO_INT_BASE + 0x20 * u32EINTn + 4) == GPIO_INT_EDETCTL_DISABLE);
            CU_ASSERT(inpw(GPIO_INT_BASE + 0x20 * u32EINTn + 8) == 0);
        }
    }
}

const uint32_t au32GpioDbClkSel[2] =
{
    GPIO_DBCTL_DBCLKSRC_HCLK,
    GPIO_DBCTL_DBCLKSRC_LIRC,
};
const uint32_t au32GpioDbCycleSel[16] =
{
    GPIO_DBCTL_DBCLKSEL_1,
    GPIO_DBCTL_DBCLKSEL_2,
    GPIO_DBCTL_DBCLKSEL_4,
    GPIO_DBCTL_DBCLKSEL_8,
    GPIO_DBCTL_DBCLKSEL_16,
    GPIO_DBCTL_DBCLKSEL_32,
    GPIO_DBCTL_DBCLKSEL_64,
    GPIO_DBCTL_DBCLKSEL_128,
    GPIO_DBCTL_DBCLKSEL_256,
    GPIO_DBCTL_DBCLKSEL_512,
    GPIO_DBCTL_DBCLKSEL_1024,
    GPIO_DBCTL_DBCLKSEL_2048,
    GPIO_DBCTL_DBCLKSEL_4096,
    GPIO_DBCTL_DBCLKSEL_8192,
    GPIO_DBCTL_DBCLKSEL_16384,
    GPIO_DBCTL_DBCLKSEL_32768,
};

void TestFunc_GPIO_TestMacro()
{
    uint32_t u32TestPort;           //port
    uint8_t u8TestPin;              //pin
    uint8_t u8GpioDbClkSelIdx;      //debounce clock souce
    uint8_t u8GpioDbCycleSelIdx;    //debounce sample cycle
    uint32_t u32PinData;

    /* test pin value */
    for (u32TestPort = 0; u32TestPort < GPIO_PORT_MAX; u32TestPort++)
    {
        for (u8TestPin = 0; u8TestPin < GPIO_PIN_MAX; u8TestPin++)
        {
            //skip reserved pin
            if (CheckPortPinReserved(u32TestPort, u8TestPin))
            {
                continue;
            }

            //set GPIO mode
            GPIO_SetMode(GpioPort[u32TestPort], 1 << u8TestPin, GPIO_MODE_OUTPUT);
            //output 0
            GPIO_PIN_DATA(u32TestPort, u8TestPin) = 0;
            u32PinData = inp32(GPIO_PIN_DATA_BASE + (u32TestPort * 0x40) + (u8TestPin << 2));
            CU_ASSERT(u32PinData == 0);

            if (u32PinData != 0)
            {
                printf("Error: port:%d, pin:%d \n", u32TestPort, u8TestPin);
            }

            //output 1
            GPIO_PIN_DATA(u32TestPort, u8TestPin) = 1;
            u32PinData = inpw(GPIO_PIN_DATA_BASE + (u32TestPort * 0x40) + (u8TestPin << 2));
            CU_ASSERT(u32PinData  == 1);

            if (u32PinData != 1)
            {
                printf("Error: port:%d, pin:%d \n", u32TestPort, u8TestPin);
            }
        }
    }

    /* test loop */
    for (u32TestPort = 0; u32TestPort < GPIO_PORT_MAX; u32TestPort++)
    {
        for (u8TestPin = 0; u8TestPin < GPIO_PIN_MAX; u8TestPin++)
        {
            //skip reserved pin
            if (CheckPortPinReserved(u32TestPort, u8TestPin))
            {
                continue;
            }

            //enable debounce
            GPIO_ENABLE_DEBOUNCE(GpioPort[u32TestPort], (1 << u8TestPin));
            CU_ASSERT(((GpioPort[u32TestPort]->DBEN) & (1 << u8TestPin)) == (1 << u8TestPin));
            //disable debounce
            GPIO_DISABLE_DEBOUNCE(GpioPort[u32TestPort], (1 << u8TestPin));
            CU_ASSERT(((GpioPort[u32TestPort]->DBEN) & (1 << u8TestPin)) == 0);
            //disable digital path
            GPIO_DISABLE_DIGITAL_PATH(GpioPort[u32TestPort], (1 << u8TestPin));
            CU_ASSERT(((GpioPort[u32TestPort]->DINOFF) & (1 << (u8TestPin + 16))) == (1 << (u8TestPin + 16)));
            //enable digital path
            GPIO_ENABLE_DIGITAL_PATH(GpioPort[u32TestPort], (1 << u8TestPin));
            CU_ASSERT(((GpioPort[u32TestPort]->DINOFF) & (1 << (u8TestPin + 16))) == 0);
            //enable data write mask
            GPIO_ENABLE_DOUT_MASK(GpioPort[u32TestPort], (1 << u8TestPin));
            CU_ASSERT(((GpioPort[u32TestPort]->DATMSK) & (1 << u8TestPin)) == (1 << u8TestPin));
            //disable data write mask
            GPIO_DISABLE_DOUT_MASK(GpioPort[u32TestPort], (1 << u8TestPin));
            CU_ASSERT(((GpioPort[u32TestPort]->DATMSK) & (1 << u8TestPin)) == 0);

            //set debounce
            for (u8GpioDbClkSelIdx = 0; u8GpioDbClkSelIdx < 2; u8GpioDbClkSelIdx++)
            {
                for (u8GpioDbCycleSelIdx = 0; u8GpioDbCycleSelIdx < 16; u8GpioDbCycleSelIdx++)
                {
                    GPIO_SET_DEBOUNCE_TIME(GpioPort[u32TestPort], au32GpioDbClkSel[u8GpioDbClkSelIdx], au32GpioDbCycleSel[u8GpioDbCycleSelIdx]);
                    CU_ASSERT((GpioPort[u32TestPort]->DBCTL & GPIO_DBCTL_DBCLKSRC_Msk) == au32GpioDbClkSel[u8GpioDbClkSelIdx]);
                    CU_ASSERT((GpioPort[u32TestPort]->DBCTL & GPIO_DBCTL_DBCLKSEL_Msk) == au32GpioDbCycleSel[u8GpioDbCycleSelIdx]);
                    //CU_ASSERT((GPIO->DBCTL & GPIO_DBCTL_ICLK_ON) == GPIO_DBCTL_ICLK_ON);
                }
            }
        }
    }

    for (u32TestPort = 0; u32TestPort < GPIO_PORT_MAX; u32TestPort++)
    {
        // GPIO_DBCTL_ICLK_OFF --> GPIO_DBCTL_ICLK_ON
        GpioPort[u32TestPort]->DBCTL = GPIO_DBCTL_ICLK_OFF;
        GPIO_SET_DEBOUNCE_ICLKON(GpioPort[u32TestPort]);
        CU_ASSERT(GpioPort[u32TestPort]->DBCTL & GPIO_DBCTL_ICLK_ON);
        CU_ASSERT(!(GpioPort[u32TestPort]->DBCTL & ~GPIO_DBCTL_ICLK_ON));
        // GPIO_DBCTL_ICLK_ON --> GPIO_DBCTL_ICLK_OFF
        GpioPort[u32TestPort]->DBCTL = GPIO_DBCTL_ICLK_ON;
        GPIO_CLR_DEBOUNCE_ICLKON(GpioPort[u32TestPort]);
        CU_ASSERT(GpioPort[u32TestPort]->DBCTL == GPIO_DBCTL_ICLK_OFF);
    }
}
void TestFunc_GPIO_TestINT()
{
    uint32_t u32TestPort;           //port
    uint8_t u8TestPin;              //pin
    uint32_t i;

    /* test loop */
    for (u32TestPort = 0; u32TestPort < GPIO_PORT_MAX; u32TestPort++)
    {
        GPIO_SET_DEBOUNCE_ICLKON(GpioPort[u32TestPort]);

        for (u8TestPin = 0; u8TestPin < GPIO_PIN_MAX; u8TestPin++)
        {
            //skip reserved pin
            if (CheckPortPinReserved(u32TestPort, u8TestPin))
            {
                continue;
            }

            //set interrupt enable
            GPIO_EnableInt(GpioPort[u32TestPort], u8TestPin, GPIO_INT_BOTH_EDGE);
            //set GPIO mode
            GPIO_SetMode(GpioPort[u32TestPort], 1 << u8TestPin, GPIO_MODE_OUTPUT);
            //output 1
            GPIO_SET_OUT_DATA(GpioPort[u32TestPort], 1 << u8TestPin);
            CU_ASSERT((GPIO_GET_IN_DATA(GpioPort[u32TestPort])) & (1 << u8TestPin));

            /*Debug*/
            if (!((GPIO_GET_IN_DATA(GpioPort[u32TestPort])) & (1 << u8TestPin)))
            {
                printf("port: %d, pin: %d\n", u32TestPort, u8TestPin);
            }

            //output 0
            GPIO_SET_OUT_DATA(GpioPort[u32TestPort], 0 << u8TestPin);
            CU_ASSERT(!((GPIO_GET_IN_DATA(GpioPort[u32TestPort])) & (1 << u8TestPin)));
            //Get interrupt flag
            CU_ASSERT(GPIO_GET_INT_FLAG(GpioPort[u32TestPort], 1 << u8TestPin) & (1 << u8TestPin));
            //Clear interrupt flag
            GPIO_CLR_INT_FLAG(GpioPort[u32TestPort], 1 << u8TestPin);
            CU_ASSERT(GPIO_GET_INT_FLAG(GpioPort[u32TestPort], 1 << u8TestPin) == 0);
            //Toggle GPIO
            GPIO_TOGGLE(*(au32GpioPortPin[(u32TestPort << 4) + u8TestPin ]));
            //Get interrupt flag
            CU_ASSERT(GPIO_GET_INT_FLAG(GpioPort[u32TestPort], 1 << u8TestPin) & (1 << u8TestPin));
            //printf("%x, %x, %x\n", u32TestPort, u8TestPin, GpioPort[u32TestPort]->INTSRC);
            //Clear interrupt flag
            GPIO_CLR_INT_FLAG(GpioPort[u32TestPort], 1 << u8TestPin);
            CU_ASSERT(GPIO_GET_INT_FLAG(GpioPort[u32TestPort], 1 << u8TestPin) == 0);
        }
    }
}
CU_TestInfo GPIO_ApiTests[] =
{
    {"Testing GPIO_SetMode Function:", TestFunc_GPIO_SetMode},
    {"Testing GPIO_SetPullCtl Function:", TestFunc_GPIO_SetPullCtl},
    {"Testing GPIO_SetSlewCtl Function:", TestFunc_GPIO_SetSlewCtl},
    {"Testing GPIO_EnableInt Function:", TestFunc_GPIO_EnableInt},

    CU_TEST_INFO_NULL
};

CU_TestInfo GPIO_MacroTests[] =
{
    {"Testing GPIO_TestMacro Function:", TestFunc_GPIO_TestMacro},
    {"Testing GPIO_TestINT Function:", TestFunc_GPIO_TestINT},

    CU_TEST_INFO_NULL
};