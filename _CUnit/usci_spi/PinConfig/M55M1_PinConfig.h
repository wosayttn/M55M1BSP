#ifndef _M55M1_PIN_CONFIG_H_
#define _M55M1_PIN_CONFIG_H_

#include <stdint.h>
//#include "USPI_CommonFunc.h"

//------------------------------------------------------------------------------
#define USPI0_PIN_OUT_GROUP                 (1)

//------------------------------------------------------------------------------
// SPI PIN init define
//------------------------------------------------------------------------------
#define SPI0_MOSI_PIN_INIT()                 SPI0_MOSI_PIN
#define SPI0_MISO_PIN_INIT()                 SPI0_MISO_PIN
#define SPI0_CLK_PIN_INIT()                  SPI0_CLK_PIN
#define SPI0_CSS_PIN_INIT()                  SPI0_CSS_PIN
#define SPI0_MFOS_PIN_INIT()                 SPI0_MFOS_PIN

#define SPI0_MOSI_PIN  SYS->GPD_MFP0 = (SYS->GPD_MFP0 & (~SYS_GPD_MFP0_PD0MFP_Msk)) | SPI0_MOSI_PD0
#define SPI0_MISO_PIN  SYS->GPD_MFP0 = (SYS->GPD_MFP0 & (~SYS_GPD_MFP0_PD1MFP_Msk)) | SPI0_MISO_PD1
#define SPI0_CLK_PIN   SYS->GPD_MFP0 = (SYS->GPD_MFP0 & (~SYS_GPD_MFP0_PD2MFP_Msk)) | SPI0_CLK_PD2
#define SPI0_CSS_PIN   SYS->GPD_MFP0 = (SYS->GPD_MFP0 & (~SYS_GPD_MFP0_PD3MFP_Msk)) | SPI0_SS_PD3
#define SPI0_MFOS_PIN  (SYS->GPD_MFOS |= 0x3)

//------------------------------------------------------------------------------
// CLKO PIN init define
//------------------------------------------------------------------------------
#define USPI_CLKO_PIN_INIT                                                                   \
    SYS->GPC_MFP3 = (SYS->GPC_MFP3 & ~SYS_GPC_MFP3_PC13MFP_Msk) | SYS_GPC_MFP3_PC13MFP_CLKO; \
    printf("Please Check CLKO PC13\n")

//------------------------------------------------------------------------------
// USPI0 PIN init define
//------------------------------------------------------------------------------
#define USPI0_MOSI_PIN_INIT()                   USPI0_MOSI_PIN
#define USPI0_MISO_PIN_INIT()                   USPI0_MISO_PIN
#define USPI0_CLK_PIN_INIT()                    USPI0_CLK_PIN
#define USPI0_CSS_PIN_INIT()                    USPI0_CSS_PIN
#define USPI0_MFOS_PIN_INIT()                   USPI0_MFOS_PIN
#define USPI0_CLK_TO_GPIO_PIN_INIT(pin)         USPI0_CLK_TO_GPIO_PIN(pin)

#if (USPI0_PIN_OUT_GROUP == 0)
#define USPI0_CLK_PIN  SYS->GPD_MFP0 = (SYS->GPD_MFP0 & (~SYS_GPD_MFP0_PD0MFP_Msk))  | SYS_GPD_MFP0_PD0MFP_USCI0_CLK
#define USPI0_MOSI_PIN SYS->GPD_MFP0 = (SYS->GPD_MFP0 & (~SYS_GPD_MFP0_PD1MFP_Msk)) | SYS_GPD_MFP0_PD1MFP_USCI0_DAT0
#define USPI0_MISO_PIN SYS->GPD_MFP0 = (SYS->GPD_MFP0 & (~SYS_GPD_MFP0_PD2MFP_Msk)) | SYS_GPD_MFP0_PD2MFP_USCI0_DAT1
#define USPI0_CSS_PIN  SYS->GPD_MFP1 = (SYS->GPD_MFP1 & (~SYS_GPD_MFP1_PD4MFP_Msk)) | SYS_GPD_MFP1_PD4MFP_USCI0_CTL0
#define USPI0_MFOS_PIN SYS->GPD_MFOS |= BIT1 | BIT2
#define USPI0_CLK_TO_GPIO_PIN (uint32_t *)((GPIO_PIN_DATA_BASE + (0x40 * 3)) + (2 << 2))

