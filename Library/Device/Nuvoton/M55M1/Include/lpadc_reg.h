/**************************************************************************//**
 * @file     lpadc_reg.h
 * @version  V1.00
 * @brief    LPADC register definition header file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#ifndef __LPADC_REG_H__
#define __LPADC_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
    @addtogroup REGISTER Control Register
    @{
*/

/**
    @addtogroup LPADC Enhanced Analog to Digital Converter (LPADC)
    Memory Mapped Structure for LPADC Controller
    @{
*/

typedef struct
{
    /**
     * @var EADC_T::DAT[19]
     * Offset: 0x00  ADC Data Register 0~18 for Sample Module 0~18
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |RESULT    |ADC Conversion Result
     * |        |          |This field contains 12 bits conversion result.
     * |        |          |When DMOF (EADC_CTL[9]) is set to 0, 12-bit ADC conversion result with unsigned format will be filled in RESULT[11:0] and zero will be filled in RESULT[15:12].
     * |        |          |When DMOF (EADC_CTL[9]) set to 1, 12-bit ADC conversion result with 2'complement format will be filled in RESULT[11:0] and signed bits to will be filled in RESULT[15:12].
     * |[16]    |OV        |Overrun Flag
     * |        |          |If converted data in RESULT[11:0] has not been read before new conversion result is loaded to this register, OV is set to 1.
     * |        |          |0 = Data in RESULT[11:0] is recent conversion result.
     * |        |          |1 = Data in RESULT[11:0] is overwrite.
     * |        |          |Note: It is cleared by hardware after EADC_DAT register is read.
     * |[17]    |VALID     |Valid Flag
     * |        |          |This bit is set to 1 when corresponding sample module channel analog input conversion is completed and cleared by hardware after EADC_DAT register is read.
     * |        |          |0 = Data in RESULT[11:0] bits is not valid.
     * |        |          |1 = Data in RESULT[11:0] bits is valid.
     * @var EADC_T::CURDAT
     * Offset: 0x4C  ADC PDMA Current Transfer Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[17:0]  |CURDAT    |ADC PDMA Current Transfer Data Register
     * |        |          |This register is a shadow register of EADC_DATn (n=0~18) for PDMA support.
     * |        |          |This is a read only register.
     * @var EADC_T::CTL
     * Offset: 0x50  ADC Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ADCEN     |ADC Converter Enable Bit
     * |        |          |0 = Disabled EADC.
     * |        |          |1 = Enabled EADC.
     * |        |          |Note: Before starting ADC conversion function, this bit should be set to 1
     * |        |          |Clear it to 0 to disable ADC converter analog circuit power consumption.
     * |[1]     |ADCRST    |ADC Converter Control Circuits Reset
     * |        |          |0 = No effect.
     * |        |          |1 = Cause ADC control circuits reset to initial state, but not change the ADC registers value.
     * |        |          |Note: ADCRST bit remains 1 during ADC reset, when ADC reset end, the ADCRST bit is automatically cleared to 0.
     * |[2]     |ADCIEN0   |Specific Sample Module ADC ADINT0 Interrupt Enable Bit
     * |        |          |The ADC converter generates a conversion end ADIF0 (EADC_STATUS2[0]) upon the end of specific sample module ADC conversion
     * |        |          |If ADCIEN0 bit is set then conversion end interrupt request ADINT0 is generated.
     * |        |          |0 = Specific sample module ADC ADINT0 interrupt function Disabled.
     * |        |          |1 = Specific sample module ADC ADINT0 interrupt function Enabled.
     * |[3]     |ADCIEN1   |Specific Sample Module ADC ADINT1 Interrupt Enable Bit
     * |        |          |The ADC converter generates a conversion end ADIF1 (EADC_STATUS2[1]) upon the end of specific sample module ADC conversion
     * |        |          |If ADCIEN1 bit is set then conversion end interrupt request ADINT1 is generated.
     * |        |          |0 = Specific sample module ADC ADINT1 interrupt function Disabled.
     * |        |          |1 = Specific sample module ADC ADINT1 interrupt function Enabled.
     * |[4]     |ADCIEN2   |Specific Sample Module ADC ADINT2 Interrupt Enable Bit
     * |        |          |The ADC converter generates a conversion end ADIF2 (EADC_STATUS2[2]) upon the end of specific sample module ADC conversion
     * |        |          |If ADCIEN2 bit is set then conversion end interrupt request ADINT2 is generated.
     * |        |          |0 = Specific sample module ADC ADINT2 interrupt function Disabled.
     * |        |          |1 = Specific sample module ADC ADINT2 interrupt function Enabled.
     * |[5]     |ADCIEN3   |Specific Sample Module ADC ADINT3 Interrupt Enable Bit
     * |        |          |The ADC converter generates a conversion end ADIF3 (EADC_STATUS2[3]) upon the end of specific sample module ADC conversion
     * |        |          |If ADCIEN3 bit is set then conversion end interrupt request ADINT3 is generated.
     * |        |          |0 = Specific sample module ADC ADINT3 interrupt function Disabled.
     * |        |          |1 = Specific sample module ADC ADINT3 interrupt function Enabled.
     * |[7:6]   |RESSEL    |Resolution Selection
     * |        |          |00 = 6-bit ADC result will be put at RESULT (EADC_DATn[5:0]).
     * |        |          |01 = 8-bit ADC result will be put at RESULT (EADC_DATn[7:0]).
     * |        |          |10 = 10-bit ADC result will be put at RESULT (EADC_DATn[9:0]).
     * |        |          |11 = 12-bit ADC result will be put at RESULT (EADC_DATn[11:0]).
     * |[8]     |DIFFEN    |Differential Analog Input Mode Enable Bit
     * |        |          |0 = Single-end analog input mode.
     * |        |          |1 = Differential analog input mode.
     * |[9]     |DMOF      |ADC Differential Input Mode Output Format
     * |        |          |0 = ADC conversion result will be filled in RESULT (EADC_DATn[15:0] , n= 0 ~18) with unsigned format.
     * |        |          |1 = ADC conversion result will be filled in RESULT (EADC_DATn[15:0] , n= 0 ~18) with 2'complement format.
     * |[11]    |PDMAEN    |PDMA Transfer Enable Bit
     * |        |          |When ADC conversion is completed, the converted data is loaded into EADC_DATn (n: 0 ~ 18) register, user can enable this bit to generate a PDMA data transfer request.
     * |        |          |0 = PDMA data transfer Disabled.
     * |        |          |1 = PDMA data transfer Enabled.
     * |        |          |Note: When set this bit field to 1, user must set ADCIENn (EADC_CTL[5:2], n=0~3) = 0 to disable interrupt.
     * @var EADC_T::SWTRG
     * Offset: 0x54  ADC Sample Module Software Start Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[18:0]  |SWTRG     |ADC Sample Module 0~18 Software Force to Start ADC Conversion
     * |        |          |0 = No effect.
     * |        |          |1 = Cause an ADC conversion when the priority is given to sample module.
     * |        |          |Note: After write this register to start ADC conversion, the EADC_PENDSTS register will show which sample module will conversion
     * |        |          |If user want to disable the conversion of the sample module, user can write EADC_PENDSTS register to clear it.
     * @var EADC_T::PENDSTS
     * Offset: 0x58  ADC Start of Conversion Pending Flag Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[18:0]  |STPF      |ADC Sample Module 0~18 Start of Conversion Pending Flag
     * |        |          |Read:
     * |        |          |0 = There is no pending conversion for sample module.
     * |        |          |1 = Sample module ADC start of conversion is pending.
     * |        |          |Write:
     * |        |          |1 = clear pending flag & cancel the conversion for sample module.
     * |        |          |Note: This bit remains 1 during pending state, when the respective ADC conversion is end, the STPFn (n=0~18) bit is automatically cleared to 0
     * @var EADC_T::OVSTS
     * Offset: 0x5C  ADC Sample Module Start of Conversion Overrun Flag Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[18:0]  |SPOVF     |ADC SAMPLE0~18 Overrun Flag
     * |        |          |0 = No sample module event overrun.
     * |        |          |1 = Indicates a new sample module event is generated while an old one event is pending.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * @var EADC_T::SCTL[19]
     * Offset: 0x80  ADC Sample Module 0~18 Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |CHSEL     |ADC Sample Module Channel Selection
     * |        |          |00H = EADC_CH0 (slow channel).
     * |        |          |01H = EADC_CH1 (slow channel).
     * |        |          |02H = EADC_CH2 (slow channel).
     * |        |          |03H = EADC_CH3 (slow channel).
     * |        |          |04H = EADC_CH4 (slow channel).
     * |        |          |05H = EADC_CH5 (slow channel).
     * |        |          |06H = EADC_CH6 (slow channel).
     * |        |          |07H = EADC_CH7 (slow channel).
     * |        |          |08H = EADC_CH8 (slow channel).
     * |        |          |09H = EADC_CH9 (slow channel).
     * |        |          |0AH = EADC_CH10 (fast channel).
     * |        |          |0BH = EADC_CH11 (fast channel).
     * |        |          |0CH = EADC_CH12 (fast channel).
     * |        |          |0DH = EADC_CH13 (fast channel).
     * |        |          |0EH = EADC_CH14 (fast channel).
     * |        |          |0FH = EADC_CH15 (fast channel).
     * |[4]     |EXTREN    |ADC External Trigger Rising Edge Enable Bit
     * |        |          |0 = Rising edge Disabled when ADC selects EADC0_ST as trigger source.
     * |        |          |1 = Rising edge Enabled when ADC selects EADC0_ST as trigger source.
     * |[5]     |EXTFEN    |ADC External Trigger Falling Edge Enable Bit
     * |        |          |0 = Falling edge Disabled when ADC selects EADC0_ST as trigger source.
     * |        |          |1 = Falling edge Enabled when ADC selects EADC0_ST as trigger source.
     * |[7:6]   |TRGDLYDIV |ADC Sample Module Start of Conversion Trigger Delay Clock Divider Selection
     * |        |          |Trigger delay clock frequency:
     * |        |          |00 = ADC_CLK/1.
     * |        |          |01 = ADC_CLK/2.
     * |        |          |10 = ADC_CLK/4.
     * |        |          |11 = ADC_CLK/16.
     * |[15:8]  |TRGDLYCNT |ADC Sample Module Start of Conversion Trigger Delay Time
     * |        |          |Trigger delay time = TRGDLYCNT x ADC_CLK x n (n=1,2,4,16 from TRGDLYDIV setting).
     * |[20:16] |TRGSEL    |ADC Sample Module Start of Conversion Trigger Source Selection
     * |        |          |0H = Disable trigger.
     * |        |          |1H = External trigger from EADC0_ST pin input.
     * |        |          |2H = ADC ADINT0 interrupt EOC (End of conversion) pulse trigger.
     * |        |          |3H = ADC ADINT1 interrupt EOC (End of conversion) pulse trigger.
     * |        |          |4H = Timer0 overflow pulse trigger.
     * |        |          |5H = Timer1 overflow pulse trigger.
     * |        |          |6H = Timer2 overflow pulse trigger.
     * |        |          |7H = Timer3 overflow pulse trigger.
     * |        |          |8H = EPWM0TG0.
     * |        |          |9H = EPWM0TG1.
     * |        |          |AH = EPWM0TG2.
     * |        |          |BH = EPWM0TG3.
     * |        |          |CH = EPWM0TG4.
     * |        |          |DH = EPWM0TG5.
     * |        |          |EH = EPWM1TG0.
     * |        |          |FH = EPWM1TG1.
     * |        |          |10H = EPWM1TG2.
     * |        |          |11H = EPWM1TG3.
     * |        |          |12H = EPWM1TG4.
     * |        |          |13H = EPWM1TG5.
     * |        |          |14H = BPWM0TG.
     * |        |          |15H = BPWM1TG.
     * |        |          |other = Reserved.
     * |[22]    |INTPOS    |Interrupt Flag Position Select
     * |        |          |0 = Set ADIFn (EADC_STATUS2[n], n=0~3) at ADC end of conversion.
     * |        |          |1 = Set ADIFn (EADC_STATUS2[n], n=0~3) at ADC start of conversion.
     * |[23]    |DBMEN     |Double Buffer Mode Enable Bit
     * |        |          |0 = Sample has one sample result register. (default).
     * |        |          |1 = Sample has two sample result registers.
     * |[31:24] |EXTSMPT   |ADC Sampling Time Extend
     * |        |          |When ADC converting at high conversion rate, the sampling time of analog input voltage may not enough if input channel loading is heavy, user can extend ADC sampling time after trigger source is coming to get enough sampling time.
     * |        |          |The range of start delay time is from 0~255 ADC clock.
     * @var EADC_T::INTSRC[4]
     * Offset: 0xD0  ADC interrupt 0~3 Source Enable Control Register.
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SPLIE0    |Sample Module 0 Interrupt Enable Bit
     * |        |          |0 = Sample Module 0 interrupt Disabled.
     * |        |          |1 = Sample Module 0 interrupt Enabled.
     * |[1]     |SPLIE1    |Sample Module 1 Interrupt Enable Bit
     * |        |          |0 = Sample Module 1 interrupt Disabled.
     * |        |          |1 = Sample Module 1 interrupt Enabled.
     * |[2]     |SPLIE2    |Sample Module 2 Interrupt Enable Bit
     * |        |          |0 = Sample Module 2 interrupt Disabled.
     * |        |          |1 = Sample Module 2 interrupt Enabled.
     * |[3]     |SPLIE3    |Sample Module 3 Interrupt Enable Bit
     * |        |          |0 = Sample Module 3 interrupt Disabled.
     * |        |          |1 = Sample Module 3 interrupt Enabled.
     * |[4]     |SPLIE4    |Sample Module 4 Interrupt Enable Bit
     * |        |          |0 = Sample Module 4 interrupt Disabled.
     * |        |          |1 = Sample Module 4 interrupt Enabled.
     * |[5]     |SPLIE5    |Sample Module 5 Interrupt Enable Bit
     * |        |          |0 = Sample Module 5 interrupt Disabled.
     * |        |          |1 = Sample Module 5 interrupt Enabled.
     * |[6]     |SPLIE6    |Sample Module 6 Interrupt Enable Bit
     * |        |          |0 = Sample Module 6 interrupt Disabled.
     * |        |          |1 = Sample Module 6 interrupt Enabled.
     * |[7]     |SPLIE7    |Sample Module 7 Interrupt Enable Bit
     * |        |          |0 = Sample Module 7 interrupt Disabled.
     * |        |          |1 = Sample Module 7 interrupt Enabled.
     * |[8]     |SPLIE8    |Sample Module 8 Interrupt Enable Bit
     * |        |          |0 = Sample Module 8 interrupt Disabled.
     * |        |          |1 = Sample Module 8 interrupt Enabled.
     * |[9]     |SPLIE9    |Sample Module 9 Interrupt Enable Bit
     * |        |          |0 = Sample Module 9 interrupt Disabled.
     * |        |          |1 = Sample Module 9 interrupt Enabled.
     * |[10]    |SPLIE10   |Sample Module 10 Interrupt Enable Bit
     * |        |          |0 = Sample Module 10 interrupt Disabled.
     * |        |          |1 = Sample Module 10 interrupt Enabled.
     * |[11]    |SPLIE11   |Sample Module 11 Interrupt Enable Bit
     * |        |          |0 = Sample Module 11 interrupt Disabled.
     * |        |          |1 = Sample Module 11 interrupt Enabled.
     * |[12]    |SPLIE12   |Sample Module 12 Interrupt Enable Bit
     * |        |          |0 = Sample Module 12 interrupt Disabled.
     * |        |          |1 = Sample Module 12 interrupt Enabled.
     * |[13]    |SPLIE13   |Sample Module 13 Interrupt Enable Bit
     * |        |          |0 = Sample Module 13 interrupt Disabled.
     * |        |          |1 = Sample Module 13 interrupt Enabled.
     * |[14]    |SPLIE14   |Sample Module 14 Interrupt Enable Bit
     * |        |          |0 = Sample Module 14 interrupt Disabled.
     * |        |          |1 = Sample Module 14 interrupt Enabled.
     * |[15]    |SPLIE15   |Sample Module 15 Interrupt Enable Bit
     * |        |          |0 = Sample Module 15 interrupt Disabled.
     * |        |          |1 = Sample Module 15 interrupt Enabled.
     * |[16]    |SPLIE16   |Sample Module 16 Interrupt Enable Bit
     * |        |          |0 = Sample Module 16 interrupt Disabled.
     * |        |          |1 = Sample Module 16 interrupt Enabled.
     * |[17]    |SPLIE17   |Sample Module 17 Interrupt Enable Bit
     * |        |          |0 = Sample Module 17 interrupt Disabled.
     * |        |          |1 = Sample Module 17 interrupt Enabled.
     * |[18]    |SPLIE18   |Sample Module 18 Interrupt Enable Bit
     * |        |          |0 = Sample Module 18 interrupt Disabled.
     * |        |          |1 = Sample Module 18 interrupt Enabled.
     * @var EADC_T::CMP[4]
     * Offset: 0xE0  ADC Result Compare Register 0~3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ADCMPEN   |ADC Result Compare Enable Bit
     * |        |          |0 = Compare Disabled.
     * |        |          |1 = Compare Enabled.
     * |        |          |Set this bit to 1 to enable compare CMPDAT (EADC_CMPn[27:16], n=0~3) with specified sample module conversion result when converted data is loaded into EADC_DAT register.
     * |[1]     |ADCMPIE   |ADC Result Compare Interrupt Enable Bit
     * |        |          |0 = Compare function interrupt Disabled.
     * |        |          |1 = Compare function interrupt Enabled.
     * |        |          |If the compare function is enabled and the compare condition matches the setting of CMPCOND (EADC_CMPn[2], n=0~3) and CMPMCNT (EADC_CMPn[11:8], n=0~3), ADCMPFn (EADC_STATUS2[7:4], n=0~3) will be asserted, in the meanwhile, if ADCMPIE is set to 1, a compare interrupt request is generated.
     * |[2]     |CMPCOND   |Compare Condition
     * |        |          |0= Set the compare condition as that when a 12-bit ADC conversion result is less than the 12-bit CMPDAT (EADC_CMPn [27:16]), the internal match counter will increase one.
     * |        |          |1= Set the compare condition as that when a 12-bit ADC conversion result is greater or equal to the 12-bit CMPDAT (EADC_CMPn [27:16]), the internal match counter will increase one.
     * |        |          |Note: When the internal counter reaches the value to (CMPMCNT (EADC_CMPn[11:8], n=0~3) +1), the CMPF bit will be set.
     * |[7:3]   |CMPSPL    |Compare Sample Module Selection
     * |        |          |00000 = Sample Module 0 conversion result EADC_DAT0 is selected to be compared.
     * |        |          |00001 = Sample Module 1 conversion result EADC_DAT1 is selected to be compared.
     * |        |          |00010 = Sample Module 2 conversion result EADC_DAT2 is selected to be compared.
     * |        |          |00011 = Sample Module 3 conversion result EADC_DAT3 is selected to be compared.
     * |        |          |00100 = Sample Module 4 conversion result EADC_DAT4 is selected to be compared.
     * |        |          |00101 = Sample Module 5 conversion result EADC_DAT5 is selected to be compared.
     * |        |          |00110 = Sample Module 6 conversion result EADC_DAT6 is selected to be compared.
     * |        |          |00111 = Sample Module 7 conversion result EADC_DAT7 is selected to be compared.
     * |        |          |01000 = Sample Module 8 conversion result EADC_DAT8 is selected to be compared.
     * |        |          |01001 = Sample Module 9 conversion result EADC_DAT9 is selected to be compared.
     * |        |          |01010 = Sample Module 10 conversion result EADC_DAT10 is selected to be compared.
     * |        |          |01011 = Sample Module 11 conversion result EADC_DAT11 is selected to be compared.
     * |        |          |01100 = Sample Module 12 conversion result EADC_DAT12 is selected to be compared.
     * |        |          |01101 = Sample Module 13 conversion result EADC_DAT13 is selected to be compared.
     * |        |          |01110 = Sample Module 14 conversion result EADC_DAT14 is selected to be compared.
     * |        |          |01111 = Sample Module 15 conversion result EADC_DAT15 is selected to be compared.
     * |        |          |10000 = Sample Module 16 conversion result EADC_DAT16 is selected to be compared.
     * |        |          |10001 = Sample Module 17 conversion result EADC_DAT17 is selected to be compared.
     * |        |          |10010 = Sample Module 18 conversion result EADC_DAT18 is selected to be compared.
     * |[11:8]  |CMPMCNT   |Compare Match Count
     * |        |          |When the specified ADC sample module analog conversion result matches the compare condition defined by CMPCOND (EADC_CMPn[2], n=0~3), the internal match counter will increase 1
     * |        |          |If the compare result does not meet the compare condition, the internal compare match counter will reset to 0
     * |        |          |When the internal counter reaches the value to (CMPMCNT +1), the ADCMPFn (EADC_STATUS2[7:4], n=0~3) will be set.
     * |[15]    |CMPWEN    |Compare Window Mode Enable Bit
     * |        |          |0 = ADCMPF0 (EADC_STATUS2[4]) will be set when EADC_CMP0 compared condition matched
     * |        |          |ADCMPF2 (EADC_STATUS2[6]) will be set when EADC_CMP2 compared condition matched
     * |        |          |1 = ADCMPF0 (EADC_STATUS2[4]) will be set when both EADC_CMP0 and EADC_CMP1 compared condition matched
     * |        |          |ADCMPF2 (EADC_STATUS2[6]) will be set when both EADC_CMP2 and EADC_CMP3 compared condition matched.
     * |        |          |Note: This bit is only present in EADC_CMP0 and EADC_CMP2 register.
     * |[27:16] |CMPDAT    |Comparison Data
     * |        |          |The 12 bits data is used to compare with conversion result of specified sample module
     * |        |          |User can use it to monitor the external analog input pin voltage transition without imposing a load on software.
     * @var EADC_T::STATUS0
     * Offset: 0xF0  ADC Status Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |VALID     |EADC_DAT0~15 Data Valid Flag
     * |        |          |It is a mirror of VALID bit in sample module ADC result data register EADC_DATn. (n=0~18).
     * |[31:16] |OV        |EADC_DAT0~15 Overrun Flag
     * |        |          |It is a mirror to OV bit in sample module ADC result data register EADC_DATn. (n=0~18).
     * @var EADC_T::STATUS1
     * Offset: 0xF4  ADC Status Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |VALID     |EADC_DAT16~18 Data Valid Flag
     * |        |          |It is a mirror of VALID bit in sample module ADC result data register EADC_DATn. (n=0~18).
     * |[18:16] |OV        |EADC_DAT16~18 Overrun Flag
     * |        |          |It is a mirror to OV bit in sample module ADC result data register EADC_DATn. (n=0~18).
     * @var EADC_T::STATUS2
     * Offset: 0xF8  ADC Status Register 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ADIF0     |ADC ADINT0 Interrupt Flag
     * |        |          |0 = No ADINT0 interrupt pulse received.
     * |        |          |1 = ADINT0 interrupt pulse has been received.
     * |        |          |Note1: This bit is cleared by writing 1 to it.
     * |        |          |Note2:This bit indicates whether an ADC conversion of specific sample module has been completed
     * |[1]     |ADIF1     |ADC ADINT1 Interrupt Flag
     * |        |          |0 = No ADINT1 interrupt pulse received.
     * |        |          |1 = ADINT1 interrupt pulse has been received.
     * |        |          |Note1: This bit is cleared by writing 1 to it.
     * |        |          |Note2:This bit indicates whether an ADC conversion of specific sample module has been completed
     * |[2]     |ADIF2     |ADC ADINT2 Interrupt Flag
     * |        |          |0 = No ADINT2 interrupt pulse received.
     * |        |          |1 = ADINT2 interrupt pulse has been received.
     * |        |          |Note1: This bit is cleared by writing 1 to it.
     * |        |          |Note2:This bit indicates whether an ADC conversion of specific sample module has been completed
     * |[3]     |ADIF3     |ADC ADINT3 Interrupt Flag
     * |        |          |0 = No ADINT3 interrupt pulse received.
     * |        |          |1 = ADINT3 interrupt pulse has been received.
     * |        |          |Note1: This bit is cleared by writing 1 to it.
     * |        |          |Note2:This bit indicates whether an ADC conversion of specific sample module has been completed
     * |[4]     |ADCMPF0   |ADC Compare 0 Flag
     * |        |          |When the specific sample module ADC conversion result meets setting condition in EADC_CMP0 then this bit is set to 1.
     * |        |          |0 = Conversion result in EADC_DAT does not meet EADC_CMP0 register setting.
     * |        |          |1 = Conversion result in EADC_DAT meets EADC_CMP0 register setting.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[5]     |ADCMPF1   |ADC Compare 1 Flag
     * |        |          |When the specific sample module ADC conversion result meets setting condition in EADC_CMP1 then this bit is set to 1.
     * |        |          |0 = Conversion result in EADC_DAT does not meet EADC_CMP1 register setting.
     * |        |          |1 = Conversion result in EADC_DAT meets EADC_CMP1 register setting.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[6]     |ADCMPF2   |ADC Compare 2 Flag
     * |        |          |When the specific sample module ADC conversion result meets setting condition in EADC_CMP2 then this bit is set to 1.
     * |        |          |0 = Conversion result in EADC_DAT does not meet EADC_CMP2 register setting.
     * |        |          |1 = Conversion result in EADC_DAT meets EADC_CMP2 register setting.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[7]     |ADCMPF3   |ADC Compare 3 Flag
     * |        |          |When the specific sample module ADC conversion result meets setting condition in EADC_CMP3 then this bit is set to 1.
     * |        |          |0 = Conversion result in EADC_DAT does not meet EADC_CMP3 register setting.
     * |        |          |1 = Conversion result in EADC_DAT meets EADC_CMP3 register setting.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[8]     |ADOVIF0   |ADC ADINT0 Interrupt Flag Overrun
     * |        |          |0 = ADINT0 interrupt flag is not overwritten to 1.
     * |        |          |1 = ADINT0 interrupt flag is overwritten to 1.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[9]     |ADOVIF1   |ADC ADINT1 Interrupt Flag Overrun
     * |        |          |0 = ADINT1 interrupt flag is not overwritten to 1.
     * |        |          |1 = ADINT1 interrupt flag is overwritten to 1.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[10]    |ADOVIF2   |ADC ADINT2 Interrupt Flag Overrun
     * |        |          |0 = ADINT2 interrupt flag is not overwritten to 1.
     * |        |          |1 = ADINT2 interrupt flag is s overwritten to 1.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[11]    |ADOVIF3   |ADC ADINT3 Interrupt Flag Overrun
     * |        |          |0 = ADINT3 interrupt flag is not overwritten to 1.
     * |        |          |1 = ADINT3 interrupt flag is overwritten to 1.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[12]    |ADCMPO0   |ADC Compare 0 Output Status (Read Only)
     * |        |          |The 12 bits compare0 data CMPDAT0 (EADC_CMP0[27:16]) is used to compare with conversion result of specified sample module
     * |        |          |User can use it to monitor the external analog input pin voltage status.
     * |        |          |0 = Conversion result in EADC_DAT less than CMPDAT0 setting.
     * |        |          |1 = Conversion result in EADC_DAT great than or equal CMPDAT0 setting.
     * |[13]    |ADCMPO1   |ADC Compare 1 Output Status (Read Only)
     * |        |          |The 12 bits compare1 data CMPDAT1 (EADC_CMP1[27:16]) is used to compare with conversion result of specified sample module
     * |        |          |User can use it to monitor the external analog input pin voltage status.
     * |        |          |0 = Conversion result in EADC_DAT less than CMPDAT1 setting.
     * |        |          |1 = Conversion result in EADC_DAT great than or equal CMPDAT1 setting.
     * |[14]    |ADCMPO2   |ADC Compare 2 Output Status (Read Only)
     * |        |          |The 12 bits compare2 data CMPDAT2 (EADC_CMP2[27:16]) is used to compare with conversion result of specified sample module
     * |        |          |User can use it to monitor the external analog input pin voltage status.
     * |        |          |0 = Conversion result in EADC_DAT less than CMPDAT2 setting.
     * |        |          |1 = Conversion result in EADC_DAT great than or equal CMPDAT2 setting.
     * |[15]    |ADCMPO3   |ADC Compare 3 Output Status (Read Only)
     * |        |          |The 12 bits compare3 data CMPDAT3 (EADC_CMP3[27:16]) is used to compare with conversion result of specified sample module
     * |        |          |User can use it to monitor the external analog input pin voltage status.
     * |        |          |0 = Conversion result in EADC_DAT less than CMPDAT3 setting.
     * |        |          |1 = Conversion result in EADC_DAT great than or equal CMPDAT3 setting.
     * |[20:16] |CHANNEL   |Current Conversion Channel (Read Only)
     * |        |          |This filed reflects ADC current conversion channel when BUSY=1.
     * |        |          |It is read only.
     * |        |          |00H = EADC_CH0.
     * |        |          |01H = EADC_CH1.
     * |        |          |02H = EADC_CH2.
     * |        |          |03H = EADC_CH3.
     * |        |          |04H = EADC_CH4.
     * |        |          |05H = EADC_CH5.
     * |        |          |06H = EADC_CH6.
     * |        |          |07H = EADC_CH7.
     * |        |          |08H = EADC_CH8.
     * |        |          |09H = EADC_CH9.
     * |        |          |0AH = EADC_CH10.
     * |        |          |0BH = EADC_CH11.
     * |        |          |0CH = EADC_CH12.
     * |        |          |0DH = EADC_CH13.
     * |        |          |0EH = EADC_CH14.
     * |        |          |0FH = EADC_CH15.
     * |        |          |10H = VBG.
     * |        |          |11H = VTEMP.
     * |        |          |12H = VBAT/4.
     * |[23]    |BUSY      |Busy/Idle (Read Only)
     * |        |          |0 = EADC is in idle state.
     * |        |          |1 = EADC is busy at conversion.
     * |[24]    |ADOVIF    |All ADC Interrupt Flag Overrun Bits Check (Read Only)
     * |        |          |n=0~3.
     * |        |          |0 = None of ADINT interrupt flag ADOVIFn (EADC_STATUS2[11:8]) is overwritten to 1.
     * |        |          |1 = Any one of ADINT interrupt flag ADOVIFn (EADC_STATUS2[11:8]) is overwritten to 1.
     * |        |          |Note: This bit will keep 1 when any ADOVIFn Flag is equal to 1.
     * |[25]    |STOVF     |for All ADC Sample Module Start of Conversion Overrun Flags Check (Read Only)
     * |        |          |n=0~18.
     * |        |          |0 = None of sample module event overrun flag SPOVFn (EADC_OVSTS[n]) is set to 1.
     * |        |          |1 = Any one of sample module event overrun flag SPOVFn (EADC_OVSTS[n]) is set to 1.
     * |        |          |Note: This bit will keep 1 when any SPOVFn Flag is equal to 1.
     * |[26]    |AVALID    |for All Sample Module ADC Result Data Register EADC_DAT Data Valid Flag Check (Read Only)
     * |        |          |n=0~18.
     * |        |          |0 = None of sample module data register valid flag VALIDn (EADC_DATn[17]) is set to 1.
     * |        |          |1 = Any one of sample module data register valid flag VALIDn (EADC_DATn[17]) is set to 1.
     * |        |          |Note: This bit will keep 1 when any VALIDn Flag is equal to 1.
     * |[27]    |AOV       |for All Sample Module ADC Result Data Register Overrun Flags Check (Read Only)
     * |        |          |n=0~18.
     * |        |          |0 = None of sample module data register overrun flag OVn (EADC_DATn[16]) is set to 1.
     * |        |          |1 = Any one of sample module data register overrun flag OVn (EADC_DATn[16]) is set to 1.
     * |        |          |Note: This bit will keep 1 when any OVn Flag is equal to 1.
     * @var EADC_T::STATUS3
     * Offset: 0xFC  ADC Status Register 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[4:0]   |CURSPL    |ADC Current Sample Module
     * |        |          |This register show the current ADC is controlled by which sample module control logic modules.
     * |        |          |If the ADC is Idle, this bit filed will set to 0x1F.
     * |        |          |This is a read only register.
     * @var EADC_T::DDAT[4]
     * Offset: 0x100  ADC Double Data Register 0 for Sample Module 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |RESULT    |ADC Conversion Results
     * |        |          |This field contains 12 bits conversion results.
     * |        |          |When the DMOF (EADC_CTL[9]) is set to 0, 12-bit ADC conversion result with unsigned format will be filled in RESULT [11:0] and zero will be filled in RESULT [15:12].
     * |        |          |When DMOF (EADC_CTL[9]) set to 1, 12-bit ADC conversion result with 2'complement format will be filled in RESULT [11:0] and signed bits to will be filled in RESULT [15:12].
     * |[16]    |OV        |Overrun Flag
     * |        |          |0 = Data in RESULT (EADC_DATn[15:0], n=0~3) is recent conversion result.
     * |        |          |1 = Data in RESULT (EADC_DATn[15:0], n=0~3) is overwrite.
     * |        |          |If converted data in RESULT[15:0] has not been read before new conversion result is loaded to this register, OV is set to 1
     * |        |          |It is cleared by hardware after EADC_DDAT register is read.
     * |[17]    |VALID     |Valid Flag
     * |        |          |0 = Double data in RESULT (EADC_DDATn[15:0]) is not valid.
     * |        |          |1 = Double data in RESULT (EADC_DDATn[15:0]) is valid.
     * |        |          |This bit is set to 1 when corresponding sample module channel analog input conversion is completed and cleared by hardware after EADC_DDATn register is read
     * |        |          |(n=0~3).
     * @var EADC_T::PWRM
     * Offset: 0x110  ADC Power Management Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PWUPRDY   |ADC Power-up Sequence Completed and Ready for Conversion (Read Only)
     * |        |          |0 = ADC is not ready for conversion may be in power down state or in the progress of start up.
     * |        |          |1 = ADC is ready for conversion.
     * |[1]     |PWUCALEN  |Power Up Calibration Function Enable Control
     * |        |          |0 = Disable the function of calibration at power up.
     * |        |          |1 = Enable the function of calibration at power up.
     * |        |          |Note: This bit work together with CALSEL (EADC_CALCTL [3]), see the following
     * |        |          |{PWUCALEN, CALSEL } Description:
     * |        |          |PWUCALEN is 0 and CALSEL is 0: No need to calibrate.
     * |        |          |PWUCALEN is 0 and CALSEL is 1: No need to calibrate.
     * |        |          |PWUCALEN is 1 and CALSEL is 0: Load calibration word when power up.
     * |        |          |PWUCALEN is 1 and CALSEL is 1: Calibrate when power up.
     * |[3:2]   |PWDMOD    |ADC Power-down Mode
     * |        |          |Set this bit fields to select ADC power down mode when system power-down.
     * |        |          |00 = ADC Deep power down mode.
     * |        |          |01 = ADC Power down.
     * |        |          |10 = ADC Standby mode.
     * |        |          |11 = ADC Deep power down mode.
     * |        |          |Note: Different PWDMOD has different power down/up sequence, in order to avoid ADC powering up with wrong sequence; user must keep PWMOD consistent each time in power down and start up
     * |[19:8]  |LDOSUT    |ADC Internal LDO Start-up Time
     * |        |          |Set this bit fields to control LDO start-up time
     * |        |          |The minimum required LDO start-up time is 20us
     * |        |          |LDO start-up time = (1/ADC_CLK) x LDOSUT.
     * @var EADC_T::CALCTL
     * Offset: 0x114  ADC Calibration Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |CALSTART  |Calibration Functional Block Start
     * |        |          |0 = Stops calibration functional block.
     * |        |          |1 = Starts calibration functional block.
     * |        |          |Note: This bit is set by SW and clear by HW after re-calibration finish
     * |[2]     |CALDONE   |Calibration Functional Block Complete (Read Only)
     * |        |          |0 = During a calibration.
     * |        |          |1 = Calibration is completed.
     * |[3]     |CALSEL    |Select Calibration Functional Block
     * |        |          |0 = Load calibration word when calibration functional block is active.
     * |        |          |1 = Execute calibration when calibration functional block is active.
     * @var EADC_T::CALDWRD
     * Offset: 0x118  ADC Calibration Load Word Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[6:0]   |CALWORD   |Calibration Word Bits
     * |        |          |Write to this register with the previous calibration word before load calibration action.
     * |        |          |Read this register after calibration done.
     * |        |          |Note: The calibration block contains two parts CALIBRATION and LOAD CALIBRATION; if the calibration block configure as CALIBRATION; then this register represent the result of calibration when calibration is completed; if configure as LOAD CALIBRATION ; configure this register before loading calibration action, after loading calibration complete, the laoded calibration word will apply to the ADC; while in loading calibration function the loaded value will not be equal to the original CALWORD until calibration is done.
     */
    __I  uint32_t ADDR[32];         /*!< [0x00-0x7C] ADC Data Register 0 ~ 31                           */
    __IO uint32_t ADCR;             /*!< [0x80] ADC Control Register                                    */
    __IO uint32_t ADCHER;           /*!< [0x84] ADC Channel Enable Register                             */
    __IO uint32_t ADCMPR[2];        /*!< [0x88-0x8C] ADC Compare Register 0/1                           */
    __IO uint32_t ADSR0;            /*!< [0x90] ADC Status Register0                                    */
    __I  uint32_t ADSR1;            /*!< [0x94] ADC Status Register1                                    */
    __I  uint32_t ADSR2;            /*!< [0x98] ADC Status Register2                                    */
    __I  uint32_t RESERVE2[1];
    __IO uint32_t ESMPCTL;          /*!< [0xA0] ADC Extend Sample Time Control Register                 */
    __IO uint32_t CFDCTL;           /*!< [0xA4] ADC Channel Floating Detect Control Register            */
    __I  uint32_t RESERVE3[21];
    __IO uint32_t ADTEST;           /*!< [0xFC] ADC Test Register                                       */
    __I  uint32_t ADPDMA;           /*!< [0x100] ADC LPPDMA Current Transfer Data Register              */
    __I  uint32_t RESERVE4[31];
    __IO uint32_t ADCAL;           /*!< [0x180] ADC Calibration Mode Register                          */
    __IO uint32_t ADCALSTS;        /*!< [0x184] ADC Calibration Status Register                        */
    __IO uint32_t ADCALDB;         /*!< [0x188] ADC Calibration Debug Mode Register                    */
    __I  uint32_t RESERVE5[413];
    __IO uint32_t AUTOCTL;          /*!< [0x800] ADC Auto Operation Control Register                    */
    __O  uint32_t AUTOSTRG;         /*!< [0x804] ADC Auto Operation Software Trigger Register           */
    __I  uint32_t RESERVE6[2];
    __IO uint32_t AUTOSTS;          /*!< [0x810] ADC Auto Operation Status Register                     */
    __I  uint32_t RESERVE7[502];
    __IO uint32_t ALDOCTL;          /*!< [0xFEC] ADC ALDO Control Register                              */
    __IO uint32_t MTEST;            /*!< [0xFF0] ADC Macro Test Mode Control Register                   */
    __IO uint32_t DEBUG;            /*!< [0xFF4] ADC Debug Mode Control Register                        */
    __I  uint32_t RESERVE8[1];
    __I  uint32_t VERSION;          /*!< [0xFFC] ADC RTL Design Version Number                          */
} LPADC_T;

