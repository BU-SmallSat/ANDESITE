string = "timeInMin180"
min = int(string[9:])
sec = 10000
sec_hex = hex(sec)
print(sec_hex)
high,low = divmod(sec,0x100)
high_hex = hex(high)
low_hex = hex(low)
print(high_hex)
print(low_hex)