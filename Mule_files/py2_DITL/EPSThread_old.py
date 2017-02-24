from worker_thread import WorkerThread
import Queue
import time
import subprocess
from eps_driver import eps_driver
from battery_driver import battery_driver

# health status file
EPSHealthFile = "/home/debian/Maria/healthFiles/EPShealth.txt"


class EPSThread(WorkerThread):

    def __init__(self, executive_queue):
        super(EPSThread,self).__init__("EPS Thread")
        self.inputQueue = Queue.Queue()
        self.executiveQueue = executive_queue
        # Command 0x01 Health Statuses (if bit is higher then there is an error)
        self.unknType = 0  # unknown command type (byte 0, bit 0)
        self.unknValue = 0  # unknown command value (byte 0, bit 1)
        self.readyADC = 0  # ADC result ready (byte 0, bit 2)
        self.oscillator = 0  # oscillator failure bit (byte 0, bit 4)
        self.resetWDT = 0  # WDT reset occurred (byte 0, bit 5)
        self.resetPower = 0  # power on reset occurred (byte 0, bit 6)
        self.brownOut = 0  # brown out reset occurred (byte 0, bit 7)
        self.errorI2C = 0  # I2C error occurred (byte 1, bit 0)
        self.writeI2C = 0  # I2C write collision (byte 1, bit 1)
        self.overflow = 0  # I2C overflow (byte 1, bit 2)
        self.msgLength = 0  # received message too long (byte 1, bit 3)

# should have a low power mode that has two different exit protocol depending on how the function was called
    # if the function was called from ground, then low power mode should not be exited until a full battery charge
    # if low power mode was called based on the battery threshold then it should exit once the battery regains a
    # sufficient voltage

# should low power mode be a function or a state in the main loop?
    # probably a separate function so that it can be called at a less frequent interval so that the battery voltage can
    # be checked periodically, but not as frequently as it monitors the voltage when it is in active mode

    def healthReport(self):
        # fill health string based on variables representing hardware components health
        if self.errorI2C != 0:
            ## WDT needs to be reset
            healthString = "EPS requires restart"
        else:
            healthString = "All EPS components are healthy"
        with open(EPSHealthFile, "w") as healthFile:
            subprocess.call(["echo", healthString], stdout=healthFile)
        with open(EPSHealthFile, "a") as healthFile:  # save all health status parameters to the EPS health file
            subprocess.call(["echo", self.unknType], stdout=healthFile)
            subprocess.call(["echo", self.unknValue], stdout=healthFile)
            subprocess.call(["echo", self.readyADC], stdout=healthFile)
            subprocess.call(["echo", self.oscillator], stdout=healthFile)
            subprocess.call(["echo", self.resetWDT], stdout=healthFile)
            subprocess.call(["echo", self.resetPower], stdout=healthFile)
            subprocess.call(["echo", self.brownOut], stdout=healthFile)
            subprocess.call(["echo", self.errorI2C], stdout=healthFile)
            subprocess.call(["echo", self.writeI2C], stdout=healthFile)
            subprocess.call(["echo", self.overflow], stdout=healthFile)
            subprocess.call(["echo", self.msgLength], stdout=healthFile)


    def processResponse(self, string):
        if string == "PC:restartEPS":
            self.restartEPS()

    def restartEPS(self):
        pass


    def init(self):
        self.interval = .5
        self.log("Initializing thread with an interval of {0}".format(self.interval))
        # threading.Timer(25, self.sendMessage).start()
        with open(EPSHealthFile, "w") as healthFile:
            subprocess.call(["echo", "Successful health file initialization"], stdout=healthFile)

    def loop(self):
        try:
            executiveResponse = self.inputQueue.get(False)
            self.processResponse(executiveResponse)
        except Queue.Empty:
            pass
        #check EPS voltages and currents every minute if it goes low send warning ground command
            #else just log most recent one in health file
        # *****put main code here****