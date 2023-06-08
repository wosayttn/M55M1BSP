/**************************************************************************//**
 * @file     img_sensor.h
 * @version  V1.00
 * @brief    image sensor capture function
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#ifndef __IMG_SENSOR_H__
#define __IMG_SENSOR_H__

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    eIMG_FMT_YUV422 = 0,
    eIMG_FMT_RGB555,
    eIMG_FMT_RGB565,
    eIMG_FMT_ONLY_Y,
    eIMG_FMT_ONLY_Y_1BIT,
    eIMG_FMT_RGB888_U8,
    eIMG_FMT_BGR888_U8,
    eIMG_FMT_RGB888_I8,
    eIMG_FMT_BGR888_I8,
    eIMG_FMT_ARGB888_U8,
    eIMG_FMT_BGRA888_U8,
    eIMG_FMT_ARGB888_I8,
    eIMG_FMT_BGRA888_I8,
}E_IMG_FMT;

	
int img_sensor_init(void);
int img_sensor_capture(uint32_t u32FrameBufAddr);
int img_sensor_config(E_IMG_FMT eImgFmt, uint32_t u32ImgWidth, uint32_t u32ImgHeight);

#ifdef __cplusplus
}
#endif

#endif
