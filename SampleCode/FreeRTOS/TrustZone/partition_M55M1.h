/**************************************************************************//**
 * @file     partition_M55M1.h
 * @version  V1.00
 * @brief    Initial Setup for Secure / Non-Secure Attribution Zones for M55M1
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#ifndef PARTITION_M55M1_H
#define PARTITION_M55M1_H

/* Copy template from Library\StdDriver\inc\partition_M55M1.h */

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
*/

/*--------------------- MPC Configuration ----------------------------
; <h> Memory Protection Controller (MPC)
 *---------------------------------------------------------------------*/
/*
// Bit 0..18
// <o.0..18> Secure SRAM Size              <0x000000=>    0 KB
//                                         <0x008000=>   32 KB
//                                         <0x010000=>   64 KB
//                                         <0x018000=>   96 KB
//                                         <0x020000=>  128 KB
//                                         <0x028000=>  160 KB
//                                         <0x030000=>  192 KB
//                                         <0x038000=>  224 KB
//                                         <0x040000=>  256 KB
//                                         <0x048000=>  288 KB
//                                         <0x050000=>  320 KB
//                                         <0x058000=>  352 KB
//                                         <0x060000=>  384 KB
//                                         <0x068000=>  416 KB
//                                         <0x070000=>  448 KB
//                                         <0x078000=>  480 KB
//                                         <0x080000=>  512 KB
//                                         <0x088000=>  544 KB
//                                         <0x090000=>  576 KB
//                                         <0x098000=>  608 KB
//                                         <0x0A0000=>  640 KB
//                                         <0x0A8000=>  672 KB
//                                         <0x0B0000=>  704 KB
//                                         <0x0B8000=>  736 KB
//                                         <0x0C0000=>  768 KB
//                                         <0x0C8000=>  800 KB
//                                         <0x0D0000=>  832 KB
//                                         <0x0D8000=>  864 KB
//                                         <0x0E0000=>  896 KB
//                                         <0x0E8000=>  928 KB
//                                         <0x0F0000=>  960 KB
//                                         <0x0F8000=>  992 KB
//                                         <0x100000=> 1024 KB
//                                         <0x108000=> 1056 KB
//                                         <0x110000=> 1088 KB
//                                         <0x118000=> 1120 KB
//                                         <0x120000=> 1152 KB
//                                         <0x128000=> 1184 KB
//                                         <0x130000=> 1216 KB
//                                         <0x138000=> 1248 KB
//                                         <0x140000=> 1280 KB
//                                         <0x148000=> 1312 KB
//                                         <0x150000=> 1344 KB
//                                         <0x158000=> 1376 KB
//                                         <0x160000=> 1408 KB
//                                         <0x168000=> 1440 KB
//                                         <0x170000=> 1472 KB
//                                         <0x178000=> 1504 KB
//                                         <0x180000=> 1536 KB

*/
#define SCU_SECURE_SRAM_SIZE      0x20000
#define NON_SECURE_SRAM_BASE      (0x30100000 + SCU_SECURE_SRAM_SIZE)

/*
// <o> Secure SPI Flash Size <0x20000-0x2000000:0x20000>
*/
#define SCU_SECURE_SPIFLASH_SIZE      0x20000

// </h>

/*--------------------------------------------------------------------------------------------------------*/
/*
    NSCBA
*/
// <e> Enable NSCBA
#define FMC_INIT_NSCBA          1

/*
// <o> Secure Flash ROM Size <0x2000-0x200000:0x2000>
*/

#define FMC_SECURE_REGION_SIZE  0x100000
// </e>

#define FMC_SECURE_END          (FMC_APROM_BASE + FMC_SECURE_REGION_SIZE)
#define FMC_NON_SECURE_BASE     (0x10100000 + FMC_SECURE_REGION_SIZE)

/*--------------------------------------------------------------------------------------------------------*/

/*--------------------- ITCM Configuration ----------------------------
; <h> ITCM Configuration
 *---------------------------------------------------------------------*/
/* Total ITCM Size */
/* 
// <o.0..17> Total ITCM Size
//  <0x0=>        0 KB
//  <0x8000=>    32 KB
//  <0x10000=>   64 KB
//  <0x20000=>  128 KB
*/
#define CFG_ITCM_SIZE       0x00010000

