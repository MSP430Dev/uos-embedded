#include "kernel/internal.h"

/*
 * Samsung ARM7-specific defines for UART driver.
 * For now, UART 0 only.
 * Transmit FIFO must be enabled, for TFFUL bit to work correctly.
 */
#define RECEIVE_IRQ(p)	((p) ? 7 : 5)	/* receive complete */
#define TRANSMIT_IRQ(p)	((p) ? 6 : 4)	/* transmit empty */

#define enable_transmitter(p)		(ARM_UCON(p) = (ARM_UCON(p) & \
						~ARM_UCON_TMODE_MASK) | \
						ARM_UCON_TMODE_IRQ | \
						ARM_UCON_TFEN, \
					ARM_UINTEN(p) |= ARM_UINTEN_THEIE)
#define disable_transmitter(p)		(ARM_UCON(p) = (ARM_UCON(p) & \
						~ARM_UCON_TMODE_MASK), \
					ARM_UINTEN(p) &= ~ARM_UINTEN_THEIE)
#define enable_receiver(p)		(ARM_UCON(p) = (ARM_UCON(p) & \
						~ARM_UCON_RMODE_MASK) | \
						ARM_UCON_RMODE_IRQ)
#define disable_receiver(p)		(ARM_UCON(p) = (ARM_UCON(p) & \
						~ARM_UCON_RMODE_MASK))
#define enable_rx_fifo(p)		(ARM_UCON(p) = (ARM_UCON(p) | \
						ARM_UCON_RFEN))
#define disable_rx_fifo(p)		(ARM_UCON(p) = (ARM_UCON(p) & \
						~ARM_UCON_RFEN))
#define reset_rx_fifo(p)		(ARM_UCON(p) = (ARM_UCON(p) | \
						ARM_UCON_RFRST))
#define enable_receive_interrupt(p)	( ARM_UINTEN(p) |= ARM_UINTEN_RDVIE | \
						ARM_UINTEN_RFREAIE)
#define disable_receive_interrupt(p)	(ARM_UINTEN(p) &= ~(ARM_UINTEN_RDVIE | \
						ARM_UINTEN_RFREAIE))
#define enable_transmit_interrupt(p) {		\
	int x;					\
	MACHDEP_INTR_DISABLE (&x);		\
	ARM_INTMSK &= ~(1 << TRANSMIT_IRQ(p));	\
	MACHDEP_INTR_RESTORE (x); }
#define disable_transmit_interrupt(p) {		\
	int x;					\
	MACHDEP_INTR_DISABLE (&x);		\
	ARM_INTMSK |= (1 << TRANSMIT_IRQ(p));   \
	MACHDEP_INTR_RESTORE (x); }

#define transmit_byte(p,c)		(ARM_UTXBUF(p) = (c))
#define test_transmit_complete(p)	(ARM_USTAT(p) & ARM_USTAT_TC)
#define get_received_byte(p)		ARM_URXBUF(p)

#define test_transmitter_enabled(p)	(ARM_UCON(p) & ARM_UCON_TMODE_MASK)
#define test_transmitter_empty(p)	(! (ARM_USTAT(p) & ARM_USTAT_TFFUL))
#define test_receive_data(p)		(ARM_USTAT(p) & ARM_USTAT_RDV)
#define test_get_receive_data(p,d)	((ARM_USTAT(p) & ARM_USTAT_RDV) ? \
					((*d) = ARM_URXBUF(p), 1) : 0)
#define test_frame_error(p)		(ARM_USTAT(p) & ARM_USTAT_FER)
#define test_parity_error(p)		(ARM_USTAT(p) & ARM_USTAT_PER)
#define test_overrun_error(p)		(ARM_USTAT(p) & ARM_USTAT_OER)
#define test_break_error(p)		(ARM_USTAT(p) & ARM_USTAT_BKD)
#define clear_frame_error(p)		(ARM_USTAT(p) = ARM_USTAT_FER)
#define clear_parity_error(p)		(ARM_USTAT(p) = ARM_USTAT_PER)
#define clear_overrun_error(p)		(ARM_USTAT(p) = ARM_USTAT_OER)
#define clear_break_error(p)		(ARM_USTAT(p) = ARM_USTAT_BKD)

/*
 * Baudrate generator source - (SYSCLK/2)
 */
#define setup_baud_rate(p, khz, baud) {                                      \
	    ARM_UCON(p) = ARM_UCON_WL_8;                                     \
	    ARM_UINTEN(p) = 0;                                               \
	    ARM_UBRDIV(p) = ((((khz)*500L / (baud)) + 8) / 16 - 1) << 4;     \
	}

/*
 * Baudrate generator source - UCLK pin
 */
#define setup_uclk_baud_rate(p, khz, baud) {                                 \
	    ARM_UCON(p) = ARM_UCON_WL_8 | ARM_UCON_CKSL;                     \
	    ARM_UINTEN(p) = 0;                                               \
	    ARM_UBRDIV(p) = ((((khz)*1000L / (baud)) + 8) / 16 - 1) << 4;    \
	}
