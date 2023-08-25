/******************************************************************************
* @file    ccap_cunit.c
* @version V1.00
* @brief   CCAP CUnit Test
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "ccap_cunit.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int suite_success_init(void)
{
    return 0;
}
int suite_success_clean(void)
{
    return 0;
}

CU_SuiteInfo CCAP_Suites[] =
{
    {"Test IP: CCAP", suite_success_init, suite_success_clean, NULL, NULL, CCAP_TestCases},
    CU_SUITE_INFO_NULL
};

/* Sensor VSync Polarity */
uint32_t g_au32VSP[]     = { CCAP_PAR_VSP_LOW, CCAP_PAR_VSP_HIGH };
/* Sensor HSync Polarity */
uint32_t g_au32HSP[]     = { CCAP_PAR_HSP_LOW, CCAP_PAR_HSP_HIGH };
/* Sensor Pixel Clock Polarity */
uint32_t g_au32PCLK[]    = { CCAP_PAR_PCLKP_LOW, CCAP_PAR_PCLKP_HIGH };
/* Sensor Input Type */
uint32_t g_au32SensorType[]  = { CCAP_PAR_SENTYPE_CCIR601, CCAP_PAR_SENTYPE_CCIR656 };
uint32_t g_au32InputFormat[] = { CCAP_PAR_INFMT_YUV422, CCAP_PAR_INFMT_RGB565, CCAP_PAR_INFMT_RGB888 };
uint32_t au32OutputFormat[]  =
{
    CCAP_PAR_OUTFMT_YUV422,    CCAP_PAR_OUTFMT_ONLY_Y,    CCAP_PAR_OUTFMT_RGB555,     CCAP_PAR_OUTFMT_RGB565,
    CCAP_PAR_OUTFMT_RGB888_U8, CCAP_PAR_OUTFMT_BGR888_U8, CCAP_PAR_OUTFMT_ARGB888_U8, CCAP_PAR_OUTFMT_BGRA888_U8,
    CCAP_PAR_OUTFMT_RGB888_I8, CCAP_PAR_OUTFMT_BGR888_I8, CCAP_PAR_OUTFMT_ARGB888_I8, CCAP_PAR_OUTFMT_BGRA888_I8
};
uint32_t g_au32INDATORD[]   =
{
    CCAP_PAR_INDATORD_YUYV, CCAP_PAR_INDATORD_YVYU, CCAP_PAR_INDATORD_UYVY, CCAP_PAR_INDATORD_VYUY,
    CCAP_PAR_INDATORD_RGGB, CCAP_PAR_INDATORD_BGGR, CCAP_PAR_INDATORD_GBRG, CCAP_PAR_INDATORD_GRBG,
    CCAP_PAR_INDATORD_RGB888_RGB, CCAP_PAR_INDATORD_RGB888_RBG, CCAP_PAR_INDATORD_RGB888_GRB, CCAP_PAR_INDATORD_RGB888_GBR, CCAP_PAR_INDATORD_RGB888_BRG, CCAP_PAR_INDATORD_RGB888_BGR
};
uint32_t g_au32PackFormat[] = { CCAP_CTL_PKTEN };

