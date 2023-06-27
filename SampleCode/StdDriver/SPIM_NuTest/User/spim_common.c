#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NuMicro.h"
#include "PinConfig.h"
#include "common.h"
#include "spim_common.h"
#include "otfc_common.h"

//------------------------------------------------------------------------------
SPIM_T *gSPIMPort[] =
{
    SPIM0,
    SPIM1,
};

__attribute__((aligned(32))) uint8_t tstbuf[TEST_BUFF_SIZE] = {0};
__attribute__((aligned(32))) uint8_t tstbuf2[TEST_BUFF_SIZE] = {0};
__attribute__((aligned(32))) uint8_t tstbuf3[TEST_BUFF_SIZE] = {0};

//------------------------------------------------------------------------------
void *SPIM_GetModule(uint32_t u32Index)
{
    return gSPIMPort[u32Index];
}

uint32_t SPIM_GetClkDivNum(void)
{
    uint32_t u32DivStart = 0;

    /* Update System Core Clock */
    SystemCoreClockUpdate();

    printf("CPU Freq = %dMhz\r\n", (CLK_GetSCLKFreq() / 1000000));
    printf("System Core Clock = %dMhz\r\n", GetSystemCoreClock() / 1000000);

    if (SystemCoreClock >= 192000000UL)
    {
        u32DivStart = 2;
    }
    else if (SystemCoreClock == 12000000UL)
    {
        u32DivStart = 0;
    }
    else
    {
        u32DivStart = 1;
    }

    return u32DivStart;
}

void SPIM_DisableInt(uint32_t u32SPIMModule)
{
    switch (u32SPIMModule)
    {
        case C_SPIM0:
            NVIC_DisableIRQ(SPIM0_IRQn);
            break;

        case C_SPIM1:
            NVIC_DisableIRQ(SPIM1_IRQn);
            break;
    }
}

void SPIM_EnableInt(uint32_t u32SPIMModule)
{
    switch (u32SPIMModule)
    {
        case C_SPIM0:
            NVIC_EnableIRQ(SPIM0_IRQn);
            break;

        case C_SPIM1:
            NVIC_EnableIRQ(SPIM1_IRQn);
            break;
    }
}

void SPIM_DisableModule(uint32_t u32SPIMModule)
{
    switch (u32SPIMModule)
    {
        case C_SPIM0:
            /* Disable SPIM0 module clock */
            CLK_DisableModuleClock(SPIM0_MODULE);
            /* Disable OTFC0 module clock */
            CLK_DisableModuleClock(OTFC0_MODULE);
            //printf("OTFCCTL = %d, addr = 0x%08X\r\n", CLK->OTFCCTL, &CLK->OTFCCTL);
            break;

        case C_SPIM1:
            /* Disable SPIM1 module clock */
            CLK_DisableModuleClock(SPIM1_MODULE);
            /* Disable OTFC1 module clock */
            CLK_DisableModuleClock(OTFC1_MODULE);
            break;
    }
}

void SPIM_EanbleModule(uint32_t u32SPIMModule)
{
    switch (u32SPIMModule)
    {
        case C_SPIM0:
            /* Enable SPIM0 module clock */
            CLK_EnableModuleClock(SPIM0_MODULE);
            /* Enable OTFC0 module clock */
            CLK_EnableModuleClock(OTFC0_MODULE);
            //printf("OTFCCTL = %d, addr = 0x%08X\r\n", CLK->OTFCCTL, &CLK->OTFCCTL);
            break;

        case C_SPIM1:
            /* Enable SPIM1 module clock */
            CLK_EnableModuleClock(SPIM1_MODULE);
            /* Enable OTFC1 module clock */
            CLK_EnableModuleClock(OTFC1_MODULE);
            break;
    }
}

void SPIM_InitModule(uint32_t u32SPIMModule)
{
    SPIM_EanbleModule(u32SPIMModule);

    switch (u32SPIMModule)
    {
        case C_SPIM0:
            /* Init SPIM0 multi-function pins */
            SPIM0_RST_PIN_INIT();
            SPIM0_CLK_PIN_INIT();
            SPIM0_CLKN_PIN_INIT();
            SPIM0_D2_PIN_INIT();
            SPIM0_D3_PIN_INIT();
            SPIM0_D4_PIN_INIT();
            SPIM0_D5_PIN_INIT();
            SPIM0_D6_PIN_INIT();
            SPIM0_D7_PIN_INIT();
            SPIM0_MISO_PIN_INIT();
            SPIM0_MOSI_PIN_INIT();
            SPIM0_SS_PIN_INIT();
            SPIM0_RWDS_PIN_INIT();

            /* Set SPIM0 I/O pins as high slew rate up to 80 MHz. */
            SPIM0_PIN_HIGH_SLEW();
            break;

        case C_SPIM1:
            /* Init SPIM1 multi-function pins */
            SPIM1_RST_PIN_INIT();
            SPIM1_CLK_PIN_INIT();
            SPIM1_CLKN_PIN_INIT();
            SPIM1_D2_PIN_INIT();
            SPIM1_D3_PIN_INIT();
            SPIM1_D4_PIN_INIT();
            SPIM1_D5_PIN_INIT();
            SPIM1_D6_PIN_INIT();
            SPIM1_D7_PIN_INIT();
            SPIM1_MISO_PIN_INIT();
            SPIM1_MOSI_PIN_INIT();
            SPIM1_SS_PIN_INIT();
            SPIM1_RWDS_PIN_INIT();

            /* Set SPIM1 I/O pins as high slew rate up to 80 MHz. */
            SPIM1_PIN_HIGH_SLEW();
            break;
    }
}

int SPIM_SelectModule(uint32_t u32SPIMPort)
{
    uint32_t u32Index = 0;
    S_TestOption sTestPort[] =
    {
        {"SPIM0 / OTFC0"},
        {"SPIM1 / OTFC1"},
    };

    SPIM_SysReset(u32SPIMPort);
    OTFC_SysReset(u32SPIMPort);
    SPIM_DisableModule(u32SPIMPort);

    u32Index = GetRequireOptions(sTestPort, sizeof(sTestPort) / sizeof(sTestPort[0]));

    SPIM_InitModule(u32Index);

    return (int)u32Index;
}

void SPIM_SysReset(uint32_t u32SPIMModule)
{
    SYS_UnlockReg();

    switch (u32SPIMModule)
    {
        case C_SPIM0:
            SYS->SPIMRST |= SYS_SPIMRST_SPIM0RST_Msk;
            SYS->SPIMRST &= ~SYS_SPIMRST_SPIM0RST_Msk;

            SYS->OTFCRST |= SYS_OTFCRST_OTFC0RST_Msk;
            SYS->OTFCRST &= ~SYS_OTFCRST_OTFC0RST_Msk;
            break;

        case C_SPIM1:
            SYS->SPIMRST |= SYS_SPIMRST_SPIM1RST_Msk;
            SYS->SPIMRST &= ~SYS_SPIMRST_SPIM1RST_Msk;

            SYS->OTFCRST |= SYS_OTFCRST_OTFC1RST_Msk;
            SYS->OTFCRST &= ~SYS_OTFCRST_OTFC1RST_Msk;
            break;
    }

    //SYS_LockReg();
}
