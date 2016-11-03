import logging
import queue
import subprocess
import sys
from threading import Timer
from .DeploymentThread import DeploymentThread
from .EPSThread import EPSThread
from .ADCThread import ADCThread
from .HealthThread import HealthThread
from .GlobalstarThread import GlobalstarThread
from .RFM22BThread import RFM22BThread
from .GPSThread import GPSThread


'''
IMPORTANT::
How to handle the interplay between low power mode and deployment attempts - possible cases:
        low power mode in the queue when executing deployment
        deployment return from GPS in queue when in low power mode (both commands at approx. same time)
        deployment command in the midst of low power mode
In low power mode should the executive thread still look through messages from other threads and log them?
Should it wait till after low power mode to sift through these commands?
    Should it execute all commands that it received during low power mode?
    if not which commands should it execute/ignore?
What to do if the low power command is called from ground when the satellite is already in low power mode
How to return from Low power mode? Should we continue executing what we were doing when we left?
    or should we first look through the messages in the queue and execute those?
    should we rank the messages that we missed and the tasks that we were doing so that we execute the most important
        ones first? or should we do them based on when they came in?

Edit the dictionaries to do error handling if the string isn't correct
'''

'''
Should we check all the messages in the queue before we enter low power mode to see if they indicate that we finished
operations for the state we were in (i.e deployment complete?) to ensure that we transition back to the correct state
when we return from low power mode
'''

# Health Status Report File location
HealthBeaconFile = "/home/debian/Maria/healthFiles/healthBeacon.txt"
GlobalstarHealthFile = "/home/debian/Maria/healthFiles/Globalstarhealth.txt"
GPSHealthFile = "/home/debian/Maria/healthFiles/GPShealth.txt"
# ADC, Deployer, GPS, Radio, Comm, EPS
simple_status = [0,0,0,0,0,0] #basic requirements of whether they are not functioning and need to be restarted
lpm = 0 # low power mode status: 1-Low Power Mode, 0-Normal

# IMPORTANT:: need a method to start the radio on the Mule if the node ejected was the first one
startNetwork = 0

# setup threads and thread communication
inputQueue = queue.Queue()   # E
Deployer = DeploymentThread(inputQueue)  # D
Power = EPSThread(inputQueue)   # P
Comm = GlobalstarThread(inputQueue)  # C
RFM22B = RFM22BThread(inputQueue)   # R
Health = HealthThread(inputQueue)  # H
ADC = ADCThread(inputQueue)   # A
GPS = GPSThread(inputQueue)  # G


def initialization():
    # this will require initialization of necessary components required for
    # individual threads to do their own hardware initialization
    # specifics need to be identified
    print("system initialization")
    # watchdog = Watchdog()
    ## working with BeagleBone

    #  Test initializing the health file
    with open(HealthBeaconFile, "w") as healthFile:
        subprocess.call(["echo", "Successful health file initialization"], stdout=healthFile)

def loop():
    if lpm == 0: #only check health when it isn't in low power mode
        failureStatus()
    else:
        pass

def lowPowerMode():
    ADC.pause()
    # ADC.inputQueue.put("AE:lowPowerMode")
    Comm.pause()
    # Comm.inputQueue.put("CE:lowPowerMode")
    Deployer.pause()
    # Deployer.inputQueue.put("DE:lowPowerMode")
    GPS.pause()
    # GPS.inputQueue.put("GE:lowPowerMode")
    Health.pause()
    # Health.inputQueue.put("HE:lowPowerMode")
    RFM22B.pause()
    # RFM22B.inputQueue.put("RE:lowPowerMode")
    Power.inputQueue.put("PE:lowPowerMode")  # to alert the EPS it has paused the necessary threads

class Watchdog:
    def __init__(self, timeout, userHandler=None):  # timeout in seconds
        self.timeout = timeout
        self.handler = userHandler if userHandler is not None else self.defaultHandler
        self.timer = Timer(self.timeout, self.handler)

    def reset(self):
        self.timer.cancel()
        self.timer = Timer(self.timeout, self.handler)

    def stop(self):
        self.timer.cancel()

    def defaultHandler(self):
        raise self

def returnFromLPM():
    lpm = 0
    ADC.resume()
    Comm.resume()
    GPS.resume()
    Health.resume()
    RFM22B.resume()


def ADCInterpreter(string):
    if string == "EA:Pointing" and CurrentState == 1:
        return 2
    elif string == "EA:1":
        simple_status[0] = 1
        Power.inputQueue.put("PE:restartIMU")
        Power.inputQueue.put("PE:restartSunsensor")
        Power.inputQueue.put("PE:restartADC")
    elif string == "EA:0":
        simple_status[0] = 0
    else:
        return CurrentState

## develop profile for comm interpreter

