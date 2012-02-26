/*
 * MCT-02 
 *
 * Copyright (C) 2010 Serge Vakulenko, <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
#ifndef _MCT_02_H_
#define _MCT_02_H_
#ifdef __cplusplus

extern "C" {
#endif

#ifdef __cplusplus
}
#endif

typedef struct {
	unsigned addr; 
	unsigned val; 
} MEM;

#define UART_MAX	4
#define UART0		0
#define UART1		1
#define UART2		2
#define UART3		3

#define	CLKEN_ADDR	0xb82f4004
#define	CLKEN_VAL	0xffffffff
#define	CRPLL_ADDR	0xb82f4000
#define	CRPLL_VAL	0x00000808
#define	CSCON0_ADDR	0xb82f1000
#define	CSCON0_VAL	0x001408fc
#define	CSCON1_ADDR	0xb82f1004
#define	CSCON1_VAL	0x003000fc
#define	CSCON3_ADDR	0xb82f100c
#define	CSCON3_VAL	0x00030000
#define	SDRCON_ADDR	0xb82f1014
#define	SDRCON_VAL	0x030d0030
#define	SDRTMR_ADDR	0xb82f1018
#define	SDRTMR_VAL	0x00f50222
#define	SDRCSR_ADDR	0xb82f101c
#define	SDRCSR_VAL	0x00000001
#define	CSREXT_ADDR	0xb82f1024
#define	CSREXT_VAL	0x00ff0000
#define	SRTMR_ADDR	0xb82f2000
#define	SRTMR_VAL	0x00330000

static MEM cscon[]={
	{ CLKEN_ADDR,  CLKEN_VAL  },
	{ CRPLL_ADDR,  CRPLL_VAL  },
	{ CSCON0_ADDR, CSCON0_VAL },
	{ CSCON1_ADDR, CSCON1_VAL },
	{ CSCON3_ADDR, CSCON3_VAL },
	{ SDRCON_ADDR, SDRCON_VAL },
	{ SDRTMR_ADDR, SDRTMR_VAL },
	{ SDRCSR_ADDR, SDRCSR_VAL },
	{ CSREXT_ADDR, CSREXT_VAL },
	{ SRTMR_ADDR,  SRTMR_VAL },
	{ 0x00000000, 0x00000000 },
};

#endif