void TestFunc_CCAP_Open_Close()
{
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t u32SensorProp, u32InFormat, u32OutFormat;

    for (a = 0; a < sizeof(g_au32VSP) / sizeof(uint32_t); a++)
        for (b = 0; b < sizeof(g_au32HSP) / sizeof(uint32_t); b++)
            for (c = 0; c < sizeof(g_au32PCLK) / sizeof(uint32_t); c++)
                for (d = 0; d < sizeof(g_au32InputFormat) / sizeof(uint32_t); d++)
                    for (e = 0; e < sizeof(g_au32SensorType) / sizeof(uint32_t); e++)
                        for (f = 0; f < sizeof(au32OutputFormat) / sizeof(uint32_t); f++)
                            for (g = 0; g < sizeof(g_au32INDATORD) / sizeof(uint32_t); g++)
                                for (h = 0; h < sizeof(g_au32PackFormat) / sizeof(uint32_t); h++)
                                {
                                    u32SensorProp = g_au32VSP[a] | g_au32HSP[b] | g_au32PCLK[c] | g_au32SensorType[e];
                                    u32InFormat   = g_au32InputFormat[d] | g_au32INDATORD[g];
                                    u32OutFormat  = au32OutputFormat[f];

                                    CCAP_Open(u32SensorProp, u32InFormat, u32OutFormat);
                                    CU_ASSERT((CCAP->CTL & CCAP_CTL_PKTEN_Msk) == g_au32PackFormat[h]);
                                    CU_ASSERT((CCAP->PAR & (CCAP_PAR_VSP_Msk | CCAP_PAR_HSP_Msk | CCAP_PAR_PCLKP_Msk | CCAP_PAR_SENTYPE_Msk)) == u32SensorProp);

                                    CU_ASSERT((CCAP->PAR & (CCAP_PAR_INFMT_Msk | CCAP_PAR_INDATORD_Msk)) == (u32InFormat & (CCAP_PAR_INFMT_Msk | CCAP_PAR_INDATORD_Msk)));
                                    CU_ASSERT((CCAP->PARM & CCAP_PARM_INFMTH_Msk) == ((u32InFormat >> 1) & CCAP_PARM_INFMTH_Msk));

                                    if (f < 4)  // Output format not RGB888 or ARGB888
                                    {
                                        CU_ASSERT((CCAP->PARM & CCAP_PARM_OUTFMTH_Msk) == 0);
                                        CU_ASSERT((CCAP->PAR & CCAP_PAR_OUTFMT_Msk) == u32OutFormat);
                                    }
                                    else
                                    {
                                        CU_ASSERT((CCAP->PARM & CCAP_PARM_OUTFMTH_Msk) == CCAP_PARM_OUTFMTH_Msk);
                                        CU_ASSERT((CCAP->PARM & CCAP_PARM_RGB888OUTORD_Msk) == (u32OutFormat & CCAP_PARM_RGB888OUTORD_Msk));
                                        CU_ASSERT((CCAP->PAR & CCAP_PAR_OUTFMT_Msk) == (u32OutFormat & CCAP_PAR_OUTFMT_Msk));
                                    }
                                }

    CCAP_Close();
    CU_ASSERT((CCAP->CTL & CCAP_CTL_CCAPEN_Msk) == 0);
}

