// LCD CODE //

// #include <TFT_eSPI.h>  
// #include <SPI.h>       
// #include <WiFi.h>
// #include <PubSubClient.h>

// // WiFi credentials
// const char* ssid = "fatimas lur";          
// const char* password = "jagvetinte";    
// const char* mqtt_server = "172.20.10.3";  

// // MQTT client
// WiFiClient lcdClient; 
// PubSubClient client(lcdClient); 

// // Player names
// String player1 = "";
// String player2 = "";

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

// TFT_eSPI tft = TFT_eSPI();  

// // This function displays the player names on the top section of the screen
// void displayPlayerNames() {
//   // Clear the top section (names)
//   tft.fillRect(0, 0, tft.width(), 60, TFT_BLACK);
//   tft.setTextColor(TFT_WHITE, TFT_BLACK);
//   tft.setTextSize(3);
  
//   // Display Player 1
//   tft.setCursor(10, 10);
//   tft.print("Player 1: ");
//   tft.setCursor(10, 40);
//   tft.print(player1);
  
//   // Display Player 2
//   tft.setCursor(10, 90);
//   tft.print("Player 2: ");
//   tft.setCursor(10, 120);
//   tft.print(player2);
// }

// void displayGameStatus(String status) {
//     // Clear the bottom section (game status)
//     tft.fillRect(0, 180, tft.width(), 40, TFT_BLACK);
//     tft.setTextColor(TFT_WHITE, TFT_BLACK);
//     tft.setTextSize(2);
//     tft.setCursor(10, 190);
//     tft.print(status);
// }

// void displayGameBoard(String boardState) {
//     // Clear the middle section (board)
//     tft.fillRect(0, 60, tft.width(), 120, TFT_BLACK);
    
//     // Draw the board grid
//     tft.setTextColor(TFT_WHITE, TFT_BLACK);
//     tft.setTextSize(3);
    
//     // Draw horizontal lines
//     tft.drawLine(10, 100, 230, 100, TFT_WHITE);
//     tft.drawLine(10, 140, 230, 140, TFT_WHITE);
    
//     // Draw vertical lines
//     tft.drawLine(80, 60, 80, 180, TFT_WHITE);
//     tft.drawLine(160, 60, 160, 180, TFT_WHITE);
    
//     // Draw X's and O's
//     for(int i = 0; i < 9; i++) {
//         int row = i / 3;
//         int col = i % 3; 
//         int x = 20 + col * 80;
//         int y = 70 + row * 40;
        
//         if(boardState[i] == 'X') {
//             tft.setTextColor(TFT_BLUE, TFT_BLACK);
//             tft.setCursor(x, y);
//             tft.print("X");
//         }
//         else if(boardState[i] == 'O') {
//             tft.setTextColor(TFT_RED, TFT_BLACK);
//             tft.setCursor(x, y);
//             tft.print("O");
//         }
//     }
// }

// // This function clears the screen and redraws the player names, game board, and status
// void redrawScreen(String boardState) {
//     tft.fillScreen(TFT_BLACK); 
//     displayPlayerNames();
//     displayGameBoard(boardState);
//     displayGameStatus("Loading game...");
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
//         } else if (message.startsWith("player2:")) {
//             player2 = message.substring(8);
//             displayPlayerNames();
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
//     tft.init();
//     tft.setRotation(1);
//     tft.fillScreen(TFT_BLACK);
//     tft.setTextColor(TFT_WHITE, TFT_BLACK);
//     tft.setTextSize(2);
//     tft.setCursor(10, 10);
//     tft.println("Connecting...");
    
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
//         // Initialize with empty board
//         redrawScreen(".........");
//     }
// }

// void loop() {
//   if (!client.connected()) {
//     reconnectMQTT();
//   }
//   client.loop();
// }





// GUI CODE //

// import tkinter as tk
// import paho.mqtt.client as mqtt
// import pygame
// from tkinter import simpledialog

