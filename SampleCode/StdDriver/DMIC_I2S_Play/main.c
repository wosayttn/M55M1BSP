/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   This sample uses DMIC as audio input(MIC) and I2S as audio output(SPK).
 *          User can process audio data before output.
 *          Data have been transfered via PDMA/LPPDMA.
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
/*
 * This sample uses internal RC as APLL0 clock source and UART0 to print messages.
 * Users may need to do extra system configuration according to their system design.
 *
 * I/D-Cache
 *   I/D-Cache are enabled by default for better performance,
 *   users can define NVT_ICACHE_OFF/NVT_DCACHE_OFF in project setting to disable cache.
 * Debug UART
 *   Default is DEBUG_PORT=UART0 in project setting
 *   system_M55M1.c has three weak functions as below to configure debug UART port.
 *     SetDebugUartMFP, SetDebugUartCLK and InitDebugUart
 *   Users can re-implement these functions according to system design.
 */
#include <stdio.h>
#include <stdlib.h>
#include "NuMicro.h"
#include "BUFCTRL.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Define global constants                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define SAMPLE_RATE              (16000)
#define BUF_COUNT                (128)

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void I2STX_Init(S_BUFCTRL *psOutBufCtrl);
void I2STX_Start(void);
void I2STX_Stop(void);
void DMIC_Init(S_BUFCTRL *psInBufCtrl);
void DMIC_Start(void);
void DMIC_Stop(void);

/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
volatile S_BUFCTRL sInBufCtrl, sOutBufCtrl; // Buffer control handler.
int32_t    ai32InBuf[BUF_COUNT];      // Buffer array: store audio data receiced from DMIC(4Channel)
int32_t    ai32OutBuf[BUF_COUNT];         // Buffer array: store audio data ready to send to DPWM(2Channel)

// Microphone(DMIC)= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
#define DMIC_LPPDMA_CH       (2)

S_BUFCTRL *psDMIC_BufCtrl = NULL;           // Provide microphone input buffer control.
LPDSCT_T     sLPPDMA_DMIC[2];               // Provide LPPDMA description for ping-pong.

