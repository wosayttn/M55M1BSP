/******************************************************************************
* @file    dpm_cunit.c
* @version V1.00
* @brief   DPM CUnit Test
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
#include "dpm_cunit.h"

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

CU_SuiteInfo DPM_Suites[] =
{
    { "DPM Const", suite_success_init, suite_success_clean, NULL, NULL, DPM_ConstTest },
    { "DPM Func",  suite_success_init, suite_success_clean, NULL, NULL, DPM_FuncTest  },
    CU_SUITE_INFO_NULL
};

void TestFunc_DPM_ConstTest(void)
{
    CU_ASSERT_FATAL(DPM_CTL_WVCODE == (0x5AUL << DPM_CTL_RWVCODE_Pos));
    CU_ASSERT_FATAL(DPM_CTL_RVCODE == (0xA5UL << DPM_CTL_RWVCODE_Pos));

    CU_ASSERT_FATAL(DPM_NSCTL_WVCODE == (0x5AUL << DPM_NSCTL_RWVCODE_Pos));
    CU_ASSERT_FATAL(DPM_NSCTL_RVCODE == (0xA5UL << DPM_NSCTL_RWVCODE_Pos));
}

void TestFunc_DPM_Access(void)
{
    DPM_T *psDPM;

    if (__PC() & NS_OFFSET)
        psDPM = DPM_NS;
    else
        psDPM = DPM;

#if !defined (TRUSTZONE_NON_SECURE)
    DPM_ENABLE_DBG_ACCESS();

    while (DPM->STS & DPM_STS_BUSY_Msk)
        ;

    CU_ASSERT((DPM->CTL & DPM_CTL_DACCDIS_Msk) == 0);

    DPM_DISABLE_DBG_ACCESS();

    while (DPM->STS & DPM_STS_BUSY_Msk)
        ;

    CU_ASSERT((DPM->CTL & DPM_CTL_DACCDIS_Msk) == DPM_CTL_DACCDIS_Msk);

    DPM_EnableDebuggerWriteAccess(SECURE_DPM);

    while (DPM->STS & DPM_STS_BUSY_Msk)
        ;

    CU_ASSERT((DPM->CTL & DPM_CTL_DACCWDIS_Msk) == 0);

    DPM_DisableDebuggerWriteAccess(SECURE_DPM);

    while (DPM->STS & DPM_STS_BUSY_Msk)
        ;

    CU_ASSERT((DPM->CTL & DPM_CTL_DACCWDIS_Msk) == DPM_CTL_DACCWDIS_Msk);
#endif

    DPM_EnableDebuggerWriteAccess(NONSECURE_DPM);

    while (psDPM->NSSTS & DPM_NSSTS_BUSY_Msk)
        ;

    CU_ASSERT((psDPM->NSCTL & DPM_NSCTL_DACCWDIS_Msk) == 0);

    DPM_DisableDebuggerWriteAccess(NONSECURE_DPM);

    while (psDPM->NSSTS & DPM_NSSTS_BUSY_Msk)
        ;

    CU_ASSERT((psDPM->NSCTL & DPM_NSCTL_DACCWDIS_Msk) == DPM_NSCTL_DACCWDIS_Msk);
}

/* Enable DPM INT */
#if !defined (TRUSTZONE_NON_SECURE)
__NONSECURE_ENTRY void EnableDpmInt(void)
{
    DPM_ENABLE_INT();

    while ((DPM->CTL & DPM_CTL_INTEN_Msk) == 0)
        ;
}
#else
extern void EnableDpmInt(void);
#endif

/* Disable DPM INT */
#if !defined (TRUSTZONE_NON_SECURE)
__NONSECURE_ENTRY void DisableDpmInt(void)
{
    DPM_DISABLE_INT();

    while ((DPM->CTL & DPM_CTL_INTEN_Msk) == DPM_CTL_INTEN_Msk);
}
#else
extern void DisableDpmInt(void);
#endif

/* Get DPM INT flag */
#if !defined (TRUSTZONE_NON_SECURE)
__NONSECURE_ENTRY uint32_t GetDpmIntFlag(void)
{
    return DPM_GetIntFlag();
}
#else
extern uint32_t GetDpmIntFlag(void);
#endif