// # Predefined tags for players
// X_TAGS = {"FF0F1ADE5C0000", "FF0FC7FE5B0000", "53DA692AA00001", "FF0FCAFE5B0000", "53387D2AA00001"}
// O_TAGS = {"FF0FC9FE5B0000", "FF0FC8FE5B0000", "53055E2AA00001", "5387642AA00001"}

// # Predefined reader positions
// READER_POSITIONS = {
//     "reader1": (0, 0), "reader2": (0, 1), "reader3": (0, 2),
//     "reader4": (1, 0), "reader5": (1, 1), "reader6": (1, 2),
//     "reader7": (2, 0), "reader8": (2, 1), "reader9": (2, 2),
// }

// # Initialize board and game state
// board = [["" for _ in range(3)] for _ in range(3)]
// current_player = "X"
// game_over = False

// # Initialize pygame for sound
// pygame.mixer.init()
// pygame.mixer.music.load("intro.mp3")  # Play intro music
// pygame.mixer.music.play()

// # GUI setup
// root = tk.Tk()
// root.title("Tic Tac Toe - RFID")
// cells = [[None for _ in range(3)] for _ in range(3)]

// status_label = tk.Label(root, text="", font=("Arial", 20))
// status_label.grid(row=3, column=0, columnspan=3, pady=10)

// # MQTT setup
// broker_ip = "172.20.10.3"
// client = mqtt.Client()
// client.connect(broker_ip, 1883, 60)

// # Player names
// player1_name = ""
// player2_name = ""

// # MQTT topics
// MQTT_TOPIC_NFC = "game/nfc/reader"
// MQTT_TOPIC_PLAYERS = "game/players"
// MQTT_TOPIC_STATUS = "game/status"
// MQTT_TOPIC_BOARD = "game/board"

// # This function gets player names from the user
// def get_player_names():
//     global player1_name, player2_name
    
//     root.withdraw() # Hide the main window while asking for names
    
//     # If the user cancels or enters an empty name, use default names
//     player1_name = simpledialog.askstring("Player 1", "Enter name for Player 1 (X):", parent=root)
//     if player1_name is None or player1_name.strip() == "":
//         player1_name = "Player 1"
    
//     player2_name = simpledialog.askstring("Player 2", "Enter name for Player 2 (O):", parent=root)
//     if player2_name is None or player2_name.strip() == "":
//         player2_name = "Player 2"
    
//     # Send player names to LCD ESP32
//     client.publish(MQTT_TOPIC_PLAYERS, f"player1:{player1_name}")
//     client.publish(MQTT_TOPIC_PLAYERS, f"player2:{player2_name}")
    
//     # Send initial board state to LCD
//     send_board_state()
    
//     # Show the main window again
//     root.deiconify()
//     update_status(f" {player1_name} (X) starts!")

// # This function updates the game status label and sends the status to MQTT
// def update_status(msg):
//     status_label.config(text=msg)
//     client.publish(MQTT_TOPIC_STATUS, msg)

// # This function highlights possible moves for the current player by changing the background color of empty cells
// def highlight_possible_moves():
//     for row in range(3):
//         for col in range(3):
//             if board[row][col] == "" and not game_over:
//                 cells[row][col].config(bg="#ddffdd") # Change background color to light blue
//             else:
//                 cells[row][col].config(bg="white") # Reset background color for occupied cells

// # This function checks for a winner or a draw
// def check_winner():
//     lines = [
//         board[0], board[1], board[2],
//         [board[0][0], board[1][0], board[2][0]],
//         [board[0][1], board[1][1], board[2][1]],
//         [board[0][2], board[1][2], board[2][2]],
//         [board[0][0], board[1][1], board[2][2]],
//         [board[0][2], board[1][1], board[2][0]],
//     ]
//     for line in lines:
//         if line.count(line[0]) == 3 and line[0] != "": # Check if all three are the same and not empty
//             return line[0] 
//     if all(cell != "" for row in board for cell in row):
//         return "draw"
//     return None

