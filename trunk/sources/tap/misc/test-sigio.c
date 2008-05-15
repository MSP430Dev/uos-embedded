#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define __USE_GNU
#include <fcntl.h>

void sigio (int signum)
{
	printf ("Got signal %d\n", signum);
	fflush (stdout);
	exit (1);
}

int main (int argc, char **argv)
{
	int flags;
#if 1
	close (0);
	open ("/dev/tty", O_NONBLOCK);
#endif
	signal (SIGUSR1, sigio);
	fcntl (0, F_SETOWN, getpid());
	fcntl (0, F_SETSIG, SIGUSR1);
#if 1
	flags = fcntl (0, F_GETFL, 0);
	printf ("flags = 0x%x\n", flags);
	flags = 0x2002;
	fcntl (0, F_SETFL, FASYNC /*flags | O_ASYNC*/);
#endif
	printf ("Sending i/o signal - press enter.\n");
	for (;;)
		pause ();
}
