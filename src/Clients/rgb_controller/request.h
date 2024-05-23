#ifndef REQUEST_H
#define REQUEST_H

#include <Arduino.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

String make_request(String url, JsonObject& jsonPayload);

#endif
