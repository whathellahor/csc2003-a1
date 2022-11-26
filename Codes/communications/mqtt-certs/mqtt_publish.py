import ssl
from time import sleep
from paho import mqtt
import paho.mqtt.client as paho
import paho.mqtt.publish as publish
import os
from config import *
from random import randint

START = '\33[42m'
END = '\33[0m'

# Set the SSL protocols
sslSettings = ssl.SSLContext(mqtt.client.ssl.PROTOCOL_TLS)

# Enter username and password
auth = {'username': username, 'password': password}

# Clear console
clear = lambda:os.system('cls')
clear()

print(START + "Sending message " + END)

try:
# Forever loop, to send message
    while True:
        
        msg = "1"
        
        # Define the message to be sent 
        msgs = [{'topic': topic, 'payload': msg}]
        
        print(topic, ":", msgs[0]['payload'])
        publish.multiple(msgs, hostname=broker, port=port, auth=auth,tls=sslSettings, protocol=paho.MQTTv31)
        sleep(5)
        break
except:
    print("Program exited!")