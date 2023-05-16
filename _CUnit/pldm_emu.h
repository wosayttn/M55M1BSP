#ifndef __PLDM_EMU__
#define __PLDM_EMU__

#include "NuMicro.h"

#define UART_Open PLDM_UART_Open

__STATIC_INLINE void PLDM_UART_Open(UART_T* uart, uint32_t u32baudrate)
{
    /* Configure UART Baudrate - The setting is only for Palladium physical UART output */
    DEBUG_PORT->BAUD = (UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(153600, 38400));
    DEBUG_PORT->LINE = (UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1);
}

#endif  // __PLDM_EMU__
