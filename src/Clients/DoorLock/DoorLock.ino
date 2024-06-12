// Error: Nulls maybe because I am curently working on esp8266 which dont work with SPI at all

#include <ESP8266WebServer.h>
#include "request.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#include "display_functions.h"
#include "request.h"


#define TFT_CS 6
#define TFT_DC 5
#define TFT_RST 7
#define open_switch 3
#define door_state 2

#define door_close_led 10
#define lock 8

const char* APssid = "Smarthome-Setup";
const char* APpassword = "123456789";

String user_secret;
String device_secret;

const char* apiURL = "http://192.168.0.244:5000/api/GetDoorLock";

ESP8266WebServer server(80);
IPAddress apIP(192, 168, 1, 1);

bool apMode = true;

String card_id = "";

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

String allowedCards[100] = {

  "23 49 67 A3",
  "43 8E 60 A3",
  "73 FD 84 13"

};

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 20px;
    }
    .main {
        display: flex;
        width: 100%;
        height: auto;
        flex-direction: column;
        align-items: center;
    }

    h1 {
      color: #333;
    }

    form {
      max-width: 500px;
      margin: 20px 0;
      padding: 15px;
      border: 1px solid #ddd;
      border-radius: 5px;
      background-color: #f9f9f9;
    }

    input {
      display: block;
      width: 100%;
      margin: 10px 0;
      padding: 8px;
      box-sizing: border-box;
    }

    input[type="submit"] {
      background-color: #4caf50;
      color: white;
      cursor: pointer;
    }

    input[type="submit"]:hover {
      background-color: #45a049;
    }

    .center {
      text-align: center;
    }

  </style>
</head>
<body>
  <div class="main">

    <form id="connectForm">
      <h3 class="center">Connect to network</h3>
      <input list="networks" name="ssid" id="network" placeholder="Select a network" required>
      <datalist id="networks">
      </datalist>
      <input type="text" name="password" id="password" placeholder="password" required>
      <br>
      <h3 class="center">Login to server</h3>
      <input type="text" name="login_api" id="login_api" placeholder="login" required>
      <input type="text" name="password_api" id="password_api" placeholder="password" required>
      <input type="submit" value="Connect">
    </form>

    <div id="internetMessage" style="display: none;">
      <h3 class="center">Connected to the Internet</h3>
      <p>You are now connected to the internet. You can proceed with other actions.</p>
    </div>

  </div>

  <script>
    function updateNetworkList(networks) {
      var dataList = document.getElementById("networks");
      dataList.innerHTML = "";
      networks.forEach(function(network) {
        var option = document.createElement("option");
        option.value = network;
        dataList.appendChild(option);
      });
    }

    document.getElementById("connectForm").addEventListener("submit", function (event) {
      event.preventDefault();

      let [network, password, login_api, password_api] = [document.getElementById("network").value, document.getElementById("password").value, document.getElementById("login_api").value, document.getElementById("password_api").value];

      if (network === "" || password === "" || login_api === "" || password_api === "") alert("Please select a network and enter a password");
      else {
        fetch("/connect", {
          method: "POST",
          headers: {
            "Content-Type": "application/x-www-form-urlencoded",
          },
          body: "ssid=" + encodeURIComponent(network) + "&password=" + encodeURIComponent(password) + "&login_api=" + encodeURIComponent(login_api) + "&password_api=" + encodeURIComponent(password_api),
        }).then(response => response.text()).then(data => {
          alert(data);
          window.location.reload();
        });
      }
    });
  </script>
</body>
</html>)rawliteral";

String getAvailableNetworks();
bool WiFiPing();


void handleConnect() {
  String ssid;
  String password;
  String login_api;
  String password_api;

  if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("login_api") && server.hasArg("password_api")) {
    ssid = server.arg("ssid");
    password = server.arg("password");
    login_api = server.arg("login_api");
    password_api = server.arg("password_api");

    Serial.println("Connecting to network...");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);
    Serial.print("login: ");
    Serial.println(login_api);
    Serial.print("password: ");
    Serial.println(password_api);
    apMode = false;

    WiFi.begin(ssid.c_str(), password.c_str());

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
      delay(500);
    }

    if (WiFi.status() == WL_CONNECTED) {
      WiFi.mode(WIFI_STA);

      user_secret = GetUserSecret(login_api, password_api);
      Serial.println("User secret: " + user_secret);

      device_secret = GetDeviceSecret(user_secret);
      Serial.println("Device secret: " + device_secret);


      server.send(200, "text/plain", "Connected to the network");
    } else {
      server.send(400, "text/plain", "Error: Failed to connect to the network. Check SSID and password.");
    }
    return;
  }
  server.send(400, "text/plain", "Error: Missing ssid or password parameters");
}


