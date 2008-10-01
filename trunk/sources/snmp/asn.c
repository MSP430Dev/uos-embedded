#ifndef TEST_ASN
#	include <runtime/lib.h>
#	include <kernel/uos.h>
#	include <mem/mem.h>
#else
#	include <stdlib.h>
#	include <string.h>
#	include <stdarg.h>
#	include <stdio.h>
	typedef struct _mem_pool_t {} mem_pool_t;
#	define mem_alloc(pool,bytes) calloc(1,bytes)
#	define mem_realloc(block,bytes) realloc(block,bytes)
#	define mem_free(block) free(block)
#	define mem_pool(block) 0
#	define debug_putchar(s,c) putchar(c)
#	define debug_printf printf
	static inline unsigned char flash_fetch (const char *p) {return *p;}
#endif /* TEST_ASN */
#include <snmp/asn.h>

asn_t asn_null = {
	.type = ASN_NULL
};

/*
 * Read length value.
 * Return 1 when ok, or 0 on failure.
 */
static bool_t
asn_get_length (unsigned char **input, unsigned *insz,
	unsigned *val)
{
	unsigned char n;

	/* Reserved value, not used. */
	if (*insz <= 0 || (n = *(*input)++) == 0xff)
		return 0;
	--*insz;

	/* Short definite form. */
	if (! (n & 0x80)) {
		*val = n;
		return 1;
	}

	/* Long definite form. */
	n &= 0x7f;
	if (n > 4 || *insz < n)
		return 0;
	*insz -= n;
	*val = 0;
	while (n-- > 0)
		*val = (*val << 8) | *(*input)++;
	return 1;
}

/*
 * Read integer value.
 * Return 0 on failure.
 */
static asn_t *
asn_get_int (mem_pool_t *pool, unsigned char *input, unsigned sz)
{
	asn_t *b;

	if (sz > 4)
		return 0;

	b = (asn_t*) mem_alloc (pool, sizeof (asn_t));
	if (! b)
		return 0;
	b->type = ASN_INTEGER;

	if (sz <= 0) {
		b->int32.val = 0;
		return b;
	}
	b->int32.val = (signed char) *input++;
	while (--sz > 0)
		b->int32.val = (b->int32.val << 8) | *input++;
	return b;
}

/*
 * Read unsigned integer value.
 * Return 0 on failure.
 */
static asn_t *
asn_get_uint (mem_pool_t *pool, unsigned char *input, unsigned sz,
	small_uint_t type)
{
	asn_t *b;

	if (sz > 4)
		return 0;

	b = (asn_t*) mem_alloc (pool, sizeof (asn_t));
	if (! b)
		return 0;
	b->type = type;

	b->uint32.val = 0;
	while (sz-- > 0)
		b->uint32.val = (b->uint32.val << 8) | *input++;
	return b;
}

/*
 * Read string value.
 * Return 0 on failure.
 */
static asn_t *
asn_get_string (mem_pool_t *pool, unsigned char *input, unsigned sz,
	small_uint_t type)
{
	asn_t *b;

	b = (asn_t*) mem_alloc (pool, SIZEOF_STRING (sz + 1));
	if (! b)
		return 0;
	b->type = type;

	b->string.len = sz;
	if (sz)
		memcpy (b->string.str, input, sz);
	b->string.str[sz] = 0;
	return b;
}

/*
 * Read OID value.
 * Return 0 on failure.
 */
static asn_t *
asn_get_oid (mem_pool_t *pool, unsigned char *input, unsigned sz)
{
	asn_t *b;
	unsigned short *id;

	b = (asn_t*) mem_alloc (pool, SIZEOF_OID (sz ? (sz+1) : 10));
	if (! b)
		return 0;
	b->type = ASN_OID;
	b->oid.len = 0;
	if (! sz)
		return b;

	id = b->oid.id;
	*id++ = *input / 40;
	*id++ = *input % 40;
	b->oid.len += 2;
	++input;
	--sz;

	for (; sz-- > 0; ++id, ++b->oid.len) {
		*id = 0;
		while (*input & 0x80) {
			if (sz-- <= 0) {
				mem_free (b);
				return 0;
			}
			*id = (*id | (*input++ & 0x7f)) << 7;
		}
		*id |= *input++;
	}
	return b;
}

