#include "WiFiEsp.h"

char ssid[] = "your-ssid";
char password[] = "your-password";

WiFiEspClient client;

void setup() {
  // Serial for debugging
  Serial.begin(115200);
  // Serial for ESP8266 communication
  Serial3.begin(115200);
  WiFi.init(&Serial3);

  // Connect to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi");
    WiFi.begin(ssid, password);
  }
  Serial.println("Connected to WiFi");

  // Print status
  printWifiStatus();
}

void loop() {
  // Execute your code here
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
