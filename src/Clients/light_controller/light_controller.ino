#include "request.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define light D4

const char* ssid = "ADAMOSS_PC";
const char* password = "Jcw8XaRaT8Jc";
const char* apiURL = "http://192.168.0.244:5000/api/GetLightController";

void setup() {
  Serial.begin(115200);

  pinMode(light, OUTPUT);

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

  JsonObject jsonObject = jsonDoc.as<JsonObject>();

  String response = make_request(apiURL, jsonObject);

  DynamicJsonDocument jsonDoc2(1024);
  DeserializationError error = deserializeJson(jsonDoc2, response);

  if (error) {
    Serial.print("Deserialization failed: ");
    Serial.println(error.c_str());
    return;
  }

  if (jsonDoc2.containsKey("state")) {
    String stateValue = jsonDoc2["state"];
    Serial.print("State: ");
    Serial.println(stateValue);

    if(stateValue == "on"){
      digitalWrite(light, HIGH);
    }else if(stateValue == "off"){
      digitalWrite(light, LOW);
    }else {
      Serial.println("JSON structure is not as expected.");
    }

  } else {
    Serial.println("error");
  }

  delay(500);
}
