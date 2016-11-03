from .worker_thread import WorkerThread
import queue
import threading
import subprocess
from .globalstar.globalstar_serial import GlobalStarSerial

# Should we have a ground command asking how many messages are in the queue?
HealthBeaconFile = "/home/debian/Maria/healthFiles/healthBeacon.txt"

# health status file
GlobalstarHealthFile = "/home/debian/Maria/healthFiles/Globalstarhealth.txt"


class GlobalstarThread(WorkerThread):

    def __init__(self, executive_queue):
        super(GlobalstarThread, self).__init__("Globalstar Thread")
        self.inputQueue = queue.Queue()
        self.executiveQueue = executive_queue

        # PC401 Health Statuses
        self.currentReset = 0  # current reset number (starts at 0, incremented for each reset, persistent over the life of the mission, 2 bytes)
        self.currentTime = 0  # current time from start of reset (seconds since last reset, 4 byte)
        self.rssi = 0  # (0 to 4, 1 byte)
        self.connectionStatus = 0  # (0 or 1, 1 byte)
        self.gateway = 0  # (0 to 256, 1 byte)
        self.contactTime = 0  # last contact time (seconds since last reset, 4 bytes)
        self.attemptTime = 0  # last attempt time (seconds since last reset, 4 bytes)
        self.opCodes = 0  # count of failed op codes (4 bytes)
    '''
    def lowPowerMode(self):
        # this function should turn off all unnecessary hardware and functions for low power mode
        print("GlobalStar thread entering low power mode")
    '''

    def healthReport(self):
        ## call command PC401 to GlobalStar device to get back stats
        # fill health string based on variables representing hardware components health
        healthString = "All Globalstar components are healthy"
        with open(GlobalstarHealthFile, "w") as healthFile:
            subprocess.call(["echo", healthString], stdout=healthFile)

    def processResponse(self, string):
        '''
        if string == "CE:lowPowerMode":
            self.lowPowerMode()
        '''
        if string == "CH:healthBeacon":
            print("Sending health beeacon to ground:")
            subprocess.call(["cat", HealthBeaconFile])
            self.composeDownlink(HealthBeaconFile)
        elif string == "CE:healthProfile":
            print("Creating health profile")
            self.healthReport()
            if self.connectionStatus == 1:
                self.executiveQueue.put("CE:0")
            else:
                self.executiveQueue.put("CE:1")

    def routeUplink(self, message):
        #message = GlobalstarSerial.receive_message()
        # case statements to determine who to send the message to by appending the correct to/from characters?
        # or will the messages from ground already have the correct formatting?
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

    def init(self):
        self.interval = .5
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        with open(GlobalstarHealthFile, "w") as healthFile:
            subprocess.call(["echo", "Successful health file initialization"], stdout=healthFile)
        threading.Timer(2, self.routeUplink, ["EC:deployState"]).start()
        # threading.Timer(8, self.routeUplink, ["RC:sensorNode2LPM"]).start()

    def loop(self):
        try:
            executiveResponse = self.inputQueue.get(False)
            self.processResponse(executiveResponse)
        except queue.Empty:
            pass
        # read something from the globalstar queue and route it to the executive queue
        #self.routeUplink()
