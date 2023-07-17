/**************************************************************************//**
 * @file     spim_code.c
 * @version  V1.00
 * @brief    Collect of sub-routines running on SPIM flash.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>

#include "NuMicro.h"

/*
 *  Put a character to UART0 transmitter
 */
void spim_putc(int ch)
{
    if ((char)ch == '\n')
    {
        while (DEBUG_PORT->FIFOSTS & UART_FIFOSTS_TXFULL_Msk) {}

        DEBUG_PORT->DAT = '\r';
    }

    while (DEBUG_PORT->FIFOSTS & UART_FIFOSTS_TXFULL_Msk) {}

    DEBUG_PORT->DAT = (uint32_t)ch;
}

/*
 *  Poll until received a character from UART0 receiver
 */
char spim_getc(void)
{
    while (1)
    {
        if ((DEBUG_PORT->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == 0U)
        {
            return ((char)DEBUG_PORT->DAT);
        }
    }
}

/*
 *  print out a string
 */
void spim_put_string(char *str)
{
    while (*str)
    {
        spim_putc(*str++);
    }
}

void spim_routine(void)
{
    spim_put_string("\n\n\n");
    spim_put_string("+------------------------------------------+\n");
    spim_put_string("|  Program is now running on SPIM flash.   |\n");
    spim_put_string("+------------------------------------------+\n\n");

    spim_put_string("\nPress any to return to main program on APROM flash...\n");

    spim_getc();
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
