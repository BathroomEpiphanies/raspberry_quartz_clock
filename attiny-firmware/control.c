#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "control.h"
#include "uart.h"


volatile uint32_t _cpu_freq = (uint32_t)20000000/24;
volatile uint8_t _clock_freq = (uint8_t)1;
volatile uint16_t _counter_period = (uint16_t)3255;
volatile uint8_t _duty_cycle = (uint8_t)127;
volatile uint16_t _second_counter = (uint16_t)0;
volatile uint8_t _period_counter = 0;


void init_cpu(void) {
    /* Clock souce 16/20MHz internal oscillator. Need to be set to 20MHz in fuse settings (default) */
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
    /* CPU clock prescaler
    In 1Hz operation: 20M/24 = 833kHz
    In 16Hz operation: 20M/24 = 833kHz */
    _cpu_freq = (uint32_t)20000000/24;
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_24X_gc | CLKCTRL_PEN_bm);
}

void set_run_mode(uint8_t mode) {
    stop_clock();
    cli();
    /* Nominal counter period 
    cpu_freq / counter_sync_prescaler / counter_prescaler / clock_freq
    In  1Hz operation:  833k/8/32/1  = 3255.2
    In 16Hz operation:  833k/4/4/16 =  3255.2 */
    _counter_period = 3255;
    uint8_t count_prescaler = 0;
    uint8_t sync_prescaler = 0;
    switch (mode) {
    case 1:
        _clock_freq = 1;
        _duty_cycle = 15;
        count_prescaler = 32;
        sync_prescaler = 8;
        break;
    case 16:
        _clock_freq = 16;
        _duty_cycle = 127;
        count_prescaler = 4;
        sync_prescaler = 4;
        break;
    }
    init_tcd0(count_prescaler, sync_prescaler);
    update_counter_period(_counter_period);
    update_duty_cycle(_duty_cycle);
    sei();
}


void init_tcd0(uint8_t count_prescaler, uint8_t sync_prescaler) {
    TCD0.CTRLA &= ~TCD_ENABLE_bm;
    while (!(TCD0.STATUS & TCD_ENRDY_bm)) {};
    TCD0.CTRLB = TCD_WGMODE_TWORAMP_gc;
    _PROTECTED_WRITE(TCD0.FAULTCTRL, TCD_CMPAEN_bm | TCD_CMPBEN_bm);
    TCD0.INTCTRL = TCD_OVF_bm;
    switch(count_prescaler) {
    case 4:
        switch(sync_prescaler) {
        case 4:
            TCD0.CTRLA = TCD_CLKSEL_SYSCLK_gc | TCD_CNTPRES_DIV4_gc | TCD_SYNCPRES_DIV4_gc;
            break;
        }
        break;
    case 32:
        switch(sync_prescaler) {
        case 8:
            TCD0.CTRLA = TCD_CLKSEL_SYSCLK_gc | TCD_CNTPRES_DIV32_gc | TCD_SYNCPRES_DIV8_gc;
            break;
        }
        break;
    }
}


ISR(TCD0_OVF_vect) {
    _period_counter++;
    if (_period_counter==_clock_freq) {
        _second_counter+=2;
        if (_second_counter>=SECONDS_IN_12H)
            _second_counter = 0;
        send_time();
        _period_counter = 0;
    }
    TCD0.INTFLAGS |= TCD_OVF_bm;
}


void update_counter_period(uint16_t counter_period) {
    while (!(TCD0.STATUS & TCD_CMDRDY_bm)) {};
    _counter_period = counter_period;
    TCD0.CMPACLR = (uint16_t)(counter_period);
    TCD0.CMPBCLR = (uint16_t)(counter_period);
    TCD0.CTRLE |= TCD_SYNC_bm;
}


void update_duty_cycle(uint8_t duty_cycle) {
    while (!(TCD0.STATUS & TCD_CMDRDY_bm)) {};
    _duty_cycle = duty_cycle;
    TCD0.CMPASET = (uint16_t)((uint32_t)_counter_period*(255-duty_cycle)/256);
    TCD0.CMPBSET = (uint16_t)((uint32_t)_counter_period*(255-duty_cycle)/256);
    TCD0.CTRLE |= TCD_SYNC_bm;
}


void update_second_counter(uint16_t time) {
    _second_counter = time;
}


void run_clock(void) {
    while (!(TCD0.STATUS & TCD_ENRDY_bm)) {};
    TCD0.CTRLA |= TCD_ENABLE_bm;
}

void stop_clock(void) {
    TCD0.CTRLA &= ~TCD_ENABLE_bm;
}


char _string_buffer[16];
void send_time(void) {
    /* Send the current time as a string over UART. */
    uint16_t tmp = _second_counter;
    utoa(tmp, _string_buffer, 10);
    uart_send_string(_string_buffer);
}