void DMIC_Init(S_BUFCTRL *psInBufCtrl)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
    // Select DMIC CLK source from PLL.
    CLK_SetModuleClock(DMIC0_MODULE, CLK_DMICSEL_DMIC0SEL_APLL1_DIV2, MODULE_NoMsk);
    // Enable DMIC clock.
    CLK_EnableModuleClock(DMIC0_MODULE);
    // DPWM IPReset.
    SYS_ResetModule(SYS_DMIC0RST);
    /* Lock protected registers */
    SYS_LockReg();
    // Set down sample rate 100 for quilty.(Suggest 96M used DMIC_CTL_DOWNSAMPLE_100_50 )
    DMIC_SET_DOWNSAMPLE(DMIC0, DMIC_DOWNSAMPLE_100);
    // Set DMIC sample rate.
    printf("DMIC SampleRate is %d\n", DMIC_SetSampleRate(DMIC0, SAMPLE_RATE));
    // Set channel's latch data falling type.
    DMIC_SET_LATCHEDGE_CH01(DMIC0, DMIC_LATCHDATA_CH01FR);
    DMIC_SET_LATCHEDGE_CH23(DMIC0, DMIC_LATCHDATA_CH23FR);
    // HPF control
    DMIC_EnableHPF(DMIC0, DMIC_CTL_CH01HPFEN_Msk | DMIC_CTL_CH23HPFEN_Msk);
    //Gain step
    DMIC_SetGainStep(DMIC0, DMIC_GAINSTEP_1_2);
    // MUTE control
    //DMIC_EnableMute(DMIC0, DMIC_CTL_CH0MUTE_Msk|DMIC_CTL_CH1MUTE_Msk|DMIC_CTL_CH2MUTE_Msk|DMIC_CTL_CH3MUTE_Msk);
    // Enable DMIC FIFO threshold interrupt.
    DMIC_ENABLE_FIFOTH_INT(DMIC0, 8);
    // Set FIFO Width 16bits
    DMIC_SetFIFOWidth(DMIC0, DMIC_FIFOWIDTH_16);

    DMIC_ClearFIFO(DMIC0);

    while (!DMIC_IS_FIFOEMPTY(DMIC0));

    DMIC_ResetDSP(DMIC0);  //SWRST

    //DMIC Gain Setting
    DMIC_SetDSPGainVolume(DMIC0, DMIC_CTL_CHEN0_Msk, 0xa800);//+48dB
    DMIC_SetDSPGainVolume(DMIC0, DMIC_CTL_CHEN1_Msk, 0xa800);//+48dB
    DMIC_SetDSPGainVolume(DMIC0, DMIC_CTL_CHEN2_Msk, 0xa800);//+48dB
    DMIC_SetDSPGainVolume(DMIC0, DMIC_CTL_CHEN3_Msk, 0xa800);//+48dB

    // MIC(RX) buffer description
    sLPPDMA_DMIC[0].CTL = ((((psInBufCtrl->u16BufCount) / 2) - 1) << PDMA_DSCT_CTL_TXCNT_Pos) | PDMA_WIDTH_32 | PDMA_SAR_FIX | PDMA_DAR_INC | PDMA_REQ_SINGLE | PDMA_OP_SCATTER;
    sLPPDMA_DMIC[0].SA = (uint32_t)(&DMIC0->FIFO);
    sLPPDMA_DMIC[0].DA = (uint32_t) & (psInBufCtrl->pai32Buf[0]);
    sLPPDMA_DMIC[0].NEXT = (uint32_t)&sLPPDMA_DMIC[1];
    sLPPDMA_DMIC[1].CTL = ((((psInBufCtrl->u16BufCount) / 2) - 1) << PDMA_DSCT_CTL_TXCNT_Pos) | PDMA_WIDTH_32 | PDMA_SAR_FIX | PDMA_DAR_INC | PDMA_REQ_SINGLE | PDMA_OP_SCATTER;
    sLPPDMA_DMIC[1].SA = (uint32_t)(&DMIC0->FIFO);
    sLPPDMA_DMIC[1].DA = (uint32_t) & (psInBufCtrl->pai32Buf[(psInBufCtrl->u16BufCount) / 2]);
    sLPPDMA_DMIC[1].NEXT = (uint32_t)&sLPPDMA_DMIC[0];
    // Open LPPDMA channel
    LPPDMA_Open(LPPDMA, (1 << DMIC_LPPDMA_CH));
    // Set TransMode
    LPPDMA_SetTransferMode(LPPDMA, DMIC_LPPDMA_CH, LPPDMA_DMIC0_RX, TRUE, (uint32_t)&sLPPDMA_DMIC[0]);
    // Enable interrupt
    LPPDMA_EnableInt(LPPDMA, DMIC_LPPDMA_CH, LPPDMA_INT_TRANS_DONE);
    // GPIO multi-function.
    SET_DMIC0_CLK_PE9();
    SET_DMIC0_DAT_PE8();
    SET_DMIC1_CLK_PE12();
    SET_DMIC1_DAT_PE11();
    // Config DMIC buffer control
    psDMIC_BufCtrl = psInBufCtrl;
}

void DMIC_Start(void)
{
    if (psDMIC_BufCtrl != NULL)
    {
        DMIC_ENABLE_CHANNEL(DMIC0, DMIC_CTL_CHEN0_Msk | DMIC_CTL_CHEN1_Msk | DMIC_CTL_CHEN2_Msk | DMIC_CTL_CHEN3_Msk);
        DMIC_ENABLE_LPPDMA(DMIC0);
    }
}

void DMIC_Stop(void)
{
    DMIC_DISABLE_LPPDMA(DMIC0);
    DMIC_DISABLE_CHANNEL(DMIC0, DMIC_CTL_CHEN0_Msk | DMIC_CTL_CHEN1_Msk | DMIC_CTL_CHEN2_Msk | DMIC_CTL_CHEN3_Msk);
}

// Speaker(DPWM) = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
#define I2S0TX_PDMA_CH     (3)

S_BUFCTRL *psI2STX_BufCtrl = NULL;            // Provide Speaker to output data.
DSCT_T     sPDMA_I2STX[2];                    // Provide PDMA description for ping-pong.

