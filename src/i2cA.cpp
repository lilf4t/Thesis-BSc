// #include <Wire.h>
// #include <Adafruit_PN532.h>

// #define TCAADDR 0x70  // Adress till TCA9548A multiplexer
// #define NUM_READERS 7  // Antal NFC-läsare

// // Funktion för att välja kanal på TCA9548A
// void tcaselect(uint8_t i) {
//   if (i > 7) return;
//   Wire.beginTransmission(TCAADDR);
//   Wire.write(1 << i);
//   Wire.endTransmission();
// }

// // Skapa PN532-instans – vi delar på Wire
// Adafruit_PN532 nfc(-1, -1, &Wire);

// void setup() {
//     Serial.begin(115200);
//     Wire.begin();
//     delay(1000);
//     Serial.println("Initierar PN532-läsarna...");
  
//     // Initiera alla läsare
//     for (uint8_t i = 0; i < NUM_READERS; i++) {
//         tcaselect(i);
//         nfc.begin();
//         if (!nfc.getFirmwareVersion()) {
//             Serial.print("Kunde inte hitta PN532 på kanal ");
//             Serial.println(i);
//         } else {
//             Serial.print("PN532 på kanal ");
//             Serial.print(i);
//             Serial.println(" initierad.");
//             nfc.SAMConfig();
//         }
//     }
//     Serial.println("Alla läsare är initierade. Väntar på taggar...");
// }

// // Funktion för att läsa från specifik kanal
// void readFromReader(uint8_t channel) {
//     uint8_t uid[7];
//     uint8_t uidLength;
//     bool found = false;
  
//     tcaselect(channel);
  
//     unsigned long startTime = millis();
//     while (millis() - startTime < 300) {  // Timeout efter 300 ms
//         if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100)) {
//             Serial.print("Läsare ");
//             Serial.print(channel);
//             Serial.print(" hittade tagg: ");
//             for (uint8_t i = 0; i < uidLength; i++) {
//                 Serial.print(uid[i], HEX);
//                 Serial.print(" ");
//             }
//             Serial.println();
//             delay(1000);  // Undvik dubbletter
//             found = true;
//             break;
//         }
//     }
  
//     if (!found) {
//         Serial.print("Läsare ");
//         Serial.print(channel);
//         Serial.println(" hittade ingen tagg.");
//     }
// }

// void loop() {
//     for (uint8_t i = 0; i < NUM_READERS; i++) {
//         readFromReader(i);
//     }
//     delay(500);
// }