/* Secure ITCM Size */
/*
// <o.0..17> Secure ITCM Size
//  <0x0=>        0 KB
//  <0x8000=>    32 KB
//  <0x10000=>   64 KB
//  <0x20000=>  128 KB
*/
#define SECURE_ITCM_SIZE    0x00008000

// </h>
/*----------------------------------------------------------------------------*/

/*--------------------- DTCM Configuration ----------------------------
; <h> DTCM Configuration
 *---------------------------------------------------------------------*/
/* Total DTCM Size */
/* 
// <o.0..17> Total DTCM Size
//  <0x0=>        0 KB
//  <0x8000=>    32 KB
//  <0x10000=>   64 KB
//  <0x20000=>  128 KB
*/
#define CFG_DTCM_SIZE       0x00020000

/* Secure DTCM Size */
/*
// <o.0..17> Secure DTCM Size
//  <0x0=>        0 KB
//  <0x8000=>    32 KB
//  <0x10000=>   64 KB
//  <0x20000=>  128 KB
*/
#define SECURE_DTCM_SIZE    0x00010000

// </h>
/*----------------------------------------------------------------------------*/

/*
// <h> Domain 0 Peripheral Secure Attribution
*/

/* SCU_D0PNS0 */
/*
// Module 0..31
//   <o.03>  NPU            <0=> Secure <1=> Non-Secure
*/
#define SCU_INIT_D0PNS0_VAL       0x0

/* SCU_D0PNS2 */
/*
// Module 0..31
//   <o.02>  SPIM0          <0=> Secure <1=> Non-Secure
//   <o.03>  SPIM1          <0=> Secure <1=> Non-Secure
*/
#define SCU_INIT_D0PNS2_VAL       0x0

/* SCU_D1PNS0 */
/*
// Module 0..31
//   <o.00>  PDMA0          <0=> Secure <1=> Non-Secure
//   <o.01>  PDMA1          <0=> Secure <1=> Non-Secure
//   <o.02>  USBH0          <0=> Secure <1=> Non-Secure
//   <o.03>  USBH1          <0=> Secure <1=> Non-Secure
//   <o.04>  HSUSBH         <0=> Secure <1=> Non-Secure
//   <o.05>  HSUSBD         <0=> Secure <1=> Non-Secure
//   <o.06>  SDH0           <0=> Secure <1=> Non-Secure
//   <o.07>  SDH1           <0=> Secure <1=> Non-Secure
//   <o.08>  EMAC0          <0=> Secure <1=> Non-Secure
//   <o.10>  CRYPTO         <0=> Secure <1=> Non-Secure
//   <o.11>  CRC            <0=> Secure <1=> Non-Secure
*/
#define SCU_INIT_D1PNS0_VAL       0x0
/*
// </h>
*/

/*
// <h> Domain 1 Peripheral Secure Attribution
*/
/* SCU_D1PNS1 */
/*
// Module 0..31
//   <o.01>  KDF            <0=> Secure <1=> Non-Secure
//   <o.02>  CANFD0         <0=> Secure <1=> Non-Secure
//   <o.03>  CANFD1         <0=> Secure <1=> Non-Secure
//   <o.04>  ETMC           <0=> Secure <1=> Non-Secure
//   <o.05>  SWDH           <0=> Secure <1=> Non-Secure
//   <o.06>  SWODEC         <0=> Secure <1=> Non-Secure
//   <o.16>  EBI            <0=> Secure <1=> Non-Secure
*/
#define SCU_INIT_D1PNS1_VAL       0x0

