import logging
import Queue
import sys
import time
from threading import BoundedSemaphore
from DeploymentThread import DeploymentThread
from EPSThread_functional import EPSThread
from ADCThread import ADCThread
from GlobalstarThread import GlobalstarThread
from RFM22BThread import RFM22BThread

def FunctionalTest():
    #should we actually turn on the switches and test communication

    print("*** BEGINING ANDESITE FLIGHT COMPUTER FUNCTIONAL TEST ***")
    Power.inputQueue.put("PE:WatchdogReset90")
    response = waitForResp(.5)
    if response == "EP:WatchdogResetFailure":
        print("------- PROBLEM OCCURED WITH EPS WATCHDOG RESET EXECUTION ----------")

    elif response == "EP:lowPowerMode":
         lowPowerMode()
    else:
        print("************ SUCCESSFUL EPS WATCHDOG RESET EXECUTION ***************")

    Power.inputQueue.put("PE:EPSHealth")
    response = waitForResp(.5)
    if response == "EP:healthTestSuccess":
        print("************ SUCCESSFUL EPS HEALTH TEST EXECUTION ***************")
    elif response == "EP:lowPowerMode":
        lowPowerMode()
    else:
        print("------- PROBLEM OCCURED WITH EPS HEALTH TEST EXECUTION ----------")
    Comm.inputQueue.put("CE:enableGlobalstar")
    print("************ ENABLED GLOBALSTAR RADIO ***************")

    Power.inputQueue.put("PE:EPSswitchesStatus")
    response = waitForResp(.5)
    if response == "EP:SwitchStateSuccess":
        print("************ SUCCESSFUL EPS SWITCHERS STATUS EXECUTION ***************")
    elif response == "EP:lowPowerMode":
        lowPowerMode()
    else:
        print("------- PROBLEM OCCURED WITH EPS SWITCHERS STATUS EXECUTION ----------")
    '''
    Power.inputQueue.put("PE:switchOnDeployer")
    response = waitForResp(.5)
    if response == "EP:DeployerOn":
        print("************ SUCCESSFUL EPS DEPLOYER SWITCHER ENABLE EXECUTION ***************")
    elif response == "EP:lowPowerMode":
        lowPowerMode()
    else:
        print("------- PROBLEM OCCURED WITH EPS DEPLOYER SWITCHER ENABLE EXECUTION ----------")

    Power.inputQueue.put("PE:EPSswitchesStatus")
    response = waitForResp(.5)
    if response == "EP:SwitchStateSuccess":
        print("************ SUCCESSFUL EPS SWITCHERS STATUS EXECUTION ***************")
    elif response == "EP:lowPowerMode":
        lowPowerMode()
    else:
        print("------- PROBLEM OCCURED WITH EPS SWITCHERS STATUS EXECUTION ----------")
    '''
    ##NEED ENABLES FOR THE EPS SWITCHES: gps, gps antenna, deployer board comm and vbatt, globalstar
    Power.inputQueue.put("PE:EPSHealth")
    response = waitForResp(.5)
    if response == "EP:healthTestSuccess":
        print("************ SUCCESSFUL EPS HEALTH TEST EXECUTION ***************")
    elif response == "EP:lowPowerMode":
        lowPowerMode()
    else:
        print("------- PROBLEM OCCURED WITH EPS HEALTH TEST EXECUTION ----------")

    ADC.inputQueue.put("AE:readMagData")
    response = waitForResp(.5)
    if response == "EA:magReadSuccess":
        print("************ SUCCESSFUL MAG SELF TEST EXECUTION ***************")
    elif response == "EP:lowPowerMode":
        lowPowerMode()
    else:
        print("------- PROBLEM OCCURED WITH MAG SELF TEST EXECUTION ----------")
    '''
    Deployer.inputQueue.put("DE:CheckSerial")
    response = waitForResp(.5)
    if response == "ED:SerialConnected":
        print("************ SUCCESSFUL DEPLOYER BOARD CONNECTION ***************")
    elif response == "EP:lowPowerMode":
        lowPowerMode()
    else:
        print("------- PROBLEM OCCURED WITH DEPLOYER BOARD CONNECTION ----------")
    '''
    Comm.inputQueue.put("CE:globalStarHealth")
    response = waitForResp(.5)
    if response == "EC:gStarFail":
        print("------- PROBLEM OCCURED WITH GLOBALSTAR HEALTH TEST EXECUTION ----------")
    elif response == "EP:lowPowerMode":
        lowPowerMode()
    else:
        print("************ SUCCESSFUL GLOBALSTAR HEALTH TEST EXECUTION ***************")
    '''
    Power.inputQueue.put("PE:switchOffDeployer")
    response = waitForResp(.5)
    if response == "EP:DeployerOff":
        print("************ SUCCESSFUL EPS DEPLOYER SWITCHER DISABLE EXECUTION ***************")
    elif response == "EP:lowPowerMode":
        lowPowerMode()
    else:
        print("------- PROBLEM OCCURED WITH EPS DEPLOYER SWITCHER DISABLE EXECUTION ----------")
    '''
    Power.inputQueue.put("PE:EPSswitchesStatus")
    response = waitForResp(.5)
    if response == "EP:SwitchStateSuccess":
        print("************ SUCCESSFUL EPS SWITCHERS STATUS EXECUTION ***************")
    elif response == "EP:lowPowerMode":
        lowPowerMode()
    else:
        print("------- PROBLEM OCCURED WITH EPS SWITCHERS STATUS EXECUTION ----------")

    ##NEED ENABLES FOR THE EPS SWITCHES: gps, gps antenna, deployer board comm and vbatt, globalstar
    Power.inputQueue.put("PE:EPSHealth")
    response = waitForResp(.5)
    if response == "EP:healthTestSuccess":
        print("************ SUCCESSFUL EPS HEALTH TEST EXECUTION ***************")
    elif response == "EP:lowPowerMode":
        lowPowerMode()
    else:
        print("------- PROBLEM OCCURED WITH EPS HEALTH TEST EXECUTION ----------")
    print("\n\n\t\t~~~*** END EPS STATUS TEST ***~~~")


