#include <runtime/lib.h>
/*#include <kernel/uos.h>*/
#include <vesa/vesa.h>
#include <runtime/i386/int86.h>

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

static int
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

	m->xres = H_ACTIVE(block);
	if (m->xres <= 1) {
		m->xres = 0;
		return 0;
	}
	m->yres = V_ACTIVE(block);

	m->khz = PIXEL_KHZ(block);

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
	return 1;
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
			m += get_extmode (m, block);
			++m;
		}
	}
}

int
vesa_get_display_info (vesa_display_t *specs)
{
	int86_regs_t reg;
	unsigned char *edid = (unsigned char*) 0x1000;
	unsigned char *block, c;
	int i;

	reg.x.ax = 0x4f15;
	reg.x.bx = 0;
	reg.x.cx = 0;
	reg.x.es = 0;
	reg.x.di = 0;

	int86 (0x10, &reg, &reg);
	if (reg.x.ax != 0x004f)
		return 0;
	/* printf ("DDC Level = %d\n", reg.h.bl); */
	/* printf ("EDID Transfer Time = %d sec\n", reg.h.bh); */

	reg.x.ax = 0x4f15;
	reg.x.bx = 1;
	reg.x.cx = 0;
	reg.x.dx = 0;

	/* Use memory at address 0x1000. */
	reg.x.di = 0;
	reg.x.es = 0x100;

	int86 (0x10, &reg, &reg);
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
		specs->input |= VESA_DISP_DDI;
	} else {
		switch ((block[0] & 0x60) >> 5) {
		case 0: specs->input |= VESA_DISP_ANA_700_300;  break;
		case 1: specs->input |= VESA_DISP_ANA_714_286;  break;
		case 2: specs->input |= VESA_DISP_ANA_1000_400; break;
		case 3: specs->input |= VESA_DISP_ANA_700_000;  break;
		}
	}

	c = block[0] & 0x0f;
	if (c & 0x10) specs->signal |= VESA_SIGNAL_BLANK_BLANK;
	if (c & 0x08) specs->signal |= VESA_SIGNAL_SEPARATE;
	if (c & 0x04) specs->signal |= VESA_SIGNAL_COMPOSITE;
	if (c & 0x02) specs->signal |= VESA_SIGNAL_SYNC_ON_GREEN;
	if (c & 0x01) specs->signal |= VESA_SIGNAL_SERRATION_ON;

	specs->max_x = block[1];
	specs->max_y = block[2];
	specs->gamma = block[3] + 100;

	c = block[4];
	if (c & (1 << 5)) specs->dpms |= VESA_DPMS_ACTIVE_OFF;
	if (c & (1 << 6)) specs->dpms |= VESA_DPMS_SUSPEND;
	if (c & (1 << 7)) specs->dpms |= VESA_DPMS_STANDBY;

	switch ((block[4] & 0x18) >> 3) {
	case 0:	 specs->input |= VESA_DISP_MONO;    break;
	case 1:	 specs->input |= VESA_DISP_RGB;	    break;
	case 2:	 specs->input |= VESA_DISP_MULTI;   break;
	default: specs->input |= VESA_DISP_UNKNOWN; break;
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
	if (c & 0x04) specs->misc |= VESA_MISC_PRIM_COLOR;
	if (c & 0x02) specs->misc |= VESA_MISC_1ST_DETAIL;
	if (c & 0x01) specs->gtf = 1;

	get_vesa_modes (specs, edid + ESTABLISHED_TIMING_1);
	get_std_modes (specs, edid + STD_TIMING_DESCRIPTIONS_START);
	get_detailed_modes (specs, edid + DETAILED_TIMING_DESCRIPTIONS_START);
	return 1;
}
