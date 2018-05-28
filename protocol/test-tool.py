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

def setup_register_area(parent, window, protocol):
	"""Setup a form layout with all the register controls.
	   Returns a tuples (flayout, read_funcs, write_funcs)."""
	flayout = QFormLayout(parent)
	read_funcs = []
	write_funcs = []
	for r in protocol:
		label = QLabel(r.name)
		hl = QHBoxLayout()
		if r.size == "accum":
			sp = QDoubleSpinBox()
			sp.setDecimals(3)
		else:
			sp = QSpinBox()
		minval, maxval = minmax_values(r.size)
		sp.setMinimum(minval)
		sp.setMaximum(maxval)
		sp.setValue(r.default)
		hl.addWidget(sp)
		
		def readfunc(r, sp):
			def inner():
				v = getattr(robolib, "get_%s"%r.name)()
				print("Read %s: %f"%(r.name, float(v)))
				if not sp.hasFocus():
					sp.setValue(v)
			return inner
		readbtn = QToolButton()
		readbtn.setText("Read")
		if r.read:
			rf = readfunc(r, sp)
			readbtn.pressed.connect(rf)
			read_funcs.append(rf)
		else:
			readbtn.setEnabled(False)
		hl.addWidget(readbtn)
		
		def writefunc(r, sp):
			def inner():
				v = sp.value()
				getattr(robolib, "set_%s"%r.name)(v)
				print("Write %s: %f"%(r.name, float(v)))
			return inner
		writebtn = QToolButton()
		writebtn.setText("Write")
		if r.write:
			rf = writefunc(r, sp)
			writebtn.pressed.connect(rf)
			sp.editingFinished.connect(rf)
			write_funcs.append(rf)
		else:
			writebtn.setEnabled(False)
		hl.addWidget(writebtn)
		
		flayout.addRow(label, hl)
	return (flayout, read_funcs, write_funcs)

def setup_controls(window, protocol):
	"""Setup the window controls."""
	window.setWindowTitle("Test Tool")
	ww = QWidget(window)
	vl = QVBoxLayout(ww)
	
	flayout, read_funcs, write_funcs = setup_register_area(ww, window, protocol)
	
	hl = QHBoxLayout(ww)
	hl.addStretch()
	def readall_func():
		for r in read_funcs:
			r()
	rabtn = QToolButton()
	rabtn.setText("Read All")
	rabtn.pressed.connect(readall_func)
	hl.addWidget(rabtn)
	def writeall_func():
		for w in write_funcs:
			w()
	wrbtn = QToolButton()
	wrbtn.setText("Write All")
	wrbtn.pressed.connect(writeall_func)
	hl.addWidget(wrbtn)
	
	vl.addLayout(hl)
	vl.addLayout(flayout)
	ww.setLayout(vl)
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
	