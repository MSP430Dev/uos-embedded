/*
 * the SNMP group
 *
 * Implementation of the SNMP group is mandatory for all
 * systems which support an SNMP protocol entity.  Some of
 * the objects defined below will be zero-valued in those
 * SNMP implementations that are optimized to support only
 * those functions specific to either a management agent or
 * a management station.  In particular, it should be
 * observed that the objects below refer to an SNMP entity,
 * and there may be several SNMP entities residing on a
 * managed node (e.g., if the node is hosting acting as
 * a management station).
 */

/*
 * "The total number of Messages delivered to the
 * SNMP entity from the transport service."
 */
#define IsnmpInPkts Isnmp "\1"          /* ::= { snmp 1 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP Messages which were
 * passed from the SNMP protocol entity to the
 * transport service."
 */
#define IsnmpOutPkts Isnmp "\2"         /* ::= { snmp 2 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP Messages which were
 * delivered to the SNMP protocol entity and were for
 * an unsupported SNMP version."
 */
#define IsnmpInBadVersions Isnmp "\3"   /* ::= { snmp 3 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP Messages delivered to
 * the SNMP protocol entity which used a SNMP
 * community name not known to said entity."
 */
#define IsnmpInBadCommunityNames Isnmp "\4" /* ::= { snmp 1 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP Messages delivered to
 * the SNMP protocol entity which represented an SNMP
 * operation which was not allowed by the SNMP
 * community named in the Message."
 */
#define IsnmpInBadCommunityUses Isnmp "\5" /* ::= { snmp 5 } */
					/* read-only, mandatory */

/*
 * "The total number of ASN.1 or BER errors
 * encountered by the SNMP protocol entity when
 * decoding received SNMP Messages."
 */
#define IsnmpInASNParseErrs Isnmp "\6"  /* ::= { snmp 6 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP PDUs which were
 * delivered to the SNMP protocol entity and for
 * which the value of the error-status field is
 * `tooBig'."
 */
#define IsnmpInTooBigs Isnmp "\10"      /* ::= { snmp 8 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP PDUs which were
 * delivered to the SNMP protocol entity and for
 * which the value of the error-status field is
 * `noSuchName'."
 */
#define IsnmpInNoSuchNames Isnmp "\11"  /* ::= { snmp 9 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP PDUs which were
 * delivered to the SNMP protocol entity and for
 * which the value of the error-status field is
 * `badValue'."
 */
#define IsnmpInBadValues Isnmp "\12"    /* ::= { snmp 10 } */
					/* read-only, mandatory */

/*
 * "The total number valid SNMP PDUs which were
 * delivered to the SNMP protocol entity and for
 * which the value of the error-status field is
 * `readOnly'.  It should be noted that it is a
 * protocol error to generate an SNMP PDU which
 * contains the value `readOnly' in the error-status
 * field, as such this object is provided as a means
 * of detecting incorrect implementations of the
 * SNMP."
 */
#define IsnmpInReadOnlys Isnmp "\13"    /* ::= { snmp 11 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP PDUs which were
 * delivered to the SNMP protocol entity and for
 * which the value of the error-status field is
 * `genErr'."
 */
#define IsnmpInGenErrs Isnmp "\14"      /* ::= { snmp 12 } */
					/* read-only, mandatory */

/*
 * "The total number of MIB objects which have been
 * retrieved successfully by the SNMP protocol entity
 * as the result of receiving valid SNMP Get-Request
 * and Get-Next PDUs."
 */
#define IsnmpInTotalReqVars Isnmp "\15" /* ::= { snmp 13 } */
					/* read-only, mandatory */

/*
 * "The total number of MIB objects which have been
 * altered successfully by the SNMP protocol entity
 * as the result of receiving valid SNMP Set-Request
 * PDUs."
 */
#define IsnmpInTotalSetVars Isnmp "\16" /* ::= { snmp 14 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP Get-Request PDUs which
 * have been accepted and processed by the SNMP
 * protocol entity."
 */
#define IsnmpInGetRequests Isnmp "\17"  /* ::= { snmp 15 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP Get-Next PDUs which have
 * been accepted and processed by the SNMP protocol
 * entity."
 */
#define IsnmpInGetNexts Isnmp "\20"     /* ::= { snmp 16 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP Set-Request PDUs which
 * have been accepted and processed by the SNMP
 * protocol entity."
 */
#define IsnmpInSetRequests Isnmp "\21"  /* ::= { snmp 17 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP Get-Response PDUs which
 * have been accepted and processed by the SNMP
 * protocol entity."
 */
#define IsnmpInGetResponses Isnmp "\22" /* ::= { snmp 18 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP Trap PDUs which have
 * been accepted and processed by the SNMP protocol
 * entity."
 */
