
/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "i2c_cunit.h"


extern uint32_t I2C_SetBusClockFreq(I2C_T *i2c, uint32_t u32BusClock);
/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

#if 0
#define DBG_MSG printf
#else
#define DBG_MSG(...)
#endif


#define SMBUS_ALERT_RESPONSE_ADDRESS 0x0C
#define SMBUS_DEFAULT_ADDRESS        0x61
#define ARP_COMMAND 0x01

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
const uint8_t g_u8SlaveAddr[4] = {0x18, 0x38, 0x58, 0x78};

volatile uint32_t slave_buff_addr;
volatile uint8_t g_u8SlvData[256];
volatile uint8_t g_au8RxData[4];
volatile uint8_t g_u8DeviceAddr;
volatile uint8_t g_u8ARA_Addr;
volatile uint8_t g_au8TxData[4];
volatile uint8_t g_u8RxData;
volatile uint8_t g_u8DataLen0;
volatile uint8_t g_u8DataLen1;
volatile uint8_t g_u8EndFlag = 0;
volatile uint8_t g_u8SendPEC = 0;
volatile uint8_t g_u8AlertInt0 = 0;
volatile uint8_t g_u8AlertInt1 = 0;
volatile uint8_t g_u8AlertAddrAck0 = 0;
volatile uint8_t g_u8AlertAddrAck1 = 0;

typedef void (*I2C_FUNC)(uint32_t u32Status);

static volatile I2C_FUNC s_I2C0HandlerFn = NULL;
static volatile I2C_FUNC s_I2C1HandlerFn = NULL;


/*Variables for read, write APIs test*/
extern I2C_FUNC fpI2C_WrRd_Test_Handler;


/*---------------------------------------------------------------------------------------------------------*/
/*  Power Wake-up IRQ Handler                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void PMC_IRQHandler(void)
{
    printf("@PMC_IRQ!!!");

    /* check power down wakeup flag */
    if ((PMC->INTSTS & PMC_INTSTS_PDWKIF_Msk) == PMC_INTSTS_PDWKIF_Msk) {
        printf("PMC_IRQ!!! PMC_INTSTS = 0x%08x\n", PMC->INTSTS);
    }

    PMC->INTSTS |= PMC_INTSTS_CLRWK_Msk;

    while (PMC->INTSTS & PMC_INTSTS_PDWKIF_Msk);

    __DSB();
    __ISB();
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_IRQHandler(void)
{
    uint32_t u32Status;
    u32Status = I2C_GET_STATUS(I2C0);
//    printf("I2C0_STA: %08X\n", u32Status);

    /* Check Transmit byte done interrupt flag */
    if ((I2C_SMBusGetStatus(I2C0) & I2C_BUSSTS_BCDONE_Msk) == I2C_BUSSTS_BCDONE_Msk) {
        I2C_SMBusClearInterruptFlag(I2C0, I2C_BUSSTS_BCDONE_Msk);
        printf("I2C0 Byte Transmit Byte Done Interrupt !\n");
        return;
    }

    /* Occur receive PEC packet error */
    if ((I2C_SMBusGetStatus(I2C0) & I2C_BUSSTS_PECERR_Msk) == I2C_BUSSTS_PECERR_Msk) {
        I2C_SMBusClearInterruptFlag(I2C0, I2C_BUSSTS_PECERR_Msk);
        printf("I2C0 PEC Error Interrupt !\n");
        return;
    }

    /* Check Alert Interrupt when I2C0 is Host */
    if (((I2C_SMBusGetStatus(I2C0) & I2C_BUSSTS_ALERT_Msk) == I2C_BUSSTS_ALERT_Msk) &
            ((I2C0->BUSCTL & I2C_BUSCTL_BMHEN_Msk) == I2C_BUSCTL_BMHEN_Msk)) {
        I2C_SMBusClearInterruptFlag(I2C0, I2C_BUSSTS_ALERT_Msk);
        printf("I2C0 Alert Interrupt !\n");
        g_u8AlertInt0 = 1;
        return ;
    }

    if (I2C_GET_TIMEOUT_FLAG(I2C0)) {
        /* Clear I2C0 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C0);
    } else {
        if (s_I2C0HandlerFn != NULL) {
            s_I2C0HandlerFn(u32Status);
        }
    }

    __DSB();
    __ISB();
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C1 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C1_IRQHandler(void)
{
    uint32_t u32Status;
    uint8_t u8data;
    u32Status = I2C_GET_STATUS(I2C1);
//    printf("I2C1_STA: %08X\n", u32Status);

    if (fpI2C_WrRd_Test_Handler) { //if not point to null=> test Wr, Rd APIs.
        /*For test APIs:
            I2C_WriteByte,
            I2C_WriteMultiBytes
            I2C_WriteByteOneReg
            I2C_WriteMultiBytesOneReg
            I2C_WriteByteTwoRegs
            I2C_WriteMultiBytesTwoRegs
            I2C_ReadByte
            I2C_ReadMultiBytes
            I2C_ReadByteOneReg
            I2C_ReadMultiBytesOneReg
            I2C_ReadByteTwoRegs
            I2C_ReadMultiBytesTwoRegs
        */
        fpI2C_WrRd_Test_Handler(u32Status);
        __DSB();
        __ISB();
        return;
    }

    /* Check Transmit byte done interrupt flag */
    if ((I2C_SMBusGetStatus(I2C1) & I2C_BUSSTS_BCDONE_Msk) == I2C_BUSSTS_BCDONE_Msk) {
        I2C_SMBusClearInterruptFlag(I2C1, I2C_BUSSTS_BCDONE_Msk);
        printf("I2C1 Byte Receive Byte Done Interrupt !\n");
        return;
    }

    /* Occur receive PEC packet error */
    if ((I2C_SMBusGetStatus(I2C1) & I2C_BUSSTS_PECERR_Msk) == I2C_BUSSTS_PECERR_Msk) {
        I2C_SMBusClearInterruptFlag(I2C1, I2C_BUSSTS_PECERR_Msk);
        printf("I2C1 PEC Error Interrupt !\n");
        return;
    }

    /* Check Alert Interrupt when I2C1 is Host */
    if (((I2C_SMBusGetStatus(I2C1) & I2C_BUSSTS_ALERT_Msk) == I2C_BUSSTS_ALERT_Msk) &
            ((I2C1->BUSCTL & I2C_BUSCTL_BMHEN_Msk) == I2C_BUSCTL_BMHEN_Msk)) {
        I2C_SMBusClearInterruptFlag(I2C1, I2C_BUSSTS_ALERT_Msk);
        printf("I2C1 Alert Interrupt !\n");
        g_u8AlertInt1 = 1;
        return ;
    }

    if (I2C_GET_TIMEOUT_FLAG(I2C1)) {
        /* Clear I2C1 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C1);
    } else {
        if (s_I2C1HandlerFn != NULL) {
            s_I2C1HandlerFn(u32Status);
        }
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Master Rx Callback Function                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterRx(uint32_t u32Status)
{
    if (u32Status == 0x08) {                         /* START has been transmitted and prepare SLA+W */
        I2C_SET_DATA(I2C0, g_u8DeviceAddr << 1);     /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
    } else if (u32Status == 0x18) {                  /* SLA+W has been transmitted and ACK has been received */
        I2C_SET_DATA(I2C0, g_au8TxData[g_u8DataLen0++]);
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
    } else if (u32Status == 0x20) {                  /* SLA+W has been transmitted and NACK has been received */
        I2C_STOP(I2C0);
        I2C_START(I2C0);
    } else if (u32Status == 0x28) {                  /* DATA has been transmitted and ACK has been received */
        if (g_u8DataLen0 != 2) {
            I2C_SET_DATA(I2C0, g_au8TxData[g_u8DataLen0++]);
            I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
        } else {
            I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STA_SI);
        }
    } else if (u32Status == 0x10) {             /* Repeat START has been transmitted and prepare SLA+R */
        I2C_SET_DATA(I2C0, ((g_u8DeviceAddr << 1) | 0x01));   /* Write SLA+R to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
    } else if (u32Status == 0x40) {             /* SLA+R has been transmitted and ACK has been received */
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
    } else if (u32Status == 0x58) {             /* DATA has been received and NACK has been returned */
        g_u8RxData = (unsigned char) I2C_GET_DATA(I2C0);
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STO_SI);
        g_u8EndFlag = 1;
    } else {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Master Tx Callback Function                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterTx(uint32_t u32Status)
{
    if (u32Status == 0x08) {                    /* START has been transmitted */
        I2C_SET_DATA(I2C0, g_u8DeviceAddr << 1);     /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
    } else if (u32Status == 0x18) {             /* SLA+W has been transmitted and ACK has been received */
        I2C_SET_DATA(I2C0, g_au8TxData[g_u8DataLen0++]);
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
    } else if (u32Status == 0x20) {             /* SLA+W has been transmitted and NACK has been received */
        I2C_STOP(I2C0);
        I2C_START(I2C0);
    } else if (u32Status == 0x28) {             /* DATA has been transmitted and ACK has been received */
        if (g_u8DataLen0 != 3) {
            I2C_SET_DATA(I2C0, g_au8TxData[g_u8DataLen0++]);
            I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
        } else {
            if (g_u8SendPEC == 0) {
                g_u8SendPEC = 1;
                I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
            } else {
                I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STO_SI);
                g_u8EndFlag = 1;
            }
        }
    } else if (u32Status == 0xF8) {
    } else {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Master Alert Callback Function                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterAlert(uint32_t u32Status)
{
    if (u32Status == 0x08) {                    /* START has been transmitted */
        I2C_SET_DATA(I2C0, (g_u8DeviceAddr << 1) + 1);             /* Write SLA+R to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
    } else if (u32Status == 0x18) {             /* SLA+W has been transmitted and ACK has been received */
        I2C_SET_DATA(I2C0, g_au8TxData[g_u8DataLen0++]);
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
    } else if (u32Status == 0x20) {             /* SLA+W has been transmitted and NACK has been received */
        I2C_STOP(I2C0);
        I2C_START(I2C0);
    } else if (u32Status == 0x40) {
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    } else if (u32Status == 0x50) {
        if (g_u8DataLen0 == 0) {
            g_au8RxData[g_u8DataLen0] = (unsigned char) I2C_GET_DATA(I2C0);
            I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
            g_u8DataLen0++;
        } else {
            g_au8RxData[g_u8DataLen0] = (unsigned char) I2C_GET_DATA(I2C0);
            I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STO_SI_AA);
            g_u8AlertAddrAck0 = 1;
        }
    } else if (u32Status == 0x28) {             /* DATA has been transmitted and ACK has been received */
        if (g_u8DataLen0 != 3) {
            I2C_SET_DATA(I2C0, g_au8TxData[g_u8DataLen0++]);
            I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
        } else {
            if (g_u8SendPEC == 0) {
                g_u8SendPEC = 1;
                I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
            } else {
                I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STO_SI);
                g_u8EndFlag = 1;
            }
        }
    } else {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Master Default Address and Acknowledge by Manual Callback Function                                 */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterDefaultAddrACKM(uint32_t u32Status)
{
    if (u32Status == 0x08) {                         /* START has been transmitted */
        I2C_SET_DATA(I2C0, g_u8DeviceAddr << 1);     /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
    } else if (u32Status == 0x18) {                  /* SLA+W has been transmitted and ACK has been received */
        I2C_SET_DATA(I2C0, g_au8TxData[g_u8DataLen0++]);
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
    } else if (u32Status == 0x20) {                  /* SLA+W has been transmitted and NACK has been received */
        I2C_STOP(I2C0);
        I2C_START(I2C0);
    } else if (u32Status == 0x28) {                  /* DATA has been transmitted and ACK has been received */
        if (g_u8SendPEC == 0) {
            g_u8SendPEC = 1;
            I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
        } else {
            I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STO_SI);
            g_u8EndFlag = 1;
        }
    } else if (u32Status == 0xF8) {
    } else {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

void I2C0_Init(void)
{
    /* Reset I2C0 */
    SYS_ResetModule(SYS_I2C0RST);
    /* Open I2C module and set bus clock */
    I2C_Open(I2C0, 100000);
    /* Get I2C0 Bus Clock */
    printf("I2C0 clock %d Hz\n", I2C_GetBusClockFreq(I2C0));
    /* Set I2C0 4 Slave addresses */
    /* Slave address : 0x15 */
    I2C_SetSlaveAddr(I2C0, 0, 0x15, 0);   /* Slave Address : 0x15 */
    /* Slave address : 0x35 */
    I2C_SetSlaveAddr(I2C0, 1, 0x35, 0);   /* Slave Address : 0x35 */
    /* Slave address : 0x55 */
    I2C_SetSlaveAddr(I2C0, 2, 0x55, 0);   /* Slave Address : 0x55 */
    /* Slave address : 0x75 */
    I2C_SetSlaveAddr(I2C0, 3, 0x75, 0);   /* Slave Address : 0x75 */
    /* Enable I2C0 interrupt */
    I2C_EnableInt(I2C0);
    NVIC_EnableIRQ(I2C0_IRQn);
}

void I2C1_Init(void)
{
    /* Reset I2C1 */
    SYS_ResetModule(SYS_I2C1RST);
    /* Open I2C module and set bus clock */
    I2C_Open(I2C1, 100000);
    /* Get I2C0 Bus Clock */
    printf("I2C1 clock %d Hz\n", I2C_GetBusClockFreq(I2C1));
    /* Set I2C1 4 Slave addresses */
    /* Slave address : 0x18 */
    I2C_SetSlaveAddr(I2C1, 0, g_u8SlaveAddr[0], 0);   /* Slave Address : 0x18 */
    /* Slave address : 0x38 */
    I2C_SetSlaveAddr(I2C1, 1, g_u8SlaveAddr[1], 0);   /* Slave Address : 0x38 */
    /* Slave address : 0x58 */
    I2C_SetSlaveAddr(I2C1, 2, g_u8SlaveAddr[2], 0);   /* Slave Address : 0x58 */
    /* Slave address : 0x78 */
    I2C_SetSlaveAddr(I2C1, 3, g_u8SlaveAddr[3], 0);   /* Slave Address : 0x78 */
    /* Set I2C1 4 Slave addresses mask bits*/
    /* Slave address mask Bits: 0x04 */
    I2C_SetSlaveAddrMask(I2C1, 0, 0x04);
    /* Slave address mask Bits: 0x02 */
    I2C_SetSlaveAddrMask(I2C1, 1, 0x02);
    /* Slave address mask Bits: 0x04 */
    I2C_SetSlaveAddrMask(I2C1, 2, 0x04);
    /* Slave address mask Bits: 0x02 */
    I2C_SetSlaveAddrMask(I2C1, 3, 0x02);
    /* Enable I2C1 interrupt */
    I2C_EnableInt(I2C1);
    NVIC_EnableIRQ(I2C1_IRQn);
}

void I2C0_Close(void)
{
    /* Disable I2C0 interrupt and clear corresponding NVIC bit */
    I2C_DisableInt(I2C0);
    NVIC_DisableIRQ(I2C0_IRQn);
    /* Disable I2C0 and close I2C0 clock */
    I2C_Close(I2C0);
    CLK_DisableModuleClock(I2C0_MODULE);
}

void I2C1_Close(void)
{
    /* Disable I2C1 interrupt and clear corresponding NVIC bit */
    I2C_DisableInt(I2C1);
    NVIC_DisableIRQ(I2C1_IRQn);
    /* Disable I2C1 and close I2C1 clock */
    I2C_Close(I2C1);
    CLK_DisableModuleClock(I2C1_MODULE);
}

int32_t SMBusSendByteTest(uint8_t slvaddr)
{
    uint32_t i;
    g_u8DeviceAddr = slvaddr;

    for (i = 0; i < 0x01; i++) {
        /* Init transmission bytes */
        g_au8TxData[0] = (uint8_t)((i & 0xFF00) >> 8);
        g_au8TxData[1] = (uint8_t)(i & 0x00FF);
        g_au8TxData[2] = (uint8_t)(g_au8TxData[1] + 3);
        g_u8DataLen0 = 0;
        g_u8EndFlag = 0;
        g_u8SendPEC = 0;
        /* I2C0 function to write data to slave */
        s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterTx;
        /* I2C0 as master sends START signal */
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STA);

        /* Wait I2C0 transmit finish */
        while (g_u8EndFlag == 0);

        g_u8EndFlag = 0;
    }

    return 0;
}

int32_t SMBusAlertTest(uint8_t slvaddr)
{
    g_u8DeviceAddr = slvaddr;
    /* I2C function to Send Alert Response Address to bus */
    s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterAlert;
    /* I2C0 Send Start condition */
    I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STA);
    /* Init receive data index */
    g_u8DataLen0 = 0;

    /* Waiting for Get Alert Address*/
    while (g_u8AlertAddrAck0 == 0);

    g_u8AlertAddrAck0 = 0;
    return 0;
}

int32_t SMBusDefaultAddressTest(uint8_t slvaddr)
{
    g_u8DeviceAddr = slvaddr;
    /* Set Transmission ARP command */
    g_au8TxData[0] = ARP_COMMAND;
    g_u8DataLen0 = 0;
    g_u8DataLen1 = 0;
    g_u8EndFlag = 0;
    g_u8SendPEC = 0;
    /* I2C0 function to write data to slave */
    s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterDefaultAddrACKM;
    /* I2C0 as master sends START signal */
    I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STA);

    /* Wait I2C0 transmit finish */
    while (g_u8EndFlag == 0);

    g_u8EndFlag = 0;
    printf("\n");
    printf("Master Sends ARP Command(0x01) to Slave (0x%X) Test OK\n", slvaddr);
    return 0;
}


/*Variables for read, write APIs test*/
typedef void (*I2C_FUNC)(uint32_t u32Status);
uint8_t *pu8dstBuf = NULL;
const uint8_t *pu8srcBuf = NULL;
volatile uint16_t g_u16AddressTmp = 0; // for EEPROM address checking
volatile uint32_t g_u32Idx;
volatile uint32_t u32RceLen;
volatile uint8_t  g_u8RegAddrBytes = 0;
volatile enum UI2C_SLAVE_EVENT s_Event;
I2C_FUNC fpI2C_WrRd_Test_Handler = NULL;

I2C_T *i2c_ports[] = {I2C0, I2C1, NULL, };

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

CU_SuiteInfo suites[] = {
#if defined(POWER_DOWN_TEST)
    {"I2C Wakeup API Macro", suite_success_init, suite_success_clean, NULL, NULL, I2C_WakeupAPIMacroTests},
#else
    {"I2C CONST", suite_success_init, suite_success_clean, NULL, NULL, I2C_ConstTests},
    {"I2C API", suite_success_init, suite_success_clean, NULL, NULL, I2C_ApiTests},
    {"I2C MACRO", suite_success_init, suite_success_clean, NULL, NULL, I2C_MacroTests},
    {"SMBus API", suite_success_init, suite_success_clean, NULL, NULL, SMBus_ApiTests},
    {"SMBus MACRO", suite_success_init, suite_success_clean, NULL, NULL, SMBus_MacroTests},
#endif
    CU_SUITE_INFO_NULL
};

void I2C_Reset(void)
{
    SYS_UnlockReg();
    SYS_ResetModule(SYS_I2C0RST);
    SYS_ResetModule(SYS_I2C1RST);
    SYS_ResetModule(SYS_I2C2RST);
    SYS_ResetModule(SYS_I2C3RST);
}

void Test_API_I2C_Open_Close()
{
    uint32_t u32BusClock, i;
    I2C_T *i2c;

    for (i = 0;; i++) {
        i2c = i2c_ports[i];

        if (i2c == NULL) {
            break; // Test Complete
        }

        CU_ASSERT(CLK->I2CCTL & (1UL << i));
        I2C_Reset();

        /* To confirm I2C0, I2C1 are disable for I2C_Open Test*/
        if ((i2c->CTL0 & I2C_CTL0_I2CEN_Msk) != 0x00) {
            while (1);
        }

        /*
            I2C Bus clock = PCLK / (4*(divider+1).
        */
        /* API I2C_Open Test*/
        u32BusClock = 100000; //standard mode
        CU_ASSERT_EQUAL(I2C_Open(i2c, u32BusClock), u32BusClock);
        CU_ASSERT((i2c->CTL0 & I2C_CTL0_I2CEN_Msk) == I2C_CTL0_I2CEN_Msk);
        /* API I2C_Close Test*/
        I2C_Close(i2c);
        CU_ASSERT((i2c->CTL0 & I2C_CTL0_I2CEN_Msk) != I2C_CTL0_I2CEN_Msk);
    }
}


/*
    I2C_Open
	I2C_GetBusClockFreq
	I2C_SetBusClockFreq
	I2C_Close
*/
void Test_API_I2C_BusClockFreq()
{
    uint32_t u32BusClock, i;
    I2C_T *i2c;

    for (i = 0;; i++) {
        i2c = i2c_ports[i];

        if (i2c == NULL) {
            break; // Test Complete
        }

        I2C_Reset();
        u32BusClock = 100000;
        I2C_Open(i2c, u32BusClock);
        CU_ASSERT_EQUAL(I2C_GetBusClockFreq(i2c), I2C_Open(i2c, u32BusClock));
        CU_ASSERT_EQUAL(I2C_SetBusClockFreq(i2c, u32BusClock), I2C_Open(i2c, u32BusClock));
        I2C_Close(i2c);
    }
}

void Test_API_I2C_INT()
{
    uint32_t i;
    I2C_T *i2c;

    for (i = 0;; i++) {
        i2c = i2c_ports[i];

        if (i2c == NULL) {
            break; // Test Complete
        }

        //
        I2C_EnableInt(i2c);
        CU_ASSERT_EQUAL((i2c->CTL0 & I2C_CTL0_INTEN_Msk), I2C_CTL0_INTEN_Msk);
        //
        I2C_DisableInt(i2c);
        CU_ASSERT_NOT_EQUAL((i2c->CTL0 & I2C_CTL0_INTEN_Msk), I2C_CTL0_INTEN_Msk);
    }
}

/*
    I2C_SetSlaveAddr
	I2C_SetSlaveAddrMask
*/
void Test_API_I2C_SetSLVAddr()
{
    uint32_t i, j;
    uint8_t I2C_SLVAddr[4] = {15, 35, 55, 75};
    uint8_t I2C_SLVAddrMsk[4] = {0x01, 0x04, 0x01, 0x04};
    I2C_T *i2c;

    for (i = 0;; i++) {
        i2c = i2c_ports[i];

        if (i2c == NULL) {
            break; // Test Complete
        }

        for (j = 0; j < 4; j++) {
            I2C_SetSlaveAddr(i2c, j, I2C_SLVAddr[j] + i, I2C_GCMODE_DISABLE);
            I2C_SetSlaveAddrMask(i2c, j, I2C_SLVAddrMsk[j]);

            switch (j) {
                case 0:
                    CU_ASSERT_EQUAL(i2c->ADDR0 & I2C_ADDR0_GC_Msk, I2C_GCMODE_DISABLE);
                    CU_ASSERT_EQUAL((i2c->ADDR0) >> I2C_ADDR0_ADDR_Pos, I2C_SLVAddr[j] + i);
                    CU_ASSERT_EQUAL((i2c->ADDRMSK0) >> I2C_ADDRMSK0_ADDRMSK_Pos, I2C_SLVAddrMsk[j]);
                    break;

                case 1:
                    CU_ASSERT_EQUAL(i2c->ADDR1 & I2C_ADDR1_GC_Msk, I2C_GCMODE_DISABLE);
                    CU_ASSERT_EQUAL((i2c->ADDR1) >> I2C_ADDR1_ADDR_Pos, I2C_SLVAddr[j] + i);
                    CU_ASSERT_EQUAL((i2c->ADDRMSK1) >> I2C_ADDRMSK1_ADDRMSK_Pos, I2C_SLVAddrMsk[j]);
                    break;

                case 2:
                    CU_ASSERT_EQUAL(i2c->ADDR2 & I2C_ADDR2_GC_Msk, I2C_GCMODE_DISABLE);
                    CU_ASSERT_EQUAL((i2c->ADDR2) >> I2C_ADDR2_ADDR_Pos, I2C_SLVAddr[j] + i);
                    CU_ASSERT_EQUAL((i2c->ADDRMSK2) >> I2C_ADDRMSK2_ADDRMSK_Pos, I2C_SLVAddrMsk[j]);
                    break;

                case 3:
                    CU_ASSERT_EQUAL(i2c->ADDR3 & I2C_ADDR3_GC_Msk, I2C_GCMODE_DISABLE);
                    CU_ASSERT_EQUAL((i2c->ADDR3) >> I2C_ADDR3_ADDR_Pos, I2C_SLVAddr[j] + i);
                    CU_ASSERT_EQUAL((i2c->ADDRMSK3) >> I2C_ADDRMSK3_ADDRMSK_Pos, I2C_SLVAddrMsk[j]);
                    break;
            }
        }

        for (j = 0; j < 4; j++) {
            I2C_SetSlaveAddr(i2c, j, I2C_SLVAddr[j] + i, I2C_GCMODE_ENABLE);

            switch (j) {
                case 0:
                    CU_ASSERT_EQUAL(i2c->ADDR0 & I2C_ADDR0_GC_Msk, I2C_GCMODE_ENABLE);
                    CU_ASSERT_EQUAL((i2c->ADDR0) >> I2C_ADDR0_ADDR_Pos, I2C_SLVAddr[j] + i);
                    break;

                case 1:
                    CU_ASSERT_EQUAL(i2c->ADDR1 & I2C_ADDR1_GC_Msk, I2C_GCMODE_ENABLE);
                    CU_ASSERT_EQUAL((i2c->ADDR1) >> I2C_ADDR1_ADDR_Pos, I2C_SLVAddr[j] + i);
                    break;

                case 2:
                    CU_ASSERT_EQUAL(i2c->ADDR2 & I2C_ADDR2_GC_Msk, I2C_GCMODE_ENABLE);
                    CU_ASSERT_EQUAL((i2c->ADDR2) >> I2C_ADDR2_ADDR_Pos, I2C_SLVAddr[j] + i);
                    break;

                case 3:
                    CU_ASSERT_EQUAL(i2c->ADDR3 & I2C_ADDR3_GC_Msk, I2C_GCMODE_ENABLE);
                    CU_ASSERT_EQUAL((i2c->ADDR3) >> I2C_ADDR3_ADDR_Pos, I2C_SLVAddr[j] + i);
                    break;
            }
        }
    }
}

void Test_API_I2C_Set_Get_Data()
{
    /*
        The I2Cn_DAT register will clear data after new data be writen in
        So we disable the following asseration.
    */
    //    I2C_SetData(I2C0, 0x5A);
    //    CU_ASSERT_EQUAL(I2C0->DAT, 0x5A);
    //    CU_ASSERT_EQUAL(I2C_GetData(I2C0), 0x5A);
    //    I2C_SetData(I2C1, 0xA5);
    //    CU_ASSERT_EQUAL(I2C1->DAT, 0xA5);
    //    CU_ASSERT_EQUAL(I2C_GetData(I2C1), 0xA5);
}

void Test_API_I2C_Timeout()
{
    uint32_t  i;
    I2C_T *i2c;

    for (i = 0;; i++) {
        i2c = i2c_ports[i];

        if (i2c == NULL) {
            break; // Test Complete
        }

        I2C_Open(i2c, 100000);
        I2C_EnableTimeout(i2c, 1);
        CU_ASSERT_EQUAL(i2c->TOCTL & I2C_TOCTL_TOCDIV4_Msk, I2C_TOCTL_TOCDIV4_Msk);
        CU_ASSERT_EQUAL(i2c->TOCTL & I2C_TOCTL_TOCEN_Msk, I2C_TOCTL_TOCEN_Msk);

        while ((i2c->TOCTL & I2C_TOCTL_TOIF_Msk) == 0);

        I2C_ClearTimeoutFlag(i2c);
        CU_ASSERT_EQUAL(i2c->TOCTL & I2C_TOCTL_TOIF_Msk, 0);
        I2C_DisableTimeout(i2c);
        CU_ASSERT_EQUAL(i2c->TOCTL & I2C_TOCTL_TOIF_Msk, 0);
        SYS_UnlockReg();
        I2C_Close(i2c);
    }
}

void Test_API_I2C_Control_Read_Status()
{
    uint32_t  i;
    I2C_T *i2c_master, *i2c_slave;

    for (i = 0;; i++) {
        i2c_master = i2c_ports[i];

        if (i2c_master == NULL) {
            DBG_MSG("\n - Test Complete\n");
            break; // Test Complete
        }

        //
        I2C_Reset();
        //
        i2c_slave = i2c_ports[i + 1];

        if (i2c_slave == NULL) {
            i2c_slave = i2c_ports[0];
        }

        DBG_MSG("\n - I2C Master & Slave = 0x%08X & 0x%08X\n", (uint32_t)i2c_master, (uint32_t)i2c_slave);
        /* Open I2C0 module and set bus clock */
        I2C_Open(i2c_master, 100000);
        /* Enable I2C */
        i2c_master->CTL0 |= I2C_CTL0_I2CEN_Msk;
        /* Open I2C1 module and set bus clock */
        I2C_Open(i2c_slave, 100000);
        /* Enable I2C */
        i2c_slave->CTL0 |= I2C_CTL0_I2CEN_Msk;
        /* Set I2C1 4 Slave Addresses */
        I2C_SetSlaveAddr(i2c_slave, 0, 0x16, 0);   /* Slave Address : 0x16 */
        I2C_SetSlaveAddr(i2c_slave, 1, 0x36, 0);   /* Slave Address : 0x36 */
        I2C_SetSlaveAddr(i2c_slave, 2, 0x56, 0);   /* Slave Address : 0x56 */
        I2C_SetSlaveAddr(i2c_slave, 3, 0x76, 0);   /* Slave Address : 0x76 */
        /* Set I2C1 4 Slave Addresses Mask */
        I2C_SetSlaveAddrMask(i2c_slave, 0, 0x04);
        I2C_SetSlaveAddrMask(i2c_slave, 1, 0x02);
        I2C_SetSlaveAddrMask(i2c_slave, 2, 0x04);
        I2C_SetSlaveAddrMask(i2c_slave, 3, 0x02);
        /* I2C enter no address SLV mode */
        I2C_SET_CONTROL_REG(i2c_slave, I2C_CTL_SI_AA);
        /* I2C as master sends START signal */
        i2c_master->CTL0 |= I2C_CTL0_STA_Msk;
        CU_ASSERT((i2c_master->CTL0 & I2C_CTL_STA) == I2C_CTL_STA);

        while (I2C_GetIntFlag(i2c_master) == 0) {};

        // while (!(i2c_master->CTL0 & I2C_CTL0_SI_Msk)) {};
//        printf("\n - I2C_GetStatus(i2c_master) = 0x%08X\n", I2C_GetStatus(i2c_master));
        CU_ASSERT((i2c_master->CTL0 & I2C_CTL0_SI_Msk) == I2C_CTL0_SI_Msk);

        CU_ASSERT(I2C_GetStatus(i2c_master) == 0x08);

        I2C_SetData(i2c_master, 0x16 << 1);

        CU_ASSERT(i2c_master->DAT == (0x16 << 1));

        CU_ASSERT(I2C_GetData(i2c_master) == (0x16 << 1));

        I2C_Trigger(i2c_master, 0, 0, 1, 0);

        CU_ASSERT((i2c_master->CTL0 & I2C_CTL0_SI_Msk) == 0);

        while (I2C_GetIntFlag(i2c_master) == 0) {};

        CU_ASSERT((i2c_master->CTL0 & I2C_CTL0_SI_Msk) == I2C_CTL0_SI_Msk);

        CU_ASSERT(I2C_GetStatus(i2c_master) == 0x18);

//        printf("\n - I2C_GetStatus(i2c_master) = 0x%08X\n", I2C_GetStatus(i2c_master));
        //
        SYS_UnlockReg();

        I2C_Close(i2c_master);

        I2C_Close(i2c_slave);

        //
        DBG_MSG("\n - I2C Test PASS\n");
    }
}