void TestFunc_DPM_Password_S()
{
    uint32_t au32Pwd_5[4] = { 0x55555555, 0x55555555, 0x55555555, 0x55555555 };
    uint32_t au32Pwd_A[4] = { 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA };
    uint32_t u32Cnt0 = 0, u32Cnt1 = 0;

    NVIC_DisableIRQ(SCU_IRQn);

    // Update password
    u32Cnt0 = (DPM->STS & DPM_STS_PWUCNT_Msk) >> DPM_STS_PWUCNT_Pos;

    if (DPM_SetPasswordUpdate(SECURE_DPM, au32Pwd_A) == 0)
    {
        printf("\n\n\tPassword Update Max Time. Please Erase Chip and Test again.");

        while (1);
    }
    else
    {
        u32Cnt1 = (DPM->STS & DPM_STS_PWUCNT_Msk) >> DPM_STS_PWUCNT_Pos;
        CU_ASSERT(u32Cnt1 = u32Cnt0 + 1);
    }

    // Compare password fail
    DPM_ENABLE_INT();
    CU_ASSERT((DPM->CTL & DPM_CTL_INTEN_Msk) == DPM_CTL_INTEN_Msk);

    if (DPM_SetPasswordCompare(SECURE_DPM, au32Pwd_5) != 0)
    {
        printf("\n\n\tPassword Compare Max Time. Please Erase Chip and Test again.");

        while (1);
    }
    else
    {
        CU_ASSERT(DPM_GetPasswordErrorFlag(SECURE_DPM) == 1);
        CU_ASSERT(DPM_GetIntFlag() == 1);
        DPM_ClearPasswordErrorFlag(SECURE_DPM);
        CU_ASSERT(DPM_GetPasswordErrorFlag(SECURE_DPM) == 0);
        CU_ASSERT(DPM_GetIntFlag() == 0);
    }

    DPM_DISABLE_INT();
    CU_ASSERT((DPM->CTL & DPM_CTL_INTEN_Msk) == 0);

    // Compare password pass
    if (DPM_SetPasswordCompare(SECURE_DPM, au32Pwd_A) != 0)
    {
        printf("\n\n\tPassword Compare Fail Max Time. Please Erase Chip and Test again.");

        while (1);
    }
}

void TestFunc_DPM_Password_NS()
{
    uint32_t au32Pwd_5[4] = { 0x55555555, 0x55555555, 0x55555555, 0x55555555 };
    uint32_t au32Pwd_A[4] = { 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA };
    uint32_t u32Cnt0, u32Cnt1;

    DPM_T *psDPM;

    if (__PC() & NS_OFFSET)
        psDPM = DPM_NS;
    else
        psDPM = DPM;

    // Update password
    u32Cnt0 = (psDPM->NSSTS & DPM_NSSTS_PWUCNT_Msk) >> DPM_NSSTS_PWUCNT_Pos;

    if (DPM_SetPasswordUpdate(NONSECURE_DPM, au32Pwd_5) == 0)
    {
        printf("\n\n\tPassword Update Max Time. Please Erase Chip and Test again.");

        while (1);
    }
    else
    {
        u32Cnt1 = (psDPM->NSSTS & DPM_NSSTS_PWUCNT_Msk) >> DPM_NSSTS_PWUCNT_Pos;
        CU_ASSERT(u32Cnt1 = u32Cnt0 + 1);
    }

    // Compare password fail
    EnableDpmInt();

    if (DPM_SetPasswordCompare(NONSECURE_DPM, au32Pwd_A) != 0)
    {
        printf("\n\n\tPassword Compare Max Time. Please Erase Chip and Test again.");

        while (1);
    }
    else
    {
        CU_ASSERT(DPM_GetPasswordErrorFlag(NONSECURE_DPM) == 1);
        DPM_ClearPasswordErrorFlag(NONSECURE_DPM);
        CU_ASSERT(DPM_GetPasswordErrorFlag(NONSECURE_DPM) == 0);
    }

    DisableDpmInt();

    // Compare password pass
    if (DPM_SetPasswordCompare(NONSECURE_DPM, au32Pwd_5) != 0)
    {
        printf("\n\n\tPassword Compare Fail Max Time. Please Erase Chip and Test again.");

        while (1);
    }
}

