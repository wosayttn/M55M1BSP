/******************************************************************************
* @file    fmc_cunit.c
* @version V1.00
* @brief   FMC CUnit Test
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "fmc_cunit.h"

uint32_t g_au32TestDataBuf[32];

/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int FMC_Tests_Init(void)
{
    return 0;
}

int FMC_Tests_Clean(void)
{
    return 0;
}

void MACRO_FMC_ENABLE_ISP()
{
    FMC_ENABLE_ISP();
    CU_ASSERT_TRUE(FMC->ISPCTL & FMC_ISPCTL_ISPEN_Msk);

    FMC_DISABLE_ISP();
    CU_ASSERT_FALSE(FMC->ISPCTL & FMC_ISPCTL_ISPEN_Msk);

    FMC_ENABLE_ISP();
}

void MACRO_FMC_ENABLE_LD_UPDATE()
{
    FMC_ENABLE_ISP();

    FMC_ENABLE_LD_UPDATE();
    CU_ASSERT_TRUE(FMC->ISPCTL & FMC_ISPCTL_LDUEN_Msk);

    FMC_DISABLE_LD_UPDATE();
    CU_ASSERT_FALSE(FMC->ISPCTL & FMC_ISPCTL_LDUEN_Msk);
}

void MACRO_FMC_ENABLE_CFG_UPDATE()
{
    FMC_ENABLE_ISP();

    FMC_ENABLE_CFG_UPDATE();
    CU_ASSERT_TRUE(FMC->ISPCTL & FMC_ISPCTL_CFGUEN_Msk);

    FMC_DISABLE_CFG_UPDATE();
    CU_ASSERT_FALSE(FMC->ISPCTL & FMC_ISPCTL_CFGUEN_Msk);
}

void MACRO_FMC_ENABLE_AP_UPDATE()
{
    FMC_ENABLE_ISP();

    FMC_ENABLE_AP_UPDATE();
    CU_ASSERT_TRUE(FMC->ISPCTL & FMC_ISPCTL_APUEN_Msk);

    FMC_DISABLE_AP_UPDATE();
    CU_ASSERT_FALSE(FMC->ISPCTL & FMC_ISPCTL_APUEN_Msk);
}

void MACRO_FMC_GET_FAIL_FLAG()
{
    FMC_ENABLE_ISP();
    FMC_DISABLE_CFG_UPDATE();

    FMC_Read(FMC_APROM_BASE);
    CU_ASSERT(FMC_GET_FAIL_FLAG() == 0);

    FMC_Write(FMC_CONFIG_BASE, 0x01);
    CU_ASSERT(FMC_GET_FAIL_FLAG() != 0);

    FMC_CLR_FAIL_FLAG();
    CU_ASSERT(FMC_GET_FAIL_FLAG() == 0);
}

void MACRO_FMC_CLR_FAIL_FLAG()
{
    FMC_ENABLE_ISP();
    FMC_DISABLE_CFG_UPDATE();

    FMC_Write(FMC_CONFIG_BASE, 0x01);
    CU_ASSERT(FMC_GET_FAIL_FLAG() != 0);

    FMC_CLR_FAIL_FLAG();
    CU_ASSERT(FMC_GET_FAIL_FLAG() == 0);
}

void MACRO_FMC_SET_APROM_BOOT()
{
    FMC_ENABLE_ISP();

    FMC_SET_LDROM_BOOT();
    CU_ASSERT((FMC->ISPCTL & FMC_ISPCTL_BS_Msk) == FMC_ISPCTL_BS_Msk);

    FMC_SET_APROM_BOOT();
    CU_ASSERT((FMC->ISPCTL & FMC_ISPCTL_BS_Msk) == 0);
}

void MACRO_FMC_SET_APWPROT(void)
{
    int32_t i, i32Bank;

    for (i32Bank = 0; i32Bank < 2; i32Bank++)
    {
        if (FMC_IS_APWPROT_LCOKED(i32Bank) == TRUE)
        {
            printf("APWPROT%d is locked. Skip Bank %d test\n", i32Bank, i32Bank);
            continue;
        }

        for (i = 0; i < 32; i++)
        {
            FMC_ENABLE_APWPROT(i32Bank, (1 << i));
            CU_ASSERT((FMC->APWPROT[i32Bank] & (1 << i)) == (1 << i));
        }

        for (i = 0; i < 32; i++)
        {
            FMC_DISABLE_APWPROT(i32Bank, (1 << i));
            CU_ASSERT((FMC->APWPROT[i32Bank] & (1 << i)) == 0);
        }

        CU_ASSERT(FMC_IS_APWPROT_LCOKED(i32Bank) == FALSE);
        FMC_LOCK_APWPROT(i32Bank);
        CU_ASSERT((FMC->APWPKEEP & (0xFFFF << (i32Bank * 16))) == (0x55AA << (i32Bank * 16)));
        CU_ASSERT(FMC_IS_APWPROT_LCOKED(i32Bank) == TRUE);
    }
}

void API_FMC_Write()
{
    int32_t   i;
    uint32_t  u32Data;

    FMC_ENABLE_ISP();
    FMC_ENABLE_LD_UPDATE();

    /*
     *  FMC_Write() test
     */
    FMC_Erase(FMC_LDROM_BASE);

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += 4)
        FMC_Write(FMC_LDROM_BASE + i, FMC_LDROM_BASE + i);

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += 4)
    {
        u32Data = FMC_Read(FMC_LDROM_BASE + i);

        if (u32Data != FMC_LDROM_BASE + i)
            CU_FAIL();
    }

    /*
     *  FMC_Write8Bytes() test
     */
    FMC_Erase(FMC_LDROM_BASE);

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += 8)
        FMC_Write8Bytes(FMC_LDROM_BASE + i, FMC_LDROM_BASE + i, FMC_LDROM_BASE + i + 4);

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += 4)
    {
        u32Data = FMC_Read(FMC_LDROM_BASE + i);

        if (u32Data != FMC_LDROM_BASE + i)
            CU_FAIL();
    }

    CU_PASS();
    FMC_DISABLE_LD_UPDATE();
}

