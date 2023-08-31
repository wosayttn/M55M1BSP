/***************************************************************************//**
 * @file     fmc_user.h
 * @version  V1.00
 * @brief    Simplified FMC driver header file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef FMC_USER_H
#define FMC_USER_H

#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Define parameter                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define ISPGO           0x01

/*---------------------------------------------------------------------------------------------------------*/
/*  FMC Macro Definitions                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/

int FMC_Read_User(unsigned int u32Addr, unsigned int *data);
void ReadData(unsigned int addr_start, unsigned int addr_end, unsigned int *data);
void WriteData(unsigned int addr_start, unsigned int addr_end, unsigned int *data);

#endif  /* FMC_USER_H */
