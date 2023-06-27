#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NuMicro.h"
#include "common.h"
#include "spim_common.h"
#include "flash_common.h"
#include "cmdcode.h"

//------------------------------------------------------------------------------
static int do_io_write_test(SPIM_T *pSPIMx)
{
    uint32_t u32PageAddr = TEST_START_ADDR, u32Offset = 0;
    uint32_t u32CMDIdx = 0;
    int i32Is4ByteAddr = 0;
    SPIM_PHASE_T *psWbWrCMD = (SPIM_PHASE_T *)SPIM_GetWinbondWrCMD(u32CMDIdx);
    SPIM_PHASE_T *psWbRdCMD = (SPIM_PHASE_T *)SPIM_GetWinbondRdCMD(0);

    for (u32CMDIdx = 0; u32CMDIdx <= Flash_PhaseTableLength(psWbWrCMD); u32CMDIdx++)
    {
        psWbWrCMD = (SPIM_PHASE_T *)SPIM_GetWinbondWrCMD(u32CMDIdx);

        if (psWbWrCMD->u32AddrWidth == PHASE_WIDTH_32)
        {
            i32Is4ByteAddr = 1;
        }
        else
        {
            i32Is4ByteAddr = 0;
        }

        printf("Write Test CMD %xh\r\n", psWbWrCMD->u32CMDCode);

        for (u32PageAddr = TEST_START_ADDR; u32PageAddr < TEST_FLASH_SIZE; u32PageAddr += TEST_JUMP_SIZE)
        {
            Flash_EraseBlock(pSPIMx, u32PageAddr, 0);

            printf("Write Test 0x%x \r\n", u32PageAddr + u32Offset);
            popDat(tstbuf, TEST_BUFF_SIZE);

            SPIM_IO_WritePhase(pSPIMx, psWbWrCMD, u32PageAddr, tstbuf, TEST_BUFF_SIZE);

            printf("Write Verify...\n");
            memset(tstbuf2, 0, TEST_BUFF_SIZE);

            // Read back via Normal mode.
            SPIM_IO_ReadPhase(pSPIMx, psWbRdCMD, u32PageAddr, tstbuf2, TEST_BUFF_SIZE);

            // Compare.
            if (memcmp(tstbuf, tstbuf2, TEST_BUFF_SIZE))
            {
                printf(RED_BOLD "!!!\tData compare 0x%02X, %d-byte address FAILED\n" RESET,
                       psWbWrCMD->u32CMDCode,
                       i32Is4ByteAddr ? 4 : 3);
                dump_compare_error(u32PageAddr + u32Offset,
                                   tstbuf,
                                   tstbuf2,
                                   TEST_BUFF_SIZE);
                // return -1;
                continue;
            }
            else
            {
                printf("\tIO Write CMD: 0x%02X Test Pass~~~\r\n\r\n", psWbWrCMD->u32CMDCode);
            }
        }
    }

    return 0;
}

int Flash_IO_WriteTest(uint32_t u32SPIMModule)
{
    SPIM_T *pSPIMx = (SPIM_T *)SPIM_GetModule(u32SPIMModule);
    uint32_t u32SIntv = 0;

    printf("\nI/O Write Test =>\n");

    for (u32SIntv = 0; u32SIntv <= 0xF; u32SIntv++)
    {
        printf("\nClock divider setting: 0x%x (%d AHB clock)\r\n,"
               "Suspend interval setting: 0x%x (%d AHB clock).\n",
               u32SIntv,
               (u32SIntv ^ 2),
               u32SIntv,
               (u32SIntv + 2));

        SPIM_SET_CLOCK_DIVIDER(pSPIMx, u32SIntv);
        SPIM_SET_SUSP_INTVL(pSPIMx, u32SIntv);

        if (do_io_write_test(pSPIMx) < 0)
        {
            printf("\nIO Write Test Failed!!\n");
            return -1;
        }
    }

    printf("Flash IO Write Test Done.\r\n\r\n");

    return 0;
}