/* SCU_D1PNS2 */
/*
// Module 0..31
//   <o.00>  WWDT0          <0=> Secure <1=> Non-Secure
//   <o.01>  EADC0          <0=> Secure <1=> Non-Secure
//   <o.02>  EPWM0          <0=> Secure <1=> Non-Secure
//   <o.03>  BPWM0          <0=> Secure <1=> Non-Secure
//   <o.04>  EQEI0          <0=> Secure <1=> Non-Secure
//   <o.05>  EQEI2          <0=> Secure <1=> Non-Secure
//   <o.06>  ECAP0          <0=> Secure <1=> Non-Secure
//   <o.07>  ECAP2          <0=> Secure <1=> Non-Secure
//   <o.08>  I2C0           <0=> Secure <1=> Non-Secure
//   <o.09>  I2C2           <0=> Secure <1=> Non-Secure
//   <o.10>  QSPI0          <0=> Secure <1=> Non-Secure
//   <o.11>  SPI0           <0=> Secure <1=> Non-Secure
//   <o.12>  SPI2           <0=> Secure <1=> Non-Secure
//   <o.13>  UART0          <0=> Secure <1=> Non-Secure
//   <o.14>  UART2          <0=> Secure <1=> Non-Secure
//   <o.15>  UART4          <0=> Secure <1=> Non-Secure
//   <o.16>  UART6          <0=> Secure <1=> Non-Secure
//   <o.17>  UART8          <0=> Secure <1=> Non-Secure
//   <o.18>  USCI0          <0=> Secure <1=> Non-Secure
//   <o.19>  SC0            <0=> Secure <1=> Non-Secure
//   <o.20>  SC2            <0=> Secure <1=> Non-Secure
//   <o.21>  PSIO           <0=> Secure <1=> Non-Secure
//   <o.22>  TMR01          <0=> Secure <1=> Non-Secure
//   <o.23>  DAC01          <0=> Secure <1=> Non-Secure
//   <o.25>  HSOTG          <0=> Secure <1=> Non-Secure
//   <o.26>  I2S            <0=> Secure <1=> Non-Secure
//   <o.27>  ACMP01         <0=> Secure <1=> Non-Secure
//   <o.28>  USBD           <0=> Secure <1=> Non-Secure
*/
#define SCU_INIT_D1PNS2_VAL       0x0

/* SCU_D1PNS4 */
/*
// Module 0..31
//   <o.00>  WWDT1          <0=> Secure <1=> Non-Secure
//   <o.01>  EADC1          <0=> Secure <1=> Non-Secure
//   <o.02>  EPWM1          <0=> Secure <1=> Non-Secure
//   <o.03>  BPWM1          <0=> Secure <1=> Non-Secure
//   <o.04>  EQEI1          <0=> Secure <1=> Non-Secure
//   <o.05>  EQEI3          <0=> Secure <1=> Non-Secure
//   <o.06>  ECAP1          <0=> Secure <1=> Non-Secure
//   <o.07>  ECAP3          <0=> Secure <1=> Non-Secure
//   <o.08>  I2C1           <0=> Secure <1=> Non-Secure
//   <o.09>  I2C3           <0=> Secure <1=> Non-Secure
//   <o.10>  QSPI1          <0=> Secure <1=> Non-Secure
//   <o.11>  SPI1           <0=> Secure <1=> Non-Secure
//   <o.12>  SPI3           <0=> Secure <1=> Non-Secure
//   <o.13>  UART1          <0=> Secure <1=> Non-Secure
//   <o.14>  UART3          <0=> Secure <1=> Non-Secure
//   <o.15>  UART5          <0=> Secure <1=> Non-Secure
//   <o.16>  UART7          <0=> Secure <1=> Non-Secure
//   <o.17>  UART9          <0=> Secure <1=> Non-Secure
//   <o.18>  SC1            <0=> Secure <1=> Non-Secure
//   <o.19>  OTG            <0=> Secure <1=> Non-Secure
//   <o.20>  KPI            <0=> Secure <1=> Non-Secure
//   <o.21>  TMR23          <0=> Secure <1=> Non-Secure
//   <o.22>  TRNG           <0=> Secure <1=> Non-Secure
//   <o.23>  RTC            <0=> Secure <1=> Non-Secure
//   <o.24>  I2C0           <0=> Secure <1=> Non-Secure
//   <o.25>  ACMP23         <0=> Secure <1=> Non-Secure
//   <o.26>  I3C0           <0=> Secure <1=> Non-Secure
//   <o.27>  UTCPD          <0=> Secure <1=> Non-Secure
*/
#define SCU_INIT_D1PNS4_VAL       0x2000
/*
// </h>
*/

