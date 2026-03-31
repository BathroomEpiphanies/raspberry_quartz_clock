#!/usr/bin/python3 -u

import serial
import sys
import traceback


port = sys.argv[1]


def main():
    while True:
        try:
            UART = serial.Serial(
                port = port,
                baudrate = 38400,
                bytesize = serial.EIGHTBITS,
                parity = serial.PARITY_NONE,
                stopbits = serial.STOPBITS_ONE,
                timeout = None,
                xonxoff = False,
                rtscts = False,
                write_timeout = None,
                dsrdtr = False,
                inter_byte_timeout = None,
                exclusive = None
            )
            UART.close()
            UART.open()
            
            while True:
                a = input()
                UART.write(f'{a}{chr(0)}'.encode('ascii'))
        except KeyboardInterrupt:
            sys.exit(0)
        except:
            traceback.print_exc(file=sys.stdout)
            pass


if __name__ == '__main__':
    main()
