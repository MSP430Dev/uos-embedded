/*
 * Testing leds
 */
#include <runtime/lib.h>

void _irq_handler_ ()
{
}

void my_delay(unsigned int count)
{
    int i;
    for (i = 0; i < count; i++)
        asm volatile ("nop");
}

unsigned short first_char;

void _init_ (void)
{
    // Enable clocks for UART3
    CM_L4PER_UART3_CLKCTRL = MODULEMODE(2);

    // Setting UART3 for debug output
    UART_MDR1(3) = UART_MODE_DISABLE;
    // Set 115200 kbps
    UART_LCR(3) = UART_CONF_MODE_B;
    UART_DLL(3) = 0x1A;
    UART_DLH(3) = 0x00;
    // Enable FIFOs   
    UART_EFR(3) |=  UART_EFR_ENHANCED_EN;
    UART_LCR(3)  =  UART_CONF_MODE_A;
    UART_MCR(3) |=  UART_MCR_TCR_TLR;
    UART_FCR(3) |=  UART_FCR_FIFO_EN;
    UART_MCR(3) &= ~UART_MCR_TCR_TLR;
    UART_LCR(3)  =  UART_CONF_MODE_B;
    UART_EFR(3) &= ~UART_EFR_ENHANCED_EN;
    // Set frame format: 8 bit/char, no parity, 1 stop bit
    UART_LCR(3) = UART_LCR_CHAR_LENGTH_8_BIT;
    // Enable UART mode with 16x divisor
    UART_MDR1(3) = UART_MODE_16X;
    
    // Set pin functions for UART3
    CONTROL_CORE_PAD0_UART3_RX_IRRX_PAD1_UART3_TX_IRTX = MUXMODE1(0) | 
        INPUTENABLE1 | MUXMODE2(0) | INPUTENABLE2;
        
    first_char = 'A';

    for (;;) {
        UART_THR(3) = first_char;
        while (UART_SSR(3) & UART_SSR_TX_FIFO_FULL);
        UART_THR(3) = 'H';
        while (UART_SSR(3) & UART_SSR_TX_FIFO_FULL);
        UART_THR(3) = 'E';
        while (UART_SSR(3) & UART_SSR_TX_FIFO_FULL);
        UART_THR(3) = 'L';
        while (UART_SSR(3) & UART_SSR_TX_FIFO_FULL);
        UART_THR(3) = 'L';
        while (UART_SSR(3) & UART_SSR_TX_FIFO_FULL);
        UART_THR(3) = 'O';
        while (UART_SSR(3) & UART_SSR_TX_FIFO_FULL);
        UART_THR(3) = '\n';
        while (UART_SSR(3) & UART_SSR_TX_FIFO_FULL);
        UART_THR(3) = '\r';
        while (UART_SSR(3) & UART_SSR_TX_FIFO_FULL);
    }
}
