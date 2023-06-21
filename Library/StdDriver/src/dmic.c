/**************************************************************************//**
 * @file     dmic.c
 * @version  V1.00
 * @brief    DMIC driver source file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include "NuMicro.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup DMIC_Driver DMIC Driver
  @{
*/

/** @addtogroup DMIC_EXPORTED_FUNCTIONS DMIC Exported Functions
  @{
*/

/**
  * @brief      Start DMIC module
  * @param[in]  dmic: The base address of DMIC module
  * @return     None.
  */
void DMIC_Open(DMIC_T *dmic, uint32_t u32ChMsk, uint32_t u32Width)
{
    uint32_t  i;
    DMIC0->CTL |= (DMIC_CTL_DSPMEMT_Msk);

    //  delay(100);
    for (i = 0; i < 128; i++)
    {
        if (i == 58 | i == 122)
        {
            DMIC0->DSP0RAMDATA = 0xF80000;
            DMIC0->DSP1RAMDATA = 0xF80000;
        }
        else if (i == 62 | i == 126)
        {
            DMIC0->DSP0RAMDATA = 0x400;
            DMIC0->DSP1RAMDATA = 0x400;
        }
        else
        {
            DMIC0->DSP0RAMDATA = 0x0;
            DMIC0->DSP1RAMDATA = 0x0;
        }
    }

    printf("RAM Initialze Done....i=%d\n", i);
    DMIC0->CTL &= (~DMIC_CTL_DSPMEMT_Msk);

    if (DMIC0->DSP0ROMMBISTH != 0x00000027 || DMIC0->DSP1ROMMBISTH != 0x00000027)
        printf("DSPxROMMBISTH Err");

    if (DMIC0->DSP0ROMMBISTL != 0xc9bf447c || DMIC0->DSP1ROMMBISTL != 0xc9bf447c)
        printf("DSPxROMMBISTL Err");

    DMIC_ENABLE_CHANNEL(dmic, u32ChMsk);
    DMIC_SET_FIFOWIDTH(dmic, u32Width);
}

/**
  * @brief      Stop DMIC module
  * @param[in]  dmic: The base address of DMIC module
  * @return     None.
  */
void DMIC_Close(DMIC_T *dmic)
{
    DMIC_DISABLE_CHANNEL(dmic, DMIC_CTL_CHEN0_Msk | DMIC_CTL_CHEN1_Msk | DMIC_CTL_CHEN2_Msk | DMIC_CTL_CHEN3_Msk);
}

void DMIC_FIFO_CLR(DMIC_T *dmic)
{
    (dmic)->DIV |= DMIC_DIV_FCLR_Msk;

    while (!DMIC_IS_FIFOEMPTY(dmic)) {};
}
/**
  * @brief      Set the sample Rate of data
  * @param[in]  dmic The base address of DMIC module
  * @param      u32SampleRate is sample Rate of data.
  * @return     Real sample rate.
  */
uint32_t DMIC_SetSampleRate(DMIC_T *dmic, uint32_t u32SampleRate)
{
    uint16_t const au16OSRTable[] = {64, 128, 256, 50, 0, 0, 0, 100};
    uint32_t u32SourceClock, u32BusClock, u32MainClock, u32OSR, u32Temp;

    // Get DMIC clock source.
    switch (CLK->DMICSEL & CLK_DMICSEL_DMIC0SEL_Msk)
    {
        case CLK_DMICSEL_DMIC0SEL_HXT:
            u32SourceClock = CLK_GetHXTFreq();
            break;

        case CLK_DMICSEL_DMIC0SEL_APLL1_DIV2:
            u32SourceClock = (CLK_GetAPLL1ClockFreq() / 2);
            break;

        case CLK_DMICSEL_DMIC0SEL_MIRC:
            u32SourceClock = CLK_GetMIRCFreq();
            break;

        case CLK_DMICSEL_DMIC0SEL_HIRC:
            u32SourceClock = __HIRC;
            break;

        case CLK_DMICSEL_DMIC0SEL_HIRC48M:
            u32SourceClock = __HIRC48M;
            break;

        case CLK_DMICSEL_DMIC0SEL_PCLK4:
            u32SourceClock = CLK_GetPCLK4Freq();
            break;

        default:
            return 0;
    }

    // Get OSR config and cal BusClock.
    switch (dmic->DIV & DMIC_DIV_OSR_Msk)
    {
        case DMIC_DIV_DOWNSAMPLE_50:
        case DMIC_DIV_DOWNSAMPLE_100:
        case DMIC_DIV_DOWNSAMPLE_64:
        case DMIC_DIV_DOWNSAMPLE_128:
        case DMIC_DIV_DOWNSAMPLE_256:
            u32OSR = au16OSRTable[(dmic->DIV & DMIC_DIV_OSR_Msk) >> DMIC_DIV_OSR_Pos];
            break;

        //      case DMIC_DIV_DOWNSAMPLE_50:
        //      case DMIC_DIV_DOWNSAMPLE_100:       u32OSR = (u32SampleRate>=32500)?50:100;                                     break;
        default:
            u32OSR = 1;
            break;
    }

    // Cal BusClock.
    u32BusClock = u32SampleRate * u32OSR;

    if (u32BusClock > u32SourceClock)
    {
        if (u32SampleRate * 50 > u32SourceClock)
        {
            dmic->CTL = 0;
            dmic->DIV = 0;
            return 0;
        }
        else if (u32SampleRate * 64 > u32SourceClock)
        {
            DMIC_ENABLE_DOWMSAMPLE(DMIC0, DMIC_DIV_DOWNSAMPLE_50);
            printf("DMIC change u32OSR %d!\n", u32OSR);
            u32OSR = 50;
        }
        else
        {
            DMIC_ENABLE_DOWMSAMPLE(DMIC0, DMIC_DIV_DOWNSAMPLE_64);
            printf("DMIC change u32OSR %d!\n", u32OSR);
            u32OSR = 64;
        }

        u32BusClock = u32SampleRate * u32OSR;
        u32MainClock = u32BusClock;
    }
    else
    {
        // Cal main working clock(Depends on whether the frequency is divided).
        u32MainClock = u32SampleRate * (((u32SourceClock % u32SampleRate) != 0) ? 256 : 500);
    }

    if (u32MainClock > u32SourceClock)
    {
        u32MainClock = u32BusClock;
        printf("DMIC MainClock %d \n", u32MainClock);
    }

    dmic->DIV = (dmic->DIV & ~(DMIC_DIV_DMCLKDIV_Msk)) | ((u32MainClock / u32BusClock - 1) << DMIC_DIV_DMCLKDIV_Pos);
    CLK->DMICDIV = (u32SourceClock / u32MainClock - 1);
    u32Temp = (CLK->DMICDIV + 1);

    return ((u32SourceClock / u32Temp) / (((dmic->DIV & DMIC_DIV_DMCLKDIV_Msk) >> DMIC_DIV_DMCLKDIV_Pos) + 1)) / u32OSR;
}

