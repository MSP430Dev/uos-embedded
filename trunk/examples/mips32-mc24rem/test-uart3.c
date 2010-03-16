#include <sys/ioctl.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <signal.h>

#define IOSZ	256
#define BAUD	115200
#define P_AA55	1
#define MAXCHAN	8
#define	TXBUF	2043

typedef struct {
	char *name;
	int fd;
	int ready_to_transmit;
	int ready_to_receive;
	struct termios old_options;
	long packets_received, packets_transmitted;
	long bytes_received, bytes_transmitted;
	long error_bytes;
	int it;
	int ir;
} chan_data;

chan_data chan [MAXCHAN];

unsigned char tbuf [2*IOSZ], rbuf [IOSZ];
long total_packets_received, total_packets_transmitted;
long total_bytes_received, total_bytes_transmitted;
long total_error_bytes;
int baud = BAUD;
struct timespec t0;

void quit (int sig)
{
	chan_data *c;

	for (c=chan; c<chan+MAXCHAN; ++c)
		if (c->fd > 0)
			tcsetattr (c->fd, TCSAFLUSH, &c->old_options);
	exit (0);
}

void print_stats ()
{
	static struct timespec last;
	struct timespec now;
	long sec, msec;
	int tx_bytesec, rx_bytesec;
	chan_data *c;

	clock_gettime (CLOCK_REALTIME, &now);

	/* Обновляем 5 раз в секунду. */
	msec = (now.tv_sec - last.tv_sec) * 1000 +
		(now.tv_nsec - last.tv_nsec) / 1000000;
	if (msec < 200)
		return;
	last = now;

	msec = (now.tv_sec - t0.tv_sec) * 1000 +
		(now.tv_nsec - t0.tv_nsec) / 1000000;
	sec = (msec + 500) / 1000;
	if (msec < 1)
		msec = 1;
	if (sec < 1)
		sec = 1;
#if 0
	for (c=chan; c<chan+MAXCHAN; ++c) {
		if (c->fd <= 0)
			continue;
		printf ("%s: transmit %ld, receive %ld, errors %ld\n", c->name,
			c->bytes_transmitted, c->bytes_received, c->error_bytes);
	}
#else
	printf ("\33[H");
	for (c=chan; c<chan+MAXCHAN; ++c) {
		if (c->fd <= 0)
			continue;
		printf ("%s:\n", c->name);
		printf ("    Bytes: transmit %ld, receive %ld, errors %ld\n",
			c->bytes_transmitted, c->bytes_received, c->error_bytes);


		if (c->bytes_transmitted < TXBUF)
			tx_bytesec = 0;
		else if (c->bytes_transmitted < 2000000)
			tx_bytesec = (c->bytes_transmitted - TXBUF) * 1000 / msec;
		else
			tx_bytesec = (c->bytes_transmitted - TXBUF) / sec;

		if (c->bytes_received < 2000000)
			rx_bytesec = c->bytes_received * 1000 / msec;
		else
			rx_bytesec = c->bytes_received / sec;

		printf ("    Speed: transmit %d%%, receive %d%%\33[K\n",
			tx_bytesec * 1000 / baud, rx_bytesec * 1000 / baud);
	}
#endif
	fflush (stdout);
}

void usage ()
{
}

