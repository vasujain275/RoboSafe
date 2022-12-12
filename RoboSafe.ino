#include "Arduino.h"
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_Fingerprint.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define mySerial Serial3
#define BTSerial Serial1

// Fingerprint
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2/19 is IN from sensor (GREEN/Yellow wire) (RX)
// pin #3/18 is OUT from arduino  (WHITE/Green wire) (TX)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(15, 14);
#else

// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#endif
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);



#define pinRST         43         // Configurable, see typical pin layout above
#define pinSS          53         // Configurable, see typical pin layout above
#define lockPin        24
#define gPin           36
#define rPin           28
#define buzzerPin      22
#define delay1         1000
#define delay2         3000

// Oled 
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// RFID
byte readCard[4];
int successRead;
String orgkey1 = "93408390";
String orgkey2 = "33e5c294";
String nkey = "";
MFRC522 mfrc522(pinSS, pinRST);  // Create MFRC522 instance
MFRC522::MIFARE_Key key;

// Bluethooth
String msg = "";
const String lkey = "2707";
const String ulkey = "0702";

void setup()  
{
  // pinModes
  pinMode(lockPin, OUTPUT);
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  BTSerial.begin(9600);

  // oled
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(" RoboSafe    v2.0");
  display.display();

  // Fingerprint
  finger.begin(57600);
  delay(5);
  
  //rfid
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	Serial.println("RFID reading.............");
  do{
    successRead = getID(mfrc522.uid.uidByte, mfrc522.uid.size);
  } while (!successRead);

  

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
  // Oled
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(" RoboSafe    v2.0"); 
  display.display();

  // Fingerprint loop
  if ( getFingerPrint() != -1) 
  {
    Serial.print("Key Matched!");
    digitalWrite(lockPin, LOW);
    digitalWrite(gPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("   Door    Unlocked");
    display.display();
    delay(delay1);
    digitalWrite(buzzerPin, LOW);
    delay(delay2);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println(" RoboSafe    v2.0");
    display.display();
    digitalWrite(gPin, LOW);
    digitalWrite(lockPin, HIGH);
  }  
  delay(50);     

  // RFID Loop
  getID(mfrc522.uid.uidByte, mfrc522.uid.size);
  if (nkey==orgkey1 || nkey==orgkey2){
    Serial.print("Key Matched!");
    digitalWrite(lockPin, LOW);
    digitalWrite(gPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("   Door    Unlocked");
    display.display();
    delay(delay1);
    digitalWrite(buzzerPin, LOW);
    delay(delay2);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println(" RoboSafe    v2.0");
    display.display();
    digitalWrite(gPin, LOW);
    digitalWrite(lockPin, HIGH);
    nkey="";
  }

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
          Serial.print("Key Matched!");
          digitalWrite(lockPin, LOW);
          digitalWrite(gPin, HIGH);
          digitalWrite(buzzerPin, HIGH);
          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(WHITE);
          display.setCursor(0,0);
          display.println("   Door    Unlocked");
          display.display();
          delay(delay1);
          digitalWrite(buzzerPin, LOW);
          delay(delay2);
          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(WHITE);
          display.setCursor(0,0);
          display.println(" RoboSafe    v2.0");
          display.display();
          digitalWrite(gPin, LOW);
        }    
        else if (msg==lkey){
                Serial.print("Key Matched!");
                digitalWrite(lockPin, HIGH);
                digitalWrite(gPin, HIGH);
                digitalWrite(buzzerPin, HIGH);
                display.clearDisplay();
                display.setTextSize(2);
                display.setTextColor(WHITE);
                display.setCursor(0,0);
                display.println("   Door    Locked  ");
                display.display();
                delay(delay1);
                digitalWrite(buzzerPin, LOW);
                delay(delay2);
                display.clearDisplay();
                display.setTextSize(2);
                display.setTextColor(WHITE);
                display.setCursor(0,0);
                display.println(" RoboSafe    v2.0");
                display.display();
                digitalWrite(gPin, LOW);
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
  //Serial.println(read_rfid);
  nkey = read_rfid;
  return;
}


// returns -1 if failed, otherwise returns ID #
int getFingerPrint() 
{
  int p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  return finger.fingerID;
}