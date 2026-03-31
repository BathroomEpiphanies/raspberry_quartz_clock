#include <string.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "control.h"


char uart_message_buffer[16];
volatile uint8_t uart_message_length = 0;
ISR(USART0_RXC_vect) {
    uart_message_buffer[uart_message_length] = USART0.RXDATAL;
    if (uart_message_buffer[uart_message_length]==0) {
        uart_message_length = 0;
        parse_uart_message();
    }
    else {
        uart_message_length++;
        if (uart_message_length>=16)
            uart_message_length = 0;
    }
    USART0.STATUS |= USART_RXCIF_bm;
}


void init_uart(void) {
    /* Baud rate calculation found online 
       UART_BAUD_RATE (((float)F_CPU*64/(16*(float)BAUD_RATE))+0.5)
     */
    extern volatile uint32_t _cpu_freq;
    uint16_t baud_rate = (uint16_t)( (float)_cpu_freq*64/16/BAUD_RATE + 0.5 );
    PORTMUX.CTRLB |= PORTMUX_USART0_bm;   /* Alternative pins for USART0 */
    PORTA.DIRSET   = PIN1_bm;             /* TX-pin output */
    PORTA.DIRCLR   = PIN2_bm;             /* RX-pin input  */
    USART0.CTRLA   = USART_RXCIE_bm;      /* Enable interrupt on recieve */
    USART0.BAUD    = (uint16_t)baud_rate;
    USART0.CTRLC   = USART_CMODE_ASYNCHRONOUS_gc
                   | USART_PMODE_DISABLED_gc
                   | USART_CHSIZE_8BIT_gc;
    USART0.CTRLB   = USART_TXEN_bm
                   | USART_RXEN_bm;
}


void parse_uart_message(void) {
    if (!strncmp(uart_message_buffer, "version", 7)) {
        uart_send_string(VERSION_DATE);
    }
    else if (!strncmp(uart_message_buffer, "reset", 5)) {
        _PROTECTED_WRITE(RSTCTRL.SWRR, RSTCTRL_SWRE_bm);
    }
    else if (!strncmp(uart_message_buffer, "stop", 4)) {
        stop_clock();
    }
    else if (!strncmp(uart_message_buffer, "start", 5)) {
        run_clock();
    }
    else if (!strncmp(uart_message_buffer, "mode:", 5)) {
        uint8_t mode = (uint8_t)atoi(uart_message_buffer+5);
        set_run_mode( mode );
    }
    else if (!strncmp(uart_message_buffer, "period:", 7)) {
        uint16_t counter_period = atoi(uart_message_buffer+7);
        update_counter_period(counter_period);
    }
    else if (!strncmp(uart_message_buffer, "duty:", 5)) {
        uint8_t duty_cycle = (uint8_t)atoi(uart_message_buffer+5);
        update_duty_cycle(duty_cycle);
    }
    else if (!strncmp(uart_message_buffer, "time:", 5)) {
        uint16_t time = (uint16_t)atoi(uart_message_buffer+5);
        update_second_counter(time);
    }
}


void uart_send_string(char* message) {
    uint8_t length = strlen(message);
    for (uint8_t i=0; i<=length; i++) {
        while (!(USART0.STATUS & USART_DREIF_bm));
        USART0.TXDATAL = message[i];
    }
}
