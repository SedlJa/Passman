# Passman

Passman is an embedded password manager with a custom hardware device and a desktop companion application. The project is developed as part of a master's thesis at CTU FEE and combines an ESP32-S3 based device, biometric authentication, local password storage, USB serial communication, and a Python desktop GUI.

The goal is to create a portable hardware password manager that stores credentials locally on the device and allows the user to view, edit, upload, and download the password database through a dedicated desktop application.

## Repository Structure

```text
HW/                 Hardware design files
  pcb/              KiCad PCB project, schematics, BOM and assembly outputs
  Fusion/           3D enclosure models and exported STL files
  block_diagram.svg System block diagram
  PassMan_schematic_v2.1.pdf
  pcb_v2.png
  pcb_2v1.png

SW/
  firmware/         ESP32-S3 firmware built with PlatformIO
  app/              Python / PySide6 desktop application

_docs/              Project documentation and supporting materials
```

## Hardware

The hardware part contains the custom PCB and enclosure design for the Passman device.

Main components:

- ESP32-S3-WROOM-1 module
- 128x64 OLED display with SH1106 controller
- Fingerprint sensor connected over UART
- Rotary encoder with push button
- USB-C connector for power, flashing, and serial communication
- Li-Po battery input and charging circuit
- MCP73831 Li-Po charger
- STBB1-APUR voltage regulator
- User LEDs and reset / boot buttons

Hardware files include:

- KiCad schematic and PCB project
- PDF schematic export
- BOM
- Assembly output
- PCB preview images
- Fusion 360 enclosure exports
- STL files for 3D printing

## Firmware

Firmware is located in `SW/firmware` and is built with PlatformIO.

Target board:

```ini
esp32-s3-devkitm-1
```

Framework:

```ini
Arduino
```

Main firmware features:

- Fingerprint based unlock
- PIN fallback after failed fingerprint attempts
- OLED menu system
- Rotary encoder navigation
- Local password database stored in SPIFFS
- USB serial communication with the desktop app
- Device information reporting
- Password database load, upload, save, and delete actions

Used libraries:

- Ai Esp32 Rotary Encoder
- Adafruit SH110X
- Adafruit GFX
- Adafruit Fingerprint Sensor Library
- ArduinoJson

### Build and Upload Firmware

```bash
cd SW/firmware
pio run
pio run -t upload
pio run -t uploadfs
```

Serial monitor:

```bash
pio device monitor -b 115200
```

## Desktop Application

The desktop app is located in `SW/app`. It is written in Python using PySide6 and communicates with the Passman device over a serial port.

Main app features:

- COM port search and connection handling
- Device information display
- Load password database from device
- Add, edit, and delete password entries
- Upload database back to device
- Password visibility toggle
- Password generator
- Local clearing of displayed data after disconnect

### Run the App

```bash
cd SW/app
python3 -m venv env
source env/bin/activate
pip install -r requirements.txt
python app.py
```

Python dependencies are listed in `SW/app/requirements.txt`.

## Basic Usage

1. Flash the firmware to the ESP32-S3.
2. Upload the SPIFFS filesystem data.
3. Unlock the device using fingerprint authentication or the PIN fallback.
4. Open the `Connect to App` menu on the device and confirm with the rotary encoder.
5. Start the desktop app.
6. Select the correct serial port and connect.
7. Use `Load DB` to read entries from the device.
8. Edit the database in the app.
9. Use `Upload DB` to send the database back to the device.
10. Save the database on the device from the firmware menu.

## Communication

The desktop application and firmware communicate over USB serial at `115200 baud`.

Supported commands include:

- `info` - read device information
- `load` - load database from the device
- `download` - send updated database to the device
- `disconnect` - close the app-device session

## Security Notice

This project is currently a prototype / thesis implementation. Before using it for real credentials, the security model should be reviewed and hardened.

Important points for future work:

- Do not commit private keys or secrets to the repository.
- Move encryption keys out of source code.
- Add a secure key provisioning process.
- Review database encryption and storage.
- Add stronger authentication and recovery handling.
- Add tests for the serial protocol and database parsing.

## Author

- Author: Bc. Jan Sedlak
- Supervisor: Ing. Ondrej Nentvich, Ph.D.
- University: Czech Technical University in Prague
- Faculty: Faculty of Electrical Engineering
- Field: Internet of Things Technology
- Department: Department of Radioelectronics
