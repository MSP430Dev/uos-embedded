#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <elvees/i2c.h>

#if defined(ELVEES_NVCOM01)

#define I2C_IRQ     23


static bool_t i2c_write(uint8_t data, uint8_t flags, mutex_t *m)
{
	MC_I2C_TXR = data;
	MC_I2C_CR = MC_I2C_SND | flags;
    //while (MC_I2C_SR & MC_I2C_IF)
        mutex_wait (m);
    MC_I2C_CR = MC_I2C_IACK;
	if (MC_I2C_SR & MC_I2C_AL)
		return 0;
    //mdelay(1);
    return 1;
}

static bool_t i2c_read(uint8_t *data, uint8_t flags, mutex_t *m)
{
    MC_I2C_CR = MC_I2C_RCV | flags;
    //while (MC_I2C_SR & MC_I2C_IF)
        mutex_wait (m);
    MC_I2C_CR = MC_I2C_IACK;
	if (MC_I2C_SR & MC_I2C_AL)
		return 0;
    //mdelay(1);
    *data = MC_I2C_RXR;
    return 1;
}

static inline void i2c_reset(elvees_i2c_t *i2c)
{
	MC_I2C_CTR = MC_I2C_PRST;
	MC_I2C_CTR = MC_I2C_EN | MC_I2C_IEN;
	MC_I2C_PRER = KHZ / (5 * i2c->rate) - 1;
}

static bool_t
trx (miic_t *c, small_uint_t sla, void *tb, small_uint_t ts,
	void *rb, small_uint_t rs)
{
    elvees_i2c_t *i2c = (elvees_i2c_t *)c;
	uint8_t* ptr;
	bool_t need_write = tb && ts;
	bool_t need_read  = rb && rs;

	assert (c && (need_write || need_read));

	sla &= ~1;
	mutex_lock (&c->lock);	/* only 1 task may use IIC at same time */
	mutex_lock_irq (&c->irq_lock, I2C_IRQ, 0, 0);

	if (need_write) {
        /* Start write transaction */
        if (!i2c_write(sla, MC_I2C_STA, &c->irq_lock))
            goto not_acked;
        
		ptr = (uint8_t*) tb;
		while (ts > 1) {
			/* debug_printf ("IIC: out data (0x%02x)\n", *ptr); */
            if (!i2c_write(*ptr++, 0, &c->irq_lock))
				goto not_acked;
            ts--;
		}
        
        if (need_read) {
            if (!i2c_write(*ptr, 0, &c->irq_lock))
				goto not_acked;
        } else {
            if (!i2c_write(*ptr, MC_I2C_NACK | MC_I2C_STO, &c->irq_lock))
				goto not_acked;
        }
	}

	if (need_read) {
		ptr = (uint8_t*) rb;
		/* out SLA with READ bit set */
		/* debug_printf ("IIC: out SLA (0x%02x)\n", sla | 1); */
        if (!i2c_write(sla | 1, MC_I2C_STA, &c->irq_lock))
            goto not_acked;

		while (rs > 1) {
            if (!i2c_read(ptr++, 0, &c->irq_lock))
                goto not_acked;
		}
        
        if (!i2c_read(ptr++, MC_I2C_NACK | MC_I2C_STO, &c->irq_lock))
            goto not_acked;
	}

	mutex_unlock_irq (&c->irq_lock);
	mutex_unlock (&c->lock);
	return 1;

not_acked:
    i2c_reset(i2c);
	mutex_unlock_irq (&c->irq_lock);
	mutex_unlock (&c->lock);

	/* debug_printf ("I2C: not acked\n"); */
	return 0;
}

void elvees_i2c_init (elvees_i2c_t *i2c, uint32_t rate)
{
    i2c->transaction = trx;
    i2c->rate = rate;
    i2c_reset(i2c);
}

#endif
