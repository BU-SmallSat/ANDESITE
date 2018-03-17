from Adafruit_I2C import Adafruit_I2C
import time

'''
Throw error returns if the response from EPS is 0xF000
this means that the reading information is not yet ready to be read
or that the read was not preceeded by a write
'''


class battery_driver:
    def __init__(self):
        self.address = 0x20
        self.batt = 0x2A
        self.i2c_bus = Adafruit_I2C(self.address,busnum=1,debug=False) #second argument tells bus number
        self.debug = 1
        self.read_wait = 0.2   # Longest delay is 200ms

    def send_command(self, byteList, recvLength):
        self.i2c_bus.writeList(self.batt, byteList)
        time.sleep(self.read_wait) #Look up in datasheet
        reply = self.i2c_bus.readList(self.batt, recvLength)
        if reply[0] == 0xFF and reply[1] == 0xFF:
            if self.debug == 1:
                print("ERROR EMPTY RESPONSE FROM EPS")
        return reply

    def Batt_Voltage(self):  # input BCR number 1-9
        rsp = self.send_command([0x10,0xE2,0x80], 2)
        Num = (rsp[0]*16*16) | rsp[1]
        result = 0.008993 * Num
        if self.debug == 1:
            print('Battery Output Voltage: ' + str(result))
        return result

    def Batt_Current(self):  # input BCR number 1-9
        rsp = self.send_command([0x10,0xE2,0x84], 2)
        Num = (rsp[0]*16*16) | rsp[1]
        result = 14.662757 * Num
        if self.debug == 1:
            print('Battery Output Current: ' + str(result))
        return result

    def Batt_Charging(self):  # input BCR number 1-9
        rsp = self.send_command([0x10,0xE2,0x8E], 2)
        Num = (rsp[0]*16*16) | rsp[1]
        if Num < 512:
            charging = 1
            if self.debug == 1:
                print('Battery is charging')
        else:
            charging = 0
            if self.debug == 1:
                print('Battery is discharging')
        return charging

    def Current_5v(self):  # input BCR number 1-9
        rsp = self.send_command([0x10,0xE2,0x14], 2)
        Num = (rsp[0]*16*16) | rsp[1]
        result = 1.327547 * Num
        if self.debug == 1:
            print('5v Bus Current draw: ' + str(result))
        return result

    def Voltage_5v(self):  # input BCR number 1-9
        rsp = self.send_command([0x10, 0xE2, 0x10], 2)
        Num = (rsp[0]*16*16) | rsp[1]
        result = .005865 * Num
        if self.debug == 1:
            print('5v Bus Voltage: ' + str(result))
        return result

    def Current_3v3(self):  # input BCR number 1-9
        rsp = self.send_command([0x10,0xE2,0x04], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 1.327547 * Num
        if self.debug == 1:
            print('3v3 Bus Current draw: ' + str(result))
        return result

    def Voltage_3v3(self):  # input BCR number 1-9
        rsp = self.send_command([0x10,0xE2,0x00], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = .004311 * Num
        if self.debug == 1:
            print('3v3 Bus Voltage: ' + str(result))
        return result

    def Motherboard_Temp(self):  # input BCR number 1-9
        rsp = self.send_command([0x10,0xE2,0x08], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.372434 * Num - 273.15
        if self.debug == 1:
            print('Motherboard Temperature: ' + str(result))
        return result

    def Daughterboard_Temp(self, board_num): # input BCR number 1-9
        if board_num > 0 and board_num < 5:
            cmd = (board_num * 16)+0x88
            rsp = self.send_command([0x10,0xE3,cmd], 2)
            Num = (rsp[0]*16*16) | rsp[1]
            result = .3976*Num - 238.57
            if self.debug == 1:
                print("Daughterboard #" + str(board_num) + " temperature: " + str(result))
            return result
        else:
            return -1

    def Daughterboard_Heater(self, board_num): # input BCR number 1-9
        if board_num > 0 and board_num < 5:
            cmd = (board_num * 16)+0x8F
            rsp = self.send_command([0x10,0xE3,cmd], 2)
            Num = (rsp[0]*16*16) | rsp[1]
            if Num < 512:
                Heater = 0
                if self.debug == 1:
                    print("Daughterboard #" + str(board_num) +" Heater is off")
            else:
                Heater = 1
                if self.debug == 1:
                    print("Daughterboard #" + str(board_num) +" Heater is on")
            return Heater
        else:
            return -1