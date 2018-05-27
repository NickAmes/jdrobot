#!/usr/bin/env python3
#Junior Design Robot
#Test tool
#Nick Ames 2018
from PyQt5.QtGui import *
from PyQt5.QtCore import *
import signal
import os
import sys

SoftwareDir=os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), os.pardir))
sys.path.append(os.path.abspath(SoftwareDir) + "/python-library/")

print(sys.path)
import robolib

def main():
	signal.signal(signal.SIGINT, signal.SIG_DFL) #Make Ctrl-C quit the program
	print("test")
	
if __name__ == "__main__":
	main()
	