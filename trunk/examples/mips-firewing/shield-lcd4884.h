/*
 * Definitions of input signals and output actions for
 * SainSmart Graphic LCD4884 Shield.
 */
#define AN_JOYSTICK     1           /* signal A0, analog input AN1 */
#define MASKA_JOYSTICK  (1 << 1)    /* signal A0, pin RA1=AN1 */
#define MASKB_LCD_SCK   (1 << 6)    /* signal D2, pin RB6 */
#define MASKB_LCD_MOSI  (1 << 7)    /* signal D3, pin RB7 */
#define MASKA_LCD_DC    (1 << 4)    /* signal D4, pin RA4 */
#define MASKB_LCD_CS    (1 << 8)    /* signal D5, pin RB8 */
#define MASKB_LCD_RST   (1 << 9)    /* signal D6, pin RB9 */
#define MASKA_LCD_BL    (1 << 3)    /* signal D7, pin RA3 */

#define MASKB_LED	(1 << 14)   /* RB14: yellow */

/*
 * Button values are determined by resistors on a board.
 */
enum {
    JOYSTICK_LEFT,
    JOYSTICK_SELECT,
    JOYSTICK_DOWN,
    JOYSTICK_RIGHT,
    JOYSTICK_UP,
    JOYSTICK_IDLE = -1,
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
 * Get a state of joystick.
 * Convert ADC value to key number.
 * Input buttons are connected to a series network of resistors:
 * GND - 3.3k - 1k - 620 - 330 - 2k - +3.3V
 * Expected values are:
 * 0 - 144 - 329 - 506 - 741 - 1023
 */
inline int joystick_get()
{
    static const unsigned level[5] = { 72, 236, 417, 623, 882 };
    unsigned input, k;

    AD1CON1CLR = PIC32_AD1CON1_ON;              // stop a/d converter
    AD1CHS = PIC32_AD1CHS_CH0SA (AN_JOYSTICK);  // select channel
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
    return JOYSTICK_IDLE;
}

/*
 * Initialize ADC for a joystick.
 */
inline void joystick_init()
{
    /* Switch the input pin to analog mode. */
    TRISASET = MASKA_JOYSTICK;
    ANSELASET = MASKA_JOYSTICK;

    /* Enable and configure the ADC. */
    AD1CSSL = 0;
    AD1CON1 = PIC32_AD1CON1_SSRC(7);    // Auto-convert
    AD1CON2 = 0;
    AD1CON3 = PIC32_AD1CON3_SAMC(15) |  // Auto-sample time = 15
              PIC32_AD1CON3_ADRC;       // Internal RC clock
    AD1CON1SET = PIC32_AD1CON1_ON;
}
