#include <stdio.h>
#include <stdlib.h>

#include "NuMicro.h"
#include "PinConfig.h"
#include "spi_common.h"

//------------------------------------------------------------------------------
char *GetTestSPIName(uint32_t u32Index)
{
    switch (u32Index)
    {
        case C_LPSPI0:
            return "LPSPI0";

        default:
            break;
    }

    return NULL;
}

void LPSPI_DISABLE_SELFTEST(uint32_t u32SpiModule)
{
    switch (u32SpiModule)
    {
        case C_LPSPI0:
            outp32(LPSPI0_BASE + INTERNAL_REG_BASE, ~SPI_INTERNAL_SELFTEST_Msk);
            break;
    }
}

void LPSPI_ENABLE_SELFTEST(uint32_t u32SpiModule)
{
    switch (u32SpiModule)
    {
        case C_LPSPI0:
            outp32(LPSPI0_BASE + INTERNAL_REG_BASE, SPI_INTERNAL_SELFTEST_Msk);
            break;
    }
}

void LPSPI_ModuleReset(uint32_t u32SPIModule)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    switch (u32SPIModule)
    {
        case C_LPSPI0:
            SYS->LPSPIRST |= (1 << (SYS_LPSPIRST_LPSPI0RST_Pos));
            SYS->LPSPIRST &= ~(1 << (SYS_LPSPIRST_LPSPI0RST_Pos));
            break;
    }

    SYS->LPSPIRST = 0;

    /* Lock protected registers */
    SYS_LockReg();
}

void LPSPI0_SetClkSrc(uint32_t u32ClkSrc)
{
    CLK->LPSPISEL &= (~CLK_LPSPISEL_LPSPI0SEL_Msk);

    switch (u32ClkSrc)
    {
        case eLPSPI_CLK_PCLK:
            CLK->LPSPISEL |= CLK_LPSPISEL_LPSPI0SEL_PCLK4;
            break;

        case eLPSPI_CLK_MIRC:
            CLK->LPSPISEL |= CLK_LPSPISEL_LPSPI0SEL_MIRC;
            break;

        case eLPSPI_CLK_HIRC:
            CLK->LPSPISEL |= CLK_LPSPISEL_LPSPI0SEL_HIRC;
            break;
    }

    LPSPI0_MOSI_PIN_INIT;
    LPSPI0_MISO_PIN_INIT;
    LPSPI0_CLK_PIN_INIT;
    LPSPI0_CSS_PIN_INIT;
}

void LPSPI_ClkDisable(uint32_t u32SPIModule)
{
    switch (u32SPIModule)
    {
        case C_LPSPI0:
            CLK->LPSPICTL &= (~CLK_LPSPICTL_LPSPI0CKEN_Msk);
            break;
    }
}

void LPSPI_ClkEnable(uint32_t u32SPIModule)
{
    switch (u32SPIModule)
    {
        case C_LPSPI0:
            CLK->LPSPICTL |= CLK_LPSPICTL_LPSPI0CKEN_Msk;
            break;
    }
}

void LPSPI_CLK_Sel(uint32_t u32SPIModule, uint32_t u32ClkSrc)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    LPSPI_ClkEnable(u32SPIModule);

    switch (u32SPIModule)
    {
        case C_LPSPI0:
            LPSPI0_SetClkSrc(u32ClkSrc);
            break;

        default:
            printf("\r\n");
            printf("===================================================\r\n");
            printf("%s initiation failed, please check your configuration\r\n",
                   GetTestSPIName(u32SPIModule));
            printf("===================================================\r\n");
            printf("\r\n");

            while (1);

            break;
    }
}