void API_FMC_WriteMultiple()
{
    FMC_WriteMultiple(TEST_FLASH_ADDR, g_au32TestDataBuf, 256);
}

void API_FMC_OTP()
{
    uint32_t    i, otp_hw, otp_lw;            /* temporary data */

    FMC_ENABLE_ISP();

    for (i = 0; i < FMC_OTP_ENTRY_CNT; i++)
    {
        if (FMC_ReadOTP(i, &otp_lw, &otp_hw) != 0)
        {
            printf("Read OTP%d failed!\n", i);
            CU_FAIL();
        }

        if ((otp_lw == 0xFFFFFFFF) && (otp_hw == 0xFFFFFFFF))
        {
            printf("OTP%d is 0xFFFFFFFF-0xFFFFFFFF. It should be a free entry.\n", i);
            break;
        }
    }

    if (i == FMC_OTP_ENTRY_CNT)
    {
        printf("All OTP entries are used.\n");
        CU_FAIL();
    }

    printf("Program OTP%d with 0x%x-0x%x...\n", i, 0x5A5A0000 | i, 0x00005A5A | i);

    if (FMC_WriteOTP(i, 0x5A5A0000 | i, 0x00005A5A | i) != 0)
    {
        printf("Failed to program OTP%d!\n", i);
        CU_FAIL();
    }

    if (FMC_ReadOTP(i, &otp_lw, &otp_hw) != 0)
    {
        printf("Read OTP%d failed after programmed!\n", i);
        CU_FAIL();
    }

    printf("Read back OTP%d: 0x%x-0x%x.\n", i, otp_lw, otp_hw);

    if ((otp_lw != (0x5A5A0000 | i)) || (otp_hw != (0x00005A5A | i)))
    {
        printf("OTP%d value is not matched with programmed value!\n", i);
        CU_FAIL();
    }

    if (FMC_IsOTPLocked(i))
    {
        CU_FAIL();
    }

    printf("Lock OTP%d...\n", i);

    if (FMC_LockOTP(i) != 0)
    {
        printf("Failed to lock OTP%d!\n", i);
        CU_FAIL();
    }

    if (!FMC_IsOTPLocked(i))
    {
        CU_FAIL();
    }

    if (FMC_ReadOTP(i, &otp_lw, &otp_hw) != 0)
    {
        printf("Read OTP%d failed after programmed!\n", i);
        CU_FAIL();
    }

    printf("Read OTP%d after locked: 0x%x-0x%x.\n", i, otp_lw, otp_hw);

    if ((otp_lw != (0x5A5A0000 | i)) || (otp_hw != (0x00005A5A | i)))
    {
        printf("OTP%d value is incorrect after locked!\n", i);
        CU_FAIL();
    }

    CU_PASS();
}

