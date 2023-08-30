/**************************************************************************//**
 * @file    main.c
 * @version V1.00
 * @brief   This sample uses DMIC as audio input(MIC) and record to WAV file.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
/*
 * This sample uses internal RC (HIRC) as APLL0 clock source and UART0 to print messages.
 * Users may need to do extra system configuration according to their system design.
 *
 * I/D-Cache
 *   I-Cache are enabled by default for better performance,
 *   users can define NVT_DCACHE_ON in project setting to enable D-Cache.
 * Debug UART
 *   Default is DEBUG_PORT=UART0 in project setting
 *   system_M55M1.c has three weak functions as below to configure UART0 debug port.
 *     SetDebugUartMFP, SetDebugUartCLK and InitDebugUart
 *   Users can re-implement these functions according to system design.
 */
#include <stdio.h>
#include <stdlib.h>
#include "NuMicro.h"
#include "BUFCTRL.h"
#include "WavFileUtil.h"
#include "ff.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Define global constants                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define SAMPLE_RATE              (16000)
#define BUF_COUNT                (128)

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void DMIC_Init(S_BUFCTRL *psInBufCtrl);
void DMIC_Start(void);
void DMIC_Stop(void);

/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
volatile S_BUFCTRL sInBufCtrl, sOutBufCtrl; // Buffer control handler.
int32_t    ai32InBuf[BUF_COUNT];      // Buffer array: store audio data receiced from DMIC(4Channel)
int32_t    ai32OutBuf[BUF_COUNT];         // Buffer array: store audio data ready to send to DPWM(2Channel)

#define DMIC_LPPDMA_CH       (2)

S_BUFCTRL *psDMIC_BufCtrl = NULL;           // Provide microphone input buffer control.
LPDSCT_T     sLPPDMA_DMIC[2];               // Provide LPPDMA description for ping-pong.

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

// Microphone(DMIC)= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
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

S_WavFileWriteInfo  sWavFileInfo;

// Close WAV media
static int32_t WAVWrite_Close(void)
{
    // Finalize WavFileUtil
    if (!WavFileUtil_Write_Finish(&sWavFileInfo))
    {
        printf("WavFileUtil_Write_Finish failed");
        return -3;
    } // if

    printf("Close Done: DMIC_WAVRecorder.wav");
    return 0;
} // WAVWrite_Close()

static int32_t WAVWrite_Open(void)
{
    // Initialize WavFileUtil
    S_WavFileWriteInfo  *psWavFileInfo = &sWavFileInfo;
    E_WavFormatTag      eFormatTag;
    int32_t             eError = 0;
    UINT16                  u16BitsPerSample;
    UINT32              u32ChannelNum = 2, u32SampleRate = 16000;

    if (!WavFileUtil_Write_Initialize(psWavFileInfo, "0:\\DMIC_WAVRecorder.wav"))
    {
        eError = -1;
        goto EndOfOpen;
    } // if

    eFormatTag = eWAVE_FORMAT_PCM;
    u16BitsPerSample = 16;  // libnmedia only supports PCM_S16LE

    if (!WavFileUtil_Write_SetFormat(psWavFileInfo, eFormatTag,
                                     u32ChannelNum, u32SampleRate,
                                     u16BitsPerSample))
    {
        eError = -2;
        goto EndOfOpen;
    } // if

EndOfOpen:

    if (eError != 0)
        // Finalize WavFileUtil and free media resource
        WAVWrite_Close();
    else
        printf("Open Done: DMIC_WAVRecorder.wav");

    return eError;
} // WAVWrite_Open()

// Write chunk
// (Application should do sync on demand by itself)
// [out]eNM_ERROR_EOM if end of media
static int32_t WAVWrite_WriteChunk(const BYTE *pDataVAddr, uint32_t u32DataSize)
{
    if (u32DataSize > 0)
    {
        if (pDataVAddr == 0)
            return -4;

        if (!WavFileUtil_Write_WriteData(&sWavFileInfo, pDataVAddr, u32DataSize))
        {
            printf("WavFileUtil_Write_WriteData(%uB) failed", u32DataSize);
            return -5;
        } // if
    } // if

    return 0;
} // WAVWrite_WriteChunk()

static void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable PLL0 180MHz clock from HIRC and switch SCLK clock source to PLL0 */
    CLK_SetBusClock(CLK_SCLKSEL_SCLKSEL_APLL0, FREQ_180MHZ);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable UART0 module clock */
    SetDebugUartCLK();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    SetDebugUartMFP();

    // LPPDMA Initial.
    CLK_EnableModuleClock(LPPDMA0_MODULE);
    CLK_EnableModuleClock(LPSRAM0_MODULE);
    SYS_ResetModule(SYS_LPPDMA0RST);
    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    TCHAR sd_path[] = { '0', ':', 0 };    /* SD drive started from 0 */
    int32_t i32Data[4];
    uint32_t u32DataCnt = 0;
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init Debug UART to 115200-8N1 for print message */
    InitDebugUart();

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(OS_USE_SEMIHOSTING)
    initialise_monitor_handles();
#endif

    printf("System core clock = %d\n", SystemCoreClock);

    printf("DMIC as audio input(MIC) and record to WAV file.\n");

    // Enable PDMA's NVIC
    NVIC_EnableIRQ(LPPDMA_IRQn);

    // These defines are from  BUFCTRL.h for buffer control in this samle.
    // Buffer control handler configuration.
    BUFCTRL_CFG((&sInBufCtrl), ai32InBuf, sizeof(ai32InBuf) / sizeof(uint32_t));
    BUFCTRL_CFG((&sOutBufCtrl), ai32OutBuf, sizeof(ai32OutBuf) / sizeof(uint32_t));
    // full empty data into output buffer.
    sOutBufCtrl.u16DataCount = sOutBufCtrl.u16BufCount;

    // Initiate microphone.
    DMIC_Init((S_BUFCTRL *)&sInBufCtrl);
    /* Configure FATFS */
    SDH_Open_Disk(SDH0, CardDetect_From_GPIO);
    f_chdrive(sd_path);          /* set default path */

    // Initiate WAV.
    WAVWrite_Open();

    // Start microphone.
    DMIC_Start();

    // while loop for processing.
    while (u32DataCnt < 16 * SAMPLE_RATE)
    {
        while (BUFCTRL_GET_COUNT((&sInBufCtrl)) >= 4)
        {
            BUFCTRL_READ((&sInBufCtrl), &i32Data[0]);
            BUFCTRL_READ((&sInBufCtrl), &i32Data[1]);
            BUFCTRL_READ((&sInBufCtrl), &i32Data[2]);
            BUFCTRL_READ((&sInBufCtrl), &i32Data[3]);
            // WAV Write.
            WAVWrite_WriteChunk((const BYTE *)i32Data, 16);
            u32DataCnt += 16;
        }
    };

    WAVWrite_Close();

    SDH_Close_Disk(SDH0);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