/*
// <h> Domain 2 Peripheral Secure Attribution
*/
/* SCU_D2PNS0 */
/*
// Module 0..31
//   <o.00>  LPDMA          <0=> Secure <1=> Non-Secure
//   <o.01>  CCAP           <0=> Secure <1=> Non-Secure
//   <o.03>  LPGPIO         <0=> Secure <1=> Non-Secure
*/
#define SCU_INIT_D2PNS0_VAL       0x0

/* SCU_D2PNS2 */
/*
// Module 0..31
//   <o.00>  LPTMR01        <0=> Secure <1=> Non-Secure
//   <o.01>  TTMR01         <0=> Secure <1=> Non-Secure
//   <o.02>  LPADC0         <0=> Secure <1=> Non-Secure
//   <o.03>  LPI2C0         <0=> Secure <1=> Non-Secure
//   <o.04>  LPSPI0         <0=> Secure <1=> Non-Secure
//   <o.05>  DMIC0          <0=> Secure <1=> Non-Secure
//   <o.06>  LPUART0        <0=> Secure <1=> Non-Secure
//   <o.07>  WDT0           <0=> Secure <1=> Non-Secure
//   <o.08>  WDT1           <0=> Secure <1=> Non-Secure
//   <o.09>  AWF            <0=> Secure <1=> Non-Secure
*/
#define SCU_INIT_D2PNS2_VAL       0x0
/*
// </h>
*/



/*
// <h> GPIO Pin Secure Attribution
*/

/*
    IONSx
*/

/*
// Bit 0..31
//   <h> PA
//   <o.0 >  PA0       <0=> Secure <1=> Non-Secure
//   <o.1 >  PA1       <0=> Secure <1=> Non-Secure
//   <o.2 >  PA2       <0=> Secure <1=> Non-Secure
//   <o.3 >  PA3       <0=> Secure <1=> Non-Secure
//   <o.4 >  PA4       <0=> Secure <1=> Non-Secure
//   <o.5 >  PA5       <0=> Secure <1=> Non-Secure
//   <o.6 >  PA6       <0=> Secure <1=> Non-Secure
//   <o.7 >  PA7       <0=> Secure <1=> Non-Secure
//   <o.8 >  PA8       <0=> Secure <1=> Non-Secure
//   <o.9 >  PA9       <0=> Secure <1=> Non-Secure
//   <o.10>  PA10      <0=> Secure <1=> Non-Secure
//   <o.11>  PA11      <0=> Secure <1=> Non-Secure
//   <o.12>  PA12      <0=> Secure <1=> Non-Secure
//   <o.13>  PA13      <0=> Secure <1=> Non-Secure
//   <o.14>  PA14      <0=> Secure <1=> Non-Secure
//   <o.15>  PA15      <0=> Secure <1=> Non-Secure
//   </h>

*/
#define SCU_INIT_IONS0_VAL      0x00000000

/*
// Bit 0..31
//   <h> PB
//   <o.0 >  PB0       <0=> Secure <1=> Non-Secure
//   <o.1 >  PB1       <0=> Secure <1=> Non-Secure
//   <o.2 >  PB2       <0=> Secure <1=> Non-Secure
//   <o.3 >  PB3       <0=> Secure <1=> Non-Secure
//   <o.4 >  PB4       <0=> Secure <1=> Non-Secure
//   <o.5 >  PB5       <0=> Secure <1=> Non-Secure
//   <o.6 >  PB6       <0=> Secure <1=> Non-Secure
//   <o.7 >  PB7       <0=> Secure <1=> Non-Secure
//   <o.8 >  PB8       <0=> Secure <1=> Non-Secure
//   <o.9 >  PB9       <0=> Secure <1=> Non-Secure
//   <o.10>  PB10      <0=> Secure <1=> Non-Secure
//   <o.11>  PB11      <0=> Secure <1=> Non-Secure
//   <o.12>  PB12      <0=> Secure <1=> Non-Secure
//   <o.13>  PB13      <0=> Secure <1=> Non-Secure
//   <o.14>  PB14      <0=> Secure <1=> Non-Secure
//   <o.15>  PB15      <0=> Secure <1=> Non-Secure
//   </h>
*/
#define SCU_INIT_IONS1_VAL      0x00000000


