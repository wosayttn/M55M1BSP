/**************************************************************************//**
 * @file     system_M55M1.h
 * @version  V1.00
 * @brief    CMSIS Device System Header File for NuMicro M55M1
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/


#ifndef __SYSTEM_M55M1_H__
#define __SYSTEM_M55M1_H__

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Macro Definition                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef DEBUG_PORT
#define DEBUG_PORT      UART0             /*!< Set default Debug UART Port used for retarget.c to output debug message */
#endif

#define __PC()                                                         \
  __extension__({                                                              \
    register unsigned int current_pc;                                          \
    __asm volatile("mov %0, pc" : "=r"(current_pc) : : );                \
    current_pc;                                                                \
  })

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/

#ifndef __HSI
        #define __HSI       (200000000UL)         /*!< PLL default output is 200 MHz */
#endif

#ifndef __HXT
#define __HXT       (12000000UL)          /*!< External Crystal Clock Frequency */
#endif

#ifndef __LXT
#define __LXT       (32768UL)             /*!< External Crystal Clock Frequency 32.768 KHz */
#endif

#define __HIRC      (12000000UL)          /*!< Internal 12 MHz RC Oscillator Frequency */
#define __HIRC48M   (48000000UL)          /*!< Internal 48 MHz RC Oscillator Frequency */
#define __MIRC      (4000000UL)           /*!< Internal 4 MHz RC Oscillator Frequency */
#define __LIRC      (32000UL)             /*!< Internal 32 KHz RC Oscillator Frequency */

#define __SYS_OSC_CLK     (    ___HSI)    /*!< Main oscillator frequency */
#define __SYSTEM_CLOCK    (1UL*__HXT)

extern uint32_t CyclesPerUs;              /*!< Cycles per micro second              */
extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock)  */
extern uint32_t PllClock;                 /*!< PLL Output Clock Frequency           */

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3L)
#if defined (__ICCARM__)
    #define __NONSECURE_ENTRY       __cmse_nonsecure_entry
    #define __NONSECURE_ENTRY_WEAK  __cmse_nonsecure_entry //__weak
    #define __NONSECURE_CALL        __cmse_nonsecure_call
#else
    #define __NONSECURE_ENTRY       __attribute__ ((cmse_nonsecure_entry))
    #define __NONSECURE_ENTRY_WEAK  __attribute__ ((cmse_nonsecure_entry, weak))
    #define __NONSECURE_CALL        __attribute__ ((cmse_nonsecure_call))
#endif
#else
    #define __NONSECURE_ENTRY
    #define __NONSECURE_ENTRY_WEAK
    #define __NONSECURE_CALL
#endif

/**
  \brief Exception / Interrupt Handler Function Prototype
*/
typedef void(*VECTOR_TABLE_Type)(void);

/**
  \brief System Clock Frequency (Core Clock)
*/
extern uint32_t SystemCoreClock;

/**
  \brief Setup the microcontroller system.

   Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);


/**
  \brief  Update SystemCoreClock variable.

   Updates the SystemCoreClock with current core Clock retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);

/**
 * @brief   Gets the internal processor clock.
 * @return  Clock frequency as unsigned 32 bit value.
 **/
extern uint32_t GetSystemCoreClock(void);

#ifndef NVT_DBG_UART_OFF
/**
 * Set debug UART multi-function pins
 *
 * @param  none
 * @return none
 *
 * @brief  Set debug UART multi-function pins
 */
extern void SetDebugUartMFP(void);

/**
 * Set debug UART clock
 *
 * @param    None
 * @return   None
 *
 * @brief  Set debug UART clock source and enable module clock.
 */
extern void SetDebugUartCLK(void);


/**
 * @brief    Init UART
 *
 * @param    None
 * @return   None
 *
 * @details  Initialize debug UART to 115200-8n1
 */
extern void InitDebugUart(void);

#endif /* NVT_DBG_UART_OFF */

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_M55M1_H__ */