static asn_t *
asn_get_sequence (mem_pool_t *pool, unsigned char *input, unsigned sz, small_uint_t type)
{
	asn_t *b;
	unsigned nelem = 5;

	b = (asn_t*) mem_alloc (pool, SIZEOF_SEQUENCE (nelem));
	if (! b)
		return 0;
	b->type = type;

	for (b->seq.count=0; sz>0; ++b->seq.count) {
		if (b->seq.count >= nelem) {
			nelem += 10;
			b = (asn_t*) mem_realloc (b, SIZEOF_SEQUENCE (nelem));
			if (! b)
				return 0;
		}
		b->seq.arr[b->seq.count] = asn_parse (pool, &input, &sz);
		if (! b->seq.arr[b->seq.count]) {
			unsigned i;

			for (i=0; i<b->seq.count; ++i)
				asn_free (b->seq.arr[i]);
			mem_free (b);
			return 0;
		}
	}
	return b;
}

/*
 * Parse the byte array and build the ASN.1 data structure.
 */
asn_t *
asn_parse (mem_pool_t *pool, unsigned char **input, unsigned *insz)
{
	unsigned char *inp, type;
	unsigned sz, len = 0;
	asn_t *b = 0;

	sz = *insz;
	if (sz < 2)
		return 0;
	inp = *input;
	type = *inp++;
	--sz;

	if (! asn_get_length (&inp, &sz, &len))
		return 0;
	if (len > sz)
		return 0;
#if 0
	asn_print_type (type);
	debug_printf (CONST(" %d bytes of %d: %02x"), len, sz, inp[0]);
	{
		unsigned i;

		for (i=1; i<len; ++i)
			debug_printf (CONST("-%02x"), inp[i]);
		if (len < sz) {
			debug_printf (CONST("\nRest: %02x"), inp[len]);
			for (i=len+1; i<sz; ++i)
				debug_printf (CONST("-%02x"), inp[i]);
		}
	}
	debug_putchar (0, '\n');
#endif
	switch (type) {
	default:
		if (! (type & ASN_CONSTRUCTED))
			break;
		b = asn_get_sequence (pool, inp, len, type);
		break;

	case ASN_NULL:
		b = &asn_null;
		break;
	case ASN_NOSUCHOBJECT:
	case ASN_NOSUCHINSTANCE:
	case ASN_ENDOFMIBVIEW:
		b = (asn_t*) mem_alloc (pool, sizeof (asn_t));
		if (b)
			b->type = type;
		break;

	case ASN_INTEGER:
		b = asn_get_int (pool, inp, len);
		break;

	case ASN_COUNTER:
	case ASN_GAUGE:
	case ASN_TIME_TICKS:
		b = asn_get_uint (pool, inp, len, type);
		break;

	case ASN_IP_ADDRESS:
		if (len != 4)
			break;
		b = asn_get_uint (pool, inp, len, type);
		break;

	case ASN_STRING:
	case ASN_OPAQUE:
		b = asn_get_string (pool, inp, len, type);
		break;

	case ASN_OID:
		b = asn_get_oid (pool, inp, len);
		break;
	}
	if (! b)
		return 0;
	*input = inp + len;
	*insz = sz - len;
	return b;
}

void
asn_free (asn_t *b)
{
	unsigned i;

	if (! b || b == &asn_null)
		return;
	if (b->type & ASN_CONSTRUCTED) {
		for (i = 0; i < b->seq.count; ++i)
			asn_free (b->seq.arr[i]);
	}
	mem_free (b);
}

