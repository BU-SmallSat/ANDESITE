from Adafruit_I2C import Adafruit_I2C
import tim

'''
Throw error returns if the response from EPS is 0xF000
this means that the reading information is not yet ready to be read
or that the read was not preceeded by a write
'''

class EPS_driver:
    def __init__(self):
        self.address = 0x2A
        self.i2c_bus = Adafruit_I2C(self.address)
        self.heater_status = 1

    def adc__battery__current__direction(self):
        self.i2c_bus.write16(0x00, 0x00) # ADC Channel 0
        time.sleep(2)
        Num = self.i2c_bus.readU16()
        print (Num)
        return Num #if Num == 0, Charge == 0. else: "Discharge" == 1

    def adc__battery__current(self):
        self.i2c_bus.write16(0x00, 0x01)  # ADC Channel 1
        time.sleep(2)
        Num = self.i2c_bus.readU16() # Unit: mA
        final_Output = -3.49185 * Num + 3173.465
        print(final_Output)  # check if necessary
        return final_Output

    def adc__cell__voltage(self):
        self.i2c_bus.write16(0x00, 0x02)  # ADC Channel 2
        time.sleep(2)
        Num = self.i2c_bus.readU16()  # Unit: V
        final_Output = -0.00483 * Num + 4.852724
        print(final_Output)  # check if necessary
        return final_Output

    def adc_battery_voltage(self):
        self.i2c_bus.write16(0x00, 0x03) # ADC Channel 3
        time.sleep(2)
        Num = self.i2c_bus.readU16()  # return data type?? value in the space
        final_Output = -3.49185 * Num + 3173.465
        print (final_Output)  # check if necessary
        return final_Output

    def adc__battery__temperature(self):
        self.i2c_bus.write16(0x00, 0x04)  # ADC Channel 4
        time.sleep(2)
        Num = self.i2c_bus.readU16()  # Unit: C
        final_Output = -0.163 * Num + 4.753
        print(final_Output)  # check if necessary
        return final_Output

    def adc__battery1__current__direction(self):
        self.i2c_bus.write16(0x00, 0x05)  # ADC Channel 5
        time.sleep(2)
        Num = self.i2c_bus.readU16()  # Range of Num??
        print (Num)
        return Num #if Num == 0, Charge == 0. else: "Discharge" == 1

    def adc__battery1__current(self):
        self.i2c_bus.write16(0x00, 0x06)  # ADC Channel 6
        time.sleep(2)
        Num = self.i2c_bus.readU16()  # Unit: mA
        final_Output = -3.49185 * Num + 3173.465
        print(final_Output)  # check if necessary
        return final_Output

    def adc__cell1__voltage(self):
        self.i2c_bus.write16(0x00, 0x07) # ADC Channel 7
        time.sleep(2)
        Num = self.i2c_bus.readU16()  # Unit: V
        final_Output = -0.00483 * Num + 4.852724
        print(final_Output)  # check if necessary
        return final_Output

    def adc__battery1__voltage(self):
        self.i2c_bus.write16(0x00, 0x08)  # ADC Channel 8
        time.sleep(2)
        Num = self.i2c_bus.readU16()  # Unit: V
        final_Output = -3.49185 * Num + 3173.465
        print(final_Output)  # check if necessary
        return final_Output

    def adc__battery1__temperature(self):
        self.i2c_bus.write16(0x00, 0x09)  # ADC Channel 9
        time.sleep(2)
        Num = self.i2c_bus.readU16()  # Unit: C
        final_Output = -0.163 * Num + 4.753
        print(final_Output)  # check if necessary
        return final_Output

    def adc__battery2__current__direction(self):
        self.i2c_bus.write16(0x00, 0x0A)  # ADC Channel 10
        time.sleep(2)
        Num = self.i2c_bus.readU16()  # Unit:
        print (Num)
        return Num #if Num == 0, Charge == 0. else: "Discharge" == 1

    def adc__battery2__current(self):
        self.i2c_bus.write16(0x00, 0x0B)  # ADC Channel 11
        time.sleep(2)
        Num = self.i2c_bus.readU16()  # Unit: mA
        final_Output = -3.49185 * Num + 3173.465
        print(final_Output)  # check if necessary
        return final_Output

    def adc__cell2__voltage(self):
        self.i2c_bus.write16(0x00, 0x0C)  # ADC Channel 12
        time.sleep(2)
        Num = self.i2c_bus.readU16()  # Unit: mA
        final_Output = -0.00483 * Num + 4.852724
        print(final_Output)  # check if necessary
        return final_Output

    def adc__battery2__voltage(self):
        self.i2c_bus.write16(0x00, 0x0D)  # ADC Channel 13
        time.sleep(2)
        Num = self.i2c_bus.readU16()  # Unit: mA
        final_Output = -0.00483 * Num + 4.852724
        print(final_Output)  # check if necessary
        return final_Output

    def adc__battery2__temperature(self):
        self.i2c_bus.write16(0x00, 0x0E)  # ADC Channel 14
        time.sleep(2)
        Num = self.i2c_bus.readU16()  # Unit: mA
        final_Output = -0.163 * Num + 4.753
        print(final_Output)  # check if necessary
        return final_Output

    def status(self):
        self.i2c_bus.write16(0x01, 0x00) # Bit 0 Unknown Command Type
        time.sleep(2)
        feedback = self.i2c_bus.readU16()
        print(feedback)

        # parsing of response still needs to be done and tested!!
        '''
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
        '''

    def heater_on(self):
        self.i2c_bus.write16(0x05, 0x01)  # Bit 0 Unknown Command Type
        time.sleep(2)
        value = self.i2c_bus.readU16()
        print(value)
        # what does this command return

    def heater_off(self):
        self.i2c_bus.write16(0x05, 0x00)  # Bit 0 Unknown Command Type
        time.sleep(2)
        value = self.i2c_bus.readU16()
        print(value)
        # what does this command return

    def heater_status(self):
        self.i2c_bus.write16(0x06, 0x00)  # Bit 0 Unknown Command Type
        time.sleep(2)
        value = self.i2c_bus.readU16()
        if value == 0x0001:
            self.heater_status = 0
        elif value == 0x0000:
            self.heater_status = 1

    def watchdog(self):
        self.i2c_bus.write16(0x80, 0x00)  # Bit 0 Unknown Command Type
        time.sleep(2)
        # perfrom restart commands to re-initialize communication with EPS