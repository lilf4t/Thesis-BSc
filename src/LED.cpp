// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <Adafruit_NeoPixel.h>

// // === LED-inställningar ===
// #define LED_PIN    4
// #define NUM_LEDS   16
// Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);


// // === WiFi & MQTT ===
// const char* ssid = "LAPTOP-24IM1PFV 1456";           // ZyXELDC2470
// const char* password = "jagvetinte";       // KEHXTVRUHF7JF
// const char* mqtt_server = "192.168.137.1";  // 192.168.1.57, 172.20.10.3

// WiFiClient espClient;
// PubSubClient client(espClient);

// // === Färger ===
// uint32_t COLOR_OFF        = leds.Color(0, 0, 0);
// uint32_t COLOR_AVAILABLE_X = leds.Color(0, 0, 255);    // Blue for X's available moves
// uint32_t COLOR_AVAILABLE_O = leds.Color(255, 0, 0);    // Red for O's available moves
// uint32_t COLOR_TAKEN      = leds.Color(255, 255, 255); // White for taken spots
// uint32_t COLOR_FLASH_BLUE = leds.Color(0, 0, 255);     // Blue for X's win flash
// uint32_t COLOR_FLASH_RED  = leds.Color(255, 0, 0);     // Red for O's win flash
// uint32_t COLOR_FLASH_WHITE = leds.Color(255, 255, 255);// White for draw flash

// // === Hjälp: mappa (row, col) till led-index ===
// int getIndex(int row, int col) {
//   // Direct mapping for each position
//   if (row == 0) {
//     return col;  // First row: 0,1,2
//   } else if (row == 1) {
//     return (2 - col) + 5;  // Second row: 5,6,7 (inverted)
//   } else {
//     return col + 8;  // Third row: 8,9,10
//   }
// }

// void callback(char* topic, byte* payload, unsigned int length) {
//   String msg;
//   for (int i = 0; i < length; i++) {
//       msg += (char)payload[i];
//   }
//   Serial.print("Mottaget meddelande på topic: ");
//   Serial.println(topic);
//   Serial.print("Payload: ");
//   Serial.println(msg);

//   if (String(topic) == "game/led_state") {
//     Serial.println("Processing LED state message");
//     int row, col;
//     char state[20];
//     if (sscanf(msg.c_str(), "%d,%d,%s", &row, &col, state) == 3) {
//       int index = getIndex(row, col);
//       Serial.print("LED-index: ");
//       Serial.println(index);
//       Serial.print("State: ");
//       Serial.println(state);

//       if (String(state) == "available_x") {
//         leds.setPixelColor(index, COLOR_AVAILABLE_X);
//       } else if (String(state) == "available_o") {
//         leds.setPixelColor(index, COLOR_AVAILABLE_O);
//       } else if (String(state) == "taken") {
//         leds.setPixelColor(index, COLOR_TAKEN);
//       } else if (String(state) == "flash_blue") {
//         leds.setPixelColor(index, COLOR_FLASH_BLUE);
//       } else if (String(state) == "flash_red") {
//         leds.setPixelColor(index, COLOR_FLASH_RED);
//       } else if (String(state) == "flash_white") {
//         leds.setPixelColor(index, COLOR_FLASH_WHITE);
//       }
//       leds.show();
//       Serial.println("LEDs updated");
//     } else {
//       Serial.println("Failed to parse LED state message");
//     }
//   }
// }

// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("Försöker ansluta till MQTT... ");
//     if (client.connect("ESP32_LED")) {
//       Serial.println("ansluten");
//       client.subscribe("game/board");
//       client.subscribe("game/led_state");  // Make sure we're subscribing to the correct topic
//       Serial.println("Subscribed to game/led_state");
//     } else {
//       Serial.print("misslyckades, rc=");
//       Serial.print(client.state());
//       delay(1000);
//     }
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   leds.begin();
//   leds.clear();
//   leds.show();  // Make sure to show the initial clear state

//   // Test all LEDs with different colors
//   Serial.println("Testing all LEDs...");
//   for (int i = 0; i < NUM_LEDS; i++) {
//     leds.setPixelColor(i, leds.Color(255, 255, 255));  // White
//   }
//   leds.show();
//   delay(1000);
  
//   // Clear all LEDs
//   leds.clear();
//   leds.show();
//   Serial.println("LED test complete");

//   WiFi.begin(ssid, password);
//   Serial.print("Ansluter till WiFi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500); Serial.print(".");
//   }
//   Serial.println("\nWiFi ansluten!");
//   client.setServer(mqtt_server, 1883);
//   client.setCallback(callback);
// }

// void loop() {
//   // Hantera MQTT-anslutning och inkommande meddelanden
//   if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();  // Hantera inkommande MQTT-meddelanden
// }