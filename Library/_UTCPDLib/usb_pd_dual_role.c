/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Dual Role (Source & Sink) USB-PD module.
 */
#if 0
    //#include "charge_manager.h"
    //#include "charge_state.h"
    //#include "dps.h"
    //#include "system.h"
    #include "usb_common.h"
    #include "usb_pd.h"
    //#include "util.h"
#else
    #include "utcpdlib.h"
#endif

#if (DBG == 1)
    #define DBG_PRINTF printf
#else
    #define DBG_PRINTF(...)
#endif

/* SW Add { */
#include "atomic.h"
#define IS_ENABLED(x)  (x == 1)

#define CPRINTS(format, args...) cprints(CC_USBPD, format, ## args)

/* The macro is used to prevent a DBZ exception while decoding PDOs. */
#define PROCESS_ZERO_DIVISOR(x) ((x) == 0 ? 1 : (x))

#if defined(PD_MAX_VOLTAGE_MV) && defined(PD_OPERATING_POWER_MW)
/*
 * As a sink, this is the max voltage (in millivolts) we can request
 * before getting source caps
 */
#if 0
static unsigned int max_request_mv[CONFIG_USB_PD_PORT_MAX_COUNT] = {PD_MAX_VOLTAGE_MV};
#else
static unsigned int max_request_mv[CONFIG_USB_PD_PORT_MAX_COUNT] = {5000};
#endif

#ifdef SW
    /* TODO(b:169532537): deprecate CONFIG_USB_PD_PREFER_MV */
    STATIC_IF_NOT(CONFIG_USB_PD_PREFER_MV)
    struct pd_pref_config_t __maybe_unused pd_pref_config;
#else
    struct pd_pref_config_t pd_pref_config;
#endif

void pd_set_max_voltage(int port, unsigned int mv)
{
    max_request_mv[port] = mv;
}

unsigned int pd_get_max_voltage(int port)
{
    return max_request_mv[port];
}

/* SW Create { */
extern int pd_snk_pdo_cnt;
extern const uint32_t pd_snk_pdo[];
/* Only handle FIXED PDO */
unsigned int pd_set_snk_default_requested_voltage(int port)
{
    //  You can find max SNK PDO to be default requested voltage
    //  uint32_t i;
    //  for(i=0; i< pd_snk_pdo_cnt; i=i+1)
    //  {
    //      pd_snk_pdo[i]
    //  }

    //  Now we set default 5V to be default requested voltage
    pd_set_max_voltage(port, 5000);
}
/* SW Create } */

#if 0
/*
 * Zinger implements a board specific usb policy that does not define
 * PD_MAX_VOLTAGE_MV and PD_OPERATING_POWER_MW. And in turn, does not
 * use the following functions.
 */
