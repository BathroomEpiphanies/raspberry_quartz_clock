#include <avr/interrupt.h>

#include "control.h"
#include "uart.h"


int main(void) {
    init_cpu();
    init_uart();
    sei();
    //set_run_mode(1);
    //set_run_mode(16);
    //run_clock();
    for (;;) {}
    return 1;
}
