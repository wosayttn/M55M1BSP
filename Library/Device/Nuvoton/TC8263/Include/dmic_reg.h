/**************************************************************************//**
 * @file     dmic_reg.h
 * @version  V1.00
 * @brief    DMIC register definition header file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#ifndef __DMIC_REG_H__
#define __DMIC_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
    @addtogroup REGISTER Control Register
    @{
*/

/**
    @addtogroup DMIC Digital Microphone Inputs (DMIC)
    Memory Mapped Structure for DMIC Controller
    @{
*/

typedef struct
{

/**
 * @var DMIC_T::CTL
 * Offset: 0x00  DMIC Control Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |CHEN0     |DMIC Channel 0 Enable
 * |        |          |Set this bit to 1 to enable DMIC channel 0 operation.
 * |        |          |0 = DMIC Channel 0 Disabled.
 * |        |          |1 = DMIC Channel 0 Enabled.
 * |[1]     |CHEN1     |DMIC Channel 1 Enable
 * |        |          |Set this bit to 1 to enable DMIC channel 1 operation.
 * |        |          |0 = DMIC Channel 1 Disabled.
 * |        |          |1 = DMIC Channel 1 Enabled.
 * |[2]     |CHEN2     |DMIC Channel 2 Enable
 * |        |          |Set this bit to 1 to enable DMIC channel 2 operation.
 * |        |          |0 = DMIC Channel 2 Disabled.
 * |        |          |1 = DMIC Channel 2 Enabled.
 * |[3]     |CHEN3     |DMIC Channel 3 Enable
 * |        |          |Set this bit to 1 to enable DMIC channel 3 operation.
 * |        |          |0 = DMIC Channel 3 Disabled.
 * |        |          |1 = DMIC Channel 3 Enabled.
 * |[8]     |LCHEDGE01 |DMIC Channel 01 Data Latch Edge
 * |        |          |The data of DMIC channel 0 and channel 1 is latched on DMIC_DATA0 pin
 * |        |          |This bit is used to select the data of DMIC channel 0 and channel 1 is latched on rising or falling edge of DMIC_CLK (DMIC bus clock).
 * |        |          |0 = The data of channel 0 is latched on falling edge of DMIC_CLK
 * |        |          |The data of channel 1 is latched on rising edge of DMIC_CLK.
 * |        |          |1 = The data of channel 0 is latched on rising edge of DMIC_CLK
 * |        |          |The data of channel 1 is latched on falling edge of DMIC_CLK.
 * |[9]     |LCHEDGE23 |DMIC Channel 23 Data Latch Edge
 * |        |          |The data of DMIC channel 2 and channel 3 is latched on DMIC_DATA0 pin
 * |        |          |This bit is used to select the data of DMIC channel 2 and channel 3 is latched on rising or falling edge of DMIC_CLK (DMIC bus clock).
 * |        |          |0 = The data of channel 2 is latched on falling edge of DMIC_CLK
 * |        |          |The data of channel 3 is latched on rising edge of DMIC_CLK.
 * |        |          |1 = The data of channel 2 is latched on rising edge of DMIC_CLK
 * |        |          |The data of channel 3 is latched on falling edge of DMIC_CLK.
 * |[12]    |CH0MUTE   |DMIC Channel 0 Mute Enable
 * |        |          |Set this bit to 1 to mute DMIC channel 0.
 * |        |          |0 = DMIC Channel 0 Unmute.
 * |        |          |1 = DMIC Channel 0 Mute.
 * |[13]    |CH1MUTE   |DMIC Channel 1 Mute Enable
 * |        |          |Set this bit to 1 to mute DMIC channel 1.
 * |        |          |0 = DMIC Channel 1 Unmute.
 * |        |          |1 = DMIC Channel 1 Mute.
 * |[14]    |CH2MUTE   |DMIC Channel 2 Mute Enable
 * |        |          |Set this bit to 1 to mute DMIC channel 2.
 * |        |          |0 = DMIC Channel 2 Unmute.
 * |        |          |1 = DMIC Channel 2 Mute.
 * |[15]    |CH3MUTE   |DMIC Channel 3 Mute Enable
 * |        |          |Set this bit to 1 to mute DMIC channel 3.
 * |        |          |0 = DMIC Channel 3 Unmute.
 * |        |          |1 = DMIC Channel 3 Mute.
 * |[20]    |CH01HPFEN |DMIC Channel 01 HPF filter Enable.
 * |        |          |Set this bit to 1 to Enable DMIC channel 0 and channel 1 HPF filter for remove DC component.
 * |        |          |0 = DMIC Channel 01 HPF Disabled.
 * |        |          |1 = DMIC Channel 01 HPF Enabled.
 * |[21]    |CH23HPFEN |DMIC Channel 23 HPF filter Enable.
 * |        |          |Set this bit to 1 to Enable DMIC channel 2 and channel 3 HPF filter for remove DC component.
 * |        |          |0 = DMIC Channel 23 HPF Disabled.
 * |        |          |1 = DMIC Channel 23 HPF Enabled.
 * |[24]    |SWRST     |Internal State Software Reset.
 * |        |          |Set this bit to 1 to Clear DMIC internal state, but all DMIC registers are kept unchanged.
 * |        |          |0 = Normal operation
 * |        |          |1 = Internal DMIC State Reset
 * |[25]    |DSPFIFOT  |DSP FIFO Test
 * |        |          |Set this bit to 1 to Enable the MCU accessing of DSP FIFO
 * |        |          |Read/Write pointer will be incremented by u201C1u201D for each read/write operation.
 * |        |          |0 = Normal operation
 * |        |          |1 = DSP FIFO test Enabled.
 * |[26]    |DSPMEMT   |DSP Memory Test
 * |        |          |Set this bit to 1 to Enable the MCU accessing of DSP RAM and ROM
 * |        |          |The read/write pointer of RAM will be incremented by u201C1u201D automatically after each RAM read or write
 * |        |          |u201C0u201D to u201C1u201D state change of this bit will clear the read/write pointer automatically.
 * |        |          |0 = Normal operation
 * |        |          |1 = DSP Memory test Enabled.
 * |[28:27] |GAINSTEP  |Volume Control Gain Adjust Step for Decimal Point.
 * |        |          |00 = 0.5dB (1/2)
 * |        |          |01 = 0.25dB (1/4)
 * |        |          |10 = 0.125dB (1/8)
 * |        |          |11 = 0.0625dB (1/16)
 * |[30:29] |OSR       |DMIC OSR Setting
 * |        |          |00 = Down sample 64
 * |        |          |01 = Down sample 128
 * |        |          |10 = Down sample 256
 * |        |          |11 = Down sample 100 or 50
 * |[31]    |FIFOWIDTH |Data Effective Bit in FIFO
 * |        |          |0 = 16-bit, Data Truncated LSB bit0~bit7. [15:0]
 * |        |          |1 = 24-bit
 * @var DMIC_T::DIV
 * Offset: 0x04  DMIC Clock Divider Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[7:0]   |DMMCLKDIV |Divider to generate the DMIC Working Main Clock
 * |        |          |The value in this field is the frequency divider for generating the DMIC working main clock
 * |        |          |The frequency is obtained according to the following equation.
 * |        |          |F_DMIC_MCLK = (F_DMIC_CLK_SRC)/(1 + DMMCLKDIV)
 * |        |          |where F_DMIC_CLK_SRC is the frequency of DMIC module clock source, which is defined in the clock control register DMICSEL (CLK_CLKSEL2[11:9]) and F_DMIC_MCLK depends on the cycle of DMIC DSP processor needed, it is 49.152MHz in general.
 * |[13:8]  |DMCLKDIV  |Divider to generate the DMIC Bus Clock
 * |        |          |The value in this field is the frequency divider for generating the DMIC bus clock
 * |        |          |The frequency is obtained according to the following equation.
 * |        |          |F_DMIC_CLK = (F_DMIC_MCLK)/(1 + DMCLKDIV)
 * |        |          |where F_DMIC_MCLK is the frequency of DMIC working main clock (DMIC_MCLK) and F_DMIC_CLK is the frequency of DMIC bus clock (DMIC_CLK).
 * |[20:16] |DMTH      |FIFO Threshold Level
 * |        |          |If the valid data count of the FIFO data buffer is more than or equal to DMTH (DMIC_DIV[20:16]) setting, the DMTHIF (DMIC_STATUS[2]) bit will set to 1, else the DMTHIF (DMIC_STATUS[2]) bit will be cleared to 0.
 * |[21]    |DMTHIE    |FIFO Threshold Interrupt
 * |        |          |0 = FIFO threshold interrupt Disabled
 * |        |          |1 = FIFO threshold interrupt Enabled.
 * |[23:22] |FCLR      |FIFO Clear
 * |        |          |11 = Clear the FIFO.
 * |        |          |Others = Reserved. Do not use.
 * |        |          |Note 1: To clear the FIFO, need to write FCLR (DMIC_DIV[23:22]) to 11b, and can read the EMPTY (DMIC_STATUS[1]) bit to make sure that the FIFO has been cleared.
 * |        |          |Note 2: This field is auto cleared by hardware.
 * @var DMIC_T::STATUS
 * Offset: 0x08  DMIC Status Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |FULL      |FIFO Full Indicator (Read Only)
 * |        |          |0 = FIFO is not full.
 * |        |          |1 = FIFO is full.
 * |[1]     |EMPTY     |FIFO Empty Indicator (Read Only)
 * |        |          |0 = FIFO is not empty.
 * |        |          |1 = FIFO is empty.
 * |[2]     |DMTHIF    |FIFO Threshold Interrupt Status (Read Only)
 * |        |          |0 = The valid data count within the FIFO data buffer is less than the setting value of DMTH (DMIC_DIV[20:16]).
 * |        |          |1 = The valid data count within the FIFO data buffer is more than or equal to the setting value of TH (DMIC_DIV[20:16])
 * |[8:4]   |FIFOPTR   |FIFO Pointer (Read Only)
 * |        |          |The FULL (DMIC_STATUS[0]) and FIFOPTR (DMIC_STATUS[8:4]) indicates the field that the valid data count within the DMIC FIFO buffer.
 * |        |          |The maximum value shown in FIFOPTR (DMIC_STATUS[8:4]) is 31
 * |        |          |When the using level of DMIC FIFO buffer equal to 32, The FULL (DMIC_STATUS[0]) is set to 1.
 * |[9]     |DSP0ROMBISTFAIL|DSP0 ROM BIST TEST Failed flag (internal use )
 * |        |          |0 = DSP0 ROM bist pass, if set DSPBISTEN [30] to u201C1u201D and DSP0ROMBISTFINISH[18] is u201C1u201D
 * |        |          |1 = DSP0 ROM bist fail, if set DSPBISTEN [30] to u201C1u201D and DSP0ROMBISTFINISH [18] is u201C1u201D
 * |[10]    |DSP1ROMBISTFAIL|DSP1 ROM BIST TEST Failed flag (internal use )
 * |        |          |0 = DSP1 ROM bist pass, if set DSPBISTEN [30] to u201C1u201D and DSP1ROMBISTFINISH[19] is u201C1u201D
 * |        |          |1 = DSP1 ROM bist fail, if set DSPBISTEN [30] to u201C1u201D and DSP1ROMBISTFINISH [19] is u201C1u201D
 * |[13]    |DSP0RAMBISTFAIL|DSP0 RAM BIST TEST Failed flag (internal use )
 * |        |          |0 = DSP0 RAM bist pass, if set DSPBISTEN [30] to u201C1u201D and DSP0RAMBISTFINISH[22] is u201C1u201D
 * |        |          |1 = DSP0 RAM bist fail, if set DSPBISTEN [30] to u201C1u201D and DSP0RAMBISTFINISH [22] is u201C1u201D
 * |[14]    |DSP1RAMBISTFAIL|DSP1 RAM BIST TEST Failed flag (internal use )
 * |        |          |0 = DSP1 RAM bist pass, if set DSPBISTEN [30] to u201C1u201D and DSP1RAMBISTFINISH[23] is u201C1u201D
 * |        |          |1 = DSP1 RAM bist fail, if set DSPBISTEN [30] to u201C1u201D and DSP1RAMBISTFINISH [23] is u201C1u201D
 * |[17]    |FIFOBISTFAIL|FIFO BIST TEST Failed flag (internal use )
 * |        |          |0 = FIFO bist pass, if set FIFOBISTEN [31] to u201C1u201D and FIFOBISTFINISH[26] is u201C1u201D
 * |        |          |1 = FIFO bist fail, if set FIFOBISTEN[31] to u201C1u201D and FIFOBISTFINISH[26] is u201C1u201D
 * |[18]    |DSP0ROMBISTFINISH|DSP0 ROM BIST TEST Finish Flag (internal use)
 * |        |          |0 = Is not finishing, if set DSPBISTEN [30] to u201C1u201D
 * |        |          |1 = Finish, if set DSPBISTEN [30] to u201C1u201D
 * |[19]    |DSP1ROMBISTFINISH|DSP1 ROM BIST TEST Finish Flag (internal use)
 * |        |          |0 = Is not finishing, if set DSPBISTEN [30] to u201C1u201D
 * |        |          |1 = Finish, if set DSPBISTEN [30] to u201C1u201D
 * |[22]    |DSP0RAMBISTFINISH|DSP0 RAM BIST TEST Finish Flag (internal use)
 * |        |          |0 = Is not finishing, if set DSPBISTEN [30] to u201C1u201D
 * |        |          |1 = Finish, if set DSPBISTEN [30] to u201C1u201D
 * |[23]    |DSP1RAMBISTFINISH|DSP1 RAM BIST TEST Finish Flag (internal use)
 * |        |          |0 = Is not finishing, if set DSPBISTEN [30] to u201C1u201D
 * |        |          |1 = Finish, if set DSPBISTEN [30] to u201C1u201D
 * |[26]    |FIFOBISTFINISH|FIFO BIST TEST Finish Flag (internal use)
 * |        |          |0 = Is not finishing, if set FIFOBISTEN [31] to u201C1u201D
 * |        |          |1 = Finish, if set FIFOBISTEN [31] to u201C1u201D
 * |[30]    |DSPBISTEN |DSP BIST Test Enable (Internal use)
 * |        |          |0 = Disable DMIC DSP BIST testing.
 * |        |          |1 = Enable DMIC DSP BIST testing.
 * |[31]    |FIFOBISTEN|FIFO BIST Test Enable (Internal use)
 * |        |          |0 = Disable DMIC FIFO BIST testing.
 * |        |          |1 = Enable DMIC FIFO BIST testing.
 * @var DMIC_T::PDMACTL
 * Offset: 0x0C  DMIC PDMA Control Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |PDMAEN    |PDMA Transfer Enable Bit
 * |        |          |0 = PDMA data transfer Disabled.
 * |        |          |1 = PDMA data transfer Enabled.
 * @var DMIC_T::FIFO
 * Offset: 0x10  DMIC FIFO Data Output Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |FIFO      |FIFO Data Output Register
 * |        |          |DMIC contains 32 level (32x24 bit) data buffer for data receive
 * |        |          |A read to this register pushes data out from FIFO data buffer and decrements the read pointer
 * |        |          |This is the address that PDMA reads audio data from
 * |        |          |The remaining data word number is indicated by FIFOPTR (DMIC_STATUS[8:4]).
 * @var DMIC_T::GAINCTL0
 * Offset: 0x14  DMIC Channel 0 and 1 Volume Control Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[15:0]  |CHyyVOL   |Channel yy volume/gain control, 0.125dB step
 * |        |          |A 16-bit signed 2u2019s complement number, format is as following
 * |        |          |15
 * |        |          |14
 * |        |          |13
 * |        |          |12
 * |        |          |11
 * |        |          |10
 * |        |          |9
 * |        |          |8
 * |        |          |7
 * |        |          |6
 * |        |          |5
 * |        |          |4
 * |        |          |3
 * |        |          |2
 * |        |          |1
 * |        |          |0
 * |        |          |Sign
 * |        |          |Integer Part
 * |        |          |1/2 dB
 * |        |          |1/4 dB
 * |        |          |1/8 dB
 * |        |          |1/16dB
 * |        |          |Reserved
 * |        |          |The mapping between real gain and programmed number is
 * |        |          |CHxVOL= -128-(Real_Gain) (dB)
 * |        |          |For example,
 * |        |          |if the desired gain is 0dB, the program value will be -128 (0xC000)
 * |        |          |if the desired gain is -20.5dB, then the programmed value will be -107.5(0xCA40)
 * |[31:16] |CHxxVOL   |Channel xx volume/gain control, 0.125dB step
 * |        |          |A 16-bit signed 2u2019s complement number, format is as following
 * |        |          |15
 * |        |          |14
 * |        |          |13
 * |        |          |12
 * |        |          |11
 * |        |          |10
 * |        |          |9
 * |        |          |8
 * |        |          |7
 * |        |          |6
 * |        |          |5
 * |        |          |4
 * |        |          |3
 * |        |          |2
 * |        |          |1
 * |        |          |0
 * |        |          |Sign
 * |        |          |Integer Part
 * |        |          |1/2 dB
 * |        |          |1/4 dB
 * |        |          |1/8 dB
 * |        |          |1/16dB
 * |        |          |Reserved
 * |        |          |The mapping between real gain and programmed number is
 * |        |          |CHxVOL= -128-(Real_Gain) (dB)
 * |        |          |For example,
 * |        |          |if the desired gain is 0dB, the program value will be -128 (0xC000)
 * |        |          |if the desired gain is -20.5dB, then the programmed value will be -107.5(0xCA40)
 * @var DMIC_T::GAINCTL1
 * Offset: 0x18  DMIC Channel 2 and 3 Volume Control Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[15:0]  |CHyyVOL   |Channel yy volume/gain control, 0.125dB step
 * |        |          |A 16-bit signed 2u2019s complement number, format is as following
 * |        |          |15
 * |        |          |14
 * |        |          |13
 * |        |          |12
 * |        |          |11
 * |        |          |10
 * |        |          |9
 * |        |          |8
 * |        |          |7
 * |        |          |6
 * |        |          |5
 * |        |          |4
 * |        |          |3
 * |        |          |2
 * |        |          |1
 * |        |          |0
 * |        |          |Sign
 * |        |          |Integer Part
 * |        |          |1/2 dB
 * |        |          |1/4 dB
 * |        |          |1/8 dB
 * |        |          |1/16dB
 * |        |          |Reserved
 * |        |          |The mapping between real gain and programmed number is
 * |        |          |CHxVOL= -128-(Real_Gain) (dB)
 * |        |          |For example,
 * |        |          |if the desired gain is 0dB, the program value will be -128 (0xC000)
 * |        |          |if the desired gain is -20.5dB, then the programmed value will be -107.5(0xCA40)
 * |[31:16] |CHxxVOL   |Channel xx volume/gain control, 0.125dB step
 * |        |          |A 16-bit signed 2u2019s complement number, format is as following
 * |        |          |15
 * |        |          |14
 * |        |          |13
 * |        |          |12
 * |        |          |11
 * |        |          |10
 * |        |          |9
 * |        |          |8
 * |        |          |7
 * |        |          |6
 * |        |          |5
 * |        |          |4
 * |        |          |3
 * |        |          |2
 * |        |          |1
 * |        |          |0
 * |        |          |Sign
 * |        |          |Integer Part
 * |        |          |1/2 dB
 * |        |          |1/4 dB
 * |        |          |1/8 dB
 * |        |          |1/16dB
 * |        |          |Reserved
 * |        |          |The mapping between real gain and programmed number is
 * |        |          |CHxVOL= -128-(Real_Gain) (dB)
 * |        |          |For example,
 * |        |          |if the desired gain is 0dB, the program value will be -128 (0xC000)
 * |        |          |if the desired gain is -20.5dB, then the programmed value will be -107.5(0xCA40)
 * @var DMIC_T::RAMDATA
 * Offset: 0x24  DMIC DSP0 RAM Test Data Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |DSPRAMDATA|MCU access DSPx RAM by read/write these register while DSP_MEM_TEST is setting as u201C1u201D
 * @var DMIC_T::RAMDATA
 * Offset: 0x64  DMIC DSP1 RAM Test Data Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |DSPRAMDATA|MCU access DSPx RAM by read/write these register while DSP_MEM_TEST is setting as u201C1u201D
 * @var DMIC_T::ROMDATA
 * Offset: 0xA4  DMIC DSP0 ROM Test Data Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |DSPROMDATA|MCU gets DSPx ROM content by read these register while DSP_MEM_TEST is setting as u201C1u201D
 * @var DMIC_T::ROMMBISTL
 * Offset: 0xE4  DMIC DSP0 ROM MBIST Signature LSB
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |DSPROMMBISTSIGL|-------
 * @var DMIC_T::ROMMBISTH
 * Offset: 0x124  DMIC DSP0 ROM MBIST Signature MSB
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |DSPROMMBISTSIGH|-------
 * @var DMIC_T::ROMDATA
 * Offset: 0x164  DMIC DSP1 ROM Test Data Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |DSPROMDATA|MCU gets DSPx ROM content by read these register while DSP_MEM_TEST is setting as u201C1u201D
 * @var DMIC_T::ROMMBISTL
 * Offset: 0x1A4  DMIC DSP1 ROM MBIST Signature LSB
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |DSPROMMBISTSIGL|-------
 * @var DMIC_T::ROMMBISTH
 * Offset: 0x1E4  DMIC DSP1 ROM MBIST Signature MSB
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |DSPROMMBISTSIGH|-------
 */
    __IO uint32_t CTL;                   /*!< [0x0000] DMIC Control Register                                            */
    __IO uint32_t DIV;                   /*!< [0x0004] DMIC Clock Divider Register                                      */
    __I  uint32_t STATUS;                /*!< [0x0008] DMIC Status Register                                             */
    __IO uint32_t PDMACTL;               /*!< [0x000c] DMIC PDMA Control Register                                       */
//    __I  uint32_t FIFO;                  /*!< [0x0010] DMIC FIFO Data Output Register                                   */
    __IO  uint32_t FIFO;//for test                  /*!< [0x0010] DMIC FIFO Data Output Register                                   */
    __IO uint32_t GAINCTL0;              /*!< [0x0014] DMIC Channel 0 and 1 Volume Control Register                     */
    __IO uint32_t GAINCTL1;              /*!< [0x0018] DMIC Channel 2 and 3 Volume Control Register                     */
    __I  uint32_t RESERVE0[5];
//    __IO uint32_t DSPRAMCFG;             /*!< [0x0024] DMIC DSP0 RAM Test Data Register                                 */
//    __IO uint32_t DSPROMCFG;             /*!< [0x0028] DMIC DSP0 ROM Test Data Register                                 */
//    __I  uint32_t RESERVE1[1];
    __IO uint32_t ZEROSAMPLETHD;         /*!< [0x0030] DMIC DSP0 Zero Cross Threshold Register                                 */
    __IO uint32_t DLYCTL;         /*!< [0x0034] DMIC DSP0 Samplig Clock Delay Control Register                                 */
	__I  uint32_t RESERVE2[2];
    __IO uint32_t DSP0RAMDATA;           /*!< [0x0040] DMIC DSP0 RAM Test Data Register                                 */
	__IO uint32_t DSP1RAMDATA;           /*!< [0x0044] DMIC DSP1 RAM Test Data Register                                 */
	__I  uint32_t RESERVE3[2];
//    __I  uint32_t DSP0ROMDATA;           /*!< [0x0050] DMIC DSP0 ROM Test Data Register*/
    __IO  uint32_t DSP0ROMDATA;//for test           /*!< [0x0050] DMIC DSP0 ROM Test Data Register                                 */
	__I  uint32_t DSP0ROMMBISTL;         /*!< [0x0054] DMIC DSP0 ROM MBIST Signature LSB                                 */
	__I  uint32_t DSP0ROMMBISTH;         /*!< [0x0058] DMIC DSP0 ROM MBIST Signature MSB                                */
    __I  uint32_t RESERVE4[1];
//    __I  uint32_t DSP1ROMDATA;           /*!< [0x0060] DMIC DSP1 ROM Test Data Register                                 */
    __IO  uint32_t DSP1ROMDATA;//for test           /*!< [0x0060] DMIC DSP1 ROM Test Data Register                                 */
	__I  uint32_t DSP1ROMMBISTL;         /*!< [0x0064] DMIC DSP1 ROM MBIST Signature LSB                                 */
	__I  uint32_t DSP1ROMMBISTH;         /*!< [0x0068] DMIC DSP1 ROM MBIST Signature MSB                                */


} DMIC_T;