// # This function ends the game and plays a sound
// def end_game(winner):
//     global game_over
//     game_over = True
//     pygame.mixer.music.stop()
//     pygame.mixer.Sound("win.mp3").play()

//     if winner == "draw":
//         update_status(" It's a draw!")
//     else:
//         winner_name = player1_name if winner == "X" else player2_name
//         update_status(f"🏆 {winner_name} won!")

// # This function sends the current board state to MQTT
// def send_board_state():
//     # Convert the 2D board to a string representation
//     board_state = ""
//     for row in board:
//         for cell in row:
//             board_state += cell if cell else "."
    
//     # Send the board state to the LCD
//     client.publish(MQTT_TOPIC_BOARD, board_state)

// # This function updates the cell when a player makes a move
// def update_cell(row, col, player):
//     global current_player
//     if board[row][col] == "" and not game_over:
//         board[row][col] = player
//         cells[row][col].config(text=player, fg="blue" if player == "X" else "red")
        
//         # Send updated board state to LCD
//         send_board_state()

//         winner = check_winner()
//         if winner:
//             end_game(winner)
//         else:
//             current_player = "O" if current_player == "X" else "X"
//             current_player_name = player1_name if current_player == "X" else player2_name
//             update_status(f" {current_player_name}'s turn")
//             highlight_possible_moves()

// # This function resets the game state and sends the initial board state to MQTT
// def reset_game():
//     global board, current_player, game_over
//     board = [["" for _ in range(3)] for _ in range(3)]
//     current_player = "X"
//     game_over = False

//     for row in range(3):
//         for col in range(3):
//             cells[row][col].config(text="", bg="white")

//     pygame.mixer.music.load("intro.mp3")
//     pygame.mixer.music.play()
//     update_status(f" {player1_name} (X) starts!")
//     highlight_possible_moves()
    
//     # Send initial board state to LCD
//     send_board_state()
    
//     # delay
//     root.after(500, lambda: None)  # 500ms delay to clear any pending RFID reads
    
// # Create the GUI grid for the game board
// for row in range(3):
//     for col in range(3):
//         label = tk.Label(root, text="", font=("Arial", 40), width=4, height=2, relief="solid", bg="white")
//         label.grid(row=row, column=col, padx=2, pady=2)
//         cells[row][col] = label

// # Reset button
// reset_button = tk.Button(root, text="Reset game", font=("Arial", 14), command=reset_game)
// reset_button.grid(row=4, column=0, columnspan=3, pady=10)

// # MQTT connection and message handling
// def on_connect(client, userdata, flags, rc):
//     print("Connected to MQTT broker")
//     client.subscribe(MQTT_TOPIC_NFC)

// def on_message(client, userdata, msg):
//     global current_player
//     if game_over:
//         print("The game is over.")
//         return

//     payload = msg.payload.decode()
//     try:
//         reader, uid = payload.split(":", 1)

//         if uid in X_TAGS:
//             player = "X"
//         elif uid in O_TAGS:
//             player = "O"
//         else:
//             print("Unknown tag:", uid)
//             return

//         if player != current_player:
//             print(f"Error, it's {current_player}'s turn.")
//             return

//         if reader in READER_POSITIONS:
//             row, col = READER_POSITIONS[reader]
//             if board[row][col] == "":
//                 update_cell(row, col, player)
//             else:
//                 print("This spot is already taken.")
//         else:
//             print("Unknown reader:", reader)

//     except Exception as e:
//         print("Error in message:", e)

// client.on_connect = on_connect
// client.on_message = on_message

// # This function continuously checks for incoming MQTT messages
// def mqtt_loop():
//     client.loop(timeout=0.1)
//     root.after(100, mqtt_loop)

// # Get player names before starting the game
// root.after(100, get_player_names)

// mqtt_loop()
// root.mainloop()