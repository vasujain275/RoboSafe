
# RoboSafe v1.0

A simple yet effective wireless controlled Locker using an Arduino UNO. It can be unlocked via Bluetooth and RIFD card.




## Materials Required
- Arduino Uno r3 (or compatible)
- HC-05 Bluetooth Module
- RC522 RFID Reader
- 220Ω Resistors
- 1kΩ Resistors
- Jumper Wires
- Solenoid Lock 12v

## Connections

RFID Reader:

VCC - Pin 3.3v

GND - GND

Reset (RST) - Pin D9

SDA(SS) - Pin D10

MOSI - Pin D11

MISO - Pin D12

SCK - Pin D13

LCD Screen:

VCC - 5v

GND - GND

SDA - Pin A4

SCL - Pin A5

Bluetooth Module:

VCC - 5v

GND - GND

TX - D2 (RX)

RX - D3 (TX)

Others:

Green Light - D7

Red Light - D4

Buzzer - D6


OLED Display:

SCL - A5

SDA - A4

VCC - 5v

GND - GND

Relay:

Relay (Red1) - Battery +ve

Relay (Middle) - Lock -ve

Lock (+ve) - Battery +ve

LockPin - D8

GND - GND

VCC - 5v


## Authors

- [@vasujain2](https://github.com/vasujain2)


 
