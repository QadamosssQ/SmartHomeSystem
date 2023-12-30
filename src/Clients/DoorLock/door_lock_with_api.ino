//door_lock_with_api.ino
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#include "display_functions.h"
#include "request.h"

const char* ssid = "AdamossWIFI";
const char* password = "00951221202611";

#define TFT_CS 6
#define TFT_DC 5
#define TFT_RST 7
#define open_switch 3
#define door_state 2

#define door_close_led 10
#define lock 8

String card_id = "";

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

String URL = "http://127.0.0.1:5000/api/GetDoorLock";

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

//all allowed cards:

String allowedCards[100] = {

  "23 49 67 A3",
  "43 8E 60 A3",
  "73 FD 84 13"

};

void setup() {

  pinMode(open_switch, INPUT_PULLUP);
  pinMode(door_close_led, OUTPUT);
  pinMode(lock, OUTPUT);
  pinMode(door_state, INPUT_PULLUP);

  Serial.begin(9600);
  SPI.begin();
  nfc.begin();

  Serial.println("Door_lock_REMASTERED v1.8");

  tft.init(240, 320);
  tft.fillScreen(ST77XX_BLACK);
  tft.invertDisplay(false);

  zero();

  digitalWrite(door_close_led, HIGH);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
}


bool checkCard(String id) {

  for (int i = 0; i < sizeof(allowedCards) / sizeof(allowedCards[0]); i++) {
    if (card_id == allowedCards[i]) {

      return true;
    }
  }
  return false;
}


void loop() {

  String response = make_request(URL);

  DynamicJsonDocument jsonDoc(1024);
  deserializeJson(jsonDoc, response);

  String door_state_ = jsonDoc["state"];

  if (door_state_ == "open") {
    digitalWrite(door_close_led, LOW);
  } else if (door_state_ == "closed") {
    digitalWrite(door_close_led, HIGH);
  } else if (door_state_ == "auto") 
    {


      if (digitalRead(door_state) == HIGH) {

        digitalWrite(lock, HIGH);
        digitalWrite(door_close_led, LOW);

        granted();
        Serial.println("granted (door open)");


        while (digitalRead(door_state) == HIGH) {
          delay(100);
        }

        delay(2000);

        digitalWrite(lock, LOW);
        digitalWrite(door_close_led, HIGH);

        zero();

      } else {

        if (digitalRead(open_switch) == LOW) {

          digitalWrite(lock, HIGH);
          digitalWrite(door_close_led, LOW);

          switch_granted();
          Serial.println("granted (open switch)");

          delay(2000);

          digitalWrite(lock, LOW);
          digitalWrite(door_close_led, HIGH);

          zero();

        } else {

          if (nfc.tagPresent()) {

            NfcTag tag = nfc.read();
            card_id = tag.getUidString();



            if (checkCard(card_id) == true) {
              granted();
              Serial.println(card_id);
              Serial.println("granted (rfid card)");
              digitalWrite(lock, HIGH);
              digitalWrite(door_close_led, LOW);

              delay(2000);

              digitalWrite(lock, LOW);
              digitalWrite(door_close_led, HIGH);

              zero();

            } else {

              denied();
              delay(2000);
              zero();
            }
          }
      }
  }
}
}

