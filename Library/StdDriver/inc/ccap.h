/**************************************************************************//**
 * @file     ccap.h
 * @version  V1.00
 * @brief    CCAP driver header file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#ifndef __CCAP_H__
#define __CCAP_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup CCAP_Driver CCAP Driver
  @{
*/

/** @addtogroup CCAP_EXPORTED_CONSTANTS CCAP Exported Constants
  @{
*/

/*---------------------------------------------------------------------------------------------------------*/
/* CTL constant definitions                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define CCAP_CTL_CCAPEN             (1ul << CCAP_CTL_CCAPEN_Pos)        /*!< CCAP CTL setting for enabling Camera Capture Interface         \hideinitializer */
#define CCAP_CTL_PKTEN              (1ul << CCAP_CTL_PKTEN_Pos)         /*!< CCAP CTL setting for enabling packet output mode               \hideinitializer */
#define CCAP_CTL_SHUTTER            (1ul << CCAP_CTL_SHUTTER_Pos)       /*!< CCAP CTL setting for enabling shutter mode                     \hideinitializer */
#define CCAP_CTL_UPDATE             (1ul << CCAP_CTL_UPDATE_Pos)        /*!< CCAP CTL setting for enabling update register at new frame     \hideinitializer */
#define CCAP_CTL_RESET              (1ul << CCAP_CTL_VPRST_Pos)         /*!< CCAP CTL setting for capture reset                             \hideinitializer */
#define CCAP_CTL_MY8_MY4            (0ul << CCAP_CTL_MY8_MY4_Pos)       /*!< CCAP CTL setting for 4-bit data I/O interface with Mono        \hideinitializer */
#define CCAP_CTL_MY8_MY4_SWAP       (1ul << CCAP_CTL_MY4_SWAP_Pos)      /*!< CCAP CTL setting for 4-bit data swap I/O interface with Mono   \hideinitializer */
#define CCAP_CTL_MY8_MY8            (1ul << CCAP_CTL_MY8_MY4_Pos)       /*!< CCAP CTL setting for 8-bit data I/O interface with Mono        \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/* PAR constant definitions                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define CCAP_PAR_INFMT_YUV422           (0ul << CCAP_PAR_INFMT_Pos)         /*!< CCAP PAR setting for Sensor Input Data YUV422 Format       \hideinitializer */
#define CCAP_PAR_INFMT_YUV422_          (2ul << CCAP_PAR_INFMT_Pos)         /*!< CCAP PAR setting for Sensor Input Data YUV422 Format       \hideinitializer */
#define CCAP_PAR_INFMT_RGB565           (1ul << CCAP_PAR_INFMT_Pos)         /*!< CCAP PAR setting for Sensor Input Data RGB565 Format       \hideinitializer */
#define CCAP_PAR_INFMT_RGB888           (3ul << CCAP_PAR_INFMT_Pos)         /*!< CCAP PAR setting for Sensor Input Data RGB888 Format       \hideinitializer */

#define CCAP_PAR_SENTYPE_CCIR601        (0ul << CCAP_PAR_SENTYPE_Pos)       /*!< CCAP PAR setting for Sensor Input CCIR601 Type             \hideinitializer */
#define CCAP_PAR_SENTYPE_CCIR656        (1ul << CCAP_PAR_SENTYPE_Pos)       /*!< CCAP PAR setting for Sensor Input CCIR656 Type             \hideinitializer */

#define CCAP_PAR_INDATORD_YUYV          (0x0ul << CCAP_PAR_INDATORD_Pos)    /*!< CCAP PAR setting for Sensor Input Data Order, YUYV         \hideinitializer */
#define CCAP_PAR_INDATORD_YVYU          (0x1ul << CCAP_PAR_INDATORD_Pos)    /*!< CCAP PAR setting for Sensor Input Data Order, YVYU         \hideinitializer */
#define CCAP_PAR_INDATORD_UYVY          (0x2ul << CCAP_PAR_INDATORD_Pos)    /*!< CCAP PAR setting for Sensor Input Data Order, UYVY         \hideinitializer */
#define CCAP_PAR_INDATORD_VYUY          (0x3ul << CCAP_PAR_INDATORD_Pos)    /*!< CCAP PAR setting for Sensor Input Data Order, VYUY         \hideinitializer */

