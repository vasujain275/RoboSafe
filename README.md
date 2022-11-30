
# RoboSafe v2.0

A simple yet effective wireless controlled Locker using an Arduino Mega. It can be unlocked via App RIFD card, Fingerprint and stream live footage to your device.

## Materials Required

- Arduino Mega (or compatible)
- HC-05 Bluetooth Module
- RC522 RFID Reader
- Copper Hooker Wires
- Solenoid Lock 12v
- R307 Fingerprint Sensor
- NodeMCU ESP-32 Cam Module

## Connections

RFID Reader:

VCC - Pin 3.3v

GND - GND

Reset (RST) - Pin D49

SDA(SS) - Pin D53

MOSI - Pin D51

MISO - Pin D50

SCK - Pin D52

Bluetooth Module:

VCC - 5v

GND - GND

TX - D2 (RX)

RX - D3 (TX)

Relay:

Relay (NO) - Lock (+ve)

Relay (COM) - Adaptor +ve

Lock (-ve) - Adaptor -ve

IN - D7

GND - GND

VCC - 5v

OLED Display:

SCL - A21

SDA - A20

VCC - 5v

GND - GND

Others:

Green Light - D35

Red Light - D37

Buzzer - D6

## Authors

- [@vasujain2](https://github.com/vasujain2)
