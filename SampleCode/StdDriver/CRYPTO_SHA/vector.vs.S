;/*---------------------------------------------------------------------------------------------------------*/
;/* @copyright SPDX-License-Identifier: Apache-2.0                                                          */
;/* @copyright Copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.                             */
;/*---------------------------------------------------------------------------------------------------------*/

	.section .rodata
	.globl	g_u32VectorDataBase
	.globl	g_u32VectorDataLimit

	.align	4

	.text

g_u32VectorDataBase:
    .incbin   "sha_test_vector"
g_u32VectorDataLimit:
    .space   4

    .end