void I2STX_Init(S_BUFCTRL *psOutBufCtrl)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
    // Select I2S CLK source from PCLK.
    CLK_SetModuleClock(I2S0_MODULE, CLK_I2SSEL_I2S0SEL_PCLK1, MODULE_NoMsk);
    // Enable I2S clock.
    CLK_EnableModuleClock(I2S0_MODULE);
    // I2S IPReset.
    SYS_ResetModule(SYS_I2S0RST);
    /* Lock protected registers */
    SYS_LockReg();
    // Open I2S0 hardware IP
    I2S_Open(I2S0, I2S_MODE_SLAVE, SAMPLE_RATE, I2S_DATABIT_16, I2S_STEREO, I2S_FORMAT_I2S);
    // I2S0 Configuration
    I2S0->CTL0 |= I2S_CTL0_ORDER_Msk;
    //Enable MCLK
    I2S_EnableMCLK(I2S0, 12000000);

    printf("  DMIC to I2S0 through FIFO test\n");

    // Clear TX FIFO buffer
    I2S_CLR_TX_FIFO(I2S0);

    // SPK(TX) buffer description
    sPDMA_I2STX[0].CTL = (((psOutBufCtrl->u16BufCount / 2) - 1) << PDMA_DSCT_CTL_TXCNT_Pos) | PDMA_WIDTH_32 | PDMA_SAR_INC | PDMA_DAR_FIX | PDMA_REQ_SINGLE | PDMA_OP_SCATTER;
    sPDMA_I2STX[0].SA = (uint32_t) & (psOutBufCtrl->pai32Buf[0]);
    sPDMA_I2STX[0].DA = (uint32_t)(&I2S0->TXFIFO);
    sPDMA_I2STX[0].NEXT = (uint32_t)&sPDMA_I2STX[1];
    sPDMA_I2STX[1].CTL = (((psOutBufCtrl->u16BufCount / 2) - 1) << PDMA_DSCT_CTL_TXCNT_Pos) | PDMA_WIDTH_32 | PDMA_SAR_INC | PDMA_DAR_FIX | PDMA_REQ_SINGLE | PDMA_OP_SCATTER;
    sPDMA_I2STX[1].SA = (uint32_t) & (psOutBufCtrl->pai32Buf[psOutBufCtrl->u16BufCount / 2]);
    sPDMA_I2STX[1].DA = (uint32_t)(&I2S0->TXFIFO);
    sPDMA_I2STX[1].NEXT = (uint32_t)&sPDMA_I2STX[0];
    // Open PDMA channel
    PDMA_Open(PDMA0, (1 << I2S0TX_PDMA_CH));
    // Set TransMode
    PDMA_SetTransferMode(PDMA0, I2S0TX_PDMA_CH, PDMA_I2S0_TX, TRUE, (uint32_t)&sPDMA_I2STX[0]);
    // Enable interrupt
    PDMA_EnableInt(PDMA0, I2S0TX_PDMA_CH, PDMA_INT_TRANS_DONE);

    // GPIO multi-function.
    SET_I2S0_BCLK_PI6();
    SET_I2S0_MCLK_PI7();
    SET_I2S0_DI_PI8();
    SET_I2S0_DO_PI9();
    SET_I2S0_LRCK_PI10();
    // Config DPWM(Speaker) buffer control
    psI2STX_BufCtrl = psOutBufCtrl;
}

void I2STX_Start(void)
{
    if (psI2STX_BufCtrl != NULL)
    {
        I2S_ENABLE_TX(I2S0);
        I2S_ENABLE_TXDMA(I2S0);
    }
}

