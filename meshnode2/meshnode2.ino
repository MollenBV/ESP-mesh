#include <WiFi.h>
#include <painlessMesh.h>
#include <ArduinoJson.h>

//setup mesh
#define   MESH_PREFIX     "ESPMESH"
#define   MESH_PASSWORD   "test1234"
#define   MESH_PORT       5555
//define mesh variables
Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

uint32_t rootnodeID = 2223841881; // root node ID
int nodeNummer = 2; // node number

//create function
void sendData();

//create tasks
Task tasksendData( TASK_SECOND * 5 , TASK_FOREVER, &sendData );


void sendData() {
  DynamicJsonDocument doc(1024);

  doc["Sensor"] = "druksensor";
  doc["Status"] = "AAN";

  String message;
  serializeJson(doc, message);
  mesh.sendSingle(rootnodeID, message);

}

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


void setup() {
  Serial.begin(115200);

  //mesh setup
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler); // mesh init
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.setRoot(false);   
  // setup tasks
  userScheduler.addTask(tasksendData);
  tasksendData.enable();
  if (tasksendData.isEnabled()) {
    Serial.println("tasksendData is enabled");
  } 
}

void loop() {
  
  mesh.update();

}

