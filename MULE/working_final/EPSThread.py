from worker_thread import WorkerThread
import Queue
import time
from eps_driver import EPS_driver
from battery_driver import battery_driver

# health status file
EPSHealthFile = "/home/debian/Maria/healthFiles/EPShealth.txt"


class EPSThread(WorkerThread):

    def __init__(self, executive_queue, fileSemaphore):
        super(EPSThread,self).__init__("EPS Thread")
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue
        self.fileSemaphore = fileSemaphore
        self.debug = 1
        self.EPS = EPS_driver(self.debug)
        self.waitTime = .2
        self.PCMdelay = .5
        self.loopIter = 0
        self.V_vbatt = 0
        self.I_vbatt = 0

    def EPShealthupdate(self):
        ##########################
        print("---*** EPS STATUS TEST ***---")
        if self.debug == 1:
            print("\nVOLTAGE BUSES HEALTH INFO")
            print("***12 VOLT BUS***")
        self.EPS.Pow_12v()
        time.sleep(self.waitTime)
        if self.debug == 1:
            print("***5 VOLT BUS***")
        self.EPS.Pow_5v()
        time.sleep(self.waitTime)
        if self.debug == 1:
            print("***3.3 VOLT BUS***")
        self.EPS.Pow_3v3()
        time.sleep(self.waitTime)
        if self.debug ==1:
            print("***RAW BATT BUS***")
        [self.I_vbatt,self.V_vbatt,P_vbatt] = self.EPS.Pow_Batt()
        time.sleep(self.waitTime)
        if self.debug ==1:
            print("***BUS CURRENT DRAW***")
        self.EPS.currentDraw_5v()
        time.sleep(self.waitTime)
        self.EPS.currentDraw_3v3()
        time.sleep(self.waitTime)
        #########################
        if self.debug == 1:
            print("\nBCR HEALTH INFO")
            print("***BCR 1***")
        self.EPS.BCR_voltage(1)  # 1
        time.sleep(self.waitTime)
        self.EPS.BCR_currentA(1)
        time.sleep(self.waitTime)
        self.EPS.BCR_currentB(1)
        time.sleep(self.waitTime)
        '''        self.EPS.BCR_tempA(1)

        time.sleep(self.waitTime)
        self.EPS.BCR_tempB(1)
        time.sleep(self.waitTime)
        self.EPS.BCR_SunA(1)
        time.sleep(self.waitTime)
        self.EPS.BCR_SunB(1)
        time.sleep(wait_time)
        '''
        if self.debug ==1:
            print("***BCR 2***")
        self.EPS.BCR_voltage(2)  # 2
        time.sleep(self.waitTime)
        self.EPS.BCR_currentA(2)
        time.sleep(self.waitTime)
        self.EPS.BCR_currentB(2)
        time.sleep(self.waitTime)
        ''''
        self.EPS.BCR_tempA(2)
        time.sleep(self.waitTime)
        self.EPS.BCR_tempB(2)
        time.sleep(self.waitTime)
        self.EPS.BCR_SunA(2)
        time.sleep(self.waitTime)
        self.EPS.BCR_SunB(2)
        time.sleep(self.waitTime)
        '''
        if self.debug ==1:
            print("***BCR 3***")
        self.EPS.BCR3_voltage()  # 3
        time.sleep(self.waitTime)
        self.EPS.BCR_currentA(3)
        time.sleep(self.waitTime)
        self.EPS.BCR_currentB(3)
        time.sleep(self.waitTime)
        if self.debug ==1:
            print("***BCR 4***")
        self.EPS.BCR_voltage(4)  # 2
        time.sleep(self.waitTime)
        self.EPS.BCR_currentA(4)
        time.sleep(self.waitTime)
        self.EPS.BCR_currentB(4)
        time.sleep(self.waitTime)
        if self.debug ==1:
            print("***BCR 5***")
        self.EPS.BCR_voltage(5)  # 2
        time.sleep(self.waitTime)
        self.EPS.BCR_currentA(5)
        time.sleep(self.waitTime)
        self.EPS.BCR_currentB(5)
        time.sleep(self.waitTime)
        if self.debug ==1:
            print("***BCR 6***")
        self.EPS.BCR_voltage(6)  # 2
        time.sleep(self.waitTime)
        self.EPS.BCR_currentA(6)
        time.sleep(self.waitTime)
        self.EPS.BCR_currentB(6)
        time.sleep(self.waitTime)
        '''
        #####################
        print("\nGENERAL BOARD INFO")
        EPS.daughterBoard_temp()
        time.sleep(wait_time)
        EPS.motherBoard_temp()
        time.sleep(wait_time)
        '''
        self.executiveQueue.put("EP:healthTestSuccess")

    def EPSswitchesStatus(self):
        expectedStates = self.EPS.get_expected_state_of_all_PDMs()
        success = 1
        for i in range(1, 11):
            actualState = self.EPS.get_PDM_N_actual_status(i)
            if actualState != expectedStates[i]:
                success = 0
            #actualStates[i] = self.EPS.get_PDM_N_actual_status(i)
        if success == 0:
            self.executiveQueue.put("EP:SwitchStateFail")
        else:
            self.executiveQueue.put("EP:SwitchStateSuccess")

    def switchOnDeployer(self):
        self.EPS.switch_PDM_N_on(8)
        time.sleep(self.PCMdelay)
        resp = self.EPS.get_PDM_N_actual_status(8)
        if resp == 1:
            self.executiveQueue.put("EP:DeployerOn")
        else:
            self.executiveQueue.put("EP:DeployerOff")

    def switchOffDeployer(self):
        self.EPS.switch_PDM_N_off(8)
        time.sleep(self.PCMdelay)
        resp = self.EPS.get_PDM_N_actual_status(8)
        if resp == 0:
            self.executiveQueue.put("EP:DeployerOff")
        else:
            self.executiveQueue.put("EP:DeployerOn")

    def switchOnGPS(self):
        self.EPS.switch_PDM_N_on(10)
        time.sleep(self.PCMdelay)
        resp = self.EPS.get_PDM_N_actual_status(10)
        if resp == 1:
            self.executiveQueue.put("EP:GPSOn")
        else:
            self.executiveQueue.put("EP:GPSOff")

    def switchOffGPS(self):
        self.EPS.switch_PDM_N_off(10)
        time.sleep(self.PCMdelay)
        resp = self.EPS.get_PDM_N_actual_status(10)
        if resp == 0:
            self.executiveQueue.put("EP:GPSOff")
        else:
            self.executiveQueue.put("EP:GPSOn")

    def switchOnAntenna(self):
        self.EPS.switch_PDM_N_on(4)
        time.sleep(self.PCMdelay)
        resp = self.EPS.get_PDM_N_actual_status(4)
        if resp == 1:
            self.executiveQueue.put("EP:AntennaOn")
        else:
            self.executiveQueue.put("EP:AntennaOff")

    def switchOffAntenna(self):
        self.EPS.switch_PDM_N_off(4)
        time.sleep(self.PCMdelay)
        resp = self.EPS.get_PDM_N_actual_status(4)
        if resp == 0:
            self.executiveQueue.put("EP:AntennaOff")
        else:
            self.executiveQueue.put("EP:AntennaOn")

    def healthPoll(self):
        self.fileSemaphore.acquire(True)
        with open(GlobalstarHealthFile, "w") as healthFile:
            healthFile.write("EPS VBATT voltage: " + V_vbatt + "\n")
            healthFile.write("EPS VBATT current: " + I_vbatt + "\n")
            healthFile.close()
        self.fileSemaphore.release()

    def processResponse(self, string):
        if string == "PC:restartEPS":
            self.restartEPS()
        elif string == "PE:EPSHealth":
            self.EPShealthupdate()
        elif string == "PE:HealthPoll":
            self.healthPoll()
        elif string == "PE:EPSswitchesStatus":
            self.EPSswitchesStatus()
        elif string == "PE:switchOnAntenna":
            self.switchOnAntenna()
        elif string == "PE:switchOffAntenna":
            self.switchOffAntenna()
        elif string == "PE:switchOnGPS":
            self.switchOnGPS()
        elif string == "PE:switchOffGPS":
            self.switchOffGPS()
        elif string == "PE:switchOnDeployer":
            self.switchOnDeployer()
        elif string == "PE:switchOffDeployer":
            self.switchOffDeployer()

    def restartEPS(self):
        pass

    def init(self):
        self.interval = 10
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        # threading.Timer(25, self.sendMessage).start()
        self.healthTimer = time.time()

    def loop(self):

        '''
        if self.EPSenabled:
            self.EPShealthupdate()
        '''
        if time.time() - self.healthTimer > 60:
            self.EPShealthupdate()
            self.healthTimer = time.time()

        queueEmpty = False
        while queueEmpty is False:
            try:
                executiveResponse = self.inputQueue.get(False)
                self.processResponse(executiveResponse)
            except Queue.Empty:
                queueEmpty = True
        #check EPS voltages and currents every minute if it goes low send warning ground command
            #else just log most recent one in health file
        # *****put main code here****