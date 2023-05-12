/**************************************************************************//**
 * @file     scu.h
 * @version  V1.00
 * @brief    SCU driver header file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#ifndef __SCU_H__
#define __SCU_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
    @{
*/

/** @addtogroup SCU_Driver SCU Driver
    @{
*/

/** @addtogroup SCU_EXPORTED_CONSTANTS SCU Exported Constants
    @{
*/

#define SRAM0_BASE          (SRAM_BASE)
#define SRAM0_SIZE          ((uint32_t) 0x00080000UL)
#define SRAM1_BASE          (SRAM0_BASE + SRAM0_SIZE)
#define SRAM1_SIZE          (0x00080000UL)
#define SRAM2_BASE          (SRAM1_BASE + SRAM1_SIZE)
#define SRAM2_SIZE          ((uint32_t) 0x00050000UL)
#define SRAM3_SIZE          ((uint32_t) 0x00002000UL)
#define LPSRAM_SIZE         ((uint32_t) 0x00002000UL)
#define SPIM0_MEM_BASE      ((uint32_t) 0x80000000UL)
#define SPIM0_MEM_SIZE      ((uint32_t) 0x02000000UL)
#define SPIM1_MEM_BASE      ((uint32_t) 0x82000000UL)
#define SPIM1_MEM_SIZE      ((uint32_t) 0x02000000UL)
/* EBI memory secure/non-secure is decided by SCU_D1PNS1 and did not support MPC config */

#define SRAM0MPC_BLK   0x8000        // Block size: 32 KB
#define SRAM1MPC_BLK   0x8000        // Block size: 32 KB
#define SRAM2MPC_BLK   0x8000        // Block size: 32 KB
#define SRAM3MPC_BLK   0x1000        // Block size: 4 KB
#define LPSRAMMPC_BLK  0x1000        // Block size: 4 KB
#define SPIM0MPC_BLK   0x20000       // Block size: 128 KB
#define SPIM1MPC_BLK   0x20000       // Block size: 128 KB

#define MASTER_ID_CPU       0
#define MASTER_ID_PDMA0     1
#define MASTER_ID_PDMA1     2
#define MASTER_ID_USBH0     3
// Reserved
#define MASTER_ID_HSUSBH    5
#define MASTER_ID_HSUSBD    6
#define MASTER_ID_SDH0      7
#define MASTER_ID_SDH1      8
#define MASTER_ID_EMAC      9
#define MASTER_ID_CRYPTO    10
#define MASTER_ID_CRC       11
#define MASTER_ID_GDMA      12
#define MASTER_ID_NPU       13
#define MASTER_ID_LPPDMA    14
#define MASTER_ID_CCAP      15
#define MASTER_ID_SPIM0     16
#define MASTER_ID_SPIM1     17

#define PVSRC_IDX_APB0      0
#define PVSRC_IDX_APB1      1
#define PVSRC_IDX_APB2      2
#define PVSRC_IDX_APB3      3
#define PVSRC_IDX_APB4      4
#define PVSRC_IDX_APB5      5
#define PVSRC_IDX_D0PPC0    8
#define PVSRC_IDX_D1PPC0    9
#define PVSRC_IDX_D1PPC1    10
#define PVSRC_IDX_D2PPC0    11
#define PVSRC_IDX_EBI       16

#define PVA_IDX_APB0        0
#define PVA_IDX_APB1        1
#define PVA_IDX_APB2        2
#define PVA_IDX_APB3        3
#define PVA_IDX_APB4        4
#define PVA_IDX_APB5        5
#define PVA_IDX_D0PPC0      8
#define PVA_IDX_D1PPC0      9
#define PVA_IDX_D1PPC1      10
#define PVA_IDX_D2PPC0      11
#define PVA_IDX_EBI         16

#define MVA_IDX_GDMA        0
#define MVA_IDX_PDMA0       1
#define MVA_IDX_PDMA1       2
#define MVA_IDX_USBH1       3
#define MVA_IDX_HSUSBH      4
#define MVA_IDX_HSUSBD      5
#define MVA_IDX_SDH0        6
#define MVA_IDX_SDH1        7
#define MVA_IDX_EMAC        8
#define MVA_IDX_CRYPTO      9
#define MVA_IDX_CRC         10
#define MVA_IDX_LPPDMA      11
#define MVA_IDX_CCAP        12
#define MVA_IDX_NPU0        13
#define MVA_IDX_NPU1        14
#define MVA_IDX_SPIM0       15
#define MVA_IDX_SPIM1       16