void API_FMC_Read()
{
    int32_t  i;
    uint32_t u32Data;
    uint32_t u32Data0, u32Data1;

    FMC_ENABLE_ISP();

    i = (FMC_APROM_BASE + 0x1000);
    CU_ASSERT(FMC_Read(i) == M32(i));
    i = (FMC_APROM_BASE + 0x1004);
    CU_ASSERT(FMC_Read(i) == M32(i));
    i = (FMC_APROM_BASE + 0x2000);
    CU_ASSERT(FMC_Read(i) == M32(i));
    i = (FMC_APROM_BASE + 0xFFC);
    CU_ASSERT(FMC_Read(i) == M32(i));

    FMC_Erase(TEST_DATA_FLASH_BASE);

    FMC_ENABLE_AP_UPDATE();

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += 4)
        FMC_Write(TEST_DATA_FLASH_BASE + i, TEST_DATA_FLASH_BASE + i);

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += 4)
    {
        u32Data = FMC_Read(TEST_DATA_FLASH_BASE + i);

        if (u32Data != TEST_DATA_FLASH_BASE + i)
        {
            printf("\nW[0x%X] R[0x%X]", TEST_DATA_FLASH_BASE + i, u32Data);
            CU_FAIL();
            return;
        }
    }

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += 8)
    {
        u32Data = FMC_Read_64(TEST_DATA_FLASH_BASE + i, &u32Data0, &u32Data1);

        if ((u32Data0 != TEST_DATA_FLASH_BASE + i) ||
                (u32Data1 != TEST_DATA_FLASH_BASE + i + 4))
        {
            CU_FAIL();
            return;
        }
    }

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += 4)
    {
        u32Data = M32(TEST_DATA_FLASH_BASE + i);

        if (u32Data != TEST_DATA_FLASH_BASE + i)
        {
            CU_FAIL();
            return;
        }
    }

    CU_PASS();
}


void API_FMC_Erase()
{
    int32_t i;
    uint32_t u32Data, u32AddrStep = 128;//4;

    FMC_ENABLE_ISP();
    FMC_ENABLE_AP_UPDATE();

    /*
     *  FMC_Erase() test
     */
    FMC_Erase(TEST_DATA_FLASH_BASE);

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += u32AddrStep)
        FMC_Write(TEST_DATA_FLASH_BASE + i, TEST_DATA_FLASH_BASE + i);

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += u32AddrStep)
    {
        u32Data = FMC_Read(TEST_DATA_FLASH_BASE + i);

        if (u32Data != TEST_DATA_FLASH_BASE + i)
            CU_FAIL();
    }

    FMC_Erase(TEST_DATA_FLASH_BASE);

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += u32AddrStep)
    {
        u32Data = FMC_Read(TEST_DATA_FLASH_BASE + i);

        if (u32Data != 0xFFFFFFFFul)
            CU_FAIL();
    }

    /*
     *  FMC_Erase_SPROM() test
     */
#if 0   // M55M1 not support
    FMC_ENABLE_SP_UPDATE();
    FMC_Erase_SPROM();

    for (i = 0; i < FMC_SPROM_SIZE; i += u32AddrStep)
        FMC_Write(FMC_SPROM_BASE + i, FMC_SPROM_BASE + i);

    for (i = 0; i < FMC_SPROM_SIZE; i += u32AddrStep)
    {
        u32Data = FMC_Read(FMC_SPROM_BASE + i);

        if (u32Data != FMC_SPROM_BASE + i)
            CU_FAIL();
    }

    FMC_ENABLE_SP_UPDATE();
    FMC_Erase_SPROM();

    for (i = 0; i < FMC_SPROM_SIZE; i += u32AddrStep)
    {
        u32Data = FMC_Read(FMC_SPROM_BASE + i);

        if (u32Data != 0xFFFFFFFFul)
            CU_FAIL();
    }

