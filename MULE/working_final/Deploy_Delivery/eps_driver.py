from Adafruit_I2C import Adafruit_I2C
import time

'''
Throw error returns if the response from EPS is 0xF000
this means that the reading information is not yet ready to be read
or that the read was not preceeded by a write
'''


class EPS_driver:
    def __init__(self,debug):
        self.address = 0x20
        self.eps = 0x2B
        self.i2c_bus = Adafruit_I2C(self.address, busnum = 1, debug = False)  # second argument tells bus number
        self.debug = debug
        self.read_wait = 0.5  # Longest delay is 200ms

    def send_command(self, byteList, recvLength):
        self.i2c_bus.writeList(self.eps, byteList)
        time.sleep(self.read_wait)  # Look up in datasheet
        reply = self.i2c_bus.readList(self.eps, recvLength)
        if reply[0] == 0xFF and reply[1] == 0xFF:
            if self.debug == 1:
                print("ERROR EMPTY RESPONSE FROM EPS")
        return reply

    def voltage_12v(self):  # input BCR number 1-9
        rsp = self.send_command([0x10, 0xE2, 0x30], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.01349 * Num
        if self.debug == 1:
            print('12v voltage: ' + str(result))
        return result

    def current_12v(self):  # input BCR number 1-9
        rsp = self.send_command([0x10, 0xE2, 0x34], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.00207 * Num
        if self.debug == 1:
            print('12v current: ' + str(result))
        return result

    def voltage_5v(self):  # input BCR number 1-9
        rsp = self.send_command([0x10, 0xE2, 0x10], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.005865 * Num
        if self.debug == 1:
            print("5v voltage: " + str(result))
        return result

    def current_5v(self):  # input BCR number 1-9
        rsp = self.send_command([0x10, 0xE2, 0x14], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.005237 * Num
        if self.debug == 1:
            print("5v current: " + str(result))
        return result

    def voltage_3v3(self):  # input BCR number 1-9
        rsp = self.send_command([0x10, 0xE2, 0x00], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.004311 * Num
        if self.debug == 1:
            print("3.3v voltage: " + str(result))
        return result

    def current_3v3(self):  # input BCR number 1-9
        rsp = self.send_command([0x10, 0xE2, 0x04], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.005237 * Num
        if self.debug == 1:
            print("3.3v current: " + str(result))
        return result

    def voltage_batt(self):  # input BCR number 1-9
        rsp = self.send_command([0x10, 0xE2, 0x20], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.008978 * Num
        if self.debug == 1:
            print("raw batt voltage: " + str(result))
        return result

    def current_batt(self):  # input BCR number 1-9
        rsp = self.send_command([0x10, 0xE2, 0x24], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.005237 * Num
        if self.debug == 1:
            print("raw batt current: " + str(result))
        return result

    # GET TELEMETRY SECTION
    def Pow_12v(self):
        V = self.voltage_12v()
        I = self.current_12v()
        P = I * V
        if self.debug == 1:
            print("12v power: " + str(P))
        return [I,V,P]

    def Pow_5v(self):
        V = self.voltage_5v()
        I = self.current_5v()
        P = I * V
        if self.debug == 1:
            print("5v power: " + str(P))
        return [I,V,P]

    def Pow_3v3(self):
        V = self.voltage_3v3()
        I = self.current_3v3()
        P = I * V
        if self.debug == 1:
            print("3.3v power: " + str(P))
        return [I,V,P]

    def Pow_Batt(self):
        V = self.voltage_batt()
        I = self.current_batt()
        P = I * V
        if self.debug == 1:
            print("raw batt power: " + str(P))
        return [I,V,P]

    def currentDraw_3v3(self):
        rsp = self.send_command([0x10, 0xE2, 0x05], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.001327547 * Num
        if self.debug == 1:
            print('3.3v current draw from EPS: ' + str(result))
        return result

    def currentDraw_5v(self):
        rsp = self.send_command([0x10, 0xE2, 0x15], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.001327547 * Num
        if self.debug == 1:
            print('5v current draw from EPS: ' + str(result))
        return result

    def BCR_voltage(self, bcr_num):  # input BCR number 1-9
        if bcr_num > 0 and bcr_num < 10:
            cmd = bcr_num * 16
            rsp = self.send_command([0x10, 0xE1, cmd], 2)
            Num = (rsp[0] * 16 * 16) | rsp[1]
            result = Num * 0.0249
            if self.debug == 1:
                print("BCR #" + str(bcr_num) + " voltage: " + str(result))
            return result
        else:
            return -1

    def BCR3_voltage(self):  # input BCR number 1-9
        cmd = 3 * 16
        rsp = self.send_command([0x10, 0xE1, cmd], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = Num * .0099706
        if self.debug == 1:
            print("BCR #3 voltage: " + str(result))
        return result

    def BCR_currentA(self, bcr_num):  # input BCR number 1-9
        if bcr_num > 0 and bcr_num < 10:
            cmd = (bcr_num * 16) + 4
            rsp = self.send_command([0x10, 0xE1, cmd], 2)
            Num = (rsp[0] * 16 * 16) | rsp[1]
            result = Num * 0.0009775
            if self.debug == 1:
                print("BCR #" + str(bcr_num) + " current on connector A: " + str(result))
            return result
        else:
            return -1

    def BCR_currentB(self, bcr_num):  # input BCR number 1-9
        if bcr_num > 0 and bcr_num < 10:
            cmd = (bcr_num * 16) + 5
            rsp = self.send_command([0x10, 0xE1, cmd], 2)
            Num = (rsp[0] * 16 * 16) | rsp[1]
            result = Num * 0.0009775
            if self.debug == 1:
                print("BCR #" + str(bcr_num) + " current on connector B: " + str(result))
            return result
        else:
            return -1

    def BCR_tempA(self, bcr_num):  # input BCR number 1-9
        if bcr_num > 0 and bcr_num < 10:
            cmd = (bcr_num * 16) + 8
            rsp = self.send_command([0x10, 0xE1, cmd], 2)
            Num = (rsp[0] * 16 * 16) | rsp[1]
            result = (0.4963 * Num) - 273.15
            if self.debug == 1:
                print("BCR #" + str(bcr_num) + " temp on connector A: " + str(result))
            return result
        else:
            return -1

    def BCR_tempB(self, bcr_num):  # input BCR number 1-9
        if bcr_num > 0 and bcr_num < 10:
            cmd = (bcr_num * 16) + 9
            rsp = self.send_command([0x10, 0xE1, cmd], 2)
            Num = (rsp[0] * 16 * 16) | rsp[1]
            result = (0.4963 * Num) - 273.15
            if self.debug == 1:
                print("BCR #" + str(bcr_num) + " temp on connector B: " + str(result))
            return result
        else:
            return -1

    def BCR_SunA(self, bcr_num):  # input BCR number 1-9
        if bcr_num > 0 and bcr_num < 10:
            cmd = (bcr_num * 16) + 0x0C
            rsp = self.send_command([0x10, 0xE1, cmd], 2)
            Num = (rsp[0] * 16 * 16) | rsp[1]
            if Num < 512:
                Illuminated = 1
                if self.debug == 1:
                    print("BCR #" + str(bcr_num) + " connector A is illuminated")
            else:
                Illuminated = 0
                if self.debug == 1:
                    print("BCR #" + str(bcr_num) + " connector A is not illuminated")
            return Illuminated
        else:
            return -1

    def BCR_SunB(self, bcr_num):  # input BCR number 1-9
        if bcr_num > 0 and bcr_num < 10:
            cmd = (bcr_num * 16) + 0x0D
            rsp = self.send_command([0x10, 0xE1, cmd], 2)
            Num = (rsp[0] * 16 * 16) | rsp[1]
            if Num < 512:
                Illuminated = 1
                if self.debug == 1:
                    print("BCR #" + str(bcr_num) + " connector B is illuminated")
            else:
                Illuminated = 0
                if self.debug == 1:
                    print("BCR #" + str(bcr_num) + " connector B is not illuminated")
            return Illuminated
        else:
            return -1

    def motherBoard_temp(self):
        rsp = self.send_command([0x10, 0xE3, 0x08], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = (0.372434 * Num) - 273.15
        if self.debug == 1:
            print("Motherboard Temp: " + str(result))
        return result

    def daughterBoard_temp(self):
        rsp = self.send_command([0x10, 0xE3, 0x88], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = (0.372434 * Num) - 273.15
        if self.debug == 1:
            print("Motherboard Temp: " + str(result))
        return result












    '''////////////////////////**************************'''
    '''UNTESTED FUNCTIONS'''


    def board_status(self):
        Result = self.send_command([0x01, 0x00], 4)  # ADC Channel 9. Unit: C

        Data0 = Result[3]
        Data1 = Result[2]

        Mother_Status = [0] * 8 # even though 2 bytes are dedicated to the motherboard, only one of them stores data

        Mother_Status[0] = int(Data0 & 0b1 != 0)
        Mother_Status[1] = int(Data0 & 0b10 != 0)
        Mother_Status[2] = int(Data0 & 0b100 != 0)
        Mother_Status[3] = int(Data0 & 0b1000 != 0)
        Mother_Status[4] = int(Data0 & 0b10000 != 0)
        Mother_Status[5] = int(Data0 & 0b100000 != 0)
        Mother_Status[6] = int(Data0 & 0b1000000 != 0)
        Mother_Status[7] = int(Data0 & 0b10000000 != 0) # Actually, the last bit is unused


        Daughter_Status = [0] * 8  # even though 2 bytes are dedicated to the daughterboard, only one of them stores data

        Daughter_Status[0] = int(Data1 & 0b1 != 0)
        Daughter_Status[1] = int(Data1 & 0b10 != 0)
        Daughter_Status[2] = int(Data1 & 0b100 != 0)
        Daughter_Status[3] = int(Data1 & 0b1000 != 0)
        Daughter_Status[4] = int(Data1 & 0b10000 != 0)
        Daughter_Status[5] = int(Data1 & 0b100000 != 0)
        Daughter_Status[6] = int(Data1 & 0b1000000 != 0)
        Daughter_Status[7] = int(Data1 & 0b10000000 != 0)  # Actually, the last bit is unused

        # print(final_Output)  # Return bytes represent the state of each PDM, 0 is off and 1 on
        if self.debug == 1:
            print('board_status Result = ' + str(bin(Data0)) + ' ' + str(bin(Data1)))

            if Mother_Status[0] == 1:
                print('Motherboard: last command not recognised')
            if Mother_Status[1] == 1:
                print('Motherboard: watchdog error occurred, resetting the device')
            if Mother_Status[2] == 1:
                print('Motherboard: the data sent along with the last command was incorrect')
            if Mother_Status[3] == 1:
                print('Motherboard: the channel passed with the last command was incorrect')
            if Mother_Status[4] == 1:
                print('Motherboard: there has been an error reading the EEPROM')
            if Mother_Status[5] == 1:
                print('Motherboard: Power On Reset error occurred')
            if Mother_Status[6] == 1:
                print('Motherboard: Brown Out Reset occurred')

            if Daughter_Status[0] == 1:
                print('Daughterboard: last command not recognised')
            if Daughter_Status[1] == 1:
                print('Daughterboard: watchdog error occurred, resetting the device')
            if Daughter_Status[2] == 1:
                print('Daughterboard: the data sent along with the last command was incorrect')
            if Daughter_Status[3] == 1:
                print('Daughterboard: the channel passed with the last command was incorrect')
            if Daughter_Status[4] == 1:
                print('Daughterboard: there has been an error reading the EEPROM')
            if Daughter_Status[5] == 1:
                print('Daughterboard: Power On Reset error occurred')
            if Daughter_Status[6] == 1:
                print('Daughterboard: Brown Out Reset occurred')

        return Result

    # def get_last_error(self):
    #     # THERE IS A TABLE 11-4 ON THE DATASHEET, BUT THE SIGNIFICANCE OF THE BITS IS AMBIGUOUS
    #
    # def get_version(self):
    #     # THERE IS A TABLE 11-4 ON THE DATASHEET, BUT THE SIGNIFICANCE OF THE BITS IS AMBIGUOUS

    def get_checksum(self):
        # NOT SURE IF IT MAKES SENSE
        rsp = self.send_command([0x05, 0x00], 4)
        Daughterboard_Checksum = (rsp[0] * 16 * 16) | rsp[1]
        Motherboard_Checksum = (rsp[2] * 16 * 16) | rsp[3]
        if self.debug == 1:
            print('The Motherboard Checksum is ', Motherboard_Checksum)
            print('and the Daughterboard Checksum is ', Daughterboard_Checksum)
        return [Motherboard_Checksum, Daughterboard_Checksum]

    # def get_checksum(self):
    #     # PROBABLY THERE IS A MORE EFFICIENT WAY OF DOING THIS
    #     # FOR INSTANCE, IT IS NOT NECESSARY TO CHECK EACH SINGLE BIT:
    #     # IT MIGHT BE ENOUGH TO JUST COMPARE THE TWO SUMS IN EACH OF THE TWO BYTE PAIRS
    #     Result = self.send_command([0x05, 0x00], 4)
    #
    #     Data0 = Result[0]
    #     Data1 = Result[1]
    #     Data2 = Result[2]
    #     Data3 = Result[3]
    #
    #     # Then we extract bits from those four bytes:
    #     Motherboard_Checksum = [0] * 16
    #     Motherboard_Checksum[0] = int(Data2 & 0b1 != 0)
    #     Motherboard_Checksum[1] = int(Data2 & 0b10 != 0)
    #     Motherboard_Checksum[2] = int(Data2 & 0b100 != 0)
    #     Motherboard_Checksum[3] = int(Data2 & 0b1000 != 0)
    #     Motherboard_Checksum[4] = int(Data2 & 0b10000 != 0)
    #     Motherboard_Checksum[5] = int(Data2 & 0b100000 != 0)
    #     Motherboard_Checksum[6] = int(Data2 & 0b1000000 != 0)
    #     Motherboard_Checksum[7] = int(Data2 & 0b10000000 != 0)
    #
    #     Motherboard_Checksum[8] = int(Data3 & 0b1 != 0)
    #     Motherboard_Checksum[9] = int(Data3 & 0b10 != 0)
    #     Motherboard_Checksum[10] = int(Data3 & 0b100 != 0)
    #     Motherboard_Checksum[11] = int(Data3 & 0b1000 != 0)
    #     Motherboard_Checksum[12] = int(Data3 & 0b10000 != 0)
    #     Motherboard_Checksum[13] = int(Data3 & 0b100000 != 0)
    #     Motherboard_Checksum[14] = int(Data3 & 0b1000000 != 0)
    #     Motherboard_Checksum[15] = int(Data3 & 0b10000000 != 0)
    #
    #     Daughterboard_Checksum = [0] * 16
    #     Daughterboard_Checksum[0] = int(Data0 & 0b1 != 0)
    #     Daughterboard_Checksum[1] = int(Data0 & 0b10 != 0)
    #     Daughterboard_Checksum[2] = int(Data0 & 0b100 != 0)
    #     Daughterboard_Checksum[3] = int(Data0 & 0b1000 != 0)
    #     Daughterboard_Checksum[4] = int(Data0 & 0b10000 != 0)
    #     Daughterboard_Checksum[5] = int(Data0 & 0b100000 != 0)
    #     Daughterboard_Checksum[6] = int(Data0 & 0b1000000 != 0)
    #     Daughterboard_Checksum[7] = int(Data0 & 0b10000000 != 0)
    #
    #     Daughterboard_Checksum[8] = int(Data1 & 0b1 != 0)
    #     Daughterboard_Checksum[9] = int(Data1 & 0b10 != 0)
    #     Daughterboard_Checksum[10] = int(Data1 & 0b100 != 0)
    #     Daughterboard_Checksum[11] = int(Data1 & 0b1000 != 0)
    #     Daughterboard_Checksum[12] = int(Data1 & 0b10000 != 0)
    #     Daughterboard_Checksum[13] = int(Data1 & 0b100000 != 0)
    #     Daughterboard_Checksum[14] = int(Data1 & 0b1000000 != 0)
    #     Daughterboard_Checksum[15] = int(Data1 & 0b10000000 != 0)
    #
    #     if self.debug == 1:
    #         print('The Motherboard Checksum is ', Motherboard_Checksum)
    #         print('and the Daughterboard Checksum is ', Daughterboard_Checksum)
    #     return [Motherboard_Checksum, Daughterboard_Checksum]


    def get_comms_watchdog_period(self):
        rsp = self.send_command([0x20, 0x00], 2)  # ADC Channel 9. Unit: C
        Num = (rsp[0] * 16 * 16) | rsp[1]
        if self.debug == 1:
            print('The Comms Watchdog Period is ' + str(Num))
        return Num

    def set_comms_watchdog_period(self, period):  # input desired timeout period in minutes, default is 4mins
        self.i2c_bus.writeList(self.eps, [0x21, period])  # minimum 1 minute, max 90 minutes
        if self.debug == 1:
            print('We have set the Comms Watchdog period to be ' + str(period) + 'minutes')

    def reset_comms_watchdog(self):
        self.i2c_bus.writeList(self.eps, [0x22, 0x00])
        # Returns 0 bytes
        if self.debug == 1:
            print('We have just reset the Comms Watchdog')

    def get_number_brownout_resets(self):
        rsp = self.send_command([0x31, 0x00], 4)
        Num_Mother = (rsp[0] * 16 * 16) | rsp[1]
        Num_Daughter = (rsp[2] * 16 * 16) | rsp[3]
        if self.debug == 1:
            print('The number of Motherboard Brown-Out Resets is ' + str(Num_Mother))
            print('and the number of Daughterboard Automatic Software Resets is ' + str(Num_Daughter))
        return [Num_Mother, Num_Daughter]

    def get_number_of_auto_software_resets(self):
        rsp = self.send_command([0x32, 0x00], 4)
        Num_Mother = (rsp[0] * 16 * 16) | rsp[1]
        Num_Daughter = (rsp[2] * 16 * 16) | rsp[3]
        if self.debug == 1:
            print('The number of Motherboard Automatic Software Resets is ' + str(Num_Mother))
            print('and the number of Daughterboard Automatic Software Resets is ' + str(Num_Daughter))
        return [Num_Mother, Num_Daughter]

    def get_number_of_manual_resets(self):
        rsp = self.send_command([0x33, 0x00], 4)
        Num_Mother = (rsp[0] * 16 * 16) | rsp[1]
        Num_Daughter = (rsp[2] * 16 * 16) | rsp[3]
        '''WHY DO WE NOT REVERSE THE ORDER OF THE ANSWER BYTES?'''
        if self.debug == 1:
            print('The number of Motherboard Manual Resets is ' + str(Num_Mother))
            print('and the number of Daughterboard Manual Resets is ' + str(Num_Daughter))
        return [Num_Mother, Num_Daughter]

    def get_number_comms_watchdog_resets(self):
        rsp = self.send_command([0x34, 0x00], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        if self.debug == 1:
            print('The number of Comms Watchdog Resets is ' + str(Num))
        return Num

    '''HERE BEGINS THE UNTESTED PDM SECTION'''

    def switch_on_all_PDMs(self):
        self.i2c_bus.writeList(self.eps, [0x40, 0x00])
        # time.sleep(self.read_wait) # According to the datasheet, there is no delay
        # Returns 0 bytes
        '''APPARENTLY WORKS'''

    def switch_off_all_PDMs(self):
        self.i2c_bus.writeList(self.eps, [0x41, 0x00])
        # time.sleep(self.read_wait) # According to the datasheet, there is no delay
        # Returns 0 bytes
        ##############################################################
        '''APPARENTLY WORKS'''

    def get_actual_state_of_all_PDMs(self):
        Result = self.send_command([0x42, 0x00], 4)  # ADC Channel 9. Unit: C
        # the Result variable should be a list of four bytes, the first of which represents the state of seven PDMs,
        # and the second representing the state of the other three PDMs
        # since we need to reverse the order, the first one is the last and the second one os the second from the end
        Data0 = Result[3]  # first 7 PDMs
        Data1 = Result[2]  # last 3 PDMs

        # Then we extract bits from those two bytes:
        PDM_States = [0] * 11
        PDM_States[0] = -1  # the zeroth item in the list stands for nothing here

        PDM_States[1] = int(Data0 & 0b10 != 0)
        PDM_States[2] = int(Data0 & 0b100 != 0)
        PDM_States[3] = int(Data0 & 0b1000 != 0)
        PDM_States[4] = int(Data0 & 0b10000 != 0)
        PDM_States[5] = int(Data0 & 0b100000 != 0)
        PDM_States[6] = int(Data0 & 0b1000000 != 0)
        PDM_States[7] = int(Data0 & 0b10000000 != 0)

        PDM_States[8] = int(Data1 & 0b1 != 0)
        PDM_States[9] = int(Data1 & 0b10 != 0)
        PDM_States[10] = int(Data1 & 0b100 != 0)

        # print(final_Output)  # Return bytes represent the state of each PDM, 0 is off and 1 on
        if self.debug == 1:
            print('get_actual_state_of_all_PDMs Result = ' + str(bin(Result[0])) + ' ' + str(bin(Result[0])))
            for i in range(1, 11):
                if PDM_States[i] == 0:
                    print(
                    'PDM #' + str(i) + 'is actually OFF  ' + str(PDM_States[i]))  # 0 stands for off, 1 stands of on
                elif PDM_States[i] == 1:
                    print(
                    'PDM #' + str(i) + 'is actually ON  ' + str(PDM_States[i]))  # 0 stands for off, 1 stands of on
                else:
                    print('ERROR')
        return PDM_States
        '''DOES NOT WORK'''

    def get_expected_state_of_all_PDMs(self):
        Result = self.send_command([0x43, 0x00], 4)  # ADC Channel 9. Unit: C
        # the Result variable should be a list of four bytes, the first of which represents the state of seven PDMs,
        # and the second representing the state of the other three PDMs
        # since we need to reverse the order, the first one is the last and the second one os the second from the end
        Data0 = Result[3]  # first 7 PDMs
        Data1 = Result[2]  # last 3 PDMs

        # Then we extract bits from those two bytes:
        PDM_expected_States = [0] * 11
        PDM_expected_States[0] = -1  # the zeroth item in the list stands for nothing here

        PDM_expected_States[1] = int(Data0 & 0b10 != 0)
        PDM_expected_States[2] = int(Data0 & 0b100 != 0)
        PDM_expected_States[3] = int(Data0 & 0b1000 != 0)
        PDM_expected_States[4] = int(Data0 & 0b10000 != 0)
        PDM_expected_States[5] = int(Data0 & 0b100000 != 0)
        PDM_expected_States[6] = int(Data0 & 0b1000000 != 0)
        PDM_expected_States[7] = int(Data0 & 0b10000000 != 0)

        PDM_expected_States[8] = int(Data1 & 0b1 != 0)
        PDM_expected_States[9] = int(Data1 & 0b10 != 0)
        PDM_expected_States[10] = int(Data1 & 0b100 != 0)

        # print(final_Output)  # Return bytes represent the state of each PDM, 0 is off and 1 on
        if self.debug == 1:
            for i in range(1, 11):
                if PDM_expected_States[i] == 0:
                    print('PDM #' + str(i) + 'is expected to be OFF ' + str(PDM_expected_States[i]))  # 0 stands for off, 1 stands of on
                elif PDM_expected_States[i] == 1:
                    print('PDM #' + str(i) + 'is expected to be ON ' + str(PDM_expected_States[i]))  # 0 stands for off, 1 stands of on
                else:
                    print('ERROR')
        return PDM_expected_States
        '''APPARENTLY WORKS'''

    def get_initial_state_of_all_PDMs(self):
        Result = self.send_command([0x44, 0x00], 4)  # ADC Channel 9. Unit: C
        # the Result variable should be a list of four bytes, the first of which represents the state of seven PDMs,
        # and the second representing the state of the other three PDMs
        # since we need to reverse the order, the first one is the last and the second one os the second from the end
        Data0 = Result[3]  # first 7 PDMs
        Data1 = Result[2]  # last 3 PDMs

        # Then we extract bits from those two bytes:
        PDM_initial_States = [0] * 11
        PDM_initial_States[0] = -1  # the zeroth item in the list stands for nothing here

        PDM_initial_States[1] = int(Data0 & 0b10 != 0)
        PDM_initial_States[2] = int(Data0 & 0b100 != 0)
        PDM_initial_States[3] = int(Data0 & 0b1000 != 0)
        PDM_initial_States[4] = int(Data0 & 0b10000 != 0)
        PDM_initial_States[5] = int(Data0 & 0b100000 != 0)
        PDM_initial_States[6] = int(Data0 & 0b1000000 != 0)
        PDM_initial_States[7] = int(Data0 & 0b10000000 != 0)

        PDM_initial_States[8] = int(Data1 & 0b1 != 0)
        PDM_initial_States[9] = int(Data1 & 0b10 != 0)
        PDM_initial_States[10] = int(Data1 & 0b100 != 0)

        # print(final_Output)  # Return bytes represent the state of each PDM, 0 is off and 1 on
        if self.debug == 1:
            for i in range(1, 11):
                if PDM_initial_States[i] == 0:
                    print('PDM #' + str(i) + 'is initially OFF  ' + str(
                        PDM_initial_States[i]))  # 0 stands for off, 1 stands of on
                elif PDM_initial_States[i] == 1:
                    print('PDM #' + str(i) + 'is initially ON  ' + str(
                        PDM_initial_States[i]))  # 0 stands for off, 1 stands of on
                else:
                    print('ERROR')
        return PDM_initial_States
        '''APPARENTLY WORKS'''

    def set_all_PDMs_to_initial_state(self):
        self.i2c_bus.writeList(self.eps, [0x45, 0x00])  # ADC Channel 9
        '''???THE AVAILIABLE COMMANDS TABLE SAYS IT RETURNS 4 BYTES, WHILE THE DESCRIPTION SAYS IT RETURNS NOTHING'''

        '''APPARENTLY WORKS'''

    def switch_PDM_N_on(self, N):  # Input PDM to turn on, PDM1 is 0x01
        self.i2c_bus.writeList(self.eps, [0x50, N])  # ADC Channel 9
        time.sleep(self.read_wait)  # Unit: C
        # Returns 0 bytes, biggest delay in thi
        '''APPARENTLY WORKS'''

    def switch_PDM_N_off(self, N):
        self.i2c_bus.writeList(self.eps, [0x51, N])  # ADC Channel 9
        time.sleep(self.read_wait)  # Unit: C
        # Returns 0 bytes
        '''APPARENTLY WORKS'''

    def set_PDM_N_initial_state_on(self, N):
        # Pre-defines the intial state of a certain PDM as "ON"
        self.i2c_bus.writeList(self.eps, [0x52, N])  # ADC Channel 9
        time.sleep(self.read_wait)
        # Returns 0 bytes
        '''APPARENTLY WORKS'''

    def set_PDM_N_initial_state_off(self, N):
        # Pre-defines the intial state of a certain PDM as "OFF"
        self.i2c_bus.writeList(self.eps, [0x53, N])  # ADC Channel 9
        time.sleep(self.read_wait)
        # Returns 0 bytes
        '''APPARENTLY WORKS'''

    def get_PDM_N_actual_status(self, N):
        rsp = self.send_command([0x54, N], 2)  # ADC Channel 9. Unit: C
        Num = (rsp[0] * 16 * 16) | rsp[1]
        if self.debug == 1:
            print('The actual status of PDM #' + str(N) + ' is ' + str(Num))
        return Num
        '''APPARENTLY WORKS'''

    def set_PDM_N_timer_limit(self, limit, N):  # Set length of time channel can remain enabled for
        self.i2c_bus.writeList(self.eps, [0x60, N, limit])  # ADC Channel 9
        """CHECK WHETHER THE ORDER OF N AND LIMIT SHOULD BE REVERSED"""
        # Righ now it seems that since Data[0] = limit and Data[1] = N, then N should go first due to the reverse
        time.sleep(self.read_wait)
        # limit = 0xFF sets PDM enabled indefinitely
        # Returns 0 bytes
        '''APPARENTLY WORKS'''

    # def get_PDM_N_timer_limit(self, N):
    #     self.i2c_bus.writeList(self.eps, [0x61, N])  # ADC Channel 9
    #     time.sleep(self.read_wait)
    #     # Returns 0 bytes (doesn't really make sense cause its a get)
    #
    # def get_PDM_N_tcurrent_timer_value(self, N):
    #     self.i2c_bus.writeList(self.eps, [0x62, N])  # ADC Channel 9
    #     time.sleep(self.read_wait)
    #     # Returns 0 bytes, same as the one before


    def get_PDM_N_timer_limit(self, N):
        rsp = self.send_command([0x61, N], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        if self.debug == 1:
            print('The timer limit of PDM #' + str(N) + ' is ' + str(Num))
        return Num
        '''APPARENTLY WORKS'''

    def get_PDM_N_tcurrent_timer_value(self, N):
        rsp = self.send_command([0x62, N], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        if self.debug == 1:
            print('The current timer value of PDM #' + str(N) + ' is ' + str(Num))
        return Num
        '''APPARENTLY WORKS'''


    def PCM_reset(self, bus):  # input strings: BatteryV, 5V, 3.3V, or 12V
        powerBus = {'BatteryV': 0x01, '5V': 0x02, '3.3V': 0x04, '12V': 0x08}
        self.i2c_bus.writeList(self.eps, [0x70, powerBus[bus]])  # ADC Channel 9
        time.sleep(self.read_wait)
        # No bytes returned

    def manual_reset(self):
        self.i2c_bus.writeList(self.eps, [0x80, 0x00])  # ADC Channel 9
        time.sleep(self.read_wait)
        # Returns 0 bytes

    '''HERE GOES THE UNTESTED SECTION WHERE I GET ALL THE OUTPUT VOLTAGES AND CURRENTS ON SWITCHES'''

    def voltage_switch_1(self):
        rsp = self.send_command([0x10, 0xE4, 0x10], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.01349 * Num
        if self.debug == 1:
            print('Switch 1 output voltage: ' + str(result))
        return result

    def current_switch_1(self):
        rsp = self.send_command([0x10, 0xE4, 0x14], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.001328 * Num
        if self.debug == 1:
            print('Switch 1 output current: ' + str(result))
        return result

    # def Pow_switch_1(self):
    #     V = self.voltage_switch_1()
    #     I = self.current_switch_1()
    #     P = I * V
    #     if self.debug == 1:
    #         print("Switch 1 output power: " + str(P))
    #     return P



    def voltage_switch_2(self):
        rsp = self.send_command([0x10, 0xE4, 0x20], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.01349 * Num
        if self.debug == 1:
            print('Switch 2 output voltage: ' + str(result))
        return result

    def current_switch_2(self):
        rsp = self.send_command([0x10, 0xE4, 0x24], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.001328 * Num
        if self.debug == 1:
            print('Switch 2 output current: ' + str(result))
        return result

    def voltage_switch_3(self):
        rsp = self.send_command([0x10, 0xE4, 0x30], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.008993 * Num
        if self.debug == 1:
            print('Switch 3 output voltage: ' + str(result))
        return result

    def current_switch_3(self):
        rsp = self.send_command([0x10, 0xE4, 0x34], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.001328 * Num
        if self.debug == 1:
            print('Switch 3 output current: ' + str(result))
        return result

    def voltage_switch_4(self):
        rsp = self.send_command([0x10, 0xE4, 0x40], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.008993 * Num
        if self.debug == 1:
            print('Switch 4 output voltage: ' + str(result))
        return result

    def current_switch_4(self):
        rsp = self.send_command([0x10, 0xE4, 0x44], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.001328 * Num
        if self.debug == 1:
            print('Switch 4 output current: ' + str(result))
        return result

    def voltage_switch_5(self):
        rsp = self.send_command([0x10, 0xE4, 0x50], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.005865 * Num
        if self.debug == 1:
            print('Switch 5 output voltage: ' + str(result))
        return result

    def current_switch_5(self):
        rsp = self.send_command([0x10, 0xE4, 0x54], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.001328 * Num
        if self.debug == 1:
            print('Switch 5 output current: ' + str(result))
        return result

    def voltage_switch_6(self):
        rsp = self.send_command([0x10, 0xE4, 0x60], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.005865 * Num
        if self.debug == 1:
            print('Switch 6 output voltage: ' + str(result))
        return result

    def current_switch_6(self):
        rsp = self.send_command([0x10, 0xE4, 0x64], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.001328 * Num
        if self.debug == 1:
            print('Switch 6 output current: ' + str(result))
        return result

    def voltage_switch_7(self):
        rsp = self.send_command([0x10, 0xE4, 0x70], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.005865 * Num
        if self.debug == 1:
            print('Switch 7 output voltage: ' + str(result))
        return result

    def current_switch_7(self):
        rsp = self.send_command([0x10, 0xE4, 0x74], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.001328 * Num
        if self.debug == 1:
            print('Switch 7 output current: ' + str(result))
        return result

    def voltage_switch_8(self):
        rsp = self.send_command([0x10, 0xE4, 0x80], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.004311 * Num
        if self.debug == 1:
            print('Switch 8 output voltage: ' + str(result))
        return result

    def current_switch_8(self):
        rsp = self.send_command([0x10, 0xE4, 0x84], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.001328 * Num
        if self.debug == 1:
            print('Switch 8 output current: ' + str(result))
        return result

    def voltage_switch_9(self):
        rsp = self.send_command([0x10, 0xE4, 0x90], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.004311 * Num
        if self.debug == 1:
            print('Switch 9 output voltage: ' + str(result))
        return result

    def current_switch_9(self):
        rsp = self.send_command([0x10, 0xE4, 0x94], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.001328 * Num
        if self.debug == 1:
            print('Switch 9 output current: ' + str(result))
        return result

    def voltage_switch_10(self):
        rsp = self.send_command([0x10, 0xE4, 0xA0], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.004311 * Num
        if self.debug == 1:
            print('Switch 10 output voltage: ' + str(result))
        return result

    def current_switch_10(self):
        rsp = self.send_command([0x10, 0xE4, 0xA4], 2)
        Num = (rsp[0] * 16 * 16) | rsp[1]
        result = 0.001328 * Num
        if self.debug == 1:
            print('Switch 10 output current: ' + str(result))
        return result


