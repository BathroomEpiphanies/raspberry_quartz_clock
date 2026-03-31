#ifndef CONTROL_H
#define CONTROL_H

#include <avr/io.h>


#define SECONDS_IN_12H 43200

void init_cpu(void);
void init_tcd0(uint8_t count_prescaler, uint8_t sync_prescaler);
void set_run_mode(uint8_t mode);
void update_counter_period(uint16_t counter_period);
void update_duty_cycle(uint8_t duty_cycle);
void update_second_counter(uint16_t time);
void run_clock(void);
void stop_clock(void);
void send_time(void);


#endif
