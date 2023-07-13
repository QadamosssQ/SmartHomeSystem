#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define r_PIN 2
#define g_PIN 3
#define b_PIN 4

const char* ssid = "wifi_name";
const char* password = "wifi_password";
const char* apiURL = "http://192.168.0.107:4040/api"; // my local ip address

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(r_PIN, OUTPUT);
  pinMode(g_PIN, OUTPUT);
  pinMode(b_PIN, OUTPUT);

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

}

void loop() {
  
  HTTPClient http;
  http.begin(apiURL);

  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    String response = http.getString();

    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, response);

    int r = jsonDoc["r"];
    int g = jsonDoc["g"];
    int b = jsonDoc["b"];
    int state = jsonDoc["state"];

    if(state == 1){

      analogWrite(r_PIN,r);
      analogWrite(g_PIN,g);
      analogWrite(b_PIN,b);

    }else{

      analogWrite(r_PIN,0);
      analogWrite(g_PIN,0);
      analogWrite(b_PIN,0);
      
    }
  }
  else {
    Serial.print("HTTP Error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  delay(500);
}
