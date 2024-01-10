/*
 * jfdctint-neon.c - accurate integer FDCT (Arm Neon)
 *
 * Copyright (C) 2020, Arm Limited.  All Rights Reserved.
 * Copyright (C) 2020, D. R. Commander.  All Rights Reserved.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */
/**************************************************************************//**
* @file     jfdctinit-helium.c
* @version  V1.00
* @brief    Porting from jpegturbo ARM32 simd jfdctinit file.
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <arm_mve.h>
#include "nvt_jpeg.h"

/* jsimd_fdct_islow_neon() performs a slower but more accurate forward DCT
 * (Discrete Cosine Transform) on one block of samples.  It uses the same
 * calculations and produces exactly the same output as IJG's original
 * jpeg_fdct_islow() function, which can be found in jfdctint.c.
 *
 * Scaled integer constants are used to avoid floating-point arithmetic:
 *    0.298631336 =  2446 * 2^-13
 *    0.390180644 =  3196 * 2^-13
 *    0.541196100 =  4433 * 2^-13
 *    0.765366865 =  6270 * 2^-13
 *    0.899976223 =  7373 * 2^-13
 *    1.175875602 =  9633 * 2^-13
 *    1.501321110 = 12299 * 2^-13
 *    1.847759065 = 15137 * 2^-13
 *    1.961570560 = 16069 * 2^-13
 *    2.053119869 = 16819 * 2^-13
 *    2.562915447 = 20995 * 2^-13
 *    3.072711026 = 25172 * 2^-13
 *
 * See jfdctint.c for further details of the DCT algorithm.  Where possible,
 * the variable names and comments here in jsimd_fdct_islow_neon() match up
 * with those in jpeg_fdct_islow().
 */

#define CONST_BITS  13
#define PASS1_BITS  2
#define HELIUM_CORR_BITS  1

#define DESCALE_P1  (CONST_BITS - PASS1_BITS +1)
#define DESCALE_P2  (CONST_BITS + PASS1_BITS)

#define F_0_298  2446
#define F_0_390  3196
#define F_0_541  4433
#define F_0_765  6270
#define F_0_899  7373
#define F_1_175  9633
#define F_1_501  12299
#define F_1_847  15137
#define F_1_961  16069
#define F_2_053  16819
#define F_2_562  20995
#define F_3_072  25172


static const int16_t jsimd_fdct_islow_helium_consts[] ALIGNMENT_ATTRIBUTE= {
    F_0_298, -F_0_390,  F_0_541,  F_0_765,
    -F_0_899,  F_1_175,  F_1_501, -F_1_847,
    -F_1_961,  F_2_053, -F_2_562,  F_3_072
};

