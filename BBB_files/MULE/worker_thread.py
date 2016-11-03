import logging
import time
from threading import Thread, BoundedSemaphore


class WorkerThread(Thread):
    def __init__(self, name):
        Thread.__init__(self)
        self.__pauseSemaphore = BoundedSemaphore(1)
        self.__logger = logging.getLogger(name)
        self.__is_terminating = False
        self.__pauseSemaphore.acquire(blocking=False)
        self.interval = 0.0
        self.name = name
        self.start()

    def run(self):
        self.init()
        while True:
            # if interval is greater than zero, sleep for that number of seconds before attempting
            # to continue the WorkerThread
            if self.interval > 0.0:
                time.sleep(self.interval)
            # Acquire and release __pauseSemaphore before calling loop
            # This will block the thread if pause() has been called
            self.__pauseSemaphore.acquire()
            self.__pauseSemaphore.release()
            if self.__is_terminating:
                self.log("Terminating")
                return
            self.loop()

    def pause(self):
        self.log("pausing")
        self.__pauseSemaphore.acquire(blocking=False)

    def resume(self):
        self.log("resuming")
        try:
            self.__pauseSemaphore.release()
        except ValueError:
            pass

    def singleRun(self):
        # self.log("run single iteration")
        self.__pauseSemaphore.acquire(blocking=False)
        self.loop()

    def terminate(self):
        self.log("terminate() called")
        self.__is_terminating = True
        self.resume()
        # self._stop()

    def log(self, message):
        self.__logger.info(message)

    def init(self):
        pass

    def loop(self):
        pass
