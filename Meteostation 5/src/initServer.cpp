#include "../include/initServer.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>
#include "../include/config.h"
#include <LittleFS.h>

extern float lastTemp;
extern float lastHumidity;

WebServer server(80);

const char html[] PROGMEM = R"rawliteral(
<html>
  <head>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' crossorigin='anonymous'>
    <title>ESP32 DHT Server</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        html { font-family: Arial; text-align: center; margin: 0 auto; }
        h2 { font-size: 3.0rem; }
        p { font-size: 3.0rem; }
        .units { font-size: 1.2rem; }
        .dht-labels { font-size: 1.5rem; vertical-align: middle; padding-bottom: 15px; }
    </style>
  </head>
  <body>
      <h2>ESP32 DHT Server!</h2>
      <p>
        <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>
        <span class='dht-labels'>Temperature</span>
        <span id="temp">--</span>
        <sup class='units'>&deg;C</sup>
      </p>
      <p>
        <i class='fas fa-tint' style='color:#00add6;'></i>
        <span class='dht-labels'>Humidity</span>
        <span id="humidity">--</span>
        <sup class='units'>&percnt;</sup>
      </p>

      <canvas id="tempChart" width="400" height="200"></canvas>
  </body>

  <script>
    const temperatureElement = document.getElementById("temp");
    const humidityElement = document.getElementById("humidity");

    const tempData = [];
    const humidityData = [];
    const labels = [];

    function getData() {
        fetch("/data", {method: "GET"})
            .then(response => response.json())
            .then(data => {
                temperatureElement.textContent = data["temperature"];
                humidityElement.textContent = data["humidity"];

                labels.push(new Date().toLocaleTimeString());
                tempData.push(data["temperature"]);
                humidityData.push(data["humidity"]);

                if (labels.length > 20) {
                    labels.shift();
                    tempData.shift();
                    humidityData.shift();
                }
                
                chart.update();
            })
            .catch(error => console.log(error));
    }

    const ctx = document.getElementById('tempChart').getContext('2d');
    const chart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: labels,
            datasets: [
                { label: 'Temperature (°C)', data: tempData, borderColor: 'red', fill: false },
                { label: 'Humidity (%)', data: humidityData, borderColor: 'blue', fill: false }
            ]
        }
    });

    setInterval(getData, 1000);
  </script>
</html>
)rawliteral";

void handleRoot()
{
  server.send(200, "text/html", html);
}

void sendData()
{
  String response = String("{\"temperature\":") + lastTemp + ",\"humidity\":" + lastHumidity + "}";
  server.send(200, "application/json", response);
}

void handleDb()
{
  File file = LittleFS.open("/database.csv", "r");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    server.send(404, "text/plain", "File not found");
    return;
  }
  String content = file.readString();
  file.close();
  server.send(200, "text/plain", content);
}

void initServer()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (LittleFS.begin())
  {
    Serial.println("LittleFS mounted successfully");
  }
  else
  {
    Serial.println("Failed to mount LittleFS");
  }

  server.on("/", handleRoot);
  server.on("/data", sendData);
  server.on("/database", handleDb);
  server.begin();
}