/**
    @addtogroup LPADC_CONST LPADC Bit Field Definition
    Constant Definitions for LPADC Controller
@{ */


#define LPADC_ADDR_RSLT_Pos             (0)                                         /*!< LPADC_T::ADDR: RSLT Position               */
#define LPADC_ADDR_RSLT_Msk             (0xffffUL << LPADC_ADDR_RSLT_Pos)           /*!< LPADC_T::ADDR: RSLT Mask                   */

#define LPADC_ADDR_OVERRUN_Pos          (16)                                        /*!< LPADC_T::ADDR: OVERRUN Position            */
#define LPADC_ADDR_OVERRUN_Msk          (0x1UL << LPADC_ADDR_OVERRUN_Pos)           /*!< LPADC_T::ADDR: OVERRUN Mask                */

#define LPADC_ADDR_VALID_Pos            (17)                                        /*!< LPADC_T::ADDR: VALID Position              */
#define LPADC_ADDR_VALID_Msk            (0x1UL << LPADC_ADDR_VALID_Pos)             /*!< LPADC_T::ADDR: VALID Mask                  */

#define LPADC_ADCR_ADEN_Pos             (0)                                         /*!< LPADC_T::ADCR: ADEN Position               */
#define LPADC_ADCR_ADEN_Msk             (0x1UL << LPADC_ADCR_ADEN_Pos)              /*!< LPADC_T::ADCR: ADEN Mask                   */

