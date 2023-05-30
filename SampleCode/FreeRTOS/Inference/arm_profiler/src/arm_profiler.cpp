/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "tensorflow/lite/kernels/internal/compatibility.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_time.h"

#include <string.h>

#include "arm_profiler.hpp"
#include <inttypes.h>
#include <stdio.h>

#include "NuMicro.h"


static uint64_t cpu_cycle_count = 0;

void Profiler_Tick_Hook(void)
{
    /* Increment the cycle counter based on load value. */
    cpu_cycle_count += SysTick->LOAD + 1;
}

/**
 * Gets the current SysTick derived counter value
 */
static uint64_t Get_SysTick_Cycle_Count(void)
{
    uint32_t systick_val;

    NVIC_DisableIRQ(SysTick_IRQn);
    systick_val = SysTick->VAL & SysTick_VAL_CURRENT_Msk;
    NVIC_EnableIRQ(SysTick_IRQn);

    return cpu_cycle_count + (SysTick->LOAD - systick_val);
}

namespace tflite {

ArmProfiler::ArmProfiler(size_t max_events) : max_events_(max_events), num_events_(0) {
    tags_        = std::make_unique<const char *[]>(max_events_);
    start_ticks_ = std::make_unique<int32_t[]>(max_events_);
    end_ticks_   = std::make_unique<int32_t[]>(max_events_);
}

uint32_t ArmProfiler::BeginEvent(const char *tag) {
    if (num_events_ == max_events_) {
        tflite::GetMicroErrorReporter()->Report("Profiling event overflow, max: %u events", max_events_);
        num_events_ = 0;
    }

    tags_[num_events_]        = tag;
    start_ticks_[num_events_] = Get_SysTick_Cycle_Count();
    end_ticks_[num_events_]   = start_ticks_[num_events_] - 1;

    return num_events_++;
}

void ArmProfiler::EndEvent(uint32_t event_handle) {
    TFLITE_DCHECK(event_handle < max_events_);
    end_ticks_[event_handle] = Get_SysTick_Cycle_Count();
    tflite::GetMicroErrorReporter()->Report(
        "%s : cycle_cnt : %u cycles", tags_[event_handle], end_ticks_[event_handle] - start_ticks_[event_handle]);
}

int32_t ArmProfiler::GetTotalTicks() const {
    int32_t ticks = 0;

    for (size_t i = 0; i < num_events_; ++i) {
        ticks += end_ticks_[i] - start_ticks_[i];
    }

    return ticks;
}

} // namespace tflite
