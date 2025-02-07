// #include <WiFi.h>
// #include <SPI.h>

// #define WIFI_SSID "FATIMA "
// #define WIFI_PASSWORD "fatima2002"
// #define TIMEOUT 20000 // 20s

// void connectToWiFi() {
//   WiFi.scanNetworks();
//   Serial.println("Scanning networks");

//   // printa ut allaaa available networks
//   Serial.print("Available networks: ");
//   for (int i = 0; i < WiFi.scanComplete(); i++) {
//     Serial.print(WiFi.SSID(i));
//     Serial.print(":");
//     Serial.println(WiFi.RSSI(i));
//   }
//   delay(1000);
//   Serial.print("Connecting to Wifi");
//   WiFi.mode(WIFI_STA); // connect till existerande network, AP mode hade varit ifall vi vill att ESP ska ha en egen Wifi
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

//   unsigned long start = millis();

//   while(WiFi.status() != WL_CONNECTED && millis() - start < TIMEOUT) {
//     Serial.print(".");
//     delay(100);
//   }

//   if(WiFi.status() == WL_CONNECTED) {
//     Serial.println("Connected to Wifi");
//     Serial.print("IP address: " + WiFi.localIP().toString());
//   } else {
//     Serial.println("Failed to connect to Wifi");
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   connectToWiFi();
// }

// void loop() {

// }