#define LPADC_ADCR_ADIE_Pos             (1)                                         /*!< LPADC_T::ADCR: ADIE Position               */
#define LPADC_ADCR_ADIE_Msk             (0x1UL << LPADC_ADCR_ADIE_Pos)              /*!< LPADC_T::ADCR: ADIE Mask                   */

#define LPADC_ADCR_ADMD_Pos             (2)                                         /*!< LPADC_T::ADCR: ADMD Position               */
#define LPADC_ADCR_ADMD_Msk             (0x3UL << LPADC_ADCR_ADMD_Pos)              /*!< LPADC_T::ADCR: ADMD Mask                   */


#define LPADC_ADCR_TRGCOND_Pos          (6)                                         /*!< LPADC_T::ADCR: TRGCOND Position            */
#define LPADC_ADCR_TRGCOND_Msk          (0x3UL << LPADC_ADCR_TRGCOND_Pos)           /*!< LPADC_T::ADCR: TRGCOND Mask                */

#define LPADC_ADCR_TRGEN_Pos            (8)                                         /*!< LPADC_T::ADCR: TRGEN Position              */
#define LPADC_ADCR_TRGEN_Msk            (0x1UL << LPADC_ADCR_TRGEN_Pos)             /*!< LPADC_T::ADCR: TRGEN Mask                  */

