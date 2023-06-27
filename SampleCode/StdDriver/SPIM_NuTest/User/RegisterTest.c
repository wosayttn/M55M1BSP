/**************************************************************************//**
 * @file     reg_tst.c
 * @version  V1.0
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NuMicro.h"
//#include "spim_test.h"
#include "spim_common.h"

//------------------------------------------------------------------------------
#define TEST_IP         "SPIM"
#define TEST_ITEM       "Register Test"

#define REG_NAME(reg)   #reg

//------------------------------------------------------------------------------
typedef struct
{
    uint32_t    addr;
    const char *name;
    int         rflag;  // Read check flag
    int         wflag;  // Write check flag
    int         iflag;  // Initial value check
    uint32_t    ival;   // Default value
    uint32_t    wmask;  // Write mask
    uint32_t    rmask;  // Read mask
    uint32_t    cmask;  // Compare mask, check Register bit R/W/Read Only
} mmreg_t;

static const mmreg_t mmregs[] =
{
    //  addr            name                         rflg    wflg    iflag       ival        rmask       wmask       cmask

    {   SPIM0_BASE + 0x00, REG_NAME(CTL0),             1,      1,      1,      0x80C00000, 0xFFFFFFFF, 0x0FFFFF7D, 0x0FFFFF7D},
    {   SPIM0_BASE + 0x04, REG_NAME(CTL1),             1,      1,      1,      0x00000010, 0xFFFFFFFF, 0xFFFF0F30, 0xFFFF0F30},
    {   SPIM0_BASE + 0x0C, REG_NAME(RXCLKDLY),         1,      1,      1,      0x00000000, 0xFFFFFFFF, 0x0000000F, 0x0000000F},
    {   SPIM0_BASE + 0x10, REG_NAME(Rx0),              1,      1,      1,      0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
    {   SPIM0_BASE + 0x14, REG_NAME(Rx1),              1,      1,      1,      0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
    {   SPIM0_BASE + 0x18, REG_NAME(Rx2),              1,      1,      1,      0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
    {   SPIM0_BASE + 0x1C, REG_NAME(Rx3),              1,      1,      1,      0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
    {   SPIM0_BASE + 0x20, REG_NAME(Tx0),              1,      1,      1,      0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    {   SPIM0_BASE + 0x24, REG_NAME(Tx1),              1,      1,      1,      0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    {   SPIM0_BASE + 0x28, REG_NAME(Tx2),              1,      1,      1,      0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    {   SPIM0_BASE + 0x2C, REG_NAME(Tx3),              1,      1,      1,      0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    {   SPIM0_BASE + 0x30, REG_NAME(SRAMADDR),         1,      1,      1,      0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    {   SPIM0_BASE + 0x34, REG_NAME(DMACNT),           1,      1,      1,      0x00000000, 0xFFFFFFFF, 0x00FFFFFF, 0x00FFFFFF},
    {   SPIM0_BASE + 0x38, REG_NAME(FADDR),            1,      1,      1,      0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    {   SPIM0_BASE + 0x44, REG_NAME(DMMCTL),           1,      1,      1,      0x00080000, 0xFFFFFFFF, 0x07FF00FF, 0x07FF00FF},
    {   SPIM0_BASE + 0x48, REG_NAME(CTL2),             1,      1,      1,      0x00080800, 0xFFFFFFFF, 0x000F0F00, 0x000F0F00},
    {   SPIM0_BASE + 0x4C, REG_NAME(CMD_CODE),         1,      1,      1,      0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    {   SPIM0_BASE + 0x50, REG_NAME(MODE),             1,      1,      1,      0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    {   SPIM0_BASE + 0x54, REG_NAME(PHDMAW),           1,      1,      1,      0x00000000, 0xFFFFFFFF, 0x7D007F7F, 0x7D007F7F},
    {   SPIM0_BASE + 0x58, REG_NAME(PHDMAR),           1,      1,      1,      0x00000000, 0xFFFFFFFF, 0x7F7F7F7F, 0x7F7F7F7F},
    {   SPIM0_BASE + 0x5C, REG_NAME(PHDMM),            1,      1,      1,      0x00000000, 0xFFFFFFFF, 0x7F7F7F7F, 0x7F7F7F7F},
    {   SPIM0_BASE + 0x68, REG_NAME(DLL0),             1,      1,      1,      0x00000000, 0xFFFFFFFF, 0x00001F1F, 0x00001F01},
    {   SPIM0_BASE + 0x6C, REG_NAME(DLL1),             1,      1,      1,      0x07D01388, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    {   SPIM0_BASE + 0x70, REG_NAME(DLL2),             1,      1,      1,      0x07D007D0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},

    {   SPIM0_BASE + 0x80, REG_NAME(HYPER_CMD),        1,      1,      1,      0x00000000, 0xFFFFFFFF, 0x0000000F, 0x0000000F},
    {   SPIM0_BASE + 0x84, REG_NAME(HYPER_CONFIG1),    1,      1,      1,      0x02ED0821, 0xFFFFFFFF, 0xFFFF0F33, 0xFFFF0F33},
    {   SPIM0_BASE + 0x88, REG_NAME(HYPER_CONFIG2),    1,      1,      1,      0x04320400, 0xFFFFFFFF, 0x1FFF0F00, 0x1FFF0F00},
    {   SPIM0_BASE + 0x8C, REG_NAME(HYPER_ADR),        1,      1,      1,      0x00000000, 0xFFFFFFFF, 0x01FFFFFF, 0x01FFFFFF},
    {   SPIM0_BASE + 0x90, REG_NAME(HYPER_WDATA),      1,      1,      1,      0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    {   SPIM0_BASE + 0x94, REG_NAME(HYPER_RDATA),      1,      1,      1,      0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
    {   SPIM0_BASE + 0x98, REG_NAME(HYPER_INTEN),      1,      1,      1,      0x00000000, 0xFFFFFFFF, 0x00000001, 0x00000001},
    {   SPIM0_BASE + 0x9C, REG_NAME(HYPER_INTSTS),     1,      1,      1,      0x00000000, 0xFFFFFFFF, 0x00000001, 0x00000000},
    {   SPIM0_BASE + 0xFFC, REG_NAME(VERSION),         1,      1,      1,      0x02020000, 0xFFFFFFFF, 0x00000000, 0x00000000},
};

static const uint32_t patterns[] =
{
    0x55aa55aa,
    0xaa55aa55,
    0x33cc33cc,
    0xcc33cc33,
    0xFFFFFFFF,
    0x00000000,
};

// Reset value test.
static void InitRegTest(uint32_t u32SPIMModule)
{
    uint32_t u32i = 0;
    uint32_t u32BaseAddr = 0;
    uint32_t rval = 0;
    uint32_t cval = 0;

    SPIM_SysReset(u32SPIMModule);

    printf("<<< SPIM%d register power-on default value test >>>\n", u32SPIMModule);

    if (u32SPIMModule == C_SPIM1)
    {
        u32BaseAddr = 0x01000UL;
    }

    // Read initial value of Memory mapped registers
    for (u32i = 0; u32i < sizeof(mmregs) / sizeof(mmregs[0]); u32i++)
    {
        if (mmregs[u32i].rflag && mmregs[u32i].iflag)
        {
            rval = inp32(mmregs[u32i].addr + u32BaseAddr) & mmregs[u32i].rmask;
            cval = mmregs[u32i].ival & mmregs[u32i].rmask;

            if (rval != cval)
            {
                printf("Unexpected default value read Reg[0x%04x](%s) = 0x%08x,"
                       "expected value = 0x%08x\n",
                       mmregs[u32i].addr + u32BaseAddr,
                       mmregs[u32i].name,
                       rval,
                       cval);
            }
        }
    }

    printf("\n<<< Modify --> Reset --> Check default value again >>>\n");

    for (u32i = 0; u32i < sizeof(mmregs) / sizeof(mmregs[0]); u32i++)
    {
        /* Write 0xFFFFFFFF to all registers */
        outp32(mmregs[u32i].addr + u32BaseAddr, 0xFFFFFFFF);
    }

    SPIM_SysReset(u32SPIMModule);

    // Read initial value of Memory mapped registers
    for (u32i = 0; u32i < sizeof(mmregs) / sizeof(mmregs[0]); u32i++)
    {
        if (mmregs[u32i].rflag && mmregs[u32i].iflag)
        {
            rval = inp32(mmregs[u32i].addr + u32BaseAddr) & mmregs[u32i].rmask;
            cval = mmregs[u32i].ival & mmregs[u32i].rmask;

            if (rval != cval)
            {
                printf("Unexpected default value read Reg[0x%04x](%s) = 0x%08x,"
                       "expected value = 0x%08x\n",
                       mmregs[u32i].addr + u32BaseAddr,
                       mmregs[u32i].name,
                       rval,
                       cval);
            }
        }
    }
}