#elif (USPI0_PIN_OUT_GROUP == 1)
#define USPI0_CLK_PIN  SYS->GPA_MFP2 = (SYS->GPA_MFP2 & (~SYS_GPA_MFP2_PA11MFP_Msk))  | SYS_GPA_MFP2_PA11MFP_USCI0_CLK
#define USPI0_MOSI_PIN SYS->GPA_MFP2 = (SYS->GPA_MFP2 & (~SYS_GPA_MFP2_PA10MFP_Msk)) | SYS_GPA_MFP2_PA10MFP_USCI0_DAT0
#define USPI0_MISO_PIN SYS->GPA_MFP2 = (SYS->GPA_MFP2 & (~SYS_GPA_MFP2_PA9MFP_Msk)) | SYS_GPA_MFP2_PA9MFP_USCI0_DAT1
#define USPI0_CSS_PIN  SYS->GPB_MFP0 = (SYS->GPB_MFP0 & (~SYS_GPB_MFP0_PB0MFP_Msk)) | SYS_GPB_MFP0_PB0MFP_USCI0_CTL0
#define USPI0_MFOS_PIN SYS->GPA_MFOS |= BIT10 | BIT9
//#define USPI0_CLK_TO_GPIO_PIN (uint32_t *)((GPIO_PIN_DATA_BASE + (0x40 * 0)) + (9 << 2))
#define USPI0_CLK_TO_GPIO_PIN(pin) \
    do{\
        pin = (volatile uint32_t *)(GPIO_PIN_DATA_BASE + (0x40 * 0) + (11 << 2));\
    }while(0)
//PA->MODE = (PA->MODE & ~GPIO_MODE_MODE11_Msk) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE11_Pos);\

#elif (USPI0_PIN_OUT_GROUP == 2)
#define USPI0_CLK_PIN  SYS->GPE_MFP0 = (SYS->GPE_MFP0 & (~SYS_GPE_MFP0_PE2MFP_Msk))  | SYS_GPE_MFP0_PE2MFP_USCI0_CLK
#define USPI0_MOSI_PIN SYS->GPE_MFP0 = (SYS->GPE_MFP0 & (~SYS_GPE_MFP0_PE3MFP_Msk)) | SYS_GPE_MFP0_PE3MFP_USCI0_DAT0
#define USPI0_MISO_PIN SYS->GPE_MFP1 = (SYS->GPE_MFP1 & (~SYS_GPE_MFP1_PE4MFP_Msk)) | SYS_GPE_MFP1_PE4MFP_USCI0_DAT1
#define USPI0_CSS_PIN  SYS->GPE_MFP1 = (SYS->GPE_MFP1 & (~SYS_GPE_MFP1_PE6MFP_Msk)) | SYS_GPE_MFP1_PE6MFP_USCI0_CTL0
#define USPI0_MFOS_PIN SYS->GPE_MFOS |= BIT3 | BIT4
#define USPI0_CLK_TO_GPIO_PIN (uint32_t *)((GPIO_PIN_DATA_BASE + (0x40 * 5)) + (4 << 2))

#elif (USPI0_PIN_OUT_GROUP == 3)
#define USPI0_CLK_PIN  SYS->GPB_MFP3 = (SYS->GPB_MFP3 & (~SYS_GPB_MFP3_PB12MFP_Msk))  | SYS_GPB_MFP3_PB12MFP_USCI0_CLK
#define USPI0_MOSI_PIN SYS->GPB_MFP3 = (SYS->GPB_MFP3 & (~SYS_GPB_MFP3_PB13MFP_Msk)) | SYS_GPB_MFP3_PB13MFP_USCI0_DAT0
#define USPI0_MISO_PIN SYS->GPB_MFP3 = (SYS->GPB_MFP3 & (~SYS_GPB_MFP3_PB14MFP_Msk)) | SYS_GPB_MFP3_PB14MFP_USCI0_DAT1
#define USPI0_CSS_PIN  SYS->GPC_MFP3 = (SYS->GPC_MFP3 & (~SYS_GPC_MFP3_PC14MFP_Msk)) | SYS_GPC_MFP3_PC14MFP_USCI0_CTL0
#define USPI0_MFOS_PIN SYS->GPE_MFOS |= BIT13 | BIT14
#define USPI0_CLK_TO_GPIO_PIN (uint32_t *)((GPIO_PIN_DATA_BASE + (0x40 * 2)) + (14 << 2))

#endif //USPI0_PIN_OUT_GROUP

#endif /* _PIN_CONFIG_H_ */