#define LPADC_ADCR_PTEN_Pos             (9)                                         /*!< LPADC_T::ADCR: PTEN Position               */
#define LPADC_ADCR_PTEN_Msk             (0x1UL << LPADC_ADCR_PTEN_Pos)              /*!< LPADC_T::ADCR: PTEN Mask                   */

#define LPADC_ADCR_DIFFEN_Pos           (10)                                        /*!< LPADC_T::ADCR: DIFFEN Position             */
#define LPADC_ADCR_DIFFEN_Msk           (0x1UL << LPADC_ADCR_DIFFEN_Pos)            /*!< LPADC_T::ADCR: DIFFEN Mask                 */

#define LPADC_ADCR_ADST_Pos             (11)                                        /*!< LPADC_T::ADCR: ADST Position               */
#define LPADC_ADCR_ADST_Msk             (0x1UL << LPADC_ADCR_ADST_Pos)              /*!< LPADC_T::ADCR: ADST Mask                   */

#define LPADC_ADCR_RESET_Pos            (12)                                        /*!< LPADC_T::ADCR: RESET Position              */
#define LPADC_ADCR_RESET_Msk            (0x1UL << LPADC_ADCR_RESET_Pos)             /*!< LPADC_T::ADCR: RESET Mask                  */

#define LPADC_ADCR_PRELDCH_Pos          (15)                                        /*!< LPADC_T::ADCR: PRELDCH Position            */
#define LPADC_ADCR_PRELDCH_Msk          (0x1UL << LPADC_ADCR_PRELDCH_Pos)           /*!< LPADC_T::ADCR: PRELDCH Mask                */

