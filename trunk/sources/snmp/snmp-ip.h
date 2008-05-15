/*
 * the IP group
 *
 * Implementation of the IP group is mandatory for all
 * systems.
 */

/*
 * "The indication of whether this entity is acting
 * as an IP gateway in respect to the forwarding of
 * datagrams received by, but not addressed to, this
 * entity.  IP gateways forward datagrams.  IP hosts
 * do not (except those source-routed via the host).
 *
 * Note that for some managed nodes, this object may
 * take on only a subset of the values possible.
 * Accordingly, it is appropriate for an agent to
 * return a `badValue' response if a management
 * station attempts to change this object to an
 * inappropriate value."
 */
#define IipForwarding Iip "\1"          /* ::= { ip 1 } */
					/* read-write */
#define SNMP_IP_FORWARDING	1	/* acting as a gateway */
#define SNMP_IP_NOT_FORWARDING	2	/* NOT acting as a gateway */

/*
 * "The default value inserted into the Time-To-Live
 * field of the IP header of datagrams originated at
 * this entity, whenever a TTL value is not supplied
 * by the transport layer protocol."
 */
#define IipDefaultTTL Iip "\2"          /* ::= { ip 2 } */
					/* INTEGER, read-write */

/*
 * "The total number of input datagrams received from
 * interfaces, including those received in error."
 */
#define IipInReceives Iip "\3"          /* ::= { ip 3 } */
					/* Counter, read-only */

/*
 * "The number of input datagrams discarded due to
 * errors in their IP headers, including bad
 * checksums, version number mismatch, other format
 * errors, time-to-live exceeded, errors discovered
 * in processing their IP options, etc."
 */
#define IipInHdrErrors Iip "\4"         /* ::= { ip 4 } */
					/* Counter, read-only */

/*
 * "The number of input datagrams discarded because
 * the IP address in their IP header's destination
 * field was not a valid address to be received at
 * this entity.  This count includes invalid
 * addresses (e.g., 0.0.0.0) and addresses of
 * unsupported Classes (e.g., Class E).  For entities
 * which are not IP Gateways and therefore do not
 * forward datagrams, this counter includes datagrams
 * discarded because the destination address was not
 * a local address."
 */
#define IipInAddrErrors Iip "\5"        /* ::= { ip 5 } */
					/* Counter, read-only */

/*
 * "The number of input datagrams for which this
 * entity was not their final IP destination, as a
 * result of which an attempt was made to find a
 * route to forward them to that final destination.
 * In entities which do not act as IP Gateways, this
 * counter will include only those packets which were
 * Source-Routed via this entity, and the Source-
 * Route option processing was successful."
 */
#define IipForwDatagrams Iip "\6"       /* ::= { ip 6 } */
					/* Counter, read-only */

/*
 * "The number of locally-addressed datagrams
 * received successfully but discarded because of an
 * unknown or unsupported protocol."
 */
#define IipInUnknownProtos Iip "\7"     /* ::= { ip 7 } */
					/* Counter, read-only */

/*
 * "The number of input IP datagrams for which no
 * problems were encountered to prevent their
 * continued processing, but which were discarded
 * (e.g., for lack of buffer space).  Note that this
 * counter does not include any datagrams discarded
 * while awaiting re-assembly."
 */
#define IipInDiscards Iip "\10"         /* ::= { ip 8 } */
					/* Counter, read-only */

/*
 * "The total number of input datagrams successfully
 * delivered to IP user-protocols (including ICMP)."
 */
#define IipInDelivers Iip "\11"         /* ::= { ip 9 } */
					/* Counter, read-only */

/*
 * "The total number of IP datagrams which local IP
 * user-protocols (including ICMP) supplied to IP in
 * requests for transmission.  Note that this counter
 * does not include any datagrams counted in
 * ipForwDatagrams."
 */
#define IipOutRequests Iip "\12"        /* ::= { ip 10 } */
					/* Counter, read-only */

/*
 * "The number of output IP datagrams for which no
 * problem was encountered to prevent their
 * transmission to their destination, but which were
 * discarded (e.g., for lack of buffer space).  Note
 * that this counter would include datagrams counted
 * in ipForwDatagrams if any such packets met this
 * (discretionary) discard criterion."
 */
#define IipOutDiscards Iip "\13"        /* ::= { ip 11 } */
					/* Counter, read-only */

/*
 * "The number of IP datagrams discarded because no
 * route could be found to transmit them to their
 * destination.  Note that this counter includes any
 * packets counted in ipForwDatagrams which meet this
 * `no-route' criterion.  Note that this includes any
 * datagarms which a host cannot route because all of
 * its default gateways are down."
 */