#endif

    /*
     *  FMC_Erase_Block() test
     */
#if 0   // M55M1 not support
    FMC_Erase_Block(0x20000);

    for (i = 0; i < FMC_FLASH_PAGE_SIZE * 4; i += u32AddrStep)
        FMC_Write(0x20000 + i, 0x20000 + i);

    for (i = 0; i < FMC_FLASH_PAGE_SIZE * 4; i += u32AddrStep)
    {
        u32Data = FMC_Read(0x20000 + i);

        if (u32Data != 0x20000 + i)
            CU_FAIL();
    }

    FMC_Erase_Block(0x20000);

    for (i = 0; i < FMC_FLASH_PAGE_SIZE * 4; i += u32AddrStep)
    {
        u32Data = FMC_Read(0x20000 + i);

        if (u32Data != 0xFFFFFFFFul)
            CU_FAIL();
    }

#endif

    /*
     *  FMC_Erase_Bank() test
     */
    FMC_Erase_Bank(FMC_APROM_BANK1_BASE);

    for (i = 0; i < FMC_APROM_BANK_SIZE; i += u32AddrStep)
        FMC_Write(FMC_APROM_BANK1_BASE + i, FMC_APROM_BANK1_BASE + i);

    for (i = 0; i < FMC_APROM_BANK_SIZE; i += u32AddrStep)
    {
        u32Data = FMC_Read(FMC_APROM_BANK1_BASE + i);

        if (u32Data != FMC_APROM_BANK1_BASE + i)
        {
            printf("[0x%08X] u32Data: 0x%08X\n", (uint32_t)(FMC_APROM_BANK1_BASE + i), u32Data);
            CU_FAIL();
        }
    }

    FMC_Erase_Bank(FMC_APROM_BANK1_BASE);

    for (i = 0; i < FMC_APROM_BANK_SIZE; i += u32AddrStep)
    {
        u32Data = FMC_Read(FMC_APROM_BANK1_BASE + i);

        if (u32Data != 0xFFFFFFFFul)
            CU_FAIL();
    }

    CU_PASS();
    FMC_DISABLE_AP_UPDATE();
}

void API_FMC_GetBootSource(void)
{
    FMC_ENABLE_ISP();

    if (FMC_GetBootSource() == 0)
    {
        CU_ASSERT((FMC->ISPCTL & FMC_ISPCTL_BS_Msk) == 0);
    }
    else
    {
        if (FMC_GetBootSource() == 1)
        {
            CU_ASSERT((FMC->ISPCTL & FMC_ISPCTL_BS_Msk) == FMC_ISPCTL_BS_Msk);
        }
        else
        {
            CU_FAIL();
        }
    }
}

void API_FMC_ReadUID(void)
{
    FMC_ENABLE_ISP();

    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadUID(0) != 0);
    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadUID(1) != 0);
    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadUID(2) != 0);

    CU_ASSERT(FMC_ReadUID(0) != FMC_ReadUID(1));
    CU_ASSERT(FMC_ReadUID(1) != FMC_ReadUID(2));
}

void API_FMC_ReadCID(void)
{
    FMC_ENABLE_ISP();
    CU_ASSERT(FMC_ReadCID() == 0x530000DA);
}

void API_FMC_ReadPID(void)
{
    FMC_ENABLE_ISP();

    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadPID() != 0);

    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadPID() != 0xFFFFFFFFul);
}


void API_FMC_ReadUCID(void)
{
    FMC_ENABLE_ISP();

    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadUCID(0) != 0);
    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadUCID(1) != 0);
    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadUCID(2) != 0);
    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadUCID(3) != 0);
}


