#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "NuMicro.h"
#include "common.h"
#include "PinConfig.h"

//------------------------------------------------------------------------------
static SPIM_T *gpasSPIMModule[] =
{
    SPIM0,
    SPIM1,
};

static __attribute__((aligned(32))) uint8_t g_buff[BUFFER_SIZE] = {0};
static __attribute__((aligned(32))) uint8_t g_buff1[BUFFER_SIZE] = {0};

static volatile uint8_t gu32TestSPIMModule = 0;

static uint8_t gu8AutoRun = 0;

static const uint32_t patterns[] =
{
    //0x00000000, 0xFFFFFFFF, 0x55aa55aa, 0xaa55aa55, 0x33cc33cc, 0xcc33cc33
    0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666
    //0x11111111, 0x22222222, 0x33333333, 0x44444444
    //0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF
};

//------------------------------------------------------------------------------
// Common API
//------------------------------------------------------------------------------
void *GetSPIMModule(uint32_t u32Idx)
{
    return gpasSPIMModule[u32Idx];
}

uint32_t GetSPIMTestModuleIdx(void)
{
    return gu32TestSPIMModule;
}

void SetSPIMTestModuleIdx(uint32_t u32SetValue)
{
    gu32TestSPIMModule = u32SetValue;
}

void *GetTestBuffer1(void)
{
    return &g_buff;
}

void *GetTestBuffer2(void)
{
    return &g_buff1;
}

uint32_t GetDmmModeMapAddr(SPIM_T *pSPIMx)
{
    if (pSPIMx == SPIM0)
    {
        return SPIM0_DMM_MAP_ADDR;
    }
    else if (pSPIMx == SPIM1)
    {
        return SPIM1_DMM_MAP_ADDR;
    }

    return SPIM0_DMM_MAP_ADDR;
}

void dump_compare_error(uint32_t addr, uint8_t *buf_expect, uint8_t *buf_compare, int count)
{
    int  i, err_cnt = 0;

    for (i = 0; (i < count) /*&& (err_cnt < 32)*/; i++)
    {
        if (buf_expect[i] != buf_compare[i])
        {
            printf("addr 0x%x, expect: 0x%x, read: 0x%x\n", addr + i, buf_expect[i], buf_compare[i]);
            err_cnt++;
        }
    }
}

void popDat(uint8_t *buf, uint32_t bufSize)
{
    uint32_t bufRmn = bufSize;

    while (bufRmn)
    {
        uint32_t nTx = sizeof(patterns);

        if (nTx > bufRmn)
        {
            nTx = bufRmn;
        }

        memcpy(buf, (uint8_t *) patterns, nTx);
        buf += nTx;                              // Advance indicator.
        bufRmn -= nTx;
    }
}

int EnableAutoRunTest(uint32_t u32SPIMModule)
{
    gu8AutoRun = 1;

    return 0;
}

int DisableAutoRunTest(uint32_t u32SPIMModule)
{
    gu8AutoRun = 0;

    return 0;
}

void DelayUs(volatile uint32_t micros)
{
    /* multiply micro with MULTIPLITER */
    micros = ((MULTIPLITER * micros) - 10);

    /* 4 cycles for one loop */
    while (micros--);
}

void DelayMs(volatile uint32_t mills)
{
    /* multiply mills with MULTIPLITER */
    mills = ((MULTIPLITER * mills * 1000) - 10);

    /* 4 cycles for one loop */
    while (mills--);
}

uint32_t sysGetNum(void)
{
    uint8_t cInputTemp = 0x00, InputString[16] = {0};
    uint32_t nLoop = 0;

    while ((cInputTemp != 0x0D) && (cInputTemp != 0x0A))
    {
        cInputTemp = getchar();

        if (cInputTemp == 27)   /* 27: ESC key */
        {
            return cInputTemp;
        }

        if (cInputTemp == 'x' || cInputTemp == 'X' || cInputTemp == 'f' ||
                cInputTemp == 'F' || cInputTemp == 'r' || cInputTemp == 'R')
        {
            return cInputTemp;
        }

        if (cInputTemp == '-')
        {
            InputString[nLoop] = cInputTemp;
            printf("%c", cInputTemp);
            nLoop++;
        }
        else if (cInputTemp >= '0' && cInputTemp <= '9')
        {
            InputString[nLoop] = cInputTemp;
            printf("%c", cInputTemp);
            nLoop++;
        }
    }

    printf("\n");

    //return atoi(InputString);
    return atoi((const char *)InputString);
}

uint32_t GetRequireOptions(S_TestOption *pOption, uint32_t u32Size)
{
    int i32Index = 0;

    for (i32Index = 0; i32Index < u32Size; i32Index++)
    {
        printf("[%d]%s\r\n",
               i32Index,
               pOption[i32Index].chName);
    }

    printf("\r\n");

    i32Index = sysGetNum();

    if (i32Index >= u32Size)
    {
        return 0;
    }

    return i32Index;
}

