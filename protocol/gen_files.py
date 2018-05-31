#!/usr/bin/env python3
#Junior Design Robot
#Protocol Code Generator
#Nick Ames 2018
#
#Generates C and Python source files for communicating with the robot,
#based on a spreadsheet.
import xlrd
import os
from collections import namedtuple

SoftwareDir = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), os.pardir)) + os.sep
SpreadsheetPath = SoftwareDir + "protocol/Protocol.xls"
FirmwareCPath = SoftwareDir + "firmware/protocol.c"
FirmwareHPath = SoftwareDir + "firmware/protocol.h"
PythonPath = SoftwareDir + "python-library/robolib.py"
PythonTemplatePath = SoftwareDir + "protocol/gen_file_robolibtemplate.py"

Reg = namedtuple("Reg",
                 ["number", "name", "size", "read", "write",
                  "default", "func", "desc"])

def parse_sheet(spreadsheet_path):
	"""Given a path to a protocol spreadsheet, parse it and return
	   a list of names tuples with fields:
	    number  - Register Number
	    name    - Register Name
	    size    - Datatype
	    read    - Readable?
	    write   - Writeable?
	    default - Default Value
	    func    - Firmware Write Function
	    desc    - Description
	"""
	def get_cols(sheet):
		"""Given a sheet, look at the first row and determine the index of each column.
		   Returns a Reg with the column number set in each field."""
		names = {"number"  : ["Register Number", -1],
		         "name"    : ["Register Name", -1],
		         "size"    : ["Size", -1],
		         "read"    : ["R/W", -1],
		         "write"   : ["R/W", -1],
		         "default" : ["Default Value", -1],
		         "func"    : ["Firmware Write Func", -1],
		         "desc"    : ["Description", -1]}
		for i in range(0, sheet.ncols):
			cell = sheet.cell(0, i).value
			for k in names:
				if names[k][0] == cell:
					 names[k][1] = i
		return Reg(number=names["number"][1], name=names["name"][1], size=names["size"][1], read=names["read"][1], write=names["write"][1], default=names["default"][1], func=names["func"][1], desc=names["desc"][1])
	
	book = xlrd.open_workbook(spreadsheet_path)
	sheet = book.sheet_by_index(0)
	cols = get_cols(sheet)
	regs = []
	for i in range(1, sheet.nrows):
		reg = Reg(number=sheet.cell(i, cols.number).value,
		          name=sheet.cell(i, cols.name).value,
		          size=sheet.cell(i, cols.size).value,
		          read='r' in sheet.cell(i, cols.read).value.lower(),
		          write='w' in sheet.cell(i, cols.write).value.lower(),
		          default=sheet.cell(i, cols.default).value,
		          func=sheet.cell(i, cols.func).value,
		          desc=sheet.cell(i, cols.desc).value)
		regs.append(reg)
	return regs

def gen_cheader(protocol):
	"""Return a c header file for the given protocol as a string."""
	s = """/* Junior Design Sp2018 Final Project
 * Robot Firmware - RPi <-> Microcontroller Communication
 * Nick Ames 2018
 * WARNING: This file is automatically generated by gen-files.py
 *          Any changes you make will be erased.
 */
#include <stdfix.h>
#include <stdint.h>
#include "config.h"

"""
	s += "struct comm_data_t {\n"
	for r in protocol:
		s += "\t" + r.size + " " + r.name + "; /* " + r.desc + " */\n"
	s += "};\n\n"
	for r in protocol:
		s += "%s get_%s(void); /* %s */\n"%(r.size, r.name, r.desc)
		s += "void set_%s(%s); /* %s */\n\n"%(r.name, r.size, r.desc)
	s += """extern volatile struct comm_data_t Data;"""
	return s

def gen_csource(protocol):
	"""Return a c source file for the given protocol as a string."""
	def format_default(reg):
		"""Given a reg, return its default value formatted as a string for inclusion in
		   a C source file."""
		if reg.size == "accum":
			return str(float(reg.default)) + "k"
		else:
			return str(int(reg.default)) + "L"

	s = """/* Junior Design Sp2018 Final Project
 * Robot Firmware - RPi <-> Microcontroller Communication
 * Nick Ames 2018
 * WARNING: This file is automatically generated by gen-files.py
 *          Any changes you make will be erased.
 */
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "protocol.h"
#include "spi.h"

"""
	s += "volatile struct comm_data_t Data = {\n"
	for r in protocol:
		s += "\t." + r.name + " = " + format_default(r) + ", /* " + r.desc + " */\n"
	s += "};\n\n"
	s += "\n"
	
	for r in protocol:
		s += "%s get_%s(void){ /* %s */\n"%(r.size, r.name, r.desc)
		s += """\t%s v;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		v = Data.%s;
	}
	return v;
}
"""%(r.size, r.name)
		s += "void set_%s(%s v){ /* %s */\n"%(r.name, r.size, r.desc)
		s += """\tATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		Data.%s = v;
	}
}

"""%(r.name)
	s += """ISR(SPI0_STC_vect){
	uint8_t reg_num = SPDR0;
	switch(reg_num){
"""
	
	for r in protocol:
		if r.write:
			s += "\t\tcase        % 2d: /* Write %s (%s) */\n"%(r.number, r.name, r.desc)
			s += "\t\t\tspi_rx((uint8_t *) &Data.%s, sizeof(Data.%s));\n"%(r.name, r.name)
			s += "\t\t\tbreak;\n"
		if r.read:
			s += "\t\tcase 0x80 + % 2d: /* Read %s (%s) */\n"%(r.number, r.name, r.desc)
			s += "\t\t\tspi_tx((uint8_t *) &Data.%s, sizeof(Data.%s));\n"%(r.name, r.name)
			s += "\t\t\tbreak;\n"
	s += """	}

	/* Clear SPIF flag */
	reg_num = SPSR0;
	reg_num = SPDR0;
}
"""	
	return s

def gen_python(protocol):
	"""Return a python source file fragment for the given protocol as a string."""
	s = ""
	for r in protocol:
		if r.write:
			s += "def set_%s(value): #%s\n"%(r.name, r.desc)
			s += "\twrite_reg_raw(%d, \"%s\", value)\n\n"%(r.number, r.size)
		if r.read:
			s += "def get_%s(): #%s\n"%(r.name, r.desc)
			s += "\treturn read_reg_raw(%d, \"%s\")\n\n"%(r.number, r.size)
	return s

def write_firmware(protocol, cpath, hpath):
	with open(hpath, "w") as hfile:
		hfile.write(gen_cheader(protocol))
	with open(cpath, "w") as cfile:
		cfile.write(gen_csource(protocol))

def write_python(protocol, pypath, temppath):
	with open(pypath, "w") as pyfile:
		with open(temppath, "r") as tempfile:
			pyfile.write(tempfile.read())
			pyfile.write(gen_python(protocol))

def main():
	p = parse_sheet(SpreadsheetPath)
	write_firmware(p, FirmwareCPath, FirmwareHPath)
	write_python(p, PythonPath, PythonTemplatePath)

if __name__ == "__main__":
	main()