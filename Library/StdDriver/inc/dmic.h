/******************************************************************************
 * @file     DMIC.h
 * @version  V1.00
 * @brief    DMIC driver header file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#ifndef __DMIC_H__
#define __DMIC_H__

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

/** @addtogroup DMIC_EXPORTED_CONSTANTS DMIC Exported Constants
  @{
*/

/*---------------------------------------------------------------------------------------------------------*/
/* DMIC CTL Constant Definitions                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define DMIC_CTL_CH0                  (DMIC_CTL_CHEN0_Msk)               /*!< DMIC Channel 0 */
#define DMIC_CTL_CH1                  (DMIC_CTL_CHEN1_Msk)               /*!< DMIC Channel 1 */
#define DMIC_CTL_CH2                  (DMIC_CTL_CHEN2_Msk)               /*!< DMIC Channel 2 */
#define DMIC_CTL_CH3                  (DMIC_CTL_CHEN3_Msk)               /*!< DMIC Channel 3 */

#define DMIC_CTL_SWRST                (DMIC_CTL_SWRST_Msk)				/*!< DMIC State Software Reset*/

#define DMIC_DIV_FCLR				  (0x3UL<<DMIC_DIV_FCLR_Pos)          /*!< DMIC FIFO Clear */

#define DMIC_DIV_DOWNSAMPLE_64        (0x0UL<<DMIC_DIV_OSR_Pos)          /*!< DMIC Down Sample Rate 32 */
#define DMIC_DIV_DOWNSAMPLE_128       (0x1UL<<DMIC_DIV_OSR_Pos)          /*!< DMIC Down Sample Rate 64 */
#define DMIC_DIV_DOWNSAMPLE_256       (0x2UL<<DMIC_DIV_OSR_Pos)          /*!< DMIC Down Sample Rate 128 */
#define DMIC_DIV_DOWNSAMPLE_50     	  (0x3UL<<DMIC_DIV_OSR_Pos)          /*!< DMIC Down Sample Rate 50 */
#define DMIC_DIV_DOWNSAMPLE_100		  (0x4UL<<DMIC_DIV_OSR_Pos)          /*!< DMIC Down Sample Rate 100 */

#define DMIC_CTL_LATCHDATA_CH01FR      (0x0UL<<DMIC_CTL_LCHEDGE01_Pos)    /*!< DMIC Data Latch Channel0 Falling Edge Channel1 Rising Edge*/
#define DMIC_CTL_LATCHDATA_CH01RF      (0x1UL<<DMIC_CTL_LCHEDGE01_Pos)    /*!< DMIC Data Latch Channel0 Rising Edge Channel1 Falling Edge*/
#define DMIC_CTL_LATCHDATA_CH23FR      (0x0UL<<DMIC_CTL_LCHEDGE23_Pos)    /*!< DMIC Data Latch Channel2 Falling Edge Channel3 Rising Edge*/
#define DMIC_CTL_LATCHDATA_CH23RF      (0x1UL<<DMIC_CTL_LCHEDGE23_Pos)    /*!< DMIC Data Latch Channel2 Rising Edge Channel3 Falling Edge*/

/*---------------------------------------------------------------------------------------------------------*/
/* VAD SINCCTL Constant Definitions                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define VAD_SINC_CTL_DOWNSAMPLE_48    (0x0UL<<VAD_SINCCTL_SINCOSR_Pos)          /*!< VAD SINC Filter Down Sample Rate 48 */
#define VAD_SINC_CTL_DOWNSAMPLE_64    (0x1UL<<VAD_SINCCTL_SINCOSR_Pos)          /*!< VAD SINC Filter Down Sample Rate 64 */
#define VAD_SINC_CTL_DOWNSAMPLE_96    (0x2UL<<VAD_SINCCTL_SINCOSR_Pos)          /*!< VAD SINC Filter Down Sample Rate 96 */

/*---------------------------------------------------------------------------------------------------------*/
/* VAD CTL Constant Definitions                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define VAD_CTL0_STAT_16MS            (0x99UL<<VAD_CTL0_STAT_Pos)                /*!< VAD Short Term Attack Time 16 ms */
#define VAD_CTL0_STAT_8MS             (0xaaUL<<VAD_CTL0_STAT_Pos)                /*!< VAD Short Term Attack Time 8 ms */
#define VAD_CTL0_STAT_4MS             (0xbbUL<<VAD_CTL0_STAT_Pos)                /*!< VAD Short Term Attack Time 4 ms */
#define VAD_CTL0_STAT_2MS             (0xccUL<<VAD_CTL0_STAT_Pos)                /*!< VAD Short Term Attack Time 2 ms */

