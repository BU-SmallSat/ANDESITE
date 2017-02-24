import time
import struct
import ctypes
from MPU9250 import MPU9250

#from IMU_Driver import IMU_driver
#imu = IMU_driver()

mpu9250 = MPU9250()

while 1 :

    gyro = mpu9250.readGyro()
    accel = mpu9250.readAccel()
    print "Gyro: ", [gyro[0], gyro[1], gyro[2]]
    print "Accel: ", [accel[0], accel[1], accel[2]],'\n\n'
    time.sleep(0.5)
