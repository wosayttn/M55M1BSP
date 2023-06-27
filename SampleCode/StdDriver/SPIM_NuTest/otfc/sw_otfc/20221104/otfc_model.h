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
#ifndef _SW_OTFC_MODEL_H_
#define _SW_OTFC_MODEL_H_

#include <stdint.h>                           /* Include standard types */
//#include "utility.h"
//#include "Tube.h"

//SYSTEM ADDRESS
#define SYS_BA             0x40000000
#define SYS_PSWCTL         0x1F4
#define PROT_BA            0x40000100  //SYSTEM unlock ADDRESS
#define CLK_BA             0x40001000
#define AHBCLK             0x04
//OTFC ADDR
#define OTFC_BA0            0x40040000
#define OTFC_BA1            0x40041000
#define OTFC_CTL            0x000
#define OTFC_STS            0x004
#define OTFC_PR0_SADDR      0x010
#define OTFC_PR0_DADDR      0x014
#define OTFC_PR0_KSCTL      0x018
#define OTFC_PR0_KEY0       0x020
#define OTFC_PR0_KEY1       0x024
#define OTFC_PR0_KEY2       0x028
#define OTFC_PR0_KEY3       0x02C
#define OTFC_PR0_KEY4       0x030
#define OTFC_PR0_KEY5       0x034
#define OTFC_PR0_KEY6       0x038
#define OTFC_PR0_KEY7       0x03C
#define OTFC_PR0_NON0       0x040
#define OTFC_PR0_NON1       0x044
#define OTFC_PR0_NON2       0x048
#define OTFC_PR0_NON3       0x04C
#define OTFC_PR1_SADDR      0x050
#define OTFC_PR1_DADDR      0x054
#define OTFC_PR1_KSCTL      0x058
#define OTFC_PR1_KEY0       0x060
#define OTFC_PR1_KEY1       0x064
#define OTFC_PR1_KEY2       0x068
#define OTFC_PR1_KEY3       0x06C
#define OTFC_PR1_KEY4       0x070
#define OTFC_PR1_KEY5       0x074
#define OTFC_PR1_KEY6       0x078
#define OTFC_PR1_KEY7       0x07C
#define OTFC_PR1_NON0       0x080
#define OTFC_PR1_NON1       0x084
#define OTFC_PR1_NON2       0x088
#define OTFC_PR1_NON3       0x08C
#define OTFC_PR2_SADDR      0x090
#define OTFC_PR2_DADDR      0x094
#define OTFC_PR2_KSCTL      0x098
#define OTFC_PR2_KEY0       0x0A0
#define OTFC_PR2_KEY1       0x0A4
#define OTFC_PR2_KEY2       0x0A8
#define OTFC_PR2_KEY3       0x0AC
#define OTFC_PR2_KEY4       0x0B0
#define OTFC_PR2_KEY5       0x0B4
#define OTFC_PR2_KEY6       0x0B8
#define OTFC_PR2_KEY7       0x0BC
#define OTFC_PR2_NON0       0x0C0
#define OTFC_PR2_NON1       0x0C4
#define OTFC_PR2_NON2       0x0C8
#define OTFC_PR2_NON3       0x0CC
#define OTFC_PR3_SADDR      0x0D0
#define OTFC_PR3_DADDR      0x0D4
#define OTFC_PR3_KSCTL      0x0D8
#define OTFC_PR3_KEY0       0x0E0
#define OTFC_PR3_KEY1       0x0E4
#define OTFC_PR3_KEY2       0x0E8
#define OTFC_PR3_KEY3       0x0EC
#define OTFC_PR3_KEY4       0x0F0
#define OTFC_PR3_KEY5       0x0F4
#define OTFC_PR3_KEY6       0x0F8
#define OTFC_PR3_KEY7       0x0FC
#define OTFC_PR3_NON0       0x100
#define OTFC_PR3_NON1       0x104
#define OTFC_PR3_NON2       0x108
#define OTFC_PR3_NON3       0x10C

//CRYPTO ADDRESS
#define CRPT_SRAM_BASE     0x20200000       //SRAM address for AES
#define CRPT_BA            0x4020A000       //CRYPTO base address - sec
#define CRPT_BA_NS         0x5020A000       //CRYPTO base address - non-sec
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
#define CRPT_ECC_X1_0      0x808
#define CRPT_ECC_X1_1      0x80C
#define CRPT_ECC_X1_2      0x810
#define CRPT_ECC_X1_3      0x814
#define CRPT_ECC_X1_4      0x818
#define CRPT_ECC_X1_5      0x81C
#define CRPT_ECC_X1_6      0x820


//KeyStore ADDRESS
#define   KS_BA           0x40220000
#define   KS_CTL          0x00
#define   KS_METADATA     0x04
#define   KS_STS          0x08
#define   KS_REMAINING    0x0C
#define   KS_SCMBKEY0     0x10
#define   KS_SCMBKEY1     0x14
#define   KS_SCMBKEY2     0x18
#define   KS_SCMBKEY3     0x1C
#define   KS_KEY0         0x20
#define   KS_KEY1         0x24
#define   KS_KEY2         0x28
#define   KS_KEY3         0x2C
#define   KS_KEY4         0x30
#define   KS_KEY5         0x34
#define   KS_KEY6         0x38
#define   KS_KEY7         0x3C
#define   KS_OTP_STS      0x40
#define   KS_REMKEYCNT    0x44
#define   KS_VERSION      0xFFC

//------------------------------------------------------------------------------
// Software OTFC Model Define Function Prototypes
//------------------------------------------------------------------------------
uint32_t GetOTFCAnsKey(uint8_t u8Index);
//void otfc_model(uint32_t key0, uint32_t key1, uint32_t key2, uint32_t key3,
//                uint32_t data0, uint32_t data1, uint32_t data2, uint32_t data3, uint32_t addr);
void otfc_model(uint32_t pr_no, uint32_t key0,  uint32_t key1,  uint32_t key2,  uint32_t key3,
                uint32_t data0, uint32_t data1, uint32_t data2, uint32_t data3, uint32_t addr);
int otfc_sw_check_result(uint32_t addr, uint32_t data, int err);

int data_swap(uint32_t input);

#endif /* _SW_OTFC_MODEL_H_ */
