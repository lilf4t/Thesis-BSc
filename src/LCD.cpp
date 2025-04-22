#include <TFT_eSPI.h>  
#include <SPI.h>       
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "fatimas lur";          
const char* password = "jagvetinte";    
const char* mqtt_server = "172.20.10.3";  

// MQTT client
WiFiClient lcdClient; 
PubSubClient client(lcdClient); 

// Player names
String player1 = "";
String player2 = "";

// Function to connect to WiFi
void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

// Function to reconnect to MQTT broker
void reconnectMQTT() {
  while (!client.connected()) {
    if (client.connect("ESP32_LCD")) break;
    delay(1000);
  }
}

TFT_eSPI tft = TFT_eSPI();  

// This function displays the player names on the top section of the screen
void displayPlayerNames() {
  // Clear the top section (names)
  tft.fillRect(0, 0, tft.width(), 60, TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);

  // Display Player 1
  tft.setCursor(10, 10);
  tft.print("Player 1: ");
  tft.setCursor(10, 40);
  tft.print(player1);

  // Display Player 2
  tft.setCursor(10, 90);
  tft.print("Player 2: ");
  tft.setCursor(10, 120);
  tft.print(player2);
  
}

void displayGameStatus(String status) {
    // Clear the bottom section (game status)
    tft.fillRect(0, 180, tft.width(), 40, TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 190);
    tft.print(status);
}

void displayGameBoard(String boardState) {
    // Clear the middle section (board)
    tft.fillRect(0, 60, tft.width(), 120, TFT_BLACK);
    
    // Draw the board grid
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    
    // Draw horizontal lines
    tft.drawLine(10, 100, 230, 100, TFT_WHITE);
    tft.drawLine(10, 140, 230, 140, TFT_WHITE);
    
    // Draw vertical lines
    tft.drawLine(80, 60, 80, 180, TFT_WHITE);
    tft.drawLine(160, 60, 160, 180, TFT_WHITE);
    
    // Draw X's and O's
    for(int i = 0; i < 9; i++) {
        int row = i / 3;
        int col = i % 3; 
        int x = 20 + col * 80;
        int y = 70 + row * 40;
        
        if(boardState[i] == 'X') {
            tft.setTextColor(TFT_BLUE, TFT_BLACK);
            tft.setCursor(x, y);
            tft.print("X");
        }
        else if(boardState[i] == 'O') {
            tft.setTextColor(TFT_RED, TFT_BLACK);
            tft.setCursor(x, y);
            tft.print("O");
        }
    }
}

// This function clears the screen and redraws the player names, game board, and status
void redrawScreen(String boardState) {
    tft.fillScreen(TFT_BLACK); 
    displayPlayerNames();
    displayGameBoard(boardState);
    displayGameStatus("Loading game...");
}

// Callback function to handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    if (String(topic) == "game/players") {
        if (message.startsWith("player1:")) {
            player1 = message.substring(8);
            displayPlayerNames();
        } else if (message.startsWith("player2:")) {
            player2 = message.substring(8);
            displayPlayerNames();
        }
    }
    else if (String(topic) == "game/status") {
        displayGameStatus(message);
    }
    else if (String(topic) == "game/board") {
        displayGameBoard(message);
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
    
    // Subscribe to topics
    if (client.connect("ESP32_LCD")) {
        client.subscribe("game/players");
        client.subscribe("game/status");
        client.subscribe("game/board");
        // Initialize with empty board
        redrawScreen(".........");
    }
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
}
