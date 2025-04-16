#include <TFT_eSPI.h>  // Include the graphics library (this includes the SPI library)
#include <SPI.h>       // Include SPI library
#include <WiFi.h>
#include <PubSubClient.h>

TFT_eSPI tft = TFT_eSPI();  

// WiFi credentials
const char* ssid = "fatimas lur";          
const char* password = "jagvetinte";    
const char* mqtt_server = "172.20.10.3";  

String currentBoard[3][3] = {{"", "", ""}, {"", "", ""}, {"", "", ""}};
String currentTurn = "";


WiFiClient lcdClient;
PubSubClient client(lcdClient);

// Player names
String player1Name = "Player 1";
String player2Name = "Player 2";

void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void reconnectMQTT() {
  while (!client.connected()) {
    if (client.connect("ESP32_LCD")) break;
    delay(1000);
  }
}

void displayPlayerNames() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  
  // Display Player 1
  tft.setCursor(10, 10);
  tft.print("Player 1: ");
  tft.setCursor(10, 40);
  tft.print(player1Name);
  
  // Display Player 2
  tft.setCursor(10, 90);
  tft.print("Player 2: ");
  tft.setCursor(10, 120);
  tft.print(player2Name);
}

// Add these new functions:
void displayGameStatus(String status) {
    tft.fillRect(0, 180, tft.width(), 40, TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 190);
    tft.print(status);
}

void updateGameBoard(String boardState) {
    // Parse the board state and update display
    // You'll need to implement the visual representation
    // of the game board on the LCD
}

void callback(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    if (String(topic) == "game/players") {
        if (message.startsWith("player1:")) {
            player1Name = message.substring(8);
            displayPlayerNames();
        } else if (message.startsWith("player2:")) {
            player2Name = message.substring(8);
            displayPlayerNames();
        }
    }
    else if (String(topic) == "game/status") {
        displayGameStatus(message);
    }
    else if (String(topic) == "game/board") {
        updateGameBoard(message);
    }
}


void setup() {
  Serial.begin(115200);
  
  // Initialize LCD
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Connecting...");
  
  // Connect to WiFi
  setupWiFi();
  
  // Connect to MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  // Subscribe to player name topics
  if (client.connect("ESP32_LCD")) {
    client.subscribe("game/players");
    client.subscribe("game/status");
    client.subscribe("game/board");
    displayPlayerNames();
    }
 }

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
}