int pd_find_pdo_index(uint32_t src_cap_cnt, const uint32_t *const src_caps,
                      int max_mv, uint32_t *selected_pdo)
{
    int i, uw, mv;
    int ret = 0;
    int cur_uw = 0;
    int has_preferred_pdo;
    int prefer_cur;
    int desired_uw = 0;
#ifdef SW
    const int prefer_mv = pd_pref_config.mv;
    const int type = pd_pref_config.type;
#else
    int prefer_mv = pd_pref_config.mv;
    int type = pd_pref_config.type;
#endif

    int __attribute__((unused)) cur_mv = 0;

    if (IS_ENABLED(CONFIG_USB_PD_PREFER_MV))
        desired_uw = charge_get_plt_plus_bat_desired_mw() * 1000;

    /* max voltage is always limited by this boards max request */
    max_mv = MIN(max_mv, PD_MAX_VOLTAGE_MV);          //PD_MAX_VOLTAGE_MV = 20000

    /* Get max power that is under our max voltage input */
    for (i = 0; i < src_cap_cnt; i++)
    {
        if (IS_ENABLED(CONFIG_USB_PD_ONLY_FIXED_PDOS) &&
                (src_caps[i] & PDO_TYPE_MASK) != PDO_TYPE_FIXED)
            continue;

        /* its an unsupported Augmented PDO (PD3.0) */
        if ((src_caps[i] & PDO_TYPE_MASK) == PDO_TYPE_AUGMENTED)
            continue;

        mv = ((src_caps[i] >> 10) & 0x3FF) * 50;

        /* Skip invalid voltage */
        if (!mv)
            continue;

        /* Skip any voltage not supported by this board */
        if (!pd_is_valid_input_voltage(mv))
            continue;

        if ((src_caps[i] & PDO_TYPE_MASK) == PDO_TYPE_BATTERY)
        {
            uw = 250000 * (src_caps[i] & 0x3FF);
        }
        else
        {
            int ma = (src_caps[i] & 0x3FF) * 10;

            ma = MIN(ma, PD_MAX_CURRENT_MA);      /* PD_MAX_CURRENT_MA = 3000 */
            uw = ma * mv;
        }

        if (mv > max_mv)
            continue;

        uw = MIN(uw, PD_MAX_POWER_MW * 1000);
        prefer_cur = 0;

        /* Apply special rules in favor of voltage  */
        if (IS_ENABLED(PD_PREFER_LOW_VOLTAGE))
        {
            if (uw == cur_uw && mv < cur_mv)
                prefer_cur = 1;
        }
        else if (IS_ENABLED(PD_PREFER_HIGH_VOLTAGE))
        {
            if (uw == cur_uw && mv > cur_mv)
                prefer_cur = 1;
        }
        else if (IS_ENABLED(CONFIG_USB_PD_PREFER_MV))
        {
            /* Pick if the PDO provides more than desired. */
            if (uw >= desired_uw)
            {
                /* pick if cur_uw is less than desired watt */
                if (cur_uw < desired_uw)
                    prefer_cur = 1;
                else if (type == PD_PREFER_BUCK)
                {
                    /*
                     * pick the smallest mV above prefer_mv
                     */
                    if (mv >= prefer_mv && mv < cur_mv)
                        prefer_cur = 1;
                    /*
                     * pick if cur_mv is less than
                     * prefer_mv, and we have higher mV
                     */
                    else if (cur_mv < prefer_mv &&
                             mv > cur_mv)
                        prefer_cur = 1;
                }
                else if (type == PD_PREFER_BOOST)
                {
                    /*
                     * pick the largest mV below prefer_mv
                     */
                    if (mv <= prefer_mv && mv > cur_mv)
                        prefer_cur = 1;
                    /*
                     * pick if cur_mv is larger than
                     * prefer_mv, and we have lower mV
                     */
                    else if (cur_mv > prefer_mv &&
                             mv < cur_mv)
                        prefer_cur = 1;
                }

                /*
                 * pick the largest power if we don't see one staisfy
                 * desired power
                 */
            }
            else if (cur_uw == 0 || uw > cur_uw)
            {
                prefer_cur = 1;
            }
        }

        /* Prefer higher power, except for tiebreaker */
        has_preferred_pdo =
            prefer_cur ||
            (!IS_ENABLED(CONFIG_USB_PD_PREFER_MV) && uw > cur_uw);

        if (has_preferred_pdo)
        {
            ret = i;
            cur_uw = uw;
            cur_mv = mv;
        }
    }

    if (selected_pdo)
        *selected_pdo = src_caps[ret];

    printf("SNK Re PDO = %d\n", ret);
    return ret;
}
#endif
/*
  SNK side evaluate the SRC PDO.

  src_cap_cnt: Source Capabilities Count
  src_caps: Source Capabilities PDOs
  max_mv: specified voltage to request

#define PD_MAX_VOLTAGE_MV                        20000       //20000
#define PD_OPERATING_POWER_MW                    100000      //65000

*/
extern const uint32_t pd_snk_pdo[];
//extern const int pd_snk_pdo_cnt;
extern int pd_snk_pdo_cnt;
int pd_find_pdo_index(uint32_t src_cap_cnt, const uint32_t *const pu32src_caps,
                      int max_mv, uint32_t *pu32selected_pdo)
{
    int ret = -1;
    int i, j, uw, mv;
    int snk_mv, snk_ma;

    /* Search Fixed PDO first */
    for (j = (pd_snk_pdo_cnt - 1); j >= 0; j = j - 1) /* From Max SNK Voltage */
    {
        for (i = 0; i < src_cap_cnt; i++)
        {
            //printf("SRCPDO %d = 0x%x\n", i, src_caps[i]);
            if (IS_ENABLED(CONFIG_USB_PD_ONLY_FIXED_PDOS) &&
                    (pu32src_caps[i] & PDO_TYPE_MASK) != PDO_TYPE_FIXED)
                continue;

            /* its an unsupported Augmented PDO (PD3.0) */
            if ((pu32src_caps[i] & PDO_TYPE_MASK) == PDO_TYPE_AUGMENTED)
                continue;

            /* SRC Fixed PDO Voltage */
            mv = ((pu32src_caps[i] >> 10) & 0x3FF) * 50;

            //printf("mv = %d\n", mv);
            /* Skip invalid voltage */
            if (!mv)
                continue;

            /* Skip any voltage not supported by this board */
            //if (!pd_is_valid_input_voltage(mv))
            //continue;

            if ((pu32src_caps[i] & PDO_TYPE_MASK) == PDO_TYPE_BATTERY)
            {
                uw = 250000 * (pu32src_caps[i] & 0x3FF);
            }
            else
            {
                int ma = (pu32src_caps[i] & 0x3FF) * 10;    /* src ma */
#if 0
                snk_mv = max_mv;
#else
                snk_mv = PDO_FIXED_VOLTAGE(pd_snk_pdo[j]);
#endif

                /* if voltage is same, check if the src current >= sink current? */
                if (mv == snk_mv)
                {
                    if (ma >= ((pu32src_caps[i] & 0x3FF) * 10))
                    {
                        //            printf("req PDO idx = %d\n", i);
                        ret = i;
                        break;
                    }
                }
            }
        }

        if (ret != -1)
            break;
    }

    if (ret != -1)
        *pu32selected_pdo = pu32src_caps[ret];
    else
    {
        //Search PPS PDO if FIX PDO not match
        for (i = 0; i < src_cap_cnt; i++)
        {
            /* its an unsupported Augmented PDO (PD3.0) */
            if ((pu32src_caps[i] & PDO_TYPE_MASK) != PDO_TYPE_AUGMENTED)
                continue;
            else
            {
                ret = i;
                break;
            }
        }

        if (ret != -1)
        {
            //Find PPS PDO
            uint32_t max_mv, min_mv, ma;
            *pu32selected_pdo = pu32src_caps[ret];
            pd_extract_pdo_power(*pu32selected_pdo, &ma, &max_mv, &min_mv);
            printf("max_mv = %d\n", max_mv);
            printf("min_mv = %d\n", min_mv);
            printf("ma = %d\n", ma);
        }
    }

    if (ret == -1)
    {
        printf("Unmatched\n");
        *pu32selected_pdo = pu32src_caps[0];
        ret = 0;      //default 5V
    }

    printf("RPDO = %d\n", ret);
    //  printf("SRC pdo = 0x%x\n", src_caps[ret]);
    return ret;
}
void pd_extract_pdo_power(uint32_t pdo, uint32_t *ma, uint32_t *max_mv,
                          uint32_t *min_mv)
{
    int max_ma, mw;

    if ((pdo & PDO_TYPE_MASK) == PDO_TYPE_FIXED)
    {
        *max_mv = PDO_FIXED_VOLTAGE(pdo);
        *min_mv = *max_mv;
    }
    else if ((pdo & PDO_TYPE_MASK) == PDO_TYPE_AUGMENTED)
    {
        *max_mv = PDO_AUG_MAX_VOLTAGE(pdo);
        *min_mv = PDO_AUG_MIN_VOLTAGE(pdo);
    }
    else if ((pdo & PDO_TYPE_MASK) == PDO_TYPE_VARIABLE)
    {
        *max_mv = PDO_VAR_MAX_VOLTAGE(pdo);
        *min_mv = PDO_VAR_MIN_VOLTAGE(pdo);
    }
    else
    {
        *max_mv = PDO_BATT_MAX_VOLTAGE(pdo);
        *min_mv = PDO_BATT_MIN_VOLTAGE(pdo);
    }

    if (*max_mv == 0)
    {
        *ma = 0;
        *min_mv = 0;
        return;
    }

    if ((pdo & PDO_TYPE_MASK) == PDO_TYPE_FIXED)
    {
        max_ma = PDO_FIXED_CURRENT(pdo);
    }
    else if ((pdo & PDO_TYPE_MASK) == PDO_TYPE_AUGMENTED)
    {
        max_ma = PDO_AUG_MAX_CURRENT(pdo);
    }
    else if ((pdo & PDO_TYPE_MASK) == PDO_TYPE_VARIABLE)
    {
        max_ma = PDO_VAR_MAX_CURRENT(pdo);
    }
    else
    {
        mw = PDO_BATT_MAX_POWER(pdo);
        max_ma = 1000 * mw / PROCESS_ZERO_DIVISOR(*min_mv);
    }

    max_ma = MIN(max_ma,
                 PD_MAX_POWER_MW * 1000 / PROCESS_ZERO_DIVISOR(*min_mv));
    *ma = MIN(max_ma, PD_MAX_CURRENT_MA);
}