static unsigned char *
asn_put_int (long val, unsigned char *p, unsigned sz)
{
	if (val < 0x80 && val >= -0x80) {
		if (sz < 3)
			return 0;
		*--p = val;
		*--p = 1;
	} else if (val < 0x8000L && val >= -0x8000L) {
		if (sz < 4)
			return 0;
		*--p = val;
		*--p = val >> 8;
		*--p = 2;
	} else if (val < 0x800000L && val >= -0x800000L) {
		if (sz < 5)
			return 0;
		*--p = val;
		*--p = val >> 8;
		*--p = val >> 16;
		*--p = 3;
	} else {
		if (sz < 6)
			return 0;
		*--p = val;
		*--p = val >> 8;
		*--p = val >> 16;
		*--p = val >> 24;
		*--p = 4;
	}
	*--p = ASN_INTEGER;
	return p;
}

static unsigned char *
asn_put_uint (unsigned long val, unsigned char *p, unsigned sz,
	small_uint_t type)
{
#if 1
	 /* LY: "хитрая" версия, пишем минимальное кол-во байт. */
	if (type == ASN_IP_ADDRESS || val > 0x7FFFFFul) {
		if (sz < 6)
			return 0;
		*--p = val;
		*--p = val >> 8;
		*--p = val >> 16;
		*--p = val >> 24;
		*--p = 4;
	} else if (val <= 0x7Fu) {
		if (sz < 3)
			return 0;
		*--p = val;
		*--p = 1;
	} else if (val <= 0x7FFFul) {
		if (sz < 4)
			return 0;
		*--p = val;
		*--p = val >> 8;
		*--p = 2;
	} else {
		if (sz < 5)
			return 0;
		*--p = val;
		*--p = val >> 8;
		*--p = val >> 16;
		*--p = 3;
	}
#else
	/* LY: "глупая" версия, всегда записываем 4 байта. */
	if (sz < 6)
		return 0;
	*--p = val;
	*--p = val >> 8;
	*--p = val >> 16;
	*--p = val >> 24;
	*--p = 4;
#endif
	*--p = type;
	return p;
}

static unsigned char *
asn_put_length (unsigned long val, unsigned char *p, unsigned sz)
{
	if (val < 0x80) {
		if (sz < 1)
			return 0;
		*--p = val;
	} else if (val < 0x100) {
		if (sz < 2)
			return 0;
		*--p = val;
		*--p = 0x81;
	} else if (val < 0x10000L) {
		if (sz < 3)
			return 0;
		*--p = val;
		*--p = val >> 8;
		*--p = 0x82;
	} else if (val < 0x1000000L) {
		if (sz < 4)
			return 0;
		*--p = val;
		*--p = val >> 8;
		*--p = val >> 16;
		*--p = 0x83;
	} else {
		if (sz < 5)
			return 0;
		*--p = val;
		*--p = val >> 8;
		*--p = val >> 16;
		*--p = val >> 24;
		*--p = 0x84;
	}
	return p;
}

static unsigned char *
asn_put_string (string_t *v, unsigned char *p, unsigned sz)
{
	unsigned char *op;

	if (sz < v->len)
		return 0;
	p -= v->len;
	sz -= v->len;
	if (v->len)
		memcpy (p, v->str, v->len);

	p = asn_put_length (v->len, op = p, sz);
	if (! p)
		return 0;

	sz -= (op - p);
	if (sz < 1)
		return 0;
	*--p = ASN_STRING;
	return p;
}

static unsigned char *
asn_put_id1 (unsigned long val, unsigned char *p, unsigned sz)
{
	if (sz-- < 1)
		return 0;
	*--p = val & 0x7f;
	while (val >= 0x80) {
		if (sz-- < 1)
			return 0;
		val >>= 7;
		*--p = 0x80 | (val & 0x7f);
	}
	return p;
}

