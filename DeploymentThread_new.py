from worker_thread import WorkerThread
import queue
import time
import subprocess
import serial
'''
implement hardware failure messages
implement communication with the Arduino
'''

# does deployment thread need to do any processing of messages from executive thread?

# health status file
DeploymentHealthFile = "/home/debian/Maria/healthFiles/Deployerhealth.txt"

class DeploymentThread(WorkerThread):

    def __init__(self, executive_queue):
        print('==========__init__')
        super().__init__("Deployment Thread")
        self.Nodes = [[0, 0], [0, 0], [1, 0], [0, 0]]
        self.inputQueue = queue.Queue()
        self.executiveQueue = executive_queue
        self.DeployCount = 1
        self.ManualControl = 0 #It is 1 when all nodes are either deployed or have 5 failing attempts
        # self.FreeNodes = 4 #The number of undeployed nodes that have less than 5 failing attempts

    def sendMessage_to_exec_thread(self):
        # self.DeployCount += 1
        if self.DeployCount >= 4:
            self.executiveQueue.put("ED:DeploymentComplete")
        elif self.There_are_Free_Nodes():
            self.executiveQueue.put("ED:NextDeployment")
        else:
            print('THERE IS NO FREE NODES')
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
        print("Deployer thread entering low power mode")

    def healthReport(self):
        # fill health string based on variables representing hardware components health
        healthString = "All Deployment components are healthy"
        with open(DeploymentHealthFile, "w") as healthFile:
            subprocess.call(["echo", healthString], stdout=healthFile)

    def processResponse(self, string):
        if string == "DE:lowPowerMode":
            self.    lowPowerMode()

    def init(self):
        print('==========init')
        self.interval = .5
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        self.PowerTime = 10
        #with open(DeploymentHealthFile, "w") as healthFile:
        #    subprocess.call(["echo", "Successful health file initialization"], stdout=healthFile)
        self.ser1 = self.UART_init(1)
        self.ser2 = self.UART_init(2)
        # self.ser3 = self.UART_init(5)
        self.ser4 = self.UART_init(4)
        # print('==========WE GOT SER4!!!')

        # self.ser = serial.Serial(port = '/dev/ttyO1', baudrate = 115200)
        # # self.ser = serial.Serial(port = 'COM4', baudrate = 115200)
        # self.ser.flush()
        #
        # # BeagleBone - Arduino Communication test:
        # st = "c"
        # st = st.encode('utf-8')
        # time.sleep(2)  # Essentially important delay; at least 2 seconds
        # self.ser.write(st)
        # Ans = self.ser.read()
        #
        # # while Ans == '\n':
        # #     Ans = self.ser.read()
        # #
        # if Ans != b'X':
        #     # print('BBB-ARD CONNECTION FAILURE')
        #     self.executiveQueue.put('PD:beaglebone-Arduino connection FAILED')
        # # else:
        # #     print('SUCCESSFULL CONNECTION')
        # #     self.executiveQueue.put('PD:beaglebone-Arduino connection FAILED')

    def UART_init(self, n):
        print("UART_init #", n)
        ser = serial.Serial(port = '/dev/ttyO' + str(n), baudrate = 115200, timeout = 10)
        # ser = serial.Serial(port = 'COM4', baudrate = 115200)
        ser.flush()

        # BeagleBone - Arduino Communication test:
        st = "c"
        st = st.encode('utf-8')
        time.sleep(2)  # Essentially important delay; at least 2 seconds
        ser.write(st)
        print('Waiting for Ans')
        Ans = ser.read()
        print('Ans = ', Ans)
        # while Ans == '\n':
        #     Ans = self.ser.read()
        #
        if Ans != b'X':
            # print('BBB-ARD CONNECTION FAILURE')
            self.executiveQueue.put('PD:beaglebone-Arduino connection FAILED')

        # else:
        #     print('SUCCESSFULL CONNECTION')
        #     self.executiveQueue.put('PD:beaglebone-Arduino connection FAILED')
        return ser


    def loop(self):
        print('==========loop')
        try:
            executiveResponse = self.inputQueue.get(False)
            self.processResponse(executiveResponse)
        except queue.Empty:
            pass
        print("==========Deploying a pair of Sensor Nodes")


        i = 0
        print('PUKUKUKUKUKUK', self.Nodes, self.DeployCount)
        Finish = 0
        Success = 0
        while Finish == 0:
            print("i =", i, 'before the loop')
            while i < 3 and self.Nodes[i][0] == 1 or (self.ManualControl == 0 and self.Nodes[i][1] >= 5):  # or self.Nodes[i][1] >= 5:
                i += 1
                print('increment i')
            print('i = ', i)
            if not (i >= 3 and self.Nodes[i][0] == 1):
                # while self.Nodes[i][0] == 0 and self.Nodes[i][1] < 5:
                Result = self.sendMessage(i + 1)
                print('Result =', Result)
                # time.sleep(120)
                if Result == b'F':
                    self.Nodes[i][1] += 1
                    # Success = 0
                    Qstring = 'ED:failed deployment of node #' + str(i + 1) + 'attempt #' + str(self.Nodes[i][1])
                    # self.executiveQueue.put(Qstring)
                elif Result == b'S':
                    Success = 1
                    self.Nodes[i][0] = 1
                    Qstring = 'ED:successful deployment of node #' + str(i + 1)
                    # self.executiveQueue.put(Qstring)
                print(self.Nodes)
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
            print("==========Deployment success", "DeployCount =", self.DeployCount)
        else:
            print("==========Deployment failure")

        # for k in range(4):
        #     self.Nodes[k][1] = 0
        self.sendMessage_to_exec_thread()


    def sendMessage(self, n):
        print('==========sendMessage')
        # self.ser.write(b'Start_the_deployment')
        # self.ser.close()
        if n == 1:
            ser = self.ser1
        elif n == 2:
            ser = self.ser2
        elif n == 3:
            ser = self.ser3
        elif n == 4:
            ser = self.ser4
        else:
            print('SUKA SUKA SUKA')
            ser = self.ser1

        if self.PowerTime < 10:
            msg = "c" + "0" + str(self.PowerTime)
        else:
            msg = "c" + str(self.PowerTime)
        # msg = "c"
        msg = msg.encode('utf-8')
        time.sleep(2)  # Essentially important delay; at least 2 seconds
        ser.write(msg)
        print(msg)
        # time.sleep(10)
        print('vvvvvvvvvvv')
        return ser.read()