void API_FMC_SetVectorPageAddr(void)
{
    uint32_t   i;

    FMC_ENABLE_ISP();
    FMC_ENABLE_AP_UPDATE();

    FMC_Erase(TEST_VECMAP_ADDR);

    for (i = 0; i < FMC_VECMAP_SIZE; i += 4)
        FMC_Write(TEST_VECMAP_ADDR + i, TEST_VECMAP_ADDR + i);

    FMC_SetVectorPageAddr(TEST_VECMAP_ADDR);

    for (i = 0; i < FMC_VECMAP_SIZE; i += 4)
    {
        if (FMC_Read(TEST_VECMAP_ADDR + i) != M32(FMC_APROM_BASE + i))
        {
            printf("Test VECMAP failed at 0x%x => 0x%x, 0x%x\n", TEST_VECMAP_ADDR + i, FMC_Read(TEST_VECMAP_ADDR + i), M32(FMC_APROM_BASE + i));
            CU_FAIL();
            break;
        }
    }

    FMC_SetVectorPageAddr(FMC_APROM_BASE);

    for (i = 0; i < FMC_VECMAP_SIZE; i += 4)
    {
        if (FMC_Read(FMC_APROM_BASE + i) != M32(FMC_APROM_BASE + i))
        {
            printf("Test VECMAP failed at 0x%x => 0x%x, 0x%x\n", (uint32_t)(FMC_APROM_BASE + i), FMC_Read(FMC_APROM_BASE + i), M32(FMC_APROM_BASE + i));
            CU_FAIL();
            break;
        }
    }

    CU_PASS();
}

void API_FMC_RemapBank(void)
{
    CU_ASSERT((FMC->ISPSTS & FMC_ISPSTS_FBS_Msk) == 0);
    FMC_RemapBank(1);
    CU_ASSERT((FMC->ISPSTS & FMC_ISPSTS_FBS_Msk) == FMC_ISPSTS_FBS_Msk);
    FMC_RemapBank(0);
    CU_ASSERT((FMC->ISPSTS & FMC_ISPSTS_FBS_Msk) == 0);
}

void API_FMC_Open(void)
{
    FMC->ISPCTL &= ~FMC_ISPCTL_ISPEN_Msk;

    FMC_Open();

    CU_ASSERT((FMC->ISPCTL & FMC_ISPCTL_ISPEN_Msk) == FMC_ISPCTL_ISPEN_Msk);
}


void API_FMC_Close(void)
{
    FMC->ISPCTL |= FMC_ISPCTL_ISPEN_Msk;

    FMC_Close();

    CU_ASSERT((FMC->ISPCTL & FMC_ISPCTL_ISPEN_Msk) == 0);
}

#if 0   // M55M1 not support
void API_FMC_ReadDataFlashBaseAddr(void)
{
    FMC_ENABLE_ISP();
    CU_ASSERT(FMC_ReadDataFlashBaseAddr() == FMC_Read(FMC_USER_CONFIG_1));
}
#endif

void API_FMC_ReadWriteConfig(void)
{
    uint32_t u32Cfg[2] = { 0xffffffba, TEST_DATA_FLASH_BASE };
    int    ret;

    FMC_ENABLE_ISP();
    FMC_ENABLE_CFG_UPDATE();

    CU_ASSERT(FMC_EraseConfig(FMC_USER_CONFIG_0) == 0);

    ret = FMC_Write(FMC_USER_CONFIG_0, u32Cfg[0]);
    CU_ASSERT((ret == 0) && (g_FMC_i32ErrCode == 0));

    CU_ASSERT(FMC_Read(FMC_USER_CONFIG_0) == 0xffffffba);
#if 0   // M55M1 not support
    CU_ASSERT(FMC_EraseConfig(FMC_USER_CONFIG_1) == 0);

    ret = FMC_Write(FMC_USER_CONFIG_1, u32Cfg[1]);
    CU_ASSERT((ret == 0) && (g_FMC_i32ErrCode == 0));

    CU_ASSERT(FMC_Read(FMC_USER_CONFIG_1) == TEST_DATA_FLASH_BASE);
#endif

    CU_ASSERT(FMC_EraseConfig(FMC_USER_CONFIG_0) == 0);

    u32Cfg[0] = 0xffffffbe;
    ret = FMC_Write(FMC_USER_CONFIG_0, u32Cfg[0]);
    CU_ASSERT((ret == 0) && (g_FMC_i32ErrCode == 0));

    CU_ASSERT(FMC_Read(FMC_USER_CONFIG_0) == 0xffffffbe);

#if 0   // M55M1 not support
    CU_ASSERT(FMC_EraseConfig(FMC_USER_CONFIG_1) == 0);

    ret = FMC_Write(FMC_USER_CONFIG_1, u32Cfg[1]);
    CU_ASSERT((ret == 0) && (g_FMC_i32ErrCode == 0));

    CU_ASSERT(FMC_Read(FMC_USER_CONFIG_1) == TEST_DATA_FLASH_BASE);
#endif

    FMC_DISABLE_CFG_UPDATE();
}