#define LPADC_ADCR_TRGS_Pos             (16)                                         /*!< LPADC_T::ADCR: TRGS Position               */
#define LPADC_ADCR_TRGS_Msk             (0xfUL << LPADC_ADCR_TRGS_Pos)              /*!< LPADC_T::ADCR: TRGS Mask                   */

///ChingI wait to check
#define LPADC_ADCR_ACMPTES_Pos          (20)                          
#define LPADC_ADCR_ACMPTES_Msk          (0x3UL << LPADC_ADCR_ACMPTES_Pos)

#define LPADC_ADCR_ULPEN_Pos            (24)                                        /*!< LPADC_T::ADCR: ULPEN Position              */
#define LPADC_ADCR_ULPEN_Msk            (0x1UL << LPADC_ADCR_ULPEN_Pos)             /*!< LPADC_T::ADCR: ULPEN Mask                  */

#define LPADC_ADCR_DECSET_Pos           (25)                                        /*!< LPADC_T::ADCR: DECSET Position             */
#define LPADC_ADCR_DECSET_Msk           (0x1UL << LPADC_ADCR_DECSET_Pos)            /*!< LPADC_T::ADCR: DECSET Mask                 */

#define LPADC_ADCR_RESSEL_Pos           (28)                                        /*!< LPADC_T::ADCR: RESSEL Position             */
#define LPADC_ADCR_RESSEL_Msk           (0x3UL << LPADC_ADCR_RESSEL_Pos)            /*!< LPADC_T::ADCR: RESSEL Mask                 */

