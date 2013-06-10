/*
 * Definitions of input signals and output actions for
 * SainSmart 1602 LCD Keypad Shield.
 */
#define AN_BUTTON       1           /* signal A0, analog input AN1 */
#define MASKA_BUTTON    (1 << 1)    /* signal A0, pin RA1=AN1 */
#define MASKA_LCD_DB4   (1 << 4)    /* signal D4, pin RA4 */
#define MASKB_LCD_DB5   (1 << 5)    /* signal D5, pin RB8 */
#define MASKB_LCD_DB6   (1 << 6)    /* signal D6, pin RB9 */
#define MASKA_LCD_DB7   (1 << 7)    /* signal D7, pin RA3 */
#define MASKA_LCD_RS    (1 << 7)    /* signal D8, pin RA2 */
#define MASKB_LCD_E     (1 << 10)   /* signal D9, pin RB10 */
#define MASKB_LCD_BL    (1 << 1)    /* signal D10, pin RB11 */

#define MASKB_LED	(1 << 15)   /* RB14: yellow */

/*
 * Button values are determined by resistors on a board.
 */
enum {
    BUTTON_RIGHT,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_SELECT,
    BUTTON_IDLE = -1,
};

/*
 * Control LED.
 */
inline void led_control (int on)
{
    if (on)
        LATBSET = MASKB_LED;
    else
        LATBCLR = MASKB_LED;
}

/*
 * Initialize LED pins.
 */
inline void led_init()
{
    /* Unlock CFGCON register. */
    SYSKEY = 0;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    CFGCON &= (1 << 13);            // clear IOLOCK

    /* Disable JTAG ports, to make more pins available. */
    CFGCON &= (1 << 3);             // clear JTAGEN

    /* Use all ports as digital. */
    ANSELA = 0;
    ANSELB = 0;

    LATBCLR = MASKB_LED;
    TRISBCLR = MASKB_LED;
}

/*
 * Get a state of buttons.
 * Convert ADC value to key number.
 * Input buttons are connected to a series network of resistors:
 * GND - 3.3k - 1k - 620 - 330 - 2k - +3.3V
 * Expected values are:
 * 0 - 144 - 329 - 506 - 741 - 1023
 */
inline int button_get()
{
    static const unsigned level[5] = { 72, 236, 417, 623, 882 };
    unsigned input, k;

    AD1CON1CLR = PIC32_AD1CON1_ON;              // stop a/d converter
    AD1CHS = PIC32_AD1CHS_CH0SA (AN_BUTTON);  // select channel
    AD1CON1SET = PIC32_AD1CON1_ON;              // start a/d converter
    AD1CON1SET = PIC32_AD1CON1_SAMP;            // start sampling
    while (! (AD1CON1 & PIC32_AD1CON1_DONE))    // wait for conversion
        continue;

    input = ADC1BUF0;
    for (k=0; k<5; k++) {
        if (input < level[k]) {
            return k;
        }
    }
    return BUTTON_IDLE;
}

/*
 * Initialize ADC for a button.
 */
inline void button_init()
{
    /* Switch the input pin to analog mode. */
    TRISASET = MASKA_BUTTON;
    ANSELASET = MASKA_BUTTON;

    /* Enable and configure the ADC. */
    AD1CSSL = 0;
    AD1CON1 = PIC32_AD1CON1_SSRC(7);    // Auto-convert
    AD1CON2 = 0;
    AD1CON3 = PIC32_AD1CON3_SAMC(15) |  // Auto-sample time = 15
              PIC32_AD1CON3_ADRC;       // Internal RC clock
    AD1CON1SET = PIC32_AD1CON1_ON;
}
