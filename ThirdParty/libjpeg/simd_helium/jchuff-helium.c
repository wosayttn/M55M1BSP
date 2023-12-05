/*
 * jchuff-neon.c - Huffman entropy encoding (32-bit Arm Neon)
 *
 * Copyright (C) 2020, Arm Limited.  All Rights Reserved.
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
 *
 * NOTE: All referenced figures are from
 * Recommendation ITU-T T.81 (1992) | ISO/IEC 10918-1:1994.
 */


 /**************************************************************************//**
 * @file     jchuff-helium.c
 * @version  V1.00
 * @brief    Porting from jpegturbo ARM32 simd jchuff file.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdlib.h>
#include <inttypes.h>
#include <arm_mve.h>
#include <stdio.h>
#include <string.h>
#include "timer_simple_platform.h"
#include "jpeglib.h"
#include "NuMicro.h"
#include "nvt_jpeg.h"
#include "jpegint.h"
#define DCTSIZE             8   /* The basic DCT block is 8x8 samples */
#define DCTSIZE2            64  /* DCTSIZE squared; # of elements in a block */
#define NUM_ARITH_TBLS      16  /* Arith-coding tables are numbered 0..15 */
#define MAX_COMPS_IN_SCAN   4   /* JPEG limit on # of components in one scan */
#define BUILTIN_CLZ  __builtin_clz
typedef unsigned char JOCTET;
typedef short JCOEF;
typedef JCOEF *JCOEFPTR;        /* useful in a couple of places */


typedef struct jpeg_common_struct *j_common_ptr;

#define EMIT_BYTE(b) { \
  buffer[0] = (JOCTET)(b); \
  buffer[1] = 0; \
  buffer -= -2 + ((JOCTET)(b) < 0xFF); \
}

/* Output the entire bit buffer.  If there are no 0xFF bytes in it, then write
 * directly to the output buffer.  Otherwise, use the EMIT_BYTE() macro to
 * encode 0xFF as 0xFF 0x00.
 */
#if defined(__aarch64__) || defined(_M_ARM64)

#define FLUSH() { \
  if (put_buffer & 0x8080808080808080 & ~(put_buffer + 0x0101010101010101)) { \
    EMIT_BYTE(put_buffer >> 56) \
    EMIT_BYTE(put_buffer >> 48) \
    EMIT_BYTE(put_buffer >> 40) \
    EMIT_BYTE(put_buffer >> 32) \
    EMIT_BYTE(put_buffer >> 24) \
    EMIT_BYTE(put_buffer >> 16) \
    EMIT_BYTE(put_buffer >>  8) \
    EMIT_BYTE(put_buffer      ) \
  } else { \
    *((uint64_t *)buffer) = BUILTIN_BSWAP64(put_buffer); \
    buffer += 8; \
  } \
}

#else

#if defined(_MSC_VER) && !defined(__clang__)
#define SPLAT() { \
  buffer[0] = (JOCTET)(put_buffer >> 24); \
  buffer[1] = (JOCTET)(put_buffer >> 16); \
  buffer[2] = (JOCTET)(put_buffer >>  8); \
  buffer[3] = (JOCTET)(put_buffer      ); \
  buffer += 4; \
}
#else
#define SPLAT() { \
  put_buffer = __builtin_bswap32(put_buffer); \
  __asm__("str %1, [%0], #4" : "+r" (buffer) : "r" (put_buffer)); \
}
#endif

#define FLUSH() { \
  if (put_buffer & 0x80808080 & ~(put_buffer + 0x01010101)) { \
    EMIT_BYTE(put_buffer >> 24) \
    EMIT_BYTE(put_buffer >> 16) \
    EMIT_BYTE(put_buffer >>  8) \
    EMIT_BYTE(put_buffer      ) \
  } else { \
    SPLAT(); \
  } \
}

#endif

/* Fill the bit buffer to capacity with the leading bits from code, then output
 * the bit buffer and put the remaining bits from code into the bit buffer.
 */
#define MY_BIT_BUF_SIZE 32
#define PUT_AND_FLUSH(code, size) { \
  put_buffer = (put_buffer << (size + free_bits)) | (code >> -free_bits); \
  FLUSH() \
  free_bits += MY_BIT_BUF_SIZE; \
  put_buffer = code; \
}

/* Insert code into the bit buffer and output the bit buffer if needed.
 * NOTE: We can't flush with free_bits == 0, since the left shift in
 * PUT_AND_FLUSH() would have undefined behavior.
 */
