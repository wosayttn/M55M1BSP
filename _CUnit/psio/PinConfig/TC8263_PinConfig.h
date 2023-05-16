#ifndef _TC8263_PIN_CONFIG_H_
#define _TC8263_PIN_CONFIG_H_

//------------------------------------------------------------------------------
//#include "psio_emulation.h"

#define PSIO_PIN_GROUP                  (7)

//------------------------------------------------------------------------------
#define PSIO0_CH0_PIN_INIT()            PSIO0_CH0_PIN
#define PSIO0_CH1_PIN_INIT()            PSIO0_CH1_PIN
#define PSIO0_CH2_PIN_INIT()            PSIO0_CH2_PIN
#define PSIO0_CH3_PIN_INIT()            PSIO0_CH3_PIN
#define PSIO0_CH4_PIN_INIT()            PSIO0_CH4_PIN
#define PSIO0_CH5_PIN_INIT()            PSIO0_CH5_PIN
#define PSIO0_CH6_PIN_INIT()            PSIO0_CH6_PIN
#define PSIO0_CH7_PIN_INIT()            PSIO0_CH7_PIN

#define PSIO0_TO_GPIO_INIT()            PSIO0_TO_GPIO_PIN

#define SET_CH1_GPIO_PIN()             CH1_GPIO_PIN
#define SET_CH3_GPIO_PIN()             CH3_GPIO_PIN
#define SET_CH5_GPIO_PIN()             CH5_GPIO_PIN
#define SET_CH7_GPIO_PIN()             CH7_GPIO_PIN

#if (PSIO_PIN_GROUP == 1)
#define PSIO0_CH0_PIN   SYS->GPB_MFP3 = (SYS->GPB_MFP3 & (~SYS_GPB_MFP3_PB15MFP_Msk)) | SYS_GPB_MFP3_PB15MFP_PSIO0_CH0
#define PSIO0_CH1_PIN   SYS->GPB_MFP3 = (SYS->GPB_MFP3 & (~SYS_GPB_MFP3_PB14MFP_Msk)) | SYS_GPB_MFP3_PB14MFP_PSIO0_CH1
#define PSIO0_CH2_PIN   SYS->GPB_MFP3 = (SYS->GPB_MFP3 & (~SYS_GPB_MFP3_PB13MFP_Msk)) | SYS_GPB_MFP3_PB13MFP_PSIO0_CH2
#define PSIO0_CH3_PIN   SYS->GPB_MFP3 = (SYS->GPB_MFP3 & (~SYS_GPB_MFP3_PB12MFP_Msk)) | SYS_GPB_MFP3_PB12MFP_PSIO0_CH3
#define PSIO0_CH4_PIN   SYS->GPB_MFP1 = (SYS->GPB_MFP1 & (~SYS_GPB_MFP1_PB5MFP_Msk)) | SYS_GPB_MFP1_PB5MFP_PSIO0_CH4
#define PSIO0_CH5_PIN   SYS->GPB_MFP1 = (SYS->GPB_MFP1 & (~SYS_GPB_MFP1_PB4MFP_Msk)) | SYS_GPB_MFP1_PB4MFP_PSIO0_CH5
#define PSIO0_CH6_PIN   SYS->GPB_MFP0 = (SYS->GPB_MFP0 & (~SYS_GPB_MFP0_PB3MFP_Msk)) | SYS_GPB_MFP0_PB3MFP_PSIO0_CH6
#define PSIO0_CH7_PIN   SYS->GPB_MFP0 = (SYS->GPB_MFP0 & (~SYS_GPB_MFP0_PB2MFP_Msk)) | SYS_GPB_MFP0_PB2MFP_PSIO0_CH7
#define PSIO0_TO_GPIO_PIN                                    \
    do                                                       \
    {                                                        \
        SYS->GPB_MFP0 &= ~(0xF << SYS_GPB_MFP0_PB2MFP_Pos);  \
        SYS->GPB_MFP1 &= ~(0xF << SYS_GPB_MFP1_PB4MFP_Pos);  \
        SYS->GPB_MFP3 &= ~(0xF << SYS_GPB_MFP3_PB12MFP_Pos); \
        SYS->GPB_MFP3 &= ~(0xF << SYS_GPB_MFP3_PB14MFP_Pos); \
    } while (0)