/**
  * @brief      Set the detect voice's sample Rate
  * @param[in]  vad The base address of VAD module
  * @param[in]  u32SampleRate Sample Rate of input voice data.
  * @return     Real detect sample rate.
  */
uint32_t VAD_SetSampleRate(VAD_T *vad, uint32_t u32SampleRate)
{
    uint16_t const au16OSRTable[4] = {48, 64, 96};
    uint32_t u32SourceClock, u32BusClock, u32MainClock, u32OSR;

    // Get VAD clock source.
    switch (CLK->DMICSEL & CLK_DMICSEL_VAD0SEL_Msk)
    {
        case CLK_DMICSEL_VAD0SEL_PCLK4:
            u32SourceClock = CLK_GetPCLK4Freq();
            break;

        case CLK_DMICSEL_VAD0SEL_MIRC:
            u32SourceClock = __MIRC;
            break;

        case CLK_DMICSEL_VAD0SEL_HIRC:
            u32SourceClock = __HIRC;
            break;

        default:
            return 0;
    }

    // Get OSR config and cal BusClock.
    u32OSR = (((vad->SINCCTL & VAD_SINCCTL_SINCOSR_Msk) >> VAD_SINCCTL_SINCOSR_Pos) >= 3) ? 1 : (au16OSRTable[(vad->SINCCTL & VAD_SINCCTL_SINCOSR_Msk) >> VAD_SINCCTL_SINCOSR_Pos]);
    // Cal BusClock.
    u32BusClock = u32SampleRate * u32OSR;
    // Cal main working clock
    u32MainClock = u32BusClock * 4;

    vad->SINCCTL = (vad->SINCCTL & ~(VAD_SINCCTL_VADMCLKDIV_Msk)) | ((u32MainClock / u32BusClock - 1) << VAD_SINCCTL_VADMCLKDIV_Pos);

    return ((u32SourceClock / (((vad->SINCCTL & VAD_SINCCTL_VADMCLKDIV_Msk) >> VAD_SINCCTL_VADMCLKDIV_Pos) + 1)) / 4 / u32OSR);
}

/**
  * @brief      Write BIQ's coefficient value.
  * @param[in]  vad The base address of VAD module
  * @return     None.
  */
void VAD_WriteBIQCoeff(VAD_T *vad, uint8_t u8Coeff, uint32_t u32Value)
{
    switch (u8Coeff)
    {
        case VAD_COEFF_B0:
            (vad)->BIQCTL1 = ((vad)->BIQCTL1 & ~VAD_BIQCTL1_BIQB0_Msk) | ((u32Value << VAD_BIQCTL1_BIQB0_Pos)&VAD_BIQCTL1_BIQB0_Msk);
            break;

        case VAD_COEFF_B1:
            (vad)->BIQCTL1 = ((vad)->BIQCTL1 & ~VAD_BIQCTL1_BIQB1_Msk) | ((u32Value << VAD_BIQCTL1_BIQB1_Pos)&VAD_BIQCTL1_BIQB1_Msk);
            break;

        case VAD_COEFF_B2:
            (vad)->BIQCTL2 = ((vad)->BIQCTL2 & ~VAD_BIQCTL2_BIQB2_Msk) | ((u32Value << VAD_BIQCTL2_BIQB2_Pos)&VAD_BIQCTL2_BIQB2_Msk);
            break;

        case VAD_COEFF_A1:
            (vad)->BIQCTL0 = ((vad)->BIQCTL0 & ~VAD_BIQCTL0_BIQA1_Msk) | ((u32Value << VAD_BIQCTL0_BIQA1_Pos)&VAD_BIQCTL0_BIQA1_Pos);
            break;

        case VAD_COEFF_A2:
            (vad)->BIQCTL0 = ((vad)->BIQCTL0 & ~VAD_BIQCTL0_BIQA2_Msk) | ((u32Value << VAD_BIQCTL0_BIQA2_Pos)&VAD_BIQCTL0_BIQA2_Pos);
            break;
    }

    //        VAD0->BIQCTL0 = 0x19a0c715;
    //        VAD0->BIQCTL1 = 0xc6bb1ca3;
    //        VAD0->BIQCTL2 = 0x80001ca3;
}

/*@}*/ /* end of group DMIC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group DMIC_Driver */

/*@}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif
