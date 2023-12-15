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
#define C_SPI0                                  (0)
#define C_SPI1                                  (1)
#define C_SPI2                                  (2)
#define C_SPI3                                  (3)

/*----------------------------------------------------------------------------*/
/* INTERNAL Constants Definitions                                             */
/*----------------------------------------------------------------------------*/
#define INTERNAL_REG_BASE                       (0x48)
#define SPI_INTERNAL_SELFTEST_Msk               (0x80)

//------------------------------------------------------------------------------
typedef enum
{
    eSPI_CLK_HIRC = 0,
    eSPI_CLK_HXT,
    eSPI_CLK_PCLK,

    /* Palladium Not Test */
    eSPI_CLK_PLL1, //M254, M256, M258 not support
    eSPI_CLK_PLL0, //M254, M256, M258 not support
    eSPI_CLK_HIRC48M,
} E_SPI_CLK;

//------------------------------------------------------------------------------
void ResetSPI(uint32_t u32SPIModule);
void SPI_ClkDisable(uint32_t u32SPIModule);
void SPI_ClkEnable(uint32_t u32SPIModule);

void SPI_CLK_Sel(uint32_t u32SPIModule, uint32_t ClkSrc);
char *GetTestSPIName(uint32_t u32Index);
#endif /* _SPI_COMMON_H_ */