#elif (PSIO_PIN_GROUP == 2)

#define PSIO0_CH0_PIN   SYS->GPE_MFP3 = (SYS->GPE_MFP3 & ~SYS_GPE_MFP3_PE14MFP_Msk) | SYS_GPE_MFP3_PE14MFP_PSIO0_CH0
#define PSIO0_CH1_PIN   SYS->GPE_MFP3 = (SYS->GPE_MFP3 & ~SYS_GPE_MFP3_PE15MFP_Msk) | SYS_GPE_MFP3_PE15MFP_PSIO0_CH1
#define PSIO0_CH2_PIN   SYS->GPD_MFP2 = (SYS->GPD_MFP2 & ~SYS_GPD_MFP2_PD9MFP_Msk) | SYS_GPD_MFP2_PD9MFP_PSIO0_CH2
#define PSIO0_CH3_PIN   SYS->GPD_MFP2 = (SYS->GPD_MFP2 & ~SYS_GPD_MFP2_PD8MFP_Msk) | SYS_GPD_MFP2_PD8MFP_PSIO0_CH3
#define PSIO0_CH4_PIN   SYS->GPD_MFP1 = (SYS->GPD_MFP1 & ~SYS_GPD_MFP1_PD7MFP_Msk) | SYS_GPD_MFP1_PD7MFP_PSIO0_CH4
#define PSIO0_CH5_PIN   SYS->GPD_MFP1 = (SYS->GPD_MFP1 & ~SYS_GPD_MFP1_PD6MFP_Msk) | SYS_GPD_MFP1_PD6MFP_PSIO0_CH5
#define PSIO0_CH6_PIN   SYS->GPD_MFP1 = (SYS->GPD_MFP1 & ~SYS_GPD_MFP1_PD5MFP_Msk) | SYS_GPD_MFP1_PD5MFP_PSIO0_CH6
#define PSIO0_CH7_PIN   SYS->GPD_MFP1 = (SYS->GPD_MFP1 & ~SYS_GPD_MFP1_PD4MFP_Msk) | SYS_GPD_MFP1_PD4MFP_PSIO0_CH7
#define PSIO0_TO_GPIO_PIN                                    \
    do                                                       \
    {                                                        \
        SYS->GPD_MFP1 &= ~(0xF << SYS_GPD_MFP1_PD4MFP_Pos);  \
        SYS->GPD_MFP1 &= ~(0xF << SYS_GPD_MFP1_PD6MFP_Pos);  \
        SYS->GPD_MFP2 &= ~(0xF << SYS_GPD_MFP2_PD8MFP_Pos);  \
        SYS->GPE_MFP3 &= ~(0xF << SYS_GPE_MFP3_PE15MFP_Pos); \
        GPIO_SetMode(PE, BIT15, GPIO_MODE_INPUT);            \
        GPIO_SetMode(PD, BIT8, GPIO_MODE_INPUT);             \
        GPIO_SetMode(PD, BIT6, GPIO_MODE_INPUT);             \
        GPIO_SetMode(PD, BIT4, GPIO_MODE_INPUT);             \
    } while (0)

//SET_GPIO_PE15();                          \
//SET_GPIO_PD8();                           \
//SET_GPIO_PD6();                           \
//SET_GPIO_PD4();                           \

#define CH1_GPIO_PIN        PE15
#define CH3_GPIO_PIN        PD8
#define CH5_GPIO_PIN        PD6
#define CH7_GPIO_PIN        PD4

