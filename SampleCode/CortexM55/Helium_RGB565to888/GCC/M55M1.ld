/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
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

/*
 * This is a simplified picture of the M55A1 memory system.
 * Please refer to the M55A1 Technical Reference Manual for
 * further information.
 *
 *
 * +---------------+   +---------------+  +-------------------+
 * | Ethos-U55     |   | Cortex-M55    +--+ ITCM/SRAM2        |
 * |               |   |               |  +-------------------+
 * |               |   |               |
 * |               |   |               |  +-------------------+ 
 * |  M1      M0   |   |               +--+ DTCM/SRAM2        |
 * +---+-------+---+   +-------+-------+  +-------------------+
 *     |       |               |
 *     |   +---+---------------+-----+
 *     |   | AMBA AXI NIC-400-Lite   |
 *     |   +---+-----------------+---+
 *     |       |                 |
 * +---+-------+------------+ +--+-------+
 * | AMBA AHB               | | SRAM 0   |
 * +---+--------+--------+--+ | SRAM 1   |
 *     |                      +----------+
 * +---+---+ 
 * | FLASH |
 * +-------+
 *
 * +-----------------------+-------------+-------------+----+-----------------------------------------------------+
 * |  Memory region name   | Base addr   |    Size     |IDAU|  MCC load address + remarks                         |
 * +-----------------------+-------------+-------------+----+-----------------------------------------------------+
 * | ITCM                  | 0x1000_0000 | 0x0002_0000 | NS | 0x1000_0000; 128 kiB.                               |
 * | ITCM                  | 0x0000_0000 | 0x0002_0000 | S  | Secure alias for NS ITCM                            |
 * | FLASH                 | 0x1010_0000 | 0x0020_0000 | NS | 0x1010_0000; 2 MiB                                  |
 * | FLASH                 | 0x0010_0000 | 0x0020_0000 | S  | Secure alias for NS BRAM                            |
 * | SRAM01                | 0x3010_0000 | 0x0010_0000 | NS | 2 banks of 512 kiB; 3cc latency)                    |
 * | SRAM01                | 0x2010_0000 | 0x0010_0000 | S  | Secure alias for NS internal SRAM
 * | DTCM                  | 0x3000_0000 | 0x0002_0000 | NS | 0x3000_0000; 128 kiB.                               |
 * | DTCM                  | 0x2000_0000 | 0x0002_0000 | S  | Secure alias for NS DTCM                            |
 * +-----------------------+-------------+-------------+----+-----------------------------------------------------+
 *
 *  Note: Ethos-U55 can access FLASH, internal SRAM 0,1 => activation buffers and
 *        the model should only be placed in those regions.
 *
 *  Note: Alias regions means that secure and non-secure addresses are mapped to the same physical
 *        memory banks.
 */

__STACK_SIZE = 0x00008000;
__HEAP_SIZE  = 0x00010000;

/* System memory brief */
MEMORY
{
  ITCM   (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00020000
  FLASH  (rx)  : ORIGIN = 0x00100000, LENGTH = 0x00200000
  SRAM01 (rwx) : ORIGIN = 0x20100000, LENGTH = 0x00100000
  DTCM   (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00020000
}


/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions ITCM and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */

ENTRY(Reset_Handler)

SECTIONS
{
  .text :
  {
    KEEP(*(.vectors))
    *(.text*)

    KEEP(*(.init))
    KEEP(*(.fini))

    /* .ctors */
    *crtbegin.o(.ctors)
    *crtbegin?.o(.ctors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    *(SORT(.ctors.*))
    *(.ctors)

    /* .dtors */
    *crtbegin.o(.dtors)
    *crtbegin?.o(.dtors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    *(SORT(.dtors.*))
    *(.dtors)

    *(.rodata*)

    KEEP(*(.eh_frame*))
  } > FLASH

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > FLASH

  .ARM.exidx :
  {
  __exidx_start = .;
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  __exidx_end = .;
  } > FLASH

  .zero.table :
  {
    . = ALIGN(4);
    __zero_table_start__ = .;
    LONG (__bss_start__)
    LONG (__bss_end__ - __bss_start__)
    __zero_table_end__ = .;

  } > FLASH

  .copy.table :
  {
    . = ALIGN(4);
    __copy_table_start__ = .;
   
    /* Section to be copied - part 1: RW data for for DTCM */
    LONG (__eitcm)
    LONG (__data_start__)
    LONG (__data_end__ - __data_start__)

    /* Section to be copied - part 2: RO code for for ITCM */
    LONG (__etext)
    LONG (__itcm_text_start__)
    LONG (__itcm_text_end__ - __itcm_text_start__ )

    __copy_table_end__ = .;
  } > FLASH

  /**
   * Location counter can end up 2byte aligned with narrow Thumb code but
   * __etext is assumed by startup code to be the LMA of a section 
   * which must be 4byte aligned
   */
  __etext = ALIGN (4);

  .itcm : AT (__etext)
  {
    __itcm_text_start__ = .;
    *(itcm)
    __itcm_text_end__ = .;
  } > ITCM

  __eitcm = __etext + (__itcm_text_end__ - __itcm_text_start__);

  .data : AT(__eitcm)
  {
    __data_start__ = .;
    *(vtable)
    *(.data)
    *(.data.*)

    . = ALIGN(4);
    /* preinit data */
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(4);
    /* init data */
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);

    . = ALIGN(4);
    /* finit data */
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);

    KEEP(*(.jcr*))
    . = ALIGN(4);
    /* All data end */
    __data_end__ = .;
  } > DTCM

  __edtcm_data = __etext + (__data_end__ - __data_start__);


  /*.sram01 :
  {
    . = ALIGN(16);
    *(.bss.tensor_arena)
  } > SRAM01*/ 

  .bss :
  {
    . = ALIGN(4);
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
  } > SRAM01

  .heap (COPY) :
  {
    . = ALIGN(8);
    __end__ = .;
    PROVIDE(end = .);
    . = . + __HEAP_SIZE;
    . = ALIGN(8);
    __HeapLimit = .;
  } > SRAM01

  .stack (ORIGIN(SRAM01) + LENGTH(SRAM01) - __STACK_SIZE) (COPY) :
  {
    . = ALIGN(8);
    __StackLimit = .;
    . = . + __STACK_SIZE;
    . = ALIGN(8);
    __StackTop = .;
  } > SRAM01
  PROVIDE(__stack = __StackTop);

  /* Check if data + heap + stack exceeds DTCM limit */
  ASSERT(__StackLimit >= __HeapLimit, "region DTCM overflowed with stack")
}
