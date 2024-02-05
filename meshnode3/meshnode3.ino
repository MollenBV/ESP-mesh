#include <painlessMesh.h>
#include <ArduinoJson.h>
// Definieer pinnen voor de seriële communicatie met een ander apparaat
#define RXp2 16
#define TXp2 17
//setup mesh
#define   MESH_PREFIX     "ESPMESH"
#define   MESH_PASSWORD   "test1234"
#define   MESH_PORT       5555
//define mesh variables
Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;
uint32_t rootnodeID = 2223841881; // root node ID
int nodeNummer = 3; // node number

// Initialisatie van variabelen voor punten en tellers
int entrancePoint = 1;
int beforePassportPoint = 0;
int afterPassportPoint = 0;
int exitPoint = 0;
bool movementDetected = false; // Vlag om bewegingsdetectie bij te houden

// task creation
void sendData();
//create tasks
Task taskCheckForArduinoData( TASK_SECOND * 5 , TASK_FOREVER, &sendData );


void sendData() {
  // Maak een JSON-document met de benodigde velden
  DynamicJsonDocument doc(200);
  doc["Sensor"] = "Bewegingsensor";
  doc["entrance_point"] = entrancePoint;
  doc["before_passport_point"] = beforePassportPoint;
  doc["after_passport_point"] = afterPassportPoint;
  doc["exit_point"] = exitPoint;

  // Maak een buffer om de JSON-gegevens op te slaan
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


// Loop-functie wordt herhaaldelijk uitgevoerd na de setup
void checkForArduinoData() {
  // Controleer of er gegevens beschikbaar zijn op de tweede seriële poort
  if (Serial2.available()) {
    // Lees de ontvangen gegevens tot het teken '>'
    String message = Serial2.readString();
    Serial.println(message);

    // Stuur JSON-gegevens naar de server alleen als beweging is gedetecteerd
    if (message.equals("Beweging")) {
      sendData();
      movementDetected = false;  // Reset de vlag na het verzenden van gegevens
    }
  }
}


// Setup-functie wordt eenmaal uitgevoerd bij het opstarten van de microcontroller
void setup() {
  // Start de seriële communicatie met baudrate 115200
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);

  //mesh setup
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 1);  //conn
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.setRoot(false);  
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2); 
  // setup tasks
  userScheduler.addTask(taskCheckForArduinoData);
  taskCheckForArduinoData.enable();
  if (taskCheckForArduinoData.isEnabled()) {
    Serial.println("taskCheckForArduinoData is enabled");
  } 
}



void loop() {
  
  mesh.update();

}