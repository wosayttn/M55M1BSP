/***************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate how to update chip flash data through CANFD interface.
 *           Nuvoton NuMicro ISP Programming Tool is also required in this
 *           sample code to connect with chip CANFD and assign update file
 *           of Flash.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define PLL_CLOCK       FREQ_180MHZ

#define GPIO_SETMODE(port, pin, u32Mode) ((port)->MODE = ((port)->MODE & ~(0x3ul << ((pin) << 1))) | ((u32Mode) << ((pin) << 1)))

#define CAN_BAUD_RATE                     500000
#define Master_ISP_ID                     0x487
#define Device0_ISP_ID                    0x784
#define CAN_ISP_DtatLength                0x08
#define CAN_RETRY_COUNTS                  0x1fffffff

#define CMD_READ_CONFIG                   0xA2000000
#define CMD_RUN_APROM                     0xAB000000
#define CMD_GET_DEVICEID                  0xB1000000

/*---------------------------------------------------------------------------*/
/*  Function Declare                                                         */
/*---------------------------------------------------------------------------*/
/* Declare a CAN message structure */
typedef struct
{
    uint32_t  Address;
    uint32_t  Data;
} STR_CANMSG_ISP_T;

static volatile CANFD_FD_MSG_T g_sRxMsgFrame;
static volatile uint8_t s_u8CANPackageFlag = 0, s_u8CANAckFlag = 0;

void CANFD00_IRQHandler(void);
int32_t SYS_Init(void);
void CAN_Package_ACK(CANFD_T *psCanfd);
void CAN_Init(void);
void ProcessHardFault(void);
void SH_Return(void);
void SendChar_ToUART(int ch);
uint32_t CAN_Parsing_MSG(uint8_t *u8pMsg);

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle CAN FD0 Line0 interrupt event                                                             */
/*---------------------------------------------------------------------------------------------------------*/
void CANFD00_IRQHandler(void)
{
    uint32_t u32IIDRstatus;

    u32IIDRstatus = CANFD0->IR;

    /**************************/
    /* Status Change interrupt*/
    /**************************/
    if ((u32IIDRstatus & CANFD_IR_RF0N_Msk) == CANFD_IR_RF0N_Msk)
    {
        /*Clear the Interrupt flag */
        CANFD_ClearStatusFlag(CANFD0, CANFD_IR_TOO_Msk | CANFD_IR_RF0N_Msk);
        CANFD_ReadRxFifoMsg(CANFD0, 0, (CANFD_FD_MSG_T *)&g_sRxMsgFrame);
        s_u8CANPackageFlag = 1;
    }
}