uint32_t g_au32IntMask[] =
{
    CCAP_INTEN_VIEN_Msk, CCAP_INTEN_MEIEN_Msk, CCAP_INTEN_ADDRMIEN_Msk, CCAP_INTEN_MDIEN_Msk
};
void TestFunc_CCAP_Function()
{

    {
        // CCAP_SetCroppingWindow START
        uint32_t i, j, k, l;
        uint32_t u32CropStart, u32CropSize;

        for (i = 0; i < 0x7FF; i += 0xF0)
            for (j = 0; j < 0x7FF; j += 0xF1)
                for (k = 0; k < 0x7FF; k += 0xF2)
                    for (l = 0; l < 0x7FF; l += 0xF3)
                    {
                        CCAP_SetCroppingWindow(i, j, k, l);
                        u32CropStart = (i << CCAP_CWSP_CWSADDRV_Pos) | j;
                        CU_ASSERT(CCAP->CWSP == u32CropStart);
                        u32CropSize = (k << CCAP_CWS_CWH_Pos) | l;
                        CU_ASSERT(CCAP->CWS == u32CropSize);
                    }
    }   // CCAP_SetCroppingWindow END

    {
        // CCAP_SetPacketBuf START
        uint32_t i;
        uint32_t au32Addr[] = { 0x00000000, 0x55555555, 0xAAAAAAAA, 0xFFFFFFFF };
        uint32_t u32FrameBuf;

        for (i = 0; i < sizeof(au32Addr) / sizeof(uint32_t); i += 1)
        {
            u32FrameBuf = au32Addr[i];
            CCAP_SetPacketBuf(u32FrameBuf);
            CU_ASSERT((CCAP->PKTBA0) == (u32FrameBuf & ~0x3));
        }
    }   // CCAP_SetPacketBuf END

    {
        // CCAP_EnableInt/CCAP_DisableInt START
        uint32_t i;

        for (i = 0; i < sizeof(g_au32IntMask) / sizeof(uint32_t); i++)
        {
            CCAP_EnableInt(g_au32IntMask[i]);
            CU_ASSERT((CCAP->INTEN & g_au32IntMask[i]) == g_au32IntMask[i]);

            CCAP_DisableInt(g_au32IntMask[i]);
            CU_ASSERT((CCAP->INTEN & g_au32IntMask[i]) == 0);
        }
    }   // CCAP_EnableInt/CCAP_DisableInt END

    {
        // CCAP_Start START
        CCAP->CTL &= ~CCAP_CTL_CCAPEN;
        CU_ASSERT((CCAP->CTL & CCAP_CTL_CCAPEN) == 0);
        CCAP_Start();
        CU_ASSERT((CCAP->CTL & CCAP_CTL_CCAPEN_Msk) == CCAP_CTL_CCAPEN);
    }   // CCAP_Start END

    {
        // CCAP_Stop START
        CCAP_Stop(FALSE);
        CU_ASSERT((CCAP->CTL & CCAP_CTL_CCAPEN) == 0);
    }   // CCAP_Stop END

    {
        // CCAP_SetPacketScaling START
        uint32_t  a, b, c, d;
        uint32_t u32Data[] = { 0x0000, 0x5555, 0xAAAA, 0xFFFF };
        uint32_t PKTSLt, PKTSMt;

        for (a = 0; a < sizeof(u32Data) / sizeof(uint32_t); a += 1)
            for (b = 0; b < sizeof(u32Data) / sizeof(uint32_t); b += 1)
                for (c = 0; c < sizeof(u32Data) / sizeof(uint32_t); c += 1)
                    for (d = 0; d < sizeof(u32Data) / sizeof(uint32_t); d += 1)
                    {
                        CCAP_SetPacketScaling(u32Data[a], u32Data[b], u32Data[c], u32Data[d]);

                        PKTSLt = (u32Data[a] & 0xFF) << 8 | (u32Data[b] & 0xFF);
                        CU_ASSERT(((CCAP->PKTSL & (CCAP_PKTSL_PKTSVNL_Msk | CCAP_PKTSL_PKTSVML_Msk)) >> CCAP_PKTSL_PKTSVML_Pos) == PKTSLt);
                        PKTSMt = ((u32Data[a] >> 8) << 8) | (u32Data[b] >> 8);
                        CU_ASSERT(((CCAP->PKTSM & (CCAP_PKTSM_PKTSVNH_Msk | CCAP_PKTSM_PKTSVMH_Msk)) >> CCAP_PKTSM_PKTSVMH_Pos) == PKTSMt);

                        PKTSLt = (u32Data[c] & 0xFF) << 8 | (u32Data[d] & 0xFF);
                        CU_ASSERT(((CCAP->PKTSL & (CCAP_PKTSL_PKTSHNL_Msk | CCAP_PKTSL_PKTSHML_Msk))) == PKTSLt);
                        PKTSMt = ((u32Data[c] >> 8) << 8) | (u32Data[d] >> 8);
                        CU_ASSERT(((CCAP->PKTSM & (CCAP_PKTSM_PKTSHNH_Msk | CCAP_PKTSM_PKTSHMH_Msk))) == PKTSMt);
                        //printf("0x%08x,0x%08x\n",CCAP->PKTSM,PKTSMt);
                    }
    }   // CCAP_SetPacketScaling END

    {
        // CCAP_SetPacketStride START
        uint32_t i;

        for (i = 0; i < 0x3FFF; i += 0x10)
        {
            CCAP_SetPacketStride(i);
            CU_ASSERT(((CCAP->STRIDE & CCAP_STRIDE_PKTSTRIDE_Msk) >> CCAP_STRIDE_PKTSTRIDE_Pos) == i);
        }
    }   // CCAP_SetPacketStride END

    {
        // CCAP_EnableMono/CCAP_DisableMono
        uint32_t i;
        uint32_t au32Interface[] = { CCAP_CTL_MY8_MY4, CCAP_CTL_MY8_MY4_SWAP, CCAP_CTL_MY8_MY8 };

        for (i = 0; i < (sizeof(au32Interface) / sizeof(uint32_t)); i++)
        {
            CCAP_EnableMono(au32Interface[i]);
            CU_ASSERT((CCAP->CTL & CCAP_CTL_MONO_Msk) == (1 << CCAP_CTL_MONO_Pos));
            CU_ASSERT((CCAP->CTL & (CCAP_CTL_MY8_MY4_Msk | CCAP_CTL_MY4_SWAP_Msk)) == au32Interface[i]);
        }

        CCAP_DisableMono();
        CU_ASSERT((CCAP->CTL & CCAP_CTL_MONO_Msk) == 0);
    }   // CCAP_EnableMono/CCAP_DisableMono END

    {
        // CCAP_EnableLumaYOne/CCAP_DisableLumaYOne
        uint32_t i;

        for (i = 0; i < 0xFF; i++)
        {
            CCAP_EnableLumaYOne(i);
            CU_ASSERT((CCAP->CTL & CCAP_CTL_Luma_Y_One_Msk) == CCAP_CTL_Luma_Y_One_Msk);
            CU_ASSERT((CCAP->LUMA_Y1_THD & CCAP_LUMA_Y1_THD_LUMA_Y1_THRESH_Msk) == i);
        }

        CCAP_DisableLumaYOne();
        CU_ASSERT((CCAP->CTL & CCAP_CTL_Luma_Y_One_Msk) == 0);
    }   // CCAP_EnableLumaYOne/CCAP_DisableLumaYOne END
}

