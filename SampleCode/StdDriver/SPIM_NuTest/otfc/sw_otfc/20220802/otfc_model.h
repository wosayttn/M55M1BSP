#ifndef _SW_OTFC_MODEL_H_
#define _SW_OTFC_MODEL_H_

// *****************************************************************************
// * nuvoTon CONFIDENTIAL                                                      *
// *                         PROPRIETARY NOTE                                  *
// *                                                                           *
// *  This file contains information confidential and proprietary              *
// *  to nuvoTon Technology Corporation.  It shall not be reproduced in        *
// *  whole or in part, or transferred to other documents, or disclosed        *
// *  to third parties, or used for any purpose other than that for            *
// *  which it was obtained, without the prior written consent of nuvoTon      *
// *  Technology Corporation.  (c) 2010, nuvoTon Technology Corporation        *
// *  All rights reserved                                                      *
// *                                                                           *
// *****************************************************************************

// *****************************************************************************
// * File       : $Id: otfc.h $
// * Version    : $Revision: 1 $
// * Author     : $Author: kywu $
// * Reviser    :
// * Date       :
// * Description:
// *
// *****************************************************************************
#include <stdio.h>                           /* Include standard types */
#include <stdint.h>                           /* Include standard types */
//#include "utility.h"

//SYSTEM ADDRESS
#define SYS_BA             0x40000000
#define SYS_PSWCTL         0x1F4
#define PROT_BA            0x40000100  //SYSTEM unlock ADDRESS
#define CLK_BA             0x40000200
#define AHBCLK             0x04
//OTFC ADDR
#define OTF_CTL            0xC00
#define OTF_STS            0xC04
#define OTF_PR0_SADDR      0xC10
#define OTF_PR0_DADDR      0xC14
#define OTF_PR0_KSCTL      0xC18
#define OTF_PR0_KEY0       0xC20
#define OTF_PR0_KEY1       0xC24
#define OTF_PR0_KEY2       0xC28
#define OTF_PR0_KEY3       0xC2C
#define OTF_PR0_KEY4       0xC30
#define OTF_PR0_KEY5       0xC34
#define OTF_PR0_KEY6       0xC38
#define OTF_PR0_KEY7       0xC3C
#define OTF_PR0_NON0       0xC40
#define OTF_PR0_NON1       0xC44
#define OTF_PR0_NON2       0xC48
#define OTF_PR0_NON3       0xC4C
#define OTF_PR1_SADDR      0xC50
#define OTF_PR1_DADDR      0xC54
#define OTF_PR1_KSCTL      0xC58
#define OTF_PR1_KEY0       0xC60
#define OTF_PR1_KEY1       0xC64
#define OTF_PR1_KEY2       0xC68
#define OTF_PR1_KEY3       0xC6C
#define OTF_PR1_KEY4       0xC70
#define OTF_PR1_KEY5       0xC74
#define OTF_PR1_KEY6       0xC78
#define OTF_PR1_KEY7       0xC7C
#define OTF_PR1_NON0       0xC80
#define OTF_PR1_NON1       0xC84
#define OTF_PR1_NON2       0xC88
#define OTF_PR1_NON3       0xC8C
#define OTF_PR2_SADDR      0xC90
#define OTF_PR2_DADDR      0xC94
#define OTF_PR2_KSCTL      0xC98
#define OTF_PR2_KEY0       0xCA0
#define OTF_PR2_KEY1       0xCA4
#define OTF_PR2_KEY2       0xCA8
#define OTF_PR2_KEY3       0xCAC
#define OTF_PR2_KEY4       0xCB0
#define OTF_PR2_KEY5       0xCB4
#define OTF_PR2_KEY6       0xCB8
#define OTF_PR2_KEY7       0xCBC
#define OTF_PR2_NON0       0xCC0
#define OTF_PR2_NON1       0xCC4
#define OTF_PR2_NON2       0xCC8
#define OTF_PR2_NON3       0xCCC
#define OTF_PR3_SADDR      0xCD0
#define OTF_PR3_DADDR      0xCD4
#define OTF_PR3_KSCTL      0xCD8
#define OTF_PR3_KEY0       0xCE0
#define OTF_PR3_KEY1       0xCE4
#define OTF_PR3_KEY2       0xCE8
#define OTF_PR3_KEY3       0xCEC
#define OTF_PR3_KEY4       0xCF0
#define OTF_PR3_KEY5       0xCF4
#define OTF_PR3_KEY6       0xCF8
#define OTF_PR3_KEY7       0xCFC
#define OTF_PR3_NON0       0xD00
#define OTF_PR3_NON1       0xD04
#define OTF_PR3_NON2       0xD08
#define OTF_PR3_NON3       0xD0C
//Only for OTFC TEST
#define OTF_TCTL           0xD10
#define OTF_TSTS           0xD14
#define OTF_TADDR0         0xD18
#define OTF_TADDR1         0xD1C
#define OTF_TANS0_0        0xD20
#define OTF_TANS0_1        0xD24
#define OTF_TANS0_2        0xD28
#define OTF_TANS0_3        0xD2C
#define OTF_TANS1_0        0xD30
#define OTF_TANS1_1        0xD34
#define OTF_TANS1_2        0xD38
#define OTF_TANS1_3        0xD3C
//CRYPTO ADDRESS
#define CRPT_SRAM_BASE     0x20008000       //SRAM address for AES
#define CRPT_BA            0x40001000       //CRYPTO base address
#define CRPT_AES_KEY0      0x110
#define CRPT_AES_KEY1      0x114
#define CRPT_AES_KEY2      0x118
#define CRPT_AES_KEY3      0x11C
#define CRPT_AES_SADDR     0x140
#define CRPT_AES_DADDR     0x144
#define CRPT_AES_CNT       0x148
#define CRPT_AES_CTL       0x100
#define CRPT_AES_STS       0x104
#define CRPT_PRNG_CTL      0x008
#define CRPT_PRNG_SEED     0x00C
#define CRPT_PRNG_KEY0     0x010
#define CRPT_PRNG_KEY1     0x014
#define CRPT_PRNG_KEY2     0x018
#define CRPT_PRNG_KEY3     0x01C
#define CRPT_PRNG_KEY4     0x020
#define CRPT_PRNG_KEY5     0x024
#define CRPT_PRNG_KEY6     0x028
#define CRPT_PRNG_KEY7     0x02C

//------------------------------------------------------------------------------
// Software OTFC Model Define Function Prototypes
//------------------------------------------------------------------------------
uint32_t GetOTFCAnsKey(uint8_t u8Index);
void otfc_model(uint32_t key0, uint32_t key1, uint32_t key2, uint32_t key3,
                uint32_t data0, uint32_t data1, uint32_t data2, uint32_t data3, uint32_t addr);
int otfc_sw_check_result(uint32_t addr, uint32_t data, int err);

#endif /* _SW_OTFC_MODEL_H_ */
