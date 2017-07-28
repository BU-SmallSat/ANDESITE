from worker_thread import WorkerThread
import Queue
from globalstar_serial import GlobalStarSerial
import Adafruit_BBIO.GPIO as GPIO
import time
import threading
import os
# Should we have a ground command asking how many messages are in the queue?
HealthBeaconFile = "/home/debian/Maria/healthFiles/healthBeacon.txt"

# health status file
GlobalstarHealthFile = "/home/debian/Maria/healthFiles/Globalstarhealth.txt"
dataFileList = "/home/debian/Maria/fileList"
WSNdataFile = "/home/debian/Maria/dataFiles/data"
'''
//*****************************//
//*******GROUND COMMANDS*******//
//*****************************//
'''

HELLO_CMD = "Hi ANDESITE!"
HEALTH_CMD = "Health Poll"
STD_BDOT_CMD = "Start Standard BDOT"
EN_DEPLOYER_CMD = "Enable Deployer Board"
DEPLOY_ONE_CMD = "Deploy Pair 1" #Pairs are ordered in sequence top to bottom
DEPLOY_TWO_CMD = "Deploy Pair 2"
DEPLOY_THREE_CMD = "Deploy Pair 3"
DEPLOY_FOUR_CMD = "Deploy Pair 4"
BDOT_TIMER_CMD = "Start BDOT:" #this string will be followed by desired duration of BDOT in seconds
EN_WSN_RADIO_CMD = "Enable RFM22B Radio"
LIST_FILES_CMD = "List Orbit Files" #respond with most recent 10 data files with naming convention including orbit number and node number






class GlobalstarThread(WorkerThread):

    def __init__(self, executive_queue, listSemaphore, downlinkSemaphore):
        super(GlobalstarThread, self).__init__("Globalstar Thread")
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue
        self.globalStar = GlobalStarSerial()
        # PC401 Health Statuses
        self.currentReset = 0  # current reset number (starts at 0, incremented for each reset, persistent over the life of the mission, 2 bytes)
        self.currentTime = 0  # current time from start of reset (seconds since last reset, 4 byte)
        self.rssi = 0  # (0 to 4, 1 byte)
        self.connectionStatus = 0  # (0 or 1, 1 byte)
        self.gateway = 0  # (0 to 256, 1 byte)
        self.contactTime = 0  # last contact time (seconds since last reset, 4 bytes)
        self.attemptTime = 0  # last attempt time (seconds since last reset, 4 bytes)
        self.opCodes = 0  # count of failed op codes (4 bytes)
        self.listSemaphore = listSemaphore
        self.downlinkSemaphore = downlinkSemaphore
        self.fileRequest = ""

    def GlobalstarEnable(self):
        #print("****Enabling Globalstar****")
        GPIO.setup("P9_15", GPIO.OUT)
        GPIO.output("P9_15", GPIO.HIGH)
        time.sleep(.5) #is this necessary
        print("****Enabling Globalstar****")


    def GlobalstarDisable(self):
        print("****Disabling Globalstar****")
        GPIO.output("P9_15", GPIO.LOW)
        self.executiveQueue.put("EC:GlobalstarDisabled")
        # send downlink to ground "Hello Andesite"

    def downlinkFileList(self):
        print('******DOWNLINKING LIST OF FILES******')
        self.listSemaphore.acquire(blocking=True)
        with open(dataFileList) as fileList:
            for line in fileList:
                print(line)
            fileList.close()
        print("*************************************")
        self.listSemaphore.release()

    def processResponse(self, string):
        if string == "CE:EnableGlobalstar":
            self.GlobalstarEnable()
        elif string == "CE:DisableGlobalstar":
            self.GlobalstarDisable()
        elif string == "CE:globalStarHealth":
            self.healthTest()
        elif string == "CE:HelloBU":
            print("DOWNLINKING HELLO BU MESSAGE")
        elif string == "CE:HealthPoll":
            print("DOWNLINKING HEALTH POLL TO GROUND")
        elif string == "CR:downlinkRequestedFile":
            self.downlinkDataFile()
        elif string == "CR:requestedDownlinkRefused":
            self.downlinkFileFailure()
        elif string == "CR:listFiles":
            self.downlinkFileList()
        elif "ER:Done:" in string:
            print("READY FOR DOWNLINK")
            pass
        elif string == "CR:WSNcontact":
            print("DOWNLINKING WSN CONTACT SUCCESS")

    def downlinkDataFile(self):
        print('******DOWNLINKING DATA FILES******')
        self.downlinkSemaphore.acquire(blocking=True)
        with open(self.fileRequest) as file:
            for line in fileList:
                print(line)
            fileList.close()
        print("*************************************")
        self.downlinkSemaphore.release()

    def downlinkFileFailure(self):
        print("FAILURE TO DOWNLINK DATA FILE - IN USE BY RFM22B THREAD")

    def healthTest(self):
        [rssi, connected] = self.globalStar.poll_health(2, .1)
        if connected == 1:
            response = "EC:ConnectionSucces:" + str(rssi)
            self.executiveQueue.put(response)
        elif connected == 0:
            self.executiveQueue.put("EC:ConnectionFail")
        else:
            self.executiveQueue.put("EC:gStarFail")

    def routeUplink(self, message):
        if "RC:pull" in message:
            self.fileRequest = WSNdataFile + message[8:9] + "Orb" + message[10:13] + ".txt"
            if os.path.isfile(self.fileRequest):
                print("sending message to executive queue: " + message)
                self.executiveQueue.put(message)
            else:
                print("FILE REQUESTED FROM GROUND DOESNT EXITS::"),
                print(self.fileRequest)
        else:
            print("sending message to executive queue: "+message)
            self.executiveQueue.put(message)

    def composeDownlink(self, file):
        '''
        # chunk the file into pieces small enough to send over globalstar radio
        # compress the files
        # get file information
        #compose the messages and send them over the serial comm
        for file in messageFiles
            response = GlobalstarSerial.send(file)
        #retry scheme if the message is not successfully sent?
        '''
        pass

    def readSMS(self):
        pass

    def init(self):
        self.interval = 10
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        # threading.Timer(8, self.routeUplink, ["RC:sensorNode2LPM"]).start()

    def loop(self):
        queueEmpty = False
        while queueEmpty is False:
            try:
                executiveResponse = self.inputQueue.get(False)
                self.processResponse(executiveResponse)
            except Queue.Empty:
                queueEmpty = True
        self.readSMS()
        # read something from the globalstar queue and route it to the executive queue
        #self.routeUplink()
