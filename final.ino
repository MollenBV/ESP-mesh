#include "painlessMesh.h"
#include <WebSocketsClient.h>
#include <Arduino_JSON.h>

//Link GPIO ports
#define S0 27
#define S1 26
#define S2 33
#define S3 32
#define sensorOut 35
#define button 34

//Setup Mesh, must be the same for all mesh devices
#define MESH_PREFIX "ColorMesh"
#define MESH_PASSWORD "RGB"
#define MESH_PORT 5555  //tcp

//add network to the mesh
#define STATION_SSID "CasaPluto"
#define STATION_PASSWORD "Plutolaan178"

//network variables
const char* ssid = "CasaPluto";
const char* password = "Plutolaan178";

int nodeNummer = 1;  //Must be 1 if root device of mesh, else it should be a unique number

Scheduler userScheduler;  // Create task schedule
painlessMesh mesh;        // Setup Mesh
WebSocketsClient webSocket; // Setup websockets

QueueHandle_t color_queue; // Define queue for color

//create functions
void sendMessage(); 
void color_scan();

//create task
Task taskColorScan(TASK_SECOND * 5, TASK_FOREVER, &color_scan);

// scans color in RGB and outputs the color in a string format
void color_scan() {
  int red = 0;
  int green = 0;
  int blue = 0;
  int redColor = 0;
  int greenColor = 0;
  int blueColor = 0;
  const char* color;
  // Setting red filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  red = pulseIn(sensorOut, LOW);
  redColor = map(red, 7, 78, 255,0);
  // Printing the value on the serial monitor
  Serial.print("R= ");//printing name
  Serial.print(redColor);//printing RED color frequency
  Serial.print(" ");
  delay(100);
  // Setting Green filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  green = pulseIn(sensorOut, LOW);
  greenColor = map(green, 6, 88, 255, 0);
  // Printing the value on the serial monitor
  Serial.print("G= ");//printing name
  Serial.print(greenColor);//printing GREEN color frequency
  Serial.print(" ");
  delay(100);
  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  blue = pulseIn(sensorOut, LOW);
  blueColor = map(blue, 5, 70, 255, 0);
  // Printing the value on the serial monitor
  Serial.print("B= ");//printing name
  Serial.print(blueColor);//printing BLUE color frequency
  Serial.println(" ");
  // Chech which color matches to the rgb values read by the sensor
  if(blueColor > 200 && redColor > 200 && greenColor > 200){
    color = "white";
  }
  if(blueColor > 200 && greenColor > 175 && redColor < 185){
    color = "blue";
  }
  if(blueColor < 50 && redColor < 50 && greenColor < 50){
    color = "black";
  }
  if(blueColor < 200 && redColor > 200 && greenColor > 200){
    color = "yellow";
  }

  // Add color string to the queue and fill up the queue
  // If queue full deleted oldest and add newest value
  if (uxQueueMessagesWaiting(color_queue) == 1000) {
      xQueueReceive(color_queue, &color, 0);
    }

  xQueueSend(color_queue, &color, 0);
  // Send color string to sendMessage function
  sendMessage("color", color);
}


//send data to Node-Red
void sendMessage(const char* type1, const char* value1) {
  //Create JSON file
  DynamicJsonDocument doc(1024);
  const char* choose_info;
  //Randomly generated weight because there was no Weight sensor available
  int weight = random(0,10000);
  // Create error if package seems to be empty or is too heavy for letterbox mail
  const char* error;
  if (weight < 100){
    error = "#FF0000";
  }
  if (value1 == "black" and weight > 7000){
    error = "#FF0000";
  }
  else{
    error="#00FF00";
  }
  // Add corresponding info to each color
  const char* info_list[] = {"regular package", "valuable and insured", "fragile packages", "letterbox size"};
  if (value1 == "white"){
    choose_info = info_list[0];
  }
  if (value1 == "yellow"){
    choose_info = info_list[1];
  }
  if (value1 == "blue"){
    choose_info = info_list[2];
  }
  if (value1 == "black"){
    choose_info = info_list[3];
  }
  //Add values to the json file
  doc["esp_id"] = random(1,26);
  doc[type1] = value1;
  doc["weight_g"] = weight;
  doc["error_code"] = error;
  doc["info"] = choose_info;
  //Json to String
  String message;
  serializeJson(doc, message);
  serializeJson(doc, Serial);
  Serial.println();
  // Send to Node-Red
  webSocket.sendTXT(message);
}

//Check if ESP is connected to the websocket of Node-Red
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("Disconnected from NodeRED websocket");
      break;
    case WStype_CONNECTED:
      Serial.println("Connected to NodeRED websocket");
      break;
  }
}


// MESH
//Print and send incoming data from other mesh nodes
void receivedCallback(uint32_t from, String& msg) {
  Serial.printf("Received from: %u msg= %s\n", from, msg.c_str());
  webSocket.sendTXT(msg); 
}
//Nofify if there is a new connection
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}
//Notify if there is a change in connection
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}
//Print time of node
void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}


void setup() {
  //Serial Speed
  Serial.begin(115200);
  //Initialize GPIO pins for color scan
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  //Calibrate the color sensor
  digitalWrite(S0,HIGH);
  digitalWrite(S1,HIGH);
  //wifi setup
  WiFi.setSleep(false);  // Turn off power safe feature
  WiFi.disconnect();     //Anti bug function
  WiFi.begin(ssid, password);  //begin Wifi
  //websocket setup
  webSocket.begin("82.74.19.84", 1880, "/ws/something");  //Connect to Node-Red Websocket on this IP, Port and location
  webSocket.onEvent(webSocketEvent);                     
  webSocket.setReconnectInterval(5000);
  //Setup color_queue
  color_queue = xQueueCreate(1000, sizeof(float));
  //mesh setup
  mesh.setDebugMsgTypes(ERROR | STARTUP);  // set before init() so that you can see startup messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);  //connect with the general Mesh. Should be the same for every node
  mesh.stationManual(STATION_SSID, STATION_PASSWORD);  //Connect with the outside network
  mesh.setRoot(true);                                  //this is the root Node in the Mesh
  mesh.setContainsRoot(true);                          //  There is a root. all nodes should know this
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  //setup tasks
  userScheduler.addTask(taskColorScan);
  taskColorScan.enable();
}

void loop() {
  webSocket.loop();  // Keep the socket alive
  mesh.update();     //Keep the Mesh alive
}