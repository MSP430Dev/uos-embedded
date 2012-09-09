#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#define PORT        0xBBBB
/* REPLACE with your server machine name*/
#define HOST        "192.168.1.52"
#define MSG_SIZE    256
#define CYCLE_CNT   1024

char hostname[100];
int rmsg[MSG_SIZE];
int tmsg[MSG_SIZE];

main(argc, argv)
int argc; char **argv;
{
	int	sd;
	struct sockaddr_in sin;
	struct sockaddr_in pin;
	struct hostent *hp;
	int tcount = 0, old_tcount = 0;
	int rcount = 0, old_rcount = 0;
	int i, cycle = 0;
	int sz;
	struct timeval start, end;
	long elapsed, seconds, useconds;    

        strcpy(hostname,HOST);
        if (argc>1)
            { strcpy(hostname,argv[1]); }

	/* go find out about the desired host machine */
	if ((hp = gethostbyname(hostname)) == 0) {
		perror("gethostbyname");
		exit(1);
	}

	/* fill in the socket structure with host information */
	memset(&pin, 0, sizeof(pin));
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
	pin.sin_port = htons(PORT);

	/* grab an Internet domain socket */
#ifdef TCP_SOCKET
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
#else
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
#endif
		perror("socket");
		exit(1);
	}

	/* connect to PORT on HOST */
	if (connect(sd,(struct sockaddr *)  &pin, sizeof(pin)) == -1) {
		perror("connect");
		exit(1);
	}

	printf("Connected to server\n");

	while (1) {
	
		for (i = 0; i < MSG_SIZE; ++i) tmsg[i] = tcount++;

		/* send a message to the server PORT on machine HOST */
		if (send(sd, tmsg, sizeof(tmsg), 0) == -1) {
			perror("send");
			exit(1);
		}

#ifdef TCP_SOCKET
		sz = recv(sd, rmsg, sizeof(rmsg), 0);
#else
		sz = recv(sd, rmsg, sizeof(rmsg), 0);
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
			if (rmsg[i] != rcount) {
				fprintf(stderr, "bad counter: %d, expected: %d\n", rmsg[i], rcount);
				rcount = rmsg[i] + 1;
				continue;
			}
			++rcount;
		}

		if ((rcount - old_rcount) >= (256 * sizeof(rmsg))) {
			gettimeofday(&end, NULL);
			seconds  = end.tv_sec  - start.tv_sec;
			useconds = end.tv_usec - start.tv_usec;
			elapsed = ((seconds) * 1000 + useconds/1000.0) + 0.5;
			printf ("send rate: %ld (bytes/sec), rcv rate: %ld (bytes/sec)\n", ((tcount - old_tcount) << 2) * 1000 / elapsed,
				((rcount - old_rcount) << 2) * 1000 / elapsed);
			old_rcount = rcount;
			old_tcount = tcount;
			memcpy (&start, &end, sizeof (struct timeval));
		}
		++cycle;
	}

	close(sd);
}

 
