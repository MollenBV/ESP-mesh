#include <WiFi.h>
#include <painlessMesh.h>
#include <ArduinoJson.h>
#define LIGHT_SENSOR_PIN  25  // ESP32 pin GPIO25 connected to light sensor
#define FORCE_SENSOR_PIN  13  // ESP32 pin GPIO13 connected to force sensor
#define LED_PIN           33  // ESP32 pin GPIO33 connected to LED
#define ANALOG_THRESHOLD  500
#define FORCE_THRESHOLD   500 // Threshold for force detection
#define TIMEOUT           5000 // 5 seconds in milliseconds

//setup mesh
#define   MESH_PREFIX     "ESPMESH"
#define   MESH_PASSWORD   "test1234"
#define   MESH_PORT       5555
//define mesh variables
Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

uint32_t rootnodeID = 2223841881; // root node ID
int nodeNummer = 2; // node number

bool isDark = false;
unsigned long forceBelowThresholdTime = 0;
bool force = false;
//create function
void sendData();
// void checktreshhold();

//create tasks
Task tasksendData( TASK_SECOND * 5 , TASK_FOREVER, &sendData );
// Task taskchecktreshhold( TASK_SECOND * 1 , TASK_FOREVER, &checktreshhold );

// // Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  // Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void sendData() {
  DynamicJsonDocument doc(1024);

  doc["Sensor"] = "druksensor";
  if (force = false) {
    doc["Status"] = "ON";
  } else {
    doc["Status"] = "OFF";
  }

  String message;
  serializeJson(doc, message);
  mesh.sendSingle(rootnodeID, message);

}

// void checktreshhold() {
  // int lightValue = analogRead(LIGHT_SENSOR_PIN);
  // int forceValue = analogRead(FORCE_SENSOR_PIN);

  // // Determine if it's dark
  // isDark = lightValue < ANALOG_THRESHOLD;

  // // Check if force is below threshold
  // if (forceValue < FORCE_THRESHOLD) {
  //   force = false;
  //   if (forceBelowThresholdTime == 0) {
  //     // Start the timer
  //     forceBelowThresholdTime = millis();
  //   } else if (millis() - forceBelowThresholdTime >= TIMEOUT) {
  //     // Turn off the LED after 5 seconds of low force
  //     digitalWrite(LED_PIN, LOW);
  //   }
  // } else {
  //   force = true;
  //   // Reset timer
  //   forceBelowThresholdTime = 0;

  //   if (isDark && forceValue >= FORCE_THRESHOLD) {
  //     digitalWrite(LED_PIN, HIGH);
  //   } else {
  //     digitalWrite(LED_PIN, LOW);
  //   }
  // }

  // Serial.print("Light Value = ");
  // Serial.print(lightValue);
  // Serial.print(", Force Value = ");
  // Serial.println(forceValue);

  // delay(250);
// }

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(FORCE_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  //mesh setup
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler); // mesh init
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.setRoot(false);   
  while (!mesh.getNodeList().size()) {
    delay(1000); // Wait for 1 second
    mesh.update(); // Update the mesh network
    Serial.println("waiting for nodes to connect");
  }
  // // setup senddata tasks
  // userScheduler.addTask(tasksendData);
  // tasksendData.enable();
  // if (tasksendData.isEnabled()) {
  //   Serial.println("tasksendData is enabled");
  // } 
  // // setup force read task
  // userScheduler.addTask(taskchecktreshhold);
  // taskchecktreshhold.enable();
  // if (taskchecktreshhold.isEnabled()) {
  //   Serial.println("tasksendData is enabled");
  // } 
}

void loop() {
  mesh.update();
  int lightValue = analogRead(LIGHT_SENSOR_PIN);
  int forceValue = analogRead(FORCE_SENSOR_PIN);

  // Determine if it's dark
  isDark = lightValue < ANALOG_THRESHOLD;

  // Check if force is below threshold
  if (forceValue < FORCE_THRESHOLD) {
    if (forceBelowThresholdTime == 0) {
      // Start the timer
      forceBelowThresholdTime = millis();
    } else if (millis() - forceBelowThresholdTime >= TIMEOUT) {
      // Turn off the LED after 5 seconds of low force
      digitalWrite(LED_PIN, LOW);
    }
  } else {
    // Reset timer
    forceBelowThresholdTime = 0;

    if (isDark && forceValue >= FORCE_THRESHOLD) {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
  }

  Serial.print("Light Value = ");
  Serial.print(lightValue);
  Serial.print(", Force Value = ");
  Serial.println(forceValue);

  delay(250);
}