typedef struct
{


/**
 * @var VAD_T::SINCCTL
 * Offset: 0x00  VAD SINC Filter Control Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[11:8]  |SINCOSR   |VAD SINC Filter OSR Setting
 * |        |          |000 = Down sample 48
 * |        |          |001 = Down sample 64
 * |        |          |010 = Down sample 96
 * |        |          |Others = Reserved. Do not use.
 * |[15:12] |SINCGAIN  |VAD SINC Filter Gain Control
 * |        |          |Gain Adjustment to the SINC filter.
 * |[28]    |DATAOFF   |VAD Sending Data to SRAM Control
 * |        |          |When the ACTIVE (VAD_STATUS0[31]) goes high, the data will be transferred to SRAM to store which can be used for keyword detection later
 * |        |          |After some time, if user needs to stop sending data to SRAM, write this bit to 1.
 * |[29]    |SW        |VAD Path Switch Control
 * |        |          |After the ACTIVE(VAD_STATUS0[31]) goes high, it will automatically switch to the DMIC path
 * |        |          |When the CPU is entering idle mode, write 1 to switch back to the VAD path.
 * |        |          |Note 1: After switch back VAD path, user need to set this bit to 0.
 * |        |          |Note 2: User need to set DMIC_CTL[3:0] to 1 and clear ACTIVE (VAD_STATUS0[31]) before set this bit 1.
 * |[30]    |ACTCL     |VAD Active Flag Clear
 * |        |          |0 = No effect.
 * |        |          |1 = Clear ACTIVE(VAD_STATUS0[31]).
 * |        |          |Note: After ACTIVE(VAD_STATUS0[31]) is cleared, user need to set set this bit to 0.
 * |[31]    |VADEN     |VAD Enable Control
 * |        |          |0 = VAD Disabled.
 * |        |          |1 = VAD Enabled.
 * |        |          |Note 1: When set this bit to 1, CHEN0 (DMIC_CTL[0]) will be set to 1 and CHEN1 (DMIC_CTL[1]), CHEN2 (DMIC_CTL[2]) and CHEN3 (DMIC_CTL[3]) will be set to 0 automatically.
 * |        |          |Note 2: When set this bit to 1, DMIC_CLK is generated by VAD module.
 * @var VAD_T::BIQCTL0
 * Offset: 0x04  VAD Biquad Filter Control Register 0
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[15:0]  |BIQA1     |VAD Biquad Filter Coefficient
 * |        |          |Biquad Filter Coefficient a1, in 3 intergers + 13 fractional bits
 * |[31:16] |BIQA2     |VAD Biquad Filter Coefficient
 * |        |          |Biquad Filter Coefficient a2, in 3 intergers + 13 fractional bits.
 * @var VAD_T::BIQCTL1
 * Offset: 0x08  VAD Biquad Filter Control Register 1
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[15:0]  |BIQB0     |VAD Biquad Filter Coefficient
 * |        |          |Biquad Filter Coefficient b0, in 3 intergers + 13 fractional bits.
 * |[31:16] |BIQB1     |VAD Biquad Filter Coefficient
 * |        |          |Biquad Filter Coefficient b1, in 3 intergers + 13 fractional bits.
 * @var VAD_T::BIQCTL2
 * Offset: 0x0C  VAD Biquad Filter Control Register 2
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[15:0]  |BIQB2     |VAD Biquad Filter Coefficient
 * |        |          |Biquad Filter Coefficient b2, in 3 intergers + 13 fractional bits.
 * |[31]    |BIQEN     |VAD Biquad Filter Enable Bit
 * |        |          |0 = VAD Biquad Filter Disabled.
 * |        |          |1 = VAD Biquad Filter Enabled.
 * @var VAD_T::CTL0
 * Offset: 0x10  VAD Control Register 0
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[7:0]   |STAT      |Short Term Power Attack Time
 * |        |          |Slow attack (e.g., 0x99): slow responding to voice, but more stable.
 * |        |          |Fast attack (e.g., 0xCC): fast responding to voice, but more sensitive to other sounds.
 * |        |          |Suggested default attack time setting: Long term power attack time (0x5), Short term power attack time (0xAA).
 * |        |          |The u201CShort Term Poweru201D, in order to detect the instant power of the voices, requires faster attack time, while u201CLong Term Poweru201D, in order to get the averaged power of the background environment, requires slower attack time to maintain its stability
 * |        |          |So the Short term power attack time should be always bigger than the Long term power attack time.
 * |[11:8]  |STAAT     |Short Term Average Power Attack Time Setting, Not used here, set it 0
 * |[14:12] |STPS      |Selection setting in Short Term Average Power calculation, Not used here, set it 0
 * |[19:16] |LTAT      |Long Term Power Attack Time
 * |        |          |Slow attack (e.g., 0x5): less sensitive to environment change.
 * |        |          |Fast attack (e.g., 0x8): more sensitive to environment change.
 * |[20]    |TESTMODE  |Set this to be 1, can data out the BIQ, SINC data through the DMIC FIFO
 * @var VAD_T::CTL1
 * Offset: 0x14  VAD Control Register 1
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[15:0]  |STTHREHWM |Short Term Power Threshold Upper Limit
 * |        |          |To check if the incoming signal is big enough to be ready for VAD activation.
 * |[31:16] |STTHRELWM |Short Term Power Threshold Lower Limit
 * |        |          |To check if the incoming signal is small enough so that VAD status can be terminated.
 * @var VAD_T::CTL2
 * Offset: 0x18  VAD Control Register 2
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[15:0]  |STATHRE   |Short Term Average Power Threshold setting, Not used here, set it 0
 * |[31:16] |LTTHRE    |Long Term Power Threshold
 * |        |          |To check the background energy, also serve as the lower limit of long term power
 * |        |          |When the long term power value is lower than the threshold, it will be set to the threshold value for VAD decision.
 * @var VAD_T::CTL3
 * Offset: 0x1C  VAD Control Register 3
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[15:0]  |DEVTHRE   |Deviation Threshold
 * |        |          |To check if the incoming signal is substantially bigger than its background
 * |        |          |This may work to exclude breath sound as it is slowly varying, but not other sounds (e.g., footsteps, hand claps) with sudden amplitude increase.
 * |        |          |Small: easy to trigger, good for far-field pick-up, but requiring quiet environment.
 * |        |          |Large: good for handheld applications, but requiring louder voice to trigger.
 * |[31:16] |HOT       |Hang Over time
 * |        |          |Hang Over time setting, means how many clocks (CLKSD) of the ACTIVE (VAD_STATUS0[31]) staying high when the calculation is no longer bigger than the threshold
 * @var VAD_T::STATUS0
 * Offset: 0x20  VAD Status Read-back Register 0
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[15:0]  |STP       |Short Term Signal Power (Read Only)
 * |        |          |This field shows the short term signal power value.
 * |[31]    |ACTIVE    |VAD Activation Flag (Read Only)
 * |        |          |When the voice active event occurs, this bit will be set to 1.
 * |        |          |0 = No effect.
 * |        |          |1 = Voice detected.
 * |        |          |Note: When wake-up from idle mode, user need to set CHENn DMIC_CTL[3:0] for DMIC path normal operation.
 * @var VAD_T::STATUS1
 * Offset: 0x24  VAD Status Read-back Register 1
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[15:0]  |DEV       |Deviation (Read Only)
 * |        |          |This field shows deviation of the Long Term Signal Power and Short Term Signal Power.
 * |[31:16] |LTP       |Long Term Signal Power (Read Only)
 * |        |          |This field shows the long term signal power value.
 */
    __IO uint32_t SINCCTL;               /*!< [0x0000] VAD SINC Filter Control Register                                 */
    __IO uint32_t BIQCTL0;               /*!< [0x0004] VAD Biquad Filter Control Register 0                             */
    __IO uint32_t BIQCTL1;               /*!< [0x0008] VAD Biquad Filter Control Register 1                             */
    __IO uint32_t BIQCTL2;               /*!< [0x000c] VAD Biquad Filter Control Register 2                             */
    __IO uint32_t CTL0;                  /*!< [0x0010] VAD Control Register 0                                           */
    __IO uint32_t CTL1;                  /*!< [0x0014] VAD Control Register 1                                           */
    __IO uint32_t CTL2;                  /*!< [0x0018] VAD Control Register 2                                           */
    __IO uint32_t CTL3;                  /*!< [0x001c] VAD Control Register 3                                           */
    __I  uint32_t STATUS0;               /*!< [0x0020] VAD Status Read-back Register 0                                  */
    __I  uint32_t STATUS1;               /*!< [0x0024] VAD Status Read-back Register 1                                  */
//	__IO uint32_t PDMEDCTL;              /*!< [0x0028] VAD PDMED Control Register                                       */
//  __IO uint32_t PDMEDSTATUS;           /*!< [0x002C] VAD PDMED Status Register                                        */

} VAD_T;

