

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
        self.address = 0x20
        self.mag = 0x10
        self.i2c_bus = Adafruit_I2C(self.address, bus_num=1)#,debug=True)
        self.heater_status = 1
        self.read_wait = .001
        self.settleTime = 2


    def send_command(self, byteList, recvLength):
        # figure out how to do ack/nak handling with the adafruit i2c library?
        ## should everything be in a try catch loop?
        self.i2c_bus.writeList(self.mag,byteList)
        time.sleep(self.read_wait)
        reply = self.i2c_bus.readList(self.mag, recvLength)
        return reply

    def unpack_stat_reply(self,reply,command):
        (cc, stat) = reply
        #print(cc, stat)
        if cc == command:
            # unpack stat
            if stat == 128:
                # accepted! return a successful transmission message - end of transmission
                return True
            else:
                print("general rejection notice")
                return False
        else:
            # error handling for mistimed reading and writing operations
            #print("Wrong CC recieved")
            return False

    # operational commads
    def software_reset(self): ## ???????????????????
        # self.i2c_bus.write16(0xAA, 0xA5)  # ADC Channel 0
        # response is only available if the reset command was not accepted
        # restart scheme to re-initialize communication with iMTQ device
       	reply = self.send_command([0xAA, 0xA5],2)
        self.unpack_stat_reply(reply,2)

    def no_op(self):
        # best used to check communication without changing any operataion
        reply = self.send_command([0x02], 2)
        self.unpack_stat_reply(reply,2)
        # reply: 0x02, STAT

    def cancel_op(self):
        reply = self.send_command([0x03], 2)
        self.unpack_stat_reply(reply, 3)
        # reply: 0x03, STAT

    def start_mtm(self):
        reply = self.send_command([0x04], 2)
        self.unpack_stat_reply(reply, 4)
        # reply: 0x04, STAT

    def start_current(self, x_axis, y_axis, z_axis, duration):
        # convert current to 10-4 A in hex
        # time in ms in hex
        # pad axis and duration to be 2 bytes each
        reply = self.send_command(
            [0x05, x_axis[1], x_axis[0], y_axis[1], y_axis[0], z_axis[1], z_axis[0], duration[1], duration[0]], 2)
        self.unpack_stat_reply(reply, 5)
        # reply: 0x05, STAT

    def start_dipole(self, x_axis, y_axis, z_axis, duration):
        # current in 10-4 A in hex
        # time in ms in hex
        # pad axis and duration to be 2 bytes each
        reply = self.send_command(
            [0x06, x_axis[1], x_axis[0], y_axis[1], y_axis[0], z_axis[1], z_axis[0], duration[1], duration[0]], 2)
        self.unpack_stat_reply(reply, 6)
        # reply: 0x06, STAT

    def start_pwm(self, x_axis, y_axis, z_axis, duration):
        # .1% duty cycle in hex
        # time in ms in hex
        # pad axis and duration to be 2 bytes each
        reply = self.send_command(
            [0x07, x_axis[1], x_axis[0], y_axis[1], y_axis[0], z_axis[1], z_axis[0], duration[1], duration[0]], 2)
        self.unpack_stat_reply(reply, 7)
        # reply: 0x07, STAT

    def start_test(self, axis):
        # pad axis to be 2 bytes in hex
        reply = self.send_command([0x08], axis, 2)
        self.unpack_stat_reply(reply, 8)
        # reply: 0x08, STAT

    def start_bdot(self,duration):
        # pad duration to be 2 bytes in hex
        reply = self.send_command([0x09,duration[1], duration[0]], 2)
        self.unpack_stat_reply(reply, 9)
        # reply: 0x09, STAT

    # data request commands
    def system_state(self):
        reply = self.send_command([0x41], 9)
        self.unpack_stat_reply(reply[0:2], 0x41)
        print(reply)
        # reply: 0x41, STAT, MODE, ERR, CONF, UPTIME

    def get_raw_mtm(self):
        reply = self.send_command([0x42], 15)
        self.unpack_stat_reply(reply[0:2], 0x42)
        print(reply)

    def get_cal_mtm(self):
        reply = self.send_command([0x43], 15)
        self.unpack_stat_reply(reply[0:2], 0x43)
        print(reply)

    def get_coil_current(self):
        reply = self.send_command([0x44], 8)
        self.unpack_stat_reply(reply[0:2], 0x44)
        print(reply)

    def get_coil_temp(self):
        reply = self.send_command([0x45], 8)
        self.unpack_stat_reply(reply[0:2], 0x45)
        print(reply)

    def get_dipole(self):
        reply = self.send_command([0x46], 8)
        self.unpack_stat_reply(reply[0:2], 0x46)
        print(reply)

    def get_self_test(self):
        reply = self.send_command([0x47], 320)
        self.unpack_stat_reply(reply[0:2], 0x47)
        print(reply)

    def get_detumble_data(self):
        reply = self.send_command([0x48], 56)
        self.unpack_stat_reply(reply[0:2], 0x48)
        print(reply)

    def get_raw_data(self):
        reply = self.send_command([0x49], 24)
        self.unpack_stat_reply(reply[0:2], 0x49)
        print(reply)

    def get_engineering_data(self):
        reply = self.send_command([0x4A], 24)
        self.unpack_stat_reply(reply[0:2], 0x4A)
        print(reply)

    def set_parameter(self, param_id, value,size):
        if(size == 1):
            reply = self.send_command([0x82,param_id[1],param_id[0],value], 5)
        elif(size == 2):
            reply = self.send_command([0x82, param_id[1],param_id[0],value[1],value[0]], 6)
        elif(size == 4):
            reply = self.send_command([0x82,param_id[1],param_id[0],value[3],value[2],value[1],value[0]], 8)
        elif(size == 8):
            reply = self.send_command([0x82,param_id[1],param_id[0],value[7],value[6],value[5],value[4],value[3],value[2],value[1],value[0]], 12)
        self.unpack_stat_reply(reply[0:2], 0x82)
        print(reply)

    # configuration comands
    def get_parameter(self, param_id,size):
        reply = self.send_command([0x81,param_id[1],param_id[0]], size+4)
        self.unpack_stat_reply(reply[0:2], 0x81)
        print(reply)

    def reset_parameter(self, param_id,size):
        reply = self.send_command([0x83,param_id[1],param_id[0]], size+4)
        self.unpack_stat_reply(reply[0:2], 0x83)
        print(reply)

    def BDOT_detumble(self):
        gain_id = [0xA0,0x00]
        gain_value = [0xC1,0x2E,0x84,0x80,0x00,0x00,0x00,0x00]
        #gain_value = [0xC0,0xC3,0x88,0x00,0x00,0x00,0x00,0x00]
        rate_id = [0x20,0x00]
        rate_value = 0x08
        #self.get_parameter(gain_id,8)
        #self.get_parameter(rate_id,1)
        self.set_parameter(rate_id,rate_value,1)
        self.set_parameter(gain_id,gain_value,8)
        self.start_bdot([0x27, 0x10])
        #self.get_parameter(rate_id,1)
        #self.get_parameter(gain_id,8)

    def pointing_mode(self):
        self.start_dipole([0x00, 0x00], [0x03, 0xE8], [0x00, 0x00], [0x27, 0x10])
