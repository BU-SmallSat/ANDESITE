from Adafruit_I2C import Adafruit_I2C
import time

'''
Throw error returns if the response from EPS is 0xF000
this means that the reading information is not yet ready to be read
or that the read was not preceeded by a write
'''
class EPS_driver:
    def __init__(self):
        self.address = 0x20
        self.eps = 0x2B
        self.i2c_bus = Adafruit_I2C(self.address, bus_num=1, debug=True) #second argument tells bus number
        self.heater_status = 1
        self.read_wait = 0.2   # Longest delay is 200ms

    def send_command(self, byteList, recvLength):
        self.i2c_bus.writeList(self.eps, byteList)
        time.sleep(self.read_wait) #Look up in datasheet
        reply = self.i2c_bus.readList(self.eps, recvLength)
        return reply

    def board__status(self):
        Num = self.send_command([0x00, 0x01], 4)
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        #print (Num)
        return Num #if Num == 0, Charge == 0. else: "Discharge" == 1

    def get__last__error(self):
        Num = self.send_command([0x00, 0x03], 4)
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        final_Output = Num
        #print(final_Output)  # check if necessary
        return final_Output

    def get__version(self):
        Num = self.send_command([0x00, 0x04], 4)
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        final_Output = Num
        #print(final_Output)  # check if necessary
        return final_Output

    def get__checksum(self):
        Num = self.send_command([0x00, 0x05], 4) # return data type?? value in the space
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        final_Output=Num
        #print (final_Output)  # check if necessary
        return final_Output

 # GET TELEMETRY SECTION
    def Pow_12v(self):
        Num = self.send_command([0x30, 0xE2, 0x10], 2)
        Num = ((Num[1]) >> 8) | ((Num[0]) << 8)
        V = 0.01349 * Num
        Num = self.send_command([0x34, 0xE2, 0x10], 2)
        Num = ((Num[1]) >> 8) | ((Num[0]) << 8)
        I = 0.00207 * Num
        return I*V

    def Pow_5v(self):
        Num = self.send_command([0x10, 0xE2, 0x10], 2)
        Num = ((Num[1]) >> 8) | ((Num[0]) << 8)
        V = 0.005865  * Num
        Num = self.send_command([0x14, 0xE2, 0x10], 2)
        Num = ((Num[1]) >> 8) | ((Num[0]) << 8)
        I = 0.005237 * Num
        return I*V


    def Pow_3v3(self):
        Num = self.send_command([0x00, 0xE2, 0x10], 2)
        Num = ((Num[1]) >> 8) | ((Num[0]) << 8)
        V = 0.004311 * Num
        Num = self.send_command([0x04, 0xE2, 0x10], 2)
        Num = ((Num[1]) >> 8) | ((Num[0]) << 8)
        I = 0.005237  * Num
        return I*V

    def Pow_Batt(self):
        Num = self.send_command([0x20, 0xE2, 0x10], 2)
        Num = ((Num[1]) >> 8) | ((Num[0]) << 8)
        V = 0.008978 * Num
        Num = self.send_command([0x24, 0xE2, 0x10], 2)
        Num = ((Num[1]) >> 8) | ((Num[0]) << 8)
        I = 0.005237 * Num
        return I * V

    def V_Batt(self):
        Num = self.send_command([0x20, 0xE2, 0x10], 2)
        Num = ((Num[1]) >> 8) | ((Num[0]) << 8)
        V = 0.008978 * Num

        return V

    def BCR_voltage(self, bcr_num): # input BCR number 1-9
        if bcr_num > 0 and bcr_num < 10:
            cmd = bcr_num * 16
            Num = self.send_command([cmd, 0xE1, 0x10], 2)
            Num = ((Num[1]) >> 8) | ((Num[0]) << 8)
            result = Num * 0.0249
            #print(result)
            return result
        else:
            return -1
    
    def BCR_current(self, bcr_num): # input BCR number 1-9
        if bcr_num > 0 and bcr_num < 10:
            cmd = bcr_num * 16 + 4
            Num = self.send_command([cmd, 0xE1, 0x10], 2)
            Num = ((Num[1]) >> 8) | ((Num[0]) << 8)
            result = Num * 0.0009775
            #print(result)
            return result
        else:
            return -1

    def BCR_temperature(self, bcr_num): # input BCR number 1-9
        if bcr_num > 0 and bcr_num < 10:
            cmd = bcr_num * 16 + 8
            Num = self.send_command([cmd, 0xE1, 0x10], 2)
            Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
            result = (0.4963*Num) - 273.15
            #print(result)
            return result
        else:
            return -1

    def EPS_currentDraw_3v3(self):
        Num = self.send_command([0x05, 0xE2, 0x10], 2)
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        result = 0.001327547 * Num
        return result

    def EPS_currentDraw_5v(self):
        Num = self.send_command([0x15, 0xE2, 0x10], 2)
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        result = 0.001327547 * Num
        return result

    def motherBoard_temperature(self):
        Num = self.send_command([0x08, 0xE3, 0x10], 2)
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        result = (0.372434 * Num) - 275.15
        return result

    def daughterBoard_temperature(self):
        Num = self.send_command([0x88, 0xE3, 0x10], 2)
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        result = (0.372434 * Num) - 275.15
        return result

    def get__comms__watchdog__period(self):
        Num = self.send_command([0x00, 0x20], 2)  # ADC Channel 5. Range of Num??
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        #print (Num)
        return Num #if Num == 0, Charge == 0. else: "Discharge" == 1

    def set__comms__watchdog__period(self, period): # input desired timeout period in minutes, default is 4mins
        self.i2c_bus.write16(self.eps, [period, 0x21]) # minimum 1 minute, max 90 minutes
        time.sleep(self.read_wait) 
        #Function does not return any bytes, no need to read

    def reset__comms__watchdog(self):
        self.i2c_bus.write16(self.eps, 0x0022) # ADC Channel 4
        time.sleep(self.read_wait)
        #Function does not return any bytes

    def get__number__brownout__resets(self):
        Num = self.send_command([0x00, 0x31], 4) # ADC Channel 6. Unit: mA
        final_Output = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        #print(final_Output)  # check if necessary
        return final_Output

    def get__number__autosoftware__resets(self):
        Num = self.send_command([0x00, 0x32], 4) #ADC channel 7. Unit: V
        final_Output = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        #print(final_Output)  # check if necessary
        return final_Output

    def get__number__manual__resets(self):
        Num = self.send_command([0x00, 0x33], 4) # ADC Channel 8. Unit: V
        final_Output = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        #print(final_Output)  # check if necessary
        return final_Output

    def get__number__comms__watchdog__resets(self):
        Num = self.send_command([0x00, 0x34], 2) # ADC Channel 9. Unit: C
        final_Output = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        #print(final_Output)  # check if necessary
        return final_Output

    def switch__on__all__PDMs(self):
        self.i2c_bus.write16(self.eps, 0x0040)  # ADC Channel 9
        time.sleep(self.read_wait)
        #Returns 0 bytes 

    def switch__off__all__PDMs(self):
        self.i2c_bus.write16(self.eps, 0x0041)  # ADC Channel 9
        time.sleep(self.read_wait)
        #Returns 0 bytes

    def get__actual__state__all__PDMs(self): 
        Num = self.send_command([0x00, 0x42], 4)  #ADC Channel 9. Unit: C
        final_Output = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        #print(final_Output)  # Return bytes represent the state of each PDM, 0 is off and 1 on
        return final_Output

    def get__expected__state__all__PDMs(self):
        Num = self.send_command([0x00, 0x43], 4)  #ADC Channel 9. Unit: C
        final_Output = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        #print(final_Output)  # check if necessary
        return final_Output

    def get__initial__state__all__PDMs(self):
        Num = self.send_command([0x00, 0x44], 4)  # ADC Channel 9. Unit: C
        final_Output = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        #print(final_Output)  # check if necessary
        return final_Output

    def set__all__PDMs__initial__state(self):
        Num = self.send_command([0x00, 0x45], 4) # ADC Channel 9. Unit: C
        final_Output = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        #print(final_Output)
        return final_Output

    def switch__PDM_N__on(self, N): #Input PDM to turn on, PDM1 is 0x01
        self.i2c_bus.writeList(self.eps, [N, 0x50])  # ADC Channel 9
        time.sleep(self.read_wait) # Unit: C
        #Returns 0 bytes, biggest delay in thi

    def switch__PDM_N__off(self, N):
        self.i2c_bus.writeList(self.eps, [N, 0x51])  # ADC Channel 9
        time.sleep(self.read_wait) # Unit: C
        #Returns 0 bytes

    def set__PDM_N__initial__state__on(self, N):
        self.i2c_bus.writeList(self.eps, [N, 0x52])  # ADC Channel 9
        time.sleep(self.read_wait)
        #Returns 0 bytes

    def set__PDM_N__initial__state__off(self, N):
        self.i2c_bus.writeList(self.eps, [N, 0x53])  # ADC Channel 9
        time.sleep(self.read_wait)
        #Returns 0 bytes

    def get__PDM_N__actual__status(self, N):
        Num = self.send_command([N, 0x54], 2) # ADC Channel 9. Unit: C
        final_Output = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        #print(final_Output)  # check if necessary
        return final_Output

    def set__PDM_N__timer__limit(self, limit, N): # Set length of time channel can remain enabled for
        self.i2c_bus.writeList(self.eps, [N, limit, 0x60])  # ADC Channel 9
        time.sleep(self.read_wait)
        #limit = 0xFF sets PDM enabled indefinitely 
        #Returns 0 bytes

    def get__PDM_N__timer__limit(self, N):
        self.i2c_bus.writeList(self.eps, [N, 0x61])  # ADC Channel 9
        time.sleep(self.read_wait)
        #Returns 0 bytes (doesn't really make sense cause its a get)

    def get__PDM_N__tcurrent__timer__value(self, N):
        self.i2c_bus.writeList(self.eps, [N, 0x62])  # ADC Channel 9
        time.sleep(self.read_wait)
        #Returns 0 bytes, same as the one before

    def PCM__reset(self, bus): #input Battery, 5V, 3.3V or 12V
        powerBus = {"Battery: 0x01, 5V: 0x02, 3.3V: 0x04, 12V: 0x08"}
        self.i2c_bus.writeList(self.eps, [powerBus[bus], 0x62])  # ADC Channel 9
        time.sleep(self.read_wait)
        #No bytes returned

    def manual__reset(self):
        self.i2c_bus.write16(self.eps, 0x0080)  # ADC Channel 9
        time.sleep(self.read_wait)
        #Returns 0 bytes


    '''
    def adc__battery2__current__direction(self):
        self.i2c_bus.write8(0x00, 0x0A)  # ADC Channel 10
        time.sleep(self.read_wait)
        Num = self.i2c_bus.readS16(0xFF)  # Unit:
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        print (Num)
        return Num #if Num == 0, Charge == 0. else: "Discharge" == 1

    def adc__battery2__current(self):
        self.i2c_bus.write8(0x00, 0x0B)  # ADC Channel 11
        time.sleep(self.read_wait)
        Num = self.i2c_bus.readS16(0xFF)  # Unit: mA
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        final_Output = -3.49185 * Num + 3173.465
        print(final_Output)  # check if necessary
        return final_Output

    def adc__cell2__voltage(self):
        self.i2c_bus.write8(0x00, 0x0C)  # ADC Channel 12
        time.sleep(self.read_wait)
        Num = self.i2c_bus.readS16(0xFF)  # Unit: mA
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        final_Output = -0.00483 * Num + 4.852724
        print(final_Output)  # check if necessary
        return final_Output

    def adc__battery2__voltage(self):
        self.i2c_bus.write8(0x00, 0x0D)  # ADC Channel 13
        time.sleep(self.read_wait)
        Num = self.i2c_bus.readS16(0xFF)  # Unit: mA
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        final_Output = -0.00483 * Num + 4.852724
        print(final_Output)  # check if necessary
        return final_Output

    def adc__battery2__temperature(self):
        self.i2c_bus.write8(0x00, 0x0E)  # ADC Channel 14
        time.sleep(self.read_wait)
        Num = self.i2c_bus.readS16(0xFF)  # Unit: mA
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        final_Output = -0.163 * Num + 4.753
        print(final_Output)  # check if necessary
        return final_Output

    def status(self):
        self.i2c_bus.write8(0x01, 0x00) # Bit 0 Unknown Command Type
        time.sleep(self.read_wait)
        Num = self.i2c_bus.readS16(0xFF)
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        print(Num)

        # parsing of response still needs to be done and tested!!
        converttobi = bin(feedback)[2:].zfill(16)
        digits = [int(i) for i in converttobi]
        dict = {
                'not_used0': 0,
                'not_used1': 0,
                'not_used2': 0,
                'not_used3': 0,
                'received_message_too_long': 0,
                'i2c_overflow': 0,
                'i2c_write_collision': 0,
                'i2c_error': 0,
                'brown_out_reset_occurred': 0,
                'power_on_reset_occurred': 0,
                'watchdog_reset_occurred': 0,
                'oscillator_bit': 0,
                'not_used4': 0,
                'adc_result_not_ready': 0,
                'unknown_command_value': 0,
                'unknown_command_type': 0,
                }
        dict['not_used0'] = digits[0]
        dict['not_used1'] = digits[1]
        dict['not_used2'] = digits[2]
        dict['not_used3'] = digits[3]
        dict['received_message_too_long'] = digits[4]
        dict['i2c_overflow'] = digits[5]
        dict['i2c_write_collision'] = digits[6]
        dict['i2c_error'] = digits[7]
        dict['brown_out_reset_occurred'] = digits[8]
        dict['power_on_reset_occurred'] = digits[9]
        dict['watchdog_reset_occurred'] = digits[10]
        dict['oscillator_bit'] = digits[11]
        dict['not_used4'] = digits[12]
        dict['adc_result_not_ready'] = digits[13]
        dict['unknown_command_value'] = digits[14]
        dict['unknown_command_type'] = digits[15]

        # Remember to get the definition for the 0 and 3 port
        function_mappings = {
                'not_used0': True,
                'not_used1': True,
                'not_used2': True,
                'not_used3': True,
                'received_message_too_long': 0,
                'i2c_overflow': 0,
                'i2c_write_collision': 0,
                'i2c_error': 0,
                'brown_out_reset_occurred': 0,
                'power_on_reset_occurred': 0,
                'watchdog_reset_occurred': 0,
                'oscillator_bit': 0,
                'not_used4': 0,
                'adc_result_not_ready': 0,
                'unknown_command_value': 0,
                'unknown_command_type': 0,
                             }

        for key in function_mappings:
            if function_mappings[key] == 1:
               function_mappings[key]
            else:
                pass

    def heater_on(self):
        self.i2c_bus.write8(0x05, 0x01)  # Bit 0 Unknown Command Type
        time.sleep(self.settleTime)
        Num = self.i2c_bus.readS16(0xFF)
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        print(Num)
        # what does this command return

    def heater_off(self):
        self.i2c_bus.write8(0x05, 0x00)  # Bit 0 Unknown Command Type
        time.sleep(self.settleTime)
        Num = self.i2c_bus.readS16(0xFF)
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        print(Num)
        # what does this command return

    def heater_status(self):
        self.i2c_bus.write8(0x06, 0x00)  # Bit 0 Unknown Command Type
        time.sleep(self.settleTime)
        Num = self.i2c_bus.readS16(0xFF)
        Num = ((Num & 0xFF00) >> 8) | ((Num & 0x00FF) << 8)
        if Num == 0x0001:
            self.heater_status = 0
        elif Num == 0x0000:
            self.heater_status = 1

    def watchdog(self):
        self.i2c_bus.write8(0x80, 0x00)  # Bit 0 Unknown Command Type
        time.sleep(self.settleTime)
        # perfrom restart commands to re-initialize communication with EPS
    '''