/**
    @addtogroup DMIC_CONST DMIC Bit Field Definition
    Constant Definitions for DMIC Controller
@{ */


#define DMIC_CTL_CHEN0_Pos               (0)                                               /*!< DMIC_T::CTL: CHEN0 Position            */
#define DMIC_CTL_CHEN0_Msk               (0x1ul << DMIC_CTL_CHEN0_Pos)                     /*!< DMIC_T::CTL: CHEN0 Mask                */

#define DMIC_CTL_CHEN1_Pos               (1)                                               /*!< DMIC_T::CTL: CHEN1 Position            */
#define DMIC_CTL_CHEN1_Msk               (0x1ul << DMIC_CTL_CHEN1_Pos)                     /*!< DMIC_T::CTL: CHEN1 Mask                */

#define DMIC_CTL_CHEN2_Pos               (2)                                               /*!< DMIC_T::CTL: CHEN2 Position            */
#define DMIC_CTL_CHEN2_Msk               (0x1ul << DMIC_CTL_CHEN2_Pos)                     /*!< DMIC_T::CTL: CHEN2 Mask                */

#define DMIC_CTL_CHEN3_Pos               (3)                                               /*!< DMIC_T::CTL: CHEN3 Position            */
#define DMIC_CTL_CHEN3_Msk               (0x1ul << DMIC_CTL_CHEN3_Pos)                     /*!< DMIC_T::CTL: CHEN3 Mask                */