void setup() {
  Serial.begin(115200);

  pinMode(open_switch, INPUT_PULLUP);
  pinMode(door_close_led, OUTPUT);
  pinMode(lock, OUTPUT);
  pinMode(door_state, INPUT_PULLUP);

  SPI.begin();
  nfc.begin();

  tft.init(240, 320);
  tft.fillScreen(ST77XX_BLACK);
  tft.invertDisplay(false);

  zero();

  digitalWrite(door_close_led, HIGH);

  WiFi.softAP(APssid, APpassword);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, []() {
    String networks = getAvailableNetworks();
    String htmlPage = String(index_html);
    htmlPage.replace("<!-- Wi-Fi networks will be dynamically populated here -->", networks);
    server.send(200, "text/html", htmlPage);
  });

  server.on("/connect", HTTP_POST, handleConnect);
  server.begin();
}


bool WiFiPing() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient wifiClient;
    if (wifiClient.connect("www.google.com", 80)) {
      return true;
    }
  }
  return false;
}

String getAvailableNetworks() {
  String networkList = "";
  int numNetworks = WiFi.scanNetworks();
  for (int i = 0; i < numNetworks; ++i) {
    networkList += "<option value=\"" + WiFi.SSID(i) + "\">";
  }
  return networkList;
}

String GetUserSecret(String login, String password) {

  DynamicJsonDocument jsonDoc(1024);
  jsonDoc["login"] = login;
  jsonDoc["password"] = password;

  JsonObject jsonObject = jsonDoc.as<JsonObject>();

  String response = make_request("http://192.168.0.244:5000/api/login", jsonObject);

  DynamicJsonDocument jsonDoc2(1024);
  DeserializationError error = deserializeJson(jsonDoc2, response);

  if (error) {
    return "GetUserSecret error: " + jsonDoc2["error"].as<String>();
  }

  if (jsonDoc2.containsKey("user_secret")) {
    return jsonDoc2["user_secret"].as<String>();
  } else {
    return "GetUserSecret error: " + jsonDoc2["error"].as<String>();
  }
}

String GetDeviceSecret(String userSecret) {
  DynamicJsonDocument jsonDoc(1024);
  jsonDoc["user_secret"] = userSecret;
  jsonDoc["device_name"] = "DoorLock";
  jsonDoc["type"] = "DoorLock";

  JsonObject jsonObject = jsonDoc.as<JsonObject>();

  String response = make_request("http://192.168.0.244:5000/api/RegisterDevice", jsonObject);


  DynamicJsonDocument jsonDoc2(1024);
  DeserializationError error = deserializeJson(jsonDoc2, response);

  if (error) {
    return "GetDeviceSecret error: " + jsonDoc2["error"].as<String>();
  }

  if (jsonDoc2.containsKey("device_secret")) {
    return jsonDoc2["device_secret"].as<String>();
  } else {
    return "GetDeviceSecret error: " + jsonDoc2["error"].as<String>();
  }
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
  Serial.println("-----");

  server.handleClient();

  if (!apMode && WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting...");
    WiFi.reconnect();
  }

  if (WiFi.status() == WL_CONNECTED) {
    WiFi.mode(WIFI_STA);

    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["user_secret"] = user_secret;
    jsonDoc["device_secret"] = device_secret;

    Serial.println("user_secret: " + user_secret);
    Serial.println("device_secret: " + device_secret);

    JsonObject jsonObject = jsonDoc.as<JsonObject>();

    String response = make_request(apiURL, jsonObject);

    DynamicJsonDocument jsonDoc2(1024);
    DeserializationError error = deserializeJson(jsonDoc2, response);

    if (error) {
      Serial.println("Failed to parse state response");
      Serial.println(response);
      delay(1000);
      return;
    }

    if (jsonDoc2.containsKey("state")) {
      String stateValue = jsonDoc2["state"];

      if (stateValue == "on") {
        digitalWrite(door_close_led, LOW);
      } else if (stateValue == "off") {
        digitalWrite(door_close_led, HIGH);
      } else if (stateValue == "auto") {

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
      } else {
        Serial.println("Error in state: " + response);
      }

      delay(500);

    } else {
      Serial.println(".");
      delay(1000);
    }
  }
}