#define IipOutNoRoutes Iip "\14"        /* ::= { ip 12 } */
					/* Counter, read-only */

/*
 * "The maximum number of seconds which received
 * fragments are held while they are awaiting
 * reassembly at this entity."
 */
#define IipReasmTimeout Iip "\15"       /* ::= { ip 13 } */
					/* INTEGER, read-only */

/*
 * "The number of IP fragments received which needed
 * to be reassembled at this entity."
 */
#define IipReasmReqds Iip "\16"         /* ::= { ip 14 } */
					/* Counter, read-only */

/*
 * "The number of IP datagrams successfully re-
 * assembled."
 */
#define IipReasmOKs Iip "\17"           /* ::= { ip 15 } */
					/* Counter, read-only */

/*
 * "The number of failures detected by the IP re-
 * assembly algorithm (for whatever reason: timed
 * out, errors, etc).  Note that this is not
 * necessarily a count of discarded IP fragments
 * since some algorithms (notably the algorithm in
 * RFC 815) can lose track of the number of fragments
 * by combining them as they are received."
 */
#define IipReasmFails Iip "\20"         /* ::= { ip 16 } */
					/* Counter, read-only */

/*
 * "The number of IP datagrams that have been
 * successfully fragmented at this entity."
 */
#define IipFragOKs Iip "\21"            /* ::= { ip 17 } */
					/* Counter, read-only */

/*
 * "The number of IP datagrams that have been
 * discarded because they needed to be fragmented at
 * this entity but could not be, e.g., because their
 * Don't Fragment flag was set."
 */
#define IipFragFails Iip "\22"          /* ::= { ip 18 } */
					/* Counter, read-only */

/*
 * "The number of IP datagram fragments that have
 * been generated as a result of fragmentation at
 * this entity."
 */
#define IipFragCreates Iip "\23"        /* ::= { ip 19 } */
					/* Counter, read-only */

/*
 * the IP address table
 *
 * The IP address table contains this entity's IP addressing
 * information.
 */

/*
 * "The table of addressing information relevant to
 * this entity's IP addresses."
 * SEQUENCE OF IpAddrEntry
 */
#define IipAddrTable Iip "\24"          /* ::= { ip 20 } */

/*
 * "The addressing information for one of this
 * entity's IP addresses."
 * INDEX { ipAdEntAddr }
 */
#define IipAddrEntry IipAddrTable "\1"  /* ::= { ipAddrTable 1 } */

/*
 * "The IP address to which this entry's addressing
 * information pertains."
 */
#define IipAdEntAddr IipAddrEntry "\1"  /* ::= { ipAddrEntry 1 } */
					/* IpAddress, read-only */

/*
 * "The index value which uniquely identifies the
 * interface to which this entry is applicable.  The
 * interface identified by a particular value of this
 * index is the same interface as identified by the
 * same value of ifIndex."
 */
#define IipAdEntIfIndex IipAddrEntry "\2" /* ::= { ipAddrEntry 2 } */
					/* INTEGER, read-only */

/*
 * "The subnet mask associated with the IP address of
 * this entry.  The value of the mask is an IP
 * address with all the network bits set to 1 and all
 * the hosts bits set to 0."
 */
#define IipAdEntNetMask IipAddrEntry "\3" /* ::= { ipAddrEntry 3 } */
					/* IpAddress, read-only */

/*
 * "The value of the least-significant bit in the IP
 * broadcast address used for sending datagrams on
 * the (logical) interface associated with the IP
 * address of this entry.  For example, when the
 * Internet standard all-ones broadcast address is
 * used, the value will be 1.  This value applies to
 * both the subnet and network broadcasts addresses
 * used by the entity on this (logical) interface."
 */
#define IipAdEntBcastAddr IipAddrEntry "\4" /* ::= { ipAddrEntry 4 } */
					/* INTEGER, read-only */

/*
 * "The size of the largest IP datagram which this
 * entity can re-assemble from incoming IP fragmented
 * datagrams received on this interface."
 */
#define IipAdEntReasmMaxSize IipAddrEntry "\5" /* ::= { ipAddrEntry 5 } */
					/* INTEGER (0..65535), read-only */

/*
 * the IP routing table
 *
 *  The IP routing table contains an entry for each route
 * presently known to this entity.
 */

/*
 * "This entity's IP Routing table."
 * SEQUENCE OF IpRouteEntry
 */
