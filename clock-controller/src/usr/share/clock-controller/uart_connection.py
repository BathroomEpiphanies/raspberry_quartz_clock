import serial
from collections import deque


class UARTConnection:
    
    uart:serial.Serial
    
    def connect(
            self,
    ) -> None:
        self.uart.close()
        self.uart.open()
    
    def __init__(
            self,
            port:str,
    ) -> None:
        self.uart = serial.Serial(
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
            exclusive = None,
        )
        self.connect()
    
    def write_string(
            self,
            message:str,
    ) -> None:
        self.uart.write(f'{message}{chr(0)}'.encode('ascii'))
    
    def read_string(
            self,
    ) -> str:
        message:deque[str] = deque()
        while True:
            c = self.uart.read(1)
            if ord(c)==0:
                return ''.join(message)
            else:
                message.append(c.decode('ascii'))
