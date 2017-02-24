from worker_thread import WorkerThread
import Queue
import subprocess
import pynmea2
import serial

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
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue
        self.__serial = serial.Serial(
            port='/dev/ttyO2',
            baudrate=115200,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
        )

    def healthReport(self):
        # fill health string based on variables representing hardware components health
        healthString = "All GPS components are healthy"
        with open(GPSHealthFile, "w") as healthFile:
            subprocess.call(["echo", healthString], stdout=healthFile)
        self.executiveQueue.put("EC:healthBeacon")

    def processResponse(self,string):
        global Deploying
        global OrbitCount
        if string == "GE:LatWait":
            Deploying = 1
        elif string == "GE:ScienceMode":
            Deploying = 0
            OrbitCount = 0
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

    def read_gps(self):
        gps = ""
        while(self.__serial.in_waiting>0):
            gps = gps + self.__serial.read()
        print gps
        #lat = self.gps_translate(gps)
        #return lat


    def gps_translate(self,gps_input):
        #print(g)
        year = 2017
        month = 01
        day = 26

        msg = pynmea2.parse(gps_input)
        msg_str = str(msg)

     #   if ' ' in msg_str:
     #       print("There's a gap")
     #   else:

        timestamp = msg.timestamp
        time_str = str(timestamp)
        hr_str = time_str[0:2]
        hour = int(hr_str)

        min_str = time_str[3:5]
        min = int(min_str)

        sec_str = time_str[6:8]
        sec = int(sec_str)

        lat = msg.latitude
        lat_dir = msg.lat_dir

        lon = msg.longitude
        lon_dir = msg.lon_dir

        alt = msg.altitude
        alt_units = msg.altitude_units

        location = [round(lat,3), round(lon, 3), alt]
        # print("Location Array: ")
        print(location)

        time = [year, month, day, hour, min, sec]
        # print("Time Array: ")
        print(time)
        return round(lat,3)

    def init(self):
        self.interval = .1
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
        except Queue.Empty:
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
        '''
        #try:
        GPSlat = self.read_gps()
        #except pynmea2.ParseError:
            #print("Waiting on GPS input on Serial Line")
        onEquator = 0
        if GPSlat <1 and GPSlat > -1:
            if(onEquator == 0):
                onEquator = 1
                if Deploying == 1:
                    OrbitCount += 1
                    if OrbitCount >= DeployOrbitNum:
                        OrbitCount = 0
                        self.executiveQueue.put("EG:DeployLat")
        elif GPSlat >5 or GPSlat<-5:
            onEquator = 0
        '''