void TestFunc_CCAP_Define()
{
    //CU_ASSERT(CCAP_CTL_ADDRSW == (1ul<<CCAP_CTL_ADDRSW_Pos));
    CU_ASSERT(CCAP_CTL_SHUTTER == (1ul << CCAP_CTL_SHUTTER_Pos));
    CU_ASSERT(CCAP_CTL_RESET   == (1ul << CCAP_CTL_VPRST_Pos));
    CU_ASSERT(CCAP_CTL_UPDATE  == (1ul << CCAP_CTL_UPDATE_Pos));

    CU_ASSERT(CCAP_INT_VIEN_ENABLE        == CCAP_INTEN_VIEN_Msk);
    CU_ASSERT(CCAP_INT_MEIEN_ENABLE       == CCAP_INTEN_MEIEN_Msk);
    CU_ASSERT(CCAP_INT_ADDRMIEN_ENABLE    == CCAP_INTEN_ADDRMIEN_Msk);
    CU_ASSERT(CCAP_INT_MDIEN_MODE1_ENABLE == (1ul << CCAP_INTEN_MDIEN_Pos));
    CU_ASSERT(CCAP_INT_MDIEN_MODE2_ENABLE == (2ul << CCAP_INTEN_MDIEN_Pos));

    CU_ASSERT(CCAP_INTSTS_MDINTF_MODE1_Msk == (1ul << CCAP_INTSTS_MDINTF_Pos));
    CU_ASSERT(CCAP_INTSTS_MDINTF_MODE2_Msk == (2ul << CCAP_INTSTS_MDINTF_Pos));

    CU_ASSERT(CCAP_MD_CELL_WIDTH     == 80ul);
    CU_ASSERT(CCAP_MD_CELL_HEIGHT    == 60ul);
    CU_ASSERT(CCAP_MD_WIDTH          == 320ul);
    CU_ASSERT(CCAP_MD_HEIGHT         == 240ul);
    CU_ASSERT(CCAP_MD_WINDOW_CNT     == 16ul);
    CU_ASSERT(CCAP_MD_MAX_WINDOW_SAD == 0x12AD4ul);
    CU_ASSERT(CCAP_MD_MAX_TOTAL_SAD  == 0x12AD40ul);
}