#define DMIC_CTL_LCHEDGE01_Pos           (8)                                               /*!< DMIC_T::CTL: LCHEDGE01 Position        */
#define DMIC_CTL_LCHEDGE01_Msk           (0x1ul << DMIC_CTL_LCHEDGE01_Pos)                 /*!< DMIC_T::CTL: LCHEDGE01 Mask            */

#define DMIC_CTL_LCHEDGE23_Pos           (9)                                               /*!< DMIC_T::CTL: LCHEDGE23 Position        */
#define DMIC_CTL_LCHEDGE23_Msk           (0x1ul << DMIC_CTL_LCHEDGE23_Pos)                 /*!< DMIC_T::CTL: LCHEDGE23 Mask            */

#define DMIC_CTL_CH0MUTE_Pos             (12)                                              /*!< DMIC_T::CTL: CH0MUTE Position          */
#define DMIC_CTL_CH0MUTE_Msk             (0x1ul << DMIC_CTL_CH0MUTE_Pos)                   /*!< DMIC_T::CTL: CH0MUTE Mask              */

#define DMIC_CTL_CH1MUTE_Pos             (13)                                              /*!< DMIC_T::CTL: CH1MUTE Position          */
#define DMIC_CTL_CH1MUTE_Msk             (0x1ul << DMIC_CTL_CH1MUTE_Pos)                   /*!< DMIC_T::CTL: CH1MUTE Mask              */

