#include <WiFi.h>
#include <painlessMesh.h>
#include <ArduinoJson.h>

// Sensor and LED configuration
#define LIGHT_SENSOR_PIN  34  // ESP32 pin GPIO25 connected to light sensor
#define FORCE_SENSOR_PIN  35  // ESP32 pin GPIO13 connected to force sensor
#define LED_PIN           32  // ESP32 pin GPIO33 connected to LED
#define ANALOG_THRESHOLD  500
#define FORCE_THRESHOLD   500 // Threshold for force detection
#define TIMEOUT           5000 // 5 seconds in milliseconds

// Mesh network configuration
#define MESH_PREFIX     "ESPMESH"
#define MESH_PASSWORD   "test1234"
#define MESH_PORT       5555

bool isDark = false;
unsigned long forceBelowThresholdTime = 0;
Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

uint32_t rootnodeID = 2223841881; // root node ID

void sendData(bool forceStatus);

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(FORCE_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  // Mesh setup
  mesh.setDebugMsgTypes(ERROR | STARTUP);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler);
  mesh.setRoot(false);
}

void loop() {
  int lightValue = analogRead(LIGHT_SENSOR_PIN);
  int forceValue = analogRead(FORCE_SENSOR_PIN);
  bool forceStatus = forceValue >= FORCE_THRESHOLD;

  // Determine if it's dark
  isDark = lightValue < ANALOG_THRESHOLD;

  // Handle LED and force sensor logic
  if (forceValue < FORCE_THRESHOLD) {
    if (forceBelowThresholdTime == 0) {
      forceBelowThresholdTime = millis();
    } else if (millis() - forceBelowThresholdTime >= TIMEOUT) {
      digitalWrite(LED_PIN, LOW);
    }
  } else {
    forceBelowThresholdTime = 0;
    digitalWrite(LED_PIN, isDark ? HIGH : LOW);
  }

  // Send force status over mesh network
  sendData(forceStatus);

  Serial.print("Light Value = ");
  Serial.print(lightValue);
  Serial.print(", Force Value = ");
  Serial.println(forceValue);

  delay(250); // Mesh network handling
  mesh.update();
}

void sendData(bool forceStatus) {
  DynamicJsonDocument doc(1024);
  doc["Sensor"] = "druksensor"; // Set sensor name as specified
  doc["Status"] = forceStatus ? "AAN" : "UIT"; // "AAN" if above threshold, otherwise "UIT"

  String message;
  serializeJson(doc, message);
  mesh.sendSingle(rootnodeID, message); // Send the status message over the mesh network
}

