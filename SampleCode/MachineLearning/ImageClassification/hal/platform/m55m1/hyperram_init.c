#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "NuMicro.h"
#include "hyperram_init.h"
#include "SPIM_PinConfig.h"

//------------------------------------------------------------------------------
static const uint32_t patterns[] =
{
    //0x00000000, 0xFFFFFFFF, 0x55aa55aa, 0xaa55aa55, 0x33cc33cc, 0xcc33cc33
    0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666
    //0x11111111, 0x22222222, 0x33333333, 0x44444444
    //0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF
};

__attribute__((aligned(32))) static uint8_t tstbuf[32] = {0};
__attribute__((aligned(32))) static uint8_t tstbuf2[32 * 4] = {0};
__attribute__((aligned(32))) static uint8_t tstbuf3[32] = {0};

//------------------------------------------------------------------------------
static void popDat(uint8_t buf[], uint32_t bufSize)
{
    uint8_t *bufInd = buf;
    uint32_t bufRmn = bufSize;

    while (bufRmn)
    {
        uint32_t nTx = sizeof(patterns);

        if (nTx > bufRmn)
        {
            nTx = bufRmn;
        }

        memcpy(bufInd, (uint8_t *) patterns, nTx);
        bufInd += nTx;                              // Advance indicator.
        bufRmn -= nTx;
    }
}

static uint32_t SPIM_GetDmmMapAddr(SPIM_T *pSPIMx)
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

static void SPIM_Hyper_EraseHRAM(SPIM_T *pSPIMx, uint32_t u32StartAddr, uint32_t u32EraseSize)
{
    uint16_t u16Data = 0;
    uint32_t u32i = 0;
    uint32_t u32RemainSize = (u32EraseSize % 2);

    for (u32i = 0; u32i <= (u32EraseSize - u32RemainSize); u32i += 2)
    {
        SPIM_Hyper_Write2Byte(pSPIMx, (u32StartAddr + u32i), 0x0000);
        u16Data = SPIM_Hyper_Read1Word(pSPIMx, (u32StartAddr + u32i));

        if (u16Data != 0x0000)
        {
            printf("Erase Hyper RAM fail!! Read address:0x%08x, data::0x%08x  expect: 0\n",
                   u32StartAddr + u32i,
                   u16Data);

            while (1);
        }
    }

    if (u32RemainSize != 0)
    {
        SPIM_Hyper_Write1Byte(pSPIMx, (u32StartAddr + (u32EraseSize - 1)), 0x00);
        u16Data = SPIM_Hyper_Read1Word(pSPIMx, (u32StartAddr + u32EraseSize));

        if ((u16Data & 0xFF) != 0)
        {
            printf("Erase Remain Hyper RAM fail!! Read address:0x%08x, data::0x%08x  expect: 0\n",
                   (u32StartAddr + (u32EraseSize - 1)),
                   u16Data);

            while (1);
        }
    }
}

static void SPIM_HRAM_IO_Write(SPIM_T *pSPIMx, uint32_t u32Saddr, void *pvWrBuf, uint32_t u32NTx)
{
    uint8_t u8RemainSize = (u32NTx % 2);
    uint16_t u16WData = 0;
    uint32_t u32i = 0;
    uint8_t *pu8WrBuf = (uint8_t *)pvWrBuf;

    for (u32i = 0; u32i <= (u32NTx - u8RemainSize); u32i += 2)
    {
        memcpy(&u16WData, (uint16_t *)&pu8WrBuf[u32i], sizeof(uint16_t));
        SPIM_Hyper_Write2Byte(pSPIMx, (u32Saddr + u32i), u16WData);
    }

    if (u8RemainSize != 0)
    {
        memcpy(&u8RemainSize, (uint8_t *)&pu8WrBuf[u32NTx - 1], sizeof(uint8_t));
        SPIM_Hyper_Write1Byte(pSPIMx, (u32Saddr + (u32NTx - 1)), u8RemainSize);
    }
}

