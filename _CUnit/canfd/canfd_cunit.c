/*---------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/* Copyright (C) 2020 Nuvoton Technology Corp.                                                             */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   canfd_cunit_test.c                                                                                    */
/*            The test function of VIC for CUnit.                                                         */
/* Project:   M253                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"
#include "canfd_cunit.h"



#define UART_CH_NUM_MAX 5

extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;


CANFD_FD_MSG_T      g_sRxFifo0MsgFrame;
CANFD_FD_MSG_T      g_sRxFifo1MsgFrame;
volatile uint8_t   g_u8RxFifo0CompleteFlag = 0;
volatile uint8_t   g_u8RxFifo1CompleteFlag = 0;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int suite_success_init(void)
{
    return 0;
}
int suite_success_clean(void)
{
    return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function: function_name                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               param - [in/out], parameter_description                                                   */
/*               param - [in/out], parameter_description                                                   */
/*                                                                                                         */
/* Returns:      the_value                                                                                 */
/* Side effects: the_effect                                                                                */
/* Description:                                                                                            */
/*               description                                                                               */
/*---------------------------------------------------------------------------------------------------------*/


CU_SuiteInfo CANFD_suites[] =
{
    {"CAN FD API", suite_success_init, suite_success_clean, NULL, NULL, CANFD_ApiTests},
    {"CAN FD MACRO", suite_success_init, suite_success_clean, NULL, NULL, CANFD_MacroTests},
    CU_SUITE_INFO_NULL
};

const uint32_t au32CANFD_INTEnableSel[30] =
{
    CANFD_IE_RF0NE_Msk,       //0 : Rx FIFO 0 New Message Interrupt
    CANFD_IE_RF0WE_Msk,       //1 : Rx FIFO 0 Watermark Reached Interrupt
    CANFD_IE_RF0FE_Msk,       //2 : Rx FIFO 0 Full Interrupt
    CANFD_IE_RF0LE_Msk,       //3 : Rx FIFO 0 Message Lost Interrupt
    CANFD_IE_RF1NE_Msk,       //4 : Rx FIFO 1 New Message Interrupt
    CANFD_IE_RF1WE_Msk,       //5 : Rx FIFO 1 Watermark Reached Interrupt
    CANFD_IE_RF1FE_Msk,       //6 : Rx FIFO 1 Full Interrupt
    CANFD_IE_RF1LE_Msk,       //7 : Rx FIFO 1 Message Lost Interrupt
    CANFD_IE_HPME_Msk,        //8 : High Priority Message Interrupt
    CANFD_IE_TCE_Msk,         //9 : Transmission Completed Interrupt
    CANFD_IE_TCFE_Msk,        //10: Transmission Cancellation Finished Interrupt
    CANFD_IE_TFEE_Msk,        //11: Tx FIFO Empty Interrupt
    CANFD_IE_TEFNE_Msk,       //12: Tx Event FIFO New Entry Interrupt
    CANFD_IE_TEFWE_Msk,       //13: Tx Event FIFO Watermark Reached Interrupt
    CANFD_IE_TEFFE_Msk,       //14: Tx Event FIFO Full Interrupt
    CANFD_IE_TEFLE_Msk,       //15: Tx Event FIFO Event Lost Interrupt
    CANFD_IE_TSWE_Msk,        //16: Timestamp Wraparound Interrupt
    CANFD_IE_MRAFE_Msk,       //17: Message RAM Access Failure Interrupt
    CANFD_IE_TOOE_Msk,        //18: Timeout Occurred Interrupt
    CANFD_IE_DRXE_Msk,        //19: Message stored to Dedicated Rx Buffer Interrupt
    CANFD_IE_BECE_Msk,        //20: Bit Error Corrected Interrupt
    CANFD_IE_BEUE_Msk,        //21: Bit Error Uncorrected Interrupt
    CANFD_IE_ELOE_Msk,        //22: Error Logging Overflow Interrupt
    CANFD_IE_EPE_Msk,         //23: Error Passive Interrupt
    CANFD_IE_EWE_Msk,         //24: Warning Status Interrupt
    CANFD_IE_BOE_Msk,         //25: Bus_Off Status Interrupt
    CANFD_IE_WDIE_Msk,        //26: Watchdog Interrupt
    CANFD_IE_PEAE_Msk,        //27: Protocol Error in Arbitration Phase Interrupt
    CANFD_IE_PEDE_Msk,        //28: Protocol Error in Data Phase Interrupt
    CANFD_IE_ARAE_Msk         //29: Access to Reserved Address Interrupt
};


/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle CAN FD0 Line0 interrupt event                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void CANFD00_IRQHandler(void)
{
    if (CANFD0->IR & CANFD_IR_RF0N_Msk)
    {
        /*Clear the Interrupt flag */
        CANFD_ClearStatusFlag(CANFD0, CANFD_IR_TOO_Msk | CANFD_IR_RF0N_Msk);
        CANFD_ReadRxFifoMsg(CANFD0, 0, &g_sRxFifo0MsgFrame);
        g_u8RxFifo0CompleteFlag = 1;
    }

    if (CANFD0->IR & CANFD_IR_RF1N_Msk)
    {
        /*Clear the Interrupt flag */
        CANFD_ClearStatusFlag(CANFD0, CANFD_IR_TOO_Msk | CANFD_IR_RF1N_Msk);
        CANFD_ReadRxFifoMsg(CANFD0, 1, &g_sRxFifo1MsgFrame);
        g_u8RxFifo1CompleteFlag = 1;
    }

}
/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle CAN FD0 Line0 interrupt event                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void CANFD10_IRQHandler(void)
{
    if (CANFD1->IR & CANFD_IR_RF0N_Msk)
    {
        /*Clear the Interrupt flag */
        CANFD_ClearStatusFlag(CANFD1, CANFD_IR_TOO_Msk | CANFD_IR_RF0N_Msk);
        CANFD_ReadRxFifoMsg(CANFD1, 0, &g_sRxFifo0MsgFrame);
        g_u8RxFifo0CompleteFlag = 1;
    }

    if (CANFD1->IR & CANFD_IR_RF1N_Msk)
    {
        /*Clear the Interrupt flag */
        CANFD_ClearStatusFlag(CANFD1, CANFD_IR_TOO_Msk | CANFD_IR_RF1N_Msk);
        CANFD_ReadRxFifoMsg(CANFD1, 1, &g_sRxFifo1MsgFrame);
        g_u8RxFifo1CompleteFlag = 1;
    }

}

void CANFD_IPReset(CANFD_T *psCanfd)
{
     /* Unlock protected registers */
    SYS_UnlockReg(); 
    switch ((uint32_t)psCanfd)
    {
        case (uint32_t)CANFD0:
            /* Reset CAN FD0 IP*/
            SYS_ResetModule(SYS_CANFD0RST);
            break;
        case (uint32_t)CANFD1:
            /* Reset CAN FD0 IP*/
            SYS_ResetModule(SYS_CANFD1RST);
            break;
        default :
            break;

    }
    /* Lock protected registers */
    SYS_LockReg();
}
uint32_t CANFD_CalculateDataBitRate(CANFD_T *psCanfd)
{
    uint32_t u32CanClk,u32DateBitRate, u32Div;
    uint8_t u8DBRP, u8DTSEG1, u8DTSEG2;
     
    if(CLK_GetModuleClockSource(CANFD0_MODULE) == (CLK_CANFDSEL_CANFD0SEL_HXT >> CLK_CANFDSEL_CANFD0SEL_Pos) )
     u32CanClk = CLK_GetHXTFreq();
    else if(CLK_GetModuleClockSource(CANFD0_MODULE) == (CLK_CANFDSEL_CANFD1SEL_APLL0_DIV2 >> CLK_CANFDSEL_CANFD0SEL_Pos) )
     u32CanClk = CLK_GetAPLL0ClockFreq()/2;
    else if(CLK_GetModuleClockSource(CANFD0_MODULE) == (CLK_CANFDSEL_CANFD0SEL_HCLK0 >> CLK_CANFDSEL_CANFD0SEL_Pos) )
     u32CanClk = CLK_GetHCLK0Freq();
    else if(CLK_GetModuleClockSource(CANFD0_MODULE) == (CLK_CANFDSEL_CANFD0SEL_HIRC >> CLK_CANFDSEL_CANFD0SEL_Pos) )
     u32CanClk = __HIRC; 
    else
     u32CanClk = __HIRC48M / 4; 

    u32Div = ((CLK->CANFDDIV & CLK_CANFDDIV_CANFD0DIV_Msk) >> CLK_CANFDDIV_CANFD0DIV_Pos) + 1;
    /* Get CANF D0 clock divider number */
    u32CanClk = u32CanClk / u32Div;
  
    u8DBRP = ((psCanfd->DBTP & CANFD_DBTP_DBRP_Msk) >> CANFD_DBTP_DBRP_Pos) + 1;
    u8DTSEG1 = ((psCanfd->DBTP & CANFD_DBTP_DTSEG1_Msk) >> CANFD_DBTP_DTSEG1_Pos);
    u8DTSEG2 = ((psCanfd->DBTP & CANFD_DBTP_DTSEG2_Msk) >> CANFD_DBTP_DTSEG2_Pos);


    u32DateBitRate = 48000000 / u32Div;
    u32DateBitRate = u32DateBitRate / ((u8DTSEG1 + u8DTSEG2 + 3) * u8DBRP);
    return u32DateBitRate;
}

uint32_t CANFD_CalculateNominalBitRate(CANFD_T *psCanfd)
{

    uint32_t u32CanClk,u32NominalBitRate, u32Div;
    uint8_t u8NBRP, u8NTSEG1, u8NTSEG2;

     if(CLK_GetModuleClockSource(CANFD0_MODULE) == (CLK_CANFDSEL_CANFD0SEL_HXT >> CLK_CANFDSEL_CANFD0SEL_Pos) )
     u32CanClk = CLK_GetHXTFreq();
    else if(CLK_GetModuleClockSource(CANFD0_MODULE) == (CLK_CANFDSEL_CANFD1SEL_APLL0_DIV2 >> CLK_CANFDSEL_CANFD0SEL_Pos) )
     u32CanClk = CLK_GetAPLL0ClockFreq()/2;
    else if(CLK_GetModuleClockSource(CANFD0_MODULE) == (CLK_CANFDSEL_CANFD0SEL_HCLK0 >> CLK_CANFDSEL_CANFD0SEL_Pos) )
     u32CanClk = CLK_GetHCLK0Freq();
    else if(CLK_GetModuleClockSource(CANFD0_MODULE) == (CLK_CANFDSEL_CANFD0SEL_HIRC >> CLK_CANFDSEL_CANFD0SEL_Pos) )
     u32CanClk = __HIRC; 
    else
     u32CanClk = __HIRC48M / 4; 
    /* Get CANF D0 clock divider number */
    u32Div = ((CLK->CANFDDIV & CLK_CANFDDIV_CANFD0DIV_Msk) >> CLK_CANFDDIV_CANFD0DIV_Pos) + 1;
    u32CanClk = u32CanClk / u32Div;
    u8NBRP = ((psCanfd->NBTP & CANFD_NBTP_NBRP_Msk) >> CANFD_NBTP_NBRP_Pos) + 1;
    u8NTSEG1 = ((psCanfd->NBTP & CANFD_NBTP_NTSEG1_Msk) >> CANFD_NBTP_NTSEG1_Pos);
    u8NTSEG2 = ((psCanfd->NBTP & CANFD_NBTP_NTSEG2_Msk) >> CANFD_NBTP_NTSEG2_Pos);
    u32NominalBitRate = 48000000 / u32Div;
    u32NominalBitRate = u32NominalBitRate / ((u8NTSEG1 + u8NTSEG2 + 3) * u8NBRP);
    return u32NominalBitRate;

}


void CANFD_CalculateRamAddress1(CANFD_RAM_PART_T *psConfigAddr, CANFD_ELEM_SIZE_T *psConfigSize)
{
    uint32_t u32RamAddrOffset = 0;

    /* Get the Standard Message ID Filter element address */
    if (psConfigSize->u32SIDFC > 0)
    {
        psConfigAddr->u32SIDFC_FLSSA = 0;
        u32RamAddrOffset += psConfigSize->u32SIDFC * sizeof(CANFD_STD_FILTER_T);
    }

    /* Get the Standard Message ID Filter element address */
    if (psConfigSize->u32XIDFC > 0)
    {
        psConfigAddr->u32XIDFC_FLESA = u32RamAddrOffset;
        u32RamAddrOffset += psConfigSize->u32XIDFC * sizeof(CANFD_EXT_FILTER_T);
    }

    /* Get the Tx Buffer element address */
    if (psConfigSize->u32TxBuf > 0)
    {
        psConfigAddr->u32TXBC_TBSA = u32RamAddrOffset;
        u32RamAddrOffset += psConfigSize->u32TxBuf * sizeof(CANFD_BUF_T);
    }

    /* Get the Rx Buffer element address */
    if (psConfigSize->u32RxBuf > 0)
    {
        psConfigAddr->u32RXBC_RBSA = u32RamAddrOffset;
        u32RamAddrOffset += psConfigSize->u32RxBuf * sizeof(CANFD_BUF_T);
    }

    /* Get the Rx FIFO0 element address */
    if (psConfigSize->u32RxFifo0 > 0)
    {
        psConfigAddr->u32RXF0C_F0SA = u32RamAddrOffset;
        u32RamAddrOffset += psConfigSize->u32RxFifo0 * sizeof(CANFD_BUF_T);
    }

    /* Get the Rx FIFO1 element address */
    if (psConfigSize->u32RxFifo1 > 0)
    {
        psConfigAddr->u32RXF1C_F1SA = u32RamAddrOffset;
        u32RamAddrOffset += psConfigSize->u32RxFifo1 * sizeof(CANFD_BUF_T);
    }

    /* Get the Rx FIFO1 element address */
    if (psConfigSize->u32TxEventFifo > 0)
    {
        psConfigAddr->u32TXEFC_EFSA = u32RamAddrOffset;
        u32RamAddrOffset += psConfigSize->u32TxEventFifo *  sizeof(CANFD_EXT_FILTER_T);
    }
}

