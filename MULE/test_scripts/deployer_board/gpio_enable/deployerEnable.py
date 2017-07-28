import Adafruit_BBIO.GPIO as GPIO
import time

print("setting pin high")
GPIO.setup("P9_15", GPIO.OUT)
GPIO.output("P9_15", GPIO.HIGH)
time.sleep(20)
print("setting pin low")
GPIO.output("P9_15", GPIO.LOW)
time.sleep(10)
print("done with script")