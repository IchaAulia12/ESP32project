#include <WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>

// Pin Definitions
#define DHT_PIN 23            // Pin untuk DHT11 Sensor

// Define sensor type for DHT11
#define DHTTYPE DHT11
#define LED_PIN 2

// Inisialisasi objek DHT
DHT dht(DHT_PIN, DHTTYPE);

// Wi-Fi credentials
const char* ssid = "your_ssid";
const char* password = "your_password";

// MQTT Broker details
const char* mqtt_broker = "broker.mqtt.cool";
const char* mqtt_topic_temp = "IchaIot/temp";
const char* mqtt_topic_humidity = "IchaIot/hum";
const char* mqtt_topic_lamp = "IchaIot/lamp";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  // Convert the message to a string
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  Serial.print("Message: ");
  Serial.println(messageTemp);

  // Check if the topic is for the LED control
  if (String(topic) == mqtt_topic_lamp) {
    if (messageTemp == "on") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED turned ON");
    } else if (messageTemp == "off") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED turned OFF");
    }
  }
  if (String(topic) == mqtt_topic_lamp) {
    Serial.println("Icha : ");
    Serial.println(messageTemp);
  }
}

void setup() {
  // Start Serial Monitor
  Serial.begin(9600);

  // Initialize the DHT11 Sensor
  dht.begin();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

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

  // Connect to MQTT Broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT Broker...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT Broker!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }

  // Subscribe to the lamp topic
  client.subscribe(mqtt_topic_lamp);
  client.subscribe("IchaIot/message");
  Serial.println("Subscribed to topic: IchaIot/lamp");
}

void loop() {
  // Reconnect to MQTT if disconnected
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read temperature and humidity from DHT11
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any reads failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    temperature = 0.0;
    humidity = 0.0;
  }

  // Publish temperature and humidity to MQTT topics
  char tempString[8];
  char humString[8];
  dtostrf(temperature, 6, 2, tempString); // Convert float to string
  dtostrf(humidity, 6, 2, humString);     // Convert float to string

  client.publish(mqtt_topic_temp, tempString);
  client.publish(mqtt_topic_humidity, humString);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(5000); // Wait 5 seconds before sending the next reading
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT Broker!");
      client.subscribe(mqtt_topic_lamp);
      Serial.println("Resubscribed to topic: IchaIot/lamp");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}
