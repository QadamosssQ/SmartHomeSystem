#ifndef INDEX_HTML_H
#define INDEX_HTML_H

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
    </html>
    )rawliteral";

#endif // INDEX_HTML_H
