#!/usr/bin/env python3
#Junior Design Robot
#Test tool
#Nick Ames 2018
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
import signal
import os
import sys
import gen_files

SoftwareDir=os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), os.pardir))
sys.path.append(os.path.abspath(SoftwareDir) + "/python-library/")
import robolib

def setup_controls(window, protocol):
	window.setWindowTitle("Test Tool")
	ww = QWidget(window)
	flayout = QFormLayout(ww)
	for r in protocol:
		label = QLabel(r.name)
		hl = QHBoxLayout()
		
		flayout.addRow(label, hl)
	ww.setLayout(flayout)
	window.setCentralWidget(ww)

def main():
	robolib.init()
	signal.signal(signal.SIGINT, signal.SIG_DFL) #Make Ctrl-C quit the program
	p = gen_files.parse_sheet(gen_files.SpreadsheetPath)
	app = QApplication(sys.argv)
	w = QMainWindow()
	setup_controls(w, p)
	w.show()
	sys.exit(app.exec_())
	
if __name__ == "__main__":
	main()
	