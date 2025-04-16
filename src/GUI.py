import tkinter as tk
import paho.mqtt.client as mqtt
import pygame
from tkinter import simpledialog

# === Fördefinierade taggar ===
X_TAGS = {"FF0F1ADE5C0000", "FF0FC7FE5B0000", "53DA692AA00001", "FF0FCAFE5B0000", "53387D2AA00001"}
O_TAGS = {"FF0FC9FE5B0000", "FF0FC8FE5B0000", "53055E2AA00001", "5387642AA00001"}

# === readerX → (row, col)
READER_POSITIONS = {
    "reader1": (0, 0), "reader2": (0, 1), "reader3": (0, 2),
    "reader4": (1, 0), "reader5": (1, 1), "reader6": (1, 2),
    "reader7": (2, 0), "reader8": (2, 1), "reader9": (2, 2),
}

board = [["" for _ in range(3)] for _ in range(3)]
current_player = "X"
game_over = False

# === Initiera ljud
pygame.mixer.init()
pygame.mixer.music.load("intro.mp3")  # Spela intro vid start
pygame.mixer.music.play()

# === Tkinter-setup
root = tk.Tk()
root.title("Tic Tac Toe - RFID")
cells = [[None for _ in range(3)] for _ in range(3)]

status_label = tk.Label(root, text="", font=("Arial", 20))
status_label.grid(row=3, column=0, columnspan=3, pady=10)

# === MQTT
broker_ip = "172.20.10.3"
client = mqtt.Client()
client.connect(broker_ip, 1883, 60)

# === Player names
player1_name = ""
player2_name = ""

MQTT_TOPIC_NFC = "game/nfc/reader"
MQTT_TOPIC_PLAYERS = "game/players"
MQTT_TOPIC_STATUS = "game/status"
MQTT_TOPIC_BOARD = "game/board"


def get_player_names():
    global player1_name, player2_name
    
    root.withdraw()
    
    player1_name = simpledialog.askstring("Player 1", "Enter name for Player 1 (X):", parent=root)
    if player1_name is None or player1_name.strip() == "":
        player1_name = "Player 1"
    
    player2_name = simpledialog.askstring("Player 2", "Enter name for Player 2 (O):", parent=root)
    if player2_name is None or player2_name.strip() == "":
        player2_name = "Player 2"
    
    # Send player names to LCD ESP32
    client.publish(MQTT_TOPIC_PLAYERS, f"player1:{player1_name}")
    client.publish(MQTT_TOPIC_PLAYERS, f"player2:{player2_name}")
    
    root.deiconify()
    update_status(f"🎯 {player1_name} (X) börjar")

def update_status(msg):
    status_label.config(text=msg)
    client.publish(MQTT_TOPIC_STATUS, msg)
    
def publish_board_state():
    board_state = ";".join([",".join(row) for row in board])
    client.publish(MQTT_TOPIC_BOARD, board_state)

def highlight_possible_moves():
    for row in range(3):
        for col in range(3):
            if board[row][col] == "" and not game_over:
                cells[row][col].config(bg="#ddffdd")
            else:
                cells[row][col].config(bg="white")

def check_winner():
    lines = [
        board[0], board[1], board[2],
        [board[0][0], board[1][0], board[2][0]],
        [board[0][1], board[1][1], board[2][1]],
        [board[0][2], board[1][2], board[2][2]],
        [board[0][0], board[1][1], board[2][2]],
        [board[0][2], board[1][1], board[2][0]],
    ]
    for line in lines:
        if line.count(line[0]) == 3 and line[0] != "":
            return line[0]
    if all(cell != "" for row in board for cell in row):
        return "draw"
    return None

def end_game(winner):
    global game_over
    game_over = True
    pygame.mixer.music.stop()
    pygame.mixer.Sound("win.mp3").play()

    if winner == "draw":
        update_status("🔁 Oavgjort!")
    else:
        winner_name = player1_name if winner == "X" else player2_name
        update_status(f"🏆 {winner_name} vann!")

def update_cell(row, col, player):
    global current_player
    if board[row][col] == "" and not game_over:
        board[row][col] = player
        cells[row][col].config(text=player, fg="blue" if player == "X" else "red")

        winner = check_winner()
        if winner:
            end_game(winner)
        else:
            current_player = "O" if current_player == "X" else "X"
            current_player_name = player1_name if current_player == "X" else player2_name
            update_status(f"🎯 {current_player_name}s tur")
            highlight_possible_moves()
            publish_board_state()

# === Återställ spelet
def reset_game():
    global board, current_player, game_over
    board = [["" for _ in range(3)] for _ in range(3)]
    current_player = "X"
    game_over = False

    for row in range(3):
        for col in range(3):
            cells[row][col].config(text="", bg="white")

    pygame.mixer.music.load("intro.mp3")
    pygame.mixer.music.play()
    update_status(f"🎯 {player1_name} börjar")
    highlight_possible_moves()
    
    # delay
    root.after(500, lambda: None)  # 500ms delay to clear any pending RFID reads
    

# === Skapa brädet
for row in range(3):
    for col in range(3):
        label = tk.Label(root, text="", font=("Arial", 40), width=4, height=2, relief="solid", bg="white")
        label.grid(row=row, column=col, padx=2, pady=2)
        cells[row][col] = label

# === Reset-knapp
reset_button = tk.Button(root, text="🔁 Starta om spel", font=("Arial", 14), command=reset_game)
reset_button.grid(row=4, column=0, columnspan=3, pady=10)

def on_connect(client, userdata, flags, rc):
    print("✅ Ansluten till MQTT")
    client.subscribe(MQTT_TOPIC_NFC)

def on_message(client, userdata, msg):
    global current_player
    if game_over:
        print("⛔️ Spelet är slut.")
        return

    payload = msg.payload.decode()
    try:
        reader, uid = payload.split(":", 1)

        if uid in X_TAGS:
            player = "X"
        elif uid in O_TAGS:
            player = "O"
        else:
            print("⚠️ Okänd tagg:", uid)
            return

        if player != current_player:
            print(f"⛔️ Fel tur. Det är {current_player}s tur.")
            return

        if reader in READER_POSITIONS:
            row, col = READER_POSITIONS[reader]
            if board[row][col] == "":
                update_cell(row, col, player)
            else:
                print("⛔️ Rutan är upptagen.")
        else:
            print("⚠️ Okänd läsare:", reader)

    except Exception as e:
        print("⚠️ Fel i meddelande:", e)

client.on_connect = on_connect
client.on_message = on_message

def mqtt_loop():
    client.loop(timeout=0.1)
    root.after(100, mqtt_loop)

# Get player names before starting the game
root.after(100, get_player_names)

mqtt_loop()
root.mainloop()