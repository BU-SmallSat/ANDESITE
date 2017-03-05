from __future__ import print_function

import math
import time

import pynmea2


class magnetorquer_driver :
    def __init__(self):
        ## Globals
        self.com = "COM26"
        self.baud = 115200
        #freq = 1
        self.freq = 1/10
        self.res = .008
        self.g = "$GPGGA,000854,7924.00,N,00000.00,W,1,08,0.9,545.4,46.9*52"


    def incrange(self,start, end, step=1):
        """Generate a list that increments by "step" from the given bounds"""
        arr = []
        i = start - step
        while (i+step) <= end:
          arr.append(i+step)
          i += step
        return arr

    def decrange(self,start, end, step=1):
        """Generate a list that decrements by "step" from the given bounds"""
        arr = []
        i = start + step
        while (i-step) >= end:
          arr.append(i-step)
          i -= step
        return arr

    def conv_gps_deg(self,arr, deg):
        """
        Convert degree values to GPS string values
        :param arr:
        :param deg:
        :return:
        """
        for i in deg:
          val = math.floor(i)
          rem = (i - val) * 60
          arr.append(val*100 + rem)
        return arr

    def gps_lat(self):
        """
        Generate simulated GPS string latitudes with the given resolution
        :return:
        """
        top = 90
        bot = -top
        start = 65

        o1 = self.incrange(start, top, self.res)
        last = top - (o1[-1] + self.res - top)
        o2 = [i for i in self.decrange(last, bot, self.res)]
        last = bot - (o2[-1] - self.res - bot)
        o3 = [i for i in self.incrange(last, start, self.res)]
        ## Not evenly spaced, fix this

        lat = o1+o2+o3

        return lat




    def gps_lon(self,lat):
        """
        Generate simulate GPS string longitudes
        :param lat:
        :return:
        """
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




    def gps_time(self,lat):
        """
        Generate the time for each point in orbit
        :param lat:
        :return:
        """
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
        """
        Generate the GPS string in GLL format
        $GPGLL,4916.45 (Lat 49deg 16.45min) ,N,12311.12 (Long 123deg.11.12min),W,225444(hh:mm:ss),A,*1D

        $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47

        Where:
        GGA          Global Positioning System Fix Data
        123519       Fix taken at 12:35:19 UTC
        4807.038,N   Latitude 48 deg 07.038' N
        01131.000,E  Longitude 11 deg 31.000' E
        1            Fix quality: 0 = invalid
                                  1 = GPS fix (SPS)
                                  2 = DGPS fix
                                  3 = PPS fix
                       4 = Real Time Kinematic
                       5 = Float RTK
                                  6 = estimated (dead reckoning) (2.3 feature)
                       7 = Manual input mode
                       8 = Simulation mode
        08           Number of satellites being tracked
        0.9          Horizontal dilution of position
        545.4,M      Altitude, Meters, above mean sea level
        46.9,M       Height of geoid (mean sea level) above WGS84
                         ellipsoid
        (empty field) time in seconds since last DGPS update
        (empty field) DGPS station ID number
        *47          the checksum data, always begins with *
        :param lat:
        :param lon:
        :param time:
        :return:
        """
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





    def kml_generator(self,alt, lat, lon):
        """
        Generate the KML file
        :param alt:
        :param lat:
        :param lon:
        :return:
        """

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


    ##
    def gps_tx(self,gps):
        """
        Transmit GPS strings to sensor nodes
        :param gps:
        :return:
        """
        #port = serial.Serial(com, baud)
        count = 0
        step = 50
        #step = 1
        global g

        for g in gps:
            if (count % step) == 0:
                #print g
                self.gps_translate(g)
            #port.write(g+'\n')
            count = count + 1
            time.sleep(freq)

        #port.close()





    ## GPS stuff
    orb_num = 2
    orb_alt = 500000
    orb_inc = 90

    print("Starting GPS serial transfer...")

    #def main():
    while True:
        orb_lat = gps_lat()
        orb_lon = gps_lon(orb_lat)
        orb_time = gps_time(orb_lat)
        orb_gps = gps_str(orb_lat, orb_lon, orb_time)

        kml_generator(orb_alt, orb_lat, orb_lon)

        x = time.time()
        gps_tx(orb_gps)

        print (len(orb_lat))
        print (time.time() - x)


        #if __name__ == '__main__':
    #    try:
    #        main()
    #    except AttributeError:
    #        print(g)
