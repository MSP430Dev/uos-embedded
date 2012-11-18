/*
 * Definitions of input signals and output actions for
 * SainSmart Graphic LCD4884 Shield.
 */
#define MASKC_JOYSTICK  (1 << 0)    /* signal A0, pin RC0=AN6 */
#define MASKC_LCD_SCK   (1 << 2)    /* signal D2, pin RC2 */
#define MASKC_LCD_MOSI  (1 << 3)    /* signal D3, pin RC3 */
#define MASKC_LCD_DC    (1 << 4)    /* signal D4, pin RC4 */
#define MASKC_LCD_CS    (1 << 5)    /* signal D5, pin RC5 */
#define MASKC_LCD_RST   (1 << 6)    /* signal D6, pin RC6 */
#define MASKC_LCD_BL    (1 << 7)    /* signal D7, pin RC7 */

/*
 * Button values are determined by resistors on a board.
 */
enum {
    JOYSTICK_LEFT,
    JOYSTICK_SELECT,
    JOYSTICK_DOWN,
    JOYSTICK_UP,
    JOYSTICK_RIGHT,
    JOYSTICK_IDLE = -1,
};

/*
 * Get a state of joystick.
 * Convert ADC value to key number.
 * Input buttons are connected to a series network of resistors:
 * GND - 3.3k - 1k - 620 - 330 - 2k - +5V
 */
inline int joystick_get()
{
    static const unsigned adc_key[5] = { 50, 200, 400, 600, 800 };
    unsigned input, k;

#if 1
    AD1CON1SET = 0x0002;    // start Converting
    while (! (AD1CON1 & 1)) // conversion done?
        continue;
#endif
    input = ADC1BUF0;
    for (k=0; k<5; k++) {
        if (input < adc_key[k]) {
            return k;
        }
    }
    return JOYSTICK_IDLE;
}

inline void joystick_init()
{
    /* Switch the input pin to analog mode. */
    TRISCSET = MASKC_JOYSTICK;
    ANSELCSET = MASKC_JOYSTICK;
    ANSELCSET = MASKC_JOYSTICK;

    /* Enable and configure the ADC. */
    AD1CON1 = PIC32_AD1CON1_ASAM;       // Sample auto-start
    AD1CHS = PIC32_AD1CHS_CH0SA(6);     // Connect AN6 as CH0 input
    AD1CSSL = 0;
    AD1CON3 = PIC32_AD1CON3_ADCS(2);    // Sample time manual, TAD = internal 6 TPB
    AD1CON2 = 0;
    AD1CON1SET = PIC32_AD1CON1_ON;      // turn ON the ADC
#if 0
    AD1CSSL = 0xFFFF;           // RetroBSD
    AD1CON2 = 0b0000 0100 0011 1100;
    AD1CON3 = 0b0000 0111 0000 0111;
    AD1CON1 = 0b1000 0000 1110 0110;
#endif
#if 0 //defined (__32MX220F032D__) || defined (__32MX250F128D__)
    AD1CHS = 0x05040000;        // Auto convert AN4 and AN5
    AD1CON1 = 0xE4;
    AD1CON2 = 0x7;
    AD1CON3 = 0x8F00;
#endif
}