void TestFunc_CANFD_TxRx_Buffer_Address(void)
{
    CANFD_FD_T sCANFD_Config;

    CANFD_IPReset(CANFD0);

    CANFD_RunToNormal(CANFD0, FALSE);
    /*Get the CAN configuration value*/
    CANFD0->CCCR |= CANFD_CCCR_CCE_Msk;
    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_MODE);

    /*Configures the Standard ID Filter element */
    CANFD_ConfigSIDFC(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize);
    CU_ASSERT(((CANFD0->SIDFC & CANFD_SIDFC_LSS_Msk) >> CANFD_SIDFC_LSS_Pos) == sCANFD_Config.sElemSize.u32SIDFC);
    CU_ASSERT((CANFD0->SIDFC & CANFD_SIDFC_FLSSA_Msk) == sCANFD_Config.sMRamStartAddr.u32SIDFC_FLSSA);
    /*Configures the Extended ID Filter element */
    CANFD_ConfigXIDFC(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize);
    CU_ASSERT(((CANFD0->XIDFC & CANFD_XIDFC_LSE_Msk) >> CANFD_XIDFC_LSE_Pos) == sCANFD_Config.sElemSize.u32XIDFC);
    CU_ASSERT((CANFD0->XIDFC & CANFD_XIDFC_FLESA_Msk) == sCANFD_Config.sMRamStartAddr.u32XIDFC_FLESA);
    /*Configures the Tx Buffer element */
    CANFD_InitTxDBuf(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, eCANFD_BYTE64);
    CU_ASSERT(((CANFD0->TXBC & CANFD_TXBC_NDTB_Msk) >> CANFD_TXBC_NDTB_Pos) == sCANFD_Config.sElemSize.u32TxBuf);
    CU_ASSERT((CANFD0->TXBC & CANFD_TXBC_TBSA_Msk) == sCANFD_Config.sMRamStartAddr.u32TXBC_TBSA);
    CU_ASSERT((CANFD0->TXESC & CANFD_TXESC_TBDS_Msk) == eCANFD_BYTE64);
    /*Configures the Rx Buffer element */
    CANFD_InitRxDBuf(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, eCANFD_BYTE64);
    CU_ASSERT((CANFD0->RXBC & CANFD_RXBC_RBSA_Msk) == sCANFD_Config.sMRamStartAddr.u32RXBC_RBSA);
    CU_ASSERT(((CANFD0->RXESC & CANFD_RXESC_RBDS_Msk) >> CANFD_RXESC_RBDS_Pos)  == eCANFD_BYTE64);
    /*Configures the Rx Fifo0 element */
    CANFD_InitRxFifo(CANFD0, 0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, 0, eCANFD_BYTE64);
    CU_ASSERT(((CANFD0->RXF0C & CANFD_RXF0C_F0S_Msk) >> CANFD_RXF0C_F0S_Pos) == sCANFD_Config.sElemSize.u32RxFifo0);
    CU_ASSERT(((CANFD0->RXF0C & CANFD_RXF0C_F0WM_Msk) >> CANFD_RXF0C_F0WM_Pos) == 0);
    CU_ASSERT((CANFD0->RXF0C &  CANFD_RXF0C_F0SA_Msk) == sCANFD_Config.sMRamStartAddr.u32RXF0C_F0SA);
    CU_ASSERT(((CANFD0->RXESC & CANFD_RXESC_F0DS_Msk) >> CANFD_RXESC_F0DS_Pos) == eCANFD_BYTE64);

    /*Configures the Rx Fifo1 element */
    CANFD_InitRxFifo(CANFD0, 1, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, 0, eCANFD_BYTE64);
    CU_ASSERT(((CANFD0->RXF1C & CANFD_RXF1C_F1S_Msk) >> CANFD_RXF1C_F1S_Pos) == sCANFD_Config.sElemSize.u32RxFifo1);
    CU_ASSERT(((CANFD0->RXF1C & CANFD_RXF1C_F1WM_Msk) >> CANFD_RXF1C_F1WM_Pos) == 0);
    CU_ASSERT((CANFD0->RXF1C & CANFD_RXF1C_F1SA_Msk) == sCANFD_Config.sMRamStartAddr.u32RXF1C_F1SA);
    CU_ASSERT(((CANFD0->RXESC & CANFD_RXESC_F1DS_Msk) >> CANFD_RXESC_F1DS_Pos) == eCANFD_BYTE64);

    /*Configures the Tx Event FIFO element */
    CANFD_InitTxEvntFifo(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, 0);
    CU_ASSERT(((CANFD0->TXEFC & CANFD_TXEFC_EFS_Msk) >> CANFD_TXEFC_EFS_Pos) == sCANFD_Config.sElemSize.u32TxEventFifo);
    CU_ASSERT((CANFD0->TXEFC & CANFD_TXEFC_EFSA_Msk) == sCANFD_Config.sMRamStartAddr.u32TXEFC_EFSA);
    CU_ASSERT(((CANFD0->TXEFC & CANFD_TXEFC_EFWM_Msk) >> CANFD_TXEFC_EFWM_Pos) == 0);

    CANFD_IPReset(CANFD0);
    CANFD_RunToNormal(CANFD0, FALSE);
    /*Get the CAN configuration value*/
    CANFD0->CCCR |= CANFD_CCCR_CCE_Msk;

    /* CAN FD Standard message ID elements as 12 elements    */
    sCANFD_Config.sElemSize.u32SIDFC = 8;
    /* CAN FD Extended message ID elements as 10 elements    */
    sCANFD_Config.sElemSize.u32XIDFC = 12;
    /* CAN FD TX Buffer elements as 3 elements    */
    sCANFD_Config.sElemSize.u32TxBuf = 1;
    /* CAN FD RX Buffer elements as 3 elements    */
    sCANFD_Config.sElemSize.u32RxBuf = 5;
    /* CAN FD RX FIFO0 elements as 3 elements    */
    sCANFD_Config.sElemSize.u32RxFifo0 = 1;
    /* CAN FD RX FIFO1 elements as 3 elements    */
    sCANFD_Config.sElemSize.u32RxFifo1 = 5;
    /* CAN FD TX Event FOFI elements as 3 elements    */
    sCANFD_Config.sElemSize.u32TxEventFifo = 5;
    /*Calculates the CAN FD RAM buffer address*/
    CANFD_CalculateRamAddress1(&sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize);

    /*Configures the Standard ID Filter element */
    CANFD_ConfigSIDFC(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize);
    CU_ASSERT(((CANFD0->SIDFC & CANFD_SIDFC_LSS_Msk) >> CANFD_SIDFC_LSS_Pos) == sCANFD_Config.sElemSize.u32SIDFC);
    CU_ASSERT((CANFD0->SIDFC & CANFD_SIDFC_FLSSA_Msk) == sCANFD_Config.sMRamStartAddr.u32SIDFC_FLSSA);
    /*Configures the Extended ID Filter element */
    CANFD_ConfigXIDFC(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize);
    CU_ASSERT(((CANFD0->XIDFC & CANFD_XIDFC_LSE_Msk) >> CANFD_XIDFC_LSE_Pos) == sCANFD_Config.sElemSize.u32XIDFC);
    CU_ASSERT((CANFD0->XIDFC & CANFD_XIDFC_FLESA_Msk) == sCANFD_Config.sMRamStartAddr.u32XIDFC_FLESA);
    /*Configures the Tx Buffer element */
    CANFD_InitTxDBuf(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, eCANFD_BYTE64);
    CU_ASSERT(((CANFD0->TXBC & CANFD_TXBC_NDTB_Msk) >> CANFD_TXBC_NDTB_Pos) == sCANFD_Config.sElemSize.u32TxBuf);
    CU_ASSERT((CANFD0->TXBC & CANFD_TXBC_TBSA_Msk) == sCANFD_Config.sMRamStartAddr.u32TXBC_TBSA);
    CU_ASSERT((CANFD0->TXESC & CANFD_TXESC_TBDS_Msk) == eCANFD_BYTE64);
    /*Configures the Rx Buffer element */
    CANFD_InitRxDBuf(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, eCANFD_BYTE64);
    CU_ASSERT((CANFD0->RXBC & CANFD_RXBC_RBSA_Msk) == sCANFD_Config.sMRamStartAddr.u32RXBC_RBSA);
    CU_ASSERT(((CANFD0->RXESC & CANFD_RXESC_RBDS_Msk) >> CANFD_RXESC_RBDS_Pos)  == eCANFD_BYTE64);
    /*Configures the Rx Fifo0 element */
    CANFD_InitRxFifo(CANFD0, 0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, 0, eCANFD_BYTE64);
    CU_ASSERT(((CANFD0->RXF0C & CANFD_RXF0C_F0S_Msk) >> CANFD_RXF0C_F0S_Pos) == sCANFD_Config.sElemSize.u32RxFifo0);
    CU_ASSERT(((CANFD0->RXF0C & CANFD_RXF0C_F0WM_Msk) >> CANFD_RXF0C_F0WM_Pos) == 0);
    CU_ASSERT((CANFD0->RXF0C &  CANFD_RXF0C_F0SA_Msk) == sCANFD_Config.sMRamStartAddr.u32RXF0C_F0SA);
    CU_ASSERT(((CANFD0->RXESC & CANFD_RXESC_F0DS_Msk) >> CANFD_RXESC_F0DS_Pos) == eCANFD_BYTE64);
    CANFD_InitRxFifo(CANFD0, 0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, 1, eCANFD_BYTE64);
    CU_ASSERT(((CANFD0->RXF0C & CANFD_RXF0C_F0WM_Msk) >> CANFD_RXF0C_F0WM_Pos) == 1);

    /*Configures the Rx Fifo1 element */
    CANFD_InitRxFifo(CANFD0, 1, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, 0, eCANFD_BYTE64);
    CU_ASSERT(((CANFD0->RXF1C & CANFD_RXF1C_F1S_Msk) >> CANFD_RXF1C_F1S_Pos) == sCANFD_Config.sElemSize.u32RxFifo1);
    CU_ASSERT(((CANFD0->RXF1C & CANFD_RXF1C_F1WM_Msk) >> CANFD_RXF1C_F1WM_Pos) == 0);
    CU_ASSERT((CANFD0->RXF1C & CANFD_RXF1C_F1SA_Msk) == sCANFD_Config.sMRamStartAddr.u32RXF1C_F1SA);
    CU_ASSERT(((CANFD0->RXESC & CANFD_RXESC_F1DS_Msk) >> CANFD_RXESC_F1DS_Pos) == eCANFD_BYTE64);
    CANFD_InitRxFifo(CANFD0, 1, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, 5, eCANFD_BYTE64);
    CU_ASSERT(((CANFD0->RXF1C & CANFD_RXF1C_F1WM_Msk) >> CANFD_RXF1C_F1WM_Pos) == 5);

    /*Configures the Tx Event FIFO element */
    CANFD_InitTxEvntFifo(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, 0);
    CU_ASSERT(((CANFD0->TXEFC & CANFD_TXEFC_EFS_Msk) >> CANFD_TXEFC_EFS_Pos) == sCANFD_Config.sElemSize.u32TxEventFifo);
    CU_ASSERT((CANFD0->TXEFC & CANFD_TXEFC_EFSA_Msk) == sCANFD_Config.sMRamStartAddr.u32TXEFC_EFSA);
    CU_ASSERT(((CANFD0->TXEFC & CANFD_TXEFC_EFWM_Msk) >> CANFD_TXEFC_EFWM_Pos) == 0);
    CANFD_InitTxEvntFifo(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, 5);
    CU_ASSERT(((CANFD0->TXEFC & CANFD_TXEFC_EFWM_Msk) >> CANFD_TXEFC_EFWM_Pos) == 5);

    CANFD_IPReset(CANFD0);
    CANFD_RunToNormal(CANFD0, FALSE);
    /*Get the CAN configuration value*/
    CANFD0->CCCR |= CANFD_CCCR_CCE_Msk;

    /* CAN FD Standard message ID elements as 12 elements    */
    sCANFD_Config.sElemSize.u32SIDFC = 0;
    /* CAN FD Extended message ID elements as 10 elements    */
    sCANFD_Config.sElemSize.u32XIDFC = 20;
    /* CAN FD TX Buffer elements as 3 elements    */
    sCANFD_Config.sElemSize.u32TxBuf = 0;
    /* CAN FD RX Buffer elements as 3 elements    */
    sCANFD_Config.sElemSize.u32RxBuf = 6;
    /* CAN FD RX FIFO0 elements as 3 elements    */
    sCANFD_Config.sElemSize.u32RxFifo0 = 6;
    /* CAN FD RX FIFO1 elements as 3 elements    */
    sCANFD_Config.sElemSize.u32RxFifo1 = 0;
    /* CAN FD TX Event FOFI elements as 3 elements    */
    sCANFD_Config.sElemSize.u32TxEventFifo = 10;
    /*Calculates the CAN FD RAM buffer address*/
    CANFD_CalculateRamAddress1(&sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize);

    /*Configures the Standard ID Filter element */
    CANFD_ConfigSIDFC(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize);
    CU_ASSERT(((CANFD0->SIDFC & CANFD_SIDFC_LSS_Msk) >> CANFD_SIDFC_LSS_Pos) == sCANFD_Config.sElemSize.u32SIDFC);
    CU_ASSERT((CANFD0->SIDFC & CANFD_SIDFC_FLSSA_Msk) == sCANFD_Config.sMRamStartAddr.u32SIDFC_FLSSA);
    /*Configures the Extended ID Filter element */
    CANFD_ConfigXIDFC(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize);
    CU_ASSERT(((CANFD0->XIDFC & CANFD_XIDFC_LSE_Msk) >> CANFD_XIDFC_LSE_Pos) == sCANFD_Config.sElemSize.u32XIDFC);
    CU_ASSERT((CANFD0->XIDFC & CANFD_XIDFC_FLESA_Msk) == sCANFD_Config.sMRamStartAddr.u32XIDFC_FLESA);
    /*Configures the Tx Buffer element */
    CANFD_InitTxDBuf(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, eCANFD_BYTE64);
    CU_ASSERT(((CANFD0->TXBC & CANFD_TXBC_NDTB_Msk) >> CANFD_TXBC_NDTB_Pos) == sCANFD_Config.sElemSize.u32TxBuf);
    CU_ASSERT((CANFD0->TXBC & CANFD_TXBC_TBSA_Msk) == sCANFD_Config.sMRamStartAddr.u32TXBC_TBSA);
    CU_ASSERT((CANFD0->TXESC & CANFD_TXESC_TBDS_Msk) == eCANFD_BYTE64);
    /*Configures the Rx Buffer element */
    CANFD_InitRxDBuf(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, eCANFD_BYTE64);
    CU_ASSERT((CANFD0->RXBC & CANFD_RXBC_RBSA_Msk) == sCANFD_Config.sMRamStartAddr.u32RXBC_RBSA);
    CU_ASSERT(((CANFD0->RXESC & CANFD_RXESC_RBDS_Msk) >> CANFD_RXESC_RBDS_Pos)  == eCANFD_BYTE64);
    /*Configures the Rx Fifo0 element */
    CANFD_InitRxFifo(CANFD0, 0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, 0, eCANFD_BYTE64);
    CU_ASSERT(((CANFD0->RXF0C & CANFD_RXF0C_F0S_Msk) >> CANFD_RXF0C_F0S_Pos) == sCANFD_Config.sElemSize.u32RxFifo0);
    CU_ASSERT(((CANFD0->RXF0C & CANFD_RXF0C_F0WM_Msk) >> CANFD_RXF0C_F0WM_Pos) == 0);
    CU_ASSERT((CANFD0->RXF0C &  CANFD_RXF0C_F0SA_Msk) == sCANFD_Config.sMRamStartAddr.u32RXF0C_F0SA);
    CU_ASSERT(((CANFD0->RXESC & CANFD_RXESC_F0DS_Msk) >> CANFD_RXESC_F0DS_Pos) == eCANFD_BYTE64);
    CANFD_InitRxFifo(CANFD0, 0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, 6, eCANFD_BYTE64);
    CU_ASSERT(((CANFD0->RXF0C & CANFD_RXF0C_F0WM_Msk) >> CANFD_RXF0C_F0WM_Pos) == 6);

    /*Configures the Rx Fifo1 element */
    CANFD_InitRxFifo(CANFD0, 1, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, 0, eCANFD_BYTE64);
    CU_ASSERT(((CANFD0->RXF1C & CANFD_RXF1C_F1S_Msk) >> CANFD_RXF1C_F1S_Pos) == sCANFD_Config.sElemSize.u32RxFifo1);
    CU_ASSERT(((CANFD0->RXF1C & CANFD_RXF1C_F1WM_Msk) >> CANFD_RXF1C_F1WM_Pos) == 0);
    CU_ASSERT((CANFD0->RXF1C & CANFD_RXF1C_F1SA_Msk) == 0);
    CU_ASSERT(((CANFD0->RXESC & CANFD_RXESC_F1DS_Msk) >> CANFD_RXESC_F1DS_Pos) == 0);

    /*Configures the Tx Event FIFO element */
    CANFD_InitTxEvntFifo(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, 0);
    CU_ASSERT(((CANFD0->TXEFC & CANFD_TXEFC_EFS_Msk) >> CANFD_TXEFC_EFS_Pos) == sCANFD_Config.sElemSize.u32TxEventFifo);
    CU_ASSERT((CANFD0->TXEFC & CANFD_TXEFC_EFSA_Msk) == sCANFD_Config.sMRamStartAddr.u32TXEFC_EFSA);
    CU_ASSERT(((CANFD0->TXEFC & CANFD_TXEFC_EFWM_Msk) >> CANFD_TXEFC_EFWM_Pos) == 0);
    CANFD_InitTxEvntFifo(CANFD0, &sCANFD_Config.sMRamStartAddr, &sCANFD_Config.sElemSize, 20);
    CU_ASSERT(((CANFD0->TXEFC & CANFD_TXEFC_EFWM_Msk) >> CANFD_TXEFC_EFWM_Pos) == 20);

    CANFD_IPReset(CANFD0);
}



