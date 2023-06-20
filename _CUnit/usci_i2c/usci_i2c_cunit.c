/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "usci_i2c_cunit.h"

volatile uint8_t g_au8SlvData[256];
volatile uint8_t g_u8DeviceAddr;
volatile uint32_t slave_buff_addr;
volatile uint8_t g_au8SlvRxData[4];
volatile uint16_t g_u16RecvAddr;
volatile uint8_t g_u8MstDataLen;
volatile uint8_t g_u8SlvDataLen;
volatile uint8_t g_u16SlvDataLen;
volatile uint8_t g_u8RegCnt;

extern void I2C_Slave_Init(uint32_t u32ClkSpeed);

typedef void (*UI2C_FUNC)(uint32_t u32Status);
typedef void (*I2C_FUNC)(uint32_t u32Status);

UI2C_FUNC s_UI2C0HandlerFn = NULL;
I2C_FUNC  s_I2C_Slave_HandlerFn = NULL;

/*---------------------------------------------------------------------------------------------------------*/
/*  USCI_I2C IRQ Handler                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void USCI0_IRQHandler(void)
{
    uint32_t u32Status;
    //UI2C0 Interrupt
    u32Status = UI2C_GET_PROT_STATUS(UI2C0);

    if (s_UI2C0HandlerFn != NULL) {
        s_UI2C0HandlerFn(u32Status);
    }
}

void I2C0_IRQHandler(void)
{
    uint32_t u32Status;
    //I2C0 Interrupt
    u32Status = I2C_GET_STATUS(I2C0);

    if (s_I2C_Slave_HandlerFn != NULL) {
        s_I2C_Slave_HandlerFn(u32Status);
    }
}

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

        if (slave_buff_addr == 256) {
            slave_buff_addr = 0;
        }

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


void UI2C0_Init(uint32_t u32ClkSpeed)
{
    /* Open USCI_I2C0 and set clock to 100k */
    UI2C_Open(UI2C0, u32ClkSpeed);
    /* Get USCI_I2C0 Bus Clock */
    printf("\n\tUI2C0 clock %d Hz\n", UI2C_GetBusClockFreq(UI2C0));
    /* Set USCI_I2C0 Slave Addresses */
    UI2C_SetSlaveAddr(UI2C0, 0, 0x15, UI2C_GCMODE_DISABLE);   /* Slave Address : 0x15 */
    UI2C_SetSlaveAddr(UI2C0, 1, 0x35, UI2C_GCMODE_DISABLE);   /* Slave Address : 0x35 */
    /* Set USCI_I2C0 Slave Addresses Mask */
    UI2C_SetSlaveAddrMask(UI2C0, 0, 0x01);                    /* Slave Address : 0x1 */
    UI2C_SetSlaveAddrMask(UI2C0, 1, 0x04);                    /* Slave Address : 0x4 */
}

void I2C_Slave_Init(uint32_t u32ClkSpeed)
{
    /* Open I2C module and set bus clock */
    I2C_Open(I2C0, u32ClkSpeed);
    /* Get I2C0 Bus Clock */
    printf("\tI2C0 clock %d Hz\n", I2C_GetBusClockFreq(I2C0));
    /* Set USCI_I2C1 Slave Addresses */
    I2C_SetSlaveAddr(I2C0, 0, 0x16, I2C_GCMODE_DISABLE);   /* Slave Address : 0x16 */
    I2C_SetSlaveAddr(I2C0, 1, 0x36, I2C_GCMODE_DISABLE);   /* Slave Address : 0x36 */
    /* Set USCI_I2C1 Slave Addresses Mask */
    I2C_SetSlaveAddrMask(I2C0, 0, 0x04);                    /* Slave Address : 0x4 */
    I2C_SetSlaveAddrMask(I2C0, 1, 0x02);                    /* Slave Address : 0x2 */
    /* Enable I2C1 interrupt */
    I2C_EnableInt(I2C0);
    NVIC_EnableIRQ(I2C0_IRQn);
    I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

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
#ifndef POWER_DOWN_TEST
    {"UI2C CONST", suite_success_init, suite_success_clean, NULL, NULL, UI2C_ConstTests},
    {"UI2C API", suite_success_init, suite_success_clean, NULL, NULL, UI2C_ApiTests},
    {"UI2C MACRO", suite_success_init, suite_success_clean, NULL, NULL, UI2C_MacroTests},
#else
    {"UI2C API_MACRO", suite_success_init, suite_success_clean, NULL, NULL, UI2C_WakeupAPIMacroTests},
#endif
    CU_SUITE_INFO_NULL
};

void UI2C_Reset(void)
{
    SYS_UnlockReg();
    SYS_ResetModule(SYS_USCI0RST);
    SYS_ResetModule(SYS_I2C0RST);
}

void Test_API_UI2C_Open_Close()
{
    uint32_t u32BusClock;
    /* Reset USCI */
    UI2C_Reset();
    /* API UI2C_Open Test*/
    u32BusClock = 100000;
    CU_ASSERT_EQUAL(UI2C_Open(UI2C0, u32BusClock), u32BusClock);
    CU_ASSERT((UI2C0->CTL & UI2C_CTL_FUNMODE_Msk) == (4 << UI2C_CTL_FUNMODE_Pos));
    /* API UI2C_Close Test*/
    UI2C_Close(UI2C0);
    CU_ASSERT((UI2C0->CTL & UI2C_CTL_FUNMODE_Msk) == 0);
}

void Test_API_UI2C_BusClockFreq()
{
    uint32_t u32BusClock;
    u32BusClock = 100000;
    UI2C_Open(UI2C0, u32BusClock);
    /* API UI2C_GetBusClockFreq Test*/
    CU_ASSERT_EQUAL(UI2C_GetBusClockFreq(UI2C0), UI2C_Open(UI2C0, u32BusClock));
    /* API UI2C_SetBusClockFreq Test*/
    CU_ASSERT_EQUAL(UI2C_SetBusClockFreq(UI2C0, u32BusClock), UI2C_Open(UI2C0, u32BusClock));
    UI2C_Close(UI2C0);
}

