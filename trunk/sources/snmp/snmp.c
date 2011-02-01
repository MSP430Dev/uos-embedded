#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <snmp/asn.h>
#include <snmp/snmp.h>

#define SNMP_V1		0
#define SNMP_V2		1

static small_int_t
compare (const snmp_t *snmp,
	const unsigned short *a_id, small_uint_t a_idlen,
	const char *b_id, small_uint_t b_idlen)
{
	small_uint_t i, len;

#if 0
	debug_printf ("compare: ");
	for (i=0; i<a_idlen; ++i)
		debug_printf (".%d", a_id[i]);
	debug_printf (" with ");
	for (i=0; i<b_idlen; ++i)
		debug_printf (".%d", FETCH_BYTE (b_id + i));
#endif
	len = a_idlen < b_idlen ? a_idlen : b_idlen;
	for (i=0; i<len; ++i) {
		unsigned x = a_id[i];
		unsigned y = FETCH_BYTE (b_id + i);

		/* Value 255 is special, it designates the enterprise ID. */
		if (y == 255)
			y = snmp->enterprise;

		if (x < y)
			goto less;
		if (x > y)
			goto greater;
	}
	if (a_idlen < b_idlen) {
less:
/*		debug_printf (" --> LESS\n");*/
		return -1;
	}
	if (a_idlen > b_idlen) {
greater:
/*		debug_printf (" --> GREATER\n");*/
		return 1;
	}
/*	debug_printf (" --> EQUAL\n");*/
	return 0;
}

/*
 * Find the name in the variables table, which
 * has the same name or immediately preceeding name.
 */
const static snmp_var_t *
find_var (snmp_t *snmp, const asn_t *name, bool_t *exact)
{
	const snmp_var_t *base, *vp;
	unsigned lim;

	*exact = 0;
/*	debug_printf ("find_var called\n");*/
	base = snmp->tab;
	lim = snmp->tab_len;
	for (;;) {
		vp = base + lim/2;
/*		debug_printf ("find_var: lim = %d\n", lim);*/
		switch (compare (snmp, name->oid.id, name->oid.len,
			(const char*) FETCH_PTR (&vp->id), FETCH_BYTE (&vp->idlen))) {
		case 0:
			*exact = 1;
found:
#if 0
			debug_printf ("find_var: returned ");
			{
			unsigned char i;
			for (i=0; i<v.idlen; ++i)
				debug_printf (".%d", FETCH_BYTE (vp.id+i));
			}
			debug_putchar (0, '\n');
#endif
			return vp;

		default:
			/* id < vp: move left */
			if (lim == 1) {
				if (vp == snmp->tab) {
/*					debug_printf ("find_var: failed\n");*/
					return 0;
				}
				--vp;
				goto found;
			}
			lim /= 2;
			continue;

		case 1:
			/* id > p: move right */
			lim -= (vp - base) + 1;
			if (lim == 0)
				goto found;
			base = vp + 1;
			continue;
		}
	}
}

/*
 * Perform a GET or SET request on the single variable.
 * Id contains the variable identifier.
 * The function should return the error code as NO_ERROR,
 * NO_SUCH_NAME or BAD_VALUE.
 * The retrieved value should be placed into val.
 */
static small_uint_t
get_set (snmp_t *snmp, const asn_t *name,
	asn_t **val, bool_t setflag)
{
	const snmp_var_t *vp;
	snmp_var_t v;
	bool_t exact;
	unsigned long i1, i2, i3, a1, a2;
#define LONG(p)         ((unsigned long)p << 24 | \
			 (unsigned long)(&p)[1] << 16 | \
			 (unsigned long)(&p)[2] << 8 | (&p)[3])

