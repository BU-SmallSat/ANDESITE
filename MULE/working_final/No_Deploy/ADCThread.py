from worker_thread import WorkerThread
import Queue
from magnetorquer_driver import magnetorquer_driver
#from sunsensor_driver import sunsensor_driver
import time

DetumbleWait = 10

'''
include interface with new microcontroller
include ADC algorithms based on data derived from microcontroller
'''

# health status file
ADCHealthFile = "/home/debian/Maria/healthFiles/ADCHealth.txt"
ADCvector = ""

ALL_AXES = 0x00
POS_X_AXIS = 0x01
NEG_X_AXIS = 0x02
POS_Y_AXIS = 0x03
NEG_Y_AXIS = 0x04
POS_Z_AXIS = 0x05
NEG_Z_AXIS = 0x06

class ADCThread(WorkerThread):
    def __init__(self, executive_queue):
        super(ADCThread, self).__init__("ADC Thread")
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue
        self.MTQ = magnetorquer_driver()
        self.debug = 1
        self.MTMwait = 2
        #self.mpu9250 = MPU9250()

    def processResponse(self, string):
        if string == "AC:restartiMTQ":
            self.restartiMTQ()
        elif string == "AE:readMagData":
            self.readMagData()
        elif string == "AE:TwoOrbitBDOT":
            self.TwoOrbitBDOT()
        elif string == "AE:DisableBDOT":
            self.disableBDOT()

    def disableBDOT(self):
        #read iMTQ and verify that it is not in the
        state = self.MTQ.system_state()
        if state is not 0:
            self.MTQ.cancel_op()
        state = self.system_state()
        if state is 0:
            self.executiveQueue.put("EA:BDOTdisabled")
        else:
            self.executiveQueue.put("EA:disableFailure")
        return state

    def readMagData(self):
        success = 1
        if self.debug == 1:
            print("\nSTARTING MTM MEASUREMENTS")
            print("~~~ CALLING START MTM MEASUREMENTS FUNCTION ~~~")
        resp = self.MTQ.start_mtm()
        if resp == 0:
            success = 0
        time.sleep(self.MTMwait)
        if self.debug == 1:
            print("\nMTM STATUS INFO")
            print("***Raw MTM***")
        [resp, rawx, rawy, rawz] = self.MTQ.get_raw_mtm()
        if resp == 0:
            success = 0
        if self.debug == 1:
            print("***Calibrated MTM***")
        [resp, calx, caly, calz] = self.MTQ.get_cal_mtm()
        if resp == 0:
            success = 0
        if success == 0:
            self.executiveQueue.put("EA:magReadFail")
        else:
            self.executiveQueue.put("EA:magReadSuccess")


    def iMTQselfTest(self):
        self.MTQ.self_test(ALL_AXES)

    def restartiMTQ(self):
        self.MTQ.software_reset()

    def TwoOrbitBDOT(self):
        print("TWO ORBIT BDOT CONTROLLER")
        self.MTQ.TwoOrbitBDOT()

    def init(self):
        self.interval = 5 # frequency of the most frequently sampled ADC hardware (
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        self.MTQ.setup_watchdog(0)
        #threading.Timer(30, self.sendMessage).start()

    def loop(self):
        #print "IN ADC LOOP CALL"
        '''
        global Detumbling
        if self.IMUenabled:
            gyro = self.mpu9250.readGyro()
            print ("Gyro: ", [gyro[0], gyro[1], gyro[2]])
        if self.MAGenabled:
            self.MTQ.no_op()
        '''
        queueEmpty = False
        while queueEmpty is False:
            try:
                executiveResponse = self.inputQueue.get(False)
                self.processResponse(executiveResponse)
            except Queue.Empty:
                queueEmpty = True
            '''
            if Detumbling == 1:
            pass
            Detumbling = 0
            threading.Timer(2, self.sendMessage).start()
            '''