
#ifndef __SENSOR_CONFIG_H__
#define __SENSOR_CONFIG_H__

#include "NuMicro.h"

typedef int (*PFN_INIT_FUNC)(uint32_t u32Param);

typedef struct s_sensor_info
{
    char            m_strName[32];
    uint32_t        m_u32Polarity;
    uint32_t        m_u32InputFormat;
    uint32_t        m_u32Width;
    uint32_t        m_u32Height;
    PFN_INIT_FUNC   m_pfnInitFunc;
} S_SENSOR_INFO;

#define OV7725SensorPolarity        (CCAP_PAR_VSP_HIGH | CCAP_PAR_HSP_LOW  | CCAP_PAR_PCLKP_HIGH)
#define OV7725DataFormatAndOrder    (CCAP_PAR_INDATORD_UYVY | CCAP_PAR_INFMT_YUV422 | CCAP_PAR_OUTFMT_YUV422)

#define STATIC_POLARITY             (CCAP_PAR_VSP_LOW | CCAP_PAR_HSP_LOW  | CCAP_PAR_PCLKP_LOW)
#define STATIC_INPUT_FORMAT         (CCAP_PAR_INDATORD_YUYV | CCAP_PAR_INFMT_YUV422)
#define STATIC_OUTPUT_FORMAT        CCAP_PAR_OUTFMT_YUV422
//#define STATIC_SENSOR_IN_WIDTH      160
//#define STATIC_SENSOR_IN_HEIGHT     120

#ifdef __cplusplus
extern "C" {
#endif

extern S_SENSOR_INFO g_sSensorHM1055;
extern S_SENSOR_INFO g_sSensorYUV422;
extern S_SENSOR_INFO g_sSensorRGB565;
extern S_SENSOR_INFO g_sSensorRGB888;
extern S_SENSOR_INFO g_sSensorMONO;
extern S_SENSOR_INFO g_sSensorCCIR656;

extern S_SENSOR_INFO g_sSensorYUV422_640x480;
extern S_SENSOR_INFO g_sSensorRGB565_640x480;
extern S_SENSOR_INFO g_sSensorRGB888_640x480;

#ifdef __cplusplus
}
#endif

#endif  // __SENSOR_CONFIG_H__