void jsimd_fdct_islow_helium(int16_t *data)
{

    //Vector
    uint16x8_t index;
    int16x8_t col0;
    int16x8_t col1;
    int16x8_t col2;
    int16x8_t col3;
    int16x8_t col4;
    int16x8_t col5;
    int16x8_t col6;
    int16x8_t col7;

    int32x4_t vmlal_buf;

    /* Load DCT constants. */
    const int16x8_t consts_v0_0 = vdupq_n_s16(jsimd_fdct_islow_helium_consts[0]);
    const int16x8_t consts_v0_1 = vdupq_n_s16(jsimd_fdct_islow_helium_consts[1]);
    const int16x8_t consts_v0_2 = vdupq_n_s16(jsimd_fdct_islow_helium_consts[2]);
    const int16x8_t consts_v0_3 = vdupq_n_s16(jsimd_fdct_islow_helium_consts[3]);

    const int16x8_t consts_v1_0 = vdupq_n_s16(jsimd_fdct_islow_helium_consts[4]);
    const int16x8_t consts_v1_1 = vdupq_n_s16(jsimd_fdct_islow_helium_consts[5]);
    const int16x8_t consts_v1_2 = vdupq_n_s16(jsimd_fdct_islow_helium_consts[6]);
    const int16x8_t consts_v1_3 = vdupq_n_s16(jsimd_fdct_islow_helium_consts[7]);

    const int16x8_t consts_v2_0 = vdupq_n_s16(jsimd_fdct_islow_helium_consts[8]);
    const int16x8_t consts_v2_1 = vdupq_n_s16(jsimd_fdct_islow_helium_consts[9]);
    const int16x8_t consts_v2_2 = vdupq_n_s16(jsimd_fdct_islow_helium_consts[10]);
    const int16x8_t consts_v2_3 = vdupq_n_s16(jsimd_fdct_islow_helium_consts[11]);

    /* Load DCT unsigned->signed conversion constants. */
    const int16x8_t consts_us2s = vdupq_n_s16(8 * CENTERJSAMPLE);


    //Create Offset
    index = vidupq_n_u16(0, 2);//
    index = vmulq_n_u16(index, 8);

    col0 = vldrhq_gather_offset_s16((int16_t*)(data), index);
    col1 = vldrhq_gather_offset_s16((int16_t*)(data+1), index);
    col2 = vldrhq_gather_offset_s16((int16_t*)(data+2), index);
    col3 = vldrhq_gather_offset_s16((int16_t*)(data+3), index);
    col4 = vldrhq_gather_offset_s16((int16_t*)(data+4), index);
    col5 = vldrhq_gather_offset_s16((int16_t*)(data+5), index);
    col6 = vldrhq_gather_offset_s16((int16_t*)(data+6), index);
    col7 = vldrhq_gather_offset_s16((int16_t*)(data+7), index);
    /* Pass 1: process rows. */

    int16x8_t tmp0 = vaddq_s16(col0, col7);
    int16x8_t tmp7 = vsubq_s16(col0, col7);
    int16x8_t tmp1 = vaddq_s16(col1, col6);
    int16x8_t tmp6 = vsubq_s16(col1, col6);
    int16x8_t tmp2 = vaddq_s16(col2, col5);
    int16x8_t tmp5 = vsubq_s16(col2, col5);
    int16x8_t tmp3 = vaddq_s16(col3, col4);
    int16x8_t tmp4 = vsubq_s16(col3, col4);

    /* Even part */
    int16x8_t tmp10 = vaddq_s16(tmp0, tmp3);
    int16x8_t tmp13 = vsubq_s16(tmp0, tmp3);
    int16x8_t tmp11 = vaddq_s16(tmp1, tmp2);
    int16x8_t tmp12 = vsubq_s16(tmp1, tmp2);

    /* Apply unsigned->signed conversion. */
    col0 = vshlq_n_s16(vsubq_s16(vaddq_s16(tmp10, tmp11), consts_us2s), PASS1_BITS);

    col4 = vshlq_n_s16(vsubq_s16(tmp10, tmp11), PASS1_BITS);

    int16x8_t tmp12_add_tmp13 = vaddq_s16(tmp12, tmp13);

    int32x4_t z1_l =
        vqdmullbq_s16(tmp12_add_tmp13, consts_v0_2);
    int32x4_t z1_h =
        vqdmulltq_s16(tmp12_add_tmp13,consts_v0_2);

    int32x4_t col2_scaled_l = vqdmullbq_s16(tmp13, consts_v0_3);

    col2_scaled_l = vaddq_s32(col2_scaled_l, z1_l);

    int32x4_t col2_scaled_h = vqdmulltq_s16(tmp13, consts_v0_3);

    col2_scaled_h = vaddq_s32(col2_scaled_h, z1_h);

    col2 = vqshrnbq_n_s32(col2, col2_scaled_l, DESCALE_P1),
    col2 = vqshrntq_n_s32(col2, col2_scaled_h, DESCALE_P1);

    int32x4_t col6_scaled_l =  vqdmullbq_s16(tmp12,consts_v1_3);
    int32x4_t col6_scaled_h = vqdmulltq_s16(tmp12, consts_v1_3);

    col6_scaled_l = vaddq_s32(col6_scaled_l, z1_l);
    col6_scaled_h = vaddq_s32(col6_scaled_h, z1_h);

    col6 = vqshrnbq_n_s32(col6, col6_scaled_l, DESCALE_P1),
    col6 = vqshrntq_n_s32(col6, col6_scaled_h, DESCALE_P1);

    /* Odd part */
    int16x8_t z1 = vaddq_s16(tmp4, tmp7);
    int16x8_t z2 = vaddq_s16(tmp5, tmp6);
    int16x8_t z3 = vaddq_s16(tmp4, tmp6);
    int16x8_t z4 = vaddq_s16(tmp5, tmp7);

    /* sqrt(2) * c3 */
    int32x4_t z5_l = vqdmullbq_s16(z3, consts_v1_1);
    int32x4_t z5_h = vqdmulltq_s16(z3, consts_v1_1);

    vmlal_buf = vqdmullbq_s16(z4, consts_v1_1);
    z5_l = vaddq_s32(z5_l, vmlal_buf );

    vmlal_buf = vqdmulltq_s16(z4, consts_v1_1);
    z5_h = vaddq_s32(z5_h, vmlal_buf );


    /* sqrt(2) * (-c1+c3+c5-c7) */
    int32x4_t tmp4_l = vqdmullbq_s16((tmp4), consts_v0_0);
    int32x4_t tmp4_h = vqdmulltq_s16((tmp4), consts_v0_0);
    /* sqrt(2) * ( c1+c3-c5+c7) */
    int32x4_t tmp5_l = vqdmullbq_s16((tmp5), consts_v2_1);
    int32x4_t tmp5_h = vqdmulltq_s16((tmp5), consts_v2_1);
    /* sqrt(2) * ( c1+c3+c5-c7) */
    int32x4_t tmp6_l = vqdmullbq_s16((tmp6), consts_v2_3);
    int32x4_t tmp6_h = vqdmulltq_s16((tmp6), consts_v2_3);
    /* sqrt(2) * ( c1+c3-c5-c7) */
    int32x4_t tmp7_l = vqdmullbq_s16((tmp7), consts_v1_2);
    int32x4_t tmp7_h = vqdmulltq_s16((tmp7), consts_v1_2);

    /* sqrt(2) * (c7-c3) */
    z1_l = vqdmullbq_s16((z1), consts_v1_0);
    z1_h = vqdmulltq_s16((z1), consts_v1_0);
    /* sqrt(2) * (-c1-c3) */
    int32x4_t z2_l = vqdmullbq_s16((z2), consts_v2_2);
    int32x4_t z2_h = vqdmulltq_s16((z2), consts_v2_2);
    /* sqrt(2) * (-c3-c5) */
    int32x4_t z3_l = vqdmullbq_s16((z3), consts_v2_0);
    int32x4_t z3_h = vqdmulltq_s16((z3), consts_v2_0);
    /* sqrt(2) * (c5-c3) */
    int32x4_t z4_l = vqdmullbq_s16((z4), consts_v0_1);
    int32x4_t z4_h = vqdmulltq_s16((z4), consts_v0_1);

    z3_l = vaddq_s32(z3_l, z5_l);
    z3_h = vaddq_s32(z3_h, z5_h);
    z4_l = vaddq_s32(z4_l, z5_l);
    z4_h = vaddq_s32(z4_h, z5_h);

    tmp4_l = vaddq_s32(tmp4_l, z1_l);
    tmp4_h = vaddq_s32(tmp4_h, z1_h);
    tmp4_l = vaddq_s32(tmp4_l, z3_l);
    tmp4_h = vaddq_s32(tmp4_h, z3_h);

    col7 = vqshrnbq_n_s32(col7, tmp4_l, DESCALE_P1),
    col7 = vqshrntq_n_s32(col7, tmp4_h, DESCALE_P1);

    tmp5_l = vaddq_s32(tmp5_l, z2_l);
    tmp5_h = vaddq_s32(tmp5_h, z2_h);
    tmp5_l = vaddq_s32(tmp5_l, z4_l);
    tmp5_h = vaddq_s32(tmp5_h, z4_h);
    col5 = vqshrnbq_n_s32(col5, tmp5_l, DESCALE_P1),
    col5 = vqshrntq_n_s32(col5, tmp5_h, DESCALE_P1);

    tmp6_l = vaddq_s32(tmp6_l, z2_l);
    tmp6_h = vaddq_s32(tmp6_h, z2_h);
    tmp6_l = vaddq_s32(tmp6_l, z3_l);
    tmp6_h = vaddq_s32(tmp6_h, z3_h);
    col3 = vqshrnbq_n_s32(col3, tmp6_l, DESCALE_P1),
    col3 = vqshrntq_n_s32(col3, tmp6_h, DESCALE_P1);

    tmp7_l = vaddq_s32(tmp7_l, z1_l);
    tmp7_h = vaddq_s32(tmp7_h, z1_h);
    tmp7_l = vaddq_s32(tmp7_l, z4_l);
    tmp7_h = vaddq_s32(tmp7_h, z4_h);
    col1 = vqshrnbq_n_s32(col1, tmp7_l, DESCALE_P1),
    col1 = vqshrntq_n_s32(col1, tmp7_h, DESCALE_P1);

    /* Transpose to work on columns in pass 2. */
    vst1q_s16((int16_t*)(data), col0);
    vst1q_s16((int16_t*)(data+8), col1);
    vst1q_s16((int16_t*)(data+16), col2);
    vst1q_s16((int16_t*)(data+24), col3);
    vst1q_s16((int16_t*)(data+32), col4);
    vst1q_s16((int16_t*)(data+40), col5);
    vst1q_s16((int16_t*)(data+48), col6);
    vst1q_s16((int16_t*)(data+56), col7);

    index = vidupq_n_u16(0, 2);//
    index = vmulq_n_u16(index, 8);

    int16x8_t row0 =  vldrhq_gather_offset_s16((int16_t*)(data), index);
    int16x8_t row1 =  vldrhq_gather_offset_s16((int16_t*)(data+1), index);
    int16x8_t row2 =  vldrhq_gather_offset_s16((int16_t*)(data+2), index);
    int16x8_t row3 =  vldrhq_gather_offset_s16((int16_t*)(data+3), index);
    int16x8_t row4 =  vldrhq_gather_offset_s16((int16_t*)(data+4), index);
    int16x8_t row5 =  vldrhq_gather_offset_s16((int16_t*)(data+5), index);
    int16x8_t row6 =  vldrhq_gather_offset_s16((int16_t*)(data+6), index);
    int16x8_t row7 =  vldrhq_gather_offset_s16((int16_t*)(data+7), index);

    /* Pass 2: process columns. */
    tmp0 = vaddq_s16(row0, row7);
    tmp7 = vsubq_s16(row0, row7);
    tmp1 = vaddq_s16(row1, row6);
    tmp6 = vsubq_s16(row1, row6);
    tmp2 = vaddq_s16(row2, row5);
    tmp5 = vsubq_s16(row2, row5);
    tmp3 = vaddq_s16(row3, row4);
    tmp4 = vsubq_s16(row3, row4);

    /* Even part */
    tmp10 = vaddq_s16(tmp0, tmp3);
    tmp13 = vsubq_s16(tmp0, tmp3);
    tmp11 = vaddq_s16(tmp1, tmp2);
    tmp12 = vsubq_s16(tmp1, tmp2);

    row0 = vrshrq_n_s16(vaddq_s16(tmp10, tmp11), PASS1_BITS);
    row4 = vrshrq_n_s16(vsubq_s16(tmp10, tmp11), PASS1_BITS);

    tmp12_add_tmp13 = vaddq_s16(tmp12, tmp13);
    z1_l = vqdmullbq_s16((tmp12_add_tmp13), consts_v0_2);
    z1_h = vqdmulltq_s16((tmp12_add_tmp13), consts_v0_2);



    int32x4_t row2_scaled_l =
        vqdmullbq_s16((tmp13), consts_v0_3);

    row2_scaled_l = vaddq_s32(row2_scaled_l, z1_l);

    int32x4_t row2_scaled_h =
        vqdmulltq_s16((tmp13), consts_v0_3);

    row2_scaled_h = vaddq_s32(row2_scaled_h, z1_h);

    row2 = vqshrnbq_n_s32(row2, row2_scaled_l, DESCALE_P2),
    row2 = vqshrntq_n_s32(row2, row2_scaled_h, DESCALE_P2);


    //Migrate from Neon to Helium, code size will double.
    int32x4_t row6_scaled_l =
        vqdmullbq_s16((tmp12), consts_v1_3);

    row6_scaled_l = vaddq_s32(row6_scaled_l, z1_l);

    int32x4_t row6_scaled_h =
        vqdmulltq_s16((tmp12), consts_v1_3);

    row6_scaled_h = vaddq_s32(row6_scaled_h, z1_h);

    row6 = vqshrnbq_n_s32(row6, row6_scaled_l, DESCALE_P2),
    row6 = vqshrntq_n_s32(row6, row6_scaled_h, DESCALE_P2);

    /* Odd part */
    z1 = vaddq_s16(tmp4, tmp7);
    z2 = vaddq_s16(tmp5, tmp6);
    z3 = vaddq_s16(tmp4, tmp6);
    z4 = vaddq_s16(tmp5, tmp7);
    /* sqrt(2) * c3 */
    z5_l = vqdmullbq_s16((z3), consts_v1_1);
    z5_h = vqdmulltq_s16((z3), consts_v1_1);

    vmlal_buf =  vqdmullbq_s16((z4), consts_v1_1);
    z5_l = vaddq_s32(vmlal_buf, z5_l);

    vmlal_buf =  vqdmulltq_s16((z4), consts_v1_1);
    z5_h = vaddq_s32(vmlal_buf, z5_h);


    /* sqrt(2) * (-c1+c3+c5-c7) */
    tmp4_l = vqdmullbq_s16((tmp4), consts_v0_0);
    tmp4_h = vqdmulltq_s16((tmp4), consts_v0_0);
    /* sqrt(2) * ( c1+c3-c5+c7) */
    tmp5_l = vqdmullbq_s16((tmp5), consts_v2_1);
    tmp5_h = vqdmulltq_s16((tmp5), consts_v2_1);
    /* sqrt(2) * ( c1+c3+c5-c7) */
    tmp6_l = vqdmullbq_s16((tmp6), consts_v2_3);
    tmp6_h = vqdmulltq_s16((tmp6), consts_v2_3);
    /* sqrt(2) * ( c1+c3-c5-c7) */
    tmp7_l = vqdmullbq_s16((tmp7), consts_v1_2);
    tmp7_h = vqdmulltq_s16((tmp7), consts_v1_2);

    /* sqrt(2) * (c7-c3) */
    z1_l = vqdmullbq_s16((z1), consts_v1_0);
    z1_h = vqdmulltq_s16((z1), consts_v1_0);
    /* sqrt(2) * (-c1-c3) */
    z2_l = vqdmullbq_s16((z2), consts_v2_2);
    z2_h = vqdmulltq_s16((z2), consts_v2_2);
    /* sqrt(2) * (-c3-c5) */
    z3_l = vqdmullbq_s16((z3), consts_v2_0);
    z3_h = vqdmulltq_s16((z3), consts_v2_0);
    /* sqrt(2) * (c5-c3) */
    z4_l = vqdmullbq_s16((z4), consts_v0_1);
    z4_h = vqdmulltq_s16((z4), consts_v0_1);

    z3_l = vaddq_s32(z3_l, z5_l);
    z3_h = vaddq_s32(z3_h, z5_h);
    z4_l = vaddq_s32(z4_l, z5_l);
    z4_h = vaddq_s32(z4_h, z5_h);

    tmp4_l = vaddq_s32(tmp4_l, z1_l);
    tmp4_h = vaddq_s32(tmp4_h, z1_h);
    tmp4_l = vaddq_s32(tmp4_l, z3_l);
    tmp4_h = vaddq_s32(tmp4_h, z3_h);

    row7 = vqshrnbq_n_s32(row7, tmp4_l, DESCALE_P2);
    row7 = vqshrntq_n_s32(row7, tmp4_h, DESCALE_P2);


    tmp5_l = vaddq_s32(tmp5_l, z2_l);
    tmp5_h = vaddq_s32(tmp5_h, z2_h);
    tmp5_l = vaddq_s32(tmp5_l, z4_l);
    tmp5_h = vaddq_s32(tmp5_h, z4_h);

    row5 = vqshrnbq_n_s32(row5, tmp5_l, DESCALE_P2);
    row5 = vqshrntq_n_s32(row5, tmp5_h, DESCALE_P2);

    tmp6_l = vaddq_s32(tmp6_l, z2_l);
    tmp6_h = vaddq_s32(tmp6_h, z2_h);
    tmp6_l = vaddq_s32(tmp6_l, z3_l);
    tmp6_h = vaddq_s32(tmp6_h, z3_h);

    row3 = vqshrnbq_n_s32(row3, tmp6_l, DESCALE_P2);
    row3 = vqshrntq_n_s32(row3, tmp6_h, DESCALE_P2);

    tmp7_l = vaddq_s32(tmp7_l, z1_l);
    tmp7_h = vaddq_s32(tmp7_h, z1_h);
    tmp7_l = vaddq_s32(tmp7_l, z4_l);
    tmp7_h = vaddq_s32(tmp7_h, z4_h);

    row1 = vqshrnbq_n_s32(row1, tmp7_l, DESCALE_P2);
    row1 = vqshrntq_n_s32(row1, tmp7_h, DESCALE_P2);

    row1 = vrshrq_n_s16(row1, HELIUM_CORR_BITS);
    row2 = vrshrq_n_s16(row2, HELIUM_CORR_BITS);
    row3 = vrshrq_n_s16(row3, HELIUM_CORR_BITS);
    row5 = vrshrq_n_s16(row5, HELIUM_CORR_BITS);
    row6 = vrshrq_n_s16(row6, HELIUM_CORR_BITS);
    row7 = vrshrq_n_s16(row7, HELIUM_CORR_BITS);

    vst1q_s16(data + 0 * DCTSIZE, row0);
    vst1q_s16(data + 1 * DCTSIZE, row1);
    vst1q_s16(data + 2 * DCTSIZE, row2);
    vst1q_s16(data + 3 * DCTSIZE, row3);
    vst1q_s16(data + 4 * DCTSIZE, row4);
    vst1q_s16(data + 5 * DCTSIZE, row5);
    vst1q_s16(data + 6 * DCTSIZE, row6);
    vst1q_s16(data + 7 * DCTSIZE, row7);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
