import pexpect, os
from datetime import datetime
from time import sleep

ADCPATH = './ADC.elf'

MAG_MEAS = 'mag_meas'
EULER_ANGLE = 'euler_angle'
SUN_MEASURE = 'sun_meas'
EPOCH = 'epoch'
LLA = 'lla'
S_FLAG = 's_flag'

class ADCRunner:
    @staticmethod
    def date2num()->[]:
        """
        Utility function which returns the current time in the correct format for passing to ADC.elf
        """
        now = datetime.utcnow()
        return [now.year,now.month,now.day,now.hour,now.minute,now.second]


    def process_output(self,values: str) -> ():
        """
        Processes the response of ADC.elf and returns a tuple containing the magnetic moment and quaternion estimates.
        :param values:
        :return:
        """
        floats = [float(f) for f in values.split(',') if values and f]
        # if len(floats) == 7:
        mag = (floats[0], floats[1], floats[2])
        quat = (floats[3], floats[4], floats[5], floats[6])
        return (mag, quat)

    def format_input(self,invals: {}) -> bytes:
        """
        Takes measurements of magnetic moment, euler angle, sun position, epoch, lla, and sun sensor status and prepares them to be passed to ADC.elf.
        :param invals:
        :return:
        """
        concat = lambda s, f: ''.join([f % x for x in s])
        retval = ''
        retval += concat(invals[MAG_MEAS], '%3.2f,')
        retval += concat(invals[EULER_ANGLE], '%3.2f,')
        retval += concat(invals[SUN_MEASURE], '%3.2f,')
        retval += concat(invals[EPOCH], '%02.0f,')
        retval += concat(invals[LLA], '%3.2f,')
        retval += concat([invals[S_FLAG]], '%1.0f,')
        retval = retval[:-1] #remove the trailing comma
        retval += os.linesep
        return retval.encode('utf-8')

    def page(self,input: {}) -> ():
        """
        Gives ADC.elf the current set of sensor measurements and returns the estimated quaternion and magnetic moment vector.
        :param input:
        :return:
        """
        formatted = self.format_input(input)
        self.pid.write(formatted)
        response = self.pid.readline()

        return self.process_output(response.decode())

    def __init__(self):
        """Begins the long-running process ADC.elf"""
        self.pid = pexpect.spawn(ADCPATH)
        self.pid.setecho(False)

    def exit(self):
        """
        Sends SIGTERM to ADC.elf.
        :return:
        """
        self.pid.terminate()



if __name__ == "__main__":
    """  Test the binary here.   This should present bogus data 100 times and then exit quietly."""
    testData = {'mag_meas': [1, 2, 3],
                'euler_angle': [4, 5, 6],
                'sun_meas': [7, 8, 9],
                'epoch': [0, 1, 2, 3, 4, 5],
                'lla': [6, 7, 8],
                's_flag': 9
                }
    runner = ADCRunner()
    i = 0
    while i < 100:
        result = runner.page(testData)
        print(result)
        i += 1
        sleep(.1)



    runner.exit()