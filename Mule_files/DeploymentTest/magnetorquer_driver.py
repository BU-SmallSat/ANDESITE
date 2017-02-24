from Adafruit_I2C import Adafruit_I2C
import time
import struct
import ctypes

'''
Throw error returns if the response from EPS is 0xF000
this means that the reading information is not yet ready to be read
or that the read was not preceeded by a write
'''

## any bytes that return 0xFF are empty/nonsense bytes and are considered the end of a response
## at least 1 millisecond delay between transfers

# all i2c operations should be done in a try catch loop in case acknowledgements are not received?

class magnetorquer_driver :
    def __init__(self):
        # create a second init function that can be called to reinitialize the magnetorquer board
        self.address = 0x10
        self.i2c_bus = Adafruit_I2C(self.address)
        self.heater_status = 1
        self.read_wait = .001

    def send_command(self, byteList, recvLength):
        # figure out how to do ack/nak handling with the adafruit i2c library?
        ## should everything be in a try catch loop?
        self.i2c_bus.writeList(byteList)
        time.sleep(self.read_wait)
        reply = self.i2c_bus.readList(recvLength)
        (cc, stat) = struct.unpack("bb", reply)
        # unpack stat
        if (stat & 0x0F) == 0:
            # accepted! return a successful transmission message - end of transmission
            return
        elif (stat & 0x0F) == 1:
            print("general rejection notice")
        else:
            # error handling for mistimed reading and writing operations
            return False
            # reply: 0x02, STAT
        return reply

    # operational commads
    def software_reset(self): ## ???????????????????
        # self.i2c_bus.write16(0xAA, 0xA5)  # ADC Channel 0
        # response is only available if the reset command was not accepted
        # restart scheme to re-initialize communication with iMTQ device
        pass

    def no_op(self):
        # best used to check communication without changing any operataion
        byteList = ctypes.create_string_buffer(1)
        struct.pack_into('b', byteList, 0, 2)
        reply = self.send_command(byteList, 2)
        (cc, stat) = struct.unpack("bb", reply)
        if cc == 2:
            # unpack stat
            if (stat & 0x0F) == 0:
                # accepted! return a successful transmission message - end of transmission
                return
            elif (stat & 0x0F) == 1:
                print("general rejection notice")

        else:
            # error handling for mistimed reading and writing operations
            return False
        # reply: 0x02, STAT

    def cancel_op(self):
        byteList = ctypes.create_string_buffer(1)
        struct.pack_into('b', byteList, 0, 3)
        reply = self.send_command(byteList, 2)
        # reply: 0x03, STAT

    def start_mtm(self):
        byteList = ctypes.create_string_buffer(1)
        struct.pack_into('b', byteList, 0, 4)
        reply = self.send_command(byteList, 2)
        # reply: 0x04, STAT

    def start_current(self, x_axis, y_axis, z_axis, duration):
        # convert current to 10-4 A
        # time in ms
        # pad axis and duration to be 2 bytes each
        byteList = ctypes.create_string_buffer(9)
        struct.pack_into('b', byteList, 0, 5)
        struct.pack_into('hhhh', byteList, 1, x_axis,y_axis,z_axis,duration)
        reply = self.send_command(byteList, 2)
        # reply: 0x05, STAT

    def start_dipole(self, x_axis, y_axis, z_axis, duration):
        # current in 10-4 A
        # time in ms
        # pad axis and duration to be 2 bytes each
        byteList = ctypes.create_string_buffer(9)
        struct.pack_into('b', byteList, 0, 6)
        struct.pack_into('hhhh', byteList, 1, x_axis,y_axis,z_axis,duration)
        reply = self.send_command(byteList, 2)
        # reply: 0x06, STAT

    def start_pwm(self, x_axis, y_axis, z_axis, duration):
        # .1% duty cycle
        # time in ms
        # pad axis and duration to be 2 bytes each
        byteList = ctypes.create_string_buffer(9)
        struct.pack_into('b', byteList, 0, 7)
        struct.pack_into('hhhh', byteList, 1, x_axis,y_axis,z_axis,duration)
        reply = self.send_command(byteList, 2)
        # reply: 0x07, STAT

    def start_test(self, axis):
        # pad axis to be 2 bytes
        byteList = ctypes.create_string_buffer(3)
        struct.pack_into('b', byteList, 0, 8)
        struct.pack_into('h', byteList, 1, axis)
        reply = self.send_command(byteList, 2)
        # reply: 0x08, STAT

    def start_bdot(self,duration):
        # pad duration to be 2 bytes
        byteList = ctypes.create_string_buffer(3)
        struct.pack_into('b', byteList, 0, 9)
        struct.pack_into('h', byteList, 1, duration)
        reply = self.send_command(byteList, 2)
        # reply: 0x09, STAT

    # data request commands
    def system_state(self):
        byteList = ctypes.create_string_buffer(1)
        struct.pack_into('b', byteList, 0, 65)
        reply = self.send_command(byteList, 9)
        # reply: 0x41, STAT, MODE, ERR, CONF, UPTIME

    def get_raw_mtm(self):
        byteList = ctypes.create_string_buffer(1)
        struct.pack_into('b', byteList, 0, 66)
        reply = self.send_command(byteList, 15)

    def get_cal_mtm(self):
        byteList = ctypes.create_string_buffer(1)
        struct.pack_into('b', byteList, 0, 67)
        reply = self.send_command(byteList, 15)

    def get_coil_current(self):
        byteList = ctypes.create_string_buffer(1)
        struct.pack_into('b', byteList, 0, 68)
        reply = self.send_command(byteList, 8)

    def get_coil_temp(self):
        byteList = ctypes.create_string_buffer(1)
        struct.pack_into('b', byteList, 0, 69)
        reply = self.send_command(byteList, 8)

    def get_dipole(self):
        byteList = ctypes.create_string_buffer(1)
        struct.pack_into('b', byteList, 0, 70)
        reply = self.send_command(byteList, 8)

    def get_self_test(self):
        byteList = ctypes.create_string_buffer(1)
        struct.pack_into('b', byteList, 0, 71)
        reply = self.send_command(byteList, 320)

    def get_detumble_data(self):
        byteList = ctypes.create_string_buffer(1)
        struct.pack_into('b', byteList, 0, 72)
        reply = self.send_command(byteList, 56)

    def get_raw_data(self):
        byteList = ctypes.create_string_buffer(1)
        struct.pack_into('b', byteList, 0, 73)
        reply = self.send_command(byteList, 24)

    def get_engineering_data(self):
        byteList = ctypes.create_string_buffer(1)
        struct.pack_into('b', byteList, 0, 74)
        reply = self.send_command(byteList, 24)


    # configuration comands
    def get_parameter(self, param_id):
        byteList = ctypes.create_string_buffer(3)
        struct.pack_into('b', byteList, 0, 129)
        struct.pack_into('h', byteList, 1, param_id)
        reply = self.send_command(byteList, 12)

    def set_parameter(self, param_id, value):
        byteList = ctypes.create_string_buffer(11)
        struct.pack_into('b', byteList, 0, 130)
        struct.pack_into('h', byteList, 1, param_id)
        struct.pack_into('d', byteList, 3, value)
        reply = self.send_command(byteList, 12)

    def reset_parameter(self, param_id):
        byteList = ctypes.create_string_buffer(3)
        struct.pack_into('b', byteList, 0, 131)
        struct.pack_into('h', byteList, 1, param_id)
        reply = self.send_command(byteList, 12)