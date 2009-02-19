/*
 * Copyright (c) 2001 Dmitry Dicky diwil@eis.ru
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS `AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: io.h,v 1.19 2009/01/11 23:11:48 sb-sf Exp $
 */

#ifndef _IO_H_
#define _IO_H_

#if defined(__MSP430_1101__) || defined(__MSP430_1111__) || defined(__MSP430_1121__)
#include <runtime/msp430/msp430x11x1.h>

#elif defined(__MSP430_110__) || defined(__MSP430_112__)
#include <runtime/msp430/msp430x11x.h>

#elif defined(__MSP430_122__) || defined(__MSP430_123__)
#include <runtime/msp430/msp430x12x.h>

#elif defined(__MSP430_1122__) || defined(__MSP430_1132__)
#include <runtime/msp430/msp430x11x2.h>

#elif defined(__MSP430_1222__) || defined(__MSP430_1232__)
#include <runtime/msp430/msp430x12x2.h>

#elif defined(__MSP430_133__) || defined(__MSP430_135__)
#include <runtime/msp430/msp430x13x.h>

#elif defined(__MSP430_147__) || defined(__MSP430_148__) || defined(__MSP430_149__)
#include <runtime/msp430/msp430x14x.h>

#elif defined(__MSP430_1331__) || defined(__MSP430_1351__)
#include <runtime/msp430/msp430x13x1.h>

#elif defined(__MSP430_1471__) || defined(__MSP430_1481__) || defined(__MSP430_1491__)
#include <runtime/msp430/msp430x14x1.h>

#elif defined(__MSP430_155__) || defined(__MSP430_156__) || defined(__MSP430_157__)
#include <runtime/msp430/msp430x15x.h>

#elif defined(__MSP430_167__) || defined(__MSP430_168__) || defined(__MSP430_169__) || defined(__MSP430_1610__) || defined(__MSP430_1611__) || defined(__MSP430_1612__)
#include <runtime/msp430/msp430x16x.h>

#elif defined(__MSP430_2001__) || defined(__MSP430_2011__)
#include <runtime/msp430/msp430x20x1.h>

#elif defined(__MSP430_2002__) || defined(__MSP430_2012__)
#include <runtime/msp430/msp430x20x2.h>

#elif defined(__MSP430_2003__) || defined(__MSP430_2013__)
#include <runtime/msp430/msp430x20x3.h>

#elif defined(__MSP430_2101__) || defined(__MSP430_2111__) || defined(__MSP430_2121__) || defined(__MSP430_2131__)
#include <runtime/msp430/msp430x21x1.h>

#elif defined(__MSP430_2112__) || defined(__MSP430_2122__) || defined(__MSP430_2132__)
#include <runtime/msp430/msp430x21x2.h>

#elif defined(__MSP430_2232__) || defined(__MSP430_2252__) || defined(__MSP430_2272__)
#include <runtime/msp430/msp430x22x2.h>

#elif defined(__MSP430_2234__) || defined(__MSP430_2254__) || defined(__MSP430_2274__)
#include <runtime/msp430/msp430x22x4.h>

#elif defined(__MSP430_233__) || defined(__MSP430_235__)
#include <runtime/msp430/msp430x23x.h>

#elif defined(__MSP430_2330__) || defined(__MSP430_2350__) || defined(__MSP430_2370__)
#include <runtime/msp430/msp430x23x0.h>

#elif defined(__MSP430_247__) || defined(__MSP430_248__) || defined(__MSP430_249__) || defined(__MSP430_2410__)
#include <runtime/msp430/msp430x24x.h>

#elif defined(__MSP430_2471__) || defined(__MSP430_2481__) || defined(__MSP430_2491__)
#include <runtime/msp430/msp430x24x1.h>

#elif defined(__MSP430_2416__) || defined(__MSP430_2417__) || defined(__MSP430_2418__) || defined(__MSP430_2419__)
#include <runtime/msp430/msp430x241x.h>

#elif defined(__MSP430_2616__) || defined(__MSP430_2617__) || defined(__MSP430_2618__) || defined(__MSP430_2619__)
#include <runtime/msp430/msp430x261x.h>

#elif defined(__MSP430_311__) || defined(__MSP430_312__) || defined(__MSP430_313__) || defined(__MSP430_314__) || defined(__MSP430_315__)
#include <runtime/msp430/msp430x31x.h>

#elif defined(__MSP430_323__) || defined(__MSP430_325__)
#include <runtime/msp430/msp430x32x.h>

#elif defined(__MSP430_336__) || defined(__MSP430_337__)
#include <runtime/msp430/msp430x33x.h>

#elif defined(__MSP430_412__) || defined(__MSP430_413__) || defined(__MSP430_415__) || defined(__MSP430_417__)
#include <runtime/msp430/msp430x41x.h>

#elif defined(__MSP430_423__) || defined(__MSP430_425__) || defined(__MSP430_427__)
#include <runtime/msp430/msp430x42x.h>

#elif defined(__MSP430_4250__) || defined(__MSP430_4260__) || defined(__MSP430_4270__)
#include <runtime/msp430/msp430x42x0.h>

#elif defined(__MSP430_G4250__) || defined(__MSP430_G4260__) || defined(__MSP430_G4270__)
#include <runtime/msp430/msp430xG42x0.h>

#elif defined(__MSP430_E423__) || defined(__MSP430_E425__) || defined(__MSP430_E427__) \
    || defined(__MSP430_E4232__) || defined(__MSP430_E4242__) ||  defined(__MSP430_E4252__) || defined(__MSP430_E4272__)
#include <runtime/msp430/msp430xE42x.h>

#elif defined(__MSP430_W423__) || defined(__MSP430_W425__) || defined(__MSP430_W427__)
#include <runtime/msp430/msp430xW42x.h>

#elif defined(__MSP430_G437__) || defined(__MSP430_G438__) || defined(__MSP430_G439__)
#include <runtime/msp430/msp430xG43x.h>

#elif defined(__MSP430_435__) || defined(__MSP430_436__) || defined(__MSP430_437__) \
    || defined(__MSP430_4351__) || defined(__MSP430_4361__) || defined(__MSP430_4371__)
#include <runtime/msp430/msp430x43x.h>

#elif defined(__MSP430_447__) || defined(__MSP430_448__) || defined(__MSP430_449__)
#include <runtime/msp430/msp430x44x.h>

#elif defined(__MSP430_G4616__) || defined(__MSP430_G4617__) || defined(__MSP430_G4618__) || defined(__MSP430_G4619__)
#include <runtime/msp430/msp430xG461x.h>

#elif defined(__MSP430_4783__) || defined(__MSP430_4784__) || defined(__MSP430_4793__) || defined(__MSP430_4794__) \
    || defined(__MSP430_47166__) || defined(__MSP430_47176__) || defined(__MSP430_47186__) || defined(__MSP430_47196__) \
    || defined(__MSP430_47167__) || defined(__MSP430_47177__) || defined(__MSP430_47187__) || defined(__MSP430_47197__)
#include <runtime/msp430/msp430x47xx.h>

#elif defined(__MSP430_5418__) || defined(__MSP430_5419__) \
    || defined(__MSP430_5435__) || defined(__MSP430_5436__) || defined(__MSP430_5437__) || defined(__MSP430_5438__)
#include <runtime/msp430/msp430x54xx.h>

#else
#warning "Unknown arch! Please check"
#include <runtime/msp430/iomacros.h>
#endif

#endif
