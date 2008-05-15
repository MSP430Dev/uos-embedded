/*
 * Очередь с приоретизацией QOS (quality of service).
 * В заголовке Ethernet-пакетов формата 802.1p/802.1q
 * есть 3-битовое поле, содержажее уровень приоритета.
 * (http://www.rhyshaden.com/ethernet.htm#802.1p)
 * Такие же биты Precedence есть в поле TOS заголовка IP.
 * (http://www.rhyshaden.com/ipdgram.htm)
 * Делаем по 8 очередей на передачу.
 * Принятые пакеты надо ставить в очередь с номером,
 * соответствующим полю Precedence.
 */
#include <buf/buf-queue.h>

#define BUF_PRIO_NQ	8

typedef struct _buf_prio_queue_t {
	int count;
	int size;
	buf_queue_t queue [BUF_PRIO_NQ];
} buf_prio_queue_t;

struct _buf_t;

void buf_prio_queue_init (buf_prio_queue_t *q, struct _buf_t **buf, int bytes);
struct _buf_t *buf_prio_queue_get (buf_prio_queue_t *q);
int buf_prio_queue_put (buf_prio_queue_t *q, struct _buf_t *p, int prio);
struct _buf_t *buf_prio_queue_exchange (buf_prio_queue_t *q, struct _buf_t *p,
	int prio);
