import threading
from DeploymentThread_for_board_python2 import DeploymentThread
from eps_driver_SERGEI import EPS_driver
import Queue
import time
import subprocess
import serial


Deploying =1

EQ = Queue.Queue()
Deployer = DeploymentThread(EQ)

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
print("*********Deployed All Nodes!***********")