#define IipRouteTable Iip "\25"         /* ::= { ip 21 } */

/*
 * "A route to a particular destination."
 * INDEX { ipRouteDest }
 */
#define IipRouteEntry IipRouteTable "\1" /* ::= { ipRouteTable 1 } */

/*
 * "The destination IP address of this route.  An
 * entry with a value of 0.0.0.0 is considered a
 * default route.  Multiple routes to a single
 * destination can appear in the table, but access to
 * such multiple entries is dependent on the table-
 * access mechanisms defined by the network
 * management protocol in use."
 */
#define IipRouteDest IipRouteEntry "\1" /* ::= { ipRouteEntry 1 } */
					/* IpAddress, read-write */

/*
 * "The index value which uniquely identifies the
 * local interface through which the next hop of this
 * route should be reached.  The interface identified
 * by a particular value of this index is the same
 * interface as identified by the same value of
 * ifIndex."
 */
#define IipRouteIfIndex IipRouteEntry "\2" /* ::= { ipRouteEntry 2 } */
					/* INTEGER, read-write */

/*
 * "The primary routing metric for this route.  The
 * semantics of this metric are determined by the
 * routing-protocol specified in the route's
 * ipRouteProto value.  If this metric is not used,
 * its value should be set to -1."
 */
#define IipRouteMetric1 IipRouteEntry "\3" /* ::= { ipRouteEntry 3 } */
					/* INTEGER, read-write */

/*
 * "An alternate routing metric for this route.  The
 * semantics of this metric are determined by the
 * routing-protocol specified in the route's
 * ipRouteProto value.  If this metric is not used,
 * its value should be set to -1."
 */
#define IipRouteMetric2 IipRouteEntry "\4" /* ::= { ipRouteEntry 4 } */
					/* INTEGER, read-write */

/*
 * "An alternate routing metric for this route.  The
 * semantics of this metric are determined by the
 * routing-protocol specified in the route's
 * ipRouteProto value.  If this metric is not used,
 * its value should be set to -1."
 */
#define IipRouteMetric3 IipRouteEntry "\5" /* ::= { ipRouteEntry 5 } */
					/* INTEGER, read-write */

/*
 * "An alternate routing metric for this route.  The
 * semantics of this metric are determined by the
 * routing-protocol specified in the route's
 * ipRouteProto value.  If this metric is not used,
 * its value should be set to -1."
 */
#define IipRouteMetric4 IipRouteEntry "\6" /* ::= { ipRouteEntry 6 } */
					/* INTEGER, read-write */

/*
 * "The IP address of the next hop of this route.
 * (In the case of a route bound to an interface
 * which is realized via a broadcast media, the value
 * of this field is the agent's IP address on that
 * interface.)"
 */
#define IipRouteNextHop IipRouteEntry "\7" /* ::= { ipRouteEntry 7 } */
					/* IpAddress, read-write */

/*
 * "The type of route.  Note that the values
 * direct(3) and indirect(4) refer to the notion of
 * direct and indirect routing in the IP
 * architecture.
 *
 * Setting this object to the value invalid(2) has
 * the effect of invalidating the corresponding entry
 * in the ipRouteTable object.  That is, it
 * effectively dissasociates the destination
 * identified with said entry from the route
 * identified with said entry.  It is an
 * implementation-specific matter as to whether the
 * agent removes an invalidated entry from the table.
 * Accordingly, management stations must be prepared
 * to receive tabular information from agents that
 * corresponds to entries not currently in use.
 * Proper interpretation of such entries requires
 * examination of the relevant ipRouteType object."
 */
#define IipRouteType IipRouteEntry "\10" /* ::= { ipRouteEntry 8 } */
					/* read-only */
#define	SNMP_ROUTE_TYPE_OTHER		1	/* none of the following */
#define	SNMP_ROUTE_TYPE_INVALID		2	/* an invalidated route */
#define	SNMP_ROUTE_TYPE_DIRECT		3	/* route to directly connected (sub-)network */
#define	SNMP_ROUTE_TYPE_INDIRECT	4	/* route to a non-local host/network/sub-network */

/*
 * "The routing mechanism via which this route was
 * learned.  Inclusion of values for gateway routing
 * protocols is not intended to imply that hosts
 * should support those protocols."
 */
#define IipRouteProto IipRouteEntry "\11" /* ::= { ipRouteEntry 9 } */
					/* read-only */
