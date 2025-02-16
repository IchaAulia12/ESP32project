#include <WiFi.h>
#include <DHT.h>

// Pin Definitions
#define DHT_PIN 23            // Pin untuk DHT11 Sensor

// Define sensor type for DHT11
#define DHTTYPE DHT11
#define LED_PIN 2

// Inisialisasi objek DHT
DHT dht(DHT_PIN, DHTTYPE);

const char* ssid = "your_ssid";
const char* password = "your_password";

void setup() {
  // Start Serial Monitor
  Serial.begin(9600);

  // Initialize the DHT11 Sensor
  dht.begin();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("Connecting to wifi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() !=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWifi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  // Membaca nilai dari DHT11
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    } else{
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");

    delay(5000);
  }

}