void RunTestFunction(S_TestMenu *pTestMenu, uint32_t u32MenuSize, uint32_t u32SPIMModule)
{
    //SPIM_T *pSPIMx = (SPIM_T *)GetSPIMPort(u32SPIMModule);
    int i32Index = 0;

    while (1)
    {
        //SPIM_SetISRTestMode(SPIM_ISR_NOR_MODE);

        for (i32Index = 0; i32Index < u32MenuSize; i32Index++)
        {
            printf("SPIM%d [%d]%s\r\n",
                   u32SPIMModule,
                   i32Index,
                   pTestMenu[i32Index].chName);
            //#if (AUTO_RUN_TEST == 1)

            if (gu8AutoRun == 1)
            {
                if (i32Index == 0)
                {
                    continue;
                }

                if ((pTestMenu[i32Index].fp != NULL) &&
                        (pTestMenu[i32Index].u32AutoTest == 1))
                {
                    pTestMenu[i32Index].fp(u32SPIMModule);
                }

                DelayUs(100);
            }

            //#endif //AUTO_RUN_TEST
        }

        gu8AutoRun = 0;

        printf("\r\n");

        i32Index = sysGetNum();

        if (i32Index == 0)
        {
            return;
        }

        if (i32Index >= u32MenuSize)
        {
            continue;
        }

        printf("===========================================================\r\n");
        printf("Current test : %s\r\n", pTestMenu[i32Index].chName);
        printf("===========================================================\r\n");

        if (pTestMenu[i32Index].fp != NULL)
        {
            pTestMenu[i32Index].fp(u32SPIMModule);
        }
    }
}

//------------------------------------------------------------------------------
// SPIM API
//------------------------------------------------------------------------------
/**
 * @brief   Phase table DMA Write
 *
 * @param spim
 * @param psPhaseTable  Check SPI Flash specifications for support command codes,
 *                      and create command phase table.
 * @param u32Addr       Write Address.
 * @param is4ByteAddr   Enable 4 Bytes Address Mode.
 * @param u32WrSize     Write Data Size.
 * @param pu8TxBuf      Write Data Buffer.
 */
void SPIM_DMA_WritePhase(SPIM_T *spim, SPIM_PHASE_T *psPhaseTable,
                         int is4ByteAddr, uint32_t u32Addr, uint32_t u32WrSize,
                         uint8_t *pu8TxBuf)
{
    if (psPhaseTable != NULL)
    {
        SPIM_DMADMM_InitPhase(spim, psPhaseTable, SPIM_CTL0_OPMODE_PAGEWRITE);
    }

    SPIM_DMA_Write(spim,
                   u32Addr,
                   is4ByteAddr,
                   u32WrSize,
                   pu8TxBuf,
                   psPhaseTable->u32CMDCode);
}

/**
 * @brief   Phase table DMA Read.
 *
 * @param spim
 * @param psPhaseTable  Check SPI Flash specifications for support command codes,
 *                      and create command phase table.
 * @param u32Addr       Read SPI Flash Address.
 * @param is4ByteAddr   Enable 4 Bytes Address.
 * @param u32RdSize     Read Data Size.
 * @param pu8RxBuf      Read Data Buffer.
 * @param isSync        Wait SPIM Enable Done
 * @return int32_t
 */
int32_t SPIM_DMA_ReadPhase(SPIM_T *spim, SPIM_PHASE_T *psPhaseTable,
                           int is4ByteAddr, uint32_t u32Addr, uint32_t u32RdSize,
                           uint8_t *pu8RxBuf, int isSync)
{
    if (psPhaseTable != NULL)
    {
        SPIM_DMADMM_InitPhase(spim, psPhaseTable, SPIM_CTL0_OPMODE_PAGEREAD);
    }

    return SPIM_DMA_Read(spim,
                         u32Addr,
                         is4ByteAddr,
                         u32RdSize,
                         pu8RxBuf,
                         psPhaseTable->u32CMDCode,
                         isSync);
}

/**
 * @brief   Phase table DMM Read.
 *
 * @param spim
 * @param psPhaseTable  Check SPI Flash specifications for support command codes,
 *                      and create command phase table.
 * @param is4ByteAddr   Enable 4 Bytes Address.
 * @param u32IdleIntvl  Direct Map Mode Idle Interval Time.
 */
void SPIM_DMM_ReadPhase(SPIM_T *spim, SPIM_PHASE_T *psPhaseTable, int is4ByteAddr, uint32_t u32IdleIntvl)
{
    SPIM_DMADMM_InitPhase(spim, psPhaseTable, SPIM_CTL0_OPMODE_DIRECTMAP);

    SPIM_EnterDirectMapMode(spim, is4ByteAddr, psPhaseTable->u32CMDCode, u32IdleIntvl);
}

/**
  * @brief      SPIM Default Config HyperBus Access Module Parameters.
  * @param      spim
  * @param      u32CSMaxLT Chip Select Maximum Low Time 0 ~ 0xFFFF, Default Set 0x02ED
  * @param      u32AcctRD Initial Read Access Time 1 ~ 0x1F, Default Set 0x04
  * @param      u32AcctWR Initial Write Access Time 1 ~ 0x1F, Default Set 0x04
  * @return     None.
  */
