#include <runtime/lib.h>
#include <buf/buf.h>

static int
print_arp_header (unsigned char *data)
{
	int hlen;

	debug_printf ("\tarp:\thwtype = %04x", data[0] << 8 | data[1]);
	if (data[0] == 0 && data[1] == 1)
		debug_printf (" (Ethernet)");
	debug_printf (", proto = %02x-%02x", data[2], data[3]);
	hlen = 4;
	if (data[2] == 8 && data[3] == 0) {
		debug_printf (" (IP)\n");
		hlen += 24;
		debug_printf ("\t\thwlen = %d, protolen = %d",
			data[4], data[5]);
		debug_printf (", opcode = %04x", data[6] << 8 | data[7]);
		if (data[6] == 0 && data[7] == 1)
			debug_printf (" (ARP Request)");
		else if (data[6] == 0 && data[7] == 2)
			debug_printf (" (ARP Reply)");
		debug_printf ("\n\t\tsrc_hw = %02x-%02x-%02x-%02x-%02x-%02x",
			data[8], data[9], data[10], data[11], data[12], data[13]);
		debug_printf (", src_ip = %d.%d.%d.%d\n",
			data[14], data[15], data[16], data[17]);
		debug_printf ("\t\tdst_hw = %02x-%02x-%02x-%02x-%02x-%02x",
			data[18], data[19], data[20], data[21], data[22], data[23]);
		debug_printf (", dst_ip = %d.%d.%d.%d\n",
			data[24], data[25], data[26], data[27]);
	} else {
		debug_putchar (0, '\n');
	}
	return hlen;
}

static int
print_ip_header (unsigned char *data)
{
	int header_length;

	debug_printf ("\tip:\tversion = %02x", data[0] >> 4);
	header_length = 4 * (data[0] & 0x0f);
	debug_printf (", header length = %d bytes\n", header_length);

	debug_printf ("\t\ttype of service = %02x", data[1]);
	debug_printf (", total length = %d bytes\n", data[2] << 8 | data[3]);

	debug_printf ("\t\tidentification = %04x", data[4] << 8 | data[5]);
	debug_printf (", flags = %x", data[6] >> 4);
	if (data[6] & 0x20)
		debug_printf (" DF");
	if (data[6] & 0x40)
		debug_printf (" MF");
	debug_printf (", fragment offset = %d bytes\n",
		(data[6] << 8 & 0xf00) | data[7]);

	debug_printf ("\t\tTTL = %02x", data[8]);
	debug_printf (", protocol = %02x", data[9]);
	switch (data[9]) {
	case 1:  debug_printf (" (ICMP)"); break;
	case 6:  debug_printf (" (TCP)");  break;
	case 17: debug_printf (" (UDP)");  break;
	}
	debug_printf (", header checksum = %04x\n", data[10] << 8 | data[11]);

	debug_printf ("\t\tsrc = %d.%d.%d.%d",
		data[12], data[13], data[14], data[15]);
	debug_printf (", dest = %d.%d.%d.%d\n",
		data[16], data[17], data[18], data[19]);

	return header_length;
}

static int
print_icmp_header (unsigned char *data)
{
	debug_printf ("\ticmp:\ttype = %02x", data[0]);
	switch (data[0]) {
	case 0:  debug_printf (" (Echo reply)");           break;
	case 8:  debug_printf (" (Echo request)");         break;
	case 13: debug_printf (" (Timestamp request)");    break;
	case 15: debug_printf (" (Information request)");  break;
	case 17: debug_printf (" (Address Mask request)"); break;
	}
	debug_printf (", code = %02x", data[1]);
	debug_printf (", checksum = %04x\n", data[2] << 8 | data[3]);

	debug_printf ("\t\tidentifier = %04x", data[4] << 8 | data[5]);
	debug_printf (", sequence number = %04x\n", data[6] << 8 | data[7]);
	return 8;
}

static int
print_tcp_header (unsigned char *data)
{
	static const char flag_bits[] = "\20\1FIN\2SYN\3RST\4PSH\5ACK\6URG";

	debug_printf ("\ttcp:\tsrc port = %u, dest port = %u\n",
		data[0] << 8 | data[1], data[2] << 8 | data[3]);
	debug_printf ("\t\tseqno = %lu, ackno = %lu\n",
		(unsigned long) data[4] << 24 |
		(unsigned long) data[5] << 16 | data[6] << 8 | data[7],
		(unsigned long) data[8] << 24 |
		(unsigned long) data[9] << 16 | data[10] << 8 | data[11]);
	debug_printf ("\t\toffset = %d, flags = %b, wnd = %u\n",
		data[12], data[13], flag_bits, data[14] << 8 | data[15]);
	debug_printf ("\t\tchecksum = %04x, urgp = %u\n",
		data[16] << 8 | data[17], data[18] << 8 | data[19]);
	return 20;
}

static int
print_udp_header (unsigned char *data)
{
	debug_printf ("\tudp:\tsrc port = %d, dest port = %d\n",
		data[0] << 8 | data[1], data[2] << 8 | data[3]);
	debug_printf ("\t\tlength = %d, checksum = %04x\n",
		data[4] << 8 | data[5], data[6] << 8 | data[7]);
	return 8;
}

void
buf_print_data (unsigned char *data, int len)
{
	if (len <= 0)
		return;
	debug_printf ("\tdata:\t%02x", *data++);
	while (--len > 0) {
		debug_printf ("-%02x", *data++);
	}
	debug_putchar (0, '\n');
}

void
buf_print (buf_t *p)
{
	buf_print_data (p->payload, p->tot_len);
}

void
buf_print_ethernet (buf_t *p)
{
	int hlen = 14;

	debug_printf ("\teth:\tdest = %02x-%02x-%02x-%02x-%02x-%02x",
		p->payload[0], p->payload[1], p->payload[2], p->payload[3], p->payload[4], p->payload[5]);
	debug_printf (", src = %02x-%02x-%02x-%02x-%02x-%02x\n",
		p->payload[6], p->payload[7], p->payload[8], p->payload[9], p->payload[10], p->payload[11]);
	debug_printf ("\t\ttype = %02x-%02x", p->payload[12], p->payload[13]);

	if (p->payload[12] == 8 && p->payload[13] == 0) {
		debug_printf (" (IP)\n");
		hlen += print_ip_header (p->payload + hlen);
		switch (p->payload [14+9]) {
		case 1:  hlen += print_icmp_header (p->payload + hlen); break;
		case 6:  hlen += print_tcp_header (p->payload + hlen);  break;
		case 17: hlen += print_udp_header (p->payload + hlen);  break;
		}
	} else if (p->payload[12] == 8 && p->payload[13] == 6) {
		debug_printf (" (ARP)\n");
		hlen += print_arp_header (p->payload + hlen);
	} else {
		debug_putchar (0, '\n');
	}
	buf_print_data (p->payload + hlen, p->tot_len - hlen);
}

void
buf_print_ip (buf_t *p)
{
	int hlen = 0;

	hlen = print_ip_header (p->payload);
	switch (p->payload [9]) {
	case 1:  hlen += print_icmp_header (p->payload + hlen); break;
	case 6:  hlen += print_tcp_header (p->payload + hlen);  break;
	case 17: hlen += print_udp_header (p->payload + hlen);  break;
	}
	buf_print_data (p->payload + hlen, p->tot_len - hlen);
}

void
buf_print_tcp (buf_t *p)
{
	int hlen;

	hlen = print_tcp_header (p->payload);
	buf_print_data (p->payload + hlen, p->tot_len - hlen);
}