void API_FMC_GetChkSum(void)
{
#if 0   // TESTCHIP_ONLY not support
    uint32_t   u32ChkSum;

    FMC_ENABLE_ISP();
    FMC_ENABLE_LD_UPDATE();            /* Enable LDROM update. */

    u32ChkSum = FMC_GetChkSum(FMC_LDROM_BASE, FMC_LDROM_SIZE);

    if (u32ChkSum == 0xFFFFFFFF)
    {
        printf("Failed on calculating LDROM CRC32 checksum!\n");
        CU_FAIL();
    }

    printf("0x%x\n", u32ChkSum);       /* print out LDROM CRC32 check sum value */
#endif
    CU_PASS();
}

void API_FMC_CheckAllOne(void)
{
#if 0   // TESTCHIP_ONLY not support
    int   ret;

    FMC_ENABLE_ISP();
    FMC_ENABLE_LD_UPDATE();            /* Enable LDROM update. */

    FMC_Erase(FMC_LDROM_BASE);         /* Erase LDROM page 0. */

    /* Run and check flash contents are all 0xFFFFFFFF. */
    ret = FMC_CheckAllOne(FMC_LDROM_BASE, FMC_FLASH_PAGE_SIZE);

    if (ret == READ_ALLONE_YES)                  /* return value READ_ALLONE_YES means all flash contents are 0xFFFFFFFF */
        printf("READ_ALLONE_YES success.\n");    /* FMC_CheckAllOne() READ_ALLONE_YES passed on LDROM page 0. */
    else
    {
        printf("READ_ALLONE_YES failed!\n");     /* FMC_CheckAllOne() READ_ALLONE_YES failed on LDROM page 0. */
        CU_FAIL();
    }

    FMC_Write(FMC_LDROM_BASE, 0);      /* program a 0 to LDROM to make it not all 0xFFFFFFFF. */

    /* Run and check flash contents are not all 0xFFFFFFFF. */
    ret = FMC_CheckAllOne(FMC_LDROM_BASE, FMC_FLASH_PAGE_SIZE);

    if (ret == READ_ALLONE_NOT)
        printf("READ_ALLONE_NOT success.\n");   /* FMC_CheckAllOne() READ_ALLONE_NOT passed on LDROM page 0. */
    else
    {
        printf("READ_ALLONE_NOT failed!\n");    /* FMC_CheckAllOne() READ_ALLONE_NOT failed on LDROM page 0. */
        CU_FAIL();
    }

#endif

    CU_PASS();
}

void API_FMC_SecureConceal(void)
{
    uint32_t u32Addr, u32PageCnt = 1;

    FMC_ENABLE_ISP();
    FMC_ENABLE_CFG_UPDATE();

    for (u32Addr = (FMC_APROM_BASE + FMC_FLASH_PAGE_SIZE); u32Addr < FMC_APROM_END; u32Addr += (FMC_FLASH_PAGE_SIZE * 32))
    {
        CU_ASSERT(FMC_ConfigSecureConceal(u32Addr, u32PageCnt, FALSE) == FMC_OK);
        CU_ASSERT(FMC_Read(FMC_USER_CONFIG_4) == u32Addr);
        CU_ASSERT(FMC_Read(FMC_USER_CONFIG_5) == u32PageCnt);
        u32PageCnt = ((u32PageCnt + 1) % 8) + 1;
    }

    CU_ASSERT(FMC_ConfigSecureConceal(FMC_APROM_BASE, 1, FALSE) == FMC_ERR_SC_INVALID_BASE);
    CU_ASSERT(FMC_ConfigSecureConceal(FMC_APROM_BANK1_BASE, 0, FALSE) == FMC_ERR_SC_INVALID_PAGECNT);

    FMC_DISABLE_CFG_UPDATE();
}

