import struct
import ctypes

byteList = ctypes.create_string_buffer(9)
struct.pack_into('b',byteList, 0, 5)
struct.pack_into('hhhh', byteList, 1, 20, 20, 20, 415)
print(int(byteList[0], 16))
(x,y,z,duration) = struct.unpack_from('hhhh', byteList, 1)
print(y)