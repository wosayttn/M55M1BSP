#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

//------------------------------------------------------------------------------
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
            printf("%d [%d]%s\r\n",
                   u32ModuleIdx,
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
