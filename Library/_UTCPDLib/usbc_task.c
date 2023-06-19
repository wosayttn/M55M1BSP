/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include "NuMicro.h"
#if 0
    //#include "battery.h"
    //#include "battery_smart.h"
    //#include "board.h"
    //#include "charge_manager.h"
    //#include "charge_state.h"
    //#include "chipset.h"
    #include "common.h"
    //#include "console.h"
    //#include "cros_version.h"
    #include "ec_commands.h"
    //#include "gpio.h"
    //#include "hooks.h"
    //#include "host_command.h"
    //#include "registers.h"
    //#include "system.h"
    //#include "task.h"
    #include "timer_ec.h"
    //#include "util.h"
    //#include "usb_charge.h"
    //#include "usb_mux.h"
    #include "usb_pd.h"
    #include "usb_pd_timer.h"
    #include "usb_prl_sm.h"
    //#include "tcpm/tcpm.h"
    #include "tcpm.h"
    #include "usb_pe_sm.h"
    #include "usb_prl_sm.h"
    #include "usb_sm.h"
    #include "usb_tc_sm.h"
    //#include "usbc_ppc.h"

    #include "tcpci.h"
#else
    #include "utcpdlib.h"
#endif

#define IS_ENABLED(x)   (x == 1)

#define USBC_EVENT_TIMEOUT (5 * MSEC)
#define USBC_MIN_EVENT_TIMEOUT (1 * MSEC)

#define CPRINTF(format, args...) cprintf(CC_USBPD, format, ## args)
#define CPRINTS(format, args...) cprints(CC_USBPD, format, ## args)

/*
 * If CONFIG_ASSERT_CCD_MODE_ON_DTS_CONNECT is not defined then
 * _GPIO_CCD_MODE_ODL is not needed. Declare as extern so IS_ENABLED will work.
 */
#ifndef CONFIG_ASSERT_CCD_MODE_ON_DTS_CONNECT
    extern int _GPIO_CCD_MODE_ODL;
#else
    #define _GPIO_CCD_MODE_ODL GPIO_CCD_MODE_ODL
#endif /* CONFIG_ASSERT_CCD_MODE_ON_DTS_CONNECT */

static uint8_t paused[CONFIG_USB_PD_PORT_MAX_COUNT];

void tc_pause_event_loop(int port)
{
    paused[port] = 1;
}

bool tc_event_loop_is_paused(int port)
{
    return paused[port];
}

void tc_start_event_loop(int port)
{
    /*
     * Only generate TASK_EVENT_WAKE event if state
     * machine is transitioning to un-paused
     */
    if (paused[port])
    {
        paused[port] = 0;
        task_set_event(PD_PORT_TO_TASK_ID(port), TASK_EVENT_WAKE);
    }
}

static void pd_task_init(int port)
{
    if (IS_ENABLED(CONFIG_USB_TYPEC_SM))
        tc_state_init(port);

    paused[port] = 0;

#ifdef SW

    /*
     * Since most boards configure the TCPC interrupt as edge
     * and it is possible that the interrupt line was asserted between init
     * and calling set_state, we need to process any pending interrupts now.
     * Otherwise future interrupts will never fire because another edge
     * never happens. Note this needs to happen after set_state() is called.
     */
    if (IS_ENABLED(CONFIG_HAS_TASK_PD_INT))
        schedule_deferred_pd_interrupt(port);

    /*
     * GPIO_CCD_MODE_ODL must be initialized with GPIO_ODR_HIGH
     * when CONFIG_ASSERT_CCD_MODE_ON_DTS_CONNECT is enabled
     */
    if (IS_ENABLED(CONFIG_ASSERT_CCD_MODE_ON_DTS_CONNECT))
        ASSERT(gpio_get_default_flags(_GPIO_CCD_MODE_ODL) &
               GPIO_ODR_HIGH);

#endif
}

static int pd_task_timeout(int port)
{
    int timeout;

    if (paused[port])
        timeout = -1;
    else
    {
        timeout = pd_timer_next_expiration(port);

        if (timeout < 0 || timeout > USBC_EVENT_TIMEOUT)
            timeout = USBC_EVENT_TIMEOUT;
        else if (timeout < USBC_MIN_EVENT_TIMEOUT)
            timeout = USBC_MIN_EVENT_TIMEOUT;
    }

    return timeout;
}

#define TASK_EVENT_TIMER       (1U << 31)       //task.h 

