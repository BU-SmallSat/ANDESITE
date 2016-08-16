from worker_thread import WorkerThread
import Queue
import subprocess
import serial
import time
import threading


# health status file
RFM22BHealthFile = "/home/debian/Maria/healthFiles/RFM22Bhealth.txt"

# where to stop all the data files - how to name them different things based on the different orbits?
# maybe just transmit data chunks to ground and let everything be interpreted on the user level side?
RFM22BDataFile = "/home/debian/Maria/dataFiles/data.txt"
networkString = ""
turnRadioOn = 0 # ability to reinitialize the radio?
radioSuccess = 0

class RFM22BThread(WorkerThread):
    def __init__(self, executive_queue):
        super(RFM22BThread, self).__init__("RFM22B Thread")
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue
        self.device = "/dev/ttyO4"
        self.ser = serial.Serial(self.device, 115200, timeout = .5)
        self.ser.flush()

        # time.sleep(.2)

    def lowPowerMode(self):
        # can't turn off the arduino, but turn off the rf radio
        # this function should turn off all unnecessary hardware and functions for low power mode
        print("RF22B thread entering low power mode")

    def healthReport(self):
        # fill health string based on variables representing hardware components health
        healthString = "All RFM22B components are healthy"
        with open(RFM22BHealthFile, "w") as healthFile:
            subprocess.call(["echo", healthString], stdout=healthFile)

    def sendMessage(self):
        self.executiveQueue.put("EP:lowPowerMode")

    def processResponse(self, string):
        global radioOn
        global radioSuccess
        global networkString
        if string == "RE:lowPowerMode":
            self.lowPowerMode()
            return 0
        elif string == "RC:requestNetwork":
            # send current network array to ground
            return 0
        elif string == "RE:radioInit":
            self.radioInit()
        elif string == "RD:updateNetwork":
            self.updateNetwork()
            return 0
        elif "RC:radioNetwork:" in string:
            # how to relay this information
            self.radioNetwork(string[3:])
            return 0
        elif "RC:sensorNode" in string:
            self.relayToNode(string[13:])
        elif string == "RE:requestHealth":
            self.healthReport()
        elif string == "newNode:":
            self.newTextFile()
            return 1
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
        print("initialize radio")
        self.ser.write("radioInit\n".encode())

    def relayToNode(self,message):
        self.ser.write(message.encode()) # add newline character to the end?  Most likely yes!
        # self.ser.write("\n".encode())

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

    def init(self):
        global nextChar
        self.interval = 1
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        with open(RFM22BHealthFile, "w") as healthFile:
            subprocess.call(["echo", "Successful health file initialization"], stdout=healthFile)
        # test serial connection by reading specific text - setup loop on arduino should just send the same
        # initialization string until the connection is coordinated?
        # self.ser.flush()
        # time.sleep(1)
        # serial communication test -- improve by restarting arduino if this doesnt work
        self.ser.write("initSuccess\n".encode())
        message = ""
        while message != "initSuccess\r\n":
            message = self.ser.readline().decode()
            # print(message)
            time.sleep(.1)
        print("arduino serial communication successful")

    def loop(self):
        try:
            executiveResponse = self.inputQueue.get(False)
            self.processResponse(executiveResponse)
             # print(executiveResponse)
        except Queue.Empty:
            pass
        # how to read in data from the rf radio?
        # need method to distinguish messages from data
        # need method to turn byte stream into usable information
        # concatenate until we reach a new line - then compare string to messages - if none assume its data and store in
        # the data file, if it is a message - do appropriate processes
        message = self.ser.readline().decode()
        print("arduino message:: " +message)
        # self.ser.read() # discard extra new line return character. IMPORTANT:: need to find safer way to do this!!
        if not self.processResponse(message):
            # do we want to automatically just write to the current data file
            with open(RFM22BDataFile, "a") as dataFile:
                # print("writing to datafile: "+message)
                # print("to datafile: " + RFM22BDataFile)
                subprocess.call(["echo", message[:-2]], stdout=dataFile)
                dataFile.close()
