Smart Safety Wearable (ESP32 + GPS + GSM + ADXL345 + OLED)

A Smart Safety Wearable designed for personal safety, especially for women, that sends SOS alerts with GPS location automatically during emergencies (fall detection, abnormal movement) or manually via a button press.

Features

Manual SOS Trigger: Double-press a button to send an SOS alert.

Automatic SOS Detection: Detects multiple shakes or abnormal motion using ADXL345 accelerometer.

False Alarm Prevention: Device vibrates/buzzes for 5 seconds before sending auto SOS, allowing cancellation.

GPS Location Sharing: Sends latitude, longitude, and Google Maps link via SMS.

OLED Display: Displays system status, GPS info, and alerts.

GSM Module Support: Sends SOS messages via SIM800/SIM900 or compatible modules.

Hardware Required

ESP32 Development Board

SIM800L/SIM900 GSM Module (or any AT-command-compatible GSM module)

Neo-6M GPS Module

ADXL345 Accelerometer (I2C)

0.96" OLED Display (I2C, SSD1306)

Push Button (SOS Trigger/Cancel)

Buzzer (Alert notification)

Jumper wires, breadboard or PCB

Pin Configuration
Component	ESP32 Pin
GPS TX	GPIO16
GPS RX	GPIO17
GSM TX	GPIO26
GSM RX	GPIO27
OLED SDA	GPIO21
OLED SCL	GPIO22
Buzzer	GPIO18
Button	GPIO19
Software Libraries Used

Install these libraries via Arduino IDE Library Manager:

Adafruit_GFX

Adafruit_SSD1306

Adafruit_Sensor

Adafruit_ADXL345_U

TinyGPS++

How It Works

Setup Phase:

Initializes GSM, GPS, OLED, and Accelerometer.

Displays "System Ready" on OLED.

Manual SOS:

Double press the button → Sends SOS with location via SMS.

Automatic SOS:

Detects multiple shakes (configurable threshold).

Triggers buzzer for 5 seconds → If not canceled → Sends SOS automatically.

SMS Format:

⚠ SOS Alert ⚠
Manual/Auto SOS Triggered
Lat: 12.345678
Lng: 98.765432
Map: https://maps.google.com/?q=12.345678,98.765432

Installation

Clone this repository:

git clone https://github.com/your-username/smart-safety-wearable.git


Open the .ino file in Arduino IDE.

Select ESP32 Dev Module in Tools > Board.

Connect and upload the code.

Future Improvements

Add IoT backend (MQTT/HTTP) for cloud-based tracking.

Add voice module for emergency call.

Add low-power optimization for longer battery life.