#define	SNMP_ROUTE_PROTO_OTHER		1	/* none of the following */
#define	SNMP_ROUTE_PROTO_LOCAL		2	/* non-protocol information, e.g., manually configured entries */
#define	SNMP_ROUTE_PROTO_NETMGMT	3	/* set via a network management protocol */
#define	SNMP_ROUTE_PROTO_ICMP		4	/* obtained via ICMP, e.g., Redirect */
#define	SNMP_ROUTE_PROTO_EGP		5	/* the remaining values are all gateway routing protocols */
#define	SNMP_ROUTE_PROTO_GGP		6
#define	SNMP_ROUTE_PROTO_HELLO		7
#define	SNMP_ROUTE_PROTO_RIP		8
#define	SNMP_ROUTE_PROTO_IS_IS		9
#define	SNMP_ROUTE_PROTO_ES_IS		10
#define	SNMP_ROUTE_PROTO_CISCO_IGRP	11
#define	SNMP_ROUTE_PROTO_BBN_SPF_IGP	12
#define	SNMP_ROUTE_PROTO_OSPF		13
#define	SNMP_ROUTE_PROTO_BGP		14

/*
 * "The number of seconds since this route was last
 * updated or otherwise determined to be correct.
 * Note that no semantics of `too old' can be implied
 * except through knowledge of the routing protocol
 * by which the route was learned."
 */
#define IipRouteAge IipRouteEntry "\12" /* ::= { ipRouteEntry 10 } */
					/* INTEGER, read-write */

/*
 * "Indicate the mask to be logical-ANDed with the
 * destination address before being compared to the
 * value in the ipRouteDest field.  For those systems
 * that do not support arbitrary subnet masks, an
 * agent constructs the value of the ipRouteMask by
 * determining whether the value of the correspondent
 * ipRouteDest field belong to a class-A, B, or C
 * network, and then using one of:
 *
 *      mask           network
 *      255.0.0.0      class-A
 *      255.255.0.0    class-B
 *      255.255.255.0  class-C
 * If the value of the ipRouteDest is 0.0.0.0 (a
 * default route), then the mask value is also
 * 0.0.0.0.  It should be noted that all IP routing
 * subsystems implicitly use this mechanism."
 */
#define IipRouteMask IipRouteEntry "\13" /* ::= { ipRouteEntry 11 } */
					/* IpAddress, read-write */

/*
 * "An alternate routing metric for this route.  The
 * semantics of this metric are determined by the
 * routing-protocol specified in the route's
 * ipRouteProto value.  If this metric is not used,
 * its value should be set to -1."
 */
#define IipRouteMetric5 IipRouteEntry "\14" /* ::= { ipRouteEntry 12 } */
					/* INTEGER, read-write */

/*
 * "A reference to MIB definitions specific to the
 * particular routing protocol which is responsible
 * for this route, as determined by the value
 * specified in the route's ipRouteProto value.  If
 * this information is not present, its value should
 * be set to the OBJECT IDENTIFIER { 0 0 }, which is
 * a syntatically valid object identifier, and any
 * conformant implementation of ASN.1 and BER must be
 * able to generate and recognize this value."
 */
#define IipRouteInfo IipRouteEntry "\15" /* ::= { ipRouteEntry 13 } */
					/* OBJECT IDENTIFIER, read-only */

/*
 * the IP Address Translation table
 *
 * The IP address translation table contain the IpAddress to
 * `physical' address equivalences.  Some interfaces do not
 * use translation tables for determining address
 * equivalences (e.g., DDN-X.25 has an algorithmic method);
 * if all interfaces are of this type, then the Address
 * Translation table is empty, i.e., has zero entries.
 */

/*
 * "The IP Address Translation table used for mapping
 * from IP addresses to physical addresses."
 * SEQUENCE OF IpNetToMediaEntry
 */
#define IipNetToMediaTable Iip "\26"            /* ::= { ip 22 } */

/*
 * "Each entry contains one IpAddress to `physical'
 * address equivalence."
 * INDEX { ipNetToMediaIfIndex, ipNetToMediaNetAddress }
 */
#define IipNetToMediaEntry IipNetToMediaTable "\1" /* ::= { ipNetToMediaTable 1 } */

/*
 * "The interface on which this entry's equivalence
 * is effective.  The interface identified by a
 * particular value of this index is the same
 * interface as identified by the same value of
 * ifIndex."
 */
#define IipNetToMediaIfIndex IipNetToMediaEntry "\1" /* ::= { ipNetToMediaEntry 1 } */
					/* INTEGER, read-write */

/*
 * "The media-dependent `physical' address."
 */
