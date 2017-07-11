from eps_driver import EPS_driver
import time
debug = 1
EPS = EPS_driver(debug)
print("\n\n---*** BEGINNING EPS STATUS TEST ***---")
time.sleep(1)
wait_time = .2
while(1):
    ##########################
    print("\nVOLTAGE BUSES HEALTH INFO")
    '''
    print("***12 VOLT BUS***")
    EPS.Pow_12v()
    time.sleep(wait_time)
    print("***5 VOLT BUS***")
    EPS.Pow_5v()
    time.sleep(wait_time)
    print("***3.3 VOLT BUS***")
    EPS.Pow_3v3()
    time.sleep(wait_time)
    '''
    print("***RAW BATT BUS***")
    EPS.Pow_Batt()
    time.sleep(wait_time)
    '''
    print("***BUS CURRENT DRAW***")
    EPS.currentDraw_5v()
    time.sleep(wait_time)
    EPS.currentDraw_3v3()
    time.sleep(wait_time)
    '''
    '''
    #########################
    print("\nBCR HEALTH INFO")
    print("***BCR 1***")
    EPS.BCR_voltage(1)  # 1
    time.sleep(wait_time)
    EPS.BCR_currentA(1)
    time.sleep(wait_time)
    EPS.BCR_currentB(1)
    time.sleep(wait_time)
    EPS.BCR_tempA(1)
    time.sleep(wait_time)
    EPS.BCR_tempB(1)
    time.sleep(wait_time)
    EPS.BCR_SunA(1)
    time.sleep(wait_time)
    EPS.BCR_SunB(1)
    time.sleep(wait_time)
    print("***BCR 2***")
    EPS.BCR_voltage(2)  # 2
    time.sleep(wait_time)
    EPS.BCR_currentA(2)
    time.sleep(wait_time)
    EPS.BCR_currentB(2)
    time.sleep(wait_time)
    EPS.BCR_tempA(2)
    time.sleep(wait_time)
    EPS.BCR_tempB(2)
    time.sleep(wait_time)
    EPS.BCR_SunA(2)
    time.sleep(wait_time)
    EPS.BCR_SunB(2)
    time.sleep(wait_time)
    '''
    print("***BCR 3***")
    EPS.BCR3_voltage()  # 3
    time.sleep(wait_time)
    EPS.BCR_currentA(3)
    time.sleep(wait_time)
    EPS.BCR_currentB(3)
    time.sleep(wait_time)
    '''
    EPS.BCR_tempA(3)
    time.sleep(wait_time)
    EPS.BCR_tempB(3)
    time.sleep(wait_time)
    EPS.BCR_SunA(3)
    time.sleep(wait_time)
    EPS.BCR_SunB(3)
    time.sleep(wait_time)
    '''
    '''
    #####################
    print("\nGENERAL BOARD INFO")
    EPS.daughterBoard_temp()
    time.sleep(wait_time)
    EPS.motherBoard_temp()
    time.sleep(wait_time)
    '''
    print("\n\n---*** END EPS STATUS TEST ***---")
    print("***WILL REPEAT INDEFINITELY - CTRL C TO EXIT***")


    time.sleep(60)

