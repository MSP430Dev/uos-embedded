/*
 * int86() - making 16-bit BIOS calls.
 *
 * Calls to BIOS are made through a small subroutine, placed
 * at address 500 (see call86.S). Input registers must be put
 * at 7E0 and 8 bytes of 16-bit code - at 560.
 * Output parameters will be located at 7E0.
 * It must be called with interrupts disabled.
 */
#define INT86_CALL		((void (*)()) 0x500)
#define INT86_CODE		((unsigned long*) 0x560)
#define INT86_REGS		(*(int86_regs_t*) 0x7e0)

typedef union _int86_regs {
	struct _x {
		unsigned short ax;
		unsigned short bx;
		unsigned short cx;
		unsigned short dx;
		unsigned short si;
		unsigned short di;
		unsigned short bp;
		unsigned short ds;
		unsigned short es;
		unsigned short flags;
	} x;
	struct _h {
		unsigned char al, ah;
		unsigned char bl, bh;
		unsigned char cl, ch;
		unsigned char dl, dh;
	} h;
} int86_regs_t;

extern unsigned short int86_irqmask;

void int86 (int intnum, int86_regs_t *inregs, int86_regs_t *outregs);

/*
 * Reboot the system.
 */
void i386_reboot (int code);
