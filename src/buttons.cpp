// #include <WiFi.h>
// #include <PubSubClient.h>

// const char* ssid = "fatimas lur";
// const char* password = "jagvetinte";
// const char* mqtt_server = "172.20.10.3";

// WiFiClient espClient;
// PubSubClient client(espClient);

// // GPIO-pins
// const int button1Pin = 4; // Tic tac toe
// const int button2Pin = 5; // Tic tac toe AI
// const int button3Pin = 17; // Othello

// void setupWiFi() {
//     delay(10);
//     Serial.println("Ansluter till WiFi...");
//     WiFi.begin(ssid, password);

//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }
//     Serial.println("\nWiFi ansluten!");
// }

// void reconnectMQTT() {
//     while (!client.connected()) {
//         Serial.print("Försöker ansluta till MQTT...");
//         if (client.connect("ESP32_GameMessage")) {
//             Serial.println(" ansluten!");
//         } else {
//             Serial.print(" misslyckades, rc=");
//             Serial.print(client.state());
//             Serial.println(" Försöker igen om 5 sekunder...");
//             delay(5000);
//         }
//     }
// }

// void setup() {
//     Serial.begin(115200);
//     setupWiFi();
//     client.setServer(mqtt_server, 1883);

//     // Ställ in knappar som ingångar
//     pinMode(button1Pin, INPUT_PULLUP);
//     pinMode(button2Pin, INPUT_PULLUP);
//     pinMode(button3Pin, INPUT_PULLUP);

//     Serial.println("Systemet är redo. Tryck på en knapp för att välja ett spel.");
// }

// void loop() {
//     if (!client.connected()) {
//         reconnectMQTT();
//     }
//     client.loop();

//     // Kontrollera om knapp 1 trycks in
//     if (digitalRead(button1Pin) == LOW) {
//         String gameMessage = "tic_tac_toe";
//         client.publish("server/game/select", gameMessage.c_str());
//         Serial.println("Meddelande skickat: " + gameMessage);
//         delay(1000); // Debounce-fördröjning
//     }

//     // Kontrollera om knapp 2 trycks in
//     if (digitalRead(button2Pin) == LOW) {
//         String gameMessage = "tic_tac_toe_ai";
//         client.publish("server/game/select", gameMessage.c_str());
//         Serial.println("Meddelande skickat: " + gameMessage);
//         delay(1000); // Debounce-fördröjning
//     }

//     // Kontrollera om knapp 3 trycks in
//     if (digitalRead(button3Pin) = LOW) {
//         String gameMessage = "othello";
//         client.publish("server/game/select", gameMessage.c_str());
//         Serial.println("Meddelande skickat: " + gameMessage);
//         delay(1000); // Debounce-fördröjning
//     }
// }