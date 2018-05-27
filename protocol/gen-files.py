#!/usr/bin/env python3
#Junior Design Robot
#Protocol Code Generator
#Nick Ames 2018
#
#Generates C and Python source files for communicating with the robot,
#based on a spreadsheet.
import xlrd
import os

SoftwareDir = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), os.pardir)) + os.sep
SpreadsheetPath = SoftwareDir + "protocol/Protocol.xls"
FirmwareCPath = SoftwareDir + "firmware/protocol.c"
FirmwareHPath = SoftwareDir + "firmware/protocol.h"



def main():
	print(SoftwareDir)

if __name__ == "__main__":
	main()