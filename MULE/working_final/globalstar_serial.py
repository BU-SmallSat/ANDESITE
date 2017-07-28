import serial
import logging
import struct
import time
from globalstar_response import GlobalStarResponse
from globalstar_message import GlobalStarMessage
import crc16


## enable pin for serial connection!

dataFiles = "/home/debian/Brent/BBB_files/Mule_py2_final/textFiles/"

SYN1=chr(0x47).encode("utf-8")
SYN2=chr(0x55).encode("utf-8")
NAK=chr(0x0F).encode("utf-8")
ACK=chr(0x06).encode("utf-8")
SMS_ESN_zero = (chr(0x53) + chr(0x4D) + chr(0x53) + chr(0x30)+chr(0x30)+chr(0x30) + chr(0x30)+chr(0x30)).encode("utf-8")
SMS_ESN_null = (chr(0x53) + chr(0x4D) + chr(0x53) + chr(0x00)+chr(0x00)+chr(0x00) + chr(0x00)+chr(0x00)).encode("utf-8")
ESN = (chr(0x00) + chr(0x00) + chr(0x00) + chr(0x00)+chr(0x00)+chr(0x00) + chr(0x00)+chr(0x00)).encode("utf-8")
SMS_NAMELEN_zero = (chr(0x30)+chr(0x30)+chr(0x30)).encode("utf-8")
SMS_NAMELEN_null = (chr(0x00)+chr(0x00)+chr(0x00)).encode("utf-8")
MSGLEN44_null = (chr(0x00)+chr(0x00)+chr(0x00)+chr(0x2C)).encode("utf-8")
LEN9 = (chr(0x00)+chr(0x00)+chr(0x00)+chr(0x09)).encode("utf-8")
LEN44 = (chr(0x00)+chr(0x00)+chr(0x00)+chr(0x2C)).encode("utf-8")
LEN12_null = (chr(0x00)+chr(0x00)+chr(0x00)+chr(0x00)+chr(0x31)+chr(0x32)).encode("utf-8")
LEN12_zero = (chr(0x30)+chr(0x30)+chr(0x30)+chr(0x30)+chr(0x31)+chr(0x32)).encode("utf-8")


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
            #write_timeout=None,
            interCharTimeout=None
        )
        self.parser = GlobalStarResponse()
        self.__logger = logging.getLogger('GlobalStarSerial')
        self.__serial.flushOutput()
        self.__serial.flushInput()

    def transmit(self,packet, timeOut, readInterval):
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
            time.sleep(readInterval*5)
        return incoming

    def ackResponse(self, poll):
        ackResponse = SYN1 + SYN2 + LEN9 + ACK + poll.encode('UTF-8')
        self.__serial.write(ackResponse)


    def sendFile(self, fileName, timeOut, readInterval):
        # receive message in string format
        # need to add length checking and splicing for files that are too long
        NAMELEN = str(len(fileName)).zfill(3)
        path = dataFiles + fileName
        with open(path, 'r') as myfile:
            message = myfile.read().rstrip('\n')
        MSGLEN = str(len(message)).zfill(6)

        # put everything together
        MSGpacket = LEN9+"RP333".encode('UTF-8') + ESN + NAMELEN + MSGLEN + fileName + message

        # get CRC value for everything together EXCEPT sync bytes
        CRC = crc16.crc16xmodem(MSGpacket)
        msbCRC = (CRC >> 8) & 0xFF
        print msbCRC
        lsbCRC = CRC & 0xFF
        print(lsbCRC)

        # put final packet together
        finalPacket = bytearray(SYN1 + SYN2 + MSGpacket + chr(msbCRC) + chr(lsbCRC))
        print("Writing message to globalstar and waiting for response: ",finalPacket)

        # ask globalstar if I can send a file
        self.push_file_poll(timeOut, readInterval)
        print finalPacket
        print "Done Push File Poll"

        # put everything together
        incoming = self.transmit(finalPacket, timeOut, readInterval)
        print("Incoming: ", incoming)  # brent edit

        if len(incoming) >= 11:
            response = self.parser.parseACK(incoming,"P333")
            if response == self.parser.error:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
        else:
            if len(incoming) == 0:
                print('Timeout on send')
                # todo: Handle this timeout better - maybe retry it or something?
            else:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)

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
        print "Writing health poll message"
        # print("response was: " + incoming)
        RSSI = 0
        Connected = 0
        incoming = self.transmit(packet,timeOut, readInterval)
        if len(incoming) > 11:
            response = self.parser.parseACK(incoming, "C401")
            if response == self.parser.error:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
            elif response == self.parser.ack and len(incoming) > 11:
                [RSSI,Connected] = self.parser.parseHealthPoll(incoming[11:])
        else:
            if len(incoming) == 0:
                print('Timeout on send')
                # todo: Handle this timeout better - maybe retry it or something?
            else:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
        return [RSSI,Connected]

    def push_file_poll(self, timeOut, readInterval):
        packet = bytearray(SYN1+SYN2+LEN9+"PP333".encode('UTF-8'))
        print packet
        print "Writing push file poll message"
        # print("response was: " + incoming)
        incoming = self.transmit(packet,timeOut, readInterval)
        if len(incoming) >= 11:
            response = self.parser.parseACK(incoming, "P333")
            if response == self.parser.error:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
            elif response == self.parser.ack and len(incoming) > 11:
               print(incoming[11:])
               return True
        else:
            if len(incoming) == 0:
                print('Timeout on send')
                # todo: Handle this timeout better - maybe retry it or something?
            else:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
        return False

    def SMS_pull_poll(self, timeOut, readInterval):
        packet = SYN1+SYN2+LEN9+"PR111".encode('UTF-8')
        print "Writing SMS pull poll message"
        # print("response was: " + incoming)
        incoming = self.transmit(packet,timeOut, readInterval)
        self.ackResponse("R111")
        print(len(incoming), " chars : ")
        print(incoming)
        if len(incoming) > 22:
            response = self.parser.parseACK(incoming,"R111")
            message = self.parser.parseSMSPoll(incoming[11:])
        else:
            if len(incoming) == 0:
                print('Timeout on send')
                # todo: Handle this timeout better - maybe retry it or something?
            else:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
        return False


    def poll_files_awaiting_download(self, timeOut, readInterval):
        packet = SYN1+SYN2+LEN9+"PC403".encode('UTF-8')
        print "Writing files awaiting download poll message"
        # print("response was: " + incoming)
        incoming = self.transmit(packet,timeOut, readInterval)
        # print(incoming)
        if len(incoming) >= 11:
            response = self.parser.parseACK(incoming,"C403")
            if response == self.parser.error:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
            elif response == self.parser.ack and len(incoming) > 11:
                self.parser.parseQueueLenPoll(incoming[11:], "C403")
        else:
            if len(incoming) == 0:
                print('Timeout on send')
                # todo: Handle this timeout better - maybe retry it or something?
            else:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
        return False

    def poll_messages_awaiting_upload(self, timeOut, readInterval):
        packet = SYN1+SYN2+LEN9+"PC405".encode('UTF-8')
        print "Writing messages awaiting upload poll message"
        # print("response was: " + incoming)
        incoming = self.transmit(packet,timeOut, readInterval)
        #print(incoming)
        if len(incoming) >= 11:
            response = self.parser.parseACK(incoming,"C405")
            if response == self.parser.error:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
            elif response == self.parser.ack and len(incoming) > 11:
                self.parser.parseQueueLenPoll(incoming[11:], "C405")
        else:
            if len(incoming) == 0:
                print('Timeout on send')
                # todo: Handle this timeout better - maybe retry it or something?
            else:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
        return False


    def poll_files_awaiting_upload(self, timeOut, readInterval):
        packet = SYN1+SYN2+LEN9+"PC407".encode('UTF-8')
        print "Writing files awaiting upload poll message"
        # print("response was: " + incoming)
        incoming = self.transmit(packet,timeOut, readInterval)
        #print(incoming)
        if len(incoming) >= 11:
            response = self.parser.parseACK(incoming,"C407")
            if response == self.parser.error:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
            elif response == self.parser.ack and len(incoming) > 11:
                self.parser.parseQueueLenPoll(incoming[11:], "C407")
        else:
            if len(incoming) == 0:
                print('Timeout on send')
                # todo: Handle this timeout better - maybe retry it or something?
            else:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
        return False


    def clear_uplink_messages(self, timeOut, readInterval):
        packet = SYN1+SYN2+LEN9+"PC500".encode('UTF-8')
        print "Writing clear uplink SMS message"
        # print("response was: " + incoming)
        incoming = self.transmit(packet,timeOut, readInterval)
        print(incoming)
        if len(incoming) >= 11:
            response = self.parser.parseACK(incoming,"C500")
            if response == self.parser.error:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
            elif response == self.parser.ack and len(incoming) > 11:
                print(incoming[11:])
        else:
            if len(incoming) == 0:
                print('Timeout on send')
                # todo: Handle this timeout better - maybe retry it or something?
            else:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
        return False


    def clear_uplink_files(self, timeOut, readInterval):
        packet = SYN1+SYN2+LEN9+"PC502".encode('UTF-8')
        print "Writing clear uplink files message"
        # print("response was: " + incoming)
        incoming = self.transmit(packet,timeOut, readInterval)
        print(incoming)
        if len(incoming) >= 11:
            response = self.parser.parseACK(incoming,"C502")
            if response == self.parser.error:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
            elif response == self.parser.ack and len(incoming) > 11:
                print(incoming[11:])
        else:
            if len(incoming) == 0:
                print('Timeout on send')
                # todo: Handle this timeout better - maybe retry it or something?
            else:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
        return False


    def clear_downlink_files(self, timeOut, readInterval):
        packet = SYN1+SYN2+LEN9+"PC504".encode('UTF-8')
        print "Writing clear downlink files message"
        # print("response was: " + incoming)
        incoming = self.transmit(packet,timeOut, readInterval)
        print(incoming)
        if len(incoming) >= 11:
            response = self.parser.parseACK(incoming,"C504")
            if response == self.parser.error:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
            elif response == self.parser.ack and len(incoming) > 11:
                print(incoming[11:])
        else:
            if len(incoming) == 0:
                print('Timeout on send')
                # todo: Handle this timeout better - maybe retry it or something?
            else:
                print('Malformed response from GlobalStar radio: ')
                print(incoming)
        return False

