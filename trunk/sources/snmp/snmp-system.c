#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <net/ip.h>
#include <snmp/asn.h>
#include <snmp/snmp.h>
#include <snmp/snmp-var.h>
#include <snmp/snmp-system.h>

SYSTEM2_VARIABLE_LIST

asn_t *snmp_get_sysDescr (snmp_t *snmp, ...)
{
	return asn_make_string_flash (snmp->pool, snmp->sys_descr);
}

asn_t *snmp_get_sysObjectID (snmp_t *snmp, ...)
{
	return asn_make_oid (snmp->pool, snmp->sys_object_id);
}

asn_t *snmp_get_sysUpTime (snmp_t *snmp, ...)
{
	unsigned long santisec = 0;

	if (snmp->ip->timer)
		santisec = timer_milliseconds (snmp->ip->timer) / 10 +
			timer_days (snmp->ip->timer) * 24L * 60 * 60 * 100;
	return asn_make_int (snmp->pool, santisec, ASN_TIME_TICKS);
}

asn_t *snmp_get_sysContact (snmp_t *snmp, ...)
{
	return asn_make_string (snmp->pool, snmp->sys_contact);
}

small_uint_t
snmp_set_sysContact (snmp_t *snmp, asn_t *val, ...)
{
	if (val->type != ASN_STRING || val->string.len <= 0
	|| val->string.len > sizeof (snmp->sys_contact) - 1)
		return SNMP_BAD_VALUE;

	strncpy (snmp->sys_contact, val->string.str, sizeof (snmp->sys_contact) - 1);

	if (snmp_change_sysContact && ! snmp_change_sysContact (snmp))
		return SNMP_GEN_ERR;
	return SNMP_NO_ERROR;
}

asn_t *snmp_get_sysName (snmp_t *snmp, ...)
{
	return asn_make_string (snmp->pool, snmp->sys_name);
}

small_uint_t
snmp_set_sysName (snmp_t *snmp, asn_t *val, ...)
{
	if (val->type != ASN_STRING || val->string.len <= 0
	|| val->string.len > sizeof (snmp->sys_name) - 1)
		return SNMP_BAD_VALUE;


	strncpy (snmp->sys_name, val->string.str, sizeof (snmp->sys_name) - 1);

	if (snmp_change_sysName && ! snmp_change_sysName (snmp))
		return SNMP_GEN_ERR;
	return SNMP_NO_ERROR;
}

asn_t *snmp_get_sysLocation (snmp_t *snmp, ...)
{
	return asn_make_string (snmp->pool, snmp->sys_location);
}

small_uint_t
snmp_set_sysLocation (snmp_t *snmp, asn_t *val, ...)
{
	if (val->type != ASN_STRING || val->string.len <= 0
	|| val->string.len > sizeof (snmp->sys_location) - 1)
		return SNMP_BAD_VALUE;

	strncpy (snmp->sys_location, val->string.str, sizeof (snmp->sys_location) - 1);

	if (snmp_change_sysLocation && ! snmp_change_sysLocation (snmp))
		return SNMP_GEN_ERR;
	return SNMP_NO_ERROR;
}

asn_t *snmp_get_sysServices (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->sys_services, ASN_INTEGER);
}

asn_t *snmp_get_sysORLastChange (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, 0, ASN_TIME_TICKS);
}

asn_t *
snmp_get_sysORID (snmp_t *snmp, unsigned id, ...)
{
	if (id != 1)
		return 0;
	return asn_make_oid (snmp->pool, snmp->sys_resource_id);
}

asn_t *
snmp_next_sysORID (snmp_t *snmp, bool_t nextflag, unsigned *id, ...)
{
	if (nextflag)
		++*id;
	else
		*id = 1;
	if (*id != 1)
		return 0;
	return asn_make_oid (snmp->pool, snmp->sys_resource_id);
}

asn_t *
snmp_get_sysORDescr (snmp_t *snmp, unsigned id, ...)
{
	if (id != 1)
		return 0;
	return asn_make_string_flash (snmp->pool, snmp->sys_resource_descr);
}

asn_t *
snmp_next_sysORDescr (snmp_t *snmp, bool_t nextflag, unsigned *id, ...)
{
	if (nextflag)
		++*id;
	else
		*id = 1;
	return snmp_get_sysORDescr (snmp, *id);
}

asn_t *
snmp_get_sysORUpTime (snmp_t *snmp, unsigned id, ...)
{
	if (id != 1)
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_TIME_TICKS);
}

asn_t *
snmp_next_sysORUpTime (snmp_t *snmp, bool_t nextflag, unsigned *id, ...)
{
	if (nextflag)
		++*id;
	else
		*id = 1;
	if (*id != 1)
		return 0;
	return asn_make_int (snmp->pool, 0, ASN_TIME_TICKS);
}
