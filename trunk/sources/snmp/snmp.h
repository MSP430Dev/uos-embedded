#ifndef __SNMP_H_
#define __SNMP_H_ 1

typedef struct _snmp_t {
	struct _mem_pool_t *pool;
	struct _ip_t	*ip;
	const struct _snmp_var_t *tab;
	unsigned tab_len;

	unsigned enterprise;
	unsigned char	user_addr [4];
	unsigned char	get_community [32];
	unsigned char	set_community [32];
	unsigned char	trap_community [32];
	unsigned char	get_addr [4];
	unsigned char   get_mask [4];
	unsigned char	set_addr [4];
	unsigned char   set_mask [4];
	unsigned char	trap_addr [4];
	unsigned char   get_masklen;
	unsigned char   set_masklen;

	unsigned char	enable_traps;
	unsigned char	auth_failure;
        unsigned char	enable_authen_traps;
	unsigned char	trap_defer_delay_ds;

	unsigned long   in_pkts;
	unsigned long	in_asn_parse_errs;
	unsigned long	in_bad_versions;
	unsigned long	in_bad_community_names;
	unsigned long	in_bad_community_uses;
	unsigned long	in_get_requests;
	unsigned long	in_get_nexts;
	unsigned long	in_get_bulks;
	unsigned long	in_set_requests;
	unsigned long	in_total_req_vars;
	unsigned long	in_total_set_vars;

	unsigned long   in_too_bigs;
	unsigned long   in_no_such_names;
	unsigned long   in_bad_values;
	unsigned long   in_read_onlys;
	unsigned long   in_gen_errs;
	unsigned long   in_get_responses;
	unsigned long   in_traps;

	unsigned long   out_pkts;
	unsigned long	out_no_such_names;
	unsigned long	out_bad_values;
	unsigned long	out_gen_errs;
	unsigned long	out_too_bigs;
	unsigned long	out_get_responses;
	unsigned long   out_traps;

	unsigned long   out_get_requests;
	unsigned long   out_get_nexts;
	unsigned long   out_get_bulks;
	unsigned long   out_set_requests;

	unsigned long   request_id;

	unsigned char	sys_services;
#define SNMP_SERVICE_REPEATER	0x01	/* bit 1 - physical */
#define SNMP_SERVICE_BRIDGE	0x02	/* bit 2 - datalink/subnetwork */
#define SNMP_SERVICE_GATEWAY	0x04	/* bit 3 - internet */
#define SNMP_SERVICE_HOST	0x08	/* bit 4 - end-to-end */
#define SNMP_SERVICE_RELAY	0x40	/* bit 7 - applications */

	const char *sys_descr;
	const char *sys_object_id;
	const char *sys_resource_id;
	const char *sys_resource_descr;
	unsigned char sys_contact [80];
	unsigned char sys_name [80];
	unsigned char sys_location [80];
} snmp_t;

typedef union _asn_t *(snmp_get_t) (snmp_t *snmp, ...);
typedef union _asn_t *(snmp_next_t) (snmp_t *snmp, bool_t nextflag, ...);
typedef uint_t (snmp_set_t) (snmp_t *snmp, union _asn_t *v, ...);

typedef struct _snmp_var_t {
	/* char *name; */			/* variable name */
	const char *id;				/* id as character string */
	unsigned char idlen;			/* char id length */
	unsigned char type;			/* type */
#define SNMP_VAR	0
#define SNMP_TAB1	1
#define SNMP_TAB4	2
#define SNMP_TAB41	3
#define SNMP_TAB14	4
#define SNMP_TAB4141	5
#define SNMP_TAB111	6

	unsigned char nargs;			/* number of args */

	/* functions for get, getnext and set operations */
	snmp_get_t	*get;
	snmp_next_t	*next;
	snmp_set_t	*set;
} snmp_var_t;

/*
 * SNMP error codes
 */
#define	SNMP_NO_ERROR		0
#define	SNMP_TOO_BIG		1
#define	SNMP_NO_SUCH_NAME	2
#define	SNMP_BAD_VALUE		3
#define	SNMP_READ_ONLY		4
#define	SNMP_GEN_ERR		5
#define	SNMP_TIMEOUT	      127	/* for snmp requests (use by RMC2) */

/*
 * Generic-Traps for use with the SNMP
 *
 * ENTERPRISE ::= { mib-2 11 }
 */
#define TRAP_ENTERPRISE ".1.3.6.1.2.1.11"

/*
 * "A coldStart trap signifies that the sending
 * protocol entity is reinitializing itself such
 * that the agent's configuration or the protocol
 * entity implementation may be altered."
 */
#define TRAP_COLD_START 0
#define TRAP2_COLD_START "1.3.6.1.6.3.1.1.5.1"

/*
 * "A warmStart trap signifies that the sending
 * protocol entity is reinitializing itself such
 * that neither the agent configuration nor the
 * protocol entity implementation is altered."
 */
#define TRAP_WARM_START 1
#define TRAP2_WARM_START "1.3.6.1.6.3.1.1.5.2"

/*
 * "A linkDown trap signifies that the sending
 * protocol entity recognizes a failure in one of
 * the communication links represented in the
 * agent's configuration."
 * VARIABLES { ifIndex }
 */
#define TRAP_LINK_DOWN 2

/*
 * "A linkUp trap signifies that the sending
 * protocol entity recognizes that one of the
 * communication links represented in the agent's
 * configuration has come up."
 * VARIABLES { ifIndex }
 */
#define TRAP_LINK_UP 3

/*
 * "An authenticationFailure trap signifies that
 * the sending protocol entity is the addressee
 * of a protocol message that is not properly
 * authenticated.  While implementations of the
 * SNMP must be capable of generating this trap,
 * they must also be capable of suppressing the
 * emission of such traps via an implementation-
 * specific mechanism."
 */
#define TRAP_AUTHENTICATION_FAILURE 4
#define TRAP2_AUTHENTICATION_FAILURE "1.3.6.1.6.3.1.1.5.5"

/*
 * "An egpNeighborLoss trap signifies that an EGP
 * neighbor for whom the sending protocol entity
 * was an EGP peer has been marked down and the
 * peer relationship no longer obtains."
 * VARIABLES { egpNeighAddr }
 */
#define TRAP_EGP_NEIGHBOR_LOSS 5

#define TRAP_ENTERPRISE_SPECIFIC 6

struct _udp_socket_t;

void snmp_init (snmp_t *snmp, struct _mem_pool_t *pool, struct _ip_t *ip,
	const snmp_var_t *tab, unsigned tab_size,
	unsigned enterprise, unsigned char services,
	const char *descr, const char *object_id,
	const char *resource_descr, const char *resource_id);
unsigned char *snmp_execute (snmp_t *snmp, unsigned char *input,
	unsigned insz, unsigned char *output, unsigned outsz);
bool_t snmp_trap_v1 (snmp_t *snmp, struct _udp_socket_t *sock,
	unsigned char *local_ip, unsigned trap_type,
	union _asn_t *oid, union _asn_t *value);
bool_t snmp_trap_v2c (snmp_t *snmp, struct _udp_socket_t *sock,
	const char *trap_type, union _asn_t *oid, union _asn_t *value);

#endif /* __SNMP_H_ */
