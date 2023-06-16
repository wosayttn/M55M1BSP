/* Copyright (c) 2022, Arm Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** \addtogroup hal */
/** @{*/

#ifndef MDH_DMA_API_H
#define MDH_DMA_API_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct mdh_dma_s mdh_dma_t;

typedef enum mdh_dma_error_e {
    MDH_DMA_ERROR_NO_ERROR = 0,
    MDH_DMA_ERROR_INVALID_ARG = -1,
    MDH_DMA_ERROR_INTERNAL_ERROR = -2,
    MDH_DMA_ERROR_NOT_READY = -3,

    // Defined for extension purposes.
    MDH_DMA_ERROR_LAST = -4
} mdh_dma_error_t;

typedef enum mdh_dma_state_e {
    MDH_DMA_STATE_READY = 0,
    MDH_DMA_STATE_BUSY = -1,
    MDH_DMA_STATE_ERROR = -2,

    // Defined for extension purposes.
    MDH_DMA_STATE_LAST = -3
} mdh_dma_state_t;

typedef void (*mdh_dma_callback_f)(void *ctx);

typedef struct mdh_dma_vtable_s {
    mdh_dma_error_t (*dma_memmove)(
        const mdh_dma_t *self, void *src, void *des, uint32_t size, mdh_dma_callback_f cbk, void *ctx);
    mdh_dma_state_t (*get_state)(const mdh_dma_t *self);
} mdh_dma_vtable_t;

struct mdh_dma_s {
    const mdh_dma_vtable_t *vfptr;
};

/**
 * \defgroup hal_dma DMA HAL functions
 *
 * # Defined behavior
 * * ::mdh_dma_memmove copies data using DMA, with handling overlapping source and destination. The
 *                     function returns after the DMA is setup.
 * * ::mdh_dma_get_state gets the execution state, whether the DMA is busy or ready for the next
 *                       command or error happend
 *
 *
 * # Undefined behavior
 *
 * @{
 */

/** Setups DMA to copy a specified number of bytes from one memory address to another.
 *         The memory ranges can overlap.
 *
 * @param self      The DMA object
 * @param src       Source address, where to copy from
 * @param des       Destination address, where to copy to
 * @param size      Number of bytes to copy
 * @param cbk       The function that will be called after the transaction is finished.
 *                  If cbk is NULL, then nothing will be called.
 * @param ctx       An application defined data passed to the callbacks. If cbk is NULL
 *                  then ctx is ignored.
 * @return  MDH_DMA_ERROR_INVALID_ARG if an argument is not correct
 *          MDH_DMA_ERROR_INTERNAL_ERROR if an error happend inside the API's implementation
 *          MDH_DMA_ERROR_NOT_READY if the DMA's state is not ready
 *          MDH_DMA_ERROR_NO_ERROR if no error happend
 **/
static inline mdh_dma_error_t
mdh_dma_memmove(const mdh_dma_t *self, void *src, void *des, uint32_t size, mdh_dma_callback_f cbk, void *ctx)
{
    assert((NULL != self) && (NULL != self->vfptr) && (NULL != self->vfptr->dma_memmove));
    return self->vfptr->dma_memmove(self, src, des, size, cbk, ctx);
}

/** Gets the state of the DMA
 *
 * @param self      The DMA object
 * @return  MDH_DMA_STATE_READY if the DMA is ready for new commands
 *          MDH_DMA_STATE_BUSY if the DMA is busy
 *          MDH_DMA_STATE_ERROR if the DMA is in error state
 **/
static inline mdh_dma_state_t mdh_dma_get_state(const mdh_dma_t *self)
{
    assert((NULL != self) && (NULL != self->vfptr) && (NULL != self->vfptr->get_state));
    return self->vfptr->get_state(self);
}

/**@}*/

#endif // MDH_DMA_API_H

/** @}*/