/**
 *  @details  Non-secure Attribution Definition.
 */
typedef enum NSATTR
{
    /******  SCU_D0PNS0 **********************************************************************************/
    NPU_Attr     =      3,

    /******  SCU_D0PNS2 **********************************************************************************/
    SPIM0_Attr   = 64 + 2,
    SPIM1_Attr   = 64 + 3,

    /******  SCU_D1PNS0 **********************************************************************************/
    PDMA0_Attr   = 256 + 0,
    PDMA1_Attr   = 256 + 1,
    USBH0_Attr   = 256 + 2,
    HSUSBH_Attr  = 256 + 4,
    HSUSBD_Attr  = 256 + 5,
    SDH0_Attr    = 256 + 6,
    SDH1_Attr    = 256 + 7,
    EMAC0_Attr   = 256 + 8,
    CRYPTO_Attr  = 256 + 10,
    CRC_Attr     = 256 + 11,

    /******  SCU_D1PNS1 **********************************************************************************/
    KDF_Attr     = 288 + 1,
    CANFD0_Attr  = 288 + 2,
    CANFD1_Attr  = 288 + 4,
    ETMC_Attr    = 288 + 6,
    SWDH_Attr    = 288 + 7,
    SWODEC_Attr  = 288 + 8,
    EBI_Attr     = 288 + 16,

    /******  SCU_D1PNS2 **********************************************************************************/
    WWDT0_Attr   = 320 + 0,
    EADC0_Attr   = 320 + 1,
    EPWM0_Attr   = 320 + 2,
    BPWM0_Attr   = 320 + 3,
    EQEI0_Attr   = 320 + 4,
    EQEI2_Attr   = 320 + 5,
    ECAP0_Attr   = 320 + 6,
    ECAP2_Attr   = 320 + 7,
    I2C0_Attr    = 320 + 8,
    I2C2_Attr    = 320 + 9,
    QSPI0_Attr   = 320 + 10,
    SPI0_Attr    = 320 + 11,
    SPI2_Attr    = 320 + 12,
    UART0_Attr   = 320 + 13,
    UART2_Attr   = 320 + 14,
    UART4_Attr   = 320 + 15,
    UART6_Attr   = 320 + 16,
    UART8_Attr   = 320 + 17,
    USCI0_Attr   = 320 + 18,
    SC0_Attr     = 320 + 19,
    SC2_Attr     = 320 + 20,
    PSIO_Attr    = 320 + 21,
    TMR01_Attr   = 320 + 22,
    DAC01_Attr   = 320 + 23,
    HSOTG_Attr   = 320 + 25,
    I2S0_Attr    = 320 + 26,
    ACMP01_Attr  = 320 + 27,
    USBD_Attr    = 320 + 28,

    /******  SCU_D1PNS4 **********************************************************************************/
    WWDT1_Attr   = 384 + 0,
    EADC1_Attr   = 384 + 1,
    EPWM1_Attr   = 384 + 2,
    BPWM1_Attr   = 384 + 3,
    EQEI1_Attr   = 384 + 4,
    EQEI3_Attr   = 384 + 5,
    ECAP1_Attr   = 384 + 6,
    ECAP3_Attr   = 384 + 7,
    I2C1_Attr    = 384 + 8,
    I2C3_Attr    = 384 + 9,
    QSPI1_Attr   = 384 + 10,
    SPI1_Attr    = 384 + 11,
    SPI3_Attr    = 384 + 12,
    UART1_Attr   = 384 + 13,
    UART3_Attr   = 384 + 14,
    UART5_Attr   = 384 + 15,
    UART7_Attr   = 384 + 16,
    UART9_Attr   = 384 + 17,
    SC1_Attr     = 384 + 18,
    OTG_Attr     = 384 + 19,
    KPI_Attr     = 384 + 20,
    TMR23_Attr   = 384 + 21,
    TRNG_Attr    = 384 + 22,
    RTC_Attr     = 384 + 23,
    I2S1_Attr    = 384 + 24,
    ACMP23_Attr  = 384 + 25,
    I3C_Attr     = 384 + 26,
    UTCPD_Attr   = 384 + 27,

    /******  SCU_D2PNS0 **********************************************************************************/
    LPDMA_Attr   = 512 + 0,
    CCAP_Attr    = 512 + 1,
    LPGPIO_Attr  = 512 + 3,

    /******  SCU_D2PNS2 **********************************************************************************/
    LPTMR01_Attr  = 576 + 0,
    TTMR01_Attr   = 576 + 1,
    LPADC0_Attr   = 576 + 2,
    LPI2C0_Attr   = 576 + 3,
    LPSPI0_Attr   = 576 + 4,
    DMIC0_Attr    = 576 + 5,
    LPUART0_Attr  = 576 + 6,
    AWF_Attr      = 576 + 9,
} NSATTR_T;


