
from DeploymentThread_May31 import DeploymentThread
import Queue
import time

initialized = 0
Deploying =1

EQ = Queue.Queue()
Deployer = DeploymentThread(EQ)
while not initialized:
    try:
        response = EQ.get(False)
        if response == "ED:DeployerInitialized":
            initialized = 1
    except Queue.Empty:
        pass
    time.sleep(1)
while Deploying:
    Deployer.inputQueue.put("DE:EjectNode")
    waiting =1
    while waiting:
        try:
            response = EQ.get(False)
            if response == "ED:DeploymentComplete":
                Deploying = 0
            elif response == "ED:NextDeployment":
                waiting =0
        except Queue.Empty:
            pass
    time.sleep(1)
print("*********Deployed All Nodes!***********")