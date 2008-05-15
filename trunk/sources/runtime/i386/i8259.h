/*
 * 8259 Programmable Interrupt Controller (PIC)
 */
#define PIC1_CTL                0x20    /* read IRR/ISR, write commands */
#define PIC2_CTL                0xa0
#define PIC1_MASK               0x21    /* read mask, write mask & init data */
#define PIC2_MASK               0xa1

/*
 * Initialization Command Word 1 (ICW1) at Port 20h and A0h
 *
 * Initialization Procedure
 * 1. write CMD_INIT to port 20h
 * 2. write INIT2 to port 21h
 * 3. write INIT3 to port 21h
 * 4. write INIT4 to port 21h
 */
#define PIC_CMD_INIT            0x11    /* initialize command */
#define PIC_INIT_SINGLE         0x02    /* 1=single 8259, 0=cascading 8259's */
#define PIC_INIT_LEVEL          0x08    /* 1=level or 0=edge triggered */

/*
 * Initialization Command Word 2 (ICW2) at Port 21h and A1h
 * The 80x86 interrupt vector.
 */
#define PIC_INIT2_VECTOR(n)     ((n) & 0xf8)

/*
 * Initialization Command Word 3 (ICW3) at Port 21h and A1h
 */
#define PIC_INIT3_MASTER(n)     (1 << (n))      /* for master: set slave mask */
#define PIC_INIT3_SLAVE(n)      ((n) & 7)       /* for slave: set slave id */

/*
 * Initialization Command Word 4 (ICW4) at Port 21h and A1h
 */
#define PIC_INIT4_8086          0x01            /* should be 1 for 8086 mode */
#define PIC_INIT4_AUTOEOI       0x02            /* auto EOI, for master only */
#define PIC_INIT4_SPECIAL       0x10            /* special fully nested mode */

/*
 * Operation Control Word 1 / Interrupt Mask Reg. (IMR) at ports 21h and A1h
 */
#define PIC_MASK(n)             (1 << (n))      /* 0=enable IRQi, 1=disable */

/*
 * Operation Control Word 2 / Interrupt Command Reg. (ICR) at ports 20h and A0h
 * Combine these values to make complex commands.
 */
#define PIC_INTR_EOI            0x20            /* non-specific EOI command */
#define PIC_INTR_ROT            0x80            /* rotation command */
#define PIC_INTR_LVL(n)        (0x40 | (n)&7)   /* irq level parameter */

/*
 * Operation Control Word 3 at ports 20h and A0h
 */
#define PIC_CMD_ENSMM           0x68            /* enable special mask mode */
#define PIC_CMD_DISMM           0x48            /* disable special mask mode */
#define PIC_CMD_POLL            0x0c            /* initiate the polling */
#define PIC_CMD_IRR             0x0a            /* read IRR on next read */
#define PIC_CMD_ISR             0x0b            /* read ISR on next read */

/*
 * Poll status byte at ports 20h and A0h
 */
#define PIC_POLL_INT            0x80            /* interrupt pending */
#define PIC_POLL_LVL(x)        (0x03 & (x))     /* interrupt request level */
