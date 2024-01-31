#include <WiFi.h>
#include <painlessMesh.h>

//setup mesh
#define   MESH_PREFIX     "ESPMESH"
#define   MESH_PASSWORD   "test1234"
#define   MESH_PORT       5555
//define mesh variables
Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

int nodeNummer = 2; // node number

//create functions
void sendMessage(); 
void sendData();

//create tasks
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task tasksendData( TASK_SECOND * 1 , TASK_FOREVER, &sendData );

void sendMessage() {
  String msg = "Hi from node_2-";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}


void sendData() {
//voeg hier je data toe
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
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}


void setup() {
  Serial.begin(115200);

  //mesh setup
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 13); // mesh init
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.setRoot(false);   
  // setup tasks
  userScheduler.addTask(taskSendMessage);
  userScheduler.addTask(tasksendData);
//   tasksendData.enable();
  taskSendMessage.enable();
//   if (tasksendData.isEnabled()) {
//     Serial.println("tasksendData is enabled");
//   } 
    if (taskSendMessage.isEnabled()) {
    Serial.println("taskSendMessage is enabled");
  } 

}

void loop() {
  mesh.update();

}