static void SPIM_HRAM_IO_Read(SPIM_T *pSPIMx, uint32_t u32Saddr, void *pvRdBuf, uint32_t u32NRx)
{
    uint32_t u32i = 0;
    uint16_t u16RData = 0;
    uint32_t u32RemainSize = (u32NRx  % 2);
    uint8_t *pu8RdBuf = (uint8_t *)pvRdBuf;

    for (u32i = 0; u32i <= (u32NRx - u32RemainSize); u32i += 2)
    {
        u16RData = SPIM_Hyper_Read1Word(pSPIMx, (u32Saddr + u32i));
        memcpy(&pu8RdBuf[u32i], &u16RData, sizeof(uint16_t));
    }

    if (u32RemainSize != 0)
    {
        u16RData = SPIM_Hyper_Read1Word(pSPIMx, (u32Saddr + u32NRx));
        outpb(&pu8RdBuf[u32NRx - 1], (u16RData & 0xFF));
    }
}

static int SPIM_HyperRAMDLLDelayTimeTraining(SPIM_T *pSPIMx)
{
    //SPIM_T *pSPIMx = (SPIM_T *)GetSPIMPort(u32SPIMModule);
    uint32_t u32i = 0;
    uint32_t u32j = 0;
    uint32_t u32OutValid =  0;
    uint32_t u32LockTime =  0;
    uint32_t u32ClkOnNum = 0;
    uint8_t u8RdDelay = 0;
    uint8_t u8Temp = 0;
    uint8_t u8RdDelayIdx = 0;
    uint8_t u8RdDelayRes[DLL_MAX_DELAY_NUM] = {0};
    uint32_t u32DLLOutVaild = 0;
    uint32_t u32RdData = 0;
    uint32_t u32SrcAddr = 0;
    uint32_t u32TestSize = 32;
    uint32_t u32ID[4] = {0};
    uint8_t u8i = 0;
    //uint8_t u8TestPatten[32] = {0};
    //uint8_t tstbuf2[32] = {0};

    SPIM_Hyper_EraseHRAM(pSPIMx, u32SrcAddr, u32TestSize);

    popDat(&tstbuf[0], u32TestSize);

    for (u32i = u32SrcAddr; u32i < u32TestSize; u32i++)
    {
        SPIM_Hyper_Write1Byte(pSPIMx, u32i, tstbuf[u32i]);
    }

    for (u8RdDelay = 0; u8RdDelay <= DLL_MAX_DELAY_NUM; u8RdDelay++)
    {
        SPIM_CtrlDLLDelayTime(pSPIMx, 0, 0, 0, 0, u8RdDelay);

        SPIM_Hyper_DMARead(pSPIMx,
                           u32SrcAddr,
                           tstbuf2,
                           u32TestSize);
        if (memcmp(tstbuf, tstbuf2, u32TestSize))
        {
            printf("!!!\tDLL Training failed at block 0x%x\n", u32SrcAddr);
        }
        else
        {
            //printf("RX Delay: %d = Pass\r\n", u8RdDelay);
            u8RdDelayRes[u8RdDelayIdx++] = u8RdDelay;
        }
	}

    //[sort]
    for (u32i = 0 ; u32i <= u8RdDelayIdx ; u32i = u32i + 1)
    {
        for (u32j = u32i + 1 ; u32j < u8RdDelayIdx ; u32j = u32j + 1)
        {
            if (u8RdDelayRes[u32i] > u8RdDelayRes[u32j])
            {
                u8Temp = u8RdDelayRes[u32i];
                u8RdDelayRes[u32i] = u8RdDelayRes[u32j];
                u8RdDelayRes[u32j] = u8Temp;
            }
        }
    }

    if (u8RdDelayIdx > 2)
    {
        u8RdDelayIdx = (u8RdDelayIdx / 2);//   -1;
    }
    else
    {
        u8RdDelayIdx = 0;
    }

    printf("\r\nDLL Delay Time Num = %d\r\n\r\n",
           u8RdDelayRes[u8RdDelayIdx]);
    SPIM_CtrlDLLDelayTime(pSPIMx, 0, 0, 0, 0, u8RdDelayRes[u8RdDelayIdx]);

    return 0;
}

static void SPIM_NVIC_Disable(SPIM_T *pSPIMx)
{
    if (pSPIMx == SPIM0)
    {
        NVIC_DisableIRQ(SPIM0_IRQn);
    }
    else if (pSPIMx == SPIM1)
    {
        NVIC_DisableIRQ(SPIM1_IRQn);
    }
}

