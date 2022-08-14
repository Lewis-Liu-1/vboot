/*
 * s3c2440 head.S:
 *   Initialise hardware
 *
 * Copyright (C) 2001 MIZI Research, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * Author: Janghoon Lyu <nandy@mizi.com>
 * Date  : $Date: 2007-07-03 13:55:10 $
 *
 * $Revision: 1.2 $
 *
 *
 * History:
 *
 * 2002-05-14: Janghoon Lyu <nandy@mizi.com>
 *   - Initial code
 *
 */
#define __ASSEMBLY__

#include "s3c2440.h"
#include "smdk2440.h"
#include "parameters.h"

@ Start of executable code 

/* Fin = 12MHz */
#define S3C2440_UPLL_48MHZ_Fin12MHz     ((0x38<<12)|(0x02<<4)|(0x02))

@
@ Exception vector table (physical address = 0x00000000)
@

	.section .text.FirstSector
	.globl first_sector

first_sector:
@ 0x00: Reset
	b	Reset

@ 0x04: Undefined instruction exception
UndefEntryPoint:
	b	UndefEntryPoint

@ 0x08: Software interrupt exception
SWIEntryPoint:
	b	SWIEntryPoint

@ 0x0c: Prefetch Abort (Instruction Fetch Memory Abort)
PrefetchAbortEnteryPoint:
	b	PrefetchAbortEnteryPoint

@ 0x10: Data Access Memory Abort
DataAbortEntryPoint:
	b	DataAbortEntryPoint

@ 0x14: Not used
NotUsedEntryPoint:
	b	NotUsedEntryPoint

@ 0x18: IRQ(Interrupt Request) exception
IRQEntryPoint:
	b	IRQHandle

@ 0x1c: FIQ(Fast Interrupt Request) exception
FIQEntryPoint:
	b	FIQEntryPoint

@0x20: Fixed address global value. will be replaced by downloader.

	.long ZBOOT_MAGIC
	.byte OS_TYPE, HAS_NAND_BIOS, (LOGO_POS & 0xFF), ((LOGO_POS >>8) &0xFF)
	.long OS_START
	.long OS_LENGTH
	.long OS_RAM_START
	.string LINUX_CMD_LINE

	.section .text
Reset:
	@ disable watch dog timer
	mov	r1, #0x53000000
	mov	r2, #0x0
	str	r2, [r1]

	@ disable all interrupts
	mov	r1, #INT_CTL_BASE
	mov	r2, #0xffffffff
	str	r2, [r1, #oINTMSK]
	ldr	r2, =0x7ff
	str	r2, [r1, #oINTSUBMSK]	

	@ initialise system clocks
	mov	r1, #CLK_CTL_BASE
	mvn	r2, #0xff000000
	str	r2, [r1, #oLOCKTIME]
	
	mov	r1, #CLK_CTL_BASE
	ldr	r2, clkdivn_value
	str	r2, [r1, #oCLKDIVN]

	mrc	p15, 0, r1, c1, c0, 0		@ read ctrl register 
	orr	r1, r1, #0xc0000000		@ Asynchronous  
	mcr	p15, 0, r1, c1, c0, 0		@ write ctrl register

	mov	r1, #CLK_CTL_BASE

	ldr r2, =S3C2440_UPLL_48MHZ_Fin12MHz
	str r2, [r1, #oUPLLCON]

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	
	ldr	sp, DW_STACK_START	@ setup stack pointer

	ldr 	r2, mpll_value_USER 		@ clock user set 12MHz
	str	r2, [r1, #oMPLLCON]
	bl	memsetup

	@ set GPIO for UART
	mov	r1, #GPIO_CTL_BASE
	add	r1, r1, #oGPIO_H
	ldr	r2, gpio_con_uart	
	str	r2, [r1, #oGPIO_CON]
	ldr	r2, gpio_up_uart
	str	r2, [r1, #oGPIO_UP]	
	bl	InitUART


	@ get read to call C functions
	mov	fp, #0			@ no previous frame, so fp=0
	mov	a2, #0			@ set argv to NULL 

	bl	Main			

1:	b	1b @


/*
 * subroutines
 */

memsetup:
	@ initialise the static memory 

	@ set memory control registers
	mov	r1, #MEM_CTL_BASE
	adrl	r2, mem_cfg_val
	add	r3, r1, #52
1:	ldr	r4, [r2], #4
	str	r4, [r1], #4
	cmp	r1, r3
	bne	1b
	mov	pc, lr


	.globl ReadPage512

ReadPage512:
	stmfd	sp!, {r2-r7}
	mov	r2, #0x200
1:
	ldr	r4, [r1]
	ldr	r5, [r1]
	ldr	r6, [r1]
	ldr	r7, [r1]
	stmia	r0!, {r4-r7}
	ldr	r4, [r1]
	ldr	r5, [r1]
	ldr	r6, [r1]
	ldr	r7, [r1]
	stmia	r0!, {r4-r7}
	ldr	r4, [r1]
	ldr	r5, [r1]
	ldr	r6, [r1]
	ldr	r7, [r1]
	stmia	r0!, {r4-r7}
	ldr	r4, [r1]
	ldr	r5, [r1]
	ldr	r6, [r1]
	ldr	r7, [r1]
	stmia	r0!, {r4-r7}
	subs	r2, r2, #64
	bne	1b;
	ldmfd	sp!, {r2-r7}
	mov	pc,lr

@ Initialize UART
@
@ r0 = number of UART port
InitUART:
	ldr	r1, SerBase
	mov	r2, #0x0
	str	r2, [r1, #oUFCON]
	str	r2, [r1, #oUMCON]
	mov	r2, #0x3
	str	r2, [r1, #oULCON]
	ldr	r2, =0x245
	str	r2, [r1, #oUCON]

//#define UART_BAUD_RATE		115200
//#define UART_PCLK_400_148    	50000000
//#define UART_PCLK           UART_PCLK_400_148
#define UART_BRD ((UART_PCLK  / (UART_BAUD_RATE * 16)) - 1)
	mov	r2, #UART_BRD
	str	r2, [r1, #oUBRDIV]

	mov	r3, #100
	mov	r2, #0x0
1:	sub	r3, r3, #0x1
	tst	r2, r3
	bne	1b

	mov	pc, lr


IRQHandle:
	ldr	pc, =0x33f00000+0x18
	nop
	nop
	
@
@ Data Area
@
@ Memory configuration values
.align 4
mem_cfg_val:
	.long	vBWSCON
	.long	vBANKCON0
	.long	vBANKCON1
	.long	vBANKCON2
	.long	vBANKCON3
	.long	vBANKCON4
	.long	vBANKCON5
	.long	vBANKCON6
	.long	vBANKCON7
	.long	vREFRESH
	.long	vBANKSIZE
	.long	vMRSRB6
	.long	vMRSRB7


@ Processor clock values
.align 4
mpll_value_USER:
	.long   vMPLLCON_NOW_USER
clkdivn_value:
	.long	vCLKDIVN_NOW

@ initial values for serial
uart_ulcon:
	.long	vULCON
uart_ucon:
	.long	vUCON
uart_ufcon:
	.long	vUFCON
uart_umcon:
	.long	vUMCON
@ inital values for GPIO
gpio_con_uart:
	.long	vGPHCON
gpio_up_uart:
	.long	vGPHUP

	.align	2
DW_STACK_START:
	.word	0x34000000-4

.align 4
SerBase:
	.long UART0_CTL_BASE
