#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <pci/pci.h>

static unsigned long pcibios_find_entry (void)
{
	/* This is the standard structure used to identify the entry point
	 * to the BIOS32 Service Directory. */
	typedef union {
		struct {
			unsigned long signature;	/* _32_ */
			unsigned long entry;		/* 32 bit physical address */
			unsigned char revision;		/* Revision level, 0 */
			unsigned char length;		/* Length in paragraphs should be 01 */
			unsigned char checksum;		/* All bytes must add up to zero */
			unsigned char reserved[5]; 	/* Must be zero */
		} fields;
		char chars[16];
	} bios32_header_t;

	/*
	 * Physical address of the service directory.
	 */
	struct {
		unsigned long address;
		unsigned short segment;
	} indirect;

	bios32_header_t *hdr;
	unsigned char sum, return_code;
	unsigned long address, entry;
	int i, length, x;

	/*
	 * Follow the standard procedure for locating the BIOS32 Service
	 * directory by scanning the permissible address range from
	 * 0xe0000 through 0xfffff for a valid BIOS32 structure.
	 */
	for (hdr = (bios32_header_t*) 0xe0000; ; ++hdr) {
		if (hdr > (bios32_header_t*) 0xffff0)
			return 0;

		if (hdr->fields.signature != *(long*) "_32_")
			continue;

		length = hdr->fields.length * 16;
		if (! length)
			continue;

		sum = 0;
		for (i=0; i<length; ++i)
			sum += hdr->chars[i];
		if (sum != 0)
			continue;

		/* We support only revision 0. */
		if (hdr->fields.revision == 0)
			break;

		/* debug_printf ("PCI: unsupported BIOS32 revision %d at 0x%p\n",
			hdr->fields.revision, hdr); */
	}
	/* debug_printf ("PCI: BIOS32 Service Directory structure at %p\n",
		hdr); */
	/* debug_printf ("PCI: BIOS32 Service Directory entry at 0x%lx\n",
		hdr->fields.entry); */
	if (hdr->fields.entry >= 0x100000) {
		/* debug_printf("-- too high in memory, cannot use.\n"); */
		return 0;
	}

	/*
	 * Get the entry point for PCI service.
	 */
	indirect.address = hdr->fields.entry;
	indirect.segment = I386_CS;
	MACHDEP_INTR_DISABLE (&x);
	__asm__("lcall *(%%edi); cld"
		: "=a" (return_code),	/* %al */
		  "=b" (address),	/* %ebx */
		  "=d" (entry)		/* %edx */
		: "0" (*(long*) "$PCI"),
		  "1" (0),
		  "D" (&indirect));
	MACHDEP_INTR_RESTORE (x);

	switch (return_code) {
	case 0:
		return address + entry;

	case 0x80:	/* Not present */
		/* debug_printf ("PCI: BIOS32 $PCI service not present\n"); */
		return 0;

	default:	/* Shouldn't happen */
		/* debug_printf ("PCI: BIOS32 returned unknown error 0x%x\n",
			return_code); */
		return 0;
	}
}

static int pcibios_check (pcibios_t *pb)
{
	unsigned long signature, eax, ebx, ecx;
	unsigned char status;
	int x;

	MACHDEP_INTR_DISABLE (&x);
	__asm__("lcall *(%%edi); cld\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=d" (signature),
		  "=a" (eax),
		  "=b" (ebx),
		  "=c" (ecx)
		: "1" (PCIBIOS_PCI_BIOS_PRESENT),
		  "D" (&pb->entry_address)
		: "memory");
	MACHDEP_INTR_RESTORE (x);

	status = (eax >> 8) & 0xff;
	pb->hw_mechanism = eax & 0xff;
	pb->version_major = (ebx >> 8) & 0xff;
	pb->version_minor = ebx & 0xff;
	pb->last_bus = ecx & 0xff;
	/* debug_printf ("PCI: BIOS probe returned s=%02x hw=%02x ver=%02x.%02x l=%02x\n",
		status, pb->hw_mechanism, pb->version_major, pb->version_minor,
		pb->last_bus); */

	if (status) {
		/* debug_printf ("PCI: BIOS error #%x\n", status); */
		return 0;
	}
	if (signature != *(long*) "PCI ") {
		/* debug_printf ("PCI: incorrect BIOS signature %08x\n",
			signature); */
		return 0;
	}
	return 1;
}

int pcibios_init (pcibios_t *pb)
{
	memset (pb, 0, sizeof (pcibios_t));

	/* Get the entry point for PCI BIOS service. */
	pb->entry_address = pcibios_find_entry ();
	if (! pb->entry_address)
		return 0;
	pb->entry_segment = I386_CS;

	if (! pcibios_check (pb))
		return 0;

	return 1;
}

