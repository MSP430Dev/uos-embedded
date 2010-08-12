#include <runtime/lib.h>

/*
 * Microsecond delay for MIPS.
 */
void udelay (unsigned usec)
{
	unsigned now = mips32_read_c0_register (C0_COUNT);
	unsigned final = now + usec * (KHZ / 1000);

	for (;;) {
		now = mips32_read_c0_register (C0_COUNT);

		/* This comparison is valid only when using a signed type. */
		if ((int) (now - final) >= 0)
			break;
	}
}
