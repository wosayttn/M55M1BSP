
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

CU_SuiteInfo suites[] =
{
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
    I3C_SET_SCL_OD_HCNT / I3C_SET_SCL_OD_LCNT
    I3C_SET_SCL_PP_HCNT / I3C_SET_SCL_PP_LCNT
    I3C_SET_SCL_FM_HCNT / I3C_SET_SCL_FM_LCNT
    I3C_SET_SCL_FMP_HCNT / I3C_SET_SCL_FMP_LCNT
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
    I3C_SET_SCL_OD_HCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLOD & I3C_SCLOD_ODHCNT_Msk) >> I3C_SCLOD_ODHCNT_Pos), 0x80);
    I3C_SET_SCL_OD_HCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLOD & I3C_SCLOD_ODHCNT_Msk) >> I3C_SCLOD_ODHCNT_Pos), 0xFF);
    CU_ASSERT_EQUAL((I3C0->SCLOD & ~I3C_SCLOD_ODHCNT_Msk), u32RegVal & ~I3C_SCLOD_ODHCNT_Msk);
    /*
        Set I3C Open Drain Low Count
    */
    u32RegVal = I3C0->SCLOD;
    I3C_SET_SCL_OD_LCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLOD & I3C_SCLOD_ODLCNT_Msk) >> I3C_SCLOD_ODLCNT_Pos), 0x80);
    I3C_SET_SCL_OD_LCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLOD & I3C_SCLOD_ODLCNT_Msk) >> I3C_SCLOD_ODLCNT_Pos), 0xFF);
    CU_ASSERT_EQUAL((I3C0->SCLOD & ~I3C_SCLOD_ODLCNT_Msk), u32RegVal & ~I3C_SCLOD_ODLCNT_Msk);
    /*
        Set I3C Push Pull High Count
    */
    u32RegVal = I3C0->SCLPP;
    I3C_SET_SCL_PP_HCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLPP & I3C_SCLPP_PPHCNT_Msk) >> I3C_SCLPP_PPHCNT_Pos), 0x80);
    I3C_SET_SCL_PP_HCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLPP & I3C_SCLPP_PPHCNT_Msk) >> I3C_SCLPP_PPHCNT_Pos), 0xFF);
    CU_ASSERT_EQUAL((I3C0->SCLPP & ~I3C_SCLPP_PPHCNT_Msk), u32RegVal & ~I3C_SCLPP_PPHCNT_Msk);
    /*
        Set I3C Push Pull Low Count
    */
    u32RegVal = I3C0->SCLPP;
    I3C_SET_SCL_PP_LCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLPP & I3C_SCLPP_PPLCNT_Msk) >> I3C_SCLPP_PPLCNT_Pos), 0x80);
    I3C_SET_SCL_PP_LCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLPP & I3C_SCLPP_PPLCNT_Msk) >> I3C_SCLPP_PPLCNT_Pos), 0xFF);
    CU_ASSERT_EQUAL((I3C0->SCLPP & ~I3C_SCLPP_PPLCNT_Msk), u32RegVal & ~I3C_SCLPP_PPLCNT_Msk);
    /*
        Set I2C Fast Mode Open Drain High Count
    */
    u32RegVal = I3C0->SCLFM;
    I3C_SET_SCL_FM_HCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMHCNT_Msk) >> I3C_SCLFM_FMHCNT_Pos), 0x80);
    I3C_SET_SCL_FM_HCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMHCNT_Msk) >> I3C_SCLFM_FMHCNT_Pos), 0xFF);
    I3C_SET_SCL_FM_HCNT(I3C0, 0x800);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMHCNT_Msk) >> I3C_SCLFM_FMHCNT_Pos), 0x800);
    I3C_SET_SCL_FM_HCNT(I3C0, 0xFFFF);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMHCNT_Msk) >> I3C_SCLFM_FMHCNT_Pos), 0xFFFF);
    CU_ASSERT_EQUAL((I3C0->SCLFM & ~I3C_SCLFM_FMHCNT_Msk), u32RegVal & ~I3C_SCLFM_FMHCNT_Msk);
    /*
        Set I2C Fast Mode Open Drain Low Count
    */
    u32RegVal = I3C0->SCLFM;
    I3C_SET_SCL_FM_LCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMLCNT_Msk) >> I3C_SCLFM_FMLCNT_Pos), 0x80);
    I3C_SET_SCL_FM_LCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMLCNT_Msk) >> I3C_SCLFM_FMLCNT_Pos), 0xFF);
    I3C_SET_SCL_FM_LCNT(I3C0, 0x800);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMLCNT_Msk) >> I3C_SCLFM_FMLCNT_Pos), 0x800);
    I3C_SET_SCL_FM_LCNT(I3C0, 0xFFFF);
    CU_ASSERT_EQUAL(((I3C0->SCLFM & I3C_SCLFM_FMLCNT_Msk) >> I3C_SCLFM_FMLCNT_Pos), 0xFFFF);
    CU_ASSERT_EQUAL((I3C0->SCLFM & ~I3C_SCLFM_FMLCNT_Msk), u32RegVal & ~I3C_SCLFM_FMLCNT_Msk);
    /*
        Set I2C Fast Mode Plus Open Drain High Count
    */
    u32RegVal = I3C0->SCLFMP;
    I3C_SET_SCL_FMP_HCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLFMP & I3C_SCLFMP_FMPHCNT_Msk) >> I3C_SCLFMP_FMPHCNT_Pos), 0x80);
    I3C_SET_SCL_FMP_HCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLFMP & I3C_SCLFMP_FMPHCNT_Msk) >> I3C_SCLFMP_FMPHCNT_Pos), 0xFF);
    CU_ASSERT_EQUAL((I3C0->SCLFMP & ~I3C_SCLFMP_FMPHCNT_Msk), u32RegVal & ~I3C_SCLFMP_FMPHCNT_Msk);
    /*
        Set I2C Fast Mode Plus Open Drain Low Count
    */
    u32RegVal = I3C0->SCLFMP;
    I3C_SET_SCL_FMP_LCNT(I3C0, 0x80);
    CU_ASSERT_EQUAL(((I3C0->SCLFMP & I3C_SCLFMP_FMPLCNT_Msk) >> I3C_SCLFMP_FMPLCNT_Pos), 0x80);
    I3C_SET_SCL_FMP_LCNT(I3C0, 0xFF);
    CU_ASSERT_EQUAL(((I3C0->SCLFMP & I3C_SCLFMP_FMPLCNT_Msk) >> I3C_SCLFMP_FMPLCNT_Pos), 0xFF);
    I3C_SET_SCL_FMP_LCNT(I3C0, 0x800);
    CU_ASSERT_EQUAL(((I3C0->SCLFMP & I3C_SCLFMP_FMPLCNT_Msk) >> I3C_SCLFMP_FMPLCNT_Pos), 0x800);
    I3C_SET_SCL_FMP_LCNT(I3C0, 0xFFFF);
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

