#include <WiFi.h>
#include <painlessMesh.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secret.h"
#define STATION_SSID "TestAP"
#define STATION_PASSWORD "test1234"
//setup mesh
#define   MESH_PREFIX     "ESPMESH"
#define   MESH_PASSWORD   "test1234"
#define   MESH_PORT       5555
//servername
const char* serverName = "http://192.168.1.1:5000/data";
// variables to store last received message
DynamicJsonDocument lastReceivedDruksensor(1024); // Adjust the size to fit your JSON data
String jsonString;
//define mesh variables
Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;
int nodeNummer = 1; // node number
int eerste = 0;
//create functions
void sendData();

//create tasks
Task tasksendData( TASK_SECOND * 5 , TASK_FOREVER, &sendData );
//mesh setup
void meshsetup() {
  if (eerste == 0) {  // Corrected the comparison operator and removed the semicolon
    if (eerste == 1) {
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
    }
    eerste = 1;
  }
  
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 1); // mesh init
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.setRoot(true);  
  mesh.setContainsRoot(true);   
  while (!mesh.getNodeList().size()) {
    delay(1000); // Wait for 1 second
    mesh.update(); // Update the mesh network
    Serial.println("waiting for nodes to connect");
  }
}

// connect wifi and send data, then reconnect mesh
void sendData() {
  WiFi.begin(ssid, password);
  Serial.println("WiFi is connecting:");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
  // Send HTTP POST request
  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");

  // Serialize the JSON document to a string
  String jsonData;
  serializeJson(lastReceivedDruksensor, jsonData);
  int httpResponseCode = http.POST(jsonData);
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  WiFi.disconnect(true);
  meshsetup();
}

// // Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  DynamicJsonDocument newMessage(1024);
  deserializeJson(newMessage, msg);
  if (newMessage["Sensor"] == "druksensor") {
    lastReceivedDruksensor = newMessage;
  }
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


void setup() {
  Serial.begin(115200);
  //mesh setup
  meshsetup();
  userScheduler.addTask(tasksendData);
  tasksendData.enable();
  if (tasksendData.isEnabled()) {
    Serial.println("tasksendData is enabled");
  } 
}

void loop() {
  mesh.update();
}

