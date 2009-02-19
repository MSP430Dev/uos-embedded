#include "runtime/lib.h"
#include "kernel/uos.h"
#include <s3c4530/iic.h>

/*
 * S3C4530A IIC definitions
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define ARM_IICCON_BF              0x01	/* Buffer flag */
#define ARM_IICCON_IEN             0x02	/* Interrupt enable */
#define ARM_IICCON_LRB             0x04	/* Last received bit */
#define ARM_IICCON_ACK             0x08	/* Acknowledge enable */
#define ARM_IICCON_COND_MASK       0x30	/* Generate condition  */
#define ARM_IICCON_COND_NO         0x00	/* no effect  */
#define ARM_IICCON_COND_START      0x10	/* Start condition  */
#define ARM_IICCON_COND_STOP       0x20	/* Stop condition  */
#define ARM_IICCON_COND_RSTART     0x30	/* Repeat start condition  */
#define ARM_IICCON_BUSY            0x40	/* Bus Busy */
#define ARM_IICCON_RESET           0x80	/* Reset I2C controller  */
#define IIC_IRQ                      20	/* iic IRQ number */
#define IICPS(mclk,rate) ((((((mclk) + (rate) - 1) / (rate)) + 12) >> 4) - 1)

static small_uint_t inline iic_wait (lock_t* l)
{
	small_uint_t iiccon;
	while (((iiccon = ARM_IICCON) & ARM_IICCON_BF) == 0)
		lock_wait (l);
	return iiccon;
}

static void inline iic_reset (uint32_t ps)
{
	ARM_IICCON = ARM_IICCON_RESET;
	ARM_IICPS = ps;
}

/*
 * Data transfer over IIC bus. Read, write, or read after write operations.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
static bool_t
iic_transfer (miic_t *c, small_uint_t sla, void *tb, small_uint_t ts,
	void *rb, small_uint_t rs)
{
	uint8_t* ptr;
	small_uint_t iiccon;
	s3c4530a_iic_t* s = (s3c4530a_iic_t*) c;
	bool_t need_write = tb && ts;
	bool_t need_read  = rb && rs;

	assert (c && (need_write || need_read));

	sla &= ~1;
	lock_take (&s->lock);	/* only 1 task may use IIC in a same time */
	lock_take_irq (&s->irq_lock, IIC_IRQ, 0, 0);

	/* Wait iic controller ready (after last operation) */
	while (ARM_IICCON & ARM_IICCON_BUSY)
		/* nothing */;

	/* Start transaction */
	ARM_IICCON = ARM_IICCON_IEN | ARM_IICCON_COND_START | ARM_IICCON_BF;
	if (need_write) {
		/* need write -> out SLA (with bit 0 clear) and wait ACK */
		/* debug_printf ("IIC: out SLA (0x%02x)\n", sla); */
		ARM_IICBUF = sla;
		iiccon = iic_wait (&s->irq_lock);
		if (iiccon & ARM_IICCON_LRB)
			goto not_acked;

		ptr = (uint8_t*) tb;
		while (ts--) {
			/* debug_printf ("IIC: out data (0x%02x)\n", *ptr); */
			ARM_IICBUF = *ptr++;
			iiccon = iic_wait (&s->irq_lock);
			if (iiccon & ARM_IICCON_LRB)
				goto not_acked;
		}
		if (need_read) {
			/* prepare to read after write */
			ARM_IICCON = ARM_IICCON_COND_RSTART;
			/* debug_printf ("IIC: prepare to read after write\n"); */
			ARM_IICCON = ARM_IICCON_IEN | ARM_IICCON_COND_START |
			             ARM_IICCON_BF;
		}
	}

	if (need_read) {
		ptr = (uint8_t*) rb;

		/* out SLA with READ bit set */
		/* debug_printf ("IIC: out SLA (0x%02x)\n", sla | 1); */
		ARM_IICBUF = sla | 1;
		iiccon = iic_wait (&s->irq_lock);
		if (iiccon & ARM_IICCON_LRB)
			goto not_acked;

		iiccon = ARM_IICCON_IEN | ARM_IICCON_BF;
		if (rs > 1)	/* no ACK for single byte */
		         iiccon |= ARM_IICCON_ACK;
		ARM_IICCON =  iiccon;
		/* dummy read to clear BF (ts - not used here) */
		ts = ARM_IICBUF;
		while (rs--) {
			iic_wait (&s->irq_lock);
			if (rs == 1)	/* no ACK for last byte */
				ARM_IICCON = ARM_IICCON_IEN | ARM_IICCON_BF;
			*ptr++ = ARM_IICBUF;
		}
	}

 	ARM_IICCON = ARM_IICCON_COND_STOP;
	lock_release_irq (&s->irq_lock);
	lock_release (&s->lock);
	return 1;

not_acked:
	iic_reset (s->iicps);
	lock_release_irq (&s->irq_lock);
	lock_release (&s->lock);

	/* debug_printf ("IIC: not acked\n"); */
	return 0;
}

/*
 * Init S3C4530A IIC controller. Caller must provide buffer
 * for s3c4530a_iic_t structure (pointed buf parameter).
 */
miic_t* s3c4530a_iic_init (void* buf, uint32_t mclk, uint32_t rate)
{
	s3c4530a_iic_t* s = (s3c4530a_iic_t*) buf;
	assert (buf && rate && mclk > rate);

	memset (buf, 0, sizeof (s3c4530a_iic_t));

	lock_take (&s->lock);

	s->transaction = iic_transfer;
	s->iicps = IICPS (mclk, rate);
	iic_reset (s->iicps);

	lock_release (&s->lock);

	return (miic_t*) buf;
}
