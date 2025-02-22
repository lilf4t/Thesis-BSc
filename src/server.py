import paho.mqtt.client as mqtt

# Callback when the client receives a CONNACK response from the broker.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    print("-----------------------------------------------------------")
    # Subscribe to the "rfid/reader" topic
    client.subscribe("rfid/reader")

# Callback when a PUBLISH message is received from the broker.
def on_message(client, userdata, msg):
    # Decode the payload and print it along with the topic
    payload_str = msg.payload.decode()
    print(payload_str)

# Broker settings (update these if needed)
broker_address = "172.20.10.3"
broker_port = 1883

# Create an MQTT client instance.
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Connect to the broker.
client.connect(broker_address, broker_port, keepalive=60)

# Process network traffic and dispatch callbacks.
client.loop_forever()