void TestFunc_CANFD_EnableInt(void)
{
    uint8_t u8UartChIdx;    //UART channel index
    uint8_t u8CanfdIntEnIdx; //bit
    CANFD_IPReset(CANFD0);
    CANFD_RunToNormal(CANFD0, FALSE);
    /*Get the CAN configuration value*/
    CANFD0->CCCR |= CANFD_CCCR_CCE_Msk;

    /* test loop */
    //CANFD0 IRQ0 enable interrupt
    for (u8CanfdIntEnIdx = 0; u8CanfdIntEnIdx < 32; u8CanfdIntEnIdx++)
    {
        if (u8CanfdIntEnIdx < 30)
        {
            CANFD_EnableInt(CANFD0, au32CANFD_INTEnableSel[u8CanfdIntEnIdx], 0, 0, 0);
            CU_ASSERT((CANFD0->IE & au32CANFD_INTEnableSel[u8CanfdIntEnIdx]) == au32CANFD_INTEnableSel[u8CanfdIntEnIdx]);
        }

        CANFD_EnableInt(CANFD0, 0, 0, ((uint32_t)1 << u8CanfdIntEnIdx), ((uint32_t)1 << u8CanfdIntEnIdx));
        CU_ASSERT((CANFD0->TXBTIE & ((uint32_t)1 << u8CanfdIntEnIdx)) == ((uint32_t)1 << u8CanfdIntEnIdx));
        CU_ASSERT((CANFD0->TXBCIE & ((uint32_t)1 << u8CanfdIntEnIdx)) == ((uint32_t)1 << u8CanfdIntEnIdx));
        CU_ASSERT((CANFD0->ILE & 0x01) == 0x01);
    }

    //CANFD0 IRQ0 disable interrupt
    for (u8CanfdIntEnIdx = 0; u8CanfdIntEnIdx < 32; u8CanfdIntEnIdx++)
    {
        if (u8CanfdIntEnIdx < 30)
        {
            CANFD_DisableInt(CANFD0, au32CANFD_INTEnableSel[u8CanfdIntEnIdx], 0, 0, 0);
            CU_ASSERT((CANFD0->IE & au32CANFD_INTEnableSel[u8CanfdIntEnIdx]) == 0);
        }

        CANFD_DisableInt(CANFD0, 0, 0, ((uint32_t)1 << u8CanfdIntEnIdx), ((uint32_t)1 << u8CanfdIntEnIdx));
        CU_ASSERT((CANFD0->TXBTIE & ((uint32_t)1 << u8CanfdIntEnIdx)) == 0);
        CU_ASSERT((CANFD0->TXBCIE & ((uint32_t)1 << u8CanfdIntEnIdx)) == 0);
        CU_ASSERT((CANFD0->ILE & 0x01) == 0x0);
    }

    //CANFD0 IRQ1 enable interrupt
    for (u8CanfdIntEnIdx = 0; u8CanfdIntEnIdx < 32; u8CanfdIntEnIdx++)
    {
        if (u8CanfdIntEnIdx < 30)
        {
            CANFD_EnableInt(CANFD0, 0, au32CANFD_INTEnableSel[u8CanfdIntEnIdx], 0, 0);
            CU_ASSERT((CANFD0->ILS & au32CANFD_INTEnableSel[u8CanfdIntEnIdx]) == au32CANFD_INTEnableSel[u8CanfdIntEnIdx]);
        }

        CANFD_EnableInt(CANFD0, 0, 0, ((uint32_t)1 << u8CanfdIntEnIdx), ((uint32_t)1 << u8CanfdIntEnIdx));
        CU_ASSERT((CANFD0->TXBTIE & ((uint32_t)1 << u8CanfdIntEnIdx)) == ((uint32_t)1 << u8CanfdIntEnIdx));
        CU_ASSERT((CANFD0->TXBCIE & ((uint32_t)1 << u8CanfdIntEnIdx)) == ((uint32_t)1 << u8CanfdIntEnIdx));

        CU_ASSERT((CANFD0->ILE & 0x02) == 0x02);
    }

    //CANFD0 IRQ1 disable interrupt
    for (u8CanfdIntEnIdx = 0; u8CanfdIntEnIdx < 30; u8CanfdIntEnIdx++)
    {
        if (u8CanfdIntEnIdx < 30)
        {
            CANFD_DisableInt(CANFD0, 0, au32CANFD_INTEnableSel[u8CanfdIntEnIdx], 0, 0);
            CU_ASSERT((CANFD0->ILS & au32CANFD_INTEnableSel[u8CanfdIntEnIdx]) == 0);
        }

        CANFD_DisableInt(CANFD0, 0, 0, ((uint32_t)1 << u8CanfdIntEnIdx), ((uint32_t)1 << u8CanfdIntEnIdx));
        CU_ASSERT((CANFD0->TXBTIE & ((uint32_t)1 << u8CanfdIntEnIdx)) == 0);
        CU_ASSERT((CANFD0->TXBCIE & ((uint32_t)1 << u8CanfdIntEnIdx)) == 0);
        CU_ASSERT((CANFD0->ILE & 0x02) == 0x0);

    }

}


void TestFunc_CANFD_Open(void)
{
    CANFD_FD_T sCANFD_Config;

    /*Get the CAN configuration value*/
    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_FD_MODE);
    sCANFD_Config.sBtConfig.bEnableLoopBack = TRUE;
    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 1000000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 4000000;
    /*Open the CAN FD feature*/
    CANFD_Open(CANFD0, &sCANFD_Config);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == CANFD_CCCR_INIT_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == CANFD_CCCR_CCE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_BRSE_Msk) == CANFD_CCCR_BRSE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_FDOE_Msk) == CANFD_CCCR_FDOE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_TEST_Msk) == CANFD_CCCR_TEST_Msk);
    CU_ASSERT((CANFD0->TEST & CANFD_TEST_LBCK_Msk) == CANFD_TEST_LBCK_Msk);
    CU_ASSERT(CANFD_CalculateDataBitRate(CANFD0) == sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate);
    CU_ASSERT(CANFD_CalculateNominalBitRate(CANFD0) == sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate);
    memset(&sCANFD_Config, 0, sizeof(sCANFD_Config));
    CANFD_IPReset(CANFD0);

    /*Get the CAN configuration value*/
    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_FD_MODE);
    sCANFD_Config.sBtConfig.bEnableLoopBack = FALSE;
    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 1000000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 3000000;
    /*Open the CAN FD feature*/
    CANFD_Open(CANFD0, &sCANFD_Config);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == CANFD_CCCR_INIT_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == CANFD_CCCR_CCE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_BRSE_Msk) == CANFD_CCCR_BRSE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_FDOE_Msk) == CANFD_CCCR_FDOE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_TEST_Msk) == 0);
    CU_ASSERT((CANFD0->TEST & CANFD_TEST_LBCK_Msk) == 0);
    CU_ASSERT(CANFD_CalculateDataBitRate(CANFD0) == sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate);
    CU_ASSERT(CANFD_CalculateNominalBitRate(CANFD0) == sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate);
    CANFD_IPReset(CANFD0);

    /*Get the CAN configuration value*/
    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_FD_MODE);
    sCANFD_Config.sBtConfig.bEnableLoopBack = TRUE;
    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 1000000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 2000000;
    /*Open the CAN FD feature*/
    CANFD_Open(CANFD0, &sCANFD_Config);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == CANFD_CCCR_INIT_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == CANFD_CCCR_CCE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_BRSE_Msk) == CANFD_CCCR_BRSE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_FDOE_Msk) == CANFD_CCCR_FDOE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_TEST_Msk) == CANFD_CCCR_TEST_Msk);
    CU_ASSERT((CANFD0->TEST & CANFD_TEST_LBCK_Msk) == CANFD_TEST_LBCK_Msk);
    CU_ASSERT(CANFD_CalculateDataBitRate(CANFD0) == sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate);
    CU_ASSERT(CANFD_CalculateNominalBitRate(CANFD0) == sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate);

    /*Get the CAN configuration value*/
    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_FD_MODE);
    sCANFD_Config.sBtConfig.bEnableLoopBack = TRUE;
    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 500000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 1000000;
    /*Open the CAN FD feature*/
    CANFD_Open(CANFD0, &sCANFD_Config);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == CANFD_CCCR_INIT_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == CANFD_CCCR_CCE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_BRSE_Msk) == CANFD_CCCR_BRSE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_FDOE_Msk) == CANFD_CCCR_FDOE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_TEST_Msk) == CANFD_CCCR_TEST_Msk);
    CU_ASSERT((CANFD0->TEST & CANFD_TEST_LBCK_Msk) == CANFD_TEST_LBCK_Msk);
    CU_ASSERT(CANFD_CalculateDataBitRate(CANFD0) == sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate);
    CU_ASSERT(CANFD_CalculateNominalBitRate(CANFD0) == sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate);

    /*Get the CAN configuration value*/
    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_MODE);
    sCANFD_Config.sBtConfig.bEnableLoopBack = TRUE;
    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 1000000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 0;
    /*Open the CAN FD feature*/
    CANFD_Open(CANFD0, &sCANFD_Config);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == CANFD_CCCR_INIT_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == CANFD_CCCR_CCE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_BRSE_Msk) == 0);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_FDOE_Msk) == 0);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_TEST_Msk) == CANFD_CCCR_TEST_Msk);
    CU_ASSERT((CANFD0->TEST & CANFD_TEST_LBCK_Msk) == CANFD_TEST_LBCK_Msk);
    CU_ASSERT(((CANFD0->DBTP & CANFD_DBTP_DSJW_Msk) >> CANFD_DBTP_DSJW_Pos) == 0x3);
    CU_ASSERT(((CANFD0->DBTP & CANFD_DBTP_DTSEG2_Msk) >> CANFD_DBTP_DTSEG2_Pos) == 0x3);
    CU_ASSERT(((CANFD0->DBTP & CANFD_DBTP_DTSEG1_Msk) >> CANFD_DBTP_DTSEG1_Pos) == 0xA);
    CU_ASSERT(CANFD_CalculateNominalBitRate(CANFD0) == sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate);
    CANFD_IPReset(CANFD0);

    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 500000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 0;
    /*Open the CAN FD feature*/
    CANFD_Open(CANFD0, &sCANFD_Config);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == CANFD_CCCR_INIT_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == CANFD_CCCR_CCE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_BRSE_Msk) == 0);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_FDOE_Msk) == 0);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_TEST_Msk) == CANFD_CCCR_TEST_Msk);
    CU_ASSERT((CANFD0->TEST & CANFD_TEST_LBCK_Msk) == CANFD_TEST_LBCK_Msk);
    CU_ASSERT(((CANFD0->DBTP & CANFD_DBTP_DSJW_Msk) >> CANFD_DBTP_DSJW_Pos) == 0x3);
    CU_ASSERT(((CANFD0->DBTP & CANFD_DBTP_DTSEG2_Msk) >> CANFD_DBTP_DTSEG2_Pos) == 0x3);
    CU_ASSERT(((CANFD0->DBTP & CANFD_DBTP_DTSEG1_Msk) >> CANFD_DBTP_DTSEG1_Pos) == 0xA);
    CU_ASSERT(CANFD_CalculateNominalBitRate(CANFD0) == sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate);

    CANFD_IPReset(CANFD0);

    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 50000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 0;
    /*Open the CAN FD feature*/
    CANFD_Open(CANFD0, &sCANFD_Config);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == CANFD_CCCR_INIT_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == CANFD_CCCR_CCE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_BRSE_Msk) == 0);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_FDOE_Msk) == 0);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_TEST_Msk) == CANFD_CCCR_TEST_Msk);
    CU_ASSERT((CANFD0->TEST & CANFD_TEST_LBCK_Msk) == CANFD_TEST_LBCK_Msk);
    CU_ASSERT(((CANFD0->DBTP & CANFD_DBTP_DSJW_Msk) >> CANFD_DBTP_DSJW_Pos) == 0x3);
    CU_ASSERT(((CANFD0->DBTP & CANFD_DBTP_DTSEG2_Msk) >> CANFD_DBTP_DTSEG2_Pos) == 0x3);
    CU_ASSERT(((CANFD0->DBTP & CANFD_DBTP_DTSEG1_Msk) >> CANFD_DBTP_DTSEG1_Pos) == 0xA);
    CU_ASSERT(CANFD_CalculateNominalBitRate(CANFD0) == sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate);
    CANFD_IPReset(CANFD0);

    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 12500;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 0;
    /*Open the CAN FD feature*/
    CANFD_Open(CANFD0, &sCANFD_Config);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == CANFD_CCCR_INIT_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == CANFD_CCCR_CCE_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_BRSE_Msk) == 0);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_FDOE_Msk) == 0);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_TEST_Msk) == CANFD_CCCR_TEST_Msk);
    CU_ASSERT((CANFD0->TEST & CANFD_TEST_LBCK_Msk) == CANFD_TEST_LBCK_Msk);
    CU_ASSERT(((CANFD0->DBTP & CANFD_DBTP_DSJW_Msk) >> CANFD_DBTP_DSJW_Pos) == 0x3);
    CU_ASSERT(((CANFD0->DBTP & CANFD_DBTP_DTSEG2_Msk) >> CANFD_DBTP_DTSEG2_Pos) == 0x3);
    CU_ASSERT(((CANFD0->DBTP & CANFD_DBTP_DTSEG1_Msk) >> CANFD_DBTP_DTSEG1_Pos) == 0xA);
    CU_ASSERT(CANFD_CalculateNominalBitRate(CANFD0) == sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate);
    CANFD_IPReset(CANFD0);


}

