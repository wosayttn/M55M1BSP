/**************************************************************************//**
 * @file     drv_pdma.h
 * @brief    PDMA high level driver for M55M1 series
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#ifndef __DRV_PDMA_H__
#define __DRV_PDMA_H__

#include "NuMicro.h"

#ifndef NU_PDMA_SGTBL_POOL_SIZE
    #define NU_PDMA_SGTBL_POOL_SIZE     (16)
#endif

#define NU_PDMA_CAP_NONE                (0 << 0)

#define NU_PDMA_EVENT_ABORT             (1 << 0)
#define NU_PDMA_EVENT_TRANSFER_DONE     (1 << 1)
#define NU_PDMA_EVENT_ALIGNMENT         (1 << 2)
#define NU_PDMA_EVENT_TIMEOUT           (1 << 3)
#define NU_PDMA_EVENT_ALL               (NU_PDMA_EVENT_ABORT | NU_PDMA_EVENT_TRANSFER_DONE | NU_PDMA_EVENT_TIMEOUT)
#define NU_PDMA_EVENT_MASK              NU_PDMA_EVENT_ALL
#define NU_PDMA_UNUSED                  (-1)

#define NU_PDMA_SG_LIMITED_DISTANCE     ((PDMA_DSCT_NEXT_NEXT_Msk>>PDMA_DSCT_NEXT_NEXT_Pos)+1)
#define NU_PDMA_MAX_TXCNT               ((PDMA_DSCT_CTL_TXCNT_Msk>>PDMA_DSCT_CTL_TXCNT_Pos)+1)

#define NVT_ALIGN(size, align)        (((size) + (align) - 1) & ~((align) - 1))
#define NVT_ALIGN_DOWN(size, align)   ((size) & ~((align) - 1))
//#define CONFIG_TICK_PER_SECOND        1000

typedef enum
{
    eMemCtl_SrcFix_DstFix,
    eMemCtl_SrcFix_DstInc,
    eMemCtl_SrcInc_DstFix,
    eMemCtl_SrcInc_DstInc,
    eMemCtl_Undefined = (-1)
} nu_pdma_memctrl_t;

typedef DSCT_T *nu_pdma_desc_t;

typedef void (*nu_pdma_cb_handler_t)(void *, uint32_t);

typedef enum
{
    eCBType_Event,
    eCBType_Trigger,
    eCBType_Disable,
    eCBType_Undefined = (-1)
} nu_pdma_cbtype_t;

struct nu_pdma_chn_cb
{
    nu_pdma_cbtype_t       m_eCBType;
    nu_pdma_cb_handler_t   m_pfnCBHandler;
    void                  *m_pvUserData;
    uint32_t               m_u32Reserved;
};
typedef struct nu_pdma_chn_cb *nu_pdma_chn_cb_t;

struct nu_module
{
    char      *name;
    void      *m_pvBase;
    uint32_t  u32RstId;
    uint32_t  u32ClkEnId;
    int       eIRQn;
} ;
typedef struct nu_module *nu_module_t;

int nu_pdma_channel_allocate(int32_t i32PeripType);
int nu_pdma_channel_free(int i32ChannID);
int nu_pdma_callback_register(int i32ChannID, nu_pdma_chn_cb_t psChnCb);
int nu_pdma_transfer(int i32ChannID, uint32_t u32DataWidth, uint32_t u32AddrSrc, uint32_t u32AddrDst, uint32_t i32TransferCnt, uint32_t u32IdleTimeout_us);
int nu_pdma_transferred_byte_get(int32_t i32ChannID, int32_t i32TriggerByteLen);
void nu_pdma_channel_terminate(int i32ChannID);
nu_pdma_memctrl_t nu_pdma_channel_memctrl_get(int i32ChannID);
int nu_pdma_channel_memctrl_set(int i32ChannID, nu_pdma_memctrl_t eMemCtrl);

nu_pdma_cb_handler_t nu_pdma_callback_hijack(int i32ChannID, nu_pdma_cbtype_t eCBType, nu_pdma_chn_cb_t psChnCb_Hijack);
int nu_pdma_filtering_set(int i32ChannID, uint32_t u32EventFilter);
uint32_t nu_pdma_filtering_get(int i32ChannID);

// For scatter-gather DMA
int nu_pdma_desc_setup(int i32ChannID, nu_pdma_desc_t dma_desc, uint32_t u32DataWidth, uint32_t u32AddrSrc, uint32_t u32AddrDst, int32_t TransferCnt, nu_pdma_desc_t next, uint32_t u32BeSilent);
int nu_pdma_sg_transfer(int i32ChannID, nu_pdma_desc_t head, uint32_t u32IdleTimeout_us);
int nu_pdma_sgtbls_allocate(nu_pdma_desc_t *ppsSgtbls, int num);
void nu_pdma_sgtbls_free(nu_pdma_desc_t *ppsSgtbls, int num);
int nu_pdma_m2m_desc_setup(nu_pdma_desc_t dma_desc, uint32_t u32DataWidth, uint32_t u32AddrSrc,
                           uint32_t u32AddrDst, int32_t i32TransferCnt, nu_pdma_memctrl_t evMemCtrl, nu_pdma_desc_t next, uint32_t u32BeSilent);

// For memory actor
void *nu_pdma_memcpy(void *dest, void *src, unsigned int count);
int nu_pdma_mempush(void *dest, void *src, uint32_t data_width, unsigned int transfer_count);

#define PDMA_ASSERT(expr)                                      \
    do {                                                       \
        if(!(expr)) {                                          \
            printf("Asserted at expression: %s", #expr);       \
            while(1);                                          \
        }                                                      \
    } while(0)


#endif // __DRV_PDMA_H___
