import serial
TestFile = "/home/debian/Maria/dataFiles/CalibrationTest.txt"


device = "/dev/ttyO4"
ser = serial.Serial(device, 115200, timeout = .5)
ser.flush()

done = 0

while done == 0:
    message = ser.readline().decode('UTF-8')
    if len(message) > 0:
        print(message)
        if (message[3] == 'E'):
            # end of file
            done = 1
        elif (message[1] != ':'):  # parse out ready commands
            try:
                with open(TestFile, "a") as dataFile:
                    # print("writing to datafile: "+message)
                    # print("to datafile: " + RFM22BDataFile)
                    print("arduino message:: " + message)
                    dataFile.write(str(message[1:-2].decode('UTF-8')))
                    dataFile.close()
            except:
                print("****CANNOT OPEN DATA FILE****")


print("DONE WITH CALIBRATION TEST")