/*
 * TCP server example
 */
void tcp_server_example (ip_t *ip, int serv_port)
{
	tcp_socket_t *lsock, *sock;
	char buffer [256];
	int n;

	lsock = tcp_listen (ip, serv_port);
	if (! lsock) {
		error ("Error on listen");
		return;
	}
	for (;;) {
		sock = tcp_accept (lsock);
		if (! sock) {
			error ("Error on accept");
			break;
		}
		n = tcp_read (sock, buffer, 256);
		if (n < 0)
			error ("Error reading from socket");
		else {
			buffer[255] = 0;
			printf ("Here is the message: %s\n", buffer);
			n = tcp_write (sock, "I got your message", 19);
			if (n < 0)
				error ("Error writing to socket");
		}
		tcp_close (sock);
		mem_free (sock);
	}
	tcp_close (lsock);
	mem_free (lsock);
}

/*
 * TCP client example
 */
void tcp_client_example (ip_t *ip, unsigned char *serv_addr, int serv_port,
	char *message)
{
	tcp_socket_t *sock;
	char buffer [256];
	int n;

	sock = tcp_connect (ip, serv_addr, serv_port);
	if (! sock) {
		error ("Error connecting");
		return;
	}
	strncpy (buffer, message, 256);
	buffer[255] = 0;
	n = tcp_write (sock, buffer, strlen (buffer) + 1);
	if (n < 0)
		error ("Error writing to socket");
	else {
		n = tcp_read (sock, buffer, 256);
		if (n < 0)
			error ("Error reading from socket");
		else {
			buffer[255] = 0;
			printf ("%s\n", buffer);
		}
	}
	tcp_close (sock);
	mem_free (sock);
}
