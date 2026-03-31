#ifndef UART_H
#define UART_H


#define BAUD_RATE 38400

void init_uart(void);
void uart_send_string(char* message);
void parse_uart_message(void);

#endif
