/*
 * the Interfaces group
 *
 * Implementation of the Interfaces group is mandatory for
 * all systems.
 */

/*
 * "The number of network interfaces (regardless of
 * their current state) present on this system."
 */
#define IifNumber Iinterfaces "\1"      /* ::= { interfaces 1 } */
					/* INTEGER, read-only */

/*
 * the Interfaces table
 *
 * The Interfaces table contains information on the entity's
 * interfaces.  Each interface is thought of as being
 * attached to a `subnetwork'.  Note that this term should
 * not be confused with `subnet' which refers to an
 * addressing partitioning scheme used in the Internet suite
 * of protocols.
 */

/*
 * "A list of interface entries.  The number of
 * entries is given by the value of ifNumber."
 * SEQUENCE OF IfEntry, not-accessible
 */
#define IifTable Iinterfaces "\2"       /* ::= { interfaces 2 } */

/*
 * "An interface entry containing objects at the
 * subnetwork layer and below for a particular
 * interface."
 * INDEX { ifIndex }
 */
#define IifEntry IifTable "\1"          /* ::= { ifTable 1 } */

/*
 * "A unique value for each interface.  Its value
 * ranges between 1 and the value of ifNumber.  The
 * value for each interface must remain constant at
 * least from one re-initialization of the entity's
 * network management system to the next re-
 * initialization."
 */
#define IifIndex IifEntry "\1"          /* ::= { ifEntry 1 } */
					/* INTEGER, read-only */

/*
 * "A textual string containing information about the
 * interface.  This string should include the name of
 * the manufacturer, the product name and the version
 * of the hardware interface."
 */
#define IifDescr IifEntry "\2"          /* ::= { ifEntry 2 } */
					/* DisplayString (SIZE (0..255)), read-only */

/*
 * "The type of interface, distinguished according to
 * the physical/link protocol(s) immediately `below'
 * the network layer in the protocol stack."
 */
#define IifType IifEntry "\3"           /* ::= { ifEntry 3 } */
					/* read-only */

/*
 * "The size of the largest datagram which can be
 * sent/received on the interface, specified in
 * octets.  For interfaces that are used for
 * transmitting network datagrams, this is the size
 * of the largest network datagram that can be sent
 * on the interface."
 */
#define IifMtu IifEntry "\4"            /* ::= { ifEntry 4 } */
					/* INTEGER, read-only */

/*
 * "An estimate of the interface's current bandwidth
 * in bits per second.  For interfaces which do not
 * vary in bandwidth or for those where no accurate
 * estimation can be made, this object should contain
 * the nominal bandwidth."
 */
#define IifSpeed IifEntry "\5"          /* ::= { ifEntry 5 } */
					/* Gauge, read-only */

/*
 * "The interface's address at the protocol layer
 * immediately `below' the network layer in the
 * protocol stack.  For interfaces which do not have
 * such an address (e.g., a serial line), this object
 * should contain an octet string of zero length."
 */
#define IifPhysAddress IifEntry "\6"    /* ::= { ifEntry 6 } */
					/* PhysAddress, read-only */

/*
 * "The desired state of the interface.  The
 * testing(3) state indicates that no operational
 * packets can be passed."
 */
#define IifAdminStatus IifEntry "\7"    /* ::= { ifEntry 7 } */
					/* read-write */
#define SNMP_IFS_UP		1	/* ready to pass packets */
#define SNMP_IFS_DOWN		2
#define SNMP_IFS_TESTING	3	/* in some test mode */

/*
 * "The current operational state of the interface.
 * The testing(3) state indicates that no operational
 * packets can be passed."
 */
#define IifOperStatus IifEntry "\10"    /* ::= { ifEntry 8 } */
					/* read-only */

/*
 * "The value of sysUpTime at the time the interface
 * entered its current operational state.  If the
 * current state was entered prior to the last re-
 * initialization of the local network management
 * subsystem, then this object contains a zero
 * value."
 */
#define IifLastChange IifEntry "\11"    /* ::= { ifEntry 9 } */
					/* TimeTicks, read-only */

/*
 * "The total number of octets received on the
 * interface, including framing characters."
 */
#define IifInOctets IifEntry "\12"      /* ::= { ifEntry 10 } */
					/* Counter, read-only */

/*
 * "The number of subnetwork-unicast packets
 * delivered to a higher-layer protocol."
 */
#define IifInUcastPkts IifEntry "\13"   /* ::= { ifEntry 11 } */
					/* Counter, read-only */

/*
 * "The number of non-unicast (i.e., subnetwork-
 * broadcast or subnetwork-multicast) packets
 * delivered to a higher-layer protocol."
 */
#define IifInNUcastPkts IifEntry "\14"  /* ::= { ifEntry 12 } */
					/* Counter, read-only */

/*
 * "The number of inbound packets which were chosen
 * to be discarded even though no errors had been
 * detected to prevent their being deliverable to a
 * higher-layer protocol.  One possible reason for
 * discarding such a packet could be to free up
 * buffer space."
 */
#define IifInDiscards IifEntry "\15"    /* ::= { ifEntry 13 } */
					/* Counter, read-only */

/*
 * "The number of inbound packets that contained
 * errors preventing them from being deliverable to a
 * higher-layer protocol."
 */
#define IifInErrors IifEntry "\16"      /* ::= { ifEntry 14 } */
					/* Counter, read-only */

