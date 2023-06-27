/*----------------------------------------------------------------------------*/
/*  Copyright(c) 2015 Nuvoton Technology Corp. All rights reserved.           */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
//#include "spim_test.h"
//#include "hr_test.h"
//#include "hf_test.h"
//#include "otfc_test.h"
#include "common.h"
#include "spim_common.h"
#include "otfc_common.h"

#include "flash_test.h"

//------------------------------------------------------------------------------
extern int RegisterTest(uint32_t u32SPIMModule);

//------------------------------------------------------------------------------
static void SPIMMainTestMenu(uint32_t u32SPIMModule)
{
    uint32_t u32Index = 0;
    uint32_t u32PortIndex = 0;
    S_TestMenu sTestMenu[] =
    {
        {"Register Test", &RegisterTest},

        {"SPI Flash Test Menu", &Flash_TestMenu},
        //{"OTFC Test Menu\r\n", &OTFCTestMenu},

        //{"MT35x Octal SPI Test Menu\r\n", &MT35xTestMenu},

        //{"HyperRAM Test Menu", &HyperRAMTestMenu},
        //{"HyperFlash Test Menu\r\n", &HyperFlashTestMenu},

        //{"Enable Auto Test", &EnableAutoRunTest},
        //{"Disable Auto Test\r\n", &DisableAutoRunTest},

        {"Select SPIM/OTFC Module\r\n", &SPIM_SelectModule},
    };

    while (1)
    {
        printf("\r\n");

        printf("+---------------------------------------------------------+\n");
        printf("|  SPIM Test Menu                                         |\n");
        printf("+---------------------------------------------------------+\n");

        for (u32Index = 0; u32Index < sizeof(sTestMenu) / sizeof(sTestMenu[0]); u32Index++)
        {
            printf("[%d]%s\r\n",
                   u32Index,
                   sTestMenu[u32Index].chName);
        }

        printf("\r\n");

        u32Index = sysGetNum();

        if (u32Index >= sizeof(sTestMenu) / sizeof(sTestMenu[0]))
        {
            continue;
        }

        printf("===========================================================\r\n");
        printf("Enter The %s\r\n", sTestMenu[u32Index].chName);
        printf("===========================================================\r\n");

        if (sTestMenu[u32Index].fp != NULL)
        {
            u32PortIndex = sTestMenu[u32Index].fp(u32SPIMModule);

            if (strcmp(sTestMenu[u32Index].chName, "Select SPIM/OTFC Module\r\n") == 0)
            {
                u32SPIMModule = u32PortIndex;
                SPIM_SysReset(u32SPIMModule);
            }
        }
    }
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_SRCCTL_HIRCEN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Enable PLL0 180MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HIRC, FREQ_180MHZ, CLK_APLL0_SELECT);

    /* Switch SCLK clock source to PLL0 and divide 1 */
    CLK_SetSCLK(CLK_SCLKSEL_SCLKSEL_APLL0);

    /* Set HCLK2 divide 2 */
    CLK_SET_HCLK2DIV(2);

    /* Set PCLKx divide 2 */
    CLK_SET_PCLK0DIV(2);
    CLK_SET_PCLK1DIV(2);
    CLK_SET_PCLK2DIV(2);
    CLK_SET_PCLK3DIV(2);
    CLK_SET_PCLK4DIV(2);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable CRYPTO and TRNG and Key Store module clock */
    CLK_EnableModuleClock(CRYPTO0_MODULE);
    CLK_EnableModuleClock(TRNG0_MODULE);
    CLK_EnableModuleClock(KS0_MODULE);
    CLK_EnableModuleClock(SRAM0_MODULE);
    CLK_EnableModuleClock(SRAM1_MODULE);
    CLK_EnableModuleClock(SRAM2_MODULE);
    CLK_EnableModuleClock(LPSRAM0_MODULE);
    CLK_EnableModuleClock(CANFD0_MODULE);
    CLK_EnableModuleClock(CANFD1_MODULE);

    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);

    CLK_EnableModuleClock(ST0_MODULE);
    CLK_EnableSysTick(CLK_STSEL_ACLK, 0xFFFFFF);

    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOD_MODULE);
    CLK_EnableModuleClock(GPIOE_MODULE);
    CLK_EnableModuleClock(GPIOF_MODULE);
    CLK_EnableModuleClock(GPIOG_MODULE);
    CLK_EnableModuleClock(GPIOH_MODULE);
    CLK_EnableModuleClock(GPIOI_MODULE);
    CLK_EnableModuleClock(GPIOJ_MODULE);

    /* Lock protected registers */
    //SYS_LockReg();
}

