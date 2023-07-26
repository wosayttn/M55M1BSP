/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief    Use packet format (all the luma and chroma data interleaved) to
 *           store captured image from NT99141 sensor to SRAM and
 *           encode the image to jpeg.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "sensor.h"

#define SENSOR_IN_WIDTH             640
#define SENSOR_IN_HEIGHT            480
#define SYSTEM_WIDTH                160
#define SYSTEM_HEIGHT               120
uint8_t g_au8FrameBuffer[DCACHE_ALIGN_LINE_SIZE(SYSTEM_WIDTH * SYSTEM_HEIGHT * 2)] __ALIGNED(DCACHE_LINE_SIZE);

void CCAPInterruptHandler(void);
void CCAP_IRQHandler(void);
void CCAPSetFreq(uint32_t u32ModFreqKHz, uint32_t u32SensorFreq);
int32_t PacketFormatDownScale(void);
void UART0_Init(void);
void SYS_Init(void);
extern void JpegEncode(unsigned char *image, unsigned char *jBuf, unsigned long *jSize, int width, int height);

/*------------------------------------------------------------------------------------------*/
/* To run CCAPInterrupt Handler, when CCAP frame end interrupt                              */
/*------------------------------------------------------------------------------------------*/
volatile uint32_t g_u32FramePass = 0;

void CCAPInterruptHandler(void)
{
    g_u32FramePass++;
}

void CCAP_IRQHandler(void)
{
    uint32_t u32IntStatus = CCAP_GET_INT_STS();

    if (CCAP_IsIntEnabled(CCAP_INTEN_VIEN_Msk) && (u32IntStatus & CCAP_INTSTS_VINTF_Msk) == CCAP_INTSTS_VINTF_Msk)
    {
        CCAPInterruptHandler();
        CCAP_CLR_INT_FLAG(CCAP_INTSTS_VINTF_Msk);   /* Clear Frame end interrupt */
        /* Invalidate data cache of received frame buffer.  */
        SCB_InvalidateDCache_by_Addr(g_au8FrameBuffer, sizeof(g_au8FrameBuffer));
    }

    if (CCAP_IsIntEnabled(CCAP_INTEN_ADDRMIEN_Msk) && (u32IntStatus & CCAP_INTSTS_ADDRMINTF_Msk) == CCAP_INTSTS_ADDRMINTF_Msk)
    {
        CCAP_CLR_INT_FLAG(CCAP_INTSTS_ADDRMINTF_Msk);   /* Clear Address match interrupt */
    }

    if (CCAP_IsIntEnabled(CCAP_INTEN_VIEN_Msk) && (u32IntStatus & CCAP_INTSTS_MEINTF_Msk) == CCAP_INTSTS_MEINTF_Msk)
    {
        CCAP_CLR_INT_FLAG(CCAP_INTSTS_MEINTF_Msk);     /* Clear Memory error interrupt */
    }

    CCAP_EnableUpdate();
}

void CCAPSetFreq(uint32_t u32ModFreqKHz, uint32_t u32SensorFreq)
{
    int32_t i32Div;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable CCAP Clock */
    CLK_EnableModuleClock(CCAP0_MODULE);

    /* Reset IP */
    SYS_ResetModule(SYS_CCAP0RST);

    /* Specified sensor clock */
    /* Not support in TESTCHIP_ONLY
        CLK_SetModuleClock(CCAP0_MODULE, CLK_CCAPSEL_CCAP0SEL_HIRC, 0);
        i32Div= (CLK_GetHCLK2Freq() / u32SensorFreq) - 1;
        if(i32Div < 0)
            i32Div = 0;
        CLK->VSENSEDIV = (CLK->VSENSEDIV & ~CLK_VSENSEDIV_VSENSEDIV_Msk) | (i32Div << CLK_VSENSEDIV_VSENSEDIV_Pos);
     * Not support in TESTCHIP_ONLY */

    /* Lock protected registers */
    SYS_LockReg();
}

