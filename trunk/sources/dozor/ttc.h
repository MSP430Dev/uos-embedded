#ifndef _TTC_H_
#define _TTC_H_

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <milandr/spi.h>

#define SPI_COMMAND_MAX_SZ	8

typedef struct _ttc_t
{
	mutex_t lock;
	int over_spi;
	int ttc_num;
	spi_t *spi;
	unsigned short spi_command [SPI_COMMAND_MAX_SZ];
	unsigned led_state;
} ttc_t;

void		ttc_init (ttc_t *ttc, int over_spi, int ttc_num, spi_t *spi);
unsigned short	ttc_read16 (ttc_t *ttc, unsigned short addr);
unsigned 	ttc_read32 (ttc_t *ttc, unsigned short addr);
void		ttc_read_array (ttc_t *ttc, unsigned short addr, void *buf, int size);
void 		ttc_write16 (ttc_t *ttc, unsigned short addr, unsigned short data);
void 		ttc_write32 (ttc_t *ttc, unsigned short addr, unsigned data);
void 		ttc_write_array (ttc_t *ttc, unsigned short addr, const void *buf, int size);

int 		ttc_get_data32 (ttc_t *ttc, uint32_t *result, unsigned addr0, unsigned addr_status0,
			unsigned addr1, unsigned addr_status1);
int 		ttc_wait_start_packet (ttc_t *ttc, unsigned addr_status0, unsigned addr_status1,
			int *cluster_mode, int *start_node, uint32_t *cluster_time, uint32_t *local_time);
int 		ttc_self_check (ttc_t *ttc);
void		ttc_led_on (ttc_t *ttc, int led_num);
void		ttc_led_off (ttc_t *ttc, int led_num);
void		ttc_led_sw (ttc_t *ttc, int led_num);
int		ttc_get_led (ttc_t *ttc, int led_num);

#endif /* _TTC_H_ */