#define DMIC_CTL_CH2MUTE_Pos             (14)                                              /*!< DMIC_T::CTL: CH2MUTE Position          */
#define DMIC_CTL_CH2MUTE_Msk             (0x1ul << DMIC_CTL_CH2MUTE_Pos)                   /*!< DMIC_T::CTL: CH2MUTE Mask              */

#define DMIC_CTL_CH3MUTE_Pos             (15)                                              /*!< DMIC_T::CTL: CH3MUTE Position          */
#define DMIC_CTL_CH3MUTE_Msk             (0x1ul << DMIC_CTL_CH3MUTE_Pos)                   /*!< DMIC_T::CTL: CH3MUTE Mask              */

#define DMIC_CTL_CH01HPFEN_Pos           (20)                                              /*!< DMIC_T::CTL: CH01HPFEN Position        */
#define DMIC_CTL_CH01HPFEN_Msk           (0x1ul << DMIC_CTL_CH01HPFEN_Pos)                 /*!< DMIC_T::CTL: CH01HPFEN Mask            */

#define DMIC_CTL_CH23HPFEN_Pos           (21)                                              /*!< DMIC_T::CTL: CH23HPFEN Position        */
#define DMIC_CTL_CH23HPFEN_Msk           (0x1ul << DMIC_CTL_CH23HPFEN_Pos)                 /*!< DMIC_T::CTL: CH23HPFEN Mask            */

#define DMIC_CTL_SWRST_Pos               (24)                                              /*!< DMIC_T::CTL: SWRST Position            */
#define DMIC_CTL_SWRST_Msk               (0x1ul << DMIC_CTL_SWRST_Pos)                     /*!< DMIC_T::CTL: SWRST Mask                */

#define DMIC_CTL_DSPFIFOT_Pos            (25)                                              /*!< DMIC_T::CTL: DSPFIFOT Position         */
#define DMIC_CTL_DSPFIFOT_Msk            (0x1ul << DMIC_CTL_DSPFIFOT_Pos)                  /*!< DMIC_T::CTL: DSPFIFOT Mask             */

#define DMIC_CTL_DSPMEMT_Pos             (26)                                              /*!< DMIC_T::CTL: DSPMEMT Position          */
#define DMIC_CTL_DSPMEMT_Msk             (0x1ul << DMIC_CTL_DSPMEMT_Pos)                   /*!< DMIC_T::CTL: DSPMEMT Mask              */

#define DMIC_CTL_GAINSTEP_Pos            (27)                                              /*!< DMIC_T::CTL: GAINSTEP Position         */
#define DMIC_CTL_GAINSTEP_Msk            (0x3ul << DMIC_CTL_GAINSTEP_Pos)                  /*!< DMIC_T::CTL: GAINSTEP Mask             */

//#define DMIC_CTL_PCMGAINSEL_Pos          (29)                                              /*!< DMIC_T::CTL: PCMGAINSEL Position       */
//#define DMIC_CTL_PCMGAINSEL_Msk          (0x3ul << DMIC_CTL_PCMGAINSEL_Pos)                /*!< DMIC_T::CTL: PCMGAINSEL Mask           */

#define DMIC_CTL_FIFOWIDTH_Pos           (31)                                              /*!< DMIC_T::CTL: FIFOWIDTH Position        */
#define DMIC_CTL_FIFOWIDTH_Msk           (0x1ul << DMIC_CTL_FIFOWIDTH_Pos)                 /*!< DMIC_T::CTL: FIFOWIDTH Mask            */

//#define DMIC_DIV_DMMCLKDIV_Pos           (0)                                               /*!< DMIC_T::DIV: DMMCLKDIV Position        */
//#define DMIC_DIV_DMMCLKDIV_Msk           (0xfful << DMIC_DIV_DMMCLKDIV_Pos)                /*!< DMIC_T::DIV: DMMCLKDIV Mask            */

