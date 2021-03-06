from worker_thread import WorkerThread
import Queue
import serial
import time
import os


# health status file
RFM22BHealthFile = "/home/debian/Maria/healthFiles/RFM22Bhealth.txt"

# where to stop all the data files - how to name them different things based on the different orbits?
# maybe just transmit data chunks to ground and let everything be interpreted on the user level side?
RFM22BDataWSN = "/home/debian/Maria/dataFiles/data"
RFM22BDataWSNerror = "/home/debian/Maria/dataFiles/dataError"


networkString = ""
turnRadioOn = 0 # ability to reinitialize the radio?
radioSuccess = 0
dataFilesDir = "/home/debian/Maria/dataFiles"
dataFileList = "/home/debian/Maria/fileList"

class RFM22BThread(WorkerThread):
    def __init__(self, executive_queue, listSemaphore, downlinkSemaphore):
        super(RFM22BThread, self).__init__("RFM22B Thread")
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue
        self.device = "/dev/ttyO4"
        self.ser = serial.Serial(self.device, 115200, timeout = .5)
        self.ser.flush()
        self.orbitFiles = [0,0,0,0,0,0,0,0]
        self.WSNcontact = 0
        self.listSemaphore = listSemaphore
        self.downlinkSemaphore = downlinkSemaphore
        # time.sleep(.2)

    def safeMode(self):
        # can't turn off the arduino, but turn off the rf radio
        # this function should turn off all unnecessary hardware and functions for low power mode
        print("RF22B thread entering safe mode")

    def listFiles(self):
        print("CREATING LIST OF FILES")
        self.listSemaphore.acquire(blocking=True)
        files = [f for f in os.listdir(dataFilesDir) if os.path.isfile(os.path.join(DataFilesDir, f))]
        print(files)
        with open(dataFileList, "w") as fileList:
            for f in files:
                fileList.write(f)
            fileList.close()
        self.listSemaphore.release()
        print("RELEASING LIST OF FILES TO GLOBALSTAR THREAD")
        self.executiveQueue.put("CR:listFiles")

    def processResponse(self, string):
        global radioOn
        global radioSuccess
        global networkString
        if string == "RE:safeMode":
            self.radioShutdown()
            return 0
        elif string == "RC:requestNetwork":
            # send current network array to ground
            return 0
        elif string == "RC:radioInit":
            print("***initializing radio***")
            self.radioInit()
            return 0
        elif string == "RD:updateNetwork":
            self.updateNetwork()
            return 0
        elif string == "RC:restartRF22":
            self.restartRF22()
            return 0
        elif string == "RC:listFiles":
            self.listFiles()
            return 0
        elif "RC:radioNetwork:" in string:
            # how to relay this information
            self.radioNetwork(string[3:])
            return 0
        elif "RC:sensorNode" in string:
            self.relayToNode(string[13:])
            return 0
        elif string == "newNode:":
            self.newTextFile()
            return 1
        elif "RC:pull" in string:
            if self.currentFiles[int(string[8:9])] is not int(string[10:13]):
                self.executiveQueue.put("CR:downlinkRequestedFile")
                self.downlinkSemaphore.release()
            else:
                self.downlinkSemaphore.acquire(blocking=True)
                self.executiveQueue.put("CR:requestedDownlinkRefused")
        elif "updatedNetwork:" in string:
            print("rf network has been updated: "+string[:-2])
            networkString = string[15:-2]
            return 1
        elif string == "RF22failed\r\n":
            radioSuccess = 1
            print(string[:-2])
            return 1
        elif string == "RF22success\r\n":
            radioSuccess = 0
            print(string[:-2])
            return 1
        elif string == "setModemConfigFailed\r\n":
            print(string)
            # what to do here?? is this a case that ever happens?
            return 1
        else:
            return 0

    def radioInit(self):
        '''
        self.ser.write("radioInit\n".encode())
        message = ""
        while message != "radioInit\r\n":
            message = self.ser.readline().decode()
            #print(message)
            time.sleep(.1)
        '''
        print("arduino serial communication successful")
        #self.ser.write("radioInit\n".encode())
        #self.executiveQueue.put("ER:RadioInitSuccess")

    def radioShutdown(self):
        self.ser.write("radioShutdown\n".encode())

    def restartRF22(self):
        print("RESART RF22B")
        pass

    def relayToNode(self,message):
        self.ser.write(message.encode()) # add newline character to the end?  Most likely yes!

    def updateNetwork(self):
        print("update network command sent to arduino")
        self.ser.write("updateNetwork\n".encode())

    def radioNetwork(self,newNetwork):
        self.ser.write(newNetwork.encode())
        # self.ser.write("\n".encode())

    def newTextFile(self):
        global nextChar
        # add error handling to write text file even if string formating is wrong
        global RFM22BDataFile
        # self.ser.read() # trash return character following new line - IMPORTANT:: Add safer method of this!!
        # nextChar = str(self.ser.read())
        message = self.ser.readline().decode()
        # print("rewriting data file to: " +message)
        RFM22BDataFile = "/home/debian/Maria/dataFiles/"+message[:-2]+".txt"

    def sendDoneFile(self,wsnNum):
        RFM22BFile = RFM22BDataWSN + wsnNum+'Orb' + str(self.orbitFile) + ".txt"
        self.executiveQueue.put("ER:Done:"+RFM22BFile)

    def init(self):
        # test serial connection by reading specific text - setup loop on arduino should just send the same
        # initialization string until the connection is coordinated?
        # self.ser.flush()
        # time.sleep(1)
        # serial communication test -- improve by restarting arduino if this doesnt work
        global nextChar
        self.interval = 10
        self.log("Initializing thread with an interval of {0}".format(self.interval))

    def loop(self):

        queueEmpty = False
        while queueEmpty is False:
            try:
                executiveResponse = self.inputQueue.get(False)
                self.processResponse(executiveResponse)
                print(executiveResponse)
            except Queue.Empty:
                queueEmpty = True
        # how to read in data from the rf radio?
        # need method to distinguish messages from data
        # need method to turn byte stream into usable information
        # concatenate until we reach a new line - then compare string to messages - if none assume its data and store in
        # the data file, if it is a message - do appropriate processes

        #NEED TO PUT THIS IN A TRY CATCH LOOP?
        message = self.ser.readline().decode('UTF-8')



        # self.ser.read() # discard extra new line return character. IMPORTANT:: need to find safer way to do this!!
        #message = str(message)
        if not self.processResponse(message) and len(message) > 0:
            #print(message)
            # do we want to  automatically just write to the current data file
            if(message[3] == 'E'):
                #end of file
                self.sendDoneFile(message[0])
            elif(message[1] != ':'): #parse out ready commands
                if self.WSNcontact is 0:
                    self.WSNcontact = 1
                    self.executiveQueue.put("CR:WSNcontact")

                if(message[0] == '1'):
                    if(len(message) > 7):
                        try:
                            toInt = int(message[1:4])
                            self.orbitFile = toInt
                            RFM22BDataFile = RFM22BDataWSN + '1Orb' + str(toInt) + ".txt"
                        except:
                            RFM22BDataFile = RFM22BDataWSN + '1Orb' + str(self.orbitFile) + ".txt"
                elif(message[0] == '2'):
                    if(len(message) > 7):
                        try:
                            toInt = int(message[1:4])
                            self.orbitFile = toInt
                            RFM22BDataFile = RFM22BDataWSN + '2Orb' + str(toInt) + ".txt"
                        except:
                            RFM22BDataFile = RFM22BDataWSN + '2Orb' + str(self.orbitFile) + ".txt"
                elif(message[0] == '3'):
                    if(len(message) > 7):
                        try:
                            toInt = int(message[1:4])
                            self.orbitFile = toInt
                            RFM22BDataFile = RFM22BDataWSN + '3Orb' + str(toInt) + ".txt"
                        except:
                            RFM22BDataFile = RFM22BDataWSN + '3Orb' + str(self.orbitFile) + ".txt"
                elif(message[0] == '4'):
                    if(len(message) > 7):
                        try:
                            toInt = int(message[1:4])
                            self.orbitFile = toInt
                            RFM22BDataFile = RFM22BDataWSN + '4Orb' + str(toInt) + ".txt"
                        except:
                            RFM22BDataFile = RFM22BDataWSN + '4Orb' + str(self.orbitFile) + ".txt"
                elif(message[0] == '5'):
                    if(len(message) > 7):
                        try:
                            toInt = int(message[1:4])
                            self.orbitFile = toInt
                            RFM22BDataFile = RFM22BDataWSN + '5Orb' + str(toInt) + ".txt"
                        except:
                            RFM22BDataFile = RFM22BDataWSN + '5Orb' + str(self.orbitFile) + ".txt"
                elif(message[0] == '6'):
                    if(len(message) > 7):
                        try:
                            toInt = int(message[1:4])
                            self.orbitFile = toInt
                            RFM22BDataFile = RFM22BDataWSN + '6Orb' + str(toInt) + ".txt"
                        except:
                            RFM22BDataFile = RFM22BDataWSN + '6Orb' + str(self.orbitFile) + ".txt"
                elif(message[0] == '7'):
                    if(len(message) > 7):
                        try:
                            toInt = int(message[1:4])
                            self.orbitFile = toInt
                            RFM22BDataFile = RFM22BDataWSN + '7Orb' + str(toInt) + ".txt"
                        except:
                            RFM22BDataFile = RFM22BDataWSN + '7Orb' + str(self.orbitFile) + ".txt"
                elif (message[0] == '8'):
                    if(len(message) > 7):
                        try:
                            toInt = int(message[1:4])
                            self.orbitFile = toInt
                            RFM22BDataFile = RFM22BDataWSN + '8Orb' + str(toInt) + ".txt"
                        except:
                            RFM22BDataFile = RFM22BDataWSN + '8Orb' + str(self.orbitFile) + ".txt"
                else:
                    RFM22BDataFile = RFM22BDataWSNerror
                try:
                    with open(RFM22BDataFile, "a") as dataFile:
                        # print("writing to datafile: "+message)
                        # print("to datafile: " + RFM22BDataFile)
                        print("arduino message:: " +message)
                        dataFile.write(str(message[1:-2].decode('UTF-8')))
                        dataFile.close()
                except:
                    try:
                        with open(RFM22BDataFile, "a") as dataFile:
                            # print("writing to datafile: "+message)
                            # print("to datafile: " + RFM22BDataFile)
                            print("arduino message::" +message)
                            dataFile.write(str(message[1:-3].decode('UTF-8')))
                            dataFile.close()
                    except:
                        print("ERROR WITH PERMISSIONS::"+message)
            elif message[1] == ':':
                print("WSN"+message)