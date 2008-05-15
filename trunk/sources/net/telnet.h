#ifndef __TELNET_H_
#define __TELNET_H_ 1

/*
 * Telnet protocol implementation, with stream interface.
 */
typedef struct _telnet_t {
	stream_t stream;
	tcp_stream_t ts;

	unsigned char local_option [256/8];
	unsigned char remote_option [256/8];
} telnet_t;

stream_t *telnet_init (tcp_socket_t *sock);

#endif /* __TELNET_H_ */