void CAN_TxBuffer_RxBuffer_ReadWrite_Test(void)
{
    uint8_t u8Cnt, u8RecvOkFlag;
    CANFD_FD_T sCANFD_Config;
    CANFD_FD_MSG_T      sRxFrame;
    CANFD_FD_MSG_T      sTxMsgFrame;

    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_MODE);
    sCANFD_Config.sBtConfig.bEnableLoopBack = TRUE;
    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 1000000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 0;
    CANFD_Open(CANFD0, &sCANFD_Config);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == CANFD_CCCR_INIT_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == CANFD_CCCR_CCE_Msk);
    /* receive 0x111 in CAN rx message buffer 0 by setting mask 0 */
    CANFD_SetSIDFltr(CANFD0, 0, CANFD_RX_BUFFER_STD(0x111, 0));
    /* receive 0x22F in CAN rx message buffer 0 by setting mask 1 */
    CANFD_SetSIDFltr(CANFD0, 1, CANFD_RX_BUFFER_STD(0x22F, 1));
    /* receive 0x333 in CAN rx message buffer 0 by setting mask 2 */
    CANFD_SetSIDFltr(CANFD0, 2, CANFD_RX_BUFFER_STD(0x333, 2));

    /* receive 0x222 (29-bit id) in CAN rx message buffer 1 by setting mask 3 */
    CANFD_SetXIDFltr(CANFD0, 0, CANFD_RX_BUFFER_EXT_LOW(0x222, 1), CANFD_RX_BUFFER_EXT_HIGH(0x222, 0));
    /* receive 0x3333 (29-bit id) in CAN rx message buffer 1 by setting mask 3 */
    CANFD_SetXIDFltr(CANFD0, 1, CANFD_RX_BUFFER_EXT_LOW(0x3333, 1), CANFD_RX_BUFFER_EXT_HIGH(0x3333, 1));
    /* receive 0x44444 (29-bit id) in CAN rx message buffer 1 by setting mask 3 */
    CANFD_SetXIDFltr(CANFD0, 2, CANFD_RX_BUFFER_EXT_LOW(0x44444, 1), CANFD_RX_BUFFER_EXT_HIGH(0x44444, 2));
    /* CAN FD0 Run to Normal mode  */
    CANFD_RunToNormal(CANFD0, TRUE);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == 0);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == 0);

    /* Set the ID Number */
    sTxMsgFrame.u32Id = 0x111;
    /*Set the ID type*/
    sTxMsgFrame.eIdType = eCANFD_SID;
    /*Set the frame type*/
    sTxMsgFrame.eFrmType = eCANFD_DATA_FRM;
    /*Set the bitrate switch */
    sTxMsgFrame.bBitRateSwitch = 0;
    /*Set the data lenght */
    sTxMsgFrame.u32DLC = 2;

    for (u8Cnt = 0; u8Cnt < sTxMsgFrame.u32DLC; u8Cnt++) sTxMsgFrame.au8Data[u8Cnt] = u8Cnt + 1;

    if (CANFD_TransmitTxMsg(CANFD0, 0, &sTxMsgFrame) != 1)
    {
        printf("Failed to transmit message\n");
    }

    u8RecvOkFlag = 0;

    /* Receive  6 messages with different ID and data size */
    do
    {
        /* check for any received messages on CAN FD0 message buffer 0 */
        if (CANFD_ReadRxBufMsg(CANFD0, 0, &sRxFrame) == 1)
        {
            CU_ASSERT(sRxFrame.u32Id == 0x111);
            CU_ASSERT(sRxFrame.u32DLC == 2);
            CU_ASSERT(sRxFrame.eIdType == eCANFD_SID);

            for (u8Cnt = 0; u8Cnt < sRxFrame.u32DLC; u8Cnt++)
            {
                CU_ASSERT(sRxFrame.au8Data[u8Cnt] == u8Cnt + 1);
            }

            u8RecvOkFlag = 1;
        }

    } while (u8RecvOkFlag ==  0);


    /* Set the ID Number */
    sTxMsgFrame.u32Id = 0x22F;
    /*Set the ID type*/
    sTxMsgFrame.eIdType = eCANFD_SID;
    /*Set the frame type*/
    sTxMsgFrame.eFrmType = eCANFD_DATA_FRM;
    /*Set the bitrate switch */
    sTxMsgFrame.bBitRateSwitch = 0;
    /*Set the data lenght */
    sTxMsgFrame.u32DLC = 4;

    for (u8Cnt = 0; u8Cnt < sTxMsgFrame.u32DLC; u8Cnt++) sTxMsgFrame.au8Data[u8Cnt] = u8Cnt + 2;

    if (CANFD_TransmitTxMsg(CANFD0, 1, &sTxMsgFrame) != 1)
    {
        printf("Failed to transmit message\n");
    }

    u8RecvOkFlag = 0;

    /* Receive  6 messages with different ID and data size */
    do
    {
        /* check for any received messages on CAN FD0 message buffer 0 */
        if (CANFD_ReadRxBufMsg(CANFD0, 1, &sRxFrame) == 1)
        {
            CU_ASSERT(sRxFrame.u32Id == 0x22F);
            CU_ASSERT(sRxFrame.u32DLC == 4);
            CU_ASSERT(sRxFrame.eIdType == eCANFD_SID);

            for (u8Cnt = 0; u8Cnt < sRxFrame.u32DLC; u8Cnt++)
            {
                CU_ASSERT(sRxFrame.au8Data[u8Cnt] == u8Cnt + 2);
            }

            u8RecvOkFlag = 1;
        }

    } while (u8RecvOkFlag ==  0);

    /* Set the ID Number */
    sTxMsgFrame.u32Id = 0x333;
    /*Set the ID type*/
    sTxMsgFrame.eIdType = eCANFD_SID;
    /*Set the frame type*/
    sTxMsgFrame.eFrmType = eCANFD_DATA_FRM;
    /*Set the bitrate switch */
    sTxMsgFrame.bBitRateSwitch = 0;
    /*Set the data lenght */
    sTxMsgFrame.u32DLC = 8;

    for (u8Cnt = 0; u8Cnt < sTxMsgFrame.u32DLC; u8Cnt++) sTxMsgFrame.au8Data[u8Cnt] = u8Cnt + 3;

    if (CANFD_TransmitTxMsg(CANFD0, 2, &sTxMsgFrame) != 1)
    {
        printf("Failed to transmit message\n");
    }

    u8RecvOkFlag = 0;

    /* Receive  6 messages with different ID and data size */
    do
    {
        /* check for any received messages on CAN FD0 message buffer 0 */
        if (CANFD_ReadRxBufMsg(CANFD0, 2, &sRxFrame) == 1)
        {
            CU_ASSERT(sRxFrame.u32Id == 0x333);
            CU_ASSERT(sRxFrame.u32DLC == 8);
            CU_ASSERT(sRxFrame.eIdType == eCANFD_SID);

            for (u8Cnt = 0; u8Cnt < sRxFrame.u32DLC; u8Cnt++)
            {
                CU_ASSERT(sRxFrame.au8Data[u8Cnt] == u8Cnt + 3);
            }

            u8RecvOkFlag = 1;
        }

    } while (u8RecvOkFlag ==  0);

    /* Set the ID Number */
    sTxMsgFrame.u32Id = 0x222;
    /*Set the ID type*/
    sTxMsgFrame.eIdType = eCANFD_XID;
    /*Set the frame type*/
    sTxMsgFrame.eFrmType = eCANFD_DATA_FRM;
    /*Set the bitrate switch */
    sTxMsgFrame.bBitRateSwitch = 0;
    /*Set the data lenght */
    sTxMsgFrame.u32DLC = 2;

    for (u8Cnt = 0; u8Cnt < sTxMsgFrame.u32DLC; u8Cnt++) sTxMsgFrame.au8Data[u8Cnt] = u8Cnt + 4;

    if (CANFD_TransmitTxMsg(CANFD0, 0, &sTxMsgFrame) != 1)
    {
        printf("Failed to transmit message\n");
    }

    u8RecvOkFlag = 0;

    /* Receive  6 messages with different ID and data size */
    do
    {
        /* check for any received messages on CAN FD0 message buffer 0 */
        if (CANFD_ReadRxBufMsg(CANFD0, 0, &sRxFrame) == 1)
        {
            CU_ASSERT(sRxFrame.u32Id == 0x222);
            CU_ASSERT(sRxFrame.u32DLC == 2);
            CU_ASSERT(sRxFrame.eIdType == eCANFD_XID);

            for (u8Cnt = 0; u8Cnt < sRxFrame.u32DLC; u8Cnt++)
            {
                CU_ASSERT(sRxFrame.au8Data[u8Cnt] == u8Cnt + 4);
            }

            u8RecvOkFlag = 1;
        }

    } while (u8RecvOkFlag ==  0);

    /* Set the ID Number */
    sTxMsgFrame.u32Id = 0x3333;
    /*Set the ID type*/
    sTxMsgFrame.eIdType = eCANFD_XID;
    /*Set the frame type*/
    sTxMsgFrame.eFrmType = eCANFD_DATA_FRM;
    /*Set the bitrate switch */
    sTxMsgFrame.bBitRateSwitch = 0;
    /*Set the data lenght */
    sTxMsgFrame.u32DLC = 4;

    for (u8Cnt = 0; u8Cnt < sTxMsgFrame.u32DLC; u8Cnt++) sTxMsgFrame.au8Data[u8Cnt] = u8Cnt + 5;

    if (CANFD_TransmitTxMsg(CANFD0, 1, &sTxMsgFrame) != 1)
    {
        printf("Failed to transmit message\n");
    }

    u8RecvOkFlag = 0;

    do
    {
        /* check for any received messages on CAN FD0 message buffer 0 */
        if (CANFD_ReadRxBufMsg(CANFD0, 1, &sRxFrame) == 1)
        {
            CU_ASSERT(sRxFrame.u32Id == 0x3333);
            CU_ASSERT(sRxFrame.u32DLC == 4);
            CU_ASSERT(sRxFrame.eIdType == eCANFD_XID);

            for (u8Cnt = 0; u8Cnt < sRxFrame.u32DLC; u8Cnt++)
            {
                CU_ASSERT(sRxFrame.au8Data[u8Cnt] == u8Cnt + 5);
            }

            u8RecvOkFlag = 1;
        }

    } while (u8RecvOkFlag ==  0);

    /* Set the ID Number */
    sTxMsgFrame.u32Id = 0x44444;
    /*Set the ID type*/
    sTxMsgFrame.eIdType = eCANFD_XID;
    /*Set the frame type*/
    sTxMsgFrame.eFrmType = eCANFD_DATA_FRM;
    /*Set the bitrate switch */
    sTxMsgFrame.bBitRateSwitch = 0;
    /*Set the data lenght */
    sTxMsgFrame.u32DLC = 8;

    for (u8Cnt = 0; u8Cnt < sTxMsgFrame.u32DLC; u8Cnt++) sTxMsgFrame.au8Data[u8Cnt] = u8Cnt + 6;

    if (CANFD_TransmitTxMsg(CANFD0, 2, &sTxMsgFrame) != 1)
    {
        printf("Failed to transmit message\n");
    }

    u8RecvOkFlag = 0;

    do
    {
        /* check for any received messages on CAN FD0 message buffer 0 */
        if (CANFD_ReadRxBufMsg(CANFD0, 2, &sRxFrame) == 1)
        {
            CU_ASSERT(sRxFrame.u32Id == 0x44444);
            CU_ASSERT(sRxFrame.u32DLC == 8);
            CU_ASSERT(sRxFrame.eIdType == eCANFD_XID);

            for (u8Cnt = 0; u8Cnt < sRxFrame.u32DLC; u8Cnt++)
            {
                CU_ASSERT(sRxFrame.au8Data[u8Cnt] == u8Cnt + 6);
            }

            u8RecvOkFlag = 1;
        }
    } while (u8RecvOkFlag ==  0);

}