#define CCAP_PAR_INDATORD_RGGB          (0x0ul << CCAP_PAR_INDATORD_Pos)    /*!< CCAP PAR setting for Sensor Input Data Order, Byte0: R[4:0] G[5:3], Byte1 G[2:0] B[4:0]    \hideinitializer */
#define CCAP_PAR_INDATORD_BGGR          (0x1ul << CCAP_PAR_INDATORD_Pos)    /*!< CCAP PAR setting for Sensor Input Data Order, Byte0: B[4:0] G[5:3], Byte1 G[2:0] R[4:0]    \hideinitializer */
#define CCAP_PAR_INDATORD_GBRG          (0x2ul << CCAP_PAR_INDATORD_Pos)    /*!< CCAP PAR setting for Sensor Input Data Order, Byte0: G[2:0] B[4:0], Byte1 R[4:0] G[5:3]    \hideinitializer */
#define CCAP_PAR_INDATORD_GRBG          (0x3ul << CCAP_PAR_INDATORD_Pos)    /*!< CCAP PAR setting for Sensor Input Data Order, Byte0: G[2:0] R[4:0], Byte1 B[4:0] G[5:3]    \hideinitializer */

#define CCAP_PAR_INDATORD_RGB888_RGB    (0x0ul << CCAP_PAR_INDATORD_Pos)    /*!< CCAP PAR setting for Sensor Input Data Order, RGB888 RGB   \hideinitializer */
#define CCAP_PAR_INDATORD_RGB888_RBG    (0x1ul << CCAP_PAR_INDATORD_Pos)    /*!< CCAP PAR setting for Sensor Input Data Order, RGB888 RBG   \hideinitializer */
#define CCAP_PAR_INDATORD_RGB888_GRB    (0x2ul << CCAP_PAR_INDATORD_Pos)    /*!< CCAP PAR setting for Sensor Input Data Order, RGB888 GRB   \hideinitializer */
#define CCAP_PAR_INDATORD_RGB888_GBR    (0x3ul << CCAP_PAR_INDATORD_Pos)    /*!< CCAP PAR setting for Sensor Input Data Order, RGB888 GBR   \hideinitializer */
#define CCAP_PAR_INDATORD_RGB888_BRG    (0x4ul << CCAP_PAR_INDATORD_Pos)    /*!< CCAP PAR setting for Sensor Input Data Order, RGB888 BRG   \hideinitializer */
#define CCAP_PAR_INDATORD_RGB888_BGR    (0x5ul << CCAP_PAR_INDATORD_Pos)    /*!< CCAP PAR setting for Sensor Input Data Order, RGB888 BGR   \hideinitializer */