void pd_build_request(int32_t vpd_vdo, uint32_t *rdo, uint32_t *ma,
                      uint32_t *mv, int port)
{
    uint32_t pdo;
    int pdo_index, flags = 0;
    int uw;
    int max_or_min_ma;
    int max_or_min_mw;
    int max_vbus;
    int vpd_vbus_dcr;
    int vpd_gnd_dcr;
    uint32_t src_cap_cnt = pd_get_src_cap_cnt(port);
    const uint32_t *const src_caps = pd_get_src_caps(port);
    int charging_allowed;
    int max_request_allowed;
    uint32_t max_request_mv = pd_get_max_voltage(port);
    uint32_t unused;
    printf("%s req max = %d\n", __FUNCTION__, max_request_mv);
    printf("mv 1 = %d\n", *mv);
#ifdef SW

    /*
     * If this port is the current charge port, or if there isn't an active
     * charge port, set this value to true. If CHARGE_PORT_NONE isn't
     * considered, then there can be a race condition in PD negotiation and
     * the charge manager which forces an incorrect request for
     * vSafe5V. This can then lead to a brownout condition when the input
     * current limit gets incorrectly set to 0.5A.
     */
    if (IS_ENABLED(CONFIG_CHARGE_MANAGER))
    {
        int chg_port = charge_manager_get_selected_charge_port();

        charging_allowed =
            (chg_port == port || chg_port == CHARGE_PORT_NONE);
    }
    else
    {
        charging_allowed = 1;
    }

#else
    charging_allowed = 1;    /* We didn't support CONFIG_CHARGE_MANAGER */
#endif

    if (IS_ENABLED(CONFIG_USB_PD_CHECK_MAX_REQUEST_ALLOWED))
        max_request_allowed = pd_is_max_request_allowed();
    else
        max_request_allowed = 1;

#ifdef SW

    if (IS_ENABLED(CONFIG_USB_PD_DPS) && dps_is_enabled())
        max_request_mv =
            MIN(max_request_mv, dps_get_dynamic_voltage());

#else

    if (IS_ENABLED(CONFIG_USB_PD_DPS))
    {
        if (dps_is_enabled())
        {
            max_request_mv =
                MIN(max_request_mv, dps_get_dynamic_voltage());
        }
    }

#endif

    /*
     * If currently charging on a different port, or we are not allowed to
     * request the max voltage, then select vSafe5V
     */
    if (charging_allowed && max_request_allowed)
    {
        /* find pdo index for max voltage we can request */
        pdo_index = pd_find_pdo_index(src_cap_cnt, src_caps,
                                      max_request_mv, &pdo);

        printf("pdo_index_0 = %d\n", pdo_index);

        //pdo = src_caps[pdo_index-1];

    }
    else
    {
        /* src cap 0 should be vSafe5V */
        pdo_index = 0;
        pdo = src_caps[0];
    }

    printf("PDO_INDEX = %d\n", pdo_index);
    printf("PDO = 0x%x\n", pdo);
    pd_extract_pdo_power(pdo, ma, mv, &unused);
    printf("req V = %d\n", *mv);
    printf("req A = %d\n", *ma);


    /*
     * Adjust VBUS current if CTVPD device was detected.
     */
    if (vpd_vdo > 0)
    {
        printf("vpd_vdo>0\n");
        max_vbus = VPD_VDO_MAX_VBUS(vpd_vdo);
        vpd_vbus_dcr = VPD_VDO_VBUS_IMP(vpd_vdo) << 1;
        vpd_gnd_dcr = VPD_VDO_GND_IMP(vpd_vdo);

        /*
         * Valid max_vbus values:
         *   00b - 20000 mV
         *   01b - 30000 mV
         *   10b - 40000 mV
         *   11b - 50000 mV
         */
        max_vbus = 20000 + max_vbus * 10000;

        if (*mv > max_vbus)
            *mv = max_vbus;

        /*
         * 5000 mA cable: 150 = 750000 / 50000
         * 3000 mA cable: 250 = 750000 / 30000
         */
        if (*ma > 3000)
            *ma = 750000 / (150 + vpd_vbus_dcr + vpd_gnd_dcr);
        else
            *ma = 750000 / (250 + vpd_vbus_dcr + vpd_gnd_dcr);
    }

#if 0
    uw = *ma * *mv;

    /* Mismatch bit set if less power offered than the operating power */
    if (uw < (1000 * PD_OPERATING_POWER_MW))
        flags |= RDO_CAP_MISMATCH;

#endif

#ifdef CONFIG_USB_PD_GIVE_BACK
    /* Tell source we are give back capable. */
    flags |= RDO_GIVE_BACK;

    /*
     * BATTERY PDO: Inform the source that the sink will reduce
     * power to this minimum level on receipt of a GotoMin Request.
     */
    max_or_min_mw = PD_MIN_POWER_MW;

    /*
     * FIXED or VARIABLE PDO: Inform the source that the sink will
     * reduce current to this minimum level on receipt of a GotoMin
     * Request.
     */
    max_or_min_ma = PD_MIN_CURRENT_MA;
#else
    /*
     * Can't give back, so set maximum current and power to
     * operating level.
     */
    max_or_min_ma = *ma;
    max_or_min_mw = uw / 1000;
#endif

    if ((pdo & PDO_TYPE_MASK) == PDO_TYPE_BATTERY)
    {
        int mw = uw / 1000;
        *rdo = RDO_BATT(pdo_index + 1, mw, max_or_min_mw, flags);
    }
    else if ((pdo & PDO_TYPE_MASK) == PDO_TYPE_AUGMENTED)
    {
        printf("mv 2 = %d", *mv);
        *rdo = RDO_PPS(pdo_index + 1, *ma, *mv, flags);
    }
    else
    {
        //Fixed
        *rdo = RDO_FIXED(pdo_index + 1, *ma, max_or_min_ma, flags);
    }


    /*
     * Ref: USB Power Delivery Specification
     * (Revision 3.0, Version 2.0 / Revision 2.0, Version 1.3)
     * 6.4.2.4 USB Communications Capable
     * 6.4.2.5 No USB Suspend
     *
     * If the port partner is capable of USB communication set the
     * USB Communications Capable flag.
     * If the port partner is sink device do not suspend USB as the
     * power can be used for charging.
     */
    if (pd_get_partner_usb_comm_capable(port))
    {
        *rdo |= RDO_COMM_CAP;

        if (pd_get_power_role(port) == PD_ROLE_SINK)
            *rdo |= RDO_NO_SUSPEND;
    }

    printf("RDO = 0x%x\n", *rdo);    /* SNK Request PDO */
}

