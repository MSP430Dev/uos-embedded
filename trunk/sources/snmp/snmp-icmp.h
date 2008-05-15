/*
 * the ICMP group
 *
 * Implementation of the ICMP group is mandatory for all
 * systems.
 */

/*
 * "The total number of ICMP messages which the
 * entity received.  Note that this counter includes
 * all those counted by icmpInErrors."
 */
#define IicmpInMsgs Iicmp "\1"          /* ::= { icmp 1 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP messages which the entity
 * received but determined as having ICMP-specific
 * errors (bad ICMP checksums, bad length, etc.)."
 */
#define IicmpInErrors Iicmp "\2"        /* ::= { icmp 2 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Destination Unreachable
 * messages received."
 */
#define IicmpInDestUnreachs Iicmp "\3"  /* ::= { icmp 3 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Time Exceeded messages
 * received."
 */
#define IicmpInTimeExcds Iicmp "\4"     /* ::= { icmp 4 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Parameter Problem messages
 * received."
 */
#define IicmpInParmProbs Iicmp "\5"     /* ::= { icmp 5 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Source Quench messages
 * received."
 */
#define IicmpInSrcQuenchs Iicmp "\6"    /* ::= { icmp 6 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Redirect messages received."
 */
#define IicmpInRedirects Iicmp "\7"	/* ::= { icmp 7 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Echo (request) messages
 * received."
 */
#define IicmpInEchos Iicmp "\10"        /* ::= { icmp 8 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Echo Reply messages received."
 */
#define IicmpInEchoReps Iicmp "\11"     /* ::= { icmp 9 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Timestamp (request) messages
 * received."
 */
#define IicmpInTimestamps Iicmp "\12"   /* ::= { icmp 10 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Timestamp Reply messages
 * received."
 */
#define IicmpInTimestampReps Iicmp "\13" /* ::= { icmp 11 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Address Mask Request messages
 * received."
 */
#define IicmpInAddrMasks Iicmp "\14"	/* ::= { icmp 12 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Address Mask Reply messages
 * received."
 */
#define IicmpInAddrMaskReps Iicmp "\15" /* ::= { icmp 13 } */
					/* read-only, mandatory */

/*
 * "The total number of ICMP messages which this
 * entity attempted to send.  Note that this counter
 * includes all those counted by icmpOutErrors."
 */
#define IicmpOutMsgs Iicmp "\16"        /* ::= { icmp 14 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP messages which this entity did
 * not send due to problems discovered within ICMP
 * such as a lack of buffers.  This value should not
 * include errors discovered outside the ICMP layer
 * such as the inability of IP to route the resultant
 * datagram.  In some implementations there may be no
 * types of error which contribute to this counter's
 * value."
 */
#define IicmpOutErrors Iicmp "\17"      /* ::= { icmp 15 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Destination Unreachable
 * messages sent."
 */
#define IicmpOutDestUnreachs Iicmp "\20" /* ::= { icmp 16 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Time Exceeded messages sent."
 */
#define IicmpOutTimeExcds Iicmp "\21"   /* ::= { icmp 17 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Parameter Problem messages
 * sent."
 */
#define IicmpOutParmProbs Iicmp "\22"   /* ::= { icmp 18 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Source Quench messages sent."
 */
#define IicmpOutSrcQuenchs Iicmp "\23"	/* ::= { icmp 19 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Redirect messages sent.  For a
 * host, this object will always be zero, since hosts
 * do not send redirects."
 */
#define IicmpOutRedirects Iicmp "\24"   /* ::= { icmp 20 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Echo (request) messages sent."
 */
#define IicmpOutEchos Iicmp "\25"       /* ::= { icmp 21 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Echo Reply messages sent."
 */
#define IicmpOutEchoReps Iicmp "\26"    /* ::= { icmp 22 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Timestamp (request) messages
 * sent."
 */
#define IicmpOutTimestamps Iicmp "\27"  /* ::= { icmp 23 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Timestamp Reply messages
 * sent."
 */
#define IicmpOutTimestampReps Iicmp "\30" /* ::= { icmp 24 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Address Mask Request messages
 * sent."
 */
#define IicmpOutAddrMasks Iicmp "\31"   /* ::= { icmp 25 } */
					/* read-only, mandatory */

/*
 * "The number of ICMP Address Mask Reply messages
 * sent."
 */
#define IicmpOutAddrMaskReps Iicmp "\32" /* ::= { icmp 26 } */
					/* read-only, mandatory */

#define ICMP_VARIABLE_LIST\
	READONLY_VARIABLE (icmpInMsgs)\
	READONLY_VARIABLE (icmpInErrors)\
	READONLY_VARIABLE (icmpInDestUnreachs)\
	READONLY_VARIABLE (icmpInTimeExcds)\
	READONLY_VARIABLE (icmpInParmProbs)\
	READONLY_VARIABLE (icmpInSrcQuenchs)\
	READONLY_VARIABLE (icmpInRedirects)\
	READONLY_VARIABLE (icmpInEchos)\
	READONLY_VARIABLE (icmpInEchoReps)\
	READONLY_VARIABLE (icmpInTimestamps)\
	READONLY_VARIABLE (icmpInTimestampReps)\
	READONLY_VARIABLE (icmpInAddrMasks)\
	READONLY_VARIABLE (icmpInAddrMaskReps)\
	READONLY_VARIABLE (icmpOutMsgs)\
	READONLY_VARIABLE (icmpOutErrors)\
	READONLY_VARIABLE (icmpOutDestUnreachs)\
	READONLY_VARIABLE (icmpOutTimeExcds)\
	READONLY_VARIABLE (icmpOutParmProbs)\
	READONLY_VARIABLE (icmpOutSrcQuenchs)\
	READONLY_VARIABLE (icmpOutRedirects)\
	READONLY_VARIABLE (icmpOutEchos)\
	READONLY_VARIABLE (icmpOutEchoReps)\
	READONLY_VARIABLE (icmpOutTimestamps)\
	READONLY_VARIABLE (icmpOutTimestampReps)\
	READONLY_VARIABLE (icmpOutAddrMasks)\
	READONLY_VARIABLE (icmpOutAddrMaskReps)