#elif (PSIO_PIN_GROUP == 3)
#define PSIO0_CH0_PIN   SYS->GPC_MFP1 = (SYS->GPC_MFP1 & ~SYS_GPC_MFP1_PC5MFP_Msk) | SYS_GPC_MFP1_PC5MFP_PSIO0_CH0
#define PSIO0_CH1_PIN   SYS->GPC_MFP1 = (SYS->GPC_MFP1 & ~SYS_GPC_MFP1_PC4MFP_Msk) | SYS_GPC_MFP1_PC4MFP_PSIO0_CH1
#define PSIO0_CH2_PIN   SYS->GPC_MFP0 = (SYS->GPC_MFP0 & ~SYS_GPC_MFP0_PC3MFP_Msk) | SYS_GPC_MFP0_PC3MFP_PSIO0_CH2
#define PSIO0_CH3_PIN   SYS->GPC_MFP0 = (SYS->GPC_MFP0 & ~SYS_GPC_MFP0_PC2MFP_Msk) | SYS_GPC_MFP0_PC2MFP_PSIO0_CH3
#define PSIO0_CH4_PIN   SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~SYS_GPA_MFP0_PA3MFP_Msk) | SYS_GPA_MFP0_PA3MFP_PSIO0_CH4
#define PSIO0_CH5_PIN   SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~SYS_GPA_MFP0_PA2MFP_Msk) | SYS_GPA_MFP0_PA2MFP_PSIO0_CH5
#define PSIO0_CH6_PIN   SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~SYS_GPA_MFP0_PA1MFP_Msk) | SYS_GPA_MFP0_PA1MFP_PSIO0_CH6
#define PSIO0_CH7_PIN   SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~SYS_GPA_MFP0_PA0MFP_Msk) | SYS_GPA_MFP0_PA0MFP_PSIO0_CH7
#define PSIO0_TO_GPIO_PIN                                    \
    do                                                       \
    {                                                        \
        SYS->GPA_MFP0 &= ~(0xF << SYS_GPA_MFP0_PA0MFP_Pos) | \
                         ~(0xF << SYS_GPA_MFP0_PA2MFP_Pos);  \
        SYS->GPC_MFP0 &= ~(0xF << SYS_GPC_MFP0_PC2MFP_Pos);  \
        SYS->GPC_MFP1 &= ~(0xF << SYS_GPC_MFP1_PC4MFP_Pos);  \
    } while (0)

#elif (PSIO_PIN_GROUP == 4)
#define PSIO0_CH0_PIN   SYS->GPE_MFP1 = (SYS->GPE_MFP1 & ~SYS_GPE_MFP1_PE7MFP_Msk) | SYS_GPE_MFP1_PE7MFP_PSIO0_CH0
#define PSIO0_CH1_PIN   SYS->GPE_MFP1 = (SYS->GPE_MFP1 & ~SYS_GPE_MFP1_PE6MFP_Msk) | SYS_GPE_MFP1_PE6MFP_PSIO0_CH1
#define PSIO0_CH2_PIN   SYS->GPE_MFP1 = (SYS->GPE_MFP1 & ~SYS_GPE_MFP1_PE5MFP_Msk) | SYS_GPE_MFP1_PE5MFP_PSIO0_CH2
#define PSIO0_CH3_PIN   SYS->GPE_MFP1 = (SYS->GPE_MFP1 & ~SYS_GPE_MFP1_PE4MFP_Msk) | SYS_GPE_MFP1_PE4MFP_PSIO0_CH3
#define PSIO0_CH4_PIN   SYS->GPA_MFP3 = (SYS->GPA_MFP3 & ~SYS_GPA_MFP3_PA12MFP_Msk) | SYS_GPA_MFP3_PA12MFP_PSIO0_CH4
#define PSIO0_CH5_PIN   SYS->GPA_MFP3 = (SYS->GPA_MFP3 & ~SYS_GPA_MFP3_PA13MFP_Msk) | SYS_GPA_MFP3_PA13MFP_PSIO0_CH5
#define PSIO0_CH6_PIN   SYS->GPA_MFP3 = (SYS->GPA_MFP3 & ~SYS_GPA_MFP3_PA14MFP_Msk) | SYS_GPA_MFP3_PA14MFP_PSIO0_CH6
#define PSIO0_CH7_PIN   SYS->GPA_MFP3 = (SYS->GPA_MFP3 & ~SYS_GPA_MFP3_PA15MFP_Msk) | SYS_GPA_MFP3_PA15MFP_PSIO0_CH7
#define PSIO0_TO_GPIO_PIN                                    \
    do                                                       \
    {                                                        \
        SYS->GPA_MFP3 &= ~(0xF << SYS_GPA_MFP3_PA15MFP_Pos); \
        SYS->GPA_MFP3 &= ~(0xF << SYS_GPA_MFP3_PA13MFP_Pos); \
        SYS->GPE_MFP1 &= ~(0xF << SYS_GPE_MFP1_PE4MFP_Pos);  \
        SYS->GPE_MFP1 &= ~(0xF << SYS_GPE_MFP1_PE6MFP_Pos);  \
    } while (0)

