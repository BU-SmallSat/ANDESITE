from worker_thread import WorkerThread
import Queue
import time
import subprocess
import serial
'''
implement hardware failure messages
implement communication with the Arduino
'''

# does deployment thread need to do any processing of messages from executive thread?

# health status file
#DeploymentHealthFile = "/home/debian/Maria/healthFiles/Deployerhealth.txt"

class DeploymentThread(WorkerThread):

    def __init__(self, executive_queue):
        print('==========__init__')
        super(DeploymentThread,self).__init__("Deployment Thread")
        self.Nodes = [[0, 0], [0, 0], [1, 0], [0, 0]]
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue
        self.DeployCount = 1
        self.ManualControl = 0 #It is 1 when all nodes are either deployed or have 5 failing attempts
        self.ArduinoID = "1"
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
        self.interval = .5
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        self.PowerTime = 10
        #with open(DeploymentHealthFile, "w") as healthFile:
        #    subprocess.call(["echo", "Successful health file initialization"], stdout=healthFile)
        self.ser = serial.Serial(port = '/dev/ttyO1', baudrate = 115200, timeout = 10)
        self.ser.flush()

        # BeagleBone - Arduino Communication test:
        time.sleep(2)  # Essentially important delay; at least 2 seconds
        message = ""
        switch_timeout = 5
        restart_timeout = 10
        timer = 0
        while message != self.ArduinoID+":initSuccess\r\n":
            self.ser.write(self.ArduinoID.encode()+"initSuccess\n".encode())
            message = self.ser.readline().decode()
            timer += 1
            if timer > switch_timeout :
                self.ArduinoID = "2"
            if timer > restart_timeout:
                self.executiveQueue.put("PE:restartDeployers")
                return
        print("SUCCESS: Initialized serial communication with deployer arduino successful")

#implement method to read messages from deployer arduinos in case of an error!?

    def loop(self):
        try:
            executiveResponse = self.inputQueue.get(False)
            self.processResponse(executiveResponse)
        except Queue.Empty:
            pass
        i = 0
        Finish = 0
        Success = 0
        while Finish == 0:
            while i < 3 and self.Nodes[i][0] == 1 or (self.ManualControl == 0 and self.Nodes[i][1] >= 5):  # or self.Nodes[i][1] >= 5:
                i += 1
            if not (i >= 3 and self.Nodes[i][0] == 1):
                # while self.Nodes[i][0] == 0 and self.Nodes[i][1] < 5:
                Result = self.sendMessage(i + 1)
                if Result[:-2] == (self.ArduinoID+":Failure"):
                    self.Nodes[i][1] += 1
                    Success = 0
                    Qstring = 'ED:failed deployment of node #' + str(i + 1) + 'attempt #' + str(self.Nodes[i][1])
                    # self.executiveQueue.put(Qstring)
                elif Result[:-2] == (self.ArduinoID+":Success"):
                    Success = 1
                    self.Nodes[i][0] = 1
                    Qstring = 'ED:successful deployment of node #' + str(i + 1)
                    # self.executiveQueue.put(Qstring)
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
            print("SUCCESS: Deplpoyed pair of WSN")
        else:
            print("ERROR: Failed to deploy pair of WSN - reattempting ejection")

        # for k in range(4):
        #     self.Nodes[k][1] = 0
        self.sendMessage_to_exec_thread()


    def sendMessage(self, n):
        transmit = self.ArduinoID + ":"+ str(n) +":"+ str(self.PowerTime).zfill(4)
        time.sleep(2)  # Essentially important delay; at least 2 seconds
        self.ser.write(transmit.encode())
        return self.ser.readline().decode()