#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <snmp/asn.h>

void
asn_print_type (small_uint_t type)
{
	switch (type) {
	case ASN_INTEGER:          debug_printf ("Integer");          break;
	case ASN_STRING:           debug_printf ("String");           break;
	case ASN_NULL:             debug_printf ("Null");             break;
	case ASN_NOSUCHOBJECT:     debug_printf ("NoSuchObject");     break;
	case ASN_NOSUCHINSTANCE:   debug_printf ("NoSuchInstance");   break;
	case ASN_ENDOFMIBVIEW:     debug_printf ("EndOfMibView");     break;
	case ASN_OID:              debug_printf ("Oid");              break;
	case ASN_IP_ADDRESS:       debug_printf ("IpAddress");        break;
	case ASN_COUNTER:          debug_printf ("Counter");          break;
	case ASN_GAUGE:            debug_printf ("Gauge");            break;
	case ASN_TIME_TICKS:       debug_printf ("TimeTicks");        break;
	case ASN_OPAQUE:           debug_printf ("Opaque");           break;
	case ASN_SEQUENCE:         debug_printf ("Sequence");         break;
	case ASN_GET_REQUEST:      debug_printf ("Get-Request");      break;
	case ASN_GET_NEXT_REQUEST: debug_printf ("Get-Next-Request"); break;
	case ASN_GET_RESPONSE:     debug_printf ("Response");         break;
	case ASN_SET_REQUEST:      debug_printf ("Set-Request");      break;
	case ASN_TRAP:             debug_printf ("Trap");             break;
	default:                   debug_printf ("[0x%x]", type);     break;
	}
}

void
asn_print (asn_t *b, unsigned off)
{
	unsigned i, n;
	unsigned long ul;

	switch (b->type) {
	default:
		asn_print_type (b->type);
		if (! (b->type & ASN_CONSTRUCTED))
			break;
		for (i=0; i<b->seq.count; ++i) {
			debug_putchar (0, '\n');
			for (n=0; n<off; ++n)
				debug_printf ("    ");
			asn_print (b->seq.arr[i], off+1);
		}
		break;

	case ASN_NULL:
		debug_printf ("(null)");
		break;

	case ASN_NOSUCHOBJECT:
		debug_printf ("(noSuchObject)");
		break;

	case ASN_NOSUCHINSTANCE:
		debug_printf ("(noSuchInstance)");
		break;

	case ASN_ENDOFMIBVIEW:
		debug_printf ("(endOfMibView)");
		break;

	case ASN_INTEGER:
		debug_printf ("%ld", b->int32.val);
		break;

	case ASN_COUNTER:
		debug_printf ("c'%lu", b->uint32.val);
		break;

	case ASN_GAUGE:
		debug_printf ("g'%lu", b->uint32.val);
		break;

	case ASN_TIME_TICKS:
		debug_printf ("t'%lu", b->uint32.val);
		break;

	case ASN_IP_ADDRESS:
		ul = b->uint32.val;
		debug_printf ("%d.%d.%d.%d",
			(unsigned char) (ul >> 24), (unsigned char) (ul >> 16),
			(unsigned char) (ul >> 8), (unsigned char) ul);
		break;

	case ASN_OPAQUE:
		debug_printf ("<");
		if (b->string.len > 0)
			debug_printf ("%x", b->string.str[0]);
		else
			debug_printf ("null-opaque");
		for (i=1; i<b->string.len; ++i)
			debug_printf ("-%x", b->string.str[i]);
		debug_printf (">");
		break;

	case ASN_STRING:
		if (b->string.len <= 0) {
			debug_printf ("null-string");
			break;
		}
		debug_printf ("\"");
		for (i=0; i<b->string.len; ++i)
			debug_printf ("%c", b->string.str[i]);
		debug_printf ("\"");
		break;

	case ASN_OID:
		if (b->string.len <= 0)
			debug_printf ("null-oid");
		for (i=0; i<b->oid.len; ++i)
			debug_printf (".%d", b->oid.id[i]);
		break;
	}
}