static unsigned char *
asn_put_oid (oid_t *v, unsigned char *p, unsigned sz)
{
	unsigned char *op, *bp;
	unsigned i, len;

	bp = p;
	for (i=v->len; --i>=2; ) {
		p = asn_put_id1 (v->id[i], op = p, sz);
		if (! p)
			return 0;
		sz -= (op - p);
	}

	if (sz-- < 1)
		return 0;
	*--p = v->id[0] * 40 + v->id[1];
	len = bp - p;

	p = asn_put_length (len, op = p, sz);
	if (! p)
		return 0;

	sz -= (op - p);
	if (sz < 1)
		return 0;
	*--p = ASN_OID;
	return p;
}

static unsigned char *
asn_put_sequence (sequence_t *seq, unsigned char *p, unsigned sz,
	small_uint_t type)
{
	unsigned char *op, *bp;
	unsigned i, len;

	bp = p;
	for (i=seq->count; i-->0; ) {
		p = asn_encode (seq->arr[i], op = p, sz);
		if (! p)
			return 0;
		sz -= (op - p);
	}
	len = bp - p;

	p = asn_put_length (len, op = p, sz);
	if (! p)
		return 0;

	sz -= (op - p);
	if (sz < 1)
		return 0;
	*--p = type;
	return p;
}

/*
 * Make a binary representation of the given data structure.
 * The output pointer gives the end of the buffer (just after the last byte).
 * The function should fill the buffer from end to beginning,
 * and return the pointer to the beginning of the final array.
 * Returns 0 on buffer overflow.
 */
unsigned char *
asn_encode (asn_t *b, unsigned char *p, unsigned sz)
{
	small_uint_t type = ASN_NULL;

	if (b)
		type = b->type;
	switch (type) {
	case ASN_NULL:
	case ASN_NOSUCHOBJECT:
	case ASN_NOSUCHINSTANCE:
	case ASN_ENDOFMIBVIEW:
		if (sz < 2)
			return 0;
		*--p = 0;
		*--p = type;
		break;

	case ASN_INTEGER:
		return asn_put_int (b->int32.val, p, sz);

	case ASN_COUNTER:
	case ASN_GAUGE:
	case ASN_TIME_TICKS:
	case ASN_IP_ADDRESS:
		return asn_put_uint (b->uint32.val, p, sz, b->type);

	case ASN_STRING:
	case ASN_OPAQUE:
		return asn_put_string (&b->string, p, sz);

	case ASN_OID:
		return asn_put_oid (&b->oid, p, sz);

	default:
		if (! (b->type & ASN_CONSTRUCTED))
			return 0;
		return asn_put_sequence (&b->seq, p, sz, b->type);
	}
	return p;
}

/*
 * Make an object identifier from the string.
 * Return 0 on failure.
 */
asn_t *
asn_make_oid (mem_pool_t *pool, const char *str)
{
	asn_t *b;
	char c;
	unsigned val;

	b = (asn_t*) mem_alloc (pool, SIZEOF_OID((strlen_flash (str) + 1) / 2));
	if (! b)
		return 0;
	b->type = ASN_OID;
	b->oid.len = 0;
	for (;;) {
		while ((c = flash_fetch(str)) == '.')
			++str;
		if (c < '0' || c > '9')
			break;
		val = c - '0';
		for (;;) {
			c = flash_fetch (++str);
			if (c < '0' || c > '9')
				break;
			val = val*10 + c - '0';
		}
		b->oid.id [b->oid.len++] = val;
	}
	return b;
}

/*
 * Make a string.
 * Return 0 on failure.
 */
asn_t *
asn_make_stringn (mem_pool_t *pool, const unsigned char *str, unsigned len)
{
	asn_t *b;

	b = (asn_t*) mem_alloc (pool, SIZEOF_STRING (len + 1));
	if (! b)
		return 0;
	b->type = ASN_STRING;
	b->string.len = len;
	if (len)
		memcpy (b->string.str, str, len);
	return b;
}

