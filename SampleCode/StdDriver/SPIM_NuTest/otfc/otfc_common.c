#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NuMicro.h"
#include "otfc_common.h"
#include "otfc_model.h"

//------------------------------------------------------------------------------
OTFC_T *gOtfcModule[] =
{
    OTFC0,
    OTFC1,
};

// Key = 0x7A29E38E 063FF08A 2F7A7F2A 93484D6F 93484D6F 2F7A7F2A 063FF08A 7A29E38E
uint32_t gau32AESKey[8] =
{
    0x93484D6F, //Key0
    0x2F7A7F2A, //Key1
    0x063FF08A, //Key2
    0x7A29E38E, //Key3
    0x7A29E38E, //Scramble
    0x063FF08A, //NONCE0
    0x2F7A7F2A, //NONCE1
    0x93484D6F, //NONCE2
};

//------------------------------------------------------------------------------
void *OTFC_GetModule(uint32_t u32Index)
{
    return gOtfcModule[u32Index];
}

void OTFC_SetAESKey(uint8_t u8Index, uint32_t u32Key)
{
    gau32AESKey[u8Index] = u32Key;
}

void *OTFC_GetAESKey(uint8_t u8Index)
{
    return &gau32AESKey[u8Index];
}

void OTFC_SW_Cipher(uint8_t u8PRx,
                    uint32_t u32StartAddr,
                    uint32_t u32TestSize,
                    uint8_t *u8SrcBuf,
                    uint8_t *u8DestBuf,
                    uint8_t u8Swap)
{
    volatile uint32_t u32Addr = 0;
    volatile uint32_t u32PlainData = 0;
    volatile uint32_t u32DataCnt = 0;
    volatile uint32_t u32KeyIdx = 0;
    volatile uint32_t u32CipherRes = 0;
    //volatile uint32_t u32CipherRes1 = 0;

    for (u32Addr = u32StartAddr; u32Addr < (u32StartAddr + u32TestSize); u32Addr += 4)
    {
        if (u8Swap == 1)
        {
            otfc_model(u8PRx,
                       data_swap(*((uint32_t *)OTFC_GetAESKey(0))),
                       data_swap(*((uint32_t *)OTFC_GetAESKey(1))),
                       data_swap(*((uint32_t *)OTFC_GetAESKey(2))),
                       data_swap(*((uint32_t *)OTFC_GetAESKey(3))),
                       data_swap(*((uint32_t *)OTFC_GetAESKey(4))),
                       *((uint32_t *)OTFC_GetAESKey(5)),
                       *((uint32_t *)OTFC_GetAESKey(6)),
                       *((uint32_t *)OTFC_GetAESKey(7)),
                       u32Addr);
        }
        else
        {
            otfc_model(u8PRx,
                       *((uint32_t *)OTFC_GetAESKey(0)),
                       *((uint32_t *)OTFC_GetAESKey(1)),
                       *((uint32_t *)OTFC_GetAESKey(2)),
                       *((uint32_t *)OTFC_GetAESKey(3)),
                       *((uint32_t *)OTFC_GetAESKey(4)),
                       *((uint32_t *)OTFC_GetAESKey(5)),
                       *((uint32_t *)OTFC_GetAESKey(6)),
                       *((uint32_t *)OTFC_GetAESKey(7)),
                       u32Addr);
        }

        memcpy((uint32_t *)&u32PlainData, &u8SrcBuf[u32DataCnt], sizeof(uint32_t));

        u32CipherRes = (u32PlainData ^ GetOTFCAnsKey(u32KeyIdx));

        memcpy(&u8DestBuf[u32DataCnt], (uint32_t *)&u32CipherRes, sizeof(uint32_t));
        u32DataCnt += 4;
        u32KeyIdx++;

        if (u32KeyIdx == 4)
        {
            u32KeyIdx = 0;
        }
    }
}

void OTFC_EnableInt(uint32_t u32OTFCModule)
{
    switch (u32OTFCModule)
    {
        case C_OTFC0:
            NVIC_EnableIRQ(OTFC0_IRQn);
            break;

        case C_OTFC1:
            NVIC_EnableIRQ(OTFC1_IRQn);
            break;
    }
}

void OTFC_DisableInt(uint32_t u32OTFCModule)
{
    switch (u32OTFCModule)
    {
        case C_OTFC0:
            NVIC_DisableIRQ(OTFC0_IRQn);
            break;

        case C_OTFC1:
            NVIC_DisableIRQ(OTFC1_IRQn);
            break;
    }
}

void OTFC_SysReset(uint32_t u32Module)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    switch (u32Module)
    {
        case C_OTFC0:
            SYS->OTFCRST |= SYS_OTFCRST_OTFC0RST_Msk;
            SYS->OTFCRST &= ~SYS_OTFCRST_OTFC0RST_Msk;
            //printf("OTFCCTL = %d, addr = 0x%08X\r\n", SYS->OTFCRST, &SYS->OTFCRST);
            break;

        case C_OTFC1:
            SYS->OTFCRST |= SYS_OTFCRST_OTFC1RST_Msk;
            SYS->OTFCRST &= ~SYS_OTFCRST_OTFC1RST_Msk;
            break;
    }

    /* Lock protected registers */
    //SYS_LockReg();
}