#define VAD_CTL0_LTAT_512MS           (0x4UL<<VAD_CTL0_LTAT_Pos)                 /*!< VAD Long Term Attack Time 512 ms */
#define VAD_CTL0_LTAT_256MS           (0x5UL<<VAD_CTL0_LTAT_Pos)                 /*!< VAD Long Term Attack Time 256 ms */
#define VAD_CTL0_LTAT_128MS           (0x6UL<<VAD_CTL0_LTAT_Pos)                 /*!< VAD Long Term Attack Time 128 ms */
#define VAD_CTL0_LTAT_64MS            (0x7UL<<VAD_CTL0_LTAT_Pos)                 /*!< VAD Long Term Attack Time 64 ms */

/*---------------------------------------------------------------------------------------------------------*/
/* VAD Biquad Constant Definitions                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define VAD_COEFF_B0                  (0)                                        /*!< VAD Biquad coefficient b0		*/
#define VAD_COEFF_B1                  (1)                                        /*!< VAD Biquad coefficient b1		*/
#define VAD_COEFF_B2                  (2)                                        /*!< VAD Biquad coefficient b2		*/
#define VAD_COEFF_A1                  (3)                                        /*!< VAD Biquad coefficient a0		*/
#define VAD_COEFF_A2                  (4)                                        /*!< VAD Biquad coefficient a1		*/

/*---------------------------------------------------------------------------------------------------------*/
/* VAD Power Threshold Definitions                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define VAD_POWERTHRE_0DB             (0x7FFFUL)                                 /*!< VAD power threshold 0DB   	*/
#define VAD_POWERTHRE_10DB            (0x2879UL)                                 /*!< VAD power threshold 10DB  	*/
#define VAD_POWERTHRE_20DB            (0x0CCCUL)                                 /*!< VAD power threshold 20DB   	*/
#define VAD_POWERTHRE_30DB            (0x040CUL)                                 /*!< VAD power threshold 30DB   	*/
#define VAD_POWERTHRE_40DB            (0x0147UL)                                 /*!< VAD power threshold 40DB   	*/
#define VAD_POWERTHRE_50DB            (0x0067UL)                                 /*!< VAD power threshold 50DB   	*/
#define VAD_POWERTHRE_60DB            (0x0020UL)                                 /*!< VAD power threshold 60DB   	*/
#define VAD_POWERTHRE_70DB            (0x000AUL)                                 /*!< VAD power threshold 70DB   	*/
#define VAD_POWERTHRE_80DB            (0x0003UL)                                 /*!< VAD power threshold 80DB   	*/
#define VAD_POWERTHRE_90DB            (0x0001UL)                                 /*!< VAD power threshold 90DB   	*/

/*@}*/ /* end of group DMIC_EXPORTED_CONSTANTS */


/** @addtogroup DMIC_EXPORTED_FUNCTIONS DMIC Exported Functions
  @{
*/

/**
  * @brief      Enable DMIC FIFO threshold interrupt.
  * @param[in]  dmic The base address of DMIC module
  * @param[in]  u8Value: FIFO buffer threshold count
  * @return     None
  * @details    DMIC FIFO threshold interrupt Enabled.
  */
#define DMIC_ENABLE_FIFOTHRESHOLDINT(dmic,u8Value)    ((dmic)->DIV = (((dmic)->DIV&~DMIC_DIV_DMTH_Msk)|((((uint32_t)u8Value)<<DMIC_DIV_DMTH_Pos)&DMIC_DIV_DMTH_Msk))|DMIC_DIV_DMTHIE_Msk)

/**
  * @brief      Disable DMIC FIFO threshold interrupt.
  * @param[in]  dmic The base address of DMIC module
  * @return     None
  * @details    DMIC FIFO threshold interrupt Disabled.
  */
#define DMIC_DISABLE_FIFOTHRESHOLDINT(dmic)           ((dmic)->DIV &= (~DMIC_DIV_DMTHIE_Msk))

/**
  * @brief      Set DMIC channel data latch edge.
  * @param[in]  dmic The base address of DMIC module
  * @param[in]  u32Value Config Channle and edge state(falliing or rising)
  *             - \ref DMIC_CTL_LATCHDATA_CH01FR
  *             - \ref DMIC_CTL_LATCHDATA_CH01RF
  *             - \ref DMIC_CTL_LATCHDATA_CH23FR
  *             - \ref DMIC_CTL_LATCHDATA_CH23RF
  * @return     None
  * @details    Channel latched on rising or falling edge of DMIC_CLK.
  */
