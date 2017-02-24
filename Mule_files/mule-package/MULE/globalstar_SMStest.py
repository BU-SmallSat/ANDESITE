# ***********************************************
# ***************** ANDESITE ********************
# ****** GLOBALSTAR RADIO TEST PROCEDURE ********
# ***********************************************
# ************* Date Created: 06/03/16 **********
# ***********************************************
import time
from globalstar_serial import GlobalStarSerial
import Adafruit_BBIO.GPIO as GPIO
import datetime


GPIO.setup("P9_15", GPIO.OUT)
GPIO.output("P9_15", GPIO.HIGH)
time.sleep(2)

print("Sending message...")
radio_stream = GlobalStarSerial()


radio_stream.poll_health(10, .1)
time.sleep(20)
radio_stream.SMS_pull_poll(10, .1)
time.sleep(20)
print("Test No. 1 Complete")
GPIO.cleanup()
