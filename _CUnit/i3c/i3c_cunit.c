
/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "i3c_cunit.h"

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


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

uint8_t i2c_buffer[0x100] __ALIGNED(4);
uint8_t i3c_buffer[0x100] __ALIGNED(4);
uint8_t tmp_buffer[0x100] __ALIGNED(4);

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint8_t g_u8SlvDataLen;
volatile uint32_t slave_buff_addr;
volatile uint8_t g_au8SlvData[256];
volatile uint8_t g_au8SlvRxData[3];

typedef void (*I2C_FUNC)(uint32_t u32Status);

volatile static I2C_FUNC s_I2C0HandlerFn = NULL;


void I2C0_IRQHandler(void)
{
    uint32_t u32Status;
    u32Status = I2C_GET_STATUS(I2C0);

    if (I2C_GET_TIMEOUT_FLAG(I2C0)) {
        /* Clear I2C0 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C0);
    } else {
        if (s_I2C0HandlerFn != NULL) {
            s_I2C0HandlerFn(u32Status);
        }
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C TRx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_SlaveTRx(uint32_t u32Status)
{
    uint8_t u8Data;
    printf("Status 0x%x @  I2C_SlaveTRx\n", u32Status);

    if (u32Status == 0x60) {                    /* Own SLA+W has been receive; ACK has been return */
        g_u8SlvDataLen = 0;
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    } else if (u32Status == 0x80)                 /* Previously address with own SLA address
                                                   Data has been received; ACK has been returned*/
    {
        u8Data = (unsigned char) I2C_GET_DATA(I2C0);

        if (g_u8SlvDataLen < 2) {
            g_au8SlvRxData[g_u8SlvDataLen++] = u8Data;
            slave_buff_addr = (uint32_t)(g_au8SlvRxData[0] << 8) + g_au8SlvRxData[1];
        } else {
            g_au8SlvData[slave_buff_addr++] = u8Data;

            if (slave_buff_addr == 256) {
                slave_buff_addr = 0;
            }
        }

        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    } else if (u32Status == 0xA8) {             /* Own SLA+R has been receive; ACK has been return */
        I2C_SET_DATA(I2C0, g_au8SlvData[slave_buff_addr]);
        slave_buff_addr++;
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    } else if (u32Status == 0xB8) {             /* Data byte in I2CDAT has been transmitted ACK has been received */
        I2C_SET_DATA(I2C0, g_au8SlvData[slave_buff_addr++]);
        I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI_AA);
    } else if (u32Status == 0xC0)                 /* Data byte or last data in I2CDAT has been transmitted
                                                   Not ACK has been received */
    {
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
    } else if (u32Status == 0xF8) { /*I2C wave keeps going*/
    } else {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
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
    {"1. I3C MACRO", suite_success_init, suite_success_clean, NULL, NULL, I3C_MacroTests},
    {"2. I3C API", suite_success_init, suite_success_clean, NULL, NULL, I3C_ApiTests},
    CU_SUITE_INFO_NULL
};

void I3C_Reset(void)
{
    SYS_UnlockReg();
    SYS_ResetModule(SYS_I3C0RST);
    SYS_ResetModule(SYS_I2C0RST);
}

