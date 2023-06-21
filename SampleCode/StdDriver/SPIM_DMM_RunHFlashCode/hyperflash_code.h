/**************************************************************************//**
 * @file        hyperflash_code.h
 * @version     V3.00
 * @brief       HyperFlash device driver
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#ifndef __HYPER_FLASH_CODE_H__
#define __HYPER_FLASH_CODE_H__

//------------------------------------------------------------------------------
// HyperFlash Operation Command
//------------------------------------------------------------------------------
//#define HFLH_START_ADDR                     (0x0)
#define HFLH_PAGE_SIZE                      (0x200)

/*
 * @Note: one sector = 256K
 * @Note: S26KL512S and S26KS512S Sector Count = 256
 * @Note: S26KL256S and S26KS256S Sector Count = 128
*/
#define HFLH_MAX_SECTOR                     (256)

//------------------------------------------------------------------------------
#define CMD_NOOP_CODE                       (0x0)
#define CMD_COMMON_555                      (0x555)
#define CMD_COMMON_AA                       (0xAA)
#define CMD_COMMON_2AA                      (0x2AA)
#define CMD_COMMON_55                       (0x55)

#define CMD_70                              (0x70) //Read Status Register
#define CMD_71                              (0x71) //Clear Status Register
#define CMD_B9                              (0xB9) //Enter Power Down
#define CMD_34                              (0x34) //Set Power Down Reset Timer
#define CMD_3C                              (0x3C) //Read Power Down Reset Timer
#define CMD_36                              (0x36) //Load Interrupt Config Register
#define CMD_C4                              (0xC4) //Read Interrupt Config Register
#define CMD_37                              (0x37) //Load Interrupt Status Register
#define CMD_C5                              (0xC5) //Read Interrupt Status Register
#define CMD_38                              (0x38) //Load Volatile Config Register
#define CMD_C7                              (0xC7) //Read Volatile Config Register
#define CMD_39                              (0x39) //Program Non-Volatile Config Register
#define CMD_C8                              (0xC8) //Erase Non-Volatile Config Register
#define CMD_C6                              (0xC6) //Read Non-Volatile Config Register
#define CMD_A0                              (0xA0) //Word Program
#define CMD_25                              (0x25) //Write Buffer
#define CMD_29                              (0x29) //Program Buffer to Flash (Confirm)
#define CMD_F0                              (0xF0) //Write To Buffer Abort Reset
#define CMD_98                              (0x98) //Read ID Command

#define CMD_80                              (0x80) //Erase first commnad
#define CMD_10                              (0x10) //Erase chip commnad
#define CMD_30                              (0x30) //Erase sector Commnad
#define CMD_33                              (0x33) //Blank check
#define CMD_WORD_PROGRAM                    (0xA0)

//------------------------------------------------------------------------------
// HyperFlash Register Address
//------------------------------------------------------------------------------
#define ERASE_NVCR_REG                      (0xC8)
#define LOAD_VCR_REG                        (0x38)
#define READ_VCR_REG                        (0xC7)
#define WRITE_NVCR_REG                      (0x39)
#define READ_NVCR_REG                       (0xC6)

//------------------------------------------------------------------------------
// HyperFlash Common Macro
//------------------------------------------------------------------------------
#define HFLH_MAX_CS_LOW                     (0xFFFF)
#define HFLH_WR_ACCTIME                     (1)
#define HFLH_RD_ACCTIME                     (9)

#define HFLH_VCR_LTCY_Pos                   (4)
#define HFLH_VCR_LTCY_Msk                   (0xFUL << HFLH_VCR_LTCY_Pos)

void HyperFlash_WriteOPCMD(SPIM_T *spim, uint32_t u32CMD, uint32_t u32Addr);
uint16_t HyperFlash_ReadData1CmdSets2Byte(SPIM_T *spim, uint32_t u32LastCMD, uint32_t u32RdAddr);
uint16_t HyperFlash_ReadData4CmdSets(SPIM_T *spim, uint32_t u32LastCMD, uint32_t u32RdAddr);

void HyperFlash_Init(SPIM_T *spim);
int32_t HyperFlash_WaitBusBusy(SPIM_T *spim);
void HyperFlash_EraseSector(SPIM_T *spim, uint32_t u32SAddr);
void HyperFlash_DMARead(SPIM_T *pSPIMx, uint32_t u32SAddr, uint8_t *pu8RdBuf, uint32_t u32NRx);
void HyperFlash_DMAWrite(SPIM_T *pSPIMx, uint32_t u32SAddr, uint8_t *pu8WrBuf, uint32_t u32NTx);

#endif  /* __HYPER_FLASH_CODE_H__ */

/*** (C) COPYRIGHT 2019 Nuvoton Technology Corp. ***/
