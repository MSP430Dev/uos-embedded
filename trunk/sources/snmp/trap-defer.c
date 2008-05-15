/*
 * SNMP trap manager.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <net/arp.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/udp.h>
#include <snmp/asn.h>
#include <snmp/snmp.h>
#include <mem/mem.h>
#include <snmp/trap-defer.h>

extern snmp_t *snmp;
extern udp_socket_t sock;
extern mem_pool_t pool;

#define TRAP_START	1
#define TRAP_ALARM	2
#define TRAP_LINK	3
#define TRAP_PORT	4
#define TRAP_AUTH 	5

typedef union _trap_t {
	unsigned char type;
	struct {
		unsigned char type;
		unsigned short reset_counter;
	} cold_start;
	struct {
		unsigned char type;
		unsigned char state;
	} alarm;
	struct {
		unsigned char type;
		unsigned char up;
		unsigned char index;
		unsigned char status;
	} linkport;
	struct {
		unsigned char type;
		unsigned long user_addr;
	} auth_failure;
} trap_t;

typedef struct _trap_flag {
	unsigned char up;
	unsigned char status;
} trap_flag_t;

static lock_t lock;
static trap_t *trap_queue;
static unsigned char *trap_countdown;
static trap_flag_t *trap_flag;
static unsigned char alarm_countdown;
static signed char alarm_state;
static trap_t *first;		/* самый "старый", его надо отправить первым */
static trap_t *last;		/* пустой, сюда записывается возникшее событие */
static unsigned short since_acc_ms;
static unsigned char ports, links;
static unsigned char delay_ds;
static unsigned char delay_limit_ds;

uint_t trap_defer_delay_ds;

bool_t trap_defer_init (uint_t max_delay_ds, uint_t links_count, uint_t ports_count)
{
	unsigned queue_size;

	lock_init (&lock);
	assert (trap_queue == 0);
	queue_size = 4				// LY: trap-старт, alarm-on/off, auth.
		+ links_count + links_count	// LY: link-up/down.
		+ ports_count + ports_count;	// LY: port-up/down.
	trap_queue = mem_alloc (&pool,
		queue_size * sizeof (trap_t)	// LY: кольцевая очередь trap-ов.
		+ ports_count + links_count	// LY: буфер для trap-countdown.
		+ (ports_count + links_count)	// LY: буфер для флагов up/down.
			* sizeof (trap_flag_t));
	if (! trap_queue)
		return 0;

	first = last = trap_queue;
	ports = ports_count;
	links = links_count;
	trap_countdown = (unsigned char *) (trap_queue + queue_size);
	trap_flag = (trap_flag_t *) (trap_countdown + ports_count + links_count);

	alarm_state = -1;
	memset (trap_flag, 0xFF, sizeof (trap_flag_t) * (ports_count + links_count));

	delay_limit_ds = max_delay_ds + 1;
	if (! delay_limit_ds)
		--delay_limit_ds;

	return 1;
}

/*
 * LY: обновляем таймауты определяющите возможность отправки trap-ов.
 * При превышении максимального времени ожидания, очередь очищается.
 */
static inline void trap_delay_update (unsigned char since_ds)
{
	unsigned char *p = (unsigned char *) trap_flag;

	while (--p >= trap_countdown) {
		if (*p > since_ds)
			*p -= since_ds;
		else
			*p = 0;
	}
	if (alarm_countdown > since_ds)
		alarm_countdown -= since_ds;
	else
		alarm_countdown = 0;

	if (since_ds > delay_limit_ds - delay_ds) {
		/* LY: Вышел тайм-аут отправки trap-ов, чистим очередь. */
		last = first;
		since_ds = 0;
	} else
		since_ds += delay_ds;
	delay_ds = since_ds;
}

static inline trap_t *trap_queue_next (trap_t *t)
{
	if (++t >= (trap_t *) trap_countdown)
		t = trap_queue;
	return t;
}

static inline trap_t *trap_queue_prev (trap_t *t)
{
	if (t <= trap_queue)
		t = (trap_t *) trap_countdown;
	return --t;
}

static inline bool_t trap_is_linkport (trap_t *t)
{
	return t->type == TRAP_LINK || t->type == TRAP_PORT;
}

// LY: Выдает индекс для доступа к массивам trap_countdown и trap_flag.
static inline unsigned trap_index (trap_t *t)
{
	unsigned index;

	assert (trap_is_linkport (t));
	index = t->linkport.index;
	if (t->type == TRAP_PORT)
		index += links;
	return index;
}

