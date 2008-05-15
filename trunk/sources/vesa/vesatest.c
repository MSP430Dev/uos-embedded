#include <dos.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * VESA generic information
 */
typedef struct {
	char 			signature [4];
	unsigned short		version;
	char far 		*oem_string;
	unsigned long		capabilities;
	unsigned short far 	*modes;
	unsigned short		memory;

	unsigned short		revision;
	char far 		*vendor_name;
	char far 		*product_name;
	char far 		*product_rev;

	char 			reserved [222+256];
} vesa_info_t;

/*
 * VESA mode information
 */
typedef struct {
	unsigned short		mode_attr;
	unsigned char		bank_a_attr;
	unsigned char		bank_b_attr;
	unsigned short		bank_granularity;
	unsigned short		bank_size;
	unsigned short		bank_a_segment;
	unsigned short		bank_b_segment;
	unsigned long		pos_func_ptr;
	unsigned short		bytes_per_scan_line;
	unsigned short		width;
	unsigned short		height;
	unsigned char		char_width;
	unsigned char		char_height;
	unsigned char		number_of_planes;
	unsigned char		bits_per_pixel;
	unsigned char		number_of_banks;
	unsigned char		memory_model;
	unsigned char		video_bank_size;
	unsigned char		image_pages;

	unsigned char		reserved_1;

	unsigned char		red_mask_size;
	unsigned char		red_field_pos;
	unsigned char		green_mask_size;
	unsigned char		green_field_pos;
	unsigned char		blue_mask_size;
	unsigned char		blue_field_pos;
	unsigned char		rsvd_mask_size;
	unsigned char		rsvd_field_pos;
	unsigned char		direct_color_info;

	unsigned long 		phys_base_ptr;
	unsigned long 		reserved_2;
	unsigned short		reserved_3;

	unsigned char		reserved [206];
} vesa_mode_info_t;

typedef struct {
	unsigned short	xres;
	unsigned short	yres;
	unsigned char	refresh;
} vesa_mode_t;

typedef struct {
	unsigned short	xres;
	unsigned short	yres;
	unsigned char	refresh;
	unsigned short	xsize;
	unsigned short	ysize;
	unsigned long	khz;
	unsigned short	xblank;
	unsigned short	yblank;
	unsigned short	hsync_len;
	unsigned short	vsync_len;
	unsigned short	right_margin;
	unsigned short	lower_margin;
	unsigned short	left_margin;
	unsigned short	upper_margin;
	unsigned char	hsync_positive;
	unsigned char	vsync_positive;
} vesa_extmode_t;

typedef struct {
	unsigned char	version;
	unsigned char	revision;
	unsigned char	manufacturer [4];
	unsigned short	model;
	unsigned long	serial;
	unsigned short	year;
	unsigned short	week;
	unsigned char	monitor [14];	/* Monitor String */
	unsigned char	serial_no [14];	/* Serial Number */
	unsigned char	ascii [14];	/* ? */
	unsigned long	hfmin;		/* hfreq lower limit (Hz) */
	unsigned long	hfmax;		/* hfreq upper limit (Hz) */
	unsigned short	vfmin;		/* vfreq lower limit (Hz) */
	unsigned short	vfmax;		/* vfreq upper limit (Hz) */
	unsigned long	dclkmax;	/* pixelclock upper limit (Hz) */
	unsigned char	gtf;		/* supports GTF */
	unsigned short	input;		/* display type - see FB_DISP_* */
#define FB_DISP_DDI		1
#define FB_DISP_ANA_700_300	2
#define FB_DISP_ANA_714_286	4
#define FB_DISP_ANA_1000_400	8
#define FB_DISP_ANA_700_000	16
#define FB_DISP_MONO		32
#define FB_DISP_RGB		64
#define FB_DISP_MULTI		128
#define FB_DISP_UNKNOWN		256

	unsigned short	signal;		/* signal Type - see FB_SIGNAL_* */
#define FB_SIGNAL_NONE		0
#define FB_SIGNAL_BLANK_BLANK	1
#define FB_SIGNAL_SEPARATE	2
#define FB_SIGNAL_COMPOSITE	4
#define FB_SIGNAL_SYNC_ON_GREEN	8
#define FB_SIGNAL_SERRATION_ON	16

	unsigned char	max_x;		/* Maximum horizontal size (cm) */
	unsigned char	max_y;		/* Maximum vertical size (cm) */
	unsigned short	gamma;		/* Gamma - in fractions of 100 */
	unsigned short	dpms;		/* DPMS support - see FB_DPMS_ */
#define FB_DPMS_ACTIVE_OFF	1
#define FB_DPMS_SUSPEND		2
#define FB_DPMS_STANDBY		4

	unsigned short	misc;		/* Misc flags - see FB_MISC_* */
#define FB_MISC_PRIM_COLOR	1
#define FB_MISC_1ST_DETAIL	2	/* First Detailed Timing is preferred */

	unsigned long	chroma_redx;	/* in fraction of 1024 */
	unsigned long	chroma_greenx;
	unsigned long	chroma_bluex;
	unsigned long	chroma_whitex;
	unsigned long	chroma_redy;
	unsigned long	chroma_greeny;
	unsigned long	chroma_bluey;
	unsigned long	chroma_whitey;
	vesa_mode_t	vesa_modes [17];
	vesa_mode_t	std_modes [8];
	vesa_extmode_t	detailed_modes [24];
} vesa_display_t;

