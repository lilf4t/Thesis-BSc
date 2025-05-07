// /*
// This code includes code for one reader and two buttons. 
// */
// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <SPI.h>
// #include <Adafruit_PN532.h>

// // CS-pin for NFC reader
// Adafruit_PN532 reader14(17);
// Adafruit_PN532 reader15(4);
// Adafruit_PN532 reader16(21); 

// // Array of readers and their names
// Adafruit_PN532* readers[3] = { &reader14, &reader15, &reader16 };
// const char* names[3] = { "reader14", "reader15", "reader16" };
// bool readerActive[3] = { false, false, false };

// // Track the last read UID for each reader
// String lastReadUID[3] = { "", "", "" };
// // Track if a tag is currently present on each reader
// bool tagPresent[3] = { false, false, false };

// // Button pins - using different pins to avoid conflicts
// const int button1Pin = 32; // Tic tac toe
// const int button2Pin = 33; // Tic tac toe AI

// // WiFi and MQTT settings
// const char* ssid = "fatimas lur";
// const char* password = "jagvetinte";
// const char* mqtt_server = "172.20.10.3";

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
//     Serial.print("Attempting MQTT connection...");
//     if (client.connect("ESP32C")) {
//       Serial.println("connected");
//       // Subscribe to topics if needed
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" try again in 5 seconds");
//       delay(5000);
//     }
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   setupWiFi();
//   client.setServer(mqtt_server, 1883);

//   // Setup buttons with internal pullup resistors
//   pinMode(button1Pin, INPUT_PULLUP);
//   pinMode(button2Pin, INPUT_PULLUP);

//   // Setup all NFC readers
//   for (int i = 0; i < 3; i++) {
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
//   if (!client.connected()) {
//     reconnectMQTT();
//   }
//   client.loop();

//   // Check buttons with debouncing
//   static unsigned long lastButton1Press = 0;
//   static unsigned long lastButton2Press = 0;
//   unsigned long currentMillis = millis();

//   // Button 1 (Tic tac toe)
//   if (digitalRead(button1Pin) == LOW) {
//     if (currentMillis - lastButton1Press > 1000) { // 1 second debounce
//       String gameMessage = "tic_tac_toe";
//       if (client.publish("server/game/select", gameMessage.c_str())) {
//         Serial.println("Button 1 pressed - Message sent: " + gameMessage);
//       } else {
//         Serial.println("Failed to send message for button 1");
//       }
//       lastButton1Press = currentMillis;
//     }
//   }

//   // Button 2 (Tic tac toe AI)
//   if (digitalRead(button2Pin) == LOW) {
//     if (currentMillis - lastButton2Press > 1000) { // 1 second debounce
//       String gameMessage = "tic_tac_toe_ai";
//       if (client.publish("server/game/select", gameMessage.c_str())) {
//         Serial.println("Button 2 pressed - Message sent: " + gameMessage);
//       } else {
//         Serial.println("Failed to send message for button 2");
//       }
//       lastButton2Press = currentMillis;
//     }
//   }

//   // Check each reader in sequence
//   for (int i = 0; i < 3; i++) {
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
//         client.publish("game/nfc/reader", payload.c_str());
//         Serial.println("Skickat → " + payload);
        
//         // Update tracking variables
//         lastReadUID[i] = currentUID;
//         tagPresent[i] = true;
//       }   
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
