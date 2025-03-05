// #include <SPI.h>
// #include <MFRC522.h>
// #include <WiFi.h> 
// #include <PubSubClient.h> 

// // RFID reader pins
// #define SDA_PIN_1 5  // Reader 1
// #define SDA_PIN_2 4  // Reader 2
// #define RST_PIN 0   

// // LED-pins
// #define LED1 16  // Turns on when RFID1 reads a tag
// #define LED2 17  // Turns on when RFID1 reads a tag

// // RFID objects
// MFRC522 rfid1(SDA_PIN_1, RST_PIN);
// MFRC522 rfid2(SDA_PIN_2, RST_PIN);
// MFRC522::MIFARE_Key key;

// // Initialize arrays that will store UIDs (unique identifier)
// byte nuidPICC_1[4];
// byte nuidPICC_2[4];

// // Function prototypes
// void printHex(byte *buffer, byte bufferSize);
// void printDec(byte *buffer, byte bufferSize);
// void readRFID(MFRC522 &rfid, byte *nuidPICC, int readerID, int ledPin);

// // WiFi and MQTT credentials
// const char* ssid = "fatimas lur";
// const char* password = "jagvetinte";
// const char* mqtt_server = "172.20.10.3"; 
// const char* topic = "rfid/reader"; 

// // WiFi and MQTT client objects
// WiFiClient espClient;
// PubSubClient client(espClient);

// // Connect to WiFi
// void connectToWiFi() {
//   WiFi.scanNetworks();
//   Serial.println("Scanning networks");

//   // Print all available networks
//   Serial.print("Available networks: ");
//   for (int i = 0; i < WiFi.scanComplete(); i++) {
//     Serial.print(WiFi.SSID(i));
//     Serial.print(":");
//     Serial.println(WiFi.RSSI(i));
//   }
//   delay(1000);
//   Serial.print("Connecting to Wifi");
//   WiFi.mode(WIFI_STA); // This will connect to an existing network, if you want to create a new WiFi network, use WIFI_AP instead
//   WiFi.begin(ssid, password);

//   unsigned long start = millis();
//   // Timeout 20 seconds in case of failure
//   while(WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
//     Serial.print(".");
//     delay(100);
//   }

//   if(WiFi.status() == WL_CONNECTED) {
//     Serial.println("Connected to Wifi");
//     Serial.println("IP address: " + WiFi.localIP().toString());
//   } else {
//     Serial.println("Failed to connect to Wifi");
//   }
// }

// // Connect to MQTT
// void connectToMQTT() {
//   client.setServer(mqtt_server, 1883);
//   while (!client.connected()) {
//     Serial.println("Attempting MQTT connection...");
//     // Create a random client ID
//     String clientId = "ESP32Client-";
//     clientId += String(random(0xffff), HEX);
//     // Attempt to connect (without username/password; add them as needed)
//     if (client.connect(clientId.c_str())) {
//       Serial.println("connected to MQTT broker");
//       // In case you need to subscribe to topics, do so here:
//       // client.subscribe(topic);
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" try again in 5 seconds");
//       delay(5000);
//     }
//   }
// }

// void loop() {
//     // Read from the first RFID reader and control LED1
//     readRFID(rfid1, nuidPICC_1, 1, LED1);
  
//     // Read from the second RFID reader and control LED2
//     readRFID(rfid2, nuidPICC_2, 2, LED2);

//     // Keep the MQTT client connection alive
//     client.loop(); 
// }

// void setup() {
//     Serial.begin(115200);
//     SPI.begin(); // Initialize SPI bus

//     // Initialize RFID readers
//     rfid1.PCD_Init();
//     rfid2.PCD_Init();

//     // Initialize LED pins
//     pinMode(LED1, OUTPUT);
//     pinMode(LED2, OUTPUT);
    
//     // Turn off LEDs from the beginning
//     digitalWrite(LED1, LOW);
//     digitalWrite(LED2, LOW);

