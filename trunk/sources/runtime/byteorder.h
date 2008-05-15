/*
 * Convert value between network and host byte order.
 */
#if defined(__motorola__)
/*
 * Big endian.
 */

#define HTONS(x)	(x)
#define HTONL(x)	(x)
#define NTOHS(x)	(x)
#define NTOHL(x)	(x)

#else
/*
 * Little endian.
 */

#define HTONS(x)	(((x)>>8 & 0xff) | ((x)<<8 & 0xff00))
#define HTONL(x)	(((x)>> 24 & 0xff) | ((x)>>8 & 0xff00) |\
			 ((x)<<8 & 0xff0000L) | ((x)<<24 & 0xff000000L))
#define NTOHS(x)	HTONS(x)
#define NTOHL(x)	HTONL(x)

#endif
