/**************************************************************************//**
 * @file     nvt_jpeg.cpp
 * @version  V1.00
 * @brief    Libjpeg simd porting related code
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include <stdio.h>
#include <arm_mve.h>
#include "jacclib.h"

/****************************************************************************
 * User Define Macro
 ****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/

/****************************************************************************
 * Function Proto Type
 ****************************************************************************/

/****************************************************************************
 * User Code
 ****************************************************************************/
int flss(uint16_t val)
{
    int bit;

    bit = 16;

    if (!val)
        return 0;

    if (!(val & 0xff00))
    {
        bit -= 8;
        val <<= 8;
    }

    if (!(val & 0xf000))
    {
        bit -= 4;
        val <<= 4;
    }

    if (!(val & 0xc000))
    {
        bit -= 2;
        val <<= 2;
    }

    if (!(val & 0x8000))
    {
        bit -= 1;
        val <<= 1;
    }

    return bit;
}

///*
// * Compute values to do a division using reciprocal.
// *
// * This implementation is based on an algorithm described in
// *   "How to optimize for the Pentium family of microprocessors"
// *   (http://www.agner.org/assem/).
// * More information about the basic algorithm can be found in
// * the paper "Integer Division Using Reciprocals" by Robert Alverson.
// *
// * The basic idea is to replace x/d by x * d^-1. In order to store
// * d^-1 with enough precision we shift it left a few places. It turns
// * out that this algoright gives just enough precision, and also fits
// * into DCTELEM:
// *
// *   b = (the number of significant bits in divisor) - 1
// *   r = (word size) + b
// *   f = 2^r / divisor
// *
// * f will not be an integer for most cases, so we need to compensate
// * for the rounding error introduced:
// *
// *   no fractional part:
// *
// *       result = input >> r
// *
// *   fractional part of f < 0.5:
// *
// *       round f down to nearest integer
// *       result = ((input + 1) * f) >> r
// *
// *   fractional part of f > 0.5:
// *
// *       round f up to nearest integer
// *       result = (input * f) >> r
// *
// * This is the original algorithm that gives truncated results. But we
// * want properly rounded results, so we replace "input" with
// * "input + divisor/2".
// *
// * In order to allow SIMD implementations we also tweak the values to
// * allow the same calculation to be made at all times:
// *
// *   dctbl[0] = f rounded to nearest integer
// *   dctbl[1] = divisor / 2 (+ 1 if fractional part of f < 0.5)
// *   dctbl[2] = 1 << ((word size) * 2 - r)
// *   dctbl[3] = r - (word size)
// *
// * dctbl[2] is for stupid instruction sets where the shift operation
// * isn't member wise (e.g. MMX).
// *
// * The reason dctbl[2] and dctbl[3] reduce the shift with (word size)
// * is that most SIMD implementations have a "multiply and store top
// * half" operation.
// *
// * Lastly, we store each of the values in their own table instead
// * of in a consecutive manner, yet again in order to allow SIMD
// * routines.
// */
int compute_reciprocal(uint16_t divisor, DCTELEM *dtbl)
{
    UDCTELEM2 fq, fr;
    UDCTELEM c;
    int b, r;

    if (divisor == 1)
    {
        /* divisor == 1 means unquantized, so these reciprocal/correction/shift
         * values will cause the C quantization algorithm to act like the
         * identity function.  Since only the C quantization algorithm is used in
         * these cases, the scale value is irrelevant.
         */
        dtbl[DCTSIZE2 * 0] = (DCTELEM)1;                        /* reciprocal */
        dtbl[DCTSIZE2 * 1] = (DCTELEM)0;                        /* correction */
        dtbl[DCTSIZE2 * 2] = (DCTELEM)1;                        /* scale */
        dtbl[DCTSIZE2 * 3] = -(DCTELEM)(sizeof(DCTELEM) * 8);   /* shift */
        return 0;
    }

    b = flss(divisor) - 1;
    r  = sizeof(DCTELEM) * 8 + b;

    fq = ((UDCTELEM2)1 << r) / divisor;
    fr = ((UDCTELEM2)1 << r) % divisor;

    c = divisor / 2;                      /* for rounding */

    if (fr == 0)                          /* divisor is power of two */
    {
        /* fq will be one bit too large to fit in DCTELEM, so adjust */
        fq >>= 1;
        r--;
    }
    else if (fr <= (divisor / 2U))        /* fractional part is < 0.5 */
    {
        c++;
    }
    else                                  /* fractional part is > 0.5 */
    {
        fq++;
    }

    dtbl[DCTSIZE2 * 0] = (DCTELEM)fq;     /* reciprocal */
    dtbl[DCTSIZE2 * 1] = (DCTELEM)c;      /* correction + roundfactor */
#ifdef WITH_SIMD
    dtbl[DCTSIZE2 * 2] = (DCTELEM)(1 << (sizeof(DCTELEM) * 8 * 2 - r)); /* scale */
#else
    dtbl[DCTSIZE2 * 2] = 1;
#endif
    dtbl[DCTSIZE2 * 3] = (DCTELEM)r - sizeof(DCTELEM) * 8; /* shift */

    if (r <= 16) return 0;
    else return 1;
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
