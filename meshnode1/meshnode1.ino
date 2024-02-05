#include <painlessMesh.h>
#include <ArduinoJson.h>
#include "secret.h"
#include <WebSocketsClient.h>

//setup mesh
#define   MESH_PREFIX     "ESPMESH"
#define   MESH_PASSWORD   "test1234"
#define   MESH_PORT       5555
//raspberri AP
#define STATION_SSID "TestAP"
#define STATION_PASSWORD "test1234"

// variables to store last received message
DynamicJsonDocument lastReceivedDruksensor(1024); // Adjust the size to fit your JSON data
DynamicJsonDocument lastReceivedBewegingsensor(1024); // Adjust the size to fit your JSON data

//define mesh variables
Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;
WebSocketsClient webSocket;
int nodeNummer = 1; // node number
bool isConnected = false;

//create functions
void sendData();

//create tasks
Task tasksendData( TASK_SECOND * 1 , TASK_FOREVER, &sendData );

//websocket
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("Disconnected!\n");
      isConnected = false; // Update connection status
      break;
    case WStype_CONNECTED:
      Serial.printf("Connected to url: %s\n", payload);
      isConnected = true;  // Update connection status
      // Send a message to the server
      webSocket.sendTXT("Hello from ESP32!");
      break;
    case WStype_TEXT:
      Serial.printf(" Received text: %s\n", payload);
      break;
  }
}

// connect wifi and send data, then reconnect mesh
void sendData() {
  //Json to String
  String message;
  serializeJson(lastReceivedDruksensor, message);
  Serial.println();
  // Send to Node-Red
  webSocket.sendTXT(message);
}

// // Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  DynamicJsonDocument newMessage(1024);
  deserializeJson(newMessage, msg);
  if (newMessage["Sensor"] == "druksensor") {
    lastReceivedDruksensor = newMessage;
  }
  if (newMessage["Sensor"] == "Bewegingsensor") {
    lastReceivedBewegingsensor = newMessage;
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
  //setup wifi
  WiFi.setSleep(false);  // Turn off power safe feature
  WiFi.disconnect();     //Anti bug function
  WiFi.begin(ssid, password);  //begin Wifi 
  //setup websocket
  webSocket.begin("192.168.1.1", 1880, "/data");  //Connect to Node-Red Websocket on this IP, Port and location
  webSocket.onEvent(webSocketEvent);                     
  webSocket.setReconnectInterval(4000);
  //setup mesh
  mesh.setDebugMsgTypes(ERROR | STARTUP);  // set before init() so that you can see startup messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 1);  //connect with the general Mesh. Should be the same for every node
  mesh.stationManual(STATION_SSID, STATION_PASSWORD);  //Connect with the outside network
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.setRoot(true);                                  //this is the root Node in the Mesh
  mesh.setContainsRoot(true); 
  // wait till mesh is connected
  while (!mesh.getNodeList().size()) {
    delay(1000); // Wait for 1 second
    mesh.update(); // Update the mesh network
    Serial.println("waiting for nodes to connect");
  }
  // 
  userScheduler.addTask(tasksendData);
  tasksendData.enable();
  if (tasksendData.isEnabled()) {
    Serial.println("tasksendData is enabled");
  } 
}

void loop() {
  webSocket.loop(); // Continue to maintain WebSocket connection
  mesh.update();
}

