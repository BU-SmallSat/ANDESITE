from .worker_thread import WorkerThread
import queue
import subprocess

# who needs to message the health thread? Should we be checking the messages more frequently?

# IMPORTANT: All these are in a separate folder - should we move them into the same folder - or create folder on startup?
# Health Beacon File location
HealthBeaconFile = "/home/debian/Maria/healthFiles/healthBeacon.txt"
# Health status files for other threads
ADCHealthFile = "/home/debian/Maria/healthFiles/ADCHealth.txt"
GlobalstarHealthFile = "/home/debian/Maria/healthFiles/Globalstarhealth.txt"
DeploymentHealthFile = "/home/debian/Maria/healthFiles/Deployerhealth.txt"
GPSHealthFile = "/home/debian/Maria/healthFiles/GPShealth.txt"
EPSHealthFile = "/home/debian/Maria/healthFiles/EPShealth.txt"
RFM22BHealthFile = "/home/debian/Maria/healthFiles/RFM22Bhealth.txt"
# do we need an executive thread health file - are there any health statuses to report?


class HealthThread(WorkerThread):

    def __init__(self, executive_queue):
        super(HealthThread, self).__init__("Health Thread")
        self.inputQueue = queue.Queue()
        self.executiveQueue = executive_queue

    def lowPowerMode(self):
        # this function should turn off all unnecessary hardware and functions for low power mode
        print("Health thread entering low power mode")

    def processResponse(self, string):
        if string == "HE:lowPowerMode":
            self.lowPowerMode()
        elif string == "HC:requestHealth":
            #how to relay this information
            pass

    def healthBeacon(self):
        # read all files into a single health beacon file
        with open(HealthBeaconFile, "w") as healthFile:
            subprocess.call(["cat", ADCHealthFile], stdout=healthFile)
        with open(HealthBeaconFile, "a") as healthFile:
            subprocess.call(["cat", GlobalstarHealthFile], stdout=healthFile)
            subprocess.call(["cat", DeploymentHealthFile], stdout=healthFile)
            subprocess.call(["cat", GPSHealthFile], stdout=healthFile)
            subprocess.call(["cat", EPSHealthFile], stdout=healthFile)
            subprocess.call(["cat", RFM22BHealthFile], stdout=healthFile)

        # send message to globalstar through executive thread
        self.executiveQueue.put("CH:healthBeacon")

    def init(self):
        self.interval = 10
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        with open(HealthBeaconFile, "w") as healthFile:
            subprocess.call(["echo", "Successful health file initialization"], stdout = healthFile)

    def loop(self):
        try:
            executiveResponse = self.inputQueue.get(False)
            self.processResponse(executiveResponse)
        except queue.Empty:
            pass
        #self.healthBeacon()
