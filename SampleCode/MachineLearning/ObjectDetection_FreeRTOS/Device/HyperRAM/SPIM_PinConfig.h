#ifndef _SPIM_PIN_CONFIG_H_
#define _SPIM_PIN_CONFIG_H_

#define SPIM0_RST_PIN_INIT()                SPIM0_RST_PIN
#define SPIM0_CLK_PIN_INIT()                SPIM0_CLK_PIN
#define SPIM0_CLKN_PIN_INIT()               SPIM0_CLKN_PIN
#define SPIM0_MOSI_PIN_INIT()               SPIM0_MOSI_PIN
#define SPIM0_MISO_PIN_INIT()               SPIM0_MISO_PIN
#define SPIM0_D2_PIN_INIT()                 SPIM0_D2_PIN
#define SPIM0_D3_PIN_INIT()                 SPIM0_D3_PIN
#define SPIM0_D4_PIN_INIT()                 SPIM0_D4_PIN
#define SPIM0_D5_PIN_INIT()                 SPIM0_D5_PIN
#define SPIM0_D6_PIN_INIT()                 SPIM0_D6_PIN
#define SPIM0_D7_PIN_INIT()                 SPIM0_D7_PIN
#define SPIM0_SS_PIN_INIT()                 SPIM0_SS_PIN
#define SPIM0_RWDS_PIN_INIT()               SPIM0_RWDS_PIN

//#if (SPIM0_PIN_GROUP == 0)
#define SPIM0_RST_PIN   SYS->GPC_MFP0 = (SYS->GPC_MFP0 & (~SYS_GPC_MFP0_PC2MFP_Msk)) | SYS_GPC_MFP0_PC2MFP_SPIM0_RESETN
#define SPIM0_CLK_PIN   SYS->GPC_MFP1 = (SYS->GPC_MFP1 & (~SYS_GPC_MFP1_PC4MFP_Msk)) | SYS_GPC_MFP1_PC4MFP_SPIM0_CLK
#define SPIM0_CLKN_PIN  SYS->GPC_MFP1 = (SYS->GPC_MFP1 & (~SYS_GPC_MFP1_PC5MFP_Msk)) | SYS_GPC_MFP1_PC5MFP_SPIM0_CLKN
#define SPIM0_MOSI_PIN  SYS->GPG_MFP2 = (SYS->GPG_MFP2 & (~SYS_GPG_MFP2_PG11MFP_Msk)) | SYS_GPG_MFP2_PG11MFP_SPIM0_MOSI
#define SPIM0_MISO_PIN  SYS->GPG_MFP3 = (SYS->GPG_MFP3 & (~SYS_GPG_MFP3_PG12MFP_Msk)) | SYS_GPG_MFP3_PG12MFP_SPIM0_MISO
#define SPIM0_D2_PIN    SYS->GPC_MFP0 = (SYS->GPC_MFP0 & (~SYS_GPC_MFP0_PC0MFP_Msk)) | SYS_GPC_MFP0_PC0MFP_SPIM0_D2
#define SPIM0_D3_PIN    SYS->GPG_MFP2 = (SYS->GPG_MFP2 & (~SYS_GPG_MFP2_PG10MFP_Msk)) | SYS_GPG_MFP2_PG10MFP_SPIM0_D3
#define SPIM0_D4_PIN    SYS->GPG_MFP2 = (SYS->GPG_MFP2 & (~SYS_GPG_MFP2_PG9MFP_Msk)) | SYS_GPG_MFP2_PG9MFP_SPIM0_D4
#define SPIM0_D5_PIN    SYS->GPG_MFP3 = (SYS->GPG_MFP3 & (~SYS_GPG_MFP3_PG13MFP_Msk)) | SYS_GPG_MFP3_PG13MFP_SPIM0_D5
#define SPIM0_D6_PIN    SYS->GPG_MFP3 = (SYS->GPG_MFP3 & (~SYS_GPG_MFP3_PG14MFP_Msk)) | SYS_GPG_MFP3_PG14MFP_SPIM0_D6
#define SPIM0_D7_PIN    SYS->GPG_MFP3 = (SYS->GPG_MFP3 & (~SYS_GPG_MFP3_PG15MFP_Msk)) | SYS_GPG_MFP3_PG15MFP_SPIM0_D7
#define SPIM0_SS_PIN    SYS->GPC_MFP0 = (SYS->GPC_MFP0 & (~SYS_GPC_MFP0_PC3MFP_Msk)) | SYS_GPC_MFP0_PC3MFP_SPIM0_SS
#define SPIM0_RWDS_PIN  SYS->GPC_MFP0 = (SYS->GPC_MFP0 & (~SYS_GPC_MFP0_PC1MFP_Msk)) | SYS_GPC_MFP0_PC1MFP_SPIM0_RWDS

#define SPIM0_PIN_HIGH_SLEW()                  \
    do                                         \
    {                                          \
        PC->SMTEN |= (GPIO_SMTEN_SMTEN0_Msk |  \
                      GPIO_SMTEN_SMTEN1_Msk |  \
                      GPIO_SMTEN_SMTEN3_Msk |  \
                      GPIO_SMTEN_SMTEN4_Msk |  \
                      GPIO_SMTEN_SMTEN5_Msk);  \
        PG->SMTEN |= (GPIO_SMTEN_SMTEN9_Msk |  \
                      GPIO_SMTEN_SMTEN10_Msk | \
                      GPIO_SMTEN_SMTEN11_Msk | \
                      GPIO_SMTEN_SMTEN12_Msk | \
                      GPIO_SMTEN_SMTEN13_Msk | \
                      GPIO_SMTEN_SMTEN14_Msk | \
                      GPIO_SMTEN_SMTEN15_Msk); \
    } while (0)

//#endif //SPIM0_PIN_GROUP

#endif /* _PIN_CONFIG_H_ */
