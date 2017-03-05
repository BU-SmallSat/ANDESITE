from __future__ import print_function

import subprocess

from eps_driver import EPS_driver
#from BatteryDriver import BatteryDriver

# health status file
EPSHealthFile = "/home/debian/Brent/healthFiles/EPShealth.txt"

eps = EPS_driver()

def EPShealthupdate():

    with open(EPSHealthFile, "w") as healthFile:
        subprocess.call(["echo", 5*"-"+"EPS Health Status"+5*"-"], stdout=healthFile)
        healthFile.close()

    BatV = 0.
    BatPow = eps.Pow_12v()
    EPS33P = eps.Pow_3v3()
    EPS5P =  eps.Pow_5v()
    EPS12P = eps.Pow_12v()
    Sol1P = eps.BCR_current(1)*eps.BCR_voltage(1) #1
    Sol2P = eps.BCR_current(2)*eps.BCR_voltage(2) #2
    Sol3P = eps.BCR_current(4)*eps.BCR_voltage(4) #4
    Sol4P = eps.BCR_current(5)*eps.BCR_voltage(5) #5
    Sol5P = eps.BCR_current(6)*eps.BCR_voltage(6) #6

    with open(EPSHealthFile, "a") as healthFile:
            subprocess.call(["echo", "Battery Voltage:    ", "{:.4f}".format(BatV)  ], stdout=healthFile)
            subprocess.call(["echo", "Battery Power:      ", "{:.4f}".format(BatPow)], stdout=healthFile)
            subprocess.call(["echo", "EPS 3.3V Bus Power: ", "{:.4f}".format(EPS33P)], stdout=healthFile)
            subprocess.call(["echo", "EPS   5V Bus Power: ", "{:.4f}".format(EPS5P) ], stdout=healthFile)
            subprocess.call(["echo", "EPS  12V Bus Power: ", "{:.4f}".format(EPS12P)], stdout=healthFile)
            subprocess.call(["echo", "Solar Cell 1 Power: ", "{:.4f}".format(Sol1P) ], stdout=healthFile)
            subprocess.call(["echo", "Solar Cell 2 Power: ", "{:.4f}".format(Sol2P) ], stdout=healthFile)
            subprocess.call(["echo", "Solar Cell 3 Power: ", "{:.4f}".format(Sol3P) ], stdout=healthFile)
            subprocess.call(["echo", "Solar Cell 4 Power: ", "{:.4f}".format(Sol4P) ], stdout=healthFile)
            subprocess.call(["echo", "Solar Cell 5 Power: ", "{:.4f}".format(Sol5P) ], stdout=healthFile)
            healthFile.close()