uint32_t u32IntFlagMask[] =
{
    CCAP_INTSTS_VINTF_Msk, CCAP_INTSTS_MEINTF_Msk, CCAP_INTSTS_ADDRMINTF_Msk, CCAP_INTSTS_MDINTF_MODE1_Msk, CCAP_INTSTS_MDINTF_MODE2_Msk
};

void TestFunc_CCAP_Macro()
{
    int32_t i, j;

    CCAP->CTL = CCAP->CTL | CCAP_CTL_CCAPEN_Msk;
    CU_ASSERT(CCAP_IS_STOPPED() == 0);

    CCAP->CTL = CCAP->CTL & ~CCAP_CTL_CCAPEN_Msk;
    CU_ASSERT(CCAP_IS_STOPPED() == 1);

    {
        // CCAP_CLR_INT_FLAG START
        for (i = 0; i < sizeof(u32IntFlagMask) / sizeof(uint32_t); i++)
        {
            CCAP_CLR_INT_FLAG(u32IntFlagMask[i]);
            CU_ASSERT((CCAP->INTSTS & u32IntFlagMask[i]) == 0);
        }
    }   // CCAP_CLR_INT_FLAG END

    {
        // CCAP_GET_INT_STS START
        CU_ASSERT((CCAP_GET_INT_STS() == CCAP->INTSTS));
    }   // CCAP_GET_INT_STS END

    {
        // CCAP_MD_ENABLE_WINDOW/CCAP_MD_DISABLE_WINDOW START
        CCAP_MD_DISABLE_WINDOW(0xFFFF);
        CU_ASSERT_EQUAL((uint32_t)CCAP->MDCTL, 0);

        for (i = 0; i < 16; i++)
        {
            CCAP_MD_ENABLE_WINDOW(1 << i);
            CU_ASSERT_EQUAL((uint32_t)CCAP->MDCTL, 1 << i);
            CCAP_MD_DISABLE_WINDOW(1 << i);
            CU_ASSERT_EQUAL((uint32_t)CCAP->MDCTL, 0);
        }
    }   // CCAP_MD_ENABLE_WINDOW/CCAP_MD_DISABLE_WINDOW END

    {
        // CCAP_MD_SET_TRIG_SRC START

    }   // CCAP_MD_SET_TRIG_SRC END

    {
        // CCAP_MD_SET_WIN_THRESHOLD START
        for (i = 0; i < 16; i++)
        {
            for (j = 0; j < 0x12AD4 + 255; j += 33)
            {
                CCAP_MD_SET_WIN_THRESHOLD(i, j);
                CU_ASSERT_EQUAL((uint32_t)CCAP->MDWTH[i], j);
            }
        }
    }   // CCAP_MD_SET_WIN_THRESHOLD END

    {
        // CCAP_MD_SET_TOTAL_THRESHOLD START
        for (i = 0; i < 0x12AD40 + 255; i += 33)
        {
            CCAP_MD_SET_TOTAL_THRESHOLD(i);
            CU_ASSERT_EQUAL((uint32_t)CCAP->MDTTH, i);
        }
    }   // CCAP_MD_SET_TOTAL_THRESHOLD END

    {
        // CCAP_FR_SET_SCALEDOWN_RATE/CCAP_FR_GET_SCALEDOWN_RATE
        for (i = 0; i <= 63; i++)
        {
            for (j = i; j <= 63; j++)
            {
                CCAP_FR_SET_SCALEDOWN_RATE(i, j);
                CU_ASSERT_EQUAL(CCAP_FR_GET_SCALEDOWN_RATE(), ((i * 100) / j));
            }
        }
    }
}

uint32_t u32Freq[]      = { 0, 1, 2, 3  };
uint32_t u32BlockSize[] = { TRUE, FALSE };
uint32_t u32Format[]    = { TRUE, FALSE };

