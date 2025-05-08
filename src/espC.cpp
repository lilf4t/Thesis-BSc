// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <SPI.h>
// #include <Adafruit_PN532.h>

// // CS-pins
// Adafruit_PN532 reader10(25);
// Adafruit_PN532 reader11(26);
// Adafruit_PN532 reader12(5);
// Adafruit_PN532 reader13(17);

// Adafruit_PN532* readers[5] = { &reader10, &reader11, &reader12, &reader13 };
// const char* names[4] = { "reader10", "reader11", "reader12", "reader13" };
// bool readerActive[4] = { false };

// // Track the last read UID for each reader
// String lastReadUID[4] = { "", "", "", "" };
// // Track if a tag is currently present on each reader
// bool tagPresent[4] = { false, false, false, false };

// // const char* ssid = "fatimas lur";           // ZyXELDC2470
// // const char* password = "jagvetinte";       // KEHXTVRUHF7JF
// // const char* mqtt_server = "172.20.10.3";  // 192.168.1.57, 172.20.10.3

// const char* ssid = "LAPTOP-24IM1PFV 1456";           // ZyXELDC2470
// const char* password = "jagvetinte";       // KEHXTVRUHF7JF
// const char* mqtt_server = "10.80.4.104";  // 192.168.1.57, 172.20.10.3

// WiFiClient espClient;
// PubSubClient client(espClient);

// void setupWiFi() {
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("WiFi connected");
// }

// void reconnectMQTT() {
//   while (!client.connected()) {
//     if (client.connect("ESP32C")) break;
//     delay(1000);
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   setupWiFi();
//   client.setServer(mqtt_server, 1883);

//   for (int i = 0; i < 4; i++) {
//     readers[i]->begin();
//     uint32_t versiondata = readers[i]->getFirmwareVersion();
//     if (versiondata) {
//       readers[i]->SAMConfig();
//       readerActive[i] = true;
//       Serial.print(names[i]); Serial.println(" aktiv");
//     }
//   }
// }

// void loop() {
//   if (!client.connected()) reconnectMQTT();
//   client.loop();

//   // Check each reader in sequence with a small delay between them
//   for (int i = 0; i < 4; i++) {
//     if (!readerActive[i]) continue;

//     // Small delay between checking each reader to give it time to initialize
//     delay(10);

//     uint8_t uid[7];
//     uint8_t uidLength;

//     if (readers[i]->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100)) {
//       // Tag detected
//       char uidStr[32] = "";
//       for (int j = 0; j < uidLength; j++) {
//         char byteStr[4];
//         sprintf(byteStr, "%02X", uid[j]);
//         strcat(uidStr, byteStr);
//       }
      
//       String currentUID = String(uidStr);
      
//       // Only process if this is a new tag or a tag that was previously removed
//       if (currentUID != lastReadUID[i] || !tagPresent[i]) {
//         String payload = String(names[i]) + ":" + currentUID;
//         client.publish("game/nfc/reader", payload.c_str());  // Changed topic
//         Serial.println("Skickat → " + payload);
        
//         // Update tracking variables
//         lastReadUID[i] = currentUID;
//         tagPresent[i] = true;
//         }   
//     } else {
//       // No tag detected on this reader
//       if (tagPresent[i]) {
//         // Tag was removed, reset the tracking
//         tagPresent[i] = false;
//         Serial.print(names[i]); Serial.println(" tag removed");
//       }
//     }
//   }
// }