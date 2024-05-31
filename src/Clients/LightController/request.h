#ifndef REQUEST_H
#define REQUEST_H

#include <ArduinoJson.h>

String make_request(String url, JsonObject& jsonPayload);

#endif
