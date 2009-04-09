#ifdef ARM_AT91SAM7A3
#include <runtime/arm/at91sam7a3.h>
#endif
#ifdef ARM_AT91SAM7S128
#include <runtime/arm/at91sam7s128.h>
#endif
#ifdef ARM_AT91SAM7S256
#include <runtime/arm/at91sam7s256.h>
#endif
#ifdef ARM_AT91SAM7S321
#include <runtime/arm/at91sam7s321.h>
#endif
#ifdef ARM_AT91SAM7S32
#include <runtime/arm/at91sam7s32.h>
#endif
#ifdef ARM_AT91SAM7S512
#include <runtime/arm/at91sam7s512.h>
#endif
#ifdef ARM_AT91SAM7S64
#include <runtime/arm/at91sam7s64.h>
#endif
#ifdef ARM_AT91SAM7SE256
#include <runtime/arm/at91sam7se256.h>
#endif
#ifdef ARM_AT91SAM7SE32
#include <runtime/arm/at91sam7se32.h>
#endif
#ifdef ARM_AT91SAM7SE512
#include <runtime/arm/at91sam7se512.h>
#endif
#ifdef ARM_AT91SAM7X128
#include <runtime/arm/at91sam7x128.h>
#endif
#ifdef ARM_AT91SAM7X256
#include <runtime/arm/at91sam7x256.h>
#endif
#ifdef ARM_AT91SAM7X512
#include <runtime/arm/at91sam7x512.h>
#endif
#ifdef ARM_AT91SAM7XC128
#include <runtime/arm/at91sam7xc128.h>
#endif
#ifdef ARM_AT91SAM7XC256
#include <runtime/arm/at91sam7xc256.h>
#endif
#ifdef ARM_AT91SAM7XC512
#include <runtime/arm/at91sam7xc512.h>
#endif
#ifdef ARM_AT91SAM9260
#include <runtime/arm/at91sam9260.h>
#endif
#ifdef ARM_AT91SAM9261
#include <runtime/arm/at91sam9261.h>
#endif
#ifdef ARM_AT91SAM9262
#include <runtime/arm/at91sam9262.h>
#endif
#ifdef ARM_AT91SAM9263
#include <runtime/arm/at91sam9263.h>
#endif
#ifdef ARM_AT91SAM9XE128
#include <runtime/arm/at91sam9xe128.h>
#endif
#ifdef ARM_AT91SAM9XE256
#include <runtime/arm/at91sam9xe256.h>
#endif
#ifdef ARM_AT91SAM9XE512
#include <runtime/arm/at91sam9xe512.h>
#endif

/*
 * Internal SRAM address and size.
 */
#ifdef AT91C_ISRAM
#define ARM_SRAM_BASE		((int) AT91C_ISRAM)
#define ARM_SRAM_SIZE		((int) AT91C_ISRAM_SIZE)
#else
#define ARM_SRAM_BASE		((int) AT91C_IRAM_2)
#define ARM_SRAM_SIZE		((int) AT91C_IRAM_2_SIZE)
#endif
