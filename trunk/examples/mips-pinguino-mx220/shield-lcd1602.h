/*
 * Definitions of input signals and output actions for
 * SainSmart 1602 LCD Keypad Shield.
 */
#define AN_BUTTON       6           /* signal A0, analog input AN6 */
#define MASKC_BUTTON    (1 << 0)    /* signal A0, pin RC0=AN6 */
#define MASKC_LCD_DB4   (1 << 4)    /* signal D4, pin RC4 */
#define MASKC_LCD_DB5   (1 << 5)    /* signal D5, pin RC5 */
#define MASKC_LCD_DB6   (1 << 6)    /* signal D6, pin RC6 */
#define MASKC_LCD_DB7   (1 << 7)    /* signal D7, pin RC7 */
#define MASKB_LCD_RS    (1 << 7)    /* signal D8, pin RB7 */
#define MASKA_LCD_E     (1 << 10)   /* signal D9, pin RA10 */
#define MASKA_LCD_BL    (1 << 1)    /* signal D10, pin RA1 */

#define MASKB_LED1	(1 << 15)   /* RB15: green */

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
    TRISCSET = MASKC_BUTTON;
    ANSELCSET = MASKC_BUTTON;

    /* Enable and configure the ADC. */
    AD1CSSL = 0;
    AD1CON1 = PIC32_AD1CON1_SSRC(7);    // Auto-convert
    AD1CON2 = 0;
    AD1CON3 = PIC32_AD1CON3_SAMC(15) |  // Auto-sample time = 15
              PIC32_AD1CON3_ADRC;       // Internal RC clock
    AD1CON1SET = PIC32_AD1CON1_ON;
}
