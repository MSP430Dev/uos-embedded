#include <runtime/lib.h>
#include <snmp/asn.h>
#include <snmp/snmp.h>
#include <snmp/snmp-var.h>
#include <snmp/snmp-snmp.h>

SNMP_VARIABLE_LIST

asn_t *snmp_get_snmpInPkts (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_pkts, ASN_COUNTER);
}

asn_t *snmp_get_snmpOutPkts (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->out_pkts, ASN_COUNTER);
}

asn_t *snmp_get_snmpInBadVersions (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_bad_versions, ASN_COUNTER);
}

asn_t *snmp_get_snmpInBadCommunityNames (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_bad_community_names, ASN_COUNTER);
}

asn_t *snmp_get_snmpInBadCommunityUses (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_bad_community_uses, ASN_COUNTER);
}

asn_t *snmp_get_snmpInASNParseErrs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_asn_parse_errs, ASN_COUNTER);
}

asn_t *snmp_get_snmpInTooBigs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_too_bigs, ASN_COUNTER);
}

asn_t *snmp_get_snmpInNoSuchNames (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_no_such_names, ASN_COUNTER);
}

asn_t *snmp_get_snmpInBadValues (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_bad_values, ASN_COUNTER);
}

asn_t *snmp_get_snmpInReadOnlys (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_read_onlys, ASN_COUNTER);
}

asn_t *snmp_get_snmpInGenErrs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_gen_errs, ASN_COUNTER);
}

asn_t *snmp_get_snmpInTotalReqVars (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_total_req_vars, ASN_COUNTER);
}

asn_t *snmp_get_snmpInTotalSetVars (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_total_set_vars, ASN_COUNTER);
}

asn_t *snmp_get_snmpInGetRequests (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_get_requests, ASN_COUNTER);
}

asn_t *snmp_get_snmpInGetNexts (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_get_nexts, ASN_COUNTER);
}

asn_t *snmp_get_snmpInSetRequests (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_set_requests, ASN_COUNTER);
}

asn_t *snmp_get_snmpInGetResponses (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_get_responses, ASN_COUNTER);
}

asn_t *snmp_get_snmpInTraps (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->in_traps, ASN_COUNTER);
}

asn_t *snmp_get_snmpOutTooBigs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->out_too_bigs, ASN_COUNTER);
}

asn_t *snmp_get_snmpOutNoSuchNames (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->out_no_such_names, ASN_COUNTER);
}

asn_t *snmp_get_snmpOutBadValues (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->out_bad_values, ASN_COUNTER);
}

asn_t *snmp_get_snmpOutGenErrs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->out_gen_errs, ASN_COUNTER);
}

asn_t *snmp_get_snmpOutGetRequests (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->out_get_requests, ASN_COUNTER);
}

asn_t *snmp_get_snmpOutGetNexts (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->out_get_nexts, ASN_COUNTER);
}

asn_t *snmp_get_snmpOutSetRequests (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->out_set_requests, ASN_COUNTER);
}

asn_t *snmp_get_snmpOutGetResponses (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->out_get_responses, ASN_COUNTER);
}

asn_t *snmp_get_snmpOutTraps (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->out_traps, ASN_COUNTER);
}

asn_t *snmp_get_snmpEnableAuthenTraps (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->enable_authen_traps ? 1 : 2,
		ASN_INTEGER);
}

uint_t
snmp_set_snmpEnableAuthenTraps (snmp_t *snmp, asn_t *val, ...)
{
	if (val->type == ASN_INTEGER) {
		switch (val->int32.val) {
		case 1:
			snmp->enable_authen_traps = 1;
			return SNMP_NO_ERROR;
		case 2:
			snmp->enable_authen_traps = 0;
			return SNMP_NO_ERROR;
		}
	}
	return SNMP_BAD_VALUE;
}