void CANFD_TxBuffer_RxBuffer_ReadWrite_Test(void)
{
    uint8_t u8Cnt, u8RecvOkFlag;
    CANFD_FD_T sCANFD_Config;
    CANFD_FD_MSG_T      sRxFrame;
    CANFD_FD_MSG_T      sTxMsgFrame;

    /*Get the CAN FD0 configuration value*/
    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_FD_MODE);
    sCANFD_Config.sBtConfig.bEnableLoopBack = TRUE;
    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 1000000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 4000000;
    /*Open the CAN FD0 feature*/
    CANFD_Open(CANFD0, &sCANFD_Config);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == CANFD_CCCR_INIT_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == CANFD_CCCR_CCE_Msk);
    /* receive 0x111 in CAN rx message buffer 0 by setting mask 0 */
    CANFD_SetSIDFltr(CANFD0, 0, CANFD_RX_BUFFER_STD(0x111, 0));
    /* receive 0x22F in CAN rx message buffer 0 by setting mask 1 */
    CANFD_SetSIDFltr(CANFD0, 1, CANFD_RX_BUFFER_STD(0x22F, 1));
    /* receive 0x333 in CAN rx message buffer 0 by setting mask 2 */
    CANFD_SetSIDFltr(CANFD0, 2, CANFD_RX_BUFFER_STD(0x333, 2));

    /* receive 0x222 (29-bit id) in CAN rx message buffer 1 by setting mask 3 */
    CANFD_SetXIDFltr(CANFD0, 0, CANFD_RX_BUFFER_EXT_LOW(0x222, 0), CANFD_RX_BUFFER_EXT_HIGH(0x222, 0));
    /* receive 0x3333 (29-bit id) in CAN rx message buffer 1 by setting mask 3 */
    CANFD_SetXIDFltr(CANFD0, 1, CANFD_RX_BUFFER_EXT_LOW(0x3333, 1), CANFD_RX_BUFFER_EXT_HIGH(0x3333, 1));
    /* receive 0x44444 (29-bit id) in CAN rx message buffer 1 by setting mask 3 */
    CANFD_SetXIDFltr(CANFD0, 2, CANFD_RX_BUFFER_EXT_LOW(0x44444, 2), CANFD_RX_BUFFER_EXT_HIGH(0x44444, 2));
    /* CAN FD0 Run to Normal mode  */
    CANFD_RunToNormal(CANFD0, TRUE);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == 0);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == 0);

    /* Set the ID Number */
    sTxMsgFrame.u32Id = 0x111;
    /*Set the ID type*/
    sTxMsgFrame.eIdType = eCANFD_SID;
    /*Set the frame type*/
    sTxMsgFrame.eFrmType = eCANFD_DATA_FRM;
    /*Set CAN FD frame format */
    sTxMsgFrame.bFDFormat = 1;
    /*Set the bitrate switch */
    sTxMsgFrame.bBitRateSwitch = 1;
    /*Set the data lenght */
    sTxMsgFrame.u32DLC = 16;

    for (u8Cnt = 0; u8Cnt < sTxMsgFrame.u32DLC; u8Cnt++) sTxMsgFrame.au8Data[u8Cnt] = u8Cnt + 1;

    if (CANFD_TransmitTxMsg(CANFD0, 0, &sTxMsgFrame) != 1)
    {
        printf("Failed to transmit message\n");
    }

    u8RecvOkFlag = 0;

    /* Receive  6 messages with different ID and data size */
    do
    {
        /* check for any received messages on CAN FD0 message buffer 0 */
        if (CANFD_ReadRxBufMsg(CANFD0, 0, &sRxFrame) == 1)
        {
            CU_ASSERT(sRxFrame.u32Id == 0x111);
            CU_ASSERT(sRxFrame.u32DLC == 16);
            CU_ASSERT(sRxFrame.eIdType == eCANFD_SID);
            CU_ASSERT(sRxFrame.bBitRateSwitch == 1);
            CU_ASSERT(sRxFrame.bFDFormat == 1);

            for (u8Cnt = 0; u8Cnt < sRxFrame.u32DLC; u8Cnt++)
            {
                CU_ASSERT(sRxFrame.au8Data[u8Cnt] == u8Cnt + 1);
            }

            u8RecvOkFlag = 1;
        }

    } while (u8RecvOkFlag ==  0);

    memset(&sRxFrame, 0, sizeof(sRxFrame));

    /* Set the ID Number */
    sTxMsgFrame.u32Id = 0x22F;
    /*Set the ID type*/
    sTxMsgFrame.eIdType = eCANFD_SID;
    /*Set the frame type*/
    sTxMsgFrame.eFrmType = eCANFD_DATA_FRM;
    /*Set CAN FD frame format */
    sTxMsgFrame.bFDFormat = 1;
    /*Set the bitrate switch */
    sTxMsgFrame.bBitRateSwitch = 1;
    /*Set the data lenght */
    sTxMsgFrame.u32DLC = 32;

    for (u8Cnt = 0; u8Cnt < sTxMsgFrame.u32DLC; u8Cnt++) sTxMsgFrame.au8Data[u8Cnt] = u8Cnt + 2;

    if (CANFD_TransmitTxMsg(CANFD0, 1, &sTxMsgFrame) != 1)
    {
        printf("Failed to transmit message\n");
    }

    u8RecvOkFlag = 0;

    /* Receive  6 messages with different ID and data size */
    do
    {
        /* check for any received messages on CAN FD0 message buffer 0 */
        if (CANFD_ReadRxBufMsg(CANFD0, 1, &sRxFrame) == 1)
        {
            CU_ASSERT(sRxFrame.u32Id == 0x22F);
            CU_ASSERT(sRxFrame.u32DLC == 32);
            CU_ASSERT(sRxFrame.eIdType == eCANFD_SID);
            CU_ASSERT(sRxFrame.bBitRateSwitch == 1);
            CU_ASSERT(sRxFrame.bFDFormat == 1);

            for (u8Cnt = 0; u8Cnt < sRxFrame.u32DLC; u8Cnt++)
            {
                CU_ASSERT(sRxFrame.au8Data[u8Cnt] == u8Cnt + 2);
            }

            u8RecvOkFlag = 1;
        }

    } while (u8RecvOkFlag ==  0);

    memset(&sRxFrame, 0, sizeof(sRxFrame));
    /* Set the ID Number */
    sTxMsgFrame.u32Id = 0x333;
    /*Set the ID type*/
    sTxMsgFrame.eIdType = eCANFD_SID;
    /*Set the frame type*/
    sTxMsgFrame.eFrmType = eCANFD_DATA_FRM;
    /*Set CAN FD frame format */
    sTxMsgFrame.bFDFormat = 1;
    /*Set the bitrate switch */
    sTxMsgFrame.bBitRateSwitch = 1;
    /*Set the data lenght */
    sTxMsgFrame.u32DLC = 64;

    for (u8Cnt = 0; u8Cnt < sTxMsgFrame.u32DLC; u8Cnt++) sTxMsgFrame.au8Data[u8Cnt] = u8Cnt + 3;

    if (CANFD_TransmitTxMsg(CANFD0, 2, &sTxMsgFrame) != 1)
    {
        printf("Failed to transmit message\n");
    }

    u8RecvOkFlag = 0;

    /* Receive  6 messages with different ID and data size */
    do
    {
        /* check for any received messages on CAN FD0 message buffer 0 */
        if (CANFD_ReadRxBufMsg(CANFD0, 2, &sRxFrame) == 1)
        {
            CU_ASSERT(sRxFrame.u32Id == 0x333);
            CU_ASSERT(sRxFrame.u32DLC == 64);
            CU_ASSERT(sRxFrame.eIdType == eCANFD_SID);
            CU_ASSERT(sRxFrame.bBitRateSwitch == 1);
            CU_ASSERT(sRxFrame.bFDFormat == 1);

            for (u8Cnt = 0; u8Cnt < sRxFrame.u32DLC; u8Cnt++)
            {
                CU_ASSERT(sRxFrame.au8Data[u8Cnt] == u8Cnt + 3);
            }

            u8RecvOkFlag = 1;
        }

    } while (u8RecvOkFlag ==  0);

    memset(&sRxFrame, 0, sizeof(sRxFrame));
    /* Set the ID Number */
    sTxMsgFrame.u32Id = 0x222;
    /*Set the ID type*/
    sTxMsgFrame.eIdType = eCANFD_XID;
    /*Set the frame type*/
    sTxMsgFrame.eFrmType = eCANFD_DATA_FRM;
    /*Set CAN FD frame format */
    sTxMsgFrame.bFDFormat = 1;
    /*Set the bitrate switch */
    sTxMsgFrame.bBitRateSwitch = 1;
    /*Set the data lenght */
    sTxMsgFrame.u32DLC = 16;

    for (u8Cnt = 0; u8Cnt < sTxMsgFrame.u32DLC; u8Cnt++) sTxMsgFrame.au8Data[u8Cnt] = u8Cnt + 4;

    if (CANFD_TransmitTxMsg(CANFD0, 0, &sTxMsgFrame) != 1)
    {
        printf("Failed to transmit message\n");
    }

    u8RecvOkFlag = 0;

    /* Receive  6 messages with different ID and data size */
    do
    {
        /* check for any received messages on CAN FD0 message buffer 0 */
        if (CANFD_ReadRxBufMsg(CANFD0, 0, &sRxFrame) == 1)
        {
            CU_ASSERT(sRxFrame.u32Id == 0x222);
            CU_ASSERT(sRxFrame.u32DLC == 16);
            CU_ASSERT(sRxFrame.eIdType == eCANFD_XID);
            CU_ASSERT(sRxFrame.bBitRateSwitch == 1);
            CU_ASSERT(sRxFrame.bFDFormat == 1);

            for (u8Cnt = 0; u8Cnt < sRxFrame.u32DLC; u8Cnt++)
            {
                CU_ASSERT(sRxFrame.au8Data[u8Cnt] == u8Cnt + 4);
            }

            u8RecvOkFlag = 1;
        }

        memset(&sRxFrame, 0, sizeof(sRxFrame));
    } while (u8RecvOkFlag ==  0);

    /* Set the ID Number */
    sTxMsgFrame.u32Id = 0x3333;
    /*Set the ID type*/
    sTxMsgFrame.eIdType = eCANFD_XID;
    /*Set the frame type*/
    sTxMsgFrame.eFrmType = eCANFD_DATA_FRM;
    /*Set CAN FD frame format */
    sTxMsgFrame.bFDFormat = 1;
    /*Set the bitrate switch */
    sTxMsgFrame.bBitRateSwitch = 1;
    /*Set the data lenght */
    sTxMsgFrame.u32DLC = 32;

    for (u8Cnt = 0; u8Cnt < sTxMsgFrame.u32DLC; u8Cnt++) sTxMsgFrame.au8Data[u8Cnt] = u8Cnt + 5;

    if (CANFD_TransmitTxMsg(CANFD0, 1, &sTxMsgFrame) != 1)
    {
        printf("Failed to transmit message\n");
    }

    u8RecvOkFlag = 0;

    do
    {
        /* check for any received messages on CAN FD0 message buffer 0 */
        if (CANFD_ReadRxBufMsg(CANFD0, 1, &sRxFrame) == 1)
        {
            CU_ASSERT(sRxFrame.u32Id == 0x3333);
            CU_ASSERT(sRxFrame.u32DLC == 32);
            CU_ASSERT(sRxFrame.eIdType == eCANFD_XID);
            CU_ASSERT(sRxFrame.bBitRateSwitch == 1);
            CU_ASSERT(sRxFrame.bFDFormat == 1);

            for (u8Cnt = 0; u8Cnt < sRxFrame.u32DLC; u8Cnt++)
            {
                CU_ASSERT(sRxFrame.au8Data[u8Cnt] == u8Cnt + 5);
            }

            u8RecvOkFlag = 1;
        }

        memset(&sRxFrame, 0, sizeof(sRxFrame));
    } while (u8RecvOkFlag ==  0);

    /* Set the ID Number */
    sTxMsgFrame.u32Id = 0x44444;
    /*Set the ID type*/
    sTxMsgFrame.eIdType = eCANFD_XID;
    /*Set the frame type*/
    sTxMsgFrame.eFrmType = eCANFD_DATA_FRM;
    /*Set CAN FD frame format */
    sTxMsgFrame.bFDFormat = 1;
    /*Set the bitrate switch */
    sTxMsgFrame.bBitRateSwitch = 1;
    /*Set the data lenght */
    sTxMsgFrame.u32DLC = 64;

    for (u8Cnt = 0; u8Cnt < sTxMsgFrame.u32DLC; u8Cnt++) sTxMsgFrame.au8Data[u8Cnt] = u8Cnt + 6;

    if (CANFD_TransmitTxMsg(CANFD0, 2, &sTxMsgFrame) != 1)
    {
        printf("Failed to transmit message\n");
    }

    u8RecvOkFlag = 0;

    do
    {
        /* check for any received messages on CAN FD0 message buffer 0 */
        if (CANFD_ReadRxBufMsg(CANFD0, 2, &sRxFrame) == 1)
        {
            CU_ASSERT(sRxFrame.u32Id == 0x44444);
            CU_ASSERT(sRxFrame.u32DLC == 64);
            CU_ASSERT(sRxFrame.eIdType == eCANFD_XID);
            CU_ASSERT(sRxFrame.bBitRateSwitch == 1);
            CU_ASSERT(sRxFrame.bFDFormat == 1);

            for (u8Cnt = 0; u8Cnt < sRxFrame.u32DLC; u8Cnt++)
            {
                CU_ASSERT(sRxFrame.au8Data[u8Cnt] == u8Cnt + 6);
            }

            u8RecvOkFlag = 1;
        }
    } while (u8RecvOkFlag ==  0);

    memset(&sRxFrame, 0, sizeof(sRxFrame));

}
void TestFunc_CANFD_TxBuffer_RxBuffer_ReadWrite(void)
{
    CAN_TxBuffer_RxBuffer_ReadWrite_Test();
    CANFD_IPReset(CANFD0);
    CANFD_TxBuffer_RxBuffer_ReadWrite_Test();
    CANFD_IPReset(CANFD0);
}

/*---------------------------------------------------------------------------------------------------------*/
/*                             CAN Tx Rx Function Test                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void CAN_TxRx_Test(CANFD_FD_MSG_T *psTxMsg, E_CANFD_ID_TYPE eFrameIdType, uint8_t u8FifoNum, uint32_t u32Id, uint8_t u8Len)
{
    uint8_t u8Cnt;

    /*Set the ID Number*/
    psTxMsg->u32Id = u32Id;
    /*Set the ID Type*/
    psTxMsg->eIdType = eFrameIdType;
    /*Set the frame type*/
    psTxMsg->eFrmType = eCANFD_DATA_FRM;
    /*Set FD frame format attribute */
    psTxMsg->bFDFormat = 0;
    /*Set the bitrate switch attribute*/
    psTxMsg->bBitRateSwitch = 0;
    /*Set data length*/
    psTxMsg->u32DLC = u8Len;

    for (u8Cnt = 0; u8Cnt < psTxMsg->u32DLC; u8Cnt++) psTxMsg->au8Data[u8Cnt] = u8Cnt;

    if (u8FifoNum == 0)
        g_u8RxFifo0CompleteFlag = 0;
    else
        g_u8RxFifo1CompleteFlag = 0;

    if (CANFD_TransmitTxMsg(CANFD0, 0, psTxMsg) != 1)
    {
        printf("Failed to transmit message\n");
    }

    if (u8FifoNum == 0)
    {
        /*Wait the Rx FIFO0 received message*/
        while (!g_u8RxFifo0CompleteFlag)
        {
        }

        CU_ASSERT(g_sRxFifo0MsgFrame.u32Id == u32Id);
        CU_ASSERT(g_sRxFifo0MsgFrame.u32DLC == u8Len);
        CU_ASSERT(g_sRxFifo0MsgFrame.eIdType == eFrameIdType);

        for (u8Cnt = 0; u8Cnt < g_sRxFifo0MsgFrame.u32DLC; u8Cnt++)
        {
            CU_ASSERT(g_sRxFifo0MsgFrame.au8Data[u8Cnt] == u8Cnt);
        }

        memset(&g_sRxFifo0MsgFrame, 0, sizeof(g_sRxFifo0MsgFrame));
    }
    else
    {
        /*Wait the Rx FIFO1 received message*/
        while (!g_u8RxFifo1CompleteFlag)
        {
        }

        CU_ASSERT(g_sRxFifo1MsgFrame.u32Id == u32Id);
        CU_ASSERT(g_sRxFifo1MsgFrame.u32DLC == u8Len);
        CU_ASSERT(g_sRxFifo1MsgFrame.eIdType == eFrameIdType);

        for (u8Cnt = 0; u8Cnt < g_sRxFifo1MsgFrame.u32DLC; u8Cnt++)
        {
            CU_ASSERT(g_sRxFifo1MsgFrame.au8Data[u8Cnt] == u8Cnt);
        }

        memset(&g_sRxFifo1MsgFrame, 0, sizeof(g_sRxFifo1MsgFrame));

    }

}