void SPIM_Hyper_DefaultConfig(SPIM_T *spim, uint32_t u32CSMaxLow, uint32_t u32AcctRD, uint32_t u32AcctWR)
{
    /* Chip Select Setup Time 2.5 */
    SPIM_HYPER_SET_CONFIG1_CSST(spim, SPIM_HYPER_CONFIG1_CSST_2_5_HCLK);

    /* Chip Select Hold Time 3.5 HCLK */
    SPIM_HYPER_SET_CONFIG1_CSH(spim, SPIM_HYPER_CONFIG1_CSH_3_5_HCLK);

    /* Chip Select High between Transaction as 2 HCLK cycles */
    SPIM_HYPER_SET_CONFIG1_CSHI(spim, 2);

    /* Chip Select Masximum low time HCLK */
    SPIM_HYPER_SET_CONFIG1_CSMAXLT(spim, u32CSMaxLow);

    /* Initial Device RESETN Low Time 255 */
    SPIM_HYPER_SET_CONFIG2_RSTNLT(spim, 0xFF);

    /* Initial Read Access Time Clock cycle*/
    SPIM_HYPER_SET_CONFIG2_ACCTRD(spim, u32AcctRD);

    /* Initial Write Access Time Clock cycle*/
    SPIM_HYPER_SET_CONFIG2_ACCTWR(spim, u32AcctWR);
}

/**
  * @brief      Erase specified address range.
  * @param      u32Addr     Start u32Address to erase.
  * @param      is4ByteAddr 4-byte address or not.
  * @param      len         Number of bytes to erase.
  * @param      u32ErsCmd      Erase command.
  * @param      u32ErsBlkSize  Erased block size.
  * @param      u32NBit     N-bit transmit/receive.
  * @details    Erase specified address range. Even though the range is not block-aligned, the start block and end block are fully erased anyway.
  * @return     None.
  */
void SPIM_EraseAddrRange(SPIM_T *pSPIMx, uint32_t u32Addr, int is4ByteAddr, uint32_t len, uint32_t u32ErsCmd, uint32_t u32ErsBlkSize, uint32_t u32NBit)
{
    printf("SPIM_EraseAddrRange: 0x%x, 0x%x\n", u32Addr, len);

    do
    {
        uint32_t u32AddrInd = u32Addr / u32ErsBlkSize * u32ErsBlkSize;
        uint32_t endAddr = (u32Addr + len - 1) / u32ErsBlkSize * u32ErsBlkSize;

        while (u32AddrInd <= endAddr)
        {
            SPIM_EraseBlock(pSPIMx, u32AddrInd, is4ByteAddr, u32ErsCmd, u32NBit, 1);
            u32AddrInd += u32ErsBlkSize;                              // Advance indicator.
        }
    } while (0);
}

uint32_t GetSPIMClkDivNum(void)
{
    uint32_t u32DivStart = 0;

    /* Update System Core Clock */
    SystemCoreClockUpdate();

    printf("CPU Freq = %dMhz\r\n", (CLK_GetSCLKFreq() / 1000000));
    printf("System Core Clock = %dMhz\r\n", GetSystemCoreClock() / 1000000);
#if 1

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

#endif //0

    return u32DivStart;
}

void SPIM_NVIC_Enable(uint32_t u32SPIMModule)
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

void SPIM_NVIC_Disable(uint32_t u32SPIMModule)
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

void SPIMx_SysReset(uint32_t u32SPIMModule)
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

    SYS_LockReg();
}

void SPIM_ClockDisable(uint32_t u32SPIMModule)
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

void SPIM_ClockEanble(uint32_t u32SPIMModule)
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

void InitSPIMPort(uint32_t u32SPIMModule)
{
    switch (u32SPIMModule)
    {
        case C_SPIM0:
            /* Enable SPIM0 module clock */
            CLK_EnableModuleClock(SPIM0_MODULE);
            /* Enable OTFC0 module clock */
            CLK_EnableModuleClock(OTFC0_MODULE);
            //printf("OTFCCTL = %d, addr = 0x%08X\r\n", CLK->OTFCCTL, &CLK->OTFCCTL);

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
            /* Enable SPIM1 module clock */
            CLK_EnableModuleClock(SPIM1_MODULE);
            /* Enable OTFC1 module clock */
            CLK_EnableModuleClock(OTFC1_MODULE);

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

int SPIMPortSelect(uint32_t u32SPIMPort)
{
    uint32_t u32Index = 0;
    S_TestOption sTestPort[] =
    {
        {"SPIM0 / OTFC0"},
        {"SPIM1 / OTFC1"},
    };

    SPIMx_SysReset(u32SPIMPort);
    //OTFCx_SysReset(u32SPIMPort);
    SPIM_ClockDisable(u32SPIMPort);

    u32Index = GetRequireOptions(sTestPort, sizeof(sTestPort) / sizeof(sTestPort[0]));

    InitSPIMPort(u32Index);
    //SelectSPIMClockSrc(u32Index);

    return (int)u32Index;
}