void API_FMC_Const_Flash_Addr()
{
    CU_ASSERT(FMC_APROM_BASE       == 0x00100000UL);
    CU_ASSERT(FMC_APROM_END        == 0x00300000UL);
    CU_ASSERT(FMC_APROM_BANK0_END  == (FMC_APROM_BASE + 0x00100000UL));
    CU_ASSERT(FMC_APROM_BANK0_BASE == (FMC_APROM_BASE));
    CU_ASSERT(FMC_APROM_BANK1_BASE == (FMC_APROM_BANK0_END));

    CU_ASSERT(FMC_LDROM_BASE      == 0x0F100000UL);
    CU_ASSERT(FMC_LDROM_END       == 0x0F102000UL);

    CU_ASSERT(FMC_CONFIG_BASE     == 0x0F300000UL);
    CU_ASSERT(FMC_USER_CONFIG_0   == 0x0F300000UL);
    CU_ASSERT(FMC_USER_CONFIG_1   == 0x0F300004UL);
    CU_ASSERT(FMC_USER_CONFIG_2   == 0x0F300008UL);
    CU_ASSERT(FMC_USER_CONFIG_3   == 0x0F30000CUL);
    CU_ASSERT(FMC_USER_CONFIG_4   == 0x0F300010UL);
    CU_ASSERT(FMC_USER_CONFIG_5   == 0x0F300014UL);
    CU_ASSERT(FMC_USER_CONFIG_6   == 0x0F300018UL);
    CU_ASSERT(FMC_USER_CONFIG_8   == 0x0F300020UL);
    CU_ASSERT(FMC_USER_CONFIG_9   == 0x0F300024UL);
    CU_ASSERT(FMC_USER_CONFIG_10  == 0x0F300028UL);
    CU_ASSERT(FMC_SCRLOCK_BASE    == 0x0F30002CUL);
    CU_ASSERT(FMC_NSCBA_BASE      == 0x0F300030UL);
    CU_ASSERT(FMC_ARLOCK_BASE     == 0x0F300034UL);
    CU_ASSERT(FMC_OTP_BASE        == 0x0F310000UL);
}

void API_FMC_Const_Size()
{
    CU_ASSERT(FMC_APROM_SIZE          == 0x00200000UL);
    CU_ASSERT(FMC_APROM_BANK_SIZE     == (FMC_APROM_SIZE / 2));
    CU_ASSERT(FMC_LDROM_SIZE          == 0x2000UL);
    CU_ASSERT(FMC_FLASH_PAGE_SIZE     == 0x2000UL);
    CU_ASSERT(FMC_PAGE_ADDR_MASK      == 0xFFFFE000UL);
    CU_ASSERT(FMC_MULTI_WORD_PROG_LEN == 512UL);
    CU_ASSERT(FMC_APWPROT_BLOCK_SIZE  == 0x8000UL);
    CU_ASSERT(FMC_OTP_ENTRY_CNT       == 256UL);
}

void API_FMC_Const_ISPCMD()
{
    CU_ASSERT(FMC_ISPCMD_READ        == 0x00);
    CU_ASSERT(FMC_ISPCMD_READ_UID    == 0x04);
    CU_ASSERT(FMC_ISPCMD_READ_ALL1   == 0x08);
    CU_ASSERT(FMC_ISPCMD_READ_CID    == 0x0B);
    CU_ASSERT(FMC_ISPCMD_READ_DID    == 0x0C);
    CU_ASSERT(FMC_ISPCMD_READ_CKS    == 0x0D);
    CU_ASSERT(FMC_ISPCMD_PROGRAM     == 0x21);
    CU_ASSERT(FMC_ISPCMD_PAGE_ERASE  == 0x22);
    CU_ASSERT(FMC_ISPCMD_BANK_ERASE  == 0x23);
    CU_ASSERT(FMC_ISPCMD_CFG_ERASE   == 0x24);
#if 0   // M55M1 not support
    CU_ASSERT(FMC_ISPCMD_BLOCK_ERASE == 0x25);
#endif
    CU_ASSERT(FMC_ISPCMD_MASS_ERASE  == 0x26);
    CU_ASSERT(FMC_ISPCMD_PROGRAM_MUL == 0x27);
    /* TESTCHIP_ONLY not support
    CU_ASSERT(FMC_ISPCMD_RUN_ALL1    == 0x28);
    */
    CU_ASSERT(FMC_ISPCMD_BANK_REMAP  == 0x2C);
    /*
    CU_ASSERT(FMC_ISPCMD_RUN_CKS     == 0x2D);
    */
    CU_ASSERT(FMC_ISPCMD_VECMAP      == 0x2E);
    CU_ASSERT(FMC_ISPCMD_READ_64     == 0x40);
    CU_ASSERT(FMC_ISPCMD_PROGRAM_64  == 0x61);
}