#define LPADC_ADCR_DMOF_Pos             (31)                                        /*!< LPADC_T::ADCR: DMOF Position               */
#define LPADC_ADCR_DMOF_Msk             (0x1UL << LPADC_ADCR_DMOF_Pos)              /*!< LPADC_T::ADCR: DMOF Mask                   */

#define LPADC_ADCHER_CHEN_Pos           (0)                                         /*!< LPADC_T::ADCHER: CHEN Position             */
#define LPADC_ADCHER_CHEN_Msk           (0xffffffffUL << LPADC_ADCHER_CHEN_Pos)     /*!< LPADC_T::ADCHER: CHEN Mask                 */

#define LPADC_ADCMPR_CMPEN_Pos          (0)                                         /*!< LPADC_T::ADCMPR: CMPEN Position            */
#define LPADC_ADCMPR_CMPEN_Msk          (0x1UL << LPADC_ADCMPR_CMPEN_Pos)           /*!< LPADC_T::ADCMPR: CMPEN Mask                */

#define LPADC_ADCMPR_CMPIE_Pos          (1)                                         /*!< LPADC_T::ADCMPR: CMPIE Position            */
#define LPADC_ADCMPR_CMPIE_Msk          (0x1UL << LPADC_ADCMPR_CMPIE_Pos)           /*!< LPADC_T::ADCMPR: CMPIE Mask                */

#define LPADC_ADCMPR_CMPCOND_Pos        (2)                                         /*!< LPADC_T::ADCMPR: CMPCOND Position          */
#define LPADC_ADCMPR_CMPCOND_Msk        (0x1UL << LPADC_ADCMPR_CMPCOND_Pos)         /*!< LPADC_T::ADCMPR: CMPCOND Mask              */

#define LPADC_ADCMPR_CMPCH_Pos          (3)                                         /*!< LPADC_T::ADCMPR: CMPCH Position            */
#define LPADC_ADCMPR_CMPCH_Msk          (0x1fUL << LPADC_ADCMPR_CMPCH_Pos)          /*!< LPADC_T::ADCMPR: CMPCH Mask                */

#define LPADC_ADCMPR_CMPMATCNT_Pos      (8)                                         /*!< LPADC_T::ADCMPR: CMPMATCNT Position        */
#define LPADC_ADCMPR_CMPMATCNT_Msk      (0xfUL << LPADC_ADCMPR_CMPMATCNT_Pos)       /*!< LPADC_T::ADCMPR: CMPMATCNT Mask            */

#define LPADC_ADCMPR_CMPWEN_Pos         (15)                                        /*!< LPADC_T::ADCMPR: CMPWEN Position           */
#define LPADC_ADCMPR_CMPWEN_Msk         (0x1UL << LPADC_ADCMPR_CMPWEN_Pos)          /*!< LPADC_T::ADCMPR: CMPWEN Mask               */

#define LPADC_ADCMPR_CMPD_Pos           (16)                                        /*!< LPADC_T::ADCMPR: CMPD Position             */
#define LPADC_ADCMPR_CMPD_Msk           (0xfffUL << LPADC_ADCMPR_CMPD_Pos)          /*!< LPADC_T::ADCMPR: CMPD Mask                 */

#define LPADC_ADSR0_ADF_Pos             (0)                                         /*!< LPADC_T::ADSR0: ADF Position               */
#define LPADC_ADSR0_ADF_Msk             (0x1UL << LPADC_ADSR0_ADF_Pos)              /*!< LPADC_T::ADSR0: ADF Mask                   */

#define LPADC_ADSR0_CMPF0_Pos           (1)                                         /*!< LPADC_T::ADSR0: CMPF0 Position             */
#define LPADC_ADSR0_CMPF0_Msk           (0x1UL << LPADC_ADSR0_CMPF0_Pos)            /*!< LPADC_T::ADSR0: CMPF0 Mask                 */

#define LPADC_ADSR0_CMPF1_Pos           (2)                                         /*!< LPADC_T::ADSR0: CMPF1 Position             */
#define LPADC_ADSR0_CMPF1_Msk           (0x1UL << LPADC_ADSR0_CMPF1_Pos)            /*!< LPADC_T::ADSR0: CMPF1 Mask                 */

#define LPADC_ADSR0_BUSY_Pos            (7)                                         /*!< LPADC_T::ADSR0: BUSY Position              */
#define LPADC_ADSR0_BUSY_Msk            (0x1UL << LPADC_ADSR0_BUSY_Pos)             /*!< LPADC_T::ADSR0: BUSY Mask                  */

#define LPADC_ADSR0_VALIDF_Pos          (8)                                         /*!< LPADC_T::ADSR0: VALIDF Position            */
#define LPADC_ADSR0_VALIDF_Msk          (0x1UL << LPADC_ADSR0_VALIDF_Pos)           /*!< LPADC_T::ADSR0: VALIDF Mask                */

#define LPADC_ADSR0_OVERRUNF_Pos        (16)                                        /*!< LPADC_T::ADSR0: OVERRUNF Position          */
#define LPADC_ADSR0_OVERRUNF_Msk        (0x1UL << LPADC_ADSR0_OVERRUNF_Pos)         /*!< LPADC_T::ADSR0: OVERRUNF Mask              */

#define LPADC_ADSR0_ADPRDY_Pos          (24)                                        /*!< LPADC_T::ADSR0: ADPRDY Position          */
#define LPADC_ADSR0_ADPRDY_Msk          (0x1UL << LPADC_ADSR0_ADPRDY_Pos)           /*!< LPADC_T::ADSR0: ADPRDY Mask              */

#define LPADC_ADSR0_CHANNEL_Pos         (27)                                        /*!< LPADC_T::ADSR0: CHANNEL Position           */
#define LPADC_ADSR0_CHANNEL_Msk         (0x1fUL << LPADC_ADSR0_CHANNEL_Pos)         /*!< LPADC_T::ADSR0: CHANNEL Mask               */

#define LPADC_ADSR1_VALID_Pos           (0)                                         /*!< LPADC_T::ADSR1: VALID Position             */
#define LPADC_ADSR1_VALID_Msk           (0xffffffffUL << LPADC_ADSR1_VALID_Pos)     /*!< LPADC_T::ADSR1: VALID Mask                 */

#define LPADC_ADSR2_OVERRUN_Pos         (0)                                         /*!< LPADC_T::ADSR2: OVERRUN Position           */
#define LPADC_ADSR2_OVERRUN_Msk         (0xffffffffUL << LPADC_ADSR2_OVERRUN_Pos)   /*!< LPADC_T::ADSR2: OVERRUN Mask               */

#define LPADC_ESMPCTL_EXTSMPT_Pos       (0)                                         /*!< LPADC_T::ESMPCTL: EXTSMPT Position         */
#define LPADC_ESMPCTL_EXTSMPT_Msk       (0x3fffUL << LPADC_ESMPCTL_EXTSMPT_Pos)     /*!< LPADC_T::ESMPCTL: EXTSMPT Mask             */