#define DMIC_DIV_DMCLKDIV_Pos            (8)                                               /*!< DMIC_T::DIV: DMCLKDIV Position         */
#define DMIC_DIV_DMCLKDIV_Msk            (0x3ful << DMIC_DIV_DMCLKDIV_Pos)                 /*!< DMIC_T::DIV: DMCLKDIV Mask             */

#define DMIC_DIV_DMTH_Pos                (16)                                              /*!< DMIC_T::DIV: DMTH Position             */
#define DMIC_DIV_DMTH_Msk                (0x1ful << DMIC_DIV_DMTH_Pos)                     /*!< DMIC_T::DIV: DMTH Mask                 */

#define DMIC_DIV_DMTHIE_Pos              (21)                                              /*!< DMIC_T::DIV: DMTHIE Position           */
#define DMIC_DIV_DMTHIE_Msk              (0x1ul << DMIC_DIV_DMTHIE_Pos)                    /*!< DMIC_T::DIV: DMTHIE Mask               */

#define DMIC_DIV_FCLR_Pos                (22)                                              /*!< DMIC_T::DIV: FCLR Position             */
#define DMIC_DIV_FCLR_Msk                (0x3ul << DMIC_DIV_FCLR_Pos)                      /*!< DMIC_T::DIV: FCLR Mask                 */

#define DMIC_DIV_OSR_Pos                 (24)                                              /*!< DMIC_T::DIV: OSR Position              */
#define DMIC_DIV_OSR_Msk                 (0x7ul << DMIC_DIV_OSR_Pos)                       /*!< DMIC_T::DIV: OSR Mask                  */

//#define DMIC_DIV_HPFCUTF_Pos             (28)                                              /*!< DMIC_T::DIV: HPFCUTF Position          */
//#define DMIC_DIV_HPFCUTF_Msk             (0xful << DMIC_DIV_HPFCUTF_Pos)                   /*!< DMIC_T::DIV: HPFCUTF Mask              */

#define DMIC_STATUS_FULL_Pos             (0)                                               /*!< DMIC_T::STATUS: FULL Position          */
#define DMIC_STATUS_FULL_Msk             (0x1ul << DMIC_STATUS_FULL_Pos)                   /*!< DMIC_T::STATUS: FULL Mask              */

#define DMIC_STATUS_EMPTY_Pos            (1)                                               /*!< DMIC_T::STATUS: EMPTY Position         */
#define DMIC_STATUS_EMPTY_Msk            (0x1ul << DMIC_STATUS_EMPTY_Pos)                  /*!< DMIC_T::STATUS: EMPTY Mask             */

#define DMIC_STATUS_DMTHIF_Pos           (2)                                               /*!< DMIC_T::STATUS: DMTHIF Position        */
#define DMIC_STATUS_DMTHIF_Msk           (0x1ul << DMIC_STATUS_DMTHIF_Pos)                 /*!< DMIC_T::STATUS: DMTHIF Mask            */

#define DMIC_STATUS_FIFOPTR_Pos          (4)                                               /*!< DMIC_T::STATUS: FIFOPTR Position       */
#define DMIC_STATUS_FIFOPTR_Msk          (0x1ful << DMIC_STATUS_FIFOPTR_Pos)               /*!< DMIC_T::STATUS: FIFOPTR Mask           */

#define DMIC_STATUS_DSP0ROMBISTFAIL_Pos  (9)                                               /*!< DMIC_T::STATUS: DSP0ROMBISTFAIL Position*/
#define DMIC_STATUS_DSP0ROMBISTFAIL_Msk  (0x1ul << DMIC_STATUS_DSP0ROMBISTFAIL_Pos)        /*!< DMIC_T::STATUS: DSP0ROMBISTFAIL Mask   */

#define DMIC_STATUS_DSP1ROMBISTFAIL_Pos  (10)                                              /*!< DMIC_T::STATUS: DSP1ROMBISTFAIL Position*/
#define DMIC_STATUS_DSP1ROMBISTFAIL_Msk  (0x1ul << DMIC_STATUS_DSP1ROMBISTFAIL_Pos)        /*!< DMIC_T::STATUS: DSP1ROMBISTFAIL Mask   */

#define DMIC_STATUS_DSP0RAMBISTFAIL_Pos  (13)                                              /*!< DMIC_T::STATUS: DSP0RAMBISTFAIL Position*/
#define DMIC_STATUS_DSP0RAMBISTFAIL_Msk  (0x1ul << DMIC_STATUS_DSP0RAMBISTFAIL_Pos)        /*!< DMIC_T::STATUS: DSP0RAMBISTFAIL Mask   */

#define DMIC_STATUS_DSP1RAMBISTFAIL_Pos  (14)                                              /*!< DMIC_T::STATUS: DSP1RAMBISTFAIL Position*/
#define DMIC_STATUS_DSP1RAMBISTFAIL_Msk  (0x1ul << DMIC_STATUS_DSP1RAMBISTFAIL_Pos)        /*!< DMIC_T::STATUS: DSP1RAMBISTFAIL Mask   */

//#define DMIC_STATUS_FIFOBISTFAIL_Pos     (17)                                              /*!< DMIC_T::STATUS: FIFOBISTFAIL Position  */
//#define DMIC_STATUS_FIFOBISTFAIL_Msk     (0x1ul << DMIC_STATUS_FIFOBISTFAIL_Pos)           /*!< DMIC_T::STATUS: FIFOBISTFAIL Mask      */

#define DMIC_STATUS_DSP0ROMBISTFINISH_Pos (18)                                             /*!< DMIC_T::STATUS: DSP0ROMBISTFINISH Position*/
#define DMIC_STATUS_DSP0ROMBISTFINISH_Msk (0x1ul << DMIC_STATUS_DSP0ROMBISTFINISH_Pos)     /*!< DMIC_T::STATUS: DSP0ROMBISTFINISH Mask */

#define DMIC_STATUS_DSP1ROMBISTFINISH_Pos (19)                                             /*!< DMIC_T::STATUS: DSP1ROMBISTFINISH Position*/
#define DMIC_STATUS_DSP1ROMBISTFINISH_Msk (0x1ul << DMIC_STATUS_DSP1ROMBISTFINISH_Pos)     /*!< DMIC_T::STATUS: DSP1ROMBISTFINISH Mask */

#define DMIC_STATUS_DSP0RAMBISTFINISH_Pos (22)                                             /*!< DMIC_T::STATUS: DSP0RAMBISTFINISH Position*/
#define DMIC_STATUS_DSP0RAMBISTFINISH_Msk (0x1ul << DMIC_STATUS_DSP0RAMBISTFINISH_Pos)     /*!< DMIC_T::STATUS: DSP0RAMBISTFINISH Mask */

#define DMIC_STATUS_DSP1RAMBISTFINISH_Pos (23)                                             /*!< DMIC_T::STATUS: DSP1RAMBISTFINISH Position*/
#define DMIC_STATUS_DSP1RAMBISTFINISH_Msk (0x1ul << DMIC_STATUS_DSP1RAMBISTFINISH_Pos)     /*!< DMIC_T::STATUS: DSP1RAMBISTFINISH Mask */

//#define DMIC_STATUS_FIFOBISTFINISH_Pos   (26)                                              /*!< DMIC_T::STATUS: FIFOBISTFINISH Position*/
//#define DMIC_STATUS_FIFOBISTFINISH_Msk   (0x1ul << DMIC_STATUS_FIFOBISTFINISH_Pos)         /*!< DMIC_T::STATUS: FIFOBISTFINISH Mask    */

#define DMIC_STATUS_DSPBISTEN_Pos        (30)                                              /*!< DMIC_T::STATUS: DSPBISTEN Position     */
#define DMIC_STATUS_DSPBISTEN_Msk        (0x1ul << DMIC_STATUS_DSPBISTEN_Pos)              /*!< DMIC_T::STATUS: DSPBISTEN Mask         */

//#define DMIC_STATUS_FIFOBISTEN_Pos       (31)                                              /*!< DMIC_T::STATUS: FIFOBISTEN Position    */
//#define DMIC_STATUS_FIFOBISTEN_Msk       (0x1ul << DMIC_STATUS_FIFOBISTEN_Pos)             /*!< DMIC_T::STATUS: FIFOBISTEN Mask        */

