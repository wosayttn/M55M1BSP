/**************************************************************************//**
 * @file     scu.c
 * @version  V1.00
 * @brief    Security Configuration Unit driver source file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include "NuMicro.h"
#include "scu/mpc.h"

/** @addtogroup Standard_Driver Standard Driver
    @{
*/

/** @addtogroup KDF_Driver Key Derivation Function Driver
    @{
*/

/** @addtogroup KDF_EXPORTED_FUNCTIONS Key Derivation Function Exported Functions
    @{
*/

/**
  * @brief      Get key store size selection
  * @retval     Others   Successful
  * @retval     < 0      Fail
  * @details    This function is used to get key store size selection.
  */

#define N_MEM_RANGES (2)
int SetupMPC(const uint32_t mpc_baseaddr,
             const uint32_t mem_baseaddr, const uint32_t mem_size,
             const uint32_t baseaddr_s,  /* Secure base address */
             const uint32_t len_s,       /* Length (in bytes) of secure region */
             const uint32_t baseaddr_ns, /* Non-secure base address */
             const uint32_t len_ns)      /* Length (in bytes) of non-secure region */
{
    //printf("[%s] Enter\n", __func__);
    /* Secure range */
    const struct mpc_sie_memory_range_t mpc_range_s = {/* base */  mem_baseaddr,
                                                       /* limit */ mem_baseaddr + mem_size - 1,
                                                       /* range_offset */ 0,
                                                       /* attr */  MPC_SIE_SEC_ATTR_SECURE};

    /* Non secure range */
    const struct mpc_sie_memory_range_t mpc_range_ns = {/* base */  mem_baseaddr + NS_OFFSET,
                                                        /* limit */ mem_baseaddr + NS_OFFSET + mem_size - 1,
                                                        /* range_offset */ 0,
                                                        /* attr */  MPC_SIE_SEC_ATTR_NONSECURE};

    /* Consolidated ranges */
    const struct mpc_sie_memory_range_t *mpc_range_list[N_MEM_RANGES] = {&mpc_range_s, &mpc_range_ns};

    /* MPC device configuration controller */
    const struct mpc_sie_dev_cfg_t mpc_dev_cfg = {mpc_baseaddr};

    /* MPC device data */
    struct mpc_sie_dev_data_t mpc_dev_data;

    /* MPC device itself */
    struct mpc_sie_dev_t mpc_dev = {&mpc_dev_cfg, &mpc_dev_data};

    enum mpc_sie_error_t ret = MPC_SIE_ERR_NONE;

    //printf("Configuring MPC range 0x%08X-0x%08X\n", mem_baseaddr, mem_baseaddr + mem_size);
    //printf("baseaddr_s: 0x%08X, len_s: 0x%08X, baseaddr_ns: 0x%08X, len_ns: 0x%08X\n", baseaddr_s, len_s, baseaddr_ns, len_ns);

    /* Initialise this MPC device */
    ret = mpc_sie_init(&mpc_dev, mpc_range_list, N_MEM_RANGES);
    if (MPC_SIE_ERR_NONE != ret) {
        printf("Error initialising MPC range 0x%08X-0x%08X\n", mem_baseaddr, mem_baseaddr + mem_size);
        return 1;
    }

    /* Configure the non secure region */
    if (len_ns > 0)
    {
        ret = mpc_sie_config_region(&mpc_dev, baseaddr_ns, baseaddr_ns + len_ns - 1, MPC_SIE_SEC_ATTR_NONSECURE);
        if (MPC_SIE_ERR_NONE != ret) {
            printf("Error configuring non-secure region 0x%08X-0x%08X (%d)\n", baseaddr_ns, baseaddr_ns + len_ns, ret);
            return 1;
        }
    }

    /* Configure the secure region */
    if (len_s > 0)
    {
        ret = mpc_sie_config_region(&mpc_dev, baseaddr_s, baseaddr_s + len_s - 1, MPC_SIE_SEC_ATTR_SECURE);
        if (MPC_SIE_ERR_NONE != ret) {
            printf("Error configuring secure region 0x%08X-0x%08X\n", baseaddr_s, baseaddr_s + len_s);
            return 1;
        }
    }
    
    if (get_sie_version(&mpc_dev) == SIE200)
    {
        ret = mpc_sie_set_sec_resp(&mpc_dev, MPC_SIE_RESP_BUS_ERROR);
        if (MPC_SIE_ERR_NONE != ret) {
            printf("Error configuring secure response\n");
            return 1;
        }
        
        ret = mpc_sie_irq_enable(&mpc_dev);
        if (MPC_SIE_ERR_NONE != ret) {
            printf("Error enable interrupt\n");
            return 1;
        }
    }
    
    mpc_sie_clear_irq(&mpc_dev);

    /* Lock down the configuration */
    ret = mpc_sie_lock_down(&mpc_dev);
    if (MPC_SIE_ERR_NONE != ret) {
        printf("Error locking down MPC range 0x%08X-0x%08X\n", mem_baseaddr, mem_baseaddr + mem_size);
        return 1;
    }

    return 0;
}

/** @} end of group KDF_EXPORTED_FUNCTIONS */
/** @} end of group KDF_Driver */
/** @} end of group Standard_Driver */