/*
 * EDID structure.
 */
#define ID_MANUFACTURER_NAME			0x08

#define EDID_STRUCT_VERSION			0x12
#define EDID_STRUCT_REVISION			0x13
#define EDID_STRUCT_DISPLAY                     0x14

#define ESTABLISHED_TIMING_1			0x23

#define STD_TIMING_DESCRIPTIONS_START           0x26
#define STD_TIMING_DESCRIPTION_SIZE             2

#define DETAILED_TIMING_DESCRIPTIONS_START	0x36
#define DETAILED_TIMING_DESCRIPTION_SIZE	18

int
vesa_get_info (vesa_info_t *i)
{
	union REGS reg;
	struct SREGS sreg;

	reg.x.ax = 0x4f00;
	i->signature[0] = 'V';
	i->signature[1] = 'B';
	i->signature[2] = 'E';
	i->signature[3] = '2';
	sreg.es = FP_SEG (i);
	reg.x.di = FP_OFF (i);

	int86x (0x10, &reg, &reg, &sreg);
	return (reg.x.ax == 0x004f);
}

/*
 * Get details about a specific VESA video mode
 */
int
vesa_get_mode_info (unsigned short mode, vesa_mode_info_t *i)
{
	union REGS reg;
	struct SREGS sreg;

	reg.x.ax = 0x4f01;
	reg.x.cx = mode;
	sreg.es = FP_SEG (i);
	reg.x.di = FP_OFF (i);

	int86x (0x10, &reg, &reg, &sreg);
	return (reg.x.ax == 0x004f);
}

/*
 * Print the info you get using vbeGetInfo
 */
void
vesa_print_info (FILE *file, vesa_info_t *info)
{
	unsigned short *mode;

	fprintf (file, "Signature = `%c%c%c%c'\n",
		info->signature[0], info->signature[1],
		info->signature[2], info->signature[3]);

	fprintf (file, "Version = %d.%d\n",
		info->version >> 8, info->version & 255);
	fprintf (file, "OEM = %s\n", info->oem_string);
	fprintf (file, "Vendor = %s\n", info->vendor_name);
	fprintf (file, "Product Name = %s\n", info->product_name);
	fprintf (file, "Product Revision = %s\n", info->product_rev);
	fprintf (file, "Revision = %d.%d\n",
		info->revision >> 8, info->revision & 255);
	fprintf (file, "Capabilities = 0x%lX\n", info->capabilities);
	fprintf (file, "Memory = %ldk\n", info->memory * 64L);

	/* fprintf (file, "Modes =");
	for (mode=info->modes; *mode!=0xffff; ++mode)
		fprintf (file, " %X", *mode);
	fprintf (file, "\n"); */
	fflush (file);
}

/*
 * Print the info we get from vbeGetModeInfo
 */