#define PUT_BITS(code, size) { \
  free_bits -= size; \
  if (free_bits < 0) \
    PUT_AND_FLUSH(code, size) \
  else \
    put_buffer = (put_buffer << size) | code; \
}

#define PUT_CODE(code, size, diff) { \
  diff |= code << nbits; \
  nbits += size; \
  PUT_BITS(diff, nbits); \
}



#define BUFSIZE  (DCTSIZE2 * 8)
#define LOAD_BUFFER() { \
  if (state->free_in_buffer < BUFSIZE) { \
    localbuf = 1; \
    buffer = _buffer; \
  } else \
    buffer = state->next_output_byte; \
}

#define STORE_BUFFER() { \
  if (localbuf) { \
    size_t bytes, bytestocopy; \
    bytes = buffer - _buffer; \
    buffer = _buffer; \
    while (bytes > 0) { \
      bytestocopy = MIN(bytes, state->free_in_buffer); \
      memcpy(state->next_output_byte, buffer, bytestocopy); \
      state->next_output_byte += bytestocopy; \
      buffer += bytestocopy; \
      state->free_in_buffer -= bytestocopy; \
      if (state->free_in_buffer == 0) \
        if (!dump_buffer(state)) return FALSE; \
      bytes -= bytestocopy; \
    } \
  } else { \
    state->free_in_buffer -= (buffer - state->next_output_byte); \
    state->next_output_byte = buffer; \
  } \


//Copy from jchuff.c to solve include error
typedef unsigned char JOCTET;

#if defined(__x86_64__) && defined(__ILP32__)
typedef unsigned long long bit_buf_type;
#else
typedef size_t bit_buf_type;
#endif

#if defined(WITH_SIMD) && !(defined(__arm__) || defined(__aarch64__) || \
                            defined(_M_ARM) || defined(_M_ARM64))
typedef unsigned long long simd_bit_buf_type;
#else
typedef bit_buf_type simd_bit_buf_type;
#endif

