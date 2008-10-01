#ifndef __ASN_H_
#define __ASN_H_ 1

/*
 * Basic ASN data types.
 */
typedef union _asn_t asn_t;

typedef struct {
	unsigned char	type;
	long		val;
} integer_t;				/* INTEGER */

typedef struct {
	unsigned char	type;
	unsigned long	val;
} uinteger_t;				/* COUNTER */

typedef struct {
	unsigned char	type;
	unsigned short	len;		/* string length */
	unsigned char	str [1];	/* variable length */
} string_t;                             /* OCTET STRING */
#define SIZEOF_STRING(n)	(sizeof(asn_t) + ((n)-1) * sizeof(unsigned char))

typedef struct {
	unsigned char	type;
	unsigned short	len;		/* id length */
	unsigned short	id [1];		/* variable length */
} oid_t;                                /* OBJECT IDENTIFIER */
#define SIZEOF_OID(n)		(sizeof(asn_t) + ((n)-1) * sizeof(unsigned short))

typedef struct {
	unsigned char	type;
	unsigned short	count;		/* number of data elements */
	asn_t *		arr [1];	/* variable length array of elements */
} sequence_t;
#define SIZEOF_SEQUENCE(n)	(sizeof(asn_t) + ((n)-1) * sizeof(asn_t*))

union _asn_t {
	unsigned char	type;
	integer_t	int32;		/* integer */
	uinteger_t	uint32;		/* counter/gauge/timeticks/ipaddress */
	string_t	string;		/* physaddress/opaque */
	oid_t		oid;
	sequence_t	seq;
};

#define ASN_CONSTRUCTED         0x20

/* Universal Class */
#define ASN_INTEGER             0x02
#define ASN_STRING              0x04
#define ASN_NULL                0x05
#define ASN_OID                 0x06
#define ASN_SEQUENCE            0x30

/* Application Class */
#define ASN_IP_ADDRESS          0x40
#define ASN_COUNTER             0x41
#define ASN_GAUGE               0x42
#define ASN_TIME_TICKS          0x43
#define ASN_OPAQUE              0x44

/* Context-Specific Class */
#define ASN_GET_REQUEST		0xa0
#define ASN_GET_NEXT_REQUEST	0xa1
#define ASN_GET_RESPONSE	0xa2
#define ASN_SET_REQUEST		0xa3
#define ASN_TRAP		0xa4
#define ASN_GET_BULK_REQUEST	0xa5
#define ASN_INFORM_REQUEST	0xa6
#define ASN_TRAP_V2		0xa7
#define ASN_REPORT		0xa8

/* SNMPv2 exceptions - RFC 1448. */
#define ASN_NOSUCHOBJECT	0x80
#define ASN_NOSUCHINSTANCE	0x81
#define ASN_ENDOFMIBVIEW	0x82

struct _mem_pool_t;

extern asn_t asn_null;

asn_t *asn_parse (struct _mem_pool_t *pool, unsigned char **input,
	unsigned *sz);

static inline __attribute__((always_inline))
asn_t *asn_make_null (struct _mem_pool_t *pool) {
	return &asn_null;
}

asn_t *asn_make_oid (struct _mem_pool_t *pool, const char *str);
asn_t *asn_make_string (struct _mem_pool_t *pool, const unsigned char *str);
#ifdef __AVR__
	asn_t *asn_make_string_flash (struct _mem_pool_t *pool, const char *str_flash);
#else
#	define asn_make_string_flash(pool, str_flash) \
		asn_make_string (pool, (const unsigned char*) str_flash)
#endif
asn_t *asn_make_stringn (struct _mem_pool_t *pool, const unsigned char *str,
	unsigned len);
asn_t *asn_make_int (struct _mem_pool_t *pool, unsigned long val,
	small_uint_t type);
asn_t *asn_make_seq (struct _mem_pool_t *pool, unsigned size,
	small_uint_t type);
asn_t *asn_make_oidn (struct _mem_pool_t *pool, unsigned enterprise,
	const char *data, unsigned char len, unsigned char cnt, ...);

unsigned char *asn_encode (asn_t*, unsigned char*, unsigned);
void asn_free (asn_t*);

void asn_print (asn_t*, unsigned);
void asn_print_type (small_uint_t);

asn_t *asn_copy (asn_t*);

/* LY: saturated counters, with UNKNOWN value. */
asn_t *asn_make_cnt16 (struct _mem_pool_t *pool, unsigned val);
asn_t *asn_make_cnt32 (struct _mem_pool_t *pool, unsigned long val);

static inline __attribute__((always_inline))
asn_t *asn_make_cnt16_typecheck (struct _mem_pool_t *pool, unsigned short *ptr) {
	return asn_make_cnt16 (pool, *ptr);
}

static inline __attribute__((always_inline))
asn_t *asn_make_cnt32_typecheck (struct _mem_pool_t *pool, unsigned long *ptr) {
	return asn_make_cnt32 (pool, *ptr);
}

/* LY: some mad for MIB-evolition. */
asn_t *asn_make_cnt32as16 (struct _mem_pool_t *pool, unsigned long val);
asn_t *asn_make_cnt16as32 (struct _mem_pool_t *pool, unsigned val);

static inline __attribute__((always_inline))
asn_t *asn_make_cnt32as16_typecheck (struct _mem_pool_t *pool, unsigned long *ptr) {
	return asn_make_cnt32as16 (pool, *ptr);
}

static inline __attribute__((always_inline))
asn_t *asn_make_cnt16as32_typecheck (struct _mem_pool_t *pool, unsigned short *ptr) {
	return asn_make_cnt16as32 (pool, *ptr);
}

#endif /* __ASM_H_ */