void
vesa_print_mode_info (FILE *file, vesa_mode_info_t *info)
{
	fprintf (file, "%4d x %-4d - %d bits/pixel %d:%d:%d",
		info->width, info->height, info->bits_per_pixel,
		info->red_mask_size, info->green_mask_size,
		info->blue_mask_size);
	if (info->direct_color_info & 0x02)
		fprintf (file, ":%d", info->rsvd_mask_size);
	if (info->phys_base_ptr)
		fprintf (file, ", memory at 0x%lx", info->phys_base_ptr);
	fprintf (file, "\n");
	fflush (file);
}

void list_modes (vesa_info_t *i, unsigned char bpp)
{
	static vesa_mode_info_t mi;
	unsigned short *mode, n;

	n = 0;
	for (mode=i->modes; *mode!=0xffff; ++mode) {
		if (! vesa_get_mode_info (*mode, &mi))
			continue;

		/* Find direct color graphics modes
		 * with linear frame buffer. */
		if (mi.memory_model != 6 ||	/* direct color */
		    ! (mi.mode_attr & 0x01) ||	/* supported */
		    ! (mi.mode_attr & 0x08) ||	/* color */
		    ! (mi.mode_attr & 0x10) ||	/* graphics */
		    ! (mi.mode_attr & 0x80) ||	/* linear */
		    mi.bits_per_pixel != bpp)
			continue;
		if (n == 0)
			printf ("\nDirect color modes with %d bits per pixel:\n",
				bpp);
		printf ("%X - ", *mode);
		vesa_print_mode_info (stdout, &mi);
		++n;
	}
}

static void
copy_string (unsigned char *c, unsigned char *s)
{
	int i;

	c += 5;
	for (i=0; i<13 && *c != 0x0A; i++)
		*s++ = *c++;
	*s = 0;
	while (i-- && (*--s == 0x20))
		*s = 0;
}

static void
store_mode (vesa_mode_t *m, int xres, int yres, int refresh)
{
	m->xres = xres;
	m->yres = yres;
	m->refresh = refresh;
}

static void
get_vesa_modes (vesa_display_t *specs, unsigned char *block)
{
	vesa_mode_t *m = specs->vesa_modes;

	if (block[0] & 0x80) store_mode (m++, 720, 400, 70);
	if (block[0] & 0x40) store_mode (m++, 720, 400, 88);
	if (block[0] & 0x20) store_mode (m++, 640, 480, 60);
	if (block[0] & 0x10) store_mode (m++, 640, 480, 67);
	if (block[0] & 0x08) store_mode (m++, 640, 480, 72);
	if (block[0] & 0x04) store_mode (m++, 640, 480, 75);
	if (block[0] & 0x02) store_mode (m++, 800, 600, 56);
	if (block[0] & 0x01) store_mode (m++, 800, 600, 60);
	if (block[1] & 0x80) store_mode (m++, 800, 600, 72);
	if (block[1] & 0x40) store_mode (m++, 800, 600, 75);
	if (block[1] & 0x20) store_mode (m++, 832, 624, 75);
	/* Interlaced - skip it.
	if (block[1] & 0x10) store_mode (m++, 1024,768, 87);
	*/
	if (block[1] & 0x08) store_mode (m++, 1024,768, 60);
	if (block[1] & 0x04) store_mode (m++, 1024,768, 70);
	if (block[1] & 0x02) store_mode (m++, 1024,768, 75);
	if (block[1] & 0x01) store_mode (m++, 1280,1024,75);
	if (block[2] & 0x80) store_mode (m++, 1152,870, 75);
}

static int
get_mode (vesa_mode_t *m, unsigned char *block)
{
	m->xres = (block[0] + 31) * 8;
	if (m->xres <= 256) {
		m->xres = 0;
		return 0;
	}

	switch ((block[1] >> 6) & 3) {
	case 0: m->yres = m->xres;          break;
	case 1:	m->yres = m->xres * 3 / 4;  break;
	case 2: m->yres = m->xres * 4 / 5;  break;
	case 3:	m->yres = m->xres * 9 / 16; break;
	}
	m->refresh = (block[1] & 0x3f) + 60;
	return 1;
}

