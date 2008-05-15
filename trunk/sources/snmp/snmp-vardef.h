#undef VAR_NAME
#undef READONLY_VARIABLE
#undef READWRITE_VARIABLE
#undef READONLY_TABLE_1
#undef READONLY_TABLE_111
#undef READWRITE_TABLE_1
#undef READONLY_TABLE_1S
#undef READWRITE_TABLE_1S
#undef READONLY_TABLE_4
#undef READWRITE_TABLE_4
#undef READONLY_TABLE_41
#undef READONLY_TABLE_14
#undef READWRITE_TABLE_14
#undef READONLY_TABLE_4141
#undef READWRITE_TABLE_4141

/*
 * Initialize the variable table.
 */
#define VAR_NAME(n)		/* empty */
#define VAR_ENTRY(n,s)          { snmp_##n, sizeof(I##n), SNMP_VAR, 0,\
				  (snmp_get_t*)snmp_get_##n,\
				  0,\
				  (snmp_set_t*)s },
#define TAB_ENTRY(n,t,a,s)	{ snmp_##n, sizeof(I##n)-1, t, a,\
				  (snmp_get_t*)snmp_get_##n,\
				  (snmp_next_t*)snmp_next_##n,\
				  (snmp_set_t*)s },
#define T1S_ENTRY(n,s)		{ snmp_##n, sizeof(I##n)-1, SNMP_TAB1, 1,\
				  (snmp_get_t*)snmp_get_##n,\
				  0,\
				  (snmp_set_t*)s },

#define READONLY_VARIABLE(n)    VAR_ENTRY (n,			0)
#define READWRITE_VARIABLE(n)   VAR_ENTRY (n,			snmp_set_##n)
#define READONLY_TABLE_1(n)     TAB_ENTRY (n, SNMP_TAB1,     1, 0)
#define READWRITE_TABLE_1(n)    TAB_ENTRY (n, SNMP_TAB1,     1, snmp_set_##n)
#define READONLY_TABLE_111(n)   TAB_ENTRY (n, SNMP_TAB111,   3, 0)
#define READONLY_TABLE_1S(n)    T1S_ENTRY (n,			0)
#define READWRITE_TABLE_1S(n)   T1S_ENTRY (n,			snmp_set_##n)
#define READONLY_TABLE_4(n)     TAB_ENTRY (n, SNMP_TAB4,     4, 0)
#define READWRITE_TABLE_4(n)    TAB_ENTRY (n, SNMP_TAB4,     4, snmp_set_##n)
#define READONLY_TABLE_41(n)    TAB_ENTRY (n, SNMP_TAB41,    5, 0)
#define READONLY_TABLE_14(n)    TAB_ENTRY (n, SNMP_TAB14,    5, 0)
#define READWRITE_TABLE_14(n)   TAB_ENTRY (n, SNMP_TAB14,    5, snmp_set_##n)
#define READONLY_TABLE_4141(n)  TAB_ENTRY (n, SNMP_TAB4141, 10, 0)
#define READWRITE_TABLE_4141(n) TAB_ENTRY (n, SNMP_TAB4141, 10, snmp_set_##n)