asn_t *
asn_make_string (mem_pool_t *pool, const unsigned char *str)
{
	return asn_make_stringn (pool, str, strlen (str));
}

#ifdef __AVR__
asn_t *
asn_make_string_flash (mem_pool_t *pool, const char *str)
{
	unsigned char len = strlen_flash (str);
	asn_t *b;

	b = (asn_t*) mem_alloc (pool, SIZEOF_STRING (len + 1));
	if (! b)
		return 0;
	b->type = ASN_STRING;
	b->string.len = len;
	if (len)
		memcpy_flash (b->string.str, str, len);

	return b;
}
#endif

/*
 * Make an integer (or unsigned integer).
 * Return 0 on failure.
 */
asn_t *
asn_make_int (mem_pool_t *pool, unsigned long val, small_uint_t type)
{
	asn_t *b;

	b = (asn_t*) mem_alloc (pool, sizeof (asn_t));
	if (! b)
		return 0;
	b->type = type;
	b->uint32.val = val;
	return b;
}

asn_t *
asn_make_seq (mem_pool_t *pool, unsigned size, small_uint_t type)
{
	asn_t *b;

	b = (asn_t*) mem_alloc (pool, SIZEOF_SEQUENCE (size));
	if (! b)
		return 0;
	b->type = type;
	b->seq.count = 0;
	memset (b->seq.arr, 0, size * sizeof (asn_t*));
	return b;
}

asn_t *
asn_make_oidn (mem_pool_t *pool, unsigned enterprise,
	const char *id, unsigned char len, unsigned char cnt, ...)
{
	va_list ap;
	asn_t *b;
	unsigned char i;

	b = (asn_t*) mem_alloc (pool, SIZEOF_OID (len + cnt + 1));
	if (! b)
		return 0;
	b->type = ASN_OID;
	b->oid.len = len + cnt;
	for (i=0; i<len; ++i) {
		b->oid.id [i] = flash_fetch (id + i);

		/* Value 255 is special, it designates the enterprise ID. */
		if (b->oid.id [i] == 255)
			b->oid.id [i] = enterprise;
	}
	va_start (ap, cnt);
	for (i=0; i<cnt; ++i)
		b->oid.id [len+i] = va_arg (ap, unsigned);
	va_end (ap);
	return b;
}

/*
 * Make a copy of the given data structure.
 * Returns 0 on failure.
 */
asn_t *
asn_copy (asn_t *s)
{
	mem_pool_t *pool;
	asn_t *b, *e;
	int i;

	if (! s)
		return 0;
	pool = mem_pool (s);

	switch (s->type) {
	case ASN_NULL:
		b = &asn_null;
		break;
	case ASN_NOSUCHOBJECT:
	case ASN_NOSUCHINSTANCE:
	case ASN_ENDOFMIBVIEW:
	case ASN_INTEGER:
	case ASN_COUNTER:
	case ASN_GAUGE:
	case ASN_TIME_TICKS:
	case ASN_IP_ADDRESS:
		b = (asn_t*) mem_alloc (pool, sizeof (asn_t));
		if (! b)
			return 0;
		*b = *s;
		break;

	case ASN_STRING:
	case ASN_OPAQUE:
		b = (asn_t*) mem_alloc (pool,
			SIZEOF_STRING (s->string.len + 1));
		if (! b)
			return 0;
		memcpy (b, s, SIZEOF_STRING (s->string.len + 1));
		break;

	case ASN_OID:
		b = (asn_t*) mem_alloc (pool, SIZEOF_OID (s->oid.len));
		if (! b)
			return 0;
		memcpy (b, s, SIZEOF_OID (s->oid.len));
		break;

	default:
		if (! (s->type & ASN_CONSTRUCTED))
			return 0;
		b = (asn_t*) mem_alloc (pool, SIZEOF_SEQUENCE (s->seq.count));
		if (! b)
			return 0;
		*b = *s;
		for (i=0; i<s->seq.count; ++i) {
			e = asn_copy (s->seq.arr[i]);
			if (! e) {
				for (--i; i>=0; --i)
					asn_free (b->seq.arr[i]);
				mem_free (b);
				return 0;
			}
			b->seq.arr[i] = e;
		}
		break;
	}
	return b;
}

