
#ifndef _TEST_FUN_H_
#define _TEST_FUN_H_

/*
 * If target chip has only one USCI-I2C port, define SINGLE_UI2C_PORT to test with I2C0
 *   UI2C0 default MFP: SDA (PA.10) and SCL (PA.11)
 *   UI2C1 default MFP: SDA (PB.2) and SCL (PB.1)
 *   I2C0 default MFP: SDA (PA.4) and SCL (PA.5)
 */
#define  SINGLE_UI2C_PORT

extern CU_SuiteInfo suites[];
extern CU_TestInfo UI2C_ConstTests[];
extern CU_TestInfo UI2C_ApiTests[];
extern CU_TestInfo UI2C_MacroTests[];
extern CU_TestInfo UI2C_WakeupAPIMacroTests[];

//#define POWER_DOWN_TEST

#endif