void InitDebugUart(void)
{
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HIRC, CLK_UARTDIV0_UART0DIV(1));

    /*------------------------------------------------------------------------*/
    /* Init UART                                                              */
    /*------------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP3 &= ~(SYS_GPB_MFP3_PB12MFP_Msk | SYS_GPB_MFP3_PB13MFP_Msk);
    SYS->GPB_MFP3 |= (SYS_GPB_MFP3_PB12MFP_UART0_RXD | SYS_GPB_MFP3_PB13MFP_UART0_TXD);
    //SYS->GPD_MFP0 &= ~(SYS_GPD_MFP0_PD2MFP_Msk | SYS_GPD_MFP0_PD3MFP_Msk);
    //SYS->GPD_MFP0 |= (SYS_GPD_MFP0_PD2MFP_UART0_RXD | SYS_GPD_MFP0_PD3MFP_UART0_TXD);
    //SYS->GPH_MFP2 = (SYS->GPH_MFP2 & (~SYS_GPH_MFP2_PH10MFP_Msk)) | (SYS_GPH_MFP2_PH10MFP_UART0_TXD);
    //SYS->GPH_MFP2 = (SYS->GPH_MFP2 & (~SYS_GPH_MFP2_PH11MFP_Msk)) | (SYS_GPH_MFP2_PH11MFP_UART0_RXD);

    /* Reset IP */
    //SYS->UARTRST |=  SYS_UARTRST_UART0RST_Msk;
    //SYS->UARTRST &= ~SYS_UARTRST_UART0RST_Msk;

#ifdef __PLDM_EMU__
#if (USE_HXT_SRC == 1)
    DEBUG_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(153600, 9600); // The setting is for Palladium
#else
    DEBUG_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(153600, 38400); // The setting is for Palladium
#endif //USE_HXT_SRC
#else
    /* Configure UART0 and set UART0 Baudrate */
    //UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(PLL_CLOCK, 115200);
    DEBUG_PORT->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(12000000, 115200);
#endif //#ifndef __PLDM_EMU__
    DEBUG_PORT->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
}

extern void spim_sb_test(void);

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    uint32_t u32SPIMModule = 0;
    uint32_t u32SPIMDiv = 0;
    SPIM_T *pSPIMx = NULL;

#ifdef __PLDM_EMU__
    SCB_DisableICache();
    SCB_DisableDCache();

    //SCB_EnableICache();
    //SCB_EnableDCache();
#endif //__PLDM_EMU__

    SYS_Init();

    InitDebugUart();

    SET_GPIO_PB7();
    GPIO_SetMode(PB, BIT7, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PB, BIT7, GPIO_PUSEL_DISABLE);
    //PB7 = 0;
    //DelayUs(1000);
    //PB7 = 1;

    SET_GPIO_PB8();
    GPIO_SetMode(PB, BIT8, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PB, BIT8, GPIO_PUSEL_DISABLE);

    //SPIM0 PSC, PSC_n
    SET_GPIO_PG8();
    GPIO_SetMode(PG, BIT8, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PG, BIT8, GPIO_PUSEL_DISABLE);
    PG8 = 1;

    SET_GPIO_PG7();
    GPIO_SetMode(PG, BIT7, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PG, BIT7, GPIO_PUSEL_DISABLE);
    PG7 = 0;

    //SPIM1 PSC, PSC_n
    SET_GPIO_PH11();
    GPIO_SetMode(PH, BIT11, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PH, BIT11, GPIO_PUSEL_DISABLE);
    PH11 = 1;

    SET_GPIO_PH10();
    GPIO_SetMode(PH, BIT10, GPIO_MODE_OUTPUT);
    GPIO_SetPullCtl(PH, BIT11, GPIO_PUSEL_DISABLE);
    PH10 = 0;

    SPIM_DisableInt(C_SPIM0);
    SPIM_DisableInt(C_SPIM1);

    OTFC_DisableInt(C_OTFC0);
    OTFC_DisableInt(C_OTFC1);

    NVIC_DisableIRQ(TIMER0_IRQn);

    u32SPIMModule = SPIM_SelectModule(C_SPIM0);

    pSPIMx = (SPIM_T *)SPIM_GetModule(u32SPIMModule);

    //SPIM Def. Enable Cipher, First Disable the test.
    SPIM_DISABLE_CIPHER(pSPIMx);

    u32SPIMDiv = SPIM_GetClkDivNum();

    SPIM_SET_CLOCK_DIVIDER(pSPIMx, u32SPIMDiv); /* Set SPIM clock as HCLK divided by 2 */
    printf("SPIM Clock Divider = %ld\r\n", SPIM_GET_CLOCK_DIVIDER(pSPIMx));
    SPIM_SET_RXCLKDLY_RDDLYSEL(pSPIMx, 0);      /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */

    SPIMMainTestMenu(u32SPIMModule);
}