/*
 * (0,  0)----(80,  0)----(160,  0)----(240,  0)---------
 * |           |            |            |            |
 * |     0     |      1     |      2     |      3     |
 * |           |            |            |            |
 * (0, 60)----(80, 60)----(160, 60)----(240, 60)---------
 * |           |            |            |            |
 * |     4     |      5     |      6     |      7     |
 * |           |            |            |            |
 * (0,120)----(80,120)----(160,120)----(240,120)---------
 * |           |            |            |            |
 * |     8     |      9     |     10     |     11     |
 * |           |            |            |            |
 * (0,180)----(80,180)----(160,180)----(240,180)---------
 * |           |            |            |            |
 * |    12     |     13     |     14     |     15     |
 * |           |            |            |            |
 * -----------------------------------------------(319,239)
 */
void TestFunc_CCAP_MotionDetection()
{
    int32_t  i, j;
    int32_t  x, y, w, h;
    uint32_t u32WinMask = 0, u32StartX, u32StartY;

    for (i = 0; i < 16; i++)
    {
        for (j = 0; j <= 100; j++)
        {
            u32WinMask = (1 << i);
            x  = (80 * (i % 4));
            y  = (60 * (i / 4));

            CCAP_MD_DISABLE_WINDOW(0xFFFF);
            CCAP_MD_SetRegionSensitivity(y, x, 60, 80, j);
            CU_ASSERT_EQUAL((uint32_t)CCAP->MDCTL, u32WinMask);
            CU_ASSERT_EQUAL(((uint32_t)CCAP->MDCTL & 0xFFFF0000), 0);
            CU_ASSERT_EQUAL(CCAP->MDWTH[i], CCAP_MD_MAX_WINDOW_SAD - ((j * CCAP_MD_MAX_WINDOW_SAD) / 100));

            if ((uint32_t)CCAP->MDCTL != u32WinMask)
            {
                printf("(%d, %d) - (%d, %d) \n", x, y, x + 80, y + 60);
                printf("CCAP->MDCTL: 0x%08X, u32WinMask: 0x%08X\n", CCAP->MDCTL, u32WinMask);
            }
        }
    }

    for (j = 0; j <= 100; j++)
    {
        CCAP_MD_SetGlobalSensitivity(j);
        CU_ASSERT_EQUAL(CCAP->MDTTH, CCAP_MD_MAX_TOTAL_SAD - ((j * CCAP_MD_MAX_TOTAL_SAD) / 100));
    }

    CCAP_MD_SET_TRIG_SRC(CCAP_MD_TRIG_LPTMR0, TRUE);
    CU_ASSERT_EQUAL(((uint32_t)CCAP->MDTRG_WK & CCAP_MDTRG_WK_TRGSEL_Msk), CCAP_MD_TRIG_LPTMR0);
    CCAP_MD_SET_TRIG_SRC(CCAP_MD_TRIG_LPTMR1, TRUE);
    CU_ASSERT_EQUAL(((uint32_t)CCAP->MDTRG_WK & CCAP_MDTRG_WK_TRGSEL_Msk), CCAP_MD_TRIG_LPTMR1);
    CCAP_MD_SET_TRIG_SRC(CCAP_MD_TRIG_TTMR0, TRUE);
    CU_ASSERT_EQUAL(((uint32_t)CCAP->MDTRG_WK & CCAP_MDTRG_WK_TRGSEL_Msk), CCAP_MD_TRIG_TTMR0);
    CCAP_MD_SET_TRIG_SRC(CCAP_MD_TRIG_TTMR1, TRUE);
    CU_ASSERT_EQUAL(((uint32_t)CCAP->MDTRG_WK & CCAP_MDTRG_WK_TRGSEL_Msk), CCAP_MD_TRIG_TTMR1);

    for (i = 0; i < 16; i++)
    {
        CCAP_MD_SET_OVERFLOW_WIN_THRESHOLD(i);
        CU_ASSERT_EQUAL(CCAP->MDWOCTH, i);
    }
}

CU_TestInfo CCAP_TestCases[] =
{
    { "CCAP Open_Close Function", TestFunc_CCAP_Open_Close },
    { "CCAP Function",            TestFunc_CCAP_Function },
    { "CCAP Define",              TestFunc_CCAP_Define },
    { "CCAP Macro",               TestFunc_CCAP_Macro },
    { "CCAP MotionDetection",     TestFunc_CCAP_MotionDetection },
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/