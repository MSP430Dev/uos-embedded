#include <runtime/lib.h>
#include <kernel/uos.h>
#include <snmp/asn.h>
#include <snmp/snmp.h>
#include <net/ip.h>
#include <snmp/snmp-var.h>
#include <snmp/snmp-icmp.h>

ICMP_VARIABLE_LIST

asn_t *snmp_get_icmpInMsgs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_in_msgs, ASN_COUNTER);
}

asn_t *snmp_get_icmpInErrors (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_in_errors, ASN_COUNTER);
}

asn_t *snmp_get_icmpInDestUnreachs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_in_dest_unreachs, ASN_COUNTER);
}

asn_t *snmp_get_icmpInTimeExcds (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_in_time_excds, ASN_COUNTER);
}

asn_t *snmp_get_icmpInParmProbs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_in_parm_probs, ASN_COUNTER);
}

asn_t *snmp_get_icmpInSrcQuenchs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_in_src_quenchs, ASN_COUNTER);
}

asn_t *snmp_get_icmpInRedirects (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_in_redirects, ASN_COUNTER);
}

asn_t *snmp_get_icmpInEchos (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_in_echos, ASN_COUNTER);
}

asn_t *snmp_get_icmpInEchoReps (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_in_echo_reps, ASN_COUNTER);
}

asn_t *snmp_get_icmpInTimestamps (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_in_timestamps, ASN_COUNTER);
}

asn_t *snmp_get_icmpInTimestampReps (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_in_timestamp_reps, ASN_COUNTER);
}

asn_t *snmp_get_icmpInAddrMasks (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_in_addr_masks, ASN_COUNTER);
}

asn_t *snmp_get_icmpInAddrMaskReps (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_in_addr_mask_reps, ASN_COUNTER);
}

asn_t *snmp_get_icmpOutMsgs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_out_msgs, ASN_COUNTER);
}

asn_t *snmp_get_icmpOutErrors (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_out_errors, ASN_COUNTER);
}

asn_t *snmp_get_icmpOutDestUnreachs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_out_dest_unreachs, ASN_COUNTER);
}

asn_t *snmp_get_icmpOutTimeExcds (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_out_time_excds, ASN_COUNTER);
}

asn_t *snmp_get_icmpOutParmProbs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_out_parm_probs, ASN_COUNTER);
}

asn_t *snmp_get_icmpOutSrcQuenchs (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_out_src_quenchs, ASN_COUNTER);
}

asn_t *snmp_get_icmpOutRedirects (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_out_redirects, ASN_COUNTER);
}

asn_t *snmp_get_icmpOutEchos (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_out_echos, ASN_COUNTER);
}

asn_t *snmp_get_icmpOutEchoReps (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_out_echo_reps, ASN_COUNTER);
}

asn_t *snmp_get_icmpOutTimestamps (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_out_timestamps, ASN_COUNTER);
}

asn_t *snmp_get_icmpOutTimestampReps (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_out_timestamp_reps, ASN_COUNTER);
}

asn_t *snmp_get_icmpOutAddrMasks (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_out_addr_masks, ASN_COUNTER);
}

asn_t *snmp_get_icmpOutAddrMaskReps (snmp_t *snmp, ...)
{
	return asn_make_int (snmp->pool, snmp->ip->icmp_out_addr_mask_reps, ASN_COUNTER);
}