/** @} end of group SCU_EXPORTED_CONSTANTS */


/** @addtogroup SCU_EXPORTED_FUNCTIONS SCU Exported Functions
    @{
*/

/**
  * @brief      Set peripheral non-secure attribution
  *
  * @param[in]  nsattr     The secure/non-secure attribution of specified module.
                           The possible value could be refer to \ref NSATTR.
  *
  * @return     None
  *
  * @details    This macro is used to set a peripheral to be non-secure peripheral.
  *
  */
#define SCU_SET_PNSSET(nsattr)   { SCU->PNSSET[(nsattr)/32] |= (1 << ((nsattr) & 0x1ful)); }

/**
 * @brief       Get peripheral secure/non-secure attribution
 *
 * @param[in]  nsattr     The secure/non-secure attribution of specified module.
                           The possible value could be refer to \ref NSATTR.
 *
 * @return      The secure/non-secure attribution of specified peripheral.
 * @retval      0 The peripheral is secure
 * @retval      1 The peripheral is non-secure
 *
 * @details     This macro gets the peripheral secure/non-secure attribution.
 */
#define SCU_GET_PNSSET(nsattr)   ((SCU->PNSSET[(nsattr)/32] >> ((nsattr) & 0x1ful)) & 1ul)


/**
 * @brief       Set secure/non-secure attribution of specified GPIO pin
 *
 * @param[in]   port        GPIO Port. It could be PA, PB, PC, PD, PE, PF, PG and PH.
 * @param[in]   bitmask     Bit mask of each bit. 0 is secure. 1 is non-secure.
 *
 * @return      None
 *
 * @details     This macro sets GPIO pin secure/non-secure attribution.
 */
#define SCU_SET_IONSSET(port, mask)   (SCU->IONSSET[((uint32_t)(port)-(GPIOA_BASE))/0x40] = (mask))


/**
 * @brief       Get secure/non-secure attribution of specified GPIO port
 *
 * @param[in]   port        GPIO Port. It could be PA, PB, PC, PD, PE, PF, PG and PH.
 *
 * @return      The secure/non-secure attribution of the port.
 * @retval      0 The relative bit of specified IO port is secure
 * @retval      1 The relative bit of specified IO port is non-secure
 *
 * @details     This macro gets IO secure/non-secure attribution of specified IO port.
 */
#define SCU_GET_IONSSET(port)   (SCU->IONSSET[((uint32_t)(port) - (GPIOA_BASE))/0x40])


/**
 * @brief       Enable sercure violation interrupts
 *
 * @param[in]   mask    The mask of each secure violation interrupt source
 *              - \ref SCU_SVIOIEN_APB0IEN_Msk
 *              - \ref SCU_SVIOIEN_APB1IEN_Msk
 *              - \ref SCU_SVIOIEN_GPIOIEN_Msk
 *              - \ref SCU_SVIOIEN_EBIIEN_Msk
 *              - \ref SCU_SVIOIEN_USBHIEN_Msk
 *              - \ref SCU_SVIOIEN_CRCIEN_Msk
 *              - \ref SCU_SVIOIEN_SDH0IEN_Msk
 *              - \ref SCU_SVIOIEN_PDMA0IEN_Msk
 *              - \ref SCU_SVIOIEN_PDMA1IEN_Msk
 *              - \ref SCU_SVIOIEN_SRAM0IEN_Msk
 *              - \ref SCU_SVIOIEN_SRAM1IEN_Msk
 *              - \ref SCU_SVIOIEN_FMCIEN_Msk
 *              - \ref SCU_SVIOIEN_FLASHIEN_Msk
 *              - \ref SCU_SVIOIEN_SCUIEN_Msk
 *              - \ref SCU_SVIOIEN_SYSIEN_Msk
 *              - \ref SCU_SVIOIEN_CRPTIEN_Msk
 *
 * @return      None
 *
 * @details     This macro is used to enable secure violation interrupt of SCU.
 *              The secure violation interrupt could be used to detect attack of secure elements.
 */