/*
// Bit 0..31
//   <h> PC
//   <o.0 >  PC0       <0=> Secure <1=> Non-Secure
//   <o.1 >  PC1       <0=> Secure <1=> Non-Secure
//   <o.2 >  PC2       <0=> Secure <1=> Non-Secure
//   <o.3 >  PC3       <0=> Secure <1=> Non-Secure
//   <o.4 >  PC4       <0=> Secure <1=> Non-Secure
//   <o.5 >  PC5       <0=> Secure <1=> Non-Secure
//   <o.6 >  PC6       <0=> Secure <1=> Non-Secure
//   <o.7 >  PC7       <0=> Secure <1=> Non-Secure
//   <o.8 >  PC8       <0=> Secure <1=> Non-Secure
//   <o.9 >  PC9       <0=> Secure <1=> Non-Secure
//   <o.10>  PC10      <0=> Secure <1=> Non-Secure
//   <o.11>  PC11      <0=> Secure <1=> Non-Secure
//   <o.12>  PC12      <0=> Secure <1=> Non-Secure
//   <o.13>  PC13      <0=> Secure <1=> Non-Secure
//   <o.14>  PC14      <0=> Secure <1=> Non-Secure
//   </h>
*/
#define SCU_INIT_IONS2_VAL      0x00000003

/*
// Bit 0..31
//   <h> PD
//   <o.0 >  PD0       <0=> Secure <1=> Non-Secure
//   <o.1 >  PD1       <0=> Secure <1=> Non-Secure
//   <o.2 >  PD2       <0=> Secure <1=> Non-Secure
//   <o.3 >  PD3       <0=> Secure <1=> Non-Secure
//   <o.4 >  PD4       <0=> Secure <1=> Non-Secure
//   <o.5 >  PD5       <0=> Secure <1=> Non-Secure
//   <o.6 >  PD6       <0=> Secure <1=> Non-Secure
//   <o.7 >  PD7       <0=> Secure <1=> Non-Secure
//   <o.8 >  PD8       <0=> Secure <1=> Non-Secure
//   <o.9 >  PD9       <0=> Secure <1=> Non-Secure
//   <o.10>  PD10      <0=> Secure <1=> Non-Secure
//   <o.11>  PD11      <0=> Secure <1=> Non-Secure
//   <o.12>  PD12      <0=> Secure <1=> Non-Secure
//   <o.13>  PD13      <0=> Secure <1=> Non-Secure
//   <o.14>  PD14      <0=> Secure <1=> Non-Secure
//   </h>
*/
#define SCU_INIT_IONS3_VAL      0x00000000


/*
// Bit 0..31
//   <h> PE
//   <o.0 >  PE0       <0=> Secure <1=> Non-Secure
//   <o.1 >  PE1       <0=> Secure <1=> Non-Secure
//   <o.2 >  PE2       <0=> Secure <1=> Non-Secure
//   <o.3 >  PE3       <0=> Secure <1=> Non-Secure
//   <o.4 >  PE4       <0=> Secure <1=> Non-Secure
//   <o.5 >  PE5       <0=> Secure <1=> Non-Secure
//   <o.6 >  PE6       <0=> Secure <1=> Non-Secure
//   <o.7 >  PE7       <0=> Secure <1=> Non-Secure
//   <o.8 >  PE8       <0=> Secure <1=> Non-Secure
//   <o.9 >  PE9       <0=> Secure <1=> Non-Secure
//   <o.10>  PE10      <0=> Secure <1=> Non-Secure
//   <o.11>  PE11      <0=> Secure <1=> Non-Secure
//   <o.12>  PE12      <0=> Secure <1=> Non-Secure
//   <o.13>  PE13      <0=> Secure <1=> Non-Secure
//   <o.14>  PE14      <0=> Secure <1=> Non-Secure
//   <o.15>  PE15      <0=> Secure <1=> Non-Secure
//   </h>
*/
#define SCU_INIT_IONS4_VAL      0x00000000


