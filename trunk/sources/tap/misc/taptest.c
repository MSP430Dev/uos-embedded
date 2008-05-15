#include <stdio.h>
#include <fcntl.h>

#define DEVNAME	"/dev/tap0"
#define MAXLEN	2048

unsigned char data [MAXLEN];

void print_eth_header (unsigned char *data)
{
	printf ("\teth:\tdest = %02x-%02x-%02x-%02x-%02x-%02x",
		data[0], data[1], data[2], data[3], data[4], data[5]);
	printf (", src = %02x-%02x-%02x-%02x-%02x-%02x\n",
		data[6], data[7], data[8], data[9], data[10], data[11]);
	printf ("\t\ttype = %02x-%02x", data[12], data[13]);
	if (data[12] == 8 && data[13] == 0)
		printf (" (IP)");
	else if (data[12] == 8 && data[13] == 6)
		printf (" (ARP)");
	printf ("\n");
}

int print_ip_header (unsigned char *data)
{
	int header_length;

	printf ("\tip:\tversion = %02x", data[0] >> 4);
	header_length = 4 * (data[0] & 0x0f);
	printf (", header length = %d bytes\n", header_length);

	printf ("\t\ttype of service = %02x", data[1]);
	printf (", total length = %d bytes\n", data[2] << 8 | data[3]);

	printf ("\t\tidentification = %04x", data[4] << 8 | data[5]);
	printf (", flags = %x", data[6] >> 4);
	if (data[6] & 0x20)
		printf (" DF");
	if (data[6] & 0x40)
		printf (" MF");
	printf (", fragment offset = %d bytes\n", data[6] << 8 & 0xf00 | data[7]);

	printf ("\t\tTTL = %02x", data[8]);
	printf (", protocol = %02x", data[9]);
	switch (data[9]) {
	case 1:  printf (" (ICMP)"); break;
	case 6:  printf (" (TCP)");  break;
	case 17: printf (" (UDP)");  break;
	}
	printf (", header checksum = %04x\n", data[10] << 8 | data[11]);

	printf ("\t\tsrc = %d.%d.%d.%d",
		data[12], data[13], data[14], data[15]);
	printf (", dest = %d.%d.%d.%d\n",
		data[16], data[17], data[18], data[19]);

	return header_length;
}

int print_icmp_header (unsigned char *data)
{
	printf ("\ticmp:\ttype = %02x", data[0]);
	switch (data[0]) {
	case 0:  printf (" (Echo reply)");           break;
	case 8:  printf (" (Echo request)");         break;
	case 13: printf (" (Timestamp request)");    break;
	case 15: printf (" (Information request)");  break;
	case 17: printf (" (Address Mask request)"); break;
	}
	printf (", code = %02x", data[1]);
	printf (", checksum = %04x\n", data[2] << 8 | data[3]);

	printf ("\t\tidentifier = %04x", data[4] << 8 | data[5]);
	printf (", sequence number = %04x\n", data[6] << 8 | data[7]);
	return 8;
}

int print_tcp_header (unsigned char *data)
{
	return 0;
}

int print_udp_header (unsigned char *data)
{
	return 0;
}

void print_data (unsigned char *data, int len)
{
	if (len <= 0)
		return;
	printf ("\tdata:\t%02x", *data++);
	while (--len > 0) {
		printf ("-%02x", *data++);
	}
	printf ("\n");
}

int main (void)
{
	int fd, len, hlen;

	fd = open (DEVNAME, O_RDWR);
        if (fd < 0) {
                perror (DEVNAME);
                exit (1);
        }

	for (;;) {
		len = read (fd, data, sizeof (data));
		if (len < 0) {
			perror ("read");
			exit (1);
		}
		printf ("read %d bytes, prefix = %02x-%02x\n",
			len, data[0], data[1]);
		print_eth_header (data + 2);
		hlen = 16;
		hlen += print_ip_header (data + hlen);
		switch (data[16+9]) {
		case 1:  hlen += print_icmp_header (data + hlen); break;
		case 6:  hlen += print_tcp_header (data + hlen);  break;
		case 17: hlen += print_udp_header (data + hlen);  break;
		}
		print_data (data + hlen, len - hlen);
	}
	return 0;
}