/*	debug_printf (setflag ? "snmp_set\n" : "snmp_get\n");*/

	/* Find the name in the variables table, which
	 * has the same name or immediately preceeding name. */
	vp = find_var (snmp, name, &exact);
	if (! vp)
		return SNMP_NO_SUCH_NAME;
	memcpy_flash (&v, (const char*) vp, sizeof (v));

	if (setflag) {
		/* vch: проверим есть ли функция установки переменной */
	 	if (! v.set) {
			/* debug_printf ("no v.set\n");*/
			return SNMP_READ_ONLY;
		}
	} else {
		*val = 0;
		/* vch: проверим есть ли функция чтения переменной */
		if (! v.get) {
			/* debug_printf ("no v.get\n");*/
			*val = asn_make_null (snmp->pool);
			return SNMP_NO_ERROR;
		}
	}
	if (exact) {
		if (v.nargs)
			return SNMP_NO_SUCH_NAME;
		if (setflag)
			return (v.set) (snmp, *val);
/*debug_printf ("before v.get, snmp=%p, descr=%s\n", snmp, snmp->sys_descr);*/
		*val = (*v.get) (snmp);
/*debug_printf ("v.get returned %p\n", *val);*/
		return (*val ? SNMP_NO_ERROR : SNMP_NO_SUCH_NAME);
	}

	if (v.idlen + v.nargs != name->oid.len)
		return SNMP_NO_SUCH_NAME;

	switch (v.type) {
	case SNMP_TAB1:
		i1 = name->oid.id [v.idlen];
		if (setflag)
			return (*v.set) (snmp, *val, i1);
		*val = (*v.get) (snmp, i1);
		break;
	case SNMP_TAB4:
		a1 = LONG (name->oid.id [v.idlen]);
		if (setflag)
			return (*v.set) (snmp, *val, a1);
		*val = (*v.get) (snmp, a1);
		break;
	case SNMP_TAB41:
		a1 = LONG (name->oid.id [v.idlen]);
		i1 = name->oid.id [v.idlen + 4];
		if (setflag)
			return (*v.set) (snmp, *val, a1, i1);
		*val = (*v.get) (snmp, a1, i1);
		break;
	case SNMP_TAB14:
		i1 = name->oid.id [v.idlen];
		a1 = LONG (name->oid.id [v.idlen + 1]);
		if (setflag)
			return (*v.set) (snmp, *val, i1, a1);
		*val = (*v.get) (snmp, i1, a1);
		break;
	case SNMP_TAB4141:
		a1 = LONG (name->oid.id [v.idlen]);
		i1 = name->oid.id [v.idlen + 4];
		a2 = LONG (name->oid.id [v.idlen + 5]);
		i2 = name->oid.id [v.idlen + 9];
		if (setflag)
			return (*v.set) (snmp, *val, a1, i1, a2, i2);
		*val = (*v.get) (snmp, a1, i1, a2, i2);
		break;
	case SNMP_TAB111:
		i1 = name->oid.id [v.idlen];
		i2 = name->oid.id [v.idlen + 1];
		i3 = name->oid.id [v.idlen + 2];
		if (setflag)
			return (*v.set) (snmp, *val, i1, i2, i3);
		*val = (*v.get) (snmp, i1, i2, i3);
		break;
	}
	return (*val ? SNMP_NO_ERROR : SNMP_NO_SUCH_NAME);
}

static unsigned long
load_req_idx (unsigned short *req_id,
	unsigned req_idlen, unsigned var_idlen)
{
	unsigned long idx = 0;
	if (req_idlen > var_idlen)
		idx = (unsigned long) req_id [var_idlen] << 24;
	if (req_idlen > var_idlen+1)
		idx |= (unsigned long) req_id [var_idlen+1] << 16;
	if (req_idlen > var_idlen+2)
		idx |= (unsigned long) req_id [var_idlen+2] << 8;
	if (req_idlen > var_idlen+3)
		idx |= (unsigned char) req_id [var_idlen+3];
	return idx;
}

/*
 * Perform a GETNEXT request on the single variable.
 * Id contains the variable identifier.
 * The function should return the error code as NO_ERROR or NO_SUCH_NAME.
 * The retrieved name/value should be placed into name/val.
 */