/*
// Bit 0..31
//   <h> PF
//   <o.0 >  PF0       <0=> Secure <1=> Non-Secure
//   <o.1 >  PF1       <0=> Secure <1=> Non-Secure
//   <o.2 >  PF2       <0=> Secure <1=> Non-Secure
//   <o.3 >  PF3       <0=> Secure <1=> Non-Secure
//   <o.4 >  PF4       <0=> Secure <1=> Non-Secure
//   <o.5 >  PF5       <0=> Secure <1=> Non-Secure
//   <o.6 >  PF6       <0=> Secure <1=> Non-Secure
//   <o.7 >  PF7       <0=> Secure <1=> Non-Secure
//   <o.8 >  PF8       <0=> Secure <1=> Non-Secure
//   <o.9 >  PF9       <0=> Secure <1=> Non-Secure
//   <o.10>  PF10      <0=> Secure <1=> Non-Secure
//   <o.11>  PF11      <0=> Secure <1=> Non-Secure
//   </h>
*/
#define SCU_INIT_IONS5_VAL      0x00000000


/*
// Bit 0..31
//   <h> PG
//   <o.0 >  PG0       <0=> Secure <1=> Non-Secure
//   <o.1 >  PG1       <0=> Secure <1=> Non-Secure
//   <o.2 >  PG2       <0=> Secure <1=> Non-Secure
//   <o.3 >  PG3       <0=> Secure <1=> Non-Secure
//   <o.4 >  PG4       <0=> Secure <1=> Non-Secure
//   <o.5 >  PG5       <0=> Secure <1=> Non-Secure
//   <o.6 >  PG6       <0=> Secure <1=> Non-Secure
//   <o.7 >  PG7       <0=> Secure <1=> Non-Secure
//   <o.8 >  PG8       <0=> Secure <1=> Non-Secure
//   <o.9 >  PG9       <0=> Secure <1=> Non-Secure
//   <o.10>  PG10      <0=> Secure <1=> Non-Secure
//   <o.11>  PG11      <0=> Secure <1=> Non-Secure
//   <o.12>  PG12      <0=> Secure <1=> Non-Secure
//   <o.13>  PG13      <0=> Secure <1=> Non-Secure
//   <o.14>  PG14      <0=> Secure <1=> Non-Secure
//   <o.15>  PG15      <0=> Secure <1=> Non-Secure
//   </h>
*/
#define SCU_INIT_IONS6_VAL      0x00000000

/*
// Bit 0..31
//   <h> PH
//   <o.0 >  PH0       <0=> Secure <1=> Non-Secure
//   <o.1 >  PH1       <0=> Secure <1=> Non-Secure
//   <o.2 >  PH2       <0=> Secure <1=> Non-Secure
//   <o.3 >  PH3       <0=> Secure <1=> Non-Secure
//   <o.4 >  PH4       <0=> Secure <1=> Non-Secure
//   <o.5 >  PH5       <0=> Secure <1=> Non-Secure
//   <o.6 >  PH6       <0=> Secure <1=> Non-Secure
//   <o.7 >  PH7       <0=> Secure <1=> Non-Secure
//   <o.8 >  PH8       <0=> Secure <1=> Non-Secure
//   <o.9 >  PH9       <0=> Secure <1=> Non-Secure
//   <o.10>  PH10      <0=> Secure <1=> Non-Secure
//   <o.11>  PH11      <0=> Secure <1=> Non-Secure
//   </h>
*/
#define SCU_INIT_IONS7_VAL      0x00000000

/*
// Bit 0..31
//   <h> PI
//   <o.0 >  PI0       <0=> Secure <1=> Non-Secure
//   <o.1 >  PI1       <0=> Secure <1=> Non-Secure
//   <o.2 >  PI2       <0=> Secure <1=> Non-Secure
//   <o.3 >  PI3       <0=> Secure <1=> Non-Secure
//   <o.4 >  PI4       <0=> Secure <1=> Non-Secure
//   <o.5 >  PI5       <0=> Secure <1=> Non-Secure
//   <o.6 >  PI6       <0=> Secure <1=> Non-Secure
//   <o.7 >  PI7       <0=> Secure <1=> Non-Secure
//   <o.8 >  PI8       <0=> Secure <1=> Non-Secure
//   <o.9 >  PI9       <0=> Secure <1=> Non-Secure
//   <o.10>  PI10      <0=> Secure <1=> Non-Secure
//   <o.11>  PI11      <0=> Secure <1=> Non-Secure
//   <o.12>  PI12      <0=> Secure <1=> Non-Secure
//   <o.13>  PI13      <0=> Secure <1=> Non-Secure
//   <o.14>  PI14      <0=> Secure <1=> Non-Secure
//   <o.15>  PI15      <0=> Secure <1=> Non-Secure
//   </h>
*/
#define SCU_INIT_IONS8_VAL      0x00000000

