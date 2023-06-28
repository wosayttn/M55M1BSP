/****************************************************************************
*                                                                           *
* Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.          *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   spi_i2s_cunit.h
*
* VERSION
*   1.0
*
* DESCRIPTION
*   The header file of CUnit test function
*
* HISTORY
*   2007-06-25    Ver 1.0 Created by NS21 WTLiu
*
* REMARK
*   None
****************************************************************************/

#ifndef _SPI_COMMON_H_
#define _SPI_COMMON_H_

//------------------------------------------------------------------------------
// SPI Port Index
//------------------------------------------------------------------------------
#define C_LPSPI0                                  (0)

/*----------------------------------------------------------------------------*/
/* INTERNAL Constants Definitions                                             */
/*----------------------------------------------------------------------------*/
#define INTERNAL_REG_BASE           (0x48)
#define SPI_INTERNAL_SELFTEST_Msk   (0x80)

//------------------------------------------------------------------------------
typedef enum
{
    eLPSPI_CLK_HIRC = 0,
    eLPSPI_CLK_MIRC,
    eLPSPI_CLK_PCLK,
} E_LPSPI_CLK;

//------------------------------------------------------------------------------
void LPSPI_ModuleReset(uint32_t u32SPIModule);
void LPSPI_ClkDisable(uint32_t u32SPIModule);
void LPSPI_ClkEnable(uint32_t u32SPIModule);

void LPSPI_CLK_Sel(uint32_t u32SPIModule, uint32_t ClkSrc);

#endif /* _SPI_COMMON_H_ */