int pcibios_read_byte (pcibios_t *pb, unsigned short busdevfn,
	unsigned char reg, unsigned char *value)
{
	unsigned long result = 0;
	int x;

	MACHDEP_INTR_DISABLE (&x);
	__asm__("lcall *(%%esi); cld\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=c" (*value),
		  "=a" (result)
		: "1" (PCIBIOS_READ_CONFIG_BYTE),
		  "b" (busdevfn),
		  "D" ((long) reg),
		  "S" (&pb->entry_address));
	MACHDEP_INTR_RESTORE (x);

	if (result & 0xff00)
		return 0;
	return 1;
}

int pcibios_read_short (pcibios_t *pb, unsigned short busdevfn,
	unsigned char reg, unsigned short *value)
{
	unsigned long result = 0;
	int x;

	MACHDEP_INTR_DISABLE (&x);
	__asm__("lcall *(%%esi); cld\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=c" (*value),
		  "=a" (result)
		: "1" (PCIBIOS_READ_CONFIG_WORD),
		  "b" (busdevfn),
		  "D" ((long) reg),
		  "S" (&pb->entry_address));
	MACHDEP_INTR_RESTORE (x);

	if (result & 0xff00)
		return 0;
	return 1;
}

int pcibios_read_long (pcibios_t *pb, unsigned short busdevfn,
	unsigned char reg, unsigned long *value)
{
	unsigned long result = 0;
	int x;

	MACHDEP_INTR_DISABLE (&x);
	__asm__("lcall *(%%esi); cld\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=c" (*value),
		  "=a" (result)
		: "1" (PCIBIOS_READ_CONFIG_DWORD),
		  "b" (busdevfn),
		  "D" ((long) reg),
		  "S" (&pb->entry_address));
	MACHDEP_INTR_RESTORE (x);

	if (result & 0xff00)
		return 0;
	return 1;
}

int pcibios_write_byte (pcibios_t *pb, unsigned short busdevfn,
	unsigned char reg, unsigned char value)
{
	unsigned long result = 0;
	int x;

	MACHDEP_INTR_DISABLE (&x);
	__asm__("lcall *(%%esi); cld\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=a" (result)
		: "0" (PCIBIOS_WRITE_CONFIG_BYTE),
		  "c" (value),
		  "b" (busdevfn),
		  "D" ((long)reg),
		  "S" (&pb->entry_address));
	MACHDEP_INTR_RESTORE (x);

	if (result & 0xff00)
		return 0;
	return 1;
}

int pcibios_write_short (pcibios_t *pb, unsigned short busdevfn,
	unsigned char reg, unsigned short value)
{
	unsigned long result = 0;
	int x;

	MACHDEP_INTR_DISABLE (&x);
	__asm__("lcall *(%%esi); cld\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=a" (result)
		: "0" (PCIBIOS_WRITE_CONFIG_WORD),
		  "c" (value),
		  "b" (busdevfn),
		  "D" ((long)reg),
		  "S" (&pb->entry_address));
	MACHDEP_INTR_RESTORE (x);

	if (result & 0xff00)
		return 0;
	return 1;
}

int pcibios_write_long (pcibios_t *pb, unsigned short busdevfn,
	unsigned char reg, unsigned long value)
{
	unsigned long result = 0;
	int x;

	MACHDEP_INTR_DISABLE (&x);
	__asm__("lcall *(%%esi); cld\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=a" (result)
		: "0" (PCIBIOS_WRITE_CONFIG_DWORD),
		  "c" (value),
		  "b" (busdevfn),
		  "D" ((long)reg),
		  "S" (&pb->entry_address));
	MACHDEP_INTR_RESTORE (x);

	if (result & 0xff00)
		return 0;
	return 1;
}

int pcibios_find_device (pcibios_t *pb, unsigned short vendor,
	unsigned short device_id, unsigned short index,
	unsigned short *busdevfn)
{
	unsigned long result = 0, bx;
	int x;

	MACHDEP_INTR_DISABLE (&x);
	__asm__("lcall *(%%edi); cld\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=b" (bx),
		  "=a" (result)
		: "1" (PCIBIOS_FIND_PCI_DEVICE),
		  "c" (device_id),
		  "d" (vendor),
		  "S" ((int) index),
		  "D" (&pb->entry_address));
	MACHDEP_INTR_RESTORE (x);

	if (result & 0xff00)
		return 0;

	*busdevfn = bx;
	return 1;
}

int pcibios_find_class_code (pcibios_t *pb, unsigned long class_code,
	unsigned short index, unsigned short *busdevfn)
{
	unsigned long result = 0, bx;
	int x;

	MACHDEP_INTR_DISABLE (&x);
	__asm__("lcall *(%%edi); cld\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=b" (bx),
		  "=a" (result)
		: "1" (PCIBIOS_FIND_PCI_CLASS_CODE),
		  "c" (class_code),
		  "S" ((int) index),
		  "D" (&pb->entry_address));
	MACHDEP_INTR_RESTORE (x);

	if (result & 0xff00)
		return 0;

	*busdevfn = bx;
	return 1;
}