void Test_API_UI2C_INT()
{
    uint8_t i, reg;
    UI2C_T *ui2c = UI2C0;
    UI2C_Open(ui2c, 100000);

    //Independent Bit test
    for (i = 0; i < 7; i++) {
        ui2c->PROTIEN = 0;
        UI2C_EnableInt(ui2c, 1 << i);
        CU_ASSERT_EQUAL(ui2c->PROTIEN, 1 << i);
        UI2C_DisableInt(ui2c, 1 << i);
        CU_ASSERT_EQUAL(ui2c->PROTIEN, 0);
    }

    //Combine Bits INT enable test
    ui2c->PROTIEN = 0;
    reg = 0;

    for (i = 0; i < 7; i++) {
        reg |= 1 << i;
        UI2C_EnableInt(ui2c, 1 << i);
        CU_ASSERT_EQUAL(ui2c->PROTIEN, reg);
    }

    //Combine Bits INT disable test
    for (i = 0; i < 7; i++) {
        reg &= ~(1 << i);
        UI2C_DisableInt(ui2c, 1 << i);
        CU_ASSERT_EQUAL(ui2c->PROTIEN, reg);
    }

    UI2C_Close(UI2C0);
}

void Test_API_UI2C_SetSLVAddr()
{
    uint8_t i;
    uint16_t UI2C_SLVAddr[2] = {0x15, 0x35};
    uint16_t UI2C_SLVAddrMsk[2] = {0x01, 0x04};
    UI2C_T *ui2c = UI2C0;
    UI2C_Open(ui2c, 100000);

    for (i = 0; i < 2; i++) {
        UI2C_SetSlaveAddr(ui2c, i, UI2C_SLVAddr[i], UI2C_GCMODE_DISABLE);
        UI2C_SetSlaveAddrMask(ui2c, i, UI2C_SLVAddrMsk[i]);

        switch (i) {
            case 0:
                CU_ASSERT_EQUAL(ui2c->PROTCTL & UI2C_PROTCTL_GCFUNC_Msk, UI2C_GCMODE_DISABLE);
                CU_ASSERT_EQUAL(ui2c->DEVADDR0, UI2C_SLVAddr[i]);
                CU_ASSERT_EQUAL((ui2c->ADDRMSK0), UI2C_SLVAddrMsk[i]);
                break;

            case 1:
                CU_ASSERT_EQUAL(ui2c->PROTCTL & UI2C_PROTCTL_GCFUNC_Msk, UI2C_GCMODE_DISABLE);
                CU_ASSERT_EQUAL(ui2c->DEVADDR1, UI2C_SLVAddr[i]);
                CU_ASSERT_EQUAL((ui2c->ADDRMSK1), UI2C_SLVAddrMsk[i]);
                break;
        }
    }

    for (i = 0; i < 2; i++) {
        UI2C_SetSlaveAddr(ui2c, i, UI2C_SLVAddr[i], UI2C_GCMODE_ENABLE);

        switch (i) {
            case 0:
                CU_ASSERT_EQUAL(ui2c->PROTCTL & UI2C_PROTCTL_GCFUNC_Msk, UI2C_GCMODE_ENABLE);
                break;

            case 1:
                CU_ASSERT_EQUAL(ui2c->PROTCTL & UI2C_PROTCTL_GCFUNC_Msk, UI2C_GCMODE_ENABLE);
                break;
        }
    }

    UI2C_Close(ui2c);
}

void Test_API_UI2C_Timeout()
{
    UI2C_T *ui2c = UI2C0;
    UI2C_Open(ui2c, 100000);
    UI2C_EnableTimeout(ui2c, 0x3FF);
    CU_ASSERT_EQUAL(ui2c->PROTCTL & UI2C_PROTCTL_TOCNT_Msk, (0x3FF << UI2C_PROTCTL_TOCNT_Pos));
    CU_ASSERT_EQUAL(ui2c->BRGEN & UI2C_BRGEN_TMCNTEN_Msk, UI2C_BRGEN_TMCNTEN_Msk);
    CU_ASSERT_EQUAL(ui2c->PROTSTS & UI2C_PROTSTS_TOIF_Msk, 0);

    while (UI2C_GetIntFlag(ui2c, UI2C_TO_INT_MASK) == 0) { }

    CU_ASSERT_EQUAL(ui2c->PROTSTS & UI2C_PROTSTS_TOIF_Msk, UI2C_PROTSTS_TOIF_Msk);
    UI2C_ClearTimeoutFlag(ui2c);
    CU_ASSERT_EQUAL(ui2c->PROTSTS & UI2C_PROTSTS_TOIF_Msk, 0);
    UI2C_DisableTimeout(ui2c);
    CU_ASSERT_EQUAL(ui2c->PROTCTL & UI2C_PROTCTL_TOCNT_Msk, 0);
    CU_ASSERT_EQUAL(ui2c->BRGEN & UI2C_BRGEN_TMCNTEN_Msk, 0);
    UI2C_Close(ui2c);
}

