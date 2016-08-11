import serial
import time
import threading

def writeContinue():
    print(time.ctime())
    mystring = "c"
    ser1.flush()
    ser1.write(mystring.encode('utf-8'))
    threading.Timer(90, writeContinue).start()

# this is to write al'%s:%s:%s'l of the data rates into a word document to analyze
connected = False

device1 = '/dev/cu.usbmodem1421'

#serin = ser.read()
# open text file to store failed and success data rates

text_file = open("AndesiteDataRate1 .txt", 'w')
#time.sleep(60)

#print ("Writing to word doc"), device1
ser1 = serial.Serial(device1, 115200)

# loop until the arduino tells us it is ready
while not connected:
    serin = ser1.read()
    connected = True

threading.Timer(90, writeContinue).start()

# write rates to text file
print(str(serin)[2], end="")
text_file.write(str(serin)[2])
#threading.Timer(.5,writeContinue).start()
def main():
    while 1:
        if ser1.inWaiting():
            x = ser1.read()
            myStr = str(x)
            if myStr[3] == "'":
                print(myStr[2], end="")
                text_file.write(myStr[2])
            else:
                text_file.write('\n')
                print('\n',end="")
                ser1.read()
            if myStr[2] == '*':
                print('test finished!')
                text_file.close()

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        # close the serial connection and text file
        text_file.close()
        ser1.close()
