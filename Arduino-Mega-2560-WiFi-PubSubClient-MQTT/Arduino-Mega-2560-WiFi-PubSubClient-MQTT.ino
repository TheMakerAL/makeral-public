/**
  MQTT connection using WiFiEspAT and PubSubClient libraries

  by MakerAL.com 2020.
*/

#include "WiFiEspAT.h"
#include "PubSubClient.h"
#include "ArduinoJson.h"

const char* MQTT_HOST = "test.mosquitto.org";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "";
const char* MQTT_PASSWORD = "";

const char* DEVICE_NAME = "arduino";
// Subscribe and publish to same topic for testing
const char* MQTT_SUB_TOPIC = "test";
const char* MQTT_PUB_TOPIC = "test";

const uint8_t MQTT_CLIENT_KEEPALIVE = 30;
const uint8_t MQTT_CLIENT_SOCKET_TIMEOUT = 15;
const bool MQTT_RETAIN = false;
const uint8_t MQTT_QOS = 0;
const uint8_t MQTT_PUBLISH_INTERVAL = 5;

unsigned long mqttLastPublishMillis = 0;

char ssid[] = "your-ssid";
char password[] = "your-password";

WiFiClient wifiClient;

PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(115200);

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setKeepAlive(MQTT_CLIENT_KEEPALIVE);
  mqttClient.setSocketTimeout(MQTT_CLIENT_SOCKET_TIMEOUT);
  mqttClient.setCallback(mqttReceived);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }

  if (!mqttClient.connected()) {
    connectMqtt();
  }

  // Keep MQTT connection alive
  mqttClient.loop();

  if (millis() - mqttLastPublishMillis >= MQTT_PUBLISH_INTERVAL * 1000) {
    mqttPublish();
    mqttLastPublishMillis = millis();
  }
}

void connectMqtt() {
  unsigned long lastMillis = millis();

  while (!mqttClient.connected()) {
    Serial.println("MQTT connecting...");

    bool mqttConnect = false;
    if (strlen(MQTT_USER) > 0 && strlen(MQTT_PASSWORD) > 0) {
      mqttConnect = mqttClient.connect(DEVICE_NAME, MQTT_USER, MQTT_PASSWORD);
    } else {
      mqttConnect = mqttClient.connect(DEVICE_NAME);
    }

    if (mqttConnect) {
      Serial.println("MQTT connected");

      if (mqttClient.subscribe(MQTT_SUB_TOPIC, MQTT_QOS)) {
        Serial.println("Subscribed to " + String(MQTT_SUB_TOPIC) + " successfully");
      } else {
        Serial.println("MQTT subscription error");
        mqttError(mqttClient.state());
      }
    }
  }
}

void mqttReceived(char *topic, byte *payload, unsigned int length) {
  StaticJsonDocument<128> doc;
  deserializeJson(doc, payload, length);

  // Cast data
  float temp = doc["temp"];
  float humidity = doc["humidity"];
  const char* room = doc["room"];

  Serial.println("Received from " + String(topic) + ":");
  Serial.println("temp: " + String(temp)
                 + ", humidity: " + String(humidity)
                 + ", room: " + String(room));
}

void mqttPublish() {
  StaticJsonDocument<128> doc;

  // Dummy data
  doc["temp"] = 80;
  doc["humidity"] = 50;
  doc["room"] = "Living Room";

  uint8_t buffer[128];
  size_t length = serializeJson(doc, buffer);

  if (mqttClient.publish(MQTT_PUB_TOPIC, buffer, length, MQTT_RETAIN)) {
    Serial.println("Published to " + String(MQTT_PUB_TOPIC) + ":");
    // Write JSON document to serial port
    serializeJson(doc, Serial);
    Serial.println("");
  } else {
    Serial.println("MQTT publish error");
    mqttError(mqttClient.state());
  }
}

void mqttError(int8_t MQTTErr) {
  if (MQTTErr == MQTT_CONNECTION_TIMEOUT) {
    Serial.print("Connection tiemout");
  } else if (MQTTErr == MQTT_CONNECTION_LOST) {
    Serial.print("Connection lost");
  } else if (MQTTErr == MQTT_CONNECT_FAILED) {
    Serial.print("Connect failed");
  } else if (MQTTErr == MQTT_DISCONNECTED) {
    Serial.print("Disconnected");
  } else if (MQTTErr == MQTT_CONNECTED) {
    Serial.print("Connected");
  } else if (MQTTErr == MQTT_CONNECT_BAD_PROTOCOL) {
    Serial.print("Connect bad protocol");
  } else if (MQTTErr == MQTT_CONNECT_BAD_CLIENT_ID) {
    Serial.print("Connect bad Client-ID");
  } else if (MQTTErr == MQTT_CONNECT_UNAVAILABLE) {
    Serial.print("Connect unavailable");
  } else if (MQTTErr == MQTT_CONNECT_BAD_CREDENTIALS) {
    Serial.print("Connect bad credentials");
  } else if (MQTTErr == MQTT_CONNECT_UNAUTHORIZED) {
    Serial.print("Connect unauthorized");
  }
}

void connectWifi() {
  Serial3.begin(115200);
  WiFi.init(&Serial3);

  // Connect to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
  }
  Serial.println("Connected to WiFi");

  // Print status
  printWifiStatus();
}

void printWifiStatus() {
  // Print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
