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
	char buffer [512];
	int n, k, len;

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
	for (;;) {
		n = recv (sock, buffer, 2, 0);
		if (n == 0) {
			printf ("Connection closed by server\n");
			break;
		}
		if (n != 2) {
			printf ("Error %d receiving packet length\n", n);
			break;
		}
		len = *(unsigned short*) buffer;
		if (len != 512) {
			printf ("Bad packet length = %d\n", len);
			break;
		}
/*		printf ("<%d> ", len);*/
		printf (".");
		fflush (stdout);
		k = 2;
		while (k < len) {
			n = recv (sock, buffer + k, len - k, 0);
			if (n <= 0) {
				if (n < 0)
					printf ("Error %d reading from socket\n", n);
				break;
			}
			k += n;
		}
	}
	printf ("\nClient finished\n");
	close (sock);
	return 0;
}
