
/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "NuMicro.h"
#include "CUnit.h"
#include "Console.h"
#include "gdma_cunit.h"

// ref: arm_corelink_dma-350_controller_technical_reference_manual_102482_0000_02_en.pdf

// DMA350 Native Driver
#include "DMA350\dma350_lib.h"
#include "DMA350\dma350_drv.h"
#include "DMA350\dma350_ch_drv.h"

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

// #define DMA350_MAX_NUM_CH 16
#define GDMA_MAX_NUM_CH 2

struct dma350_cmdlink_gencfg_t cmdlink_1d_cfg;

/* DMA350 driver structures */
static const struct dma350_dev_cfg_t GDMA_DEV_CFG_S =
{
    .dma_sec_cfg = (DMASECCFG_TypeDef *)(GDMA_S + 0x0UL),
    .dma_sec_ctrl = (DMASECCTRL_TypeDef *)(GDMA_S + 0x100UL),
    .dma_nsec_ctrl = (DMANSECCTRL_TypeDef *)(GDMA_S + 0x200UL),
    .dma_info = (DMAINFO_TypeDef *)(GDMA_S + 0xF00UL)
};

static struct dma350_dev_data_t GDMA_DEV_DATA_S =
{
    .state = 0
};

struct dma350_dev_t GDMA_DEV_S =
{
    &(GDMA_DEV_CFG_S),
    &(GDMA_DEV_DATA_S)
};

struct dma350_ch_dev_t GDMA_CH0_DEV_S =
{
    .cfg = {
        .ch_base = (DMACH_TypeDef *)(GDMA_S + 0x1000UL),
        .channel = 0
    },
    .data = {0}
};

struct dma350_ch_dev_t GDMA_CH1_DEV_S =
{
    .cfg = {
        .ch_base = (DMACH_TypeDef *)(GDMA_S + 0x1100UL),
        .channel = 1
    },
    .data = {0}
};

struct dma350_ch_dev_t *const GDMA_CH_DEV_S[] =
{
    &GDMA_CH0_DEV_S,
    &GDMA_CH1_DEV_S,
};

uint8_t src_array[0x100] __ALIGNED(4);
uint8_t des_array[0x100] __ALIGNED(4);
uint32_t descriptors[0x100];

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
    {"dma350_drv test", suite_success_init, suite_success_clean, NULL, NULL, GDMA_Examples},
    CU_SUITE_INFO_NULL
};

void GDMA_Reset(void)
{
    SYS_UnlockReg();
    SYS_ResetModule(SYS_GDMA0RST);
}

void printf_array(const void *src,
                  uint32_t src_width, uint16_t src_height)
{
    uint32_t i, j;

    for (i = 0; i < src_width; i++)
    {
        printf("\n[0x%03X] ", i * src_width);

        for (j = 0; j < src_height; j++)
        {
            printf("0x%02X ", des_array[i * src_width + j]);
        }
    }

    printf("\n\n");
}

/*
    GDMA_2DMirror        @ GDMA_2DTransfer
    GDMA_2DRotate        @ GDMA_2DTransfer
    GDMA_2DWrap          @ GDMA_2DTransfer
    GDMA_BasicMode       @ GDMA_1DTransfer
    GDMA_ScatterGather   @ GDMA_CommandLinking
    GDMA_TemplatedMode   @ GDMA_1DTransfer
*/

