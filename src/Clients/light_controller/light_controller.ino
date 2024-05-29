# TODO: Wykminić jak zrobić by host wiedział do jakiego urządzenia się odnosi (muszę jakoś przekazać "device_secret" do hosta)


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "request.h"
#include <ArduinoJson.h>

#define light D4

const char* APssid = "Smarthome-Setup";
const char* APpassword = "123456789";

String user_secret;

const char* apiURL = "http://192.168.0.244:5000/api/GetLightController";

ESP8266WebServer server(80);
IPAddress apIP(192, 168, 1, 1);

bool apMode = true;

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

      DynamicJsonDocument jsonDoc(1024);
      jsonDoc["login"] = login_api;
      jsonDoc["password"] = password_api;

      JsonObject jsonObject = jsonDoc.as<JsonObject>();

      String response = make_request("http://192.168.0.244:5000/api/login", jsonObject);

      DynamicJsonDocument jsonDoc2(1024);
      DeserializationError error = deserializeJson(jsonDoc2, response);

      if (error) {
        Serial.print("Deserialization failed in login: ");
        Serial.println(error.c_str());
        Serial.println("Response JSON:");
        Serial.println(response);
        return;
      }

      if (jsonDoc2.containsKey("user_secret")) {
        user_secret = jsonDoc2["user_secret"].as<const char*>();
      } else {
        Serial.println("error");
      }

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

  pinMode(light, OUTPUT);

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

void loop() {
  server.handleClient();

  if (!apMode && WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting...");
    WiFi.reconnect();
  }

  if (WiFi.status() == WL_CONNECTED) {
    WiFi.mode(WIFI_STA);

    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["user_secret"] = user_secret;
    jsonDoc["device_secret"] = "i0uDTS0vJV6x8t11Jyf565WSJruhJn6MtR8ZwUMh5eI";

    JsonObject jsonObject = jsonDoc.as<JsonObject>();

    String response = make_request(apiURL, jsonObject);

    DynamicJsonDocument jsonDoc2(1024);
    DeserializationError error = deserializeJson(jsonDoc2, response);

    if (error) {
      Serial.print("Deserialization failed: ");
      Serial.println(error.c_str());
      Serial.println("Response JSON:");
      Serial.println(response);
      return;
    }

    if (jsonDoc2.containsKey("state")) {
      String stateValue = jsonDoc2["state"];

      if (stateValue == "on") {
        digitalWrite(light, HIGH);
      } else if (stateValue == "off") {
        digitalWrite(light, LOW);
      } else {
        Serial.println("JSON structure is not as expected.");
      }

    } else {
      Serial.println("error");
    }

    delay(500);

  } else {
    Serial.println("No Internet connectivity.");
  }

  delay(1000);
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