def waitForResp(timeout):
    while True:
        try:
            threadResponse = inputQueue.get(False)
            print(threadResponse)
            # response = processMessage(threadResponse)
            return threadResponse
        except Queue.Empty:
            pass
        time.sleep(timeout)


def lowPowerMode():
    # tell EPS thread that we are ready for low power mode
    print("***** EPS reported voltage levels requiring LOW POWER MODE ******")
    print("Charge EPS before resuming tests")
    # wait for response from EPS thread saying that low power mode is over
    sys.exit(0)
    # turn back on all threads


start_time = time.time()

EPSSemaphore = BoundedSemaphore(1)
ADCSemaphore = BoundedSemaphore(1)
ListSemaphore = BoundedSemaphore(1)
DownlinkSemaphore = BoundedSemaphore(1)

EPSSemaphore.acquire(blocking=False)
ADCSemaphore.acquire(blocking=False)

# setup threads and thread communication
inputQueue = Queue.Queue()  # E
Deployer = DeploymentThread(inputQueue)  # D
Power = EPSThread(inputQueue, EPSSemaphore)  # P
Comm = GlobalstarThread(inputQueue, ListSemaphore, DownlinkSemaphore)  # C
RFM22B = RFM22BThread(inputQueue, ListSemaphore, DownlinkSemaphore)  # R
ADC = ADCThread(inputQueue, ADCSemaphore)  # A

# logging setup
logging.basicConfig(stream=sys.stdout, level=logging.INFO, format="%(asctime)s.%(msecs)03d:%(name)s:%(message)s",
                    datefmt='%Y-%m-%d,%H:%M:%S')
logger = logging.getLogger("main")

routeMessage = {'A': ADC.inputQueue, 'C': Comm.inputQueue, 'D': Deployer.inputQueue,
                'P': Power.inputQueue, 'R': RFM22B.inputQueue}

ADC.resume()
Comm.resume()
Deployer.resume()
Power.resume()

FunctionalTest()


logger.info("After calling terminate on threads")
sys.exit(0)
