from .worker_thread import WorkerThread
import queue
import subprocess

# health status file
GPSHealthFile = "/home/debian/Maria/healthFiles/GPShealth.txt"

GPSlat = 0
GPSdir = 1
OrbitCount = 0
Deploying = 0
DeployOrbitNum = 5


# IMPORTANT:: everyone who reads the GPS value needs to check that the GPS is not out of service (read service variable)
# when this thread is paused it should update that variable
# how to make the GPS information available to the other threads


class GPSThread(WorkerThread):

    def __init__(self, executive_queue):
        super(GPSThread, self).__init__("GPS Thread")
        self.inputQueue = queue.Queue()
        self.executiveQueue = executive_queue

    def lowPowerMode(self):
        # this function should turn off all unnecessary hardware and functions for low power mode
        print("GPS thread entering low power mode")

    def healthReport(self):
        # fill health string based on variables representing hardware components health
        healthString = "All GPS components are healthy"
        with open(GPSHealthFile, "w") as healthFile:
            subprocess.call(["echo", healthString], stdout=healthFile)

    def processResponse(self,string):
        global Deploying
        global OrbitCount
        if string == "GE:LatWait":
            Deploying = 1
        elif string == "GE:ScienceMode":
            Deploying = 0
            OrbitCount = 0
        elif string == "GE:lowPowerMode":
            self.lowPowerMode()
        elif "GC:resetOrbitCount:" in string:
            self.resetOrbitCount(string[19:])
        elif string == "GE:healthRequest":
            self.healthReport()

    def resetOrbitCount(self,string):
        global DeployOrbitNum
        print("resetting orbit delay count to: " + string)
        # convert to an integer
        # update variable containing
        newNum = int(string)
        DeployOrbitNum = newNum

    def init(self):
        self.interval = .05
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        with open(GPSHealthFile, "w") as healthFile:
            subprocess.call(["echo", "Successful health file initialization"], stdout=healthFile)

    def loop(self):
        global GPSlat
        global GPSdir
        global DeployOrbitNum
        global OrbitCount
        # self.log("Entered loop()")
        try:
            executiveResponse = self.inputQueue.get(False)
            self.processResponse(executiveResponse)
        except queue.Empty:
            pass
        if GPSdir == 1:
            GPSlat += 1
            if GPSlat > 99:
                GPSdir = 0
        else:
            GPSlat -= 1
            if GPSlat < -99:
                GPSdir = 1
        if GPSlat == 0:
            if Deploying == 1:
                OrbitCount += 1
                if OrbitCount >= DeployOrbitNum:
                    OrbitCount = 0
                    self.executiveQueue.put("EG:DeployLat")
