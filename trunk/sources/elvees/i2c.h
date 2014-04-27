#ifndef __ELVEES_I2C_H_
#define __ELVEES_I2C_H_

#ifndef __IIC_MIIC_H_
#include <iic/miic.h>
#endif

typedef struct _elvees_i2c_t {
	MIIC_T;
    uint32_t rate;
} elvees_i2c_t;

void elvees_i2c_init (elvees_i2c_t *i2c, uint32_t rate);

#endif /* __ELVEES_I2C_H_ */