#define DMIC_SET_LATCHDATA(dmic,u32Value)             ((dmic)->CTL = ((dmic)->CTL & ~(DMIC_CTL_LCHEDGE01_Msk|DMIC_CTL_LCHEDGE23_Msk))|u32Value)

/**
  * @brief      Enable DMIC down sample rate.
  * @param[in]  dmic The base address of DMIC module
  * @param[in]  u32Value Down sample rate value.
  *             - \ref DMIC_DIV_DOWNSAMPLE_64
  *             - \ref DMIC_DIV_DOWNSAMPLE_128
  *             - \ref DMIC_DIV_DOWNSAMPLE_256
  *             - \ref DMIC_DIV_DOWNSAMPLE_50
  *             - \ref DMIC_DIV_DOWNSAMPLE_100
  * @return     None
  * @details    Enable DMIC down sample rate funciton and set down sample value.
  */
#define DMIC_ENABLE_DOWMSAMPLE(dmic,u32Value)         ((dmic)->DIV = (((dmic)->DIV) & (~DMIC_DIV_OSR_Msk))|u32Value)

/**
  * @brief      Disable DMIC down sample rate.
  * @param[in]  dmic The base address of DMIC module
  * @return     None
  * @details    Disable DMIC down sample rate funciton.
  */
#define DMIC_DISABLE_DOWMSAMPLE(dmic)                 ((dmic)->DIV |= DMIC_DIV_OSR_Msk)

/**
  * @brief      Enable DMIC's channel
  * @param[in]  dmic The base address of DMIC module
  * @param[in]  u32Ch Enable channle.
  *             - \ref DMIC_CTL_CH0
  *             - \ref DMIC_CTL_CH1
  *             - \ref DMIC_CTL_CH2
  *             - \ref DMIC_CTL_CH3
  * @return     None
  * @details    Enable channle to start input data.
  */
#define DMIC_ENABLE_CHANNEL(dmic,u32Ch)               ((dmic)->CTL |= u32Ch)

/**
  * @brief      Disable DMIC's channel
  * @param[in]  dmic The base address of DMIC module
  * @param[in]  u32Ch Disable channle.
  *             - \ref DMIC_CTL_CH0
  *             - \ref DMIC_CTL_CH1
  *             - \ref DMIC_CTL_CH2
  *             - \ref DMIC_CTL_CH3
  * @return     None
  * @details    Disable channle to start input data.
  */
#define DMIC_DISABLE_CHANNEL(dmic,u32Ch)              ((dmic)->CTL &= (~u32Ch))

/**
  * @brief      Enable DMIC's channel mute
  * @param[in]  dmic The base address of DMIC module
  * @param[in]  u32Ch Enable channle Mute.
  *             - \ref DMIC_CTL_CH0MUTE_Msk
  *             - \ref DMIC_CTL_CH1MUTE_Msk
  *             - \ref DMIC_CTL_CH2MUTE_Msk
  *             - \ref DMIC_CTL_CH3MUTE_Msk
  * @return     None
  * @details    Enable channle to mute.
  */
#define DMIC_ENABLE_MUTE(dmic,u32ChMute)               ((dmic)->CTL |= u32ChMute)

/**
  * @brief      Disable DMIC's channel mute
  * @param[in]  dmic The base address of DMIC module
  * @param[in]  u32Ch Disable channle Mute.
  *             - \ref DMIC_CTL_CH0MUTE_Msk
  *             - \ref DMIC_CTL_CH1MUTE_Msk
  *             - \ref DMIC_CTL_CH2MUTE_Msk
  *             - \ref DMIC_CTL_CH3MUTE_Msk
  * @return     None
  * @details    Disable channle to mute.
  */
#define DMIC_DISABLE_MUTE(dmic,u32ChMute)              ((dmic)->CTL &= (~u32ChMute))

/**
  * @brief      Enable DMIC's channel HPF filter
  * @param[in]  dmic The base address of DMIC module
  * @param[in]  u32Ch Enable channle HPF filter.
  *             - \ref DMIC_CTL_CH01HPFEN_Msk
  *             - \ref DMIC_CTL_CH23HPFEN_Msk
  * @return     None
  * @details    Enable channle to mute.
  */
