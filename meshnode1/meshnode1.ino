#include <painlessMesh.h>
#include <ArduinoJson.h>
#include "secret.h"
#include <WebSocketsClient.h>
// Mesh settings
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
painlessMesh  mesh; // needed voor the mesh
WebSocketsClient webSocket; // needed for the websocket
int nodeNummer = 1; // node number

//function
void sendData();

//create task from function
Task tasksendData( TASK_SECOND * 1 , TASK_FOREVER, &sendData );

//websocket event
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

// serialize the last received message's from the ESP sensor nodes then send them via the websocket to the node-red.
void sendData() {
  //Json to String
  String message;
  serializeJson(lastReceivedDruksensor, message);
  Serial.println();
  // Send to Node-Red
  webSocket.sendTXT(message);
  serializeJson(lastReceivedBewegingsensor, message);
  Serial.println();
  // Send to Node-Red
  webSocket.sendTXT(message);
}

// activates when message is received from another mesh
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  DynamicJsonDocument newMessage(1024); // create json document
  deserializeJson(newMessage, msg); // deserialize the json message and load it into newMessage
  // if the message is from the Forcesensor load the newMessage to the lastReceivedForcesensor variable
  if (newMessage["Sensor"] == "Forcesensor") { 
    lastReceivedForcesensor = newMessage;
  }
   // if the message is from the Movementsensor load the newMessage to the lastReceivedMovementsensor variable
  if (newMessage["Sensor"] == "Movementsensor") {
    lastReceivedMovementsensor = newMessage;
  }
  if (newMessage["SequenceNumber"]){ // if the new message has a sequence number
  // send an Acknowledge message back
  String ackMessage = "Ack";
  mesh.sendSingle(from, ackMessage);
  }
}
// Changedconnection
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}
// Changedconnection
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}


void setup() {
  Serial.begin(115200);
  //setup wifi
  WiFi.disconnect();     // required or it will sometimes not form mesh
  WiFi.begin(ssid, password);  // connect to wifi
  //setup websocket
  webSocket.begin("192.168.1.1", 1880, "/data");  //Connect to Node-Red Websocket on this IP, Port and location
  webSocket.onEvent(webSocketEvent);  // wel tell if it is connected or not                   
  webSocket.setReconnectInterval(4000); // interval in at which rate to connect
  //setup mesh
  mesh.setDebugMsgTypes(ERROR | STARTUP);  // For Startup messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 1);  // initialize mesh
  mesh.stationManual(STATION_SSID, STATION_PASSWORD);  //Connect to the Raspberri AP
  mesh.onReceive(&receivedCallback); // set the receivedCallback function
  mesh.onNewConnection(&newConnectionCallback); // set the newConnectionCallback function
  mesh.setRoot(true);                                  //this is the root Node in the Mesh
  mesh.setContainsRoot(true);  // the mesh knows there is a root node
  // wait till mesh is connected
  while (!mesh.getNodeList().size()) {
    delay(1000); // Wait for 1 second
    mesh.update(); // Update the mesh network
    Serial.println("waiting for nodes to connect");
  }
  // enable the task send data
  userScheduler.addTask(tasksendData);
  tasksendData.enable();
  if (tasksendData.isEnabled()) {
    Serial.println("tasksendData is enabled");
  } 
}

void loop() {
  webSocket.loop(); // Continue to maintain Websocket connection
  mesh.update(); // Continue to maintain Mesh connection
}