void GDMA_1DTransfer(void)
{
    int i, j;
    uint32_t SRCADDR, DESADDR, SRCXSIZE;
    uint32_t SRCTMPLT, SRCTMPLTSIZE, DESTMPLT, DESTMPLTSIZE;
    //
    GDMA_Reset();
    // dma350_drv.h
    // Initializes DMA350 DMA
    CU_ASSERT(DMA350_ERR_NONE == dma350_init(&GDMA_DEV_S));
    // Checks if DMA350 device is initialised
    CU_ASSERT(1 == dma350_is_init(&GDMA_DEV_S));
    // Gets number of channels available in DMA350 device
    CU_ASSERT((GDMA_MAX_NUM_CH - 1) == dma350_get_num_ch(&GDMA_DEV_S));

    for (i = 0; i < 8 * 8; i++)
    {
        src_array[i] = i;
    }

    memset(des_array, 0, 0x100);
    // 5.2.1 Transfer type 1D
    // Figure 5-1: 1D transfer example
    SRCADDR = 0x011;
    DESADDR = 0x014;
    SRCXSIZE = 23;
    CU_ASSERT(DMA350_LIB_ERR_NONE == dma350_memcpy(GDMA_CH_DEV_S[0],
                                                   src_array + SRCADDR,
                                                   des_array + DESADDR,
                                                   SRCXSIZE,
                                                   DMA350_LIB_EXEC_BLOCKING));

    for (i = 0; i < SRCXSIZE; i++)
    {
        CU_ASSERT(src_array[SRCADDR + i] == des_array[DESADDR + i]);
    }

    // 5.3.3 Templated transfers
    // Figure 5-14: TMPLT
    SRCADDR = 0x011;
    DESADDR = 0x014;
    SRCXSIZE = 12;
    SRCTMPLT = (BIT0 | BIT2 | BIT3 | BIT6);
    DESTMPLT = (BIT0 | BIT1 | BIT2 | BIT3);
    SRCTMPLTSIZE = 7;
    DESTMPLTSIZE = 3;
    //
    dma350_ch_set_tmplt_src_size(GDMA_CH_DEV_S[0], SRCTMPLTSIZE);
    dma350_ch_set_tmplt_des_size(GDMA_CH_DEV_S[0], DESTMPLTSIZE);
    dma350_ch_set_tmplt_src(GDMA_CH_DEV_S[0], SRCTMPLT);
    dma350_ch_set_tmplt_des(GDMA_CH_DEV_S[0], DESTMPLT);
    CU_ASSERT(DMA350_LIB_ERR_NONE == dma350_memcpy(GDMA_CH_DEV_S[0],
                                                   src_array + SRCADDR,
                                                   des_array + DESADDR,
                                                   SRCXSIZE,
                                                   DMA350_LIB_EXEC_BLOCKING));
    //
    j = SRCXSIZE / 4; // nonzero bits count = 4

    for (i = 0; i < 3; i++)
    {
        CU_ASSERT_EQUAL(src_array[SRCADDR + ((SRCTMPLTSIZE + 1) *  i) + 0],
                        des_array[DESADDR + ((DESTMPLTSIZE + 1) *  i) + 0]);
        CU_ASSERT_EQUAL(src_array[SRCADDR + ((SRCTMPLTSIZE + 1) *  i) + 2],
                        des_array[DESADDR + ((DESTMPLTSIZE + 1) *  i) + 1]);
        CU_ASSERT_EQUAL(src_array[SRCADDR + ((SRCTMPLTSIZE + 1) *  i) + 3],
                        des_array[DESADDR + ((DESTMPLTSIZE + 1) *  i) + 2]);
        CU_ASSERT_EQUAL(src_array[SRCADDR + ((SRCTMPLTSIZE + 1) *  i) + 6],
                        des_array[DESADDR + ((DESTMPLTSIZE + 1) *  i) + 3]);
    }
}

void GDMA_2DTransfer(void)
{
    enum dma350_lib_error_t lib_err;
    int i, j;
    uint32_t src_width, src_height;
    //
    GDMA_Reset();
    // dma350_drv.h
    // Initializes DMA350 DMA
    CU_ASSERT(DMA350_ERR_NONE == dma350_init(&GDMA_DEV_S));
    // Checks if DMA350 device is initialised
    CU_ASSERT(1 == dma350_is_init(&GDMA_DEV_S));
    // Gets number of channels available in DMA350 device
    CU_ASSERT((GDMA_MAX_NUM_CH - 1) == dma350_get_num_ch(&GDMA_DEV_S));

    //
    // Figure 5-7: 2D transformations using increments
    for (i = 0; i < 8 * 8; i++)
    {
        src_array[i] = i;
    }

    src_width = 8;
    src_height = 8;
    /*!< Mirror along X axis. Top to bottom */
    lib_err = dma350_2d_copy(GDMA_CH_DEV_S[0],
                             src_array, des_array,
                             src_width, src_height,
                             DMA350_CH_TRANSIZE_8BITS,
                             DMA350_LIB_TRANSFORM_MIRROR_VER,
                             DMA350_LIB_EXEC_BLOCKING);
    CU_ASSERT(lib_err == DMA350_LIB_ERR_NONE);

    if (lib_err == DMA350_LIB_ERR_NONE)
    {
        printf("\n\t 2D Mirror along X axis. Top to bottom PASS");

        // printf_array(des_array, src_width, src_height);
        for (i = 0; i < src_height; i++)
        {
            for (j = 0; j < src_width; j++)
            {
                /*!< Mirror along X axis. Top to bottom */
                CU_ASSERT(src_array[i * src_width + j] == des_array[(src_height - i - 1) * src_width + j]);
            }
        }
    }

    /*!< Rotate 90 degrees clockwise */
    lib_err = dma350_2d_copy(GDMA_CH_DEV_S[0],
                             src_array, des_array,
                             8, 8,
                             DMA350_CH_TRANSIZE_8BITS,
                             DMA350_LIB_TRANSFORM_ROTATE_90,
                             DMA350_LIB_EXEC_BLOCKING);
    CU_ASSERT(lib_err == DMA350_LIB_ERR_NONE);

    if (lib_err == DMA350_LIB_ERR_NONE)
    {
        printf("\n\t 2D Rotate 90 degrees clockwise PASS");

        // printf_array(des_array, src_width, src_height);
        for (i = 0; i < src_height; i++)
        {
            for (j = 0; j < src_width; j++)
            {
                /*!< Mirror along X axis. Top to bottom */
                CU_ASSERT(src_array[i * src_width + j] == des_array[j * src_width + (src_width - i - 1)]);
            }
        }
    }

    // 5.3.2 WRAP for 2D
    lib_err = dma350_draw_from_canvas(GDMA_CH_DEV_S[0],
                                      src_array, des_array,
                                      3, 3,
                                      3,
                                      8, 8,
                                      8,
                                      DMA350_CH_TRANSIZE_8BITS,
                                      DMA350_LIB_TRANSFORM_NONE,
                                      DMA350_LIB_EXEC_BLOCKING);
    CU_ASSERT(lib_err == DMA350_LIB_ERR_NONE);

    if (lib_err == DMA350_LIB_ERR_NONE)
    {
        // printf("\n des_array (2d Wrap size 3x3, width 3 to size 8x8 width 8) = \n");
        printf("\n\t 2D Wrap Test Case1 PASS");

        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 8; j++)
            {
                CU_ASSERT(des_array[i * 8 + j] == src_array[(i % 3) * 3 + (j % 3)]);
            }
        }
    }

    lib_err = dma350_draw_from_canvas(GDMA_CH_DEV_S[0],
                                      src_array, des_array,
                                      3, 3,
                                      16,
                                      8, 8,
                                      8,
                                      DMA350_CH_TRANSIZE_8BITS,
                                      DMA350_LIB_TRANSFORM_NONE,
                                      DMA350_LIB_EXEC_BLOCKING);

    if (lib_err == DMA350_LIB_ERR_NONE)
    {
        printf("\n\t 2D Wrap Test Case2 PASS");

        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 8; j++)
            {
                CU_ASSERT(des_array[i * 8 + j] == src_array[(i % 3) * 16 + (j % 3)]);
            }
        }
    }
}

