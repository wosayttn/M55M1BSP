#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>

//------------------------------------------------------------------------------
// SPIM Macro
//------------------------------------------------------------------------------
#define C_SPIM0                     (0)
#define C_SPIM1                     (1)

#define FLASH_BLOCK_SIZE            0x1000//0x10000
#define BUFFER_SIZE                 0x200

//------------------------------------------------------------------------------
// Common Macro
//------------------------------------------------------------------------------
//#define AUTO_RUN_TEST                   (0)
#define MULTIPLITER                     (12000000UL / 4000000)

#define YES                             (1)
#define NO                              (0)

#define USE_HXT_SRC                     (0)

//#define FOR_SIMULATION

#ifdef FOR_SIMULATION
    #define log_info(...)
#else
    #define log_info printf
#endif

#define RED_BOLD          "\x1b[;31;1m"
#define BLU_BOLD          "\x1b[;34;1m"
#define YEL_BOLD          "\x1b[;33;1m"
#define GRN_BOLD          "\x1b[;32;1m"
#define CYAN_BOLD_ITALIC  "\x1b[;36;1;3m"
#define RESET             "\x1b[0;m"

/**
  * @brief Get a 64-bit unsigned value from specified address
  * @param[in] addr Address to get 64-bit data from
  * @return  64-bit unsigned value stored in specified address
  * @note The input address must be 64-bit aligned
  */
#define M64(addr) (*((vu64 *) (addr)))

/**
  * @brief Set a 64-bit unsigned value to specified I/O port
  * @param[in] port Port address to set 64-bit data
  * @param[in] value Value to write to I/O port
  * @return  None
  * @note The output port must be 64-bit aligned
  */
#define outpll(port,value)   (*((volatile unsigned long long *)(port))=(value))

/**
  * @brief Get a 64-bit unsigned value from specified I/O port
  * @param[in] port Port address to get 64-bit data from
  * @return  64-bit unsigned value stored in specified I/O port
  * @note The input port must be 64-bit aligned
  */
#define inpll(port)          ((*((volatile unsigned long long *)(port))))

//------------------------------------------------------------------------------
typedef struct
{
    uint32_t u32CMD;
    uint32_t u32DummyCycle;
    uint32_t u32Is4ByteAddr;
    uint32_t u32CMDPhase;
    uint32_t u32AddrPhase;
    uint32_t u32DataPhase;
    uint32_t u32TransMode;
    //uint32_t u32ContMode; //Support Continue Read Mode
    uint32_t u32DTRMode;    //Support Double Data Rate Mode
}  CMD_PHASE_T;

typedef struct
{
    char *chName;
} S_TestOption;

typedef struct
{
    char *chName;
    int (*fp)(uint32_t);
    uint32_t u32AutoTest;
} S_TestMenu;

//------------------------------------------------------------------------------
// Common Define Function Prototypes
//------------------------------------------------------------------------------
void *GetSPIMModule(uint32_t u32Idx);
uint32_t GetSPIMTestModuleIdx(void);
void SetSPIMTestModuleIdx(uint32_t u32SetValue);
uint32_t GetDmmModeMapAddr(SPIM_T *pSPIMx);

void *GetTestBuffer1(void);
void *GetTestBuffer2(void);

void DelayUs(volatile uint32_t micros);
void DelayMs(volatile uint32_t mills);

uint32_t sysGetNum(void);
uint32_t GetRequireOptions(S_TestOption *pOption, uint32_t u32Size);
void RunTestFunction(S_TestMenu *pTestMenu, uint32_t u32MenuSize, uint32_t u32SPIMModule);

int EnableAutoRunTest(uint32_t u32SPIMModule);
int DisableAutoRunTest(uint32_t u32SPIMModule);

void popDat(uint8_t *buf, uint32_t bufSize);
void dump_compare_error(uint32_t addr, uint8_t *buf_expect, uint8_t *buf_compare, int count);

//SPIM API
void SPIM_Hyper_DefaultConfig(SPIM_T *spim, uint32_t u32CSMaxLow, uint32_t u32AcctRD, uint32_t u32AcctWR);
uint32_t GetSPIMClkDivNum(void);
void SPIM_NVIC_Enable(uint32_t u32SPIMModule);
void SPIM_NVIC_Disable(uint32_t u32SPIMModule);
void InitSPIMPort(uint32_t u32SPIMModule);
void SPIMx_SysReset(uint32_t u32SPIMModule);
int SPIMPortSelect(uint32_t u32SPIMPort);
void SPIM_EraseAddrRange(SPIM_T *pSPIMx, uint32_t u32Addr, int is4ByteAddr, uint32_t len, uint32_t u32ErsCmd, uint32_t u32ErsBlkSize, uint32_t u32NBit);

void SPIM_DMA_WritePhase(SPIM_T *spim, SPIM_PHASE_T *psPhaseTable,
                         uint32_t u32Addr, uint8_t *pu8TxBuf, uint32_t u32WrSize);
int32_t SPIM_DMA_ReadPhase(SPIM_T *spim, SPIM_PHASE_T *psPhaseTable,
                           uint32_t u32Addr, uint8_t *pu8RxBuf,
                           uint32_t u32RdSize, int isSync);
void SPIM_DMM_ReadPhase(SPIM_T *spim, SPIM_PHASE_T *psPhaseTable, int is4ByteAddr, uint32_t u32IdleIntvl);

#endif /* _COMMON_H_ */
