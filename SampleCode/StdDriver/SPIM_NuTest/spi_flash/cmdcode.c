#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NuMicro.h"

//------------------------------------------------------------------------------
static SPIM_PHASE_T gWinBondWrCMD[] =
{
    /* 0x02h : CMD_NORMAL_PAGE_PROGRAM Command Phase Table */
    {
        CMD_NORMAL_PAGE_PROGRAM,                                    //Command Code
        PHASE_NORMAL_MODE, PHASE_WIDTH_8,  PHASE_DISABLE_DTR,       //Command Phase
        PHASE_NORMAL_MODE, PHASE_WIDTH_24, PHASE_DISABLE_DTR,       //Address Phase
        PHASE_NORMAL_MODE, PHASE_ORDER_MODE0,  PHASE_DISABLE_DTR,   //Data Phase
        0,
    },
    /* 0x12h : CMD_NORMAL_PAGE_PROGRAM_4B Command Phase Table */
    {
        CMD_NORMAL_PAGE_PROGRAM_4B,                                 //Command Code
        PHASE_NORMAL_MODE, PHASE_WIDTH_8,  PHASE_DISABLE_DTR,       //Command Phase
        PHASE_NORMAL_MODE, PHASE_WIDTH_32, PHASE_DISABLE_DTR,       //Address Phase
        PHASE_NORMAL_MODE, PHASE_ORDER_MODE0,  PHASE_DISABLE_DTR,   //Data Phase
        0,
    },
    /* 0x32h : CMD_QUAD_PAGE_PROGRAM_WINBOND Command Phase Table */
    {
        CMD_QUAD_PAGE_PROGRAM_WINBOND,                              //Command Code
        PHASE_NORMAL_MODE, PHASE_WIDTH_8,  PHASE_DISABLE_DTR,       //Command Phase
        PHASE_NORMAL_MODE, PHASE_WIDTH_24, PHASE_DISABLE_DTR,       //Address Phase
        PHASE_QUAD_MODE, PHASE_ORDER_MODE0,  PHASE_DISABLE_DTR,     //Data Phase
        0,
    },
    /* 0x34h : CMD_QUAD_PAGE_PROGRAM_WINBOND_4B Command Phase Table */
    {
        CMD_QUAD_PAGE_PROGRAM_WINBOND_4B,                           //Command Code
        PHASE_NORMAL_MODE, PHASE_WIDTH_8,  PHASE_DISABLE_DTR,       //Command Phase
        PHASE_QUAD_MODE, PHASE_WIDTH_32, PHASE_DISABLE_DTR,         //Address Phase
        PHASE_QUAD_MODE, PHASE_ORDER_MODE0,  PHASE_DISABLE_DTR,     //Data Phase
        0,
    },
};

static SPIM_PHASE_T gWinBondRdCMD[] =
{
    /* 0x03: CMD_DMA_NORMAL_READ Command Phase Table */
    {
        CMD_DMA_NORMAL_READ,                                        // Command Code
        PHASE_NORMAL_MODE, PHASE_WIDTH_8, PHASE_DISABLE_DTR,        // Command Phase
        PHASE_NORMAL_MODE, PHASE_WIDTH_24, PHASE_DISABLE_DTR,       // Address Phase
        PHASE_NORMAL_MODE, PHASE_ORDER_MODE0, PHASE_DISABLE_DTR,    // Data Phase
        0,                                                          // Dummy Cycle Phase
    },
    /* 0xEB: CMD_DMA_FAST_QUAD_READ Command Phase Table */
    {
        CMD_DMA_FAST_QUAD_READ,                                                     // Command Code
        PHASE_NORMAL_MODE, PHASE_WIDTH_8, PHASE_DISABLE_DTR,                        // Command Phase
        PHASE_QUAD_MODE, PHASE_WIDTH_24, PHASE_DISABLE_DTR,                         // Address Phase
        PHASE_QUAD_MODE, PHASE_ORDER_MODE0, PHASE_DISABLE_DTR,                      // Data Phase
        4,                                                                          // Dummy Cycle Phase
        PHASE_ENABLE_CONT_READ, PHASE_QUAD_MODE, PHASE_WIDTH_8, PHASE_DISABLE_DTR,  // Read Mode Phase
    },
    /* 0xED: CMD_DMA_FAST_QUAD_DTR_READ Command Phase Table */
    {
        CMD_DMA_FAST_QUAD_DTR_READ,                                                 // Command Code
        PHASE_NORMAL_MODE, PHASE_WIDTH_8, PHASE_DISABLE_DTR,                        // Command Phase
        PHASE_QUAD_MODE, PHASE_WIDTH_24, PHASE_ENABLE_DTR,                          // Address Phase
        PHASE_QUAD_MODE, PHASE_ORDER_MODE0, PHASE_ENABLE_DTR,                       // Data Phase
        7,                                                                          // Dummy Cycle Phase
        PHASE_ENABLE_CONT_READ, PHASE_QUAD_MODE, PHASE_WIDTH_8, PHASE_ENABLE_DTR,   // Read Mode Phase
    },
};

//------------------------------------------------------------------------------
void *SPIM_GetWinbondWrCMD(uint32_t u32Idx)
{
    return &gWinBondWrCMD[u32Idx];
}

void *SPIM_GetWinbondRdCMD(uint32_t u32Idx)
{
    return &gWinBondRdCMD[u32Idx];
}

int Flash_PhaseTableLength(SPIM_PHASE_T *pPhase)
{
    int i32i = 0;
    int i32Cnt = 0;

    for (i32i = 0; pPhase[i32i].u32CMDCode != '\0'; i32i++)
    {
        i32Cnt++;
    }

    return i32Cnt;
}

