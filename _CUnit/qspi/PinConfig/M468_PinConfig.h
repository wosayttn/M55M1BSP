#ifndef _M468_PIN_CONFIG_H_
#define _M468_PIN_CONFIG_H_

#include "SPI_CommonFunc.h"

//------------------------------------------------------------------------------
// QSPI PIN Configure
//------------------------------------------------------------------------------
#define QSPI0_MOSI_PIN_INIT                   QSPI0_MOSI_PIN
#define QSPI0_MISO_PIN_INIT                   QSPI0_MISO_PIN
#define QSPI0_CLK_PIN_INIT                    QSPI0_CLK_PIN
#define QSPI0_CSS_PIN_INIT                    QSPI0_CSS_PIN
#define QSPI0_MOSI1_PIN_INIT                  QSPI0_MOSI1_PIN
#define QSPI0_MISO1_PIN_INIT                  QSPI0_MISO1_PIN

#define QSPI0_MOSI1_TO_GPIO_INIT              QSPI0_MOSI1_TO_GPIO_PIN
#define QSPI0_MISO1_TO_GPIO_INIT              QSPI0_MISO1_TO_GPIO_PIN

#define QSPI0_MFOS_PIN_INIT                   SPI0_MFOS_PIN

#if (QSPI0_PIN_OUT_GROUP == 0)
#define QSPI0_MOSI_PIN  SET_QSPI0_MOSI0_PA0()
#define QSPI0_MISO_PIN  SET_QSPI0_MISO0_PA1()
#define QSPI0_CLK_PIN   SET_QSPI0_CLK_PA2()
#define QSPI0_CSS_PIN   SET_QSPI0_SS_PA3()
#define QSPI0_MOSI1_PIN SET_QSPI0_MOSI1_PA4()
#define QSPI0_MISO1_PIN SET_QSPI0_MISO1_PA5()
#define QSPI0_MFOS_PIN  (SYS->GPA_MFOS |= 0x30)

#define QSPI0_MOSI1_TO_GPIO_PIN  \
    SYS->GPC_MFP1 &= ~(SYS_GPC_MFP1_PC4MFP_Msk) | SYS_GPC_MFP1_PC4MFP_GPIO; \
    PC->MODE = (PC->MODE & ~(0x3ul << (4 << 1))) | (1 << (4 << 1));\
    PC4 = 1

#define QSPI0_MISO1_TO_GPIO_PIN \
    SYS->GPC_MFP1 &= ~(SYS_GPC_MFP1_PC5MFP_Msk) | SYS_GPC_MFP1_PC5MFP_GPIO; \
    PC->MODE = (PC->MODE & ~(0x3ul << (5 << 1))) | (1 << (5 << 1));\
    PC5 = 1

#elif (QSPI0_PIN_OUT_GROUP == 1)

#define QSPI0_MOSI_PIN  SET_QSPI0_MOSI0_PC0()
#define QSPI0_MISO_PIN  SET_QSPI0_MISO0_PC1()
#define QSPI0_CLK_PIN   SET_QSPI0_CLK_PC2()
#define QSPI0_CSS_PIN   SET_QSPI0_SS_PC3()
#define QSPI0_MOSI1_PIN SYS->GPC_MFP1 = (SYS->GPC_MFP1 & (~SYS_GPC_MFP1_PC4MFP_Msk)) | QSPI0_MOSI1_PC4 //SET_QSPI0_MOSI1_PC4()
#define QSPI0_MISO1_PIN SYS->GPC_MFP1 = (SYS->GPC_MFP1 & (~SYS_GPC_MFP1_PC5MFP_Msk)) | QSPI0_MISO1_PC5//SET_QSPI0_MISO1_PC5()
#define QSPI0_MFOS_PIN  (SYS->GPC_MFOS |= 0x03)

#define QSPI0_MOSI1_TO_GPIO_PIN  \
    PC->MODE = (PC->MODE & ~(0x3ul << (4 << 1))) | (1 << (4 << 1));\
    SYS->GPC_MFP1 &= ~(SYS_GPC_MFP1_PC4MFP_Msk) | SYS_GPC_MFP1_PC4MFP_GPIO; \
    PC4 = 1