static void InitSPIMPort(SPIM_T *pSPIMx)
{
    if (pSPIMx == SPIM0)
    {
		//SPIM and OTFC clock was enabled on secure-domain code
        /* Enable SPIM0 module clock */
        CLK_EnableModuleClock(SPIM0_MODULE);
        /* Enable OTFC0 module clock */
        CLK_EnableModuleClock(OTFC0_MODULE);
        //printf("OTFCCTL = %d, addr = 0x%08X\r\n", CLK->OTFCCTL, &CLK->OTFCCTL);

        /* Init SPIM0 multi-function pins */
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
    }
    else if (pSPIMx == SPIM1)
    {
     	//SPIM and OTFC clock was enabled on secure-domain code
        /* Enable SPIM1 module clock */
        CLK_EnableModuleClock(SPIM1_MODULE);
        /* Enable OTFC1 module clock */
        CLK_EnableModuleClock(OTFC1_MODULE);

        /* Init SPIM1 multi-function pins */
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
    }
}

/**
  * @brief      SPIM Connect HyperRAM Initial
  * @param[in]  pSPIM
  * @param[in]  u32CacheOn Enable/Disable SPIM Cache
  * @return     None
  * @details    This Macro Set Hyper Chip Initial Access Time
  * \hideinitializer
  */
void SPIM_HyperRAM_Init(SPIM_T *pSPIMx, uint32_t u32CacheOn)
{
    uint32_t u32Delay = 0;

    //SPIM0 PSC, PSC_n
    SET_GPIO_PG8();
    GPIO_SetMode(PG, BIT8, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PG, BIT8, GPIO_PUSEL_DISABLE);
    PG8 = 1;

    SET_GPIO_PG7();
    GPIO_SetMode(PG, BIT7, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PG, BIT7, GPIO_PUSEL_DISABLE);
    PG7 = 0;

    //SPIM1 PSC, PSC_n
    SET_GPIO_PH11();
    GPIO_SetMode(PH, BIT11, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PH, BIT11, GPIO_PUSEL_DISABLE);
    PH11 = 1;

    SET_GPIO_PH10();
    GPIO_SetMode(PH, BIT10, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PH, BIT11, GPIO_PUSEL_DISABLE);
    PH10 = 0;

    SPIM_NVIC_Disable(pSPIMx);

    InitSPIMPort(pSPIMx);

    //SPIM Def. Enable Cipher, First Disable the test.
    SPIM_DISABLE_CIPHER(pSPIMx);

    SPIM_SET_CLOCK_DIVIDER(pSPIMx, 1); /* Set SPIM clock as HCLK divided by 2 */
    //printf("SPIM Clock Divider = %ld\r\n", SPIM_GET_CLOCK_DIVIDER(pSPIMx));
    //SPIM_SET_RXCLKDLY_RDDLYSEL(pSPIMx, 0);      /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */

    SPIM_SET_HYPER_MODE(pSPIMx, 1);     //Enable HyperBus Mode
#if (SPIM_CACHE_EN == 1)
    SPIM_DISABLE_CACHE(pSPIMx);
#endif
	SPIM_Hyper_DefConfig(pSPIMx, 780, 7, 7);

//    SPIM_HyperRAMDLLDelayTimeTraining(pSPIMx);

    SPIM_Hyper_ResetHyperRAM(pSPIMx);

    //for (u32Delay = 0; u32Delay < 0x400; u32Delay++) {}

#if (SPIM_CACHE_EN == 1)
    if (u32CacheOn == 1)
    {
        SPIM_ENABLE_CACHE(pSPIMx);
    }
#endif

    //SPIM_HyperRAMDLLDelayTimeTraining(pSPIMx);
    SPIM_CtrlDLLDelayTime(pSPIMx, 0, 0, 0, 0, 2);	
    //SPIM_CtrlDLLDelayTime(pSPIMx, 0, 0, 0, 0, 1);		//for fast clock 500MHz RD design

    SPIM_Hyper_Write4Byte(pSPIMx, 0x1000, 0x12345678);

    SPIM_Hyper_EnterDirectMapMode(pSPIMx); // Hyper Mode Switch to Direct Map mode.
}
