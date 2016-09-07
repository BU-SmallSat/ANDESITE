import logging
import Queue
import sys
import time
from DeploymentThread_forTest import DeploymentThread


def DeployerInterpreter(string):
    if string == "ED:DeploymentComplete":
        print("****SUCCESS: Deployment Test Completed****")
    elif string == "ED:NextDeployment":
        Deployer.singleRun()
    elif string == "ED:AttemptsExceeded":
        print("****ERROR: Limit reached on deployment attempts****")
    elif string == "PE:restartDeployers":
        print("****ERROR: serial bus connection failed - check deployment board****")
    else:
        print("ERROR: Improper message from the Deployment Thread")

# logging setup
logging.basicConfig(stream=sys.stdout, level=logging.INFO, format="%(asctime)s.%(msecs)03d:%(name)s:%(message)s",
                    datefmt='%Y-%m-%d,%H:%M:%S')
logger = logging.getLogger("main")


# setup threads and thread communication
inputQueue = Queue.Queue()   # E
Deployer = DeploymentThread(inputQueue)  # D
time.sleep(10)
try:
    print("ready to start ejections")
    Deployer.singleRun()

    while True:
        try:
            threadResponse = inputQueue.get(False)
            thread_char = threadResponse[0]
            subsystem_char = threadResponse[1]
            DeployerInterpreter(threadResponse)
        except Queue.Empty:
            pass

    logger.info("After calling terminate on threads")
    sys.exit(0)

except KeyboardInterrupt:
    print("****KEYBOARD INTERRUPT: Quitting process****")
    Deployer.terminate()
    sys.exit(0)

