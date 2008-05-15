#include <runtime/lib.h>
/*#include <kernel/uos.h>*/
#include <vesa/vesa.h>
#include <runtime/i386/int86.h>

/*
 * Use 512 bytes of low core memory.
 */
#define LOWMEM		0x800

/*
 * Make linear pointer from offset-segment pair.
 */
static void
fix_pointer (void *to, void *from, unsigned long *ptr)
{
	unsigned short off = ((unsigned short*) ptr) [0];
	unsigned short segm = ((unsigned short*) ptr) [1];

	*ptr = off + ((unsigned long) segm << 4);
	if ((void*) *ptr >= from && (void*) *ptr < from+512)
		*ptr += to - from;
	/* debug_printf ("%04x:%04x -> %08x\n", segm, off, *ptr); */
}

int
vesa_get_info (vesa_info_t *i)
{
	int86_regs_t reg;
	unsigned char *data = (unsigned char*) LOWMEM;

	reg.x.ax = 0x4f00;
	reg.x.di = 0;
	reg.x.es = LOWMEM >> 4;
	memcpy (data, "VBE2", 4);

	int86 (0x10, &reg, &reg);
	if (reg.x.ax != 0x004f)
		return 0;

	memcpy (i, data, sizeof (*i));
	fix_pointer (i, data, (unsigned long*) &i->oem_string);
	fix_pointer (i, data, (unsigned long*) &i->modes);
	fix_pointer (i, data, (unsigned long*) &i->vendor_name);
	fix_pointer (i, data, (unsigned long*) &i->product_name);
	fix_pointer (i, data, (unsigned long*) &i->product_rev);
	return 1;
}

/*
 * Get details about a specific VESA video mode
 */
int
vesa_get_mode_info (unsigned short mode, vesa_mode_info_t *i)
{
	int86_regs_t reg;

	reg.x.ax = 0x4f01;
	reg.x.cx = mode;
	reg.x.di = 0;
	reg.x.es = LOWMEM >> 4;

	int86 (0x10, &reg, &reg);
	if (reg.x.ax != 0x004f)
		return 0;

	memcpy (i, (void*) LOWMEM, sizeof (*i));
	return 1;
}

unsigned short
vesa_get_mode ()
{
	int86_regs_t reg;

	reg.x.ax = 0x4f03;
	int86 (0x10, &reg, &reg);
	if (reg.x.ax != 0x004f)
		return 0;
	return reg.x.bx;
}

int
vesa_set_mode (unsigned short mode)
{
	int86_regs_t reg;

	reg.x.ax = 0x4f02;
	reg.x.bx = mode & ~0x0800; /* no CRTC values */
	int86 (0x10, &reg, &reg);
	if (reg.x.ax != 0x004f)
		return 0;
	return 1;
}

int
vesa_set_palette (int first, int count, void *data)
{
	int86_regs_t reg;

	reg.x.ax = 0x4f09;
	reg.x.bx = 0;
	reg.x.cx = count;
	reg.x.dx = first;
	reg.x.di = 0;
	reg.x.es = LOWMEM >> 4;
	memcpy ((void*) LOWMEM, data, 4*count);

	int86 (0x10, &reg, &reg);
	if (reg.x.ax != 0x004f)
		return 0;
	return 1;
}
