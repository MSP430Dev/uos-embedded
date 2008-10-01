/*
 * LY: Deferred TRAP-sending abilities. Allow us to send thru a queue,
 * especialy for ARP-resolving.
 * Developed by vich@cronyx.ru, placed here by ly@cronyx.ru
 */

extern small_uint_t trap_defer_delay_ds;

bool_t trap_defer_init (small_uint_t max_delay_ds /* LY: in deciseconds */,
	small_uint_t links_count, small_uint_t ports_count);
small_int_t trap_defer_poll (unsigned short since_ms);
void trap_defer_cold_start (unsigned short reset_counter);
void trap_defer_auth_failure (void);
void trap_defer_link (bool_t up, small_uint_t link_index, small_uint_t link_status);
void trap_defer_port (bool_t up, small_uint_t port_index, small_uint_t port_status);
void trap_defer_alarm (bool_t alarm);

extern bool_t send_start_trap (unsigned short counter);
extern bool_t send_alarm_trap (bool_t alarm);
extern bool_t send_link_trap (bool_t up, small_uint_t link_index, small_uint_t link_status);
extern bool_t send_port_trap (bool_t up, small_uint_t port_index, small_uint_t port_status);
extern bool_t send_auth_trap (unsigned long user_ipv4addr);
