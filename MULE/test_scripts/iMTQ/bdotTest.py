from magnetorquer_driver import magnetorquer_driver
import time


mag = magnetorquer_driver()

print("****STARTING BDOT TEST****")
#mag.software_reset()
#mag.cancel_op()
#mag.no_op()
#mag.no_op()
mag.set_BDOT_TEST()
#time.sleep(2)
mag.start_bdot([0x27,0x10])
time.sleep(10)
#mag.reset_parameter([0xA0,00],8)
#mag.set_BDOT_TEST()
#mag.no_op()
#time.sleep(1)
#mag.no_op()

#mag.cancel_op()
def main():
	while(1):
		mag.no_op()
		time.sleep(30)

'''
while(1):
	mag.start_dipole([0x07,0xD0],[0x00,0x00],[0x00,0x00],[0x00,0x64])
	time.sleep(.5)
	mag.start_dipole([0x05,0xDC],[0x00,0x00],[0x00,0x00],[0x00,0x64])
	time.sleep(.5)
	mag.start_dipole([0x03,0xE8],[0x00,0x00],[0x00,0x00],[0x00,0x64])
	time.sleep(.5)
	mag.start_dipole([0x01,0xF4],[0x00,0x00],[0x00,0x00],[0x00,0x64])
	time.sleep(1)
	mag.start_dipole([0xFE,0x0C],[0x00,0x00],[0x00,0x00],[0x00,0x64])
	time.sleep(.5)
	mag.start_dipole([0xFC,0x18],[0x00,0x00],[0x00,0x00],[0x00,0x64])
	time.sleep(.5)
	mag.start_dipole([0xFA,0x24],[0x00,0x00],[0x00,0x00],[0x00,0x64])
        time.sleep(.5)
	mag.start_dipole([0xF8,0x30],[0x00,0x00],[0x00,0x00],[0x00,0x64])
        time.sleep(1)
        mag.start_dipole([0x00,0x00],[0x07,0xD0],[0x00,0x00],[0x00,0x64])
        time.sleep(.5)
        mag.start_dipole([0x00,0x00],[0x05,0xDC],[0x00,0x00],[0x00,0x64])
        time.sleep(.5)
        mag.start_dipole([0x00,0x00],[0x03,0xE8],[0x00,0x00],[0x00,0x64])
        time.sleep(.5)
        mag.start_dipole([0x00,0x00],[0x01,0xF4],[0x00,0x00],[0x00,0x64])
        time.sleep(1)
        mag.start_dipole([0x00,0x00],[0xFE,0x0C],[0x00,0x00],[0x00,0x64])
        time.sleep(.5)
        mag.start_dipole([0x00,0x00],[0xFC,0x18],[0x00,0x00],[0x00,0x64])
        time.sleep(.5)
        mag.start_dipole([0x00,0x00],[0xFA,0x24],[0x00,0x00],[0x00,0x64])
        time.sleep(.5)
        mag.start_dipole([0x00,0x00],[0xF8,0x30],[0x00,0x00],[0x00,0x64])
        time.sleep(1)
        mag.start_dipole([0x00,0x00],[0x00,0x00],[0x07,0xD0],[0x00,0x64])
        time.sleep(.5)
        mag.start_dipole([0x00,0x00],[0x00,0x00],[0x05,0xDC],[0x00,0x64])
        time.sleep(.5)
        mag.start_dipole([0x00,0x00],[0x00,0x00],[0x03,0xE8],[0x00,0x64])
        time.sleep(.5)
        mag.start_dipole([0x00,0x00],[0x00,0x00],[0x01,0xF4],[0x00,0x64])
        time.sleep(1)
        mag.start_dipole([0x00,0x00],[0x00,0x00],[0xFE,0x0C],[0x00,0x64])
        time.sleep(.5)
        mag.start_dipole([0x00,0x00],[0x00,0x00],[0xFC,0x18],[0x00,0x64])
        time.sleep(.5)
        mag.start_dipole([0x00,0x00],[0x00,0x00],[0xFA,0x24],[0x00,0x64])
        time.sleep(.5)
        mag.start_dipole([0x00,0x00],[0x00,0x00],[0xF8,0x30],[0x00,0x64])
        time.sleep(1)
'''

if __name__ == '__main__':
	try:
		main()
	except KeyboardInterrupt:
		mag.cancel_op()
	print("done with self test")