def CommInterpreter(string):
# ground commands to configure which b-dot detumbling and pointing algorithm
# get and set methods for which method were using
    global EPSReturnState
    if string == "EC:lowPowerMode":
        EPSReturnState = CurrentState
        return 5
    elif string == "EC:deployState" and CurrentState == 0:
        # should we be able to re-enter deploy state from other states?
        return 1
    elif string == "EC:deployNow" and CurrentState != 5:
        # should we also not be able to deploy now from state 0?
        return 3
    elif string == "EC:restartRF22":
        RFM22B.pause()
        processMessage("PE:restartRF22")
        return CurrentState
    elif string == "EC:restartIMU":
        ADC.pause()
        processMessage("PE:restartIMU")
        return CurrentState
    elif string == "EC:restartSunsensor":
        ADC.pause()
        processMessage("PE:restartSunsensor")
        return CurrentState
    elif string == "EC:restartGPS":
        GPS.pause()
        processMessage("PE:restartGPS")
        return CurrentState
    elif string == "EC:restartDeployers":
        Deployer.pause()
        processMessage("PE:restartDeployers")
        return CurrentState
    elif string == "EC:1":
        simple_status[4] = 1
        Power.inputQueue.put("CE:restartGlobalstar")
    elif string == "EC:0":
        simple_status[4] = 0
    # add restarts for temperature sensors and motor controller micro controllers
    else:
        return CurrentState


def DeployerInterpreter(string):
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
    elif string == "EG:1":
        simple_status[2] = 1
        Power.inputQueue.put("PE:restartGPS")
    elif string == "EG:0":
        simple_status[2] = 0

    else:
        return CurrentState


def GPSInterpreter(string):
    if string == "EG:DeployLat" and CurrentState == 2:
        return 3
    elif string == "EG:1":
        simple_status[2] = 1
        Power.inputQueue.put("PE:restartGPS")
    elif string == "EG:0":
        simple_status[2] = 0
    elif string == "ED:test":
        failureStatus()
    else:
        return CurrentState

## Develop profile for power interpreter

def HealthInterpreter(string):
    return CurrentState


def PowerInterpreter(string):
    global EPSReturnState
    global MessageRecovery
    if string == "EP:lowPowerMode":
        if CurrentState == 2 or CurrentState == 3:
            if DeployState == 1:
                EPSReturnState = 1
            else:
                EPSReturnState = 4
        elif CurrentState == 5:
                pass
        else:
                EPSReturnState = CurrentState
        return 5
    elif string == "EP:SufficientPower" and CurrentState == 5:
        MessageRecovery = 1
        returnFromLPM()
        return EPSReturnState
    elif string == "EP:RF22restarted":
        # only resume RFM22B thread if we are not in detumbling mode?
        # RFM22B.resume()
        return CurrentState
    elif string == "EP:IMUrestarted":
        ADC.resume()
        return CurrentState
    elif string == "EP:SunsensorRestarted":
        # call ADC init function for sunsensor re-initialization function
        ADC.resume()
        return CurrentState
    elif string == "EP:GPSrestarted":
        GPS.resume()
        print("resuming GPS thread")
        return CurrentState
    elif string == "EP:Deployersrestarted":
        # what to do if we were in the middle of deployment when we needed to restart deployer hardware?
        return CurrentState
    elif string == "EC:1":
        simple_status[5] = 1
        Power.inputQueue.put("CE:restartPower")
    elif string == "EC:0":
        simple_status[5] = 0
    else:
        return CurrentState


def RFM22BInterpreter(string):
    if string == "ER:1":
        simple_status[3] = 1
        Power.inputQueue.put("PE:restartRF22")
    elif string == "ER:0":
        simple_status[3] = 0
    return CurrentState


def failureStatus():
    # check if all threads are healthy and create a health txt file
    Comm.inputQueue.put("CE:healthProfile")
    Power.inputQueue.put("PE:healthProfile")
    Deployer.inputQueue.put("DE:healthRequest")
    GPS.inputQueue.put("GE:healthRequest")
    RFM22B.inputQueue.put("RE:healthRequest")
    ADC.inputQueue.put("AE:healthRequest")
    ## threads send back either .txt file and/or healthy bool in the list

    ## will it need time to get response back from the threads?
    with open(HealthBeaconFile, "w") as healthFile:
        subprocess.call(["cat", "Initialize Report."], stdout=healthFile)
    # read all results into a single health beacon file
    with open(HealthBeaconFile, "a") as healthFile:
        if simple_status[0] == 1:
            subprocess.call(["cat", "ADC failure detected."], stdout=healthFile)
        elif simple_status[3] == 1:
            subprocess.call(["cat", "RFM22B failure detected."], stdout=healthFile)
        elif simple_status[1] == 1:
            subprocess.call(["cat", "GPS failure detected."], stdout=healthFile)
        elif simple_status[2] == 1:
            subprocess.call(["cat", "Deployer failure detected."], stdout=healthFile)
        elif simple_status[4] == 1:
            subprocess.call(["cat", "Communication failure detected."], stdout=healthFile)
        elif simple_status[5] == 1:
            subprocess.call(["cat", "Power failure detected."], stdout=healthFile)
        else:
            subprocess.call(["cat", "No failures detected."], stdout=healthFile)
    # append the profiles from Comm Thread and EPS Thread
    #subprocess.call(["cat", GlobalstarHealthFile], stdout=healthFile)
    #subprocess.call(["cat", EPSHealthFile], stdout=healthFile)

    # send message to globalstar
    Comm.inputQueue.put("CE:healthBeacon")

