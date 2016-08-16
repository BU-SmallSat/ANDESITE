from worker_thread_python2 import WorkerThread
import Queue
import time
import subprocess
import serial
from Adafruit_I2C import Adafruit_I2C

'''
implement hardware failure messages
implement communication with the Arduino
'''

'''
The code seemed to work well on testing with two and three Arduinos,
but some debugging still needs to be done. For example, I did not explicitly test
the mode when all nodes are deployed
'''

# does deployment thread need to do any processing of messages from executive thread?

# health status file
DeploymentHealthFile = "/home/debian/Maria/healthFiles/Deployerhealth.txt"

class DeploymentThread(WorkerThread):

    def __init__(self, executive_queue):
        super(DeploymentThread, self).__init__("Deployment Thread")
        self.Nodes = [[0, 0], [0, 0], [0, 0], [0, 0]] #First value in each sublist indicates whether the node was deployed,
        #  and the second value is the number of failed attempts to deploy each node
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue
        self.DeployCount = 0
        self.ManualControl = 0 #It is 1 when all nodes are either deployed or have 5 failing attempts
        # self.FreeNodes = 4 #The number of undeployed nodes that have less than 5 failing attempts

    def sendMessage_to_exec_thread(self):
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
        if string == "DE:lowPowerMode":
            self.    lowPowerMode()

    def init(self):
        self.interval = .5
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        self.PowerTime = 10
        #with open(DeploymentHealthFile, "w") as healthFile:
        #    subprocess.call(["echo", "Successful health file initialization"], stdout=healthFile)
        self.Ard1 = self.I2C_init(1)
        self.Ard2 = self.I2C_init(2)
        self.Ard3 = self.I2C_init(3)
        self.Ard4 = self.I2C_init(4)

    def I2C_init(self, n):
        Ard = Adafruit_I2C(n)
        # Communication test:
        Ans = Ard.readU8(14)
        if Ans != b'X':
            # print('BBB-ARD CONNECTION FAILURE')
            self.executiveQueue.put('PD:Beaglebone-Arduino connection FAILED')
        # else:
        #     print('SUCCESSFUL CONNECTION')
        return Ard

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
                Result = self.sendMessage(i + 1)
                if Result == 70:
                    self.Nodes[i][1] += 1
                elif Result == 83:
                    Success = 1
                    self.Nodes[i][0] = 1
                print self.Nodes

            if self.Nodes[i][0] == 0 and i < 3:
                i += 1
            else:
                Finish = 1


        # the return from the arduino needs to be checked for a success or not successful message
        # if the return is successful, then we need to update the radio network to include the new WSN
        # else the radio thread doesnt need to be alerted
        self.executiveQueue.put("RD:updateNetwork")

        if Success:
            self.DeployCount += 1
        # else:
        #     print "==========Deployment failure"

        self.sendMessage_to_exec_thread()


    def sendMessage(self, n):
        #This function send the deployment command to an Arduino
        if n == 1:
            Ard = self.Ard1
        elif n == 2:
            Ard = self.Ard2
        elif n == 3:
            Ard = self.Ard3
        elif n == 4:
            Ard = self.Ard4
        else:
            Ard = self.Ard1
        Ard.write8(100, self.PowerTime)
        time.sleep(self.PowerTime + 2)
        return Ard.readU8(114)

# EQ = Queue.Queue()
# Deployer = DeploymentThread(EQ)
# while Deployer.DeployCount < 4:
#     print 'Command to deploy'
#     Deployer.singleRun()
#     time.sleep(Deployer.PowerTime + 2)
#     print 'Now DeployCount is:'
#     print Deployer.DeployCount
# print 'Deployment state complete'