void TestFunc_DPM_Status_S()
{
    // DPM_GetDebugDisable
    // DPM_SetDebugDisable
    if (DPM_GetDebugDisable(SECURE_DPM) == 0)
    {
        DPM_SetDebugDisable(SECURE_DPM);

        while (DPM->STS & DPM_STS_BUSY_Msk)
            ;

        CU_ASSERT((DPM->CTL & DPM_CTL_DBGDIS_Msk) == DPM_CTL_DBGDIS_Msk);
        printf("\n\n\tSet Secure DPM debug disable. Chip reset");
        UART_WAIT_TX_EMPTY(UART0);
        SYS_ResetChip();
    }
    else if (DPM_GetDebugLock(SECURE_DPM) == 0)
    {
        DPM_SetDebugLock(SECURE_DPM);

        while (DPM->STS & DPM_STS_BUSY_Msk)
            ;

        CU_ASSERT((DPM->CTL & DPM_CTL_LOCK_Msk) == DPM_CTL_LOCK_Msk);
        printf("\n\n\tSet Secure DPM debug lock. Chip reset");
        UART_WAIT_TX_EMPTY(UART0);
        SYS_ResetChip();
    }

    CU_ASSERT(DPM_GetDebugDisable(SECURE_DPM) == 1);
    CU_ASSERT(DPM_GetDebugLock(SECURE_DPM) == 1);
}

void TestFunc_DPM_Status_NS()
{
    DPM_T *psDPM = DPM;
    SYS_T *psSYS = SYS;

    if (__PC() & NS_OFFSET)
    {
        psDPM = DPM_NS;
        psSYS = SYS_NS;
    }

    // DPM_GetDebugDisable
    // DPM_SetDebugDisable
    // DPM_GetDebugLock
    // DPM_SetDebugLock
    if (DPM_GetDebugDisable(NONSECURE_DPM) == 0)
    {
        DPM_SetDebugDisable(NONSECURE_DPM);

        while (psDPM->NSSTS & DPM_NSSTS_BUSY_Msk)
            ;

        CU_ASSERT((psDPM->NSCTL & DPM_CTL_DBGDIS_Msk) == DPM_CTL_DBGDIS_Msk);
        printf("\n\n\tSet Non-secure DPM debug disable. Chip reset");
        UART_WAIT_TX_EMPTY(DEBUG_PORT);
        psSYS->RSTCTL |= SYS_RSTCTL_CHIPRST_Msk;
    }
    else if (DPM_GetDebugLock(NONSECURE_DPM) == 0)
    {
        DPM_SetDebugLock(NONSECURE_DPM);

        while (psDPM->NSSTS & DPM_NSSTS_BUSY_Msk)
            ;

        CU_ASSERT((psDPM->NSCTL & DPM_CTL_LOCK_Msk) == DPM_CTL_LOCK_Msk);
        printf("\n\n\tSet Non-secure DPM debug lock. Chip reset");
        UART_WAIT_TX_EMPTY(DEBUG_PORT);
        psSYS->RSTCTL |= SYS_RSTCTL_CHIPRST_Msk;
    }

    CU_ASSERT(DPM_GetDebugDisable(NONSECURE_DPM) == 1);
    CU_ASSERT(DPM_GetDebugLock(NONSECURE_DPM) == 1);
}

CU_TestInfo DPM_ConstTest[] =
{
    { "DPM Const", TestFunc_DPM_ConstTest },
    CU_TEST_INFO_NULL
};

CU_TestInfo DPM_FuncTest[] =
{
    { "DPM_Access", TestFunc_DPM_Access},
#if !defined (TRUSTZONE_NON_SECURE)
    { "DPM_Password_S", TestFunc_DPM_Password_S },
#endif
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#else
    { "DPM_Password_NS", TestFunc_DPM_Password_NS },
#endif
#if !defined (TRUSTZONE_NON_SECURE)
    { "DPM_Status_S", TestFunc_DPM_Status_S },
#endif
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#else
    { "DPM_Status_NS", TestFunc_DPM_Status_NS },
#endif

    CU_TEST_INFO_NULL
};

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/