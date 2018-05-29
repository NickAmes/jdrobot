#!/usr/bin/env python3
#Junior Design Robot
#Interface Library
#Nick Ames 2018
#WARNING: This file is automatically generated by gen-files.py
#Any changes you make will be erased.
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
		value = int(value)
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
def set_left_motor(value): #Left Motor Target Speed in RPM
	write_reg_raw(1, "accum", value)

def get_left_motor(): #Left Motor Target Speed in RPM
	return read_reg_raw(1, "accum")

def set_right_motor(value): #Right Motor Target Speed in RPM
	write_reg_raw(2, "accum", value)

def get_right_motor(): #Right Motor Target Speed in RPM
	return read_reg_raw(2, "accum")

def get_left_power(): #Left Motor Power
	return read_reg_raw(3, "int16_t")

def get_right_power(): #Right Motor Power
	return read_reg_raw(4, "int16_t")

def get_left_speed(): #Left Motor Actual Speed in RPM
	return read_reg_raw(5, "accum")

def get_right_speed(): #Right Motor Actual Speed in RPM
	return read_reg_raw(6, "accum")

def set_motor_p(value): #Motor Speed Control P Constant
	write_reg_raw(7, "accum", value)

def get_motor_p(): #Motor Speed Control P Constant
	return read_reg_raw(7, "accum")

def set_motor_i(value): #Motor Speed Control I Constant
	write_reg_raw(8, "accum", value)

def get_motor_i(): #Motor Speed Control I Constant
	return read_reg_raw(8, "accum")

def set_motor_d(value): #Motor Speed Control D Constant
	write_reg_raw(9, "accum", value)

def get_motor_d(): #Motor Speed Control D Constant
	return read_reg_raw(9, "accum")