def processMessage(string):
    global MessageRecovery
    thread_char = string[0]
    subsystem_char = string[1]
    if thread_char == 'E':
        if CurrentState == 5:
            if string == "EP:SufficientPower":
                # print("registering sufficient power command")
                # messageRecovery()
                MessageRecovery = 1
                returnFromLPM()
                return EPSReturnState
            else:
                reserveQueue.put(string)
                return CurrentState
        else:
            # include error handling for if subsystem char is not one of the items in the dictionary
            next_state = interpretMessage[subsystem_char](string)
            # how/where to update deploy state?
            return next_state
    else:
        # include error handling for if the thread char is not one of the items in the dictionary
        # print("sending to thread: "+string)
        routeMessage[thread_char].put(string)
        return CurrentState

# logging setup
logging.basicConfig(stream=sys.stdout, level=logging.INFO, format="%(asctime)s.%(msecs)03d:%(name)s:%(message)s",
                    datefmt='%Y-%m-%d,%H:%M:%S')
logger = logging.getLogger("main")

# system Initialization
initialization()
# failureStatus()
# threading.Thread(1080, loop())

# create reserve queue for messages received while in low power mode
reserveQueue = queue.Queue()

routeMessage = {'A': ADC.inputQueue, 'C': Comm.inputQueue, 'D': Deployer.inputQueue, 'G': GPS.inputQueue,
                'H': Health.inputQueue, 'P': Power.inputQueue, 'R': RFM22B.inputQueue}
interpretMessage = {'A': ADCInterpreter, 'C': CommInterpreter, 'D': DeployerInterpreter, 'G': GPSInterpreter,
                    'H': HealthInterpreter, 'P': PowerInterpreter, 'R': RFM22BInterpreter}

EPSReturnState = 0
DeployState = 1
CurrentState = 0
MessageRecovery = 0

# begin threading algorithms
Comm.resume()
GPS.resume()
Health.resume()
Power.resume()
ADC.resume()
RFM22B.resume()
# when to start rfm22b radio? should not start while still deep in detumbling mode?

print("performing ADC detumbling algorithms")
# print("waiting for deploy ready command from ground")

# wd = open("/dev/watchdog", "w+")

# State Machine
while True:
    if CurrentState == 0: # ADC algorithms state
        print("waiting for deploy ready command from ground")
    if CurrentState == 1:  # ADC detumbling for deployment
        # should we keep detumbling when ground tells us we are okay?
        # should we tell ADC thread to send message when it is stable?
        # or should it constantly be sending the messages and we only begin to listen to them now?
        processMessage("AE:Detumbling")
        print("Detumbling mode to prep for node ejection")
    elif CurrentState == 2:  # waiting to deploy state and pointing
        # should we tell GPS thread to send message when it reaches the equator && right orbit count?
        # or should it constantly be sending the messages and we only begin to listen to them now?
        processMessage("GE:LatWait")
        print("successfully entered pointing mode - waiting on orbit number and GPS location")
        # wait on GPS and orbit count number to prep for deployment
    elif CurrentState == 3:  # deployment state
        print("GPS indicates ready to deploy - running single deployment algorithm")
        if startNetwork == 0:
            startNetwork = 1
            processMessage("RE:radioInit")
        Deployer.singleRun()
        # Should we wait here while the node is deploying? We should keep routing messages, but we shouldn't be able to
        # transition to a different state (unless that state is low power mode?)
    elif CurrentState == 4:  # data collection and transmission state
        processMessage("RE:ScienceMode")
        processMessage("GE:ScienceMode")
        print("Deployment state complete - Entering Science Mode")
    elif CurrentState == 5:   # low power mode
        # Be very careful with conflicts between low power mode and other modes
            # make sure that all other running processes are stopped and transitioned to low power mode before
            # hardware is turned off - also make sure that messages that should indicate a state transition that come
            # during low power mode (or on the cusp before and after) are handled correctly
            # lots of potential for edge cases that could cause a lot of problems!
        print("entering low power mode state in executive thread")
        lpm = 1
        lowPowerMode()
    tempState = None
    while tempState is None or tempState == CurrentState:
        try:
            if MessageRecovery == 1:
                threadResponse = reserveQueue.get(False)
            else:
                threadResponse = inputQueue.get(False)
            tempState = processMessage(threadResponse)
            # print(threadResponse)
        except queue.Empty:
            MessageRecovery = 0
            tempState = None
    CurrentState = tempState

logger.info("After calling terminate on threads")
sys.exit(0)