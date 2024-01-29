#ifndef __PLDM_EMU__
#define __PLDM_EMU__

#include "NuMicro.h"

#define __TUBE_PRINTF__

__INLINE void InitDebugUart(void)
{
    /* Configure UART Baudrate - The setting is only for Palladium physical UART output */
    DEBUG_PORT->BAUD = (UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(153600, 38400));
    DEBUG_PORT->LINE = (UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1);
}

__STATIC_INLINE void PLDM_UART_Open(UART_T *psUART, uint32_t u32Baudrate)
{
    InitDebugUart();
}

__STATIC_INLINE void tube_printf(char *strMsg)
{
    uint32_t u32TubeAddr = 0x5000F000;
    char *ptr;

    for (ptr = strMsg; *ptr != 0; ptr++)
    {
        M32(u32TubeAddr) = *ptr;
    }

    M32(u32TubeAddr) = '\r';
}

#define DEBUG_PORT_Init PLDM_UART_Open

#endif  // __PLDM_EMU__