int32_t SYS_Init(void)
{
    uint32_t u32TimeOutCnt = SystemCoreClock >> 1; /* 500ms time-out */

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable Internal RC clock */
    CLK->SRCCTL |= (CLK_SRCCTL_HIRCEN_Msk | CLK_SRCCTL_HIRC48MEN_Msk);

    /* Waiting for Internal RC clock ready */
    while ((CLK->STATUS & (CLK_STATUS_HIRCSTB_Msk | CLK_STATUS_HIRC48MSTB_Msk)) != (CLK_STATUS_HIRCSTB_Msk | CLK_STATUS_HIRC48MSTB_Msk))
    {
        if (--u32TimeOutCnt == 0)
        {
            return -1;
        }
    }

    /* Set SCLK clock source as HIRC first */
    CLK->SCLKSEL = (CLK->SCLKSEL & (~CLK_SCLKSEL_SCLKSEL_Msk)) | CLK_SCLKSEL_SCLKSEL_HIRC;

    /* Disable PLL clock before setting PLL frequency */
    CLK->SRCCTL &= ~CLK_SRCCTL_APLL0EN_Msk;

    /* Set PLL clock as 180MHz from HIRC */
    CLK->APLL0CTL = CLK_APLLCTL_180MHz;

    /* Wait for PLL clock ready */
    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
    while (!(CLK->STATUS & CLK_STATUS_APLL0STB_Msk))
    {
        if (--u32TimeOutCnt == 0)
            return -1;
    }

    /* Switch to power level 0 for safe */
    PMC->PLCTL = (PMC->PLCTL & (~PMC_PLCTL_PLSEL_Msk)) | PMC_PLCTL_PLSEL_PL0;

    /* Set Flash Access Cycle to 8 for safe */
    FMC->CYCCTL = (FMC->CYCCTL & (~FMC_CYCCTL_CYCLE_Msk)) | (8);

    /* Switch SCLK to new SCLK source */
    CLK->SCLKSEL = (CLK->SCLKSEL & (~CLK_SCLKSEL_SCLKSEL_Msk)) | CLK_SCLKSEL_SCLKSEL_APLL0;

    /* Set power level according to new SCLK */
    PMC->PLCTL = (PMC->PLCTL & (~PMC_PLCTL_PLSEL_Msk)) | PMC_PLCTL_PLSEL_PL1;

    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */

    while (PMC->PLSTS & PMC_PLSTS_PLCBUSY_Msk)
    {
        if (u32TimeOutCnt-- == 0) break;
    }

    /* Switch flash access cycle to suitable value base on SCLK */
    FMC->CYCCTL = (FMC->CYCCTL & (~FMC_CYCCTL_CYCLE_Msk)) | (6);

    /* Set HCLK2 divide 2 */
    CLK_SET_HCLK2DIV(2);

    /* Set PCLKx divide 2 */
    CLK_SET_PCLK0DIV(2);
    CLK_SET_PCLK1DIV(2);
    CLK_SET_PCLK2DIV(2);
    CLK_SET_PCLK3DIV(2);
    CLK_SET_PCLK4DIV(2);

    /* Update System Core Clock */
    PllClock        = PLL_CLOCK;
    SystemCoreClock = PllClock;
    CyclesPerUs = SystemCoreClock / 1000000UL;  /* For CLK_SysTickDelay() */

    /* Select CAN FD0 clock source is HCLK */
    CLK->CANFDDIV = (CLK->CANFDDIV & (~CLK_CANFDDIV_CANFD0DIV_Msk)) | CLK_CANFDDIV_CANFD0DIV(1);
    CLK->CANFDSEL = (CLK->CANFDSEL & (~CLK_CANFDSEL_CANFD0SEL_Msk)) | CLK_CANFDSEL_CANFD0SEL_HCLK0;

    /* Enable CAN FD0 peripheral clock */
    CLK->CANFDCTL |= CLK_CANFDCTL_CANFD0CKEN_Msk;

    /* Enable GPIO B clock */
    CLK->GPIOCTL |= CLK_GPIOCTL_GPIOCCKEN_Msk;

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PC multi-function pins for CAN FD0 RXD and TXD */
    SET_CAN0_RXD_PC4();
    SET_CAN0_TXD_PC5();

    return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Tx Msg by Normal Mode Function (With Message RAM)                                                      */
/*---------------------------------------------------------------------------------------------------------*/
void CAN_Package_ACK(CANFD_T *psCanfd)
{
    CANFD_FD_MSG_T  sTxMsgFrame;
    s_u8CANAckFlag = 1;
    /* Send a 11-bit Standard Identifier message */
    sTxMsgFrame.eFrmType = eCANFD_DATA_FRM;
    sTxMsgFrame.eIdType  = eCANFD_SID;
    sTxMsgFrame.u32Id    = Device0_ISP_ID;
    sTxMsgFrame.u32DLC   = CAN_ISP_DtatLength;

    sTxMsgFrame.au32Data[0] = g_sRxMsgFrame.au32Data[0];
    sTxMsgFrame.au32Data[1] = g_sRxMsgFrame.au32Data[1];

    if (CANFD_TransmitTxMsg(psCanfd, 0, &sTxMsgFrame) == FALSE)    // Configure Msg RAM and send the Msg in the RAM
    {
        return;
    }
}

void CAN_Init(void)
{
    CANFD_FD_T sCANFD_Config;

    /* Set CAN transceiver to high speed mode */
    GPIO_SETMODE(PC, 11, GPIO_MODE_OUTPUT);
    PC11 = 0;

    /* Get the CAN configuration value */
    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_MODE);
    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = CAN_BAUD_RATE;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 0;

    /* Open the CAN feature */
    CANFD_Open(CANFD0, &sCANFD_Config);
    NVIC_EnableIRQ(CANFD00_IRQn);

    /* Set CAN reveive message */
    CANFD_SetSIDFltr(CANFD0, 0, CANFD_RX_FIFO0_STD_MASK(Master_ISP_ID, 0x7FF));

    /* Enable Standard ID and  Extended ID Filter as RX FOFI0*/
    CANFD_SetGFC(CANFD0, eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0, eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0, 1, 1);
    /* Enable RX fifo0 new message interrupt using interrupt line 0. */
    CANFD_EnableInt(CANFD0, (CANFD_IE_TOOE_Msk | CANFD_IE_RF0NE_Msk | CANFD_IE_TCE_Msk), 0, 0, 0);

    /* CAN FD0 Run to Normal mode  */
    CANFD_RunToNormal(CANFD0, TRUE);
}

/*---------------------------------------------------------------------------------------------------------*/
/*                                    Fini CAN FD0                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void CANFD_Fini(void)
{
    NVIC_DisableIRQ(CANFD00_IRQn);
    CANFD_Close(CANFD0);
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    /* Unlock write-protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    if (SYS_Init() < 0)
        goto lexit;

    /* Enable ISP */
    FMC_Open();
    FMC_ENABLE_AP_UPDATE();

    SCB->VTOR = FMC_LDROM_BASE;
    /* Init CAN port */
    CAN_Init();

    SysTick->LOAD = 300000 * CyclesPerUs;
    SysTick->VAL  = (0x00);
    SysTick->CTRL = SysTick->CTRL | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    while (1)
    {
        if (s_u8CANPackageFlag == 1)
        {
            break;
        }

        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
        {
            goto lexit;
        }
    }

    /* state of update program */
    while (1)
    {
        if (s_u8CANPackageFlag)
        {
            volatile STR_CANMSG_ISP_T *psISPCanMsg = (STR_CANMSG_ISP_T *)&g_sRxMsgFrame.au32Data[0];
            s_u8CANPackageFlag = 0;

            if (psISPCanMsg->Address == CMD_GET_DEVICEID)
            {
                psISPCanMsg->Data = SYS->PDID;
            }
            else if (psISPCanMsg->Address == CMD_READ_CONFIG)
            {
                psISPCanMsg->Data = FMC_Read(psISPCanMsg->Data);
            }
            else if (psISPCanMsg->Address == CMD_RUN_APROM)
            {
                break;
            }
            else
            {
                PD2 = 0;
                if ((psISPCanMsg->Address % FMC_FLASH_PAGE_SIZE) == 0)
                {
                    FMC_Erase(psISPCanMsg->Address);
                }

                FMC_Write(psISPCanMsg->Address, psISPCanMsg->Data);
                psISPCanMsg->Data = FMC_Read(psISPCanMsg->Address);
                PD2 = 1;
            }

            /* send CAN FD ISP Package (ACK) */
            CAN_Package_ACK(CANFD0);
        }
    }

lexit:

    /* CAN FD interface finalization */
    CANFD_Fini();

    /* Trap the CPU */
    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/