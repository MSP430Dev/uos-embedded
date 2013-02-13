/*
 * IWDT driver for Milandr 1986BE microcontrollers.
 *
 * Copyright (C) 2013 Dmitry Podkhvatilin, <vatilin@gmail.com>
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

#include <runtime/lib.h>
#include <stream/stream.h>

void init_iwdt(unsigned timeout)
{
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_IWDT;
	ARM_IWDG->KR = ARM_IWDG_KEY_UNBLOCK;
	while (ARM_IWDG->SR & ARM_IWDG_PVU);
	ARM_IWDG->PR = ARM_IWDG_PR(3);                  // 1 тик таймера - 800 мкс
	while (ARM_IWDG->SR & ARM_IWDG_RVU);
debug_printf("RLR = %X\n", ARM_IWDG_RLR((timeout*1000)/800 + 1));	
	ARM_IWDG->RLR = ARM_IWDG_RLR((timeout*1000)/800 + 1);
debug_printf("before start\n");
	ARM_IWDG->KR = ARM_IWDG_KEY_START;
	ARM_IWDG->KR = ARM_IWDG_KEY_ALIVE;
}

void ack_iwdt()
{
	ARM_IWDG->KR = ARM_IWDG_KEY_ALIVE;
}