/*
    I3C_SET_SCLOD_HCNT / I3C_SET_SCLOD_LCNT
    I3C_SET_SCLPP_HCNT / I3C_SET_SCLPP_LCNT
    I3C_SET_SCLFM_HCNT / I3C_SET_SCLFM_LCNT
    I3C_SET_SCLFMP_HCNT / I3C_SET_SCLFMP_LCNT
    I3C_IS_ENABLE_DMA
*/
void Test_MACRO_I3C()
{
    uint32_t u32RegVal;
    I3C_Open(I3C0, I3C_MASTER, 0, 0);
    CU_ASSERT_EQUAL(I3C0->DEVCTLE & I3C_DEVCTLE_OPERMODE_Msk, I3C_MASTER);
    CU_ASSERT_EQUAL(I3C0->DEVCTL & I3C_DEVCTL_ENABLE_Msk, I3C_DEVCTL_ENABLE_Msk);
    /*
        Set I3C Open Drain High Count
    */
    u32RegVal = I3C0->SCLOD;
    I3C_SET_SCLOD_HCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLOD & I3C_SCLOD_ODHCNT_Msk) >> I3C_SCLOD_ODHCNT_Pos), 0x80);
    I3C_SET_SCLOD_HCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLOD & I3C_SCLOD_ODHCNT_Msk) >> I3C_SCLOD_ODHCNT_Pos), 0xFF);
    CU_ASSERT_EQUAL((I3C0->SCLOD & ~I3C_SCLOD_ODHCNT_Msk), u32RegVal & ~I3C_SCLOD_ODHCNT_Msk);
    /*
        Set I3C Open Drain Low Count
    */
    u32RegVal = I3C0->SCLOD;
    I3C_SET_SCLOD_LCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLOD & I3C_SCLOD_ODLCNT_Msk) >> I3C_SCLOD_ODLCNT_Pos), 0x80);
    I3C_SET_SCLOD_LCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLOD & I3C_SCLOD_ODLCNT_Msk) >> I3C_SCLOD_ODLCNT_Pos), 0xFF);
    CU_ASSERT_EQUAL((I3C0->SCLOD & ~I3C_SCLOD_ODLCNT_Msk), u32RegVal & ~I3C_SCLOD_ODLCNT_Msk);
    /*
        Set I3C Push Pull High Count
    */
    u32RegVal = I3C0->SCLPP;
    I3C_SET_SCLPP_HCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLPP & I3C_SCLPP_PPHCNT_Msk) >> I3C_SCLPP_PPHCNT_Pos), 0x80);
    I3C_SET_SCLPP_HCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLPP & I3C_SCLPP_PPHCNT_Msk) >> I3C_SCLPP_PPHCNT_Pos), 0xFF);
    CU_ASSERT_EQUAL((I3C0->SCLPP & ~I3C_SCLPP_PPHCNT_Msk), u32RegVal & ~I3C_SCLPP_PPHCNT_Msk);
    /*
        Set I3C Push Pull Low Count
    */
    u32RegVal = I3C0->SCLPP;
    I3C_SET_SCLPP_LCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLPP & I3C_SCLPP_PPLCNT_Msk) >> I3C_SCLPP_PPLCNT_Pos), 0x80);
    I3C_SET_SCLPP_LCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLPP & I3C_SCLPP_PPLCNT_Msk) >> I3C_SCLPP_PPLCNT_Pos), 0xFF);
    CU_ASSERT_EQUAL((I3C0->SCLPP & ~I3C_SCLPP_PPLCNT_Msk), u32RegVal & ~I3C_SCLPP_PPLCNT_Msk);
    /*
        Set I2C Fast Mode Open Drain High Count
    */
    u32RegVal = I3C0->SCLFM;
    I3C_SET_SCLFM_HCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMHCNT_Msk) >> I3C_SCLFM_FMHCNT_Pos), 0x80);
    I3C_SET_SCLFM_HCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMHCNT_Msk) >> I3C_SCLFM_FMHCNT_Pos), 0xFF);
    I3C_SET_SCLFM_HCNT(I3C0, 0x800);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMHCNT_Msk) >> I3C_SCLFM_FMHCNT_Pos), 0x800);
    I3C_SET_SCLFM_HCNT(I3C0, 0xFFFF);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMHCNT_Msk) >> I3C_SCLFM_FMHCNT_Pos), 0xFFFF);
    CU_ASSERT_EQUAL((I3C0->SCLFM & ~I3C_SCLFM_FMHCNT_Msk), u32RegVal & ~I3C_SCLFM_FMHCNT_Msk);
    /*
        Set I2C Fast Mode Open Drain Low Count
    */
    u32RegVal = I3C0->SCLFM;
    I3C_SET_SCLFM_LCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMLCNT_Msk) >> I3C_SCLFM_FMLCNT_Pos), 0x80);
    I3C_SET_SCLFM_LCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMLCNT_Msk) >> I3C_SCLFM_FMLCNT_Pos), 0xFF);
    I3C_SET_SCLFM_LCNT(I3C0, 0x800);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMLCNT_Msk) >> I3C_SCLFM_FMLCNT_Pos), 0x800);
    I3C_SET_SCLFM_LCNT(I3C0, 0xFFFF);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMLCNT_Msk) >> I3C_SCLFM_FMLCNT_Pos), 0xFFFF);
    CU_ASSERT_EQUAL((I3C0->SCLFM & ~I3C_SCLFM_FMLCNT_Msk), u32RegVal & ~I3C_SCLFM_FMLCNT_Msk);
    /*
        Set I2C Fast Mode Plus Open Drain High Count
    */
    u32RegVal = I3C0->SCLFMP;
    I3C_SET_SCLFMP_HCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLFMP & I3C_SCLFMP_FMPHCNT_Msk) >> I3C_SCLFMP_FMPHCNT_Pos), 0x80);
    I3C_SET_SCLFMP_HCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLFMP & I3C_SCLFMP_FMPHCNT_Msk) >> I3C_SCLFMP_FMPHCNT_Pos), 0xFF);
    CU_ASSERT_EQUAL((I3C0->SCLFMP & ~I3C_SCLFMP_FMPHCNT_Msk), u32RegVal & ~I3C_SCLFMP_FMPHCNT_Msk);
    /*
        Set I2C Fast Mode Plus Open Drain Low Count
    */
    u32RegVal = I3C0->SCLFMP;
    I3C_SET_SCLFMP_LCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLFMP & I3C_SCLFMP_FMPLCNT_Msk) >> I3C_SCLFMP_FMPLCNT_Pos), 0x80);
    I3C_SET_SCLFMP_LCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLFMP & I3C_SCLFMP_FMPLCNT_Msk) >> I3C_SCLFMP_FMPLCNT_Pos), 0xFF);
    I3C_SET_SCLFMP_LCNT(I3C0, 0x800);
    CU_ASSERT_EQUAL(((I3C0->SCLFMP & I3C_SCLFMP_FMPLCNT_Msk) >> I3C_SCLFMP_FMPLCNT_Pos), 0x800);
    I3C_SET_SCLFMP_LCNT(I3C0, 0xFFFF);
    CU_ASSERT_EQUAL(((I3C0->SCLFMP & I3C_SCLFMP_FMPLCNT_Msk) >> I3C_SCLFMP_FMPLCNT_Pos), 0xFFFF);
    CU_ASSERT_EQUAL((I3C0->SCLFMP & ~I3C_SCLFMP_FMPLCNT_Msk), u32RegVal & ~I3C_SCLFMP_FMPLCNT_Msk);
    //
    I3C_EnableDMA(I3C0);
    CU_ASSERT_EQUAL(I3C_IS_ENABLE_DMA(I3C0), 1);
    I3C_DisableDMA(I3C0);
    CU_ASSERT_EQUAL(I3C_IS_ENABLE_DMA(I3C0), 0);
    //
    CU_ASSERT_EQUAL(I3C_Disable(I3C0), I3C_STS_NO_ERR);
}

