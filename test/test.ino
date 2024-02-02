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
#define HOSTNAME "Bridge"
//servername
const char* serverName = "http://192.168.1.1:5000/data";
// variables to store last received message
DynamicJsonDocument lastReceivedDruksensor(1024); // Adjust the size to fit your JSON data
String jsonString;
//define mesh variables
Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;
int nodeNummer = 1; // node number

IPAddress getlocalIP();

IPAddress myIP(0.0.0.0);


IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}

//create tasks
Task tasksendData( TASK_SECOND * 5 , TASK_FOREVER, &sendData );
//mesh setup
void meshsetup() {
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 1); // mesh init
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);
  delay(1000);
  mesh.setRoot(true);  
  mesh.setContainsRoot(true);   
  while (!mesh.getNodeList().size()) {
    delay(1000); // Wait for 1 second
    mesh.update(); // Update the mesh network
    Serial.println("waiting for nodes to connect");
  }
}


// // Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
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

}

void loop() {
  mesh.update();
  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());

}