void CANFD_CANFD_TxRx_Test(CANFD_FD_MSG_T *psTxMsg, E_CANFD_ID_TYPE eFrameIdType, uint8_t u8FifoNum, uint32_t u32Id, uint8_t u8LenType)
{
    uint8_t u8Cnt;
    /*Set the ID Number*/
    psTxMsg->u32Id = u32Id;
    /*Set the frame type*/
    psTxMsg->eIdType = eFrameIdType;
    /*Set FD frame format attribute */
    psTxMsg->bFDFormat = 1;
    /*Set the bitrate switch attribute*/
    psTxMsg->bBitRateSwitch = 1;

    /*Set data length*/
    if (u8LenType == 0)      psTxMsg->u32DLC = 8;
    else if (u8LenType == 1) psTxMsg->u32DLC = 12;
    else if (u8LenType == 2) psTxMsg->u32DLC = 16;
    else if (u8LenType == 3) psTxMsg->u32DLC = 20;
    else if (u8LenType == 4) psTxMsg->u32DLC = 24;
    else if (u8LenType == 5) psTxMsg->u32DLC = 32;
    else if (u8LenType == 6) psTxMsg->u32DLC = 48;
    else if (u8LenType == 7) psTxMsg->u32DLC = 64;

    for (u8Cnt = 0; u8Cnt < psTxMsg->u32DLC; u8Cnt++) psTxMsg->au8Data[u8Cnt] = u8Cnt;

    if (u8FifoNum == 0)
        g_u8RxFifo0CompleteFlag = 0;
    else
        g_u8RxFifo1CompleteFlag = 0;

    if (CANFD_TransmitTxMsg(CANFD0, 0, psTxMsg) != 1)
    {
        printf("Failed to transmit message\n");
    }

    if (u8FifoNum == 0)
    {
        /* Wait the Rx FIFO0 received message */
        while (!g_u8RxFifo0CompleteFlag)
        {
        }

        CU_ASSERT(g_sRxFifo0MsgFrame.u32Id == u32Id);
        CU_ASSERT(g_sRxFifo0MsgFrame.u32DLC == psTxMsg->u32DLC);
        CU_ASSERT(g_sRxFifo0MsgFrame.eIdType == eFrameIdType);
        CU_ASSERT(g_sRxFifo0MsgFrame.bBitRateSwitch == 1);
        CU_ASSERT(g_sRxFifo0MsgFrame.bFDFormat == 1);

        for (u8Cnt = 0; u8Cnt < g_sRxFifo0MsgFrame.u32DLC; u8Cnt++)
        {
            CU_ASSERT(g_sRxFifo0MsgFrame.au8Data[u8Cnt] == u8Cnt);
        }

        memset(&g_sRxFifo0MsgFrame, 0, sizeof(g_sRxFifo0MsgFrame));
    }
    else
    {
        /* Wait the Rx FIFO1 received message */
        while (!g_u8RxFifo1CompleteFlag)
        {
        }

        CU_ASSERT(g_sRxFifo1MsgFrame.u32Id == u32Id);
        CU_ASSERT(g_sRxFifo1MsgFrame.u32DLC == psTxMsg->u32DLC);
        CU_ASSERT(g_sRxFifo1MsgFrame.eIdType == eFrameIdType);
        CU_ASSERT(g_sRxFifo1MsgFrame.bBitRateSwitch == 1);
        CU_ASSERT(g_sRxFifo1MsgFrame.bFDFormat == 1);

        for (u8Cnt = 0; u8Cnt < g_sRxFifo1MsgFrame.u32DLC; u8Cnt++)
        {
            CU_ASSERT(g_sRxFifo1MsgFrame.au8Data[u8Cnt] == u8Cnt);
        }

        memset(&g_sRxFifo1MsgFrame, 0, sizeof(g_sRxFifo1MsgFrame));

    }
}
void CANFD_TxBuffer_RxFifo_ReadWrite(uint8_t u8FifoNum)
{
    uint8_t u8Loop;
    CANFD_FD_T sCANFD_Config;
    CANFD_FD_MSG_T      sTxMsgFrame;
    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_FD_MODE);
    sCANFD_Config.sBtConfig.bEnableLoopBack = TRUE;
    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 1000000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 4000000;
    CANFD_Open(CANFD0, &sCANFD_Config);

    if (u8FifoNum == 0)
    {
        /* receive 0x110~0x11F in CAN FD0 rx fifo1 buffer by setting mask 0 */
        CANFD_SetSIDFltr(CANFD0, 0, CANFD_RX_FIFO0_STD_MASK(0x110, 0x7F0));
        /* receive 0x220 in CAN FD0 rx fifo1 buffer by setting mask 1 */
        CANFD_SetSIDFltr(CANFD0, 1, CANFD_RX_FIFO0_STD_MASK(0x22F, 0x7FF));
        /* receive 0x333 in CAN FD0 rx fifo1 buffer by setting mask 2 */
        CANFD_SetSIDFltr(CANFD0, 2, CANFD_RX_FIFO0_STD_MASK(0x333, 0x7FF));

        /* receive 0x220~0x22f (29-bit id) in CAN FD0 rx fifo1 buffer by setting mask 0 */
        CANFD_SetXIDFltr(CANFD0, 0, CANFD_RX_FIFO0_EXT_MASK_LOW(0x220), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFF0));
        /* receive 0x3333 (29-bit id) in CAN FD0 rx message buffer by setting mask 1 */
        CANFD_SetXIDFltr(CANFD0, 1, CANFD_RX_FIFO0_EXT_MASK_LOW(0x3333), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFFF));
        /* receive 0x44444 (29-bit id) in CAN FD0 rx fifo buffer by setting mask 2 */
        CANFD_SetXIDFltr(CANFD0, 2, CANFD_RX_FIFO0_EXT_MASK_LOW(0x44444), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFFF));
        /* Reject Non-Matching Standard ID and Extended ID Filter(RX fifo1)*/
        CANFD_SetGFC(CANFD0, eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0, eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0, 1, 1);
        /* Enable RX fifo1 new message interrupt using interrupt line 0. */
        CANFD_EnableInt(CANFD0, (CANFD_IE_TOOE_Msk | CANFD_IE_RF0NE_Msk), 0, 0, 0);
    }
    else
    {
        /* receive 0x110~0x11F in CAN FD0 rx fifo1 buffer by setting mask 0 */
        CANFD_SetSIDFltr(CANFD0, 0, CANFD_RX_FIFO1_STD_MASK(0x110, 0x7F0));
        /* receive 0x220 in CAN FD0 rx fifo1 buffer by setting mask 1 */
        CANFD_SetSIDFltr(CANFD0, 1, CANFD_RX_FIFO1_STD_MASK(0x22F, 0x7FF));
        /* receive 0x333 in CAN FD0 rx fifo1 buffer by setting mask 2 */
        CANFD_SetSIDFltr(CANFD0, 2, CANFD_RX_FIFO1_STD_MASK(0x333, 0x7FF));

        /* receive 0x220~0x22f (29-bit id) in CAN FD0 rx fifo1 buffer by setting mask 0 */
        CANFD_SetXIDFltr(CANFD0, 0, CANFD_RX_FIFO1_EXT_MASK_LOW(0x220), CANFD_RX_FIFO1_EXT_MASK_HIGH(0x1FFFFFF0));
        /* receive 0x3333 (29-bit id) in CAN FD0 rx message buffer by setting mask 1 */
        CANFD_SetXIDFltr(CANFD0, 1, CANFD_RX_FIFO1_EXT_MASK_LOW(0x3333), CANFD_RX_FIFO1_EXT_MASK_HIGH(0x1FFFFFFF));
        /* receive 0x44444 (29-bit id) in CAN FD0 rx fifo buffer by setting mask 2 */
        CANFD_SetXIDFltr(CANFD0, 2, CANFD_RX_FIFO1_EXT_MASK_LOW(0x44444), CANFD_RX_FIFO1_EXT_MASK_HIGH(0x1FFFFFFF));
        /* Reject Non-Matching Standard ID and Extended ID Filter(RX fifo1)*/
        CANFD_SetGFC(CANFD0, eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0, eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0, 1, 1);
        /* Enable RX fifo1 new message interrupt using interrupt line 0. */
        CANFD_EnableInt(CANFD0, (CANFD_IE_TOOE_Msk | CANFD_IE_RF1NE_Msk), 0, 0, 0);

    }

    /* CAN FD0 Run to Normal mode  */
    CANFD_RunToNormal(CANFD0, TRUE);

    for (u8Loop = 0  ; u8Loop < 8; u8Loop++)
    {
        CANFD_CANFD_TxRx_Test(&sTxMsgFrame, eCANFD_SID, u8FifoNum, 0x110 + u8Loop, u8Loop);
    }

    CANFD_CANFD_TxRx_Test(&sTxMsgFrame, eCANFD_SID, u8FifoNum, 0x22F, 8);
    CANFD_CANFD_TxRx_Test(&sTxMsgFrame, eCANFD_SID, u8FifoNum, 0x333, 8);

    for (u8Loop = 0 ; u8Loop < 8; u8Loop++)
    {
        CANFD_CANFD_TxRx_Test(&sTxMsgFrame, eCANFD_XID, u8FifoNum, 0x220 + u8Loop, u8Loop);
    }

    CANFD_CANFD_TxRx_Test(&sTxMsgFrame, eCANFD_XID, u8FifoNum, 0x3333, 8);
    CANFD_CANFD_TxRx_Test(&sTxMsgFrame, eCANFD_XID, u8FifoNum, 0x44444, 8);

}
void CAN_TxBuffer_RxFifo_ReadWrite(uint8_t u8FifoNum)
{
    uint8_t u8Loop;
    CANFD_FD_T sCANFD_Config;
    CANFD_FD_MSG_T      sTxMsgFrame;
    /*Get the CAN configuration value*/
    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_MODE);
    /*Enable internal loopback mode*/
    sCANFD_Config.sBtConfig.bEnableLoopBack = TRUE;
    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 1000000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 0;
    /*Open the CAN FD feature*/
    CANFD_Open(CANFD0, &sCANFD_Config);

    if (u8FifoNum == 0)
    {
        /* receive 0x110~0x11F in CAN rx fifo0 buffer by setting mask 0 */
        CANFD_SetSIDFltr(CANFD0, 0, CANFD_RX_FIFO0_STD_MASK(0x110, 0x7F0));
        /* receive 0x22F in CAN rx fifo0 buffer by setting mask 1 */
        CANFD_SetSIDFltr(CANFD0, 1, CANFD_RX_FIFO0_STD_MASK(0x22F, 0x7FF));
        /* receive 0x333 in CAN rx fifo0 buffer by setting mask 2 */
        CANFD_SetSIDFltr(CANFD0, 2, CANFD_RX_FIFO0_STD_MASK(0x333, 0x7FF));

        /* receive 0x220~0x22F (29-bit id) in CAN rx fifo0 buffer by setting mask 0 */
        CANFD_SetXIDFltr(CANFD0, 0, CANFD_RX_FIFO0_EXT_MASK_LOW(0x220), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFF0));
        /* receive 0x3333 (29-bit id) in CAN rx fifo0 buffer by setting mask 1 */
        CANFD_SetXIDFltr(CANFD0, 1, CANFD_RX_FIFO0_EXT_MASK_LOW(0x3333), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFFF));
        /* receive 0x44444 (29-bit id) in CAN rx fifo0 buffer by setting mask 2 */
        CANFD_SetXIDFltr(CANFD0, 2, CANFD_RX_FIFO0_EXT_MASK_LOW(0x44444), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFFF));
        /* Enable Standard ID and  Extended ID Filter as RX FOFI0*/
        CANFD_SetGFC(CANFD0, eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0, eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0, 1, 1);
        /* Enable RX fifo0 new message interrupt using interrupt line 0. */
        CANFD_EnableInt(CANFD0, (CANFD_IE_TOOE_Msk | CANFD_IE_RF0NE_Msk), 0, 0, 0);
    }
    else
    {
        /* receive 0x110~0x11F in CAN rx fifo0 buffer by setting mask 0 */
        CANFD_SetSIDFltr(CANFD0, 0, CANFD_RX_FIFO1_STD_MASK(0x110, 0x7F0));
        /* receive 0x22F in CAN rx fifo0 buffer by setting mask 1 */
        CANFD_SetSIDFltr(CANFD0, 1, CANFD_RX_FIFO1_STD_MASK(0x22F, 0x7FF));
        /* receive 0x333 in CAN rx fifo0 buffer by setting mask 2 */
        CANFD_SetSIDFltr(CANFD0, 2, CANFD_RX_FIFO1_STD_MASK(0x333, 0x7FF));

        /* receive 0x220~0x22F (29-bit id) in CAN rx fifo0 buffer by setting mask 0 */
        CANFD_SetXIDFltr(CANFD0, 0, CANFD_RX_FIFO1_EXT_MASK_LOW(0x220),CANFD_RX_FIFO1_EXT_MASK_HIGH(0x1FFFFFF0));
        /* receive 0x3333 (29-bit id) in CAN rx fifo0 buffer by setting mask 1 */
        CANFD_SetXIDFltr(CANFD0, 1, CANFD_RX_FIFO1_EXT_MASK_LOW(0x3333),CANFD_RX_FIFO1_EXT_MASK_HIGH(0x1FFFFFFF));
        /* receive 0x44444 (29-bit id) in CAN rx fifo0 buffer by setting mask 2 */
        CANFD_SetXIDFltr(CANFD0, 2, CANFD_RX_FIFO1_EXT_MASK_LOW(0x44444),CANFD_RX_FIFO1_EXT_MASK_HIGH(0x1FFFFFFF));
        /* Enable Standard ID and  Extended ID Filter as RX FOFI0*/
        CANFD_SetGFC(CANFD0, eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0, eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0, 1, 1);
        /* Enable RX fifo0 new message interrupt using interrupt line 0. */
        CANFD_EnableInt(CANFD0, (CANFD_IE_TOOE_Msk | CANFD_IE_RF1NE_Msk), 0, 0, 0);
    }

    /* CAN FD0 Run to Normal mode  */
    CANFD_RunToNormal(CANFD0, TRUE);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == 0);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == 0);

    for (u8Loop = 1 ; u8Loop < 8; u8Loop++)
    {
        CAN_TxRx_Test(&sTxMsgFrame, eCANFD_SID, u8FifoNum, 0x110 + u8Loop, u8Loop);
    }

    CAN_TxRx_Test(&sTxMsgFrame, eCANFD_SID, u8FifoNum, 0x22F, 8);
    CAN_TxRx_Test(&sTxMsgFrame, eCANFD_SID, u8FifoNum, 0x333, 8);

    for (u8Loop = 1 ; u8Loop < 8; u8Loop++)
    {
        CAN_TxRx_Test(&sTxMsgFrame, eCANFD_XID, u8FifoNum, 0x220 + u8Loop, u8Loop);
    }

    CAN_TxRx_Test(&sTxMsgFrame, eCANFD_XID, u8FifoNum, 0x3333, 8);
    CAN_TxRx_Test(&sTxMsgFrame, eCANFD_XID, u8FifoNum, 0x44444, 8);

}

