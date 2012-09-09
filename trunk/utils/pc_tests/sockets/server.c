#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

#define PORT 		0xBBBB
#define MSG_SIZE		365

int msg[MSG_SIZE];
int	sz;

main()
{
	int 	 sd, sd_current, cc, fromlen, tolen;
	int 	 addrlen;
	struct   sockaddr_in sin;
	struct   sockaddr_in pin;
	int count = 0, old_count = 0, i;
	struct timeval start, end;
	long elapsed, seconds, useconds; 
 
	/* get an internet domain socket */
#ifdef TCP_SOCKET
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
#else
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
#endif
		perror("socket");
		exit(1);
	}

	/* complete the socket structure */
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(PORT);

	/* bind the socket to the port number */
	if (bind(sd, (struct sockaddr *) &sin, sizeof(sin)) == -1) {
		perror("bind");
		exit(1);
	}
#ifdef TCP_SOCKET
	/* show that we are willing to listen */
	if (listen(sd, 5) == -1) {
		perror("listen");
		exit(1);
	}
	/* wait for a client to talk to us */
        addrlen = sizeof(pin); 
	if ((sd_current = accept(sd, (struct sockaddr *)  &pin, &addrlen)) == -1) {
		perror("accept");
		exit(1);
	}
	printf ("Client connected\n");
#endif
	
/* if you want to see the ip address and port of the client, uncomment the 
    next two lines */

       /*
printf("Hi there, from  %s#\n",inet_ntoa(pin.sin_addr));
printf("Coming from port %d\n",ntohs(pin.sin_port));
        */

	while (1) {
		/* get a message from the client */
#ifdef TCP_SOCKET
		sz = recv(sd_current, msg, sizeof(msg), 0);
#else
		sz = recv(sd, msg, sizeof(msg), 0);
#endif
		//printf("sz = %d\n", sz);
		if (sz == -1) {
			perror("recv");
			exit(1);
		} else if (sz == 0) {
			printf("Disconnected\n");
			exit(0);
		}		

		sz >>= 2;

		for (i = 0; i < sz; ++i) {
			if (msg[i] != count) {
				fprintf(stderr, "bad counter: %d, expected: %d\n", msg[i], count);
				count = msg[i] + 1;
				continue;
			}
			++count;
		}

		if ((count - old_count) >= (256 * sizeof(msg))) {
			gettimeofday(&end, NULL);
			seconds  = end.tv_sec  - start.tv_sec;
			useconds = end.tv_usec - start.tv_usec;
			elapsed = ((seconds) * 1000 + useconds/1000.0) + 0.5;
			printf ("rcv rate: %ld (bytes/sec)\n", ((count - old_count) << 2) * 1000 / elapsed);
			old_count = count;
			memcpy (&start, &end, sizeof (struct timeval));
		}
	}

	/* close up both sockets */
	close(sd_current); close(sd);
        
	/* give client a chance to properly shutdown */
	sleep(1);
}

 
