/*********************************************************************
 *
 *                  Compiler and hardware specific definitions
 *
 *********************************************************************
 * FileName:        Compiler.h
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F
 * Complier:        Microchip C18 v3.02 or higher
 *					Microchip C30 v2.01 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * This software is owned by Microchip Technology Inc. ("Microchip")
 * and is supplied to you for use exclusively as described in the
 * associated software agreement.  This software is protected by
 * software and other intellectual property laws.  Any use in
 * violation of the software license may subject the user to criminal
 * sanctions as well as civil liability.  Copyright 2006 Microchip
 * Technology Inc.  All rights reserved.
 *
 * This software is provided "AS IS."  MICROCHIP DISCLAIMS ALL
 * WARRANTIES, EXPRESS, IMPLIED, STATUTORY OR OTHERWISE, NOT LIMITED
 * TO MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
 * INFRINGEMENT.  Microchip shall in no event be liable for special,
 * incidental, or consequential damages.
 *
 *
 * Author               Date    	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		10/03/2006	Original, copied from old Compiler.h
 ********************************************************************/
#ifndef __COMPILER_H
#define __COMPILER_H

#define __C32__ //TODO: (DF) - this is a temp fix until the compiler spits something out that works
//#include <p32xxxx.h>
#ifndef PIC32
    #define PIC32
#endif

//#include "HardwareProfileDef.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#define ROM			const
#define memcmppgm2ram(a,b,c)	memcmp(a,b,c)
#define memcpypgm2ram(a,b,c)	memcpy(a,b,c)
#define strcpypgm2ram(a, b)	strcpy(a,b)
#define	strlenpgm(a)		strlen(a)
#define strstrrampgm(a,b)	strstr(a,b)
#define Reset()			asm("reset")

#endif
