#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include "flash.h"

#define SHUFFLE(h)	(((h) & 0x8001) | \
			((h) & 2) << 1 | \
			((h) & 4) << 2 | \
			((h) & 8) << 3 | \
			((h) & 0x10) << 4 | \
			((h) & 0x20) << 5 | \
			((h) & 0x40) << 6 | \
			((h) & 0x80) << 7 | \
			((h) & 0x100) >> 7 | \
			((h) & 0x200) >> 6 | \
			((h) & 0x400) >> 5 | \
			((h) & 0x800) >> 4 | \
			((h) & 0x1000) >> 3 | \
			((h) & 0x2000) >> 2 | \
			((h) & 0x4000) >> 1)

static int flash_detected (flash_t *f)
{
	unsigned long x;

	THUMB_SWITCH_TO_ARM();
	ARM_INTR_DISABLE (&x);
	ARM_SWITCH_TO_THUMB();

	/* Read device code.
	 * Must be 29LV800B or 29LV800T. */
debug_printf ("Before *555 = aa\n");
	*f->flash_addr_555 = f->flash_cmd_aa;
debug_printf ("Before *2aa = 55\n");
	*f->flash_addr_2aa = f->flash_cmd_55;
debug_printf ("Before *555 = aa\n");
	*f->flash_addr_555 = f->flash_cmd_90;
	f->flash_id = f->memory[1];
	f->memory[0] = f->flash_cmd_f0;
debug_printf ("Flash id = %04x\n", f->flash_id);

	/* Read MFR code. */
	*f->flash_addr_555 = f->flash_cmd_aa;
	*f->flash_addr_2aa = f->flash_cmd_55;
	*f->flash_addr_555 = f->flash_cmd_90;
	f->mfr_id = f->memory[0];
	f->memory[0] = f->flash_cmd_f0;
debug_printf ("Mfr id = %04x\n", f->mfr_id);

	/* Stop read ID mode. */
	THUMB_SWITCH_TO_ARM();
	ARM_INTR_RESTORE (x);
	ARM_SWITCH_TO_THUMB();

	if (f->flash_id != f->id_29lv800_b &&
	    f->flash_id != f->id_29lv800_t) {
#ifndef NDEBUG
		debug_printf ("Bad flash id = %04x, must be %04x or %04x\n",
			f->flash_id, f->id_29lv800_b, f->id_29lv800_t);
#endif
		return 0;
	}
	return 1;
}

/*
 * Open the device.
 */
int flash_open (flash_t *f, unsigned long base)
{
	f->memory = (unsigned short*) base;

	/* Cronyx bridge board. */
	f->flash_addr_555 = (unsigned short*) (base + 0x0000aa82);
	f->flash_addr_2aa = (unsigned short*) (base + 0x00015500);
	f->flash_cmd_aa   = SHUFFLE (0xaa);
	f->flash_cmd_55   = SHUFFLE (0x55);
	f->flash_cmd_90   = SHUFFLE (0x90);
	f->flash_cmd_80   = SHUFFLE (0x80);
	f->flash_cmd_10   = SHUFFLE (0x10);
	f->flash_cmd_a0   = SHUFFLE (0xa0);
	f->flash_cmd_30   = SHUFFLE (0x30);
	f->flash_cmd_f0   = SHUFFLE (0xf0);
	f->id_alliance    = SHUFFLE (0x52);
	f->id_amd         = SHUFFLE (0x01);
	f->id_29lv800_b   = SHUFFLE (0x225b);
	f->id_29lv800_t   = SHUFFLE (0x22da);
/*	if (flash_detected (f))*/
		return 1;

	/* SNDS300 development board. */
	f->flash_addr_555 = (unsigned short*) (base + 0x00000aaa);
	f->flash_addr_2aa = (unsigned short*) (base + 0x00000554);
	f->flash_cmd_aa   = 0xaa;
	f->flash_cmd_55   = 0x55;
	f->flash_cmd_90   = 0x90;
	f->flash_cmd_80   = 0x80;
	f->flash_cmd_10   = 0x10;
	f->flash_cmd_a0   = 0xa0;
	f->flash_cmd_30   = 0x30;
	f->flash_cmd_f0   = 0xf0;
	f->id_alliance    = 0x52;
	f->id_amd         = 0x01;
	f->id_29lv800_b   = 0x225b;
	f->id_29lv800_t   = 0x22da;
	if (flash_detected (f))
		return 1;

	return 0;
}

