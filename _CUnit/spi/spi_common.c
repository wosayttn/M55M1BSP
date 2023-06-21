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
        case C_SPI0:
            return "SPI0";

        case C_SPI1:
            return "SPI1";

        case C_SPI2:
            return "SPI2";

        case C_SPI3:
            return "SPI3";

        default:
            break;
    }

    return NULL;
}

void ResetSPI(uint32_t u32SPIModule)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    switch (u32SPIModule)
    {
        case C_SPI0:
            SYS->SPIRST |= (1 << (SYS_SPIRST_SPI0RST_Pos));
            SYS->SPIRST &= ~(1 << (SYS_SPIRST_SPI0RST_Pos));
            break;

        case C_SPI1:
            SYS->SPIRST |= (1 << (SYS_SPIRST_SPI1RST_Pos));
            SYS->SPIRST &= ~(1 << (SYS_SPIRST_SPI1RST_Pos));
            break;

        case C_SPI2:
            SYS->SPIRST |= (1 << (SYS_SPIRST_SPI2RST_Pos));
            SYS->SPIRST &= ~(1 << (SYS_SPIRST_SPI2RST_Pos));
            break;

        case C_SPI3:
            SYS->SPIRST |= (1 << (SYS_SPIRST_SPI3RST_Pos));
            SYS->SPIRST &= ~(1 << (SYS_SPIRST_SPI3RST_Pos));
            break;
    }

    //SYS->SPIRST = 0;

    /* Lock protected registers */
    SYS_LockReg();
}

void SPI3_SetClkSrc(uint32_t u32ClkSrc)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    CLK->SPISEL &= (~CLK_SPISEL_SPI3SEL_Msk);

    switch (u32ClkSrc)
    {
        case eSPI_CLK_HXT:
            CLK->SPISEL |= CLK_SPISEL_SPI3SEL_HXT;
            break;

        case eSPI_CLK_PLL1:
            CLK->SPISEL |= CLK_SPISEL_SPI3SEL_APLL1_DIV2;
            break;

        case eSPI_CLK_PLL0:
            CLK->SPISEL |= CLK_SPISEL_SPI3SEL_APLL0_DIV2;
            break;

        case eSPI_CLK_PCLK:
            CLK->SPISEL |= CLK_SPISEL_SPI3SEL_PCLK2;
            break;

        case eSPI_CLK_HIRC:
            CLK->SPISEL |= CLK_SPISEL_SPI3SEL_HIRC;
            break;

        case eSPI_CLK_HIRC48M:
            CLK->SPISEL |= CLK_SPISEL_SPI3SEL_HIRC48M;
            break;
    }

    SPI3_MOSI_PIN_INIT;
    SPI3_MISO_PIN_INIT;
    SPI3_CLK_PIN_INIT;
    SPI3_CSS_PIN_INIT;
    SPI3_MCLK_PIN_INIT;
}

void SPI2_SetClkSrc(uint32_t u32ClkSrc)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    CLK->SPISEL &= (~CLK_SPISEL_SPI2SEL_Msk);

    switch (u32ClkSrc)
    {
        case eSPI_CLK_HXT:
            CLK->SPISEL |= CLK_SPISEL_SPI2SEL_HXT;
            break;

        case eSPI_CLK_PLL1:
            CLK->SPISEL |= CLK_SPISEL_SPI2SEL_APLL1_DIV2;
            break;

        case eSPI_CLK_PLL0:
            CLK->SPISEL |= CLK_SPISEL_SPI2SEL_APLL0_DIV2;
            break;

        case eSPI_CLK_PCLK:
            CLK->SPISEL |= CLK_SPISEL_SPI2SEL_PCLK0;
            break;

        case eSPI_CLK_HIRC:
            CLK->SPISEL |= CLK_SPISEL_SPI2SEL_HIRC;
            break;

        case eSPI_CLK_HIRC48M:
            CLK->SPISEL |= CLK_SPISEL_SPI2SEL_HIRC48M;
            break;
    }

    SPI2_MOSI_PIN_INIT;
    SPI2_MISO_PIN_INIT;
    SPI2_CLK_PIN_INIT;
    SPI2_CSS_PIN_INIT;
    SPI2_MCLK_PIN_INIT;
}

