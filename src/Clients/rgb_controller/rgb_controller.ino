#include "request.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define r_pin D1
#define g_pin D2
#define b_pin D4

const char* ssid = "AdamossWIFI";
const char* password = "00951221202611";
const char* apiURL = "http://192.168.0.114:5000/api/GetRGBController";


void setup() {
  Serial.begin(115200);

  pinMode(r_pin, OUTPUT);
  pinMode(g_pin, OUTPUT);
  pinMode(b_pin, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
}


void loop() {
  DynamicJsonDocument jsonDoc(1024);
  jsonDoc["user_secret"] = "sAHZZN8xXS1HMk9dSHU0NHaYJPE_oUcZrESRnH26rhg";
  jsonDoc["device_secret"] = "Aki3hjZ3T6Vekm-Hr1zge_jYT-Tw0Zl7VsWGiFaUBMM";

  JsonObject jsonPayload = jsonDoc.to<JsonObject>();

  String response = make_request(apiURL, jsonPayload);
  # TODO: Send this in headers no in payloads
  deserializeJson(jsonDoc, response);

  if (jsonDoc.containsKey("r") && jsonDoc.containsKey("g") && jsonDoc.containsKey("b") && jsonDoc.containsKey("state")) {
    int r = jsonDoc["r"];
    int g = jsonDoc["g"];
    int b = jsonDoc["b"];
    bool state = jsonDoc["state"];

    if (state) {
      analogWrite(r_pin, r);
      analogWrite(g_pin, g);
      analogWrite(b_pin, b);
    } else {
      analogWrite(r_pin, 0);
      analogWrite(g_pin, 0);
      analogWrite(b_pin, 0);
    }
  } else {
    Serial.println("Error: JSON response does not contain all keys");
  }
  delay(1000);
}
