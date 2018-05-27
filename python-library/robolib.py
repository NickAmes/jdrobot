#!/usr/bin/env python3
#Junior Design Robot
#Interface Library
#Nick Ames 2018
import wiringpi
import struct
import math

def interpret_size_code(size_code):
	"""Return the (byte length, fixedpoint, structcode) as a tuple
	   for a size code."""
	return {"accum"    : (4, True, None),
			"int8_t"   : (1, False, "<b"),
			"uint8_t"  : (1, False, "<B"),
			"int16_t"  : (2, False, "<h"),
			"uint16_t" : (2, False, "<H"),
			"int32_t"  : (4, False, "<l"),
			"uint32_t" : (4, False, "<L")}[size_code]
		 

def read_reg_raw(reg_num, size_code):
	"""Given a register number and size in bytes, read it
	   and return the value (as an int or float).
	   If the register represents a fixed-point number,
	   fixedpoint must be true."""
	size, fixedpoint, structcode = interpret_size_code(size_code)
	buf = bytes([reg_num + 0x80, 0] + [0] * size)
	_ , retdata = wiringpi.wiringPiSPIDataRW(0, buf)
	data = list(retdata[2:])
	v = 0
	if not fixedpoint:
		v = struct.unpack(structcode, bytes(data))[0]
	else:
		if data[3] & 0x80:
			neg = True
			data = struct.pack("<L", (~struct.unpack("<L", bytes(data))[0] + 1) & 0xFFFFFFFF)
		else:
			neg = False
		frac = 0
		for b in range(0, 15):
			i = int(b > 7)
			if ((data[0+i] >> b - (i * 8)) & 0x01) == 1: frac += 2**-(15-b)
		v += data[1] >> 7
		v += data[2] << 1 
		v += (data[3] & 0x7F) << 9
		v += frac
		if neg: v = -v
	return v
	
def write_reg_raw(reg_num, size_code, value):
	"""Write a register, given the size in bytes.
	   If the register represents a fixed-point number,
	   fixedpoint must be true."""
	size, fixedpoint, structcode = interpret_size_code(size_code)
	if not fixedpoint:
		data = struct.pack(structcode, value)
	else:
		if value < 0:
			neg = True
			value = -value
		else:
			neg = False
		data = list(struct.pack("<H", math.floor(abs(value) * 2**15) & 0xFFFF)) + list(struct.pack("<H", math.floor(value)))
		data[1] = (data[2] & 0x01) << 7 | (data[1] & 0x7F)
		data[2] = ((data[2] >> 1) & 0x7F) |(data[3] & 0x01) << 7 
		data[3] = (data[3] >> 1) & 0x7F
		if neg:
			data = struct.pack("<L", (~struct.unpack("<L", bytes(data))[0] + 1) & 0xFFFFFFFF)
	buf = bytes([reg_num, 0] + list(data))
	wiringpi.wiringPiSPIDataRW(0, buf[0:size+2])
	
def init():
	"""Setup the SPI interface for communication with the microcontroller."""
	wiringpi.wiringPiSetupGpio()  # For GPIO pin numbering
	wiringpi.wiringPiSPISetup(0, 500000)

def main():
	init()
	write_reg_raw(42, "accum", -12345.12345678)
	print(read_reg_raw(42, "accum"))

if __name__ == "__main__":
	main()