#define QSPI0_MISO1_TO_GPIO_PIN \
    PC->MODE = (PC->MODE & ~(0x3ul << (5 << 1))) | (1 << (5 << 1));\
    SYS->GPC_MFP1 &= ~(SYS_GPC_MFP1_PC5MFP_Msk) | SYS_GPC_MFP1_PC5MFP_GPIO; \
    PC5 = 1

#elif (QSPI0_PIN_OUT_GROUP == 2)
#define QSPI0_MOSI_PIN  SET_QSPI0_MOSI0_PE0()
#define QSPI0_MISO_PIN  SET_QSPI0_MISO0_PE1()
#define QSPI0_CLK_PIN   SET_QSPI0_CLK_PH8()
#define QSPI0_CSS_PIN   SET_QSPI0_SS_PH9()
#define QSPI0_MOSI1_PIN SYS->GPH_MFP3 = (SYS->GPH_MFP3 & (~SYS_GPH_MFP3_PH11MFP_Msk)) | QSPI0_MOSI1_PH11 //SET_QSPI0_MOSI1_PH4()
#define QSPI0_MISO1_PIN SYS->GPH_MFP3 = (SYS->GPH_MFP3 & (~SYS_GPH_MFP3_PH10MFP_Msk)) | QSPI0_MISO1_PH10//SET_QSPI0_MISO1_PH5()
#define QSPI0_MFOS_PIN  (SYS->GPC_MFOS |= 0x03)

#define QSPI0_MOSI1_TO_GPIO_PIN  \
    PC->MODE = (PC->MODE & ~(0x3ul << (4 << 1))) | (1 << (4 << 1));\
    SYS->GPC_MFP1 &= ~(SYS_GPC_MFP1_PC4MFP_Msk) | SYS_GPC_MFP1_PC4MFP_GPIO; \
    PC4 = 1

#define QSPI0_MISO1_TO_GPIO_PIN \
    PC->MODE = (PC->MODE & ~(0x3ul << (5 << 1))) | (1 << (5 << 1));\
    SYS->GPC_MFP1 &= ~(SYS_GPC_MFP1_PC5MFP_Msk) | SYS_GPC_MFP1_PC5MFP_GPIO; \
    PC5 = 1

#elif (QSPI0_PIN_OUT_GROUP == 3)
#define QSPI0_MOSI_PIN  SYS->GPJ_MFP0 = (SYS->GPJ_MFP0 & (~SYS_GPJ_MFP0_PJ1MFP_Msk)) | SYS_GPJ_MFP0_PJ1MFP_QSPI0_MOSI0
#define QSPI0_MISO_PIN  SYS->GPJ_MFP0 = (SYS->GPJ_MFP0 & (~SYS_GPJ_MFP0_PJ0MFP_Msk)) | SYS_GPJ_MFP0_PJ0MFP_QSPI0_MISO0
#define QSPI0_CLK_PIN   SYS->GPI_MFP3 = (SYS->GPI_MFP3 & (~SYS_GPI_MFP3_PI15MFP_Msk)) | SYS_GPI_MFP3_PI15MFP_QSPI0_CLK
#define QSPI0_CSS_PIN   SYS->GPI_MFP3 = (SYS->GPI_MFP3 & (~SYS_GPI_MFP3_PI14MFP_Msk)) | SYS_GPI_MFP3_PI14MFP_QSPI0_SS
#define QSPI0_MOSI1_PIN SYS->GPI_MFP3 = (SYS->GPI_MFP3 & (~SYS_GPI_MFP3_PI13MFP_Msk)) | SYS_GPI_MFP3_PI13MFP_QSPI0_MOSI1
#define QSPI0_MISO1_PIN SYS->GPI_MFP3 = (SYS->GPI_MFP3 & (~SYS_GPI_MFP3_PI12MFP_Msk)) | SYS_GPI_MFP3_PI12MFP_QSPI0_MISO1
#define QSPI0_MFOS_PIN  (SYS->GPC_MFOS |= 0x03)