#ifdef TEST_ASN
asn_t *make_request (int n)
{
	asn_t *b, *pdu, *tab, *var;
	int i;

	/* Create list of N vars. */
	tab = asn_make_seq (0, n, ASN_SEQUENCE);
	tab->seq.count = n;
	for (i=0; i<n; ++i) {
		var = asn_make_seq (0, 2, ASN_SEQUENCE);
		var->seq.count = 2;
		var->seq.arr[0] = asn_make_oid (0, ".1.3.6.1.2.1.1.3.0");
		var->seq.arr[1] = asn_make_int (0, 0, ASN_NULL); /* Value */
		tab->seq.arr[i] = var;
	}

	/* Create packet data unit. */
	pdu = asn_make_seq (0, 4, ASN_GET_REQUEST);
	pdu->seq.count = 4;
	pdu->seq.arr[0] = asn_make_int (0, 123, ASN_INTEGER); /* id */
	pdu->seq.arr[1] = asn_make_int (0, 0, ASN_INTEGER); /* Error code */
	pdu->seq.arr[2] = asn_make_int (0, 0, ASN_INTEGER); /* Error index */
	pdu->seq.arr[3] = tab;

	/* Create SNMP request. */
	b = asn_make_seq (0, 3, ASN_SEQUENCE);
	b->seq.count = 3;
	b->seq.arr[0] = asn_make_int (0, 0, ASN_INTEGER); /* SNMP version */
	b->seq.arr[1] = asn_make_string (0, "public"); /* Community */
	b->seq.arr[2] = pdu;
	return b;
}

int main ()
{
	unsigned char outbuf [2000], *output;
	asn_t *b;
	int n, maxsize, len;

	for (;;) {
		maxsize = 10 + random() % (sizeof(outbuf) - 10);
		for (n=1; n<200; ++n) {
			b = make_request (n);
			memset (outbuf, 'Z', sizeof (outbuf));
			output = asn_encode (b, outbuf + sizeof (outbuf) - 1,
				maxsize);
			if (! output) {
				printf ("Cannot encode %d vars to %d bytes ",
					n, maxsize);
				/*asn_print (b, 1);*/
				printf ("\n");
				asn_free (b);
				break;
			}
			len = outbuf + sizeof (outbuf) - output;
			if (output[-1] != 'Z') {
				printf ("Encode %d vars to %d bytes - using %d bytes\n",
					n, maxsize, len);
				printf ("CRASH at index %d\n",
					output-1-outbuf);
				exit (1);
			}
			if (outbuf[sizeof(outbuf)-1] != 'Z') {
				printf ("Encode %d vars to %d bytes - using %d bytes\n",
					n, maxsize, len);
				printf ("CRASH at index %d\n",
					sizeof(outbuf)-1);
				exit (1);
			}
			if (output[0] == 'Z') {
				printf ("Encode %d vars to %d bytes - using %d bytes\n",
					n, maxsize, len);
				printf ("UNUSED byte at index %d\n",
					output-outbuf);
				exit (1);
			}
			if (outbuf[sizeof(outbuf)-2] == 'Z') {
				printf ("Encode %d vars to %d bytes - using %d bytes\n",
					n, maxsize, len);
				printf ("UNUSED byte at index %d\n",
					sizeof(outbuf)-2);
				exit (1);
			}
			/*printf ("%d vars - %d bytes\n", n, len);*/
			asn_free (b);
		}
	}
	return 0;
}
#endif
