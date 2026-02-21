# pylint: disable=all

import sys
from PySide6 import QtWidgets, QtCore, QtGui
from PySide6.QtWidgets import QApplication, QMainWindow
from PySide6.QtSerialPort import QSerialPort, QSerialPortInfo
from PySide6.QtCore import QIODevice
from PySide6.QtGui import QIcon
import hashlib
import serial.tools.list_ports
from encryption import *
import random
import string


# search for avalable serial ports
available_ports = serial.tools.list_ports.comports()
available_ports_list = [port.device for port in available_ports]

class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.setGeometry(300, 200, 1100, 520)
        self.setWindowTitle("PassMan Application")
        self.setWindowIcon(QtGui.QIcon("img/blueSafe.png"))
        # Set the default font to "Sans Serif"
        font = QtGui.QFont("Sans Serif")
        app = QApplication.instance()
        if app:
            app.setFont(font)
        # Fix the window size
        self.setFixedSize(1050, 520)
        self.initUI()
        
        self.serial = QSerialPort(self)
        self.deviceConnected = False
        
        # Decrypted database
        self.entryID = []
        self.entryUSRNAME = []
        self.entryPSW = []

        # device info
        self.deviceName = ""
        self.fwVersion = ""
        self.hwVersion = ""
        self.serialNumber = ""
        
        # Set background color to dark blue
        palette = self.palette()
        palette.setColor(self.backgroundRole(), QtGui.QColor(17, 21, 41))  # RGB for dark 
        self.setPalette(palette)
        
    def initUI(self):
        """
            Initialization UI
        """
        self.comPortDropdown = QtWidgets.QComboBox(self)
        self.comPortDropdown.setGeometry(45, 32, 310, 30)
        self.comPortDropdown.clear()

        # Search available COM ports button
        self.searchCOMButton = QtWidgets.QPushButton(self)
        self.searchCOMButton.setText("Search COMs")
        self.searchCOMButton.clicked.connect(self.search_com_ports)
        self.searchCOMButton.setGeometry(370, 30, 100, 30)
        self.searchCOMButton.setStyleSheet("""
            QPushButton {
            background-color: #2f3654; /* Dodger blue */
            color: white;
            border: none;
            border-radius: 5px; /* Makes the button rounded */
            font-weight: bold;
            }
            QPushButton:hover {
            background-color: #1c86ee; /* Slightly darker blue on hover */
            }
        """)

        # Load available ports from the start of the application
        for port in reversed(available_ports_list):
            self.comPortDropdown.addItem(port)

        # Connect button
        self.connectButton = QtWidgets.QPushButton(self)
        self.connectButton.setText("Connect")
        self.connectButton.clicked.connect(self.connect_to_port)
        self.connectButton.setGeometry(480, 30, 100, 30)
        self.connectButton.setStyleSheet("""
            QPushButton {
            background-color: #0b6623; /* Dark green */
            color: white;
            border: none;
            border-radius: 5px; /* Makes the button rounded */
            font-weight: bold;
            }
            QPushButton:hover {
            background-color: #065e1f; /* Even darker green on hover */
            }
        """)

        # Disconnect button
        self.disconnectButton = QtWidgets.QPushButton(self)
        self.disconnectButton.setText("Disconnect")
        self.disconnectButton.clicked.connect(self.disconnect_from_port)
        self.disconnectButton.setGeometry(590, 30, 100, 30)
        self.disconnectButton.setStyleSheet("""
            QPushButton {
            background-color: #7d1010;
            color: white;
            border: none;
            border-radius: 8px; /* Makes the button rounded */
            font-weight: bold;
            }
            QPushButton:hover {
            background-color: darkred; /* Darker red on hover */
            }
        """)

        # --- Device Info Section ---
        self.deviceInfoGroupBox = QtWidgets.QGroupBox("Device Information", self)
        self.deviceInfoGroupBox.setGeometry(800, 75, 200, 140)
        self.deviceInfoGroupBox.setStyleSheet(
            "QGroupBox { background-color: #1b2038; color: white; border: 1px solid #555; border-radius: 5px; margin-top: 10px; padding-top: 10px; }"
            "QGroupBox::title { subcontrol-origin: margin; left: 0px; padding: 5px 42px; background-color: #2f3654; color: white; border-radius: 5px; }"
        )
        info_layout = QtWidgets.QVBoxLayout(self.deviceInfoGroupBox)
        info_layout.setContentsMargins(8, 15, 8, 8)
        info_layout.setSpacing(4)
        # Refresh device info button
        self.refreshDeviceInfoButton = QtWidgets.QPushButton(self.deviceInfoGroupBox)
        self.refreshDeviceInfoButton.setIcon(QtGui.QIcon("img/refresh.png"))  # Assuming you have a refresh icon at this path
        self.refreshDeviceInfoButton.setToolTip("Refresh Device Info")
        self.refreshDeviceInfoButton.clicked.connect(self.refresh_device_info)
        self.refreshDeviceInfoButton.setStyleSheet("""
            QPushButton {
            background-color: #2f3654;
            color: white;
            border: none;
            border-radius: 5px;
            font-weight: bold;
            }
            QPushButton:hover {
            background-color: #1c86ee;
            }
        """)
        self.refreshDeviceInfoButton.setFixedSize(30, 30)
        self.refreshDeviceInfoButton.clicked.connect(self.update_device_info_display)
        info_layout.addWidget(self.refreshDeviceInfoButton, alignment=QtCore.Qt.AlignRight)

        self.deviceNameLabel = QtWidgets.QLabel("Name: N/A")
        self.deviceNameLabel.setStyleSheet("color: white; font-size: 11px;")
        info_layout.addWidget(self.deviceNameLabel)

        self.fwVersionLabel = QtWidgets.QLabel("FW: N/A")
        self.fwVersionLabel.setStyleSheet("color: white; font-size: 11px;")
        info_layout.addWidget(self.fwVersionLabel)

        self.hwVersionLabel = QtWidgets.QLabel("HW: N/A")
        self.hwVersionLabel.setStyleSheet("color: white; font-size: 11px;")
        info_layout.addWidget(self.hwVersionLabel)

        self.serialNumberLabel = QtWidgets.QLabel("SN: N/A")
        self.serialNumberLabel.setStyleSheet("color: white; font-size: 11px;")
        info_layout.addWidget(self.serialNumberLabel)

        self.connectionStatusLabel = QtWidgets.QLabel("Status: Disconnected")
        self.connectionStatusLabel.setStyleSheet("color: #ff6666; font-size: 11px; font-weight: bold;")
        info_layout.addWidget(self.connectionStatusLabel)

        # Right-side button X position
        btn_x = 800
        btn_width = 200

        # --- Database Section ---
        self.databaseGroupBox = QtWidgets.QGroupBox("Database", self)
        self.databaseGroupBox.setGeometry(btn_x, 220, btn_width, 140)
        self.databaseGroupBox.setStyleSheet(
            "QGroupBox { background-color: #1b2038; color: white; border: 1px solid #555; border-radius: 5px; margin-top: 10px; padding-top: 10px; }"
            "QGroupBox::title { subcontrol-origin: margin; left: 0px; padding: 5px 70px; background-color: #2f3654; color: white; border-radius: 5px; }"
        )

        db_layout = QtWidgets.QVBoxLayout(self.databaseGroupBox)
        db_layout.setContentsMargins(8, 8, 8, 8)
        db_layout.setSpacing(3)

        # Add a new entry
        self.addEntryButton = QtWidgets.QPushButton(self.databaseGroupBox)
        self.addEntryButton.setText("Add Entry")
        self.addEntryButton.clicked.connect(self.add_entry)
        self.addEntryButton.setStyleSheet("""
            QPushButton {
            background-color: #1e90ff;
            color: white;
            border: none;
            border-radius: 5px;
            font-weight: bold;
            }
            QPushButton:hover {
            background-color: #1c86ee;
            }
        """)
        db_layout.addWidget(self.addEntryButton)

        # Load password database
        self.loadDBButton = QtWidgets.QPushButton("Load DB", self.databaseGroupBox)
        self.loadDBButton.clicked.connect(self.load_database)
        self.loadDBButton.setStyleSheet("""
            QPushButton {
            background-color: #2f3654; /* Dodger blue */
            color: white;
            border: none;
            border-radius: 5px; /* Makes the button rounded */
            font-weight: bold;
            }
            QPushButton:hover {
            background-color: #1c86ee; /* Slightly darker blue on hover */
            }
        """)
        db_layout.addWidget(self.loadDBButton)

        # Upload database to device
        self.uploadDBButton = QtWidgets.QPushButton("Upload DB", self.databaseGroupBox)
        self.uploadDBButton.clicked.connect(self.upload_database)
        self.uploadDBButton.setStyleSheet("""
            QPushButton {
            background-color: #2f3654;
            color: white;
            border: none;
            border-radius: 5px;
            font-weight: bold;
            }
            QPushButton:hover {
            background-color: #1c86ee;
            }
        """)
        db_layout.addWidget(self.uploadDBButton)

        # Edit button to edit username and password of an entry
        self.editEntryButton = QtWidgets.QPushButton(self.databaseGroupBox)
        self.editEntryButton.setText("Edit Entry")
        self.editEntryButton.clicked.connect(self.edit_entry)
        self.editEntryButton.setStyleSheet("""
            QPushButton {
            background-color: #2f3654;
            color: white;
            border: none;
            border-radius: 5px;
            font-weight: bold;
            }
            QPushButton:hover {
            background-color: #1c86ee;
            }
        """)
        db_layout.addWidget(self.editEntryButton)

        # --- Danger Zone Section ---
        self.dangerZoneGroupBox = QtWidgets.QGroupBox("Danger Zone", self)
        self.dangerZoneGroupBox.setGeometry(btn_x, 365, btn_width, 100)
        self.dangerZoneGroupBox.setStyleSheet(
            "QGroupBox { background-color: #1b2038; color: white; border: 1px solid #555; border-radius: 5px; margin-top: 10px; padding-top: 10px; }"
            "QGroupBox::title { subcontrol-origin: margin; left: 0px; padding: 5px 59px; background-color: #7d1010; color: white; border-radius: 5px; }"
        )

        danger_layout = QtWidgets.QVBoxLayout(self.dangerZoneGroupBox)
        danger_layout.setContentsMargins(8, 15, 8, 8)
        danger_layout.setSpacing(4)

        # Delete button to delete ENTRIES
        self.deleteEntryButton = QtWidgets.QPushButton(self.dangerZoneGroupBox)
        self.deleteEntryButton.setText("Delete Entry")
        self.deleteEntryButton.clicked.connect(self.delete_entry)
        self.deleteEntryButton.setStyleSheet("""
            QPushButton {
            background-color: #7d1010; /* Red */
            color: white;
            border: none;
            border-radius: 5px;
            font-weight: bold;
            }
            QPushButton:hover {
            background-color: darkred; /* Darker red on hover */
            }
        """)
        danger_layout.addWidget(self.deleteEntryButton)

        # Button to delete ENTIRE database
        self.deleteDBButton = QtWidgets.QPushButton(self.dangerZoneGroupBox)
        self.deleteDBButton.setText("Delete ALL")
        self.deleteDBButton.clicked.connect(self.delete_database)
        self.deleteDBButton.setStyleSheet("""
            QPushButton {
            background-color: #7d1010; /* Red */
            color: white;
            border: none;
            border-radius: 5px;
            font-weight: bold;
            }
            QPushButton:hover {
            background-color: darkred; /* Darker red on hover */
            }
        """)
        danger_layout.addWidget(self.deleteDBButton)

        # Toggle button - password visibility
        self.togglePasswordButton = QtWidgets.QPushButton(self)
        self.togglePasswordButton.setText("Toggle Visibility")
        self.togglePasswordButton.clicked.connect(self.toggle_password_visibility)
        self.togglePasswordButton.setGeometry(btn_x, 475, btn_width, 20)
        self.togglePasswordButton.setStyleSheet("""
            QPushButton {
            background-color: #2f3654;
            color: white;
            border: none;
            border-radius: 5px;
            font-weight: bold;
            }
            QPushButton:hover {
            background-color: #1c86ee;
            }
        """)

        # Generator button
        self.generatorButton = QtWidgets.QPushButton(self.databaseGroupBox)
        self.generatorButton.setText("Generate PSW")
        self.generatorButton.clicked.connect(self.generate_password)
        self.generatorButton.setStyleSheet("""
            QPushButton {
            background-color: #2f3654;
            color: white;
            border: none;
            border-radius: 5px;
            font-weight: bold;
            }
            QPushButton:hover {
            background-color: #1c86ee;
            }
        """)
        db_layout.addWidget(self.generatorButton)

        # Lists for database entries
        self.idList = QtWidgets.QListWidget(self)
        self.idList.setStyleSheet("background-color: #1b2038; color: white;")
        self.idList.setGeometry(45, 95, 100, 400)
        self.idList.setSelectionMode(QtWidgets.QAbstractItemView.SingleSelection)
        self.idList.setFont(QtGui.QFont("Courier", 11))

        self.usernameList = QtWidgets.QListWidget(self)
        self.usernameList.setStyleSheet("background-color: #1b2038; color: white;")
        self.usernameList.setGeometry(150, 95, 300, 400)
        self.usernameList.setSelectionMode(QtWidgets.QAbstractItemView.SingleSelection)
        self.usernameList.setFont(QtGui.QFont("Courier", 11))

        self.passwordList = QtWidgets.QListWidget(self)
        self.passwordList.setStyleSheet("background-color: #1b2038; color: white;")
        self.passwordList.setGeometry(455, 95, 340, 400)
        self.passwordList.setSelectionMode(QtWidgets.QAbstractItemView.SingleSelection)
        self.passwordList.setFont(QtGui.QFont("Courier", 11))

        # Labels for the entries list
        self.idLabel = QtWidgets.QLabel(self)
        self.idLabel.setText("ID:")
        self.idLabel.setStyleSheet("background-color: #2f3654; color: white; border-bottom: 1px solid white;")
        self.idLabel.setGeometry(45, 75, 100, 20)

        self.usernameLabel = QtWidgets.QLabel(self)
        self.usernameLabel.setText("USERNAME:")
        self.usernameLabel.setStyleSheet("background-color: #2f3654; color: white; border-bottom: 1px solid white;")
        self.usernameLabel.setGeometry(150, 75, 300, 20)

        self.passwordLabel = QtWidgets.QLabel(self)
        self.passwordLabel.setText("PASSWORD:")
        self.passwordLabel.setStyleSheet("background-color: #2f3654; color: white; border-bottom: 1px solid white;")
        self.passwordLabel.setGeometry(455, 75, 340, 20)

    def update_device_info_display(self):
        """
            Update the device info labels with current device information
        """
        self.deviceNameLabel.setText(f"Name: {self.deviceName or 'N/A'}")
        self.fwVersionLabel.setText(f"FW: {self.fwVersion or 'N/A'}")
        self.hwVersionLabel.setText(f"HW: {self.hwVersion or 'N/A'}")
        self.serialNumberLabel.setText(f"SN: {self.serialNumber or 'N/A'}")

        if self.deviceConnected:
            self.connectionStatusLabel.setText("Status: Connected")
            self.connectionStatusLabel.setStyleSheet("color: #66ff66; font-size: 11px; font-weight: bold;")
        else:
            self.connectionStatusLabel.setText("Status: Disconnected")
            self.connectionStatusLabel.setStyleSheet("color: #ff6666; font-size: 11px; font-weight: bold;")

    def search_com_ports(self):
        """
            Search for available ports
        """
        self.comPortDropdown.clear()
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.comPortDropdown.addItem(port.device)

    def connect_to_port(self):
        """
            Connect app to serial port
        """
        selected_port = self.comPortDropdown.currentText()
        if selected_port:
            self.serial.setPortName(selected_port)
            self.serial.setBaudRate(QSerialPort.Baud115200)
            self.serial.setDataBits(QSerialPort.Data8)
            self.serial.setParity(QSerialPort.NoParity)
            self.serial.setStopBits(QSerialPort.OneStop)
            self.serial.setFlowControl(QSerialPort.NoFlowControl)
            # Device connection handler
            self.deviceConnected = True

            if self.serial.open(QIODevice.OpenModeFlag.ReadWrite):
                QtWidgets.QMessageBox.information(self, "Connection", f"Connected to {selected_port}")

                # Read device information
                QtCore.QThread.msleep(100)  # Allow some time for the device to send data
                data = self.serial.readAll().data().decode('utf-8')
                lines = data.strip().split('\n')
                for line in lines:
                    if line.startswith("Name: "):
                        self.deviceName = line.split("Name: ")[1].strip()
                    elif line.startswith("FW: "):
                        self.fwVersion = line.split("FW: ")[1].strip()
                    elif line.startswith("HW: "):
                        self.hwVersion = line.split("HW: ")[1].strip()
                    elif line.startswith("SN: "):
                        self.serialNumber = line.split("SN: ")[1].strip()

                # Update the device info section in the UI
                self.update_device_info_display()

            else:
                self.deviceConnected = False
                self.update_device_info_display()
                QtWidgets.QMessageBox.critical(self, "Error", f"Failed to connect to {selected_port}")

    def disconnect_from_port(self):
        """
            Disconnect app from serial port
        """
        if self.serial.isOpen():
            # Send "disconnect" command
            self.serial.write(b"disconnect\n")
            QtCore.QThread.msleep(200)  # Add a delay to ensure the command is processed
            
            
            # Clear database for enhanced security while the device is disconnected
            self.entryID.clear()
            self.entryUSRNAME.clear()
            self.entryPSW.clear()
            self.idList.clear()
            self.usernameList.clear()
            self.passwordList.clear()
            
            # Device connection handler
            self.deviceConnected = False

            # Clear device info
            self.deviceName = ""
            self.fwVersion = ""
            self.hwVersion = ""
            self.serialNumber = ""
            self.update_device_info_display()
            
            QtWidgets.QMessageBox.information(self, "Disconnection", "Disconnected from the serial port")
        else:
            QtWidgets.QMessageBox.warning(self, "Warning", "Serial port is not open")
        self.serial.close()
        
    def load_database(self):
        """
            Load database from PassMan device
            Each entry is loaded encrypted - decrpytion is a part of this function
        """
        self.serial.write(b"load\n")
        
        if self.serial.isOpen():
            try:
                data = self.serial.readAll().data().decode('utf-8')
                # Read database from serail
                lines = data.strip().split('\n')  # Assuming each entry is on a new line
                for line in lines:
                    parts = line.split(';')
                    if len(parts) == 3:
                        try:
                            # Use the incoming data as is (no decryption)
                            entryID = parts[0].strip()
                            entryUSRNAME = parts[1].strip()
                            entryPSW = parts[2].strip()
                            print(f"ID: {entryID}, Username: {entryUSRNAME}, Password: {entryPSW}")

                            # Decode the base64-encoded strings and decrypt them
                            entryID = decrypt_data((entryID))
                            entryUSRNAME = decrypt_data((entryUSRNAME))
                            entryPSW = decrypt_data((entryPSW))

                            # Save decrypted entries to variables
                            self.entryID.append(entryID)
                            self.entryUSRNAME.append(entryUSRNAME)
                            self.entryPSW.append(entryPSW)

                        except Exception as e:
                            QtWidgets.QMessageBox.critical(self, "Decryption Error", f"Failed to decrypt data: {str(e)}")
                            continue

                        # Add each part of the entry to the respective list
                        self.idList.addItem(entryID)
                        self.usernameList.addItem(entryUSRNAME)
                        self.passwordList.addItem(entryPSW)

                        # Update password list to show inputted passwords as 12 asterisks
                        for i in range(self.passwordList.count()):
                            item = self.passwordList.item(i)
                            item.setText("•" * 12)

            except Exception as e:
                    QtWidgets.QMessageBox.critical(self, "Error", f"Failed to read data: {str(e)}")
        else:
            QtWidgets.QMessageBox.warning(self, "Warning", "Serial port is not open")

    def upload_database(self):
        """
            Function to upload password database to a PassMan device
        """
        # Correct IDs to be in order (1, 2, 3, ...)
        self.entryID = [str(i + 1) for i in range(len(self.entryID))]
        
        # Update the display with corrected IDs
        self.idList.clear()

        for entry_id in self.entryID:
            self.idList.addItem(entry_id)

        self.serial.write(b"download\n") # download command
        db_length = len(self.entryID) # get db length and send it
        self.serial.write(f"{db_length}\n".encode('utf-8'))

        if(self.serial.isOpen()):
            try:
                # Prepare the database content
                if not self.entryID:
                    QtWidgets.QMessageBox.warning(self, "Warning", "Database is empty")
                    return
                
                database_content = ""
                for i in range(len(self.entryID)):
                    """+= f"{encrypt_data(self.entryID[i])};{encrypt_data(self.entryUSRNAME[i])};{encrypt_data(self.entryPSW[i])}\n"""
                    database_content += f"{(self.entryID[i])};{(self.entryUSRNAME[i])};{(self.entryPSW[i])}\n"
                    
                # Send the database content over the serial port
                self.serial.write(database_content.encode('utf-8'))
                self.serial.write("end".encode("utf-8"))
                print(database_content)

            except Exception as e:
                QtWidgets.QMessageBox.critical(self, "Error", f"Failed to upload data to device: {str(e)}")
        else:
            QtWidgets.QMessageBox.warning(self, "Warning", "Serial port is not open")

    def add_entry(self):
        """
            Add password to app database
        """
        if self.deviceConnected:
            # Prompt user for entry details in a single dialog
            dialog = QtWidgets.QDialog(self)
            dialog.setWindowTitle("Add Entry")
            dialog.setModal(True)
            dialog.setFixedSize(400, 300)

            layout = QtWidgets.QVBoxLayout(dialog)

            last_id = max(map(int, self.entryID), default=0)
            new_id = last_id + 1
            id_label = QtWidgets.QLabel(f"ID: {new_id}")
            layout.addWidget(id_label)

            username_label = QtWidgets.QLabel("Username:")
            username_input = QtWidgets.QLineEdit()
            username_input.setMinimumHeight(30)  # Make the input box larger
            layout.addWidget(username_label)
            layout.addWidget(username_input)

            password_label = QtWidgets.QLabel("Password:")
            password_input = QtWidgets.QLineEdit()
            password_input.setEchoMode(QtWidgets.QLineEdit.Password)
            password_input.setMinimumHeight(30)  # Make the input box larger
            layout.addWidget(password_label)
            layout.addWidget(password_input)

            # Add a checkbox to toggle password visibility
            show_password_checkbox = QtWidgets.QCheckBox("Show Password")
            layout.addWidget(show_password_checkbox)

            def toggle_password_visibility():
                if show_password_checkbox.isChecked():
                    password_input.setEchoMode(QtWidgets.QLineEdit.Normal)
                else:
                    password_input.setEchoMode(QtWidgets.QLineEdit.Password)

            show_password_checkbox.stateChanged.connect(toggle_password_visibility)

            button_box = QtWidgets.QDialogButtonBox(QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel)
            layout.addWidget(button_box)

            def on_accept():
                entry_id = str(new_id).strip()
                username = username_input.text().strip()
                password = password_input.text().strip()

                if not entry_id:
                    QtWidgets.QMessageBox.warning(self, "Warning", "Entry ID cannot be empty")
                    return
                if not entry_id.isdigit():
                    QtWidgets.QMessageBox.warning(self, "Warning", "Entry ID must be a number")
                    return
                if entry_id in self.entryID:
                    QtWidgets.QMessageBox.warning(self, "Warning", "Entry ID must be unique")
                    return
                if not username:
                    QtWidgets.QMessageBox.warning(self, "Warning", "Username cannot be empty")
                    return
                if not password:
                    QtWidgets.QMessageBox.warning(self, "Warning", "Password cannot be empty")
                    return

                # Add each part to entry list
                self.entryID.append(entry_id)
                self.entryUSRNAME.append(username)
                self.entryPSW.append(password)

                # Add each part of the entry to the respective list
                self.idList.addItem(entry_id)
                self.usernameList.addItem(username)
                self.passwordList.addItem("•" * 12)

                dialog.accept()

            button_box.accepted.connect(on_accept)
            button_box.rejected.connect(dialog.reject)

            dialog.exec()
        else:
            QtWidgets.QMessageBox.warning(self, "Error", "No device connected")

    def edit_entry(self):
        """
            Edit both username and password of an entry
        """
        selected_item = self.idList.currentItem()
        if selected_item:
            entry_index = self.idList.currentRow()

            # Prompt user for new username and password in a single dialog
            dialog = QtWidgets.QDialog(self)
            dialog.setWindowTitle("Edit Entry")
            dialog.setModal(True)
            dialog.setFixedSize(400, 350)

            layout = QtWidgets.QVBoxLayout(dialog)

            id_label = QtWidgets.QLabel(f"ID: {self.entryID[entry_index]}")
            layout.addWidget(id_label)

            username_label = QtWidgets.QLabel("Username:")
            username_input = QtWidgets.QLineEdit()
            username_input.setText(self.entryUSRNAME[entry_index])
            username_input.setMinimumHeight(30)
            layout.addWidget(username_label)
            layout.addWidget(username_input)

            password_label = QtWidgets.QLabel("Password:")
            password_input = QtWidgets.QLineEdit()
            password_input.setText(self.entryPSW[entry_index])
            password_input.setEchoMode(QtWidgets.QLineEdit.Password)
            password_input.setMinimumHeight(30)
            layout.addWidget(password_label)
            layout.addWidget(password_input)

            # Add a checkbox to toggle password visibility
            show_password_checkbox = QtWidgets.QCheckBox("Show Password")
            layout.addWidget(show_password_checkbox)

            def toggle_password_visibility():
                if show_password_checkbox.isChecked():
                    password_input.setEchoMode(QtWidgets.QLineEdit.Normal)
                else:
                    password_input.setEchoMode(QtWidgets.QLineEdit.Password)

            show_password_checkbox.stateChanged.connect(toggle_password_visibility)

            button_box = QtWidgets.QDialogButtonBox(QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel)
            layout.addWidget(button_box)

            def on_accept():
                new_username = username_input.text().strip()
                new_password = password_input.text().strip()

                if not new_username:
                    QtWidgets.QMessageBox.warning(self, "Warning", "Username cannot be empty")
                    return
                if not new_password:
                    QtWidgets.QMessageBox.warning(self, "Warning", "Password cannot be empty")
                    return

                # Update the entry in the lists
                self.entryUSRNAME[entry_index] = new_username
                self.entryPSW[entry_index] = new_password

                # Update the display
                self.usernameList.item(entry_index).setText(new_username)
                self.passwordList.item(entry_index).setText("•" * 12)

                dialog.accept()

            button_box.accepted.connect(on_accept)
            button_box.rejected.connect(dialog.reject)

            dialog.exec()
        else:
            QtWidgets.QMessageBox.warning(self, "Warning", "No entry selected")

    def delete_entry(self):
        """
            Delete entry
        """
        selected_item = self.idList.currentItem()
        if selected_item:
            entry_index = self.idList.currentRow()

            if 0 <= entry_index < len(self.entryID):
                # Remove the entry from all lists
                del self.entryID[entry_index]
                del self.entryUSRNAME[entry_index]
                del self.entryPSW[entry_index]

                # Remove the entry from the display
                self.idList.takeItem(entry_index)
                self.usernameList.takeItem(entry_index)
                self.passwordList.takeItem(entry_index)
            else:
                QtWidgets.QMessageBox.warning(self, "Warning", "Invalid entry index")
        else:
            QtWidgets.QMessageBox.warning(self, "Warning", "No entry selected")

    def delete_database(self):
        """
            Delete entire database
        """
        confirmation = QtWidgets.QMessageBox.question(
            self,
            "Delete All",
            "Are you sure you want to delete the entire database?",
            QtWidgets.QMessageBox.Yes | QtWidgets.QMessageBox.No
        )

        if confirmation == QtWidgets.QMessageBox.Yes:
            # Clear all entries from the lists
            self.entryID.clear()
            self.entryUSRNAME.clear()
            self.entryPSW.clear()

            # Clear the display
            self.idList.clear()
            self.usernameList.clear()
            self.passwordList.clear()

            QtWidgets.QMessageBox.information(self, "Delete All", "The entire database has been deleted.")
        
    def toggle_password_visibility(self):
        """
            Toggle password visibility in the password list
        """
        for i in range(self.passwordList.count()):
            item = self.passwordList.item(i)
            if item.text().startswith("•"):  # Check if the password is hidden
                item.setText(self.entryPSW[i])  # Show the actual password
            else:
                item.setText("•" * 12)  # Hide the password

    def generate_password(self):
        """
            Generate random password
        """
        while True:
            # Generate a random password
            length, ok = QtWidgets.QInputDialog.getInt(self, "Generate Password", "Enter password length:", 12, 8, 32)
            if not ok:
                return

            characters = string.ascii_letters + string.digits + string.punctuation
            password = ''.join(random.choice(characters) for _ in range(length))

            # Ask user if they want to regenerate or copy to clipboard
            msg_box = QtWidgets.QMessageBox(self)
            msg_box.setWindowTitle("Generated Password")
            msg_box.setText(f"Password: {password}")
            msg_box.setInformativeText("Do you want to regenerate a new password or copy this one to clipboard?")
            regenerate_button = msg_box.addButton("Regenerate", QtWidgets.QMessageBox.ActionRole)
            copy_button = msg_box.addButton("Copy to Clipboard", QtWidgets.QMessageBox.ActionRole)
            cancel_button = msg_box.addButton("Cancel", QtWidgets.QMessageBox.RejectRole)
            msg_box.exec()

            if msg_box.clickedButton() == regenerate_button:
                continue
            elif msg_box.clickedButton() == copy_button:
                clipboard = QApplication.clipboard()
                clipboard.setText(password)
                break
            else:
                break
    
    def refresh_device_info(self):
        """
            Refresh device information by sending "info" command to the device
        """
        if self.serial.isOpen():
            try:
                self.serial.write(b"info\n")  # Send the "info" command
                QtCore.QThread.msleep(100)  # Allow some time for the device to respond
                data = self.serial.readAll().data().decode('utf-8')
                lines = data.strip().split('\n')
                
                for line in lines:
                    if line.startswith("Name: "):
                        self.deviceName = line.split("Name: ")[1].strip()
                    elif line.startswith("FW: "):
                        self.fwVersion = line.split("FW: ")[1].strip()
                    elif line.startswith("HW: "):
                        self.hwVersion = line.split("HW: ")[1].strip()
                    elif line.startswith("SN: "):
                        self.serialNumber = line.split("SN: ")[1].strip()

                # Update the device info section in the UI
                self.update_device_info_display()

            except Exception as e:
                QtWidgets.QMessageBox.critical(self, "Error", f"Failed to refresh device info: {str(e)}")
        else:
            QtWidgets.QMessageBox.warning(self, "Warning", "Serial port is not open")

def Window():
    app = QApplication(sys.argv)
    win = MainWindow()

    win.show()
    sys.exit(app. exec())

Window()
