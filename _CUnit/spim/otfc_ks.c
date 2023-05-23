/**************************************************************************//**
 * @file     OtfcTest.c
 * @version  V1.00
 * @brief    Demonstrate how to use FMC CRC32 ISP command to calculate the
 *           CRC32 checksum of APROM, LDROM, and SPROM.
 *
 *
 * @copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "common.h"

//------------------------------------------------------------------------------
int32_t KS_SetAESToKeyStoreSRAM(KS_MEM_Type eMemType, uint32_t u32Readable, uint32_t au32Key[])
{
    int32_t i32RetCode = 0;
    int32_t i, i32KeyIdx, i32KeyCnt = 0;
    uint32_t u32KeyMeta = KS_META_128 | KS_META_AES;
    uint32_t au32ReadKey[4] = {0};
    //uint32_t au32Key[4] = { 0x93484D6F, 0x2F7A7F2A, 0x063FF08A, 0x7A29E38E }, // Key = 0x7A29E38E 063FF08A 2F7A7F2A 93484D6F
    //                      au32ReadKey[4];

    //printf("[%s] Location: %s, readable: %s\n",
    //       __func__, eMemType ? "Flash" : "SRAM",
    //       PRINT_TRUE_FALSE(u32Readable));

    //KS_Open();

    if (u32Readable)
        u32KeyMeta |= KS_META_READABLE;

    KS_EraseAll(eMemType);

    //while (KS_GetRemainKeyCount(eMemType) > 0)
    {
        //for (i = 0; i < 4; i++)
        //{
        //    au32Key[i] += i32KeyIdx;
        //}

        i32KeyIdx = KS_Write(eMemType, u32KeyMeta, au32Key);

        if (i32KeyIdx < 0)
        {
            printf("Fail to write key to Key Store!\n");
            printf("KS remain size = %d\n", KS_GetRemainSize(eMemType));
            printf("KS remain count = %d\n", KS_GetRemainKeyCount(eMemType));

            while (1) {}
        }

        //printf("KS SRAM remain size: %d\n", KS_GetRemainSize(eMemType));
        //printf("The key in key store:\n");
        //DumpBuff((uint8_t *)&au32Key[0], 16);
        memset(au32ReadKey, 0x0, sizeof(au32ReadKey));

        i32RetCode = KS_Read(eMemType, i32KeyIdx, au32ReadKey, sizeof(au32ReadKey) / sizeof(au32ReadKey[0]));

        if (u32Readable)
        {
            if (i32RetCode != 0 || memcmp(au32Key, au32ReadKey, sizeof(au32ReadKey)) != 0)
            {
                i32RetCode = -1;
                goto KS_AES_Test_Exit;
            }
        }
        else
        {
            // Read key from KS should return error if KS_META_READABLE is not set
            if (i32RetCode == 0)
            {
                i32RetCode = -1;
                goto KS_AES_Test_Exit;
            }

            i32RetCode = 0;

            for (i = 0; i < 8; i++)
            {
                if (au32ReadKey[i] != 0)
                {
                    i32RetCode = -1;
                    goto KS_AES_Test_Exit;
                }
            }
        }

        //if (AES_Test(CRYPTO, eMemType, i32KeyIdx) != 0)
        //{
        //    i32RetCode = -2;
        //    goto KS_AES_Test_Exit;
        //}

        i32KeyCnt++;
    }

KS_AES_Test_Exit:

    if (i32RetCode == 0)
        printf("  [Pass] Test key count: %d\n", i32KeyCnt);
    else
        printf("  [Fail] Test key count: %d\n", i32KeyCnt);

    return i32KeyIdx;
}

void ClearStatus(void)
{
    KS->STS = KS_STS_IF_Msk | KS_STS_EIF_Msk;
}

int32_t CheckStatus(uint32_t u32ExpectStatus)
{
    uint32_t u32Status;

    u32Status = KS->STS;

    if ((u32Status & KS_STS_IF_Msk) != (u32ExpectStatus & KS_STS_IF_Msk))
    {
        printf("[Error]\n");
        printf("  Done interrupt test Error!\n");
        return -1;
    }

    if ((u32Status & KS_STS_EIF_Msk) != (u32ExpectStatus & KS_STS_EIF_Msk))
    {
        printf("[Error]\n");
        printf("  Error flag test Error\n");
        return -1;
    }

    if ((u32Status & KS_STS_BUSY_Msk) != (u32ExpectStatus & KS_STS_BUSY_Msk))
    {
        printf("[Error]\n");
        printf("  Busy flag test Error!\n");
        return -1;
    }

    if ((u32Status & KS_STS_SRAMFULL_Msk) != (u32ExpectStatus & KS_STS_SRAMFULL_Msk))
    {
        printf("[Error]\n");
        printf("  SRAM full flag test Error!\n");
        return -1;
    }

    if (u32Status & KS_STS_FLASHFULL_Msk)
    {
        printf("[Error]\n");
        printf("  Flash full flag test Error\n");
        return -1;
    }

    if ((u32Status & KS_STS_INITDONE_Msk) != KS_STS_INITDONE_Msk)
    {
        printf("[Error]\n");
        printf("  Init flag test Error\n");
        return -1;
    }

    return 0;
}

/**
  * @brief      Erase a key from key store
  * @param[in]  eType       The memory type. It could be:
                            \ref KS_SRAM
                            \ref KS_OTP (Up to 3 times)
  *             i32KeyIdx   The key index to read
  * @retval     0           Successful
  * @retval     -1          Fail
  * @details    This function is used to erase a key from SRAM or OTP of key store.
   */