static void
get_extmode (vesa_extmode_t *m, unsigned char *block)
{
#define UPPER_NIBBLE(x) 	(((x) >> 4) & 15)
#define LOWER_NIBBLE(x) 	((x) & 15)
#define COMBINE_HI_8LO(hi,lo) 	(((hi) << 8) | (lo))
#define COMBINE_HI_4LO(hi,lo)	(((hi) << 4) | (lo))

#define PIXEL_KHZ(b)		(COMBINE_HI_8LO (b[1], b[0]) * 10L)

#define H_ACTIVE(b)		COMBINE_HI_8LO (UPPER_NIBBLE (b[4]), b[2])
#define H_BLANKING(b)		COMBINE_HI_8LO (LOWER_NIBBLE (b[4]), b[3])

#define V_ACTIVE(b)		COMBINE_HI_8LO (UPPER_NIBBLE (b[7]), b[5])
#define V_BLANKING(b)		COMBINE_HI_8LO (LOWER_NIBBLE (b[7]), b[6])

#define V_SYNC_WIDTH(b)		COMBINE_HI_4LO (b[11] & 3, LOWER_NIBBLE(b[10]))
#define V_SYNC_OFFSET(b)	COMBINE_HI_4LO ((b[11] >> 2) & 3, UPPER_NIBBLE(b[10]))

#define H_SYNC_WIDTH(b)		COMBINE_HI_4LO ((b[11] >> 4) & 3, b[9])
#define H_SYNC_OFFSET(b)	COMBINE_HI_4LO ((b[11] >> 6) & 3, b[8])

#define H_SIZE(b)		COMBINE_HI_8LO (UPPER_NIBBLE(b[14]), b[12])
#define V_SIZE(b)		COMBINE_HI_8LO (LOWER_NIBBLE(b[14]), b[13])

	m->khz = PIXEL_KHZ(block);

	m->xres = H_ACTIVE(block);
	m->yres = V_ACTIVE(block);

	m->xsize = H_SIZE(block);
	m->ysize = V_SIZE(block);

	m->xblank = H_BLANKING(block);
	m->yblank = V_BLANKING(block);

	m->hsync_len = H_SYNC_WIDTH(block);
	m->vsync_len = V_SYNC_WIDTH(block);

	m->right_margin = H_SYNC_OFFSET(block);
	m->lower_margin = V_SYNC_OFFSET(block);

	if (block[17] & 4)
		m->hsync_positive = 1;

	if (block[17] & 2)
		m->vsync_positive = 1;

	m->left_margin = m->xblank - m->right_margin - m->hsync_len;
	m->upper_margin = m->yblank - m->lower_margin - m->vsync_len;

	m->refresh = m->khz * 1000 /
		((long) (m->xres + m->xblank) * (m->yres + m->yblank));
}

static void
get_std_modes (vesa_display_t *specs, unsigned char *block)
{
	vesa_mode_t *m = specs->std_modes;
	int i;

	for (i=0; i<8; i++, block += STD_TIMING_DESCRIPTION_SIZE)
		m += get_mode (m, block);
}

static void
get_detailed_modes (vesa_display_t *specs, unsigned char *block)
{
	vesa_extmode_t *m = specs->detailed_modes;
	unsigned char *subblock;
	int i, j;

	for (i=0; i<4; i++, block += DETAILED_TIMING_DESCRIPTION_SIZE) {
		if (block[0] == 0x00 && block[1] == 0x00) {
			if (block[3] == 0xfa) {
				subblock = block + 5;
				for (j=0; j<6; j++, subblock += STD_TIMING_DESCRIPTION_SIZE)
					m += get_mode ((vesa_mode_t*) m, subblock);
			}
		} else  {
			get_extmode (m, block);
			++m;
		}
	}
}