/*
 * "The number of packets received via the interface
 * which were discarded because of an unknown or
 * unsupported protocol."
 */
#define IifInUnknownProtos IifEntry "\17" /* ::= { ifEntry 15 } */
					/* Counter, read-only */

/*
 * "The total number of octets transmitted out of the
 * interface, including framing characters."
 */
#define IifOutOctets IifEntry "\20"     /* ::= { ifEntry 16 } */
					/* Counter, read-only */

/*
 * "The total number of packets that higher-level
 * protocols requested be transmitted to a
 * subnetwork-unicast address, including those that
 * were discarded or not sent."
 */
#define IifOutUcastPkts IifEntry "\21"  /* ::= { ifEntry 17 } */
					/* Counter, read-only */

/*
 * "The total number of packets that higher-level
 * protocols requested be transmitted to a non-
 * unicast (i.e., a subnetwork-broadcast or
 * subnetwork-multicast) address, including those
 * that were discarded or not sent."
 */
#define IifOutNUcastPkts IifEntry "\22" /* ::= { ifEntry 18 } */
					/* Counter, read-only */

/*
 * "The number of outbound packets which were chosen
 * to be discarded even though no errors had been
 * detected to prevent their being transmitted.  One
 * possible reason for discarding such a packet could
 * be to free up buffer space."
 */
#define IifOutDiscards IifEntry "\23"   /* ::= { ifEntry 19 } */
					/* Counter, read-only */

/*
 * "The number of outbound packets that could not be
 * transmitted because of errors."
 */
#define IifOutErrors IifEntry "\24"     /* ::= { ifEntry 20 } */
					/* Counter, read-only */

/*
 * "The length of the output packet queue (in
 * packets)."
 */
#define IifOutQLen IifEntry "\25"       /* ::= { ifEntry 21 } */
					/* Gauge, read-only */

/*
 * "A reference to MIB definitions specific to the
 * particular media being used to realize the
 * interface.  For example, if the interface is
 * realized by an ethernet, then the value of this
 * object refers to a document defining objects
 * specific to ethernet.  If this information is not
 * present, its value should be set to the OBJECT
 * IDENTIFIER { 0 0 }, which is a syntatically valid
 * object identifier, and any conformant
 * implementation of ASN.1 and BER must be able to
 * generate and recognize this value."
 */
#define IifSpecific IifEntry "\26"      /* ::= { ifEntry 22 } */
					/* OBJECT IDENTIFIER, read-only */

#define IF_VARIABLE_LIST\
	READONLY_VARIABLE (ifNumber)\
	READONLY_TABLE_1 (ifIndex)\
	READONLY_TABLE_1 (ifDescr)\
	READONLY_TABLE_1 (ifType)\
	READONLY_TABLE_1 (ifMtu)\
	READONLY_TABLE_1 (ifSpeed)\
	READONLY_TABLE_1 (ifPhysAddress)\
	READWRITE_TABLE_1 (ifAdminStatus)\
	READONLY_TABLE_1 (ifOperStatus)\
	READONLY_TABLE_1 (ifLastChange)\
	READONLY_TABLE_1 (ifInOctets)\
	READONLY_TABLE_1 (ifInUcastPkts)\
	READONLY_TABLE_1 (ifInNUcastPkts)\
	READONLY_TABLE_1 (ifInDiscards)\
	READONLY_TABLE_1 (ifInErrors)\
	READONLY_TABLE_1 (ifInUnknownProtos)\
	READONLY_TABLE_1 (ifOutOctets)\
	READONLY_TABLE_1 (ifOutUcastPkts)\
	READONLY_TABLE_1 (ifOutNUcastPkts)\
	READONLY_TABLE_1 (ifOutDiscards)\
	READONLY_TABLE_1 (ifOutErrors)\
	READONLY_TABLE_1 (ifOutQLen)\
	READONLY_TABLE_1 (ifSpecific)

/*
 * Some variables are obsolete in SNMP v2.
 * TODO:
 *	ifLinkUpDownTrapEnable
 *	ifConnectorPresent
 *	ifHighSpeed
 *	ifName
 *	ifAlias
 *	ifTableLastChange
 *	ifInMulticastPkts
 *	ifInBroadcastPkts
 *	ifOutMulticastPkts
 *	ifOutBroadcastPkts
 *	ifPromiscuousMode
 */
#define IF2_VARIABLE_LIST\
	READONLY_VARIABLE (ifNumber)\
	READONLY_TABLE_1 (ifDescr)\
	READONLY_TABLE_1 (ifType)\
	READONLY_TABLE_1 (ifMtu)\
	READONLY_TABLE_1 (ifSpeed)\
	READONLY_TABLE_1 (ifPhysAddress)\
	READWRITE_TABLE_1 (ifAdminStatus)\
	READONLY_TABLE_1 (ifOperStatus)\
	READONLY_TABLE_1 (ifLastChange)\
	READONLY_TABLE_1 (ifInOctets)\
	READONLY_TABLE_1 (ifInUcastPkts)\
	READONLY_TABLE_1 (ifInDiscards)\
	READONLY_TABLE_1 (ifInErrors)\
	READONLY_TABLE_1 (ifInUnknownProtos)\
	READONLY_TABLE_1 (ifOutOctets)\
	READONLY_TABLE_1 (ifOutUcastPkts)\
	READONLY_TABLE_1 (ifOutDiscards)\
	READONLY_TABLE_1 (ifOutErrors)