#define LPADC_CFDCTL_PRECHEN_Pos        (0)                                         /*!< LPADC_T::CFDCTL: PRECHEN Position          */
#define LPADC_CFDCTL_PRECHEN_Msk        (0x1UL << LPADC_CFDCTL_PRECHEN_Pos)         /*!< LPADC_T::CFDCTL: PRECHEN Mask              */

#define LPADC_CFDCTL_DISCHEN_Pos        (1)                                         /*!< LPADC_T::CFDCTL: DISCHEN Position          */
#define LPADC_CFDCTL_DISCHEN_Msk        (0x1UL << LPADC_CFDCTL_DISCHEN_Pos)         /*!< LPADC_T::CFDCTL: DISCHEN Mask              */

#define LPADC_CFDCTL_FDETCHEN_Pos       (8)                                         /*!< LPADC_T::CFDCTL: FDETCHEN Position         */
#define LPADC_CFDCTL_FDETCHEN_Msk       (0x1UL << LPADC_CFDCTL_FDETCHEN_Pos)        /*!< LPADC_T::CFDCTL: FDETCHEN Mask             */

#define LPADC_ADTEST_FIFOCNT_Pos        (0)                                         /*!< LPADC_T::ADTEST: FIFOCNT Position          */
#define LPADC_ADTEST_FIFOCNT_Msk        (0xfUL << LPADC_ADTEST_FIFOCNT_Pos)         /*!< LPADC_T::ADTEST: FIFOCNT Mask              */

#define LPADC_ADTEST_CalUnLock_Pos      (28)                                        /*!< LPADC_T::ADTEST: CalUnLock Position        */
#define LPADC_ADTEST_CalUnLock_Msk      (0x1UL << LPADC_ADTEST_CalUnLock_Pos)       /*!< LPADC_T::ADTEST: CalUnLock Mask            */

#define LPADC_ADPDMA_CURDAT_Pos         (0)                                         /*!< LPADC_T::ADPDMA: CURDAT Position           */
#define LPADC_ADPDMA_CURDAT_Msk         (0x3ffffUL << LPADC_ADPDMA_CURDAT_Pos)      /*!< LPADC_T::ADPDMA: CURDAT Mask               */

#define LPADC_ADCAL_CALEN_Pos          (0)                                         /*!< LPADC_T::ADCAL: CALEN Position            */
#define LPADC_ADCAL_CALEN_Msk          (0x1UL << LPADC_ADCAL_CALEN_Pos)           /*!< LPADC_T::ADCAL: CALEN Mask                */

#define LPADC_ADCAL_CALIE_Pos          (1)                                         /*!< LPADC_T::ADCAL: CALIE Position            */
#define LPADC_ADCAL_CALIE_Msk          (0x1UL << LPADC_ADCAL_CALIE_Pos)           /*!< LPADC_T::ADCAL: CALIE Mask                */

#define LPADC_ADCAL_MODEVM_Pos         (2)                                         /*!< LPADC_T::ADCAL: MODEVM Position           */
#define LPADC_ADCAL_MODEVM_Msk         (0x1UL << LPADC_ADCAL_MODEVM_Pos)          /*!< LPADC_T::ADCAL: MODEVM Mask               */

#define LPADC_ADCAL_CALSEL16T_Pos      (3)                                         /*!< LPADC_T::ADCAL: CALSEL16T Position        */
#define LPADC_ADCAL_CALSEL16T_Msk      (0x1UL << LPADC_ADCAL_CALSEL16T_Pos)       /*!< LPADC_T::ADCAL: CALSEL16T Mask            */

#define LPADC_ADCAL_CALSEL_Pos         (4)                                         /*!< LPADC_T::ADCAL: CALSEL Position           */
#define LPADC_ADCAL_CALSEL_Msk         (0x7UL << LPADC_ADCAL_CALSEL_Pos)          /*!< LPADC_T::ADCAL: CALSEL Mask               */

#define LPADC_ADCALSTS_CALIF_Pos       (0)                                         /*!< LPADC_T::ADCALSTS: CALIF Position         */
#define LPADC_ADCALSTS_CALIF_Msk       (0x1UL << LPADC_ADCALSTS_CALIF_Pos)        /*!< LPADC_T::ADCALSTS: CALIF Mask             */

#define LPADC_ADCALDB_CALRD_Pos        (0)                                         /*!< LPADC_T::ADCALDB: CALRD Position          */
#define LPADC_ADCALDB_CALRD_Msk        (0x1UL << LPADC_ADCALDB_CALRD_Pos)         /*!< LPADC_T::ADCALDB: CALRD Mask              */

#define LPADC_ADCALDB_CALWR_Pos        (1)                                         /*!< LPADC_T::ADCALDB: CALWR Position          */
#define LPADC_ADCALDB_CALWR_Msk        (0x1UL << LPADC_ADCALDB_CALWR_Pos)         /*!< LPADC_T::ADCALDB: CALWR Mask              */

#define LPADC_ADCALDB_OUTSEL_Pos      (2)                                         /*!< LPADC_T::ADCALDB: OUTSEL Position        */
#define LPADC_ADCALDB_OUTSEL_Msk      (0x1UL << LPADC_ADCALDB_OUTSEL_Pos)       /*!< LPADC_T::ADCALDB: OUTSEL Mask            */

#define LPADC_ADCALDB_CALADDR_Pos      (3)                                         /*!< LPADC_T::ADCALDB: CALADDR Position        */
#define LPADC_ADCALDB_CALADDR_Msk      (0x1fUL << LPADC_ADCALDB_CALADDR_Pos)      /*!< LPADC_T::ADCALDB: CALADDR Mask            */

#define LPADC_ADCALDB_CALWDATA_Pos     (8)                                         /*!< LPADC_T::ADCALDB: CALWDATA Position       */
#define LPADC_ADCALDB_CALWDATA_Msk     (0xffUL << LPADC_ADCALDB_CALWDATA_Pos)     /*!< LPADC_T::ADCALDB: CALWDATA Mask           */

#define LPADC_ADCALDB_CALRDATA_Pos     (16)                                        /*!< LPADC_T::ADCALDB: CALRDATA Position       */
#define LPADC_ADCALDB_CALRDATA_Msk     (0xfffUL << LPADC_ADCALDB_CALRDATA_Pos)    /*!< LPADC_T::ADCALDB: CALRDATA Mask           */

#define LPADC_ADCALDB_DEBUGEN_Pos      (31)                                        /*!< LPADC_T::ADCALDB: DEBUGEN Position        */
#define LPADC_ADCALDB_DEBUGEN_Msk      (0x1UL << LPADC_ADCALDB_DEBUGEN_Pos)       /*!< LPADC_T::ADCALDB: DEBUGEN Mask            */

#define LPADC_AUTOCTL_TRIGSEL_Pos       (0)                                         /*!< LPADC_T::AUTOCTL: TRIGSEL Position         */
#define LPADC_AUTOCTL_TRIGSEL_Msk       (0x7UL << LPADC_AUTOCTL_TRIGSEL_Pos)        /*!< LPADC_T::AUTOCTL: TRIGSEL Mask             */

#define LPADC_AUTOCTL_TRIGEN_Pos        (4)                                         /*!< LPADC_T::AUTOCTL: TRIGEN Position          */
#define LPADC_AUTOCTL_TRIGEN_Msk        (0x1UL << LPADC_AUTOCTL_TRIGEN_Pos)         /*!< LPADC_T::AUTOCTL: TRIGEN Mask              */

#define LPADC_AUTOCTL_WKEN_Pos          (8)                                         /*!< LPADC_T::AUTOCTL: WKEN Position            */
#define LPADC_AUTOCTL_WKEN_Msk          (0x1UL << LPADC_AUTOCTL_WKEN_Pos)           /*!< LPADC_T::AUTOCTL: WKEN Mask                */

#define LPADC_AUTOCTL_CMP0WKEN_Pos      (9)                                         /*!< LPADC_T::AUTOCTL: CMP0WKEN Position            */
#define LPADC_AUTOCTL_CMP0WKEN_Msk      (0x1UL << LPADC_AUTOCTL_CMP0WKEN_Pos)           /*!< LPADC_T::AUTOCTL: CMP0WKEN Mask                */

#define LPADC_AUTOCTL_CMP1WKEN_Pos      (10)                                        /*!< LPADC_T::AUTOCTL: CMP1WKEN Position            */
#define LPADC_AUTOCTL_CMP1WKEN_Msk      (0x1UL << LPADC_AUTOCTL_CMP1WKEN_Pos)       /*!< LPADC_T::AUTOCTL: CMP1WKEN Mask                */

#define LPADC_AUTOCTL_AUTOEN_Pos        (31)                                        /*!< LPADC_T::AUTOCTL: AUTOEN Position          */
#define LPADC_AUTOCTL_AUTOEN_Msk        (0x1UL << LPADC_AUTOCTL_AUTOEN_Pos)         /*!< LPADC_T::AUTOCTL: AUTOEN Mask              */

