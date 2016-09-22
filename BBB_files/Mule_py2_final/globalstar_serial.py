import serial
import logging
import struct
import time
from globalstar_response import GlobalStarResponse
from globalstar_message import GlobalStarMessage


## enable pin for serial connection!


SYN1=chr(0x47).encode("utf-8")
SYN2=chr(0x55).encode("utf-8")
NAK=chr(0x0F).encode("utf-8")
ACK=chr(0x06).encode("utf-8")
LEN9 = (chr(0x00)+chr(0x00)+chr(0x00)+chr(0x09)).encode("utf-8")
LEN44 = (chr(0x00)+chr(0x00)+chr(0x00)+chr(0x2C)).encode("utf-8")

class GlobalStarSerial:
    def __init__(self):
        self.__serial = serial.Serial(
            port='/dev/ttyO1',
            baudrate=38400,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            xonxoff=False,
            rtscts=False,
            dsrdtr=False,
            timeout=1,
            write_timeout=None,
            interCharTimeout=None
        )
        self.parser = GlobalStarResponse()
        self.__logger = logging.getLogger('GlobalStarSerial')
        self.__serial.flushOutput()
        self.__serial.flushInput()

    def send(self, message):
        self.__serial.write(message)
        self.__serial.timeout = 0.1
        result = self.__serial.read(3)
        response = self.parser.parse(result)
        if response == self.parser.error:
            if len(result) == 0:
                self.__logger.error('Timeout on send')
                # todo: Handle this timeout better - maybe retry it or something?
            else:
                self.__logger.error(
                    'Malformed response from GlobalStar radio. Expected ACK or NAK, got [0x{0:x} 0x{1:x} 0x{2:x}]'.format(
                        response[0], response[1], response[2]))
        return response

    def receive_message(self):
        sync = self.__serial.read(2)
        if sync[0] != 0x47 or sync[1] != 0x55:
            return self.parser.error
        esn = self.__serial.read(8)
        file_name_size_buffer = self.__serial.read(3)
        file_name_size = int(file_name_size_buffer)
        data_byte_count_buffer = self.__serial.read(6)
        data_byte_count = int(data_byte_count_buffer)
        payload_buffer = self.__serial.read(data_byte_count)
        crc_buffer = self.__serial.read(2)
        return GlobalStarMessage.from_bytearray(
            bytearray(source=[esn, file_name_size_buffer, data_byte_count_buffer, payload_buffer, crc_buffer]))

    def poll_health(self, timeOut, readInterval):
        packet = SYN1+SYN2+LEN9+"PC401".encode('UTF-8')
        print "Writing message to globalstar and waiting for response"
        startTime = time.time()
        waitTime = 0
        self.__serial.flushInput()
        self.__serial.write(packet)
        while self.__serial.inWaiting() == 0 and waitTime < timeOut:
            time.sleep(readInterval)
            waitTime += readInterval
        incoming = bytearray()
        while self.__serial.inWaiting() > 0:
            incoming.extend(self.__serial.read(self.__serial.inWaiting()))
            time.sleep(readInterval)
        # print("response was: " + incoming)
        if len(incoming) >= 11:
            response = self.parser.parseACK(incoming)
            if response == self.parser.error:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
            elif response == self.parser.ack and (incoming) > 11:
                healthInfo = self.parser.parseHealthPoll(incoming[11:])
        else:
            if len(incoming) == 0:
                print('Timeout on send')
                # todo: Handle this timeout better - maybe retry it or something?
            else:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)


    def poll_files_awaiting_download(self):
        response = self.send(bytearray('PC403'))
        if response == self.parser.ack:
            file_count = struct.unpack('>I', self.__serial.read(4))
        else:
            self.__logger.error('poll_files_awaiting_download (PC403) got a NAK')

    def poll_messages_awaiting_upload(self):
        response = self.send(bytearray('PC405'))
        if response == self.parser.ack:
            message_count = struct.unpack('>I', self.__serial.read(4))
        else:
            self.__logger.error('poll_messages_awaiting_upload (PC405) got a NAK')

    def poll_files_awaiting_upload(self):
        response = self.send(bytearray('PC407'))
        if response == self.parser.ack:
            file_count = struct.unpack('>I', self.__serial.read(4))
        else:
            self.__logger.error('poll_files_awaiting_upload (PC407) got a NAK')

    def clear_uplink_messages(self):
        response = self.send(bytearray('PC500'))
        if response == self.parser.nak:
            self.__logger.error('clear_uplink_messages (PC500) got a NAK')

    def clear_uplink_files(self):
        response = self.send(bytearray('PC502'))
        if response == self.parser.nak:
            self.__logger.error('clear_uplink_files (PC502) got a NAK')

    def clear_downlink_files(self):
        response = self.send(bytearray('PC504'))
        if response == self.parser.nak:
            self.__logger.error('clear_downlink_files (PC504) got a NAK')