#define CCAP_PAR_OUTFMT_YUV422          (0x0ul << CCAP_PAR_OUTFMT_Pos)      /*!< CCAP PAR setting for Image Data YUV422 Format Output to System Memory                      \hideinitializer */
#define CCAP_PAR_OUTFMT_ONLY_Y          (0x1ul << CCAP_PAR_OUTFMT_Pos)      /*!< CCAP PAR setting for Image Data ONLY_Y Format Output to System Memory                      \hideinitializer */
#define CCAP_PAR_OUTFMT_RGB555          (0x2ul << CCAP_PAR_OUTFMT_Pos)      /*!< CCAP PAR setting for Image Data RGB555 Format Output to System Memory                      \hideinitializer */
#define CCAP_PAR_OUTFMT_RGB565          (0x3ul << CCAP_PAR_OUTFMT_Pos)      /*!< CCAP PAR setting for Image Data RGB565 Format Output to System Memory                      \hideinitializer */
/*!< CCAP PAR/PARM setting for Image Data RGB888 UINT8 Format Output to System Memory                \hideinitializer */
#define CCAP_PAR_OUTFMT_RGB888_U8       ((1 << CCAP_PARM_RGB888OUTORD_Pos) | CCAP_PARM_OUTFMTH_Msk | (0x0ul << CCAP_PAR_OUTFMT_Pos))
#define CCAP_PAR_OUTFMT_BGR888_U8       ((0 << CCAP_PARM_RGB888OUTORD_Pos) | CCAP_PARM_OUTFMTH_Msk | (0x0ul << CCAP_PAR_OUTFMT_Pos))
#define CCAP_PAR_OUTFMT_ARGB888_U8      ((3 << CCAP_PARM_RGB888OUTORD_Pos) | CCAP_PARM_OUTFMTH_Msk | (0x0ul << CCAP_PAR_OUTFMT_Pos))
#define CCAP_PAR_OUTFMT_BGRA888_U8      ((2 << CCAP_PARM_RGB888OUTORD_Pos) | CCAP_PARM_OUTFMTH_Msk | (0x0ul << CCAP_PAR_OUTFMT_Pos))
/*!< CCAP PAR/PARM setting for Image Data RGB888 INT8  Format Output to System Memory                \hideinitializer */
#define CCAP_PAR_OUTFMT_RGB888_I8       ((1 << CCAP_PARM_RGB888OUTORD_Pos) | CCAP_PARM_OUTFMTH_Msk | (0x1ul << CCAP_PAR_OUTFMT_Pos))
#define CCAP_PAR_OUTFMT_BGR888_I8       ((0 << CCAP_PARM_RGB888OUTORD_Pos) | CCAP_PARM_OUTFMTH_Msk | (0x1ul << CCAP_PAR_OUTFMT_Pos))
#define CCAP_PAR_OUTFMT_ARGB888_I8      ((3 << CCAP_PARM_RGB888OUTORD_Pos) | CCAP_PARM_OUTFMTH_Msk | (0x1ul << CCAP_PAR_OUTFMT_Pos))
#define CCAP_PAR_OUTFMT_BGRA888_I8      ((2 << CCAP_PARM_RGB888OUTORD_Pos) | CCAP_PARM_OUTFMTH_Msk | (0x1ul << CCAP_PAR_OUTFMT_Pos))

#define CCAP_PAR_VSP_LOW                (0x0ul << CCAP_PAR_VSP_Pos)         /*!< CCAP PAR setting for Sensor Vsync Polarity                 \hideinitializer */
#define CCAP_PAR_VSP_HIGH               (0x1ul << CCAP_PAR_VSP_Pos)         /*!< CCAP PAR setting for Sensor Vsync Polarity                 \hideinitializer */
#define CCAP_PAR_HSP_LOW                (0x0ul << CCAP_PAR_HSP_Pos)         /*!< CCAP PAR setting for Sensor Hsync Polarity                 \hideinitializer */
#define CCAP_PAR_HSP_HIGH               (0x1ul << CCAP_PAR_HSP_Pos)         /*!< CCAP PAR setting for Sensor Hsync Polarity                 \hideinitializer */
#define CCAP_PAR_PCLKP_LOW              (0x0ul << CCAP_PAR_PCLKP_Pos)       /*!< CCAP PAR setting for Sensor Pixel Clock Polarity           \hideinitializer */
#define CCAP_PAR_PCLKP_HIGH             (0x1ul << CCAP_PAR_PCLKP_Pos)       /*!< CCAP PAR setting for Sensor Pixel Clock Polarity           \hideinitializer */
#define CCAP_PAR_CCIR656_NO_BLANK       (0x1ul << CCAP_PAR_FBB_Pos)         /*!< CCAP PAR setting for Sensor Pixel Clock Polarity           \hideinitializer */