#define SCU_ENABLE_INT(mask)    (SCU->SVIOIEN |= (mask))


/**
 * @brief       Disable sercure violation interrupts
 *
 * @param[in]   mask    The mask of each secure violation interrupt source
 *              - \ref SCU_SVIOIEN_APB0IEN_Msk
 *              - \ref SCU_SVIOIEN_APB1IEN_Msk
 *              - \ref SCU_SVIOIEN_GPIOIEN_Msk
 *              - \ref SCU_SVIOIEN_EBIIEN_Msk
 *              - \ref SCU_SVIOIEN_USBHIEN_Msk
 *              - \ref SCU_SVIOIEN_CRCIEN_Msk
 *              - \ref SCU_SVIOIEN_SDH0IEN_Msk
 *              - \ref SCU_SVIOIEN_PDMA0IEN_Msk
 *              - \ref SCU_SVIOIEN_PDMA1IEN_Msk
 *              - \ref SCU_SVIOIEN_SRAM0IEN_Msk
 *              - \ref SCU_SVIOIEN_SRAM1IEN_Msk
 *              - \ref SCU_SVIOIEN_FMCIEN_Msk
 *              - \ref SCU_SVIOIEN_FLASHIEN_Msk
 *              - \ref SCU_SVIOIEN_SCUIEN_Msk
 *              - \ref SCU_SVIOIEN_SYSIEN_Msk
 *              - \ref SCU_SVIOIEN_CRPTIEN_Msk
 *
 * @return      None
 *
 * @details     This macro is used to disable secure violation interrupt of SCU.
 *
 */
#define SCU_DISABLE_INT(mask)    (SCU->SVIOIEN &= (~(mask)))


/**
  * @brief    Get secure violation interrupt status
  *
  * @param    mask  The interrupt flag mask bit
  *
  * @return   The value of SCU_SVINTSTS register
  *
  * @details  Return interrupt flag of SCU_SVINTSTS register.
  *
  */
#define SCU_GET_INT_FLAG(mask)         (SCU->SVINTSTS&(mask))

/**
  * @brief      Clear secure violation interrupt flag
  *
  * @param[in]  flag The combination of the specified interrupt flags.
  *             Each bit corresponds to a interrupt source.
  *             This parameter decides which interrupt flags will be cleared.
  *             - \ref SCU_SVINTSTS_APB0IF_Msk
  *             - \ref SCU_SVINTSTS_APB1IF_Msk
  *             - \ref SCU_SVINTSTS_GPIOIF_Msk
  *             - \ref SCU_SVINTSTS_EBIIF_Msk
  *             - \ref SCU_SVINTSTS_USBHIF_Msk
  *             - \ref SCU_SVINTSTS_CRCIF_Msk
  *             - \ref SCU_SVINTSTS_SDH0IF_Msk
  *             - \ref SCU_SVINTSTS_PDMA0IF_Msk
  *             - \ref SCU_SVINTSTS_PDMA1IF_Msk
  *             - \ref SCU_SVINTSTS_SRAM0IF_Msk
  *             - \ref SCU_SVINTSTS_SRAM1IF_Msk
  *             - \ref SCU_SVINTSTS_FMCIF_Msk
  *             - \ref SCU_SVINTSTS_FLASHIF_Msk
  *             - \ref SCU_SVINTSTS_SCUIF_Msk
  *             - \ref SCU_SVINTSTS_SYSIF_Msk
  *             - \ref SCU_SVINTSTS_CRPTIF_Msk
  *
  * @return     None
  *
  * @details    Clear SCU related interrupt flags specified by flag parameter.
  *
  */
