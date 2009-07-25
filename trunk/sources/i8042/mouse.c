/*
 * Driver for PS/2 mouse.
 *
 * Copyright (C) 2005 Serge Vakulenko
 *
 * Some code in this file is extracted from the Vsta mouse driver.
 * Specifications of PS/2 mouse interface are available
 * at http://www.computer-engineering.org/ps2mouse/
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "input/mouse.h"
#include "i8042/mouse.h"
#include "i8042/i8042.h"

#if I386
#   define RECEIVE_IRQ		12
#endif

static int
set_sample_rate (int rate)
{
	unsigned char ack;

	/* Send a reset and wait for an ack response. */
	i8042_aux_write (KBDK_TYPEMATIC);
	if (! i8042_read (&ack) || ack != KBDR_ACK) {
		/*debug_printf ("set_sample_rate: no ACK after typematic\n");*/
		return 0;
	}
	i8042_aux_write (rate);
	if (! i8042_read (&ack) || ack != KBDR_ACK) {
		/*debug_printf ("set_sample_rate: no ACK after rate\n");*/
		return 0;
	}
	return 1;
}

/*
 * Detect Intellimouse device.
 */
static int
detect_wheel ()
{
	int count;
	unsigned char ack;

	set_sample_rate (200);
	set_sample_rate (100);
	set_sample_rate (80);

	/* Send a reset and wait for an ack response. */
	i8042_aux_write (KBDK_READ_ID);
	if (! i8042_read (&ack) || ack != KBDR_ACK) {
		/*debug_printf ("mouse: wheel: no ACK after READ_ID\n");*/
		return 0;
	}

	/* Get device id. */
	for (count=0; count<1000; ++count) {
		if (i8042_read (&ack)) {
			/*debug_printf ("mouse: wheel: %02x (%d)\n", ack, count);*/
			if (ack == 3)
				return 1;
			return 0;
		}
	}
	/*debug_printf ("mouse: wheel: no reply after READ_ID\n");*/
	return 0;
}

/*
 * Read mouse movement data. Return immediately, do not wait.
 * Return 1 if data available, 0 if no data.
 */
static int
mouse_ps2_get_move (mouse_ps2_t *u, mouse_move_t *data)
{
	mutex_lock (&u->lock);
	if (u->in_first == u->in_last) {
		mutex_unlock (&u->lock);
		return 0;
	} else {
		*data = *u->in_first++;
		if (u->in_first >= u->in_buf + MOUSE_INBUFSZ)
			u->in_first = u->in_buf;
		mutex_unlock (&u->lock);
		return 1;
	}
}

/*
 * Wait for the mouse movement and return it.
 */
static void
mouse_ps2_wait_move (mouse_ps2_t *u, mouse_move_t *data)
{
	mutex_lock (&u->lock);

	/* Wait until receive data available. */
	while (u->in_first == u->in_last)
		mutex_wait (&u->lock);

	*data = *u->in_first++;
	if (u->in_first >= u->in_buf + MOUSE_INBUFSZ)
		u->in_first = u->in_buf;

	mutex_unlock (&u->lock);
}

static void
make_move (mouse_ps2_t *u, mouse_move_t *m)
{
	m->buttons = 0;
	if (u->buf[0] & 1)
		m->buttons |= MOUSE_BTN_LEFT;
	if (u->buf[0] & 2)
		m->buttons |= MOUSE_BTN_RIGHT;
	if (u->buf[0] & 4)
		m->buttons |= MOUSE_BTN_MIDDLE;

	if (! (u->buf[0] & 0x40)) {
		m->dx = u->buf[1];
		if (u->buf[0] & 0x10)
			m->dx -= 256;
	} else {
		/* X oferflow */
		m->dx = (u->buf[0] & 0x10) ? -256 : 255;
	}
	if (! (u->buf[0] & 0x80)) {
		m->dy = u->buf[2];
		if (u->buf[0] & 0x20)
			m->dy -= 256;
	} else {
		/* Y oferflow */
		m->dy = (u->buf[0] & 0x20) ? -256 : 255;
	}
	if (u->wheel)
		m->dz = (signed char) u->buf[3];
	else
		m->dz = 0;

	/* Invert vertical axis. */
	m->dy = -m->dy;
	/*debug_printf ("mouse: (%d, %d) %d\n", m->dx, m->dy, m->buttons);*/
}