#define CCAP_MD_TRIG_LPTMR0             (0x0ul << CCAP_MDTRG_WK_TRGSEL_Pos) /*!< CCAP Motion Detection trigger source for LPTMR0            \hideinitializer */
#define CCAP_MD_TRIG_LPTMR1             (0x1ul << CCAP_MDTRG_WK_TRGSEL_Pos) /*!< CCAP Motion Detection trigger source for LPTMR1            \hideinitializer */
#define CCAP_MD_TRIG_TTMR0              (0x2ul << CCAP_MDTRG_WK_TRGSEL_Pos) /*!< CCAP Motion Detection trigger source for TTMR0             \hideinitializer */
#define CCAP_MD_TRIG_TTMR1              (0x3ul << CCAP_MDTRG_WK_TRGSEL_Pos) /*!< CCAP Motion Detection trigger source for TTMR1             \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/* INT constant definitions                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define CCAP_INT_VIEN_ENABLE            CCAP_INT_VIEN_Msk                   /*!< VININT setting for Video Frame End Interrupt enable            \hideinitializer */
#define CCAP_INT_MEIEN_ENABLE           CCAP_INT_MEIEN_Msk                  /*!< VININT setting for Bus Master Transfer Error Interrupt enable  \hideinitializer */
#define CCAP_INT_ADDRMIEN_ENABLE        CCAP_INT_ADDRMIEN_Msk               /*!< VININT setting for Memory Address Match Interrupt enable       \hideinitializer */
#define CCAP_INT_MDIEN_MODE1_ENABLE     (1ul << CCAP_INT_MDIEN_Pos)         /*!< VININT setting for Motion Detection Modes Interrupt enable     \hideinitializer */
#define CCAP_INT_MDIEN_MODE2_ENABLE     (2ul << CCAP_INT_MDIEN_Pos)         /*!< VININT setting for Motion Detection Modes Interrupt enable     \hideinitializer */

#define CCAP_INTSTS_MDINTF_MODE1        (1ul << CCAP_INTSTS_MDINTF_Pos)     /*!< VININT status for Motion Detection Modes 1                     \hideinitializer */
#define CCAP_INTSTS_MDINTF_MODE2        (2ul << CCAP_INTSTS_MDINTF_Pos)     /*!< VININT status for Motion Detection Modes 2                     \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/* CCAP Define Error Code                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define CCAP_OK             ( 0L)   /*!< CCAP operation OK */
#define CCAP_ERR_FAIL       (-1L)   /*!< CCAP operation failed */
#define CCAP_ERR_TIMEOUT    (-2L)   /*!< CCAP operation abort due to timeout error */


/** @} end of group CCAP_EXPORTED_CONSTANTS */

/** @addtogroup CCAP_EXPORTED_FUNCTIONS CCAP Exported Functions
  @{
*/

/**
 * @brief     Is CCAP module stopped
 *
 * @param     None
 *
 * @retval    0   CCAP module is enabled
 * @retval    1   CCAP module is disabled(stopped)
 *
 * @details   Check Camera Capture Interface module Enable or Disable(stopped)
 *  \hideinitializer
 */
#define CCAP_IS_STOPPED()  ((CCAP->CTL & CCAP_CTL_CCAPEN_Msk) ? 0 : 1)

/**
 * @brief     Clear CCAP flag
 *
 * @param[in] u32IntMask interrupt flags settings. It could be
 *                   - \ref CCAP_INTSTS_VINTF_Msk
 *                   - \ref CCAP_INTSTS_MEINTF_Msk
 *                   - \ref CCAP_INTSTS_ADDRMINTF_Msk
 *                   - \ref CCAP_INTSTS_MDINTF_Msk
 *
 * @return    None
 *
 * @details   Clear Camera Capture Interface interrupt flag
 *  \hideinitializer
 */
#define CCAP_CLR_INT_FLAG(u32IntMask) (CCAP->INTSTS |= (u32IntMask))

/**
 * @brief     Get CCAP Interrupt status
 *
 * @param     None
 *
 * @return    CCAP Interrupt Register
 *
 * @details   Get Camera Capture Interface interrupt status.
 * \hideinitializer
 */
#define CCAP_GET_INT_STS() (CCAP->INT)

/**
 * @brief     Enable CCAP Motion Detection Window
 *
 * @param     Motion Detection Window Bitmask
 *
 * @return    None
 *
 * @details   Enable Camera Capture Interface motion detection window
 * \hideinitializer
 */
#define CCAP_ENABLE_MD_WINDOW(u32WindowMsk) (CCAP->MDCTL |= u32WindowMsk)

