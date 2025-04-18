/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "tvm_ethosu_runtime.h"

#include <ethosu_driver.h>

int32_t TVMEthosULaunch(tvm_device_ethos_u_t *context, void *cms_data, size_t cms_data_size,
                        uint64_t *base_addrs, size_t *base_addrs_size, int num_tensors)
{
    //  struct ethosu_driver* driver = (struct ethosu_driver*)context;
    struct ethosu_driver *driver = ethosu_reserve_driver();

    int32_t result =
        ethosu_invoke(driver, cms_data, cms_data_size, base_addrs, base_addrs_size, num_tensors);

    ethosu_release_driver(driver);

    // Map errors in invoke to TVM errors
    if (result != 0)
    {
        return -1;
    }

    return 0;
}

int32_t TVMDeviceEthosUActivate(tvm_device_ethos_u_t *context)
{
    return 0;
}
int32_t TVMDeviceEthosUOpen(tvm_device_ethos_u_t *context)
{
    return 0;
}
int32_t TVMDeviceEthosUClose(tvm_device_ethos_u_t *context)
{
    return 0;
}
int32_t TVMDeviceEthosUDeactivate(tvm_device_ethos_u_t *context)
{
    return 0;
}
