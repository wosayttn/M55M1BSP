#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NuMicro.h"
#include "common.h"
#include "spim_common.h"
#include "flash_common.h"
#include "cmdcode.h"

//------------------------------------------------------------------------------
int Flash_DMA_Test(uint32_t u32SPIMModule)
{
    int is4ByteAddr = 0;
    uint32_t u32SIntv = 0;
    SPIM_T *pSPIMx = (SPIM_T *)SPIM_GetModule(u32SPIMModule);
    SPIM_PHASE_T *psWbWrCMD = (SPIM_PHASE_T *)SPIM_GetWinbondWrCMD(0);

    //printf("1 psWbWrCMD = %d\r\n", Flash_PhaseTableLength(psWbWrCMD));
    //printf("2 psWbWrCMD = %d\r\n", sizeof(psWbWrCMD) / sizeof(SPIM_PHASE_T));

    Flash_IO_MakePattern(pSPIMx, 1, TEST_START_ADDR, 0x10000);

    return SPIM_OK;
}