int32_t PacketFormatDownScale(void)
{
    uint32_t u32Frame;

    /* Initialize NT99141 sensor and set NT99141 output YUV422 format */
    if (InitNT99141_VGA_YUV422() == FALSE) return -1;

    /* Enable External CCAP Interrupt */
    NVIC_EnableIRQ(CCAP_IRQn);

    /* Enable External CCAP Interrupt */
    CCAP_EnableInt(CCAP_INTEN_VIEN_Msk);

    /* Set Vsync polarity, Hsync polarity, pixel clock polarity, Sensor Format and Order */
    CCAP_Open(NT99141SensorPolarity, (CCAP_PAR_INDATORD_YUYV | CCAP_PAR_INFMT_YUV422), CCAP_PAR_OUTFMT_YUV422);

    /* Set Cropping Window Vertical/Horizontal Starting Address and Cropping Window Size */
    CCAP_SetCroppingWindow(0, 0, SENSOR_IN_HEIGHT, SENSOR_IN_WIDTH);

    /* Set System Memory Packet Base Address Register */
    CCAP_SetPacketBuf((uint32_t)g_au8FrameBuffer);

    /* Set Packet Scaling Vertical/Horizontal Factor Register */
    CCAP_SetPacketScaling(SYSTEM_HEIGHT, SENSOR_IN_HEIGHT, SYSTEM_WIDTH, SENSOR_IN_WIDTH);

    /* Set Packet Frame Output Pixel Stride Width */
    CCAP_SetPacketStride(SYSTEM_WIDTH);

    /* Start Image Capture Interface */
    CCAP_Start();

    u32Frame = g_u32FramePass;

    while (1)
    {
        if (g_u32FramePass != (u32Frame))
        {
            u32Frame = g_u32FramePass;
            printf("g_u32FramePass=%d\n", g_u32FramePass);
        }

        if (g_u32FramePass > 10)
            break;
    }

    /* Stop capturing images */
    CCAP_Stop(TRUE);
    return 0;
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Enable PLL0 180MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_180MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to PLL0 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0);

    /* Set HCLK2 divide 2 */
    CLK_SET_HCLK2DIV(2);

    /* Set PCLKx divide 2 */
    CLK_SET_PCLK0DIV(2);
    CLK_SET_PCLK1DIV(2);
    CLK_SET_PCLK2DIV(2);
    CLK_SET_PCLK3DIV(2);
    CLK_SET_PCLK4DIV(2);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    /* Set multi-function pins for CCAP */
    SET_CCAP_DATA7_PG2();
    SET_CCAP_DATA6_PG3();
    SET_CCAP_DATA5_PG4();
    SET_CCAP_DATA4_PF11();
    SET_CCAP_DATA3_PF10();
    SET_CCAP_DATA2_PF9();
    SET_CCAP_DATA1_PF8();
    SET_CCAP_DATA0_PF7();
    SET_CCAP_PIXCLK_PG9();
    SET_CCAP_SCLK_PG10();
    SET_CCAP_VSYNC_PG12();
    SET_CCAP_HSYNC_PG13();

    /* Lock protected registers */
    SYS_LockReg();
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t u8JpegBuffer[SYSTEM_WIDTH * SYSTEM_HEIGHT];

int32_t main(void)
{
    unsigned long u32JpegSize = sizeof(u8JpegBuffer);

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART for print message */
    InitDebugUart();

    /* Init CCAP clock and Sensor clock */
    CCAPSetFreq(12000000, 12000000);

    /* Using Packet format to Image down scale */
    if (PacketFormatDownScale() >= 0)
    {
        /* jpeg encode */
        JpegEncode(g_au8FrameBuffer, u8JpegBuffer, &u32JpegSize, SYSTEM_WIDTH, SYSTEM_HEIGHT);
    }

    /* Forces a write of all user-space buffered data for the given output */
    fflush(stdout);

    while (1);
}
