extern unsigned long gpio_data;

void gpio_config (int pnum, int mode);

/*
 * Get the value of N-th general purpose pin.
 * Returns 0 or 1. Pnum must be in range 0..25.
 */
static inline bool_t
gpio_get (int pnum)
{
	/* IOPDATA */
	return ((*(volatile unsigned long*) 0x07ff5008) & (1ul << pnum)) != 0;
}

/*
 * Set the value of N-th general purpose pin.
 * Val must be 0 or 1. Pnum must be in range 0..25.
 */
static inline void
gpio_set_nocli (int pnum, bool_t val)
{
	volatile unsigned long *iopdata = (volatile unsigned long*) 0x07ff5008;

	if (val) *iopdata = (gpio_data |= 1ul << pnum);
	else	 *iopdata = (gpio_data &= ~(1ul << pnum));
}

static inline void
gpio_set (int pnum, bool_t val)
{
	int x;

	arm_intr_disable (&x);
	gpio_set_nocli (pnum, val);
	arm_intr_restore (x);
}
