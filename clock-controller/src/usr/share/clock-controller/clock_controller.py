import sys
from datetime import datetime

from uart_connection import UARTConnection


SECONDS_IN_1H = 3600
SECONDS_IN_2H = 7200
SECONDS_IN_6H = 21600
SECONDS_IN_12H = 43200


class ClockController:
    
    connection:UARTConnection
    startstamp:datetime
    mode:int
    period:int
    duty_cycle:int
    
    def __init__(
            self,
            connection:UARTConnection,
            datestamp:datetime,
            mode:int = 16,
            period:int = 3255,
            duty_cycle:int = 127,
    ) -> None:
        self.connection = connection
        self.startstamp = datestamp
        self.mode = mode
        self.period = period
        self.duty_cycle = duty_cycle
    
    def reset(
            self,
    ):
        print('resetting controller')
        self.connection.write_string('reset')
    
    def version(
            self,
    ):
        print('asking controller for version')
        self.connection.write_string('version')
    
    def set_mode(
            self,
            mode:int,
    ) -> None:
        print(f'setting mode to: {mode}')
        self.connection.write_string(f'mode:{mode}')
    
    def start_clock(
            self,
    ) -> None:
        print('starting the clock')
        self.connection.write_string('start')
    
    def stop_clock(
            self,
    ) -> None:
        print('stopping the clock')
        self.connection.write_string('stop')
    
    def set_period(
            self,
            period:int,
    ) -> None:
        self.connection.write_string(f'period:{period}')
    
    def set_duty_cycle(
            self,
            duty_cycle:int,
    ) -> None:
        self.connection.write_string(f'duty:{duty_cycle}')
    
    def set_time(
            self,
            second:int,
    ) -> None:
        print(f'setting time to: {second} seconds')
        self.connection.write_string(f'time:{second}')
    
    def get_split(
            self,
    ) -> tuple[str,float,int,float]:
        message = ''
        try:
            message = self.connection.read_string()
            datestamp = datetime.now()
            timestamp = 3600*(datestamp.hour%12) + \
                          60*datestamp.minute + \
                             datestamp.second + \
                        1e-6*datestamp.microsecond
            split = int(message)%SECONDS_IN_12H
            error = (split-timestamp+SECONDS_IN_1H)%SECONDS_IN_12H - SECONDS_IN_1H
            return datestamp.strftime('%H:%M:%S'),timestamp,split,error
        except ValueError:
            print(message, file=sys.stderr)
            raise
    
    def run(
            self,
    ) -> None:
        self.reset()
        while True:
            try:
                self.version()
                version = self.connection.read_string()
                print(f'got version: {version}')
                break
            except UnicodeDecodeError:
                pass
        
        startstamp = (3600*self.startstamp.hour + 60*self.startstamp.minute + self.startstamp.second)%SECONDS_IN_12H
        self.set_time(startstamp)
        while True:
            period:float = self.period
            duty_cycle:int = self.duty_cycle
            fudged_period:float = period
            
            self.set_mode(self.mode)
            self.set_period(period)
            self.set_duty_cycle(duty_cycle)
            self.start_clock()
            
            datestamp,timestamp,split,error = self.get_split()
            while True:
                timestamp_ = timestamp
                split_ = split
                datestamp,timestamp,split,error = self.get_split()
                
                delta_split = (split-split_)%SECONDS_IN_12H
                delta_time = (timestamp-timestamp_)%SECONDS_IN_12H
                period = (59*period + fudged_period/(delta_time/delta_split))/60
                
                fudged_period = period+100*error
                fudged_period = min(1.25*period, fudged_period)
                fudged_period = max(0.80*period, fudged_period)
                fudged_period = round(fudged_period)
                print(f'{datestamp} | {timestamp =:10.3f} | {delta_time=:8.5f} | {delta_split=:8.5f} | {error=:10.6f} | {period=:10.4f} | {fudged_period=:10.4f}')
                
                self.set_period(fudged_period)