static int32_t do_io_read(SPIM_T *pSPIMx, SPIM_PHASE_T *psWbRdCMD,
                          uint32_t u32SAddr, uint8_t *u32SrcData, uint32_t u32TestSize)
{
    uint32_t u32Is4ByteAddr = 0;

    printf("Read Test CMD %xh\r\n", psWbRdCMD->u32CMDCode);

    if (psWbRdCMD->u32AddrWidth == PHASE_WIDTH_32)
    {
        u32Is4ByteAddr = 1;
    }
    else
    {
        u32Is4ByteAddr = 0;
    }

    memset(tstbuf2, 0, u32TestSize);

    // Read back via Normal mode.
    SPIM_IO_ReadPhase(pSPIMx, psWbRdCMD, u32SAddr, tstbuf2, u32TestSize);

    // Compare.
    if (memcmp(u32SrcData, tstbuf2, u32TestSize))
    {
        printf(RED_BOLD "!!!\tData compare 0x%02X, %d-byte address FAILED\n" RESET,
               psWbRdCMD->u32CMDCode,
               u32Is4ByteAddr ? 4 : 3);
        dump_compare_error(u32SAddr,
                           u32SrcData,
                           tstbuf2,
                           u32TestSize);
        return SPIM_ERR_FAIL;
        //continue;
    }
    else
    {
        printf("\tIO Write CMD: 0x%02X Test Pass~~~\r\n\r\n", psWbRdCMD->u32CMDCode);
    }

    return SPIM_OK;
}

static int do_io_read_test(SPIM_T *pSPIMx)
{
    uint32_t u32PageAddr = TEST_START_ADDR, u32Offset = 0;
    uint32_t u32CMDIdx = 0;
    int i32Is4ByteAddr = 0;
    SPIM_PHASE_T *psWbWrCMD = (SPIM_PHASE_T *)SPIM_GetWinbondWrCMD(0);
    SPIM_PHASE_T *psWbRdCMD = (SPIM_PHASE_T *)SPIM_GetWinbondRdCMD(u32CMDIdx);

    for (u32PageAddr = TEST_START_ADDR; u32PageAddr < TEST_FLASH_SIZE; u32PageAddr += TEST_JUMP_SIZE)
    {
        Flash_EraseBlock(pSPIMx, u32PageAddr, 0);

        printf("Write Test 0x%x \r\n", u32PageAddr + u32Offset);
        popDat(tstbuf, TEST_BUFF_SIZE);

        SPIM_IO_WritePhase(pSPIMx, psWbWrCMD, u32PageAddr, tstbuf, TEST_BUFF_SIZE);

        printf("Write Verify...\n");
        memset(tstbuf2, 0, TEST_BUFF_SIZE);

        if (do_io_read(pSPIMx, psWbRdCMD, u32PageAddr, tstbuf, TEST_BUFF_SIZE) != SPIM_OK)
        {
            continue;
        }
    }

    for (u32CMDIdx = 1; u32CMDIdx <= Flash_PhaseTableLength(psWbRdCMD); u32CMDIdx++)
    {
        psWbRdCMD = (SPIM_PHASE_T *)SPIM_GetWinbondRdCMD(u32CMDIdx);

        for (u32PageAddr = TEST_START_ADDR; u32PageAddr < TEST_FLASH_SIZE; u32PageAddr += TEST_JUMP_SIZE)
        {
            if (do_io_read(pSPIMx, psWbRdCMD, u32PageAddr, tstbuf, TEST_BUFF_SIZE) != SPIM_OK)
            {
                continue;
            }
        }
    }

    return 0;
}

int Flash_IO_ReadTest(uint32_t u32SPIMModule)
{
    SPIM_T *pSPIMx = (SPIM_T *)SPIM_GetModule(u32SPIMModule);
    uint32_t u32SIntv = 0;

    printf("\nI/O Read Test =>\n");

    for (u32SIntv = 0; u32SIntv <= 0xF; u32SIntv++)
    {
        printf("\nClock divider setting: 0x%x (%d AHB clock)\r\n,"
               "Suspend interval setting: 0x%x (%d AHB clock).\n",
               u32SIntv,
               (u32SIntv ^ 2),
               u32SIntv,
               (u32SIntv + 2));

        SPIM_SET_CLOCK_DIVIDER(pSPIMx, u32SIntv);
        SPIM_SET_SUSP_INTVL(pSPIMx, u32SIntv);

        if (do_io_read_test(pSPIMx) < 0)
        {
            printf("\nIO Write Test Failed!!\n");
            return -1;
        }
    }

    printf("Flash IO Write Test Done.\r\n\r\n");

    return 0;
}
