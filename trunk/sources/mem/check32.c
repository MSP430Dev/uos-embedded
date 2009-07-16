#include <runtime/lib.h>
#include <mem/mem.h>

/*
 * Fast write/read check of 32-bit memory.
 * On error, print it and continue testing.
 * When finished, loop forever on last error, calling user defined function.
 */
void mem_check32 (uint32_t start, uint32_t end,
	void (*error_callback) (void))
{
	volatile uint32_t *addr;
	uint32_t offset, off;
	uint32_t val, rval;
	const uint32_t VAL1 = (uint32_t) 0x55555555UL;
	const uint32_t VAL2 = (uint32_t) 0xaaaaaaaaUL;
	volatile uint32_t *err_addr = 0;
	uint32_t err_val = 0;

	/* Testing data bus. Write running 1 to offset 0.
	 * On error, loop forever. */
	addr = (volatile uint32_t*) start;
	for (val=1; val; val<<=1) {
		*addr = val;
		rval = *addr;
		if (rval != val) {
			err_addr = addr;
			err_val = val;
			debug_printf ("\nData bus error at address %08x: written %08x, read %08x",
				addr, val, rval);
		}
	}
	if (err_addr) {
		/* Loop for oscilloscope. */
loop_forever:	for (;;) {
			error_callback ();
			*err_addr = err_val;
			rval = *err_addr;
			if (rval != err_val) {
				debug_printf ("\nData bus error at address %08x: written %08x, read %08x",
					err_addr, err_val, rval);
			}
		}
	}

	/* Running 1 on address bus.
	 * Write VAL1 to nonzero addresses.
	 */
	for (offset = sizeof (uint32_t); offset && start+offset < end; offset <<= 1) {
		addr = (volatile uint32_t*) (start + offset);
		*addr = VAL1;
	}

	/* Verify that writing to 0 address does not touch nonzero addresses. */
	addr = (volatile uint32_t*) start;
	*addr = VAL2;
	for (offset = sizeof (uint32_t); offset && start+offset < end; offset <<= 1) {
		addr = (volatile uint32_t*) (start + offset);
		rval = *addr;
		if (rval != VAL1) {
			err_addr = addr;
			err_val = VAL1;
			debug_printf ("\nAddress %08x bus error: written %08x, read %08x",
				addr, VAL1, rval);
		}
	}
	if (err_addr)
		goto loop_forever;
	addr = (volatile uint32_t*) start;
	*addr = VAL1;

	/* Verify that writing to nonzero addresses does not touch other data. */
	for (offset = sizeof (uint32_t); offset && start+offset < end; offset <<= 1) {
		/* Write VAL2 to one of nonzero addreses. */
		addr = (volatile uint32_t*) (start + offset);
		*addr = VAL2;

		/* Verify zero address. */
		addr = (volatile uint32_t*) start;
		rval = *addr;
		if (rval != VAL1) {
			err_addr = addr;
			err_val = VAL1;
			debug_printf ("\nAddress %08x bus error: written %08x, read %08x",
				addr, VAL1, rval);
		}

		/* Verify other nonzero addresses. */
		for (off = sizeof (uint32_t); off && start+off < end; off <<= 1) {
			if (off == offset)
				continue;
			addr = (volatile uint32_t*) (start + off);
			rval = *addr;
			if (rval != VAL1) {
				err_addr = addr;
				err_val = VAL1;
				debug_printf ("\nAddress %08x bus error: written %08x, read %08x",
					addr, VAL1, rval);
			}
		}

		/* Restore VAL1 back. */
		addr = (volatile uint32_t*) (start + offset);
		*addr = VAL1;
	}
	if (err_addr)
		goto loop_forever;
}
