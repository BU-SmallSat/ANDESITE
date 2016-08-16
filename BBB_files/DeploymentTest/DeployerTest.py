import logging
import Queue
import sys
from DeploymentThread import DeploymentThread


def DeployerInterpreter(string):
    if string == "ED:DeploymentComplete":
        print("****SUCCESS: Deployment Test Completed****")
    elif string == "ED:NextDeployment":
        Deployer.singleRun()
    elif string == "ED:AttemptsExceeded":
        print("****ERROR: Limit reached on deployment attempts****")
    else:
        print("ERROR: Improper message from the Deployment Thread")

# logging setup
logging.basicConfig(stream=sys.stdout, level=logging.INFO, format="%(asctime)s.%(msecs)03d:%(name)s:%(message)s",
                    datefmt='%Y-%m-%d,%H:%M:%S')
logger = logging.getLogger("main")


# setup threads and thread communication
inputQueue = Queue.Queue()   # E
Deployer = DeploymentThread(inputQueue)  # D

Deployer.singleRun()

while True:
        try:
            threadResponse = inputQueue.get(False)
            thread_char = threadResponse[0]
            subsystem_char = threadResponse[1]
            if thread_char == 'E' and subsystem_char == 'D':
                DeployerInterpreter(threadResponse)
            else:
                print("ERROR: Improper message from the Deployment Thread")
        except Queue.Empty:
            pass

logger.info("After calling terminate on threads")
sys.exit(0)
