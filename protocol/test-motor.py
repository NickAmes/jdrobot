#!/usr/bin/env python3
#Junior Design Robot
#LED Test tool
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

def add_slider(layout, labelstr, func, extra=None):
	lbl = QLabel(labelstr)
	sl = QSlider(Qt.Horizontal)
	sl.setMinimum(0)
	sl.setMaximum(255)
	sl.setTracking(True)
	sl.valueChanged.connect(func)
	if extra: sl.valueChanged.connect(extra)
	layout.addRow(lbl, sl)

def add_spinbox(layout, labelstr, func, extra=None):
	lbl = QLabel(labelstr)
	sp = QDoubleSpinBox()
	sp.setMinimum(-10)
	sp.setMaximum(10)
	sp.valueChanged.connect(func)
	if extra: sl.valueChanged.connect(extra)
	layout.addRow(lbl, sp)

def setup_controls(window, protocol):
	"""Setup the window controls."""
	window.setWindowTitle("Motor Tool")
	ww = QWidget(window)
	fl = QFormLayout(ww)
	add_slider(fl, "Left", robolib.set_motor_l_target)
	add_slider(fl, "Right", robolib.set_motor_r_target)
	add_slider(fl, "Both", robolib.set_motor_r_target, extra=robolib.set_motor_l_target)
	
	add_spinbox(fl, "Kp", robolib.set_motor_kp)
	add_spinbox(fl, "Ki", robolib.set_motor_ki)
	add_spinbox(fl, "Kd", robolib.set_motor_kd)
	ww.setLayout(fl)
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
	