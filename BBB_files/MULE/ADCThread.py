from .worker_thread import WorkerThread
from .adc.adc_runner import AdcInput, ADCRunner
import queue
import threading
import subprocess

'''
include interface with new microcontroller
include ADC algorithms based on data derived from microcontroller
'''

# health status file
ADCHealthFile = "/home/debian/Maria/healthFiles/ADCHealth.txt"
ADCvector = ""
Detumbling = False

class ADCThread(WorkerThread):
    def __init__(self, executive_queue: queue.Queue):
        super(ADCThread, self).__init__("ADC Thread")
        self.inputQueue = queue.Queue()
        self.executiveQueue = executive_queue
        self.adcRunner = ADCRunner()

    def lowPowerMode(self):
        # this function should turn off all unnecessary hardware and functions for low power mode
        print("ADC thread entering low power mode")

    def healthReport(self):
        # fill health string based on variables representing hardware components health
        healthString = "All ADC components are healthy"
        with open(ADCHealthFile, "w") as healthFile:
            subprocess.call(["echo", healthString], stdout=healthFile)

    def sendMessage(self):
        # When should this detumbling message be sent? And how often?
        # We dont want to send it every iteration that is in pointing mode, because we don't want to overload the
        # executive thread with useless messages
        self.executiveQueue.put("EA:Pointing")

    def processResponse(self, messageString:str):
        global Detumbling
        if messageString == "AE:Detumbling":
            Detumbling = True
        elif messageString == "AE:lowPowerMode":
            self.lowPowerMode()
        elif "AC:ADCvector:" in messageString:
            self.changeVector(messageString[13:])
        elif messageString == "AC:burstVector":
            self.burstVector()

    def burstVector(self):
        pass

    def changeVector(self, newVector):
        global ADCvector
        ADCvector = newVector

    def init(self):
        self.interval = .01  # frequency of the most frequently sampled ADC hardware (
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        with open(ADCHealthFile, "w") as healthFile:
            subprocess.call(["echo", "Successful health file initialization"], stdout=healthFile)
            # threading.Timer(30, self.sendMessage).start()

    def loop(self):
        global Detumbling
        try:
            executiveResponse = self.inputQueue.get(False)
            self.processResponse(executiveResponse)
        except queue.Empty:
            pass
        if Detumbling:
            Detumbling = False
            threading.Timer(2, self.sendMessage).start()

    def terminate(self):
        super(ADCThread,self).terminate()
        self.adcRunner.exit()