void pd_process_source_cap(int port, int cnt, uint32_t *src_caps)
{
    pd_set_src_caps(port, cnt, src_caps);
#ifdef SW

    if (IS_ENABLED(CONFIG_CHARGE_MANAGER))
    {
        uint32_t ma, mv, pdo, unused;
        uint32_t max_mv = pd_get_max_voltage();

        if (IS_ENABLED(CONFIG_USB_PD_DPS) && dps_is_enabled())
            max_mv = MIN(max_mv, dps_get_dynamic_voltage());

        /* Get max power info that we could request */
        pd_find_pdo_index(pd_get_src_cap_cnt(port),
                          pd_get_src_caps(port),
                          max_mv, &pdo);
        pd_extract_pdo_power(pdo, &ma, &mv, &unused);

        /* Set max. limit, but apply 500mA ceiling */
        charge_manager_set_ceil(port, CEIL_REQUESTOR_PD, PD_MIN_MA);
        pd_set_input_current_limit(port, ma, mv);
    }

#else
    {
        uint32_t ma, mv, pdo, unused;
        uint32_t max_mv = pd_get_max_voltage(port);

        if (IS_ENABLED(CONFIG_USB_PD_DPS) && dps_is_enabled())
            max_mv = MIN(max_mv, dps_get_dynamic_voltage());

        /* Get max power info that we could request */
        pd_find_pdo_index(pd_get_src_cap_cnt(port),
                          pd_get_src_caps(port),
                          max_mv, &pdo);
        pd_extract_pdo_power(pdo, &ma, &mv, &unused);

#ifdef SW
        /* Set max. limit, but apply 500mA ceiling */
        charge_manager_set_ceil(port, CEIL_REQUESTOR_PD, PD_MIN_MA);
#endif
        pd_set_input_current_limit(port, ma, mv);
    }
#endif
}
#endif /* defined(PD_MAX_VOLTAGE_MV) && defined(PD_OPERATING_POWER_MW) */