// LY: Ищем trap начиная с конца.
static inline trap_t *trap_lookup (trap_t *trap)
{
	trap_t *t;
	for (t = last; t != first; ) {
		t = trap_queue_prev (t);
		if (trap->type != t->type)
			continue;
		if (trap_is_linkport (trap)
		&& trap->linkport.index != t->linkport.index)
			continue;
		return t;
	}
	return 0;
}

// LY: пора ли отправлять стоящий в очереди trap, или нет.
static inline bool_t trap_should_delay (trap_t *t)
{
	if (trap_is_linkport (t) && trap_countdown [trap_index (t)])
		return 1;
	if (t->type == TRAP_ALARM && alarm_countdown)
		return 1;
	return 0;
}

// LY: решаем, что делать с поступившим trap'ом:
//       > 0 - поставить в очередь, замещая трап в очереди;
//       = 0 - игнорировать трап и почистить очередь;
//       < 0 - игнорировать трап, и не трогать очередь;
//     или:
//       - чистить очередь если >= 0;
//       - ставить трап в очередь если > 0;
static sign_t trap_decide (trap_t *now, trap_t *queued)
{
	unsigned i;
	assert (queued == 0 || now->type == queued->type);

	switch (now->type) {
	case TRAP_AUTH:
		if (queued) {
			// LY: только один трап аутонтификации может находится в очереди,
			//     обновляем данные и игнорируем новый трап.
			queued->auth_failure.user_addr = now->auth_failure.user_addr;
			return -1;
		}
		break;

	case TRAP_ALARM:
		if ((int) alarm_state == now->alarm.state)
			// LY: вернулись к уже индицированному состоянию,
			//     чистим очередь и игнорируем новый трап.
			return 0;

		if (queued && queued->alarm.state == now->alarm.state)
			// LY: игнорируем повторный trap.
			return -1;

		// LY: переход в "хорошее" состояние индицируется с задержкой.
		alarm_countdown = trap_defer_delay_ds;

		if (now->alarm.state > (int) alarm_state)
			// LY: был не-alarm, а теперь alarm,
			//     сбрасываем время, чтобы не задерживать отправку.
			alarm_countdown = 0;
		break;

	case TRAP_LINK:
	case TRAP_PORT:
		assert (queued == 0 || now->linkport.index == queued->linkport.index);
		i = trap_index (now);
		if (trap_flag[i].up == now->linkport.up
			&& trap_flag[i].status == now->linkport.status)
			// LY: вернулись к уже индицированному состоянию,
			//     чистим очередь и игнорируем новый трап.
			return 0;

		if (queued && now->linkport.up == queued->linkport.up) {
			// LY: обновляем статус и удаляем повторный trap.
			queued->linkport.status = now->linkport.status;
			return -1;
		}

		// LY: переход в "хорошее" состояние индицируется с задержкой.
		trap_countdown[i] = trap_defer_delay_ds;

		if (now->linkport.up < trap_flag[i].up)
			// LY: был не down, а теперь down,
			//     сбрасываем время, чтобы не задерживать отправку.
			trap_countdown[i] = 0;
		break;
	}

	// LY: добавляем новый trap.
	return 1;
}

/* Посылает trap-пакет, при неудаче вернет 0. */
static inline bool_t send_trap (trap_t *t)
{
	unsigned i;

	if (t->type == TRAP_AUTH) {
		if (snmp->enable_authen_traps)
			return send_auth_trap (t->auth_failure.user_addr);
	} else if (snmp->enable_traps) switch (t->type) {
		case TRAP_START:
			return send_start_trap (t->cold_start.reset_counter);

		case TRAP_ALARM:
			if (! send_alarm_trap (t->alarm.state))
				return 0;
			alarm_countdown = trap_defer_delay_ds;
			alarm_state = t->alarm.state;
			break;

		case TRAP_LINK:
			if (! send_link_trap (t->linkport.up, t->linkport.index, t->linkport.status))
				return 0;
			goto update;
		case TRAP_PORT:
			if (! send_port_trap (t->linkport.up, t->linkport.index, t->linkport.status))
				return 0;
	update:
			i = trap_index (t);
			trap_countdown[i] = trap_defer_delay_ds;
			trap_flag[i].up = t->linkport.up;
			trap_flag[i].status = t->linkport.status;
			break;
		default:
			assert (0);
	}

	return 1;
}