#define SCU_CLR_INT_FLAG(flag)     (SCU->SVINTSTS = (flag))



/**
  * @brief      Control the behavior of non-secure monitor when CPU is in idle state.
  *
  * @param[in]  opt Option for behavior control of non-secure monitor when CPU in idle.
  *              - true     The counter keeps counting when CPU is in idle.
                 - false    The counter will stop when CPU is in idle.
  *
  * @return     None
  *
  * @details    To control non-secure monitor counter when CPU is in idle.
  *
  */
#define SCU_NSM_IDLE_ON(opt)    ((opt)?(SCU->NSMCTL |= SCU_NSMCTL_IDLEON_Msk):(SCU->NSMCTL &= ~SCU_NSMCTL_IDLEON_Msk))

/**
  * @brief      Control the behavior of non-secure monitor when CPU is in debug state.
  *
  * @param[in]  opt Option for behavior control of non-secure monitor when CPU in debug.
  *              - true     The counter keeps counting when CPU is in debug.
                 - false    The counter will stop when CPU is in debug.
  *
  * @return     None
  *
  * @details    To control non-secure monitor counter when CPU is in debug.
  *
  */
#define SCU_NSM_DBG_ON(opt)    ((opt)?(SCU->NSMCTL |= SCU_NSMCTL_DBGON_Msk):(SCU->NSMCTL &= ~SCU_NSMCTL_DBGON_Msk))


/* Declare these inline functions here to avoid MISRA C 2004 rule 8.1 error */
__STATIC_INLINE void SCU_NSMConfig(uint32_t u32Ticks, uint32_t u32Prescale);
__STATIC_INLINE void SCU_TimerConfig(uint32_t u32Ticks, uint32_t u32Prescale);


/**
  * @brief      Config non-secure monitor to detect timeout in non-secure state.
  *
  * @param[in]  u32Ticks       A specified period for timeout in non-secure state
  * @param[in]  u32Prescale    A pre-scale divider to non-secure monitor clock

  *
  * @return     None
  *
  * @details    This function is used to configure non-secure monitor. If the CPU state stay in non-secure state for
  *             a specified period. The non-secure monitor will timeout and assert an interrupt. Otherwise, the
  *             non-secure monitor will auto clear whenever returning to secure state. This could be used to avoid
  *             CPU state in non-secure state too long time for security purpose. User must enable SCU_IRQn if interrupt
  *             is necessary.
  *
  */
__STATIC_INLINE void SCU_NSMConfig(uint32_t u32Ticks, uint32_t u32Prescale)
{

    SCU->NSMLOAD = u32Ticks;
    SCU->NSMVAL  = 0ul;
    SCU->NSMCTL  = SCU_NSMCTL_AUTORLD_Msk | SCU_NSMCTL_NSMIEN_Msk | (u32Prescale & 0xfful);
}

/**
  * @brief      Config non-secure monitor to be a timer.
  *
  * @param[in]  u32Ticks       A specified period for timer interrupt.
  * @param[in]  u32Prescale    A pre-scale divider to timer clock source.

  *
  * @return     None
  *
  * @details    This function is used to configure non-secure monitor as a timer. In other words, the timer counter
  *             keeps counting even CPU is in secure state.
  *
  */
__STATIC_INLINE void SCU_TimerConfig(uint32_t u32Ticks, uint32_t u32Prescale)
{

    SCU->NSMLOAD = u32Ticks;
    SCU->NSMVAL  = 0ul;
    SCU->NSMCTL  = SCU_NSMCTL_AUTORLD_Msk | SCU_NSMCTL_NSMIEN_Msk | SCU_NSMCTL_TMRMOD_Msk | (u32Prescale & 0xfful);
}

int SetupMPC(const uint32_t mpc_baseaddr,
             const uint32_t mem_baseaddr, const uint32_t mem_size,
             const uint32_t baseaddr_s,  /* Secure base address */
             const uint32_t len_s,       /* Length (in bytes) of secure region */
             const uint32_t baseaddr_ns, /* Non-secure base address */
             const uint32_t len_ns);      /* Length (in bytes) of non-secure region */
/** @} end of group SCU_EXPORTED_FUNCTIONS */
/** @} end of group SCU_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __SCU_H__ */
