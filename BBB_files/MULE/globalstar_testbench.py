# ***********************************************
# ***************** ANDESITE ********************
# ****** GLOBALSTAR RADIO TEST PROCEDURE ********
# ***********************************************
# ************* Date Created: 06/03/16 **********
# ***********************************************
from globalstar_serial import GlobalStarSerial

print("Sending message...")
radio_stream = GlobalStarSerial()
radio_stream.poll_health(10, .1)
print("Test No. 1 Complete")