/*
 * Отсылка trap-ов, возвращает:
 *   0 - не требуется посылки trap-ов
 *   >0 - успешная отсылка всех имеющихся trap-ов
 *   <0 - неудачная отсылка
 */
static sign_t trap_try (void)
{
	sign_t result = 0;
	trap_t* t, *e;

	t = first; e = last;
	while (t != e) {
		if (trap_should_delay (t)) {
			/* LY: trap должен быть задержен,
			 * переставим его в конец очереди и продолжим.
			 */
			*last = *t;
			last = trap_queue_next (last);
		} else if (send_trap (t)) {
			/* Пакет отправлен удачно. */
			result = 1;
		} else {
			/* Ошибка отсылки trap-пакетов. */
			result = -1;
			break;	/* перестаем посылать trap-пакеты */
		}
		delay_ds = 0;
		t = trap_queue_next (t);
	}
	first = t;
	return result;
}

static void trap_enqueue_release (trap_t *trap)
{
	trap_t *t;
	sign_t action;

	action = trap_decide (trap, t = trap_lookup (trap));
	if (action >= 0) {
		if (t) {
			// LY: удаляем trap, стоящий в очереди.
			for (;;) {
				trap_t *p = trap_queue_next (t);
				if (p == last) {
					last = t;
					break;
				}
				*t = *p; t = p;
			}
		}
		if (action > 0) {
			// LY: добавляем новый trap.
			*last = *trap;
			last = trap_queue_next (last);

			// LY: если очередь полна, теряем самый "старый" trap.
			if (last == first)
				first = trap_queue_next (first);
			trap_try ();
		}
	}
	lock_release (&lock);
}

sign_t trap_defer_poll (unsigned short since_ms)
{
	sign_t result;

	result = 0;
	if (snmp && trap_queue && since_ms) {
		lock_take (&lock);
		if (since_ms > 255 * 99)
			// LY: чтобы небыло переполнения в байте после сложения и деления на 100.
			since_ms = 255 * 99;
		since_ms += since_acc_ms;
		if (since_ms < since_acc_ms)
			since_ms = 255 * 100;
		if (since_ms >= 100) {
			// LY: Не пытаемся отправлять TRAP'ы чаще чем 10 раз в секунду.
			trap_delay_update (since_ms / 100);
			since_ms %= 100;
			result = trap_try ();
		}
		since_acc_ms = since_ms;
		lock_release (&lock);
	}

	return result;
}

void trap_defer_cold_start (unsigned short reset_counter)
{
	trap_t t;

	if (! snmp || ! snmp->enable_traps)
		return;

	assert (trap_queue != 0);
	lock_take (&lock);
	t.type = TRAP_START;
	t.cold_start.reset_counter = reset_counter;
	trap_enqueue_release (&t);
}

void trap_defer_auth_failure (void)
{
	trap_t t;

	if (! snmp || ! snmp->enable_authen_traps)
		return;

	assert (trap_queue != 0);
	lock_take (&lock);
	t.type = TRAP_AUTH;
	t.auth_failure.user_addr = *(unsigned long*) snmp->user_addr;
	trap_enqueue_release (&t);
}

void trap_defer_link (bool_t up, uint_t link_index, uint_t link_status)
{
	trap_t t;

	if (! snmp || ! snmp->enable_traps)
		return;

	assert (trap_queue != 0);
	assert (link_index < links);

	lock_take (&lock);
	t.type = TRAP_LINK;
	t.linkport.up = up;
	t.linkport.index = link_index;
	t.linkport.status = link_status;
	trap_enqueue_release (&t);
}

void trap_defer_port (bool_t up, uint_t port_index, uint_t port_status)
{
	trap_t t;

	if (! snmp || ! snmp->enable_traps)
		return;

	assert (trap_queue != 0);
	assert (port_index < ports);

	lock_take (&lock);
	t.type = TRAP_PORT;
	t.linkport.up = up;
	t.linkport.index = port_index;
	t.linkport.status = port_status;
	trap_enqueue_release (&t);
}

void trap_defer_alarm (bool_t alarm)
{
	trap_t t;

	if (! snmp || ! snmp->enable_traps)
		return;

	assert (trap_queue != 0);
	lock_take (&lock);
	t.type = TRAP_ALARM;
	t.alarm.state = alarm;
	trap_enqueue_release (&t);
}