JOCTET *jsimd_huff_encode_one_block_helium(void *state, JOCTET *buffer,
        JCOEFPTR block, int last_dc_val,
        c_derived_tbl *dctbl,
        c_derived_tbl *actbl)
{
    uint8_t block_nbits[DCTSIZE2];
    uint16_t block_diff[DCTSIZE2];
    uint64_t tick_cnt;
    int16_t * ps16_block = (int16_t*)(block);
    /* Load rows of coefficients from DCT block in zig-zag order. */

    /* Compute DC coefficient difference value. (F.1.1.5.1) */
    int16x8_t row0 = vdupq_n_s16(ps16_block[0] - last_dc_val);
    row0 = vsetq_lane_s16(ps16_block[1], row0, 1);
    row0 = vsetq_lane_s16(ps16_block[8], row0, 2);
    row0 = vsetq_lane_s16(ps16_block[16], row0, 3);
    row0 = vsetq_lane_s16(ps16_block[9], row0, 4);
    row0 = vsetq_lane_s16(block[2], row0, 5);
    row0 = vsetq_lane_s16(block[3], row0, 6);
    row0 = vsetq_lane_s16(block[10], row0, 7);

    int16x8_t row1 = vdupq_n_s16(ps16_block[0] + 17);
    row1 = vsetq_lane_s16(ps16_block[24], row1, 1);
    row1 = vsetq_lane_s16(ps16_block[32], row1, 2);
    row1 = vsetq_lane_s16(ps16_block[25], row1, 3);
    row1 = vsetq_lane_s16(ps16_block[18], row1, 4);
    row1 = vsetq_lane_s16(ps16_block[11], row1, 5);
    row1 = vsetq_lane_s16(ps16_block[  4], row1, 6);
    row1 = vsetq_lane_s16(ps16_block[  5], row1, 7);

    int16x8_t row2 = vdupq_n_s16(ps16_block[0] + 12);
    row2 = vsetq_lane_s16(ps16_block[19], row2, 1);
    row2 = vsetq_lane_s16(ps16_block[26], row2, 2);
    row2 = vsetq_lane_s16(ps16_block[33], row2, 3);
    row2 = vsetq_lane_s16(ps16_block[40], row2, 4);
    row2 = vsetq_lane_s16(ps16_block[48], row2, 5);
    row2 = vsetq_lane_s16(ps16_block[41], row2, 6);
    row2 = vsetq_lane_s16(ps16_block[34], row2, 7);

    int16x8_t row3 = vdupq_n_s16(ps16_block[0] + 27);
    row3 = vsetq_lane_s16(ps16_block[20], row3, 1);
    row3 = vsetq_lane_s16(ps16_block[13], row3, 2);
    row3 = vsetq_lane_s16(ps16_block[ 6], row3, 3);
    row3 = vsetq_lane_s16(ps16_block[7], row3, 4);
    row3 = vsetq_lane_s16(ps16_block[14], row3, 5);
    row3 = vsetq_lane_s16(ps16_block[21], row3, 6);
    row3 = vsetq_lane_s16(ps16_block[28], row3, 7);

    int16x8_t abs_row0 = vabsq_s16(row0);
    int16x8_t abs_row1 = vabsq_s16(row1);
    int16x8_t abs_row2 = vabsq_s16(row2);
    int16x8_t abs_row3 = vabsq_s16(row3);

    int16x8_t row0_lz = vclzq_s16(abs_row0);
    int16x8_t row1_lz = vclzq_s16(abs_row1);
    int16x8_t row2_lz = vclzq_s16(abs_row2);
    int16x8_t row3_lz = vclzq_s16(abs_row3);

    /* Compute number of bits required to represent each coefficient. */
    uint16x8_t row0_nbits = vsubq_u16(vdupq_n_s16(16), vreinterpretq_u16_s16(row0_lz));
    uint16x8_t row1_nbits = vsubq_u16(vdupq_n_s16(16), vreinterpretq_u16_s16(row1_lz));
    uint16x8_t row2_nbits = vsubq_u16(vdupq_n_s16(16), vreinterpretq_u16_s16(row2_lz));
    uint16x8_t row3_nbits = vsubq_u16(vdupq_n_s16(16), vreinterpretq_u16_s16(row3_lz));

    vstrbq_u16(block_nbits + 0 * DCTSIZE, row0_nbits);
    vstrbq_u16(block_nbits + 1 * DCTSIZE, row1_nbits);
    vstrbq_u16(block_nbits + 2 * DCTSIZE, row2_nbits);
    vstrbq_u16(block_nbits + 3 * DCTSIZE, row3_nbits);

    uint16x8_t row0_mask =
        vshlq_u16(vreinterpretq_u16_s16(vshrq_n_s16(row0, 15)),
                  vnegq_s16(row0_lz));
    uint16x8_t row1_mask =
        vshlq_u16(vreinterpretq_u16_s16(vshrq_n_s16(row1, 15)),
                  vnegq_s16(row1_lz));
    uint16x8_t row2_mask =
        vshlq_u16(vreinterpretq_u16_s16(vshrq_n_s16(row2, 15)),
                  vnegq_s16(row2_lz));
    uint16x8_t row3_mask =
        vshlq_u16(vreinterpretq_u16_s16(vshrq_n_s16(row3, 15)),
                  vnegq_s16(row3_lz));

    uint16x8_t row0_diff = veorq_u16(vreinterpretq_u16_s16(abs_row0), row0_mask);
    uint16x8_t row1_diff = veorq_u16(vreinterpretq_u16_s16(abs_row1), row1_mask);
    uint16x8_t row2_diff = veorq_u16(vreinterpretq_u16_s16(abs_row2), row2_mask);
    uint16x8_t row3_diff = veorq_u16(vreinterpretq_u16_s16(abs_row3), row3_mask);

    /* Store diff values for rows 0, 1, 2, and 3. */
    vstrbq_u16((uint8_t*)(block_diff + 0 * DCTSIZE), row0_diff);
    vstrbq_u16((uint8_t*)(block_diff + 1 * DCTSIZE), row1_diff);
    vstrbq_u16((uint8_t*)(block_diff + 2 * DCTSIZE), row2_diff);
    vstrbq_u16((uint8_t*)(block_diff + 3 * DCTSIZE), row3_diff);


    /* Load last four rows of coefficients from DCT block in zig-zag order. */
    int16x8_t row4 = vdupq_n_s16(ps16_block[0] + 35);
    row4 = vsetq_lane_s16(ps16_block[42], row4, 1);
    row4 = vsetq_lane_s16(ps16_block[49], row4, 2);
    row4 = vsetq_lane_s16(ps16_block[56], row4, 3);
    row4 = vsetq_lane_s16(ps16_block[57], row4, 4);
    row4 = vsetq_lane_s16(ps16_block[50], row4, 5);
    row4 = vsetq_lane_s16(ps16_block[43], row4, 6);
    row4 = vsetq_lane_s16(ps16_block[36], row4, 7);

    int16x8_t row5 = vdupq_n_s16(ps16_block[29]);
    row5 = vsetq_lane_s16(ps16_block[29], row5, 1);
    row5 = vsetq_lane_s16(ps16_block[15], row5, 2);
    row5 = vsetq_lane_s16(ps16_block[23], row5, 3);
    row5 = vsetq_lane_s16(ps16_block[30], row5, 4);
    row5 = vsetq_lane_s16(ps16_block[37], row5, 5);
    row5 = vsetq_lane_s16(ps16_block[44], row5, 6);
    row5 = vsetq_lane_s16(ps16_block[51], row5, 7);

    int16x8_t row6 = vdupq_n_s16(ps16_block[58]);
    row6 = vsetq_lane_s16(ps16_block[59], row6, 1);
    row6 = vsetq_lane_s16(ps16_block[52], row6, 2);
    row6 = vsetq_lane_s16(ps16_block[45], row6, 3);
    row6 = vsetq_lane_s16(ps16_block[38], row6, 4);
    row6 = vsetq_lane_s16(ps16_block[31], row6, 5);
    row6 = vsetq_lane_s16(ps16_block[39], row6, 6);
    row6 = vsetq_lane_s16(ps16_block[46], row6, 7);

    int16x8_t row7 = vdupq_n_s16(ps16_block[53]);
    row7 = vsetq_lane_s16(ps16_block[60], row7, 1);
    row7 = vsetq_lane_s16(ps16_block[61], row7, 2);
    row7 = vsetq_lane_s16(ps16_block[54], row7, 3);
    row7 = vsetq_lane_s16(ps16_block[47], row7, 4);
    row7 = vsetq_lane_s16(ps16_block[55], row7, 5);
    row7 = vsetq_lane_s16(ps16_block[62], row7, 6);
    row7 = vsetq_lane_s16(ps16_block[63], row7, 7);

    int16x8_t abs_row4 = vabsq_s16(row4);
    int16x8_t abs_row5 = vabsq_s16(row5);
    int16x8_t abs_row6 = vabsq_s16(row6);
    int16x8_t abs_row7 = vabsq_s16(row7);

    int16x8_t row4_lz = vclzq_s16(abs_row4);
    int16x8_t row5_lz = vclzq_s16(abs_row5);
    int16x8_t row6_lz = vclzq_s16(abs_row6);
    int16x8_t row7_lz = vclzq_s16(abs_row7);

    /* Compute number of bits required to represent each coefficient. */
    uint16x8_t row4_nbits = vsubq_u16(vdupq_n_s16(16), vreinterpretq_u16_s16(row4_lz));
    uint16x8_t row5_nbits = vsubq_u16(vdupq_n_s16(16), vreinterpretq_u16_s16(row5_lz));
    uint16x8_t row6_nbits = vsubq_u16(vdupq_n_s16(16), vreinterpretq_u16_s16(row6_lz));
    uint16x8_t row7_nbits = vsubq_u16(vdupq_n_s16(16), vreinterpretq_u16_s16(row7_lz));

    vstrbq_u16(block_nbits + 4 * DCTSIZE, row4_nbits);
    vstrbq_u16(block_nbits + 5 * DCTSIZE, row5_nbits);
    vstrbq_u16(block_nbits + 6 * DCTSIZE, row6_nbits);
    vstrbq_u16(block_nbits + 7 * DCTSIZE, row7_nbits);

    uint16x8_t row4_mask =
        vshlq_u16(vreinterpretq_u16_s16(vshrq_n_s16(row4, 15)),
                  vnegq_s16(row4_lz));
    uint16x8_t row5_mask =
        vshlq_u16(vreinterpretq_u16_s16(vshrq_n_s16(row5, 15)),
                  vnegq_s16(row5_lz));
    uint16x8_t row6_mask =
        vshlq_u16(vreinterpretq_u16_s16(vshrq_n_s16(row6, 15)),
                  vnegq_s16(row6_lz));
    uint16x8_t row7_mask =
        vshlq_u16(vreinterpretq_u16_s16(vshrq_n_s16(row7, 15)),
                  vnegq_s16(row7_lz));

    uint16x8_t row4_diff = veorq_u16(vreinterpretq_u16_s16(abs_row4), row4_mask);
    uint16x8_t row5_diff = veorq_u16(vreinterpretq_u16_s16(abs_row5), row5_mask);
    uint16x8_t row6_diff = veorq_u16(vreinterpretq_u16_s16(abs_row6), row6_mask);
    uint16x8_t row7_diff = veorq_u16(vreinterpretq_u16_s16(abs_row7), row7_mask);

    /* Store diff values for rows 4, 5, 6, and 7. */
    vst1q_u16(block_diff + 4 * DCTSIZE, row4_diff);
    vst1q_u16(block_diff + 5 * DCTSIZE, row5_diff);
    vst1q_u16(block_diff + 6 * DCTSIZE, row6_diff);
    vst1q_u16(block_diff + 7 * DCTSIZE, row7_diff);

    /* Construct bitmap to accelerate encoding of AC coefficients.  A set bit
     * means that the corresponding coefficient != 0.
     */
    uint16x8_t  row0_nbits_gt0 = vandq_u16(row0_nbits, vdupq_n_u16(0));
    uint16x8_t  row1_nbits_gt0 = vandq_u16(row1_nbits, vdupq_n_u16(0));
    uint16x8_t  row2_nbits_gt0 = vandq_u16(row2_nbits, vdupq_n_u16(0));
    uint16x8_t  row3_nbits_gt0 = vandq_u16(row3_nbits, vdupq_n_u16(0));
    uint16x8_t  row4_nbits_gt0 = vandq_u16(row4_nbits, vdupq_n_u16(0));
    uint16x8_t  row5_nbits_gt0 = vandq_u16(row5_nbits, vdupq_n_u16(0));
    uint16x8_t  row6_nbits_gt0 = vandq_u16(row6_nbits, vdupq_n_u16(0));
    uint16x8_t  row7_nbits_gt0 = vandq_u16(row7_nbits, vdupq_n_u16(0));

    uint16x8_t bitmap_mask;
   
    bitmap_mask = vsetq_lane_u16(0x01, bitmap_mask, 0);
    bitmap_mask = vsetq_lane_u16(0x02, bitmap_mask, 1);
    bitmap_mask = vsetq_lane_u16(0x04, bitmap_mask, 2);
    bitmap_mask = vsetq_lane_u16(0x08, bitmap_mask, 3);
    bitmap_mask = vsetq_lane_u16(0x10, bitmap_mask, 4);
    bitmap_mask = vsetq_lane_u16(0x20, bitmap_mask, 5);
    bitmap_mask = vsetq_lane_u16(0x40, bitmap_mask, 6);
    bitmap_mask = vsetq_lane_u16(0x80, bitmap_mask, 7);

    row0_nbits_gt0 = vandq_u16(row0_nbits_gt0, bitmap_mask);
    row1_nbits_gt0 = vandq_u16(row1_nbits_gt0, bitmap_mask);
    row2_nbits_gt0 = vandq_u16(row2_nbits_gt0, bitmap_mask);
    row3_nbits_gt0 = vandq_u16(row3_nbits_gt0, bitmap_mask);
    row4_nbits_gt0 = vandq_u16(row4_nbits_gt0, bitmap_mask);
    row5_nbits_gt0 = vandq_u16(row5_nbits_gt0, bitmap_mask);
    row6_nbits_gt0 = vandq_u16(row6_nbits_gt0, bitmap_mask);
    row7_nbits_gt0 = vandq_u16(row7_nbits_gt0, bitmap_mask);

    uint16x8_t bitmap_rows_10 = vaddq_u16(row1_nbits_gt0, row0_nbits_gt0);
    uint16x8_t bitmap_rows_32 = vaddq_u16(row3_nbits_gt0, row2_nbits_gt0);
    uint16x8_t bitmap_rows_54 = vaddq_u16(row5_nbits_gt0, row4_nbits_gt0);
    uint16x8_t bitmap_rows_76 = vaddq_u16(row7_nbits_gt0, row6_nbits_gt0);
    uint16x8_t bitmap_rows_3210 = vaddq_u16(bitmap_rows_32, bitmap_rows_10);
    uint16x8_t bitmap_rows_7654 = vaddq_u16(bitmap_rows_76, bitmap_rows_54);
    uint16x8_t bitmap = vaddq_u16(bitmap_rows_7654, bitmap_rows_3210);

    /* Shift left to remove DC bit. */
//bitmap = vreinterpretq_u16_u64(vshlq_n_u64(vreinterpretq_u64_u16(bitmap), 1));

    bitmap = vqrshlq_n_u16(bitmap, 1);


    /* Move bitmap to 32-bit scalar registers. */
    uint32_t bitmap_1_32 = vgetq_lane_u32(vreinterpretq_u32_u16(bitmap), 1);
    uint32_t bitmap_33_63 = vgetq_lane_u32(vreinterpretq_u32_u16(bitmap), 0);

    /* Set up state and bit buffer for output bitstream. */
    working_state_simd *state_ptr = (working_state_simd *)state;
    int free_bits = state_ptr->cur.free_bits;
    INT32 put_buffer = state_ptr->cur.put_buffer;
    //size_t put_buffer = state_ptr->cur.put_buffer.simd;

    /* Encode DC coefficient. */

    unsigned int nbits = block_nbits[0];
    /* Emit Huffman-coded symbol and additional diff bits. */
    unsigned int diff = block_diff[0];
    PUT_CODE(dctbl->ehufco[nbits], dctbl->ehufsi[nbits], diff);

    /* Encode AC coefficients. */

    unsigned int r = 0;  /* r = run length of zeros */
    unsigned int i = 1;  /* i = number of coefficients encoded */
    /* Code and size information for a run length of 16 zero coefficients */
    const unsigned int code_0xf0 = actbl->ehufco[0xf0];
    const unsigned int size_0xf0 = actbl->ehufsi[0xf0];

    while (bitmap_1_32 != 0) {
        r = BUILTIN_CLZ(bitmap_1_32);
        i += r;
        bitmap_1_32 <<= r;
        nbits = block_nbits[i];
        diff = block_diff[i];
        while (r > 15) {
            /* If run length > 15, emit special run-length-16 codes. */
            PUT_BITS(code_0xf0, size_0xf0);
            r -= 16;
        }
        /* Emit Huffman symbol for run length / number of bits. (F.1.2.2.1) */
        unsigned int rs = (r << 4) + nbits;
        PUT_CODE(actbl->ehufco[rs], actbl->ehufsi[rs], diff);
        i++;
        bitmap_1_32 <<= 1;
    }

    r = 33 - i;
    i = 33;

    while (bitmap_33_63 != 0) {
        unsigned int leading_zeros = BUILTIN_CLZ(bitmap_33_63);
        r += leading_zeros;
        i += leading_zeros;
        bitmap_33_63 <<= leading_zeros;
        nbits = block_nbits[i];
        diff = block_diff[i];
        while (r > 15) {
            /* If run length > 15, emit special run-length-16 codes. */
            PUT_BITS(code_0xf0, size_0xf0);
            r -= 16;
        }
        /* Emit Huffman symbol for run length / number of bits. (F.1.2.2.1) */
        unsigned int rs = (r << 4) + nbits;
        PUT_CODE(actbl->ehufco[rs], actbl->ehufsi[rs], diff);
        r = 0;
        i++;
        bitmap_33_63 <<= 1;
    }

    /* If the last coefficient(s) were zero, emit an end-of-block (EOB) code.
     * The value of RS for the EOB code is 0.
     */
    if (i != 64) {
        PUT_BITS(actbl->ehufco[0], actbl->ehufsi[0]);
    }

    state_ptr->cur.put_buffer = put_buffer;
    state_ptr->cur.free_bits = free_bits;
    //tick_cnt = Get_SysTick_Cycle_Count_Export();
    //printf("\n jchuff  helium tick Counters are:%lld!\n", tick_cnt);
    return buffer;
}

LOCAL(boolean)
dump_buffer(working_state_simd *state)
/* Empty the output buffer; return TRUE if successful, FALSE if must suspend */
{
    struct jpeg_destination_mgr *dest = state->cinfo->dest;

    if (!(*dest->empty_output_buffer) (state->cinfo))
        return FALSE;
    /* After a successful buffer dump, must reset buffer pointers */
    state->next_output_byte = dest->next_output_byte;
    state->free_in_buffer = dest->free_in_buffer;
    return TRUE;
}
}

boolean encode_one_block_simd(working_state_simd *state, JCOEFPTR block, int last_dc_val,
                              c_derived_tbl *dctbl, c_derived_tbl *actbl)
{
    JOCTET _buffer[BUFSIZE], *buffer;
    int localbuf = 0;

    LOAD_BUFFER()

    buffer = jsimd_huff_encode_one_block_helium(state, buffer, block, last_dc_val,
             dctbl, actbl);

    STORE_BUFFER()

    return TRUE;
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
