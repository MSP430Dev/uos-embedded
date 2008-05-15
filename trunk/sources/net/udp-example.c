/*
 * UDP server example
 */
void udp_server_example (ip_t *ip, int serv_port)
{
	udp_socket_t sock;
	unsigned char client_addr [4];
	unsigned short client_port;
	buf_t *p, *r;

	udp_socket (&sock, ip, serv_port);
	for (;;) {
		p = udp_recvfrom (&sock, client_addr, &client_port);

		/* Process packet p, produce reply r. */
		r = process (p);
		buf_free (p);

		udp_sendto (&sock, r, client_addr, client_port);
	}
	udp_close (&sock);
}

/*
 * UDP client example
 */
void udp_client_example (ip_t *ip, unsigned char *serv_addr, int serv_port,
	int client_port, char *message)
{
	udp_socket_t sock;
	buf_t *p, *r;

	udp_socket (&sock, ip, client_port);

	/* Produce packet p. */
	p = ...;

	if (! udp_sendto (&sock, p, serv_addr, serv_port))
		error ("Error writing to socket");
	else {
		r = udp_recvfrom (&sock, client_addr, &client_port);

		/* Process reply r. */
		consume (r);
		buf_free (r);
	}
	udp_close (&sock);
}
