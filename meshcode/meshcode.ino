#include <WiFi.h>
#include <secret.h>

// WiFi credentials to connect to the Raspberry Pi

// Data sending (to Raspberry Pi)
const char* host = "raspberrypi.local"; // or Raspberry Pi's IP
const uint16_t port = 80;

int connectionAttempts = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to Raspberry Pi...");
  }
  Serial.println("Connected to Raspberry Pi!");

  WiFiClient client;
  while (!client.connect(host, port)) {
  Serial.println("Connection to Raspberry Pi failed, retrying...");
  connectionAttempts++;
  if (connectionAttempts > 5) {
    Serial.println("Failed to connect after 5 attempts, going to sleep");
    // Put the ESP32 into deep sleep mode here, if desired
    return;
  }
  delay(1000); // Wait a second before trying to reconnect
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

