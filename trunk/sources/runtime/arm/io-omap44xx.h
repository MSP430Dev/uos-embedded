#define OMAP_R(a)       *(volatile unsigned*)(a)

#define ARM_CORTEX_REG_BASE	0x48240000

#include "cortex-a.h"
#include "omap44xx-prcm.h"
#include "omap44xx-control.h"
#include "omap44xx-gpio.h"
#include "omap44xx-sercom.h"


#define ARM_SRAM_BASE	0x40308000
#define ARM_SRAM_SIZE	0x00004000

#define OMAP_UNDEF_EXCEPTION_VECT	OMAP_R(0x4030D024)
#define OMAP_SWI_VECT			OMAP_R(0x4030D028)
#define OMAP_PREFETCH_ABORT_VECT	OMAP_R(0x4030D02C)
#define OMAP_DATA_ABORT_VECT		OMAP_R(0x4030D030)
#define OMAP_IRQ_VECT			OMAP_R(0x4030D038)
#define OMAP_FIQ_VECT			OMAP_R(0x4030D03C)
