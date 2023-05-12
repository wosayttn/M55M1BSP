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
  * @param      None.
  * @return     None.
  */
void DMIC_Open(DMIC_T *dmic)
{
}

/**
  * @brief      Stop DMIC module
  * @param      None.
  * @return     None.
  */
void DMIC_Close(DMIC_T *dmic)
{
	DMIC_DISABLE_CHANNEL(dmic,DMIC_CTL_CH0|DMIC_CTL_CH1|DMIC_CTL_CH2|DMIC_CTL_CH3);
}

/**
  * @brief      Set the sample Rate of data
  * @param      u32SampleRate is sample Rate of data.
  * @return     Real sample rate.
  */
uint32_t DMIC_SetSampleRate(DMIC_T* dmic,uint32_t u32SampleRate)
{
	uint16_t const au16OSRTable[5]={64,128,256,50,100};
	uint32_t u32SourceClock,u32BusClock,u32MainClock,u32OSR, u32Temp;

	// Get DMIC clock source.
	switch(CLK->DMICSEL&CLK_DMICSEL_DMIC0SEL_Msk)
	{
		case CLK_DMICSEL_DMIC0SEL_HXT:  u32SourceClock = CLK_GetHXTFreq();      break;
		case CLK_DMICSEL_DMIC0SEL_APLL1_DIV2:    u32SourceClock = (CLK_GetAPLL1ClockFreq()/2);   break;
		case CLK_DMICSEL_DMIC0SEL_MIRC:    u32SourceClock = CLK_GetMIRCFreq();        break;
		case CLK_DMICSEL_DMIC0SEL_HIRC:   u32SourceClock = __HIRC;                  break;
		case CLK_DMICSEL_DMIC0SEL_HIRC48M:   u32SourceClock = __HIRC48M;                  break;
		case CLK_DMICSEL_DMIC0SEL_PCLK4:   u32SourceClock = CLK_GetPCLK4Freq();                  break;
		default:                                                                   return 0;
	}
	// Get OSR config and cal BusClock.
	switch( dmic->DIV&DMIC_DIV_OSR_Msk )
	{
		case DMIC_DIV_DOWNSAMPLE_50:
		case DMIC_DIV_DOWNSAMPLE_100:
		case DMIC_DIV_DOWNSAMPLE_64:
		case DMIC_DIV_DOWNSAMPLE_128:
		case DMIC_DIV_DOWNSAMPLE_256:		u32OSR = au16OSRTable[(dmic->DIV&DMIC_DIV_OSR_Msk)>>DMIC_DIV_OSR_Pos];		break;
//		case DMIC_DIV_DOWNSAMPLE_50:
//		case DMIC_DIV_DOWNSAMPLE_100:		u32OSR = (u32SampleRate>=32500)?50:100;										break;
		default:     						u32OSR = 1;																	break;
	}

	// Cal BusClock.
	u32BusClock = u32SampleRate*u32OSR;
	if(u32BusClock > u32SourceClock){
		if(u32SampleRate*50 > u32SourceClock){
			dmic->CTL=0;
			dmic->DIV=0;
		  return 0;
		}
	  else if(u32SampleRate*64 > u32SourceClock){	
			DMIC_ENABLE_DOWMSAMPLE(DMIC0,DMIC_DIV_DOWNSAMPLE_50);
		  printf("DMIC change u32OSR %d!\n",u32OSR);
			u32OSR =50;
		}
		else{
			DMIC_ENABLE_DOWMSAMPLE(DMIC0,DMIC_DIV_DOWNSAMPLE_64);
		  printf("DMIC change u32OSR %d!\n",u32OSR);
			u32OSR =64;
		}
	  u32BusClock = u32SampleRate*u32OSR;
		u32MainClock = u32BusClock;
	}
	else{
		// Cal main working clock(Depends on whether the frequency is divided).
		u32MainClock = u32SampleRate*(((u32SourceClock%u32SampleRate)!=0)? 256:500);
	}

	if(u32MainClock > u32SourceClock){
		u32MainClock = u32BusClock;
		  printf("DMIC MainClock %d \n",u32MainClock);
	}
	dmic->DIV = (dmic->DIV & ~(DMIC_DIV_DMCLKDIV_Msk)) | ((u32MainClock/u32BusClock-1)<<DMIC_DIV_DMCLKDIV_Pos);
  CLK->DMICDIV = (u32SourceClock/u32MainClock-1);
  u32Temp = (CLK->DMICDIV+1);

	return ((u32SourceClock/u32Temp)/(((dmic->DIV&DMIC_DIV_DMCLKDIV_Msk)>>DMIC_DIV_DMCLKDIV_Pos)+1))/u32OSR;
}

