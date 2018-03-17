from worker_thread import WorkerThread
import Queue
from globalstar_serial import GlobalStarSerial
import time
import threading
import os
# Should we have a ground command asking how many messages are in the queue?
HealthBeaconFile = "/home/debian/Maria/healthFiles/healthBeacon.txt"

# health status file
HealthFile = "/home/debian/Maria/healthFiles/EPShealth.txt"
WSNdataFile = "/home/debian/Maria/dataFiles/data"
dataFileList = "/home/debian/Maria/groundFiles/filesList.txt"


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


dataFiles = "/home/debian/Maria/dataFiles/"
healthFiles = "/home/debian/Maria/healthFiles/"
groundFiles = "/home/debian/Maria/groundFiles/"


class GlobalstarThread(WorkerThread):

    def __init__(self, executive_queue,downlinkSemaphore,EPSsemaphore):
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
        self.downlinkSemaphore = downlinkSemaphore
        self.epsSemaphore = EPSsemaphore
        self.fileRequest = ""

    def GlobalstarEnable(self):
        #print("****Enabling Globalstar****")
        GPIO.setup("P9_15", GPIO.OUT)
        GPIO.output("P9_15", GPIO.HIGH)
        time.sleep(.5) #is this necessary
        print("****Enabling Globalstar****")
        self.executiveQueue.put("EC:GlobalstarEnabled")
        #threading.Timer(20, self.routeUplink, ["EC:HelloAndesite"]).start()
        #threading.Timer(30, self.routeUplink, ["EC:EnableDeployer"]).start()
        #threading.Timer(40, self.routeUplink, ["EC:deployPair1"]).start()
        #threading.Timer(45, self.routeUplink, ["RC:radioInit"]).start()
        #threading.Timer(50, self.routeUplink, ["RC:listFiles"]).start()
        #threading.Timer(100, self.routeUplink, ["RC:listFiles"]).start()
        #threading.Timer(105, self.routeUplink, ["RC:pull_1_001_file"]).start()
        #send downlink to ground "Hello Andesite" - but only on startup enable

    def GlobalstarDisable(self):
        print("****Disabling Globalstar****")
        GPIO.output("P9_15", GPIO.LOW)
        self.executiveQueue.put("EC:GlobalstarDisabled")
        # send downlink to ground "Hello Andesite"

    def processResponse(self, string):
        if string == "CE:EnableGlobalstar":
            self.GlobalstarEnable()
        elif string == "CE:DisableGlobalstar":
            self.GlobalstarDisable()
        elif string == "CE:HelloBU":
            self.composeDownlink("helloWorld.txt",groundFiles)
            print("DOWNLINKING HELLO BU MESSAGE")
        elif string == "CE:HealthPoll":
            self.downlinkHealthFile()
            print("DOWNLINKING HEALTH POLL TO GROUND")
        elif "CR:Done:" in string:
            print("READY FOR DOWNLINK")
            pass
        elif string == "CR:WSNcontact":
            self.composeDownlink("WSNcontact.txt",groundFiles)
            print("DOWNLINKING WSN CONTACT SUCCESS")
        elif string == "CP:DeployerOn":
            self.composeDownlink("DeployerOn.txt",groundFiles)
        elif string == "CP:DeployerNotOn":
            self.composeDownlink("DeployerFail.txt", groundFiles)
        elif string == "CP:DeployerOff":
            self.composeDownlink("DeployerOff.txt", groundFiles)
        elif string == "CP:DeployerNotOff":
            self.composeDownlink("DeployerFail.txt", groundFiles)
        elif string == "CD:SerialDisconnected" :
            self.composeDownlink("DeployerCommFail.txt",groundFiles)
        elif string == "CD:SerialConnected":
            self.composeDownlink("DeployerCommSuccess.txt",groundFiles)

    def downlinkFileList(self):
        print('******DOWNLINKING LIST OF FILES******')
        try:
            files = os.listdir(dataFiles)
            with open(dataFileList, 'w') as fileList:
                for line in files:
                    fileList.write(line)
                    fileList.write("\n")
                fileList.close()
            print("*************************************")
            self.composeDownlink("filesList.txt",groundFiles)
        except:
            self.composeDownlink("FileNotFound.txt", groundFiles)

    def downlinkDataFile(self):
        print('******DOWNLINKING DATA FILES******')
        try:
            self.downlinkSemaphore.acquire(blocking=False)
            with open(self.fileRequest) as file:
                for line in fileList:
                    print(line)
                fileList.close()
            print("*************************************")
            self.composeDownlink(self.fileRequest,dataFiles)
            self.downlinkSemaphore.release()
        except:
            self.downlinkSemaphore.release()
            self.composeDownlink("SendFileFail.txt", groundFiles)
            print("FAILURE TO DOWNLINK DATA FILE - IN USE BY RFM22B THREAD")

    def downlinkHealthFile(self):
        print('******DOWNLINKING HEALTH FILES******')
        try:
            self.epsSemaphore.acquire(blocking=False)
            with open(HealthFile) as file:
                for line in fileList:
                    print(line)
                fileList.close()
            print("*************************************")
            self.composeDownlink("EPShealth.txt", healthFiles)
            self.epsSemaphore.release()
        except:
            self.epsSemaphore.release()
            self.composeDownlink("SendFileFail.txt", groundFiles)
            print("FAILURE TO DOWNLINK DATA FILE - IN USE BY EPS THREAD")

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
        if "CG:pullFile" in message:
            self.fileRequest = WSNdataFile + message[11:12] + "Orb" + message[13:16] + ".txt"
            if os.path.isfile(self.fileRequest):
                self.downlinkDataFile()
            else:
                print("FILE REQUESTED FROM GROUND DOESNT EXITS::"),
                print(self.fileRequest)
                self.composeDownlink("FileNotFound.txt", groundFiles)
        elif "CG:remove" in message:
            file = WSNdataFile + message[9:10] + "Orb" + message[11:14] + ".txt"
            if os.path.isfile(file):
                try:
                    os.remove(file)
                except:
                    self.composeDownlink("DeleteFileFail.txt", groundFiles)
            else:
                print("FILE REQUESTED FROM GROUND DOESNT EXITS::"),
                print(file)
                self.composeDownlink("FileNotFound.txt", groundFiles)
        elif string == "CG:pullHealth":
            try:
                self.downlinkHealthFile()
            except:
                self.composeDownlink("HealtDownlinkFailed.txt", groundfiles) # # #
        elif string == "CG:HelloAndesite":
            print("CONTACT WITH GROUND STATION")
        elif string == "CG:listFiles":
            try:
                self.downlinkFileList()
            except:
                self.composeDownlink("FileListDownlinkFailed.txt", groundfiles) # # #
        elif string == "CG:clearQueue":
            try:
                self.globalStar.clear_downlink_files(10,.1)
            except:
                self.composeDownlink("HealtDownlinkFailed.txt", groundfiles) # # #
        else:
            print("sending message to executive queue: "+message)
            self.executiveQueue.put(message)

    def composeDownlink(self, file,path):
        '''
        # chunk the file into pieces small enough to send over globalstar radio
        # compress the files
        # get file information
        #compose the messages and send them over the serial comm
        for file in messageFiles
            response = GlobalstarSerial.send(file)
        #retry scheme if the message is not successfully sent?
        '''
        self.globalStar.sendFile(file, path, 30, .1)

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
        resp = self.globalStar.SMS_pull_poll(10, .1)
        if resp != "":
            parseCommand(resp)
        # read something from the globalstar queue and route it to the executive queue
        #self.routeUplink()