static void ReadWriteRegTest(uint32_t u32SPIMModule)
{
    int i, pidx;
    uint32_t u32BaseAddr = 0;
    uint32_t rval;
    uint32_t cval;

    if (u32SPIMModule == C_SPIM1)
    {
        u32BaseAddr = 0x01000UL;
    }

    // Write and Read test
    for (i = 0; i < sizeof(mmregs) / sizeof(mmregs[0]); i++)
    {
        for (pidx = 0; pidx < sizeof(patterns) / sizeof(patterns[0]); pidx++)
        {
            //printf("\n<<< R/W 0x%08X value Test >>>\n", patterns[pidx]);

            if (mmregs[i].wflag)
            {
                // Write pattern data
                outp32(mmregs[i].addr + u32BaseAddr, ((patterns[pidx]) & mmregs[i].wmask));

                // Read back and compare.
                if (mmregs[i].rflag)
                {
                    rval = inp32(mmregs[i].addr + u32BaseAddr) & mmregs[i].cmask;
                    cval = patterns[pidx] & mmregs[i].cmask;

                    //if (strcmp(mmregs[i].name, REG_NAME(DLL0)) == 0)
                    //{
                    //    cval |= 0x10;
                    //}

                    if (rval != cval)
                    {
                        printf("Unexpected value read from Reg[0x%03x](%s): W = 0x%08x, "
                               "R = 0x%08x\n",
                               mmregs[i].addr + u32BaseAddr,
                               mmregs[i].name,
                               cval,
                               rval);
                    }
                }
            }
        }
    }
}

int RegisterTest(uint32_t u32SPIMModule)
{
    SPIM_T *pSPIMx = (SPIM_T *)SPIM_GetModule(u32SPIMModule);

    printf("[%s][%s] ...\n", TEST_IP, TEST_ITEM);

    SPIM_SET_CLOCK_DIVIDER(pSPIMx, 0);

    do
    {
        InitRegTest(u32SPIMModule);
        ReadWriteRegTest(u32SPIMModule);
    } while (0);

    {
        printf("[%s][%s]FAILED\n", TEST_IP, TEST_ITEM);
        return -1;
    }

    printf("[%s][%s]PASSED\n", TEST_IP, TEST_ITEM);

    SPIM_SET_CLOCK_DIVIDER(pSPIMx, 10);

    return 0;
}