bool pd_is_battery_capable(void)
{
#ifdef SW_BAT_CAP
    bool capable;

    /* Battery is present and at some minimum percentage. */
    capable = (usb_get_battery_soc() >=
               CONFIG_USB_PD_TRY_SRC_MIN_BATT_SOC);

#ifdef CONFIG_BATTERY_REVIVE_DISCONNECT
    /*
     * Not capable if the battery is in the disconnect state. The discharge
     * FET may not be enabled and so attempting being a SRC may cut off
     * our only power source at the time.
     */
    capable &= (battery_get_disconnect_state() ==
                BATTERY_NOT_DISCONNECTED);
#elif defined(CONFIG_BATTERY_PRESENT_CUSTOM) ||  \
  defined(CONFIG_BATTERY_PRESENT_GPIO)
    /*
     * When battery is cutoff in ship mode it may not be reliable to
     * check if battery is present with its state of charge.
     * Also check if battery is initialized and ready to provide power.
     */
    capable &= (battery_is_present() == BP_YES);
#endif /* CONFIG_BATTERY_PRESENT_[CUSTOM|GPIO] */

    return capable;
#else
    bool capable = 0;
    return capable;
#endif
}

#ifdef CONFIG_USB_PD_TRY_SRC
bool pd_is_try_source_capable(void)
{
    int i;
    uint8_t try_src = 0;
    bool new_try_src;

    for (i = 0; i < board_get_usb_pd_port_count(); i++)
        try_src |= (pd_get_dual_role(i) == PD_DRP_TOGGLE_ON);

    /*
     * Enable try source when dual-role toggling AND battery is capable
     * of powering the whole system.
     */
    new_try_src = (try_src && pd_is_battery_capable());

#if CONFIG_DEDICATED_CHARGE_PORT_COUNT > 0
    /*
     * If a dedicated supplier is present, power is not a concern and
     * therefore allow Try.Src if we're toggling.
     */
    new_try_src = try_src && (charge_manager_get_supplier() ==
                              CHARGE_SUPPLIER_DEDICATED);
#endif /* CONFIG_DEDICATED_CHARGE_PORT_COUNT */

    return new_try_src;
}
#endif /* CONFIG_USB_PD_TRY_SRC */