void GDMA_CommandLinking(void)
{
    struct dma350_ch_dev_t *dev;
    enum dma350_lib_error_t lib_err;
    struct dma350_cmdlink_gencfg_t cmdlink_cfg;
    union dma350_ch_status_t status;
    uint32_t *buffer;
    int i, j;
    //
    uint32_t size = 8;
    //
    GDMA_Reset();
    CU_ASSERT(DMA350_ERR_NONE == dma350_init(&GDMA_DEV_S));

    for (i = 0; i < 8 * 8; i++)
    {
        src_array[i] = i;
    }

    memset(des_array, 0, 0x100);
    // 5.7 Command linking
    // Table 5-15: Command link example memory map, HEADER_2
    dma350_cmdlink_init(&cmdlink_cfg);
    dma350_cmdlink_set_srcaddr32(&cmdlink_cfg, (uint32_t)src_array);
    dma350_cmdlink_set_desaddr32(&cmdlink_cfg, (uint32_t)des_array + size);
    dma350_cmdlink_set_xsize16(&cmdlink_cfg, (uint16_t)size * 2, (uint16_t)size * 2);
    dma350_cmdlink_set_linkaddr32(&cmdlink_cfg, 0);
    buffer = dma350_cmdlink_generate(&cmdlink_cfg, descriptors, descriptors + 0x100 - 1);
    CU_ASSERT(buffer != NULL);

    //

    if (buffer != NULL)
    {
        dma350_ch_enable_linkaddr(GDMA_CH_DEV_S[0]);
        dma350_ch_set_linkaddr32(GDMA_CH_DEV_S[0], (uint32_t) descriptors);
        lib_err = dma350_memcpy(GDMA_CH_DEV_S[0],
                                src_array,
                                des_array,
                                size,
                                DMA350_LIB_EXEC_BLOCKING);
        CU_ASSERT(DMA350_LIB_ERR_NONE == lib_err);

        if (DMA350_LIB_ERR_NONE == lib_err)
        {
            // first command
            for (i = 0; i < size; i++)
            {
                CU_ASSERT(des_array[i] == src_array[i]);
            }

            // second command
            for (i = 0; i < size * 2; i++)
            {
                CU_ASSERT(des_array[size + i] == src_array[i]);
            }
        }

        // printf_array(des_array, 8, 8);
    }
}
CU_TestInfo GDMA_Examples[] =
{
    {" 1: Test 1DTransfer (Basic & Template) ...", GDMA_1DTransfer},
    {" 2: Test 2DTransfer (Rotate, Mirror & Wrap) ...", GDMA_2DTransfer},
    {" 3: Test ScatterGather ...", GDMA_CommandLinking},
    CU_TEST_INFO_NULL
};