void Test_API_UI2C_Control_Read_Status()
{
    UI2C_T *ui2c = UI2C0;
    /* Open UI2C0 module and set bus clock */
    UI2C_Open(ui2c, 100000);
    /* Set UI2C0 Slave Addresses */
    UI2C_SetSlaveAddr(ui2c, 0, 0x15, 0);   /* Slave Address : 0x15 */
    // I2C0 Slave
    {
        /* Open I2C0 module and set bus clock */
        I2C_Open(I2C0, 100000);
        /* Enable I2C */
        I2C0->CTL0 |= I2C_CTL0_I2CEN_Msk;
        /* Set I2C0 Slave Addresses */
        I2C_SetSlaveAddr(I2C0, 0, 0x16, 0);   /* Slave Address : 0x16 */
        /* Set I2C0 Slave Addresses Mask */
        I2C_SetSlaveAddrMask(I2C0, 0, 0x04);
        /* I2C enter no address SLV mode */
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    }
    /* UI2C0 as master sends START signal */
    UI2C_Trigger(UI2C0, 1, 0, 0, 0);
    CU_ASSERT((UI2C0->PROTCTL & UI2C_PROTCTL_STA_Msk) == UI2C_PROTCTL_STA_Msk);

    while ((UI2C_GetIntFlag(UI2C0, UI2C_STAR_INT_MASK) & UI2C_STAR_INT_MASK) == 0) { }

    CU_ASSERT((UI2C0->PROTSTS & UI2C_PROTSTS_STARIF_Msk) == UI2C_PROTSTS_STARIF_Msk);
    UI2C_ClearIntFlag(UI2C0, UI2C_STAR_INT_MASK);
    CU_ASSERT((UI2C0->PROTSTS & UI2C_PROTSTS_STARIF_Msk) == 0);
    UI2C_SetData(UI2C0, (0x16 << 1) | 1);
    UI2C_Trigger(UI2C0, 0, 0, 1, 0);
    CU_ASSERT((UI2C0->PROTCTL & UI2C_PROTCTL_PTRG_Msk) == 0);

    while ((UI2C_GetIntFlag(UI2C0, UI2C_ACK_INT_MASK) & UI2C_ACK_INT_MASK) != UI2C_ACK_INT_MASK) { }

    CU_ASSERT((UI2C0->PROTSTS & UI2C_PROTSTS_ACKIF_Msk) == UI2C_PROTSTS_ACKIF_Msk);
    UI2C_ClearIntFlag(UI2C0, UI2C_ACK_INT_MASK);
    CU_ASSERT((UI2C0->PROTSTS & UI2C_PROTSTS_ACKIF_Msk) == 0);
    // I2C0 Slave
    {
        while (I2C_GetIntFlag(I2C0) == 0) {};

        CU_ASSERT((I2C0->CTL0 & I2C_CTL0_SI_Msk) == I2C_CTL0_SI_Msk);

        CU_ASSERT(I2C_GetStatus(I2C0) == 0xA8);

        I2C_Trigger(I2C0, 0, 0, 1, 0);

        CU_ASSERT((I2C0->CTL0 & I2C_CTL0_SI_Msk) == 0);

        CU_ASSERT(I2C_GetData(I2C0) == (0x16 << 1 | 1));

        I2C_SetData(I2C0, 0x55);

        I2C_Trigger(I2C0, 0, 0, 0, 1);
    }
    UI2C_Trigger(UI2C0, 0, 0, 1, 0);

    while ((UI2C_GetIntFlag(UI2C0, UI2C_NACK_INT_MASK) & UI2C_NACK_INT_MASK) != UI2C_NACK_INT_MASK) { }

    CU_ASSERT((UI2C0->PROTSTS & UI2C_PROTSTS_NACKIF_Msk) == UI2C_PROTSTS_NACKIF_Msk);
    UI2C_ClearIntFlag(UI2C0, UI2C_NACK_INT_MASK);
    CU_ASSERT((UI2C0->PROTSTS & UI2C_PROTSTS_NACKIF_Msk) == 0);
    // I2C0 Slave
    {
        while (I2C_GetIntFlag(I2C0) == 0) {};

        CU_ASSERT((I2C0->CTL0 & I2C_CTL0_SI_Msk) == I2C_CTL0_SI_Msk);

        CU_ASSERT(I2C_GetStatus(I2C0) == 0xC0);

        I2C_Trigger(I2C0, 0, 0, 1, 0);

        CU_ASSERT((I2C0->CTL0 & I2C_CTL0_SI_Msk) == 0);

        I2C_Trigger(I2C0, 0, 0, 0, 1);
    }
    UI2C_Trigger(UI2C0, 0, 1, 1, 0);
    UI2C_Close(UI2C0);
    I2C_Close(I2C0);
}

void Test_Macro_UI2C_Timeout()
{
    UI2C_T *ui2c = UI2C0;
    UI2C_Open(ui2c, 100000);
    UI2C_EnableTimeout(ui2c, 40);
    CU_ASSERT_EQUAL(ui2c->PROTCTL & UI2C_PROTCTL_TOCNT_Msk, (40 << UI2C_PROTCTL_TOCNT_Pos));
    CU_ASSERT_EQUAL(ui2c->BRGEN & UI2C_BRGEN_TMCNTEN_Msk, UI2C_BRGEN_TMCNTEN_Msk);
    CU_ASSERT_EQUAL(ui2c->PROTSTS & UI2C_PROTSTS_TOIF_Msk, 0);

    while (UI2C_GET_TIMEOUT_FLAG(ui2c) == 0) { }

    CU_ASSERT_EQUAL(ui2c->PROTSTS & UI2C_PROTSTS_TOIF_Msk, UI2C_PROTSTS_TOIF_Msk);
    UI2C_CLR_PROT_INT_FLAG(ui2c, UI2C_PROTSTS_TOIF_Msk);
    CU_ASSERT_EQUAL(ui2c->PROTSTS & UI2C_PROTSTS_TOIF_Msk, 0);
    UI2C_DisableTimeout(ui2c);
    CU_ASSERT_EQUAL(ui2c->PROTCTL & UI2C_PROTCTL_TOCNT_Msk, 0);
    CU_ASSERT_EQUAL(ui2c->BRGEN & UI2C_BRGEN_TMCNTEN_Msk, 0);
    UI2C_Close(ui2c);
}

void Test_Macro_UI2C_Enable_10Bit_Addr_Mode()
{
    UI2C_T *ui2c = UI2C0;
    UI2C_Open(ui2c, 100000);
    UI2C_ENABLE_10BIT_ADDR_MODE(ui2c);
    CU_ASSERT_EQUAL(ui2c->PROTCTL & UI2C_PROTCTL_ADDR10EN_Msk, UI2C_PROTCTL_ADDR10EN_Msk);
    UI2C_DISABLE_10BIT_ADDR_MODE(ui2c);
    CU_ASSERT_EQUAL(ui2c->PROTCTL & UI2C_PROTCTL_ADDR10EN_Msk, 0);
    UI2C_Close(ui2c);
}

