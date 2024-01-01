#include "request.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#define light D4


const char* ssid = "AdamossWIFI";
const char* password = "00951221202611";
const char* apiURL = "http://192.168.0.114:5000/api/GetLightController";

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

  String response =  make_request(apiURL);

  DynamicJsonDocument jsonDoc(1024);
  deserializeJson(jsonDoc, response);

  if (jsonDoc.containsKey("state")) {

    String stateValue = jsonDoc["state"];
    Serial.print("State: ");
    Serial.println(stateValue);
    
    if(stateValue == "on"){
      digitalWrite(light, HIGH);
    }else if(stateValue == "off"){
      digitalWrite(light, LOW);
    }else {
      Serial.println("JSON structure is not as expected.");
    }

  }else{
    
    Serial.println("error");
  }
  
  delay(500);

}