/**
  * @brief      Set the detect voice's sample Rate
  * @param[in]  vad The base address of VAD module
  * @param[in]  u32SampleRate Sample Rate of input voice data.
  * @return     Real detect sample rate.
  */
uint32_t VAD_SetSampleRate(VAD_T* vad,uint32_t u32SampleRate)
{
	uint16_t const au16OSRTable[4]={48,64,96};
	uint32_t u32SourceClock,u32BusClock,u32MainClock,u32OSR;

	// Get VAD clock source.
	switch(CLK->DMICSEL & CLK_DMICSEL_VAD0SEL_Msk)
	{
		case CLK_DMICSEL_VAD0SEL_PCLK4:  u32SourceClock = CLK_GetPCLK4Freq();      break;
		case CLK_DMICSEL_VAD0SEL_MIRC:    u32SourceClock = __MIRC;                 break;
		case CLK_DMICSEL_VAD0SEL_HIRC:   u32SourceClock = __HIRC;                  break;
		default:                                                                   return 0;
	}
	// Get OSR config and cal BusClock.
	u32OSR = (((vad->SINCCTL&VAD_SINCCTL_SINCOSR_Msk)>>VAD_SINCCTL_SINCOSR_Pos)>=3)?1:(au16OSRTable[(vad->SINCCTL&VAD_SINCCTL_SINCOSR_Msk)>>VAD_SINCCTL_SINCOSR_Pos]);
	// Cal BusClock.
	u32BusClock = u32SampleRate*u32OSR;
	// Cal main working clock
	u32MainClock = u32BusClock*4;

	vad->SINCCTL = (vad->SINCCTL & ~(VAD_SINCCTL_VADMCLKDIV_Msk)) | ((u32MainClock/u32BusClock-1)<<VAD_SINCCTL_VADMCLKDIV_Pos);

	return ((u32SourceClock/(((vad->SINCCTL&VAD_SINCCTL_VADMCLKDIV_Msk)>>VAD_SINCCTL_VADMCLKDIV_Pos)+1))/4/u32OSR);
}

/**
  * @brief      Write BIQ's coefficient value.
  * @param[in]  vad The base address of VAD module
  * @return     Real sample rate.
  */
void VAD_WriteBIQCoeff(VAD_T *vad, uint8_t u8Coeff, uint32_t u32Value)
{
	switch(u8Coeff)
	{
		case VAD_COEFF_B0:	(vad)->BIQCTL1 = ((vad)->BIQCTL1 & ~VAD_BIQCTL1_BIQB0_Msk)|((u32Value<<VAD_BIQCTL1_BIQB0_Pos)&VAD_BIQCTL1_BIQB0_Msk);	break;
		case VAD_COEFF_B1:	(vad)->BIQCTL1 = ((vad)->BIQCTL1 & ~VAD_BIQCTL1_BIQB1_Msk)|((u32Value<<VAD_BIQCTL1_BIQB1_Pos)&VAD_BIQCTL1_BIQB1_Msk);	break;
		case VAD_COEFF_B2:	(vad)->BIQCTL2 = ((vad)->BIQCTL2 & ~VAD_BIQCTL2_BIQB2_Msk)|((u32Value<<VAD_BIQCTL2_BIQB2_Pos)&VAD_BIQCTL2_BIQB2_Msk);	break;
		case VAD_COEFF_A1:	(vad)->BIQCTL0 = ((vad)->BIQCTL0 & ~VAD_BIQCTL0_BIQA1_Msk)|((u32Value<<VAD_BIQCTL0_BIQA1_Pos)&VAD_BIQCTL0_BIQA1_Pos);	break;
		case VAD_COEFF_A2:	(vad)->BIQCTL0 = ((vad)->BIQCTL0 & ~VAD_BIQCTL0_BIQA2_Msk)|((u32Value<<VAD_BIQCTL0_BIQA2_Pos)&VAD_BIQCTL0_BIQA2_Pos);	break;
	}
}

/*@}*/ /* end of group DMIC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group DMIC_Driver */

/*@}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif
