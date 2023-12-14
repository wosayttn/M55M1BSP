
/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "lpi2c_cunit.h"


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


/*Variables for read, write APIs test*/
typedef void (*LPI2C_FUNC)(uint32_t u32Status);
uint8_t *pu8dstBuf = NULL;
const uint8_t *pu8srcBuf = NULL;
volatile uint16_t g_u16AddressTmp = 0; // for EEPROM address checking
volatile uint32_t g_u32Idx;
volatile uint32_t u32RceLen;
volatile uint8_t  g_u8RegAddrBytes = 0;
LPI2C_FUNC fpI2C_WrRd_Test_Handler = NULL;


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

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
static volatile I2C_FUNC s_LPI2C0HandlerFn = NULL;


/*Variables for read, write APIs test*/
extern I2C_FUNC fpI2C_WrRd_Test_Handler;

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_IRQHandler(void)
{
    uint32_t u32Status;
    uint8_t u8data;
    u32Status = I2C_GET_STATUS(I2C0);

    if (fpI2C_WrRd_Test_Handler) { //if not point to null=> test Wr, Rd APIs.
        /*For test APIs:
            LPI2C_WriteByte,
            LPI2C_WriteMultiBytes
            LPI2C_WriteByteOneReg
            LPI2C_WriteMultiBytesOneReg
            LPI2C_WriteByteTwoRegs
            LPI2C_WriteMultiBytesTwoRegs
            LPI2C_ReadByte
            LPI2C_ReadMultiBytes
            LPI2C_ReadByteOneReg
            LPI2C_ReadMultiBytesOneReg
            LPI2C_ReadByteTwoRegs
            LPI2C_ReadMultiBytesTwoRegs
        */
        fpI2C_WrRd_Test_Handler(u32Status);
        return;
    }
}

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
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void LPI2C0_IRQHandler(void)
{
    uint32_t u32Status;
    u32Status = I2C_GET_STATUS(I2C0);
    printf("I2C0_STA: %08X\n", u32Status);

    if (I2C_GET_TIMEOUT_FLAG(I2C0)) {
        /* Clear I2C0 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C0);
    } else {
        if (s_LPI2C0HandlerFn != NULL) {
            s_LPI2C0HandlerFn(u32Status);
        }
    }

    /* make sure that interrupt flag has been cleared. */
    __DSB();
    __ISB();
}

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
    {"LPI2C Wakeup API Macro", suite_success_init, suite_success_clean, NULL, NULL, LPI2C_WakeupAPIMacroTests},
#else
    {"LPI2C CONST", suite_success_init, suite_success_clean, NULL, NULL, LPI2C_ConstTests},
    {"LPI2C API", suite_success_init, suite_success_clean, NULL, NULL, LPI2C_ApiTests},
    {"LPI2C MACRO", suite_success_init, suite_success_clean, NULL, NULL, LPI2C_MacroTests},
#endif
    CU_SUITE_INFO_NULL
};

void LPI2C_Reset(void)
{
    SYS_UnlockReg();
    SYS_ResetModule(SYS_LPI2C0RST);
    SYS_ResetModule(SYS_I2C0RST);
}

/*
    LPI2C_Open
    LPI2C_Close
*/
void Test_API_LPI2C_Open_Close()
{
    uint32_t u32BusClock;
    LPI2C_T *i2c = LPI2C0;
    CU_ASSERT((CLK->LPI2CCTL & CLK_LPI2CCTL_LPI2C0CKEN_Msk) == CLK_LPI2CCTL_LPI2C0CKEN_Msk);
    LPI2C_Reset();

    if ((i2c->CTL0 & LPI2C_CTL0_LPI2CEN_Msk) != 0x00) {
        while (1);
    }

    /* API LPI2C_Open Test*/
    u32BusClock = 100000; //standard mode
    CU_ASSERT_EQUAL(LPI2C_Open(i2c, u32BusClock), LPI2C_GetBusClockFreq(i2c));
    CU_ASSERT((i2c->CTL0 & LPI2C_CTL0_LPI2CEN_Msk) == LPI2C_CTL0_LPI2CEN_Msk);
    u32BusClock = 200000;
    CU_ASSERT_EQUAL(LPI2C_Open(i2c, u32BusClock), LPI2C_GetBusClockFreq(i2c));
    CU_ASSERT((i2c->CTL0 & LPI2C_CTL0_LPI2CEN_Msk) == LPI2C_CTL0_LPI2CEN_Msk);
//    u32BusClock = 400000;  //Fast mode, PCLK=12MHz cannot be Divisible for 400KHz case
//    CU_ASSERT_EQUAL(LPI2C_Open(i2c, u32BusClock), u32BusClock);
//    CU_ASSERT((i2c->CTL0 & LPI2C_CTL0_LPI2CEN_Msk) == LPI2C_CTL0_LPI2CEN_Msk);
    u32BusClock = 1000000; //Fast plus mode
    CU_ASSERT_EQUAL(LPI2C_Open(i2c, u32BusClock), LPI2C_GetBusClockFreq(i2c));
    CU_ASSERT((i2c->CTL0 & LPI2C_CTL0_LPI2CEN_Msk) == LPI2C_CTL0_LPI2CEN_Msk);
    /* API LPI2C_Close Test*/
    LPI2C_Close(i2c);
    CU_ASSERT((i2c->CTL0 & LPI2C_CTL0_LPI2CEN_Msk) != LPI2C_CTL0_LPI2CEN_Msk);
}

/*
    LPI2C_Open
	LPI2C_GetBusClockFreq
	LPI2C_SetBusClockFreq
	LPI2C_Close
*/
void Test_API_LPI2C_BusClockFreq()
{
    uint32_t u32BusClock;
    LPI2C_T *i2c = LPI2C0;
    u32BusClock = 100000;
    LPI2C_Open(i2c, u32BusClock);
    CU_ASSERT_EQUAL(LPI2C_GetBusClockFreq(i2c), LPI2C_Open(i2c, u32BusClock));
    u32BusClock = LPI2C_GetBusClockFreq(i2c);
    CU_ASSERT_EQUAL(LPI2C_SetBusClockFreq(i2c, u32BusClock), LPI2C_Open(i2c, u32BusClock));
    LPI2C_Close(i2c);
}

/*
    LPI2C_EnableInt
	LPI2C_DisableInt
*/
void Test_API_LPI2C_INT()
{
    LPI2C_T *i2c = LPI2C0;
    LPI2C_EnableInt(i2c);
    CU_ASSERT_EQUAL((i2c->CTL0 & LPI2C_CTL0_INTEN_Msk), LPI2C_CTL0_INTEN_Msk);
    //
    LPI2C_DisableInt(i2c);
    CU_ASSERT_NOT_EQUAL((i2c->CTL0 & LPI2C_CTL0_INTEN_Msk), LPI2C_CTL0_INTEN_Msk);
}

/*
    LPI2C_SetSlaveAddr
    LPI2C_SetSlaveAddrMask
*/
void Test_API_LPI2C_SetSLVAddr()
{
    uint32_t j;
    uint8_t LPI2C_SLVAddr[4] = {15, 35, 55, 75};
    uint8_t LPI2C_SLVAddrMsk[4] = {0x01, 0x04, 0x01, 0x04};
    LPI2C_T *i2c = LPI2C0;

    for (j = 0; j < 4; j++) {
        LPI2C_SetSlaveAddr(i2c, j, LPI2C_SLVAddr[j], LPI2C_GCMODE_DISABLE);
        LPI2C_SetSlaveAddrMask(LPI2C0, j, LPI2C_SLVAddrMsk[j]);

        switch (j) {
            case 0:
                CU_ASSERT_EQUAL(i2c->ADDR0 & LPI2C_ADDR0_GC_Msk, LPI2C_GCMODE_DISABLE);
                CU_ASSERT_EQUAL((i2c->ADDR0) >> LPI2C_ADDR0_ADDR_Pos, LPI2C_SLVAddr[j]);
                CU_ASSERT_EQUAL((i2c->ADDRMSK0) >> LPI2C_ADDRMSK0_ADDRMSK_Pos, LPI2C_SLVAddrMsk[j]);
                break;

            case 1:
                CU_ASSERT_EQUAL(i2c->ADDR1 & LPI2C_ADDR1_GC_Msk, LPI2C_GCMODE_DISABLE);
                CU_ASSERT_EQUAL((i2c->ADDR1) >> LPI2C_ADDR1_ADDR_Pos, LPI2C_SLVAddr[j]);
                CU_ASSERT_EQUAL((i2c->ADDRMSK1) >> LPI2C_ADDRMSK1_ADDRMSK_Pos, LPI2C_SLVAddrMsk[j]);
                break;

            case 2:
                CU_ASSERT_EQUAL(i2c->ADDR2 & LPI2C_ADDR2_GC_Msk, LPI2C_GCMODE_DISABLE);
                CU_ASSERT_EQUAL((i2c->ADDR2) >> LPI2C_ADDR2_ADDR_Pos, LPI2C_SLVAddr[j]);
                CU_ASSERT_EQUAL((i2c->ADDRMSK2) >> LPI2C_ADDRMSK2_ADDRMSK_Pos, LPI2C_SLVAddrMsk[j]);
                break;

            case 3:
                CU_ASSERT_EQUAL(i2c->ADDR3 & LPI2C_ADDR3_GC_Msk, LPI2C_GCMODE_DISABLE);
                CU_ASSERT_EQUAL((i2c->ADDR3) >> LPI2C_ADDR3_ADDR_Pos, LPI2C_SLVAddr[j]);
                CU_ASSERT_EQUAL((i2c->ADDRMSK3) >> LPI2C_ADDRMSK3_ADDRMSK_Pos, LPI2C_SLVAddrMsk[j]);
                break;
        }
    }

    //
    for (j = 0; j < 4; j++) {
        LPI2C_SetSlaveAddr(i2c, j, LPI2C_SLVAddr[j], LPI2C_GCMODE_ENABLE);

        switch (j) {
            case 0:
                CU_ASSERT_EQUAL(i2c->ADDR0 & LPI2C_ADDR0_GC_Msk, LPI2C_GCMODE_ENABLE);
                break;

            case 1:
                CU_ASSERT_EQUAL(i2c->ADDR1 & LPI2C_ADDR1_GC_Msk, LPI2C_GCMODE_ENABLE);
                break;

            case 2:
                CU_ASSERT_EQUAL(i2c->ADDR2 & LPI2C_ADDR2_GC_Msk, LPI2C_GCMODE_ENABLE);
                break;

            case 3:
                CU_ASSERT_EQUAL(i2c->ADDR3 & LPI2C_ADDR3_GC_Msk, LPI2C_GCMODE_ENABLE);
        }
    }
}

void Test_API_LPI2C_Set_Get_Data()
{
    /*
        The LPI2Cn_DAT register will clear data after new data be writen in
        So we disable the following asseration.
    */
    //    LPI2C_SetData(LPI2C0, 0x5A);
    //    CU_ASSERT_EQUAL(LPI2C0->DAT, 0x5A);
    //    CU_ASSERT_EQUAL(LPI2C_GetData(LPI2C0), 0x5A);
    //    LPI2C_SetData(I2C0, 0xA5);
    //    CU_ASSERT_EQUAL(I2C0->DAT, 0xA5);
    //    CU_ASSERT_EQUAL(LPI2C_GetData(I2C0), 0xA5);
}

/*
    LPI2C_Open
    LPI2C_EnableTimeout
    LPI2C_ClearTimeoutFlag
    LPI2C_DisableTimeout
    LPI2C_DisableInt
*/
void Test_API_LPI2C_Timeout()
{
    uint32_t  i;
    LPI2C_T *i2c = LPI2C0;
    LPI2C_Open(i2c, 100000);
    LPI2C_EnableTimeout(i2c, 1);
    CU_ASSERT_EQUAL(i2c->TOCTL & LPI2C_TOCTL_TOCDIV4_Msk, LPI2C_TOCTL_TOCDIV4_Msk);
    CU_ASSERT_EQUAL(i2c->TOCTL & LPI2C_TOCTL_TOCEN_Msk, LPI2C_TOCTL_TOCEN_Msk);

    while ((i2c->TOCTL & LPI2C_TOCTL_TOIF_Msk) == 0);

    LPI2C_ClearTimeoutFlag(i2c);
    CU_ASSERT_EQUAL(i2c->TOCTL & LPI2C_TOCTL_TOIF_Msk, 0);
    LPI2C_DisableTimeout(i2c);
    CU_ASSERT_EQUAL(i2c->TOCTL & LPI2C_TOCTL_TOIF_Msk, 0);
    SYS_UnlockReg();
    LPI2C_Close(i2c);
}

