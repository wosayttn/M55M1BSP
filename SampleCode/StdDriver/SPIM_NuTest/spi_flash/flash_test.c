#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NuMicro.h"
#include "common.h"
#include "spim_common.h"
#include "flash_test.h"

//------------------------------------------------------------------------------
int Flash_TestMenu(uint32_t u32SPIMModule)
{
    uint8_t u8DivStart = SPIM_GetClkDivNum();
    SPIM_T *pSPIMx = (SPIM_T *)SPIM_GetModule(u32SPIMModule);

    S_TestMenu sMainMenu[] =
    {
        {"\r\n\r\nExit SPIM Test\r\n", NULL},

        {"ID Test", &Flash_IDTest, 1},

        //No Test{"RX Delay Time test", &SPIM_RXDelayTimeSet, 1},

        {"IO Write Test", &Flash_IO_WriteTest, 1},
        {"IO Read Test", &Flash_IO_ReadTest, 1},

        {"DMA Read Test", &Flash_DMA_Test, 1},

        //{"DMA Write Test", &dma_write_test, 1},
        //{"DMM Mode Test", &dmm_mode_test, 1},
        //{"DMM Wrap Read Test", &dmm_wrapread_test, 1},
        //No Test {"DMM Mode Simple Test", &dmm_mode_simple_test, 1},

        //{"I/O Mode Read Test", &io_mode_test, 1},
        //{"I/O Mode Write Test", &io_write_test, 1},
        //{"Load Code And Run Test", &load_code_and_run, 0},
        //{"Clock Divider Test (Real Chip)", &clock_divider_test, 1},
        //{"SPIM DMA Interrupt Test", &SPIM_InterruptTest, 0},

        //No Test {"DTR/DDR DMA Read Test", &dtr_ddr_dma_read_test, 1},
        //No Test {"DTR/DDR DMM Read Test", &dtr_ddr_dmm_read_test, 1},
        //No Test {"CCM mode test", &ccm_sram_test, 1},

        //#if (ENABLE_SPIM_CACHE == 1)
        //{"Cache Invalid Test\r\n", &cache_invalid_test, 0},

        //{"Octal Flash DLL/DMA/DMM test\r\n", &SPIM_OctalDllDmaDmmTest, 0},
        //#endif //ENABLE_SPIM_CACHE

        //No Test {"Select multi-function pin test", &select_mfp, 1},
        //No Test {"Select SPIM Freq", &SetSPIMFrequence, 1},
    };

    SPIM_SET_CLOCK_DIVIDER(pSPIMx, u8DivStart);

    SPIM_InitFlash(pSPIMx, 1);  // Init flash.
    Flash_IDTest(u32SPIMModule);

    RunTestFunction(sMainMenu, sizeof(sMainMenu) / sizeof(sMainMenu[0]), u32SPIMModule);

    return SPIM_OK;
}