#define DMIC_PDMACTL_PDMAEN_Pos          (0)                                               /*!< DMIC_T::PDMACTL: PDMAEN Position       */
#define DMIC_PDMACTL_PDMAEN_Msk          (0x1ul << DMIC_PDMACTL_PDMAEN_Pos)                /*!< DMIC_T::PDMACTL: PDMAEN Mask           */

#define DMIC_PDMACTL_LPPDMAEN_Pos          (0)                                               /*!< DMIC_T::PDMACTL: PDMAEN Position       */
#define DMIC_PDMACTL_LPPDMAEN_Msk          (0x1ul << DMIC_PDMACTL_LPPDMAEN_Pos)                /*!< DMIC_T::PDMACTL: PDMAEN Mask           */

#define DMIC_LPPDMACTL_LPPDMAEN_Pos          (0)                                               /*!< DMIC_T::PDMACTL: PDMAEN Position       */
#define DMIC_LPPDMACTL_LPPDMAEN_Msk          (0x1ul << DMIC_LPPDMACTL_LPPDMAEN_Pos)                /*!< DMIC_T::PDMACTL: PDMAEN Mask           */

#define DMIC_FIFO_FIFO_Pos               (0)                                               /*!< DMIC_T::FIFO: FIFO Position            */
#define DMIC_FIFO_FIFO_Msk               (0xfffffful << DMIC_FIFO_FIFO_Pos)              /*!< DMIC_T::FIFO: FIFO Mask                */

#define DMIC_GAINCTL0_CHyyLVOL_Pos        (0)                                               /*!< DMIC_T::GAINCTL0: CHyyLVOL Position     */
#define DMIC_GAINCTL0_CHyyLVOL_Msk        (0xfffful << DMIC_GAINCTL0_CHyyLVOL_Pos)           /*!< DMIC_T::GAINCTL0: CHyyLVOL Mask         */

#define DMIC_GAINCTL0_CHxxRVOL_Pos        (16)                                              /*!< DMIC_T::GAINCTL0: CHxxRVOL Position     */
#define DMIC_GAINCTL0_CHxxRVOL_Msk        (0xfffful << DMIC_GAINCTL0_CHxxRVOL_Pos)           /*!< DMIC_T::GAINCTL0: CHxxRVOL Mask         */

#define DMIC_GAINCTL1_CHyyLVOL_Pos        (0)                                               /*!< DMIC_T::GAINCTL1: CHyyLVOL Position     */
#define DMIC_GAINCTL1_CHyyLVOL_Msk        (0xfffful << DMIC_GAINCTL1_CHyyLVOL_Pos)           /*!< DMIC_T::GAINCTL1: CHyyLVOL Mask         */

#define DMIC_GAINCTL1_CHxxRVOL_Pos        (16)                                              /*!< DMIC_T::GAINCTL1: CHxxRVOL Position     */
#define DMIC_GAINCTL1_CHxxRVOL_Msk        (0xfffful << DMIC_GAINCTL1_CHxxRVOL_Pos)           /*!< DMIC_T::GAINCTL1: CHxxRVOL Mask         */

#define DMIC_ZEROSAMPLETHD_ZEROCROSSTH_Pos (0)                                               /*!< DMIC_T::ZEROSAMPLETHD: ZEROCROSSTH Position     */
#define DMIC_ZEROSAMPLETHD_ZEROCROSSTH_Msk (0xfffffful << DMIC_ZEROSAMPLETHD_ZEROCROSSTH_Pos)      /*!< DMIC_T::ZEROSAMPLETHD: ZEROCROSSTH Mask         */

#define DMIC_DLYCTL_DSP0CLKTYP_Pos      (0)                                               /*!< DMIC_T::DLYCTL: DSP0CLKTYP Position   */
#define DMIC_DLYCTL_DSP0CLKTYP_Msk      (0x1ul << DMIC_DLYCTL_DSP0CLKTYP_Pos)     /*!< DMIC_T::DLYCTL: DSP0CLKTYP Mask       */

#define DMIC_DLYCTL_DSP1CLKTYP_Pos      (1)                                               /*!< DMIC_T::DLYCTL: DSP1CLKTYP Position   */
#define DMIC_DLYCTL_DSP1CLKTYP_Msk      (0x1ul << DMIC_DLYCTL_DSP1CLKTYP_Pos)     /*!< DMIC_T::DLYCTL: DSP1CLKTYP Mask       */

#define DMIC_DLYCTL_DSP0DLY_Pos      (8)                                               /*!< DMIC_T::DLYCTL: DSP0DLY Position   */
#define DMIC_DLYCTL_DSP0DLY_Msk      (0x7ul << DMIC_DLYCTL_DSP0DLY_Pos)     /*!< DMIC_T::DLYCTL: DSP0DLY Mask       */

#define DMIC_DLYCTL_DSP1DLY_Pos      (12)                                               /*!< DMIC_T::DLYCTL: DSP1DLY Position   */
#define DMIC_DLYCTL_DSP1DLY_Msk      (0x7ul << DMIC_DLYCTL_DSP1DLY_Pos)     /*!< DMIC_T::DLYCTL: DSP1DLY Mask       */

#define DMIC_RAMDATA_DSPRAMDATA_Pos      (0)                                               /*!< DMIC_T::RAMDATA: DSPRAMDATA Position   */
#define DMIC_RAMDATA_DSPRAMDATA_Msk      (0xfffffffful << DMIC_RAMDATA_DSPRAMDATA_Pos)     /*!< DMIC_T::RAMDATA: DSPRAMDATA Mask       */

#define DMIC_ROMDATA_DSPROMDATA_Pos      (0)                                               /*!< DMIC_T::ROMDATA: DSPROMDATA Position   */
#define DMIC_ROMDATA_DSPROMDATA_Msk      (0xfffffffful << DMIC_ROMDATA_DSPROMDATA_Pos)     /*!< DMIC_T::ROMDATA: DSPROMDATA Mask       */

#define DMIC_ROMMBISTL_DSPROMMBISTSIGL_Pos (0)                                             /*!< DMIC_T::ROMMBISTL: DSPROMMBISTSIGL Position*/
#define DMIC_ROMMBISTL_DSPROMMBISTSIGL_Msk (0xfffffffful << DMIC_ROMMBISTL_DSPROMMBISTSIGL_Pos) /*!< DMIC_T::ROMMBISTL: DSPROMMBISTSIGL Mask*/

#define DMIC_ROMMBISTH_DSPROMMBISTSIGH_Pos (0)                                             /*!< DMIC_T::ROMMBISTH: DSPROMMBISTSIGH Position*/
#define DMIC_ROMMBISTH_DSPROMMBISTSIGH_Msk (0xfffffffful << DMIC_ROMMBISTH_DSPROMMBISTSIGH_Pos) /*!< DMIC_T::ROMMBISTH: DSPROMMBISTSIGH Mask*/

#define VAD_SINCCTL_SINCOSR_Pos          (8)                                               /*!< VAD_T::SINCCTL: SINCOSR Position       */
#define VAD_SINCCTL_SINCOSR_Msk          (0xful << VAD_SINCCTL_SINCOSR_Pos)                /*!< VAD_T::SINCCTL: SINCOSR Mask           */

#define VAD_SINCCTL_SINCGAIN_Pos         (12)                                              /*!< VAD_T::SINCCTL: SINCGAIN Position      */
#define VAD_SINCCTL_SINCGAIN_Msk         (0xful << VAD_SINCCTL_SINCGAIN_Pos)               /*!< VAD_T::SINCCTL: SINCGAIN Mask          */

#define VAD_SINCCTL_VADMCLKDIV_Pos       (16)                                              /*!< VAD_T::SINCCTL: VADMCLKDIV Position      */
#define VAD_SINCCTL_VADMCLKDIV_Msk       (0xfful << VAD_SINCCTL_VADMCLKDIV_Pos)               /*!< VAD_T::SINCCTL: VADMCLKDIV Mask          */

