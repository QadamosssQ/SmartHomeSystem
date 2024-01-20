//request.cpp
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

String make_request(String url){

  WiFiClient wifi;
  HTTPClient http;

  http.begin(wifi, url);

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