static small_uint_t
next (snmp_t *snmp, asn_t **name, asn_t **val)
{
	const snmp_var_t *vp;
	bool_t nextflag, exact;
	unsigned long a1, a2;
	unsigned i1, i2, i3, id_len = (*name)->oid.len;
	unsigned short *id = (*name)->oid.id;

	nextflag = i1 = i2 = a1 = a2 = 0;

	/* Find the name in the variables table, which
	 * has the same or immediately preceeding name. */
	vp = find_var (snmp, *name, &exact);
	if (! vp) {
		/* The first table entry must have no args. */
		vp = snmp->tab;
	} else {
		const char* vid = FETCH_PTR (&vp->id);
		small_uint_t vlen = FETCH_BYTE (&vp->idlen);
		unsigned min_len = vlen < id_len ? vlen : id_len;

		if (! FETCH_BYTE (&vp->nargs) ||
		    compare (snmp, id, min_len, vid, min_len) != 0) {
			++vp;
		} else if (vlen < id_len &&
		    compare (snmp, id, vlen, vid, vlen) == 0) {
			/* Extract arguments. */
			nextflag = 1;
			switch (FETCH_BYTE (&vp->type)) {
			case SNMP_TAB1:
				if (id_len > vlen)
					i1 = id [vlen];
				break;

			case SNMP_TAB4:
				a1 = load_req_idx (id, id_len, vlen);
				break;

			case SNMP_TAB41:
				a1 = load_req_idx (id, id_len, vlen);
				if (id_len > vlen+4)
					i1 = id [vlen+4];
				break;

			case SNMP_TAB14:
				if (id_len > vlen)
					i1 = id [vlen];
				a1 = load_req_idx (id, id_len, vlen+1);
				break;

			case SNMP_TAB4141:
				a1 = load_req_idx (id, id_len, vlen);
				if (id_len > vlen+4)
					i1 = id [vlen+4];
				a2 = load_req_idx (id, id_len, vlen+5);
				if (id_len > vlen+9)
					i2 = id [vlen+9];
				break;

			case SNMP_TAB111:
				if (id_len > vlen)
					i1 = id [vlen];
				if (id_len > vlen+1)
					i2 = id [vlen+1];
				if (id_len > vlen+2)
					i3 = id [vlen+2];
				break;
			}
		}
	}
	for (*val=0; !*val && vp < snmp->tab+snmp->tab_len; ++vp) {
		snmp_var_t v; memcpy_flash (&v, (const char*) vp, sizeof (v));
		switch (v.type) {
		default:
		case SNMP_VAR:
			if (v.get)
				*val = (*v.get) (snmp);
			else
				*val = asn_make_null (snmp->pool);
			if (*val)
				*name = asn_make_oidn (snmp->pool, snmp->enterprise,
					v.id, v.idlen, 0);
			break;
		case SNMP_TAB1:
			if (v.next)
				*val = (*v.next) (snmp, nextflag, &i1);
			else {
				/* Simple table: all entries are numbered
				 * continuously starting from 0. */
				if (nextflag)
					++i1;
				else
					i1 = 0;
				if (v.get)
					*val = (*v.get) (snmp, i1);
				else
					*val = asn_make_null (snmp->pool);
			}
			if (*val)
				*name = asn_make_oidn (snmp->pool, snmp->enterprise,
					v.id, v.idlen, 1, i1);
			break;
		case SNMP_TAB4:
			if (v.next) {
				*val = (*v.next) (snmp, nextflag, &a1);
				if (*val)
					*name = asn_make_oidn (snmp->pool, snmp->enterprise,
						v.id, v.idlen, 4,
						(unsigned char) (a1>>24), (unsigned char) (a1>>16),
						(unsigned char) (a1>>8), (unsigned char) a1);
			}
			break;
		case SNMP_TAB41:
			if (v.next) {
				*val = (*v.next) (snmp, nextflag, &a1, &i1);
				if (*val)
					*name = asn_make_oidn (snmp->pool, snmp->enterprise,
						v.id, v.idlen, 5,
						(unsigned char) (a1>>24), (unsigned char) (a1>>16),
						(unsigned char) (a1>>8), (unsigned char) a1, i1);
			}
			break;
		case SNMP_TAB14:
			if (v.next) {
				*val = (*v.next) (snmp, nextflag, &i1, &a1);
				if (*val)
					*name = asn_make_oidn (snmp->pool, snmp->enterprise,
						v.id, v.idlen, 5, i1,
						(unsigned char) (a1>>24), (unsigned char) (a1>>16),
						(unsigned char) (a1>>8), (unsigned char) a1);
			}
			break;
		case SNMP_TAB4141:
			if (v.next) {
				*val = (*v.next) (snmp, nextflag, &a1, &i1, &a2, &i2);
				if (*val)
					*name = asn_make_oidn (snmp->pool, snmp->enterprise,
						v.id, v.idlen, 10,
						(unsigned char) (a1>>24), (unsigned char) (a1>>16),
						(unsigned char) (a1>>8), (unsigned char) a1,
						i1,
						(unsigned char) (a2>>24), (unsigned char) (a2>>16),
						(unsigned char) (a2>>8), (unsigned char) a2,
						i2);
			}
			break;
		case SNMP_TAB111:
			if (v.next) {
				*val = (*v.next) (snmp, nextflag, &i1, &i2, &i3);
				if (*val)
					*name = asn_make_oidn (snmp->pool, snmp->enterprise,
						v.id, v.idlen, 3,
						i1, i2, i3);
			}
			break;
		}
		if (*val && !*name) {
			asn_free (*val);
			*val = 0;
		}
		nextflag = i1 = i2 = a1 = a2 = 0;
	}
	return (*val ? SNMP_NO_ERROR : SNMP_NO_SUCH_NAME);
}