void TestFunc_CANFD_TxBuffer_RxFifo0_ReadWrite()
{
    CAN_TxBuffer_RxFifo_ReadWrite(0);
    CANFD_IPReset(CANFD0);
    CANFD_TxBuffer_RxFifo_ReadWrite(0);
    CANFD_IPReset(CANFD0);
}

void TestFunc_CANFD_TxBuffer_RxFifo1_ReadWrite()
{
    CAN_TxBuffer_RxFifo_ReadWrite(1);
    CANFD_IPReset(CANFD0);
    CANFD_TxBuffer_RxFifo_ReadWrite(1);
    CANFD_IPReset(CANFD0);
}


void TestFunc_CANFD_TestMacroBR(void)
{
    CANFD_IPReset(CANFD0);
    CANFD_RunToNormal(CANFD0, FALSE);
    /*Get the CAN configuration value*/
    CANFD0->CCCR |= CANFD_CCCR_CCE_Msk;
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == CANFD_CCCR_INIT_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == CANFD_CCCR_CCE_Msk);
    CANFD0->CCCR |= (0x01 << 2);
    CU_ASSERT_EQUAL(CANFD0->CCCR & CANFD_CCCR_ASM_Msk, CANFD_CCCR_ASM_Msk);
    CANFD0->CCCR |= (0x01 << 5);
    CU_ASSERT_EQUAL(CANFD0->CCCR & CANFD_CCCR_MON_Msk, CANFD_CCCR_MON_Msk);
    CANFD0->CCCR |= (0x01 << 6);
    CU_ASSERT_EQUAL(CANFD0->CCCR & CANFD_CCCR_DAR_Msk, CANFD_CCCR_DAR_Msk);
    CANFD0->CCCR |= (0x01 << 7);
    CU_ASSERT_EQUAL(CANFD0->CCCR & CANFD_CCCR_TEST_Msk, CANFD_CCCR_TEST_Msk);
    CANFD0->CCCR |= (0x01 << 8);
    CU_ASSERT_EQUAL(CANFD0->CCCR & CANFD_CCCR_FDOE_Msk, CANFD_CCCR_FDOE_Msk);
    CANFD0->CCCR |= (0x01 << 9);
    CU_ASSERT_EQUAL(CANFD0->CCCR & CANFD_CCCR_BRSE_Msk, CANFD_CCCR_BRSE_Msk);
    CANFD0->CCCR |= (0x01 << 12);
    CU_ASSERT_EQUAL(CANFD0->CCCR & CANFD_CCCR_PXHD_Msk, CANFD_CCCR_PXHD_Msk);
    CANFD0->CCCR |= (0x01 << 13);
    CU_ASSERT_EQUAL(CANFD0->CCCR & CANFD_CCCR_EFBI_Msk, CANFD_CCCR_EFBI_Msk);
    CANFD0->CCCR |= (0x01 << 14);
    CU_ASSERT_EQUAL(CANFD0->CCCR & CANFD_CCCR_TXP_Msk, CANFD_CCCR_TXP_Msk);
    CANFD0->CCCR |= (0x01 << 15);
    CU_ASSERT_EQUAL(CANFD0->CCCR & CANFD_CCCR_NISO_Msk, CANFD_CCCR_NISO_Msk);

    CANFD0->DBTP = 0xf;
    CU_ASSERT_EQUAL(CANFD0->DBTP & CANFD_DBTP_DSJW_Msk, CANFD_DBTP_DSJW_Msk);
    CANFD0->DBTP = (0xf << 4);
    CU_ASSERT_EQUAL(CANFD0->DBTP & CANFD_DBTP_DTSEG2_Msk, CANFD_DBTP_DTSEG2_Msk);
    CANFD0->DBTP = (0x1f << 8);
    CU_ASSERT_EQUAL(CANFD0->DBTP & CANFD_DBTP_DTSEG1_Msk, CANFD_DBTP_DTSEG1_Msk);
    CANFD0->DBTP = (0x1f << 16);
    CU_ASSERT_EQUAL(CANFD0->DBTP & CANFD_DBTP_DBRP_Msk, CANFD_DBTP_DBRP_Msk);
    CANFD0->DBTP = (0x1 << 23);
    CU_ASSERT_EQUAL(CANFD0->DBTP & CANFD_DBTP_TDC_Msk, CANFD_DBTP_TDC_Msk);

    CANFD0->NBTP = 0x7f;
    CU_ASSERT_EQUAL(CANFD0->NBTP & CANFD_NBTP_NTSEG2_Msk, CANFD_NBTP_NTSEG2_Msk);
    CANFD0->NBTP = (0xff << 8);
    CU_ASSERT_EQUAL(CANFD0->NBTP & CANFD_NBTP_NTSEG1_Msk, CANFD_NBTP_NTSEG1_Msk);
    CANFD0->NBTP = (0x1ff << 16);
    CU_ASSERT_EQUAL(CANFD0->NBTP & CANFD_NBTP_NBRP_Msk, CANFD_NBTP_NBRP_Msk);
    CANFD0->NBTP = (0x7f << 25);
    CU_ASSERT_EQUAL(CANFD0->NBTP & CANFD_NBTP_NSJW_Msk, CANFD_NBTP_NSJW_Msk);
    CANFD0->GFC = 0x01;
    CU_ASSERT_EQUAL(CANFD0->GFC & CANFD_GFC_RRFE_Msk, CANFD_GFC_RRFE_Msk);
    CANFD0->GFC = (0x01 << 1);
    CU_ASSERT_EQUAL(CANFD0->GFC & CANFD_GFC_RRFS_Msk, CANFD_GFC_RRFS_Msk);
    CANFD0->GFC = (0x03 << 2);
    CU_ASSERT_EQUAL(CANFD0->GFC & CANFD_GFC_ANFE_Msk, CANFD_GFC_ANFE_Msk);
    CANFD0->GFC = (0x03 << 4);
    CU_ASSERT_EQUAL(CANFD0->GFC & CANFD_GFC_ANFS_Msk, CANFD_GFC_ANFS_Msk);

    CANFD_IPReset(CANFD0);

}

void TestFunc_UART_TestMacroFilter()
{
    CANFD_IPReset(CANFD0);
    CANFD_RunToNormal(CANFD0, FALSE);
    /*Get the CAN configuration value*/
    CANFD0->CCCR |= CANFD_CCCR_CCE_Msk;
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == CANFD_CCCR_INIT_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == CANFD_CCCR_CCE_Msk);
    CANFD0->SIDFC = (0x3fff << 2);
    CU_ASSERT_EQUAL(CANFD0->SIDFC & CANFD_SIDFC_FLSSA_Msk, CANFD_SIDFC_FLSSA_Msk);
    CANFD0->SIDFC = (0xff << 16);
    CU_ASSERT_EQUAL(CANFD0->SIDFC & CANFD_SIDFC_LSS_Msk, CANFD_SIDFC_LSS_Msk);

    CANFD0->XIDFC = (0x3fff << 2);
    CU_ASSERT_EQUAL(CANFD0->XIDFC & CANFD_XIDFC_FLESA_Msk, CANFD_XIDFC_FLESA_Msk);
    CANFD0->XIDFC = (0x7f << 16);
    CU_ASSERT_EQUAL(CANFD0->XIDFC & CANFD_XIDFC_LSE_Msk, CANFD_XIDFC_LSE_Msk);

    CANFD0->XIDAM = (0x1fffffff << 0);
    CU_ASSERT_EQUAL(CANFD0->XIDAM & CANFD_XIDAM_EIDM_Msk, CANFD_XIDAM_EIDM_Msk);
    CANFD_IPReset(CANFD0);
}

void TestFunc_UART_TestMacroMemory()
{
    CANFD_IPReset(CANFD0);
    CANFD_RunToNormal(CANFD0, FALSE);
    /*Get the CAN configuration value*/
    CANFD0->CCCR |= CANFD_CCCR_CCE_Msk;
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_INIT_Msk) == CANFD_CCCR_INIT_Msk);
    CU_ASSERT((CANFD0->CCCR & CANFD_CCCR_CCE_Msk) == CANFD_CCCR_CCE_Msk);

    CANFD0->RXF0C = (0x3fff << 2);
    CU_ASSERT_EQUAL(CANFD0->RXF0C & CANFD_RXF0C_F0SA_Msk , CANFD_RXF0C_F0SA_Msk);
    CANFD0->RXF0C = (0x7f << 16);
    CU_ASSERT_EQUAL(CANFD0->RXF0C & CANFD_RXF0C_F0S_Msk, CANFD_RXF0C_F0S_Msk);
    CANFD0->RXF0C = (0x7f << 24);
    CU_ASSERT_EQUAL(CANFD0->RXF0C & CANFD_RXF0C_F0WM_Msk, CANFD_RXF0C_F0WM_Msk);
    CANFD0->RXF0C = (0x1 << 31);
    CU_ASSERT_EQUAL(CANFD0->RXF0C & CANFD_RXF0C_F0OM_Msk, CANFD_RXF0C_F0OM_Msk);

    CANFD0->RXBC = (0x3fff << 2);
    CU_ASSERT_EQUAL(CANFD0->RXBC & CANFD_RXBC_RBSA_Msk, CANFD_RXBC_RBSA_Msk);

    CANFD0->RXF1C = (0x3fff << 2);
    CU_ASSERT_EQUAL(CANFD0->RXF1C & CANFD_RXF1C_F1SA_Msk, CANFD_RXF1C_F1SA_Msk);
    CANFD0->RXF0C = (0x7f << 16);
    CU_ASSERT_EQUAL(CANFD0->RXF0C & CANFD_RXF1C_F1S_Msk, CANFD_RXF1C_F1S_Msk);
    CANFD0->RXF0C = (0x7f << 24);
    CU_ASSERT_EQUAL(CANFD0->RXF0C & CANFD_RXF1C_F1WM_Msk, CANFD_RXF1C_F1WM_Msk);
    CANFD0->RXF0C = (0x1 << 31);
    CU_ASSERT_EQUAL(CANFD0->RXF0C & CANFD_RXF1C_F1OM_Msk, CANFD_RXF1C_F1OM_Msk);

    CANFD0->RXESC = (0x7f << 0);
    CU_ASSERT_EQUAL(CANFD0->RXESC & CANFD_RXESC_F0DS_Msk, CANFD_RXESC_F0DS_Msk);
    CANFD0->RXESC = (0x7f << 4);
    CU_ASSERT_EQUAL(CANFD0->RXESC & CANFD_RXESC_F1DS_Msk, CANFD_RXESC_F1DS_Msk);
    CANFD0->RXESC = (0x7f << 8);
    CU_ASSERT_EQUAL(CANFD0->RXESC & CANFD_RXESC_RBDS_Msk, CANFD_RXESC_RBDS_Msk);

    CANFD0->TXBC = (0x3fff << 2);
    CU_ASSERT_EQUAL(CANFD0->TXBC & CANFD_TXBC_TBSA_Msk, CANFD_TXBC_TBSA_Msk);
    CANFD0->TXBC = (0x3f << 16);
    CU_ASSERT_EQUAL(CANFD0->TXBC & CANFD_TXBC_NDTB_Msk, CANFD_TXBC_NDTB_Msk);
    CANFD0->TXBC = (0x3f << 24);
    CU_ASSERT_EQUAL(CANFD0->TXBC & CANFD_TXBC_TFQS_Msk, CANFD_TXBC_TFQS_Msk);
    CANFD0->TXBC = (0x1 << 30);
    CU_ASSERT_EQUAL(CANFD0->TXBC & CANFD_TXBC_TFQM_Msk, CANFD_TXBC_TFQM_Msk);

    CANFD0->TXBC = (0x3fff << 2);
    CU_ASSERT_EQUAL(CANFD0->TXBC & CANFD_TXBC_TBSA_Msk, CANFD_TXBC_TBSA_Msk);
    CANFD0->TXBC = (0x3f << 16);
    CU_ASSERT_EQUAL(CANFD0->TXBC & CANFD_TXBC_NDTB_Msk, CANFD_TXBC_NDTB_Msk);
    CANFD0->TXBC = (0x3f << 24);
    CU_ASSERT_EQUAL(CANFD0->TXBC & CANFD_TXBC_TFQS_Msk, CANFD_TXBC_TFQS_Msk);
    CANFD0->TXBC = (0x1 << 30);
    CU_ASSERT_EQUAL(CANFD0->TXBC & CANFD_TXBC_TFQM_Msk, CANFD_TXBC_TFQM_Msk);

    CANFD0->TXESC = (0x7 << 0);
    CU_ASSERT_EQUAL(CANFD0->TXESC & CANFD_TXESC_TBDS_Msk, CANFD_TXESC_TBDS_Msk);

    CANFD0->TXEFC = (0x3fff << 2);
    CU_ASSERT_EQUAL(CANFD0->TXEFC & CANFD_TXEFC_EFSA_Msk, CANFD_TXEFC_EFSA_Msk);
    CANFD0->TXEFC = (0x3f << 16);
    CU_ASSERT_EQUAL(CANFD0->TXEFC & CANFD_TXEFC_EFS_Msk, CANFD_TXEFC_EFS_Msk);
    CANFD0->TXEFC = (0x3f << 24);
    CU_ASSERT_EQUAL(CANFD0->TXEFC & CANFD_TXEFC_EFWM_Msk, CANFD_TXEFC_EFWM_Msk);

    CANFD_IPReset(CANFD0);
}


