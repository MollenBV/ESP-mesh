#include <WiFi.h>
#include <secret.h>

// WiFi credentials to connect to the Raspberry Pi

// Data sending (to Raspberry Pi)
const char* host = "raspberrypi.local"; // or Raspberry Pi's IP
const uint16_t port = 80;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to Raspberry Pi...");
  }
  Serial.println("Connected to Raspberry Pi!");

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("Connection to Raspberry Pi failed");
    return;
  }

  Serial.println("Connected to Raspberry Pi server");

  // Example data
  String dataToSend = "Data from ESP32 Node 4";
  client.println(dataToSend);
  Serial.println("Data sent to Raspberry Pi");

  client.stop();
  printIn(ssid)
  printIn(password)
}

void loop() {
  // Nothing here for now
}