#ifdef SW_BBRAM
static int get_bbram_idx(uint8_t port)
{
    if (port < MAX_SYSTEM_BBRAM_IDX_PD_PORTS)
        return (port + SYSTEM_BBRAM_IDX_PD0);

    return -1;
}

int pd_get_saved_port_flags(int port, uint8_t *flags)
{
    if (system_get_bbram(get_bbram_idx(port), flags) != EC_SUCCESS)
    {
#ifndef CHIP_HOST
        ccprintf("PD NVRAM FAIL");
#endif
        return EC_ERROR_UNKNOWN;
    }

    return EC_SUCCESS;
}

static void pd_set_saved_port_flags(int port, uint8_t flags)
{
    if (system_set_bbram(get_bbram_idx(port), flags) != EC_SUCCESS)
    {
#ifndef CHIP_HOST
        ccprintf("PD NVRAM FAIL");
#endif
    }
}

void pd_update_saved_port_flags(int port, uint8_t flag, uint8_t do_set)
{
    uint8_t saved_flags;

    if (pd_get_saved_port_flags(port, &saved_flags) != EC_SUCCESS)
        return;

    if (do_set)
        saved_flags |= flag;
    else
        saved_flags &= ~flag;

    pd_set_saved_port_flags(port, saved_flags);
}
#endif  /* SW_BBRAM */