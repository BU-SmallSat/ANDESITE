from magnetorquer_driver import magnetorquer_driver
import time


mag = magnetorquer_driver()
time.sleep(2)

print("\n\n---*** BEGINNING iMTQ STATUS TEST ***---")

print("\nSTARTING MTM MEASUREMENTS")
# print("~~~ CALLING START MTM MEASUREMENTS FUNCTION ~~~")
mag.start_mtm()
time.sleep(2)
print("\nMTM STATUS INFO")
print("***Raw MTM***")
mag.get_raw_mtm()
time.sleep(1)
print("***Calibrated MTM***")
mag.get_cal_mtm()
print("***CHECKING STATUS***")
mag.system_state()

print("***RUNNING TWO ORBIT DETUMBLE***")
mag.TwoOrbitBDOT()
time.sleep(.5)
while 1:

    print("\nSTARTING MTM MEASUREMENTS")
    #print("~~~ CALLING START MTM MEASUREMENTS FUNCTION ~~~")
    mag.start_mtm()
    time.sleep(2)
    print("\nMTM STATUS INFO")
    print("***Raw MTM***")
    mag.get_raw_mtm()
    time.sleep(1)
    print("***Calibrated MTM***")
    mag.get_cal_mtm()
    print("***CHECKING STATUS***")
    mag.system_state()
    print("\n\n---*** END EPS STATUS TEST ***---")
    print("***WILL REPEAT INDEFINITELY - CTRL C TO EXIT***")
    time.sleep(1)