void flash_get_name (flash_t *f, char *name)
{
	strcpy (name, "");
	if (f->mfr_id == f->id_alliance)
		strcat (name, "Alliance ");
	else if (f->mfr_id == f->id_amd)
		strcat (name, "AMD ");

	if (f->flash_id == f->id_29lv800_b)
		strcat (name, "29LV800B");
	else if (f->flash_id == f->id_29lv800_t)
		strcat (name, "29LV800T");
	else
		strcat (name, "Unknown");
}

/*
 * Chip erase.
 */
void flash_erase_chip (flash_t *f)
{
	unsigned long x;

	THUMB_SWITCH_TO_ARM();
	ARM_INTR_DISABLE (&x);
	ARM_SWITCH_TO_THUMB();

	*f->flash_addr_555 = f->flash_cmd_aa;
	*f->flash_addr_2aa = f->flash_cmd_55;
	*f->flash_addr_555 = f->flash_cmd_80;
	*f->flash_addr_555 = f->flash_cmd_aa;
	*f->flash_addr_2aa = f->flash_cmd_55;
	*f->flash_addr_555 = f->flash_cmd_10;

	THUMB_SWITCH_TO_ARM();
	ARM_INTR_RESTORE (x);
	ARM_SWITCH_TO_THUMB();
}

/*
 * Chip erase.
 */
void flash_erase_sector (flash_t *f, unsigned long addr)
{
	unsigned long x;

	THUMB_SWITCH_TO_ARM();
	ARM_INTR_DISABLE (&x);
	ARM_SWITCH_TO_THUMB();

	*f->flash_addr_555 = f->flash_cmd_aa;
	*f->flash_addr_2aa = f->flash_cmd_55;
	*f->flash_addr_555 = f->flash_cmd_80;
	*f->flash_addr_555 = f->flash_cmd_aa;
	*f->flash_addr_2aa = f->flash_cmd_55;
	*(unsigned short*) ((char*)f->memory + addr) = f->flash_cmd_30;

	THUMB_SWITCH_TO_ARM();
	ARM_INTR_RESTORE (x);
	ARM_SWITCH_TO_THUMB();
}

/*
 * Check that chip is erased.
 * Returns 1 when chip is successfully erased.
 * Returns 0 if not erased yet, must wait.
 * Returns -1 when erasing failed and flash_erase() must be repeated.
 */
int flash_is_erased (flash_t *f, unsigned long addr)
{
	unsigned short word;

	word = *(unsigned short*) ((char*)f->memory + addr);
	if (word == 0xffff) {
		return 1;
	}
	if (word != 0x0040 && word != 0x1050 &&
	    word != 0x0140 && word != 0x1150) {
		return -1;
	}
	return 0;
}

void flash_write16 (flash_t *f, unsigned long addr, unsigned short val)
{
	unsigned long x;

	THUMB_SWITCH_TO_ARM();
	ARM_INTR_DISABLE (&x);
	ARM_SWITCH_TO_THUMB();

	*f->flash_addr_555 = f->flash_cmd_aa;
	*f->flash_addr_2aa = f->flash_cmd_55;
	*f->flash_addr_555 = f->flash_cmd_a0;
	*(unsigned short*) ((char*)f->memory + addr) = val;

	THUMB_SWITCH_TO_ARM();
	ARM_INTR_RESTORE (x);
	ARM_SWITCH_TO_THUMB();
}
