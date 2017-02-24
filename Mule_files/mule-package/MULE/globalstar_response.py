import crc16

SYN1 = chr(0x47).encode("utf-8")
SYN2 = chr(0x55).encode("utf-8")
NAK = chr(0x0F).encode("utf-8")
ACK = chr(0x06).encode("utf-8")
LEN9 = (chr(0x00)+chr(0x00)+chr(0x00)+chr(0x09)).encode("utf-8")
LEN44 = (chr(0x00)+chr(0x00)+chr(0x00)+chr(0x2C)).encode("utf-8")

class GlobalStarResponse():
    def __init__(self):
        self.ack = 1
        self.nak = 2
        self.error = 3

    def from_bytes(self, data, big_endian=False):
        if isinstance(data, str):
            data = bytearray(data)
        if big_endian:
            data = reversed(data)
        num = 0
        for offset, byte in enumerate(data):
            num += byte << (offset * 8)
        return num

    def timeElapsedDisplay(self,secs):
        d = secs // 86400
        secs = secs - (d * 86400)
        h = secs // 3600
        secs = secs - (h * 3600)
        m = secs // 60
        secs = secs - (m * 60)
        return "%03d:%02d:%02d:%02d" % (d, h, m, secs)

    def oldparse(self, buffer):
        assert type(buffer) is bytes
        if len(buffer) != 3:
            return self.error
        if buffer[0] == 0x47 and buffer[1] == 0x55:
            if buffer[2] == 0x06:
                return self.ack
            if buffer[2] == 0x0F:
                return self.nak
        else:
            return self.error

    def parseACK(self, incoming,poll):
        POLL_ACK = SYN1 + SYN2 + LEN9 + ACK + poll.encode('UTF-8')
        POLL_NAK = SYN1 + SYN2 + LEN9 + NAK + poll.encode('UTF-8')
        if len(incoming) >= 11:
            #print "***Parsing ack/nak string***"
            # print("recieved string: "+ incoming[:10])
            if incoming[:11] == POLL_ACK:
                print "ACK received!"
                return self.ack
            elif incoming[:11] == POLL_NAK:
                print "NAK received!"
                return self.nak
            else:
                print('Malformed response from GlobalStar radio')
                print 'Recieved string: ', incoming[0:11]
                print 'Expected string: ', POLL_ACK
                return self.error
        else:
            print("recieved insufficient bytes for poll ACK!")
            print("recieved only " + len(incoming) + " bytes")
        return self.error

    def parseSMSPoll(self, incoming):
        # CANNOT HARD CODE POLL HEADER BECAUSE LENGTH WILL BE DETERMINED BY SIZE OF SMS MESSAGE
        SYN_HDR = SYN1 + SYN2
        MSG_HDR = "R111".encode('UTF-8')
        if (len(incoming) >= 11):
            print "***Parsing poll response header string***"
            # print("recieved string: "+ buffer[:11])
            #if incoming[:2] == SYN_HDR and incoming[6:11] == MSG_HDR:
            if len(incoming) > 11:
                print("\n")
                print("Response:", incoming)
                print("ESN: " + incoming[11:19].decode("utf8"))
                fileNameSize = incoming[19:22].decode("utf8")
                print("File Name Size: " + fileNameSize)
                payloadSize = incoming[22:28].decode("utf8")
                print("Payload Size (max 34): " + payloadSize)
                print("File name (internal use only)", incoming[28:28 + int(fileNameSize)])
                print(
                "Payload: " + incoming[28 + int(fileNameSize):28 + int(fileNameSize) + int(payloadSize)].decode(
                    "utf8"))
                print("CRC Integer: ", int(str(incoming[-2:]).encode('hex'), 16))

                print("\n")
            else:
                print('Malformed response from GlobalStar radio')
                print 'Recieved string: ', incoming
                return self.error
            #else:
            #    print('Malformed response from GlobalStar radio')
            #    print 'Recieved string: ', incoming[0:11]
            #    print 'Expected string: ', MSG_HDR
            #    return self.error
        else:
            print("recieved insufficient bytes for poll RESPONSE!")
            print("recieved only " + len(incoming) + " bytes")
            print 'Recieved string: ', incoming
            return self.error

    def parseHealthPoll(self, incoming):
        POLL_HDR = SYN1 + SYN2 + LEN44 + "RC401".encode('UTF-8')
        if(len(incoming) >= 11):
            #print "***Parsing poll response header string***"
            # print("recieved string: "+ buffer[:11])
            if incoming[:11] == POLL_HDR:
                if len(incoming)>=46:
                    #print "successfull Health Poll header recieved"
                    print "***Parsing Health Status string***"
                    print("Epoch: " + str(self.from_bytes(incoming[11:15], True)))
                    print("Elapsed time: " + self.timeElapsedDisplay(self.from_bytes(incoming[15:19], True)))
                    print("RSSI: " + str(self.from_bytes(incoming[19:20], True)))
                    print("Connected: " + str(self.from_bytes(incoming[20:21], True)))
                    print("Gateway: " + str(self.from_bytes(incoming[21:22], True)))
                    print("Last Contact: " + self.timeElapsedDisplay(self.from_bytes(incoming[22:26], True)))
                    print("Last Attempt: " + self.timeElapsedDisplay(self.from_bytes(incoming[26:30], True)))
                    print("Number of Call Attempts: " + str(self.from_bytes(incoming[30:34], True)))
                    print("Number of Successful Connects: " + str(self.from_bytes(incoming[34:38], True)))
                    print("Average Connection Duration: " + str(self.from_bytes(incoming[38:42], True)))
                    print("Average Connection Duration SD: " + str(self.from_bytes(incoming[42:46], True)))
                    print("Recieved string: ", (incoming))
                else:
                    print('Malformed response from GlobalStar radio')
                    print 'Recieved string: ', incoming
            else:
                print('Malformed response from GlobalStar radio')
                print 'Recieved string: ', incoming[0:11]
                print 'Expected string: ', POLL_HDR
        else:
            print("recieved insufficient bytes for poll RESPONSE!")
            print("recieved only " + len(incoming) + " bytes")
            print 'Recieved string: ', incoming

    def parseQueueLenPoll(self,incoming, poll):
        POLL_HDR = SYN1 + SYN2 + LEN9 + ('R'+poll).encode('UTF-8')
        if(len(incoming) >= 11):
            #print "***Parsing poll response header string***"
            # print("recieved string: "+ buffer[:11])
            #if incoming[:11] == POLL_HDR:
            if len(incoming)>=15:
                print("Messages Queue: " + str(self.from_bytes(incoming[11:], True)))
                #print 'recieved string: ', incoming
            else:
                print('Malformed response from GlobalStar radio')
                print 'Recieved string: ', incoming
            #else:
                #print('Malformed response from GlobalStar radio')
                #print 'Recieved string: ', incoming[0:11]
                #print 'Expected string: ', POLL_HDR
        else:
            print("recieved insufficient bytes for poll RESPONSE!")
            print("recieved only " + len(incoming) + " bytes")
            print 'Recieved string: ', incoming
