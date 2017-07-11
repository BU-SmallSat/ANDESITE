import logging
import Queue
import sys

from DeploymentThread import DeploymentThread
from EPSThread import EPSThread
from ADCThread import ADCThread
from HealthThread import HealthThread
from GlobalstarThread import GlobalstarThread
from RFM22BThread import RFM22BThread
from GPSThread_old import GPSThread


'''*******************GLOBAL SYSTEM DEFINES************************'''
EPSenabled=0
MAGenabled=0
GPSenabled=0
IMUenabled=0
WSN_RADIOenabled=0
DEPLOYenabled=0
MULE_RADIOenabled=0
'''****************************************************************'''
CurrentState =0

# setup threads and thread communication
inputQueue = Queue.Queue()   # E
Deployer = DeploymentThread(inputQueue,DEPLOYenabled)  # D
Power = EPSThread(inputQueue,EPSenabled)   # P
Comm = GlobalstarThread(inputQueue,MULE_RADIOenabled)  # C
RFM22B = RFM22BThread(inputQueue,WSN_RADIOenabled)   # R
Health = HealthThread(inputQueue)  # H
ADC = ADCThread(inputQueue,MAGenabled,IMUenabled)   # A
GPS = GPSThread(inputQueue,GPSenabled)  # G

# logging setup
logging.basicConfig(stream=sys.stdout, level=logging.INFO, format="%(asctime)s.%(msecs)03d:%(name)s:%(message)s",
                    datefmt='%Y-%m-%d,%H:%M:%S')
logger = logging.getLogger("main")

def ADCInterpreter(string):
    global ADCHealth
    global PointingState
    global DeployState
    if string == "EA:Pointing":
        PointingState = 1
        print("Done Detumbling Entering Pointing Mode")
        if DeployState == 1:
            return 2
        else:
            return CurrentState
    elif string == "EA:HealthUpdate":
        ADCHealth = 1
    else:
        return CurrentState

## develop profile for comm interpreter

def CommInterpreter(string):
# ground commands to configure which b-dot detumbling and pointing algorithm
# get and set methods for which method were using
    global EPSReturnState
    global globalstarHealth
    global PointingState
    global DeployState
    if string == "EC:lowPowerMode":
        EPSReturnState = CurrentState
        return 5
    elif string == "EC:safeMode":
        processMessage("CE:safeMode")
        processMessage("RE:safeMode")
        return 5
    elif string == "EC:deployState":
        DeployState =1
        print("***RECEIVED GROUND COMMAND: 'EC:deployNow'***")
        if PointingState == 1:
            return 2
        else:
            return CurrentState
    elif string == "EC:deployNow" and CurrentState != 5:
        # should we also not be able to deploy now from state 0?
        return
    elif string == "EC:HealthUpdate":
        globalstarHealth = 1
    # add restarts for temperature sensors and motor controller micro controllers
    else:
        return CurrentState


def DeployerInterpreter(string):
    global DeploymentHealth
    if string == "ED:DeploymentComplete" and CurrentState == 3:
        #send update to ground on current deployment state
        #set boolean that says we should accept grount commands for ejection attempts
        return 4
    elif string == "ED:NextDeployment" and CurrentState == 3:
        return 1
    elif string == "ED:AttemptsExceeded":
        #send update to ground on current deployment state
        #set boolean that says we should accept grount commands for ejection attempts
        return 4
    elif string == "ED:HealthUpdate":
        DeploymentHealth = 1
    else:
        return CurrentState


def GPSInterpreter(string):
    global gpsHealth
    if string == "EG:DeployLat" and CurrentState == 2:
        return 3
    elif string == "EG:HealthUpdate":
        gpsHealth = 1
    else:
        return CurrentState

## Develop profile for power interpreter

def HealthInterpreter(string):
    global EPSHealth
    global ADCHealth
    global DeploymentHealth
    global globalstarHealth
    global gpsHealth
    global RFM22BHealth
    if string == "CH:healthBeacon":
        #set all health variables to 0
        EPSHealth = 0
        ADCHealth = 0
        DeploymentHealth = 0
        globalstarHealth = 0
        gpsHealth = 0
        RFM22BHealth = 0
        #threading.Timer(HealthFreq, Health.healthBeacon, [ADCHealth, EPSHealth, DeploymentHealth, globalstarHealth, gpsHealth, RFM22BHealth]).start()
    return CurrentState


def PowerInterpreter(string):
    global EPSReturnState
    global MessageRecovery
    global EPSHealth
    global DeployState
    if string == "EP:HealthUpdate":
        EPSHealth = 1
    else:
        return CurrentState


def RFM22BInterpreter(string):
    global RF22BHealth
    if string == "ER:HealthUpdate":
        RF22BHealth = 1
    elif "ER:Done:" in string:
        processMessage("CE"+string[2:])
    return CurrentState

def processMessage(string):
    global MessageRecovery
    thread_char = string[0]
    subsystem_char = string[1]
    if thread_char == 'E':

        # include error handling for if subsystem char is not one of the items in the dictionary
        next_state = interpretMessage[subsystem_char](string)
        # how/where to update deploy state?
        return next_state
    else:
        # include error handling for if the thread char is not one of the items in the dictionary
        print("sending to thread: "+string)

        routeMessage[thread_char].put(string)
        return CurrentState

routeMessage = {'A': ADC.inputQueue, 'C': Comm.inputQueue, 'D': Deployer.inputQueue, 'G': GPS.inputQueue,
                'H': Health.inputQueue, 'P': Power.inputQueue, 'R': RFM22B.inputQueue}
