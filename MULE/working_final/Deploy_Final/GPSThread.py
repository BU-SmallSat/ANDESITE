from worker_thread import WorkerThread
import Queue
import subprocess
import pynmea2
import serial
import math
import time
import pynmea2


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

    def __init__(self, executive_queue,GPSenabled):
        super(GPSThread, self).__init__("GPS Thread")
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue
        self.GPSenabled = GPSenabled
        # Simulation Stuff
        self.com = "COM26"
        self.baud = 115200
        self.interval = 0
        '''self.__serial = serial.Serial(
            port='/dev/ttyO2',
            baudrate=115200,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
        )'''

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
        print (gps)
        #lat = self.gps_translate(gps)
        #return lat

    def gps_translate(self,gps_input):

        #print(gps_input)

        year = 2017
        month = 01
        day = 26

        msg = pynmea2.parse(gps_input)
        msg_str = str(msg)

        timestamp = msg.timestamp
        time_str = str(timestamp)
        hr_str = time_str[0:2]
        hour = int(hr_str)

        min_str = time_str[3:5]
        min = int(min_str)

        sec_str = time_str[6:8]
        sec = int(sec_str)

        lat = msg.latitude
        lat_str = str(lat)
        lat_dir = msg.lat_dir

        lon = msg.longitude
        lon_dir = msg.lon_dir

        alt = msg.altitude

        location = [round(lat,3), round(lon, 3), alt]
        #print(location)

        time = [year, month, day, hour, min, sec]
        #print(time)
        #print(lat)
        return round(lat,2)

    def init(self):
        self.g = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47"
        self.orb_gps = 0
        self.count = 0
        self.freq = 1/50
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        self.orb_lat = self.gps_lat()
        self.orb_lon = self.gps_lon(self.orb_lat)
        self.orb_time = self.gps_time(self.orb_lat)
        self.orb_gps = self.gps_str(self.orb_lat, self.orb_lon, self.orb_time)
        self.onEquator = 0

   #     with open(GPSHealthFile, "w") as healthFile:
   #         subprocess.call(["echo", "Successful health file initialization: GPS"], stdout=healthFile)


    def loop(self):
        gps = self.orb_gps[self.count]
        #print gps
        GPS_lat = self.gps_translate(gps)
        print(GPS_lat)

        #port.close()

        global GPSlat
        global GPSdir
        global DeployOrbitNum
        global OrbitCount
        global onEquator
        # self.log("Entered loop()")

        try:
            executiveResponse = self.inputQueue.get(False)
            self.processResponse(executiveResponse)
        except Queue.Empty:
            pass

        #try:
        #GPSlat = self.read_gps()
        #except pynmea2.ParseError:
            #print("Waiting on GPS input on Serial Line")


#############################################################################################################################################################################################


    ## #######################################
    ## ##### EVENLY SPACED LIST CREATION #####
    ## #######################################

    ## Generate a list that increments by "step" from the given bounds
    def incrange(self, start, end, step=1):
       arr = []

       i = start - step
       while (i+step) <= end:
          arr.append(i+step)

          i += step

       return arr


    ## Generate a list that decrements by "step" from the given bounds
    def decrange(self, start, end, step=1):
       arr = []

       i = start + step
       while (i-step) >= end:
          arr.append(i-step)

          i -= step

       return arr


    ## #########################################
    ## ##### CONVERT DEGREES TO GPS FORMAT #####
    ## #########################################

    ## Convert degree values to GPS string values
    def conv_gps_deg(self, arr, deg):
       for i in deg:
          val = math.floor(i)
          rem = (i - val) * 60
          arr.append(val*100 + rem)

       return arr


    ## ###########################
    ## ##### GPS INFORMATION #####
    ## ###########################

    ## Generate simulated GPS string latitudes with the given resolution
    def gps_lat(self):
        res = 60*(self.freq)
        top = 90
        bot = -top
        start = 65

        o1 = self.incrange(start, top, 1)
        last = top - (o1[-1] + res - top)
        o2 = [i for i in self.decrange(last, bot, 1)]
        last = bot - (o2[-1] - res - bot)
        o3 = [i for i in self.incrange(last, start, 1)]
        ## Not evenly spaced, fix this

        lat = o1+o2+o3
        #print lat
        return lat


    ## Generate simulate GPS string longitudes
    def gps_lon(self, lat):
        n = len(lat)
        last = 0

        lon = []
        for i in lat:
           if i >= last:
              lon.append(0)
           else:
              lon.append(18000.000)

           last = i
        #print lon
        return lon


    ## Generate the time for each point in orbit
    def gps_time(self, lat):
       n = len(lat)
       total = 90.0 * 60.0
       step = total / n

       time = []
       for i in range(0, n):
          val = int( round(i*step) )
          hour = (val / 3600) % 24
          minute = (val / 60) % 60
          sec = val % 60

          unit = str(hour).zfill(2) + str(minute).zfill(2) + str(sec).zfill(2)

          time.append(unit)

       return time

    def gps_str(self,lat, lon, time):
       head = "$GPGGA"
       lat_dir = "N"
       lon_dir = "W"
       tail = "A"

       last = 0
       newlat = self.conv_gps_deg([], lat)

       gps_str = []
       for i, j, k in zip(newlat, lon, time):
          if i < 0:
             i = -i
             lat_dir = "S"
          else:
             lat_dir = "N"

          newi = "{:07.2F}".format(i)
          newj = "{:08.2F}".format(j)
          newk = str(k)

          msg = pynmea2.GGA("GP", "GGA", (newk,newi,lat_dir, newj, lon_dir, "1","08","0.9","500000","46.9"))
          #print str(msg)
          gps_str.append(str(msg))
       #print gps_str
       return gps_str


GPSenabled = 0
inputQueue = Queue.Queue()
GPS = GPSThread(inputQueue,GPSenabled)
GPS.resume()