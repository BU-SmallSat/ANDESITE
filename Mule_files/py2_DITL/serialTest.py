
import time
import serial

SYN1=chr(0x47).encode("utf-8")
SYN2=chr(0x55).encode("utf-8")
NAK=chr(0x0F).encode("utf-8")
ACK=chr(0x06).encode("utf-8")
LEN9 = (chr(0x00)+chr(0x00)+chr(0x00)+chr(0x09)).encode("utf-8")
LEN44 = (chr(0x00)+chr(0x00)+chr(0x00)+chr(0x2C)).encode("utf-8")


__serial = serial.Serial(
port = '/dev/ttyO4',
baudrate = 38400,
parity = serial.PARITY_NONE,
stopbits = serial.STOPBITS_ONE,
bytesize = serial.EIGHTBITS,
xonxoff = False,
rtscts = False,
dsrdtr = False,
timeout = 1,
write_timeout = None,
interCharTimeout = None
)
packet = "hello World".encode('UTF-8')
while(1):
    __serial.flushOutput()
    __serial.flushInput()
    __serial.write(packet)
    time.sleep(.5)
