import tkinter as tk
import paho.mqtt.client as mqtt
import pygame
from tkinter import simpledialog

class TicTacToe:
    def __init__(self, root, client):
        self.root = root
        self.client = client
        
        # MQTT topics
        self.MQTT_TOPIC_NFC = "game/nfc/reader"
        self.MQTT_TOPIC_PLAYERS = "game/players"
        self.MQTT_TOPIC_STATUS = "game/status"
        self.MQTT_TOPIC_BOARD = "game/board"
        
        # Player names
        self.player1_name = ""
        self.player2_name = ""
        
        # Initialize board and game state
        self.board = [["" for _ in range(3)] for _ in range(3)]
        self.current_player = "X"
        self.game_over = False
        self.cells = [[None for _ in range(3)] for _ in range(3)]
        
        # Predefined tags for players
        self.X_TAGS = {"FF0F1ADE5C0000", "FF0FC7FE5B0000", "53DA692AA00001", "FF0FCAFE5B0000", "53387D2AA00001"}
        self.O_TAGS = {"FF0FC9FE5B0000", "FF0FC8FE5B0000", "53055E2AA00001", "5387642AA00001"}
        
        # Predefined reader positions
        self.READER_POSITIONS = {
            "reader1": (0, 0), "reader2": (0, 1), "reader3": (0, 2),
            "reader4": (1, 0), "reader5": (1, 1), "reader6": (1, 2),
            "reader7": (2, 0), "reader8": (2, 1), "reader9": (2, 2),
        }

        # GUI setup
        self.status_label = tk.Label(root, text="", font=("Arial", 20))
        self.status_label.grid(row=3, column=0, columnspan=3, pady=10)

        self.setup_board()
        self.root.after(100, self.get_player_names)  # Delay to allow the intro music to play
        self.reset_game()

    # Setup the game board
    def setup_board(self):
        for row in range(3):
            for col in range(3):
                label = tk.Label(self.root, text="", font=("Arial", 40), width=4, height=2, relief="solid", bg="white")
                label.grid(row=row, column=col, padx=2, pady=2)
                self.cells[row][col] = label
        
        # Create a reset button        
        reset_button = tk.Button(self.root, text="Reset game", font=("Arial", 14), command=self.reset_game)
        reset_button.grid(row=4, column=0, columnspan=3, pady=10)
    
    # Get player names from the user
    def get_player_names(self):
        self.root.withdraw()  # Hide main window
        
        self.player1_name = tk.simpledialog.askstring("Player 1", "Enter name for Player 1 (X):", parent=self.root)
        if self.player1_name is None or self.player1_name.strip() == "":
            self.player1_name = "Player 1"
        
        self.player2_name = tk.simpledialog.askstring("Player 2", "Enter name for Player 2 (O):", parent=self.root)
        if self.player2_name is None or self.player2_name.strip() == "":
            self.player2_name = "Player 2"
        
        # Send player names to LCD ESP32
        self.client.publish(self.MQTT_TOPIC_PLAYERS, f"player1:{self.player1_name}")
        self.client.publish(self.MQTT_TOPIC_PLAYERS, f"player2:{self.player2_name}")
        
        # Send initial board state
        self.send_board_state()
        
        self.root.deiconify() # Show main window again
        self.update_status(f"{self.player1_name} (X) starts!")
        
    # Send the current board state to the ESP32
    def send_board_state(self):
        board_state = ""
        for row in self.board:
            for cell in row:
                board_state += cell if cell else "."
        self.client.publish(self.MQTT_TOPIC_BOARD, board_state)

    # Update the status label and publish to MQTT
    def update_status(self, msg):
        self.status_label.config(text=msg)
        self.client.publish(self.MQTT_TOPIC_STATUS, msg)

    # Highlight possible moves for the current player
    def highlight_possible_moves(self):
        for row in range(3):
            for col in range(3):
                if self.board[row][col] == "" and not self.game_over:
                    # Green background for possible moves (empty cells)
                    self.cells[row][col].config(bg="#ddffdd")

                    # Send MQTT message for possible moves
                    if self.current_player == "X":
                        color = "red"  # Red for X
                    else:
                        color = "blue"  # Blue for O

                    # Send message with row, col, and color
                    message = f"{row},{col},{color}"
                    self.client.publish("game/possible_moves", message)
                else:
                    self.cells[row][col].config(bg="white")
                    
    # Check for a winner or a draw
    def check_winner(self):
        lines = [
            self.board[0], self.board[1], self.board[2],
            [self.board[0][0], self.board[1][0], self.board[2][0]],
            [self.board[0][1], self.board[1][1], self.board[2][1]],
            [self.board[0][2], self.board[1][2], self.board[2][2]],
            [self.board[0][0], self.board[1][1], self.board[2][2]],
            [self.board[0][2], self.board[1][1], self.board[2][0]],
        ]
        for line in lines:
            if line.count(line[0]) == 3 and line[0] != "":
                return line[0]
        if all(cell != "" for row in self.board for cell in row):
            return "draw"
        return None

    # End the game and announce the winner
    def end_game(self, winner):
        self.game_over = True
        pygame.mixer.music.stop()
        pygame.mixer.Sound("win.mp3").play()

        if winner == "draw":
            self.update_status("It's a draw!")
        else:
            self.update_status(f"Player {winner} Won!")

    # Update the cell when a player makes a move
    def update_cell(self, row, col, player):
        if self.board[row][col] == "" and not self.game_over:
            self.board[row][col] = player
            self.cells[row][col].config(text=player, fg="blue" if player == "X" else "red")
            
            self.send_board_state()  # Send updated board state to LCD esp32

            winner = self.check_winner()
            if winner:
                self.end_game(winner)
            else:
                self.current_player = "O" if self.current_player == "X" else "X"
                current_player_name = self.player1_name if self.current_player == "X" else self.player2_name
                self.update_status(f"Player {current_player_name}'s turn'")
                self.highlight_possible_moves()

    # Reset the game
    def reset_game(self):
        self.board = [["" for _ in range(3)] for _ in range(3)]
        self.current_player = "X"
        self.game_over = False

        for row in range(3):
            for col in range(3):
                self.cells[row][col].config(text="", bg="white")

        pygame.mixer.music.load("intro.mp3")
        pygame.mixer.music.play()
        self.update_status(f"{self.player1_name} (X) starts!")
        self.highlight_possible_moves()
        
        self.send_board_state()  # Send initial board state to LCD esp32
        
        self.root.after(500, lambda: None)  # Delay to allow the sound to play

    # Handle incoming MQTT messages
    def handle_message(self, topic, payload):
        if topic == "game/nfc/reader":  # hosseins kod har "rfid/reader" fast min espA kod har "game/nfc/reader"
            try:
                reader, uid = payload.split(":", 1)

                if uid in self.X_TAGS:
                    player = "X"
                elif uid in self.O_TAGS:
                    player = "O"
                else:
                    print("Unknown tag:", uid)
                    return

                if player != self.current_player:
                    print(f"It's {self.current_player}'s turn.")
                    return

                if reader in self.READER_POSITIONS:
                    row, col = self.READER_POSITIONS[reader]
                    self.update_cell(row, col, player)
                else:
                    print("Unknown reader", reader)
            except Exception as e:
                print("Error in message:", e)