static unsigned long
fetch_addr (unsigned char *addr)
{
	return *(long*) addr;
}

/*
 * Check the client access privileges.
 * Return 1 on success, 0 on failure.
 */
static bool_t
snmp_auth (snmp_t *snmp, unsigned char *community, bool_t setflag)
{
	unsigned long *cnt = &snmp->in_bad_community_names;
	unsigned long addr;

	if (strncmp (community, snmp->set_community,
	    sizeof (snmp->set_community)) == 0) {
		addr = fetch_addr (snmp->user_addr) ^ fetch_addr (snmp->set_addr);
		if ((addr & fetch_addr (snmp->set_mask)) == 0)
			return 1;
		cnt = &snmp->in_bad_community_uses;
	}
	if (! setflag && strncmp (community, snmp->get_community,
	    sizeof (snmp->get_community)) == 0) {
		addr = fetch_addr (snmp->user_addr) ^ fetch_addr (snmp->get_addr);
		if ((addr & fetch_addr (snmp->get_mask)) == 0)
			return 1;
		cnt = &snmp->in_bad_community_uses;
	}
	++*cnt;
	return 0;
}

/*
 * Add name/value pair to the list of variable bindings.
 * Return 1 on success, 0 on failure.
 */
static bool_t
add_pair (asn_t *outbind, asn_t *name, asn_t *val)
{
	asn_t *pair;

	if (! name || ! val)
		return 0;

	pair = asn_make_seq (mem_pool (outbind), 2, ASN_SEQUENCE);
	if (! pair)
		return 0;
	pair->seq.count = 2;
	pair->seq.arr[0] = name;
	pair->seq.arr[1] = val;

	outbind->seq.arr [outbind->seq.count++] = pair;
	return 1;
}

