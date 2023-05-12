/**************************************************************************//**
 * @file     ttmr.c
 * @version  V1.00
 * @brief    TTMR driver source file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup TTMR_Driver TTMR Driver
  @{
*/

/** @addtogroup TTMR_EXPORTED_FUNCTIONS TTMR Exported Functions
  @{
*/

/**
  * @brief      Open Timer with Operate Mode and Frequency
  *
  * @param[in]  ttmr        The pointer of the specified Timer module. It could be TTMR0, TTMR1, TTMR2, TTMR3.
  * @param[in]  u32Mode     Operation mode. Possible options are
  *                         - \ref TTMR_ONESHOT_MODE
  *                         - \ref TTMR_PERIODIC_MODE
  *                         - \ref TTMR_CONTINUOUS_MODE
  * @param[in]  u32Freq     Target working frequency
  *
  * @return     Real ttmr working frequency
  *
  * @details    This API is used to configure ttmr to operate in specified mode and frequency.
  *             If ttmr cannot work in target frequency, a closest frequency will be chose and returned.
  * @note       After calling this API, Timer is \b NOT running yet. But could start ttmr running be calling
  *             \ref TTMR_Start macro or program registers directly.
  */
uint32_t TTMR_Open(TTMR_T *ttmr, uint32_t u32Mode, uint32_t u32Freq)
{
    uint32_t u32Clk = TTMR_GetModuleClock(ttmr);
    uint32_t u32Cmpr = 0UL, u32Prescale = 0UL;

    /* Fastest possible ttmr working freq is (u32Clk / 2). While cmpr = 2, prescaler = 0. */
    if(u32Freq > (u32Clk / 2UL))
    {
        u32Cmpr = 2UL;
    }
    else
    {
        u32Cmpr = u32Clk / u32Freq;
        u32Prescale = (u32Cmpr >> 24);  /* for 24 bits CMPDAT */
        if (u32Prescale > 0UL)
            u32Cmpr = u32Cmpr / (u32Prescale + 1UL);
    }

    ttmr->CTL = (u32Mode | u32Prescale);
    ttmr->CMP = u32Cmpr;

    return(u32Clk / (u32Cmpr * (u32Prescale + 1UL)));
}


/**
  * @brief      Stop Timer Counting
  *
  * @param[in]  ttmr   The pointer of the specified Timer module. It could be TTMR0, TTMR1, TTMR2, TTMR3.
  *
  * @return     None
  *
  * @details    This API stops ttmr counting and disable all ttmr interrupt function.
  */
void TTMR_Close(TTMR_T *ttmr)
{
    ttmr->CTL = 0UL;
//    ttmr->EXTCTL = 0UL;
}


/**
  * @brief      Create a specify Delay Time
  *
  * @param[in]  ttmr        The pointer of the specified Timer module. It could be TTMR0, TTMR1, TTMR2, TTMR3.
  * @param[in]  u32Usec     Delay period in micro seconds. Valid values are between 100~1000000 (100 micro second ~ 1 second).
  *
  * @return     None
  *
  * @details    This API is used to create a delay loop for u32usec micro seconds by using ttmr one-shot mode.
  * @note       This API overwrites the register setting of the ttmr used to count the delay time.
  * @note       This API use polling mode. So there is no need to enable interrupt for the ttmr module used to generate delay.
  */
void TTMR_Delay(TTMR_T *ttmr, uint32_t u32Usec)
{
    uint32_t u32Clk = TTMR_GetModuleClock(ttmr);
    uint32_t u32Prescale = 0UL, delay = (SystemCoreClock / u32Clk) + 1UL;
    uint32_t u32Cmpr, u32NsecPerTick;

    /* Clear current ttmr configuration */
    ttmr->CTL = 0UL;
//    ttmr->EXTCTL = 0UL;

    if(u32Clk <= 1000000UL)   /* min delay is 1000 us if ttmr clock source is <= 1 MHz */
    {
        if(u32Usec < 1000UL)
        {
            u32Usec = 1000UL;
        }
        if(u32Usec > 1000000UL)
        {
            u32Usec = 1000000UL;
        }
    }
    else
    {
        if(u32Usec < 100UL)
        {
            u32Usec = 100UL;
        }
        if(u32Usec > 1000000UL)
        {
            u32Usec = 1000000UL;
        }
    }

    if(u32Clk <= 1000000UL)
    {
        u32Prescale = 0UL;
        u32NsecPerTick = 1000000000UL / u32Clk;
        u32Cmpr = (u32Usec * 1000UL) / u32NsecPerTick;
    }
    else
    {
        u32Cmpr = u32Usec * (u32Clk / 1000000UL);
        u32Prescale = (u32Cmpr >> 24);  /* for 24 bits CMPDAT */
        if (u32Prescale > 0UL)
            u32Cmpr = u32Cmpr / (u32Prescale + 1UL);
    }

    ttmr->CMP = u32Cmpr;
    ttmr->CTL = TTMR_CTL_CNTEN_Msk | TTMR_ONESHOT_MODE | u32Prescale;

    /* When system clock is faster than ttmr clock, it is possible ttmr active bit cannot set in time while we check it.
       And the while loop below return immediately, so put a tiny delay here allowing ttmr start counting and raise active flag. */
    for(; delay > 0UL; delay--)
    {
        __NOP();
    }

    while(ttmr->CTL & TTMR_CTL_ACTSTS_Msk)
    {
        ;
    }
}


