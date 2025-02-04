import tkinter as tk
from paho.mqtt import client as mqtt_client 

def on_message(client, userdata, msg): 
    reader_id = msg.payload.decode()  
    label['text'] = f"RFID Reader {reader_id} Detected a Tag!" 

def connect_mqtt(): 
    client = mqtt_client.Client("PythonSubscriber", callback_api_version=2.1) 
    client.connect("172.20.10.3", 1883) # Replace with your MQTT broker details
    client.subscribe("rfid/reader") 
    client.on_message = on_message  
    client.loop_start()
    return client

# GUI setup 
window = tk.Tk()
window.title("RFID Reader Status") 

label = tk.Label(window, text="Waiting for RFID tag...", font=("Arial", 24)) 
label.pack(pady=50)

client = connect_mqtt() # Connect to MQTT broker
window.mainloop()
