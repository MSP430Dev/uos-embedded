/*
 * the UDP group
 *
 * Implementation of the UDP group is mandatory for all
 * systems which implement the UDP.
 */

/*
 * "The total number of UDP datagrams delivered to
 * UDP users."
 */
#define IudpInDatagrams Iudp "\1"       /* ::= { udp 1 } */
					/* read-only, mandatory */

/*
 * "The total number of received UDP datagrams for
 * which there was no application at the destination
 * port."
 */
#define IudpNoPorts Iudp "\2"           /* ::= { udp 2 } */
					/* read-only, mandatory */

/*
 * "The number of received UDP datagrams that could
 * not be delivered for reasons other than the lack
 * of an application at the destination port."
 */
#define IudpInErrors Iudp "\3"          /* ::= { udp 3 } */
					/* read-only, mandatory */

/*
 * "The total number of UDP datagrams sent from this
 * entity."
 */
#define IudpOutDatagrams Iudp "\4"      /* ::= { udp 4 } */
					/* read-only, mandatory */

/*
 * the UDP Listener table
 *
 * The UDP listener table contains information about this
 * entity's UDP end-points on which a local application is
 * currently accepting datagrams.
 */
/*
 * "A table containing UDP listener information."
 * SEQUENCE OF UdpEntry,
 * not-accessible, mandatory
 */
#define IudpTable Iudp "\5"             /* ::= { udp 5 } */

/*
 * "Information about a particular current UDP
 * listener."
 * SEQUENCE {
 *      udpLocalAddress IpAddress,
 *      udpLocalPort    INTEGER (0..65535)
 * }
 * INDEX { udpLocalAddress, udpLocalPort }
 * not-accessible, mandatory
 */
#define IudpEntry IudpTable "\1"        /* ::= { udpTable 1 } */

/*
 * "The local IP address for this UDP listener.  In
 * the case of a UDP listener which is willing to
 * accept datagrams for any IP interface associated
 * with the node, the value 0.0.0.0 is used."
 */
#define IudpLocalAddress IudpEntry "\1" /* ::= { udpEntry 1 } */
					/* read-only, mandatory */

/*
 * "The local port number for this UDP listener."
 */
#define IudpLocalPort IudpEntry "\2"    /* ::= { udpEntry 2 } */
					/* INTEGER (0..65535),
					   read-only, mandatory */

#define UDP_VARIABLE_LIST\
	READONLY_VARIABLE (udpInDatagrams)\
	READONLY_VARIABLE (udpNoPorts)\
	READONLY_VARIABLE (udpInErrors)\
	READONLY_VARIABLE (udpOutDatagrams)\
	READONLY_TABLE_41 (udpLocalAddress)\
	READONLY_TABLE_41 (udpLocalPort)