/**
 * @brief     Disable CCAP Motion Detection Window
 *
 * @param     Motion Detection Window Bitmask
 *
 * @return    None
 *
 * @details   Disable Camera Capture Interface motion detection window
 * \hideinitializer
 */
#define CCAP_DISABLE_MD_WINDOW(u32WindowMsk) (CCAP->MDCTL &= ~u32WindowMsk)

/**
 * @brief     Set CCAP Motion Detection Trigger Source
 *
 * @param     Motion Detection Trigger Source. It could be
 *            - \ref CCAP_MD_TRIG_LPTMR0
 *            - \ref CCAP_MD_TRIG_LPTMR1
 *            - \ref CCAP_MD_TRIG_TTMR0
 *            - \ref CCAP_MD_TRIG_TTMR1
 *
 * @return    None
 *
 * @details   Set Camera Capture Interface motion detection trigger source
 * \hideinitializer
 */
#define CCAP_SET_MD_TRIG_SRC(u32TrigSrc, bWakeUpCPU) \
    (CCAP->MDTRG_WK = (CCAP->MDTRG_WK & ~(CCAP_MDTRG_WK_WKEN_Msk | CCAP_MDTRG_WK_TRGSEL_Msk)) | (u32TrigSrc | (bWakeUpCPU << CCAP_MDTRG_WK_WKEN_Pos)))
    
/**
 * @brief     Set CCAP Motion Detection Total Threshold
 *
 * @param     Motion Detection Total Threshold (Maximum value is 0x12AD40)
 *
 * @return    None
 *
 * @details   Set Camera Capture Interface motion detection total threshold
 * \hideinitializer
 */
#define CCAP_SET_MD_TOTAL_THRESH(u32TotalThreshold) (CCAP->MDTTH = u32TotalThreshold)

/**
 * @brief     Set CCAP Motion Detection Window Overflow Count
 *
 * @param     Motion Detection Total Window Overflow Count (Maximum value is 0xF)
 *
 * @return    None
 *
 * @details   Set Camera Capture Interface motion detection window overflow count
 * \hideinitializer
 */
#define CCAP_SET_MD_WIN_OVC_THRESH(u32WinOverflowCnt) (CCAP->MDWOCTH = u32WinOverflowCnt)

/**
 * @brief     Set CCAP Motion Detection Window Threshold
 *
 * @param     Motion Detection Window Threshold (Maximum value is 0x12AD4)
 * @param[in]  u32WinIdx:    Motion Detection Window Index. It should be 0x0 ~ 0xF.
 * @param[in]  u32Threshold: Motion Detection Window Threshold (Maximum value is 0x12AD4).
 *
 * @return    None
 *
 * @details   Set Camera Capture Interface motion detection total threshold
 * \hideinitializer
 */
#define CCAP_SET_MD_WIN_THRESH(u32WinIdx, u32Threshold) (CCAP->MDWTH[u32WinIdx] = u32Threshold)

void CCAP_Open(uint32_t u32SensorProp, uint32_t u32InFormat, uint32_t u32OutFormat);
void CCAP_SetCroppingWindow(uint32_t u32VStart, uint32_t u32HStart, uint32_t u32Height, uint32_t u32Width);
void CCAP_SetPacketBuf(uint32_t u32Address);
void CCAP_Close(void);
void CCAP_EnableInt(uint32_t u32IntMask);
void CCAP_DisableInt(uint32_t u32IntMask);
void CCAP_Start(void);
int32_t CCAP_Stop(uint32_t u32FrameComplete);
void CCAP_SetPacketScaling(uint32_t u32VNumerator, uint32_t u32VDenominator, uint32_t u32HNumerator, uint32_t u32HDenominator);
void CCAP_SetPacketStride(uint32_t u32Stride);
void CCAP_EnableMono(uint32_t u32Interface);
void CCAP_DisableMono(void);
void CCAP_EnableLumaYOne(uint32_t u32th);
void CCAP_DisableLumaYOne(void);

/** @} end of group CCAP_EXPORTED_FUNCTIONS */
/** @} end of group CCAP_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif //__CCAP_H__
