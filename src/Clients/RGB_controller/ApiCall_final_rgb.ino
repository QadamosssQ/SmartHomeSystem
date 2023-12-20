#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "AdamossWIFI";
const char* password = "00951221202611";
const char* apiURL = "http://192.168.0.114:5000/api/GetRGBController"; // Update with your Flask API URL

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
}

void loop() {
  WiFiClient wifi;
  HTTPClient http;

  http.begin(wifi, apiURL);
  http.addHeader("Origin", "http://192.168.0.113"); // Replace with your Arduino's IP

  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    String response = http.getString();

    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, response);

    // Check if the JSON object has "r", "g", "b", and "state" keys
    if (jsonDoc.containsKey("r") && jsonDoc.containsKey("g") && jsonDoc.containsKey("b") && jsonDoc.containsKey("state")) {
      // Access RGB values and state
      int redValue = jsonDoc["r"];
      int greenValue = jsonDoc["g"];
      int blueValue = jsonDoc["b"];

      // Print RGB values
      Serial.println("RGB Values:");
      Serial.print("Red: ");
      Serial.println(redValue);
      Serial.print("Green: ");
      Serial.println(greenValue);
      Serial.print("Blue: ");
      Serial.println(blueValue);

      // You can also print the state if needed
      bool stateValue = jsonDoc["state"];
      Serial.print("State: ");
      Serial.println(stateValue ? "true" : "false");
      delay(10000);
    } else {
      Serial.println("JSON structure is not as expected.");
    }
  } else {
    analogWrite(LED_BUILTIN, LOW); // Indicate HTTP error
    // Serial.print("HTTP Error: ");
    // Serial.println(httpResponseCode);
  }

  http.end();
  delay(5000); // Delay for 5 seconds before making the next request
}
