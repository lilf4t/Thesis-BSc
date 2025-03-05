#include <SPI.h>
#include <Adafruit_PN532.h>
#include <WiFi.h> 
#include <PubSubClient.h> 

// RFID reader pins
#define PN532_CS_1 5  
#define PN532_CS_2 4

// LED-pins
#define LED1 16  // Turns on when RFID1 reads a tag
#define LED2 17  // Turns on when RFID2 reads a tag (currently unused, but you can adapt the code)

// RFID object
Adafruit_PN532 nfc1(PN532_CS_1);
Adafruit_PN532 nfc2(PN532_CS_2);

// Initialize strings to store last read UIDs for each reader
String lastReadUid1 = ""; 
String lastReadUid2 = ""; 

// WiFi and MQTT credentials
const char* ssid = "fatimas lur";
const char* password = "jagvetinte"; 
const char* mqtt_server = "172.20.10.3"; 
const char* topic = "rfid/reader"; 

// WiFi and MQTT client objects
WiFiClient espClient;
PubSubClient client(espClient);

void readAndHandleRFID(Adafruit_PN532 *nfc, int ledPin); 

// Connect to WiFi
void connectToWiFi() {
  WiFi.scanNetworks();
  Serial.println("Scanning networks");

  // Print all available networks
  Serial.print("Available networks: ");
  for (int i = 0; i < WiFi.scanComplete(); i++) {
    Serial.print(WiFi.SSID(i));
    Serial.print(":");
    Serial.println(WiFi.RSSI(i));
  }
  delay(1000);
  Serial.print("Connecting to Wifi");
  WiFi.mode(WIFI_STA); // This will connect to an existing network, if you want to create a new WiFi network, use WIFI_AP instead
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  // Timeout 20 seconds in case of failure
  while(WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    Serial.print(".");
    delay(100);
  }

  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to Wifi");
    Serial.println("IP address: " + WiFi.localIP().toString());
  } else {
    Serial.println("Failed to connect to Wifi");
  }
}

// Connect to MQTT
void connectToMQTT() {
  client.setServer(mqtt_server, 1883);
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect (without username/password; add them as needed)
    if (client.connect(clientId.c_str())) {
      Serial.println("connected to MQTT broker");
      // In case you need to subscribe to topics, do so here:
      // client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Function to read from an RFID reader and handle the data
void readAndHandleRFID(Adafruit_PN532 *nfc, int readerID, int ledPin, String &lastReadUid) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  
  uint8_t uidLength;     

  success = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    // --- Convert the UID to a String for easier handling ---
    String currentUidHex = "";
    for (uint8_t i = 0; i < uidLength; i++) {
      currentUidHex += String(uid[i], HEX);
      currentUidHex += " "; // Add space for readability
    }

    // Check if the card has already been read before on this reader
    if (currentUidHex != lastReadUid) { 
      lastReadUid = currentUidHex;

      // Start building the tagData string (similar to main.cpp)
      String tagData = "[Reader ";
      tagData += readerID; 
      tagData += "] New card detected.\n";
      tagData += "The tag's UID (Hex): ";
      tagData += currentUidHex;
      tagData += "\n";

       // --- Publish RFID data to MQTT ---
      Serial.println(F("Preparing to publish RFID data..."));
      Serial.println("Publishing Tag Data: ");
      Serial.println(tagData);

      if (client.connected()) {
        client.publish(topic, tagData.c_str());
        Serial.println("Data published successfully!");
        Serial.println("-----------------------------------------------------------");
      } else {
        Serial.println("Error: MQTT client not connected!");
      }

      // Turn on LED when a new card is read
      digitalWrite(ledPin, HIGH);
      delay(500); 
      digitalWrite(ledPin, LOW);
    } else {
      Serial.print("Reader ");
      Serial.print(readerID);
      Serial.println(" - Card already read."); 
    } 
  } else {
    // Handle read errors (add more specific error handling if needed)
    Serial.print("Reader ");
    Serial.print(readerID);
    Serial.println(": Error: Failed to read a tag!"); 
  }
}

void loop() {
  readAndHandleRFID(&nfc1, 1, LED1, lastReadUid1);
  readAndHandleRFID(&nfc2, 2, LED2, lastReadUid2);
  client.loop();
}


void setup() {
  Serial.begin(115200);
  SPI.begin(); // Initialize SPI bus

  // Explicitly set SPI settings for PN532 (before nfc.begin()) 
  SPI.setClockDivider(SPI_CLOCK_DIV16);  // Set SPI clock speed
  SPI.setDataMode(SPI_MODE0);           // Set SPI mode (MODE0 is common)
  SPI.setBitOrder(MSBFIRST);            // Set bit order

  // Initialize RFID reader
  nfc1.begin(); 
  nfc2.begin();

  // configure board to read RFID tags
  nfc1.SAMConfig();
  nfc2.SAMConfig();

  // Initialize LED pins
  pinMode(LED1, OUTPUT);

  // Turn off LEDs from the beginning
  digitalWrite(LED1, LOW); 

  // WiFi and MQTT setup
  connectToWiFi();
  connectToMQTT();

  Serial.println(F("The system is ready to read RFID tags."));
  Serial.println("-----------------------------------------------------------");
}