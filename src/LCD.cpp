// #include <TFT_eSPI.h>  
// #include <SPI.h>       
// #include <WiFi.h>
// #include <PubSubClient.h>

// // WiFi credentials
// // const char* ssid = "fatimas lur";   
// // const char* password = "jagvetinte";       
// // const char* mqtt_server = "172.20.10.3";  

// const char* ssid = "LAPTOP-24IM1PFV 1456";           // ZyXELDC2470
// const char* password = "jagvetinte";       // KEHXTVRUHF7JF
// const char* mqtt_server = "10.80.4.104";  // 192.168.1.57, 172.20.10.3


// // MQTT client
// WiFiClient lcdClient; 
// PubSubClient client(lcdClient); 

// // Player names
// String player1 = "";
// String player2 = "";

// bool gameStarted = false;  // Track if game has started

// TFT_eSPI tft1 = TFT_eSPI();  
// TFT_eSPI tft2 = TFT_eSPI();  

// // Function to connect to WiFi
// void setupWiFi() {
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("WiFi connected");
// }

// // Function to reconnect to MQTT broker
// void reconnectMQTT() {
//   while (!client.connected()) {
//     if (client.connect("ESP32_LCD")) break;
//     delay(1000);
//   }
// }

// // This function displays the player names on the top section of the screen
// void displayPlayerNames() {

//     // Display on first screen
//     digitalWrite(TFT_CS, LOW);    // Select first display
//     digitalWrite(TFT_CS2, HIGH);  // Ensure second is deselected
    
//     tft1.fillRect(0, 0, tft1.width(), 60, TFT_BLACK);
//     tft1.setTextColor(TFT_WHITE, TFT_BLACK);
//     tft1.setTextSize(2);
//     tft1.setCursor(10, 10);
//     tft1.print(player1);
//     tft1.print(" vs ");
//     tft1.print(player2);

//     digitalWrite(TFT_CS, HIGH);   // Deselect first display

//     // Display on second screen
//     digitalWrite(TFT_CS2, LOW);   // Select second display
//     digitalWrite(TFT_CS, HIGH);   // Ensure first is deselected

//     tft2.fillRect(0, 0, tft2.width(), 60, TFT_BLACK);
//     tft2.setTextColor(TFT_WHITE, TFT_BLACK);
//     tft2.setTextSize(2);
//     tft2.setCursor(10, 10);
//     tft2.print(player2);
//     tft2.print(" vs ");
//     tft2.print(player1);    
    
//     digitalWrite(TFT_CS2, HIGH);  // Deselect second display
// }


// void displayGameStatus(String status) {
//     // Display status on first screen
//     digitalWrite(TFT_CS, LOW);    // Select first display
//     digitalWrite(TFT_CS2, HIGH);  // Ensure second is deselected
//     tft1.fillRect(0, 180, tft1.width(), 40, TFT_BLACK);
//     tft1.setTextColor(TFT_WHITE, TFT_BLACK);
//     tft1.setTextSize(2);
//     tft1.setCursor(10, 190);
//     tft1.print(status);
//     digitalWrite(TFT_CS, HIGH);   // Deselect first display

//     // Display status on second screen
//     digitalWrite(TFT_CS2, LOW);   // Select second display
//     digitalWrite(TFT_CS, HIGH);   // Ensure first is deselected
//     tft2.fillRect(0, 180, tft2.width(), 40, TFT_BLACK);
//     tft2.setTextColor(TFT_WHITE, TFT_BLACK);
//     tft2.setTextSize(2);
//     tft2.setCursor(10, 190);
//     tft2.print(status);
//     digitalWrite(TFT_CS2, HIGH);  // Deselect second display
// }

// void displayGameBoard(String boardState) {
//     if (!gameStarted) return;

//     // First display
//     digitalWrite(TFT_CS, LOW);    // Select first display
//     digitalWrite(TFT_CS2, HIGH);  // Ensure second is deselected
    
//     tft1.fillRect(0, 60, tft1.width(), 120, TFT_BLACK);
//     tft1.drawLine(10, 100, 230, 100, TFT_WHITE);
//     tft1.drawLine(10, 140, 230, 140, TFT_WHITE);
//     tft1.drawLine(80, 60, 80, 180, TFT_WHITE);
//     tft1.drawLine(160, 60, 160, 180, TFT_WHITE);
    
//     digitalWrite(TFT_CS, HIGH);   // Deselect first display

//     // Second display
//     digitalWrite(TFT_CS2, LOW);   // Select second display
//     digitalWrite(TFT_CS, HIGH);   // Ensure first is deselected
    
//     tft2.fillRect(0, 60, tft2.width(), 120, TFT_BLACK);
//     tft2.drawLine(10, 100, 230, 100, TFT_WHITE);
//     tft2.drawLine(10, 140, 230, 140, TFT_WHITE);
//     tft2.drawLine(80, 60, 80, 180, TFT_WHITE);
//     tft2.drawLine(160, 60, 160, 180, TFT_WHITE);
    
//     digitalWrite(TFT_CS2, HIGH);  // Deselect second display
    