/*
// Bit 0..31
//   <h> PJ
//   <o.0 >  PJ0       <0=> Secure <1=> Non-Secure
//   <o.1 >  PJ1       <0=> Secure <1=> Non-Secure
//   <o.2 >  PJ2       <0=> Secure <1=> Non-Secure
//   <o.3 >  PJ3       <0=> Secure <1=> Non-Secure
//   <o.4 >  PJ4       <0=> Secure <1=> Non-Secure
//   <o.5 >  PJ5       <0=> Secure <1=> Non-Secure
//   <o.6 >  PJ6       <0=> Secure <1=> Non-Secure
//   <o.7 >  PJ7       <0=> Secure <1=> Non-Secure
//   <o.8 >  PJ8       <0=> Secure <1=> Non-Secure
//   <o.9 >  PJ9       <0=> Secure <1=> Non-Secure
//   <o.10>  PJ10      <0=> Secure <1=> Non-Secure
//   <o.11>  PJ11      <0=> Secure <1=> Non-Secure
//   <o.12>  PJ12      <0=> Secure <1=> Non-Secure
//   <o.13>  PJ13      <0=> Secure <1=> Non-Secure
//   </h>
*/
#define SCU_INIT_IONS9_VAL      0x00000000

/*
// </h>
*/

/*
// <h> GPIO and EINT Interrupt Secure Attribution
*/

/*
    Initialize GPIO/EINT ITNS (Interrupts 0..31)
*/

/*
// Bit 0..31
//   <o.0 >  GPA        <0=> Secure <1=> Non-Secure
//   <o.1 >  GPB        <0=> Secure <1=> Non-Secure
//   <o.2 >  GPC        <0=> Secure <1=> Non-Secure
//   <o.3 >  GPD        <0=> Secure <1=> Non-Secure
//   <o.4 >  GPE        <0=> Secure <1=> Non-Secure
//   <o.5 >  GPF        <0=> Secure <1=> Non-Secure
//   <o.6 >  GPG        <0=> Secure <1=> Non-Secure
//   <o.7 >  GPH        <0=> Secure <1=> Non-Secure
//   <o.8 >  GPI        <0=> Secure <1=> Non-Secure
//   <o.9 >  GPJ        <0=> Secure <1=> Non-Secure
//   <o.10>  EINT0      <0=> Secure <1=> Non-Secure
//   <o.11>  EINT1      <0=> Secure <1=> Non-Secure
//   <o.12>  EINT2      <0=> Secure <1=> Non-Secure
//   <o.13>  EINT3      <0=> Secure <1=> Non-Secure
//   <o.14>  EINT4      <0=> Secure <1=> Non-Secure
//   <o.15>  EINT5      <0=> Secure <1=> Non-Secure
//   <o.16>  EINT6      <0=> Secure <1=> Non-Secure
//   <o.17>  EINT7      <0=> Secure <1=> Non-Secure
*/
#define SCU_INIT_IO_ITNS_VAL      0x0000
/*
// </h>
*/

/* ---------------------------------------------------------------------------------------------------- */

/*
// <e>Secure Attribute Unit (SAU) Control
*/
#define SAU_INIT_CTRL 1

/*
//   <q> Enable SAU
//   <i> To enable Secure Attribute Unit (SAU).
*/
#define SAU_INIT_CTRL_ENABLE 1

/*
//   <o> All Memory Attribute When SAU is disabled
//     <0=> All Memory is Secure
//     <1=> All Memory is Non-Secure
//   <i> To set the ALLNS bit in SAU CTRL.
//   <i> When all Memory is Non-Secure (ALLNS is 1), IDAU can override memory map configuration.
*/
#define SAU_INIT_CTRL_ALLNS  1

/*
// </e>
*/


/*
// <h>Enable and Set Secure/Non-Secure region
*/
#define SAU_REGIONS_MAX   8                 /* Max. number of SAU regions */

