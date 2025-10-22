# pylint: disable

import sys
from PySide6 import QtWidgets, QtCore
from PySide6.QtWidgets import QApplication, QMainWindow
from PySide6.QtSerialPort import QSerialPort, QSerialPortInfo
from PySide6.QtCore import QIODevice
import serial.tools.list_ports


class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.setGeometry(200, 200, 400, 400)
        self.setWindowTitle("PassMan Application")
        self.initUI()
        self.serial = QSerialPort(self)

    def initUI(self):
        # search available COM ports button
        self.searchCOMButton = QtWidgets.QPushButton(self)
        self.searchCOMButton.setText("Search COMs")
        self.searchCOMButton.clicked.connect(self.search_com_ports)
        self.searchCOMButton.move(100, 0)

        # drop menu where available COM ports appears after search
        self.comPortDropdown = QtWidgets.QComboBox(self)
        self.comPortDropdown.setGeometry(50, 100, 300, 30)
        self.comPortDropdown.clear()

        # connect button
        self.connectButton = QtWidgets.QPushButton(self)
        self.connectButton.setText("Connect")
        self.connectButton.clicked.connect(self.connect_to_port)
        self.connectButton.move(200, 0)

        # text area to display content from the connected serial port
        self.serial_output_textedit = QtWidgets.QTextEdit(self)
        self.serial_output_textedit.setGeometry(50, 150, 300, 200)
        self.serial_output_textedit.setTextColor("green")
        self.serial_output_textedit.setReadOnly(True)

    def search_com_ports(self):
        self.comPortDropdown.clear()
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.comPortDropdown.addItem(port.device)

    def connect_to_port(self):
        selected_port = self.comPortDropdown.currentText()
        if selected_port:
            self.serial.setPortName(selected_port)
            self.serial.setBaudRate(QSerialPort.Baud115200)

            if self.serial.open(QIODevice.OpenModeFlag.ReadWrite):
                QtWidgets.QMessageBox.information(self, "Connection", f"Connected to {selected_port}")
                
            else:
                QtWidgets.QMessageBox.critical(self, "Error", f"Failed to connect to {selected_port}")

def Window():
    app = QApplication(sys.argv)
    win = MainWindow()


    win.show()
    sys.exit(app. exec())

Window()