void SPI1_SetClkSrc(uint32_t u32ClkSrc)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    CLK->SPISEL &= (~CLK_SPISEL_SPI1SEL_Msk);

    switch (u32ClkSrc)
    {
        case eSPI_CLK_HXT:
            CLK->SPISEL |= CLK_SPISEL_SPI1SEL_HXT;
            break;

        case eSPI_CLK_PLL1:
            CLK->SPISEL |= CLK_SPISEL_SPI1SEL_APLL1_DIV2;
            break;

        case eSPI_CLK_PLL0:
            CLK->SPISEL |= CLK_SPISEL_SPI1SEL_APLL0_DIV2;
            break;

        case eSPI_CLK_PCLK:
            CLK->SPISEL |= CLK_SPISEL_SPI1SEL_PCLK2;
            break;

        case eSPI_CLK_HIRC:
            CLK->SPISEL |= CLK_SPISEL_SPI1SEL_HIRC;
            break;

        case eSPI_CLK_HIRC48M:
            CLK->SPISEL |= CLK_SPISEL_SPI1SEL_HIRC48M;
            break;
    }

    SPI1_MOSI_PIN_INIT;
    SPI1_MISO_PIN_INIT;
    SPI1_CLK_PIN_INIT;
    SPI1_CSS_PIN_INIT;
    SPI1_MCLK_PIN_INIT;
}

void SPI0_SetClkSrc(uint32_t u32ClkSrc)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    CLK->SPISEL &= (~CLK_SPISEL_SPI0SEL_Msk);

    switch (u32ClkSrc)
    {
        case eSPI_CLK_HXT:
            CLK->SPISEL |= CLK_SPISEL_SPI0SEL_HXT;
            break;

        case eSPI_CLK_PLL1:
            CLK->SPISEL |= CLK_SPISEL_SPI0SEL_APLL1_DIV2;
            break;

        case eSPI_CLK_PLL0:
            CLK->SPISEL |= CLK_SPISEL_SPI0SEL_APLL0_DIV2;
            break;

        case eSPI_CLK_PCLK:
            CLK->SPISEL |= CLK_SPISEL_SPI0SEL_PCLK0;
            break;

        case eSPI_CLK_HIRC:
            CLK->SPISEL |= CLK_SPISEL_SPI0SEL_HIRC;
            break;

        case eSPI_CLK_HIRC48M:
            CLK->SPISEL |= CLK_SPISEL_SPI0SEL_HIRC48M;
            break;
    }

    SPI0_MOSI_PIN_INIT;
    SPI0_MISO_PIN_INIT;
    SPI0_CLK_PIN_INIT;
    SPI0_CSS_PIN_INIT;
    SPI0_MCLK_PIN_INIT;
}

void SPI_ClkDisable(uint32_t u32SPIModule)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    switch (u32SPIModule)
    {
        case C_SPI0:
            CLK->SPICTL &= (~CLK_SPICTL_SPI0CKEN_Msk);
            break;

        case C_SPI1:
            CLK->SPICTL &= (~CLK_SPICTL_SPI1CKEN_Msk);
            break;

        case C_SPI2:
            CLK->SPICTL &= (~CLK_SPICTL_SPI2CKEN_Msk);
            break;

        case C_SPI3:
            CLK->SPICTL &= (~CLK_SPICTL_SPI3CKEN_Msk);
            break;
    }
}

void SPI_ClkEnable(uint32_t u32SPIModule)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    switch (u32SPIModule)
    {
        case C_SPI0:
            CLK->SPICTL |= CLK_SPICTL_SPI0CKEN_Msk;
            break;

        case C_SPI1:
            CLK->SPICTL |= CLK_SPICTL_SPI1CKEN_Msk;
            break;

        case C_SPI2:
            CLK->SPICTL |= CLK_SPICTL_SPI2CKEN_Msk;
            break;

        case C_SPI3:
            CLK->SPICTL |= CLK_SPICTL_SPI3CKEN_Msk;
            break;
    }
}

void SPI_CLK_Sel(uint32_t u32SPIModule, uint32_t u32ClkSrc)
{
    SPI_ClkEnable(u32SPIModule);

    switch (u32SPIModule)
    {
        case C_SPI0:
            SPI0_SetClkSrc(u32ClkSrc);
            break;

        case C_SPI1:
            SPI1_SetClkSrc(u32ClkSrc);
            break;

        case C_SPI2:
            SPI2_SetClkSrc(u32ClkSrc);
            break;

        case C_SPI3:
            SPI3_SetClkSrc(u32ClkSrc);
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
