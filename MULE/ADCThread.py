from __future__ import print_function

from worker_thread import WorkerThread
import Queue
import threading
import subprocess
from magnetorquer_driver import magnetorquer_driver
# from MPU9250 import MPU9250
# from SunSensorDriver import SunSensorDriver


DETUMBLE_WAIT = 10

'''
todo
include interface with new microcontroller
include ADC algorithms based on data derived from microcontroller
'''

# health status file
ADCHealthFile = "/home/debian/Maria/healthFiles/ADCHealth.txt"
ADCvector = ""


class ADCThread(WorkerThread):
    def __init__(self, executive_queue):
        super(ADCThread, self).__init__("ADC Thread")
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue
        self.MTQ = magnetorquer_driver()
        #self.mpu9250 = MPU9250()
        #self.SS = SunSensorDriver()

    def healthReport(self):
        # fill health string based on variables representing hardware components health
        healthString = "All ADC components are healthy"
        with open(ADCHealthFile, "w") as healthFile:
            subprocess.call(["echo", healthString], stdout=healthFile)

    def processResponse(self, string):
        global Detumbling
        if string == "AE:Detumbling":
            self.BDOT_detumble()
        elif "AC:ADCvector:" in string:
            self.changeVector(string[13:])
        elif string == "AC:burstVector":
            self.burstVector()
        elif string == "AC:restartIMU":
            self.restartIMU()
        elif string == "AC:restartSunsensor":
            self.restartSunSensor()
        elif string == "AC:restartiMTQ":
            self.restartiMTQ()

    def restartiMTQ(self):
        self.MTQ.software_reset()

    def restartIMU(self):
        #reset command?
        pass

    def restartSunSensor(self):
        #reset driver
        pass

    def burstVector(self):
        pass

    def changeVector(self,newVector):
        global ADCvector
        ADCvector = newVector

    def BDOT_detumble(self):
        self.MTQ.BDOT_detumble()
        threading.Timer(DETUMBLE_WAIT, self.startPointing).start()

    def startPointing(self):
        self.MTQ.pointing_mode()
        threading.Timer(DETUMBLE_WAIT, self.executiveQueue.put, ["EA:Pointing"]).start()

    def init(self):
        self.interval = .1 # frequency of the most frequently sampled ADC hardware (
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        with open(ADCHealthFile, "w") as healthFile:
            subprocess.call(["echo", "Successful health file initialization"], stdout=healthFile)
        #threading.Timer(30, self.sendMessage).start()

    def loop(self):
        #print "IN ADC LOOP CALL"
        global Detumbling
        #gyro = self.mpu9250.readGyro()
        #sunsensor = self.SS.read_data()
        #print "sunsensor: ",[sunsensor[0],sunsensor[1],sunsensor[2]]
        #print "Gyro: ", [gyro[0], gyro[1], gyro[2]]
        self.MTQ.no_op()
        try:
            executiveResponse = self.inputQueue.get(False)
            self.processResponse(executiveResponse)
        except Queue.Empty:
            pass
            # if Detumbling == 1:
            # Detumbling = 0
            # threading.Timer(2, self.sendMessage).start()
