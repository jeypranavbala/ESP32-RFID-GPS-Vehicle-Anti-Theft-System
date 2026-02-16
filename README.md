# ESP32 RFID Vehicle Anti-Theft System with GPS & Email Alerts

ESP32-based vehicle security system using RFID authentication, GPS tracking, and real-time email alerts over WiFi.

## Features

- RFID-based vehicle authorization (MFRC522)
- Ignition button monitoring
- Unauthorized start detection
- GPS location retrieval (NEO-6M)
- Email alert via SMTP (WiFi)
- Authorized RFID tag database
- Real-time security alerts

## Microcontroller

ESP32 Dev Module

## Hardware Components

- ESP32 Dev Board  
- MFRC522 RFID Reader  
- RFID Tags / Cards  
- u-blox NEO-6M GPS Module  
- Push Button (Ignition)  

## Pin Connections

### RFID MFRC522 (SPI)

| MFRC522 | ESP32 |
|--------|------|
SDA | GPIO 5 |
RST | GPIO 4 |
MOSI | GPIO 23 |
MISO | GPIO 19 |
SCK | GPIO 18 |
GND | GND |
3.3V | 3.3V |

### GPS (NEO-6M)

| GPS | ESP32 |
|-----|------|
TX | GPIO 16 |
RX | GPIO 17 |
VCC | 3.3V / 5V |
GND | GND |

### Ignition Button

| Button | ESP32 |
|-------|------|
One side | GPIO 25 |
Other side | GND |

## Operation

1. Scan authorized RFID card  
2. Press ignition button  
3. If unauthorized → email alert with GPS location  
4. If authorized → ignition allowed  

## Email Alert

System sends email when:

- Ignition ON without RFID
- Unauthorized RFID scanned

## Configuration

Edit credentials in code:

#define WIFI_SSID "..."
#define WIFI_PASSWORD "..."
#define sender_email "..."
#define sender_password "..."
#define Recipient_email "..."


## Applications

- Vehicle anti-theft
- Fleet tracking
- Smart ignition systems
- IoT vehicle security
