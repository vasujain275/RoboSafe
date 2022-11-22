#include "Arduino.h"
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal.h>


const byte rxPin = 2;
const byte txPin = 3;
#define pinRST         5          // Configurable, see typical pin layout above
#define pinSS          53         // Configurable, see typical pin layout above
#define lockPin        8
#define gPin           7
#define rPin           4
#define buzzerPin      6

// LCD
const int rs = 22, en = 24, d4 = 26, d5 = 28, d6 = 30, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// RFID
byte readCard[4];
int successRead;
String orgkey = "93408390";
String nkey = "";
MFRC522 mfrc522(pinSS, pinRST);  // Create MFRC522 instance
MFRC522::MIFARE_Key key;

// Bluetooth
SoftwareSerial BTSerial(rxPin, txPin); // RX TX
String msg = "";
const String lkey = "2707";
const String ulkey = "0702";

// FingerPrint
// pin #2 is IN from sensor (Yellow wire)
// pin #3 is OUT from arduino  (Green wire)
SoftwareSerial mySerial(8, 9);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup()  
{
  // pinModes
  pinMode(lockPin, OUTPUT);
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  Serial.begin(9600);
  BTSerial.begin(9600);

  //rfid
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	Serial.println("RFID reading.............");
  pinMode(lockPin, OUTPUT);
  do{
    successRead = getID(mfrc522.uid.uidByte, mfrc522.uid.size);
  } while (!successRead);

  // FingerPrint
  Serial.begin(9600);
  // while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");
  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  finger.getTemplateCount();
    if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  } 
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 4);
  // Print a message to the LCD.
  lcd.print("hello, world!");
}

// Bluetooth
const char START_TOKEN = '?';
const char END_TOKEN = ';';
const char DELIMIT_TOKEN = '&';
const int CHAR_TIMEOUT = 20;

bool waitingForStartToken = true;
String messageBuffer = "";



void loop()                     
{
  // RFID Loop
  getID(mfrc522.uid.uidByte, mfrc522.uid.size);
  if (nkey==orgkey){
    Serial.print("Key Matched!");
    digitalWrite(lockPin, LOW);
    digitalWrite(gPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    delay(3000);
    digitalWrite(gPin, LOW);
    digitalWrite(lockPin, HIGH);
    nkey="";
  }

  // FingerPrint Loop
  getFingerprintIDez();
  delay(50);

  // handle Bluetooth link
  char nextData;
  if (BTSerial.available()) {

    // check for start of message
    if (waitingForStartToken) {
      do {
        nextData = BTSerial.read();
      } while((nextData != START_TOKEN) && BTSerial.available());
      if (nextData == START_TOKEN) {
        Serial.println("message start");
        waitingForStartToken = false;
      }
    }

    // read command
    if (!waitingForStartToken && BTSerial.available()){
      do {
        nextData = BTSerial.read();
        messageBuffer += nextData;
      } while((nextData != END_TOKEN) && BTSerial.available());

      // check if message complete
      if (nextData == END_TOKEN) {
        // remove last character
        messageBuffer = messageBuffer.substring(0, messageBuffer.length() - 1);
        Serial.println("message - " + messageBuffer);
        msg = messageBuffer;
        if (msg==ulkey){
          digitalWrite(lockPin, LOW);
          digitalWrite(gPin, HIGH);
          digitalWrite(buzzerPin, HIGH);
          delay(1000);
          digitalWrite(buzzerPin, LOW);
          delay(3000);
          digitalWrite(gPin, LOW);
          digitalWrite(lockPin, HIGH);
        }    
        else if (msg==lkey){
              digitalWrite(lockPin, HIGH);
              digitalWrite(rPin, HIGH);
              digitalWrite(buzzerPin, HIGH);
              delay(1000);
              digitalWrite(buzzerPin, LOW);
              delay(1000);
              digitalWrite(rPin, LOW);
        }
        messageBuffer = "";
        waitingForStartToken = true;
      }

      // check for char timeout
      if (messageBuffer.length() > CHAR_TIMEOUT) {
        Serial.println("message data timeout - " + messageBuffer);
        messageBuffer = "";
        waitingForStartToken = true;
      }
    }
  }

  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);
}


// RFID Function

int getID(byte *buffer, byte bufferSize) {
  String read_rfid="";
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  Serial.println("UID: ");
    for (byte i = 0; i < bufferSize; i++) {
        read_rfid=read_rfid + String(buffer[i], HEX);
    }
  Serial.println(read_rfid);
  nkey = read_rfid;
  return;
}


// FingerPrint Functions
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 

  return finger.fingerID;
}
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}