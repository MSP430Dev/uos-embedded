/*
 * Definitions of input signals and output actions for
 * SainSmart Graphic LCD4884 Shield.
 */
#define AN_JOYSTICK     6           /* signal A0, analog input AN6 */
#define MASKC_JOYSTICK  (1 << 0)    /* signal A0, pin RC0=AN6 */
#define MASKC_LCD_SCK   (1 << 2)    /* signal D2, pin RC2 */
#define MASKC_LCD_MOSI  (1 << 3)    /* signal D3, pin RC3 */
#define MASKC_LCD_DC    (1 << 4)    /* signal D4, pin RC4 */
#define MASKC_LCD_CS    (1 << 5)    /* signal D5, pin RC5 */
#define MASKC_LCD_RST   (1 << 6)    /* signal D6, pin RC6 */
#define MASKC_LCD_BL    (1 << 7)    /* signal D7, pin RC7 */

#define MASKB_LED1	(1 << 15)   /* RB15: green */
#define MASKA_LED2	(1 << 10)   /* RA10: red */

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
        LATBSET = MASKB_LED1;
    else
        LATBCLR = MASKB_LED1;
}

/*
 * Initialize LED pins.
 */
inline void led_init()
{
    /* Use all ports as digital. */
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;

    LATBCLR = MASKB_LED1;
    TRISBCLR = MASKB_LED1;
    LATACLR = MASKA_LED2;
    TRISACLR = MASKA_LED2;
}

/*
 * Get a state of joystick.
 * Convert ADC value to key number.
 * Input buttons are connected to a series network of resistors:
 * GND - 3.3k - 1k - 620 - 330 - 2k - +5V
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
    TRISCSET = MASKC_JOYSTICK;
    ANSELCSET = MASKC_JOYSTICK;

    /* Enable and configure the ADC. */
    AD1CSSL = 0;
    AD1CON1 = PIC32_AD1CON1_SSRC(7);    // Auto-convert
    AD1CON2 = 0;
    AD1CON3 = PIC32_AD1CON3_SAMC(15) |  // Auto-sample time = 15
              PIC32_AD1CON3_ADRC;       // Internal RC clock
    AD1CON1SET = PIC32_AD1CON1_ON;
}