//     // Draw X's and O's
//     for(int i = 0; i < 9; i++) {
//         int row = i / 3;
//         int col = i % 3; 
//         int x = 20 + col * 80;
//         int y = 70 + row * 40;
        
//         if(boardState[i] == 'X') {
//             // First display
//             digitalWrite(TFT_CS, LOW);
//             digitalWrite(TFT_CS2, HIGH);
//             tft1.setTextColor(TFT_BLUE, TFT_BLACK);
//             tft1.setCursor(x, y);
//             tft1.print("X");
//             digitalWrite(TFT_CS, HIGH);

//             // Second display
//             digitalWrite(TFT_CS2, LOW);
//             digitalWrite(TFT_CS, HIGH);
//             tft2.setTextColor(TFT_BLUE, TFT_BLACK);
//             tft2.setCursor(x, y);
//             tft2.print("X");
//             digitalWrite(TFT_CS2, HIGH);
//         }
//         else if(boardState[i] == 'O') {
//             // First display
//             digitalWrite(TFT_CS, LOW);
//             digitalWrite(TFT_CS2, HIGH);
//             tft1.setTextColor(TFT_RED, TFT_BLACK);
//             tft1.setCursor(x, y);
//             tft1.print("O");
//             digitalWrite(TFT_CS, HIGH);

//             // Second display
//             digitalWrite(TFT_CS2, LOW);
//             digitalWrite(TFT_CS, HIGH);
//             tft2.setTextColor(TFT_RED, TFT_BLACK);
//             tft2.setCursor(x, y);
//             tft2.print("O");
//             digitalWrite(TFT_CS2, HIGH);
//         }
//     }
// }


// // This function displays the initial "Choose a game" message
// void displayInitialMessage() {

//     digitalWrite(TFT_CS, LOW);
//     digitalWrite(TFT_CS2, HIGH);
//     tft1.fillScreen(TFT_BLACK);
//     tft1.setTextColor(TFT_WHITE, TFT_BLACK);
//     tft1.setTextSize(3);
//     tft1.setCursor(10, 50);
//     tft1.print("Choose a");
//     tft1.setCursor(10, 100);
//     tft1.print("game...");
//     digitalWrite(TFT_CS, HIGH);

//     digitalWrite(TFT_CS2, LOW);
//     digitalWrite(TFT_CS, HIGH);
//     tft2.fillScreen(TFT_BLACK);
//     tft2.setTextColor(TFT_WHITE, TFT_BLACK);
//     tft2.setTextSize(3);
//     tft2.setCursor(10, 50);
//     tft2.print("Choose a");
//     tft2.setCursor(10, 100);
//     tft2.print("game...");
//     digitalWrite(TFT_CS2, HIGH);
// }

// // Callback function to handle incoming MQTT messages
// void callback(char* topic, byte* payload, unsigned int length) {
//     String message = "";
//     for (int i = 0; i < length; i++) {
//         message += (char)payload[i];
//     }
    
//     if (String(topic) == "game/players") {
//         if (message.startsWith("player1:")) {
//             player1 = message.substring(8);
//             displayPlayerNames();
//             // Check if both players are set
//             if (player1 != "" && player2 != "") {
//                 gameStarted = true;
//                 displayGameBoard(".........");  // Draw empty board
//             }
//         } else if (message.startsWith("player2:")) {
//             player2 = message.substring(8);
//             displayPlayerNames();
//             // Check if both players are set
//             if (player1 != "" && player2 != "") {
//                 gameStarted = true;
//                 displayGameBoard(".........");  // Draw empty board
//             }
//         }
//     }
//     else if (String(topic) == "game/status") {
//         displayGameStatus(message);
//     }
//     else if (String(topic) == "game/board") {
//         displayGameBoard(message);
//     }
// }

// void setup() {
//     Serial.begin(115200);
    
//     // Initialize LCD
//     pinMode(TFT_CS, OUTPUT);
//     pinMode(TFT_CS2, OUTPUT);
//     digitalWrite(TFT_CS, HIGH);   // Deselect first display
//     digitalWrite(TFT_CS2, HIGH);  // Deselect second display
    
//     // Initialize first display
//     digitalWrite(TFT_CS, LOW);    // Select first display
//     tft1.init();
//     tft1.setRotation(1);
//     digitalWrite(TFT_CS, HIGH);   // Deselect first display
    
//     // Initialize second display
//     digitalWrite(TFT_CS2, LOW);   // Select second display
//     tft2.init();
//     tft2.setRotation(1);
//     digitalWrite(TFT_CS2, HIGH);  // Deselect second display
    
//     // Connect to WiFi
//     setupWiFi();
    
//     // Connect to MQTT
//     client.setServer(mqtt_server, 1883);
//     client.setCallback(callback);
    
//     // Subscribe to topics
//     if (client.connect("ESP32_LCD")) {
//         client.subscribe("game/players");
//         client.subscribe("game/status");
//         client.subscribe("game/board");
//         // Display initial message
//         displayInitialMessage();
//     }
// }

// void loop() {
//   if (!client.connected()) {
//     reconnectMQTT();
//   }
//   client.loop();
// }
