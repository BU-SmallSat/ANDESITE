##
## CONTRIBUTORS:
##
##     * Osi Van Dessel
##     * Gabriel Gonzalez (gabeg@bu.edu)
##
##
## LICENSE:
##
##     None
##
##
## NAME:
##
##     gpskml - Creats a .kml file of a simulated satellite orbit.
##
##
## SYNTAX:
##
##     gpskml [-h]
##
##
## PURPOSE:
##
##     Create a .kml file that, when uploaded to a .kml file analyzer (such as
##     Google Earth), will display the orbit defined in the file.
##
##
## OPTIONS:
##
##     -h
##         Print program usage.
##
##
## FUNCTIONS:
##
##     Function - Function description.
##
##
## FILE STRUCTURE:
##
##     * Objective
##
##
## MODIFICATION HISTORY:
##
##     ovdessel Jan 23 2015 <> Created.
##
##     gabeg Jan 23 2015 <> Created.
##
##
## NOTICE:
##
##     Notice.
##
## **********************************************************************************



## ===============
## LIBRARY IMPORTS
## ===============

## Import libraries
import serial
import math
import time

## Globals
com = "COM10"
baud = 115200
freq = 1/50.0
VALUE = 60



## #######################################
## ##### EVENLY SPACED LIST CREATION #####
## #######################
################

## Generate a list that increments by "step" from the given bounds
def incrange(start, end, step=1):
   arr = []

   i = start - step
   while (i+step) <= end:
      arr.append(i+step)

      i += step

   return arr



## Generate a list that decrements by "step" from the given bounds
def decrange(start, end, step=1):
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
def conv_gps_deg(arr, deg):
   for i in deg:
      val = math.floor(i)
      rem = (i - val) * 60
      arr.append(val*100 + rem)

   return arr



## ###########################
## ##### GPS INFORMATION #####
## ###########################

## Generate simulated GPS string latitudes with the given resolution
def gps_lat():
    res = 360.0 / VALUE * freq
    top = 90
    bot = -top
    start = 65

    o1 = incrange(start, top, res)
    last = top - (o1[-1] + res - top)
    o2 = [i for i in decrange(last, bot, res)]
    last = bot - (o2[-1] - res - bot)
    o3 = [i for i in incrange(last, start, res)]
    ## Not evenly spaced, fix this

    lat = o1+o2+o3

    return lat



## Generate simulate GPS string longitudes
def gps_lon(lat):
    n = len(lat)
    last = 0

    lon = []
    for i in lat:
       if i >= last:
          lon.append(0)
       else:
          lon.append(180)

       last = i

    return lon



## Generate the time for each point in orbit
def gps_time(lat):
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
import pynmea2
def gps_str(lat, lon, time):
   head = "$GPGGA"
   lat_dir = "N"
   lon_dir = "W"
   tail = "A"

   last = 0
   newlat = conv_gps_deg([], lat)

   gps_str = []
   for i, j, k in zip(newlat, lon, time):
      if i < 0:
         i = -i
         lat_dir = "S"
      else:
         lat_dir = "N"

      newi = "{:7.2F}".format(i)
      newj = "{:08.2F}".format(j)
      newk = str(k)

      msg = pynmea2.GGA("GP", "GGA", (newk,newi,lat_dir, newj, lon_dir, "1","08","0.9","545.4","46.9"))
   #   print str(msg)
      gps_str.append( str(msg) )

   return gps_str





## Generate the GPS .kml file
def kml_generator(alt, lat, lon):

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


## Transmit GPS strings to sensor nodes
def gps_tx(gps):
    port = serial.Serial(com, baud)
    count = 0
    step = int( 1 / freq )

    for g in gps:
        if (count % step) == 0:
            print (g)
        port.write(g.encode()+"\n".encode())
        time.sleep(freq)
        count = count + 1

    port.close()


## GPS stuff
orb_num = 2
orb_alt = 500
orb_inc = 90

print ("Starting GPS serial transfer...")

while True:
   orb_lat = gps_lat()
   orb_lon = gps_lon(orb_lat)
   orb_time = gps_time(orb_lat)
   orb_gps = gps_str(orb_lat, orb_lon, orb_time)

   ## kml_generator(orb_alt, orb_lat, orb_lon)

   x = time.time()
   gps_tx(orb_gps)

   print (len(orb_lat))
   print (time.time() - x)