/**
  * @brief      Enable Timer Capture Function
  *
  * @param[in]  ttmr        The pointer of the specified Timer module. It could be TTMR0, TTMR1, TTMR2, TTMR3.
  * @param[in]  u32CapMode  Timer capture mode. Could be
  *                         - \ref TTMR_CAPTURE_FREE_COUNTING_MODE
  *                         - \ref TTMR_CAPTURE_COUNTER_RESET_MODE
  * @param[in]  u32Edge     Timer capture trigger edge. Possible values are
  *                         - \ref TTMR_CAPTURE_EVENT_FALLING
  *                         - \ref TTMR_CAPTURE_EVENT_RISING
  *                         - \ref TTMR_CAPTURE_EVENT_FALLING_RISING
  *                         - \ref TTMR_CAPTURE_EVENT_RISING_FALLING
  *                         - \ref TTMR_CAPTURE_EVENT_GET_LOW_PERIOD
  *                         - \ref TTMR_CAPTURE_EVENT_GET_HIGH_PERIOD
  *
  * @return     None
  *
  * @details    This API is used to enable ttmr capture function with specify capture trigger edge \n
  *             to get current counter value or reset counter value to 0.
  * @note       Timer frequency should be configured separately by using \ref TTMR_Open API, or program registers directly.
  */
//void TTMR_EnableCapture(TTMR_T *ttmr, uint32_t u32CapMode, uint32_t u32Edge)
//{
//    ttmr->EXTCTL = (ttmr->EXTCTL & ~(TTMR_EXTCTL_CAPFUNCS_Msk | TTMR_EXTCTL_CAPEDGE_Msk)) |
//                     u32CapMode | u32Edge | TTMR_EXTCTL_CAPEN_Msk;
//}


/**
  * @brief      Disable Timer Capture Function
  *
  * @param[in]  ttmr   The pointer of the specified Timer module. It could be TTMR0, TTMR1, TTMR2, TTMR3.
  *
  * @return     None
  *
  * @details    This API is used to disable the ttmr capture function.
  */
//void TTMR_DisableCapture(TTMR_T *ttmr)
//{
//    ttmr->EXTCTL &= ~TTMR_EXTCTL_CAPEN_Msk;
//}


/**
  * @brief      Enable Timer Counter Function
  *
  * @param[in]  ttmr        The pointer of the specified Timer module. It could be TTMR0, TTMR1, TTMR2, TTMR3.
  * @param[in]  u32Edge     Detection edge of counter pin. Could be ether
  *                         - \ref TTMR_COUNTER_EVENT_FALLING, or
  *                         - \ref TTMR_COUNTER_EVENT_RISING
  *
  * @return     None
  *
  * @details    This function is used to enable the ttmr counter function with specify detection edge.
  * @note       Timer compare value should be configured separately by using \ref TTMR_SET_CMP_VALUE macro or program registers directly.
  * @note       While using event counter function, \ref TTMR_TOGGLE_MODE cannot set as ttmr operation mode.
  */
//void TTMR_EnableEventCounter(TTMR_T *ttmr, uint32_t u32Edge)
//{
//    ttmr->EXTCTL = (ttmr->EXTCTL & ~TTMR_EXTCTL_CNTPHASE_Msk) | u32Edge;
//    ttmr->CTL |= TTMR_CTL_EXTCNTEN_Msk;
//}


/**
  * @brief      Disable Timer Counter Function
  *
  * @param[in]  ttmr   The pointer of the specified Timer module. It could be TTMR0, TTMR1, TTMR2, TTMR3.
  *
  * @return     None
  *
  * @details    This API is used to disable the ttmr event counter function.
  */
//void TTMR_DisableEventCounter(TTMR_T *ttmr)
//{
//    ttmr->CTL &= ~TTMR_CTL_EXTCNTEN_Msk;
//}


/**
  * @brief      Get Timer Clock Frequency
  *
  * @param[in]  ttmr   The pointer of the specified Timer module. It could be TTMR0, TTMR1, TTMR2, TTMR3.
  *
  * @return     Timer clock frequency
  *
  * @details    This API is used to get the ttmr clock frequency.
  * @note       This API cannot return correct clock rate if ttmr source is from external clock input.
  */
