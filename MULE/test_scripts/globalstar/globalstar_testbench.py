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

#move GPIO enable to the globalstar_serial initialization code
print "Starting test - initializing globalstar"
start_time = time.time()

GPIO.setup("P9_15", GPIO.OUT)
GPIO.output("P9_15", GPIO.HIGH)
time.sleep(2)

print("Sending message...")
radio_stream = GlobalStarSerial()

#radio_stream.poll_health()
#radio_stream.poll_health(2, .1)
#time.sleep(20)
#radio_stream.SMS_pull_poll(5, .2)
#time.sleep(20)
elapsed_time = time.time() - start_time
timer = str(datetime.timedelta(seconds=int(elapsed_time)))
print "Time since initialization of globalstar: ",
print timer

# radio_stream.clear_downlink_files(2,.1)
# radio_stream.poll_health(2, .1)
# time.sleep(2)
# radio_stream.poll_files_awaiting_upload(2,.1)
# time.sleep(2)
# radio_stream.poll_files_awaiting_download(2,.1)
# radio_stream.sendFile("OsiMessage.txt",30,.1)
# time.sleep(5)
# radio_stream.sendFile("Hello.txt",30,.1)
# time.sleep(5)

returnNum = 0
while(1):
    radio_stream.poll_health(2, .1)
    time.sleep(2)

    #radio_stream.poll_messages_awaiting_upload(2,.1)
    #time.sleep(2)
    #radio_stream.poll_files_awaiting_upload(2,.1)
    #time.sleep(20)
    #radio_stream.poll_files_awaiting_download(2,.1)
    #time.sleep(20)
    #radio_stream.clear_downlink_files(2,.1)
    #time.sleep(20)
    #radio_stream.clear_uplink_files(2,.1)
    #time.sleep(20)
    #radio_stream.clear_uplink_messages(2,.1)
    #time.sleep(20)

    #radio_stream.SMS_pull_poll(2,.1)
    #time.sleep(5)

    # radio_stream.push_file_poll(1,.1)
    #time.sleep(20)

    #radio_stream.sendFile("Hello.txt",2,.1)
    #time.sleep(5)

    radio_stream.poll_files_awaiting_download(2,.1)
    # time.sleep(20)

    elapsed_time = time.time()-start_time
    timer = str(datetime.timedelta(seconds = int(elapsed_time)))
    print "Time since initialization of globalstar: ",
    print timer
    print(80*'-')

print("Test No. 2 Complete")
# GPIO.cleanup()
