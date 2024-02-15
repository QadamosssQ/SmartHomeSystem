#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "ESP8266-Access-Point";
const char* password = "123456789";

AsyncWebServer server(80);

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
        flex-direction: column;
        align-items: center;
        justify-content: center;
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
  </style>
</head>
<body>
  <div class="main">
    <h1>Available Networks</h1>

    <form id="connectForm">
      <input list="networks" name="ssid" id="network" placeholder="Select a Network" required>
      <datalist id="networks"><option value="Network-1"></datalist>
      <input type="password" name="password" id="password" placeholder="Password" required>
      <input type="submit" value="Connect">
    </form>
  </div>

  <script>
    document.getElementById("connectForm").addEventListener("submit", function (event) {
      event.preventDefault();

      let [network, password] = [document.getElementById("network").value, document.getElementById("password").value];

      if (network === "" || password === "") alert("Please select a network and enter a password");
      else {
        fetch("/connect", {
          method: "POST",
          headers: {
            "Content-Type": "application/x-www-form-urlencoded",
          },
          body: "ssid=" + encodeURIComponent(network) + "&password=" + encodeURIComponent(password),
        }).then(response => response.text()).then(data => {
          alert(data);
          window.location.reload();  // Odśwież stronę po nawiązaniu połączenia
        });
      }
    });
  </script>
</body>
</html>)rawliteral";

void handleConnect(AsyncWebServerRequest *request) {
  String ssid;
  String password;

  if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
    ssid = request->getParam("ssid", true)->value();
    password = request->getParam("password", true)->value();

    Serial.println("Connecting to network...");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);
    apMode = false;

    // Attempt to connect to WiFi
    WiFi.begin(ssid, password);
    request->send(200, "text/plain", "Connected to the network");
    return;

    // // Wait for WiFi connection
    // unsigned long startTime = millis();
    // while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    //   delay(500);
    // }

    // // Check connection status
    // if (WiFi.status() == WL_CONNECTED) {
    //   request->send(200, "text/plain", "Connected to the network");
    // } else {
    //   request->send(400, "text/plain", "Error: Failed to connect to the network. Check SSID and password.");
    // }
    // return;
  }
  request->send(400, "text/plain", "Error: Missing ssid or password parameters");
}


void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    
    Serial.print("Request URL: ");
    Serial.println(request->url());

    request->send_P(200, "text/html", index_html);
  });

  server.on("/connect", HTTP_POST, handleConnect);

  server.begin();
}

void loop() {
  if (!apMode && WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting...");
    WiFi.reconnect();
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi. Checking Internet connectivity...");
    WiFi.mode(WIFI_STA);
    WiFi.enableAP(false);
    WiFi.softAPdisconnect();
    WiFi.softAPdisconnect(false);

    if (WiFiPing()) {
      Serial.println("Internet is accessible!");
    } else {
      Serial.println("No Internet connectivity.");
    }
  }

  if (apMode) {
    Serial.println("AP Mode - Looping...");
  } else {
    Serial.println("Client Mode - Looping...");
  }

  delay(5000);
}


bool WiFiPing() {
  HTTPClient http;

  WiFiClient wifiClient;

  if (http.begin(wifiClient, "http://www.google.com")) {
    int httpCode = http.GET();
    http.end();
    return httpCode == HTTP_CODE_OK;
  }

  return false;
}