#define DMIC_ENABLE_HPF(dmic,u32ChHPF)               ((dmic)->CTL |= u32ChHPF)

/**
  * @brief      Disable DMIC's channel HPF filter
  * @param[in]  dmic The base address of DMIC module
  * @param[in]  u32Ch Disable channle HPF filter.
  *             - \ref DMIC_CTL_CH01HPFEN_Msk
  *             - \ref DMIC_CTL_CH23HPFEN_Msk
  * @return     None
  * @details    Disable channle to mute.
  */
#define DMIC_DISABLE_HPF(dmic,u32ChHPF)              ((dmic)->CTL &= (~u32ChHPF))

/**
  * @brief     	Enable DMIC PDMA function.
  * @param[in] 	dmic The base address of DMIC module
  * @return   	None.
  * @details   	DMIC will request data to PDMA controller whenever there is space in FIFO.
  */
#define DMIC_ENABLE_PDMA(dmic)                        ((dmic)->PDMACTL |= DMIC_PDMACTL_PDMAEN_Msk)

/**
  * @brief     	Enable DMIC LPPDMA function.
  * @param[in] 	dmic The base address of DMIC module
  * @return   	None.
  * @details   	DMIC will request data to LPPDMA controller whenever there is space in FIFO.
  */
#define DMIC_ENABLE_LPPDMA(dmic)                        ((dmic)->PDMACTL |= DMIC_PDMACTL_LPPDMAEN_Msk)

/**
  * @brief     	Disable DMIC PDMA function.
  * @param[in] 	dmic The base address of DMIC module
  * @return    	None.
  */
#define DMIC_DISABLE_PDMA(dmic)                       ((dmic)->PDMACTL &= (~DMIC_PDMACTL_PDMAEN_Msk))

/**
  * @brief     	Disable DMIC LPPDMA function.
  * @param[in] 	dmic The base address of DMIC module
  * @return    	None.
  */
#define DMIC_DISABLE_LPPDMA(dmic)                       ((dmic)->PDMACTL &= (~DMIC_PDMACTL_LPPDMAEN_Msk))

/**
  * @brief      Check DMIC FIFO empty or not
  * @param[in]  dmic The base address of DMIC module
  * @return     0 = FIFO is not empty
  *             1 = FIFO is empty
  */
#define DMIC_IS_FIFOEMPTY(dmic)                       (((dmic)->STATUS) & DMIC_STATUS_EMPTY_Msk)

/**
  * @brief      Check DPWM FIFO full or not
  * @param[in]  dmic The base address of DMIC module
  * @return     0 = FIFO is not full
  *             1 = FIFO is full
  */
#define DMIC_IS_FIFOFULL(dmic)                        (((dmic)->STATUS) & DMIC_STATUS_FULL_Msk)

/**
  * @brief     	Read DMIC FIFO Audio Data Input.
  * @param[in]  dmic The base address of DMIC module
  * @return    	None.
  * @details   	A read function to this register pop data from the DMIC FIFO.
  */
#define DMIC_READ_DATA(dmic)                          ((dmic)->FIFO)

/**
  * @brief      Enable VAD down sample rate.
  * @param[in]  vad The base address of VAD module
  * @param[in]  u32Value Down sample rate value.
  *             - \ref VAD_SINC_CTL_DOWNSAMPLE_48
  *             - \ref VAD_SINC_CTL_DOWNSAMPLE_64
  *             - \ref VAD_SINC_CTL_DOWNSAMPLE_96
  * @return     None
  * @details    Enable VAD down sample rate funciton and set down sample value.
  */
#define VAD_ENABLE_DOWMSAMPLE(vad,u32Value)         ((vad)->SINCCTL = ((vad)->SINCCTL & ~VAD_SINCCTL_SINCOSR_Msk)|u32Value)

/**
  * @brief      Disable VAD down sample rate.
  * @param[in]  vad The base address of VAD module
  * @return     None
  * @details    Disable VAD down sample rate funciton.
  */
#define VAD_DISABLE_DOWMSAMPLE(vad)                 ((vad)->SINCCTL |= VAD_SINCCTL_SINCOSR_Msk)

/**
  * @brief      Enable VAD function.
  * @param[in]  vad The base address of VAD module
  * @return     None
  * @details    Start to detect voice from DMIC0
  */
#define VAD_ENABLE(vad)                             ((vad)->SINCCTL |= VAD_SINCCTL_VADEN_Msk)

/**
  * @brief      Disable VAD function.
  * @param[in]  vad The base address of VAD module
  * @return     None
  * @details    Stop to detect voice.
  */
