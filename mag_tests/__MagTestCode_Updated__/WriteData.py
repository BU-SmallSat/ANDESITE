import serial
import datetime
dataFile = open('ISLtest160sps.txt', 'w')
strPort='COM3'
ser = serial.Serial(strPort, 115200)
while(1):
    #data = [float(val) for val in line.split()]
    try:
          line = ser.readline()
          data = [float(val) for val in line.split()]
          # print data
          
          if(len(data) == 2):
              dataFile.write(str(datetime.datetime.now()))
              dataFile.write(', ')
              dataFile.write(str(data[0]))
              dataFile.write('\n')
    except KeyboardInterrupt:
          dataFile.close()
          ser.flush()
          ser.close()
          break
