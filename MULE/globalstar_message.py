from __future__ import print_function

import crc16


class GlobalStarMessage:
    def __init__(self, esn=None, filename=None, payload=None):
        self.__esn = bytearray(esn or b'00000000')
        self.__file_name_size = 0
        self.__file_name = bytearray(filename or '')
        self.__data_bytes = 0
        self.__payload = bytearray(payload or '')
        self.__crc = b'\0x00\0x00'

    @classmethod
    def from_bytearray(cls, buffer):
        calculated_crc = bytearray(crc16.crc16xmodem(buffer[0:len(buffer) - 2]))
        esn = buffer[:8]
        file_name_size = int(buffer[8: 11])
        file_name_start = 11
        file_name = []
        if file_name_size > 0:
            file_name = buffer[file_name_start: file_name_start + file_name_size]
        payload_size_start = file_name_start + file_name_size
        payload_size = int(buffer[payload_size_start: payload_size_start + 6])
        payload_start = payload_size_start + 6
        payload = buffer[payload_start: payload_start + payload_size]
        crc_start = payload_start + payload_size
        assert crc_start == len(buffer) - 2
        received_crc = buffer[-2:]
        assert received_crc[0] == calculated_crc[0] and received_crc[1] == calculated_crc[1]
        return cls(esn, file_name, payload)

    def to_bytearray(cls, fileNameSize, fileName, fileSize):
        pass

    @property
    def esn(self):
        return self.__esn

    @esn.setter
    def esn(self, esn):
        assert(len(esn) == 8)