int32_t KS_EraseKey_New(KS_MEM_Type eType, int32_t i32KeyIdx)
{
    uint32_t u32TimeOutCount = KS_TIMEOUT;

    /* Just return when key store is in busy */
    if (KS->STS & KS_STS_BUSY_Msk)
        return -1;

    /* Clear error flag */
    KS->STS = KS_STS_EIF_Msk;

    /* Specify the key address */
    KS->METADATA = (eType << KS_METADATA_DST_Pos) | KS_TOMETAKEY(i32KeyIdx);

    /* Clear Status */
    KS->STS = KS_STS_EIF_Msk | KS_STS_IF_Msk;

    /* Erase the key */
    KS->CTL = KS_OP_ERASE | KS_CTL_START_Msk  | (KS->CTL & (KS_CTL_SILENT_Msk | KS_CTL_SCMB_Msk));

    /* Waiting for processing */
    while (KS->STS & KS_STS_BUSY_Msk)
    {
        if (--u32TimeOutCount == 0)
            return -1;
    }

    /* Check error flag */
    if (KS->STS & KS_STS_EIF_Msk)
        return -1;

    return 0;

}

int32_t KS_EraseAESToKeyStoreOTP(int i32KeyIdx)
{
    int i32RetCode = 0;
    int i32EraseCnt = 0;

    printf("Modify OTP Key Test ....................... ");
    ClearStatus();

    i32RetCode = KS_EraseAll(KS_OTP);

    //KS_Open();

    if (i32RetCode == 0)
    {
        printf("[Error]\n");
        printf("  OTP key did not support EraseAll !\n");
        goto ReadWriteOTPTest_Exit;
    }

    //#ifdef __PLDM_EMU__

    //for (i32KeyIdx = 0; i32KeyIdx < KS_MAX_OTP_KEY_CNT; i32KeyIdx++)
    {
        //i32EraseCnt = 0;
        //#else
        //    i32KeyIdx  = 6;
        //#endif

        //do
        //{
        i32RetCode = KS_EraseOTPKey(i32KeyIdx);

        if (i32RetCode == 0)
        {
            if (i32EraseCnt >= 3)
            {
                printf("[Error]\n");
                printf("  OTP key should not be erased over 3 times !\n");
                goto ReadWriteOTPTest_Exit;
            }
            else
            {
                if (KS->OTPSTS & (1 << i32KeyIdx))
                {
                    printf("[Error]\n");
                    printf("  OTP key status is not unused after erase !\n");
                    goto ReadWriteOTPTest_Exit;
                }
            }
        }
        else
        {
            if (i32EraseCnt < 3)
            {
                printf("[Error]\n");
                printf("  OTP key should be erased 3 times !\n");
                goto ReadWriteOTPTest_Exit;
            }
        }

        //i32RetCode = KS_WriteOTP(i32KeyIdx, KS_META_256 | KS_META_CPU | KS_META_PRIV | KS_META_READABLE, au32Key);

        //if (i32RetCode == 0 && i32EraseCnt >= 3)
        //{
        //    printf("[Error]\n");
        //    printf("  OTP key should not be able to modify more than 3 times !\n");
        //    goto ReadWriteOTPTest_Exit;
        //}

        //i32EraseCnt++;
        //} while (i32EraseCnt <= 3);
    }

ReadWriteOTPTest_Exit:

    if (i32RetCode == 0)
        printf("  [Pass] OTP Erase key\n");
    else
        printf("  [Fail] OTP Erase key\n");

    return 0;
}

