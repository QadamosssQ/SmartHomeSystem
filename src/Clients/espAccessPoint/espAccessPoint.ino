#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ESP8266-Access-Point";
const char* password = "123456789";

ESP8266WebServer server(80);

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

void handleConnect() {
  String ssid;
  String password;

  if (server.hasArg("ssid") && server.hasArg("password")) {
    ssid = server.arg("ssid");
    password = server.arg("password");

    Serial.println("Connecting to network...");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);
    apMode = false;

    // Attempt to connect to WiFi
    WiFi.begin(ssid.c_str(), password.c_str());

    // Wait for WiFi connection
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
      delay(500);
    }

    // Check connection status
    if (WiFi.status() == WL_CONNECTED) {
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
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", index_html);
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
    Serial.println("Connected to WiFi. Checking Internet connectivity...");
    WiFi.mode(WIFI_STA);
    WiFi.softAPdisconnect();
    WiFi.softAPdisconnect(false);

    if (WiFiPing()) {
      Serial.println("Internet is accessible!");
      // here code for requests:


      
    } else {
      Serial.println("No Internet connectivity.");
    }
  } else {
    Serial.println("No Internet connectivity.");
  }

  if (apMode) {
    Serial.println("AP Mode - Looping...");
  } else {
    Serial.println("Client Mode - Looping...");
  }

  delay(5000);
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