void Test_MACRO_UI2C_Control_Read_Status()
{
    UI2C_Reset();
    /* Open UI2C0 module and set bus clock */
    UI2C_Open(UI2C0, 100000);
    /* Set UI2C1 Slave Addresses */
    UI2C_SetSlaveAddr(UI2C0, 0, 0x15, 0);   /* Slave Address : 0x15 */
#if 1
    /* Open I2C0 module and set bus clock */
    I2C_Open(I2C0, 100000);
    /* Set I2C0 Slave Addresses */
    I2C_SetSlaveAddr(I2C0, 0, 0x16, 0);   /* Slave Address : 0x16 */
    /* Set I2C0 Slave Addresses Mask */
    I2C_SetSlaveAddrMask(I2C0, 0, 0x04);
    /* I2C enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
#endif
    UI2C_START(UI2C0);
    CU_ASSERT((UI2C0->PROTCTL & UI2C_PROTCTL_STA_Msk) == UI2C_PROTCTL_STA_Msk);

    while ((UI2C_GET_PROT_STATUS(UI2C0) & UI2C_PROTSTS_STARIF_Msk) != UI2C_PROTSTS_STARIF_Msk) { }

    CU_ASSERT((UI2C0->PROTSTS & UI2C_PROTSTS_STARIF_Msk) == UI2C_PROTSTS_STARIF_Msk);
    UI2C_CLR_PROT_INT_FLAG(UI2C0, UI2C_PROTSTS_STARIF_Msk);
    CU_ASSERT((UI2C0->PROTSTS & UI2C_PROTSTS_STARIF_Msk) == 0);
    //
    UI2C_SET_DATA(UI2C0, 0x16 << 1);
    UI2C_SET_CONTROL_REG(UI2C0, UI2C_CTL_PTRG);
    CU_ASSERT((UI2C0->PROTCTL & UI2C_PROTCTL_PTRG_Msk) == 0);

    while ((UI2C_GET_PROT_STATUS(UI2C0) & UI2C_PROTSTS_ACKIF_Msk) != UI2C_PROTSTS_ACKIF_Msk) { }

    CU_ASSERT((UI2C0->PROTSTS & UI2C_PROTSTS_ACKIF_Msk) == UI2C_PROTSTS_ACKIF_Msk);
    UI2C_CLR_PROT_INT_FLAG(UI2C0, UI2C_PROTSTS_ACKIF_Msk);
    CU_ASSERT((UI2C0->PROTSTS & UI2C_PROTSTS_ACKIF_Msk) == 0);
#if 1

    while (I2C_GET_STATUS(I2C0) != 0x60)
        ;

    I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
#endif
    //UI2C_SET_CONTROL_REG(UI2C0, (UI2C_CTL_PTRG | UI2C_CTL_STO));
    UI2C_STOP(UI2C0);
#if 1

    while (I2C_GET_STATUS(I2C0) != 0xA0)
        ;

    I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
#endif
    UI2C_Close(UI2C0);
    I2C_Close(I2C0);
}

void Test_MACRO_UI2C_Enable_Protocol_INT()
{
    UI2C_Open(UI2C0, 100000);
    UI2C_ENABLE_PROT_INT(UI2C0, UI2C_PROTIEN_TOIEN_Msk);
    CU_ASSERT_EQUAL(UI2C0->PROTIEN & UI2C_PROTIEN_TOIEN_Msk, UI2C_PROTIEN_TOIEN_Msk);
    UI2C_ENABLE_PROT_INT(UI2C0, UI2C_PROTIEN_STARIEN_Msk);
    CU_ASSERT_EQUAL(UI2C0->PROTIEN & UI2C_PROTIEN_STARIEN_Msk, UI2C_PROTIEN_STARIEN_Msk);
    UI2C_ENABLE_PROT_INT(UI2C0, UI2C_PROTIEN_STORIEN_Msk);
    CU_ASSERT_EQUAL(UI2C0->PROTIEN & UI2C_PROTIEN_STORIEN_Msk, UI2C_PROTIEN_STORIEN_Msk);
    UI2C_ENABLE_PROT_INT(UI2C0, UI2C_PROTIEN_NACKIEN_Msk);
    CU_ASSERT_EQUAL(UI2C0->PROTIEN & UI2C_PROTIEN_NACKIEN_Msk, UI2C_PROTIEN_NACKIEN_Msk);
    UI2C_ENABLE_PROT_INT(UI2C0, UI2C_PROTIEN_ARBLOIEN_Msk);
    CU_ASSERT_EQUAL(UI2C0->PROTIEN & UI2C_PROTIEN_ARBLOIEN_Msk, UI2C_PROTIEN_ARBLOIEN_Msk);
    UI2C_ENABLE_PROT_INT(UI2C0, UI2C_PROTIEN_ERRIEN_Msk);
    CU_ASSERT_EQUAL(UI2C0->PROTIEN & UI2C_PROTIEN_ERRIEN_Msk, UI2C_PROTIEN_ERRIEN_Msk);
    UI2C_ENABLE_PROT_INT(UI2C0, UI2C_PROTIEN_ACKIEN_Msk);
    CU_ASSERT_EQUAL(UI2C0->PROTIEN & UI2C_PROTIEN_ACKIEN_Msk, UI2C_PROTIEN_ACKIEN_Msk);
    UI2C_DISABLE_PROT_INT(UI2C0, UI2C_PROTIEN_ACKIEN_Msk);
    CU_ASSERT_EQUAL(UI2C0->PROTIEN & 0x7F, 0x3F);
    UI2C_DISABLE_PROT_INT(UI2C0, UI2C_PROTIEN_ERRIEN_Msk);
    CU_ASSERT_EQUAL(UI2C0->PROTIEN & 0x7F, 0x1F);
    UI2C_DISABLE_PROT_INT(UI2C0, UI2C_PROTIEN_ARBLOIEN_Msk);
    CU_ASSERT_EQUAL(UI2C0->PROTIEN & 0x7F, 0x0F);
    UI2C_DISABLE_PROT_INT(UI2C0, UI2C_PROTIEN_NACKIEN_Msk);
    CU_ASSERT_EQUAL(UI2C0->PROTIEN & 0x7F, 0x07);
    UI2C_DISABLE_PROT_INT(UI2C0, UI2C_PROTIEN_STORIEN_Msk);
    CU_ASSERT_EQUAL(UI2C0->PROTIEN & 0x7F, 0x03);
    UI2C_DISABLE_PROT_INT(UI2C0, UI2C_PROTIEN_STARIEN_Msk);
    CU_ASSERT_EQUAL(UI2C0->PROTIEN & 0x7F, 0x01);
    UI2C_DISABLE_PROT_INT(UI2C0, UI2C_PROTIEN_TOIEN_Msk);
    CU_ASSERT_EQUAL(UI2C0->PROTIEN & 0x7F, 0x00);
    UI2C_Close(UI2C0);
}

#if defined(POWER_DOWN_TEST)

volatile enum UI2C_SLAVE_EVENT s_Event;
volatile uint32_t g_u32WKfromAddr;
volatile uint8_t g_u8SlvPWRDNWK = 0, g_u8SlvI2CWK = 0;
volatile uint8_t g_u8DataLenS;
volatile uint8_t g_u8SlvDataRet = 0;

/*---------------------------------------------------------------------------------------------------------*/
/*  UI2C0 address match wake-up                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UI2C_SLV_Address_Wakeup(uint32_t u32Status)
{
    uint32_t temp;

    if ((UI2C0->WKSTS & UI2C_WKSTS_WKF_Msk) == UI2C_WKSTS_WKF_Msk) {
        g_u32WKfromAddr = 1;
        g_u8SlvI2CWK = 1;

        /* */
        while ((UI2C0->PROTSTS & UI2C_PROTSTS_WKAKDONE_Msk) == 0) {};

        CU_ASSERT(UI2C_GET_WAKEUP_FLAG(UI2C0) == 1);

        /* Clear WK flag */
        UI2C_CLR_WAKEUP_FLAG(UI2C0);

        CU_ASSERT(UI2C_GET_WAKEUP_FLAG(UI2C0) == 0);

        UI2C0->PROTSTS = UI2C_PROTSTS_WKAKDONE_Msk;

        return;
    }

    if ((u32Status & UI2C_PROTSTS_STARIF_Msk) == UI2C_PROTSTS_STARIF_Msk) {
        /* Clear START INT Flag */
        UI2C_CLR_PROT_INT_FLAG(UI2C0, UI2C_PROTSTS_STARIF_Msk);
        /* Event process */
        g_u8DataLenS = 0;
        s_Event = SLAVE_ADDRESS_ACK;
        UI2C_SET_CONTROL_REG(UI2C0, (UI2C_CTL_PTRG | UI2C_CTL_AA));
    } else if ((u32Status & UI2C_PROTSTS_ACKIF_Msk) == UI2C_PROTSTS_ACKIF_Msk) {
        /* Clear ACK INT Flag */
        UI2C_CLR_PROT_INT_FLAG(UI2C0, UI2C_PROTSTS_ACKIF_Msk);

        /* Event process */
        if (s_Event == SLAVE_ADDRESS_ACK) {
            g_u8DataLenS = 0;

            if ((UI2C0->PROTSTS & UI2C_PROTSTS_SLAREAD_Msk) == UI2C_PROTSTS_SLAREAD_Msk) {
                /* Own SLA+R has been receive; ACK has been return */
                s_Event = SLAVE_SEND_DATA;
                UI2C_SET_DATA(UI2C0, g_u8SlvDataRet);
                g_u8SlvDataRet++;
            } else {
                s_Event = SLAVE_GET_DATA;
            }

            g_u16RecvAddr = (uint8_t)UI2C_GET_DATA(UI2C0);
        } else if (s_Event == SLAVE_GET_DATA) {
            g_au8SlvData[g_u8SlvDataLen] = (unsigned char)UI2C_GET_DATA(UI2C0);
            g_u8SlvDataLen++;
        }

        UI2C_SET_CONTROL_REG(UI2C0, (UI2C_CTL_PTRG | UI2C_CTL_AA));
    } else if ((u32Status & UI2C_PROTSTS_NACKIF_Msk) == UI2C_PROTSTS_NACKIF_Msk) {
        /* Clear NACK INT Flag */
        UI2C_CLR_PROT_INT_FLAG(UI2C0, UI2C_PROTSTS_NACKIF_Msk);
        /* Event process */
        // g_u8DataLenS = 0;
        s_Event = SLAVE_ADDRESS_ACK;
        UI2C_SET_CONTROL_REG(UI2C0, (UI2C_CTL_PTRG | UI2C_CTL_AA));
    } else if ((u32Status & UI2C_PROTSTS_STORIF_Msk) == UI2C_PROTSTS_STORIF_Msk) {
        /* Clear STOP INT Flag */
        UI2C_CLR_PROT_INT_FLAG(UI2C0, UI2C_PROTSTS_STORIF_Msk);
        // g_u8DataLenS = 0;
        s_Event = SLAVE_ADDRESS_ACK;
        UI2C_SET_CONTROL_REG(UI2C0, (UI2C_CTL_PTRG | UI2C_CTL_AA));
    }
}