#define QSPI0_MOSI1_TO_GPIO_PIN  \
    PC->MODE = (PC->MODE & ~(0x3ul << (4 << 1))) | (1 << (4 << 1));\
    SYS->GPC_MFP1 &= ~(SYS_GPC_MFP1_PC4MFP_Msk) | SYS_GPC_MFP1_PC4MFP_GPIO; \
    PC4 = 1

#define QSPI0_MISO1_TO_GPIO_PIN \
    PC->MODE = (PC->MODE & ~(0x3ul << (5 << 1))) | (1 << (5 << 1));\
    SYS->GPC_MFP1 &= ~(SYS_GPC_MFP1_PC5MFP_Msk) | SYS_GPC_MFP1_PC5MFP_GPIO; \
    PC5 = 1

#endif //QSPI0_PIN_OUT_GROUP

//------------------------------------------------------------------------------
// SPI0 PIN Configure
//------------------------------------------------------------------------------
#define SPI0_MOSI_PIN_INIT                   SPI0_MOSI_PIN
#define SPI0_MISO_PIN_INIT                   SPI0_MISO_PIN
#define SPI0_CLK_PIN_INIT                    SPI0_CLK_PIN
#define SPI0_CSS_PIN_INIT                    SPI0_CSS_PIN
#define SPI0_MCLK_PIN_INIT                   SPI0_MCLK_PIN
#define SPI0_MFOS_PIN_INIT                   SPI0_MFOS_PIN

#if (SPI0_PIN_OUT_GROUP == 0)
    #define SPI0_MOSI_PIN  SET_SPI0_MOSI_PD0()
    #define SPI0_MISO_PIN  SET_SPI0_MISO_PD1()
    #define SPI0_CLK_PIN   SET_SPI0_CLK_PD2()
    #define SPI0_CSS_PIN   SET_SPI0_SS_PD3()
    #define SPI0_MCLK_PIN  SET_SPI0_I2SMCLK_PD13()
    #define SPI0_MFOS_PIN  (SYS->GPD_MFOS |= 0x3)

#elif (SPI0_PIN_OUT_GROUP == 1)
    #define SPI0_MOSI_PIN  SPI0_MOSI_PF6()
    #define SPI0_MISO_PIN  SPI0_MISO_PF7()
    #define SPI0_CLK_PIN   SPI0_CLK_PF8()
    #define SPI0_CSS_PIN   SPI0_SS_PF9()
    #define SPI0_MCLK_PIN  SPI0_I2SMCLK_PF10()
    #define SPI0_MFOS_PIN  (SYS->GPF_MFOS |= 0xC0)

#endif //SPI0_PIN_OUT_GROUP


//------------------------------------------------------------------------------
// SPI1 PIN define
//------------------------------------------------------------------------------
#define SPI1_MOSI_PIN_INIT                     SPI1_MOSI_PIN
#define SPI1_MISO_PIN_INIT                     SPI1_MISO_PIN
#define SPI1_CLK_PIN_INIT                      SPI1_CLK_PIN
#define SPI1_CSS_PIN_INIT                      SPI1_CSS_PIN
#define SPI1_MCLK_PIN_INIT                     SPI1_MCLK_PIN
#define SPI1_MFOS_PIN_INIT                     SPI1_MFOS_PIN

#if(SPI1_PIN_OUT_GROUP == 0)
    #define SPI1_MOSI_PIN //SPI1_MOSI_PI8()
    #define SPI1_MISO_PIN //SPI1_MISO_PI7()
    #define SPI1_CLK_PIN  //SPI1_CLK_PI9()
    #define SPI1_CSS_PIN  //SPI1_SS_PI10()
    #define SPI1_MCLK_PIN //SPI1_I2SMCLK_PI6()
    #define SPI1_MFOS_PIN //(SYS->GPI_MFOS |= 0x30)

#endif //SPI1_PIN_OUT_GROUP

//#endif //CHIP_VER

#endif /* _M468_PIN_CONFIG_H_ */