void I2STX_Stop(void)
{
    I2S_DISABLE_TX(I2S0);
    I2S_DISABLE_TXDMA(I2S0);
}
// PMDA =========================================================================================================
NVT_ITCM void PDMA0_IRQHandler(void)
{
    uint32_t u32TDStatus = PDMA_GET_TD_STS(PDMA0);

    if (u32TDStatus & (1 << I2S0TX_PDMA_CH))
    {
        PDMA_CLR_TD_FLAG(PDMA0, (1 << I2S0TX_PDMA_CH));
        psI2STX_BufCtrl->u16DataCount -= (psI2STX_BufCtrl->u16BufCount / 2);

        if ((psI2STX_BufCtrl->u16ReadIdx += (psI2STX_BufCtrl->u16BufCount / 2)) >= psI2STX_BufCtrl->u16BufCount)
            psI2STX_BufCtrl->u16ReadIdx = 0;
    }
}
// LPPMDA =========================================================================================================
NVT_ITCM void LPPDMA_IRQHandler(void)
{
    uint32_t u32TDStatus = LPPDMA_GET_TD_STS(LPPDMA);

    if (u32TDStatus & (1 << DMIC_LPPDMA_CH))
    {
        LPPDMA_CLR_TD_FLAG(LPPDMA, (1 << DMIC_LPPDMA_CH));
        psDMIC_BufCtrl->u16DataCount += (psDMIC_BufCtrl->u16BufCount / 2);

        if ((psDMIC_BufCtrl->u16WriteIdx += (psDMIC_BufCtrl->u16BufCount / 2)) >= psDMIC_BufCtrl->u16BufCount)
            psDMIC_BufCtrl->u16WriteIdx = 0;
    }
}

static void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Enable PLL0 180MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_180MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to PLL0 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0);

    /* Set HCLK2 divide 2 */
    CLK_SET_HCLK2DIV(2);

    /* Set PCLKx divide 2 */
    CLK_SET_PCLK0DIV(2);
    CLK_SET_PCLK1DIV(2);
    CLK_SET_PCLK2DIV(2);
    CLK_SET_PCLK3DIV(2);
    CLK_SET_PCLK4DIV(2);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    // PDMA/LPPDMA Initial.
    // Enable PDMA clock.
    CLK_EnableModuleClock(PDMA0_MODULE);
    CLK_EnableModuleClock(LPPDMA0_MODULE);
    // Reset PDMA module
    SYS_ResetModule(SYS_PDMA0RST);
    SYS_ResetModule(SYS_LPPDMA0RST);
    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    int32_t i32Data[4];
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    initialise_monitor_handles();
#endif

    printf("System core clock = %d\n", SystemCoreClock);
    printf("DMIC as audio input(MIC) and I2S as audio output(SPK).\n");

    // Enable PDMA's NVIC
    NVIC_EnableIRQ(PDMA0_IRQn);
    NVIC_EnableIRQ(LPPDMA_IRQn);

    // These defines are from  BUFCTRL.h for buffer control in this samle.
    // Buffer control handler configuration.
    BUFCTRL_CFG((&sInBufCtrl), ai32InBuf, sizeof(ai32InBuf) / sizeof(uint32_t));
    BUFCTRL_CFG((&sOutBufCtrl), ai32OutBuf, sizeof(ai32OutBuf) / sizeof(uint32_t));
    // full empty data into output buffer.
    sOutBufCtrl.u16DataCount = sOutBufCtrl.u16BufCount;

    // Initiate microphone.
    DMIC_Init((S_BUFCTRL *)&sInBufCtrl);
    // Initiate speaker.
    I2STX_Init((S_BUFCTRL *)&sOutBufCtrl);

    // Start microphone.
    DMIC_Start();
    // Start speaker.
    I2STX_Start();

    // while loop for processing.
    while (1)
    {
        while (BUFCTRL_GET_COUNT((&sInBufCtrl)) >= 4 && !BUFCTRL_IS_FULL((&sOutBufCtrl)))
        {
            BUFCTRL_READ((&sInBufCtrl), &i32Data[0]);
            BUFCTRL_READ((&sInBufCtrl), &i32Data[1]);
            BUFCTRL_READ((&sInBufCtrl), &i32Data[2]);
            BUFCTRL_READ((&sInBufCtrl), &i32Data[3]);
            // 4 channel mixer to 2 channe
            //          i32Data[0] = i32Data[0]+i32Data[2];
            //          i32Data[1] = i32Data[1]+i32Data[3];
            BUFCTRL_WRITE((&sOutBufCtrl), i32Data[0]);
            BUFCTRL_WRITE((&sOutBufCtrl), i32Data[1]);
            BUFCTRL_WRITE((&sOutBufCtrl), i32Data[2]);
            BUFCTRL_WRITE((&sOutBufCtrl), i32Data[3]);
        }
    };
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