/*
    I3C_Open
    I3C_SetDeviceAddr
    I3C_Write / I3C_Read
*/

#if 0
void Test_API_I3C_RW_I2CEEPROM()
{
    uint32_t i, j;
    int32_t ret;
    uint8_t u8DevIndex = 0;
    uint32_t u8SlaveAddr = 0x16;
    /*
    */
    I3C_Reset();
    /* Open I2C0 and set clock to 100k */
    I2C_Open(I2C0, 100000);
    I2C_SetSlaveAddr(I2C0, 0, 0x50, I2C_GCMODE_DISABLE);   /* Slave Address : 0x15 */
    I2C_EnableInt(I2C0);
    NVIC_EnableIRQ(I2C0_IRQn);
    /* I2C function to Transmit/Receive data as slave */
    s_I2C0HandlerFn = I2C_SlaveTRx;
    /*
        Set I3C Open Drain High Count
    */
    //
    I3C_Open(I3C0, I3C_MASTER, 0, 0);
    CU_ASSERT_EQUAL(I3C0->DEVCTLE & I3C_DEVCTLE_OPERMODE_Msk, I3C_MASTER);
    CU_ASSERT_EQUAL(I3C0->DEVCTL & I3C_DEVCTL_ENABLE_Msk, I3C_DEVCTL_ENABLE_Msk);

    for (i = 0; i < 0x100; i++) {
        i3c_buffer[i] = i;
    }

    memset(i2c_buffer, 0, 0x100);
    // Setup Device Address Location
    // I2C Slave (0x68)
    I3C_SetDeviceAddr(I3C0, 0, I3C_DEVTYPE_I2C, 0x00, 0x68);
    CU_ASSERT_EQUAL((I3C0->DEV1ADR & I3C_DEVADR_DEVICE_Msk), I3C_DEVADR_DEVICE_Msk);
    CU_ASSERT_EQUAL((I3C0->DEV1ADR & I3C_DEVADR_SADDR_Msk), 0x68 << I3C_DEVADR_SADDR_Pos);
    // I3C Slave (0x39)
    I3C_SetDeviceAddr(I3C0, 1, I3C_DEVTYPE_I3C, 0x39, 0x68);
    CU_ASSERT_EQUAL((I3C0->DEV2ADR & I3C_DEVADR_DEVICE_Msk), 0);
    CU_ASSERT_EQUAL((I3C0->DEV2ADR & I3C_DEVADR_SADDR_Msk), 0x68 << I3C_DEVADR_SADDR_Pos);
    CU_ASSERT_EQUAL((I3C0->DEV2ADR & I3C_DEVADR_DADDR_Msk), (0x39 | 0x80) << I3C_DEVADR_DADDR_Pos);
    // I2C EEPROM (0x50)
    I3C_SetDeviceAddr(I3C0, 2, I3C_DEVTYPE_I2C, 0x00, 0x50);
    CU_ASSERT_EQUAL((I3C0->DEV3ADR & I3C_DEVADR_DEVICE_Msk), I3C_DEVADR_DEVICE_Msk);
    CU_ASSERT_EQUAL((I3C0->DEV3ADR & I3C_DEVADR_SADDR_Msk), 0x50 << I3C_DEVADR_SADDR_Pos);
    CU_ASSERT_EQUAL((I3C0->DEV3ADR & I3C_DEVADR_DADDR_Msk), (0x00 | 0x80) << I3C_DEVADR_DADDR_Pos);
    // I2C Slave (0x15)
    I3C_SetDeviceAddr(I3C0, 3, I3C_DEVTYPE_I2C, 0x00, 0x15);
    CU_ASSERT_EQUAL((I3C0->DEV4ADR & I3C_DEVADR_DEVICE_Msk), I3C_DEVADR_DEVICE_Msk);
    CU_ASSERT_EQUAL((I3C0->DEV4ADR & I3C_DEVADR_SADDR_Msk), 0x15 << I3C_DEVADR_SADDR_Pos);
    //
    // eeprom offset: 0x0000
    i3c_buffer[0] = 0;
    i3c_buffer[1] = 0;
    printf("I3C0->SCLFM = 0x%08X\n", I3C0->SCLFM);
    I3C0->SCLFM = 0xFFFFFFFF;
    printf("I3C0->SCLFM = 0x%08X\n", I3C0->SCLFM);
    ret = I3C_Write(I3C0, 2, I3C_DEVI2C_SPEED_I2CFM, (uint32_t *)i3c_buffer, 16 + 2);
    CU_ASSERT_EQUAL(ret, I3C_STS_NO_ERR);
    // another write to reset offset @ eeprom
    ret = I3C_Write(I3C0, 2, I3C_DEVI2C_SPEED_I2CFM, (uint32_t *)i3c_buffer, 1 + 2);
    CU_ASSERT_EQUAL(ret, I3C_STS_NO_ERR);
    ret = I3C_Read(I3C0, 2, I3C_DEVI2C_SPEED_I2CFM, (uint32_t *)tmp_buffer, 16);
    CU_ASSERT_EQUAL(ret, I3C_STS_NO_ERR);

    for (i = 0; i < 16; i++) {
        CU_ASSERT_EQUAL(tmp_buffer[i], i3c_buffer[i + 2]);
    }

    return;
}

