import logging
import Queue
import sys
import time
from threading import BoundedSemaphore
from RFM22BThread import RFM22BThread

'''*******************GLOBAL SYSTEM DEFINES************************'''
WSNcontact = False
'''****************************************************************'''

# setup threads and thread communication
inputQueue = Queue.Queue()  # E

ListSemaphore = BoundedSemaphore(1)
DownlinkSemaphore = BoundedSemaphore(1)
RFM22B = RFM22BThread(inputQueue, ListSemaphore, DownlinkSemaphore)  # R

# logging setup
logging.basicConfig(stream=sys.stdout, level=logging.INFO, format="%(asctime)s.%(msecs)03d:%(name)s:%(message)s",
                    datefmt='%Y-%m-%d,%H:%M:%S')
logger = logging.getLogger("main")

RFM22B.resume()

RFM22B.inputQueue.put("RC:radioInit")


