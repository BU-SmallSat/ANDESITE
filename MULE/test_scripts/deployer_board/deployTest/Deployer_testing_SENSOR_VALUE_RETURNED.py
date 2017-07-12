import threading
# from DeploymentThread_Experimental_for_board_python2 import DeploymentThread
# from DeploymentThread_EJECTION_TEST import DeploymentThread
from DeploymentThread_SENSOR_VALUE_RETURNED import DeploymentThread

# from eps_driver_SERGEI import EPS_driver
import Queue
import time
import subprocess
import serial

# WORKS WELL WITH _2017_jun20_FULL_GROUND_CONTROL.ino

'''INITIALIZATION SECTION'''
print('INITIALIZATION SECTION')
initialized = 0
Deploying =1

EQ = Queue.Queue()
Deployer = DeploymentThread(EQ)
Deployer.resume()
while not initialized:
    try:
        response = EQ.get(False)
        if response == "ED:DeployerInitialized":
            initialized = 1
    except Queue.Empty:
        pass
    time.sleep(1)


'''SERIAL COMMUNICATION TEST SECTION'''
print('SERIAL COMMUNICATION TEST SECTION')
Deployer.inputQueue.put("DE:CheckSerial")
waiting =1
while waiting:
    try:
        response = EQ.get(False)
        if response == "ED:SerialConnected":
            waiting = 0
            print('YES YES YES WE HAVE TESTED THE CONNECTION')
        elif response == "ED:SerialDisconnected":
            waiting =0
            print('NO NO NO THE CONNCETION HAS FAILED')
    except Queue.Empty:
        pass
time.sleep(1)


# print("CHECK THE IR SENSOR #1")
# Deployer.inputQueue.put("DE:IR_Sensor1")
# waiting =1
# while waiting:
#     try:
#         response = EQ.get(False)
#         if len(response) >= 12 and response[:12] == "ED:IR_Sensor":
#             print(response)
#             waiting = 0
#     except Queue.Empty:
#         pass
# time.sleep(10)


'''SETTING THE POWER TIME AND THE DUTY CYCLE'''
Deployer.inputQueue.put("DE:setPowerTime8")
time.sleep(1)
Deployer.inputQueue.put("DE:setDutyCycle10")
time.sleep(1)
print('DUTY CYCLE = ' + str(Deployer.DutyCycle))
print('POWER TIME = ' + str(Deployer.PowerTime))
print('WE HAVE JUST SET THE POWER TIME AND THE DUTY CYCLE')




'''DEPLOYMENT SECTION'''
print('DEPLOYMENT SECTION')

print('DEPLOYING NODE1')
Deployer.inputQueue.put("DE:Deploy1")
waiting =1
while waiting:
    try:
        response = EQ.get(False)
        if response == 'ED:Deploy1Success':
            waiting =0
        waiting = 0
    except Queue.Empty:
        pass
time.sleep(1)

print('DEPLOYING NODE2')
Deployer.inputQueue.put("DE:Deploy2")
waiting =1
while waiting:
    try:
        response = EQ.get(False)
        if response == 'ED:Deploy1Success':
            waiting =0
        waiting = 0
    except Queue.Empty:
        pass
time.sleep(1)

print('DEPLOYING NODE3')
Deployer.inputQueue.put("DE:Deploy3")
waiting =1
while waiting:
    try:
        response = EQ.get(False)
        if response == 'ED:Deploy1Success':
            waiting =0
        waiting = 0
    except Queue.Empty:
        pass
time.sleep(1)

print('DEPLOYING NODE4')
Deployer.inputQueue.put("DE:Deploy4")
waiting =1
while waiting:
    try:
        response = EQ.get(False)
        if response == 'ED:Deploy1Success':
            waiting =0
        waiting = 0
    except Queue.Empty:
        pass
time.sleep(1)
print("*********Deployed All Nodes!***********")
