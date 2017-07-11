from battery_driver import battery_driver
import time


BAT = battery_driver()
print("\n\n---*** BEGINNING BATTERY STATUS TEST ***---")
while(1):
    ##########################
    print("\nBATTERY STATUS INFO")
    BAT.Batt_Voltage()
    time.sleep(.5)
    BAT.Batt_Current()
    time.sleep(.5)
    BAT.Batt_Charging()
    time.sleep(.5)

    ##########################
    print("\nBATTERY TEMPERATURE INFO")
    print("***MOTHERBOARD***")
    BAT.Motherboard_Temp()
    time.sleep(.5)
    print("***DAUGHTERBOARDS***")
    BAT.Daughterboard_Temp(1)
    time.sleep(.5)
    BAT.Daughterboard_Temp(2)
    time.sleep(.5)
    BAT.Daughterboard_Temp(3)
    time.sleep(.5)
    BAT.Daughterboard_Temp(4)
    time.sleep(.5)

    ##########################
    print("\nBATTERY HEATER INFO")
    BAT.Daughterboard_Heater(1)
    time.sleep(.5)
    BAT.Daughterboard_Heater(2)
    time.sleep(.5)
    BAT.Daughterboard_Heater(3)
    time.sleep(.5)
    BAT.Daughterboard_Heater(4)
    time.sleep(.5)

    ##########################
    print("\nVOLTAGE BUS HEALTH INFO")
    print("***3.3 VOLT BUS***")
    BAT.Current_3v3()
    time.sleep(.5)
    BAT.Voltage_3v3()
    time.sleep(.5)
    print("***5 VOLT BUS***")
    BAT.Current_5v()
    time.sleep(.5)
    BAT.Voltage_5v()

    print("\n\n---*** END EPS STATUS TEST ***---")
    print("***WILL REPEAT INDEFINITELY - CTRL C TO EXIT***")
    time.sleep(60)