/*
 * Process a byte, received from mouse.
 * Return 1 when new move record is generated.
 */
static int
receive_byte (mouse_ps2_t *u, unsigned char byte)
{
	mouse_move_t *newlast;

	/* Synchronize to mouse stream. */
	if (u->count == 0 && ! (byte & 8))
		return 0;

	u->buf [u->count++] = byte;
	if (u->count == 1 && u->buf[0] == KBDR_ACK) {
		/* Ignore excess ACK. */
		u->count = 0;
		return 0;
	}
	if (u->count == 2 && u->buf[0] == KBDR_TEST_OK && u->buf[1] == 0) {
		/* New device connected. */
		i8042_aux_enable ();
		u->count = 0;
		u->wheel = 0;
		u->in_first = u->in_last = u->in_buf;
		if (detect_wheel ())
			u->wheel = 1;
		return 0;
	}
	if (u->count < 3)
		return 0;
	if (u->wheel && u->count < 4)
		return 0;
	u->count = 0;

	/* Advance queue pointer. */
	newlast = u->in_last + 1;
	if (newlast >= u->in_buf + MOUSE_INBUFSZ)
		newlast = u->in_buf;

	/* Ignore input on buffer overflow. */
	if (newlast == u->in_first)
		return 0;

	/* Make move record. */
	make_move (u, u->in_last);
	u->in_last = newlast;
	return 1;
}

/*
 * Process mouse interrupt.
 * Get raw data and put to buffer. Return 1 when not enough data
 * for a move record.
 * Return 0 when a new move record is generated and
 * a signal to mouse task is needed.
 */
static bool_t
mouse_ps2_interrupt (mouse_ps2_t *u)
{
	unsigned char c, sts, strobe;
	int event_generated = 0;

	/* Read the pending information. */
	for (;;) {
		sts = inb (KBDC_AT_CTL);
		if (! (sts & KBSTS_DATAVL))
			return (event_generated == 0);

		c = inb (KBD_DATA);
		if (! (sts & KBSTS_AUX_DATAVL)) {
			/* This byte is for keyboard.
			 * Just ignore it for now (TODO). */
			continue;
		}

		/* Strobe the keyboard to ack the char. */
		strobe = inb (KBDC_XT_CTL);
		outb (strobe | KBDC_XT_CLEAR, KBDC_XT_CTL);
		outb (strobe, KBDC_XT_CTL);

		/*debug_printf ("<%02x> ", c);*/
		if (receive_byte (u, c))
			event_generated = 1;
	}
}

/*
 * Mouse task.
 */
static void
mouse_ps2_task (void *arg)
{
	mouse_ps2_t *u = arg;

	mutex_lock_irq (&u->lock, RECEIVE_IRQ,
		(handler_t) mouse_ps2_interrupt, u);

	if (i8042_aux_probe ()) {
		i8042_aux_enable ();
		if (detect_wheel ())
			u->wheel = 1;
	}

	for (;;) {
		mutex_wait (&u->lock);
		/* Nothing to do. */
	}
}

static mouse_interface_t mouse_ps2_interface = {
	(void (*) (mouse_t*, mouse_move_t*))	mouse_ps2_wait_move,
	(int (*) (mouse_t*, mouse_move_t*))	mouse_ps2_get_move,
};

void
mouse_ps2_init (mouse_ps2_t *u, int prio)
{
        u->interface = &mouse_ps2_interface;
	u->count = 0;
	u->wheel = 0;
	u->in_first = u->in_last = u->in_buf;

	/* Create mouse receive task. */
	task_create (mouse_ps2_task, u, "mouse", prio,
		u->stack, sizeof (u->stack));
}
