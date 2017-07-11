import time
import serial

device = "/dev/ttyO4"
ser = serial.Serial(device, 115200, timeout = .5)
ser.write("initSuccess\n".encode())
message = ""
while message != "initSuccess\r\n":
    message = ser.readline().decode()
    print(message)
    time.sleep(.1)
print("arduino serial communication successful")
