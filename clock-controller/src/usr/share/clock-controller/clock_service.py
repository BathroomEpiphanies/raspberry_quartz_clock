#!/usr/bin/env python3

import argparse
import sys
import traceback

from datetime import datetime
from uart_connection import UARTConnection
from clock_controller import ClockController


def main():
        
    parser = argparse.ArgumentParser(
        description = 'PID control slave clock'
    )
    parser.add_argument(
        '--port',
        required = True,
        help = 'Serial port.'
    )
    parser.add_argument(
        '--period',
        type = int,
        default = 3255,
        help = 'Initial timer overflow.'
    )
    parser.add_argument(
        '--duty-cycle',
        type = int,
        default = 127,
        help = 'Output duty cycle [0-255].'
    )
    parser.add_argument(
        '--stop-clock',
        action = 'store_true',
        help = 'Stop the clock.'
    )
    parser.add_argument(
        '--initiate-clock',
        type = str,
        help = 'Initiate the clock to the current clock face [HH:MM:SS].'
    )
    parser.add_argument(
        '--debug',
        action = 'store_true',
        help = 'Print debug information.'
    )
    args = parser.parse_args()
    
    if args.initiate_clock:
        datestamp = datetime.strptime(args.initiate_clock, '%H:%M:%S')
    else:
        datestamp = datetime.now()
    
    connection = UARTConnection(args.port)
    controller = ClockController(
        connection = connection,
        datestamp = datestamp,
        period = args.period,
        duty_cycle = args.duty_cycle,
    )
    
    if args.stop_clock:
        controller.stop_clock()
        exit()
    
    while True:
        try:
            connection.connect()
            controller.run_clock()
        except KeyboardInterrupt:
            controller.stop_clock()
            sys.exit(0)
        except ValueError:
            pass
        except:
            traceback.print_exc()


if __name__ == "__main__":
    main()