void Test_API_MACRO_UI2C_Wakeup()
{
    UI2C_T *ui2c = UI2C0;
    /* Reset USCI */
    UI2C_Reset();
    /* Open I2C module and set bus clock */
    UI2C_Open(ui2c, 100000);
    /* Set UI2C Slave Addresses */
    UI2C_SetSlaveAddr(ui2c, 0, 0x15, UI2C_GCMODE_DISABLE);   /* Slave Address : 0x15 */
    /* Set UI2C Slave Addresses Mask */
    UI2C_SetSlaveAddrMask(ui2c, 0, 0x01);
    /* Set UI2C0 enter Not Address SLAVE mode */
    UI2C_SET_CONTROL_REG(ui2c, (UI2C_CTL_PTRG | UI2C_CTL_AA));
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Enable power wake-up interrupt */
    UI2C_ENABLE_PROT_INT(ui2c, (UI2C_PROTIEN_ACKIEN_Msk | UI2C_PROTIEN_NACKIEN_Msk | UI2C_PROTIEN_STORIEN_Msk | UI2C_PROTIEN_STARIEN_Msk));
    //NVIC_EnableIRQ(PWRWU_IRQn);
    /* Enable I2C wake-up */
    UI2C_EnableWakeup(ui2c, UI2C_ADDR_MATCH_WK);
    s_Event = SLAVE_ADDRESS_ACK;
    /* I2C function to Slave receive/transmit data */
    s_UI2C0HandlerFn = UI2C_SLV_Address_Wakeup;
    CU_ASSERT((ui2c->WKCTL & UI2C_WKCTL_WKADDREN_Msk) == UI2C_WKCTL_WKADDREN_Msk);
    CU_ASSERT((ui2c->WKCTL & UI2C_WKCTL_WKEN_Msk) == UI2C_WKCTL_WKEN_Msk);
    NVIC_EnableIRQ(USCI0_IRQn);
    printf("\n");
    printf("Enter PD 0x%x 0x%x\n", ui2c->PROTCTL, ui2c->PROTSTS);
    printf("\n");
    printf("CHIP enter power down status.\n");

    /* Waiting for UART printf finish*/
    while (((UART0->FIFOSTS) & UART_FIFOSTS_TXEMPTYF_Msk) == 0);

    if (ui2c->PROTSTS != 0) {
        ui2c->PROTSTS = ui2c->PROTSTS;
    }

    PMC_SetPowerDownMode(PMC_NPD0, PMC_PLCTL_PLSEL_PL1);
    PMC_PowerDown();
    printf("Wake up!!\n");
    /* Lock protected registers */
    SYS_LockReg();
    UI2C_DisableWakeup(ui2c);
    CU_ASSERT((ui2c->WKCTL & UI2C_WKCTL_WKEN_Msk) == 0);
    UI2C_Close(ui2c);
}


