from worker_thread import WorkerThread
import Queue
import time
import subprocess
'''
implement hardware failure messages
implement communication with the Arduino
'''

# does deployment thread need to do any processing of messages from executive thread?

# health status file
DeploymentHealthFile = "/home/debian/Maria/healthFiles/Deployerhealth.txt"

class DeploymentThread(WorkerThread):
    def __init__(self, executive_queue):
        super(DeploymentThread,self).__init__("Deployment Thread")
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue
        self.DeployCount = 0

    def sendMessage(self):
        self.DeployCount += 1
        # self.executiveQueue.put("RD:updateNetwork") # only call this with a successful deployment
        if self.DeployCount >= 4:
            self.executiveQueue.put("ED:DeploymentComplete")
        else:
            self.executiveQueue.put("ED:NextDeployment")

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
            self.lowPowerMode()

    def init(self):
        self.interval = .5
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        with open(DeploymentHealthFile, "w") as healthFile:
            subprocess.call(["echo", "Successful health file initialization"], stdout=healthFile)

    def loop(self):
        try:
            executiveResponse = self.inputQueue.get(False)
            self.processResponse(executiveResponse)
        except Queue.Empty:
            pass
        print("Deploying a pair of Sensor Nodes")
        time.sleep(5)
        # the return from the arduino needs to be checked for a success or not successful message
        # if the return is successful, then we need to update the radio network to include the new WSN
        # else the radio thread doesnt need to be alerted
        # self.executiveQueue.put("RD:updateNetwork")
        print("Deployment success")
        self.sendMessage()