#elif (PSIO_PIN_GROUP == 5)
#define PSIO0_CH0_PIN   SYS->GPC_MFPL = (SYS->GPC_MFPL & ~SYS_GPC_MFPL_PC5MFP_Msk) | SYS_GPC_MFPL_PC5MFP_PSIO0_CH0
#define PSIO0_CH1_PIN   SYS->GPC_MFPL = (SYS->GPC_MFPL & ~SYS_GPC_MFPL_PC4MFP_Msk)  | SYS_GPC_MFPL_PC4MFP_PSIO0_CH1
#define PSIO0_CH2_PIN   SYS->GPC_MFPL = (SYS->GPC_MFPL & ~SYS_GPC_MFPL_PC3MFP_Msk)  | SYS_GPC_MFPL_PC3MFP_PSIO0_CH2
#define PSIO0_CH3_PIN   SYS->GPC_MFPL = (SYS->GPC_MFPL & ~SYS_GPC_MFPL_PC2MFP_Msk)  | SYS_GPC_MFPL_PC2MFP_PSIO0_CH3
#define PSIO0_CH4_PIN   SYS->GPA_MFPL = (SYS->GPA_MFPL & ~SYS_GPA_MFPL_PA3MFP_Msk)  | SYS_GPA_MFPL_PA3MFP_PSIO0_CH4
#define PSIO0_CH5_PIN   SYS->GPA_MFPL = (SYS->GPA_MFPL & ~SYS_GPA_MFPL_PA2MFP_Msk)  | SYS_GPA_MFPL_PA2MFP_PSIO0_CH5
#define PSIO0_CH6_PIN   SYS->GPA_MFPL = (SYS->GPA_MFPL & ~SYS_GPA_MFPL_PA1MFP_Msk)  | SYS_GPA_MFPL_PA1MFP_PSIO0_CH6
#define PSIO0_CH7_PIN   SYS->GPD_MFPH = (SYS->GPD_MFPH & ~SYS_GPD_MFPH_PD15MFP_Msk) | SYS_GPD_MFPH_PD15MFP_PSIO0_CH7
#define PSIO0_TO_GPIO_PIN                                    \
    do                                                       \
    {                                                        \
        SYS->GPD_MFP3 &= ~(0xF << SYS_GPD_MFP3_PD15MFP_Pos); \
        SYS->GPA_MFP0 &= ~(0xF << SYS_GPA_MFP0_PA2MFP_Pos);  \
        SYS->GPC_MFP0 &= ~(0xF << SYS_GPC_MFP0_PC2MFP_Pos);  \
        SYS->GPC_MFP1 &= ~(0xF << SYS_GPC_MFP1_PC4MFP_Pos);  \
    } while (0)

