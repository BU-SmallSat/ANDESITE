class GlobalStarResponse():
    ack = 1
    nak = 2
    error = 3

    @staticmethod
    def parse(buffer):
        assert type(buffer) is bytes
        if len(buffer) != 3:
            return GlobalStarResponse.error
        if buffer[0] == 0x47 and buffer[1] == 0x55:
            if buffer[2] == 0x06:
                return GlobalStarResponse.ack
            if buffer[2] == 0x0F:
                return GlobalStarResponse.nak
        else:
            return GlobalStarResponse.error
