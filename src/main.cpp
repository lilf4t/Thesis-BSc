#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h> // Assuming you are using WiFi for ESP32
#include <PubSubClient.h> // Include MQTT library 

// Definiera RFID-pinnar
#define SS_PIN_1 5  
#define SS_PIN_2 4  
#define RST_PIN 0   

// LED-pinnar
#define LED1 16  // Lyser när RFID1 läser ett kort
#define LED2 17  // Lyser när RFID2 läser ett kort

// Skapa RFID-objekt
MFRC522 rfid1(SS_PIN_1, RST_PIN);
MFRC522 rfid2(SS_PIN_2, RST_PIN);
MFRC522::MIFARE_Key key;

// Initiera arrays som lagrar UID
byte nuidPICC_1[4];
byte nuidPICC_2[4];

// Funktionsprototyper
void printHex(byte *buffer, byte bufferSize);
void printDec(byte *buffer, byte bufferSize);
void readRFID(MFRC522 &rfid, byte *nuidPICC, int readerID, int ledPin);

// WiFi and MQTT credentials
const char* ssid = "eduroam";  // Replace with your WiFi SSID
const char* password = "XBqj+238";  // Replace with your WiFi password
const char* mqtt_server = "172.20.10.3"; // Replace with your MQTT broker IP address
const char* topic = "rfid/reader"; // MQTT topic

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    Serial.begin(115200);
    SPI.begin(); // Initiera SPI-bussen

    // Initiera RFID-läsare
    rfid1.PCD_Init();
    rfid2.PCD_Init();

    // Initiera LED-pinnar
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    
    // Stäng av LED från början
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);

    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Systemet är redo att läsa RFID-taggar från två läsare."));

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    delay(5000);
    Serial.println("ESP32 IP Address: " + WiFi.localIP()); 

    // Connect to MQTT broker
  client.setServer(mqtt_server, 1883); // Default MQTT port
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("ESP32Client")) { // Replace "ESP32Client" with a client ID 
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  } 
}

void loop() {
    // Läs från den första RFID-läsaren och styr LED1
    readRFID(rfid1, nuidPICC_1, 1, LED1);
    
    // Läs från den andra RFID-läsaren och styr LED2
    readRFID(rfid2, nuidPICC_2, 2, LED2);

    client.loop(); // Keep the MQTT client connection alive
}

void readRFID(MFRC522 &rfid, byte *nuidPICC, int readerID, int ledPin) {
    if (!rfid.PICC_IsNewCardPresent()) return;
    if (!rfid.PICC_ReadCardSerial()) return;

    Serial.print(F("[Läsare "));
    Serial.print(readerID);
    Serial.print(F("] PICC typ: "));
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    Serial.println(rfid.PICC_GetTypeName(piccType));

    // Kontrollera om det är en MIFARE Classic-tagg
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
        piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
        piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("Din tagg är inte av typen MIFARE Classic."));
        return;
    }

    // Kontrollera om kortet redan har lästs
    if (rfid.uid.uidByte[0] != nuidPICC[0] ||
        rfid.uid.uidByte[1] != nuidPICC[1] ||
        rfid.uid.uidByte[2] != nuidPICC[2] ||
        rfid.uid.uidByte[3] != nuidPICC[3]) {
        
        Serial.print(F("[Läsare "));
        Serial.print(readerID);
        Serial.println(F("] Nytt kort har detekterats."));

        // Publish reader ID to MQTT topic
        char msg[3];
        sprintf(msg, "%d", readerID); 
        client.publish(topic, msg);  

        // Spara UID i arrayen
        for (byte i = 0; i < 4; i++) {
            nuidPICC[i] = rfid.uid.uidByte[i];
        }

        Serial.println(F("Taggens UID:"));
        Serial.print(F("I hex: "));
        printHex(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();
        Serial.print(F("I dec: "));
        printDec(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();

        // Tänd LED när kort läses
        digitalWrite(ledPin, HIGH);
        delay(500);  // Låt LED lysa i 500 ms
        digitalWrite(ledPin, LOW);

    } else {
        Serial.print(F("[Läsare "));
        Serial.print(readerID);
        // Tänd LED när kort läses
        digitalWrite(ledPin, HIGH);
        delay(500);  // Låt LED lysa i 500 ms
        digitalWrite(ledPin, LOW);
        Serial.println(F("] Kortet har redan lästs tidigare."));
    }

    // Stoppa kommunikationen med kortet
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}

/**
 * Skriver ut en byte-array i hex-format
 */
void printHex(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

/**
 * Skriver ut en byte-array i decimal-format
 */
void printDec(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(' ');
        Serial.print(buffer[i], DEC);
    }
}