#define VAD_DISABLE(vad)                            ((vad)->SINCCTL &= ~VAD_SINCCTL_VADEN_Msk)

/**
  * @brief      Set Short Term Attack Time
  * @param[in]  vad The base address of VAD module
  * @param[in]  u32Time Short Term Attack Time.
  *             - \ref VAD_CTL0_STAT_16MS
  *             - \ref VAD_CTL0_STAT_8MS
  *             - \ref VAD_CTL0_STAT_4MS
  *             - \ref VAD_CTL0_STAT_2MS
  * @return     None
  */
#define VAD_SET_STAT(vad,u32Time)                   ((vad)->CTL0 = ((vad)->CTL0 & ~VAD_CTL0_STAT_Msk)|u32Time)

/**
  * @brief      Set Long Term Attack Time
  * @param[in]  vad The base address of VAD module
  * @param[in]  u32Time Long Term Attack Time.
  *             - \ref VAD_CTL0_LTAT_512MS
  *             - \ref VAD_CTL0_LTAT_256MS
  *             - \ref VAD_CTL0_LTAT_128MS
  *             - \ref VAD_CTL0_LTAT_64MS
  * @return     None
  */
#define VAD_SET_LTAT(vad,u32Time)                   ((vad)->CTL0 = ((vad)->CTL0 & ~VAD_CTL0_LTAT_Msk)|u32Time)

/**
  * @brief      Set Short Term Power Threshold.
  * @param[in]  vad The base address of VAD module
  * @param[in]  u32Lower Lower Limit Threshold
  * @param[in]  u32Upper Upper Limit Threshold
  * @return     None
  */
#define VAD_SET_STTHRE(vad,u32Lower,u32Upper)       ((vad)->CTL1 = (u32Lower<<VAD_CTL1_STTHRELWM_Pos)|(u32Upper&VAD_CTL1_STTHREHWM_Msk))

/**
  * @brief      Set Long Term Power Threshold.
  * @param[in]  vad The base address of VAD module
  * @param[in]  u32Value Long Term Power Threshold
  * @return     None
  */
#define VAD_SET_LTTHRE(vad,u32Value)       ((vad)->CTL2 = ((vad)->CTL2&~VAD_CTL2_LTTHRE_Msk)|((u32Value<<VAD_CTL2_LTTHRE_Pos)&VAD_CTL2_LTTHRE_Msk))

/**
  * @brief      Set Deviation Threshold
  * @param[in]  vad The base address of VAD module
  * @param[in]  u32Value Deviation Threshold
  * @return     None
  */
#define VAD_SET_DEVTHRE(vad,u32Value)                ((vad)->CTL3 = ((vad)->CTL3&~VAD_CTL3_DEVTHRE_Msk)|(u32Value&VAD_CTL3_DEVTHRE_Msk))

/**
  * @brief     	Enable VAD's BIQ filter.
  * @param[in] 	vad The base address of VAD module
  * @return   	None.
  * @details   	Before you enable this function, please make sure your BIQ coefficient correct.
  */
#define VAD_ENABLE_BIQ(vad)                         ((vad)->BIQCTL2 |= VAD_BIQCTL2_BIQEN_Msk)

/**
  * @brief     	Disable VAD's BIQ filter.
  * @param[in] 	vad The base address of VAD module
  * @return    	None.
  */
#define VAD_DISABLE_BIQ(vad)                        ((vad)->BIQCTL2 &= ~VAD_BIQCTL2_BIQEN_Msk)
#define VAD_IS_ACTIVE(vad)                        ((vad)->STATUS0 & VAD_STATUS0_ACTIVE_Msk)
#define VAD_CLR_ACTIVE(vad)                        ((vad)->SINCCTL |= VAD_SINCCTL_ACTCL_Msk)
uint32_t VAD_SetSampleRate(VAD_T *vad, uint32_t u32SampleRate);

void VAD_WriteBIQCoeff(VAD_T *vad, uint8_t u8Coeff, uint32_t u32Value);

void DMIC_Open(DMIC_T *dmic);

void DMIC_Close(DMIC_T *dmic);

uint32_t DMIC_SetSampleRate(DMIC_T* dmic,uint32_t u32SampleRate);

/*@}*/ /* end of group DMIC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group DMIC_Driver */

/*@}*/ /* end of group Device_Driver */

#ifdef __cplusplus
}
#endif

#endif //__DMIC_H__