#define VAD_SINCCTL_DATAOFF_Pos          (28)                                              /*!< VAD_T::SINCCTL: DATAOFF Position       */
#define VAD_SINCCTL_DATAOFF_Msk          (0x1ul << VAD_SINCCTL_DATAOFF_Pos)                /*!< VAD_T::SINCCTL: DATAOFF Mask           */

#define VAD_SINCCTL_SW_Pos               (29)                                              /*!< VAD_T::SINCCTL: SW Position            */
#define VAD_SINCCTL_SW_Msk               (0x1ul << VAD_SINCCTL_SW_Pos)                     /*!< VAD_T::SINCCTL: SW Mask                */

#define VAD_SINCCTL_ACTCL_Pos            (30)                                              /*!< VAD_T::SINCCTL: ACTCL Position         */
#define VAD_SINCCTL_ACTCL_Msk            (0x1ul << VAD_SINCCTL_ACTCL_Pos)                  /*!< VAD_T::SINCCTL: ACTCL Mask             */

#define VAD_SINCCTL_VADEN_Pos            (31)                                              /*!< VAD_T::SINCCTL: VADEN Position         */
#define VAD_SINCCTL_VADEN_Msk            (0x1ul << VAD_SINCCTL_VADEN_Pos)                  /*!< VAD_T::SINCCTL: VADEN Mask             */

#define VAD_BIQCTL0_BIQA1_Pos            (0)                                               /*!< VAD_T::BIQCTL0: BIQA1 Position         */
#define VAD_BIQCTL0_BIQA1_Msk            (0xfffful << VAD_BIQCTL0_BIQA1_Pos)               /*!< VAD_T::BIQCTL0: BIQA1 Mask             */

#define VAD_BIQCTL0_BIQA2_Pos            (16)                                              /*!< VAD_T::BIQCTL0: BIQA2 Position         */
#define VAD_BIQCTL0_BIQA2_Msk            (0xfffful << VAD_BIQCTL0_BIQA2_Pos)               /*!< VAD_T::BIQCTL0: BIQA2 Mask             */

#define VAD_BIQCTL1_BIQB0_Pos            (0)                                               /*!< VAD_T::BIQCTL1: BIQB0 Position         */
#define VAD_BIQCTL1_BIQB0_Msk            (0xfffful << VAD_BIQCTL1_BIQB0_Pos)               /*!< VAD_T::BIQCTL1: BIQB0 Mask             */

#define VAD_BIQCTL1_BIQB1_Pos            (16)                                              /*!< VAD_T::BIQCTL1: BIQB1 Position         */
#define VAD_BIQCTL1_BIQB1_Msk            (0xfffful << VAD_BIQCTL1_BIQB1_Pos)               /*!< VAD_T::BIQCTL1: BIQB1 Mask             */

#define VAD_BIQCTL2_BIQB2_Pos            (0)                                               /*!< VAD_T::BIQCTL2: BIQB2 Position         */
#define VAD_BIQCTL2_BIQB2_Msk            (0xfffful << VAD_BIQCTL2_BIQB2_Pos)               /*!< VAD_T::BIQCTL2: BIQB2 Mask             */

#define VAD_BIQCTL2_BIQEN_Pos            (31)                                              /*!< VAD_T::BIQCTL2: BIQEN Position         */
#define VAD_BIQCTL2_BIQEN_Msk            (0x1ul << VAD_BIQCTL2_BIQEN_Pos)                  /*!< VAD_T::BIQCTL2: BIQEN Mask             */

#define VAD_CTL0_STAT_Pos                (0)                                               /*!< VAD_T::CTL0: STAT Position             */
#define VAD_CTL0_STAT_Msk                (0xfful << VAD_CTL0_STAT_Pos)                     /*!< VAD_T::CTL0: STAT Mask                 */

#define VAD_CTL0_STAAT_Pos               (8)                                               /*!< VAD_T::CTL0: STAAT Position            */
#define VAD_CTL0_STAAT_Msk               (0xful << VAD_CTL0_STAAT_Pos)                     /*!< VAD_T::CTL0: STAAT Mask                */

#define VAD_CTL0_STPS_Pos                (12)                                              /*!< VAD_T::CTL0: STPS Position             */
#define VAD_CTL0_STPS_Msk                (0x7ul << VAD_CTL0_STPS_Pos)                      /*!< VAD_T::CTL0: STPS Mask                 */

#define VAD_CTL0_LTAT_Pos                (16)                                              /*!< VAD_T::CTL0: LTAT Position             */
#define VAD_CTL0_LTAT_Msk                (0xful << VAD_CTL0_LTAT_Pos)                      /*!< VAD_T::CTL0: LTAT Mask                 */

#define VAD_CTL0_TESTMODE_Pos            (20)                                              /*!< VAD_T::CTL0: TESTMODE Position         */
#define VAD_CTL0_TESTMODE_Msk            (0x1ul << VAD_CTL0_TESTMODE_Pos)                  /*!< VAD_T::CTL0: TESTMODE Mask             */

#define VAD_CTL1_STTHREHWM_Pos           (0)                                               /*!< VAD_T::CTL1: STTHREHWM Position        */
#define VAD_CTL1_STTHREHWM_Msk           (0xfffful << VAD_CTL1_STTHREHWM_Pos)              /*!< VAD_T::CTL1: STTHREHWM Mask            */

#define VAD_CTL1_STTHRELWM_Pos           (16)                                              /*!< VAD_T::CTL1: STTHRELWM Position        */
#define VAD_CTL1_STTHRELWM_Msk           (0xfffful << VAD_CTL1_STTHRELWM_Pos)              /*!< VAD_T::CTL1: STTHRELWM Mask            */

#define VAD_CTL2_STATHRE_Pos             (0)                                               /*!< VAD_T::CTL2: STATHRE Position          */
#define VAD_CTL2_STATHRE_Msk             (0xfffful << VAD_CTL2_STATHRE_Pos)                /*!< VAD_T::CTL2: STATHRE Mask              */

#define VAD_CTL2_LTTHRE_Pos              (16)                                              /*!< VAD_T::CTL2: LTTHRE Position           */
#define VAD_CTL2_LTTHRE_Msk              (0xfffful << VAD_CTL2_LTTHRE_Pos)                 /*!< VAD_T::CTL2: LTTHRE Mask               */

#define VAD_CTL3_DEVTHRE_Pos             (0)                                               /*!< VAD_T::CTL3: DEVTHRE Position          */
#define VAD_CTL3_DEVTHRE_Msk             (0xfffful << VAD_CTL3_DEVTHRE_Pos)                /*!< VAD_T::CTL3: DEVTHRE Mask              */

#define VAD_CTL3_HOT_Pos                 (16)                                              /*!< VAD_T::CTL3: HOT Position              */
#define VAD_CTL3_HOT_Msk                 (0xfffful << VAD_CTL3_HOT_Pos)                    /*!< VAD_T::CTL3: HOT Mask                  */

#define VAD_STATUS0_STP_Pos              (0)                                               /*!< VAD_T::STATUS0: STP Position           */
#define VAD_STATUS0_STP_Msk              (0xfffful << VAD_STATUS0_STP_Pos)                 /*!< VAD_T::STATUS0: STP Mask               */

#define VAD_STATUS0_ACTIVE_Pos           (31)                                              /*!< VAD_T::STATUS0: ACTIVE Position        */
#define VAD_STATUS0_ACTIVE_Msk           (0x1ul << VAD_STATUS0_ACTIVE_Pos)                 /*!< VAD_T::STATUS0: ACTIVE Mask            */

#define VAD_STATUS1_DEV_Pos              (0)                                               /*!< VAD_T::STATUS1: DEV Position           */
#define VAD_STATUS1_DEV_Msk              (0xfffful << VAD_STATUS1_DEV_Pos)                 /*!< VAD_T::STATUS1: DEV Mask               */

#define VAD_STATUS1_LTP_Pos              (16)                                              /*!< VAD_T::STATUS1: LTP Position           */
#define VAD_STATUS1_LTP_Msk              (0xfffful << VAD_STATUS1_LTP_Pos)                 /*!< VAD_T::STATUS1: LTP Mask               */

/** @} DMIC_CONST */
/** @} end of DMIC register group */
/** @} end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __DMIC_REG_H__ */
