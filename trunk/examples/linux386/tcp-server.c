/*
 * TCP server example
 */
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
	int lsock, sock;
	unsigned short serv_port = 2222;
	struct sockaddr_in sin;
	char buffer [256];
	socklen_t sn;
	int n;

	printf ("Server started\n");
	lsock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (lsock < 0) {
		printf ("Error creating socket\n");
		return 1;
	}
	memset (&sin, 0, sizeof (sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl (INADDR_ANY);
	sin.sin_port = htons (serv_port);
	if (bind (lsock, (struct sockaddr*) &sin, sizeof(sin)) < 0) {
		printf ("Error on bind\n");
		return 1;
	}
	if (listen (lsock, 5) < 0) {
		printf ("Error on listen\n");
		return 1;
	}
	for (;;) {
		printf ("Waiting for client...\n");
		sn = sizeof (sin);
		sock = accept (lsock, (struct sockaddr*) &sin, &sn);
		if (sock < 0) {
			printf ("Error on accept\n");
			break;
		}
		printf ("Handling client %s\n", inet_ntoa (sin.sin_addr));
		for (;;) {
			n = recv (sock, buffer, 256, 0);
			if (n <= 0) {
				if (n < 0)
					printf ("Error reading from socket\n");
				break;
			}
			printf ("Message: %.*s\n", n, buffer);
			if (send (sock, buffer, n, 0) != n)
				printf ("Error writing to socket\n");
		}
		close (sock);
	}
	close (lsock);
	printf ("Server finished\n");
	return 0;
}
