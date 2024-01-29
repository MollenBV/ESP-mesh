// #include <WiFi.h>
// #include <HTTPClient.h>
// #include "secret.h"

// int connectionAttempts = 0;

// void setup() {
//   Serial.begin(115200);
//   WiFi.begin(ssid, password);
  
//   while (WiFi.status() != WL_CONNECTED) {
//   Serial.println("Connection to Raspberry Pi failed, retrying...");
//   connectionAttempts++;
//   if (connectionAttempts > 5) {
//     Serial.println("Failed to connect after 5 attempts, going to sleep");
//     // Put the ESP32 into deep sleep mode here, if desired
//     return;
//   }
//   delay(1000); // Wait a second before trying to reconnect
// }

//   // Make sure your Raspberry Pi IP address and the endpoint match
//   String serverName = "http://10.42.0.1:5000/data";

//   // JSON data you want to send
//   String jsonData = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

//   // Your code to send the HTTP POST request
//   HTTPClient http;   
//   http.begin(serverName);
//   http.addHeader("Content-Type", "application/json"); // Specify content-type header

//   int httpResponseCode = http.POST(jsonData);   // Send the actual POST request

//   if (httpResponseCode>0) {
//     String response = http.getString(); // Get the response to the request
//     Serial.println(httpResponseCode);   // Print return code
//     Serial.println(response);           // Print request answer
//   } else {
//     Serial.print("Error on sending POST: ");
//     Serial.println(httpResponseCode);
//   }

//   http.end(); // Free resources
// }

// void loop() {
//   // Nothing here for now
// }

// Static IP configuration details
// IPAddress staticIP(10, 42, 0, 145); // Set the static IP address you want for the ESP32
// IPAddress gateway(10, 42, 0, 1);    // Set the gateway IP address (usually the router's IP)
// IPAddress subnet(255, 255, 255, 0);   // Set the subnet mask (255.255.255.0 is common)
// Optional: DNS Server. Only use if needed.
// IPAddress primaryDNS(8, 8, 8, 8);     // Optional: Google's primary DNS
// IPAddress secondaryDNS(8, 8, 4, 4);   // Optional: Google's secondary DNS

#include <WiFi.h>

const char* ssid     = "TestAP";
const char* password = "testap1234";


void setup() {
    Serial.begin(115200);

    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("WiFi connected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
  
}

// #include <BLEDevice.h>
// #include <BLEServer.h>
// #include <BLEUtils.h>
// #include <BLE2902.h>
 
// BLEServer *pServer = NULL;
// BLECharacteristic * pTxCharacteristic;
// bool deviceConnected = false;
// bool oldDeviceConnected = false;
// uint8_t txValue = 0;
 
// // See the following for generating UUIDs:
// // https://www.uuidgenerator.net/
 
// #define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
// #define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
// #define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
 
 
// class MyServerCallbacks: public BLEServerCallbacks {
//     void onConnect(BLEServer* pServer) {
//       deviceConnected = true;
//     };
 
//     void onDisconnect(BLEServer* pServer) {
//       deviceConnected = false;
//     }
// };
 
// class MyCallbacks: public BLECharacteristicCallbacks {
//     void onWrite(BLECharacteristic *pCharacteristic) {
//       std::string rxValue = pCharacteristic->getValue();
 
//       if (rxValue.length() > 0) {
//         Serial.println("*********");
//         Serial.print("Received Value: ");
//         for (int i = 0; i < rxValue.length(); i++)
//           Serial.print(rxValue[i]);
 
//         Serial.println();
//         Serial.println("*********");
//       }
//     }
// };
 
 
// void setup() {
//   Serial.begin(115200);
 
//   // Create the BLE Device
//   BLEDevice::init("UART Service For ESP32");
 
//   // Create the BLE Server
//   pServer = BLEDevice::createServer();
//   pServer->setCallbacks(new MyServerCallbacks());
 
//   // Create the BLE Service
//   BLEService *pService = pServer->createService(SERVICE_UUID);
 
//   // Create a BLE Characteristic
//   pTxCharacteristic = pService->createCharacteristic(
//                                         CHARACTERISTIC_UUID_TX,
//                                         BLECharacteristic::PROPERTY_NOTIFY
//                                     );
                      
//   pTxCharacteristic->addDescriptor(new BLE2902());
 
//   BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
//                                              CHARACTERISTIC_UUID_RX,
//                                             BLECharacteristic::PROPERTY_WRITE
//                                         );
 
//   pRxCharacteristic->setCallbacks(new MyCallbacks());
 
//   // Start the service
//   pService->start();
 
//   // Start advertising
//   pServer->getAdvertising()->start();
//   Serial.println("Waiting a client connection to notify...");
// }
 
// void loop() {
 
//     if (deviceConnected) {
//         pTxCharacteristic->setValue(&txValue, 1);
//         pTxCharacteristic->notify();
//         txValue++;
//         delay(10); // bluetooth stack will go into congestion, if too many packets are sent
//     }
 
//     // disconnecting
//     if (!deviceConnected && oldDeviceConnected) {
//         delay(500); // give the bluetooth stack the chance to get things ready
//         pServer->startAdvertising(); // restart advertising
//         Serial.println("start advertising");
//         oldDeviceConnected = deviceConnected;
//     }
//     // connecting
//     if (deviceConnected && !oldDeviceConnected) {
//         // do stuff here on connecting
//         oldDeviceConnected = deviceConnected;
//     }
// }

