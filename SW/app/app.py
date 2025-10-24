# pylint: disable=all

import sys
from PySide6 import QtWidgets, QtCore
from PySide6.QtWidgets import QApplication, QMainWindow
from PySide6.QtSerialPort import QSerialPort, QSerialPortInfo
from PySide6.QtCore import QIODevice
import serial.tools.list_ports


class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.setGeometry(500, 350, 740, 400)
        self.setWindowTitle("PassMan Application")
        self.initUI()
        self.serial = QSerialPort(self)
        self.entryID = []
        self.entryUSRNAME = []
        self.entryPSW = []

    def initUI(self):
        # drop menu where available COM ports appears after search
        self.comPortDropdown = QtWidgets.QComboBox(self)
        self.comPortDropdown.setGeometry(45, 32, 310, 30)
        self.comPortDropdown.clear()

        # search available COM ports button
        self.searchCOMButton = QtWidgets.QPushButton(self)
        self.searchCOMButton.setText("Search COMs")
        self.searchCOMButton.clicked.connect(self.search_com_ports)
        self.searchCOMButton.setGeometry(370, 30, 100, 30)

        # Connect button
        self.connectButton = QtWidgets.QPushButton(self)
        self.connectButton.setText("Connect")
        self.connectButton.clicked.connect(self.connect_to_port)
        self.connectButton.setGeometry(480, 30, 100, 30)

        # Disconnect button
        self.disconnectButton = QtWidgets.QPushButton(self)
        self.disconnectButton.setText("Disconnect")
        self.disconnectButton.clicked.connect(self.disconnect_from_port)
        self.disconnectButton.setGeometry(590, 30, 100, 30)

        # Load password database
        self.loadDBButton = QtWidgets.QPushButton(self)
        self.loadDBButton.setText("Load DB")
        self.loadDBButton.clicked.connect(self.load_database)
        self.loadDBButton.move(590, 95)

        # Upload database to device
        self.uploadDBButton = QtWidgets.QPushButton(self)
        self.uploadDBButton.setText("Upload DB")
        self.uploadDBButton.move(590, 125)

        # Add button to add a new entry
        self.addEntryButton = QtWidgets.QPushButton(self)
        self.addEntryButton.setText("Add Entry")
        self.addEntryButton.clicked.connect(self.add_entry)
        self.addEntryButton.move(590, 155)

        # Edit button to edit username of entry
        self.editDBButton = QtWidgets.QPushButton(self)
        self.editDBButton.setText("Edit Username")
        self.editDBButton.clicked.connect(self.edit_username_entry)
        self.editDBButton.move(590, 185)

        # Edit button to edit password of entry
        self.editPasswordButton = QtWidgets.QPushButton(self)
        self.editPasswordButton.setText("Edit Password")
        self.editPasswordButton.clicked.connect(self.edit_password_entry)
        self.editPasswordButton.move(590, 215)

        # Delete button to delete entries
        self.deleteEntryButton = QtWidgets.QPushButton(self)
        self.deleteEntryButton.setText("Delete Entry")
        self.deleteEntryButton.clicked.connect(self.delete_entry)
        self.deleteEntryButton.move(590, 245)

        # List of database entries
        self.dbEntriesList = QtWidgets.QListWidget(self)
        self.dbEntriesList.setGeometry(45, 95, 535, 270)
        self.dbEntriesList.setSelectionMode(QtWidgets.QAbstractItemView.SingleSelection)

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
    
    def load_database(self):
        # load database method
        print("Loading database...\n")
        self.serial.write(b"load\n")

        if self.serial.isOpen():
            try:
                data = self.serial.readAll().data().decode('utf-8')
                # Read database from serail
                lines = data.strip().split('\n')  # Assuming each entry is on a new line
                for line in lines:
                    parts = line.split(';')
                    if len(parts) == 3:
                        entryID, entryUSRNAME, entryPSW = parts
                        print(f"ID: {entryID}, Username: {entryUSRNAME}, Password: {entryPSW}")
                        self.entryID.append(entryID)
                        self.entryUSRNAME.append(entryUSRNAME)
                        self.entryPSW.append(entryPSW)
                        # Add entry to entries list
                        self.dbEntriesList.addItem(f"{entryID}   {entryUSRNAME}\t\t{entryPSW}")

            except Exception as e:
                    QtWidgets.QMessageBox.critical(self, "Error", f"Failed to read data: {str(e)}")
        else:
            QtWidgets.QMessageBox.warning(self, "Warning", "Serial port is not open")

    def add_entry(self):
        # Prompt user for entry details
        entry_id, ok1 = QtWidgets.QInputDialog.getText(self, "Add Entry", "Enter Entry ID:")
        if not ok1 or not entry_id.strip():
            QtWidgets.QMessageBox.warning(self, "Warning", "Wrong input")
            return
        if not entry_id.isdigit():
            QtWidgets.QMessageBox.warning(self, "Warning", "ID is not a number")
            return
        if entry_id in self.entryID:
            QtWidgets.QMessageBox.warning(self, "Warning", "ID must be unique")
            return

        username, ok2 = QtWidgets.QInputDialog.getText(self, "Add Entry", "Enter Username:")
        if not ok2 or not username.strip():
            QtWidgets.QMessageBox.warning(self, "Warning", "Wrong input")
            return

        password, ok3 = QtWidgets.QInputDialog.getText(self, "Add Entry", "Enter Password:")
        if not ok3 or not password.strip():
            QtWidgets.QMessageBox.warning(self, "Warning", "Wrong input")
            return

        # Append the new entry to the lists
        self.entryID.append(entry_id)
        self.entryUSRNAME.append(username)
        self.entryPSW.append(password)

        # Add the new entry to the display
        self.dbEntriesList.addItem(f"{entry_id}   {username}\t\t{password}")

    def edit_username_entry(self):
        selected_item = self.dbEntriesList.currentItem()
        if selected_item:
            selected_text = selected_item.text()
            parts = selected_text.split()
            if len(parts) >= 3:
                entry_index = self.dbEntriesList.currentRow()
                entryID = self.entryID[entry_index]
                entryPSW = self.entryPSW[entry_index]

                # Prompt user for new username and password
                new_username, ok1 = QtWidgets.QInputDialog.getText(self, "Edit Username", "Enter new username:")
                if not ok1:
                    return
                if not new_username.strip():
                    new_username = "-" # In case user only clicks OK

            # Update the entry in the lists
            self.entryUSRNAME[entry_index] = new_username

            # Update the display
            self.dbEntriesList.item(entry_index).setText(f"{entryID}   {new_username}\t\t{entryPSW}")
        else:
            QtWidgets.QMessageBox.warning(self, "Warning", "No entry selected")

    def edit_password_entry(self):
        selected_item = self.dbEntriesList.currentItem()
        if selected_item:
            selected_text = selected_item.text()
            parts = selected_text.split()
            if len(parts) >=3:
                entry_index = self.dbEntriesList.currentRow()
                entryID = self.entryID[entry_index]
                entryUSRNAME = self.entryUSRNAME[entry_index]

                # Prompt user for new password
                new_password, ok2 = QtWidgets.QInputDialog.getText(self, "Edit Password", "Enter new password:")
                if not ok2:
                    return
                if not new_password.strip():
                    new_password = "-"  # In case user only clicks OK

                # Update the entry in the lists
                self.entryPSW[entry_index] = new_password

                # Update the display
                self.dbEntriesList.item(entry_index).setText(f"{entryID}   {entryUSRNAME}\t\t{new_password}")
        else:
            QtWidgets.QMessageBox.warning(self, "Warning", "No entry selected")
    def delete_entry(self):
        selected_item = self.dbEntriesList.currentItem()
        if selected_item:
            entry_index = self.dbEntriesList.currentRow()

            # Remove the entry from the lists
            del self.entryID[entry_index]
            del self.entryUSRNAME[entry_index]
            del self.entryPSW[entry_index]

            # Remove the entry from the display
            self.dbEntriesList.takeItem(entry_index)
        else:
            QtWidgets.QMessageBox.warning(self, "Warning", "No entry selected")

def Window():
    app = QApplication(sys.argv)
    win = MainWindow()


    win.show()
    sys.exit(app. exec())

Window()
