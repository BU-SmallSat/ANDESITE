

from Adafruit_I2C import Adafruit_I2C
import time
import struct
from array import array
'''
Throw error returns if the response from EPS is 0xF000
this means that the reading information is not yet ready to be read
or that the read was not preceeded by a write
'''

## any bytes that return 0xFF are empty/nonsense bytes and are considered the end of a response
## at least 1 millisecond delay between transfers

# all i2c operations should be done in a try catch loop in case acknowledgements are not received?

GAIN_ID = [0xA0,0x00]
RATE_ID = [0x20, 0x00]

POS_X = 0x01
NEG_X = 0x02
POS_Y = 0x03
NEG_Y = 0x04
POS_Z = 0x05
NEG_Z = 0x06



THREE_TWENTY = 140  #Since 320 is more than one byte of data cannot send as response side, so encoding as 140 (hex representation of 320) and decoding on interface arduino

class magnetorquer_driver :
    def __init__(self):
        # create a second init function that can be called to reinitialize the magnetorquer board
        self.address = 0x20
        self.mag = 0x10
        self.i2c_bus = Adafruit_I2C(self.address,busnum=2,debug=False)
        self.debug = 1
        self.read_wait = 1
        self.settleTime = 2


    def send_command(self, byteList):
        # figure out how to do ack/nak handling with the adafruit i2c library?
        ## should everything be in a try catch loop?
        recvLength = byteList[0]
        self.i2c_bus.writeList(self.mag,byteList)
        time.sleep(self.read_wait)
        if(recvLength == 140):
            recvLength = 320
        reply = self.i2c_bus.readList(self.mag, recvLength)
        return reply

    def unpack_stat_reply(self,reply,command):
        (cc, stat) = reply
        #print(cc, stat)
        if cc == command:
            # unpack stat
            if stat == 128:
                # accepted! return a successful transmission message - end of transmission
                if self.debug == 1:
                    print("Accepted Command")
                return 1
            elif stat == 0:
                if self.debug == 1:
                    print("Accepted Command - Old Data")
                return 1
            else:
                if self.debug == 1:
                    print("general rejection notice: "+str(stat))
                return 0
        else:
            # error handling for mistimed reading and writing operations
            if self.debug == 1:
                print("Wrong CC recieved")
            return 0

    # operational commads
    def software_reset(self): ## ???????????????????
        # self.i2c_bus.write16(0xAA, 0xA5)  # ADC Channel 0
        # response is only available if the reset command was not accepted
        # restart scheme to re-initialize communication with iMTQ device
       	reply = self.send_command([2,0xAA, 0xA5])
        success = self.unpack_stat_reply(reply,2)

    def no_op(self):
        # best used to check communication without changing any operataion
        reply = self.send_command([2,0x02])
        success = self.unpack_stat_reply(reply,2)
        # reply: 0x02, STAT

    def cancel_op(self):
        reply = self.send_command([2,0x03])
        success = self.unpack_stat_reply(reply, 3)
        # reply: 0x03, STAT

    def start_mtm(self):
        reply = self.send_command([2,0x04])
        success = self.unpack_stat_reply(reply, 4)
        return success
        # reply: 0x04, STAT

    def start_current(self, x_axis, y_axis, z_axis, duration):
        # convert current to 10-4 A in hex
        # time in ms in hex
        # pad axis and duration to be 2 bytes each
        reply = self.send_command(
            [2,0x05, x_axis[1], x_axis[0], y_axis[1], y_axis[0], z_axis[1], z_axis[0], duration[1], duration[0]])
        success = self.unpack_stat_reply(reply, 5)
        # reply: 0x05, STAT

    def start_dipole(self, x_axis, y_axis, z_axis, duration):
        # current in 10-4 A in hex
        # time in ms in hex
        # pad axis and duration to be 2 bytes each
        reply = self.send_command(
            [2,0x06, x_axis[1], x_axis[0], y_axis[1], y_axis[0], z_axis[1], z_axis[0], duration[1], duration[0]])
        success = self.unpack_stat_reply(reply, 6)
        # reply: 0x06, STAT

    def start_pwm(self, x_axis, y_axis, z_axis, duration):
        # .1% duty cycle in hex
        # time in ms in hex
        # pad axis and duration to be 2 bytes each
        reply = self.send_command(
            [2,0x07, x_axis[1], x_axis[0], y_axis[1], y_axis[0], z_axis[1], z_axis[0], duration[1], duration[0]])
        success = self.unpack_stat_reply(reply, 7)
        # reply: 0x07, STAT

    def start_bdot(self,duration):
        # pad duration to be 2 bytes in hex
        reply = self.send_command([2,0x09,duration[1], duration[0]])
        success = self.unpack_stat_reply(reply, 9)
        # reply: 0x09, STAT

    # data request commands
    def system_state(self):
        reply = self.send_command([9,0x41])
        success = self.unpack_stat_reply(reply[0:2], 0x41)
        [mode,err,conf,uptime] = [reply[2],reply[3],reply[4],reply[5:9]]
        if self.debug == 1:
            if mode == 0:
                print("iMTQ is in IDLE state")
            elif mode == 1:
                print("iMTQ is in SELFTEST state")
            elif mode == 2:
                print ("iMTQ is in DETUMBLE state")
        print(reply)
        # reply: 0x41, STAT, MODE, ERR, CONF, UPTIME

    def get_raw_mtm(self):
        reply = self.send_command([15,0x42])
        success = self.unpack_stat_reply(reply[0:2], 0x42)
        [rawx,rawy,rawz] = self.convert3axis_int32(reply[2:14])
        ##NEED TO ADD MULTIPLIER FOR RAW CALCULATIONS
        act = reply[14]
        if self.debug == 1:
            print("raw MTM x-axis (T): " + str(rawx))
            print("raw MTM y-axis (T): " + str(rawy))
            print("raw MTM z-axis (T): " + str(rawz))
            if act == 1:
                print("Torque coils are currentling actuating")
            else:
                print("No activity on Torque coils")
        return [success, rawx, rawy, rawz]


    def get_cal_mtm(self):
        reply = self.send_command([15,0x43])
        success = self.unpack_stat_reply(reply[0:2], 0x43)
        [calx,caly,calz] = self.convert3axis_int32(reply[2:14])
        act = reply[14]
        if self.debug == 1:
            print("calibrated MTM x-axis (nT): " + str(calx))
            print("calibrated MTM y-axis (nT): " + str(caly))
            print("calibrated MTM z-axis (nT): " + str(calz))
            if act == 1:
                print("Torque coils are currentling actuating")
            else:
                print("No activity on Torque coils")
        return [success, calx, caly, calz]

    def get_coil_current(self):
        reply = self.send_command([8,0x44])
        success = self.unpack_stat_reply(reply[0:2], 0x44)
        [curx,cury,curz] = self.convert3axis_int16(reply[2:8])
        ##NEED TOO ADD MULTIPLIER (10 ** -4)
        if self.debug == 1:
            print("coil current x-axis: " + str(curx))
            print("coil current y-axis: " + str(cury))
            print("coil current z-axis: " + str(curz))

    def get_coil_temp(self):
        reply = self.send_command([8,0x45])
        success = self.unpack_stat_reply(reply[0:2], 0x45)
        [tempx,tempy,tempz] = reply[2:8]
        if self.debug == 1:
            print("coil temp x-axis: " + str(tempx))
            print("coil temp y-axis: " + str(tempy))
            print("coil temp z-axis: " + str(tempz))

    def get_dipole(self):
        reply = self.send_command([8,0x46])
        success = self.unpack_stat_reply(reply[0:2], 0x46)
        [dipx,dipy,dipz] = self.convert3axis_int16(reply[2:8])
        if self.debug == 1:
            print("coil dipole x-axis: " + str(dipx))
            print("coil dipole y-axis: " + str(dipy))
            print("coil dipole z-axis: " + str(dipz))



    def self_test(self, axis):
        # pad axis to be 2 bytes in hex
        if(axis == 0):
            reply = self.send_command([2,0x08, POS_X])
            success = self.unpack_stat_reply(reply, 8)
            time.sleep(10)  ##NEED TO DETERMINE HOW LONG THIS TEST ACTUALLY NEEDS TO WAIT FOR
            if self.debug ==1:
                print("********** POSITIVE X-AXIS SELF TEST*********")
            self.get_self_test(POS_X)

            reply = self.send_command([2,0x08, NEG_X])
            success = self.unpack_stat_reply(reply, 8)
            time.sleep(10)  ##NEED TO DETERMINE HOW LONG THIS TEST ACTUALLY NEEDS TO WAIT FOR
            if self.debug ==1:
                print("**********NEGATIVE X-AXIS SELF TEST*********")
            self.get_self_test(NEG_X)

            reply = self.send_command([2,0x08, POS_Y])
            success = self.unpack_stat_reply(reply, 8)
            time.sleep(10)  ##NEED TO DETERMINE HOW LONG THIS TEST ACTUALLY NEEDS TO WAIT FOR
            if self.debug ==1:
                print("**********POSITIVE Y-AXIS SELF TEST*********")
            self.get_self_test(POS_Y)

            reply = self.send_command([2,0x08, NEG_Y])
            success = self.unpack_stat_reply(reply, 8)
            time.sleep(10)  ##NEED TO DETERMINE HOW LONG THIS TEST ACTUALLY NEEDS TO WAIT FOR
            if self.debug ==1:
                print("**********NEGATIVE Y-AXIS SELF TEST*********")
            self.get_self_test(NEG_Y)

            reply = self.send_command([2,0x08, POS_Z])
            success = self.unpack_stat_reply(reply, 8)
            time.sleep(10)  ##NEED TO DETERMINE HOW LONG THIS TEST ACTUALLY NEEDS TO WAIT FOR
            if self.debug ==1:
                print("**********POSITIVE Z-AXIS SELF TEST*********")
            self.get_self_test(POS_Z)

            reply = self.send_command([2,0x08, NEG_Z])
            success = self.unpack_stat_reply(reply, 8)
            time.sleep(10)  ##NEED TO DETERMINE HOW LONG THIS TEST ACTUALLY NEEDS TO WAIT FOR
            if self.debug ==1:
                print("**********NEGATIVE Z-AXIS SELF TEST*********")
            self.get_self_test(NEG_Z)

        else:
            reply = self.send_command([2,0x08, axis])
            success = self.unpack_stat_reply(reply, 8)
            time.sleep(10)  ##NEED TO DETERMINE HOW LONG THIS TEST ACTUALLY NEEDS TO WAIT FOR
            if self.debug ==1:
                print("**********SELF TEST*********")
            self.get_self_test(axis)
        # reply: 0x08, STAT


    def get_self_test(self,axis):
        if axis == 0:
            '''
            reply = self.send_command([120, 0x47])
            steps = [reply[0:40], reply[40:80], reply[80:120]]
            reply = self.send_command([320,0x47])
            steps = [reply[0:40],reply[40:80],reply[80:120],reply[120:160],
                     reply[160:200],reply[200:240],reply[240:280],reply[280:320]]
            '''
            print("ERR:: PROBLEM WITH READING ALL AXES ON iMTQ")
        else:
            reply = self.send_command([120,0x47])
            steps = [reply[0:40], reply[40:80], reply[80:120]]
        for step in steps:
            success = self.unpack_stat_reply(step[0:2], 0x47)
            ERR_List = step[2]
            STEP_List = step[3]
            [rawx,rawy,rawz] = self.convert3axis_int32(reply[4:16])
            if self.debug == 1:
                print("raw MTM x-axis (T): " + str(rawx))
                print("raw MTM y-axis (T): " + str(rawy))
                print("raw MTM z-axis (T): " + str(rawz))
            [calx,caly,calz] = self.convert3axis_int32(reply[16:28])
            if self.debug == 1:
                print("calibrated MTM x-axis (nT): " + str(calx))
                print("calibrated MTM y-axis (nT): " + str(caly))
                print("calibrated MTM z-axis (nT): " + str(calz))
            [curx,cury,curz] = self.convert3axis_int32(reply[28:34])
            if self.debug == 1:
                print("coil current x-axis: " + str(curx))
                print("coil current y-axis: " + str(cury))
                print("coil current z-axis: " + str(curz))
            [tempx,tempy,tempz] = self.convert3axis_int16(reply[34:40])
            if self.debug == 1:
                print("coil temp x-axis: " + str(tempx))
                print("coil temp y-axis: " + str(tempy))
                print("coil temp z-axis: " + str(tempz))
        #print(reply)

    def get_detumble_data(self):
        reply = self.send_command([56,0x48])
        success = self.unpack_stat_reply(reply[0:2], 0x48)
        print(reply)

    def get_raw_data(self):
        reply = self.send_command([24,0x49])
        success = self.unpack_stat_reply(reply[0:2], 0x49)
        print(reply)

    def get_engineering_data(self):
        reply = self.send_command([24,0x4A])
        success = self.unpack_stat_reply(reply[0:2], 0x4A)
        print(reply)

    def set_parameter(self, param_id, value,size):
        if(size == 1):
            reply = self.send_command([5,0x82,param_id[1],param_id[0],value])
        elif(size == 2):
            reply = self.send_command([6,0x82, param_id[1],param_id[0],value[1],value[0]])
        elif(size == 4):
            reply = self.send_command([8,0x82,param_id[1],param_id[0],value[3],value[2],value[1],value[0]])
        elif(size == 8):
            reply = self.send_command([12,0x82,param_id[1],param_id[0],value[7],value[6],value[5],value[4],value[3],value[2],value[1],value[0]])
            success = self.unpack_stat_reply(reply[0:2], 0x82)
        print(reply)

    # configuration comands
    def get_parameter(self, param_id,size):
        rspLen = size+4
        reply = self.send_command([rspLen,0x81,param_id[1],param_id[0]])
        success = self.unpack_stat_reply(reply[0:2], 0x81)
        print(reply)

    def reset_parameter(self, param_id,size):
        rspLen = size + 4
        reply = self.send_command([rspLen,0x83,param_id[1],param_id[0]])
        success = self.unpack_stat_reply(reply[0:2], 0x83)
        print(reply)

    def setup_ADCS_param(self):
        gain_value = [0xC1,0x2E,0x84,0x80,0x00,0x00,0x00,0x00] #double precison floating point representation of -1E6
        rate_value = 0x08
        self.get_parameter(GAIN_ID,8)
        self.get_parameter(RATE_ID,1)
        self.set_parameter(RATE_ID, rate_value, 1)
        self.set_parameter(GAIN_ID, gain_value, 8)
        self.get_parameter(RATE_ID,1)
        self.get_parameter(GAIN_ID,8)

    def TwoOrbitBDOT(self):
        self.setup_ADCS_param()
        self.start_bdot([0x27, 0x10])

    def pointing_mode(self):
        self.start_dipole([0x00, 0x00], [0x03, 0xE8], [0x00, 0x00], [0x27, 0x10])


    def convert3axis_int32(self,response):
        X_List = response[0:4]
        bytes = array('B', X_List)
        x = (struct.unpack('<i', bytes)[0])
        Y_List = response[4:8]
        bytes = array('B', Y_List)
        y = (struct.unpack('<i', bytes)[0])
        Z_List = response[8:12]
        bytes = array('B',Z_List)
        z = (struct.unpack('<i', bytes)[0])
        return x,y,z

    def convert3axis_int16(self,response):
        X_List = response[0:2]
        bytes = array('B', X_List)
        x = (struct.unpack('<h', bytes)[0])
        Y_List = response[2:4]
        bytes = array('B', Y_List)
        y = (struct.unpack('<h', bytes)[0])
        Z_List = response[4:6]
        bytes = array('B', Z_List)
        z = (struct.unpack('<h', bytes)[0])
        return [x,y,z]
