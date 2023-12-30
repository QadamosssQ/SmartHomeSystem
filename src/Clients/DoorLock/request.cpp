#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "AdamossWIFI";
const char* password = "00951221202611";

String make_request(String url){

  WiFiClient wifi;
  HTTPClient http;

  http.begin(wifi, URL);

  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    String response = http.getString();

    http.end();
    return response;

  }else{
    Serial.print("HTTP Error: ");
    Serial.println(httpResponseCode);
    http.end();
    return "error";
  }

}

