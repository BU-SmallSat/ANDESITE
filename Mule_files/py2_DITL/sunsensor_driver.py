from Adafruit_I2C import Adafruit_I2C


class sunsensor_driver :
    def __init__(self):
        # create a second init function that can be called to reinitialize the magnetorquer board
        self.address = 0x77
        self.i2c_bus = Adafruit_I2C(self.address,busnum=2)#,debug=True)
        self.data_size = 6

    def read_data(self):
        reply = self.i2c_bus.readList(0xFF,self.data_size)
        az = (((reply&0x00000000FF00)>>8) * 256 + (reply&0x0000000000FF)) - 360
        el = (((reply&0x0000FF000000)>>24) * 256 + ((reply&0x000000FF0000)>>16)) - 360
        con = ((reply&0xFF0000000000)>>40) * 256 + ((reply&0x00FF00000000)>>32)
        return [az,el,con]