/*
//   <e>SAU Region 0
//   <i> Setup SAU Region 0
*/
#define SAU_INIT_REGION0    0
/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START0     0x0003F000      /* start address of SAU region 0 */
/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END0       0x0003FFFF      /* end address of SAU region 0 */
/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC0       1
/*
//   </e>
*/

/*
//   <e>SAU Region 1
//   <i> Setup SAU Region 1
*/
#define SAU_INIT_REGION1    0
/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START1     0x10040000
/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END1       0x1007FFFF
/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC1       0
/*
//   </e>
*/

/*
//   <e>SAU Region 2
//   <i> Setup SAU Region 2
*/
#define SAU_INIT_REGION2    0
/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START2     0x2000F000
/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END2       0x2000FFFF
/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC2       1
/*
//   </e>
*/

/*
//   <e>SAU Region 3
//   <i> Setup SAU Region 3
*/
#define SAU_INIT_REGION3    0
/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START3     0x3f000
/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END3       0x3f7ff
/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC3       1
/*
//   </e>
*/

/*
   <e>SAU Region 4
   <i> Setup SAU Region 4
*/
#define SAU_INIT_REGION4    1
/*
     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START4     FMC_NON_SECURE_BASE      /* start address of SAU region 4 */

/*
     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END4       0x100FFFFF      /* end address of SAU region 4 */

/*
     <o>Region is
         <0=>Non-Secure
         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC4       0
/*
   </e>
*/

/*
   <e>SAU Region 5
   <i> Setup SAU Region 5
*/
#define SAU_INIT_REGION5    1

/*
     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START5     0x00807E00

/*
     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END5       0x00807FFF

/*
     <o>Region is
         <0=>Non-Secure
         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC5       1
/*
   </e>
*/

/*
   <e>SAU Region 6
   <i> Setup SAU Region 6
*/
#define SAU_INIT_REGION6    1

/*
     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START6     NON_SECURE_SRAM_BASE

/*
     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END6       0x3004FFFF

/*
     <o>Region is
         <0=>Non-Secure
         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC6       0
/*
   </e>
*/

/*
   <e>SAU Region 7
   <i> Setup SAU Region 7
*/
#define SAU_INIT_REGION7    1

/*
     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START7     0x50000000

/*
     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END7       0x5FFFFFFF

/*
     <o>Region is
         <0=>Non-Secure
         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC7       0
/*
   </e>
*/

/*
// </h>
*/

/*
// <e>Setup behavior of Sleep and Exception Handling
*/
#define SCB_CSR_AIRCR_INIT  1

/*
//   <o> Deep Sleep can be enabled by
//     <0=>Secure and Non-Secure state
//     <1=>Secure state only
//   <i> Value for SCB->CSR register bit DEEPSLEEPS
*/
#define SCB_CSR_DEEPSLEEPS_VAL  0

/*
//   <o>System reset request accessible from
//     <0=> Secure and Non-Secure state
//     <1=> Secure state only
//   <i> Value for SCB->AIRCR register bit SYSRESETREQS
*/
#define SCB_AIRCR_SYSRESETREQS_VAL  0

/*
//   <o>Priority of Non-Secure exceptions is
//     <0=> Not altered
//     <1=> Lowered to 0x80-0xFF
//   <i> Value for SCB->AIRCR register bit PRIS
*/
#define SCB_AIRCR_PRIS_VAL      0

/* Assign HardFault to be always secure for safe */
#define SCB_AIRCR_BFHFNMINS_VAL 0

/*
// </e>
*/

/*
    max 128 SAU regions.
    SAU regions are defined in partition.h
 */

#define SAU_INIT_REGION(n) \
    SAU->RNR  =  (n                                     & SAU_RNR_REGION_Msk); \
    SAU->RBAR =  (SAU_INIT_START##n                     & SAU_RBAR_BADDR_Msk); \
    SAU->RLAR =  (SAU_INIT_END##n                       & SAU_RLAR_LADDR_Msk) | \
                ((SAU_INIT_NSC##n << SAU_RLAR_NSC_Pos)  & SAU_RLAR_NSC_Msk)   | 1U


#endif  /* PARTITION_M55M1_H */