# MQTT Callbacks
def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker")
    client.subscribe("game/nfc/reader") # hosseins kod har "rfid/reader" fast min espA kod har "game/nfc/reader"
    client.subscribe("server/game/select") 

# MQTT message handler
def on_message(client, userdata, msg):
    global active_game, game_selected

    topic = msg.topic
    payload = msg.payload.decode()

    if topic == "server/game/select":
        print(f"Meddelande mottaget: {payload}")
        if payload == "tic_tac_toe":
            print("Starting Tic Tac Toe...")
            active_game = TicTacToe(root, client)
            game_selected = True  # Set game_selected to True to wait for a game selection
        elif payload == "othello":
            print("Starting Othello... ")
            # Här kan lägga till Othello klassen
            game_selected = True  # 
        else:
            print("Unknown game", payload)
    elif active_game:
        active_game.handle_message(topic, payload)

# === Main Program ===
pygame.mixer.init()
root = tk.Tk()
root.title("Board game")

broker_ip = "172.20.10.3"
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(broker_ip, 1883, 60)

# === Choose game ===
active_game = None
game_selected = False  # Control variable to wait for a game selection

# # Vänta tills ett spel har valts
# print("Väntar på meddelande för att välja spel...")
# while not game_selected:
#     client.loop(timeout=0.1)

active_game = TicTacToe(root, client)
game_selected = True

print("A game has been selected. Starting GUI...")

def mqtt_loop():
    client.loop(timeout=0.1)
    root.after(100, mqtt_loop)

mqtt_loop()
root.mainloop()