#endif

#define SINGLE_BYTE_RW_TEST_COUNT 10
#define MULTI_BYTES_RW_TEST_SIZE  16

void Test_API_UI2C_Single_Multi_Bytes_RW(void)
{
    uint32_t i, u32APIretV;
    uint8_t u8tmp, u8data, err, txbuf[256] = {0}, rDataBuf[256] = {0};
    /* Init USCI_I2C0 and I2C0 */
    UI2C_Reset();
    UI2C0_Init(100000);
    I2C_Slave_Init(100000);

    for (i = 0; i < 0x100; i++) {
        g_au8SlvData[i] = 0;
    }

    g_u8RegCnt = 0;
    s_I2C_Slave_HandlerFn = I2C_SlaveTRx;
    g_u8DeviceAddr = 0x16;
    slave_buff_addr = 0;
    err = 0;

    for (i = 0; i < SINGLE_BYTE_RW_TEST_COUNT; i++) {
        u8tmp = (uint8_t)i + 4;
        u32APIretV = UI2C_WriteByte(UI2C0, g_u8DeviceAddr, u8tmp);
        CU_ASSERT(u32APIretV == 0);

        if (u32APIretV) {
            printf("[NoReg] Single byte write fail...\n");

            while (1);
        } else {
            printf(".");
        }
    }

    slave_buff_addr = 0;

    for (i = 0; i < SINGLE_BYTE_RW_TEST_COUNT; i++) {
        u8tmp = (uint8_t)i + 4;
        u8data = UI2C_ReadByte(UI2C0, g_u8DeviceAddr);
        CU_ASSERT_EQUAL(u8data, u8tmp);

        if (u8data != u8tmp) {
            printf("[NoReg] %03d: Error W(0x%X)/W(0x%X) \n", i, u8tmp, u8data);

            while (1);
        } else {
            printf("o");
        }
    }

    if (err) {
        printf("\n[NoReg] Single byte Read/Write access Fail.....\n");
    } else {
        printf("\n[NoReg] Single byte Read/Write access Pass.....\n");
    }

    //======================================================
    s_I2C_Slave_HandlerFn = I2C_SlaveTRx;
    slave_buff_addr = 0;
    g_u8DeviceAddr = 0x16;
    err = 0;

    for (i = 0; i < 256; i++) {
        rDataBuf[i] = 0;
        txbuf[i] = (uint8_t) i + 3;
    }

    u32APIretV = UI2C_WriteMultiBytes(UI2C0, g_u8DeviceAddr, txbuf, MULTI_BYTES_RW_TEST_SIZE);
    CU_ASSERT(u32APIretV == MULTI_BYTES_RW_TEST_SIZE);

    if (u32APIretV != MULTI_BYTES_RW_TEST_SIZE) {
        printf("[NoReg] Multi byte write fail....\n");

        while (1);
    }

    g_u16SlvDataLen = 0;
    slave_buff_addr = 0;
    u32APIretV = UI2C_ReadMultiBytes(UI2C0, g_u8DeviceAddr, rDataBuf, MULTI_BYTES_RW_TEST_SIZE);
    CU_ASSERT(u32APIretV == MULTI_BYTES_RW_TEST_SIZE);

    if (u32APIretV != MULTI_BYTES_RW_TEST_SIZE) {
        printf("[NoReg] Multi byte read fail....\n");

        while (1);
    } else {
        for (i = 0; i < MULTI_BYTES_RW_TEST_SIZE; i++) {
            CU_ASSERT_EQUAL(txbuf[i], rDataBuf[i]);

            if (txbuf[i] != rDataBuf[i]) {
                err = 1;
                printf("[NoReg][%d]: W(0x%X), R(0x%X)\n", i, txbuf[i], rDataBuf[i]);

                while (1);
            }
        }
    }

    if (err) {
        printf("\n[NoReg] Multi bytes Read/Write access Fail.....\n");
    } else {
        printf("\n[NoReg] Multi bytes Read/Write access Pass.....\n");
    }

    UI2C_Close(UI2C0);
    I2C_Close(I2C0);
}