uint32_t TTMR_GetModuleClock(TTMR_T *ttmr)
{
    uint32_t u32Src = 0UL, u32Clk;
    const uint32_t au32Clk[] = {0UL, __LXT, __LIRC, __MIRC, __HIRC};

    if(ttmr == TTMR0)
    {
        u32Src = (CLK->TTMRSEL & CLK_TTMRSEL_TTMR0SEL_Msk) >> CLK_TTMRSEL_TTMR0SEL_Pos;
    }
    else if(ttmr == TTMR1)
    {
        u32Src = (CLK->TTMRSEL & CLK_TTMRSEL_TTMR1SEL_Msk) >> CLK_TTMRSEL_TTMR1SEL_Pos;
    }

    if(u32Src == 0UL)
    {
        u32Clk = CLK_GetPCLK2Freq();
    }
    else
    {
        u32Clk = au32Clk[u32Src];
    }

    return u32Clk;
}


/**
  * @brief  This function is used to enable the Timer frequency counter function
  * @param[in] ttmr         The base address of Timer module. Can be \ref TTMR0 or \ref TTMR2
  * @param[in] u32DropCount This parameter has no effect in M55M1 Series BSP
  * @param[in] u32Timeout   This parameter has no effect in M55M1 Series BSP
  * @param[in] u32EnableInt Enable interrupt assertion after capture complete or not. Valid values are TRUE and FALSE
  * @return None
  * @details This function is used to calculate input event frequency. After enable
  *          this function, a pair of ttmrs, TTMR0 and TTMR1, or TTMR2 and TTMR3
  *          will be configured for this function. The mode used to calculate input
  *          event frequency is mentioned as "Inter Timer Trigger Mode" in Technical
  *          Reference Manual
  */
//void TTMR_EnableFreqCounter(TTMR_T *ttmr,
//                              uint32_t u32DropCount,
//                              uint32_t u32Timeout,
//                              uint32_t u32EnableInt)
//{
//    TTMR_T *t;    /* store the ttmr base to configure compare value */

//    t = TTMR1;

//    t->CMP = 0xFFFFFFUL;
//    t->EXTCTL = u32EnableInt ? TTMR_EXTCTL_CAPIEN_Msk : 0UL;
//    ttmr->CTL = TTMR_CTL_INTRGEN_Msk | TTMR_CTL_CNTEN_Msk;

//    return;
//}


/**
  * @brief This function is used to disable the Timer frequency counter function.
  * @param[in] ttmr The base address of Timer module
  * @return None
  */
//void TTMR_DisableFreqCounter(TTMR_T *ttmr)
//{
//    ttmr->CTL &= ~TTMR_CTL_INTRGEN_Msk;
//}

/**
  * @brief This function is used to select the interrupt source used to trigger other modules.
  * @param[in] ttmr   The base address of Timer module
  * @param[in] u32Src Selects the interrupt source to trigger other modules. Could be:
  *              - \ref TTMR_TRGSRC_TIMEOUT_EVENT
  *              - \ref TTMR_TRGSRC_CAPTURE_EVENT
  * @return None
  */
//void TTMR_SetTriggerSource(TTMR_T *ttmr, uint32_t u32Src)
//{
//    ttmr->TRGCTL = (ttmr->TRGCTL & ~TTMR_TRGCTL_TRGSSEL_Msk) | u32Src;
//}

/**
  * @brief This function is used to set modules trigger by ttmr interrupt
  * @param[in] ttmr    The base address of Timer module
  * @param[in] u32Mask The mask of modules (LPADC, LPI2C, LPSPI, LPUART, CCAP and LPPDMA) trigger by ttmr. Is the combination of
  *             - \ref TTMR_TRGEN
  *             - \ref TTMR_TRG_TO_LPPDMA
  * @return None
  */
void TTMR_SetTriggerTarget(TTMR_T *ttmr, uint32_t u32Mask)
{
    ttmr->TRGCTL = (ttmr->TRGCTL & ~( TTMR_TRGCTL_TRGEN_Msk | TTMR_TRGCTL_TRGLPPDMA_Msk)) | u32Mask;
}

/**
  * @brief      Reset Counter
  *
  * @param[in]  ttmr       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  *
  * @retval     TTMR_OK          Timer reset success
  * @retval     TTMR_ERR_TIMEOUT Timer reset failed
  *
  * @details    This function is used to reset current counter value and internal prescale counter value.
  */
int32_t TTMR_ResetCounter(TTMR_T *ttmr)
{
    uint32_t u32Delay;

    ttmr->CNT = 0UL;
    /* Takes 2~3 ECLKs to reset timer counter */
    u32Delay = (SystemCoreClock / TTMR_GetModuleClock(ttmr)) * 3;
    while(((ttmr->CNT&TTMR_CNT_RSTACT_Msk) == TTMR_CNT_RSTACT_Msk) && (--u32Delay))
    {
        __NOP();
    }
    return u32Delay > 0 ? TTMR_OK : TTMR_ERR_TIMEOUT;
}

/** @} end of group TTMR_EXPORTED_FUNCTIONS */
/** @} end of group TTMR_Driver */
/** @} end of group Standard_Driver */
