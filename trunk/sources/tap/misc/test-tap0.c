#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define __USE_GNU
#include <fcntl.h>

#define DEVNAME	"/dev/tap0"
#define SIGNUM	SIGIO

int fd;

void sigio (int signum)
{
	char buf [2000];
	int len, i;

	printf ("Got signal %d\n", signum);
	fflush (stdout);
	for (;;) {
		len = read (fd, buf, sizeof (buf));
		if (len <= 0)
			break;
		printf ("%d bytes: %02x", len, buf[0]);
		for (i=1; i<len; ++i)
			printf ("-%02x", (unsigned char) buf[i]);
		printf ("\n", len);
		fflush (stdout);
	}
}

int main (int argc, char **argv)
{
	fd = open (DEVNAME, O_RDWR | O_NDELAY);
        if (fd < 0) {
                perror (DEVNAME);
                exit (1);
        }
	signal (SIGNUM, sigio);
	fcntl (fd, F_SETOWN, getpid());
	fcntl (fd, F_SETSIG, SIGNUM);

	fcntl (fd, F_SETFL, fcntl (fd, F_GETFL, 0) | O_ASYNC);

	printf ("Waiting for i/o signal.\n");
	printf ("Ping /dev/tap0 interface.\n");
	for (;;)
		pause ();
}