void Test_API_UI2C_Single_Multi_Bytes_RW_OneReg(void)
{
    uint32_t i, u32APIretV;
    uint8_t u8tmp, u8data, err, txbuf[256] = {0}, rDataBuf[256] = {0};
    /* Init USCI_I2C0 and I2C0 */
    UI2C_Reset();
    UI2C0_Init(100000);
    I2C_Slave_Init(100000);

    for (i = 0; i < 0x100; i++) {
        g_au8SlvData[i] = 0;
    }

    g_u8RegCnt = 1;
    s_I2C_Slave_HandlerFn = I2C_SlaveTRx;
    g_u8DeviceAddr = 0x16;
    err = 0;

    for (i = 0; i < SINGLE_BYTE_RW_TEST_COUNT; i++) {
        u8tmp = (uint8_t)i + 4;
        u32APIretV = UI2C_WriteByteOneReg(UI2C0, g_u8DeviceAddr, i, u8tmp);
        CU_ASSERT(u32APIretV == 0);

        if (u32APIretV) {
            printf("[OneReg] Single byte write fail...\n");

            while (1);
        } else {
            printf(".");
        }

        u8data = UI2C_ReadByteOneReg(UI2C0, g_u8DeviceAddr, i);
        CU_ASSERT_EQUAL(u8data, u8tmp);

        if (u8data != u8tmp) {
            err = 1;
            printf("[OneReg] %03d: Error W(0x%X)/W(0x%X) \n", i, u8tmp, u8data);
        } else {
            printf("o");
        }
    }

    if (err) {
        printf("\n[OneReg] Single byte Read/Write access Fail.....\n");
    } else {
        printf("\n[OneReg] Single byte Read/Write access Pass.....\n");
    }

    g_u8RegCnt = 1;
    s_I2C_Slave_HandlerFn = I2C_SlaveTRx;
    err = 0;
    g_u8DeviceAddr = 0x16;

    for (i = 0; i < 256; i++) {
        rDataBuf[i] = 0;
        txbuf[i] = (uint8_t) i + 3;
    }

    u32APIretV = UI2C_WriteMultiBytesOneReg(UI2C0, g_u8DeviceAddr, 0x0, txbuf, MULTI_BYTES_RW_TEST_SIZE);
    CU_ASSERT(u32APIretV == MULTI_BYTES_RW_TEST_SIZE);

    if (u32APIretV != MULTI_BYTES_RW_TEST_SIZE) {
        printf("[OneReg] Multi byte write fail....\n");

        while (1);
    }

    u32APIretV = UI2C_ReadMultiBytesOneReg(UI2C0, g_u8DeviceAddr, 0x0000, rDataBuf, MULTI_BYTES_RW_TEST_SIZE);
    CU_ASSERT(u32APIretV == MULTI_BYTES_RW_TEST_SIZE);

    if (u32APIretV != MULTI_BYTES_RW_TEST_SIZE) {
        printf("[OneReg] Multi byte read fail....\n");

        while (1);
    } else {
        for (i = 0; i < MULTI_BYTES_RW_TEST_SIZE; i++) {
            CU_ASSERT_EQUAL(txbuf[i], rDataBuf[i]);

            if (txbuf[i] != rDataBuf[i]) {
                printf("[OneReg][%d]: W(0x%X), R(0x%X)\n", i, txbuf[i], rDataBuf[i]);

                while (1);
            }
        }
    }

    if (err) {
        printf("\n[OneReg] Multi bytes Read/Write access Fail.....\n");
    } else {
        printf("\n[OneReg] Multi bytes Read/Write access Pass.....\n");
    }

    UI2C_Close(UI2C0);
    I2C_Close(I2C0);
}

