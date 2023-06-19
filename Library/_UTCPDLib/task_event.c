/******************************************************************************
 * @file     task_event.c
 * @version  V1.00
 * $Revision: 13 $
 * $Date: 18/07/18 3:19p $
 * @brief    Simply the task function from ec
 *
 * @note
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#if 0
    #include <stdio.h>
    #include "NuMicro.h"
    #include "atomic.h"
    #include "usb_common.h"
    #include "timer_ec.h"
#else
    #include "utcpdlib.h"
#endif


#define USBC_EVENT_TIMEOUT (5 * MSEC)
#define USBC_MIN_EVENT_TIMEOUT (1 * MSEC)

#if 1
//#define TASK_ID_TO_PD_PORT(id) ((id) - TASK_ID_PD_C0)
//#define PD_PORT_TO_TASK_ID(port) (TASK_ID_PD_C0 + (port))

static volatile uint32_t port_events[CONFIG_USB_PD_PORT_MAX_COUNT];

//uint32_t task_set_event(task_id_t tskid, uint32_t event)
uint32_t task_set_event(task_id_t tskid, uint32_t event_mask)
{
    //printf("\nTask Set Event Mask 0x%x\n", event_mask);
    atomic_or(&port_events[TASK_ID_TO_PD_PORT(tskid)], event_mask);
}

//SW Add {
uint32_t task_clear_event(task_id_t tskid, uint32_t event_mask)
{
    atomic_clear_bits(&port_events[TASK_ID_TO_PD_PORT(tskid)], event_mask);
}
//SW Add }

uint32_t task_wait_event(int port)
{
    //Seems not used in porting code. due to we didn't enable CONFIG_USB_PD_TCPC_LOW_POWER
    //printf("%s\n",__FUNCTION__);
    return port_events[port];
}

task_id_t task_get_current(void)
{
    //printf("%s\n",__FUNCTION__);

}

//void task_wake(task_id_t tskid)
void task_wake(uint8_t tskid)
{
    //printf("%s\n",__FUNCTION__);
}
#endif