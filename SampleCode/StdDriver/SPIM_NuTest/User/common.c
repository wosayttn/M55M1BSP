#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

//------------------------------------------------------------------------------
static const uint32_t gu32PopPattern[] =
{
    //0x00000000, 0xFFFFFFFF, 0x55aa55aa, 0xaa55aa55, 0x33cc33cc, 0xcc33cc33
    0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666
    //0x11111111, 0x22222222, 0x33333333, 0x44444444
    //0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF
};

//------------------------------------------------------------------------------
void HexDump(uint8_t *pucBuff, int nSize)
{
    int     nIdx, i;

    nIdx = 0;

    while (nSize > 0)
    {
        printf("0x%04X  ", nIdx);

        for (i = 0; i < 16; i++)
            printf("%02x ", pucBuff[nIdx + i]);

        printf("  ");

        for (i = 0; i < 16; i++)
        {
            if ((pucBuff[nIdx + i] >= 0x20) && (pucBuff[nIdx + i] < 127))
                printf("%c", pucBuff[nIdx + i]);
            else
                printf(".");

            nSize--;
        }

        nIdx += 16;
        printf("\n");
    }

    printf("\n");
}

void dump_compare_error(uint32_t addr, uint8_t *buf_expect, uint8_t *buf_compare, int count)
{
    int  i, err_cnt = 0;

    for (i = 0; (i < count) && (err_cnt < 32); i++)
    {
        if (buf_expect[i] != buf_compare[i])
        {
            printf("addr 0x%x, expect: 0x%x, read: 0x%x\n", addr + i, buf_expect[i], buf_compare[i]);
            err_cnt++;
        }
    }
}

int CompareTestPatten(uint32_t u32TestCMD,
                      uint32_t u32StarAddr,
                      uint8_t *u8SrcData,
                      uint8_t *u8DestData,
                      uint32_t u32TestSize)
{
    // Compare.
    if (memcmp(u8SrcData, u8DestData, u32TestSize))
    {
        printf(RED_BOLD"!!!\tData compare CMD: 0x%02X, Address: 0x%x FAILED\n"RESET,
               u32TestCMD,
               u32StarAddr);
        //dump_compare_error(u32StarAddr, u8SrcData, u8DestData, u32TestSize);
        return -1;
    }
    else
    {
        printf("\t CMD: 0x%02X Test Pass~~~\r\n\r\n", u32TestCMD);
    }

    return 0;
}

void popDat(uint8_t *pu8buf, uint32_t bufSize)
{
    uint8_t *bufInd = (uint8_t *)pu8buf;
    uint32_t bufRmn = bufSize;

    while (bufRmn)
    {
        uint32_t nTx = sizeof(gu32PopPattern);

        if (nTx > bufRmn)
        {
            nTx = bufRmn;
        }

        memcpy(bufInd, (uint8_t *) gu32PopPattern, nTx);
        bufInd += nTx;                              // Advance indicator.
        bufRmn -= nTx;
    }
}

uint32_t sysGetNum(void)
{
    uint8_t cInputTemp = 0x00, InputString[16] = {0};
    uint32_t nLoop = 0;

    while ((cInputTemp != 0x0D) && (cInputTemp != 0x0A))
    {
        cInputTemp = getchar();

        if (cInputTemp == 27)   /* 27: ESC key */
        {
            return cInputTemp;
        }

        if (cInputTemp == 'x' || cInputTemp == 'X' || cInputTemp == 'f' ||
                cInputTemp == 'F' || cInputTemp == 'r' || cInputTemp == 'R')
        {
            return cInputTemp;
        }

        if (cInputTemp == '-')
        {
            InputString[nLoop] = cInputTemp;
            printf("%c", cInputTemp);
            nLoop++;
        }
        else if (cInputTemp >= '0' && cInputTemp <= '9')
        {
            InputString[nLoop] = cInputTemp;
            printf("%c", cInputTemp);
            nLoop++;
        }
    }

    printf("\n");

    //return atoi(InputString);
    return atoi((const char *)InputString);
}

uint32_t GetRequireOptions(S_TestOption *pOption, uint32_t u32Size)
{
    int i32Index = 0;

    for (i32Index = 0; i32Index < u32Size; i32Index++)
    {
        printf("[%d]%s\r\n",
               i32Index,
               pOption[i32Index].chName);
    }

    printf("\r\n");

    i32Index = sysGetNum();

    if (i32Index >= u32Size)
    {
        return 0;
    }

    return i32Index;
}

void RunTestFunction(S_TestMenu *pTestMenu, uint32_t u32MenuSize, uint32_t u32ModuleIdx)
{
    int i32Index = 0;

    while (1)
    {
        for (i32Index = 0; i32Index < u32MenuSize; i32Index++)
        {
            printf("[%d]%s\r\n",
                   i32Index,
                   pTestMenu[i32Index].chName);
        }

        printf("\r\n");

        i32Index = sysGetNum();

        if (i32Index == 0)
        {
            return;
        }

        if (i32Index >= u32MenuSize)
        {
            continue;
        }

        printf("===========================================================\r\n");
        printf("Current test : %s\r\n", pTestMenu[i32Index].chName);
        printf("===========================================================\r\n");

        if (pTestMenu[i32Index].fp != NULL)
        {
            pTestMenu[i32Index].fp(u32ModuleIdx);
        }
    }
}