void TestFunc_CANFD_FilterElementTest()
{
    CANFD_STD_FILTER_T *psStdFilter;
    CANFD_EXT_FILTER_T *psExtFilter;
    CANFD_FD_T sCANFD_Config;
    CANFD_IPReset(CANFD0);
    /*Get the CAN configuration value*/
    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_FD_MODE);
    sCANFD_Config.sBtConfig.bEnableLoopBack = TRUE;
    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 1000000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 4000000;
    /*Open the CAN FD feature*/
    CANFD_Open(CANFD0, &sCANFD_Config);
    /* receive 0x110~0x11F in CAN rx fifo0 buffer by setting mask 0 */
    CANFD_SetSIDFltr(CANFD0, 0, CANFD_RX_BUFFER_STD(0x111, 0));
    psStdFilter = (CANFD_STD_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->SIDFC & CANFD_SIDFC_FLSSA_Msk) + (0 * sizeof(CANFD_STD_FILTER_T)));
    CU_ASSERT(psStdFilter->SFT  == 0);
    CU_ASSERT(psStdFilter->SFID1  == 0x111);
    CU_ASSERT(psStdFilter->SFID2  == 0);
    CU_ASSERT(psStdFilter->SFEC  == 0x7);
    /* receive 0x22F in CAN rx fifo0 buffer by setting mask 1 */
    CANFD_SetSIDFltr(CANFD0, 1, CANFD_RX_BUFFER_STD(0x222, 1));
    psStdFilter = (CANFD_STD_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->SIDFC & CANFD_SIDFC_FLSSA_Msk) + (1 * sizeof(CANFD_STD_FILTER_T)));
    CU_ASSERT(psStdFilter->SFT  == 0);
    CU_ASSERT(psStdFilter->SFID1  == 0x222);
    CU_ASSERT(psStdFilter->SFID2  == 0x01);
    CU_ASSERT(psStdFilter->SFEC  == 0x7);
    /* receive 0x333 in CAN rx fifo0 buffer by setting mask 2 */
    CANFD_SetSIDFltr(CANFD0, 2, CANFD_RX_BUFFER_STD(0x333, 2));
    psStdFilter = (CANFD_STD_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->SIDFC & CANFD_SIDFC_FLSSA_Msk) + (2 * sizeof(CANFD_STD_FILTER_T)));
    CU_ASSERT(psStdFilter->SFT  == 0);
    CU_ASSERT(psStdFilter->SFID1  == 0x333);
    CU_ASSERT(psStdFilter->SFID2  == 0x02);
    CU_ASSERT(psStdFilter->SFEC  == 0x7);
    /* receive 0x110~0x11F in CAN rx fifo0 buffer by setting mask 0 */
    CANFD_SetSIDFltr(CANFD0, 3, CANFD_RX_FIFO0_STD_MASK(0x444, 0x7F0));
    psStdFilter = (CANFD_STD_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->SIDFC & CANFD_SIDFC_FLSSA_Msk) + (3 * sizeof(CANFD_STD_FILTER_T)));
    CU_ASSERT(psStdFilter->SFT  == 2);
    CU_ASSERT(psStdFilter->SFID1  == 0x444);
    CU_ASSERT(psStdFilter->SFID2  == 0x7F0);
    CU_ASSERT(psStdFilter->SFEC  == 0x1);
    /* receive 0x22F in CAN rx fifo0 buffer by setting mask 1 */
    CANFD_SetSIDFltr(CANFD0, 4, CANFD_RX_FIFO0_STD_MASK(0x555, 0x7FF));
    psStdFilter = (CANFD_STD_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->SIDFC & CANFD_SIDFC_FLSSA_Msk) + (4 * sizeof(CANFD_STD_FILTER_T)));
    CU_ASSERT(psStdFilter->SFT  == 2);
    CU_ASSERT(psStdFilter->SFID1  == 0x555);
    CU_ASSERT(psStdFilter->SFID2  == 0x7FF);
    CU_ASSERT(psStdFilter->SFEC  == 0x1);
    /* receive 0x333 in CAN rx fifo0 buffer by setting mask 2 */
    CANFD_SetSIDFltr(CANFD0, 5, CANFD_RX_FIFO0_STD_MASK(0x666, 0x7FF));
    psStdFilter = (CANFD_STD_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->SIDFC & CANFD_SIDFC_FLSSA_Msk) + (5 * sizeof(CANFD_STD_FILTER_T)));
    CU_ASSERT(psStdFilter->SFT  == 2);
    CU_ASSERT(psStdFilter->SFID1  == 0x666);
    CU_ASSERT(psStdFilter->SFID2  == 0x7FF);
    CU_ASSERT(psStdFilter->SFEC  == 0x1);
    /* receive 0x110~0x11F in CAN rx fifo0 buffer by setting mask 0 */
    CANFD_SetSIDFltr(CANFD0, 6, CANFD_RX_FIFO1_STD_MASK(0x777, 0x7F0));
    psStdFilter = (CANFD_STD_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->SIDFC & CANFD_SIDFC_FLSSA_Msk) + (6 * sizeof(CANFD_STD_FILTER_T)));
    CU_ASSERT(psStdFilter->SFT  == 2);
    CU_ASSERT(psStdFilter->SFID1  == 0x777);
    CU_ASSERT(psStdFilter->SFID2  == 0x7F0);
    CU_ASSERT(psStdFilter->SFEC  == 0x2);
    /* receive 0x22F in CAN rx fifo0 buffer by setting mask 1 */
    CANFD_SetSIDFltr(CANFD0, 7, CANFD_RX_FIFO1_STD_MASK(0x2FF, 0x7FF));
    psStdFilter = (CANFD_STD_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->SIDFC & CANFD_SIDFC_FLSSA_Msk) + (7 * sizeof(CANFD_STD_FILTER_T)));
    CU_ASSERT(psStdFilter->SFT  == 2);
    CU_ASSERT(psStdFilter->SFID1  == 0x2FF);
    CU_ASSERT(psStdFilter->SFID2  == 0x7FF);
    CU_ASSERT(psStdFilter->SFEC  == 0x2);
    /* receive 0x333 in CAN rx fifo0 buffer by setting mask 2 */
    CANFD_SetSIDFltr(CANFD0, 8, CANFD_RX_FIFO1_STD_MASK(0x7FF, 0x7FF));
    psStdFilter = (CANFD_STD_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0)+ (CANFD0->SIDFC & CANFD_SIDFC_FLSSA_Msk) + (8 * sizeof(CANFD_STD_FILTER_T)));
    CU_ASSERT(psStdFilter->SFT  == 2);
    CU_ASSERT(psStdFilter->SFID1  == 0x7FF);
    CU_ASSERT(psStdFilter->SFID2  == 0x7FF);
    CU_ASSERT(psStdFilter->SFEC  == 0x2);

    /* receive 0x220~0x22F (29-bit id) in CAN rx fifo0 buffer by setting mask 0 */
    CANFD_SetXIDFltr(CANFD0, 0, CANFD_RX_BUFFER_EXT_LOW(0x11111111, 0), CANFD_RX_BUFFER_EXT_HIGH(0x11111111, 0));
    psExtFilter = (CANFD_EXT_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->XIDFC & CANFD_XIDFC_FLESA_Msk) + (0 * sizeof(CANFD_EXT_FILTER_T)));
    CU_ASSERT(psExtFilter->EFEC  == 0x7);
    CU_ASSERT(psExtFilter->EFID1  == 0x11111111);
    CU_ASSERT(psExtFilter->EFID2  == 0);
    CU_ASSERT(psExtFilter->EFT  == 0);
    /* receive 0x3333 (29-bit id) in CAN rx fifo0 buffer by setting mask 1 */
    CANFD_SetXIDFltr(CANFD0, 1, CANFD_RX_BUFFER_EXT_LOW(0x12222222, 1), CANFD_RX_BUFFER_EXT_HIGH(0x12222222, 1));
    psExtFilter = (CANFD_EXT_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->XIDFC & CANFD_XIDFC_FLESA_Msk) + (1 * sizeof(CANFD_EXT_FILTER_T)));
    CU_ASSERT(psExtFilter->EFEC  == 0x7);
    CU_ASSERT(psExtFilter->EFID1  == 0x12222222);
    CU_ASSERT(psExtFilter->EFID2  == 0x1);
    CU_ASSERT(psExtFilter->EFT  == 0);
    /* receive 0x44444 (29-bit id) in CAN rx fifo0 buffer by setting mask 2 */
    CANFD_SetXIDFltr(CANFD0, 2, CANFD_RX_BUFFER_EXT_LOW(0x13333333, 2), CANFD_RX_BUFFER_EXT_HIGH(0x13333333, 2));
    psExtFilter = (CANFD_EXT_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->XIDFC & CANFD_XIDFC_FLESA_Msk) + (2 * sizeof(CANFD_EXT_FILTER_T)));
    CU_ASSERT(psExtFilter->EFEC  == 0x7);
    CU_ASSERT(psExtFilter->EFID1  == 0x13333333);
    CU_ASSERT(psExtFilter->EFID2  == 0x2);
    CU_ASSERT(psExtFilter->EFT  == 0);
    /* receive 0x220~0x22F (29-bit id) in CAN rx fifo0 buffer by setting mask 0 */
    CANFD_SetXIDFltr(CANFD0, 3, CANFD_RX_FIFO0_EXT_MASK_LOW(0x14444444), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFF0));
    psExtFilter = (CANFD_EXT_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0)+ (CANFD0->XIDFC & CANFD_XIDFC_FLESA_Msk) + (3 * sizeof(CANFD_EXT_FILTER_T)));
    CU_ASSERT(psExtFilter->EFEC  == 0x1);
    CU_ASSERT(psExtFilter->EFID1  == 0x14444444);
    CU_ASSERT(psExtFilter->EFID2  == 0x1FFFFFF0);
    CU_ASSERT(psExtFilter->EFT  == 0x2);
    /* receive 0x3333 (29-bit id) in CAN rx fifo0 buffer by setting mask 1 */
    CANFD_SetXIDFltr(CANFD0, 4, CANFD_RX_FIFO0_EXT_MASK_LOW(0x15555555), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFFF));
    psExtFilter = (CANFD_EXT_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->XIDFC & CANFD_XIDFC_FLESA_Msk) + (4 * sizeof(CANFD_EXT_FILTER_T)));
    CU_ASSERT(psExtFilter->EFEC  == 0x1);
    CU_ASSERT(psExtFilter->EFID1  == 0x15555555);
    CU_ASSERT(psExtFilter->EFID2  == 0x1FFFFFFF);
    CU_ASSERT(psExtFilter->EFT  == 0x2);
    /* receive 0x44444 (29-bit id) in CAN rx fifo0 buffer by setting mask 2 */
    CANFD_SetXIDFltr(CANFD0, 5, CANFD_RX_FIFO0_EXT_MASK_LOW(0x16666666), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFFF));
    psExtFilter = (CANFD_EXT_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->XIDFC & CANFD_XIDFC_FLESA_Msk) + (5 * sizeof(CANFD_EXT_FILTER_T)));
    CU_ASSERT(psExtFilter->EFEC  == 0x1);
    CU_ASSERT(psExtFilter->EFID1  == 0x16666666);
    CU_ASSERT(psExtFilter->EFID2  == 0x1FFFFFFF);
    CU_ASSERT(psExtFilter->EFT  == 0x2);
    /* receive 0x220~0x22F (29-bit id) in CAN rx fifo0 buffer by setting mask 0 */
    CANFD_SetXIDFltr(CANFD0, 6, CANFD_RX_FIFO1_EXT_MASK_LOW(0x17777777), CANFD_RX_FIFO1_EXT_MASK_HIGH(0x1FFFFFF0));
    psExtFilter = (CANFD_EXT_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->XIDFC & CANFD_XIDFC_FLESA_Msk) + (6 * sizeof(CANFD_EXT_FILTER_T)));
    CU_ASSERT(psExtFilter->EFEC  == 0x2);
    CU_ASSERT(psExtFilter->EFID1  == 0x17777777);
    CU_ASSERT(psExtFilter->EFID2  == 0x1FFFFFF0);
    CU_ASSERT(psExtFilter->EFT  == 0x2);

    /* receive 0x3333 (29-bit id) in CAN rx fifo0 buffer by setting mask 1 */
    CANFD_SetXIDFltr(CANFD0, 7, CANFD_RX_FIFO1_EXT_MASK_LOW(0x1AAAAAAA), CANFD_RX_FIFO1_EXT_MASK_HIGH(0x1FFFFFFF));
    psExtFilter = (CANFD_EXT_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->XIDFC & CANFD_XIDFC_FLESA_Msk) + (7 * sizeof(CANFD_EXT_FILTER_T)));
    CU_ASSERT(psExtFilter->EFEC  == 0x2);
    CU_ASSERT(psExtFilter->EFID1  == 0x1AAAAAAA);
    CU_ASSERT(psExtFilter->EFID2  == 0x1FFFFFFF);
    CU_ASSERT(psExtFilter->EFT  == 0x2);
    /* receive 0x44444 (29-bit id) in CAN rx fifo0 buffer by setting mask 2 */
    CANFD_SetXIDFltr(CANFD0, 8, CANFD_RX_FIFO1_EXT_MASK_LOW(0x1FFFFFFF), CANFD_RX_FIFO1_EXT_MASK_HIGH(0x1FFFFFFF));
    psExtFilter = (CANFD_EXT_FILTER_T *)(CANFD_SRAM_BASE_ADDR(CANFD0) + (CANFD0->XIDFC & CANFD_XIDFC_FLESA_Msk) + (8 * sizeof(CANFD_EXT_FILTER_T)));
    CU_ASSERT(psExtFilter->EFEC  == 0x2);
    CU_ASSERT(psExtFilter->EFID1  == 0x1FFFFFFF);
    CU_ASSERT(psExtFilter->EFID2  == 0x1FFFFFFF);
    CU_ASSERT(psExtFilter->EFT  == 0x2);

}


CU_TestInfo CANFD_ApiTests[] =
{
    {"CANFD Element Star address Function.", TestFunc_CANFD_TxRx_Buffer_Address},
    {"CANFD Enable/Disable Interrupt Function.", TestFunc_CANFD_EnableInt},
    {"CANFD Open Function.", TestFunc_CANFD_Open},
    {"CANFD SID/XID Filter Element Test.", TestFunc_CANFD_FilterElementTest},
    {"CANFD Tx Buffer/Rx Buffer Function.", TestFunc_CANFD_TxBuffer_RxBuffer_ReadWrite},
    {"CANFD Tx Buffer/Rx FIFO0 Function.", TestFunc_CANFD_TxBuffer_RxFifo0_ReadWrite},
    {"CANFD Tx Buffer/Rx FIFO1 Function.", TestFunc_CANFD_TxBuffer_RxFifo1_ReadWrite},
    CU_TEST_INFO_NULL
};


CU_TestInfo CANFD_MacroTests[] =
{
    {"Macro about baud rate.", TestFunc_CANFD_TestMacroBR},
    {"Macro about Filter.", TestFunc_UART_TestMacroFilter},
    {"Macro about Memory.", TestFunc_UART_TestMacroMemory},
    CU_TEST_INFO_NULL
};