void API_FMC_Const_BOOT()
{
    CU_ASSERT(IS_BOOT_FROM_APROM == 0);
    CU_ASSERT(IS_BOOT_FROM_LDROM == 1);
}

void API_FMC_Const_ReadAllOne()
{
    CU_ASSERT(READ_ALLONE_YES      == 0xA11FFFFF);
    CU_ASSERT(READ_ALLONE_NOT      == 0xA1100000);
    CU_ASSERT(READ_ALLONE_CMD_FAIL == 0xFFFFFFFF);
}

CU_SuiteInfo FMC_Suites[] =
{
    { "FMC CONST", FMC_Tests_Init, FMC_Tests_Clean, NULL, NULL, FMC_ConstTest },
    { "FMC MACRO", FMC_Tests_Init, FMC_Tests_Clean, NULL, NULL, FMC_MacroTest },
    { "FMC API",   FMC_Tests_Init, FMC_Tests_Clean, NULL, NULL, FMC_FuncTest  },
    CU_SUITE_INFO_NULL
};

CU_TestInfo FMC_ConstTest[] =
{
    { "FMC CONST Flash Addr",   API_FMC_Const_Flash_Addr    },
    { "FMC CONST Size",         API_FMC_Const_Size          },
    { "FMC CONST ISPCMD",       API_FMC_Const_ISPCMD        },
    { "FMC CONST Boot",         API_FMC_Const_BOOT          },
    { "FMC CONST Read All One", API_FMC_Const_ReadAllOne    },
    CU_TEST_INFO_NULL
};

CU_TestInfo FMC_MacroTest[] =
{
    { "Enable/Disable ISP",         MACRO_FMC_ENABLE_ISP        },
    { "Enable/Disable LD Update",   MACRO_FMC_ENABLE_LD_UPDATE  },
    { "Enable/Disable CFG Update",  MACRO_FMC_ENABLE_CFG_UPDATE },
    { "Enable/Disable AP Update",   MACRO_FMC_ENABLE_AP_UPDATE  },
    { "Get ISP Fail Flag",          MACRO_FMC_GET_FAIL_FLAG     },
    { "Clear ISP Fail Flag",        MACRO_FMC_CLR_FAIL_FLAG     },
    { "Set APROM/LDROM Boot",       MACRO_FMC_SET_APROM_BOOT    },
    { "Set APROM Protect",          MACRO_FMC_SET_APWPROT       },

    CU_TEST_INFO_NULL
};

CU_TestInfo FMC_FuncTest[] =
{
    { "FMC Write",              API_FMC_Write                   },
    { "FMC Read",               API_FMC_Read                    },
    { "FMC Erase",              API_FMC_Erase                   },
    { "FMC OTP",                API_FMC_OTP                     },
    { "FMC Get Boot Source",    API_FMC_GetBootSource           },
    { "FMC Read UID",           API_FMC_ReadUID                 },
    { "FMC Read CID",           API_FMC_ReadCID                 },
    { "FMC Read PID",           API_FMC_ReadPID                 },
    { "FMC Read UCID",          API_FMC_ReadUCID                },
    { "VECMAP",                 API_FMC_SetVectorPageAddr       },
    { "Remap Bank",             API_FMC_RemapBank               },
    { "Open",                   API_FMC_Open                    },
    { "Close",                  API_FMC_Close                   },
#if 0   // M55M1 not support
    { "DFBA",                   API_FMC_ReadDataFlashBaseAddr   },
#endif
    { "Read/Write Config",      API_FMC_ReadWriteConfig         },
    { "CRC32 Checksum",         API_FMC_GetChkSum               },
    { "Read All One",           API_FMC_CheckAllOne             },
    { "Secure Conceal",         API_FMC_SecureConceal           },
    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