void Test_API_I3C_RW_I2CEEPROM()
{
    uint32_t i, j;
    int32_t ret;
    uint8_t u8DevIndex = 0;
    uint32_t u8SlaveAddr = 0x16;
    I3C_Reset();
    /*
        Set I3C Open Drain High Count
    */
    //
    I3C_Open(I3C0, I3C_MASTER, 0, 0);
    CU_ASSERT_EQUAL(I3C0->DEVCTLE & I3C_DEVCTLE_OPERMODE_Msk, I3C_MASTER);
    CU_ASSERT_EQUAL(I3C0->DEVCTL & I3C_DEVCTL_ENABLE_Msk, I3C_DEVCTL_ENABLE_Msk);

    for (i = 0; i < 0x100; i++)
    {
        i3c_buffer[i] = i;
    }

    memset(i2c_buffer, 0, 0x100);
    // Setup Device Address Location
    // I2C Slave (0x68)
    I3C_SetDeviceAddr(I3C0, 0, 1, 0x00, 0x68);
    CU_ASSERT_EQUAL((I3C0->DEV1ADR & I3C_DEVADR_DEVICE_Msk), I3C_DEVADR_DEVICE_Msk);
    CU_ASSERT_EQUAL((I3C0->DEV1ADR & I3C_DEVADR_SADDR_Msk), 0x68 << I3C_DEVADR_SADDR_Pos);
    // I3C Slave (0x39)
    I3C_SetDeviceAddr(I3C0, 1, 0, 0x39, 0x68);
    CU_ASSERT_EQUAL((I3C0->DEV2ADR & I3C_DEVADR_DEVICE_Msk), 0);
    CU_ASSERT_EQUAL((I3C0->DEV2ADR & I3C_DEVADR_SADDR_Msk), 0x68 << I3C_DEVADR_SADDR_Pos);
    CU_ASSERT_EQUAL((I3C0->DEV2ADR & I3C_DEVADR_DADDR_Msk), (0x39 | 0x80) << I3C_DEVADR_DADDR_Pos);
    // I2C EEPROM (0x50)
    I3C_SetDeviceAddr(I3C0, 2, 1, 0x00, 0x50);
    CU_ASSERT_EQUAL((I3C0->DEV3ADR & I3C_DEVADR_DEVICE_Msk), I3C_DEVADR_DEVICE_Msk);
    CU_ASSERT_EQUAL((I3C0->DEV3ADR & I3C_DEVADR_SADDR_Msk), 0x50 << I3C_DEVADR_SADDR_Pos);
    CU_ASSERT_EQUAL((I3C0->DEV3ADR & I3C_DEVADR_DADDR_Msk), (0x00 | 0x80) << I3C_DEVADR_DADDR_Pos);
    // I2C Slave (0x15)
    I3C_SetDeviceAddr(I3C0, 3, 1, 0x00, 0x15);
    CU_ASSERT_EQUAL((I3C0->DEV4ADR & I3C_DEVADR_DEVICE_Msk), I3C_DEVADR_DEVICE_Msk);
    CU_ASSERT_EQUAL((I3C0->DEV4ADR & I3C_DEVADR_SADDR_Msk), 0x15 << I3C_DEVADR_SADDR_Pos);
    //
    // eeprom offset: 0x0000
    i3c_buffer[0] = 0;
    i3c_buffer[1] = 0;
    ret = I3C_Write(I3C0, 2, I2C_SPEED_I2CFM, 0, (uint32_t *)i3c_buffer, 16 + 2);
    CU_ASSERT_EQUAL(ret, I3C_STS_NO_ERR);
    // another write to reset offset @ eeprom
    ret = I3C_Write(I3C0, 2, I2C_SPEED_I2CFM, 0, (uint32_t *)i3c_buffer, 1 + 2);
    CU_ASSERT_EQUAL(ret, I3C_STS_NO_ERR);
    ret = I3C_Read(I3C0, 2, I2C_SPEED_I2CFM, 0, (uint32_t *)tmp_buffer, 16);
    CU_ASSERT_EQUAL(ret, I3C_STS_NO_ERR);

    for (i = 0; i < 16; i++)
    {
        CU_ASSERT_EQUAL(tmp_buffer[i], i3c_buffer[i + 2]);
    }

    return;
}

CU_TestInfo I3C_ApiTests[] =
{
    // An I2C EEPROM (Address: 0x50) is required to test this item.
    {" 1: API, I3C Read Write EEPROM.", Test_API_I3C_RW_I2CEEPROM},
    CU_TEST_INFO_NULL
};

CU_TestInfo I3C_MacroTests[] =
{
    {" 1: MACRO, I3C Master Timing Config", Test_MACRO_I3C},
    CU_TEST_INFO_NULL
};

