#include "Arduino.h"
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const byte rxPin = 2;
const byte txPin = 3;
#define pinRST         9          // Configurable, see typical pin layout above
#define pinSS          10         // Configurable, see typical pin layout above
#define lockPin        8
#define gPin           7
#define rPin           4
#define buzzerPin      6

SoftwareSerial BTSerial(rxPin, txPin); // RX TX
String msg = "";
const String lkey = "2707";
const String ulkey = "0702";

byte readCard[4];
int successRead;
String orgkey = "93408390";
String nkey = "";
MFRC522 mfrc522(pinSS, pinRST);  // Create MFRC522 instance
MFRC522::MIFARE_Key key;  

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

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


void setup() {
  //bluetooth
  pinMode(lockPin, OUTPUT);
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  BTSerial.begin(9600);
  Serial.begin(9600);
  digitalWrite(lockPin, LOW);


  //rfid
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	Serial.println("RFID reading.............");
  pinMode(lockPin, OUTPUT);
  do{
    successRead = getID(mfrc522.uid.uidByte, mfrc522.uid.size);
  } while (!successRead);

  // oled
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // default text
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(" RoboSafe    v1.0");
  display.display();
}


const char START_TOKEN = '?';
const char END_TOKEN = ';';
const char DELIMIT_TOKEN = '&';
const int CHAR_TIMEOUT = 20;

bool waitingForStartToken = true;
String messageBuffer = "";

void loop() {
  //rfid
  getID(mfrc522.uid.uidByte, mfrc522.uid.size);
  if (nkey==orgkey){
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
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    delay(3000);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println(" RoboSafe    v1.0");
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
          digitalWrite(lockPin, LOW);
          digitalWrite(gPin, HIGH);
          digitalWrite(buzzerPin, HIGH);

          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(WHITE);
          display.setCursor(0,0);
          display.println("   Door    Unlocked");
          display.display();

          delay(1000);
          digitalWrite(buzzerPin, LOW);
          delay(3000);

          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(WHITE);
          display.setCursor(0,0);
          display.println(" RoboSafe    v1.0");
          display.display();

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
}