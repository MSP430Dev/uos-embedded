/*
 * TCP client example
 */
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main (int argc, char **argv)
{
	int sock;
	struct sockaddr_in sin;
	char *serv_addr;
	unsigned short serv_port = 2222;
	char message [] = "Hello, Net!\n";
	char buffer [256];
	int n, k;

	if (argc != 2) {
		printf ("Usage: tcp-client <server-ip-address>\n");
		return 1;
	}
	serv_addr = argv[1];

	sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		printf ("Error creating socket, aborted\n");
		return 1;
	}
	memset (&sin, 0, sizeof (sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr (serv_addr);
	sin.sin_port = htons (serv_port);
	printf ("Client started, connecting to %s\n", inet_ntoa (sin.sin_addr));
	if (connect (sock, (struct sockaddr*) &sin, sizeof(sin)) < 0) {
		printf ("Error connecting, aborted\n");
		return 1;
	}
	if (send (sock, message, strlen(message) + 1, 0) < 0) {
		printf ("Error writing to socket");
		return 1;
	}
	k = 0;
	printf ("Received: ");
	while (k < strlen(message) + 1) {
		n = recv (sock, buffer, 256, 0);
		if (n <= 0) {
			if (n < 0)
				printf ("<Error reading from socket>");
			break;
		}
		buffer[n] = 0;
		printf ("%s", buffer);
		k += n;
	}
	printf ("\nClient finished\n");
	close (sock);
	return 0;
}
