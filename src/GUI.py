
import tkinter as tk
import paho.mqtt.client as mqtt
import pygame
from tkinter import simpledialog
from ticTacToe import TicTacToe, TicTacToeAI
from othello import Othello


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
        print(f"Message received: {payload}")
        if payload == "tic_tac_toe":  # Button 1 pressed
            print("Starting regular Tic Tac Toe...")
            active_game = TicTacToe(root, client)
            game_selected = True
        elif payload == "tic_tac_toe_ai":  # Button 2 pressed
            print("Starting Tic Tac Toe AI...")
            active_game = TicTacToeAI(root, client)
            game_selected = True
        elif payload == "othello":  # Button 3 pressed
            print("Starting Othello...")
            active_game = Othello(root, client)
            game_selected = True
        else:
            print("Unknown game:", payload)
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


# Vänta tills ett spel har valts
print("Väntar på meddelande för att välja spel...")
# while not game_selected:
#     client.loop(timeout=0.1)

active_game = TicTacToe(root, client)
game_selected = True  # Set to true to start the game immediately

print("A game has been selected. Starting GUI...")

def mqtt_loop():
    client.loop(timeout=0.1)
    root.after(100, mqtt_loop)

mqtt_loop()
root.mainloop()