int
vesa_get_display_info (vesa_display_t *specs)
{
	union REGS reg;
	struct SREGS sreg;
	unsigned char edid [128];
	unsigned char *block, c;
	int i;

	reg.x.ax = 0x4f15;
	reg.x.bx = 0;
	reg.x.cx = 0;
	sreg.es = 0;
	reg.x.di = 0;

	int86x (0x10, &reg, &reg, &sreg);
	if (reg.x.ax != 0x004f)
		return 0;
	/* printf ("DDC Level = %d\n", reg.h.bl); */
	/* printf ("EDID Transfer Time = %d sec\n", reg.h.bh); */

	reg.x.ax = 0x4f15;
	reg.x.bx = 1;
	reg.x.cx = 0;
	reg.x.dx = 0;
	sreg.es = FP_SEG (edid);
	reg.x.di = FP_OFF (edid);

	int86x (0x10, &reg, &reg, &sreg);
	if (reg.x.ax != 0x004f)
		return 0;

	/* Decode EDID. */
	memset (specs, 0, sizeof (*specs));
	specs->version = edid [EDID_STRUCT_VERSION];
	specs->revision = edid [EDID_STRUCT_REVISION];

	specs->manufacturer[0] = ((edid[ID_MANUFACTURER_NAME+0] & 0x7c) >> 2) + '@';
	specs->manufacturer[1] = ((edid[ID_MANUFACTURER_NAME+0] & 0x03) << 3) +
		((edid[ID_MANUFACTURER_NAME+1] & 0xe0) >> 5) + '@';
	specs->manufacturer[2] = (edid[ID_MANUFACTURER_NAME+1] & 0x1f) + '@';
	specs->manufacturer[3] = 0;
	specs->model = edid[ID_MANUFACTURER_NAME+2] + (edid[ID_MANUFACTURER_NAME+3] << 8);
	specs->serial = edid[ID_MANUFACTURER_NAME+4] + (edid[ID_MANUFACTURER_NAME+5] << 8) +
	       (edid[ID_MANUFACTURER_NAME+6] << 16) + (edid[ID_MANUFACTURER_NAME+7] << 24);
	specs->year = edid[ID_MANUFACTURER_NAME+9] + 1990;
	specs->week = edid[ID_MANUFACTURER_NAME+8];

	block = edid + DETAILED_TIMING_DESCRIPTIONS_START;
	for (i=0; i<4; i++, block += DETAILED_TIMING_DESCRIPTION_SIZE) {
		if (block[0] != 0 || block[1] != 0 ||
		    block[2] != 0 || block[4] != 0)
			continue;
		switch (block[3]) {
		case 0xff:
			copy_string (block, specs->serial_no);
			break;
		case 0xfe:
			copy_string (block, specs->ascii);
			break;
		case 0xfc:
			copy_string(block, specs->monitor);
			break;
		case 0xfd:
			specs->hfmin = block[7] * 1000L;
			specs->hfmax = block[8] * 1000L;
			specs->vfmin = block[5];
			specs->vfmax = block[6];
			specs->dclkmax = block[9] * 10000000L;
			specs->gtf = block[10] ? 1 : 0;
			break;
		}
	}

	block = edid + EDID_STRUCT_DISPLAY;
	c = block[0] & 0x80;
	if (c) {
		specs->input |= FB_DISP_DDI;
	} else {
		switch ((block[0] & 0x60) >> 5) {
		case 0: specs->input |= FB_DISP_ANA_700_300;  break;
		case 1: specs->input |= FB_DISP_ANA_714_286;  break;
		case 2: specs->input |= FB_DISP_ANA_1000_400; break;
		case 3: specs->input |= FB_DISP_ANA_700_000;  break;
		}
	}

	c = block[0] & 0x0f;
	if (c & 0x10) specs->signal |= FB_SIGNAL_BLANK_BLANK;
	if (c & 0x08) specs->signal |= FB_SIGNAL_SEPARATE;
	if (c & 0x04) specs->signal |= FB_SIGNAL_COMPOSITE;
	if (c & 0x02) specs->signal |= FB_SIGNAL_SYNC_ON_GREEN;
	if (c & 0x01) specs->signal |= FB_SIGNAL_SERRATION_ON;

	specs->max_x = block[1];
	specs->max_y = block[2];
	specs->gamma = block[3] + 100;

	c = block[4];
	if (c & (1 << 5)) specs->dpms |= FB_DPMS_ACTIVE_OFF;
	if (c & (1 << 6)) specs->dpms |= FB_DPMS_SUSPEND;
	if (c & (1 << 7)) specs->dpms |= FB_DPMS_STANDBY;

	switch ((block[4] & 0x18) >> 3) {
	case 0:	 specs->input |= FB_DISP_MONO;	  break;
	case 1:	 specs->input |= FB_DISP_RGB;	  break;
	case 2:	 specs->input |= FB_DISP_MULTI;	  break;
	default: specs->input |= FB_DISP_UNKNOWN; break;
	}

	/* Chromaticity data */
	i = ((block[5] >> 6) & 3) | (block[7] << 2);
	specs->chroma_redx = (i*1000L + 512) / 1024;

	i = ((block[5] >> 4) & 3) | (block[8] << 2);
	specs->chroma_redy = (i*1000L + 512) / 1024;

	i = ((block[5] >> 2) & 3) | (block[9] << 2);
	specs->chroma_greenx = (i*1000L + 512) / 1024;

	i = (block[5] & 3) | (block[10] << 2);
	specs->chroma_greeny = (i*1000L + 512) / 1024;

	i = ((block[6] >> 6) & 3) | (block[11] << 2);
	specs->chroma_bluex = (i*1000L + 512) / 1024;

	i = ((block[6] >> 4) & 3) | (block[12] << 2);
	specs->chroma_bluey = (i*1000L + 512) / 1024;

	i = ((block[6] >> 2) & 3) | (block[13] << 2);
	specs->chroma_whitex = (i*1000L + 512) / 1024;

	i = (block[6] & 3) | (block[14] << 2);
	specs->chroma_whitey = (i*1000L + 512) / 1024;

	c = block[4] & 0x7;
	if (c & 0x04) specs->misc |= FB_MISC_PRIM_COLOR;
	if (c & 0x02) specs->misc |= FB_MISC_1ST_DETAIL;
	if (c & 0x01) specs->gtf = 1;

	get_vesa_modes (specs, edid + ESTABLISHED_TIMING_1);
	get_std_modes (specs, edid + STD_TIMING_DESCRIPTIONS_START);
	get_detailed_modes (specs, edid + DETAILED_TIMING_DESCRIPTIONS_START);
	return 1;
}

