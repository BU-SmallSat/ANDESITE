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

    def __init__(self, executive_queue):
        super(GPSThread, self).__init__("GPS Thread")
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue

        # Simulation Stuff
        self.com = "COM26"
        self.baud = 115200
        # freq = 1
        self.freq = 1 / 10
        self.res = .008
        self.g = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47"
        self.Orb_lat = 0
        self.orb_gps = 0
        self.count = 0
        orb_lat = self.gps_lat()
        orb_lon = self.gps_lon(orb_lat)
        orb_time = self.gps_time(orb_lat)
        self.orb_gps = self.gps_str(orb_lat, orb_lon, orb_time)



        #self.run()
        self.loop()

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
        self.interval = .05
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        orb_lat = self.gps_lat()
        orb_lon = self.gps_lon(orb_lat)
        orb_time = self.gps_time(orb_lat)
        self.orb_gps = self.gps_str(orb_lat, orb_lon, orb_time)
        self.onEquator = 0

   #     with open(GPSHealthFile, "w") as healthFile:
   #         subprocess.call(["echo", "Successful health file initialization: GPS"], stdout=healthFile)


    def loop(self):
        gps = self.orb_gps[self.count]
        GPS_lat = self.gps_translate(gps)
        #print(GPS_lat)

        if self.count < 361:
            self.count += 1
        else:
            self.count = 0
        #print(self.count)

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

        if GPS_lat <1 and GPS_lat > -1:
            if Deploying == 1:
                if(self.onEquator == 0):
                    OrbitCount += 1
                    self.onEquator = 1
                if OrbitCount >= DeployOrbitNum:
                    OrbitCount = 0
                    self.executiveQueue.put("EG:DeployLat")
        elif GPS_lat >5 or GPS_lat<-5:
            self.onEquator = 0
        #print(OrbitCount)

        '''
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
        top = 90
        bot = -top
        start = 65

        o1 = self.incrange(start, top, 1)
        last = top - (o1[-1] + self.res - top)
        o2 = [i for i in self.decrange(last, bot, 1)]
        last = bot - (o2[-1] - self.res - bot)
        o3 = [i for i in self.incrange(last, start, 1)]
        ## Not evenly spaced, fix this

        lat = o1+o2+o3

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



    ## ######################
    ## ##### GPS STRING #####
    ## ######################

    ## Generate the GPS string in GLL format
    ##     $GPGLL,4916.45 (Lat 49deg 16.45min) ,N,12311.12 (Long 123deg.11.12min),W,225444(hh:mm:ss),A,*1D
    ##
    ## $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
    ##
    ##Where:
    ##     GGA          Global Positioning System Fix Data
    ##     123519       Fix taken at 12:35:19 UTC
    ##     4807.038,N   Latitude 48 deg 07.038' N
    ##     01131.000,E  Longitude 11 deg 31.000' E
    ##     1            Fix quality: 0 = invalid
    ##                               1 = GPS fix (SPS)
    ##                               2 = DGPS fix
    ##                               3 = PPS fix
    ##			       4 = Real Time Kinematic
    ##			       5 = Float RTK
    ##                               6 = estimated (dead reckoning) (2.3 feature)
    ##			       7 = Manual input mode
    ##			       8 = Simulation mode
    ##     08           Number of satellites being tracked
    ##     0.9          Horizontal dilution of position
    ##     545.4,M      Altitude, Meters, above mean sea level
    ##     46.9,M       Height of geoid (mean sea level) above WGS84
    ##                      ellipsoid
    ##     (empty field) time in seconds since last DGPS update
    ##     (empty field) DGPS station ID number
    ##     *47          the checksum data, always begins with *

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
       #   print str(msg)
          gps_str.append( str(msg) )

       return gps_str


    ## Generate the GPS .kml file
    def kml_generator(self, alt, lat, lon):

        ## Write the header to the file
        kml = open("output.kml","w")
        kml.write('<?xml version="1.0" encoding="UTF-8"?>\n')
        kml.write('<kml xmlns="http://www.opengis.net/kml/2.2" xmlns:gx="http://www.google.com/kml/ext/2.2" xmlns:kml="http://www.opengis.net/kml/2.2" xmlns:atom="http://www.w3.org/2005/Atom">\n')
        kml.write("<Document>\n")
        kml.write("	<name>kml_output.kml</name>\n")
        kml.write('	<Style id="s_ylw-pushpin">\n')
        kml.write('		<IconStyle>\n')
        kml.write('			<scale>1.1</scale>\n')
        kml.write('			<Icon>\n')
        kml.write('				<href>http://maps.google.com/mapfiles/kml/pushpin/ylw-pushpin.png</href>\n')
        kml.write('			</Icon>\n')
        kml.write('			<hotSpot x="20" y="2" xunits="pixels" yunits="pixels"/>\n')
        kml.write('		</IconStyle>\n')
        kml.write('	</Style>\n')
        kml.write('	<Style id="s_ylw-pushpin_hl">\n')
        kml.write('		<IconStyle>\n')
        kml.write('			<scale>1.3</scale>\n')
        kml.write('			<Icon>\n')
        kml.write('				<href>http://maps.google.com/mapfiles/kml/pushpin/ylw-pushpin.png</href>\n')
        kml.write('			</Icon>\n')
        kml.write('			<hotSpot x="20" y="2" xunits="pixels" yunits="pixels"/>\n')
        kml.write('		</IconStyle>\n')
        kml.write('	</Style>\n')
        kml.write('	<StyleMap id="m_ylw-pushpin">\n')
        kml.write('		<Pair>\n')
        kml.write('			<key>normal</key>\n')
        kml.write('			<styleUrl>#s_ylw-pushpin</styleUrl>\n')
        kml.write('		</Pair>\n')
        kml.write('		<Pair>\n')
        kml.write('			<key>highlight</key>\n')
        kml.write('			<styleUrl>#s_ylw-pushpin_hl</styleUrl>\n')
        kml.write('		</Pair>\n')
        kml.write('	</StyleMap>\n')
        kml.write('	<Placemark>\n')
        kml.write('		<name>GPS Simulation</name>\n')
        kml.write('		<styleUrl>#m_ylw-pushpin</styleUrl>\n')
        kml.write('		<LineString>\n')
        kml.write('			<tessellate>1</tessellate>\n')
        kml.write('			<coordinates>\n')

        for i,j in zip(lat, lon):
            kml.write(str(j)+","+str(i)+","+str(alt)+" \n")

        kml.write('			</coordinates>\n')
        kml.write('		</LineString>\n')
        kml.write('	</Placemark>\n')
        kml.write('</Document>\n')
        kml.write('</kml>\n')
        kml.close()



    #if __name__ == '__main__':
    #    try:
    #        main()
    #    except AttributeError:
    #        print(g)