#else
volatile uint32_t   g_RxBuf[I3C_DEVICE_RX_BUF_CNT], g_TxBuf[I3C_DEVICE_RX_BUF_CNT];

void Test_API_I3C_RW_SLAVE()
{
    uint8_t *pu8Data;
    uint32_t i, j;
    int32_t ret;
    uint8_t u8DevIndex = 0;
    uint32_t u8SlaveAddr = 0x16;
    /*
    */
    I3C_Reset();
    I3C_Open(I3C0, I3C_MASTER, 0, 0);
    I3C_SetDeviceAddr(I3C0, 0, I3C_DEVTYPE_I3C, 0x18, 0x00);

    while (1) {
        printf("\npress any key to broadcast ENTDAA\n");
        getchar();

        if (1 == I3C_BroadcastENTDAA(I3C0, 1)) {
            printf("I3C Device found:\n");
            printf(" - Provisional ID = 0x%08X%02X\n", I3C0->DEV1CH[0], I3C0->DEV1CH[1]);
            printf(" - BCR, DCR = 0x%08X\n", I3C0->DEV1CH[2]);
            printf(" - DADR = 0x%08X\n\n", I3C0->DEV1CH[3]);
            break;
        }
    }

    pu8Data = (uint8_t *)g_TxBuf;

    for (i = 0; i < 16; i++) {
        pu8Data[i] = i;
    }

    if (1) {
        printf("press any key to Write I3C Target \n");
        getchar();
        ret = I3C_Write(I3C0, 0, I3C_DEVI3C_SPEED_SDR0, (uint32_t *)g_TxBuf, 16);

        if (ret != I3C_STS_NO_ERR) {
            printf("I3C_Write Fail\n");

            while (1);
        }

        printf("press any key to Read I3C Target \n");
        getchar();
        ret = I3C_Read(I3C0, 0, I3C_DEVI3C_SPEED_SDR0, (uint32_t *)g_RxBuf, 16);

        if (ret != I3C_STS_NO_ERR) {
            printf("I3C_Read Fail\n");

            while (1);
        }

        for (i = 0; i < 4; i++) {
            CU_ASSERT_EQUAL(g_RxBuf[i], g_TxBuf[i]);
        }
    }

    return;
}


#endif

CU_TestInfo I3C_ApiTests[] = {
    // An I2C EEPROM (Address: 0x50) is required to test this item.
    // {" 1: API, I3C Read Write EEPROM.", Test_API_I3C_RW_I2CEEPROM},
    {" 1: API, I3C Read Write SLAVE.", Test_API_I3C_RW_SLAVE},

    CU_TEST_INFO_NULL
};

CU_TestInfo I3C_MacroTests[] = {
    {" 1: MACRO, I3C Master Timing Config", Test_MACRO_I3C},
    CU_TEST_INFO_NULL
};

