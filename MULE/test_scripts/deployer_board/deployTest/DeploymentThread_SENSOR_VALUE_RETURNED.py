from worker_thread import WorkerThread
from eps_driver_Original import EPS_driver
import Queue
import time
import subprocess
import serial
'''
implement hardware failure messages
implement communication with the Arduino
'''
'''THIS IS THE DEPLOYMENT THREAD FOR THE TOTAL GROUND CONTROL SCHEME THAT HAS BEEN IMPLEMENTED ON THE FINAL STAGE
IT RECEIVES THE COMMAND TO DEPLOY A CERTAIN PAIR OF NODES AND DEPLOYS IT WHILE STORING SOME INFO ABOUT IT
IT CAN ALSO SET THE POWER TIME AND THE DUTY CYCLE ON THE DEPLOYER'''
# THIS CODE WORKS WELL WITH _2017_jun20_FULL_GROUND_CONTROL.ino
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



        self.EPS = EPS_driver()


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
        # if string == "DE:EjectNode":
        #     self.TestDeploy()
        if string == "DE:CheckSerial":
            self.Check_Serial()
        if string == "DE:lowPowerMode":
            self.lowPowerMode()
        if string == "DE:Deploy1":
            self.deploy_certain_pair(1)
        if string == "DE:Deploy2":
            self.deploy_certain_pair(2)
        if string == "DE:Deploy3":
            self.deploy_certain_pair(3)
        if string == "DE:Deploy4":
            self.deploy_certain_pair(4)
        if len(string) >= 15 and string[:15] == "DE:setPowerTime": # An example string is DE:setPowerTime15
            self.PowerTime = int(string[15:])
        if len(string) >= 15 and string[:15] == "DE:setDutyCycle":  # An example string is DE:setPowerTime15
            self.DutyCycle = int(string[15:])
        if len(string) >= 12 and string[:12] == "DE:IR_Sensor":  # An example string is DE:IR_Sensor1
            self.Check_IR_Sensor(int(string[-1]))

    # def TestDeploy(self):
    #     if self.There_are_Free_Nodes():
    #         print 'Command to deploy'
    #         self.deploy()
    #     print 'Deployment state complete'


    def init(self):
        print '==========init'
        self.interval = .5
        # self.log("Initializing thread with an interval of {0}".format(self.interval))
        self.PowerTime = 10
        #with open(DeploymentHealthFile, "w") as healthFile:
        #    subprocess.call(["echo", "Successful health file initialization"], stdout=healthFile)
        self.ser1 = serial.Serial(port = '/dev/ttyO5', baudrate = 57600, timeout = 10) #direct enabling uart instead of via function
        # self.ser1 = serial.Serial(port = '/dev/ttyO2', baudrate = 57600, timeout = 10) #direct enabling uart instead of via function
        # self.ser1 = serial.Serial(port = 'COM4', baudrate = 115200)
        self.ser1.flush()

        self.DutyCycle = 30

        self.executiveQueue.put("ED:DeployerInitialized")


    def deploy_certain_pair(self, n): # n is the number of the specified node pair
        print "==========Deploying a  CERTAIN pair of Sensor Nodes #" + str(n)

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
        if Result[:2] == '1:':
            self.Nodes[n - 1][1] += 1
            Qstring = 'ED:Deploy' + str(n) + 'Success'
            self.executiveQueue.put(Qstring)
            print('I, THE DEPLOYMENT THREAD, REPORT THAT THE IR SENSOR SHOWS' + Result)
        elif Result[:3] == '1:S':
            self.Nodes[n - 1][0] = 1
            Qstring = 'ED:Deploy' + str(n) + 'Failure'
            self.executiveQueue.put(Qstring)


    def loop(self):
        print '==========loop'
        try:
            executiveResponse = self.inputQueue.get(False)
            self.processResponse(executiveResponse)
        except Queue.Empty:
            pass


    def sendMessage(self, n):
        # This function sends a firing message to the arduino
        print '==========sendMessage'
        # transmit = self.ArduinoID + ":"+ str(n) +":"+ str(self.PowerTime).zfill(4) # Example: 1:1:0010
        transmit = "t:" + self.ArduinoID + ":"+ str(n) +":"+ str(self.PowerTime // 2).zfill(4) # Example: t:1:1:0010
        # WE MULTIPLY POWERTIME BY 0.5 DUE TO THE DIFFERENT BAUD RATES
        transmit += ":" + str(self.DutyCycle).zfill(2) # WE ARE NOT GOING TO USE THE DUTY CYCLE OF 100
        '''NOW AN EXAMPLE MESSAGE IS t:1:1:0010:35'''
        time.sleep(2)  # Essentially important delay; at least 2 seconds
        print(transmit.encode())
        self.ser1.write(transmit.encode())
        res = "0"
        while(res[0] != self.ArduinoID):

            '''////////////////////////////////////////'''
            # print("12 VOLT BUS:")
            # self.EPS.Pow_12v()
            # time.sleep(.5)
            # print("5 VOLT BUS:")
            # self.EPS.Pow_5v()
            # time.sleep(.5)
            # print("3.3 VOLT BUS:")
            # self.EPS.Pow_3v3()
            # time.sleep(.5)
            # print("RAW BATT BUS:")
            self.EPS.Pow_Batt()
            time.sleep(.05)
            '''////////////////////////////////////////'''

            temp = self.ser1.readline()
            print(temp.decode())
            if(len(temp) > 0):
                res = temp.decode()



        return res


    def Check_Serial(self):
        # This function checks the serial communication. It is very similar to the sendMessage, but with 3 as ArduinoID
        print '==========Check_Serial'
        transmit = "t:" + "3" + ":" + str(1) + ":" + str(self.PowerTime // 2).zfill(4)  # Example: t:1:1:0010
        # WE MULTIPLY POWERTIME BY 0.5 DUE TO THE DIFFERENT BAUD RATES
        transmit += ":" + str(self.DutyCycle).zfill(2)  # WE ARE NOT GOING TO USE THE DUTY CYCLE OF 100
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


    def Check_IR_Sensor(self, n):
        # This function checks the IR Sensor value at the current moment.
        # It is similar to the SendMessage and Check_Serial, but with ArduinoID == 4, 5, 6, or 7
        print '==========Check_IR_Sensor'
        transmit = "t:" + str(n + 3) + ":" + str(1) + ":" + str(self.PowerTime // 2).zfill(4)  # Example: t:1:1:0010
        # WE MULTIPLY POWERTIME BY 0.5 DUE TO THE DIFFERENT BAUD RATES
        transmit += ":" + str(self.DutyCycle).zfill(2)  # WE ARE NOT GOING TO USE THE DUTY CYCLE OF 100
        time.sleep(2)  # Essentially important delay; at least 2 seconds
        print(transmit.encode())
        self.ser1.write(transmit.encode())
        res = "0"
        k = 0
        while k < 20 and res.find("QRE:") == -1:
            temp = self.ser1.readline()
            print('I AM Check_IR_Sensor AND I GET' + str(temp.decode()) + '\n\n')
            if (len(temp) > 0):
                res = temp.decode()
            else:
                k = 20
            k += 1
            # time.sleep(3)

        response = res[res.find("QRE:") + 4:]
        response = response.rstrip('\n')
        # response = int(response)
        # print('I AM Check_Serial AND I GET' + str(res) + '\n\n\n\n\n\n\n')
        self.executiveQueue.put("ED:IR_Sensor" + str(n) + ":" + response)



#
# EQ = Queue.Queue()
# Deployer = DeploymentThread(EQ)
# time.sleep(10) # previously 30sec
# # while Deployer.DeployCount < 4: # This is the while condition that I previously had
# while Deployer.There_are_Free_Nodes():
#     print 'Command to deploy'
#     Deployer.singleRun()
#     time.sleep(10)
#     print 'Now DeployCount is:'
#     print Deployer.DeployCount
# print 'Deployment state complete'