#define IipNetToMediaPhysAddress IipNetToMediaEntry "\2" /* ::= { ipNetToMediaEntry 2 } */
					/* PhysAddress, read-write */

/*
 * "The IpAddress corresponding to the media-
 * dependent `physical' address."
 */
#define IipNetToMediaNetAddress IipNetToMediaEntry "\3" /* ::= { ipNetToMediaEntry 3 } */
					/* IpAddress, read-write */

/*
 * "The type of mapping.
 *
 * Setting this object to the value invalid(2) has
 * the effect of invalidating the corresponding entry
 * in the ipNetToMediaTable.  That is, it effectively
 * dissasociates the interface identified with said
 * entry from the mapping identified with said entry.
 * It is an implementation-specific matter as to
 * whether the agent removes an invalidated entry
 * from the table.  Accordingly, management stations
 * must be prepared to receive tabular information
 * from agents that corresponds to entries not
 * currently in use.  Proper interpretation of such
 * entries requires examination of the relevant
 * ipNetToMediaType object."
 */
#define IipNetToMediaType IipNetToMediaEntry "\4" /* ::= { ipNetToMediaEntry 4 } */
					/* read-write */
#define SNMP_NTM_TYPE_OTHER	1	/* none of the following */
#define SNMP_NTM_TYPE_INVALID	2	/* an invalidated mapping */
#define SNMP_NTM_TYPE_DYNAMIC	3
#define SNMP_NTM_TYPE_STATIC	4

/*
 * additional IP objects
 */

/*
 * "The number of routing entries which were chosen
 * to be discarded even though they are valid.  One
 * possible reason for discarding such an entry could
 * be to free-up buffer space for other routing
 * entries."
 */
#define IipRoutingDiscards Iip "\27"	/* ::= { ip 23 } */
					/* Counter, read-only */

#define IP_VARIABLE_LIST\
	READWRITE_VARIABLE (ipForwarding)\
	READWRITE_VARIABLE (ipDefaultTTL)\
	READONLY_VARIABLE (ipInReceives)\
	READONLY_VARIABLE (ipInHdrErrors)\
	READONLY_VARIABLE (ipInAddrErrors)\
	READONLY_VARIABLE (ipForwDatagrams)\
	READONLY_VARIABLE (ipInUnknownProtos)\
	READONLY_VARIABLE (ipInDiscards)\
	READONLY_VARIABLE (ipInDelivers)\
	READONLY_VARIABLE (ipOutRequests)\
	READONLY_VARIABLE (ipOutDiscards)\
	READONLY_VARIABLE (ipOutNoRoutes)\
	READONLY_VARIABLE (ipReasmTimeout)\
	READONLY_VARIABLE (ipReasmReqds)\
	READONLY_VARIABLE (ipReasmOKs)\
	READONLY_VARIABLE (ipReasmFails)\
	READONLY_VARIABLE (ipFragOKs)\
	READONLY_VARIABLE (ipFragFails)\
	READONLY_VARIABLE (ipFragCreates)\
	READONLY_TABLE_4 (ipAdEntAddr)\
	READONLY_TABLE_4 (ipAdEntIfIndex)\
	READONLY_TABLE_4 (ipAdEntNetMask)\
	READONLY_TABLE_4 (ipAdEntBcastAddr)\
	READONLY_TABLE_4 (ipAdEntReasmMaxSize)\
	READWRITE_TABLE_4 (ipRouteDest)\
	READWRITE_TABLE_4 (ipRouteIfIndex)\
	READWRITE_TABLE_4 (ipRouteMetric1)\
	READWRITE_TABLE_4 (ipRouteMetric2)\
	READWRITE_TABLE_4 (ipRouteMetric3)\
	READWRITE_TABLE_4 (ipRouteMetric4)\
	READWRITE_TABLE_4 (ipRouteNextHop)\
	READWRITE_TABLE_4 (ipRouteType)\
	READONLY_TABLE_4 (ipRouteProto)\
	READWRITE_TABLE_4 (ipRouteAge)\
	READWRITE_TABLE_4 (ipRouteMask)\
	READWRITE_TABLE_4 (ipRouteMetric5)\
	READONLY_TABLE_4 (ipRouteInfo)\
	READWRITE_TABLE_14 (ipNetToMediaIfIndex)\
	READWRITE_TABLE_14 (ipNetToMediaPhysAddress)\
	READWRITE_TABLE_14 (ipNetToMediaNetAddress)\
	READWRITE_TABLE_14 (ipNetToMediaType)\
	READONLY_VARIABLE (ipRoutingDiscards)