#elif (PSIO_PIN_GROUP == 6)
#define PSIO0_CH0_PIN   SYS->GPE_MFPL = (SYS->GPE_MFPL & ~SYS_GPE_MFPL_PE7MFP_Msk) | SYS_GPE_MFPL_PE7MFP_PSIO0_CH0
#define PSIO0_CH1_PIN   SYS->GPE_MFPL = (SYS->GPE_MFPL & ~SYS_GPE_MFPL_PE6MFP_Msk) | SYS_GPE_MFPL_PE6MFP_PSIO0_CH1
#define PSIO0_CH2_PIN   SYS->GPE_MFPL = (SYS->GPE_MFPL & ~SYS_GPE_MFPL_PE5MFP_Msk) | SYS_GPE_MFPL_PE5MFP_PSIO0_CH2
#define PSIO0_CH3_PIN   SYS->GPE_MFPL = (SYS->GPE_MFPL & ~SYS_GPE_MFPL_PE4MFP_Msk) | SYS_GPE_MFPL_PE4MFP_PSIO0_CH3
#define PSIO0_CH4_PIN   SYS->GPD_MFPL = (SYS->GPD_MFPL & ~SYS_GPD_MFPL_PD7MFP_Msk) | SYS_GPD_MFPL_PD7MFP_PSIO0_CH4
#define PSIO0_CH5_PIN   SYS->GPD_MFPL = (SYS->GPD_MFPL & ~SYS_GPD_MFPL_PD6MFP_Msk) | SYS_GPD_MFPL_PD6MFP_PSIO0_CH5
#define PSIO0_CH6_PIN   SYS->GPD_MFPL = (SYS->GPD_MFPL & ~SYS_GPD_MFPL_PD5MFP_Msk) | SYS_GPD_MFPL_PD5MFP_PSIO0_CH6
#define PSIO0_CH7_PIN   SYS->GPD_MFPL = (SYS->GPD_MFPL & ~SYS_GPD_MFPL_PD4MFP_Msk) | SYS_GPD_MFPL_PD4MFP_PSIO0_CH7
#define PSIO0_TO_GPIO_PIN                                   \
    do                                                      \
    {                                                       \
        SYS->GPD_MFP1 &= ~(0xF << SYS_GPD_MFP1_PD4MFP_Pos); \
        SYS->GPD_MFP1 &= ~(0xF << SYS_GPD_MFP1_PD6MFP_Pos); \
        SYS->GPE_MFP1 &= ~(0xF << SYS_GPE_MFP1_PE4MFP_Pos); \
        SYS->GPE_MFP1 &= ~(0xF << SYS_GPE_MFP1_PE6MFP_Pos); \
    } while (0)

#elif (PSIO_PIN_GROUP == 7) //Self Test PIN
#define PSIO0_CH0_PIN   SYS->GPB_MFP3 = (SYS->GPB_MFP3 & (~SYS_GPB_MFP3_PB15MFP_Msk)) | SYS_GPB_MFP3_PB15MFP_PSIO0_CH0
#define PSIO0_CH1_PIN   SYS->GPB_MFP3 = (SYS->GPB_MFP3 & (~SYS_GPB_MFP3_PB14MFP_Msk)) | SYS_GPB_MFP3_PB14MFP_PSIO0_CH1
#define PSIO0_CH2_PIN   SYS->GPB_MFP3 = (SYS->GPB_MFP3 & (~SYS_GPB_MFP3_PB13MFP_Msk)) | SYS_GPB_MFP3_PB13MFP_PSIO0_CH2
#define PSIO0_CH3_PIN   SYS->GPB_MFP3 = (SYS->GPB_MFP3 & (~SYS_GPB_MFP3_PB12MFP_Msk)) | SYS_GPB_MFP3_PB12MFP_PSIO0_CH3
#define PSIO0_CH4_PIN   SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~SYS_GPA_MFP0_PA3MFP_Msk) | SYS_GPA_MFP0_PA3MFP_PSIO0_CH4
#define PSIO0_CH5_PIN   SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~SYS_GPA_MFP0_PA2MFP_Msk) | SYS_GPA_MFP0_PA2MFP_PSIO0_CH5
#define PSIO0_CH6_PIN   SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~SYS_GPA_MFP0_PA1MFP_Msk) | SYS_GPA_MFP0_PA1MFP_PSIO0_CH6
#define PSIO0_CH7_PIN   SYS->GPA_MFP0 = (SYS->GPA_MFP0 & ~SYS_GPA_MFP0_PA0MFP_Msk) | SYS_GPA_MFP0_PA0MFP_PSIO0_CH7
#define PSIO0_TO_GPIO_PIN                                   \
    do                                                      \
    {                                                       \
        SYS->GPA_MFP0 &= ~(0xF << SYS_GPA_MFP0_PA0MFP_Pos); \
        SYS->GPA_MFP0 &= ~(0xF << SYS_GPD_MFP0_PA2MFP_Pos); \
        SYS->GPB_MFP3 &= ~(0xF << SYS_GPB_MFP3_PB12MFP_Pos); \
        SYS->GPB_MFP3 &= ~(0xF << SYS_GPB_MFP3_PB14MFP_Pos); \
    } while (0)

#endif //PSIO_PIN_GROUP

#endif /* _TC8263_PIN_CONFIG_H_ */
