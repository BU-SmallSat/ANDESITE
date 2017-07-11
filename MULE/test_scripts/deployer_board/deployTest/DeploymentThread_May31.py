from worker_thread import WorkerThread
import Queue
import time
import subprocess
import serial
'''
implement hardware failure messages
implement communication with the Arduino
'''
# THIS CODE WORKS WELL WITH sketch_jan29_Deployer_Test_for_direct_upload_from_IDE (but without firing)
# THIS CODE WORKS VERY WELL WITH sketch_apr10a_FOR_BOARD_CHANGED_PINS_FULL

# does deployment thread need to do any processing of messages from executive thread?

# health status file
DeploymentHealthFile = "/home/debian/Maria/healthFiles/Deployerhealth.txt"

class DeploymentThread(WorkerThread):

    def __init__(self, executive_queue):
        print '==========__init__'
        super(DeploymentThread, self).__init__("Deployment Thread")
        self.Nodes = [[0, 0], [0, 0], [0, 0], [0, 0]]
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue
        self.DeployCount = 2
        self.ManualControl = 0 #It is 1 when all nodes are either deployed or have 5 failing attempts
        # self.FreeNodes = 4 #The number of undeployed nodes that have less than 5 failing attempts
        self.ArduinoID = "1"
        self.PowerTime = 20

    def sendMessage_to_exec_thread(self):
        # This function send a result message to the ecexutive queue. NOT TO BE CONFUSED with just sendMessage!
        # self.DeployCount += 1
        if self.DeployCount >= 4:
            self.executiveQueue.put("ED:DeploymentComplete")
        elif self.There_are_Free_Nodes():
            self.executiveQueue.put("ED:NextDeployment")
        else:
            print 'THERE IS NO FREE NODES'
            self.executiveQueue.put("ED:AttemptsExceeded")


    def There_are_Free_Nodes(self):
        # Function checks whether there are undeployed nodes with less than 5 attempts
        Free = 4
        for k in range(4):
            if self.Nodes[k][0] == 1 or self.Nodes[k][1] >= 5:
                Free -= 1
        if Free == 0:
            return False
        else:
            return True

    def lowPowerMode(self):
        # this function should turn off all unnecessary hardware and functions for low power mode
        print "Deployer thread entering low power mode"

    def healthReport(self):
        # fill health string based on variables representing hardware components health
        healthString = "All Deployment components are healthy"
        with open(DeploymentHealthFile, "w") as healthFile:
            subprocess.call(["echo", healthString], stdout=healthFile)

    def processResponse(self, string):
        if string == "DE:EjectNode":
            self.TestDeploy()
        if string == "DE:lowPowerMode":
            self.lowPowerMode()

    def TestDeploy(self):
        if self.There_are_Free_Nodes():
            print 'Command to deploy'
            self.deploy()
        print 'Deployment state complete'
        self.sendMessage_to_exec_thread()

    def init(self):
        print '==========init'
        self.interval = .5
        # self.log("Initializing thread with an interval of {0}".format(self.interval))
        self.PowerTime = 10
        #with open(DeploymentHealthFile, "w") as healthFile:
        #    subprocess.call(["echo", "Successful health file initialization"], stdout=healthFile)

        # self.ser1 = self.UART_init(1)
        # self.ser2 = self.UART_init(2)


        self.ser1 = serial.Serial(port = '/dev/ttyO5', baudrate = 57600, timeout = 10) #direct enabling uart instead of via function
        # self.ser1 = serial.Serial(port = '/dev/ttyO2', baudrate = 57600, timeout = 10) #direct enabling uart instead of via function
        # self.ser1 = serial.Serial(port = 'COM4', baudrate = 115200)
        self.ser1.flush()



        # BeagleBone - Arduino Communication test:
        time.sleep(2)  # Essentially important delay; at least 2 seconds
        message = ""
        switch_timeout = 5
        restart_timeout = 10
        timer = 0
        print("Waiting for serial connection to arduino")
        while message[:13] != self.ArduinoID + ":initSuccess":  # originially ":initSuccess\r\n"
            self.ser1.write(self.ArduinoID.encode() + ":initSuccess".encode())  # originially ":initSuccess\n"
            print 'we wrote the message to arduino'
            message = self.ser1.readline()
            if message[:13] == '1:initSuccess':
                print 'PYTHON GOT THE RIGHT ANSWER'
            else:
                print 'python got wrong answer'
                print message
            # for i in range(0,len(message)):
            #     print(" %02x",message[i])
            # print("end of string")
            timer += 1
            # if timer > switch_timeout:
            #     self.ArduinoID = "2"
            if timer > restart_timeout:
                self.executiveQueue.put("PE:restartDeployers")
                return
        print("SUCCESS: Initialized serial communication with deployer arduino successful")
        self.executiveQueue.put("ED:DeployerInitialized")


    # implement method to read messages from deployer arduinos in case of an error!?
    # IMPLEMENT METHOD TO READ MESSAGES FROM DEPLOYER ARDUINOS IN CASE OF AN ERROR!?

    # def UART_init(self, n):
    # # This function used to initialize single arduinos in the old scheme of the deployment,
    # # when each arduino was reponsible for one node
    #     print "UART_init #:"
    #     print n
    #     # ser = serial.Serial(port = '/dev/ttyO' + str(n), baudrate = 115200, timeout = 10)
    #     ser = serial.Serial(port = 'COM4', baudrate = 115200)
    #     ser.flush()
    #
    #     # BeagleBone - Arduino Communication test:
    #     st = "c"
    #     st = st.encode('utf-8')
    #     time.sleep(2)  # Essentially important delay; at least 2 seconds
    #     ser.write(st)
    #     print 'Waiting for Ans'
    #     Ans = ser.read()
    #     print 'Ans = '
    #     print Ans
    #     # while Ans == '\n':
    #     #     Ans = self.ser.read()
    #     #
    #     if Ans != b'X':
    #         # print('BBB-ARD CONNECTION FAILURE')
    #         self.executiveQueue.put('PD:beaglebone-Arduino connection FAILED')
    #
    #     else:
    #         print 'SUCCESSFULL CONNECTION'
    #     #     self.executiveQueue.put('PD:beaglebone-Arduino connection FAILED')
    #     return ser


    def loop(self):
        print '==========loop'
        try:
            executiveResponse = self.inputQueue.get(False)
            self.processResponse(executiveResponse)
        except Queue.Empty:
            pass


    def deploy(self):
        print "==========Deploying a pair of Sensor Nodes"

        i = 0
        print 'PUKUKUKUKUKUK Nodes and DeployCount:'
        print self.Nodes
        print self.DeployCount
        Finish = 0
        Success = 0
        while Finish == 0:
            # print("i =", i, 'before the loop')
            while i < 3 and self.Nodes[i][0] == 1 or (
                    self.ManualControl == 0 and self.Nodes[i][1] >= 5):  # or self.Nodes[i][1] >= 5:
                i += 1
                print 'increment i, NOW i = '
                print i
            # print('i = ', i)
            if not (i >= 3 and self.Nodes[i][0] == 1):
                # while self.Nodes[i][0] == 0 and self.Nodes[i][1] < 5:
                Result = self.sendMessage(i + 1)
                print 'Result ='
                print Result
                # time.sleep(120)
                if Result[:3] == '1:F':  # Result == b'F':
                    self.Nodes[i][1] += 1
                    # Success = 0
                    Qstring = 'ED:failed deployment of node #' + str(i + 1) + 'attempt #' + str(self.Nodes[i][1])
                    # self.executiveQueue.put(Qstring)
                elif Result[:3] == '1:S':  # Result == b'S':
                    Success = 1
                    self.Nodes[i][0] = 1
                    Qstring = 'ED:successful deployment of node #' + str(i + 1)
                    # self.executiveQueue.put(Qstring)
                print self.Nodes
                # end of former while loop

            if self.Nodes[i][0] == 0 and i < 3:
                # if not Success and i < 3:
                i += 1
            else:
                Finish = 1

        # the return from the arduino needs to be checked for a success or not successful message
        # if the return is successful, then we need to update the radio network to include the new WSN
        # else the radio thread doesnt need to be alerted
        self.executiveQueue.put("RD:updateNetwork")

        if Success:
            # if self.Nodes[i][0] == 1:
            self.DeployCount += 1
            print "==========Deployment success"
            print "DeployCount ="
            print self.DeployCount
        else:
            print "==========Deployment failure"

        # for k in range(4):
        #     self.Nodes[k][1] = 0
        self.sendMessage_to_exec_thread()

    def sendMessage(self, n):
        # This function sends a firing message to the arduino
        print '==========sendMessage'
        # transmit = self.ArduinoID + ":"+ str(n) +":"+ str(self.PowerTime).zfill(4) # Example: 1:1:0010
        transmit = "t:" + self.ArduinoID + ":"+ str(n) +":"+ str(self.PowerTime // 2).zfill(4) # Example: t:1:1:0010
        # WE MULTIPLY POWERTIME BY 0.5 DUE TO THE DIFFERENT BAUD RATES
        time.sleep(2)  # Essentially important delay; at least 2 seconds
        print(transmit.encode())
        self.ser1.write(transmit.encode())
        res = "0"
        while(res[0] != self.ArduinoID):
            temp = self.ser1.readline()
            print(temp.decode())
            if(len(temp) > 0):
                res = temp.decode()
        return res
