void
i386_control_a20 (int enable)
{
	void kbd_wait ()
	{
		int count;

		/* Wait until output buffer is free. */
		for (count=0; count<10000; ++count)
			if (! (inb (KBDC_AT_CTL) & KBSTS_CMDBSY))
				break;

		/* Get data from input buffer. */
		for (count=0; count<10000; ++count) {
			if (! (inb (KBDC_AT_CTL) & KBSTS_DATAVL))
				break;
			inb (KBD_DATA);
		}
	}

	kbd_wait ();

	/* Write output port */
	outb (KBDC_WOUTP, KBDC_AT_CTL);
	kbd_wait ();

	/* Enable/disable A20 address line - bit 0x02. */
	outb (enable ? 0xDF : 0xDD, KBD_DATA);
	kbd_wait ();

	/* Output a dummy command. */
	outb (0xFF, KBDC_AT_CTL);
	kbd_wait ();
}
