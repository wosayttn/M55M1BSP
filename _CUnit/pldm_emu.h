#ifndef __PLDM_EMU__
#define __PLDM_EMU__

#include "NuMicro.h"

__STATIC_INLINE void PLDM_UART_Open(UART_T* psUART, uint32_t u32Baudrate)
{
    /* Set GPB12 as UART0 RXD and GPB13 as UART0 TXD */
    SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13();

    /* Configure UART Baudrate - The setting is only for Palladium physical UART output */
    DEBUG_PORT->BAUD = (UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(153600, 38400));
    DEBUG_PORT->LINE = (UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1);
}

#define DEBUG_PORT_Init PLDM_UART_Open

#endif  // __PLDM_EMU__
