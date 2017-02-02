import logging
import Queue
import subprocess
import sys
from threading import Timer
from DeploymentThread import DeploymentThread
from EPSThread import EPSThread
from ADCThread import ADCThread
from HealthThread import HealthThread
from GlobalstarThread import GlobalstarThread
from RFM22BThread import RFM22BThread
from GPSThread import GPSThread
import threading




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

# IMPORTANT:: need a method to start the radio on the Mule if the node ejected was the first one
startNetwork = 0
HealthFreq = 5
ADCHealth = 0
EPSHealth = 0
DeploymentHealth = 0
globalstarHealth = 0
gpsHealth = 0
RFM22BHealth = 0

# setup threads and thread communication
inputQueue = Queue.Queue()   # E
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
    # specifics need to be identified
    print("*** BEGINING ANDESITE FLIGHT COMPUTER OPERATIONS***")
    # watchdog = Watchdog()
    ## working with BeagleBone
    threading.Timer(8, Comm.GlobalstarEnable).start()
    threading.Timer(HealthFreq, Health.healthBeacon, [ADCHealth,EPSHealth,DeploymentHealth,globalstarHealth,gpsHealth,RFM22BHealth]).start()

    #  Test initializing the health file
    with open(HealthBeaconFile, "w") as healthFile:
        subprocess.call(["echo", "Successful health file initialization"], stdout=healthFile)

def loop():
    pass

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
        threading.Timer(HealthFreq, Health.healthBeacon, [ADCHealth, EPSHealth, DeploymentHealth, globalstarHealth, gpsHealth, RFM22BHealth]).start()
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

# logging setup
logging.basicConfig(stream=sys.stdout, level=logging.INFO, format="%(asctime)s.%(msecs)03d:%(name)s:%(message)s",
                    datefmt='%Y-%m-%d,%H:%M:%S')
logger = logging.getLogger("main")

# system Initialization
initialization()
# failureStatus()
# threading.Thread(1080, loop())

# create reserve queue for messages received while in low power mode
reserveQueue = Queue.Queue()

routeMessage = {'A': ADC.inputQueue, 'C': Comm.inputQueue, 'D': Deployer.inputQueue, 'G': GPS.inputQueue,
                'H': Health.inputQueue, 'P': Power.inputQueue, 'R': RFM22B.inputQueue}
interpretMessage = {'A': ADCInterpreter, 'C': CommInterpreter, 'D': DeployerInterpreter, 'G': GPSInterpreter,
                    'H': HealthInterpreter, 'P': PowerInterpreter, 'R': RFM22BInterpreter}

EPSReturnState = 0
CurrentState = 1
MessageRecovery = 0
PointingState = 0
DeployState = 0

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
    if CurrentState == 1: # ADC algorithms state
        processMessage("AE:Detumbling")
        print("waiting for pointing mode and deploy ready command from ground")
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
    elif CurrentState == 5:   # safe mode
        # Be very careful with conflicts between low power mode and other modes
            # make sure that all other running processes are stopped and transitioned to low power mode before
            # hardware is turned off - also make sure that messages that should indicate a state transition that come
            # during low power mode (or on the cusp before and after) are handled correctly
            # lots of potential for edge cases that could cause a lot of problems!
        Comm.GlobalstarDisable()
        print("entering safe mode state in executive thread")
    tempState = None
    while tempState is None or tempState == CurrentState:
        try:
            if MessageRecovery == 1:
                threadResponse = reserveQueue.get(False)
            else:
                threadResponse = inputQueue.get(False)
            tempState = processMessage(threadResponse)
            # print(threadResponse)
        except Queue.Empty:
            MessageRecovery = 0
            tempState = None
    CurrentState = tempState

logger.info("After calling terminate on threads")
sys.exit(0)
