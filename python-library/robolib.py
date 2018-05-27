#!/usr/bin/env python3
#Junior Design Robot
#Interface Library
#Nick Ames 2018
import wiringpi
import struct

def interpret_size_code(size_code):
	"""Return the (byte length, fixedpoint, signed) as a tuple
	   for a size code."""
	if size_code == "accum":
		return (4, True, True)
	else:
		pass

def read_reg_raw(reg_num, size_code):
	"""Given a register number and size in bytes, read it
	   and return the value (as an int or float).
	   If the register represents a fixed-point number,
	   fixedpoint must be true."""
	size, fixedpoint, signed = interpret_size_code(size_code)
	buf = bytes([reg_num + 0x80, 0] + [0] * size)
	_ , retdata = wiringpi.wiringPiSPIDataRW(0, buf)
	data = list(retdata[2:]) + [0]
	v = 0
	if not fixedpoint:
		for b,i in zip(data, range(0, len(data))):
			v += b << (i * 8)
	else:
		if data[3] & 0x80:
			neg = True
			data = struct.pack("<L", (~struct.unpack("<L", bytes(data[0:4]))[0] + 1) & 0xFFFFFFFF)
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
	
def write_reg_raw(reg_num, size_code, fixedpoint, value):
	"""Write a register, given the size in bytes.
	   If the register represents a fixed-point number,
	   fixedpoint must be true."""
	if not fixedpoint:
		data = struct.pack("<l", value)
	else:
		pass
	buf = bytes([reg_num, 0] + list(data))
	wiringpi.wiringPiSPIDataRW(0, buf[0:size+2])
	
def init():
	"""Setup the SPI interface for communication with the microcontroller."""
	wiringpi.wiringPiSetupGpio()  # For GPIO pin numbering
	wiringpi.wiringPiSPISetup(0, 500000)

def main():
	init()
	print(read_reg_raw(42, "accum"))

if __name__ == "__main__":
	main()