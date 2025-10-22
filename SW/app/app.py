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
        self.setGeometry(200, 200, 800, 400)
        self.setWindowTitle("PassMan Application")
        self.initUI()
        self.serial = QSerialPort(self)
        self.entryID = []
        self.entryUSRNAME = []
        self.entryPSW = []

    def initUI(self):
        # search available COM ports button
        self.searchCOMButton = QtWidgets.QPushButton(self)
        self.searchCOMButton.setText("Search COMs")
        self.searchCOMButton.clicked.connect(self.search_com_ports)
        self.searchCOMButton.move(50, 0)

        # drop menu where available COM ports appears after search
        self.comPortDropdown = QtWidgets.QComboBox(self)
        self.comPortDropdown.setGeometry(45, 30, 310, 30)
        self.comPortDropdown.clear()

        # Connect button
        self.connectButton = QtWidgets.QPushButton(self)
        self.connectButton.setText("Connect")
        self.connectButton.clicked.connect(self.connect_to_port)
        self.connectButton.move(150, 0)

        # Disconnect button
        self.disconnectButton = QtWidgets.QPushButton(self)
        self.disconnectButton.setText("Disconnect")
        self.disconnectButton.clicked.connect(self.disconnect_from_port)
        self.disconnectButton.move(250, 0)

        # Read Serial port button
        self.readSerialButton = QtWidgets.QPushButton(self)
        self.readSerialButton.setText("Read Serial")
        self.readSerialButton.clicked.connect(self.read_serial_data)
        self.readSerialButton.move(250, 75)

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
            self.serial.setBaudRate(QSerialPort.Baud9600)
            self.serial.setDataBits(QSerialPort.Data8)
            self.serial.setParity(QSerialPort.NoParity)
            self.serial.setStopBits(QSerialPort.OneStop)
            self.serial.setFlowControl(QSerialPort.NoFlowControl)

            if self.serial.open(QIODevice.OpenModeFlag.ReadWrite):
                QtWidgets.QMessageBox.information(self, "Connection", f"Connected to {selected_port}")
            else:
                QtWidgets.QMessageBox.critical(self, "Error", f"Failed to connect to {selected_port}")
    def disconnect_from_port(self):
        if self.serial.isOpen():
            self.serial.close()
            QtWidgets.QMessageBox.information(self, "Disconnection", "Disconnected from the serial port")
        else:
            QtWidgets.QMessageBox.warning(self, "Warning", "Serial port is not open")
    
    def read_serial_data(self):
        if self.serial.isOpen():
            try:
                data = self.serial.readAll().data().decode('utf-8')
                self.serial_output_textedit.append(data)
                if data.strip() == "load":
                    self.serial.write(b"ok\n")
                    print("got load command")
                elif data.strip() == "end":
                    print("End of communication..")
                    for i in range(len(self.entryID)):
                        print(f"Entry {i + 1}: ID={self.entryID[i]}, Username={self.entryUSRNAME[i]}, Password={self.entryPSW[i]}")
                else:
                    lines = data.strip().split('\n')  # Assuming each entry is on a new line
                    for line in lines:
                        parts = line.split(';')
                        if len(parts) == 3:
                            entryID, entryUSRNAME, entryPSW = parts
                            print(f"ID: {entryID}, Username: {entryUSRNAME}, Password: {entryPSW}")
                            self.entryID.append(entryID)
                            self.entryUSRNAME.append(entryUSRNAME)
                            self.entryPSW.append(entryPSW)
                

            except Exception as e:
                QtWidgets.QMessageBox.critical(self, "Error", f"Failed to read data: {str(e)}")
        else:
            QtWidgets.QMessageBox.warning(self, "Warning", "Serial port is not open")

def Window():
    app = QApplication(sys.argv)
    win = MainWindow()


    win.show()
    sys.exit(app. exec())

Window()