int main (int argc, char ** argv)
{
	int i, nchan, s, nready, k;
	chan_data *c;
	struct termios opt;

	for (i=0; i<sizeof(tbuf); ++i)
		tbuf[i] = i;
	while ((s = getopt (argc, argv, "b:f:a:c")) != -1)
		switch (s) {
		case 'b':
			baud = atoi (optarg);
			break;
		case 'f':
			k = strtol (optarg, 0, 16) & 0xff;
			for (i=0; i<sizeof(tbuf); ++i)
				tbuf[i] = k;
			break;
		case 'a':
			k = strtol (optarg, 0, 16) & 0xff;
			for (i=0; i<sizeof(tbuf); ++i)
				tbuf[i] = (i & 1) ? ~k : k;
			break;
		case 'c':
			for (i=0; i<sizeof(tbuf); ++i)
				tbuf[i] = i;
			break;
		default:
			printf ("Invalid option '%c'.\n", s);
usage:			system ("use diagser");
			exit (0);
	}
	argc -= optind;
	argv += optind;
	if (argc < 1)
		goto usage;

	signal (SIGINT, quit);
	signal (SIGTERM, quit);
	signal (SIGQUIT, quit);

	nchan = 0;
	for (i=0; i<argc && i<MAXCHAN; ++i, ++argv) {
		chan[i].name = *argv;
		if (strchr (chan[i].name, '/') == 0) {
			char buf [256];
			strcpy (buf, "/dev/");
			strcat (buf, chan[i].name);
			chan[i].name = strdup (buf);
			if (! chan[i].name) {
				printf ("Out of memory.\n");
				exit (-1);
			}
		}

		chan[i].fd = open (chan[i].name, O_RDWR | O_NONBLOCK);
		if (chan[i].fd < 0) {
			fprintf (stderr, "%s: ", chan[i].name);
			perror ("open");
			continue;
		}
		if (tcgetattr (chan[i].fd, &chan[i].old_options) < 0) {
			fprintf (stderr, "%s: ", chan[i].name);
			perror ("tcgetattr");
			close (chan[i].fd);
			chan[i].fd = -1;
			continue;
		}

		opt = chan[i].old_options;
		cfmakeraw (&opt);
		cfsetospeed (&opt, baud);
		cfsetispeed (&opt, baud);
		opt.c_cflag = CREAD | CS8;
		opt.c_lflag &= ~ICANON;
		opt.c_cc[VMIN] = 0;
		opt.c_cc[VTIME] = 1;
		if (tcsetattr (chan[i].fd, TCSANOW, &opt) < 0) {
			fprintf (stderr, "%s: ", chan[i].name);
			perror ("tcsetattr");
			close (chan[i].fd);
			chan[i].fd = -1;
			continue;
		}
		++nchan;
	}
	if (nchan <= 0) {
		printf ("No channels to test.\n");
		exit (-1);
	}
	printf ("Started %d channels.\n", nchan);

	if (tbuf[0] == 0 && tbuf[1] == 1)
		printf ("Pattern: counter\n");
	else
		printf ("Pattern: %02x-%02x\n", tbuf[0], tbuf[1]);

	printf ("\33[H\33[J");
	clock_gettime (CLOCK_REALTIME, &t0);
	for (;;) {
		print_stats ();

		for (c=chan; c<chan+MAXCHAN; ++c) {
			if (c->fd <= 0)
				continue;
			c->ready_to_transmit = 1;
			c->ready_to_receive = 1;
		}
		nready = 0;
		for (c=chan; c<chan+MAXCHAN; ++c) {
			if (c->fd <= 0)
				continue;
			if (c->ready_to_transmit) {
				s = write (c->fd, tbuf + c->it, IOSZ - c->it);
				if (s <= 0) {
					if (s < 0) {
						if (errno != EAGAIN)
							perror ("\nwrite");
						if (errno == EIO)
							quit (0);
					}
					c->ready_to_transmit = 0;
				} else {
					++c->packets_transmitted;
					++total_packets_transmitted;
					c->bytes_transmitted += s;
					total_bytes_transmitted += s;
					c->it = (c->it + s) % IOSZ;
					++nready;
				}
			}
			if (! c->ready_to_receive)
				continue;
			s = read (c->fd, rbuf, sizeof (rbuf));
			if (s <= 0) {
				if (s < 0) {
					if (errno != EWOULDBLOCK)
						perror ("\nread");
					if (errno == EIO)
						quit (0);
				}
				c->ready_to_receive = 0;
				continue;
			}
			for (k = 0; k<s; k++) {
				int irs = c->ir;
				if (rbuf[k] != tbuf[irs]) {
/*printf ("Error %s byte %ld: expected %02x, received %02x\n", c->name, c->bytes_received + k, tbuf[irs], rbuf[k]);*/
					do {
						irs++;
					} while (rbuf[k] != tbuf[irs] &&
					    (irs - c->ir) % IOSZ != 0);
					c->error_bytes++;
					total_error_bytes++;
				}
				c->ir = (irs + 1) % IOSZ;
			}
			++c->packets_received;
			++total_packets_received;
			c->bytes_received += s;
			total_bytes_received += s;
			++nready;
		}
	}
	return 0;
}
