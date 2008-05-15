/*
 * RFC1213-MIB DEFINITIONS
 */

/*
 * Basic object identifiers.
 */
#define Iinternet	"\1\3\6\1"	/* internet ::= { iso org(3) dod(6) 1 } */

#define Idirectory	Iinternet "\1"	/* directory	::= { internet 1 } */
#define Imgmt		Iinternet "\2"	/* mgmt		::= { internet 2 } */
#define Iexperimental	Iinternet "\3"	/* experimental	::= { internet 3 } */
#define Iprivate	Iinternet "\4"	/* private	::= { internet 4 } */

#define Ienterprises	Iprivate "\1"	/* enterprises	::= { private 1 } */

#define Imib2           Imgmt "\1"      /* mib-2        ::= { mgmt 1 } */

#define Isystem         Imib2 "\1"      /* system       ::= { mib-2 1 } */
#define Iinterfaces     Imib2 "\2"      /* interfaces   ::= { mib-2 2 } */
#define Iat             Imib2 "\3"      /* at           ::= { mib-2 3 } */
#define Iip             Imib2 "\4"      /* ip           ::= { mib-2 4 } */
#define Iicmp           Imib2 "\5"      /* icmp         ::= { mib-2 5 } */
#define Itcp            Imib2 "\6"      /* tcp          ::= { mib-2 6 } */
#define Iudp            Imib2 "\7"      /* udp          ::= { mib-2 7 } */
#define Iegp            Imib2 "\10"     /* egp          ::= { mib-2 8 } */
#define Itransmission   Imib2 "\12"     /* transmission ::= { mib-2 10 } */
#define Isnmp           Imib2 "\13"     /* snmp         ::= { mib-2 11 } */

#define VAR_NAME(n)		extern const char snmp_##n[]

/*
 * Declare get/set functions.
 */
#define READONLY_VARIABLE(n)    VAR_NAME(n); union _asn_t *snmp_get_##n (snmp_t*, ...);
#define READWRITE_VARIABLE(n)   READONLY_VARIABLE(n)\
				uint_t snmp_set_##n (snmp_t*, union _asn_t*, ...);

#define READONLY_TABLE_1(n)     VAR_NAME(n); union _asn_t *snmp_get_##n (snmp_t*, unsigned, ...);\
				union _asn_t *snmp_next_##n (snmp_t *snmp, bool_t nextflag, unsigned*, ...);
#define READWRITE_TABLE_1(n)    READONLY_TABLE_1(n)\
				uint_t snmp_set_##n (snmp_t*, union _asn_t*, unsigned, ...);

#define READONLY_TABLE_111(n)   VAR_NAME(n); union _asn_t *snmp_get_##n (snmp_t*, unsigned, unsigned, unsigned, ...);\
				union _asn_t *snmp_next_##n (snmp_t *snmp, bool_t nextflag, unsigned*, unsigned*, unsigned*, ...);

#define READONLY_TABLE_1S(n)    VAR_NAME(n); union _asn_t *snmp_get_##n (snmp_t*, unsigned, ...);
#define READWRITE_TABLE_1S(n)   READONLY_TABLE_1S(n)\
				uint_t snmp_set_##n (snmp_t*, union _asn_t*, unsigned, ...);

#define READONLY_TABLE_4(n)     VAR_NAME(n); union _asn_t *snmp_get_##n (snmp_t*, unsigned long, ...);\
				union _asn_t *snmp_next_##n (snmp_t *snmp, bool_t nextflag, unsigned long*, ...);
#define READWRITE_TABLE_4(n)    READONLY_TABLE_4(n)\
				uint_t snmp_set_##n (snmp_t*, union _asn_t*, unsigned long, ...);

#define READONLY_TABLE_41(n)    VAR_NAME(n); union _asn_t *snmp_get_##n (snmp_t*, unsigned long, unsigned, ...);\
				union _asn_t *snmp_next_##n (snmp_t *snmp, bool_t nextflag, unsigned long*, unsigned*, ...);

#define READONLY_TABLE_14(n)    VAR_NAME(n); union _asn_t *snmp_get_##n (snmp_t*, unsigned, unsigned long, ...);\
				union _asn_t *snmp_next_##n (snmp_t *snmp, bool_t nextflag, unsigned*, unsigned long*, ...);
#define READWRITE_TABLE_14(n)   READONLY_TABLE_14(n)\
				uint_t snmp_set_##n (snmp_t*, union _asn_t*, unsigned, unsigned long, ...);

#define READONLY_TABLE_4141(n)  VAR_NAME(n); union _asn_t *snmp_get_##n (snmp_t*, unsigned long, unsigned,\
						unsigned long, unsigned, ...);\
				union _asn_t *snmp_next_##n (snmp_t *snmp, bool_t nextflag, unsigned long*, unsigned*,\
						unsigned long*, unsigned*, ...);
#define READWRITE_TABLE_4141(n) READONLY_TABLE_4141(n)\
				uint_t snmp_set_##n (snmp_t*, union _asn_t*, unsigned long, unsigned,\
						 unsigned long, unsigned, ...);