#define IsnmpInTraps Isnmp "\23"        /* ::= { snmp 19 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP PDUs which were
 * generated by the SNMP protocol entity and for
 * which the value of the error-status field is
 * `tooBig.'"
 */
#define IsnmpOutTooBigs Isnmp "\24"     /* ::= { snmp 20 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP PDUs which were
 * generated by the SNMP protocol entity and for
 * which the value of the error-status is
 * `noSuchName'."
 */
#define IsnmpOutNoSuchNames Isnmp "\25" /* ::= { snmp 21 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP PDUs which were
 * generated by the SNMP protocol entity and for
 * which the value of the error-status field is
 * `badValue'."
 */
#define IsnmpOutBadValues Isnmp "\26"   /* ::= { snmp 22 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP PDUs which were
 * generated by the SNMP protocol entity and for
 * which the value of the error-status field is
 * `genErr'."
 */
#define IsnmpOutGenErrs Isnmp "\30"     /* ::= { snmp 24 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP Get-Request PDUs which
 * have been generated by the SNMP protocol entity."
 */
#define IsnmpOutGetRequests Isnmp "\31" /* ::= { snmp 25 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP Get-Next PDUs which have
 * been generated by the SNMP protocol entity."
 */
#define IsnmpOutGetNexts Isnmp "\32"    /* ::= { snmp 26 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP Set-Request PDUs which
 * have been generated by the SNMP protocol entity."
 */
#define IsnmpOutSetRequests Isnmp "\33" /* ::= { snmp 27 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP Get-Response PDUs which
 * have been generated by the SNMP protocol entity."
 */
#define IsnmpOutGetResponses Isnmp "\34" /* ::= { snmp 28 } */
					/* read-only, mandatory */

/*
 * "The total number of SNMP Trap PDUs which have
 * been generated by the SNMP protocol entity."
 */
#define IsnmpOutTraps Isnmp "\35"       /* ::= { snmp 29 } */
					/* read-only, mandatory */

/*
 * "Indicates whether the SNMP agent process is
 * permitted to generate authentication-failure
 * traps.  The value of this object overrides any
 * configuration information; as such, it provides a
 * means whereby all authentication-failure traps may
 * be disabled.
 *
 * Note that it is strongly recommended that this
 * object be stored in non-volatile memory so that it
 * remains constant between re-initializations of the
 * network management system."
 */
#define IsnmpEnableAuthenTraps Isnmp "\36" /* ::= { snmp 30 } */
					/* read-write, mandatory */
					/* INTEGER { enabled(1),... */
					/* ...disabled(2) } */

#define SNMP_VARIABLE_LIST\
	READONLY_VARIABLE (snmpInPkts)\
	READONLY_VARIABLE (snmpOutPkts)\
	READONLY_VARIABLE (snmpInBadVersions)\
	READONLY_VARIABLE (snmpInBadCommunityNames)\
	READONLY_VARIABLE (snmpInBadCommunityUses)\
	READONLY_VARIABLE (snmpInASNParseErrs)\
	READONLY_VARIABLE (snmpInTooBigs)\
	READONLY_VARIABLE (snmpInNoSuchNames)\
	READONLY_VARIABLE (snmpInBadValues)\
	READONLY_VARIABLE (snmpInReadOnlys)\
	READONLY_VARIABLE (snmpInGenErrs)\
	READONLY_VARIABLE (snmpInTotalReqVars)\
	READONLY_VARIABLE (snmpInTotalSetVars)\
	READONLY_VARIABLE (snmpInGetRequests)\
	READONLY_VARIABLE (snmpInGetNexts)\
	READONLY_VARIABLE (snmpInSetRequests)\
	READONLY_VARIABLE (snmpInGetResponses)\
	READONLY_VARIABLE (snmpInTraps)\
	READONLY_VARIABLE (snmpOutTooBigs)\
	READONLY_VARIABLE (snmpOutNoSuchNames)\
	READONLY_VARIABLE (snmpOutBadValues)\
	READONLY_VARIABLE (snmpOutGenErrs)\
	READONLY_VARIABLE (snmpOutGetRequests)\
	READONLY_VARIABLE (snmpOutGetNexts)\
	READONLY_VARIABLE (snmpOutSetRequests)\
	READONLY_VARIABLE (snmpOutGetResponses)\
	READONLY_VARIABLE (snmpOutTraps)\
	READWRITE_VARIABLE (snmpEnableAuthenTraps)

/*
 * Some variables are obsolete in SNMP v2.
 */
#define SNMP2_VARIABLE_LIST\
	READONLY_VARIABLE (snmpInPkts)\
	READONLY_VARIABLE (snmpInBadVersions)\
	READONLY_VARIABLE (snmpInBadCommunityNames)\
	READONLY_VARIABLE (snmpInBadCommunityUses)\
	READONLY_VARIABLE (snmpInASNParseErrs)\
	READWRITE_VARIABLE (snmpEnableAuthenTraps)