void Test_API_UI2C_Single_Multi_Bytes_RW_TwoReg(void)
{
    uint32_t i, u32APIretV;
    uint8_t u8tmp, u8data, err, txbuf[256] = {0}, rDataBuf[256] = {0};
    /* Init USCI_I2C0 and I2C0 */
    UI2C_Reset();
    UI2C0_Init(100000);
    I2C_Slave_Init(100000);

    for (i = 0; i < 0x100; i++) {
        g_au8SlvData[i] = 0;
    }

    g_u8RegCnt = 2;
    s_I2C_Slave_HandlerFn = I2C_SlaveTRx;
    g_u8DeviceAddr = 0x16;
    err = 0;

    for (i = 0; i < SINGLE_BYTE_RW_TEST_COUNT; i++) {
        u8tmp = (uint8_t)i + 3;
        /* Single Byte Write (Two Registers) */
        u32APIretV =  UI2C_WriteByteTwoRegs(UI2C0, g_u8DeviceAddr, 255 - i, u8tmp);
        CU_ASSERT(u32APIretV == 0);

        if (u32APIretV) {
            err = 1;
            printf("\n[TwoReg] Single byte write data fail....\n");

            while (1);
        } else {
            printf(".");
        }

//        CLK_SysTickDelay(5000);              //If Slave reponse slowly , add delay here
        /* Single Byte Read (Two Registers) */
        u8data  = UI2C_ReadByteTwoRegs(UI2C0, g_u8DeviceAddr, 255 - i);
        CU_ASSERT_EQUAL(u8data, u8tmp);

        if (u8data  != u8tmp) {
            err = 1;
            printf("[TwoReg] %03d: Single byte write data fail,  W(0x%X)/W(0x%X) \n", i, u8tmp, u8tmp);

            while (1);
        } else {
            printf("o");
        }
    }

    if (err) {
        printf("\n[TwoReg] Single byte Read/Write access Fail.....\n");
    } else {
        printf("\n[TwoReg] Single byte Read/Write access Pass.....\n");
    }

    err = 0;
    g_u8RegCnt = 2;
    s_I2C_Slave_HandlerFn = I2C_SlaveTRx;
    g_u8DeviceAddr = 0x16;

    /* Prepare data for transmission */
    for (i = 0; i < 256; i++) {
        txbuf[i] = (uint8_t) i + 3;
    }

    {
        /* Write 32 bytes data to Slave */
        u32APIretV = UI2C_WriteMultiBytesTwoRegs(UI2C0, g_u8DeviceAddr, 0x0000, txbuf, MULTI_BYTES_RW_TEST_SIZE);
        CU_ASSERT(u32APIretV == MULTI_BYTES_RW_TEST_SIZE);

        if (u32APIretV != MULTI_BYTES_RW_TEST_SIZE) {
            err = 1;
            printf("\n[TwoReg] Multi bytes write data fail....\n");

            while (1);
        }
    }

    /* Use Multi Bytes Read from Slave (Two Registers) */
    u32APIretV = UI2C_ReadMultiBytesTwoRegs(UI2C0, g_u8DeviceAddr, 0x0000, rDataBuf, MULTI_BYTES_RW_TEST_SIZE);
    CU_ASSERT(u32APIretV == MULTI_BYTES_RW_TEST_SIZE);

    if (u32APIretV != MULTI_BYTES_RW_TEST_SIZE) {
        err  = 1;
        /* if transmission occurs error */
        printf("\n[TwoReg] Multi byes read Data Fail...\n");

        while (1);
    } else {
        /* Compare TX data and RX data */
        for (i = 0; i < MULTI_BYTES_RW_TEST_SIZE; i++) {
            CU_ASSERT_EQUAL(txbuf[i], rDataBuf[i]);

            if (txbuf[i] != rDataBuf[i]) {
                err = 1;
                printf("[TwoReg] R[%d] Data: 0x%X\n", i, rDataBuf[i]);
            }
        }
    }

    if (err) {
        printf("\n[TwoReg] Multi bytes Read/Write access Fail.....\n");
    } else {
        printf("\n[TwoReg] Multi bytes Read/Write access Pass.....\n");
    }

    UI2C_Close(UI2C0);
    I2C_Close(I2C0);
}

void Test_CONST_UI2C(void)
{
    //
    CU_ASSERT(UI2C_CTL_PTRG == UI2C_PROTCTL_PTRG_Msk);
    CU_ASSERT(UI2C_CTL_STA == UI2C_PROTCTL_STA_Msk);
    CU_ASSERT(UI2C_CTL_STO == UI2C_PROTCTL_STO_Msk);
    CU_ASSERT(UI2C_CTL_AA == UI2C_PROTCTL_AA_Msk);
    //
    CU_ASSERT(UI2C_GCMODE_ENABLE == UI2C_PROTCTL_GCFUNC_Msk);
    CU_ASSERT(UI2C_GCMODE_DISABLE == 0);
    //
    CU_ASSERT(UI2C_DATA_TOGGLE_WK == 0);
    CU_ASSERT(UI2C_ADDR_MATCH_WK == UI2C_WKCTL_WKADDREN_Msk);
    //
    CU_ASSERT(UI2C_TO_INT_MASK == 0x001);
    CU_ASSERT(UI2C_STAR_INT_MASK == 0x002);
    CU_ASSERT(UI2C_STOR_INT_MASK == 0x004);
    CU_ASSERT(UI2C_NACK_INT_MASK == 0x008);
    CU_ASSERT(UI2C_ARBLO_INT_MASK == 0x010);
    CU_ASSERT(UI2C_ERR_INT_MASK == 0x020);
    CU_ASSERT(UI2C_ACK_INT_MASK == 0x040);
}

CU_TestInfo UI2C_ConstTests[] = {
    {" 1: CONST UI2C All.", Test_CONST_UI2C},
    CU_TEST_INFO_NULL
};

CU_TestInfo UI2C_ApiTests[] = {

    {" 1: API UI2C_Open_Close.", Test_API_UI2C_Open_Close},
    {" 2: API UI2C_Get_Set_Bus_Clock", Test_API_UI2C_BusClockFreq},
    {" 3: API UI2C_INT_En_Dis_ABLE", Test_API_UI2C_INT},
    {" 4: API UI2C_Set_SLV_Address", Test_API_UI2C_SetSLVAddr},
    {" 5: API UI2C_Time-out_En_Dis_able", Test_API_UI2C_Timeout},
    {" 6: API UI2C_Control_Read_STATUS", Test_API_UI2C_Control_Read_Status},
    {" 7: API Single/Multi Byte RW", Test_API_UI2C_Single_Multi_Bytes_RW},
    {" 8: API Single/Multi Byte RW OneReg", Test_API_UI2C_Single_Multi_Bytes_RW_OneReg},
    {" 9: API Single/Multi Byte RW TwoReg", Test_API_UI2C_Single_Multi_Bytes_RW_TwoReg},
    CU_TEST_INFO_NULL
};

CU_TestInfo UI2C_MacroTests[] = {
    {" 1: MACRO UI2C_Get_TimeOutFlag", Test_Macro_UI2C_Timeout},
    {" 2: MACRO UI2C_Enable_10BIT_ADDR", Test_Macro_UI2C_Enable_10Bit_Addr_Mode},
    {" 3: MACRO UI2C_Control_Read_STATUS", Test_MACRO_UI2C_Control_Read_Status},
    {" 4: MACRO UI2C_Enable_Protocol_INT", Test_MACRO_UI2C_Enable_Protocol_INT},
    CU_TEST_INFO_NULL
};

#if defined(POWER_DOWN_TEST)

CU_TestInfo UI2C_WakeupAPIMacroTests[] = {
    {" 1: API_MACRO UI2C_Wakeup", Test_API_MACRO_UI2C_Wakeup},
    CU_TEST_INFO_NULL
};


#endif // #if defined(POWER_DOWN_TEST)
