#include <WiFi.h>
#include <DHT.h>

#define DHTPIN 23      // GPIO pin for DHT11
#define DHTTYPE DHT11  // DHT 11 sensor type
#define LED_PIN 2      // GPIO pin for LED (built-in LED on ESP32)

DHT dht(DHTPIN, DHTTYPE);

// Replace with your network credentials
const char* ssid = "Icha Comel";
const char* password = "changbinganteng";

WiFiServer server(80);

String ledState = "OFF"; // LED state

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  pinMode(LED_PIN, OUTPUT); // Set LED pin as output
  digitalWrite(LED_PIN, LOW); // Ensure LED is off at startup
  
  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Start the server
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client Connected!");
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    // Handle LED control
    if (request.indexOf("/LED=ON") != -1) {
      digitalWrite(LED_PIN, HIGH); // Turn LED on
      ledState = "ON";
    }
    if (request.indexOf("/LED=OFF") != -1) {
      digitalWrite(LED_PIN, LOW); // Turn LED off
      ledState = "OFF";
    }

    // Read DHT11 sensor data
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Check if readings are valid
    if (isnan(temperature) || isnan(humidity)) {
      temperature = 0.0;
      humidity = 0.0;
    }

    // Create HTML response
    String html = "<!DOCTYPE html><html>";
    html += "<head>";
    html += "<title>ESP32 DHT11 & LED Control</title>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"; // Responsive design
    html += "<style>";
    html += "body { font-family: 'Arial', sans-serif; margin: 0; padding: 0; background-color: #f0f4f8; color: #333; }";
    html += ".container { max-width: 400px; margin: auto; padding: 20px; text-align: center; background: #ffffff; border-radius: 12px; box-shadow: 0 8px 15px rgba(0, 0, 0, 0.1); animation: fadeIn 1.5s; }";
    html += "h1 { font-size: 26px; color: #007BFF; margin-bottom: 20px; }";
    html += ".data { font-size: 18px; margin: 10px 0; padding: 15px; background: #f7fafc; border-radius: 10px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.05); animation: slideIn 1s; }";
    html += ".data span { font-weight: bold; color: #007BFF; }";
    html += "button { font-size: 18px; padding: 12px 25px; margin: 15px; border: none; border-radius: 8px; cursor: pointer; transition: 0.3s ease; }";
    html += ".btn-green { background: linear-gradient(135deg, #28a745, #58d68d); color: white; }";
    html += ".btn-green:hover { background: linear-gradient(135deg, #1d8b34, #46bf74); }";
    html += ".btn-red { background: linear-gradient(135deg, #dc3545, #f78a8c); color: white; }";
    html += ".btn-red:hover { background: linear-gradient(135deg, #a72b36, #e86b70); }";
    html += "@keyframes fadeIn { from { opacity: 0; } to { opacity: 1; } }";
    html += "@keyframes slideIn { from { transform: translateY(20px); opacity: 0; } to { transform: translateY(0); opacity: 1; } }";
    html += "</style>";
    html += "<script>";
    html += "function updateTime() {";
    html += "  const now = new Date();";
    html += "  const date = now.toLocaleDateString();";
    html += "  const time = now.toLocaleTimeString();";
    html += "  document.getElementById('datetime').innerHTML = `<b>Date:</b> ${date}<br><b>Time:</b> ${time}`;";
    html += "}";
    html += "setInterval(updateTime, 1000);"; // Update time every second
    html += "function refreshData() {";
    html += "  fetch('/refresh').then(response => response.json()).then(data => {";
    html += "    document.getElementById('temperature').innerText = data.temperature + ' °C';";
    html += "    document.getElementById('humidity').innerText = data.humidity + ' %';";
    html += "    document.getElementById('ledState').innerText = data.ledState;";
    html += "  });";
    html += "}";
    html += "setInterval(refreshData, 5000);"; // Refresh sensor data every 5 seconds
    html += "</script>";
    html += "</head>";
    html += "<body onload='updateTime()'>";
    html += "<div class='container'>";
    html += "<h1>ESP32 DHT11 & LED Control</h1>";
    html += "<div id='datetime' class='data'></div>";
    html += "<div class='data'>Temperature: <span id='temperature'>" + String(temperature) + " °C</span></div>";
    html += "<div class='data'>Humidity: <span id='humidity'>" + String(humidity) + " %</span></div>";
    html += "<div class='data'>LED State: <span id='ledState'>" + ledState + "</span></div>";
    html += "<button class='btn-green' onclick=\"location.href='/LED=ON'\">Turn ON</button>";
    html += "<button class='btn-red' onclick=\"location.href='/LED=OFF'\">Turn OFF</button>";
    html += "</div>";
    html += "</body></html>";



    // Send the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
    client.println(html);
    client.println();

    client.stop();
    Serial.println("Client Disconnected.");
  }
}
