/*
 * IIC (I2C) driver (master mode only)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#ifndef __IIC_MIIC_H_
#define __IIC_MIIC_H_

#ifndef __SYS_LIB_H_
#include <runtime/lib.h>
#endif

#define MIIC_T		                                            \
	lock_t lock;     /* semaphore (to prevent concurent use) */ \
	lock_t irq_lock; /* irq lock */                             \
	bool_t (*transaction) (struct _miic_t *c, uint_t sla,       \
		void *tb, uint_t ts, void *rb, uint_t rs);
/* TODO: add IIC controller statistics */

typedef struct _miic_t {
	MIIC_T;
} miic_t;

#define miic_transaction(c,sla,tb,ts,rb,rs) \
	(*((c)->transaction)) ((c), (sla) ,(tb), (ts), (rb), (rs))
#define miic_write(c,sla,buf,sz) \
	(*((c)->transaction)) ((c), (sla) ,(buf), (sz), 0, 0)
#define miic_read(c,sla,buf,sz) \
	(*((c)->transaction)) ((c), (sla) , 0, 0, (buf), (sz))

#endif /* __IIC_MIIC_H_ */
