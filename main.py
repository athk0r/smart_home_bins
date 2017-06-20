#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import sys
import subprocess
import PyQt5
import os
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
import mainwindow_auto

class MainWindow(QMainWindow, mainwindow_auto.Ui_MainWindow):
    # Zustand des Sensors zur Demonstration der Signalstörung
    def changeStateSigStoerung(self):
        if self.switchStateSignalstoerung.isChecked():
            print("Fenster Sig. Störung auf")
            #subprocess.call("ZustandEin.sh"
            os.system('ps -a | grep disturb | cut -d " " -f2 | xargs kill -10');
        else:
            print("Fenster Sig. Störung zu")
            #subprocess.call("ZustandAus.sh")
            os.system('ps -a | grep disturb | cut -d " " -f2 | xargs kill -10');

    # Signalstörung ein/aus Schalten
    def Stoerung(self):
        if self.StoerungEinAus.isChecked():
            print("Störung Ein")

        else:
            print("Störung Aus")

    # Zustand des Sensors zur Demonstration der Paketverdopplung
    def changeStatePaketverdopplung(self):
        if self.switchStatePaketverdoppelung.isChecked():
            print("Fenster Verdopplung auf")
            os.system('ps -a | grep duplicate | cut -d " " -f2 | xargs kill -12');
        else:
            print("Fenster Verdopplung zu")
            os.system('ps -a | grep duplicate | cut -d " " -f2 | xargs kill -12');

    # Zustand des Sensors zur Demonstration des Herdes
    def changeStateHerd(self):
        if self.HerdOnOff.isChecked():
            print("Herd Ein")
            os.system('ps -a | grep herd | cut -d " " -f2 | xargs kill -10');
        else:
            print("Herd Aus")
            os.system('ps -a | grep herd | cut -d " " -f2 | xargs kill -10');

    # Zustand des Sensors zur Demonstration des Wassers
    def changeStateWasser(self):
        if self.WasserOnOff.isChecked():
            print("Wasser Ein")
            os.system('ps -a | grep wasser | cut -d " " -f2 | xargs kill -10');
        else:
            print("Wasser Aus")
            os.system('ps -a | grep wasser | cut -d " " -f2 | xargs kill -10');

    # Paketverdopplung starten
    def Verdopplung(self):
        if self.VerdopplungEinAus.isChecked():
            print("Paketverdopplung eigeschaltet")
            self.timer.start(50,self)
            self.VerdopplungEinAus.setEnabled(False)

        else:
            print("Paketverdopplung ausgeschaltet")
            self.VerdopplungEinAus.setEnabled(True)
            self.i=0
            self.timer.stop()


    def timerEvent(self,e):
        if self.i>=100:
            self.timer.stop()
            self.VerdopplungEinAus.toggle()
            self.Verdopplung()
            self.progressVerdopplung.setValue(0)
            self.i=0
        self.progressVerdopplung.setValue(self.i)
        self.i = self.i+1


    def __init__(self):
        super(self.__class__, self).__init__()
        self.setupUi(self)
        self.switchStateSignalstoerung.clicked.connect(lambda: self.changeStateSigStoerung())
        self.StoerungEinAus.clicked.connect(lambda: self.Stoerung())
        self.switchStatePaketverdoppelung.clicked.connect(lambda: self.changeStatePaketverdopplung())
        self.HerdOnOff.clicked.connect(lambda: self.changeStateHerd())
        self.WasserOnOff.clicked.connect(lambda: self.changeStateWasser())
        self.VerdopplungEinAus.clicked.connect(lambda: self.Verdopplung())
        self.progressVerdopplung.setValue(0)
        self.timer = QBasicTimer()
        self.i = 0


def main():

    app = QApplication(sys.argv)
    form = MainWindow()
    form.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