#if 0
    static bool pd_task_loop(int port)
#else
    bool pd_task_loop(int port)
#endif
{
#ifdef SW_UNDER_CHECK
    /* wait for next event/packet or timeout expiration */
    const uint32_t evt = task_wait_event(pd_task_timeout(port));

    /* Manage expired PD Timers on timeouts */
    if (evt & TASK_EVENT_TIMER)
        pd_timer_manage_expired(port);

#else
    int timeout;
#ifdef SW
    uint32_t evt;
#else
    static uint32_t evt = 0;
#endif
    timeout = pd_task_timeout(port);

    switch (timeout)
    {
        case USBC_MIN_EVENT_TIMEOUT:
            evt |= TASK_EVENT_TIMER;
    }

    //evt |= task_wait_event(port);

    /* Manage expired PD Timers on timeouts */
    if (evt & TASK_EVENT_TIMER)
    {
        pd_timer_manage_expired(port);
    }

    evt = task_wait_event(port);
#endif

#ifdef SW

    /*
     * Re-use TASK_EVENT_RESET_DONE in tests to restart the USB task
     * if this code is running in a unit test.
     */
    if (IS_ENABLED(TEST_BUILD) && (evt & TASK_EVENT_RESET_DONE))
        return false;

#endif

    /* handle events that affect the state machine as a whole */
    if (IS_ENABLED(CONFIG_USB_TYPEC_SM))
        tc_event_check(port, evt);

#ifdef SW

    /*
     * run port controller task to check CC and/or read incoming
     * messages
     */
    if (IS_ENABLED(CONFIG_USB_PD_TCPC))
        tcpc_run(port, evt);

#endif

    /* Run policy engine state machine */
    if (IS_ENABLED(CONFIG_USB_PE_SM))
        pe_run(port, evt, tc_get_pd_enabled(port));

    /* Run protocol state machine */
    if (IS_ENABLED(CONFIG_USB_PRL_SM) || IS_ENABLED(CONFIG_TEST_USB_PE_SM))
        prl_run(port, evt, tc_get_pd_enabled(port));

    /* Run TypeC state machine */
    if (IS_ENABLED(CONFIG_USB_TYPEC_SM))
        tc_run(port);

    return true;
}


#if 1
void pd_task_reinit(int port)
{
    pd_timer_init(port);
    pd_task_init(port);
}
#else
//uint64_t bTime, etime;
void pd_task(void *u)
{
    int port = TASK_ID_TO_PD_PORT(task_get_current());

    /*
     * If port does not exist, return
     */
    if (port >= board_get_usb_pd_port_count())
        return;

    //  bTime = __hw_clock_source_read64();
    while (1)
    {
        pd_timer_init(port);
        pd_task_init(port);

        /* As long as pd_task_loop returns true, keep running the loop.
         * pd_task_loop returns false when the code needs to re-init
         * the task, so once the code breaks out of the inner while
         * loop, the re-init code at the top of the outer while loop
         * will run.
         */
        while (pd_task_loop(port))
        {
#if (CONFIG_COMMAND_SHELL == 1)
            UART_Commandshell(port);
#if 0

            //SW ADD{
            if (pd_get_src_cap_cnt(port) != 0)
            {
                uint32_t *u32Ptr;
                uint8_t cnt, i;
                u32Ptr = pd_get_src_caps(port);
                cnt = pd_get_src_cap_cnt(port);

                for (i = 0; i < cnt; i = i + 1)
                {
                    DBG_PRINTF("SRC PDO %d, 0x%x\n", i, *u32Ptr++);
                }

                pd_set_src_caps(port, 0, NULL);
            }

            if (pd_get_snk_cap_cnt(port) != 0)
            {
                uint32_t *u32Ptr;
                uint8_t cnt, i;
                u32Ptr = pd_get_snk_caps(port);
                cnt = pd_get_snk_cap_cnt(port);

                for (i = 0; i < cnt; i = i + 1)
                {
                    DBG_PRINTF("SNK PDO %d, 0x%x\n", i, *u32Ptr++);
                }

                pe_set_snk_caps(port, 0, NULL);
            }

#endif
#else
            //          etime = __hw_clock_source_read64();
            //          if(etime > (bTime+15000000))
            //          {
            //              bTime = etime;
            //              printf("Issue PR_SWAP\n");
            //              pd_dpm_request(port, DPM_REQUEST_PR_SWAP);
            //          }
#endif
            continue;
        }
    }
}
#endif