void KS_ReadKeyFromKeyStoreOTP(int i32KeyIdx, uint32_t *pu32DestBuf)
{
    uint32_t au32ReadKey[8] = {0};
    uint32_t u32i = 0;
    int i32RetCode = 0;

    memset(pu32DestBuf, 0, sizeof(au32ReadKey));

    ClearStatus();

    i32RetCode = KS_Read(KS_OTP, i32KeyIdx, pu32DestBuf, 8);
    printf("Key Store Read OTP Key =>\r\n");

    for (u32i = 0; u32i < 8; u32i++)
    {
        printf("0x%08X\r\n", pu32DestBuf[u32i]);
    }

    printf("\r\n");

    if (i32RetCode < 0)
    {
        if (i32KeyIdx != KS_KDF_ROOT_OTP_KEY)
        {
            printf("[Error]\n");
            printf("  Read OTP key fail!\n");
            goto ReadWriteOTPTest_Exit;
        }
    }

ReadWriteOTPTest_Exit:

    if (i32RetCode == 0)
        printf("  [Pass] Test OTP key\n");
    else
        printf("  [Fail] Test OTP key\n");

    return;
}

int32_t KS_SetAESToKeyStoreOTP(int i32KeyIdx, uint32_t *pu32AESKey)
{
    int32_t i32RetCode = 0;
    //int32_t i = 0;
    //int i32KeyCnt = 0;
    uint32_t u32i = 0;
    uint32_t au32ReadKey[8] = {0};

    //KS_Open();

    i32RetCode = KS_WriteOTP(i32KeyIdx,
                             (KS_META_256 | KS_META_AES | KS_META_PRIV | KS_META_READABLE),
                             pu32AESKey);

    if (i32RetCode < 0)
    {
        printf("[Error]\n");
        printf("  Write OTP key Error!\n");
        goto ReadWriteOTPTest_Exit;
    }

    i32RetCode = CheckStatus(KS_STS_INITDONE_Msk | KS_STS_IF_Msk);

    if (i32RetCode)
    {
        printf("[Error]\n");
        printf("  Check status fail\n");
        goto ReadWriteOTPTest_Exit;
    }

    memset(au32ReadKey, 0, sizeof(au32ReadKey));
    ClearStatus();
    i32RetCode = KS_Read(KS_OTP, i32KeyIdx, au32ReadKey, 8);
    printf("Key Store Read OTP Key =>\r\n");

    for (u32i = 0; u32i < 8; u32i++)
    {
        printf("0x%08X\r\n", au32ReadKey[u32i]);
    }

    printf("\r\n");

    if (i32RetCode < 0)
    {
        if (i32KeyIdx != KS_KDF_ROOT_OTP_KEY)
        {
            printf("[Error]\n");
            printf("  Read OTP key fail!\n");
            goto ReadWriteOTPTest_Exit;
        }
    }

    if (i32KeyIdx == KS_KDF_ROOT_OTP_KEY)
    {
        if ((KS->STS & KS_STS_EIF_Msk) == 0)
        {
            printf("[Error]\n");
            printf("  Read KDF root OTP key did not trigger error flag !\n");
            goto ReadWriteOTPTest_Exit;
        }
    }
    else
    {
        i32RetCode = CheckStatus(KS_STS_INITDONE_Msk | KS_STS_IF_Msk);

        if (i32RetCode)
        {
            printf("[Error]\n");
            printf("  Check status fail\n");
            goto ReadWriteOTPTest_Exit;
        }

        if (strncmp((char *)&pu32AESKey[0], (char *)&au32ReadKey[0], 16))
        {
            printf("[Error]\n");
            printf("  Key compare fail!\n");
            printf("  Write Key=0x%08x %08x %08x %08x\n", pu32AESKey[0], pu32AESKey[1], pu32AESKey[2], pu32AESKey[3]);
            printf("  Read  Key=0x%08x %08x %08x %08x\n", au32ReadKey[0], au32ReadKey[1], au32ReadKey[2], au32ReadKey[3]);
            i32RetCode = -1;
            goto ReadWriteOTPTest_Exit;
        }
    }

ReadWriteOTPTest_Exit:

    if (i32RetCode == 0)
        printf("  [Pass] Test OTP key\n");
    else
        printf("  [Fail] Test OTP key\n");

    return i32KeyIdx;
}