#define LPADC_AUTOSTRG_SWTRIG_Pos       (0)                                         /*!< LPADC_T::AUTOSTRG: SWTRIG Position         */
#define LPADC_AUTOSTRG_SWTRIG_Msk       (0x1UL << LPADC_AUTOSTRG_SWTRIG_Pos)        /*!< LPADC_T::AUTOSTRG: SWTRIG Mask             */

#define LPADC_AUTOSTS_WKIF_Pos          (0)                                         /*!< LPADC_T::AUTOSTS: WKIF Position            */
#define LPADC_AUTOSTS_WKIF_Msk          (0x1UL << LPADC_AUTOSTS_WKIF_Pos)           /*!< LPADC_T::AUTOSTS: WKIF Mask                */

#define LPADC_AUTOSTS_CMP0WKF_Pos       (1)                                      /*!< LPADC_T::AUTOSTS: CMP0WKF Position            */
#define LPADC_AUTOSTS_CMP0WKF_Msk       (0x1UL << LPADC_AUTOSTS_CMP0WKF_Pos)     /*!< LPADC_T::AUTOSTS: CMP0WKF Mask                */

#define LPADC_AUTOSTS_CMP1WKF_Pos       (2)                                      /*!< LPADC_T::AUTOSTS: CMP1WKF Position            */
#define LPADC_AUTOSTS_CMP1WKF_Msk       (0x1UL << LPADC_AUTOSTS_CMP1WKF_Pos)     /*!< LPADC_T::AUTOSTS: CMP1WKF Mask                */

#define LPADC_ALDOCTL_VTRIM_Pos         (0)                                         /*!< LPADC_T::ALDOCTL: VTRIM Position           */
#define LPADC_ALDOCTL_VTRIM_Msk         (0x7UL << LPADC_ALDOCTL_VTRIM_Pos)          /*!< LPADC_T::ALDOCTL: VTRIM Mask               */

#define LPADC_ALDOCTL_ISEL_Pos          (4)                                         /*!< LPADC_T::ALDOCTL: ISEL Position            */
#define LPADC_ALDOCTL_ISEL_Msk          (0x3UL << LPADC_ALDOCTL_ISEL_Pos)           /*!< LPADC_T::ALDOCTL: ISEL Mask                */

#define LPADC_ALDOCTL_TESTEN_Pos        (7)                                         /*!< LPADC_T::ALDOCTL: TESTEN Position          */
#define LPADC_ALDOCTL_TESTEN_Msk        (0x1UL << LPADC_ALDOCTL_TESTEN_Pos)         /*!< LPADC_T::ALDOCTL: TESTEN Mask              */

#define LPADC_ALDOCTL_ALDOEN_Pos        (8)                                         /*!< LPADC_T::ALDOCTL: ALDOEN Position          */
#define LPADC_ALDOCTL_ALDOEN_Msk        (0x1UL << LPADC_ALDOCTL_ALDOEN_Pos)         /*!< LPADC_T::ALDOCTL: ALDOEN Mask              */

#define LPADC_ALDOCTL_STBSEL_Pos        (16)                                        /*!< LPADC_T::ALDOCTL: STBSEL Position          */
#define LPADC_ALDOCTL_STBSEL_Msk        (0xfUL << LPADC_ALDOCTL_STBSEL_Pos)         /*!< LPADC_T::ALDOCTL: STBSEL Mask              */

#define LPADC_ALDOCTL_IGENEN_Pos        (23)                                        /*!< LPADC_T::ALDOCTL: IGENEN Position          */
#define LPADC_ALDOCTL_IGENEN_Msk        (0x1UL << LPADC_ALDOCTL_IGENEN_Pos)         /*!< LPADC_T::ALDOCTL: IGENEN Mask              */

#define LPADC_MTEST_PCHSEN_Pos          (0)                                         /*!< LPADC_T::MTEST: PCHSEN Position            */
#define LPADC_MTEST_PCHSEN_Msk          (0x1UL << LPADC_MTEST_PCHSEN_Pos)           /*!< LPADC_T::MTEST: PCHSEN Mask                */

#define LPADC_MTEST_PCHSSEL_Pos         (4)                                         /*!< LPADC_T::MTEST: PCHSSEL Position           */
#define LPADC_MTEST_PCHSSEL_Msk         (0x7UL << LPADC_MTEST_PCHSSEL_Pos)          /*!< LPADC_T::MTEST: PCHSSEL Mask               */

#define LPADC_MTEST_DECADD_Pos          (8)                                         /*!< LPADC_T::MTEST: DECADD Position            */
#define LPADC_MTEST_DECADD_Msk          (0x1UL << LPADC_MTEST_DECADD_Pos)           /*!< LPADC_T::MTEST: DECADD Mask                */

#define LPADC_MTEST_OSR_Pos             (24)                                        /*!< LPADC_T::MTEST: OSR Position               */
#define LPADC_MTEST_OSR_Msk             (0xffUL << LPADC_MTEST_OSR_Pos)             /*!< LPADC_T::MTEST: OSR Mask                   */

#define LPADC_DEBUG_ENFLO_Pos           (0)                                         /*!< LPADC_T::DEBUG: ENFLO Position             */
#define LPADC_DEBUG_ENFLO_Msk           (0x1UL << LPADC_DEBUG_ENFLO_Pos)            /*!< LPADC_T::DEBUG: ENFLO Mask                 */

#define LPADC_DEBUG_ENBOOST_Pos         (1)                                         /*!< LPADC_T::DEBUG: ENBOOST Position           */
#define LPADC_DEBUG_ENBOOST_Msk         (0x1UL << LPADC_DEBUG_ENBOOST_Pos)          /*!< LPADC_T::DEBUG: ENBOOST Mask               */

#define LPADC_DEBUG_SELAYNC_Pos         (4)                                         /*!< LPADC_T::DEBUG: SELAYNC Position           */
#define LPADC_DEBUG_SELAYNC_Msk         (0x1UL << LPADC_DEBUG_SELAYNC_Pos)          /*!< LPADC_T::DEBUG: SELAYNC Mask               */

#define LPADC_DEBUG_SELASYF_Pos         (5)                                         /*!< LPADC_T::DEBUG: SELASYF Position           */
#define LPADC_DEBUG_SELASYF_Msk         (0x1UL << LPADC_DEBUG_SELASYF_Pos)          /*!< LPADC_T::DEBUG: SELASYF Mask               */

#define LPADC_DEBUG_ASDELAYC_Pos        (8)                                         /*!< LPADC_T::DEBUG: ASDELAYC Position          */
#define LPADC_DEBUG_ASDELAYC_Msk        (0x3UL << LPADC_DEBUG_ASDELAYC_Pos)         /*!< LPADC_T::DEBUG: ASDELAYC Mask              */

#define LPADC_DEBUG_ASDELAYF_Pos        (10)                                        /*!< LPADC_T::DEBUG: ASDELAYF Position          */
#define LPADC_DEBUG_ASDELAYF_Msk        (0x3UL << LPADC_DEBUG_ASDELAYF_Pos)         /*!< LPADC_T::DEBUG: ASDELAYF Mask              */

#define LPADC_DEBUG_TRIMSMPL_Pos        (16)                                        /*!< LPADC_T::DEBUG: TRIMSMPL Position          */
#define LPADC_DEBUG_TRIMSMPL_Msk        (0xffUL << LPADC_DEBUG_TRIMSMPL_Pos)        /*!< LPADC_T::DEBUG: TRIMSMPL Mask              */

#define LPADC_DEBUG_DEBUGEN_Pos         (31)                                        /*!< LPADC_T::DEBUG: DEBUGEN Position           */
#define LPADC_DEBUG_DEBUGEN_Msk         (0x1UL << LPADC_DEBUG_DEBUGEN_Pos)          /*!< LPADC_T::DEBUG: DEBUGEN Mask               */

#define LPADC_VERSION_MINOR_Pos       (0)                                         /*!< LPADC_T::VERSION: MINOR Position         */
#define LPADC_VERSION_MINOR_Msk       (0xffffUL << LPADC_VERSION_MINOR_Pos)     /*!< LPADC_T::VERSION: MINOR Mask             */

#define LPADC_VERSION_SUB_Pos         (16)                                        /*!< LPADC_T::VERSION: SUB Position           */
#define LPADC_VERSION_SUB_Msk         (0xffUL << LPADC_VERSION_SUB_Pos)         /*!< LPADC_T::VERSION: SUB Mask               */

#define LPADC_VRSION_MAJOR_Pos       (24)                                        /*!< LPADC_T::VERSION: MAJOR Position         */
#define LPADC_VERSION_MAJOR_Msk       (0xffUL << LPADC_VRSION_MAJOR_Pos)       /*!< LPADC_T::VERSION: MAJOR Mask             */


/** @} LPADC_CONST */
/** @} end of LPADC register group */
/** @} end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __LPADC_REG_H__ */
