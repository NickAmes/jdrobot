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

def minmax_values(size_code):
	"""Given a size code, return the (min, max) values as a tuple."""
	return {"accum"    : (-2**15, 2**15 - 1),
			"int8_t"   : (-2**7,2**7 - 1),
			"uint8_t"  : (0,2**8),
			"int16_t"  : (-2**15, 2**15 - 1),
			"uint16_t" : (0,2**16),
			"int32_t"  : (-2**31,2**31 - 1),
			"uint32_t" : (0,2**32)}[size_code]

def setup_controls(window, protocol):
	"""Setup the window controls."""
	window.setWindowTitle("Test Tool")
	ww = QWidget(window)
	flayout = QFormLayout(ww)
	for r in protocol:
		label = QLabel(r.name)
		hl = QHBoxLayout()
		if r.size == "accum":
			sp = QDoubleSpinBox()
		else:
			sp = QSpinBox()
		minval, maxval = minmax_values(r.size)
		sp.setMinimum(minval)
		sp.setMaximum(maxval)
		sp.setValue(r.default)
		hl.addWidget(sp)
		#hl.addWidget(readbtn)
		#hl.addWidget(writebtn)
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
	