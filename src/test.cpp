// #include <WiFi.h>
//  #include <PubSubClient.h>
//  #include <SPI.h>
//  #include <Adafruit_PN532.h>
 
//  // CS-pins
//  Adafruit_PN532 reader1(5);
//  Adafruit_PN532 reader2(17);
//  Adafruit_PN532 reader3(21);
//  Adafruit_PN532 reader4(25);
//  Adafruit_PN532 reader5(26);
 
//  Adafruit_PN532* readers[5] = { &reader1, &reader2, &reader3, &reader4, &reader5 };
//  const char* names[5] = { "reader1", "reader2", "reader3", "reader4", "reader5" };
//  bool readerActive[5] = { false };
 
//  const char* ssid = "fatimas lur";
//  const char* password = "jagvetinte";
//  const char* mqtt_server = "172.20.10.3";
 
 
//  WiFiClient espClient;
//  PubSubClient client(espClient);
 
//  void setupWiFi() {
//    WiFi.begin(ssid, password);
//    while (WiFi.status() != WL_CONNECTED) {
//      delay(500);
//      Serial.print(".");
//    }
//    Serial.println("WiFi connected");
//  }
 
//  void reconnectMQTT() {
//    while (!client.connected()) {
//      if (client.connect("ESP32A")) break;
//      delay(1000);
//    }
//  }
 
//  void setup() {
//    Serial.begin(115200);
//    setupWiFi();
//    client.setServer(mqtt_server, 1883);
 
//    for (int i = 0; i < 5; i++) {
//      readers[i]->begin();
//      uint32_t versiondata = readers[i]->getFirmwareVersion();
//      if (versiondata) {
//        readers[i]->SAMConfig();
//        readerActive[i] = true;
//        Serial.print(names[i]); Serial.println(" aktiv");
//      }
//    }
//  }
 
//  void loop() {
//    if (!client.connected()) reconnectMQTT();
//    client.loop();
 
//    // Check each reader in sequence with a small delay between them
//    for (int i = 0; i < 5; i++) {
//      if (!readerActive[i]) continue;
 
//      // Small delay between checking each reader to give it time to initialize
//      delay(10);
 
//      uint8_t uid[7];
//      uint8_t uidLength;
 
//      if (readers[i]->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100)) {
//        char uidStr[32] = "";
//        for (int j = 0; j < uidLength; j++) {
//          char byteStr[4];
//          sprintf(byteStr, "%02X", uid[j]);
//          strcat(uidStr, byteStr);
//        }
 
//        String payload = String(names[i]) + ":" + uidStr;
//        client.publish("rfid/reader", payload.c_str());
//        Serial.println("Skickat → " + payload);
//      }
//    }
 
//    // No delay at the end of the loop to keep checking as fast as possible
//  }