void Test_API_LPI2C_Control_Read_Status()
{
    uint32_t  i;
    LPI2C_T *i2c_master = LPI2C0;
    I2C_T *i2c_slave = I2C0;
    //
    LPI2C_Reset();
    //
    /* Open LPI2C0 module and set bus clock */
    LPI2C_Open(i2c_master, 100000);
    /* Enable LPI2C */
    i2c_master->CTL0 |= LPI2C_CTL0_LPI2CEN_Msk;
    /* Open I2C0 module and set bus clock */
    I2C_Open(i2c_slave, 100000);
    /* Enable LPI2C */
    i2c_slave->CTL0 |= I2C_CTL0_I2CEN_Msk;
    /* Set i2c_slave 4 Slave Addresses */
    I2C_SetSlaveAddr(i2c_slave, 0, 0x16, 0);   /* Slave Address : 0x16 */
    I2C_SetSlaveAddr(i2c_slave, 1, 0x36, 0);   /* Slave Address : 0x36 */
    I2C_SetSlaveAddr(i2c_slave, 2, 0x56, 0);   /* Slave Address : 0x56 */
    I2C_SetSlaveAddr(i2c_slave, 3, 0x76, 0);   /* Slave Address : 0x76 */
    /* Set i2c_slave 4 Slave Addresses Mask */
    I2C_SetSlaveAddrMask(i2c_slave, 0, 0x04);
    I2C_SetSlaveAddrMask(i2c_slave, 1, 0x02);
    I2C_SetSlaveAddrMask(i2c_slave, 2, 0x04);
    I2C_SetSlaveAddrMask(i2c_slave, 3, 0x02);
    /* LPI2C enter no address SLV mode */
    I2C_SET_CONTROL_REG(i2c_slave, I2C_CTL_SI_AA);
    /* LPI2C as master sends START signal */
    i2c_master->CTL0 |= LPI2C_CTL0_STA_Msk;
    // CU_ASSERT((i2c_master->CTL0 & LPI2C_CTL_STA) == LPI2C_CTL_STA);

    // while (LPI2C_GetIntFlag(i2c_master) == 0) {};
    while (!(i2c_master->CTL0 & LPI2C_CTL0_SI_Msk)) {};

//        printf("\n - LPI2C_GetStatus(i2c_master) = 0x%08X\n", LPI2C_GetStatus(i2c_master));
    CU_ASSERT((i2c_master->CTL0 & LPI2C_CTL0_SI_Msk) == LPI2C_CTL0_SI_Msk);

    CU_ASSERT(LPI2C_GetStatus(i2c_master) == 0x08);

    LPI2C_SetData(i2c_master, 0x16 << 1);

    CU_ASSERT(i2c_master->DAT == (0x16 << 1));

    CU_ASSERT(LPI2C_GetData(i2c_master) == (0x16 << 1));

    LPI2C_Trigger(i2c_master, 0, 0, 1, 0);

    CU_ASSERT((i2c_master->CTL0 & LPI2C_CTL0_SI_Msk) == 0);

    while (LPI2C_GetIntFlag(i2c_master) == 0) {};

    CU_ASSERT((i2c_master->CTL0 & LPI2C_CTL0_SI_Msk) == LPI2C_CTL0_SI_Msk);

    CU_ASSERT(LPI2C_GetStatus(i2c_master) == 0x18);

//        printf("\n - LPI2C_GetStatus(i2c_master) = 0x%08X\n", LPI2C_GetStatus(i2c_master));
    //
    SYS_UnlockReg();

    LPI2C_Close(i2c_master);

    I2C_Close(i2c_slave);

    //
    DBG_MSG("\n - LPI2C Test PASS\n");
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
    uint8_t u8Data;

    if (u32Status == 0x60) {                    /* Own SLA+W has been receive; ACK has been return */
        g_u8SlvDataLen = 0;
        g_u16RecvAddr = (uint8_t)I2C_GET_DATA(I2C0);
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    } else if (u32Status == 0x80)                 /* Previously address with own SLA address
                                                   Data has been received; ACK has been returned*/
    {
        u8Data = (uint8_t) I2C_GET_DATA(I2C0);

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
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    } else if (u32Status == 0xA8) {             /* Own SLA+R has been receive; ACK has been return */
        I2C_SET_DATA(I2C0, g_au8SlvData[slave_buff_addr++]);
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    } else if (u32Status == 0xB8) {             /* Data byte in I2CDAT has been transmitted ACK has been received */
        I2C_SET_DATA(I2C0, g_au8SlvData[slave_buff_addr++]);

        if (slave_buff_addr == 256) {
            slave_buff_addr = 0;
        }

        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    } else if (u32Status == 0xC0)                 /* Data byte or last data in I2CDAT has been transmitted
                                                   Not ACK has been received */
    {
        g_u8SlvDataLen = 0;
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    } else if (u32Status == 0x88)                 /* Previously addressed with own SLA address; NOT ACK has
                                                   been returned */
    {
        g_u8SlvDataLen = 0;
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    } else if (u32Status == 0xA0)                 /* A STOP or repeated START has been received while still
                                                   addressed as Slave/Receiver*/
    {
        g_u8SlvDataLen = 0;
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    } else {
        if (u32Status == 0xF8) { // Bus Released
            return;
        }

        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

void Test_API_LPI2C_ReadWriteRelatedTest(void)
{
    uint8_t I2C_SLVAddr[4] = {15, 35, 55, 75};
    uint8_t au8Src[7] = {0x94, 0x56, 0xAA, 0x55, 0xFF, 0xAA, 0x87};
    uint8_t au8Tmp[7] = {0};
    uint32_t rxLen;
    uint32_t i;
    /*Reset all test flow control variables*/
    g_u32Idx = 0; //reset index for destination buffer I2C0 IRQHandler
    u32RceLen = 0;
    /*LPI2C0 as master, I2C0 as slave*/
    LPI2C_Open(LPI2C0, 400000);
    //
    I2C_Open(I2C0, 400000);
    /* Enable I2C0 interrupt */
    I2C_EnableInt(I2C0);
    NVIC_EnableIRQ(I2C0_IRQn);

    /*Set I2C0 slave address*/
    for (i = 0; i < 4; i++) {
        I2C_SetSlaveAddr(I2C0, i, I2C_SLVAddr[i], 0);
    }

    /*initialize the dst buffer*/
    memset(pu8dstBuf, 0, 256);
    // LPI2C_WriteByte
    {
        printf("\n\tTest LPI2C_WriteByte");
        u32RceLen = 1;
        g_u8RegCnt = 0;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        /*Set slave I2C0*/
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
        LPI2C_WriteByte(LPI2C0, I2C_SLVAddr[1], 0x88);
        CU_ASSERT_EQUAL(0x88, g_au8SlvData[0]);
    }
    // LPI2C_WriteMultiBytes
    {
        printf("\n\tTest LPI2C_WriteMultiBytes");
        u32RceLen = sizeof(au8Src);
        g_u8RegCnt = 0;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        /*Set slave I2C0*/
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
        LPI2C_WriteMultiBytes(LPI2C0, I2C_SLVAddr[2], au8Src, u32RceLen);

        for (i = 0 ; i < u32RceLen; i++) {
            CU_ASSERT_EQUAL(au8Src[i], g_au8SlvData[i]);
        }
    }
    //LPI2C_ReadByte
    {
        printf("\n\tTest LPI2C_ReadByte");
        u32RceLen = sizeof(au8Src);
        g_u8RegCnt = 0;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
        CU_ASSERT_EQUAL(LPI2C_ReadByte(LPI2C0, I2C_SLVAddr[3]), g_au8SlvData[0]);
    }
    //LPI2C_ReadMultiBytes
    {
        printf("\n\tTest LPI2C_ReadMultiBytes");
        u32RceLen = sizeof(au8Tmp);
        /*reset the dst buffer*/
        memset(au8Tmp, 0, sizeof(au8Tmp));
        g_u8RegCnt = 0;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
        LPI2C_ReadMultiBytes(LPI2C0, I2C_SLVAddr[0], au8Tmp, sizeof(au8Tmp));

        for (i = 0; i < sizeof(au8Tmp); i++) {
            CU_ASSERT_EQUAL(au8Tmp[i], g_au8SlvData[i]);
        }
    }
    //LPI2C_WriteByteOneReg
    {
        printf("\n\tTest LPI2C_WriteByteOneReg");
        g_u8RegCnt = 1;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        u32RceLen = 2;
        memset((void *)g_au8SlvData, 0, 256);
        /*Set slave I2C0*/
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
        LPI2C_WriteByteOneReg(LPI2C0, I2C_SLVAddr[2], 0x87, 0x94);
        CU_ASSERT_EQUAL(0x87, g_au8SlvRxData[0]);
        CU_ASSERT_EQUAL(0x94, g_au8SlvData[0x87]);
    }
    //LPI2C_WriteMultiBytesOneReg
    {
        printf("\n\tTest LPI2C_WriteMultiBytesOneReg");
        g_u8RegCnt = 1;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        u32RceLen = sizeof(au8Src) - 1 + 1;
        memset((void *)g_au8SlvData, 0, 256);
        /*Set slave I2C0*/
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
        LPI2C_WriteMultiBytesOneReg(LPI2C0, I2C_SLVAddr[2], 0x56, au8Src, sizeof(au8Src) - 1);
        CU_ASSERT_EQUAL(0x56, g_au8SlvRxData[0]);

        for (i = 0; i < (sizeof(au8Src) - 1); i++) {
            CU_ASSERT_EQUAL(au8Src[i], g_au8SlvData[0x56 + i]);
        }
    }
    //LPI2C_WriteByteTwoRegs
    {
        printf("\n\tTest LPI2C_WriteByteTwoRegs");
        g_u8RegCnt = 2;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        u32RceLen = 3;
        memset((void *)g_au8SlvData, 0, 256);
        /*Set slave I2C0*/
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
        LPI2C_WriteByteTwoRegs(LPI2C0, I2C_SLVAddr[3], 0x9999, 99);
        CU_ASSERT_EQUAL(((0x9999) >> 8) & 0xff, g_au8SlvRxData[0]);
        CU_ASSERT_EQUAL(0x9999 & 0xff, g_au8SlvRxData[1]);
        CU_ASSERT_EQUAL(99, g_au8SlvData[0x9999 & 0xFF]);
    }
    //LPI2C_WriteMultiBytesTwoRegs
    {
        printf("\n\tTest LPI2C_WriteMultiBytesTwoRegs");
        g_u8RegCnt = 2;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        u32RceLen = sizeof(au8Src) - 2 + 2;
        memset((void *)g_au8SlvData, 0, 256);
        /*Set slave I2C0*/
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
        LPI2C_WriteMultiBytesTwoRegs(LPI2C0, I2C_SLVAddr[3], 9999, au8Src, sizeof(au8Src) - 2);
        CU_ASSERT_EQUAL(((9999) >> 8) & 0xff, g_au8SlvRxData[0]);
        CU_ASSERT_EQUAL(9999 & 0xff, g_au8SlvRxData[1]);

        for (i = 0; i < (sizeof(au8Src) - 2); i++) {
            CU_ASSERT_EQUAL(au8Src[i], g_au8SlvData[(9999 + i) & 0xFF]);
        }
    }
    //LPI2C_ReadByteOneReg
    {
        printf("\n\tTest LPI2C_ReadByteOneReg");
        g_au8SlvData[0x78] = 0x88;
        g_u8RegCnt = 1;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
        CU_ASSERT_EQUAL(LPI2C_ReadByteOneReg(LPI2C0, I2C_SLVAddr[0], 0x78), 0x88);
    }
    //LPI2C_ReadMultiBytesOneReg
    {
        printf("\n\tTest LPI2C_ReadMultiBytesOneReg");
        u32RceLen = sizeof(au8Src); //fist dst is used as address buffer
        /*reset the dst buffer*/
        memcpy((void *)(g_au8SlvData + 0x87), au8Src, sizeof(au8Src));
        memset((void *)au8Tmp, 0, sizeof(au8Tmp));
        g_u8RegCnt = 1;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
        CU_ASSERT_EQUAL(LPI2C_ReadMultiBytesOneReg(LPI2C0, I2C_SLVAddr[0], 0x87, au8Tmp, u32RceLen), u32RceLen);
        CU_ASSERT_EQUAL(0x87, g_au8SlvRxData[0]);

        for (i = 0; i < sizeof(au8Tmp); i++) {
            CU_ASSERT_EQUAL(au8Tmp[i], g_au8SlvData[(0x87 + i) & 0xFF]);
        }
    }
    //LPI2C_ReadMultiBytesTwoRegs
    {
        printf("\n\tTest LPI2C_ReadMultiBytesTwoRegs");
        u32RceLen = sizeof(au8Src);
        /*reset the dst buffer*/
        /*reset the dst buffer*/
        memcpy((void *)(g_au8SlvData + (0x5566 & 0xFF)), au8Src, sizeof(au8Src));
        memset((void *)au8Tmp, 0, sizeof(au8Tmp));
        g_u8RegCnt = 2;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
        CU_ASSERT_EQUAL(LPI2C_ReadMultiBytesTwoRegs(LPI2C0, I2C_SLVAddr[3], 0x5566, au8Tmp, u32RceLen), u32RceLen);
        CU_ASSERT_EQUAL(0x55, g_au8SlvRxData[0]);
        CU_ASSERT_EQUAL(0x66, g_au8SlvRxData[1]);

        for (i = 0; i < sizeof(au8Tmp); i++) {
            CU_ASSERT_EQUAL(au8Tmp[i], g_au8SlvData[(0x5566 + i) & 0xFF]);
        }
    }
    //LPI2C_ReadByteTwoRegs
    {
        printf("\n\tTest LPI2C_ReadByteTwoRegs");
        u32RceLen = sizeof(au8Src); //fist dst is used as address buffer
        /*reset the dst buffer*/
        g_au8SlvData[0x9930 & 0xFF] = 0x77;
        g_u8RegCnt = 2;
        slave_buff_addr = 0;
        fpI2C_WrRd_Test_Handler = I2C_SlaveTRx;
        LPI2C_SET_CONTROL_REG(I2C0, LPI2C_CTL_SI_AA);
        CU_ASSERT_EQUAL(LPI2C_ReadByteTwoRegs(LPI2C0, I2C_SLVAddr[3], 0x9930), 0x77);
        CU_ASSERT_EQUAL(0x99, g_au8SlvRxData[0]);
        CU_ASSERT_EQUAL(0x30, g_au8SlvRxData[1]);
    }
    /*Close all rlated settings*/
    fpI2C_WrRd_Test_Handler = NULL; // back to non-Wr/Rd API test
    LPI2C_Close(LPI2C0);
    I2C_Close(I2C0);
}

void Test_Macro_LPI2C_Timeout()
{
    LPI2C_Open(LPI2C0, 100000);
    LPI2C_EnableTimeout(LPI2C0, 1);
    CU_ASSERT_EQUAL(LPI2C0->TOCTL & LPI2C_TOCTL_TOCDIV4_Msk, LPI2C_TOCTL_TOCDIV4_Msk);
    CU_ASSERT_EQUAL(LPI2C0->TOCTL & LPI2C_TOCTL_TOCEN_Msk, LPI2C_TOCTL_TOCEN_Msk);
    CU_ASSERT_EQUAL(LPI2C0->TOCTL & LPI2C_TOCTL_TOIF_Msk, 0);

    while (LPI2C_GET_TIMEOUT_FLAG(LPI2C0) == 0) {};

    CU_ASSERT_EQUAL(LPI2C0->TOCTL & LPI2C_TOCTL_TOIF_Msk, LPI2C_TOCTL_TOIF_Msk);

    LPI2C_ClearTimeoutFlag(LPI2C0);

    CU_ASSERT_EQUAL(LPI2C0->TOCTL & LPI2C_TOCTL_TOIF_Msk, 0);

    LPI2C_DisableTimeout(LPI2C0);

    CU_ASSERT_EQUAL(LPI2C0->TOCTL & LPI2C_TOCTL_TOCEN_Msk, 0);

    LPI2C_Close(LPI2C0);
}

void Test_MACRO_LPI2C_Control_Read_Status()
{
    /* Open LPI2C0 module and set bus clock */
    LPI2C_Open(LPI2C0, 100000);
    /* Open I2C0 module and set bus clock */
    I2C_Open(I2C0, 100000);
    /* Set I2C0 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C0, 0, 0x16, 0);   /* Slave Address : 0x16 */
    I2C_SetSlaveAddr(I2C0, 1, 0x36, 0);   /* Slave Address : 0x36 */
    I2C_SetSlaveAddr(I2C0, 2, 0x56, 0);   /* Slave Address : 0x56 */
    I2C_SetSlaveAddr(I2C0, 3, 0x76, 0);   /* Slave Address : 0x76 */
    /* Set I2C0 4 Slave Addresses Mask */
    I2C_SetSlaveAddrMask(I2C0, 0, 0x04);
    I2C_SetSlaveAddrMask(I2C0, 1, 0x02);
    I2C_SetSlaveAddrMask(I2C0, 2, 0x04);
    I2C_SetSlaveAddrMask(I2C0, 3, 0x02);
    /* I2C0 enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    //
    LPI2C_START(LPI2C0);
    CU_ASSERT((LPI2C0->CTL0 & LPI2C_CTL_STA) == LPI2C_CTL_STA);
    LPI2C_WAIT_READY(LPI2C0) {};
    CU_ASSERT((LPI2C0->CTL0 & LPI2C_CTL0_SI_Msk) == LPI2C_CTL0_SI_Msk);

    while (LPI2C_GET_STATUS(LPI2C0) != 0x08);

    LPI2C_SET_DATA(LPI2C0, 0x16 << 1);
    CU_ASSERT(LPI2C_GET_DATA(LPI2C0) == (0x16 << 1));
    LPI2C_SET_CONTROL_REG(LPI2C0, LPI2C_CTL_SI);
    CU_ASSERT((LPI2C0->CTL0 & LPI2C_CTL_SI) == 0);
    LPI2C_WAIT_READY(LPI2C0);

    while (LPI2C_GET_STATUS(LPI2C0) != 0x18);

    LPI2C_SET_CONTROL_REG(LPI2C0, LPI2C_CTL_SI);
    //

    while (I2C_GET_STATUS(I2C0) != 0x60);

    I2C_SET_CONTROL_REG(I2C0, LPI2C_CTL_SI);
    //
    LPI2C_STOP(LPI2C0);
    CU_ASSERT((LPI2C0->CTL0 & LPI2C_CTL_STO_SI) != LPI2C_CTL_STO);
    //

    while (I2C_GET_STATUS(I2C0) != 0xA0);

    I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    //
    LPI2C_Close(LPI2C0);
    I2C_Close(I2C0);
}

void Test_API_MACRO_LPI2C_Wakeup()
{
    /* Open LPI2C module and set bus clock */
    LPI2C_Open(LPI2C0, 100000);
    /* Set LPI2C 4 Slave Addresses */
    LPI2C_SetSlaveAddr(LPI2C0, 0, 0x15, 0);   /* Slave Address : 0x15 */
    LPI2C_SetSlaveAddr(LPI2C0, 1, 0x35, 0);   /* Slave Address : 0x35 */
    LPI2C_SetSlaveAddr(LPI2C0, 2, 0x55, 0);   /* Slave Address : 0x55 */
    LPI2C_SetSlaveAddr(LPI2C0, 3, 0x75, 0);   /* Slave Address : 0x75 */
    /* Set LPI2C 4 Slave Addresses Mask */
    LPI2C_SetSlaveAddrMask(LPI2C0, 0, 0x01);
    LPI2C_SetSlaveAddrMask(LPI2C0, 1, 0x04);
    LPI2C_SetSlaveAddrMask(LPI2C0, 2, 0x01);
    LPI2C_SetSlaveAddrMask(LPI2C0, 3, 0x04);
    /* Set LPI2C0 enter Not Address SLAVE mode */
    LPI2C_SET_CONTROL_REG(LPI2C0, LPI2C_CTL_SI_AA);
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Enable power wake-up interrupt */
    PMC->INTEN |= PMC_INTEN_PDWKIEN_Msk;
    NVIC_EnableIRQ(PMC_IRQn);
    /* Enable LPI2C wake-up */
    LPI2C_EnableWakeup(LPI2C0);
    CU_ASSERT((LPI2C0->WKCTL & LPI2C_WKCTL_WKEN_Msk) == LPI2C_WKCTL_WKEN_Msk);
    printf("\n");
    printf("Enter PD 0x%x 0x%x\n", LPI2C0->CTL0, LPI2C0->STATUS0);
    printf("\n");
    printf("CHIP enter power down status.\n");

    /* Waiting for UART printf finish*/
    while (((UART0->FIFOSTS) & UART_FIFOSTS_TXEMPTY_Msk) == 0);

    if (((LPI2C0->CTL0)&LPI2C_CTL0_SI_Msk) != 0) {
        LPI2C_SET_CONTROL_REG(LPI2C0, LPI2C_CTL_SI);
    }

    PMC_SetPowerDownMode(PMC_NPD0, PMC_PLCTL_PLSEL_PL1);
    PMC_PowerDown();
    printf("Wake up!!\n");
    NVIC_DisableIRQ(PMC_IRQn);
    printf(">>>1\n");

    if ((LPI2C0->WKSTS & LPI2C_WKSTS_WKIF_Msk) == LPI2C_WKSTS_WKIF_Msk) {
        printf(">>>2\n");
        CU_ASSERT(LPI2C_GET_WAKEUP_FLAG(LPI2C0) == 1);
        LPI2C_CLEAR_WAKEUP_FLAG(LPI2C0);
        CU_ASSERT(LPI2C_GET_WAKEUP_FLAG(LPI2C0) == 0);
    }

    /* Lock protected registers */
    SYS_LockReg();
    LPI2C_DisableWakeup(LPI2C0);
    CU_ASSERT((LPI2C0->WKCTL & LPI2C_WKCTL_WKEN_Msk) == 0);
    LPI2C_Close(LPI2C0);
    printf(">>>3\n");
}

void Test_API_MACRO_LPI2C_PDMA()
{
    /* Enable/disable TX PDMA */
    CU_ASSERT((LPI2C0->CTL1 & LPI2C_CTL1_TXPDMAEN_Msk) == 0);
    LPI2C_ENABLE_TX_PDMA(LPI2C0);
    CU_ASSERT((LPI2C0->CTL1 & LPI2C_CTL1_TXPDMAEN_Msk) == LPI2C_CTL1_TXPDMAEN_Msk);
    LPI2C_DISABLE_TX_PDMA(LPI2C0);
    CU_ASSERT((LPI2C0->CTL1 & LPI2C_CTL1_TXPDMAEN_Msk) == 0);
    /* Enable/disable RX PDMA */
    CU_ASSERT((LPI2C0->CTL1 & LPI2C_CTL1_RXPDMAEN_Msk) == 0);
    LPI2C_ENABLE_RX_PDMA(LPI2C0);
    CU_ASSERT((LPI2C0->CTL1 & LPI2C_CTL1_RXPDMAEN_Msk) == LPI2C_CTL1_RXPDMAEN_Msk);
    LPI2C_DISABLE_RX_PDMA(LPI2C0);
    CU_ASSERT((LPI2C0->CTL1 & LPI2C_CTL1_RXPDMAEN_Msk) == 0);
    /* Enable/disable PDMA stretch function */
    CU_ASSERT((LPI2C0->CTL1 & LPI2C_CTL1_PDMASTR_Msk) == 0);
    LPI2C_ENABLE_PDMA_STRETCH(LPI2C0);
    CU_ASSERT((LPI2C0->CTL1 & LPI2C_CTL1_PDMASTR_Msk) == LPI2C_CTL1_PDMASTR_Msk);
    LPI2C_DISABLE_PDMA_STRETCH(LPI2C0);
    CU_ASSERT((LPI2C0->CTL1 & LPI2C_CTL1_PDMASTR_Msk) == 0);
}

void Test_CONST_LPI2C_CTL(void)
{
    CU_ASSERT(LPI2C_CTL_STA_SI == (LPI2C_CTL0_STA_Msk | LPI2C_CTL0_SI_Msk));
    CU_ASSERT(LPI2C_CTL_STA_SI_AA == (LPI2C_CTL0_STA_Msk | LPI2C_CTL0_SI_Msk | LPI2C_CTL0_AA_Msk));
    CU_ASSERT(LPI2C_CTL_STO_SI == (LPI2C_CTL0_STO_Msk | LPI2C_CTL0_SI_Msk));
    CU_ASSERT(LPI2C_CTL_STO_SI_AA == (LPI2C_CTL0_STO_Msk | LPI2C_CTL0_SI_Msk | LPI2C_CTL0_AA_Msk));
    CU_ASSERT(LPI2C_CTL_SI == LPI2C_CTL0_SI_Msk);
    CU_ASSERT(LPI2C_CTL_SI_AA == (LPI2C_CTL0_SI_Msk | LPI2C_CTL0_AA_Msk));
    CU_ASSERT(LPI2C_CTL_STA == LPI2C_CTL0_STA_Msk);
    CU_ASSERT(LPI2C_CTL_STO == LPI2C_CTL0_STO_Msk);
    CU_ASSERT(LPI2C_CTL_AA == LPI2C_CTL0_AA_Msk);
}

void Test_CONST_LPI2C_GCMode(void)
{
    CU_ASSERT(LPI2C_GCMODE_ENABLE == 1);
    CU_ASSERT(LPI2C_GCMODE_DISABLE == 0);
}

CU_TestInfo LPI2C_ConstTests[] = {
    {" 1: CONST LPI2C_CTL.", Test_CONST_LPI2C_CTL},
    {" 2: CONST LPI2C_GCMode.", Test_CONST_LPI2C_GCMode},
    CU_TEST_INFO_NULL
};

CU_TestInfo LPI2C_ApiTests[] = {
    {" 1: API LPI2C_Open_Close.", Test_API_LPI2C_Open_Close},
    {" 2: API LPI2C_Get_Bus_Clock", Test_API_LPI2C_BusClockFreq},
    {" 3: API LPI2C_INT_En_Dis_ABLE", Test_API_LPI2C_INT},
    {" 4: API LPI2C_Set_SLV_Address", Test_API_LPI2C_SetSLVAddr},
    {" 5: API LPI2C_Set_Get_Data", Test_API_LPI2C_Set_Get_Data},
    {" 6: API LPI2C_Time-out_En_Dis_able", Test_API_LPI2C_Timeout},
    {" 7: API LPI2C_Control_Read_STATUS", Test_API_LPI2C_Control_Read_Status},
    {" 8: API LPI2C_ReadWriteRelatedTest", Test_API_LPI2C_ReadWriteRelatedTest},
    CU_TEST_INFO_NULL
};

CU_TestInfo LPI2C_MacroTests[] = {
    {" 1: MACRO LPI2C_Get_TimeOutFlag", Test_Macro_LPI2C_Timeout},
    {" 2: MACRO LPI2C_Control_Read_STATUS", Test_MACRO_LPI2C_Control_Read_Status},
    {" 3: MACRO LPI2C_PDMA", Test_API_MACRO_LPI2C_PDMA},
    CU_TEST_INFO_NULL
};

CU_TestInfo LPI2C_WakeupAPIMacroTests[] = {
    {" 1: API_MACRO LPI2C_Wakeup", Test_API_MACRO_LPI2C_Wakeup}, //NuBridge may pull the bus, so it should test independently
    CU_TEST_INFO_NULL
};