/*
    I2C_SlaveTRx can be a normal I2C Slave (g_u8RegCnt == 0),
    or I2C EEPROM with 1 Reg Offset (g_u8RegCnt == 1),
    or I2C EEPROM with 2 Reg Offset (g_u8RegCnt == 2).
*/
volatile uint8_t g_u8SlvDataLen;
volatile uint16_t g_u16RecvAddr;
volatile uint8_t g_u8RegCnt;
volatile uint8_t g_au8SlvRxData[4];
volatile uint32_t slave_buff_addr;
volatile uint8_t g_au8SlvData[256];

void I2C_SlaveTRx(uint32_t u32Status)
{
    I2C_T *i2c = I2C1;
    uint8_t u8Data;

    if (u32Status == 0x60) {                    /* Own SLA+W has been receive; ACK has been return */
        g_u8SlvDataLen = 0;
        g_u16RecvAddr = (uint8_t)I2C_GET_DATA(I2C1);
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
    } else if (u32Status == 0x80)                 /* Previously address with own SLA address
                                                   Data has been received; ACK has been returned*/
    {
        u8Data = (uint8_t) I2C_GET_DATA(I2C1);

        if (g_u8SlvDataLen < g_u8RegCnt) {
            g_au8SlvRxData[g_u8SlvDataLen++] = u8Data;

            if (g_u8RegCnt == 1) {
                slave_buff_addr = g_au8SlvRxData[0];
            } else {
                slave_buff_addr = (g_au8SlvRxData[0] << 8) + g_au8SlvRxData[1];
            }
        } else {
            g_au8SlvData[slave_buff_addr++] = u8Data;
        }

        // Only support 256 Bytes
        slave_buff_addr &= 0x00FF;
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
    } else if (u32Status == 0xA8) {             /* Own SLA+R has been receive; ACK has been return */
        I2C_SET_DATA(I2C1, g_au8SlvData[slave_buff_addr++]);
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
    } else if (u32Status == 0xB8) {             /* Data byte in I2CDAT has been transmitted ACK has been received */
        I2C_SET_DATA(I2C1, g_au8SlvData[slave_buff_addr++]);

        if (slave_buff_addr == 256) {
            slave_buff_addr = 0;
        }

        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
    } else if (u32Status == 0xC0)                 /* Data byte or last data in I2CDAT has been transmitted
                                                   Not ACK has been received */
    {
        g_u8SlvDataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
    } else if (u32Status == 0x88)                 /* Previously addressed with own SLA address; NOT ACK has
                                                   been returned */
    {
        g_u8SlvDataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
    } else if (u32Status == 0xA0)                 /* A STOP or repeated START has been received while still
                                                   addressed as Slave/Receiver*/
    {
        g_u8SlvDataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
    } else {
        if (u32Status == 0xF8) { // Bus Released
            return;
        }

        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

void Test_API_I2C_ReadWriteRelatedTest(void)
{
    uint8_t I2C_SLVAddr[4] = {15, 35, 55, 75};
    uint8_t au8Src[7] = {0x94, 0x56, 0xAA, 0x55, 0xFF, 0xAA, 0x87};
    uint8_t au8Tmp[7] = {0};
    uint32_t rxLen;
    uint32_t i;
    /*Reset all test flow control variables*/
    g_u32Idx = 0; //reset index for destination buffer I2C0 IRQHandler
    u32RceLen = 0;
    /* I2C0 as master, I2C1 as slave*/
    I2C0->CTL0 |= I2C_CTL0_I2CEN_Msk;
    I2C0->CLKDIV = 10;
    //
    I2C1->CTL0 |= I2C_CTL0_I2CEN_Msk;
    /* Enable I2C1 interrupt */
    I2C_EnableInt(I2C1);
    NVIC_EnableIRQ(I2C1_IRQn);
    I2C1->CLKDIV = 10;

    /*Set I2C1 slave address*/
    for (i = 0; i < 4; i++) {
        I2C_SetSlaveAddr(I2C1, i, I2C_SLVAddr[i], 0);
    }

    /*initialize the dst buffer*/
    memset(pu8dstBuf, 0, 256);
    // I2C_WriteByte
    {
        printf("\n\tTest I2C_WriteByte");
        u32RceLen = 1;
        g_u8RegCnt = 0;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        /*Set slave I2C1*/
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
        I2C_WriteByte(I2C0, I2C_SLVAddr[1], 0x88);
        CU_ASSERT_EQUAL(0x88, g_au8SlvData[0]);
    }
    // I2C_WriteMultiBytes
    {
        printf("\n\tTest I2C_WriteMultiBytes");
        u32RceLen = sizeof(au8Src);
        g_u8RegCnt = 0;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        /*Set slave I2C1*/
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
        I2C_WriteMultiBytes(I2C0, I2C_SLVAddr[2], au8Src, u32RceLen);

        for (i = 0 ; i < u32RceLen; i++) {
            CU_ASSERT_EQUAL(au8Src[i], g_au8SlvData[i]);
        }
    }
    //I2C_ReadByte
    {
        printf("\n\tTest I2C_ReadByte");
        u32RceLen = sizeof(au8Src);
        g_u8RegCnt = 0;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
        CU_ASSERT_EQUAL(I2C_ReadByte(I2C0, I2C_SLVAddr[3]), g_au8SlvData[0]);
    }
    //I2C_ReadMultiBytes
    {
        printf("\n\tTest I2C_ReadMultiBytes");
        u32RceLen = sizeof(au8Tmp);
        /*reset the dst buffer*/
        memset(au8Tmp, 0, sizeof(au8Tmp));
        g_u8RegCnt = 0;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
        I2C_ReadMultiBytes(I2C0, I2C_SLVAddr[0], au8Tmp, sizeof(au8Tmp));

        for (i = 0; i < sizeof(au8Tmp); i++) {
            CU_ASSERT_EQUAL(au8Tmp[i], g_au8SlvData[i]);
        }
    }
    //I2C_WriteByteOneReg
    {
        printf("\n\tTest I2C_WriteByteOneReg");
        g_u8RegCnt = 1;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        u32RceLen = 2;
        memset((void *)g_au8SlvData, 0, 256);
        /*Set slave I2C1*/
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
        I2C_WriteByteOneReg(I2C0, I2C_SLVAddr[2], 0x87, 0x94);
        CU_ASSERT_EQUAL(0x87, g_au8SlvRxData[0]);
        CU_ASSERT_EQUAL(0x94, g_au8SlvData[0x87]);
    }
    //I2C_WriteMultiBytesOneReg
    {
        printf("\n\tTest I2C_WriteMultiBytesOneReg");
        g_u8RegCnt = 1;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        u32RceLen = sizeof(au8Src) - 1 + 1;
        memset((void *)g_au8SlvData, 0, 256);
        /*Set slave I2C1*/
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
        I2C_WriteMultiBytesOneReg(I2C0, I2C_SLVAddr[2], 0x56, au8Src, sizeof(au8Src) - 1);
        CU_ASSERT_EQUAL(0x56, g_au8SlvRxData[0]);

        for (i = 0; i < (sizeof(au8Src) - 1); i++) {
            CU_ASSERT_EQUAL(au8Src[i], g_au8SlvData[0x56 + i]);
        }
    }
    //I2C_WriteByteTwoRegs
    {
        printf("\n\tTest I2C_WriteByteTwoRegs");
        g_u8RegCnt = 2;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        u32RceLen = 3;
        memset((void *)g_au8SlvData, 0, 256);
        /*Set slave I2C1*/
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
        I2C_WriteByteTwoRegs(I2C0, I2C_SLVAddr[3], 0x9999, 99);
        CU_ASSERT_EQUAL(((0x9999) >> 8) & 0xff, g_au8SlvRxData[0]);
        CU_ASSERT_EQUAL(0x9999 & 0xff, g_au8SlvRxData[1]);
        CU_ASSERT_EQUAL(99, g_au8SlvData[0x9999 & 0xFF]);
    }
    //I2C_WriteMultiBytesTwoRegs
    {
        printf("\n\tTest I2C_WriteMultiBytesTwoRegs");
        g_u8RegCnt = 2;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        u32RceLen = sizeof(au8Src) - 2 + 2;
        memset((void *)g_au8SlvData, 0, 256);
        /*Set slave I2C1*/
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
        I2C_WriteMultiBytesTwoRegs(I2C0, I2C_SLVAddr[3], 9999, au8Src, sizeof(au8Src) - 2);
        CU_ASSERT_EQUAL(((9999) >> 8) & 0xff, g_au8SlvRxData[0]);
        CU_ASSERT_EQUAL(9999 & 0xff, g_au8SlvRxData[1]);

        for (i = 0; i < (sizeof(au8Src) - 2); i++) {
            CU_ASSERT_EQUAL(au8Src[i], g_au8SlvData[(9999 + i) & 0xFF]);
        }
    }
    //I2C_ReadByteOneReg
    {
        printf("\n\tTest I2C_ReadByteOneReg");
        g_au8SlvData[0x78] = 0x88;
        g_u8RegCnt = 1;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
        CU_ASSERT_EQUAL(I2C_ReadByteOneReg(I2C0, I2C_SLVAddr[0], 0x78), 0x88);
    }
    //I2C_ReadMultiBytesOneReg
    {
        printf("\n\tTest I2C_ReadMultiBytesOneReg");
        u32RceLen = sizeof(au8Src); //fist dst is used as address buffer
        /*reset the dst buffer*/
        memcpy((void *)(g_au8SlvData + 0x87), au8Src, sizeof(au8Src));
        memset((void *)au8Tmp, 0, sizeof(au8Tmp));
        g_u8RegCnt = 1;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
        CU_ASSERT_EQUAL(I2C_ReadMultiBytesOneReg(I2C0, I2C_SLVAddr[0], 0x87, au8Tmp, u32RceLen), u32RceLen);
        CU_ASSERT_EQUAL(0x87, g_au8SlvRxData[0]);

        for (i = 0; i < sizeof(au8Tmp); i++) {
            CU_ASSERT_EQUAL(au8Tmp[i], g_au8SlvData[(0x87 + i) & 0xFF]);
        }
    }
    //I2C_ReadMultiBytesTwoRegs
    {
        printf("\n\tTest I2C_ReadMultiBytesTwoRegs");
        u32RceLen = sizeof(au8Src);
        /*reset the dst buffer*/
        /*reset the dst buffer*/
        memcpy((void *)(g_au8SlvData + (0x5566 & 0xFF)), au8Src, sizeof(au8Src));
        memset((void *)au8Tmp, 0, sizeof(au8Tmp));
        g_u8RegCnt = 2;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
        CU_ASSERT_EQUAL(I2C_ReadMultiBytesTwoRegs(I2C0, I2C_SLVAddr[3], 0x5566, au8Tmp, u32RceLen), u32RceLen);
        CU_ASSERT_EQUAL(0x55, g_au8SlvRxData[0]);
        CU_ASSERT_EQUAL(0x66, g_au8SlvRxData[1]);

        for (i = 0; i < sizeof(au8Tmp); i++) {
            CU_ASSERT_EQUAL(au8Tmp[i], g_au8SlvData[(0x5566 + i) & 0xFF]);
        }
    }
    //I2C_ReadByteTwoRegs
    {
        printf("\n\tTest I2C_ReadByteTwoRegs");
        u32RceLen = sizeof(au8Src); //fist dst is used as address buffer
        /*reset the dst buffer*/
        g_au8SlvData[0x9930 & 0xFF] = 0x77;
        g_u8RegCnt = 2;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
        CU_ASSERT_EQUAL(I2C_ReadByteTwoRegs(I2C0, I2C_SLVAddr[3], 0x9930), 0x77);
        CU_ASSERT_EQUAL(0x99, g_au8SlvRxData[0]);
        CU_ASSERT_EQUAL(0x30, g_au8SlvRxData[1]);
    }
    /*Close all rlated settings*/
    fpI2C_WrRd_Test_Handler = NULL; // back to non-Wr/Rd API test
    I2C_Close(I2C0);
    I2C_Close(I2C1);
}

void Test_API_SMBus_Open_Close()
{
    uint32_t u32BusClock, i;
    I2C_T *i2c;

    for (i = 0;; i++) {
        i2c = i2c_ports[i];

        if (i2c == NULL) {
            break; // Test Complete
        }

        /* Open I2C0 module and set bus clock */
        I2C_Open(i2c, 100000);
        /* Enable I2C */
        i2c->CTL0 |= I2C_CTL0_I2CEN_Msk;
        /* I2C0 Bus Management enable */
        I2C_SMBusOpen(i2c, I2C_SMBH_ENABLE);
        CU_ASSERT_EQUAL((i2c->BUSCTL) & I2C_BUSCTL_BUSEN_Msk, I2C_BUSCTL_BUSEN_Msk);
        CU_ASSERT_EQUAL((i2c->BUSCTL) & I2C_BUSCTL_BMHEN_Msk, I2C_BUSCTL_BMHEN_Msk);
        I2C_SMBusClose(i2c);
        CU_ASSERT_EQUAL((i2c->BUSCTL), 0);
        SYS_UnlockReg();
        I2C_Close(i2c);
    }
}

void Test_API_SMBus_Time_out()
{
    uint32_t u32BusClock, i;
    I2C_T *i2c;

    for (i = 0;; i++) {
        i2c = i2c_ports[i];

        if (i2c == NULL) {
            break; // Test Complete
        }

        /* Open I2C0 module and set bus clock */
        I2C_Open(i2c, 100000);
        /* I2C0 Bus Management enable */
        I2C_SMBusOpen(i2c, I2C_SMBH_ENABLE);
        I2C_SMBusIdleTimeout(i2c, 100, 48000000);
        CU_ASSERT(((i2c->BUSCTL) & I2C_BUSCTL_TIDLE_Msk) == I2C_BUSCTL_TIDLE_Msk);
        CU_ASSERT((i2c->BUSTOUT) == 0xFF);
        I2C_SMBusIdleTimeout(i2c, 5, 48000000);
        CU_ASSERT(((i2c->BUSCTL) & I2C_BUSCTL_TIDLE_Msk) == I2C_BUSCTL_TIDLE_Msk);
        CU_ASSERT((i2c->BUSTOUT) == ((((5 * 48000) / 1000) >> 2) - 1));
        I2C_SMBusTimeout(i2c, 100, 48000000);
        CU_ASSERT(((i2c->TOCTL) & I2C_TOCTL_TOCEN_Msk) == I2C_TOCTL_TOCEN_Msk);
        CU_ASSERT(i2c ->BUSTOUT == ((((100 * 48000) / (16 * 1024 * 4)) - 1) & 0xFF)) //The max value is 255
        I2C_SMBusTimeout(i2c, 50, 48000000);
        CU_ASSERT(((i2c->TOCTL) & I2C_TOCTL_TOCEN_Msk) == 0);
        CU_ASSERT(i2c ->BUSTOUT == (((50 * 48000) / (16 * 1024)) - 1))
        I2C_SMBusClockLoTimeout(i2c, 100, 48000000);
        CU_ASSERT(((i2c->TOCTL) & I2C_TOCTL_TOCEN_Msk) == I2C_TOCTL_TOCEN_Msk);
        CU_ASSERT(i2c ->CLKTOUT == ((((100 * 48000) / (16 * 1024 * 4)) - 1) & 0xFF)) //The max value is 255
        I2C_SMBusClockLoTimeout(i2c, 50, 48000000);
        CU_ASSERT(((i2c->TOCTL) & I2C_TOCTL_TOCEN_Msk) == 0);
        CU_ASSERT(i2c ->CLKTOUT == (((50 * 48000) / (16 * 1024)) - 1))
        I2C_SMBusClose(i2c);
        SYS_UnlockReg();
        I2C_Close(i2c);
    }
}

void Test_API_SMBus_PEC_ByteCount()
{
    /* Reset I2C0 */
    SYS_ResetModule(SYS_I2C0RST);
    /* Open I2C module and set bus clock */
    I2C_Open(I2C0, 100000);
    /* Set I2C0 4 Slave addresses */
    /* Slave address : 0x15 */
    I2C_SetSlaveAddr(I2C0, 0, 0x15, 0);   /* Slave Address : 0x15 */
    /* Slave address : 0x35 */
    I2C_SetSlaveAddr(I2C0, 1, 0x35, 0);   /* Slave Address : 0x35 */
    /* Slave address : 0x55 */
    I2C_SetSlaveAddr(I2C0, 2, 0x55, 0);   /* Slave Address : 0x55 */
    /* Slave address : 0x75 */
    I2C_SetSlaveAddr(I2C0, 3, 0x75, 0);   /* Slave Address : 0x75 */
    /* Reset I2C1 */
    SYS_ResetModule(SYS_I2C1RST);
    /* Open I2C module and set bus clock */
    I2C_Open(I2C1, 100000);
    /* Set I2C1 4 Slave addresses */
    /* Slave address : 0x16 */
    I2C_SetSlaveAddr(I2C1, 0, 0x16, 0);   /* Slave Address : 0x16 */
    /* Slave address : 0x36 */
    I2C_SetSlaveAddr(I2C1, 1, 0x36, 0);   /* Slave Address : 0x36 */
    /* Slave address : 0x56 */
    I2C_SetSlaveAddr(I2C1, 2, 0x56, 0);   /* Slave Address : 0x56 */
    /* Slave address : 0x76 */
    I2C_SetSlaveAddr(I2C1, 3, 0x76, 0);   /* Slave Address : 0x76 */
    /* Set I2C1 4 Slave addresses mask bits*/
    /* Slave address mask Bits: 0x04 */
    I2C_SetSlaveAddrMask(I2C1, 0, 0x04);
    /* Slave address mask Bits: 0x02 */
    I2C_SetSlaveAddrMask(I2C1, 1, 0x02);
    /* Slave address mask Bits: 0x04 */
    I2C_SetSlaveAddrMask(I2C1, 2, 0x04);
    /* Slave address mask Bits: 0x02 */
    I2C_SetSlaveAddrMask(I2C1, 3, 0x02);
    /* I2C1 enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
    /* I2C0 Bus Management enable */
    I2C_SMBusOpen(I2C0, I2C_SMBH_ENABLE);
    CU_ASSERT_EQUAL(I2C0->BUSCTL & (I2C_BUSCTL_BMHEN_Msk | I2C_BUSCTL_BUSEN_Msk), I2C_BUSCTL_BMHEN_Msk | I2C_BUSCTL_BUSEN_Msk);
    /* I2C0 Bus PEC Check and transmit enable */
    I2C_SMBusPECTxEnable(I2C0, I2C_PECTX_ENABLE);
    CU_ASSERT_EQUAL(I2C0->BUSCTL & (I2C_BUSCTL_PECEN_Msk | I2C_BUSCTL_PECTXEN_Msk), I2C_BUSCTL_PECEN_Msk | I2C_BUSCTL_PECTXEN_Msk);
    /* I2C1 Bus Management init */
    I2C_SMBusOpen(I2C1, I2C_SMBD_ENABLE);
    CU_ASSERT_EQUAL(I2C1->BUSCTL & (I2C_BUSCTL_BMDEN_Msk | I2C_BUSCTL_BUSEN_Msk), I2C_BUSCTL_BMDEN_Msk | I2C_BUSCTL_BUSEN_Msk);
    /* I2C1 Bus PEC check enable */
    I2C_SMBusPECTxEnable(I2C1, I2C_PECTX_DISABLE);
    CU_ASSERT_EQUAL(I2C0->BUSCTL & (I2C_BUSCTL_PECEN_Msk), I2C_BUSCTL_PECEN_Msk);
    /* Set I2C0 Payload bytes */
    I2C_SMBusSetPacketByteCount(I2C0, 2);            // I2C0: 1byte address + 1byte data
    CU_ASSERT_EQUAL(I2C0->PKTSIZE, 2);
    /* Set I2C1 Payload bytes */
    I2C_SMBusSetPacketByteCount(I2C1, 2);            // I2C1: 1byte address + 1byte data
    CU_ASSERT_EQUAL(I2C1->PKTSIZE, 2);
    I2C_Trigger(I2C0, 1, 0, 0, 0);

    while (I2C_GetIntFlag(I2C0) == 0);

    if (I2C_GetStatus(I2C0) != 0x08) {
        goto SMBus_Error;
    }

    I2C_SetData(I2C0, 0x16 << 1);
    I2C_Trigger(I2C0, 0, 0, 1, 0);

    while (I2C_GetIntFlag(I2C0) == 0);

    if (I2C_GetStatus(I2C0) != 0x18) {
        goto SMBus_Error;
    }

    while (I2C_GetStatus(I2C1) != 0x60);

    I2C_Trigger(I2C1, 0, 0, 1, 1);
    I2C_SetData(I2C0, 0x5A);
    I2C_Trigger(I2C0, 0, 0, 1, 0);

    while (I2C_GetIntFlag(I2C1) == 0);

    if (I2C_GetStatus(I2C1) != 0x80) {
        goto SMBus_Error;
    }

    if (I2C_GetData(I2C1) != 0x5A) {
        goto SMBus_Error;
    }

    I2C_Trigger(I2C1, 0, 0, 1, 1);

    while (I2C_GetIntFlag(I2C0) == 0);

    if (I2C_GetStatus(I2C0) != 0x28) {
        goto SMBus_Error;
    }

    I2C_Trigger(I2C0, 0, 0, 1, 0);

    while (I2C_GetIntFlag(I2C1) == 0);

    if (I2C_GetStatus(I2C1) != 0x80) {
        goto SMBus_Error;
    }

    CU_ASSERT(I2C_SMBusGetPECValue(I2C1) == I2C_GetData(I2C0));
    CU_ASSERT(I2C_SMBusGetPECValue(I2C1) == I2C_SMBusGetPECValue(I2C0));
    CU_ASSERT((I2C_SMBusGetStatus(I2C0) & I2C_BUSSTS_BCDONE_Msk) == I2C_BUSSTS_BCDONE_Msk);
    I2C_SMBusClearInterruptFlag(I2C0, I2C_BUSSTS_BCDONE_Msk);
    CU_ASSERT((I2C_SMBusGetStatus(I2C0) & I2C_BUSSTS_BCDONE_Msk) == 0);
    I2C_Trigger(I2C1, 0, 0, 1, 1);

    while (I2C_GetIntFlag(I2C0) == 0);

    if (I2C_GetStatus(I2C0) != 0x28) {
        goto SMBus_Error;
    }

    I2C_STOP(I2C0);
    I2C_SMBusClose(I2C0);
    I2C_SMBusClose(I2C1);
    I2C_Close(I2C0);
    I2C_Close(I2C1);
    return;
SMBus_Error:

    while (1);
}

void Test_Macro_I2C_Timeout()
{
    uint32_t i;
    I2C_T *i2c;
    printf("\n\t@%s", __FUNCTION__);

    for (i = 0;; i++) {
        i2c = i2c_ports[i];

        if (i2c == NULL) {
            break; // Test Complete
        }

        CU_ASSERT(CLK->I2CCTL & (1UL << i));
        I2C_Reset();
        I2C_Open(i2c, 100000);
        I2C_EnableTimeout(i2c, 1);
        CU_ASSERT_EQUAL(i2c->TOCTL & I2C_TOCTL_TOCDIV4_Msk, I2C_TOCTL_TOCDIV4_Msk);
        CU_ASSERT_EQUAL(i2c->TOCTL & I2C_TOCTL_TOCEN_Msk, I2C_TOCTL_TOCEN_Msk);
        CU_ASSERT_EQUAL(i2c->TOCTL & I2C_TOCTL_TOIF_Msk, 0);

        while (I2C_GET_TIMEOUT_FLAG(i2c) == 0) {};

        CU_ASSERT_EQUAL(i2c->TOCTL & I2C_TOCTL_TOIF_Msk, I2C_TOCTL_TOIF_Msk);

        I2C_ClearTimeoutFlag(i2c);

        CU_ASSERT_EQUAL(i2c->TOCTL & I2C_TOCTL_TOIF_Msk, 0);

        I2C_DisableTimeout(i2c);

        CU_ASSERT_EQUAL(i2c->TOCTL & I2C_TOCTL_TOCEN_Msk, 0);

        I2C_Close(i2c);
    }
}

void Test_MACRO_I2C_Control_Read_Status()
{
    /* Open I2C0 module and set bus clock */
    I2C_Open(I2C0, 100000);
    /* Set I2C0 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C0, 0, 0x15, 0);   /* Slave Address : 0x15 */
    I2C_SetSlaveAddr(I2C0, 1, 0x35, 0);   /* Slave Address : 0x35 */
    I2C_SetSlaveAddr(I2C0, 2, 0x55, 0);   /* Slave Address : 0x55 */
    I2C_SetSlaveAddr(I2C0, 3, 0x75, 0);   /* Slave Address : 0x75 */
    /* Open I2C1 module and set bus clock */
    I2C_Open(I2C1, 100000);
    /* Set I2C1 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C1, 0, 0x16, 0);   /* Slave Address : 0x16 */
    I2C_SetSlaveAddr(I2C1, 1, 0x36, 0);   /* Slave Address : 0x36 */
    I2C_SetSlaveAddr(I2C1, 2, 0x56, 0);   /* Slave Address : 0x56 */
    I2C_SetSlaveAddr(I2C1, 3, 0x76, 0);   /* Slave Address : 0x76 */
    /* Set I2C1 4 Slave Addresses Mask */
    I2C_SetSlaveAddrMask(I2C1, 0, 0x04);
    I2C_SetSlaveAddrMask(I2C1, 1, 0x02);
    I2C_SetSlaveAddrMask(I2C1, 2, 0x04);
    I2C_SetSlaveAddrMask(I2C1, 3, 0x02);
    /* I2C enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
    //
    I2C_START(I2C0);
    CU_ASSERT((I2C0->CTL0 & I2C_CTL_STA) == I2C_CTL_STA);
    I2C_WAIT_READY(I2C0) {};
    CU_ASSERT((I2C0->CTL0 & I2C_CTL0_SI_Msk) == I2C_CTL0_SI_Msk);

    while (I2C_GET_STATUS(I2C0) != 0x08);

    I2C_SET_DATA(I2C0, 0x16 << 1);
    CU_ASSERT(I2C_GET_DATA(I2C0) == (0x16 << 1));
    I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
    CU_ASSERT((I2C0->CTL0 & I2C_CTL_SI) == 0);
    I2C_WAIT_READY(I2C0);

    while (I2C_GET_STATUS(I2C0) != 0x18);

    I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
    //

    while (I2C_GET_STATUS(I2C1) != 0x60);

    I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI);
    //
    I2C_STOP(I2C0);
    CU_ASSERT((I2C0->CTL0 & I2C_CTL_STO_SI) != I2C_CTL_STO);

    //
    while (I2C_GET_STATUS(I2C1) != 0xA0);

    I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
    //
    I2C_Close(I2C0);
    I2C_Close(I2C1);
}

void Test_Macro_SMBus_Alert_Control()
{
    uint32_t u32SUSCON;
    /* Reset I2C0 */
    SYS_ResetModule(SYS_I2C0RST);
    /* Open I2C module and set bus clock */
    I2C_Open(I2C0, 100000);
    /* Set I2C0 4 Slave addresses */
    /* Slave address : 0x15 */
    I2C_SetSlaveAddr(I2C0, 0, 0x15, 0);   /* Slave Address : 0x15 */
    /* Slave address : 0x35 */
    I2C_SetSlaveAddr(I2C0, 1, 0x35, 0);   /* Slave Address : 0x35 */
    /* Slave address : 0x55 */
    I2C_SetSlaveAddr(I2C0, 2, 0x55, 0);   /* Slave Address : 0x55 */
    /* Slave address : 0x75 */
    I2C_SetSlaveAddr(I2C0, 3, 0x75, 0);   /* Slave Address : 0x75 */
    /* Reset I2C1 */
    SYS_ResetModule(SYS_I2C1RST);
    /* Open I2C module and set bus clock */
    I2C_Open(I2C1, 100000);
    /* Set I2C1 4 Slave addresses */
    /* Slave address : 0x16 */
    I2C_SetSlaveAddr(I2C1, 0, 0x16, 0);   /* Slave Address : 0x16 */
    /* Slave address : 0x36 */
    I2C_SetSlaveAddr(I2C1, 1, 0x36, 0);   /* Slave Address : 0x36 */
    /* Slave address : 0x56 */
    I2C_SetSlaveAddr(I2C1, 2, 0x56, 0);   /* Slave Address : 0x56 */
    /* Slave address : 0x76 */
    I2C_SetSlaveAddr(I2C1, 3, 0x76, 0);   /* Slave Address : 0x76 */
    /* Set I2C1 4 Slave addresses mask bits*/
    /* Slave address mask Bits: 0x04 */
    I2C_SetSlaveAddrMask(I2C1, 0, 0x04);
    /* Slave address mask Bits: 0x02 */
    I2C_SetSlaveAddrMask(I2C1, 1, 0x02);
    /* Slave address mask Bits: 0x04 */
    I2C_SetSlaveAddrMask(I2C1, 2, 0x04);
    /* Slave address mask Bits: 0x02 */
    I2C_SetSlaveAddrMask(I2C1, 3, 0x02);
    /* I2C1 enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
    /* I2C0 Bus Management enable */
    I2C_SMBusOpen(I2C0, I2C_SMBH_ENABLE);
    /* I2C1 Bus Management init */
    I2C_SMBusOpen(I2C1, I2C_SMBD_ENABLE);
    //
    I2C_SMBUS_ENABLE_ALERT(I2C0);
    CU_ASSERT(((I2C0->BUSCTL)& I2C_BUSCTL_ALERTEN_Msk) == I2C_BUSCTL_ALERTEN_Msk);
    //
    I2C_SMBUS_ENABLE_ALERT(I2C1);
    CU_ASSERT(((I2C1->BUSCTL)& I2C_BUSCTL_ALERTEN_Msk) == I2C_BUSCTL_ALERTEN_Msk);
    //
    I2C_SMBUS_DISABLE_ALERT(I2C0);
    CU_ASSERT(((I2C0->BUSCTL)& I2C_BUSCTL_ALERTEN_Msk) == 0);
    //
    I2C_SMBUS_DISABLE_ALERT(I2C1);
    CU_ASSERT(((I2C1->BUSCTL)& I2C_BUSCTL_ALERTEN_Msk) == 0);
    //
    I2C_SMBUS_SET_SUSCON_OUT(I2C0);
    CU_ASSERT((I2C0->BUSCTL & I2C_BUSCTL_SCTLOEN_Msk) == I2C_BUSCTL_SCTLOEN_Msk);
    //
    I2C_SMBUS_SET_SUSCON_IN(I2C1);
    CU_ASSERT((I2C1->BUSCTL & I2C_BUSCTL_SCTLOEN_Msk) == 0);
    //
    I2C_SMBUS_SET_SUSCON_HIGH(I2C0);
    CU_ASSERT((I2C0->BUSCTL & I2C_BUSCTL_SCTLOSTS_Msk) == I2C_BUSCTL_SCTLOSTS_Msk);
    u32SUSCON = I2C_SMBUS_GET_STATUS(I2C0);
    CU_ASSERT((u32SUSCON & I2C_BUSSTS_SCTLDIN_Msk) == I2C_BUSSTS_SCTLDIN_Msk);
    I2C_SMBUS_SET_SUSCON_LOW(I2C0);
    CU_ASSERT((I2C0->BUSCTL & I2C_BUSCTL_SCTLOSTS_Msk) == 0);
    u32SUSCON = I2C_SMBUS_GET_STATUS(I2C0);
    CU_ASSERT((u32SUSCON & I2C_BUSSTS_SCTLDIN_Msk) == 0);
    I2C_SMBusClose(I2C0);
    I2C_Close(I2C0);
    I2C_SMBusClose(I2C1);
    I2C_Close(I2C1);
}

void Test_Macro_SMBus_PEC_ByteCount()
{
    I2C_Reset();
    /* Open I2C module and set bus clock */
    I2C_Open(I2C0, 100000);
    /* Set I2C0 4 Slave addresses */
    /* Slave address : 0x15 */
    I2C_SetSlaveAddr(I2C0, 0, 0x15, 0);   /* Slave Address : 0x15 */
    /* Slave address : 0x35 */
    I2C_SetSlaveAddr(I2C0, 1, 0x35, 0);   /* Slave Address : 0x35 */
    /* Slave address : 0x55 */
    I2C_SetSlaveAddr(I2C0, 2, 0x55, 0);   /* Slave Address : 0x55 */
    /* Slave address : 0x75 */
    I2C_SetSlaveAddr(I2C0, 3, 0x75, 0);   /* Slave Address : 0x75 */
    /* Open I2C module and set bus clock */
    I2C_Open(I2C1, 100000);
    /* Set I2C1 4 Slave addresses */
    /* Slave address : 0x16 */
    I2C_SetSlaveAddr(I2C1, 0, 0x16, 0);   /* Slave Address : 0x16 */
    /* Slave address : 0x36 */
    I2C_SetSlaveAddr(I2C1, 1, 0x36, 0);   /* Slave Address : 0x36 */
    /* Slave address : 0x56 */
    I2C_SetSlaveAddr(I2C1, 2, 0x56, 0);   /* Slave Address : 0x56 */
    /* Slave address : 0x76 */
    I2C_SetSlaveAddr(I2C1, 3, 0x76, 0);   /* Slave Address : 0x76 */
    /* Set I2C1 4 Slave addresses mask bits*/
    /* Slave address mask Bits: 0x04 */
    I2C_SetSlaveAddrMask(I2C1, 0, 0x04);
    /* Slave address mask Bits: 0x02 */
    I2C_SetSlaveAddrMask(I2C1, 1, 0x02);
    /* Slave address mask Bits: 0x04 */
    I2C_SetSlaveAddrMask(I2C1, 2, 0x04);
    /* Slave address mask Bits: 0x02 */
    I2C_SetSlaveAddrMask(I2C1, 3, 0x02);
    /* I2C1 enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
    /* I2C0 Bus Management enable */
    I2C_SMBusOpen(I2C0, I2C_SMBH_ENABLE);
    /* I2C0 Bus PEC Check and transmit enable */
    I2C_SMBusPECTxEnable(I2C0, I2C_PECTX_ENABLE);
    /* I2C1 Bus Management init */
    I2C_SMBusOpen(I2C1, I2C_SMBD_ENABLE);
    /* I2C1 Bus PEC check enable */
    I2C_SMBusPECTxEnable(I2C1, I2C_PECTX_DISABLE);
    /* Set I2C0 Payload bytes */
    I2C_SMBUS_SET_PACKET_BYTE_COUNT(I2C0, 2);            // I2C0: 1byte address + 1byte data
    CU_ASSERT_EQUAL(I2C0->PKTSIZE, 2);
    /* Set I2C1 Payload bytes */
    I2C_SMBUS_SET_PACKET_BYTE_COUNT(I2C1, 2);            // I2C1: 1byte address + 1byte data
    CU_ASSERT_EQUAL(I2C1->PKTSIZE, 2);
    I2C_Trigger(I2C0, 1, 0, 0, 0);

    while (I2C_GetIntFlag(I2C0) == 0);

    if (I2C_GetStatus(I2C0) != 0x08) {
        goto SMBus_Error;
    }

    I2C_SetData(I2C0, 0x16 << 1);
    I2C_Trigger(I2C0, 0, 0, 1, 0);

    while (I2C_GetIntFlag(I2C0) == 0);

    if (I2C_GetStatus(I2C0) != 0x18) {
        goto SMBus_Error;
    }

    while (I2C_GetStatus(I2C1) != 0x60);

    I2C_Trigger(I2C1, 0, 0, 1, 1);
    I2C_SetData(I2C0, 0x5A);
    I2C_Trigger(I2C0, 0, 0, 1, 0);

    while (I2C_GetIntFlag(I2C1) == 0);

    if (I2C_GetStatus(I2C1) != 0x80) {
        goto SMBus_Error;
    }

    if (I2C_GetData(I2C1) != 0x5A) {
        goto SMBus_Error;
    }

    I2C_Trigger(I2C1, 0, 0, 1, 1);

    while (I2C_GetIntFlag(I2C0) == 0);

    if (I2C_GetStatus(I2C0) != 0x28) {
        goto SMBus_Error;
    }

    I2C_Trigger(I2C0, 0, 0, 1, 0);

    while (I2C_GetIntFlag(I2C1) == 0);

    if (I2C_GetStatus(I2C1) != 0x80) {
        goto SMBus_Error;
    }

    CU_ASSERT(I2C_SMBUS_GET_PEC_VALUE(I2C1) == I2C_GetData(I2C0));
    CU_ASSERT(I2C_SMBUS_GET_PEC_VALUE(I2C1) == I2C_SMBusGetPECValue(I2C0));
    I2C_SMBusClearInterruptFlag(I2C0, I2C_BUSSTS_BCDONE_Msk);
    I2C_Trigger(I2C1, 0, 0, 1, 1);

    while (I2C_GetIntFlag(I2C0) == 0);

    if (I2C_GetStatus(I2C0) != 0x28) {
        goto SMBus_Error;
    }

    I2C_STOP(I2C0);
    I2C_SMBusClose(I2C0);
    I2C_SMBusClose(I2C1);
    I2C_Close(I2C0);
    I2C_Close(I2C1);
    return;
SMBus_Error:

    while (1);
}

void Test_Macro_SMBus_ACKMEN_PEC_AT_STA()
{
    /* Reset I2C0 */
    SYS_ResetModule(SYS_I2C0RST);
    /* Open I2C module and set bus clock */
    I2C_Open(I2C0, 100000);
    /* Set I2C0 4 Slave addresses */
    /* Slave address : 0x15 */
    I2C_SetSlaveAddr(I2C0, 0, 0x15, 0);   /* Slave Address : 0x15 */
    /* Slave address : 0x35 */
    I2C_SetSlaveAddr(I2C0, 1, 0x35, 0);   /* Slave Address : 0x35 */
    /* Slave address : 0x55 */
    I2C_SetSlaveAddr(I2C0, 2, 0x55, 0);   /* Slave Address : 0x55 */
    /* Slave address : 0x75 */
    I2C_SetSlaveAddr(I2C0, 3, 0x75, 0);   /* Slave Address : 0x75 */
    /* Reset I2C1 */
    SYS_ResetModule(SYS_I2C1RST);
    /* Open I2C module and set bus clock */
    I2C_Open(I2C1, 100000);
    /* Set I2C1 4 Slave addresses */
    /* Slave address : 0x16 */
    I2C_SetSlaveAddr(I2C1, 0, 0x16, 0);   /* Slave Address : 0x16 */
    /* Slave address : 0x36 */
    I2C_SetSlaveAddr(I2C1, 1, 0x36, 0);   /* Slave Address : 0x36 */
    /* Slave address : 0x56 */
    I2C_SetSlaveAddr(I2C1, 2, 0x56, 0);   /* Slave Address : 0x56 */
    /* Slave address : 0x76 */
    I2C_SetSlaveAddr(I2C1, 3, 0x76, 0);   /* Slave Address : 0x76 */
    /* Set I2C1 4 Slave addresses mask bits*/
    /* Slave address mask Bits: 0x04 */
    I2C_SetSlaveAddrMask(I2C1, 0, 0x04);
    /* Slave address mask Bits: 0x02 */
    I2C_SetSlaveAddrMask(I2C1, 1, 0x02);
    /* Slave address mask Bits: 0x04 */
    I2C_SetSlaveAddrMask(I2C1, 2, 0x04);
    /* Slave address mask Bits: 0x02 */
    I2C_SetSlaveAddrMask(I2C1, 3, 0x02);
    /* I2C1 enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
    /* I2C0 Bus Management enable */
    I2C_SMBusOpen(I2C0, I2C_SMBH_ENABLE);
    /* I2C0 Bus PEC Check and transmit enable */
    I2C_SMBusPECTxEnable(I2C0, I2C_PECTX_ENABLE);
    /* I2C1 Bus Management init */
    I2C_SMBusOpen(I2C1, I2C_SMBD_ENABLE);
    /* I2C1 Bus PEC check enable */
    I2C_SMBusPECTxEnable(I2C1, I2C_PECTX_DISABLE);
    //#define I2C_SMBUS_ACK_MANUAL(i2c)   ((i2c)->BUSCTL |= I2C_BUSCTL_ACKMEN_Msk)
    //#define I2C_SMBUS_ACK_AUTO(i2c)   ((i2c)->BUSCTL &= ~I2C_BUSCTL_ACKMEN_Msk)
    //#define I2C_SMBUS_9THBIT_INT_ENABLE(i2c)   ((i2c)->BUSCTL |= I2C_BUSCTL_ACKM9SI_Msk)
    //#define I2C_SMBUS_9THBIT_INT_DISABLE(i2c)   ((i2c)->BUSCTL &= ~I2C_BUSCTL_ACKM9SI_Msk)
    //#define I2C_SMBUS_RST_PEC_AT_START_ENABLE(i2c)   ((i2c)->BUSCTL |= I2C_BUSCTL_PECCLR_Msk)
    //#define I2C_SMBUS_RST_PEC_AT_START_DISABLE(i2c)   ((i2c)->BUSCTL &= ~I2C_BUSCTL_PECCLR_Msk)
    I2C_SMBUS_ACK_MANUAL(I2C0);
    CU_ASSERT((I2C0->BUSCTL & I2C_BUSCTL_ACKMEN_Msk) == I2C_BUSCTL_ACKMEN_Msk);
    I2C_SMBUS_ACK_MANUAL(I2C1);
    CU_ASSERT((I2C1->BUSCTL & I2C_BUSCTL_ACKMEN_Msk) == I2C_BUSCTL_ACKMEN_Msk);
    I2C_SMBUS_ACK_AUTO(I2C0);
    CU_ASSERT((I2C0->BUSCTL & I2C_BUSCTL_ACKMEN_Msk) == 0);
    I2C_SMBUS_ACK_AUTO(I2C1);
    CU_ASSERT((I2C1->BUSCTL & I2C_BUSCTL_ACKMEN_Msk) == 0);
    I2C_SMBUS_9THBIT_INT_ENABLE(I2C0);
    CU_ASSERT((I2C0->BUSCTL & I2C_BUSCTL_ACKM9SI_Msk) == I2C_BUSCTL_ACKM9SI_Msk);
    I2C_SMBUS_9THBIT_INT_ENABLE(I2C1);
    CU_ASSERT((I2C1->BUSCTL & I2C_BUSCTL_ACKM9SI_Msk) == I2C_BUSCTL_ACKM9SI_Msk);
    I2C_SMBUS_9THBIT_INT_DISABLE(I2C0);
    CU_ASSERT((I2C0->BUSCTL & I2C_BUSCTL_ACKM9SI_Msk) == 0);
    I2C_SMBUS_9THBIT_INT_DISABLE(I2C1);
    CU_ASSERT((I2C1->BUSCTL & I2C_BUSCTL_ACKM9SI_Msk) == 0);
    I2C_SMBUS_RST_PEC_AT_START_ENABLE(I2C0);
    CU_ASSERT((I2C0->BUSCTL & I2C_BUSCTL_PECCLR_Msk) == I2C_BUSCTL_PECCLR_Msk);
    I2C_SMBUS_RST_PEC_AT_START_ENABLE(I2C1);
    CU_ASSERT((I2C1->BUSCTL & I2C_BUSCTL_PECCLR_Msk) == I2C_BUSCTL_PECCLR_Msk);
    I2C_SMBUS_RST_PEC_AT_START_DISABLE(I2C0);
    CU_ASSERT((I2C0->BUSCTL & I2C_BUSCTL_PECCLR_Msk) == 0);
    I2C_SMBUS_RST_PEC_AT_START_DISABLE(I2C1);
    CU_ASSERT((I2C1->BUSCTL & I2C_BUSCTL_PECCLR_Msk) == 0);
    I2C_SMBusClose(I2C0);
    I2C_SMBusClose(I2C1);
    I2C_Close(I2C0);
    I2C_Close(I2C1);
}

void Test_API_MACRO_I2C_Wakeup()
{
    /* Open I2C module and set bus clock */
    I2C_Open(I2C0, 100000);
    /* Set I2C 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C0, 0, 0x15, 0);   /* Slave Address : 0x15 */
    I2C_SetSlaveAddr(I2C0, 1, 0x35, 0);   /* Slave Address : 0x35 */
    I2C_SetSlaveAddr(I2C0, 2, 0x55, 0);   /* Slave Address : 0x55 */
    I2C_SetSlaveAddr(I2C0, 3, 0x75, 0);   /* Slave Address : 0x75 */
    /* Set I2C 4 Slave Addresses Mask */
    I2C_SetSlaveAddrMask(I2C0, 0, 0x01);
    I2C_SetSlaveAddrMask(I2C0, 1, 0x04);
    I2C_SetSlaveAddrMask(I2C0, 2, 0x01);
    I2C_SetSlaveAddrMask(I2C0, 3, 0x04);
    /* Set I2C0 enter Not Address SLAVE mode */
    I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Enable power wake-up interrupt */
    PMC->INTEN |= PMC_INTEN_PDWKIEN_Msk;
    NVIC_EnableIRQ(PMC_IRQn);
    /* Enable I2C wake-up */
    I2C_EnableWakeup(I2C0);
    CU_ASSERT((I2C0->WKCTL & I2C_WKCTL_WKEN_Msk) == I2C_WKCTL_WKEN_Msk);
    printf("\n");
    printf("Enter PD 0x%x 0x%x\n", I2C0->CTL0, I2C0->STATUS0);
    printf("\n");
    printf("CHIP enter power down status.\n");

    /* Waiting for UART printf finish*/
    while (((UART0->FIFOSTS) & UART_FIFOSTS_TXEMPTY_Msk) == 0);

    if (((I2C0->CTL0)&I2C_CTL0_SI_Msk) != 0) {
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
    }

    PMC_SetPowerDownMode(PMC_NPD0, PMC_PLCTL_PLSEL_PL1);
    PMC_PowerDown();
    printf("Wake up!!\n");
    NVIC_DisableIRQ(PMC_IRQn);
    printf(">>>1\n");

    if ((I2C0->WKSTS & I2C_WKSTS_WKIF_Msk) == I2C_WKSTS_WKIF_Msk) {
        printf(">>>2\n");
        CU_ASSERT(I2C_GET_WAKEUP_FLAG(I2C0) == 1);
        I2C_CLEAR_WAKEUP_FLAG(I2C0);
        CU_ASSERT(I2C_GET_WAKEUP_FLAG(I2C0) == 0);
    }

    /* Lock protected registers */
    SYS_LockReg();
    I2C_DisableWakeup(I2C0);
    CU_ASSERT((I2C0->WKCTL & I2C_WKCTL_WKEN_Msk) == 0);
    I2C_Close(I2C0);
    printf(">>>3\n");
}

void Test_API_MACRO_I2C_PDMA()
{
    uint32_t i;
    I2C_T *i2c;
    printf("\n\t@%s", __FUNCTION__);

    for (i = 0;; i++) {
        i2c = i2c_ports[i];

        if (i2c == NULL) {
            break; // Test Complete
        }

        I2C_Reset();
        /* Enable/disable TX PDMA */
        CU_ASSERT((i2c->CTL1 & I2C_CTL1_TXPDMAEN_Msk) == 0);
        I2C_ENABLE_TX_PDMA(i2c);
        CU_ASSERT((i2c->CTL1 & I2C_CTL1_TXPDMAEN_Msk) == I2C_CTL1_TXPDMAEN_Msk);
        I2C_DISABLE_TX_PDMA(i2c);
        CU_ASSERT((i2c->CTL1 & I2C_CTL1_TXPDMAEN_Msk) == 0);
        /* Enable/disable RX PDMA */
        CU_ASSERT((i2c->CTL1 & I2C_CTL1_RXPDMAEN_Msk) == 0);
        I2C_ENABLE_RX_PDMA(i2c);
        CU_ASSERT((i2c->CTL1 & I2C_CTL1_RXPDMAEN_Msk) == I2C_CTL1_RXPDMAEN_Msk);
        I2C_DISABLE_RX_PDMA(i2c);
        CU_ASSERT((i2c->CTL1 & I2C_CTL1_RXPDMAEN_Msk) == 0);
        /* Enable/disable PDMA stretch */
        CU_ASSERT((i2c->CTL1 & I2C_CTL1_PDMASTR_Msk) == 0);
        I2C_ENABLE_PDMA_STRETCH(i2c);
        CU_ASSERT((i2c->CTL1 & I2C_CTL1_PDMASTR_Msk) == I2C_CTL1_PDMASTR_Msk);
        I2C_DISABLE_PDMA_STRETCH(i2c);
        CU_ASSERT((i2c->CTL1 & I2C_CTL1_PDMASTR_Msk) == 0);
    }
}

void Test_CONST_I2C_CTL(void)
{
    CU_ASSERT(I2C_CTL_STA_SI == (I2C_CTL0_STA_Msk | I2C_CTL0_SI_Msk));
    CU_ASSERT(I2C_CTL_STA_SI_AA == (I2C_CTL0_STA_Msk | I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk));
    CU_ASSERT(I2C_CTL_STO_SI == (I2C_CTL0_STO_Msk | I2C_CTL0_SI_Msk));
    CU_ASSERT(I2C_CTL_STO_SI_AA == (I2C_CTL0_STO_Msk | I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk));
    CU_ASSERT(I2C_CTL_SI == I2C_CTL0_SI_Msk);
    CU_ASSERT(I2C_CTL_SI_AA == (I2C_CTL0_SI_Msk | I2C_CTL0_AA_Msk));
    CU_ASSERT(I2C_CTL_STA == I2C_CTL0_STA_Msk);
    CU_ASSERT(I2C_CTL_STO == I2C_CTL0_STO_Msk);
    CU_ASSERT(I2C_CTL_AA == I2C_CTL0_AA_Msk);
}

void Test_CONST_I2C_GCMode(void)
{
    CU_ASSERT(I2C_GCMODE_ENABLE == 1);
    CU_ASSERT(I2C_GCMODE_DISABLE == 0);
}

void Test_CONST_I2C_SMBus(void)
{
    CU_ASSERT(I2C_SMBH_ENABLE == 1);
    CU_ASSERT(I2C_SMBD_ENABLE == 0);
    CU_ASSERT(I2C_PECTX_ENABLE == 1);
    CU_ASSERT(I2C_PECTX_DISABLE == 0);
}

CU_TestInfo I2C_ConstTests[] = {
    {" 1: CONST I2C_CTL.", Test_CONST_I2C_CTL},
    {" 2: CONST I2C_GCMode.", Test_CONST_I2C_GCMode},
    {" 3: CONST I2C_SMBus.", Test_CONST_I2C_SMBus},
    CU_TEST_INFO_NULL
};

CU_TestInfo I2C_ApiTests[] = {
    {" 1: API I2C_Open_Close.", Test_API_I2C_Open_Close},
    {" 2: API I2C_Get_Set_Bus_Clock", Test_API_I2C_BusClockFreq},
    {" 3: API I2C_INT_En_Dis_ABLE", Test_API_I2C_INT},
    {" 4: API I2C_Set_SLV_Address", Test_API_I2C_SetSLVAddr},
    {" 5: API I2C_Set_Get_Data", Test_API_I2C_Set_Get_Data},
    {" 6: API I2C_Time-out_En_Dis_able", Test_API_I2C_Timeout},
    {" 7: API I2C_Control_Read_STATUS", Test_API_I2C_Control_Read_Status},
    {" 8: API I2C_ReadWriteRelatedTest", Test_API_I2C_ReadWriteRelatedTest},
    CU_TEST_INFO_NULL
};


void SMBusTest(void);

CU_TestInfo SMBus_ApiTests[] = {
    {" 1: API SMBus_Open_Close", Test_API_SMBus_Open_Close},
    {" 2: API SMBus_Timeout", Test_API_SMBus_Time_out},
    {" 3: API SMBus_PEC_ByteCount", Test_API_SMBus_PEC_ByteCount},
    CU_TEST_INFO_NULL
};

CU_TestInfo I2C_MacroTests[] = {
    {" 1: MACRO I2C_Get_TimeOutFlag", Test_Macro_I2C_Timeout},
    {" 2: MACRO I2C_Control_Read_STATUS", Test_MACRO_I2C_Control_Read_Status},
    {" 3: MACRO I2C_PDMA", Test_API_MACRO_I2C_PDMA},
    CU_TEST_INFO_NULL
};

CU_TestInfo SMBus_MacroTests[] = {
    {" 1: Macro SMBus_Alert_Control", Test_Macro_SMBus_Alert_Control},
    {" 2: Macro SMBus_PEC_ByteCount", Test_Macro_SMBus_PEC_ByteCount},
    {" 3: Macro SMBus_ACKMEN_PEC_AT_START", Test_Macro_SMBus_ACKMEN_PEC_AT_STA},
    CU_TEST_INFO_NULL
};

CU_TestInfo I2C_WakeupAPIMacroTests[] = {
    {" 1: API_MACRO I2C_Wakeup", Test_API_MACRO_I2C_Wakeup}, //NuBridge may pull the bus, so it should test independently
    CU_TEST_INFO_NULL
};