//     // Initialize the key
//     for (byte i = 0; i < 6; i++) {
//         key.keyByte[i] = 0xFF;
//     }

//     connectToWiFi();
//     connectToMQTT();

//     Serial.println(F("The system is ready to read RFID tags from two readers."));
//     Serial.println("-----------------------------------------------------------");
// }

// // Read RFID tags from the specified reader and control the specified LED
// // Read RFID tags from the specified reader and control the specified LED
// void readRFID(MFRC522 &rfid, byte *nuidPICC, int readerID, int ledPin) {
//     if (!rfid.PICC_IsNewCardPresent()) return;
//     if (!rfid.PICC_ReadCardSerial()) return;

//     // Start building the tagData string 
//     String tagData = "";

//     tagData += "[Reader ";
//     tagData += readerID;
//     tagData += "] PICC typ: ";
//     MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
//     tagData += rfid.PICC_GetTypeName(piccType);
//     tagData += "\n";

//     // Check if it is a MIFARE Classic tag
//     if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
//         piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
//         piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
//         tagData += "Your tag is not a MIFARE Classic tag.\n";
//         return;
//     }

//     // Check if the card has already been read before
//     if (rfid.uid.uidByte[0] != nuidPICC[0] ||
//         rfid.uid.uidByte[1] != nuidPICC[1] ||
//         rfid.uid.uidByte[2] != nuidPICC[2] ||
//         rfid.uid.uidByte[3] != nuidPICC[3]) {
        
//         tagData += "[Reader ";
//         tagData += readerID;
//         tagData += "] New card detected.\n";

//         // Save UID in array
//         for (byte i = 0; i < 4; i++) {
//             nuidPICC[i] = rfid.uid.uidByte[i];
//         }

//         tagData += "The tags UID:\n";
//         tagData += "Hex: ";
//         // Use a temporary string to capture the Hex output
//         String hexOutput = "";
//         for (byte i = 0; i < rfid.uid.size; i++) {
//           hexOutput += String(rfid.uid.uidByte[i], HEX);
//           hexOutput += " ";
//         }
//         tagData += hexOutput;
//         tagData += "\n";

//         tagData += "Dec: ";
//         // Use a temporary string to capture the Dec output
//         String decOutput = "";
//         for (byte i = 0; i < rfid.uid.size; i++) {
//           decOutput += String(rfid.uid.uidByte[i], DEC);
//           decOutput += " ";
//         }
//         tagData += decOutput;
//         tagData += "\n"; 

//     } else {
//         tagData += "[Reader ";
//         tagData += readerID;
//         tagData += "] The card has already been read before.\n";
//     }

//     // --- Publish RFID data to MQTT ---
//     Serial.println(F("Preparing to publish RFID data..."));
//     Serial.println("Publishing Tag Data: ");
//     Serial.println(tagData);

//     if (client.connected()) {
//       client.publish(topic, tagData.c_str());
//       Serial.println("Data published successfully!");
//       Serial.println("-----------------------------------------------------------");
//     } else {
//       Serial.println("Error: MQTT client not connected!");
//     }

//     // Turn on LED when card is read
//     digitalWrite(ledPin, HIGH);
//     delay(500); 
//     digitalWrite(ledPin, LOW);

//     // Stop communication with the card
//     rfid.PICC_HaltA();
//     rfid.PCD_StopCrypto1();
// }


// /**
//  * Writes a byte array in hex format to the serial port
//  */
// void printHex(byte *buffer, byte bufferSize) {
//     for (byte i = 0; i < bufferSize; i++) {
//         Serial.print(buffer[i] < 0x10 ? " 0" : " ");
//         Serial.print(buffer[i], HEX);
//     }
// }

// /**
//  * Writes a byte array in dec format to the serial port
//  */
// void printDec(byte *buffer, byte bufferSize) {
//     for (byte i = 0; i < bufferSize; i++) {
//         Serial.print(' ');
//         Serial.print(buffer[i], DEC);
//     }
// }