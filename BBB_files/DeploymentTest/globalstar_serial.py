import serial
import logging
import struct
from globalstar_response import GlobalStarResponse
from globalstar_message import GlobalStarMessage


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
            timeout=0.5,
            write_timeout=0.5
        )
        self.__logger = logging.getLogger('GlobalStarSerial')

    def send(self, message):
        self.__serial.write(bytearray[0x47, 0x55, message])
        self.__serial.timeout = 0.1
        result = self.__serial.read(3)
        response = GlobalStarResponse.parse(result)
        if response == GlobalStarResponse.error:
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
            return GlobalStarResponse.error
        esn = self.__serial.read(8)
        file_name_size_buffer = self.__serial.read(3)
        file_name_size = int(file_name_size_buffer)
        data_byte_count_buffer = self.__serial.read(6)
        data_byte_count = int(data_byte_count_buffer)
        payload_buffer = self.__serial.read(data_byte_count)
        crc_buffer = self.__serial.read(2)
        return GlobalStarMessage.from_bytearray(
            bytearray(source=[esn, file_name_size_buffer, data_byte_count_buffer, payload_buffer, crc_buffer]))

    def poll_health(self):
        response = self.send(bytearray('PC401'))
        if response == GlobalStarResponse.ack:
            reset_number = struct.unpack('>I', self.__serial.read(4))
            rssi = struct.unpack('>B', self.__serial.read(1))
            connection_status = struct.unpack('>B', self.__serial.read(1))
            gateway = struct.unpack('>B', self.__serial.read(1))
            last_contact_time = struct.unpack('>I', self.__serial.read(4))
            last_attempt_time = struct.unpack('>I', self.__serial.read(4))
            failed_opcode_count = struct.unpack('>I', self.__serial.read(4))
        else:
            self.__logger.error('poll_health (PC401) got a NAK')

    def poll_files_awaiting_download(self):
        response = self.send(bytearray('PC403'))
        if response == GlobalStarResponse.ack:
            file_count = struct.unpack('>I', self.__serial.read(4))
        else:
            self.__logger.error('poll_files_awaiting_download (PC403) got a NAK')

    def poll_messages_awaiting_upload(self):
        response = self.send(bytearray('PC405'))
        if response == GlobalStarResponse.ack:
            message_count = struct.unpack('>I', self.__serial.read(4))
        else:
            self.__logger.error('poll_messages_awaiting_upload (PC405) got a NAK')

    def poll_files_awaiting_upload(self):
        response = self.send(bytearray('PC407'))
        if response == GlobalStarResponse.ack:
            file_count = struct.unpack('>I', self.__serial.read(4))
        else:
            self.__logger.error('poll_files_awaiting_upload (PC407) got a NAK')

    def clear_uplink_messages(self):
        response = self.send(bytearray('PC500'))
        if response == GlobalStarResponse.nak:
            self.__logger.error('clear_uplink_messages (PC500) got a NAK')

    def clear_uplink_files(self):
        response = self.send(bytearray('PC502'))
        if response == GlobalStarResponse.nak:
            self.__logger.error('clear_uplink_files (PC502) got a NAK')

    def clear_downlink_files(self):
        response = self.send(bytearray('PC504'))
        if response == GlobalStarResponse.nak:
            self.__logger.error('clear_downlink_files (PC504) got a NAK')



