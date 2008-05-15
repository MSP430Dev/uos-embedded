#include <runtime/lib.h>
#include <snmp/asn.h>

asn_t *asn_make_cnt16 (struct _mem_pool_t *pool, unsigned val)
{
	if (val == (unsigned short) -1)
		return asn_make_null (pool);
	return asn_make_int (pool, val, ASN_INTEGER);
}

asn_t *asn_make_cnt32 (struct _mem_pool_t *pool, unsigned long val)
{
	if (val == -1)
		return asn_make_null (pool);
	return asn_make_int (pool, val, ASN_COUNTER);
}

asn_t *asn_make_cnt32as16 (struct _mem_pool_t *pool, unsigned long val)
{
	if (val == -1)
		return asn_make_null (pool);
	if (val > (unsigned short) -2)
		val = (unsigned short) -2;
	return asn_make_int (pool, val, ASN_INTEGER);
}

asn_t *asn_make_cnt16as32 (struct _mem_pool_t *pool, unsigned val)
{
	if (val == (unsigned short) -1)
		return asn_make_null (pool);
	return asn_make_int (pool, val, ASN_COUNTER);
}
