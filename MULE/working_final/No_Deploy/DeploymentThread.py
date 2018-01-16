from worker_thread import WorkerThread
import Queue
import time
import serial
'''
implement hardware failure messages
implement communication with the Arduino
'''
'''THIS IS THE DEPLOYMENT THREAD FOR THE INCEASED GROUND CONTROL SCHEME THAT HAS BEEN IMPLEMENTED ON THE FINAL STAGE
IT RECEIVES THE COMMAND TO DEPLOY A CERTAIN PAIR OF NODES AND DEPLOYS IT WHILE STORING SOME INFO ABOUT IT'''
# THIS CODE WORKS WELL WITH _2017_jun1_SEPARATE_SERIAL_CHECK
# IT BURNS ONE WIRE AT A TIME, EXACTLY AS IT IS SUPPOSED TO DO ON THE ACTUAL SATELLITE

# does deployment thread need to do any processing of messages from executive thread?

# health status file
DeploymentHealthFile = "/home/debian/Maria/healthFiles/Deployerhealth.txt"

class DeploymentThread(WorkerThread):

    def __init__(self, executive_queue):
        print '==========__init__'
        super(DeploymentThread, self).__init__("Deployment Thread")
        self.Nodes = [[0, 0], [1, 0], [1, 0], [1, 0]]
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue
        self.DeployCount = 2
        self.ManualControl = 0 #It is 1 when all nodes are either deployed or have 5 failing attempts
        # self.FreeNodes = 4 #The number of undeployed nodes that have less than 5 failing attempts
        self.ArduinoID = "1"
        # self.PowerTime = 20

        self.Next_Node = 0 # This is the number of the next node pair to deploy if we need to deploy pair one at a time


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
        print ("Deployer thread entering low power mode")

    def processResponse(self, string):
        # if string == "DE:EjectNode":
        #     self.TestDeploy()
        if string == "DE:CheckSerial":
            self.Check_Serial()
        if string == "DE:lowPowerMode":
            self.lowPowerMode()
        if string == "DE:Deploy1":
            print ("==========Deploying a  CERTAIN pair of Sensor Nodes #" + str(n))
            #self.deploy_certain_pair(1)
        if string == "DE:Deploy2":
            print ("==========Deploying a  CERTAIN pair of Sensor Nodes #" + str(n))
            #self.deploy_certain_pair(2)
        if string == "DE:Deploy3":
            print ("==========Deploying a  CERTAIN pair of Sensor Nodes #" + str(n))
            #self.deploy_certain_pair(3)
        if string == "DE:Deploy4":
            print ("==========Deploying a  CERTAIN pair of Sensor Nodes #" + str(n))
            #self.deploy_certain_pair(4)

    def init(self):
        self.interval = 10
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        self.PowerTime = 10
        self.ser1 = serial.Serial(port = '/dev/ttyO5', baudrate = 57600, timeout = 10) #direct enabling uart instead of via function
        # self.ser1 = serial.Serial(port = '/dev/ttyO2', baudrate = 57600, timeout = 10) #direct enabling uart instead of via function
        # self.ser1 = serial.Serial(port = 'COM4', baudrate = 115200)
        self.ser1.flush()

    def deploy_certain_pair(self, n): # n is the number of the specified node pair

        '''////////////////////////////////////////'''
        # print("12 VOLT BUS:")
        self.EPS.Pow_12v()
        time.sleep(.5)
        # print("5 VOLT BUS:")
        self.EPS.Pow_5v()
        time.sleep(.5)
        # print("3.3 VOLT BUS:")
        self.EPS.Pow_3v3()
        time.sleep(.5)
        # print("RAW BATT BUS:")
        self.EPS.Pow_Batt()
        time.sleep(.5)
        '''////////////////////////////////////////'''

        Result = self.sendMessage(n)
        print ('Result = ' + str(Result))
        # time.sleep(120)
        if Result[:3] == '1:F':
            self.Nodes[n - 1][1] += 1
            Qstring = 'ED:Deploy' + str(n) + 'Success'
            self.executiveQueue.put(Qstring)
        elif Result[:3] == '1:S':
            self.Nodes[n - 1][0] = 1
            Qstring = 'ED:Deploy' + str(n) + 'Failure'
            self.executiveQueue.put(Qstring)


    def loop(self):
        queueEmpty = False
        while queueEmpty is False:
            try:
                executiveResponse = self.inputQueue.get(False)
                self.processResponse(executiveResponse)
            except Queue.Empty:
                queueEmpty = True


    def sendMessage(self, n):
        # This function sends a firing message to the arduino
        print ('==========sendMessage')
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


    def Check_Serial(self):
        # This function checks the serial communication. It is very similar to the sendMessage, but with 3 as ArduinoID
        print ('==========Check_Serial')
        transmit = "t:" + "3" + ":" + str(1) + ":" + str(self.PowerTime // 2).zfill(4)  # Example: t:1:1:0010
        # WE MULTIPLY POWERTIME BY 0.5 DUE TO THE DIFFERENT BAUD RATES
        time.sleep(2)  # Essentially important delay; at least 2 seconds
        print(transmit.encode())
        self.ser1.write(transmit.encode())
        res = "0"
        k = 0
        while k < 20 and res.find("privyet") == -1:
            temp = self.ser1.readline()
            print('I AM Check_Serial AND I GET' + str(temp.decode()) + '\n\n')
            if (len(temp) > 0):
                res = temp.decode()
            else:
                k = 20
            k += 1
            # time.sleep(3)

        # print('I AM Check_Serial AND I GET' + str(res) + '\n\n\n\n\n\n\n')
        if res.find("privyet") == -1:
            self.executiveQueue.put("ED:SerialDisconnected")
        else:
            self.executiveQueue.put("ED:SerialConnected")
        # return res