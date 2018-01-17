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
        self.LPM = 0
        self.LPM_threshold = 6.9
        self.LPM_threshold_exit = 7.75

    def EPShealthupdate(self):
        ##########################
        try:
            print("---*** EPS STATUS TEST ***---")
            self.fileSemaphore.acquire(blocking=False)
            with open(EPSHealthFile, "w") as dataFile:
                if self.debug == 1:
                    print("\nVOLTAGE BUSES HEALTH INFO")
                    print("***12 VOLT BUS***")
                dataFile.write("VOLTAGE BUSES HEALTH INFO\n")
                dataFile.write("***12 VOLT BUS***\n")
                result = self.EPS.voltage_12v()
                dataFile.write('12v voltage: ' + str(result) + '\n')
                time.sleep(self.waitTime)
                result = self.EPS.current_12v()
                dataFile.write('12v current: ' + str(result) + '\n')
                time.sleep(self.waitTime)

                if self.debug == 1:
                    print("***5 VOLT BUS***")
                dataFile.write("***5 VOLT BUS***\n")
                result = self.EPS.voltage_5v()
                dataFile.write('5v voltage: ' + str(result) + '\n')
                time.sleep(self.waitTime)
                result = self.EPS.current_5v()
                dataFile.write('5v current: ' + str(result) + '\n')
                time.sleep(self.waitTime)

                if self.debug == 1:
                    print("***3.3 VOLT BUS***")
                dataFile.write("***3.3 VOLT BUS***\n")
                result = self.EPS.voltage_3v3()
                dataFile.write('3.3v voltage: ' + str(result) + '\n')
                time.sleep(self.waitTime)
                result = self.EPS.current_3v3()
                dataFile.write('3.3v current: ' + str(result) + '\n')
                time.sleep(self.waitTime)

                if self.debug ==1:
                    print("***RAW BATT BUS***")
                dataFile.write("***RAW BATT BUS***\n")
                result = self.EPS.voltage_batt()
                dataFile.write('raw batt voltage: ' + str(result) + '\n')
                if result < self.LPM_threshold:
                    self.LPM = 1
                    self.executiveQueue.put("EP:lowPowerMode")
                    if self.debug == 1:
                        print("~~~~ENTERED LOW POWER MODE~~~~")
                    dataFile.write("~~~~ENTERED LOW POWER MODE~~~~\n")
                    dataFile.close()
                    self.fileSemaphore.release()
                    self.LowPowerMode()
                    return
                time.sleep(self.waitTime)
                result = self.EPS.current_batt()
                dataFile.write('raw batt current: ' + str(result) + '\n')
                time.sleep(self.waitTime)

                if self.debug ==1:
                    print("***BUS CURRENT DRAW***")
                dataFile.write("***BUS CURRENT DRAW***\n")
                result = self.EPS.currentDraw_5v()
                dataFile.write('5v current draw from EPS: ' + str(result) + '\n')
                time.sleep(self.waitTime)
                result = self.EPS.currentDraw_3v3()
                dataFile.write('3.3v current draw from EPS: ' + str(result) + '\n')
                time.sleep(self.waitTime)
                #########################

                if self.debug == 1:
                    print("\nBCR HEALTH INFO")
                    print("***BCR 6***")
                dataFile.write("BCR HEALTH INFO\n")
                dataFile.write("***BCR 6***\n")
                result = self.EPS.BCR_voltage(6)
                dataFile.write("BCR #6 voltage: " + str(result) + '\n')
                time.sleep(self.waitTime)
                result = self.EPS.BCR_currentA(6)
                dataFile.write("BCR #6 current on connector A:" + str(result) + '\n')
                time.sleep(self.waitTime)
                result = self.EPS.BCR_currentB(6)
                dataFile.write("BCR #6 current on connector B:" + str(result) + '\n')
                time.sleep(self.waitTime)

                if self.debug ==1:
                    print("***BCR 7***")
                dataFile.write("***BCR 7***\n")
                result = self.EPS.BCR_voltage(7)
                dataFile.write("BCR #7 voltage: " + str(result) + '\n')
                time.sleep(self.waitTime)
                result = self.EPS.BCR_currentA(7)
                dataFile.write("BCR #7 current on connector A:" + str(result) + '\n')
                time.sleep(self.waitTime)
                result = self.EPS.BCR_currentB(7)
                dataFile.write("BCR #7 current on connector B:" + str(result) + '\n')
                time.sleep(self.waitTime)

                if self.debug ==1:
                    print("***BCR 8***")
                dataFile.write("***BCR 8***\n")
                result = self.EPS.BCR_voltage(8)
                dataFile.write("BCR #8 voltage: " + str(result) + '\n')
                time.sleep(self.waitTime)
                result = self.EPS.BCR_currentA(8)
                dataFile.write("BCR #8 current on connector A:" + str(result) + '\n')
                time.sleep(self.waitTime)
                result = self.EPS.BCR_currentB(8)
                dataFile.write("BCR #8 current on connector B:" + str(result) + '\n')
                time.sleep(self.waitTime)

                if self.debug ==1:
                    print("***BCR 9***")
                dataFile.write("***BCR 9***\n")
                result = self.EPS.BCR_voltage(9)
                dataFile.write("BCR #9 voltage: " + str(result) + '\n')
                time.sleep(self.waitTime)
                result = self.EPS.BCR_currentA(9)
                dataFile.write("BCR #9 current on connector A:" + str(result) + '\n')
                time.sleep(self.waitTime)
                result = self.EPS.BCR_currentB(9)
                dataFile.write("BCR #9 current on connector B:" + str(result) + '\n')
                time.sleep(self.waitTime)
                dataFile.close()
                self.fileSemaphore.release()
        except:
            self.fileSemaphore.release()
            try:
                with open(EPSHealthFile, "a") as dataFile:
                    dataFile.write("ERROR COULDN'T COMMUNICATE WITH EPS")
                    dataFile.close()
            except:
                print("ERROR::COULDNT OPEN EPS DATA FILE")

    def LowPowerMode(self):
        #count = 1
        while(self.LPM == 1):
            print("WAITING IN LOW POWER MODE")
            result = self.EPS.voltage_batt()
            if result >= self.LPM_threshold_exit:
                self.executiveQueue.put("EP:lowPowerExit")
                self.LPM = 0
            #count = count +1
            #if count > 5:
                print("ATTEMPTING EXIT FROM LOW POWER MODE")
            #    self.executiveQueue.put("EP:lowPowerExit")
            #    self.LPM = 0
            time.sleep(60)


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
            self.executiveQueue.put("CP:DeployerOn")
        else:
            self.executiveQueue.put("CP:DeployerNotOn")

    def switchOffDeployer(self):
        self.EPS.switch_PDM_N_off(8)
        time.sleep(self.PCMdelay)
        resp = self.EPS.get_PDM_N_actual_status(8)
        if resp == 0:
            self.executiveQueue.put("CP:DeployerOff")
        else:
            self.executiveQueue.put("CP:DeployerNotOff")

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

    def processResponse(self, string):
        if string == "PC:restartEPS":
            self.restartEPS()
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
        elif string == "PG:enableDeployer":
            self.switchOnDeployer()
        elif string == "PG:disableDeployer":
            self.switchOffDeployer()

    def restartEPS(self):
        pass

    def init(self):
        self.interval = 10
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        #disable watchdog timer on EPS
        self.EPS.set_comms_watchdog_period(0)
        ret = self.EPS.get_comms_watchdog_period()
        if ret == 0:
            print("EPS watchdog timer successfully disabled")
        # threading.Timer(25, self.sendMessage).start()
        self.healthTimer = time.time()

    def loop(self):

        '''
        if self.EPSenabled:
            self.EPShealthupdate()
        '''
        if time.time() - self.healthTimer > 120:
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