void
vesa_print_mode (vesa_mode_t *m)
{
	if (! m->xres)
		return;
	printf ("      %d x %d at %d Hz\n",
		m->xres, m->yres, m->refresh);
}

void
vesa_print_extmode (vesa_extmode_t *m)
{
	if (! m->xres)
		return;

	if (! m->khz) {
		vesa_print_mode ((vesa_mode_t*) m);
		return;
	}
	printf ("      %d x %d at %d Hz, size %d x %d\n",
		m->xres, m->yres, m->refresh, m->xsize, m->ysize);
	printf ("      %d MHz ", (int) (m->khz / 1000));
	printf ("%d %d %d %d ", m->xres, m->xres + m->right_margin,
		m->xres + m->right_margin + m->hsync_len,
		m->xres + m->xblank);
	printf ("%d %d %d %d ", m->yres, m->yres + m->lower_margin,
		m->yres + m->lower_margin + m->vsync_len,
		m->yres + m->yblank);
	printf ("%sHSync %sVSync\n",
		m->hsync_positive ? "+" : "-",
		m->vsync_positive ? "+" : "-");
}

void
vesa_print_display (vesa_display_t *specs)
{
	int i;

	printf ("   EDID Version %d.%d\n", (int) specs->version,
	       (int) specs->revision);
	printf ("   Manufacturer: %s\n", specs->manufacturer);
	printf ("   Model: %x\n", specs->model);
	printf ("   Serial#: %u\n", specs->serial);
	printf ("   Year: %u Week %u\n", specs->year, specs->week);

	if (specs->monitor[0])
		printf ("   Monitor Name: %s\n", specs->monitor);
	if (specs->serial_no[0])
		printf ("   Serial Number: %s\n", specs->serial_no);
	if (specs->ascii[0])
		printf ("   ASCII Block: %s\n", specs->ascii);

	printf ("   Display Characteristics:\n");
	if (specs->vfmax != 0) {
		printf ("      Monitor Operating Limits: ");
		printf ("H: %d-%d KHz  V: %d-%d Hz  DCLK: %d MHz\n",
			(int) (specs->hfmin / 1000),
			(int) (specs->hfmax / 1000),
			specs->vfmin, specs->vfmax,
			(int) (specs->dclkmax / 1000000));
	}
	if (specs->input & FB_DISP_DDI) {
		printf ("      Digital Display Input");
	} else {
		printf ("      Analog Display Input: Input Voltage - ");
		if (specs->input & FB_DISP_ANA_700_300)
			printf ("0.700V/0.300V");
		else if (specs->input & FB_DISP_ANA_714_286)
			printf ("0.714V/0.286V");
		else if (specs->input & FB_DISP_ANA_1000_400)
			printf ("1.000V/0.400V");
		else if (specs->input & FB_DISP_ANA_700_000)
			printf ("0.700V/0.000V");
		else
			printf ("???");
		printf ("\n");
	}

	printf ("      Sync: ");
	if (specs->signal & FB_SIGNAL_BLANK_BLANK)
		printf ("Blank to Blank ");
	if (specs->signal & FB_SIGNAL_SEPARATE)
		printf ("Separate ");
	if (specs->signal & FB_SIGNAL_COMPOSITE)
		printf ("Composite ");
	if (specs->signal & FB_SIGNAL_SYNC_ON_GREEN)
		printf ("Sync on Green ");
	if (specs->signal & FB_SIGNAL_SERRATION_ON)
		printf ("Serration on ");
	printf ("\n");

	if (specs->max_x && specs->max_y)
		printf ("      Max. Image Size: %d x %d centimeters\n",
			specs->max_x, specs->max_y);

	printf ("      Gamma: %d.%d\n", specs->gamma/100, specs->gamma % 100);

	printf ("      DPMS: Active %s, Suspend %s, Standby %s\n",
	       (specs->dpms & FB_DPMS_ACTIVE_OFF) ? "yes" : "no",
	       (specs->dpms & FB_DPMS_SUSPEND)    ? "yes" : "no",
	       (specs->dpms & FB_DPMS_STANDBY)    ? "yes" : "no");

	if (specs->input & FB_DISP_MONO)
		printf ("      Monochrome/Grayscale\n");
	else if (specs->input & FB_DISP_RGB)
		printf ("      RGB Color Display\n");
	else if (specs->input & FB_DISP_MULTI)
		printf ("      Non-RGB Multicolor Display\n");
	else if (specs->input & FB_DISP_UNKNOWN)
		printf ("      Unknown\n");

	printf ("      Chroma\n");
	printf ("         Red: 0.%03d x 0.%03d\n",
		specs->chroma_redx, specs->chroma_redy);
	printf ("         Green: 0.%03d x 0.%03d\n",
		specs->chroma_greenx, specs->chroma_greeny);
	printf ("         Blue: 0.%03d x 0.%03d\n",
		specs->chroma_bluex, specs->chroma_bluey);
	printf ("         White: 0.%03d x 0.%03d\n",
		specs->chroma_whitex, specs->chroma_whitey);

        if (specs->misc & FB_MISC_PRIM_COLOR)
                printf ("      Default color format is primary\n");
        if (specs->misc & FB_MISC_1ST_DETAIL)
                printf ("      First DETAILED Timing is preferred\n");
        if (specs->gtf)
                printf ("      Display is GTF capable\n");

	printf ("   Supported VESA Modes\n");
	for (i=0; i<17; ++i)
		vesa_print_mode (specs->vesa_modes + i);

	printf ("   Standard Timings\n");
	for (i=0; i<8; ++i)
		vesa_print_mode (specs->std_modes + i);

	printf ("   Detailed Timings\n");
	for (i=0; i<24; ++i)
		vesa_print_extmode (specs->detailed_modes + i);
}

int main ()
{
	static vesa_info_t info;
	static vesa_display_t display;

	if (! vesa_get_info (&info)) {
		fprintf (stderr, "No VESA found.\n");
		exit (-1);
	}
	vesa_print_info (stdout, &info);

	/* List all supported direct color linear graphics modes
	 * with a given number of bits per pixel. */
	list_modes (&info, 15);
	list_modes (&info, 16);
	list_modes (&info, 24);
	list_modes (&info, 32);

	printf ("\n========================================\n");
	printf ("Display Information (EDID)\n");
	printf ("========================================\n");
	if (vesa_get_display_info (&display))
		vesa_print_display (&display);
	printf ("========================================\n");
	return 0;
}
