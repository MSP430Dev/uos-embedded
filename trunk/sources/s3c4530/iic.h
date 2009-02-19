/*
 * Samsung S3C4530A IIC (I2C) driver.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * S3C4530A support only master mode.
 */

#ifndef __S3C4530_IIC_H_
#define __S3C4530_IIC_H_

#ifndef __IIC_MIIC_H_
#include <iic/miic.h>
#endif

typedef struct _s3c4530a_iic_t {
	MIIC_T;
	uint32_t iicps;
} s3c4530a_iic_t;

#define SIZE_S3C4530A_IIC_T sizeof (s3c4530a_iic_t)

miic_t* s3c4530a_iic_init (void* buf, uint32_t mclk, uint32_t rate);

#endif /* __S3C4530_IIC_H_ */