interpretMessage = {'A': ADCInterpreter, 'C': CommInterpreter, 'D': DeployerInterpreter, 'G': GPSInterpreter,
                    'H': HealthInterpreter, 'P': PowerInterpreter, 'R': RFM22BInterpreter}

def waitForResp():
    while True:
        try:
            threadResponse = inputQueue.get(False)
            print(threadResponse)
            #response = processMessage(threadResponse)
            return threadResponse
        except Queue.Empty:
            pass


#Comm.resume()    ##periodically calling globalstar health beacon and reading executive messages
Power.resume()   ##periodically calling EPS health beacons and reading executive messages
ADC.resume()     ##periodically reading executive messages
#RFM22B.resume()  ##periodically reading executive messages
Deployer.resume()

Power.inputQueue.put("PE:EPSswitchesStatus")
response = waitForResp()
if response == "EP:SwitchStateSuccess":
    print("************ SUCCESSFUL EPS SWITCHERS STATUS EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH EPS SWITCHERS STATUS EXECUTION ----------")


Power.inputQueue.put("PE:switchOnAntenna")
response = waitForResp()
if response == "EP:AntennaOn":
    print("************ SUCCESSFUL EPS ANTENNA SWITCHER ENABLE EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH EPS ANTENNA SWITCHER ENABLE EXECUTION ----------")

Power.inputQueue.put("PE:switchOnGPS")
response = waitForResp()
if response == "EP:GPSOn":
    print("************ SUCCESSFUL EPS GPS SWITCHER ENABLE EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH EPS GPS SWITCHER ENABLE EXECUTION ----------")

Power.inputQueue.put("PE:switchOnDeployer")
response = waitForResp()
if response == "EP:DeployerOn":
    print("************ SUCCESSFUL EPS DEPLOYER SWITCHER ENABLE EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH EPS DEPLOYER SWITCHER ENABLE EXECUTION ----------")


Power.inputQueue.put("PE:EPSswitchesStatus")
response = waitForResp()
if response == "EP:SwitchStateSuccess":
    print("************ SUCCESSFUL EPS SWITCHERS STATUS EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH EPS SWITCHERS STATUS EXECUTION ----------")

'''
Comm.inputQueue.put("CE:enableGlobalstar")
##NEED ENABLES FOR THE EPS SWITCHES: gps, gps antenna, deployer board comm and vbatt, globalstar?
Comm.inputQueue.put("CE:globalStarHealth")
response = waitForResp()
if response == "EC:ConnectionFail" or  "EC:ConnectionSucces:" in response :
    print("************ SUCCESSFUL GLOBALSTAR HEALTH TEST EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH GLOBALSTAR HEALTH TEST EXECUTION ----------")
'''

Power.inputQueue.put("PE:EPSHealth")
response = waitForResp()
if response == "EP:healthTestSuccess":
    print("************ SUCCESSFUL EPS HEALTH TEST EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH EPS HEALTH TEST EXECUTION ----------")


ADC.inputQueue.put("AE:readMagData")
response = waitForResp()
if response == "EA:magReadSuccess":
    print("************ SUCCESSFUL MAG SELF TEST EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH MAG SELF TEST EXECUTION ----------")

'''
ADC.inputQueue.put("AE:iMTQSelfTest")
response = waitForResp()
if response == "EA:selfTestSuccess":
    print("************ SUCCESSFUL MAG SELF TEST EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH MAG SELF TEST EXECUTION ----------")
ADC.inputQueue.put("AE:Detumbling")
response = waitForResp()
if response == "EA:detumblingSuccess":
    print("************ SUCCESSFUL B-DOT TEST EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH B-DOT TEST EXECUTION ----------")
ADC.inputQueue.put("DE:SerialTest")
response = waitForResp()
if response == "EA:detumbleSuccess":
    print("************ SUCCESSFUL B-DOT TEST EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH B-DOT TEST EXECUTION ----------")
'''

'''
Deployer.inputQueue.put("DE:CheckSerial")
response = waitForResp()
if response == "ED:SerialConnected":
    print("************ SUCCESSFUL DEPLOYER BOARD CONNECTION ***************")
else:
    print("------- PROBLEM OCCURED WITH DEPLOYER BOARD CONNECTION ----------")
'''

Power.inputQueue.put("PE:switchOffAntenna")
response = waitForResp()
if response == "EP:AntennaOff":
    print("************ SUCCESSFUL EPS ANTENNA SWITCHER DISABLE EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH EPS ANTENNA SWITCHER DISABLE EXECUTION ----------")

Power.inputQueue.put("PE:switchOffGPS")
response = waitForResp()
if response == "EP:GPSOff":
    print("************ SUCCESSFUL EPS GPS SWITCHER DISABLE EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH EPS GPS SWITCHER DISABLE EXECUTION ----------")

Power.inputQueue.put("PE:switchOffDeployer")
response = waitForResp()
if response == "EP:DeployerOff":
    print("************ SUCCESSFUL EPS DEPLOYER SWITCHER DISABLE EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH EPS DEPLOYER SWITCHER DISABLE EXECUTION ----------")


Power.inputQueue.put("PE:EPSswitchesStatus")
response = waitForResp()
if response == "EP:SwitchStateSuccess":
    print("************ SUCCESSFUL EPS SWITCHERS STATUS EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH EPS SWITCHERS STATUS EXECUTION ----------")

'''
RFM22B.inputQueue.put(""RE:radioInit")
response = waitForResp()
if response == "ER:RadioInitSuccess":
    print("************ SUCCESSFUL RFM22B RADIO ENABLE EXECUTION ***************")
else:
    print("------- PROBLEM OCCURED WITH RFM22B RADIO ENABLE EXECUTION ----------")
'''
