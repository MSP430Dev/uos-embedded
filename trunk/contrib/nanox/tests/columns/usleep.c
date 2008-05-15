# include <sys/time.h>

#ifdef TESTUSLEEP
main (argc, argv)
char **argv;
{
	register i, t;

	if (argc != 2) {
		printf ("Usage: usleep microsecs\n");
		exit (1);
	}
	t = atoi (argv [1]);
	for (i=0; ; ++i) {
		printf ("%d\n", i);
		usleep (t);
	}
}
#endif

int usleep (usec)
{
#ifdef I386
	/* uOS */
	extern timer_t *uos_timer;

	timer_delay (uos_timer, usec / 1000);
#else
	/* Unix */
	struct timeval tv;

	tv.tv_sec = usec / 1000000L;
	tv.tv_usec = usec % 1000000L;
	select (0, 0, 0, 0, &tv);
#endif
	return 0;
}
