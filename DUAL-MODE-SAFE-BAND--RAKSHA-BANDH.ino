#include <Wire.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();

#define BUZZER_PIN 18
#define BUTTON_PIN 19

TinyGPSPlus gps;
HardwareSerial SerialGPS(1); 
#define RXD2 16  
#define TXD2 17  

HardwareSerial SerialGSM(2); 
#define GSM_RX 26   
#define GSM_TX 27   
#define PHONE_NUMBER "+91xxxxxxxxxx"  


unsigned long lastButtonPress = 0;
int buttonPressCount = 0;

bool sosActive = false;
bool buzzerActive = false;
unsigned long buzzerStartTime = 0;

int shakeCount = 0;
unsigned long lastShakeTime = 0;
const int SHAKE_THRESHOLD = 15;   
const int REQUIRED_SHAKES = 5;      
const int SHAKE_TIMEOUT = 1500;    

void setup() {
  Serial.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, RXD2, TXD2);  
  SerialGSM.begin(9600, SERIAL_8N1, GSM_RX, GSM_TX); 

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 failed"));
    while(1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if(!accel.begin()) {
    Serial.println("ADXL345 not detected");
    while(1);
  }
  accel.setRange(ADXL345_RANGE_16_G);

  showMessage("System Ready");
  delay(2000);

  initGSM();
}

void loop() {
  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());
  }

  checkButton();
  checkAccelerometer();
  handleBuzzer();
}

void initGSM() {
  delay(1000);
  Serial.println("Initializing GSM...");
  SerialGSM.println("AT");
  delay(500);
  SerialGSM.println("AT+CMGF=1");  
  delay(500);
  SerialGSM.println("AT+CNMI=1,2,0,0,0");
  delay(500);
}

void checkButton() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(BUTTON_PIN);

  if (lastState == HIGH && currentState == LOW) {
    unsigned long now = millis();
    if (now - lastButtonPress < 400) {
      buttonPressCount++;
    } else {
      buttonPressCount = 1;
    }
    lastButtonPress = now;
  }
  lastState = currentState;

  if (buttonPressCount == 1 && (millis() - lastButtonPress > 500) && buzzerActive) {
    cancelSOS();
    buttonPressCount = 0;
  }

  if (buttonPressCount == 2 && (millis() - lastButtonPress > 500)) {
    sendSOS("Manual SOS Sent");
    buttonPressCount = 0;
  }
}

void checkAccelerometer() {
  sensors_event_t event; 
  accel.getEvent(&event);

  float x = abs(event.acceleration.x);
  float y = abs(event.acceleration.y);
  float z = abs(event.acceleration.z);

  if (x > SHAKE_THRESHOLD || y > SHAKE_THRESHOLD || z > SHAKE_THRESHOLD) {
    unsigned long now = millis();
    if (now - lastShakeTime < SHAKE_TIMEOUT) {
      shakeCount++;
    } else {
      shakeCount = 1;
    }
    lastShakeTime = now;
    Serial.println("Shake detected!");
    showMessage("Shake Detected");

    if (shakeCount >= REQUIRED_SHAKES && !sosActive) {
      triggerFalseDetection();
    }
  }
}

void triggerFalseDetection() {
  buzzerActive = true;
  buzzerStartTime = millis();
  digitalWrite(BUZZER_PIN, HIGH);
  showMessage("False Alert?\nPress Btn");
}

void handleBuzzer() {
  if (buzzerActive && millis() - buzzerStartTime >= 5000) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerActive = false;
    sendSOS("Auto SOS Sent");
  }
}

void cancelSOS() {
  digitalWrite(BUZZER_PIN, LOW);
  buzzerActive = false;
  sosActive = false;
  shakeCount = 0;
  showMessage("SOS Cancelled\nSystem Ready");
  Serial.println("SOS Cancelled");
}

void sendSOS(const char* msg) {
  sosActive = true;
  showMessage(msg);
  Serial.println(msg);

  if (!gps.location.isValid()) {
    showMessage("No GPS Fix\nWaiting...");
    Serial.println("SOS not sent: No GPS fix available");
    sosActive = false;
    return;
  }

  double lat = gps.location.lat();
  double lng = gps.location.lng();

  Serial.print("Latitude: "); Serial.println(lat, 6);
  Serial.print("Longitude: "); Serial.println(lng, 6);
  Serial.print("Google Maps: https://maps.google.com/?q=");
  Serial.print(lat, 6); Serial.print(","); Serial.println(lng, 6);

  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(msg);
  display.setCursor(0, 25);
  display.print("Lat: "); display.println(lat, 6);
  display.print("Lng: "); display.println(lng, 6);
  display.display();
  delay(3000);

  String smsText = "⚠ SOS Alert ⚠\n";
  smsText += "User: " + userName + "\n";
  smsText += String(msg) + "\n";
  smsText += "Lat: " + String(lat, 6) + "\n";
  smsText += "Lng: " + String(lng, 6) + "\n";
  smsText += "Map: https://maps.google.com/?q=" + String(lat, 6) + "," + String(lng, 6);

  sendSMS(PHONE_NUMBER, smsText);

  showMessage("System Ready");
  sosActive = false;
  shakeCount = 0;
}

void sendSMS(String number, String text) {
  Serial.println("Sending SMS...");
  SerialGSM.println("AT+CMGF=1");  
  delay(500);
  SerialGSM.print("AT+CMGS=\"");
  SerialGSM.print(number);
  SerialGSM.println("\"");
  delay(500);
  SerialGSM.print(text);
  delay(500);
  SerialGSM.write(26); 
  delay(5000);
  Serial.println("SMS Sent!");
}

void showMessage(const char* msg) {
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println(msg);
  display.display();
}
