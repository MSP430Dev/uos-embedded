/*
 * the System group
 *
 * Implementation of the System group is mandatory for all
 * systems.  If an agent is not configured to have a value
 * for any of these variables, a string of length 0 is
 * returned.
 */

/*
 * "A textual description of the entity.  This value
 * should include the full name and version
 * identification of the system's hardware type,
 * software operating-system, and networking
 * software.  It is mandatory that this only contain
 * printable ASCII characters."
 */
#define IsysDescr Isystem "\1"          /* ::= { system 1 } */
					/* DisplayString (SIZE (0..255)) */
					/* read-only, mandatory */

/*
 * "The vendor's authoritative identification of the
 * network management subsystem contained in the
 * entity.  This value is allocated within the SMI
 * enterprises subtree (1.3.6.1.4.1) and provides an
 * easy and unambiguous means for determining `what
 * kind of box' is being managed.  For example, if
 * vendor `Flintstones, Inc.' was assigned the
 * subtree 1.3.6.1.4.1.4242, it could assign the
 * identifier 1.3.6.1.4.1.4242.1.1 to its `Fred
 * Router'."
 */
#define IsysObjectID Isystem "\2"       /* ::= { system 2 } */
					/* read-only, mandatory */

/*
 * "The time (in hundredths of a second) since the
 * network management portion of the system was last
 * re-initialized."
 */
#define IsysUpTime Isystem "\3"         /* ::= { system 3 } */
					/* read-only, mandatory */

/*
 * "The textual identification of the contact person
 * for this managed node, together with information
 * on how to contact this person."
 */
#define IsysContact Isystem "\4"        /* ::= { system 4 } */
					/* DisplayString (SIZE (0..255)) */
					/* read-write, mandatory */
bool_t (*snmp_change_sysContact) (snmp_t *snmp);

/*
 * "An administratively-assigned name for this
 * managed node.  By convention, this is the node's
 * fully-qualified domain name."
 */
#define IsysName Isystem "\5"           /* ::= { system 5 } */
					/* DisplayString (SIZE (0..255)) */
					/* read-write, mandatory */
bool_t (*snmp_change_sysName) (snmp_t *snmp);

/*
 * "The physical location of this node (e.g.,
 * `telephone closet, 3rd floor')."
 */
#define IsysLocation Isystem "\6"       /* ::= { system 6 } */
					/* DisplayString (SIZE (0..255)) */
					/* read-write, mandatory */
bool_t (*snmp_change_sysLocation) (snmp_t *snmp);

/*
 * "A value which indicates the set of services that
 * this entity primarily offers.
 *
 * The value is a sum.  This sum initially takes the
 * value zero, Then, for each layer, L, in the range
 * 1 through 7, that this node performs transactions
 * for, 2 raised to (L - 1) is added to the sum.  For
 * example, a node which performs primarily routing
 * functions would have a value of 4 (2^(3-1)).  In
 * contrast, a node which is a host offering
 * application services would have a value of 72
 * (2^(4-1) + 2^(7-1)).  Note that in the context of
 * the Internet suite of protocols, values should be
 * calculated accordingly:
 *
 *      layer  functionality
 *          1  physical (e.g., repeaters)
 *          2  datalink/subnetwork (e.g., bridges)
 *          3  internet (e.g., IP gateways)
 *          4  end-to-end  (e.g., IP hosts)
 *          7  applications (e.g., mail relays)
 *
 * For systems including OSI protocols, layers 5 and
 * 6 may also be counted."
 */
#define IsysServices Isystem "\7"       /* ::= { system 7 } */
					/* INTEGER (0..127) read-only, mandatory */

/*
 * Object resource information.
 *
 * A collection of objects which describe the SNMP entity's
 * (statically and dynamically configurable) support of
 * various MIB modules.
 */
/*
 * "The value of sysUpTime at the time of the most recent
 * change in state or value of any instance of sysORID."
 */
#define IsysORLastChange Isystem "\10"	/* ::= { system 8 } */
					/* TimeStamp read-only */

/*
 * "The (conceptual) table listing the capabilities of
 * the local SNMP application acting as a command
 * responder with respect to various MIB modules.
 * SNMP entities having dynamically-configurable support
 * of MIB modules will have a dynamically-varying number
 * of conceptual rows."
 * SEQUENCE OF sysOREntry, not-accessible
 */
#define IsysORTable Isystem "\11"	/* ::= { system 9 } */

/*
 * "An entry (conceptual row) in the sysORTable."
 * INDEX { sysORIndex }
 */
#define IsysOREntry IsysORTable "\1"	/* ::= { sysORTable 1 } */

/*
 * "The auxiliary variable used for identifying instances
 * of the columnar objects in the sysORTable."
 */
#define IsysORIndex IsysOREntry "\1"	/* ::= { sysOREntry 1 } */
					/* INTEGER, not-accessible */

/*
 * "An authoritative identification of a capabilities
 * statement with respect to various MIB modules supported
 * by the local SNMP application acting as a command
 * responder."
 */
#define IsysORID IsysOREntry "\2"	/* ::= { sysOREntry 2 } */
					/* OBJECT IDENTIFIER, read-only */

/*
 * "A textual description of the capabilities identified
 * by the corresponding instance of sysORID."
 */
#define IsysORDescr IsysOREntry "\3"	/* ::= { sysOREntry 3 } */
					/* DisplayString, read-only */

/*
 * "The value of sysUpTime at the time this conceptual
 * row was last instantiated."
 */
#define IsysORUpTime IsysOREntry "\4"	/* ::= { sysOREntry 4 } */
					/* TimeStamp, read-only */

#define SYSTEM_VARIABLE_LIST\
	READONLY_VARIABLE (sysDescr)\
	READONLY_VARIABLE (sysObjectID)\
	READONLY_VARIABLE (sysUpTime)\
	READWRITE_VARIABLE (sysContact)\
	READWRITE_VARIABLE (sysName)\
	READWRITE_VARIABLE (sysLocation)\
	READONLY_VARIABLE (sysServices)

#define SYSTEM2_VARIABLE_LIST\
	READONLY_VARIABLE (sysDescr)\
	READONLY_VARIABLE (sysObjectID)\
	READONLY_VARIABLE (sysUpTime)\
	READWRITE_VARIABLE (sysContact)\
	READWRITE_VARIABLE (sysName)\
	READWRITE_VARIABLE (sysLocation)\
	READONLY_VARIABLE (sysORLastChange)\
	READONLY_TABLE_1 (sysORID)\
	READONLY_TABLE_1 (sysORDescr)\
	READONLY_TABLE_1 (sysORUpTime)