unsigned char *
snmp_execute (snmp_t *snmp, unsigned char *input, unsigned insz,
	unsigned char *outbuf, unsigned outsz)
{
	asn_t *b, *pdu, *inbind, *outbind = 0;
	unsigned char *community, *output;
	small_int_t error_code;
	unsigned i, nonrepeaters, repetitions;

	/* debug_printf ("snmp_execute: received %d bytes, source %d.%d.%d.%d\n",
		insz, snmp->user_addr[0], snmp->user_addr[1], snmp->user_addr[2],
		snmp->user_addr[3]); */

	++snmp->in_pkts;
	snmp->auth_failure = 0;
	b = asn_parse (snmp->pool, &input, &insz);
	if (! b) {
		++snmp->in_asn_parse_errs;
err:            /*debug_printf ("snmp_execute: bad pdu\n");*/
		asn_free (outbind);
		asn_free (b);
		return 0;
	}
/*	debug_puts ("snmp request: "); asn_print (b, 0); debug_putchar (0, '\n');*/

	if (b->type != ASN_SEQUENCE || b->seq.count != 3 ||
	    b->seq.arr[0]->type != ASN_INTEGER ||
	    b->seq.arr[1]->type != ASN_STRING) {
		/*debug_printf ("snmp_execute: bad packet\n");*/
		++snmp->in_asn_parse_errs;
		goto err;
	}
	community = b->seq.arr[1]->string.str;
	pdu = b->seq.arr[2];

	if (b->seq.arr[0]->int32.val != SNMP_V1 &&
	    b->seq.arr[0]->int32.val != SNMP_V2) {
		++snmp->in_bad_versions;
		/*debug_printf ("snmp_execute: invalid SNMP version `%d'\n",
			b->seq.arr[0]->int32.val);*/
		asn_free (b);
		return 0;
	}
	if (! snmp_auth (snmp, community, pdu->type == ASN_SET_REQUEST)) {
		/*debug_printf ("snmp_execute: no permission to %S for community `%s'\n",
			pdu->type == ASN_SET_REQUEST ? "configure" : "read",
			community);*/
		snmp->auth_failure = 1;
		asn_free (b);
		return 0;
	}
	if (pdu->seq.count != 4 ||
	    pdu->seq.arr[0]->type != ASN_INTEGER ||
	    pdu->seq.arr[1]->type != ASN_INTEGER ||
	    pdu->seq.arr[2]->type != ASN_INTEGER ||
	    pdu->seq.arr[3]->type != ASN_SEQUENCE) {
		/*debug_printf ("snmp_execute: bad request, community=`%s'\n",
			community);*/
		++snmp->in_asn_parse_errs;
		goto err;
	}
	inbind = pdu->seq.arr[3];
	nonrepeaters = inbind->seq.count;
	repetitions = 0;
	switch (pdu->type) {
	case ASN_GET_REQUEST:
		++snmp->in_get_requests;
		break;
	case ASN_GET_NEXT_REQUEST:
		++snmp->in_get_nexts;
		break;
	case ASN_GET_BULK_REQUEST:
		++snmp->in_get_bulks;
		if (pdu->seq.arr[1]->int32.val < 0)
			nonrepeaters = 0;
		else if (pdu->seq.arr[1]->int32.val > inbind->seq.count)
			nonrepeaters = inbind->seq.count;
		else
			nonrepeaters = pdu->seq.arr[1]->int32.val;
		if (pdu->seq.arr[2]->int32.val < 0)
			repetitions = 0;
		else
			repetitions = pdu->seq.arr[2]->int32.val;
		if (nonrepeaters == 0 && repetitions == 0)
			nonrepeaters = inbind->seq.count;
		/*debug_printf ("snmp_execute: nonrepeaters=%d, repetitions=%d\n",
			nonrepeaters, repetitions);*/
		break;
	case ASN_SET_REQUEST:
		++snmp->in_set_requests;
		break;
	default:
		/*debug_printf ("snmp_execute: bad request type = 0x%02x\n",
			pdu->type);*/
		++snmp->in_asn_parse_errs;
		goto err;
	}
	/* Clear error status and index. */
	pdu->seq.arr[1]->int32.val = 0;
	pdu->seq.arr[2]->int32.val = 0;
	outbind = asn_make_seq (snmp->pool, nonrepeaters +
		(inbind->seq.count - nonrepeaters) * repetitions, ASN_SEQUENCE);
	if (! outbind) {
		/*debug_printf ("snmp_execute: no memory for table\n");*/
		++snmp->out_too_bigs;
		goto err;
	}

	/* Process all variables one by one. */
	for (i=0; i<nonrepeaters; ++i) {
		asn_t *pair, *name, *val;

		pair = inbind->seq.arr[i];
		name = pair->seq.arr[0];
		val = pair->seq.arr[1];
		/*debug_printf ("snmp_execute: processing ");
		asn_print (pair, 0); debug_putchar (0, '\n');*/

		/* Check the correctness of name/value pair. */
		if (pair->type != ASN_SEQUENCE || pair->seq.count < 2 ||
		    name->type != ASN_OID) {
			/*debug_printf ("snmp_execute: bad name/value pair\n");*/
			++snmp->in_asn_parse_errs;
			goto err;
		}
		switch (pdu->type) {
		default:
			/*debug_printf ("snmp_execute: bad pdu type\n");*/
			++snmp->in_asn_parse_errs;
			goto err;

		case ASN_GET_REQUEST:
			/* Get the variable value. */
			error_code = get_set (snmp, name, &val, 0);
			++snmp->in_total_req_vars;
			if (error_code != SNMP_NO_ERROR)
				goto fatal;
			name = asn_copy (name);
			if (! add_pair (outbind, name, val)) {
				error_code = SNMP_GEN_ERR;
				goto fatal;
			}
			break;

		case ASN_SET_REQUEST:
			/* Set the variable value. */
			error_code = get_set (snmp, name, &val, 1);
			++snmp->in_total_set_vars;
			if (error_code != SNMP_NO_ERROR)
				goto fatal;
			name = asn_copy (name);
			val = asn_copy (pair->seq.arr[1]);
			if (! add_pair (outbind, name, val)) {
				error_code = SNMP_GEN_ERR;
				goto fatal;
			}
			break;

		case ASN_GET_NEXT_REQUEST:
		case ASN_GET_BULK_REQUEST:
			/* Get the next variable value. */
			error_code = next (snmp, &name, &val);
			++snmp->in_total_req_vars;
			if (error_code == SNMP_NO_SUCH_NAME &&
			    b->seq.arr[0]->int32.val != SNMP_V1) {
				name = asn_copy (name);
				val = asn_make_int (snmp->pool, 0,
					ASN_ENDOFMIBVIEW);
			} else if (error_code != SNMP_NO_ERROR)
				goto fatal;
			if (! add_pair (outbind, name, val)) {
				error_code = SNMP_GEN_ERR;
				goto fatal;
			}
			break;
		}
		/*debug_printf ("snmp_execute: done ");
		asn_print (name, 0); debug_putchar (0, ' ');
		asn_print (val, 0); debug_putchar (0, '\n');*/
	}
	if (pdu->type == ASN_GET_BULK_REQUEST &&
	    b->seq.arr[0]->int32.val != SNMP_V1) {
		/* Repeat variables. */
		int r;

		for (r=0; r<repetitions; ++r) {
			for (i=nonrepeaters; i<inbind->seq.count; ++i) {
				asn_t *pair, *name, *val;

				if (r == 0)
					pair = inbind->seq.arr [i];
				else
					pair = outbind->seq.arr [i +
						(r - 1) * (inbind->seq.count -
						nonrepeaters)];
				name = pair->seq.arr[0];
				val = pair->seq.arr[1];
				/*debug_printf ("bulk %d: processing ", repetitions);
				asn_print (pair, 0); debug_putchar (0, '\n');*/

				error_code = next (snmp, &name, &val);
				++snmp->in_total_req_vars;

				if (error_code == SNMP_NO_SUCH_NAME) {
					name = asn_copy (name);
					val = asn_make_int (snmp->pool, 0,
						ASN_ENDOFMIBVIEW);
				} else if (error_code != SNMP_NO_ERROR)
					goto fatal;

				if (! add_pair (outbind, name, val)) {
					error_code = SNMP_GEN_ERR;
					goto fatal;
				}
				/*debug_printf ("bulk %d: done ", repetitions);
				asn_print (name, 0); debug_putchar (0, ' ');
				asn_print (val, 0); debug_putchar (0, '\n');*/
			}
		}
	}

	/* Try to make a response packet. */
	pdu->type = ASN_GET_RESPONSE;
	pdu->seq.arr[3] = outbind;
	/*debug_puts ("snmp reply: "); asn_print (b, 0); debug_putchar (0, '\n');*/
	output = asn_encode (b, outbuf + outsz, outsz);
	if (output) {
		++snmp->out_get_responses;
		asn_free (inbind);
	} else {
		/* Error - package too big. */
		/*debug_printf ("snmp_execute: reply too big\n");*/
		++snmp->out_too_bigs;
		pdu->seq.arr[1]->int32.val = SNMP_TOO_BIG;

		/* Return back the old values. */
		pdu->seq.arr[3] = inbind;
		asn_free (outbind);
/*		asn_print (b, 0); debug_putchar (0, '\n');*/
		output = asn_encode (b, outbuf + outsz, outsz);
	}
ret:
	asn_free (b);

	++snmp->out_pkts;
	return output;

fatal:
	/* On error should free all values got thus far. */
	asn_free (outbind);

	/* There are two possible errors here: NO_SUCH_NAME and BAD_VALUE.
	 * Return the original packet with error/error_index set. */
	switch (error_code) {
	case SNMP_NO_SUCH_NAME:
/*		debug_printf ("snmp_execute: no such name ");*/
/*		asn_print ((asn_t*) id, 0); debug_putchar (0, '\n');*/
		++snmp->out_no_such_names;
		break;
	case SNMP_BAD_VALUE:
/*		debug_printf ("snmp_execute: bad value ");*/
/*		asn_print ((asn_t*) id, 0); debug_putchar (0, '\n');*/
		++snmp->out_bad_values;
		break;
	case SNMP_GEN_ERR:
/*		debug_printf ("snmp_execute: gen err ");*/
/*		asn_print ((asn_t*) id, 0); debug_putchar (0, '\n');*/
		++snmp->out_gen_errs;
		break;
	}
	pdu->type = ASN_GET_RESPONSE;
	pdu->seq.arr[1]->int32.val = error_code;
	pdu->seq.arr[2]->int32.val = i + 1;

/*	asn_print (b, 0); debug_putchar (0, '\n');*/
	output = asn_encode (b, outbuf + outsz, outsz);
	goto ret;
}

void
snmp_init (snmp_t *snmp, mem_pool_t *pool, struct _ip_t *ip,
	const snmp_var_t *tab, unsigned tab_size,
	unsigned enterprise, unsigned char services,
	const char *descr, const char *object_id,
	const char *resource_descr, const char *resource_id)
{
	snmp->pool = pool;
	snmp->ip = ip;
	snmp->tab = tab;
	snmp->tab_len = tab_size / sizeof(snmp_var_t);
	snmp->enterprise = enterprise;
	snmp->sys_services = services;
	snmp->sys_descr = descr;
	snmp->sys_object_id = object_id;
	snmp->sys_resource_descr = resource_descr;
	snmp->sys_resource_id = resource_id;

	/* By default, "get" access is permitted for everybody,
	 * with community name "public". */
	snmp->get_community[0] = 'p';
	snmp->get_community[1] = 'u';
	snmp->get_community[2] = 'b';
	snmp->get_community[3] = 'l';
	snmp->get_community[4] = 'i';
	snmp->get_community[5] = 'c';
	snmp->get_community[6] = 0;
}
